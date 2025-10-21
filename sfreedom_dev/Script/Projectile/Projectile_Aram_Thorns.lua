function PROJECTILE_Aram_Thorns_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_Aram_Thorns_OnArrivedAtTarget(kProjectile)

	local	kActor = GetMyActor()
	if nil ~= kActor and kActor:IsNil() == false then
		kActor:AttachSound(2783,"Aram_Skill02");
	end
	return	false;	-- returning false means Delete This projectile
end
function PROJECTILE_Aram_Thorns_OnTargetListModified(kProjectile)


	return	PROJECTILE_ARROW_OnTargetListModified(kProjectile);
end