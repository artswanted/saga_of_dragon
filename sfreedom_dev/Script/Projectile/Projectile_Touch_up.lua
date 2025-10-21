function PROJECTILE_TOUCH_UP_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_TOUCH_UP_OnArrivedAtTarget(kProjectile)
	if Act_Mon_Touch_up_NextProjectile(kProjectile) then
		return true
	end
	
	SkillFunc_ProjectileHitOneTime(kProjectile)

	return	false;
end

function PROJECTILE_TOUCH_UP_OnTargetListModified(kProjectile)
	return	PROJECTILE_ARROW_OnTargetListModified(kProjectile);
end

function PROJECTILE_TOUCH_UP_OnFlyingStart(kProjectile)
	return PROJECTILE_Parabola_OnFlyingStart(kProjectile)
end