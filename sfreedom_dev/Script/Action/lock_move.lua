-- Action Key

function Act_Lock_Move_OnEnter(actor, action)
	if actor:IsNil() == false and action:IsNil() == false then
		if actor:IsRidingPet() then
			action:SetSlot(1)
			actor:PlayCurrentSlot()
		end
	end
	return true
end

function Act_Lock_Move_OnUpdate(actor, accumTime, frameTime)
	return false
end
function Act_Lock_Move_OnCleanUp(actor, action)
end

function Act_Lock_Move_OnLeave(actor, action)
	if action:GetID() == "a_unlock_move" 
		or action:GetID() == "a_class_promotion_human" 
		or action:GetID() == "a_class_promotion_dragonian" 
	then
		return true
	end
	return false
end
