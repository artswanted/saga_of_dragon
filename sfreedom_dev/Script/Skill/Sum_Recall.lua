-- spawn

function Skill_Recall_OnCheckCanEnter(actor, action)
	return true
end

function Skill_Recall_OnEnter(actor, action)
	return true
end

function Skill_Recall_OnUpdate(actor, accumTime, frameTime)
	actor:SeeFront(1)
	actorID = actor:GetID()
	action = actor:GetAction()
	animDone = actor:IsAnimationDone()
	currentSlot = action:GetCurrentSlot()

	if currentSlot == 0 and
		animDone == true then
		actor:PlayNext()
		return false;
	end
	return true
end
function Skill_Recall_OnCleanUp(actor, action)
end

function Skill_Recall_OnLeave(actor, action)
	return true
end