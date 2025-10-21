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