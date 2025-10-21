function PROJECTILE_BIG_SIEGE_SHOT_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_BIG_SIEGE_SHOT_OnArrivedAtTarget(kProjectile)
	local	kActor = GetMyActor()
	if kActor:IsNil() == false then
	
		local	kTargetPos = kProjectile:GetTargetLoc()
--		ODS("PROJECTILE_ARAM_STONE_DOWN_OnArrivedAtTarget1 Pos Z : " ..kTargetPos:GetZ().."\n", false, 1509)
		kTargetPos:SetZ(kTargetPos:GetZ()+50)
		
		kActor:AttachParticleToPoint(710+math.random(1, 5000),kTargetPos,"ef_Siegedragon_Big_skill_05_char_root")
		QuakeCamera(0.1,1)
		kActor:AttachSound(810+math.random(1, 50), "Mon_SiegeDragon_Skill05_Attk-01")
	end
	return	false	-- returning false means Delete This projectile
end
function PROJECTILE_BIG_SIEGE_SHOT_OnTargetListModified(kProjectile)
	return	PROJECTILE_ARROW_OnTargetListModified(kProjectile)
end
