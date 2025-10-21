-- Up 

function Act_Up_OnEnter(actor, action)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	if g_world:ToLadder(actor) == true then
		actor:ReserveTransitAction("a_ladder_up")
		return false
	elseif actor:HangOnRope() == true then
		actor:ReserveTransitAction("a_rope")
		return false
	end

	local prevAction = actor:GetAction()
	if prevAction:IsNil() == false then
		prevActionID = prevAction:GetID()
		if prevActionID == "a_jump" then
			return false
		elseif prevActionID == "a_ladder_down" or
			prevActionID == "a_ladder_idle" then
			actor:ReserveTransitAction("a_ladder_up")
			return false
		elseif prevActionID == "a_run_left" or
			prevActionID == "a_run_right" then
			return false
		end
	end

	return false
--	actor:SeeFront(false)
--	return true
end

function Act_Up_OnUpdate(actor, accumTime, frameTime)
	return false
end

function Act_Up_OnCleanUp(actor, action)
	return true
end

function Act_Up_OnLeave(actor, action)
	return true
end
