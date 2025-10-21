function Act_LAVALON_HEART_Die_OnEnter(actor, action)
	ODS("Act_LAVALON_HEART_Die_OnEnter\n")
	if false == actor:IsNil() then
		actor:SeeFront(true, true)
	end
	return true
end

function Act_LAVALON_HEART_Die_OnUpdate(actor, accumTime, frameTime)
	return true
end
function Act_LAVALON_HEART_Die_OnCleanUp(actor, action)
	return true
end
function Act_LAVALON_HEART_Die_OnLeave(actor, action)
	return true
end

function Act_LAVALON_HEART_Die_OnEvent(actor, textKey, seqID)
	return true
end
