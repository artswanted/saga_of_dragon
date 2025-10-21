function DoHighJump_OnEnter(trigger, actor)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	if actor:GetAbil(AT_HP) > 0 then
		if actor:IsMyActor() and actor:IsNowFollowing()==false then
			local kAction
			if actor:IsRidingPet() then
				kAction = actor:ReserveTransitAction("a_rp_jump")
			else
				kAction = actor:ReserveTransitAction("a_jump")
			end
			CheckNil(nil == kAction);
			CheckNil(kAction:IsNil());

			kAction:SetSlot(1);
			kAction:SetParam(4,trigger:GetParam());
			kAction:SetParam(5,"HiJump");
		end
	end
	return true
end
function DoHighJump_OnUpdate(trigger, actor)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	if action:IsNil() == false then
		action:SetParam(119, "jump_trap")
	end
	return true
end

function DoHighJump_OnCleanUp(trigger, actor)
end

function DoHighJump_OnLeave(trigger, actor)
end
