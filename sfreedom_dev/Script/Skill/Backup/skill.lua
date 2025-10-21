-- skill

function Act_Skill_OnEnter(actor, action)
	prevAction = actor:GetAction()
	if prevAction:GetID() ~= "a_jump" then
		actor:Stop()
	end
	return true
end

function Act_Skill_OnUpdate(actor, accumTime, frameTime)
	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end

function Act_Skill_OnLeave(actor, action)
	actionID = action:GetID()

	if actionID == "a_dmg" or
		actionID == "a_die" or
		actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true
	elseif actor:IsAnimationDone() == true then
		if actionID == "a_run_right" or
			actionID == "a_run_left" then
		end
		return true
	end
	return false 
end

