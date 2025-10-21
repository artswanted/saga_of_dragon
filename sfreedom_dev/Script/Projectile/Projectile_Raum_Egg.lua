function PROJECTILE_RAUM_EGG_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_RAUM_EGG_OnArrivedAtTarget(kProjectile)
	local	kActor = GetMyActor()
	if kActor:IsNil() == false then
		local	kTargetPos = kProjectile:GetWorldPos()
		kTargetPos:SetZ(kTargetPos:GetZ()+50)
		kActor:AttachParticleToPoint(71002,kTargetPos,"ef_boom_01")
		kActor:AttachSound(2784+math.random(0,100),"named_fire",60)	--사운드 볼륨 조절
	end
	return	false;	-- returning false means Delete This projectile
end
function PROJECTILE_RAUM_EGG_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end
