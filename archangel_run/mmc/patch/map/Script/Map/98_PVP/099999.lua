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
	DoTeleJump(actor, "telejump_zone_01", 460)
end

function DoTeleJump1_OnUpdate(trigger, actor)
end

function DoTeleJump1_OnLeave(trigger, actor)
end

function DoTeleJump2_OnEnter(trigger, actor)
	DoTeleJump(actor, "telejump_zone_02", 460)
end

function DoTeleJump2_OnUpdate(trigger, actor)
end

function DoTeleJump2_OnLeave(trigger, actor)
end

function DoTeleJump3_OnEnter(trigger, actor)
	DoTeleJump(actor, "telejump_zone_03", 460)
end

function DoTeleJump3_OnUpdate(trigger, actor)
end

function DoTeleJump3_OnLeave(trigger, actor)
end

function DoTeleJump4_OnEnter(trigger, actor)
	DoTeleJump(actor, "telejump_zone_04", 460)
end

function DoTeleJump4_OnUpdate(trigger, actor)
end

function DoTeleJump4_OnLeave(trigger, actor)
end

--[KTH 모드 언덕 트리거]////////////////////////
function DoKingHillUp01_OnEnter(trigger, actor)
	local kPacket = NewPacket(30048)
	kPacket:PushByte(0)
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function DoKingHillUp01_OnLeave(trigger, actor)
	local kPacket = NewPacket(30049)
	kPacket:PushByte(0)
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function DoKingHillUp01_OnUpdate(trigger, actor)
end

function DoKingHillUp02_OnEnter(trigger, actor)
	local kPacket = NewPacket(30048)
	kPacket:PushByte(1)
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function DoKingHillUp02_OnLeave(trigger, actor)
	local kPacket = NewPacket(30049)
	kPacket:PushByte(1)
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function DoKingHillUp02_OnUpdate(trigger, actor)
end
