function Trap_Jump_OnEnter(trigger, actor)
	actor:ReserveTransitAction("a_idle")
	actor:StartJump(trigger:GetParam())
	Net_PT_C_M_REQ_INDUN_TREE(1)
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

function Call_OpenDungeon(trigger, actor)
	ODS("Call_OpenDungeon " .. trigger:GetParam() .. "\n")
	Net_PT_C_M_REQ_INDUN_TREE(trigger:GetParam())
	return true
end