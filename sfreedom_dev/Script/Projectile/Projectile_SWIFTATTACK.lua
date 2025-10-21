function PROJECTILE_SWIFTATTACK_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_SWIFTATTACK_OnArrivedAtTarget(kProjectile)
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		return false;
	end
	
	local i = 0;
	local	kTargetList = kProjectile:GetActionTargetList();
	
	local	iTargetCount = kTargetList:size();
	if iTargetCount == 0 then
		local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

		if kParentPilot:IsNil() then
			return false;
		end

		local	kParentActor = kParentPilot:GetActor()
		local	kAction = kParentActor:GetAction();
		if kAction:GetParamInt(13) == 1 then
			kAction:SetParamInt(10, 2);
			return	false;
		end
	end
	
	local	kActionTargetInfo = kTargetList:GetTargetInfo(0);
	if kActionTargetInfo:IsNil() then
		return	false;
	end
	
	local	kActionEffect = kActionTargetInfo:GetActionResult();
	local	kTargetPilot = nil;
	if kActionEffect:IsMissed() == false then

		kTargetPilot =  g_pilotMan:FindPilot(kActionTargetInfo:GetTargetGUID());
		if kTargetPilot:IsNil() == false then

			local	kTargetActor = kTargetPilot:GetActor()
			local	kParentActor = kParentPilot:GetActor()

			local	kSoundID = kProjectile:GetParamValue("DMG_SOUND");
			if kSoundID~=nil and kSound~="" then
				kTargetActor:AttachSound(7285,kSoundID);
			end

			local	kTargetGUID = kActionTargetInfo:GetTargetGUID();
			
			local	kSkillDef	= GetSkillDef(kProjectile:GetParentActionNo());
			local actionName = kSkillDef:GetActionName():GetStr()
			if kActionEffect:GetCritical() then
				actionName = "Critical_dmg"
			end
			kTargetActor:PlayWeaponSound(WST_HIT, kParentActor, actionName, 0, kActionTargetInfo);
			kTargetActor:PlayWeaponSound(WST_HIT, kParentActor, kSkillDef:GetActionName():GetStr(), 0, kActionTargetInfo)
			
			local	iSphereIndex = kActionTargetInfo:GetABVIndex();
			local pt = kTargetActor:GetABVShapeWorldPos(iSphereIndex);		

			kTargetActor:AttachParticleToPointS(12, pt, "e_dmg_cri",0.5)

			if iTargetCount == 1 then -- 마지막 하나 남았을때 세팅
				
				local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

				if kParentPilot:IsNil() then
					return false;
				end

				local	kParentActor = kParentPilot:GetActor()
				local	kAction = kParentActor:GetAction();
				if kAction:GetParamInt(10) == 0 then
					kAction:SetParamInt(10, 1);
					kAction:SetParamAsPoint(0, pt);
					kAction:SetParamInt(11, kParentActor:IsToLeft());
				end
			end
	
			local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

			if kParentPilot:IsNil() then
				return false;
			end

			local	kParentActor = kParentPilot:GetActor()
			if kParentActor:IsMyActor() then
				QuakeCamera(0.1,3,0.05,10);
			end
		end
	end

	kTargetList:ApplyActionEffectsTarget(kActionTargetInfo:GetTargetGUID());
	kTargetList:DeleteTargetInfo(0);
	
	iTargetCount = kTargetList:size();
	if iTargetCount == 0 then
		local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
		if kParentPilot:IsNil() then
			return false;
		end

		local	kParentActor = kParentPilot:GetActor()
		local	kAction = kParentActor:GetAction();
		if kAction:GetParamInt(13) == 0 then
			kProjectile:SetTargetLoc(kParentActor:GetPos());
			kProjectile:Fire();
			kAction:SetParamInt(13, 1);
		end
	else
		--	다음 타겟에개로 다시 발사하자.
		kActionTargetInfo = kTargetList:GetTargetInfo(0);
		kTargetPilot =  g_pilotMan:FindPilot(kActionTargetInfo:GetTargetGUID());		
	end

		
	if kTargetPilot == nil then
		return false;
	end

	if kTargetPilot:IsNil() then
		return	 false;
	end
	
	kProjectile:SetTargetObjectList(kTargetList);
	kProjectile:Fire();

	return	true;	-- returning false means Delete This projectile
end
