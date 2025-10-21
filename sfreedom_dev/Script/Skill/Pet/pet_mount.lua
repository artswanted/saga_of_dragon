function Act_Pet_Mount_OnCheckCanEnter(actor,action)
	local kMyPilot = actor:GetPilot()
	if kMyPilot:IsNil() then return false end
	local kMyUnit = kMyPilot:GetUnit()
	if kMyUnit:IsNil() then return false end
	local kCallerPilot = g_pilotMan:FindPilot(kMyUnit:GetCaller())
	if kCallerPilot:IsNil() then return false end
	local kCallerActor = kCallerPilot:GetActor()
	if kCallerActor:IsNil() then return false end		
	local kCallerAction = kCallerActor:GetAction()
	if kCallerAction:IsNil() then return false end
	
	local kCurPetAction = actor:GetAction()
	if kCurPetAction:IsNil() == false and kCurPetAction:GetID() == "a_pet_melee_02" then --타고 있을 때, 내리고 있을 때 이 액션을 통과할 수 있는지 필터링 (펫 액션)
		return false
	end
	--아이들/일반이동때만 승하차 가능
	if kCallerActor:IsRidingPet() then --타고 있을때 이 액션을 통과할 수 있는지 필터링
		if kCallerAction:GetID() ~= "a_idle" and
			kCallerAction:GetID() ~= "a_rp_idle" and
			kCallerAction:GetID() ~= "a_rp_walk" and
			kCallerAction:GetID() ~= "a_lock_move" and
			kCallerAction:GetID() ~= "a_unlock_move" and
			kCallerAction:GetID() ~= "a_dmg" and
			kCallerAction:GetID() ~= "a_down_damage" and
			kCallerAction:GetID() ~= "a_down_damage_2" and
			kCallerAction:GetID() ~= "a_die" and
			kCallerAction:GetID() ~= "a_trap" and
			kCallerAction:GetID() ~= "a_walk" and
			kCallerAction:GetID() ~= "a_walk_left" and
			kCallerAction:GetID() ~= "a_walk_right" and
			kCallerAction:GetID() ~= "a_walk_up" and
			kCallerAction:GetID() ~= "a_walk_down" and
			kCallerAction:GetID() ~= "a_run" and
			kCallerAction:GetID() ~= "a_run_left" and
			kCallerAction:GetID() ~= "a_run_right" and
			kCallerAction:GetID() ~= "a_run_up" and
			kCallerAction:GetID() ~= "a_run_down" and
			kCallerAction:GetID() ~= "a_battle_idle" and
			kCallerAction:GetID() ~= "a_knockdown" and
			kCallerAction:GetID() ~= "a_fall" and
			kCallerAction:GetID() ~= "a_lightning_default" and
			kCallerAction:GetID() ~= "a_lightning_low" and
			kCallerAction:GetID() ~= "a_lightning_float_dmg" and
			kCallerAction:GetID() ~= "a_lightning_throw_down" and
			kCallerAction:GetID() ~= "a_throw_down" and
			kCallerAction:GetID() ~= "a_throw_down2" and
			kCallerAction:GetID() ~= "a_stun" and
			kCallerAction:GetID() ~= "a_knock_back" and
			kCallerAction:GetID() ~= "a_float_dmg" and
			kCallerAction:GetID() ~= "a_hold_down" and
			kCallerAction:GetID() ~= "a_touch_dmg" and
			kCallerAction:GetID() ~= "a_creep_down" and
			kCallerAction:GetID() ~= "a_blow_up" and
			kCallerAction:GetID() ~= "a_blow_up_large" and
			kCallerAction:GetID() ~= "a_blow_up_medium" and
			kCallerAction:GetID() ~= "a_blow_up_small" and
			kCallerAction:GetID() ~= "a_blow_down" then
			return false
		end
	else --내리고 있을때 이 액션을 통과할 수 있는지 필터링
		--모든 변신 상태에서는 탑승 불가
		if kCallerActor:CheckStatusEffectExist("se_transform_to_metamorphosis") or
			kCallerActor:CheckStatusEffectExist("se_transform_to_mahalka_Fig") or
			kCallerActor:CheckStatusEffectExist("se_transform_to_mahalka_Mag") or
			kCallerActor:CheckStatusEffectExist("se_transform_to_mahalka_Arc") or
			kCallerActor:CheckStatusEffectExist("se_transform_to_mahalka_Thi") or
			kCallerActor:CheckStatusEffectExist("se_barricade") or
			--kCallerActor:CheckStatusEffectExist("se_TimeReverse_Caster") or --타임리버스와 오펜시브오라는 탑승 허용해주자
			--kCallerActor:CheckStatusEffectExist("se_OffecsiveAura_Caster") or
			kCallerActor:CheckStatusEffectExist("se_burrow") or
			kCallerActor:CheckStatusEffectExist("se_transform_to_fu_box") or 
			kCallerActor:CheckStatusEffectExist("se_twin_fusion")
			then
			g_ChatMgrClient:Notice_Show(GetTextW(451003), 1) --변신 상태에서는 탑승할 수 없습니다.
			return false
		end
		if kCallerActor:CheckStatusEffectTypeExist(2) or
			kCallerActor:CheckStatusEffectExist("se_ProtectEdge") then --디버프 상태라면 탑승 불가
			g_ChatMgrClient:Notice_Show(GetTextW(451006), 1) --탑승할 수 없는 상태입니다
			return false
		end
		
		if kCallerAction:GetID() ~= "a_idle" and
			kCallerAction:GetID() ~= "a_rp_idle" and
			kCallerAction:GetID() ~= "a_rp_walk" and
			kCallerAction:GetID() ~= "a_walk" and
			kCallerAction:GetID() ~= "a_walk_left" and
			kCallerAction:GetID() ~= "a_walk_right" and
			kCallerAction:GetID() ~= "a_walk_up" and
			kCallerAction:GetID() ~= "a_walk_down" and
			kCallerAction:GetID() ~= "a_run" and
			kCallerAction:GetID() ~= "a_run_left" and
			kCallerAction:GetID() ~= "a_run_right" and
			kCallerAction:GetID() ~= "a_run_up" and
			kCallerAction:GetID() ~= "a_run_down" and
			kCallerAction:GetID() ~= "a_battle_idle" and
			kCallerAction:GetID() ~= "a_step_the_beat" and
			kCallerAction:GetID() ~= "a_step_the_beat_idle" then
			return false
		end
	end
	
	if kCallerActor:IsMyActor() and actor:IsRidingPet() then --하차시
		local kWnd = GetUIWnd("SFRM_TRANSTOWER")
		if kWnd:IsNil() == false then
			local kWndByPet = kWnd:GetControl("FRM_CALLER_IS_PET") --펫스킬에 의해 순간이동 UI 창이 떠있다면
			if kWndByPet:IsNil() == false and kWndByPet:IsVisible() == true then
				kWnd:Close() --창을 지워준다
			end
		end
	end
	
	return true
end

function Act_Pet_Mount_OnEnter(actor, action)
	return true
end

function Act_Pet_Mount_OnCastingCompleted(actor, action)
end

function Act_Pet_Mount_OnFindTarget(actor,action,kTargets)
end

function Act_Pet_Mount_OnUpdate(actor, accumTime, frameTime)
	return false
end

function Act_Pet_Mount_OnCleanUp(actor)
	return true
end

function Act_Pet_Mount_OnLeave(actor, action)
	return true
end

function Act_Pet_Mount_OnEvent(actor,textKey)
	return true
end
