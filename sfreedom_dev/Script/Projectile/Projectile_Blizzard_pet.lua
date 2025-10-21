-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function PROJECTILE_Blizzard_pet_OnLoadingStart(kProjectile)

	return	true;
end
function PROJECTILE_Blizzard_pet_OnLoading(kProjectile)

	return	true;
end
function PROJECTILE_Blizzard_pet_OnFlyingStart(kProjectile)
	return	true;
end
function PROJECTILE_Blizzard_pet_OnFlying(kProjectile)

	return	true;
end

function PROJECTILE_Blizzard_pet_OnCollision(kProjectile,kCollideActor,kTargets)
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

function PROJECTILE_Blizzard_pet_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end


function PROJECTILE_Blizzard_pet_OnArrivedAtTarget(kProjectile)
	ODS("PROJECTILE_Blizzard_pet_OnArrivedAtTarget\n");

	--PROJECTILE_Blizzard_pet_OnTargetListModified(kProjectile);

	--시작 시간을 기록
	kProjectile:SetParamValue("start", ""..timeGetTime())
		
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		return true;
	end
	
	local	kParentActor = kParentPilot:GetActor()

	if "PROJECTILE_Blizzard_pet" == kProjectile:GetID() then
		kParentActor:AttachParticleToPointWithRotate(20 + tonumber(kProjectile:GetParamValue("index")), kProjectile:GetWorldPos(),"ef_blizzard_pet_02", Quaternion(math.random(0,0), Point3(0,0,1)), 1);
	elseif "PROJECTILE_Meteor_pet" == kProjectile:GetID() then
		kParentActor:AttachParticleToPointWithRotate(20 + tonumber(kProjectile:GetParamValue("index")), kProjectile:GetWorldPos(),"ef_meteo_pet_02", Quaternion(math.random(0,0), Point3(0,0,1)), 1);
	end

	kProjectile:SetHide(true)

	return	true;
end
function PROJECTILE_Blizzard_pet_OnAnimationEvent(kProjectile)

	return	true
end

function PROJECTILE_Blizzard_pet_OnUpdate(kProjectile)

	local kStart = tonumber(kProjectile:GetParamValue("start"));
	local kNow = timeGetTime();

	if nil ~= kStart then
--		ODS("kStart : "..kStart.." kNow : "..kNow.."\n",false,1509)
		if kNow - kStart > 3000 then
			return false;
		end
	end

	return	true
end