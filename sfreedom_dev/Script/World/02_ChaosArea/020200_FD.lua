function Trap_Jump_OnEnter(trigger, actor)
	actor:ReserveTransitAction("a_idle")
	actor:StartJump(700)
	return true
end

function Trap_Jump_OnUpdate(trigger, actor)
	action = actor:GetAction()
	if action:IsNil() == false then
		action:SetParam(119, "jump_trap")
	end
	return true
end

function Trap_Jump_OnLeave(trigger, actor)
end

function DoTeleJump1_OnEnter(trigger, actor)
	DoTeleJump(actor, "telejump_zone_01", 100)
end

function DoTeleJump1_OnUpdate(actor, trigger)
end

function DoTeleJump1_OnLeave(actor, target)
end

function DoTeleJump2_OnEnter(trigger, actor)
	DoTeleJump(actor, "telejump_zone_02", 100)
end

function DoTeleJump2_OnUpdate(actor, trigger)
end

function DoTeleJump2_OnLeave(actor, target)
end

function DoTeleJump3_OnEnter(trigger, actor)
	DoTeleJump(actor, "telejump_zone_03", 100)
end

function DoTeleJump3_OnUpdate(actor, trigger)
end

function DoTeleJump3_OnLeave(actor, target)
end

function DoTeleJump4_OnEnter(trigger, actor)
	DoTeleJump(actor, "telejump_zone_04", 100)
end

function DoTeleJump4_OnUpdate(actor, trigger)
end

function DoTeleJump4_OnLeave(actor, target)
end
