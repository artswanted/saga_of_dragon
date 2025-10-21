-- transform

function Act_Transform_7Kick_OnEnter(actor, action)
	actor:Stop()
	return true
end

function Act_Transform_7Kick_OnUpdate(actor, accumTime, frameTime)


	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end

function Act_Transform_7Kick_OnLeave(actor, action)
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end

	if actor:IsMyActor() == false then
		g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
		return	true;
	end

	if actor:IsAnimationDone() == false then
		return false
	end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
	return false 
end

