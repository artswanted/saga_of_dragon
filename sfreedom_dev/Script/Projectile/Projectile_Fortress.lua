function PROJECTILE_FORTRESS_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_FORTRESS_OnArrivedAtTarget(kProjectile)
	return	false;	-- returning false means Delete This projectile
end
function PROJECTILE_FORTRESS_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end