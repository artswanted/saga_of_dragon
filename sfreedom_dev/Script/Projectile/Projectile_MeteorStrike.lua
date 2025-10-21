function Projectile_MeteorStrike_OnFlyingStart(kProjectile)
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
	if kParentPilot:IsNil() then
		return false;
	end
	
--	PROJECTILE_ARROW_SHOWER_HitOneTime(kProjectile,kParentPilot:GetActor());
	local ptcl = kProjectile:GetTargetLoc();
	ptcl:SetZ(ptcl:GetZ()+5)
	kParentPilot:GetActor():AttachParticleToPointS(1509, ptcl, "ef_Meteorstrike_01_char_root", tonumber(kProjectile:GetParamValue("Scale")));	
	return	true;
end

function Projectile_MeteorStrike_OnCollision(kProjectile,kCollideActor,kTargets)
	kProjectile:SetHide(true);
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function Projectile_MeteorStrike_OnArrivedAtTarget(kProjectile)
	kProjectile:SetHide(true);
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		return false;
	end
	
	local	kParentActor = kParentPilot:GetActor()
	
	--kParentActor:AttachParticleToPointS(12,kProjectile:GetWorldPos(),"efx_meteor_strike", tonumber(kProjectile:GetParamValue("Scale")));
	kParentActor:AttachParticleToPointS(12,kProjectile:GetWorldPos(),"efx_meteor_strike", 0.35);
	kParentActor:AttachSound(7285,"Deapodong");
	
	local i = 0;
	local kTargetList = kProjectile:GetActionTargetList();
	local   bWeaponSoundPlayed = false;
	
	while i < kTargetList:size() do
		
		local kActionTargetInfo = kProjectile:GetActionTargetInfo(i);
		local kActionResult = kActionTargetInfo:GetActionResult();
		
		if kActionResult:IsMissed() == false then
		
			local	kTargetPilot =  g_pilotMan:FindPilot(kActionTargetInfo:GetTargetGUID());
			if kTargetPilot:IsNil() == false then

  				local	kTargetActor = kTargetPilot:GetActor()
  				local	kParentActor = kParentPilot:GetActor()

				local	kTargetGUID = kActionTargetInfo:GetTargetGUID();
				
                if bWeaponSoundPlayed == false then
                    bWeaponSoundPlayed = true;
                    -- 피격 소리 재생
					local actionName = GetSkillDef(kProjectile:GetParentActionNo()):GetActionName():GetStr()
					if kActionResult:GetCritical() then
						actionName = "Critical_dmg"
					end
					kTargetActor:PlayWeaponSound(WST_HIT, kParentActor, actionName, 0, kActionTargetInfo);
                end	
   			end
		
		end
		
		i=i+1;
		
	end

	kTargetList:ApplyActionEffects();
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	kProjectile:SetParamValue("ARRIVED","TRUE");
	kProjectile:SetParamValue("ARRIVED_TIME",""..g_world:GetAccumTime());
	
	if kParentActor:IsMyActor() then
		QuakeCamera(g_fMeleeDropQuakeValue[1]
		, g_fMeleeDropQuakeValue[2]
		, g_fMeleeDropQuakeValue[3]
		, g_fMeleeDropQuakeValue[4]
		, g_fMeleeDropQuakeValue[5])
	end	
	
	return	true;	-- returning false means Delete This projectile
end
function Projectile_MeteorStrike_OnUpdate(kProjectile)

	if kProjectile:GetParamValue("ARRIVED") == "TRUE" then
	
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local	fElapsedTime = g_world:GetAccumTime() - tonumber(kProjectile:GetParamValue("ARRIVED_TIME"));
		local	fTotalTime = 2;
		local	fAlphaFadingTime = 0.5;
		
		
		if fElapsedTime>fTotalTime then
		
			return	false;
		
		end
		
		if fElapsedTime>(fTotalTime-fAlphaFadingTime) then
			kProjectile:SetHide(true);
--			local	fAlpha = 1 - ((fElapsedTime-(fTotalTime-fAlphaFadingTime))/fAlphaFadingTime);
	--		kProjectile:SetAlpha(fAlpha);
			
		end
	end

	return	true;

end

function Projectile_MeteorStrike_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end
