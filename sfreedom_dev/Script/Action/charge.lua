-- Charge

function Act_Charge_OnEnter(actor, action)

	actor:SetMagicFireActionID("a_charge");

	return true
end

function Act_Charge_OnUpdate(actor, accumTime, frameTime)
	return true
end

function Act_Charge_OnCleanUp(actor, action)
	actor:SetMagicFireActionID("");
	return true
end

function Act_Charge_OnLeave(actor, action)
	return true
end

