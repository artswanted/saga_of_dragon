-- Ladder Dash

-- actor에 있는 action은 Prviuos Action이다.
-- action 인자는 현재 Ladder를 나타낸다.
function Act_Ladder_Dash_OnEnter(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	local prevAction = actor:GetAction();
	if( false == prevAction:IsNil() ) then
		local prevActionID = prevAction:GetID();
		if prevActionID ~= "a_ladder_up" then
			action:SetDoNotBroadCast(true)
			return false
		end
	end
	actor:Stop()
	actor:Concil(false)
	actor:SeeFront(false, true)
	actor:StartJump(80)
	actor:HideParts(6, true)

	return true
end

function Act_Ladder_Dash_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction();

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	local nextActionName = action:GetID()
	local vel = actor:GetVelocity() 	-- Current Velocity
	local z = vel:GetZ()
	local findLadder = actor:ClimbUpLadder()

	--actor:SeeFront(false)

	if findLadder == false then
		action:SetNextActionName("a_jump")
		return false
	end

	if z < -5 then
		action:SetNextActionName("a_ladder_idle")
		return false
	end

	return true
end
function Act_Ladder_Dash_OnCleanUp(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	if actor:IsMyActor() then
		UseCameraHeightUseCameraHeightAdjust(true)
	end
end


-- action 인자는 다음으로 전이될 Next Action이다.
function Act_Ladder_Dash_OnLeave(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	local curAction = actor:GetAction()
	local actionID = action:GetID()

	CheckNil(nil == curAction);
	CheckNil(curAction:IsNil());

	local	bUnhided = false;	-- 파츠 숨김을 해제하였는가?
	if actor:IsMyActor() == false then
		actor:HideParts(6,false);
		bUnhided = true;
	end
	
	if action:GetEnable() == true then
		if actionID == "a_ladder_idle" then
			if bUnhided==false then
				actor:HideParts(6, false)
			end
			return true
		elseif actionID == "a_jump" then
			if actor:IsMyActor() then
				UseCameraHeightAdjust(true)	-- 카메라 높이 조정을 켬
			end
			actor:HideParts(6, false)
			actor:FreeMove(false)
			actor:Concil(true)
			actor:SeeFront(true, true)
			action:SetSlot(1)
			if bUnhided==false then
				actor:HideParts(6, false)
			end
			return true
		end
	end
	return false 
end
