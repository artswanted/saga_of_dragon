-- Intro_Idle
function Act_Intro_Idle_OnEnter(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil == g_selectStage);

	local ActorID = actor:GetPilotGuid()
	local selectedSlot = g_selectStage:GetSpawnSlot(ActorID)
	action:SetSlot(selectedSlot-1)
		
	actor:SetParam("REFRASH_PARTICLE", "TRUE")
	actor:HideParts(EQUIP_POS_WEAPON, true)
	return true
end
function Act_Intro_Idle_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	if actor:GetParam("REFRASH_PARTICLE") == "TRUE" then
		iSeqID = actor:GetAniSequenceID()
		local iBaseClass = actor:GetAbil(AT_BASE_CLASS)
		local bHuman = true
		if 4 < iBaseClass then
			bHuman = false
		end
		if iSeqID == 1300011 then
			if nil~=g_BullDogActor and false == g_BullDogActor:IsNil() then
				--g_BullDogActor:SetHide(false)
				local kAction = g_BullDogActor:ReserveTransitAction("a_BullDog_Idle")
				kAction:SetSlot(0)
				g_BullDogActor:PlayCurrentSlot()
				actor:SetParam("REFRASH_PARTICLE", "FALSE")
			end
		elseif iSeqID == 1300012 then
			if nil~=g_BullDogActor and false == g_BullDogActor:IsNil() then
				--g_BullDogActor:SetHide(false)
				local kAction = g_BullDogActor:ReserveTransitAction("a_BullDog_Idle")
				kAction:SetSlot(1)
				g_BullDogActor:PlayCurrentSlot()
				actor:SetParam("REFRASH_PARTICLE", "FALSE")
			end 
		elseif iSeqID == 1300031 then
			actor:DetachFrom(3851, true)
			if true == bHuman then
				actor:AttachParticleS(3851, "p_wp_r_hand", "eff_char_select03_01", 1.0)
			else
				actor:AttachParticleS(3851, "p_pt_r_hand", "eff_char_select03_01", 1.0)
			end
			actor:SetParam("REFRASH_PARTICLE", "FALSE")
		elseif iSeqID == 1300032 then
			actor:DetachFrom(3851, true)
			if true == bHuman then
				actor:AttachParticleS(3851, "p_wp_r_hand", "eff_char_select03_02", 1.0)
			else
				actor:AttachParticleS(3851, "p_pt_r_hand", "eff_char_select03_02", 1.0)
			end
			actor:SetParam("REFRASH_PARTICLE", "FALSE")
		end
	end
	
	if actor:IsAnimationDone() == true then
		actor:SetParam("REFRASH_PARTICLE", "TRUE")
		return false
	end
	return true
end
function Act_Intro_Idle_OnCleanUp(actor, action)
	actor:HideParts(EQUIP_POS_WEAPON, false)
	return true
end
function Act_Intro_Idle_OnLeave(actor, action)
	return false
end

-- Intro_Up
function Act_Intro_Up_OnEnter(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil == g_selectStage);

	local ActorID = actor:GetPilotGuid()
	local selectedSlot = g_selectStage:GetSpawnSlot(ActorID)
	action:SetSlot(selectedSlot-1)
	actor:HideParts(EQUIP_POS_WEAPON, true)
	actor:DetachFrom(3851, true)	--intro_idle 이펙트 제거
	actor:AttachParticleS(49023852, "char_root", "eff_char_select01",1.0)
	PlaySoundByID( "cha-select" )
	
	--ODS("Intro_Up SetSlot"..selectedSlot.."\n",false, 3851)
	if 0 == selectedSlot then
		if nil~=g_BullDogActor and false == g_BullDogActor:IsNil() then
			--g_BullDogActor:SetHide(true)
			local kAction = g_BullDogActor:ReserveTransitAction("a_BullDog_Select")
			kAction:SetSlot(2)
			g_BullDogActor:PlayCurrentSlot()
		end
	end
	return true
end
function Act_Intro_Up_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	if 	actor:IsAnimationDone() == true then
		return false
	end
	return true
end
function Act_Intro_Up_OnCleanUp(actor, action)
	actor:HideParts(EQUIP_POS_WEAPON, false)
	return true
end
function Act_Intro_Up_OnLeave(actor, action)
	return true
end

-- Intro_Select
function Act_Intro_Select_OnEnter(actor, action)
	CheckNil(nil == action);
	CheckNil(action:IsNil());

	local ActorID = actor:GetPilotGuid()
	local selectedSlot = g_selectStage:GetSpawnSlot(ActorID)
	local iSlot = action:GetParamInt(1)
	action:SetSlot(iSlot)
	if 1 == selectedSlot then
		if nil~=g_BullDogActor and false == g_BullDogActor:IsNil() then
			--g_BullDogActor:SetHide(true)
			local kAction = g_BullDogActor:ReserveTransitAction("a_BullDog_Select_Idle")
		end
	end
	return true
end
function Act_Intro_Select_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end
function Act_Intro_Select_OnCleanUp(actor, action)
	return true
end
function Act_Intro_Select_OnLeave(actor, action)
	return true
end

-- Intro_Select_Idle
function Act_Intro_Select_Idle_OnEnter(actor, action)
	CheckNil(nil == action);
	CheckNil(action:IsNil());

	local iSlot = action:GetParamInt(1)
	action:SetSlot(iSlot)
	return true
end
function Act_Intro_Select_Idle_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	if actor:IsAnimationDone() == true then
		return false
	end
	
	return true
end
function Act_Intro_Select_Idle_OnCleanUp(actor, action)
	return true
end
function Act_Intro_Select_Idle_OnLeave(actor, action)
	return true
end

-- Intro_Select_Back
function Act_Intro_Select_Back_OnEnter(actor, action)
	CheckNil(nil == action);
	CheckNil(action:IsNil());

	local iSlot = action:GetParamInt(1)
	action:SetSlot(iSlot)
	--ODS("Act_Intro_Select_Back_OnEnter\n", false, 3851)
	return true
end
function Act_Intro_Select_Back_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end
function Act_Intro_Select_Back_OnCleanUp(actor, action)
	return true
end
function Act_Intro_Select_Back_OnLeave(actor, action)
	return true
end

-- Intro_Down
function Act_Intro_Down_OnEnter(actor, action)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil == g_selectStage);
	
	local ActorID = actor:GetPilotGuid()
	local selectedSlot = g_selectStage:GetSpawnSlot(ActorID)
	action:SetSlot(selectedSlot-1)
	actor:HideParts(EQUIP_POS_WEAPON, true)
	--ODS("Intro_Down SetSlot"..selectedSlot.."\n",false, 3851)
	if 1 == selectedSlot then
		if nil~=g_BullDogActor and false == g_BullDogActor:IsNil() then
			--g_BullDogActor:SetHide(true)
			local kAction = g_BullDogActor:ReserveTransitAction("a_BullDog_Cancle")
			g_BullDogActor:PlayCurrentSlot()
		end
	end	
	return true
end
function Act_Intro_Down_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction();

	CheckNil(nil == action);
	CheckNil(action:IsNil());

	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end
function Act_Intro_Down_OnCleanUp(actor, action)
	actor:HideParts(EQUIP_POS_WEAPON, false)
	return true
end
function Act_Intro_Down_OnLeave(actor, action)
	local ActorID = actor:GetPilotGuid()
	local selectedSlot = g_selectStage:GetSpawnSlot(ActorID)
	action:SetSlot(selectedSlot-1)
	return true
end

-- Act_BullDog_Idle
function Act_BullDog_Idle_OnEnter(actor, action)
	CheckNil(nil == action);
	CheckNil(action:IsNil());
	--ODS("Act_BullDog_Idle_OnEnter\n", false, 3851)
	return true
end
function Act_BullDog_Idle_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil == actor);
	CheckNil(actor:IsNil());

	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end
function Act_BullDog_Idle_OnCleanUp(actor, action)
	return true
end
function Act_BullDog_Idle_OnLeave(actor, action)
	return false
end