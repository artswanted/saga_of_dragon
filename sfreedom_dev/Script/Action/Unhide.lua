function Act_Unhide_OnCheckCanEnter(actor,action)
	return	true;
end

function Act_Unhide_OnEnter(actor, action)
	return true
end

function Act_Unhide_OnUpdate(actor, accumTime, frameTime)

	if actor:IsAnimationDone() then
	
		return false;
	
	end

	return true
end
function Act_Unhide_OnCleanUp(actor, action)
	return true
end

function Act_Unhide_OnLeave(actor, action)
	return true
end
