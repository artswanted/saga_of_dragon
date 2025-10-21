function Act_Hide_OnCheckCanEnter(actor,action)
	return	true;
end

function Act_Hide_OnEnter(actor, action)
	return true
end

function Act_Hide_OnUpdate(actor, accumTime, frameTime)

	if actor:IsAnimationDone() then
	
		return false;
	
	end

	return true
end
function Act_Hide_OnCleanUp(actor, action)
end

function Act_Hide_OnLeave(actor, action)
	return true
end
