function PROJECTILE_RIFLE_CHARGE_SHOT_OnFlyingStart(kProjectile)
	return	true;
end

function PROJECTILE_RIFLE_CHARGE_SHOT_OnCollision(kProjectile,kCollideActor,kTargets)
	
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
	
	local iPentrationCount = kProjectile:GetPenetrationCount();
	if iPentrationCount == nil then
		iPentrationCount = 0;	
	end

	if iPentrationCount ~= 0 then
		kTargets:clear();
		kTargets:SetActionEffectApplied(false);
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
	
	if iPentrationCount ~= 0 then
		fRange = kAction:GetSkillRange(0,kParentActor)
		kParam:SetParam_1(kPos,kParentActor:GetLookingDir());
		kParam:SetParam_2(fRange,30,0,iPentrationCount);
		kParam:SetParam_3(true,FTO_NORMAL);	
		
		return kAction:FindTargetsEx2(kProjectile:GetParentActionNo(),TAT_BAR,kParam,kTargets,kTargets,kParentPilot);
	else
		return kAction:FindTargetsEx2(kProjectile:GetParentActionNo(),TAT_SPHERE,kParam,kTargets,kTargets,kParentPilot);
	end
end

function PROJECTILE_RIFLE_CHARGE_SHOT_OnTargetListModified(kProjectile)

	SkillFunc_ProjectileHitOneTime(kProjectile)

	return	true;
end

function PROJECTILE_RIFLE_CHARGE_SHOT_OnArrivedAtTarget(kProjectile)
	PROJECTILE_RIFLE_CHARGE_SHOT_OnTargetListModified(kProjectile);	
	return	false;
end

function PROJECTILE_RIFLE_CHARGE_SHOT_OnAnimationEvent(kProjectile)
	return	true
end

function PROJECTILE_RIFLE_CHARGE_SHOT_OnUpdate(kProjectile)
	return true;
end