-- 필드 PVP 시스템 중앙의 깃발 액션

function Act_PVP_FLAG_OnEnter(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	CheckNil(nil == action);
	CheckNil(action:IsNil());
	

	action:SetSlot(0)
	actor:PlayCurrentSlot()
	action:SetNextActionName("a_pvp_flag_default")
	return true
end

function Act_PVP_FLAG_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	 actor:SeeFront(true, true)
	if actor:IsAnimationDone() == true then --오프닝이 끝났다면 아이들로 넘어가자
		if action:GetCurrentSlot() == 0 then
			local pt = actor:GetTranslate()
			actor:AttachParticleToPoint(1, pt, "eff_battle_system_01")
		end
		action:SetSlot(1)
		actor:PlayCurrentSlot()
	end
	
	return true
end

function Act_PVP_FLAG_OnLeave(actor, action)
	return false
end

function Act_PVP_FLAG_OnCleanUp(actor, action)
	actor:DetachFrom(0)
end
