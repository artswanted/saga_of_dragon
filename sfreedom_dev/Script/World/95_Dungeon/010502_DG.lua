function Trap_Jump_OnEnter(trigger, actor)
	if actor:IsMyActor() then
		UseCameraHeightAdjust(false)
	end
	actor:ResetLastFloorPos()
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
	if actor:IsMyActor() then
		UseCameraHeightAdjust(true)
	end
end
