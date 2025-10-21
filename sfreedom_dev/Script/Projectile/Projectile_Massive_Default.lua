function PROJECTILE_MASSIVE_DEFAULT_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_MASSIVE_DEFAULT_OnArrivedAtTarget(kProjectile)

	local	kActor = GetMyActor()
	if kActor:IsNil() == false then
	
		local	kTargetPos = kProjectile:GetTargetLoc()
		kTargetPos:SetZ(kTargetPos:GetZ()+50)

		local kEffect = kProjectile:GetParamValue("ARRIVED_EFFECT")
		if nil~=kEffect and ""~=kEffect then
			local kScale = kProjectile:GetParamValue("ARRIVED_EFFECT_SCALE")
			if nil~=kScale and ""~=kScale then
				kScale = tonumber(kScale)
			else
				kScale = 1
			end
			kActor:AttachParticleToPointS(710+math.random(1, 5000),kTargetPos,kEffect,kScale)
		end

		local kSound = kProjectile:GetParamValue("ARRIVED_SOUND")
		if nil~=kSound and ""~=kSound then
			kActor:AttachSound(810+math.random(1, 50), kSound)
		end

		local kFactor = kProjectile:GetParamValue("QUAKE_FACTOR")
		if nil~=kFactor and ""~=kFactor then
			kFactor = tonumber(kFactor)
			QuakeCamera(0.1,kFactor)
		end

	end
	return	false	-- returning false means Delete This projectile
end
function PROJECTILE_MASSIVE_DEFAULT_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end
