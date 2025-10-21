function PROJECTILE_ARAM_STONE_DOWN_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_ARAM_STONE_DOWN_OnArrivedAtTarget(kProjectile)
	local	kActor = GetMyActor()
	if kActor:IsNil() == false then
	
		local	kTargetPos = kProjectile:GetTargetLoc()
--		ODS("PROJECTILE_ARAM_STONE_DOWN_OnArrivedAtTarget1 Pos Z : " ..kTargetPos:GetZ().."\n", false, 1509)
		kTargetPos:SetZ(kTargetPos:GetZ()+50)
		
		local kArriveEffect = kProjectile:GetParamValue("ARRIVED_EFFECT")
		if nil==kArriveEffect or ""==kArriveEffect then
			kArriveEffect = "ef_Aram_skill_01_04_char_root"
		end

		if "NONE"~= kArriveEffect then
			kActor:AttachParticleToPoint(710+math.random(1, 5000),kTargetPos,kArriveEffect)
		end

		local bQuakeCamera = kProjectile:GetParamValue("IS_QUAKE_CAMERA")
		if nil==bQuakeCamera or "FALSE"~=bQuakeCamera then
			QuakeCamera(0.1,1)
		end
		kActor:AttachSound(810+math.random(1, 50), "HeapOfEarth_Break")
	end
	return	false	-- returning false means Delete This projectile
end
function PROJECTILE_ARAM_STONE_DOWN_OnTargetListModified(kProjectile)
	return	PROJECTILE_ARROW_OnTargetListModified(kProjectile)
end
