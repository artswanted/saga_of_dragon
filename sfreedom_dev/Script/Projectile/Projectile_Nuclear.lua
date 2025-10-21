-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function PROJECTILE_NUCLEAR_OnLoadingStart(kProjectile)
	return	true;
end
function PROJECTILE_NUCLEAR_OnLoading(kProjectile)
	return	true;
end

function PROJECTILE_NUCLEAR_OnFlying(kProjectile)	
	return	true;
end

function PROJECTILE_NUCLEAR_OnCollision(kProjectile, kCollideActor, kTargets)
	ODS("PROJECTILE_NUCLEAR_OnCollision\n", false, 912)	
	if kProjectile:IsNil() then
		return 0;
	end

	local fTime = tonumber(kProjectile:GetParamValue("TIME"))
	if(GetWorldAccumTime() - fTime < 0.2) then
		-- 시작하자 마자 충돌 시키진 않는다 (미사일이 날아가는걸 좀 보여주려고)
		return 0
	end	
	
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		return 0;
	end
	
	local	kParentActor = kParentPilot:GetActor();
	if kParentActor:IsNil() then
		return	0
	end
	
	local kCurPos = kProjectile:GetWorldPos()
	
--[[
	local iPentrationCount = kProjectile:GetPenetrationCount();
	if iPentrationCount == nil then
		iPentrationCount = 0;	
	end

	if iPentrationCount ~= 0 then
		kTargets:clear();
		kTargets:SetActionEffectApplied(false);
	end	
	--ODS("iPentrationCount : "..iPentrationCount.."\n")
]]
	
	
	local	kAction = kParentActor:GetAction();
	if kAction:IsNil() then
		return	0
	end
		
	local	kPos = kCollideActor:GetHitABVCenterPos()
	local	fRange = kAction:GetSkillRange(0,kParentActor)
	if(0 == fRange) then 
		fRange = 100
	end
	
	local iMaxTarget = kAction:GetAbil(AT_MAX_TARGETNUM)
	if(nil == iMaxTarget) then 
		iMaxTarget = 1
	end
	
--	ODS("Range: "..fRange .."\n", false, 912)
	kParam = FindTargetParam(0, kParentActor)
	kParam:SetParam_1(kPos,kParentActor:GetLookingDir())
	kParam:SetParam_2(fRange, 360, fRange, iMaxTarget) 	--SetParam_2( fRange, fWideAngle,float fEffectRadius,int iMaxTargets)
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN)

	if kParentActor:IsUnderMyControl() then
		kParentPilot = GetMyActor():GetPilot()
	end
	
--	local kCurPos = kProjectile:GetWorldPos()
--	kParentActor:AttachParticleToPoint(12992,kCurPos,"ef_boom_06");
--[[	
	if iPentrationCount ~= 0 then
		fRange = kAction:GetSkillRange(0,kParentActor)
		kParam:SetParam_1(kPos,kParentActor:GetLookingDir());
		kParam:SetParam_2(fRange,30,0,iPentrationCount + 1);
		kParam:SetParam_3(true,FTO_NORMAL);	
		
		return kAction:FindTargetsEx2(kProjectile:GetParentActionNo(),TAT_BAR,kParam,kTargets,kTargets,kParentPilot);
	else
		return kAction:FindTargetsEx2(kProjectile:GetParentActionNo(),TAT_SPHERE,kParam,kTargets,kTargets,kParentPilot);
	end
	]]
	return kAction:FindTargetsEx2(kProjectile:GetParentActionNo(),TAT_SPHERE,kParam,kTargets,kTargets,kParentPilot);
end

function PROJECTILE_NUCLEAR_OnTargetListModified(kProjectile)

	local bResult = SkillFunc_ProjectileHitOneTime(kProjectile)
	if( true == bResult ) then return true end
	
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
	if kParentPilot:IsNil() then
		return false;
	end
	local	kParentActor = kParentPilot:GetActor();
	if kParentActor:IsNil() then
		return	false
	end
	
	local kCurPos = kProjectile:GetWorldPos()
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local ptcl = g_world:ThrowRay(kCurPos, Point3(0,0,-1),500);
	if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
		ptcl = g_world:ThrowRay(kCurPos, Point3(0,0,1),500);
		if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
			ptcl = kProjectile:GetWorldPos()
		end
	end
		
	kParentActor:AttachParticleToPoint(12992,ptcl,"ef_boom_07");	
	--kParentActor:AttachParticleToPointS(12993,ptcl,"ef_boom_06", 0.7);
	kParentActor:AttachParticleToPointS(12994,ptcl,"ef_Hammer_Crush_01", 2);
	kParentActor:AttachSound(2783,"NuClear_Bomb");
	
	return	false;
end

-- 이거 return false의 의미가 멀까
function PROJECTILE_NUCLEAR_OnArrivedAtTarget(kProjectile)

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
	
	ODS("PROJECTILE_NUCLEAR_OnArrivedAtTarget\n", false, 912)		
--[[------------------------------	---------------	---------------	---------------	---------------	---------------	
	
	local	kPos = kProjectile:GetWorldPos()
	local	fRange = kAction:GetSkillRange(0,kParentActor)
	if(0 == fRange) then 
		fRange = 100
	end
	
	local iMaxTarget = kAction:GetAbil(AT_MAX_TARGETNUM)
	if(nil == iMaxTarget) then 
		ODS("뭐야 이거!\n", false, 912)
		SetBreak()
		iMaxTarget = 1
	end

	local kParam = FindTargetParam();
	kParam = FindTargetParam(0, kParentActor)
	kParam:SetParam_1(kPos,kParentActor:GetLookingDir())
	kParam:SetParam_2(200, 360, 200, iMaxTarget) 	--SetParam_2( fRange, fWideAngle,float fEffectRadius,int iMaxTargets)
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN)

	local	kTargetsList = CreateActionTargetList(kParentActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	iTargets = kAction:FindTargets(TAT_SPHERE,kParam,kTargetsList,kTargetsList);
	if(0 == iTargets) then 
		DeleteActionTargetList(kTargetsList)	-- 없으면 임시 타겟 리스트 지움		 
	elseif(false == kTargetsList:IsNil()) then 
		--타겟 설정 
		kProjectile:SetTargetObjectList(kTargetsList)
		ODS("강제 타겟 브로드 캐스팅!\n", false, 912)
		--브로드 캐스팅	
		--g_pilotMan:BroadCast_PAction(kProjectile,kTargetsList)		
	end	
	]]
-----------------------------------------------------------------------------------------------------------------------
	PROJECTILE_NUCLEAR_OnTargetListModified(kProjectile)
end

function PROJECTILE_NUCLEAR_OnAnimationEvent(kProjectile)
	return	true
end