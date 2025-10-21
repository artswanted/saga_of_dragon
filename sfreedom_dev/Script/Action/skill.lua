-- skill

function Act_Skill_OnEnter(actor, action)
	local prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	return true
end

function Act_Skill_OnUpdate(actor, accumTime, frameTime)
	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end
function Act_Skill_OnCleanUp(actor, action)
end

function Act_Skill_OnLeave(actor, action)
	actionID = action:GetID()
	if actionID == "a_dmg" and actionID == "a_idle" or actionID == "a_die" then
		return true
	end

	return false 
end
