
function Act_Nature_Blessing_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	
	return true
end

function Act_Nature_Blessing_OnEnter(actor, action)

	action:CreateActionTargetList(actor, true)

	return true
end

function Act_Nature_Blessing_OnUpdate(actor, accumTime, frameTime)
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

function Act_Nature_Blessing_OnCleanUp(actor, action)
end

function Act_Nature_Blessing_OnLeave(actor, action)
	return true
end
