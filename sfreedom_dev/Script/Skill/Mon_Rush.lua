-- skill

function Act_Mon_Rush_OnCheckCanEnter(actor, action)
	return true
end

function Act_Mon_Rush_SetState(actor,action,kState)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iNewState = -1;
	ODS("kState : "..kState.."\n", false, 1509)
	if kState == "BATTLEIDLE_START" then
	
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false	
		end
				
		action:SetSlot(0)
		iNewState = 0;
		
		local	effect = actor:GetAnimationInfoFromAction("FIRE_EFFECT", action, 1000205)
		if nil ~= effect then
			local	node = actor:GetAnimationInfoFromAction("FIRE_EFFECT_NODE", action, 1000205)
			if nil == node then
				node = "char_root"
			end
		    actor:AttachParticle(129,node,effect)
		end
	
	elseif kState == "BATTLEIDLE_LOOP" then
		
		action:SetSlot(1)
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
		
			ODS("Act_Mon_Rush_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n");
		
			action:SetSlot(3);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1;
		
		if actor:IsEqualObjectName("m_Parel") then
		    actor:AttachParticle(129,"char_root","ef_parel_skill_05_01");
		else
			local	effect = actor:GetAnimationInfoFromAniName("FIRE_EFFECT", "dashattk_btlidle_01")
			if "NONE"==effect:GetStr() then

			else
				if nil==effect then
					effect = "ef_Mahalka_skill_03_01_char_root"
				end
			    actor:AttachParticle(129,"char_root",effect);
			end
		end
		
	elseif kState == "FIRE" then
		action:SetSlot(2)
		iNewState = 2;
	elseif kState == "RETURN" then
		action:SetSlot(4)
		if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then	
			action:SetSlot(3);	--	Finish 모션이 없을 때는 그냥 Idle 모션을 하자.
		end

		iNewState = 3;
	end
	
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(0,iNewState)

	return	true;

end

function Act_Mon_Rush_OnEnter(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_Mon_Rush_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");

	actor:SetNoWalkingTarget(false)
	actor:FreeMove(true)
	
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
			ODS("[SkillError] Act_Mon_Rush_OnEnter, kPacket is not valid. actor : "..actor:GetPilotGuid():GetString().." action:"..action:GetID().."\n")
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
		Act_Mon_Rush_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mon_Rush_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Rush_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	

		actor:LookAt(ptTargetPos,true,false,true);
		actor:SetMovingDir(actor:GetLookingDir());

	end
	
	action:SetParamInt(10,0)

			local pilot = actor:GetPilot()
			if false == pilot:IsNil() then
				pilot:SetGoalPos(Point3(0,0,0))
			end
	return true
end
function Act_Mon_Rush_OnCastingCompleted(actor,action)	

	Act_Mon_Rush_SetState(actor,action,"FIRE")
	
	Act_Mon_Rush_Fire(actor,action);	
end

function Act_Mon_Rush_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--	타겟 좌표 바라보기
	local	ptTargetPos = action:GetParamAsPoint(0)
	actor:LookAt(ptTargetPos,true,false,true);

	--	방향 벡터 구하기
	local	kCurrentPos = actor:GetPos()
	local	kDirVector = ptTargetPos:_Subtract(kCurrentPos)
	kDirVector:Unitize()
	
	action:SetParamAsPoint(1,kDirVector)
	
	if actor:IsEqualObjectName("m_Parel") then
		actor:AttachParticle(129,"char_root","ef_parel_skill_05_02");
	else
		local	effect = actor:GetAnimationInfo("FIRE_EFFECT")
		if nil ~= effect then
			local	node = actor:GetAnimationInfo("FIRE_EFFECT_NODE")
			if nil == node then
				node = "char_root"
			end
			if "NONE"~=effect then
			    actor:AttachParticle(129,node,effect)
			end
		else
		    actor:AttachParticle(129,"char_root","ef_Mahalka_skill_03_02_char_root");
		end
	end

	local	kSoundID = actor:GetAnimationInfo("SOUND_ID")
	if nil~=kSoundID and kSoundID~="" then
		actor:AttachSound(2784,kSoundID)
	end
end

function Act_Mon_Rush_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
			local pilot = actor:GetPilot()
			if false == pilot:IsNil() then
				pilot:SetGoalPos(Point3(0,0,0))
			end

	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0)

	if action:GetActionParam() == AP_FIRE then

		local	kMovingDir = action:GetParamAsPoint(1)
		local	kMovingTarget = action:GetParamAsPoint(0)
		local	fMoveSpeed = actor:GetAbil(AT_MOVESPEED)*(1+action:GetAbil(AT_R_MOVESPEED)/10000.0);
			
		local	ptTargetPos = action:GetParamAsPoint(0)
		local	kCurrentPos = actor:GetPos()

		local	kCurrentDir = ptTargetPos:_Subtract(kCurrentPos)
		ODS("거리 : " .. kCurrentDir:Length() .. "\n")
		kCurrentDir:Unitize()
		
		local	fDot = kCurrentDir:Dot(kMovingDir)
		if fDot<0 then
--			return	false
--		elseif fDot<0.01 then
			actor:DetachFrom(129)
			if 0 == action:GetParamInt(10) then
				if 0 < actor:GetAnimationLength(action:GetSlotAnimName(4)) then	--리런이 있으면
					Act_Mon_Rush_SetState(actor,action,"RETURN")
					action:SetParamInt(10,1)
				else
					return false	--리턴이 없으면 여기서 끝내고
				end
			else	--리턴 애니중이면
				if actor:IsAnimationDone() then
					return false
				end
			end
		else
			kMovingDir:Multiply(fMoveSpeed)
		
			actor:SetMovingDelta(kMovingDir);
		end
		
	elseif action:GetActionParam() == AP_CASTING then
 
		local fAddZ = actor:GetAnimationInfo("ADD_Z")
		
		if nil ~= fAddZ then

			local pos = actor:GetPos()
			fAddZ = tonumber(fAddZ)
			pos:SetZ(pos:GetZ() + fAddZ*frameTime)
			actor:SetTranslate(pos)
		end
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_Rush_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Rush_OnCastingCompleted(actor,action)
			
			end
		end
	end

	return true
end
function Act_Mon_Rush_OnCleanUp(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	actor:DetachFrom(129)
	actor:FreeMove(false)
end

function Act_Mon_Rush_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	ODS("Act_Mon_Rush_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
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
function Act_Mon_Rush_OnTargetListModified(actor,action,bIsBefore)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	local pilot = actor:GetPilot()
	if false == pilot:IsNil() then
		pilot:SetGoalPos(Point3(0,0,0))
	end
	
--	if bIsBefore == false then
        Actor_Mob_Base_DoDefaultMeleeAttack(actor,action);   --  타격 효과
        action:GetTargetList():ApplyActionEffects();    
--	end

end

function Act_Mon_Rush_OnEvent(actor,textKey)
	return true;
end
