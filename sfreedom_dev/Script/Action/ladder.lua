-- Ladder Idle

-- actor에 있는 action은 Prviuos Action이다.
-- action 인자는 현재 Ladder를 나타낸다.
function Act_Ladder_OnEnter(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

--	CheckNil(nil == action);
--	CheckNil(action:IsNil());

	local prevAction = actor:GetAction();
	if( false == prevAction:IsNil() ) then
		local prevActionID = prevAction:GetID()
		if prevActionID == "a_jump" or 
			prevActionID == "a_ladder_dash" then
			actor:StopJump()
		end
	end

	actor:LockBidirection(false)
	actor:Stop()
	actor:SeeLadder();
	actor:Concil(false)
	actor:FreeMove(true)	
	if actor:IsMyActor() then
		UseCameraHeightAdjust(false)	-- 카메라 높이 조정을 끔
	end

	return true
end

function Act_Ladder_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction();

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	local nextActionName = action:GetID();
	--actor:SeeFront(false)
	local ladderSpeed = 1.6 
	actor:SetAnimSpeed(ladderSpeed)
	actor:StopJump()

	local up = actor:ContainsDirection(DIR_UP)
	local down = actor:ContainsDirection(DIR_DOWN)

	if actor:IsAnimationDone() == true then
		if up then
			nextActionName = "a_ladder_up"
		elseif down then
			nextActionName = "a_ladder_down"
		else
			nextActionName = "a_ladder_idle"
		end
	end

	if actor:IsMyActor() then
		-- 캐릭터등 뒤에 사다리가 없다면!
		if actor:ClimbUpLadder(actor) == false then
			if nextActionName == "a_ladder_up" then		
				-- 다 올라갔거나
				nextActionName = "a_jump"
				action:SetParam(15, "ClimbUpFinish")
			elseif nextActionName == "a_ladder_down" then	
				-- 다 내려왔다(사다리가 공중에서 끝났을 때)				
				nextActionName = "a_idle"
				action:SetParam(15, "ClimbDownFinish")
			end
			action:SetNextActionName(nextActionName)
			return false 
		elseif actor:IsMeetFloor() == true and
			nextActionName == "a_ladder_down" then
			nextActionName = "a_idle"
			action:SetParam(15, "ClimbDownFinish")			
		end
	end

	local param = action:GetParam(100)
	if param == "null" then
		action:SetParam(100, tostring(accumTime))
		param = tostring(accumTime)
	end

	local lastAccumTime = tonumber(param)	

	local timeDiff = accumTime - lastAccumTime
	local ladderVel = 10
	if timeDiff == 0.0 then
		ladderVel = 10 
	elseif timeDiff < 0.267 * 0.7 and timeDiff > 0.0 then
		ladderVel = math.sin(timeDiff) * 400 * ladderSpeed
	elseif timeDiff < 0.4 * 0.7 and timeDiff > 0.267 * 0.7 then
		ladderVel = math.sin(timeDiff-0.267 * 0.7) * 20 * ladderSpeed
	elseif timeDiff < 0.667 * 0.7 and timeDiff > 0.4 * 0.7 then
		ladderVel = math.sin(timeDiff-0.4 * 0.7) * 400 * ladderSpeed
	elseif timeDiff < 0.8 * 0.7 and timeDiff > 0.667 * 0.7 then
		ladderVel = math.sin(timeDiff-0.667 * 0.7) * 20 * ladderSpeed
	end

	ladderVel = 100
	if nextActionName == "a_ladder_down" then
		ladderVel = ladderVel * -1.0
	end

	actor:MoveActor(0,0,ladderVel)

	if action:GetID() ~= nextActionName and
		actor:IsMyActor() then
		action:SetNextActionName(nextActionName)
		return false
	elseif actor:IsAnimationDone() == true then		
		actor:ActivateAnimation()
		action:SetParam(100, tostring(accumTime))
		return true
	end

	return true
end

function Act_Ladder_ReallyLeave(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

--	CheckNil(nil == action);
--	CheckNil(action:IsNil());

	if actor:IsNil() == true then
		return false
	end
	if actor:GetParam("LADDER_WEAPON_HIDE") == "TRUE" then
		actor:HideParts(6, false)
		actor:SetParam("LADDER_WEAPON_HIDE","FALSE")
	end	
	actor:FreeMove(false)
	actor:Concil(true)
	actor:SeeFront(true, true)
	if actor:IsMyActor() then
		UseCameraHeightAdjust(true)
	end
	return true
end
function Act_Ladder_OnCleanUp(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	CheckNil(nil == action);
	CheckNil(action:IsNil());
	
	if action ~=nil and action:IsNil() == false then
	
		if action:GetID() == "a_teleport" then
			action:SetParam(1423, "ladder")
		end	
	end
	
	if string.sub(action:GetID(),1,8) ~= "a_ladder" then
		Act_Ladder_ReallyLeave(actor, action)
	end
	
	actor:RestoreLockBidirection()
end


-- action 인자는 다음으로 전이될 Next Action이다.
function Act_Ladder_OnLeave(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	local left = actor:ContainsDirection(DIR_LEFT)
	local right = actor:ContainsDirection(DIR_RIGHT)
	local bMyActor = actor:IsMyActor()
	
	local curAction = actor:GetAction()

	CheckNil(nil == curAction);
	CheckNil(curAction:IsNil());

	local curActionID = curAction:GetID()
	local actionID = action:GetID()
	local dir = actor:GetDirection()

	if action:GetEnable() == true then
		if bMyActor == true then
			if actionID == "a_jump" then
				if left ~= right or
					curAction:GetParam(15) == "ClimbUpFinish" then
					action:SetSlot(1)
					return Act_Ladder_ReallyLeave(actor, action)	
				end
			elseif actionID == "a_ladder_down" or
				actionID == "a_ladder_up" or
				actionID == "a_ladder_dash" or
				actionID == "a_ladder_idle" then
				if curActionID =="a_ladder_idle" or
					(curActionID == "a_ladder_up" and actionID == "a_ladder_dash") or
					actor:IsAnimationDone() == true then
					return true
				end
				return false
			elseif actionID == "a_idle" or actionID == "a_run" then
				if curAction:GetParam(15) == "ClimbDownFinish" then
--					actor:IsMyActor() == false then
					return Act_Ladder_ReallyLeave(actor, action)	
				end
			elseif actionID == "a_teleport" then
				action:SetParam(1423, "ladder")
				return Act_Ladder_ReallyLeave(actor, action)
			end
		else
			-- 다른 액터에 대해서는 체크 할 필요 없이 return true 
			if actionID == "a_ladder_down" or
				actionID == "a_ladder_up" or
				actionID == "a_ladder_dash" or
				actionID == "a_ladder_idle" then
				if curActionID =="a_ladder_idle" or
					(curActionID == "a_ladder_up" and actionID == "a_ladder_dash") or
					actor:IsAnimationDone() == true then
					return true
				end
				return false
			elseif actionID == "a_jump" or
				actionID == "a_idle" or 
				actionID == "a_run" then
				return Act_Ladder_ReallyLeave(actor, action)
			elseif actionID == "a_teleport" then
				action:SetParam(1423, "ladder")
				return Act_Ladder_ReallyLeave(actor, action)
			end
		end
	end
	return false 
end


function Act_Ladder_Idle_OnEnter(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	local prevAction = actor:GetAction();
	if( false == prevAction:IsNil() ) then 
		local prevActionID = prevAction:GetID()
		if prevActionID == "a_ladder_dash" or
			prevActionID == "a_jump" then
			actor:StopJump()
		end
	end

	-- 무기를 제거한다.
	actor:Stop()
	actor:LockBidirection(false)
	actor:SeeLadder();
	actor:Concil(false)
	actor:FreeMove(true)
	if actor:IsMyActor() then
		UseCameraHeightAdjust(false)	-- 카메라 높이 조정을 끔
	end

	return true
end

function Act_Ladder_Idle_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	local nextActionName = "a_ladder_idle"
	--actor:SeeFront(false)

	local up = actor:ContainsDirection(DIR_UP)
	local down = actor:ContainsDirection(DIR_DOWN)

	if up then
		nextActionName = "a_ladder_up"
	elseif down then
		nextActionName = "a_ladder_down"
	end

	if action:GetID() ~= nextActionName then
		action:SetNextActionName(nextActionName)
		return false
	end

	return true
end
function Act_Ladder_Idle_OnCleanUp(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	if actor:IsMyActor() then
		UseCameraHeightAdjust(true)
	end
	
	if string.sub(action:GetID(),1,8) ~= "a_ladder" then
		Act_Ladder_ReallyLeave(actor, action)
	end
	actor:RestoreLockBidirection()	
end

function Act_Ladder_Idle_OnLeave(actor, action)
	local bReturn = Act_Ladder_OnLeave(actor, action)
	
	return	bReturn;
end
