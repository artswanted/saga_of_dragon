-- Hold Trans Action
function Act_RePosition_OnEnter(actor, action)
	ODS("\n111111111111111111111111111111111\n")
	return true
end

function Act_RePosition_OnUpdate(actor, accumTime, frameTime)
	ODS("\n22222222222222222222222222\n")
	return false
end

function Act_RePosition_OnLeave(actor, action)
	ODS("\n아3333333333333333333333333333333\n")
	return true
end

function Act_RePosition_OnCleanUp(actor, action)

	ODS("\n444444444444444444444444444444444\n")
	return true
end
