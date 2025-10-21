-- Idle

function Act_PetUIIdle_OnEnter(actor, action)

	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	CheckNil(action == nil);
	CheckNil(action:IsNil());
	
	action:SetDoNotBroadCast(true);	

	UpdateUIModelAbil(actor)

	actor:MakePetActionQueue(-1)
	action:SetParamInt(6,0);

	return true
end

function Act_PetUIIdle_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());

	UpdateUIModelAbil(actor)

	local param = action:GetParam(0)
	local iIdleType = action:GetParamInt(6);
	
	--Pos
	local iNowSlot = action:GetCurrentSlot()
	local iNextSlot = iNowSlot
	local bForceNextSlot = false
	
	-- Next

	local kNew = actor:UpdatePetActionQueue(accumTime)
	if kNew~=iIdleType then
		iNextSlot = kNew
		bForceNextSlot = true
		action:SetParamInt(6,iNextSlot)
	end

	if actor:IsAnimationDone() or bForceNextSlot then
		action:SetSlot(iNextSlot)
		actor:PlayCurrentSlot(false)
		ChangeBalloonState(actor, iNextSlot)
		
		actor:DetachFrom(3333,true)

		local kEffectID = actor:GetAnimationInfoFromAction("EFFECT_ID", action, 0);	
		local kAttachNode = actor:GetAnimationInfoFromAction("ATTACH_NODE", action, 0);	
		local kScale = actor:GetAnimationInfoFromAction("SCALE", action, 0);

		if nil==kEffectID or ""==kEffectID then
			return true;
		end

		if nil==kAttachNode or ""==kAttachNode then
			return true;
		end

		if nil==kScale or ""==kScale then
			kScale = 1.0
		else
			kScale = tonumber(kScale)
		end
		
		actor:AttachParticleS(3333, kAttachNode, kEffectID, kScale);
	end
	
	return true

end

function Act_PetUIIdle_OnCleanUp(actor, action)
	DelRenderModelNIF("PgMyActorViewPetActor", "emo")
	return true
end

function Act_PetUIIdle_OnLeave(actor, action)
	return true
end

function UpdateUIModelAbil(actor)
	local kPlayer = g_pilotMan:GetPlayerUnit()
	if kPlayer:IsNil() then return true end

	local kPetGuid = kPlayer:GetSelectedPet()
	if kPetGuid:IsNil() then return true end
	local kPetPilot = g_pilotMan:FindPilot(kPetGuid)
	if kPetPilot:IsNil() then return true end	--진짜 펫 찾자
	local kMyPilot = actor:GetPilot()
	if kMyPilot:IsNil() then return true end	--이놈의 UI상의 펫
--	local kOld = kMyPilot:GetConditionGrade()
--	kMyPilot:SetAbil(AT_HUNGER, kPetPilot:GetAbil(AT_HUNGER))
--	kMyPilot:SetAbil(AT_HEALTH, kPetPilot:GetAbil(AT_HEALTH))
--	kMyPilot:SetAbil(AT_MENTAL, kPetPilot:GetAbil(AT_MENTAL))
--	local kNew = kMyPilot:GetConditionGrade()
--	if kOld~=kNew then
--		actor:MakePetActionQueue(-1)
--	end
end

function ChangeBalloonState(actor, iNextSlot)
	if 3>iNextSlot then
		HideBalloonToIdleActionChange()
	else
		ShowBalloonToIdleActionChange(iNextSlot - 2)
	end
end