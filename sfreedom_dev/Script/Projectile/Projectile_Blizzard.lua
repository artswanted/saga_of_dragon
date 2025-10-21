-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function PROJECTILE_Blizzard_OnLoadingStart(kProjectile)

	return	true;
end
function PROJECTILE_Blizzard_OnLoading(kProjectile)

	return	true;
end
function PROJECTILE_Blizzard_OnFlyingStart(kProjectile)

	return	true;
end
function PROJECTILE_Blizzard_OnFlying(kProjectile)

	return	true;
end

function PROJECTILE_Blizzard_OnCollision(kProjectile,kCollideActor,kTargets)
	--	타겟잡기
	
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		return 0;
	end
	
	local	kParentActor = kParentPilot:GetActor();
	if kParentActor:IsNil() then
		return	0
	end
	
	
	local	kAction = kParentActor:GetAction();
	if kAction:IsNil() then
		return	0
	end
		
	local	kPos = kCollideActor:GetHitABVCenterPos();
	local	fRange = 30;
	
	kParam = FindTargetParam();
	kParam:SetParam_1(kPos,kParentActor:GetLookingDir());
	kParam:SetParam_2(0,0,fRange,0);
	kParam:SetParam_3(true,FTO_NORMAL);

	if kParentActor:IsUnderMyControl() then
		kParentPilot = GetMyActor():GetPilot();
	end
	
	return kAction:FindTargetsEx2(kProjectile:GetParentActionNo(),TAT_SPHERE,kParam,kTargets,kTargets,kParentPilot);
	
end

function PROJECTILE_Blizzard_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end


function PROJECTILE_Blizzard_OnArrivedAtTarget(kProjectile)
	ODS("PROJECTILE_Blizzard_OnArrivedAtTarget\n");

	--PROJECTILE_Blizzard_OnTargetListModified(kProjectile);

	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		return false;
	end
	
	local	kParentActor = kParentPilot:GetActor()

	kParentActor:AttachParticleToPointWithRotate(20 + tonumber(kProjectile:GetParamValue("index")), kProjectile:GetWorldPos(),"ef_Blizzard_00002", Quaternion(math.random(0,360), Point3(0,0,1)), 1);

	return	false;
end
function PROJECTILE_Blizzard_OnAnimationEvent(kProjectile)

	return	true
end
