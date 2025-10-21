function PROJECTILE_SiegeDragon_skill_03_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_SiegeDragon_skill_03_OnArrivedAtTarget(kProjectile)

	local	kActor = GetMyActor()
	if kActor:IsNil() == false then
	
		local	kTargetPos = kProjectile:GetWorldPos();
		kTargetPos:SetZ(kTargetPos:GetZ()+50)
		
		--kActor:AttachParticleToPoint(71002,kTargetPos,"ef_Siegedragon_Big2_skill_03_02_char_root")
	
	end
	return	false;	-- returning false means Delete This projectile
end
function PROJECTILE_SiegeDragon_skill_03_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end
