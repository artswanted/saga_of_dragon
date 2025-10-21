-- skill

function Act_Mon_RushPiercing_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Act_Mon_RushPiercing_OnTargetListModified(actor,action,bIsBefore)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if bIsBefore == false then
		ODS("Act_Mon_RushPiercing_OnTargetListModified\n");
		Act_Mon_Melee_DoDamage(actor,actor:GetAction());
	end

end

function Act_Mon_RushPiercing_SetState(actor,action,kState)
	
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
		
			ODS("Act_Mon_RushPiercing_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n");
		
			action:SetSlot(3);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1;
		
	elseif kState == "FIRE" then
		action:SetSlot(2)
		iNewState = 2;
	elseif kState == "WAIT" then
		action:SetSlot(4)
		local fAniTime = actor:GetAnimationLength(action:GetSlotAnimName(4))
		if fAniTime == 0 then
		
			ODS("Act_Mon_RushPiercing_SetState SlotName["..action:GetSlotAnimName(4).."] Not Exist -> SetSlot To Next\n")
		
			action:SetSlot(3);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
			fAniTime = 1.0;
		end
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		action:SetParamFloat(3,g_world:GetAccumTime());
		action:SetParamFloat(30,fAniTime);
		iNewState = 3
		actor:DetachFrom(129)
	end
	
	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	action:SetParamInt(0,iNewState)
	return	true

end

function Act_Mon_RushPiercing_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_Mon_RushPiercing_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");
	
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
			ODS("[SkillError] Act_Mon_RushPiercing_OnEnter, kPacket is not valid. actor : "..actor:GetPilotGuid():GetString().." action:"..action:GetID().."\n");
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
		Act_Mon_RushPiercing_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mon_RushPiercing_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_RushPiercing_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
		
	    ODS("Act_Mon_RushPiercing_OnEnter Start Casting CurPos :"..actor:GetPos():GetX()..
	    ","..actor:GetPos():GetY()..
	    ","..actor:GetPos():GetZ()..
	    " TargetPos: "..ptTargetPos:GetX()..","..ptTargetPos:GetY()..","..ptTargetPos:GetZ().."\n");

		--SetBreak();
		actor:LookAt(ptTargetPos,true,false,true);
		actor:SetMovingDir(actor:GetLookingDir());
		
		local	kLookDir = actor:GetLookingDir();
	    ODS("Act_Mon_RushPiercing_OnEnter Looking Dir:"..kLookDir:GetX()..","..kLookDir:GetY()..","..kLookDir:GetZ().."\n");
		

	end
	
	action:SetParamInt(10,0)	--캐스팅 이펙트용
	
	return true
end
function Act_Mon_RushPiercing_OnCastingCompleted(actor,action)	

	Act_Mon_RushPiercing_SetState(actor,action,"FIRE")
	
	Act_Mon_RushPiercing_Fire(actor,action);
	
end

function Act_Mon_RushPiercing_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_Mon_RushPiercing_Fire\n");
	
	--	타겟 좌표 바라보기
	local	ptTargetPos = action:GetParamAsPoint(0)
	--actor:LookAt(ptTargetPos,true,false,true);

	--	방향 벡터 구하기
	local	kCurrentPos = actor:GetPos()
	local	kDirVector = ptTargetPos:_Subtract(kCurrentPos)
	kDirVector:Unitize()
	
	action:SetParamAsPoint(1,kDirVector)

	local Effect = actor:GetAnimationInfo("FIRE_EFFECT")
	if nil==Effect or ""==Effect then
		Effect = "ef_Vagavond_skill_03_02_char_root"
	end

	local EffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE")
	if nil==EffectNode or ""==EffectNode then
		EffectNode = "char_root"
	end

	local EffectScale = actor:GetAnimationInfo("FIRE_EFFECT_SCALE")
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
		actor:AttachSound(2784,kSoundID);
	end	

	Act_Mon_Melee_DoDamage(actor,actor:GetAction())
end

function Act_Mon_RushPiercing_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end	
	
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0)
	
	local	kLookDir = actor:GetLookingDir();
    ODS("Act_Mon_RushPiercing_OnUpdate Looking Dir:"..kLookDir:GetX()..","..kLookDir:GetY()..","..kLookDir:GetZ().."\n");

	
	if action:GetActionParam() == AP_FIRE then
	
		if iState == 3 then
		
			local	fTotalTime = action:GetParamFloat(30)
			local	fElapsedTime = accumTime - action:GetParamFloat(3)
			if fElapsedTime>fTotalTime then
				return	false
			end
		
		else
		
			local	kMovingDir = action:GetParamAsPoint(1)
--			local	kMovingTarget = action:GetParamAsPoint(0)
			local	fMoveSpeed = actor:GetAbil(AT_MOVESPEED)*(1+action:GetAbil(AT_R_MOVESPEED)/10000.0);
				
			local	ptTargetPos = action:GetParamAsPoint(0)
			local	kCurrentPos = actor:GetPos()

			local	kCurrentDir = ptTargetPos:_Subtract(kCurrentPos)
			kCurrentDir:Unitize()
			
			local	fDot = kCurrentDir:Dot(kMovingDir)
			if fDot<0 then
				Act_Mon_RushPiercing_SetState(actor,action,"WAIT");
				return	true;
			end
			
			kMovingDir:Multiply(fMoveSpeed)
			
			actor:SetMovingDelta(kMovingDir);
			
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_RushPiercing_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		elseif iState == 1 then
			if action:GetParamInt(10) == 0 then
				local Effect = actor:GetAnimationInfo("FIRE_EFFECT")
				local EffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE")
				if nil~=EffectNode and nil~=Effect and "NONE"~=Effect then
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
				Act_Mon_RushPiercing_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	return true
end
function Act_Mon_RushPiercing_OnCleanUp(actor, action)
	actor:DetachFrom(129)
end

function Act_Mon_RushPiercing_OnLeave(actor, action)
	
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	ODS("Act_Mon_RushPiercing_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
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

function Act_Mon_RushPiercing_OnEvent(actor,textKey)

	return true
end
