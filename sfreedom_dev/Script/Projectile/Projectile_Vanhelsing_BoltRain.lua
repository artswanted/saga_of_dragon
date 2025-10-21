function PROJECTILE_VANHELSING_BOLTRAIN_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_VANHELSING_BOLTRAIN_OnArrivedAtTarget(kProjectile)

	local	kActor = GetMyActor()
	if kActor:IsNil() == false then
	
		local	kTargetPos = kProjectile:GetWorldPos();
		kTargetPos:SetZ(kTargetPos:GetZ())
		
		kActor:AttachParticleToPoint(723+math.random(0,50),kTargetPos,"ef_vanhelsing_skill_02_02")

		kActor:AttachSound(2848,"Vanhelsing_Attk03");
	
	end
	return	false;	-- returning false means Delete This projectile
end
function PROJECTILE_VANHELSING_BOLTRAIN_OnTargetListModified(kProjectile)


	return	PROJECTILE_ARROW_OnTargetListModified(kProjectile);
end
