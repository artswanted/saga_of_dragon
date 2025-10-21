function Effect_Riding_Begin(actor, iEffectID, iEffectKey)
	local kMyPilot = actor:GetPilot()
	if kMyPilot:IsNil() then return true end
	local kMyUnit = kMyPilot:GetUnit()
	if kMyUnit:IsNil() then return true end
	local kCallerPilot = g_pilotMan:FindPilot(kMyUnit:GetCaller())
	if kCallerPilot:IsNil() then return true end
	local kCallerActor = kCallerPilot:GetActor()
	if kCallerActor:IsNil() then return true end
	
	local bSuccess = kCallerActor:MountPet()
	if bSuccess then		
		--actor:AttachParticle(289, "p_pt_Ride", "eff_pet_ride_00"); --펫의 종류에 따라 붙히기 노드 이름이 다를수 있으므로
		kCallerActor:AttachParticle(289, "char_root", "eff_pet_ride_00"); --그냥 PC에 파티클을 붙힌다.
		
		local iPetBaseClassID = actor:GetAbil(AT_BASE_CLASS)
		if iPetBaseClassID == 601500 or iPetBaseClassID == 601600 or iPetBaseClassID == 601700 or iPetBaseClassID == 601800 then
			PlaySoundByID("pet_getonoff_halloween") --할로윈 펫인가?
		else
			PlaySoundByID("pet_getonoff")
		end
	end
	if kCallerActor:IsMyActor() then
		kMyPilot:SetAbil(AT_MOUNTED_PET_SKILL, bSuccess)
	end
	
	return
end

function Effect_Riding_End(actor, iEffectID, iEffectKey)
	local kMyPilot = actor:GetPilot()
	if kMyPilot:IsNil() then return true end
	local kMyUnit = kMyPilot:GetUnit()
	if kMyUnit:IsNil() then return true end
	local kCallerPilot = g_pilotMan:FindPilot(kMyUnit:GetCaller())
	if kCallerPilot:IsNil() then return true end
	local kCallerActor = kCallerPilot:GetActor()
	if kCallerActor:IsNil() then return true end

	local bSuccess = kCallerActor:UnmountPet()
	if bSuccess then
		--actor:AttachParticle(289, "p_pt_Ride", "eff_pet_ride_00");
		kCallerActor:AttachParticle(289, "char_root", "eff_pet_ride_00");
		
		local iPetBaseClassID = actor:GetAbil(AT_BASE_CLASS)
		if iPetBaseClassID == 601500 or iPetBaseClassID == 601600 or iPetBaseClassID == 601700 or iPetBaseClassID == 601800 then
			PlaySoundByID("pet_getonoff_halloween") --할로윈 펫인가?
		else
			PlaySoundByID("pet_getonoff")
		end
	end
	
	if kCallerActor:IsMyActor() then
		kMyPilot:SetAbil(AT_MOUNTED_PET_SKILL, not bSuccess)
	end

	return
end

function Effect_Riding_OnTick(actor, iEffectID, fAccumTime)
end
