-- Test_Clamp

function Act_Test_Clamp_OnEnter(actor, action)
	return true
end

function Act_Test_Clamp_OnUpdate(actor, accumTime, frameTime)
	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end
function Act_Test_Clamp_OnCleanUp(actor, action)
	return true
end
function Act_Test_Clamp_OnLeave(actor, action)
	return true
end

-- Test_Loop

function Act_Test_Loop_OnEnter(actor, action)
	return true
end

function Act_Test_Loop_OnUpdate(actor, accumTime, frameTime)
	return true
end
function Act_Test_Loop_OnCleanUp(actor, action)
	return true
end
function Act_Test_Loop_OnLeave(actor, action)
	return true
end

