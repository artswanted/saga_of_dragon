-- Action Key

function Act_Unlock_Move_OnEnter(actor, action)
	ODS("Act_Unlock_Move_OnEnter \n")
	if actor:IsNil() == false and action:IsNil() == false then
		if actor:IsRidingPet() then
			action:SetSlot(1)
			actor:PlayCurrentSlot()
		end
	end
	return true
end

function Act_Unlock_Move_OnUpdate(actor, accumTime, frameTime)
	ODS("Act_Unlock_Move_OnUpdate \n")
	return false
end
function Act_Unlock_Move_OnCleanUp(actor, action)
	return true
end

function Act_Unlock_Move_OnLeave(actor, action)
	ODS("Act_Unlock_Move_OnLeave \n")
	return true
end
