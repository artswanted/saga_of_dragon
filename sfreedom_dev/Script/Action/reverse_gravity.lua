-- reverse_gravity

function Act_ReverseGravity_OnEnter(actor, action)
	actor:Stop()
	return true
end

function Act_ReverseGravity_OnUpdate(actor, accumTime, frameTime)
	return true
end

function Act_ReverseGravity_OnCleanUp(actor, action)
end

function Act_ReverseGravity_OnLeave(actor, action)
	-- 이러는게 좋은지는 모르겠다. Skill/ReverseGravity.lua의 OnLeave부분을 볼 것
	if action:GetID() == "a_run_left" or action:GetID() == "a_run_right" or
		action:GetID() == "a_idle" then
		return false
	end
	return true
end
