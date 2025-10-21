function PROJECTILE_MUTISHA_SHOT_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_MUTISHA_SHOT_OnArrivedAtTarget(kProjectile)

	local	kActor = GetMyActor()
	if kActor:IsNil() == false then
	
		local	kTargetPos = kProjectile:GetWorldPos();
		kTargetPos:SetZ(kTargetPos:GetZ()+50)
		
		kActor:AttachParticleToPoint(71002+math.random(0,50),kTargetPos,"ef_Mutisha_skill_01_03_char_root")

		kActor:AttachSound(2784,"Mutisha_Skill02");
	
	end
	return	false;	-- returning false means Delete This projectile
end
function PROJECTILE_MUTISHA_SHOT_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end
