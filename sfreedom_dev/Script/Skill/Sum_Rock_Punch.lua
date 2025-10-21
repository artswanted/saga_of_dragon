-- skill

function Sum_Rock_Punch_OnCheckCanEnter(actor, action)
	return true
end

function Sum_Rock_Punch_SetState(actor,action,kState)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	local	iNewState = -1;
	ODS("kState : "..kState.."\n", false, 1509)
	if kState == "BATTLEIDLE_START" then
	
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false	
		end
				
		action:SetSlot(0)
		iNewState = 0;
	
	elseif kState == "BATTLEIDLE_LOOP" then
		
		action:SetSlot(1)
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
		
			ODS("Sum_Rock_Punch_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n");
		
			action:SetSlot(3);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1;
		
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

function Sum_Rock_Punch_OnEnter(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Sum_Rock_Punch_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n",false,6482);
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Sum_Rock_Punch_OnCastingCompleted(actor,action)
	else
	
		if( Sum_Rock_Punch_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Sum_Rock_Punch_SetState(actor,action,"BATTLEIDLE_LOOP")
		end

	end
	
	action:SetParamInt(12,0)

	return true
end

function Sum_Rock_Punch_OnCastingCompleted(actor,action)	

	Sum_Rock_Punch_SetState(actor,action,"FIRE")
	
	Sum_Rock_Punch_Fire(actor,action);	
	
end

function Sum_Rock_Punch_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(10,g_world:GetAccumTime())
	
	--ODS("Sum_Rock_Punch_Fire: "..action:GetParamFloat(10).."\n",false,6482)
	
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
		actor:AttachParticle(129,"char_root","efx_gorgon_rock_punch_02");
	end

	local	kSoundID = actor:GetAnimationInfo("SOUND_ID")
	if nil~=kSoundID and kSoundID~="" then
		actor:AttachSound(2784,kSoundID)
	end
end

function Sum_Rock_Punch_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
		
		local	fElapsedTime = accumTime - action:GetParamFloat(10)
		local	fTotalTime = action:GetAbil(AT_MAINTENANCE_TIME) / 1000.0

		--ODS("ElapsedTime: "..fElapsedTime..", TotalTime: "..fTotalTime.."\n",false,6482)
		
		if fElapsedTime > fTotalTime then
			
			--ODS("Param10: "..action:GetParamInt(12).."\n",false,6482)
			
			if 0 == action:GetParamInt(12) then
				if 0 < actor:GetAnimationLength(action:GetSlotAnimName(4)) then	--리런이 있으면
					Sum_Rock_Punch_SetState(actor,action,"RETURN")
					action:SetParamInt(12,1)
				else
					return false	--리턴이 없으면 여기서 끝내고
				end
			else	--리턴 애니중이면
				if actor:IsAnimationDone() then
					--ODS("리턴 애니끝\n",false,6482)
					return false
				end
			end
		end

	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Sum_Rock_Punch_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end

	end

	return true
end
function Sum_Rock_Punch_OnCleanUp(actor, action)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	actor:DetachFrom(129)
end

function Sum_Rock_Punch_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	ODS("Sum_Rock_Punch_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
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
function Sum_Rock_Punch_OnTargetListModified(actor,action,bIsBefore)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
			
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
--	if bIsBefore == false then
        Actor_Mob_Base_DoDefaultMeleeAttack(actor,action);   --  타격 효과

		local QuakeTime = actor:GetAnimationInfo("QUAKE_TIME")
		if nil ~= QuakeTime and "" ~= QuakeTime then
			QuakeTime = tonumber(QuakeTime)
			local QuakeFactor = actor:GetAnimationInfo("QUAKE_FACTOR")
			if  nil == QuakeFactor then
				QuakeFactor = 1.0
			else
				QuakeFactor = tonumber(QuakeFactor)
			end
			QuakeCamera(QuakeTime, QuakeFactor)
		end
--	end

end

function Sum_Rock_Punch_OnEvent(actor,textKey)
	return true;
end
