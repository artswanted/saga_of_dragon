function Trap_StoneHand_OnEnter(trigger, actor)
	actor:AttachParticle(938, "char_root", "e_dmg_msg_04")
	actor:ReserveTransitAction("a_knockdown")
	return true
end

function Trap_StoneHand_OnUpdate(trigger, actor)
end

function Trap_StoneHand_OnLeave(trigger, actor)
end

function Trap_Jump_OnEnter(trigger, actor)
	actor:ReserveTransitAction("a_idle")
	actor:StartJump(1050)
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
