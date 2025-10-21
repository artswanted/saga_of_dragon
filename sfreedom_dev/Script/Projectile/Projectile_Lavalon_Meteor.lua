function Projectile_Lavalon_Meteor_OnCollision(kProjectile,kCollideActor,kTargets)
	if kProjectile:GetParamValue("ARRIVED") ~= "TRUE" then
		return 0
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local	fElapsedTime = g_world:GetAccumTime() - tonumber(kProjectile:GetParamValue("ARRIVED_TIME"));
	if 0.5 < fElapsedTime then
		return 0
	end

	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function Projectile_Lavalon_Meteor_OnArrivedAtTarget(kProjectile)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	kProjectile:SetParamValue("ARRIVED","TRUE");
	kProjectile:SetParamValue("ARRIVED_TIME",""..g_world:GetAccumTime());

	local kMyActor = GetPlayer()
	if kMyActor:IsNil() then
		return false
	end

	local kEffectID = "e_ef_fossilearth_explosion"
	if tonumber(kProjectile:GetParamValue("EXTENSION")) == 1 then
		kEffectID = "e_ef_fossilearth_explosion_blue"
	end

	local 	kProPos = kProjectile:GetWorldPos()

	kMyActor:AttachParticleToPoint(10000 + math.random(100), kProPos, kEffectID);
	kMyActor:AttachSound(10000 + math.random(100) * math.random(100), "boss_meteor");
	QuakeCamera(0.3, 1, 1, 1, 1)
	
	return	true;	-- returning false means Delete This projectile
end

function Projectile_Lavalon_Meteor_OnUpdate(kProjectile)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if kProjectile:GetParamValue("ARRIVED") == "TRUE" then
		local	fElapsedTime = g_world:GetAccumTime() - tonumber(kProjectile:GetParamValue("ARRIVED_TIME"));
		local	fTotalTime = 1;
		local	fAlphaFadingTime = 0.5;
		
		if fElapsedTime > fTotalTime then
			return	false;
		end
		
		if fElapsedTime>(fTotalTime-fAlphaFadingTime) then
			local fAlpha = 1 - ((fElapsedTime-(fTotalTime-fAlphaFadingTime))/fAlphaFadingTime);
			kProjectile:SetAlpha(fAlpha);
		end
	end

	return	true
end

function Projectile_Lavalon_Meteor_OnTargetListModified(kProjectile)
	if kProjectile:GetParamValue("ARRIVED") ~= "TRUE" then
		return true
	end
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end
