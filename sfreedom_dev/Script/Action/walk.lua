g_mmst = -1;	--	Monster Move Speed For Test 테스트용 이동속도 -1일 경우 이 값이 적용되지 않는다. -1 이외의 값일 경우 적용된다.

function Act_Walk_OnEnter(actor, action)

	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local prevAction = actor:GetAction()
	if prevAction:IsNil() == false then
		prevActionID = prevAction:GetID()
		if prevActionID == "a_jump" then
			-- 현재 액션이 점프이고 슬롯이 3번이 아니거나,
			-- 바닥이 아니면, 전이 불가능
			prevSlot = prevAction:GetCurrentSlot()
			if prevSlot ~= 3 or
				actor:IsMeetFloor() == false then 
				action:SetDoNotBroadCast(true)
				return false
			end
		elseif prevActionID == "a_walk_left" or
			prevActionID == "a_walk_right" or
			prevActionID == "a_walk_up" or
			prevActionID == "a_walk_down" then
			action:SetDoNotBroadCast(true)
			return false
		end
		-- 그 외에는 전이 가능
	end

	actor:FindPathNormal()
	if action:GetID() == "a_walk_left" then
		actor:ToLeft(true,true);
	elseif action:GetID() == "a_walk_right" then 
		actor:ToLeft(false,true);
	end
	
	actor:ResetAnimation();
	
	return true
end

function Act_Walk_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	if( CheckNil(action == nil) ) then return false end	
	if( CheckNil(action:IsNil()) ) then return false end
	
	local fOriginalMoveSpeed = actor:GetAbil(AT_MOVESPEED)
	local movingSpeed = actor:GetAbil(AT_C_MOVESPEED)

	if fOriginalMoveSpeed == 0 then
		fOriginalMoveSpeed = movingSpeed
	end
		
	local	fAnimSpeed = 0.0;
	
	if fOriginalMoveSpeed>0 then
		fAnimSpeed = movingSpeed/fOriginalMoveSpeed
	end
	
	actor:SetAnimSpeed(fAnimSpeed);
	
	if string.sub(actorID, 1, 2) == "c_" then
		movingSpeed = movingSpeed * 0.6
	else
		if g_mmst ~= -1 then
			movingSpeed = g_mmst;
		end
	end
	
	if IsSingleMode() == true then
		movingSpeed = 120 * 0.6
	end

	local dir = actor:GetDirection() 

    ODS("Act_Walk_OnUpdate actor:"..actorID.." GUID:"..actor:GetPilotGuid():GetString().." movingSpeed:"..movingSpeed.."\n", false, 1509);
	if actor:Walk(dir, movingSpeed, frameTime) == false then
	    return  false
	end

	local vel = actor:GetVelocity()
	local z = vel:GetZ()
	
	-- 뛰어가다가 발이 땅에서 떨어졌을 경우
	-- 올라가는 점프를 해야 할지, 내려오는 점프를 해야 할지 결정
	if actor:IsMeetFloor() == false then
		if z < -2 then
			action:SetNextActionName("a_jump")
			action:SetParam(2, "fall_down")
			return false
		elseif z > 2 then
			action:SetNextActionName("a_jump")
			action:SetParam(2, "fall_up")
			return false
		end
	end


	return true
end
function Act_Walk_OnCleanUp(actor, action)
	return true;
end

function Act_Walk_OnLeave(actor, action)
	
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local curAction = actor:GetAction()

	if curAction:IsNil() == false and 
		action:GetID() == "a_jump" then
		param = curAction:GetParam(2)
		if param == "fall_down" then
			action:SetSlot(2)
		elseif param == "fall_up" or
			param == "null" then
			action:SetSlot(1)
		end
	end

	return true
end

function Act_Walk_OnEvent(actor, textKey)
	
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	if textKey == "start" then
		actor:AttachParticle(math.random(-100, -200), "char_root", "e_run")
	end
end
