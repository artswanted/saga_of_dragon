-- skill

function Act_Mon_RushAttack_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Act_Mon_RushAttack_OnTargetListModified(actor,action,bIsBefore)

    local	kTargetList = action:GetTargetList();
    local	iTargetCount = kTargetList:size();
	local 	kDoDamage = actor:GetAnimationInfo("DO_DAMAGE")
	if nil ~= kDoDamage and "TRUE" == kDoDamage then
		kDoDamage = true
	else
		kDoDamage = false
	end
	if 1==action:GetParamInt(4) or kDoDamage then
		Act_Mon_RushAttack_DoDamage(actor,action)
	end

end

function Act_Mon_RushAttack_SetState(actor,action,kState)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iNewState = -1;
	if kState == "BATTLEIDLE_START" then
	
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false	
		end
				
		action:SetSlot(0)
		iNewState = 0;
	
	elseif kState == "BATTLEIDLE_LOOP" then
		
		action:SetSlot(1)
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
		
			ODS("Act_Mon_RushAttack_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n");
		
			--action:SetSlot(3);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1
		
	elseif kState == "FIRE" then
		action:SetSlot(2)
		iNewState = 2;
		actor:DetachFrom(129)
	elseif kState == "WAIT" then
		action:SetSlot(1)
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
		
			ODS("Act_Mon_RushAttack_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n");
		
			--action:SetSlot(3);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		action:SetParamFloat(3,g_world:GetAccumTime())
		iNewState = 3
		actor:DetachFrom(129)
	elseif kState == "ATTACK" then
		action:SetSlot(3)
		iNewState = 4
		actor:DetachFrom(129)
	end
	
	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	action:SetParamInt(0,iNewState)
	action:SetParamInt(952,0)
	
	return	true

end

function Act_Mon_RushAttack_OnEnter(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end	

	ODS("Act_Mon_RushAttack_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	
	--	rush target pos
	local	ptTargetPos = 0;
	
	if IsSingleMode() then
		ptTargetPos = GetMyActor():GetPos();
	else
		local	kPacket = action:GetParamAsPacket();
		if kPacket == nil or kPacket:IsNil() then
			ODS("[SkillError] Act_Mon_RushAttack_OnEnter, kPacket is not valid. actor : "..actor:GetPilotGuid():GetString().." action:"..action:GetID().."\n");
			return	false;
			
		else
			ptTargetPos = Point3(0,0,0);
			ptTargetPos:SetX(kPacket:PopFloat());
			ptTargetPos:SetY(kPacket:PopFloat());
			ptTargetPos:SetZ(kPacket:PopFloat());
		end
		
	end
	
	action:SetParamAsPoint(0,ptTargetPos);
		
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_RushAttack_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mon_RushAttack_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_RushAttack_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
		
	    ODS("Act_Mon_RushAttack_OnEnter Start Casting CurPos :"..actor:GetPos():GetX()..
	    ","..actor:GetPos():GetY()..
	    ","..actor:GetPos():GetZ()..
	    " TargetPos: "..ptTargetPos:GetX()..","..ptTargetPos:GetY()..","..ptTargetPos:GetZ().."\n");

		--SetBreak();
		actor:LookAt(ptTargetPos,true,false,true);
		actor:SetMovingDir(actor:GetLookingDir());
		
		local	kLookDir = actor:GetLookingDir();
	    ODS("Act_Mon_RushAttack_OnEnter Looking Dir:"..kLookDir:GetX()..","..kLookDir:GetY()..","..kLookDir:GetZ().."\n")
	end
	
	action:SetParamInt(10,0)	--캐스팅 이펙트용
	action:SetParamInt(4,0)		--TargetListModified보다 fire키가 먼저 올 경우

	action:SetParamInt(104,0)	--rush애니가 어느 형태인가? Loop=0 / Clamp=1
	local Loop = actor:GetAnimationInfoFromAniName("LOOP",action:GetSlotAnimName(2))
	if false==Loop:IsNil() and "FALSE"==Loop:GetStr() then
		action:SetParamInt(104,1)
	end

	return true
end
function Act_Mon_RushAttack_OnCastingCompleted(actor,action)	

	Act_Mon_RushAttack_SetState(actor,action,"FIRE")
	
	Act_Mon_RushAttack_Fire(actor,action);
	
end

function Act_Mon_RushAttack_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--ODS("Act_Mon_RushAttack_Fire\n");
	
	--	타겟 좌표 바라보기
	local	ptTargetPos = action:GetParamAsPoint(0)
	--actor:LookAt(ptTargetPos,true,false,true);

	if action:IsSkillFireType(EFireType_Ani_Moving) then
		local fAniTime = actor:GetAnimationLength(action:GetCurrentSlotAnimName())	--dash_after_attk_rush
		action:SetParamFloat(5, fAniTime)
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		action:SetParamFloat(1,g_world:GetAccumTime())
		action:SetParamAsPoint(10,actor:GetPos());	--	Start Pos

		-- 거리
		local Dist = ptTargetPos:Distance( actor:GetPos() )
		action:SetParamFloat(33, Dist)
		
		if actor:HaveAnimationTextKey("hit",action:GetCurrentSlotAnimName()) then
			action:SetParamInt(952,1)	--	State
		end
	end

	--	방향 벡터 구하기
	local	kCurrentPos = actor:GetPos()
	local	kDirVector = ptTargetPos:_Subtract(kCurrentPos)
	kDirVector:Unitize()
	
	action:SetParamAsPoint(1,kDirVector)
	
	local Effect = actor:GetAnimationInfo("FIRE_EFFECT")
	local EffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE")
	local EffectScale = actor:GetAnimationInfo("FIRE_EFFECT_SCALE")
	if nil==Effect or Effect=="" then
		Effect = "ef_Vagavond_skill_03_02_char_root"
	end
	if nil==EffectNode or EffectNode=="" then
		EffectNode = "char_root"
	end
	if nil==EffectScale or ""==EffectScale then
		EffectScale = 1.0
	else
		EffectScale = tonumber(EffectScale)
	end

	if "NONE"~=Effect then
		actor:AttachParticleS(129, EffectNode, Effect, EffectScale)
	end

	local	kSoundID = actor:GetAnimationInfo("SOUND_ID")
	if nil~=kSoundID and kSoundID~="" then
		actor:AttachSound(2784,kSoundID)
	end

end

function Act_Mon_RushAttack_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	local	iState = action:GetParamInt(0)
	
	local	kLookDir = actor:GetLookingDir();
    --ODS("Act_Mon_RushAttack_OnUpdate Looking Dir:"..kLookDir:GetX()..","..kLookDir:GetY()..","..kLookDir:GetZ().."\n");
	
	
	if action:GetActionParam() == AP_FIRE then
	
		if iState == 3 then
		
			local	fTotalTime = 1.0
			local	fElapsedTime = accumTime - action:GetParamFloat(3)
			if fElapsedTime>fTotalTime then
				return	false
			end
		elseif iState == 4 then
			if actor:IsAnimationDone() then
				return	false
			end
		else
		
			local	kMovingDir = action:GetParamAsPoint(1)
			local	ptTargetPos = action:GetParamAsPoint(0)
			local	kCurrentPos = actor:GetPos()

			local	kCurrentDir = ptTargetPos:_Subtract(kCurrentPos)
			kCurrentDir:Unitize()
			local	fAniTime = action:GetParamFloat(5)
			if fAniTime > 0 then
				local	fElapsedTime = accumTime - action:GetParamFloat(1)
				local	fRate  = fElapsedTime / fAniTime
				if fRate>1.0 then
					fRate = 1.0
				end

				local	fMovingDist = (fRate)*action:GetParamFloat(33)
				local	kStartPos = action:GetParamAsPoint(10)
				kMovingDir:Multiply(fMovingDist)
				local	kNewPos = kStartPos:_Add(kMovingDir)
				if( CheckNil(g_world == nil) ) then return false end
				if( CheckNil(g_world:IsNil()) ) then return false end
				kNewPos = g_world:ThrowRay(kNewPos,Point3(0,0,-1),500)
				if kNewPos:GetX() ~= -1 and kNewPos:GetY() ~= -1 and kNewPos:GetZ() ~= -1 then
					kNewPos:SetZ(kNewPos:GetZ()+25)
					actor:SetTranslate(kNewPos);
				end

				if fRate == 1.0 then
					if 0==action:GetParamInt(104) or (1==action:GetParamInt(104) and actor:IsAnimationDone()) then
						Act_Mon_RushAttack_SetState(actor,action,"ATTACK")
					end
					return	true
				end
			else
			
	--			local	kMovingTarget = action:GetParamAsPoint(0)
				local	fMoveSpeed = actor:GetAbil(AT_MOVESPEED)*(1+action:GetAbil(AT_R_MOVESPEED)/10000.0);
				local	fDot = kCurrentDir:Dot(kMovingDir)
				if fDot<0 then
					if 0==action:GetParamInt(104) or (1==action:GetParamInt(104) and actor:IsAnimationDone()) then
						Act_Mon_RushAttack_SetState(actor,action,"ATTACK")
					end
					return	true
				end

				kMovingDir:Multiply(fMoveSpeed)

				actor:SetMovingDelta(kMovingDir)
			end
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_RushAttack_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		elseif iState == 1 then
			if action:GetParamInt(10) == 0 then
				local Effect = actor:GetAnimationInfo("FIRE_EFFECT")
				local EffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE")
				if nil~=EffectNode and nil~=Effect then
					local EffectScale = actor:GetAnimationInfo("FIRE_EFFECT_SCALE")
					if nil==EffectScale or ""==EffectScale then
						EffectScale = 1
					else
						EffectScale = tonumber(EffectScale)
					end
					actor:AttachParticleS(129, EffectNode, Effect, EffectScale)
				end
				action:SetParamInt(10,1) 
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_RushAttack_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	return true
end
function Act_Mon_RushAttack_OnCleanUp(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	actor:DetachFrom(129)
	actor:DetachFrom(125)

	local CurAction = actor:GetAction()
	
	CheckNil(nil==CurAction)
	CheckNil(CurAction:IsNil())

--	local kPos = CurAction:GetParamAsPoint(0)
--	if false==kPos:IsZero() then
--		actor:SetTranslate(kPos)
--	end
end

function Act_Mon_RushAttack_OnLeave(actor, action)
	
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local	kCurAction = actor:GetAction();
	
	if( CheckNil(nil==kCurAction) ) then return true end
	if( CheckNil(kCurAction:IsNil()) ) then return true end
	
	local	actionID = action:GetID()
	ODS("Act_Mon_RushAttack_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
	if action:GetActionType()=="EFFECT" or
		actionID == "a_freeze" or
		actionID == "a_reverse_gravity" or
		actionID == "a_blow_up" or
		actionID == "a_blow_up_small" or
		actionID == "a_knock_back" or
		actionID == "a_die" or
		actionID == "a_idle" or
		actionID == "a_run_right" or
		actionID == "a_run_left" or
		actionID == "a_handclaps" then
		return true
	end
	return false 
end

function Act_Mon_RushAttack_OnEvent(actor,textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
    if textKey == "hit" or textKey == "fire" then
		local   action = actor:GetAction()
		if( CheckNil(nil==action) ) then return false end
		if( CheckNil(action:IsNil()) ) then return false end
		
		if 0==action:GetParamInt(4) and 4==action:GetParamInt(0) then	--공격상태일 때만
			action:SetParamInt(4,1)
			Act_Mon_RushAttack_DoDamage(actor, action)
		else
			if 1==action:GetParamInt(952) then
				ODS("if 1==action:GetParamInt(952) then\n",false,6482)
				action:SetParamInt(4,1)
			end
		end
	end
	return true
end

function Act_Mon_RushAttack_DoDamage(actor, action)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

    local	kTargetList = action:GetTargetList();
    local	iTargetCount = kTargetList:size();
	if 0>=iTargetCount then
		return
	end

	Act_Mon_Melee_DoDamage(actor,action)
	local QuakeTime = actor:GetAnimationInfo("QUAKE_TIME")
	if nil ~= QuakeTime and "" ~= QuakeTime then
		local QuakeFactor = actor:GetAnimationInfo("QUAKE_FACTOR")	
		if  nil == QuakeFactor then
			QuakeFactor = 1.0
		else
			QuakeFactor = tonumber(QuakeFactor)
		end
		QuakeCamera(QuakeTime, QuakeFactor)
	end

	local EffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE")
	local Effect = actor:GetAnimationInfo("FIRE_EFFECT")
	if nil~=EffectNode and nil~=Effect then
		actor:AttachParticle(125, EffectNode, Effect)
	end
	action:SetParamInt(4,0)
end
