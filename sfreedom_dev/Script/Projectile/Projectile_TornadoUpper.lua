-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function PROJECTILE_TORNADO_UPPER_OnLoadingStart(kProjectile)

	return	true;
end
function PROJECTILE_TORNADO_UPPER_OnLoading(kProjectile)

	return	true;
end
function PROJECTILE_TORNADO_UPPER_OnFlyingStart(kProjectile)	
	return	true;
end
function PROJECTILE_TORNADO_UPPER_OnFlying(kProjectile)

	return	true;
end

function PROJECTILE_TORNADO_UPPER_OnCollision(kProjectile,kCollideActor,kTargets)
	ODS("PROJECTILE_TORNADO_UPPER_OnCollision\n")
	
	--	타겟잡기
	
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		return 0;
	end
	
	local	kParentActor = kParentPilot:GetActor();
	if kParentActor:IsNil() then
		return	0
	end

	local iPentrationCount = kProjectile:GetPenetrationCount();
	if iPentrationCount == nil then
		iPentrationCount = 0;	
	end

	if iPentrationCount ~= 0 then
		kTargets:clear();
		kTargets:SetActionEffectApplied(false);
	end	

	--ODS("iPentrationCount : "..iPentrationCount.."\n")
	
	local	kAction = kParentActor:GetAction();
	if kAction:IsNil() then
		return	0
	end
		
	local	kPos = kCollideActor:GetHitABVCenterPos();
	local	fRange = 50
	
	kParam = FindTargetParam();	

	if kParentActor:IsUnderMyControl() then
		kParentPilot = GetMyActor():GetPilot();
	end

	--if iPentrationCount ~= 0 then
		--fRange = kAction:GetSkillRange(0,kParentActor)
	--	kParam:SetParam_2(fRange,fRange,0, iPentrationCount + 1);
	--else
		kParam:SetParam_2(0,0,fRange,0);		
	--end
	kParam:SetParam_1(kPos,kParentActor:GetLookingDir());
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return kAction:FindTargetsEx2(kProjectile:GetParentActionNo(),TAT_SPHERE,kParam,kTargets,kTargets,kParentPilot);
end

function PROJECTILE_TORNADO_UPPER_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end


function PROJECTILE_TORNADO_UPPER_OnArrivedAtTarget(kProjectile)
	
	PROJECTILE_TORNADO_UPPER_OnTargetListModified(kProjectile);	

	return	false;
end
function PROJECTILE_TORNADO_UPPER_OnAnimationEvent(kProjectile)

	return	true
end
