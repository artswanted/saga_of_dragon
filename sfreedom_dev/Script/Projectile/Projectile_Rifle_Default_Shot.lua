function PROJECTILE_RIFLE_DEFAULT_SHOT_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_RIFLE_DEFAULT_SHOT_OnArrivedAtTarget(kProjectile)
	return	false;	-- returning false means Delete This projectile
end

function PROJECTILE_RIFLE_DEFAULT_SHOT_OnTargetListModified(kProjectile)
	SkillFunc_ProjectileHitOneTime(kProjectile)	
	return	false;

end
