-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function PROJECTILE_Blizzard02_OnLoadingStart(kProjectile)

	return	true;
end
function PROJECTILE_Blizzard02_OnLoading(kProjectile)

	return	true;
end
function PROJECTILE_Blizzard02_OnFlyingStart(kProjectile)

	return	true;
end
function PROJECTILE_Blizzard02_OnFlying(kProjectile)

	return	true;
end

function PROJECTILE_Blizzard02_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);	
end

function PROJECTILE_Blizzard02_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end


function PROJECTILE_Blizzard02_OnArrivedAtTarget(kProjectile)
	ODS("PROJECTILE_Blizzard02_OnArrivedAtTarget\n");

	--PROJECTILE_Blizzard02_OnTargetListModified(kProjectile);

	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		return false;
	end
	
	local	kParentActor = kParentPilot:GetActor()
	local Effect = kProjectile:GetParamValue("effect")
	if nil==Effect or ""==Effect then
		Effect="ef_Blizzard_00002"
	end

	kParentActor:AttachParticleToPointWithRotate(20 + tonumber(kProjectile:GetParamValue("index")), kProjectile:GetWorldPos(),Effect, Quaternion(math.random(0,360), Point3(0,0,1)), 1);

	return	false;
end
function PROJECTILE_Blizzard02_OnAnimationEvent(kProjectile)

	return	true
end
