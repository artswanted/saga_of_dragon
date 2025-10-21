function Act_Run_OnCheckCanEnter(actor,action)
--	if action:GetEnable() == false then
		-- 음.. 일단 주석처리..
--		local up = actor:GetActionState("a_run_up")
--		local down = actor:GetActionState("a_run_down")
--		local right = actor:GetActionState("a_run_right")
--		local left = actor:GetActionState("a_run_left")		
	
--		if up ~=0 or down ~=0 or right ~=0 or left ~=0 then
--			return false
--		end
--	end
	
	local curAction = actor:GetAction()
	local curActionID = curAction:GetID()
	if curActionID == "a_run" and actor:IsNowFollowing() == false then
		ODS("Current Action is \"a_run\" : transit failed!\n")
		return false 
	end

	return true
end

function Act_Run_OnEnter(actor, action)
	local prevAction = actor:GetAction()


	if actor:IsMyActor() then

		ODS("Run is Entered PrevAction : "..actor:GetAction():GetID().." \n")
	
	end

	if prevAction:IsNil() == false then
		local prevActionID = prevAction:GetID()
		local prevSlot = prevAction:GetCurrentSlot()
		if prevActionID == "a_jump" then
			-- 현재 액션이 점프이고 슬롯이 3번이 아니거나,
			-- 바닥이 아니면, 전이 불가능
			if prevSlot ~= 3 or
				actor:IsMeetFloor() == false then 
				action:SetDoNotBroadCast(true)
				ODS("__________________return false on run action _________________\n")
				return false
			end
		elseif prevActionID == "a_run" then
			return false
		end
		-- 그 외에는 전이 가능
	end

	actor:UseSkipUpdateWhenNotVisible(false);
	action:SetParamInt(1,0)
	
	local	kTargetPos = action:GetParamAsPoint(0)
	if kTargetPos:IsZero() == false then
	
		action:SetParamInt(3,1);
		
		local	kMoveDirection = kTargetPos:_Subtract(actor:GetPos())
		kMoveDirection:Unitize();
		action:SetParamAsPoint(1,kMoveDirection);
		action:SetParamAsPoint(2,actor:GetPos());
		
		actor:LookAt(kTargetPos,true,true,false);
		
	end
	

	return true
end

function Act_Run_OnUpdate(actor, accumTime, frameTime)
	local actorID = actor:GetID()
	local action = actor:GetAction()
	local movingSpeed = 0
	local	bMoveToPos = (action:GetParamInt(3) == 1);

	

	if IsSingleMode() == true then
		movingSpeed = 120 
	else
		movingSpeed = actor:GetAbil(AT_C_MOVESPEED)
	end
	
	local fOriginalMoveSpeed = actor:GetAbil(AT_MOVESPEED)
	if fOriginalMoveSpeed == 0 then
		fOriginalMoveSpeed = movingSpeed
	end
	
	local	fAnimSpeed = 0.0;
	
	if fOriginalMoveSpeed>0 then
		fAnimSpeed = movingSpeed/fOriginalMoveSpeed
	end
	
	actor:SetAnimSpeed(fAnimSpeed);	
	
	Act_Idle_DoAutoFire(actor);
	
	if actor:IsMyActor() and action:GetParamInt(1) == 0 then
	
		if accumTime - action:GetActionEnterTime() > 0.1 then
		
			actor:SetComboCount(0);
			action:SetParamInt(1,1);
		end
	
	end
	
	
	if bMoveToPos then
		
		local	kMoveTargetPos = action:GetParamAsPoint(0)
		local	kMoveDirection = action:GetParamAsPoint(1)	
		local	kMoveStartPos = action:GetParamAsPoint(2)
		
		local	kDir1 = actor:GetPos():_Subtract(kMoveTargetPos);
		kDir1:Unitize();
		local	kDir2 = kMoveStartPos:_Subtract(kMoveTargetPos);
		kDir2:Unitize();
		
		if kDir1:Dot(kDir2) < 0 then
		
			actor:SetTranslate(kMoveTargetPos);
			return	false;
		
		end
		
		kMoveDirection:Multiply(movingSpeed);
		
		actor:SetMovingDelta(kMoveDirection);
		
	
		return	true;
	end
	
	local dir = actor:GetDirection()
	
	if actor:IsMyActor() and actor:IsNowFollowing() == false then
		local	bKeyUp = KeyIsDown(KEY_UP);
		local	bKeyDown = KeyIsDown(KEY_DOWN);
		local	bKeyLeft = KeyIsDown(KEY_LEFT);
		local	bKeyRight = KeyIsDown(KEY_RIGHT);
		
		if (dir == DIR_UP and bKeyUp == false) or
			(dir == DIR_LEFT and bKeyLeft == false) or
			(dir == DIR_RIGHT and bKeyRight == false) or
			(dir == DIR_DOWN and bKeyDown == false) or
			(bKeyUp == false and
				bKeyDown == false and
				bKeyLeft == false and
				bKeyRight == false) then
		
			actor:SetDirection(0);
			dir = 0;
			return	false;
			
		end
	end
	
	if dir == DIR_NONE then
		if actor:GetWalkingToTarget() == false then
			ODS("[Act_Run_OnUpdate] Direction is None : transit Next Action\n")
			--actor:FindPathNormal()
			return false 
		end
	end
	
	if movingSpeed == 0 then
	    return  false
    end

	actor:Walk(dir, movingSpeed)

	local vel = actor:GetVelocity()
	local z = vel:GetZ()

	-- 뛰어가다가 발이 땅에서 떨어졌을 경우
	-- 올라가는 점프를 해야 할지, 내려오는 점프를 해야 할지 결정
	if actor:IsMeetFloor() == false then
		if z < -2 then
			action:SetNextActionName("a_jump")
			action:SetParam(2, "fall_down")
			ODS("Jump(fall down)\n")
			return false
		elseif z > 2 then
			action:SetNextActionName("a_jump")
			action:SetParam(2, "fall_up")
			ODS("Jump(fall up)\n")
			return false
		end
	end

	-- 사다리 체크
	if actor:ContainsDirection(DIR_UP) and 
		actor:IsMyActor() and
		actor:ClimbUpLadder() then 
		action:SetNextActionName("a_ladder_idle")
		ODS("Direction is None3\n")
		return false
	end
	
	return true
end
function Act_Run_OnCleanUp(actor, action)
end

function Act_Run_OnLeave(actor, action)
	local curAction = actor:GetAction()
	local kActionID = action:GetID()
	ODS("Act_Run_OnLeave nextid : "..action:GetID().."\n");

	if kActionID == "a_jump" then
		local param = curAction:GetParam(2)
		if param == "fall_down" then
			action:SetSlot(2)
			action:SetDoNotBroadCast(true)
		elseif param == "fall_up" then 
			action:SetSlot(1)
			action:SetDoNotBroadCast(true)
		elseif param == "null" then
			action:SetSlot(1)
		end
	elseif kActionID == "a_ladder_idle" or
		kActionID == "a_ladder_down" or
		kActionID == "a_ladder_up" then
		actor:HideParts(6, true)
		actor:SetParam("LADDER_WEAPON_HIDE","TRUE");
	elseif kActionID == "a_idle" then
		--action:SetDoNotBroadCast(true)
		return true
	end
	actor:UseSkipUpdateWhenNotVisible(true);

	return true
end

function Act_Run_OnEvent(actor, textKey)
	if textKey == "start" then
		actor:AttachParticle(math.random(-100, -200), "char_root", "e_run")
	end
end
