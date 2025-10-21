-- Idle

function Act_Pet_Opening_OnEnter(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	if actor:IsRidingPet() then
		return false
	end

	action:SetSlot(0)
	actor:PlayCurrentSlot()
	
	return true
end

function Act_Pet_Opening_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()
	CheckNil(nil == action);
	CheckNil(action:IsNil());
	
	if actor:IsAnimationDone() == true then	
		return false
	end
	
	return true
end
function Act_Pet_Opening_OnCleanUp(actor, action)
	return true
end

function Act_Pet_Opening_OnLeave(actor, action)
	return true
end

