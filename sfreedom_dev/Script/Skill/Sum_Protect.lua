
function Act_Protect_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	
	return true
end

function Act_Protect_OnEnter(actor, action)

	action:CreateActionTargetList(actor, true)

	return true
end

function Act_Protect_OnUpdate(actor, accumTime, frameTime)
	actor:SeeFront(1)
	actorID = actor:GetID()
	action = actor:GetAction()
	animDone = actor:IsAnimationDone()
	currentSlot = action:GetCurrentSlot()

	if currentSlot == 0 and
		animDone == true then
		actor:PlayNext()
	else
		return false
	end
	return true
end

function Act_Protect_OnCleanUp(actor, action)
end

function Act_Protect_OnLeave(actor, action)
	return true
end
