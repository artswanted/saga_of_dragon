function PROJECTILE_CHAIN_ATTACK_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_CHAIN_ATTACK_OnArrivedAtTarget(kProjectile)
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		return false;
	end
	
	local i = 0;
	local	kTargetList = kProjectile:GetActionTargetList();
	
	local	iTargetCount = kTargetList:size();
	if iTargetCount == 0 then
		return	false;
	end
	
	local	kActionTargetInfo = kTargetList:GetTargetInfo(0);
	if kActionTargetInfo:IsNil() then
		return	false;
	end
	
	local	kActionEffect = kActionTargetInfo:GetActionResult();
	if kActionEffect:IsMissed() == false then

		local	kTargetPilot =  g_pilotMan:FindPilot(kActionTargetInfo:GetTargetGUID());
		if kTargetPilot:IsNil() == false then

			local	kTargetActor = kTargetPilot:GetActor()
			local	kParentActor = kParentPilot:GetActor()

			local	kSoundID = kProjectile:GetParamValue("DMG_SOUND");
			if kSoundID~=nil and kSound~="" then
				kTargetActor:AttachSound(7285,kSoundID);
			end

			local	kTargetGUID = kActionTargetInfo:GetTargetGUID();
			
			local	kSkillDef	= GetSkillDef(kProjectile:GetParentActionNo());
			local actionName = kSkillDef:GetActionName():GetStr();
			if kActionEffect:GetCritical() then
				actionName = "Critical_dmg"
			end
			kTargetActor:PlayWeaponSound(WST_HIT, kParentActor, actionName, 0, kActionTargetInfo);
			
			local	iSphereIndex = kActionTargetInfo:GetABVIndex();
			local pt = kTargetActor:GetABVShapeWorldPos(iSphereIndex);		

			if kActionEffect:GetCritical() then
				kTargetActor:AttachParticleToPoint(12, pt, "e_dmg_cri")
			else
				kTargetActor:AttachParticleToPoint(12, pt, "e_dmg")
			end	

			--kTargetActor:SetShakeInPeriod(5,200);
		
		end
	
	end

	kTargetList:ApplyActionEffectsTarget(kActionTargetInfo:GetTargetGUID());
	kTargetList:DeleteTargetInfo(0);
	
	iTargetCount = kTargetList:size();
	if iTargetCount == 0 then
		return	false;
	end	
	
	--	다음 타겟에개로 다시 발사하자.
	kActionTargetInfo = kTargetList:GetTargetInfo(0);
	kTargetPilot =  g_pilotMan:FindPilot(kActionTargetInfo:GetTargetGUID());
	
	if kTargetPilot:IsNil() then
		return	 false;
	end
	
	kProjectile:SetTargetObjectList(kTargetList);
	kProjectile:Fire();

	return	true;	-- returning false means Delete This projectile
end
