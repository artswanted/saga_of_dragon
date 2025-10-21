-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function PROJECTILE_MagicCannonBall_OnLoadingStart(kProjectile)	
	return	true;
end
function PROJECTILE_MagicCannonBall_OnLoading(kProjectile)
	return	true;
end

function PROJECTILE_MagicCannonBall_OnFlyingStart(kProjectile)
	kProjectile:SetMovingType(2); -- 베지어 곡선으로 이동 한다.
	local kPos1 = kProjectile:GetFireStartPos();
	local kPos4 = kProjectile:GetTargetLoc();

	--local fexValue = tonumber(kProjectile:GetParamValue("exValue"));
	--kProjectile:SetSpeed(200 + fexValue * 1.3);

	local kPos2 = kProjectile:GetFireStartPos();
	local kPos3 = kProjectile:GetTargetLoc();
	kPos2:SetZ(kPos2:GetZ() + 240);
	kPos3:SetZ(kPos3:GetZ() + 150);

	local kDistance = kPos1:Distance(kPos4);
	local kDir = kProjectile:GetDirection();
	kDir:Multiply(-kDistance*0.15);
	kPos2:Add(kDir);

	kDir = kProjectile:GetDirection();
	kDir:Multiply(kDistance*0.40);
	kPos3:Add(kDir);
	
	kProjectile:SetParam_Bezier4SplineType(kPos1, kPos2, kPos3, kPos4);
	return	true;
end
function PROJECTILE_MagicCannonBall_OnFlying(kProjectile)

	return	true;
end

function PROJECTILE_MagicCannonBall_OnCollision(kProjectile, kCollideActor, kTargets)
	--	Find Target
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

	--[[if kProjectile:GetParamValue("action") == "a_WP Granade" then
		fRange = tonumber(kProjectile:GetParamValue("range"));
	end]]

	
	kParam = FindTargetParam();
	kParam:SetParam_1(kPos,kParentActor:GetLookingDir());
	kParam:SetParam_2(0,0,fRange,0);
	kParam:SetParam_3(true,FTO_NORMAL);

	if kParentActor:IsUnderMyControl() then
		kParentPilot = GetMyActor():GetPilot();
	end
	
	return kAction:FindTargetsEx2(kProjectile:GetParentActionNo(),TAT_SPHERE,kParam,kTargets,kTargets,kParentPilot);
end

function PROJECTILE_MagicCannonBall_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end

function PROJECTILE_MagicCannonBall_OnUpdate(kProjectile)
	if kProjectile:GetParamValue("ARRIVED") ~= "TRUE" then
		return true;
	end
	
	local	iCurrentTime = timeGetTime();
	local	iPrevTime = tonumber(kProjectile:GetParamValue("ARRIVED_TIME"));

	if (iCurrentTime - iPrevTime) > 90 then
		kProjectile:SetParamValue("ARRIVED_TIME",""..timeGetTime());
		local iRet = PROJECTILE_MagicCannonBall_HitOneTime(kProjectile,kParentPilot:GetActor());

		return iRet;
	end
	

	return	true;
end

function PROJECTILE_MagicCannonBall_OnArrivedAtTarget(kProjectile)
	--ODS( GetAccumTime() ..  " PROJECTILE_MagicCannonBall_OnArrivedAtTarget >>>>>> \n", false, 998 )
	PROJECTILE_MagicCannonBall_OnTargetListModified(kProjectile)
	
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
	if kParentPilot:IsNil() then
		return false;
	end
	
	local	kParentActor = kParentPilot:GetActor();
	
	local kPos = kProjectile:GetWorldPos();
	kPos:Add( Point3(0, -100, 0) )
	-- Can add Explosion Effect
	--kParentActor:AttachParticleToPointS(12, kPos,"ef_bang", 30.0);
	kParentActor:AttachParticleToPointS(12, kPos,"ef_bang_02_p_ef_heart", 50.0);
	kParentActor:AttachSound(12, "MagicCannon_Shot02")
	
	return	false;
end
function PROJECTILE_MagicCannonBall_OnAnimationEvent(kProjectile)
	return	true
end
