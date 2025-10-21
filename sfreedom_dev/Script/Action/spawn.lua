-- spawn

function Act_Spawn_OnEnter(actor, action)
	return true
end

function Act_Spawn_OnUpdate(actor, accumTime, frameTime)
	actor:SeeFront(1)
	actorID = actor:GetID()
	action = actor:GetAction()
	animDone = actor:IsAnimationDone()
	currentSlot = action:GetCurrentSlot()

	if currentSlot == 0 and
		animDone == true then
		actor:PlayNext()
	end
	return true
end
function Act_Spawn_OnCleanUp(actor, action)
end

function Act_Spawn_OnLeave(actor, action)
	return true
end
