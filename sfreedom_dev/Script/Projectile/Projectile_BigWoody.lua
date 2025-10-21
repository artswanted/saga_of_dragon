function PROJECTILE_BIGWOODY_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets)
end

function PROJECTILE_BIGWOODY_OnArrivedAtTarget(kProjectile)

	local	kActor = GetMyActor()
	if kActor:IsNil() == false then
	
		local	kTargetPos = kProjectile:GetWorldPos()
		local effect = kProjectile:GetParamValue("EFFECT_TO_GROUND")
		if nil~=effect and "NONE"~=effect then
			kActor:AttachParticleToPoint(71002,kTargetPos,effect)
		end
		local qtime = kProjectile:GetParamValue("QUAKE_TIME")
		if nil~=qtime then
			local qfactor = kProjectile:GetParamValue("QUAKE_FACTOR")
			if nil==qfactor then
				qfactor = 2.1
			end
			QuakeCamera(tonumber(qtime),tonumber(qfactor),1)
		end

		local kSoundID = kProjectile:GetParamValue("SOUND_ID")
		if nil~=kSoundID and ""~=kSoundID then
			kActor:AttachSound(2785,kSoundID)
		end
	end
	return	false;	-- returning false means Delete This projectile
end
function PROJECTILE_BIGWOODY_OnTargetListModified(kProjectile)
	PROJECTILE_ARROW_OnTargetListModified(kProjectile)
	return	true
end

function PROJECTILE_BIGWOODY_OnUpdate(kProjectile)
	return true
end
