function Act_Pet_AIOnOff_OnCheckCanEnter(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	return true
end

function Act_Pet_AIOnOff_OnEnter(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	actor:SetAttackState(1)

	return true
end

function Act_Pet_AIOnOff_OnCastingCompleted(actor, action)
end

function Act_Pet_AIOnOff_OnFindTarget(actor,action,kTargets)
end

function Act_Pet_AIOnOff_OnUpdate(actor, accumTime, frameTime)
	return false
end

function Act_Pet_AIOnOff_OnCleanUp(actor)
	return true
end

function Act_Pet_AIOnOff_OnLeave(actor, action)
	return true
end

function Act_Pet_AIOnOff_OnEvent(actor,textKey)
	return true
end
