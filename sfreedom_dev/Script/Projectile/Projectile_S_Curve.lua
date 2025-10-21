function PROJECTILE_S_CURVE_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_S_CURVE_OnArrivedAtTarget(kProjectile)
	if Act_Mon_Shot_S_Curve_SetProjectileDirection(kProjectile) then
		return true
	end
	
	PROJECTILE_ARROW_OnTargetListModified(kProjectile);

	return	false;
end

function PROJECTILE_S_CURVE_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end
