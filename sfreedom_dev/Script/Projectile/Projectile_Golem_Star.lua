function PROJECTILE_GOLEM_STAR_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_GOLEM_STAR_OnArrivedAtTarget(kProjectile)

	local	kActor = GetMyActor()
	if kActor:IsNil() == false then
	
		local	kTargetPos = kProjectile:GetWorldPos();
		kTargetPos:SetZ(kTargetPos:GetZ()+50)
		
		kActor:AttachParticleToPoint(71002,kTargetPos,"ef_Golem_skill_02_03")
	
	end
	return	false;	-- returning false means Delete This projectile
end
function PROJECTILE_GOLEM_STAR_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end
