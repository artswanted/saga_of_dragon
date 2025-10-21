function PROJECTILE_PINPOINTSHOT_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_PINPOINTSHOT_OnArrivedAtTarget(kProjectile)
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		return false;
	end
	
	local i = 0;
	local	kTargetList = kProjectile:GetActionTargetList();
	
	local   bWeaponSoundPlayed = false;
	
	while i<kTargetList:size() do
		
		local	kActionTargetInfo = kProjectile:GetActionTargetInfo(i);
		local	kActionEffect = kActionTargetInfo:GetActionResult();
		
		if kActionEffect:IsMissed() == false then
		
			local	kTargetPilot =  g_pilotMan:FindPilot(kActionTargetInfo:GetTargetGUID());
			if kTargetPilot:IsNil() == false then

  				local	kTargetActor = kTargetPilot:GetActor()
  				local	kParentActor = kParentPilot:GetActor()

				local	kTargetGUID = kActionTargetInfo:GetTargetGUID();
				
				
                if bWeaponSoundPlayed == false then
                    bWeaponSoundPlayed = true;
                    -- 피격 소리 재생
					local actionName = GetSkillDef(kProjectile:GetParentActionNo()):GetActionName():GetStr()
					if kActionEffect:GetCritical() then
						actionName = "Critical_dmg"
					end
					kTargetActor:PlayWeaponSound(WST_HIT, kParentActor, actionName, 0, kActionTargetInfo);
                end					
                
				kTargetActor:AttachParticle(12, "char_root", "ef_Pinpointshot_02_char_root")

			--	kTargetActor:SetShakeInPeriod(5,200);
					
			end
			
		end
		
		i=i+1;
		
	end
	
	kTargetList:ApplyActionEffects();
	
	return	false;	-- returning false means Delete This projectile
end
