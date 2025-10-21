function PROJECTILE_GATEKEEPER_STONE_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_GATEKEEPER_STONE_OnArrivedAtTarget(kProjectile)

	local	kActor = GetMyActor()
	if kActor:IsNil() == false then
	
		local kArriveEffectZ = kProjectile:GetParamValue("ARRIVED_EFFECT_Z")
		if nil~=kArriveEffectZ and ""~=kArriveEffectZ then
			kArriveEffectZ = tonumber(kArriveEffectZ)
		else
			kArriveEffectZ = 50
		end

		local	kTargetPos = kProjectile:GetWorldPos();
		kTargetPos:SetZ(kTargetPos:GetZ()+kArriveEffectZ)
		
		local kArriveEffect = kProjectile:GetParamValue("ARRIVED_EFFECT")
		if nil==kArriveEffect or ""==kArriveEffect then
			kArriveEffect = "ef_gatekeeper_stone_04_char_root"
		end

		local fScale = kProjectile:GetParamValue("ARRIVED_EFFECT_SCALE")
		if nil~=fScale and ""~=fScale then
			fScale = tonumber(fScale)
		else
			fScale = 1
		end

		if "NONE"~=kArriveEffect then
			kActor:AttachParticleToPointS(71002+math.random(0,50), kTargetPos, kArriveEffect, fScale)
		end

		kActor:AttachSound(2784,"Gatekeeper_Attk04");
	
	end
	return	false;	-- returning false means Delete This projectile
end
function PROJECTILE_GATEKEEPER_STONE_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end
