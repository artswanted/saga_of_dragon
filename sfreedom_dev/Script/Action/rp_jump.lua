jumpForce = 100 			-- StartJump에서 사용
secondJumpForce = jumpForce -- StartJump에서 사용
landingThreshold = 70

function Act_RP_Jump_CheckBreakFall(actor)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	if actor:IsMyActor() then	--	낙법 처리
	
		local	action = actor:GetAction();
		CheckNil(nil == action);
		CheckNil(action:IsNil());

		if action:IsNil() == false and action:CanBreakFall() then
			local	fLastPressTime = action:GetParamFloat(6)
			if fLastPressTime == 0 then
			
				fLastPressTime = g_world:GetAccumTime();
				action:SetParamFloat(6,fLastPressTime);
				
			end		
		end
	
		if actor:IsDownState() then
		
			local	fDownTime = actor:GetTotalDownTime()
			fDownTime = fDownTime - 0.5
			if fDownTime < 0 then
				fDownTime = 0
			end
			actor:SetTotalDownTime(fDownTime);
			
		end
				
	end
	
end

function Act_RP_Jump_IsFloatEvasion(actor,action)

	--ODS("Act_Jump_IsFloatEvasion\n");

	if action:GetParamInt(7) == 1 then
		return	true
	end

	if actor:IsMyActor() == false then
		return	false
	end

	if actor:IsMeetFloor() then
		return	false
	end

	local	kCurAction = actor:GetAction();
	if kCurAction:IsNil() then
		return	false
	end

	local	kInputSlotInfo = action:GetInputSlotInfo();
	if kInputSlotInfo:IsNil() then
		--ODS("Act_Jump_IsFloatEvasion kInputSlotInfo:IsNil()\n");
		return	false
	end

	if kInputSlotInfo:GetUKey() ~= 3023 then
		--ODS("Act_Jump_IsFloatEvasion kInputSlotInfo:GetUKey() ~= 3023\n");
		return	false
	end

	if actor:GetParam("FLOAT_EVASION") == "FALSE" then
		return	false
	end

	if actor:IsBlowUp() == false then
		--ODS("Act_Jump_IsFloatEvasion actor:IsBlowUp() == false\n");
		return	false
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local	fActionStartTime = kCurAction:GetActionEnterTime();
	local	fCurrentTime = g_world:GetAccumTime()

	if (fCurrentTime - fActionStartTime) < g_fEvasionStartTime then
		return	false
	end

	action:SetParamInt(7,1)
	actor:SetParam("FLOAT_EVASION","FALSE");
	actor:SetCanHit(false);

	return	true;


end


-- Jump 
function Act_RP_Jump_OnCheckCanEnter(actor,action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	local prevAction = actor:GetAction();

	if prevAction:IsNil() then
		return	true
	end


	local kPacket = action:GetParamAsPacket()
	if kPacket ~= nil and kPacket:IsNil() == false then		
		action:SetParamFloat(4,kPacket:PopFloat());
		action:SetParamInt(7,kPacket:PopInt());
	end


	local actionName = prevAction:GetID()

	--	낙법 체크
	Act_RP_Jump_CheckBreakFall(actor);

	if IsSingleMode() == false and actor:GetAbil(AT_C_MOVESPEED) == 0 then -- 0 이면 점프 할 수 없는 상태
		return false;
	end

	local kActorPet = actor:GetMountTargetPet()
	if kActorPet:IsNil() then
		return true
	end
	local kPetAction = kActorPet:GetAction()
	if kPetAction:IsNil() then
		return true
	end
	
	if kPetAction:GetID() ~= "a_riding_ground" then
		return false
	end
		
	if Act_RP_Jump_IsFloatEvasion(actor,action) then
		actor:ClearAllActionEffect();
		return	true
	end

    if actor:IsMeetFloor() == false then
        if 	actionName == "a_rp_walk" or
			actionName == "a_rp_jump" then
			--ODS("Act_Jump_OnCheckCanEnter - true : "..actionName.."\n", false, 3851)
		    return  true
		end
        return  false
    end

	return true;
end

function Act_RP_Jump_OnEnter(actor, action)
	-- check prveious action
	-- 아직까지는 actor의 Action이 이전 액션이다.
	-- OnEnter에서 true를 되돌려 주면, 이전의 액션은 
	-- 완전히 삭제된다.

--	if action:GetParamFloat(123456) == 0 then
--		action:SetParamFloat(123456, timeGetTime())
--	end

	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	local prevAction = actor:GetAction()

	if	prevAction:IsNil() then
		return	true
	end
	
	local actionName = prevAction:GetID()

	local kActorPet = actor:GetMountTargetPet()
	if kActorPet:IsNil() then
		return false
	end
	local kActionPet = kActorPet:GetAction()
	if kActionPet:IsNil() then
		return false
	end
	--ODS("Act_Jump_OnEnter actionName:"..actionName.."\n");
	
	local	fJumpForce = jumpForce;
	local	bIsFloatEvasion = (action:GetParamInt(7) == 1);
	
	if action:GetParam(4) ~= "null" then
		fJumpForce = tonumber(action:GetParam(4));
		if fJumpForce == nil then
			fJumpForce = jumpForce
		end
	end
	
	if bIsFloatEvasion then
		fJumpForce = 30;
	end
		
	action:SetParamFloat(4,fJumpForce);
	
	if action:GetParam(5) == "HiJump" then
		--UseCameraHeightAdjust(false)
	end
	
	if bIsFloatEvasion then
		actor:StartJump(fJumpForce)	
		return	true;
	end
	
	if actionName == "a_rp_walk" or
		action:GetParam(5) == "HiJump" then
		--if action:GetCurrentSlot() == 1 then
		if kActionPet:GetCurrentSlot() == 8 then
			if prevAction:GetParam(2) ~= "fall_up" then
				actor:StartJump(fJumpForce)				
			end
			local	pt = actor:GetTranslate()
			pt:SetZ(pt:GetZ() - 30)
			kActorPet:AttachParticleToPoint(2, pt, "e_jump")
		--elseif action:GetCurrentSlot() == 2 then
		elseif kActionPet:GetCurrentSlot() == 9 then
			if actor:IsJumping() == false then
				actor:StartJump(0)
			end
		end
		kActionPet:SetSlot(8)
		kActorPet:PlayCurrentSlot()
		return true
	elseif actionName == "a_rp_jump" then
		if action:GetParam(1) == "jump_again" then
			actor:StartJump(fJumpForce/2)
			action:SetSlot(4)
			kActionPet:SetSlot(11)
			return true
		end
		if actor:IsMeetFloor() == true and
			actor:IsSlide() == false then
			action:SetSlot(1)
			kActionPet:SetSlot(7)
			kActorPet:PlayCurrentSlot()
			actor:StartJump(fJumpForce)
			
			return true
		else
		
			action:SetSlot(2);
			kActionPet:SetSlot(9)
		
		    --ODS("Act_Jump_OnEnter Failed. actionname is a_jump\n");
			return false
		end		
	elseif actionName == "a_rp_idle" then	    	
		if actor:IsMeetFloor() == false and
			action:GetCurrentSlot() ~= 2 then
			--ODS("Act_Jump_OnEnter Failed action:GetCurrentSlot() ~= 2 / actionName : "..actionName.."\n", false, 3851);
			return false
		end
		--ODS("Act_Jump_OnEnter - true : "..actionName.."\n", false, 3851)

		kActionPet:SetSlot(7)
		kActorPet:PlayCurrentSlot()
		return true
	end

	--ODS("Act_Jump_OnEnter Failed actionName : "..actionName.."\n", false, 3851);
	return false 
end
function Act_RP_Jump_OnOverridePacket(actor, action, packet)
	CheckNil(nil == action);
	CheckNil(action:IsNil());

	CheckNil(nil == packet);
	CheckNil(packet:IsNil());

	packet:PushFloat(action:GetParamFloat(4))
	packet:PushInt(action:GetParamInt(7))	--	Is this Float Evasion ?
end

function Act_RP_Jump_OnUpdate(actor, accumTime, frameTime)
	if actor:IsNil() then
		return false
	end
	local action = actor:GetAction()
	if action:IsNil() then
		return false
	end

	local kActorPet = actor:GetMountTargetPet()
	if kActorPet:IsNil() then
		return false
	end
	local kActionPet = kActorPet:GetAction()
	if kActionPet:IsNil() then
		return false
	end

	local curAnimSlot = kActionPet:GetCurrentSlot() -- Current Animation Slot
	local movingSpeed = 0
	local fJumpForce = action:GetParamFloat(4);

	if IsSingleMode() == true then
		movingSpeed = 150
	else
		movingSpeed = kActorPet:GetAbil(AT_C_MOVESPEED)
	end

	if nil ~=g_world then
		if g_world:GetAttr() == GATTR_VILLAGE then
			movingSpeed = movingSpeed + actor:GetAbil(AT_C_VILLAGE_MOVESPEED)
		end
	end

	local vel = actor:GetVelocity() 	-- Current Velocity
	local z = vel:GetZ() 				-- Gravity

	local param = action:GetParam(0)

	-- Gravity값이 깔끔하게 0이 나오지 않는 것에 주의.
	local IsAnimDone = kActorPet:IsAnimationDone()

	local nextAction = "a_rp_idle"
	local dir = actor:GetDirection()

	----ODS("Current Slot = " .. curAnimSlot .. "\n")

	if dir ~= DIR_NONE then
		actor:Walk(dir, movingSpeed)
		nextAction = "a_rp_walk"
	elseif actor:GetWalkingToTarget() == true then
		actor:Walk(dir, movingSpeed, false)
--	else
--		WriteToConsole("________공중에서 움직인 시간 : " .. timeGetTime() - action:GetParamFloat(123456) .. "AccumHeight : " .. actor:GetJumpAccumHeight() .. " JumpTime : " .. actor:GetJumpTime() .. " Moving Speed : " .. movingSpeed .. "\n")
	end

	if curAnimSlot == 7 then
		if IsAnimDone == true then
			actor:StartJump(fJumpForce)
            --Act_Jump_SetComboAdvisor(actor,action);
			
			local	pt = actor:GetTranslate()
			pt:SetZ(pt:GetZ() - 30)
			kActorPet:AttachParticleToPoint(2, pt, "e_jump")
			actor:PlayNext()
			kActorPet:PlayNext()
			return true
		elseif dir ~= DIR_NONE then
			actor:PlayNext()
			kActorPet:PlayNext()
			actor:StartJump(fJumpForce)
            --Act_Jump_SetComboAdvisor(actor,action);

			
			local	pt = actor:GetTranslate()
			pt:SetZ(pt:GetZ() - 30)
			kActorPet:AttachParticleToPoint(2, pt, "e_jump")
		elseif z > landingThreshold then
			actor:PlayNext()
			kActorPet:PlayNext()
		end
	
	elseif curAnimSlot == 8 then
		if kActorPet:IsMeetFloor() == true and 
			actor:IsSlide() == false and
			actor:IsJumping() == false then
			if dir == DIR_NONE then
				actor:Stop()
				action:SetSlot(3)
				actor:PlayCurrentSlot()
				kActionPet:SetSlot(9)
				kActorPet:PlayCurrentSlot()
			else
				-- Jump가 3번 슬롯이어야 run으로 Enter가능
				-- param으로 run이 들어오면 착지후에 달리는 애니를 주겠다는 뜻
				if nextAction ~= "a_rp_idle" then
					action:SetSlot(3)
					kActionPet:SetSlot(9)
					action:SetNextActionName(nextAction)
					return false
				elseif action:GetParam(1) ~= "jump_again" then
					action:SetNextActionName("a_rp_jump")
					return false
				end
			end
		elseif IsAnimDone == true then
			actor:PlayNext()
			kActorPet:PlayNext()
		end
	
	elseif curAnimSlot == 9 then
		if kActorPet:IsMeetFloor() == true and
			actor:IsSlide() == false then
			if dir == DIR_NONE then
				actor:Stop()
				actor:PlayNext()
				kActorPet:PlayNext()
			else
				-- param으로 run이 들어오면 착지후에 달리는 애니를 주겠다는 뜻
				action:SetSlot(3)
				kActionPet:SetSlot(11)
				action:SetNextActionName(nextAction)
				return false
			end		
		end
	elseif curAnimSlot == 10 then 
		if IsAnimDone == true then
			return false
		else
			if nextAction == "a_rp_jump" then
				action:SetNextActionName(nextActionName)
				kActionPet:SetSlot(7)
				return false
			end
		end
	elseif curAnimSlot == 11 then 
		if kActorPet:IsMeetFloor() == true then
			-- param으로 run이 들어오면 착지후에 달리는 애니를 주겠다는 뜻
			action:SetSlot(3)
			kActionPet:SetSlot(10)
			action:SetNextActionName(nextAction)
			return false
		elseif IsAnimDone == true then
			action:SetSlot(2)
			actor:PlayCurrentSlot()
			kActionPet:SetSlot(9)
			kActorPet:PlayCurrent()
		end
	end
	
	return true
end
function Act_RP_Jump_OnCleanUp(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	local curAction = actor:GetAction()

	CheckNil(nil == curAction);
	CheckNil(curAction:IsNil());
	
	if curAction ~= nil and curAction:IsNil() == false and curAction:GetParam(5) == "HiJump" then
		UseCameraHeightAdjust(true)
	end
end

function Act_RP_Jump_OnLeave(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	local curAction = actor:GetAction()
	CheckNil(nil == curAction);
	CheckNil(curAction:IsNil());

	local nextActionName = action:GetID()
	CheckNil(nil == action);
	CheckNil(action:IsNil());
	
	--ODS("Act_Jump_OnLeave nextActionName:"..nextActionName.."\n");

	-- 새로운 액션이 들어왔을 때
	if action:GetEnable() == true then
		if nextActionName == "a_rp_jump" and 
			curAction:GetParam(1) ~= "jump_again" then
			if actor:GetAbil(AT_DOUBLE_JUMP_USE) == 1 then
				if curAction:GetCurrentSlot() == 1 or 
					curAction:GetCurrentSlot() == 2 then
					if "TRUE" == actor:GetParam("DOUBLE_JUMP") then
						action:SetParam(1, "jump_again")
					end
				end
				return true
			end
		elseif nextActionName == "a_rp_walk" or
			actor:IsMeetFloor() and 
			actor:IsMeetSide() == false then
			--action:SetDoNotBroadCast(true)
			return true
		elseif nextActionName == "a_rp_idle" then
			curAction:SetParam(0, "null")
			actor:DetachFrom(2)
		elseif nextActionName ~= "a_telejump" and
				nextActionName ~= "a_jump" and
				nextActionName ~= "a_idle" and
				nextActionName ~= "a_dmg" and
				nextActionName ~= "a_walk" and
				nextActionName ~= "a_walk_left" and
				nextActionName ~= "a_walk_right" and
				nextActionName ~= "a_walk_up" and
				nextActionName ~= "a_walk_down" and
				nextActionName ~= "a_run" and
				nextActionName ~= "a_trap" and
				nextActionName ~= "a_run_left" and
				nextActionName ~= "a_run_right" and
				nextActionName ~= "a_run_up" and
				nextActionName ~= "a_run_down" then
			return false
		end
		return true
	end
	return false
end

