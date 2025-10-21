-- spawn

function Skill_Summon_OnCheckCanEnter(actor, action)
	return true
end

function Skill_Summon_OnEnter(actor, action)
	return true
end

function Skill_Summon_OnUpdate(actor, accumTime, frameTime)
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
function Skill_Summon_OnCleanUp(actor, action)
end

function Skill_Summon_OnLeave(actor, action)
	return true
end

function Skill_Summon_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local kAction = actor:GetAction();
	
	
	if textKey == "hit" then
	end
	
	return true
end