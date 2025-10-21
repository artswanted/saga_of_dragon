-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function PROJECTILE_VALCAN300_OnFlyingStart(kProjectile)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	kProjectile:SetParamValue("7", "".. g_world:GetAccumTime());
	kProjectile:SetParamValue("8", "".. 0);

	local ptcl = g_world:ThrowRay(kProjectile:GetWorldPos(), Point3(0,0,-1), 100);
	if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
		--바닥을 찾지못하면 이펙트를 붙이지 않는다.
		return true;
	end
		
	ptcl:SetZ(ptcl:GetZ()+5);
	-- 첫발 발사시에 나오는 이펙트
	g_world:AttachParticleSWithRotate("ef_Splash_Trap_dummy01", ptcl, 0.5, Point3(0,0,0));
	return	true;
end


function PROJECTILE_VALCAN300_OnCollision(kProjectile,kCollideActor,kTargets)
	
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

function PROJECTILE_VALCAN300_OnTargetListModified(kProjectile)
	SkillFunc_ProjectileHitOneTime(kProjectile)
	return	true;
end


function PROJECTILE_VALCAN300_OnArrivedAtTarget(kProjectile)
	ODS("PROJECTILE_VALCAN300_OnArrivedAtTarget\n");

	PROJECTILE_VALCAN300_OnTargetListModified(kProjectile);	

	return	false;
end
function PROJECTILE_VALCAN300_OnAnimationEvent(kProjectile)
	return	true
end

function PROJECTILE_VALCAN300_OnUpdate(kProjectile)
	local fPrevTime = tonumber(kProjectile:GetParamValue("7"));
	local iCount = tonumber(kProjectile:GetParamValue("8"));
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local fBoomElapsedTime = g_world:GetAccumTime() - fPrevTime;
	if fBoomElapsedTime >= 0.15 and iCount < 6 then
		
		kProjectile:SetParamValue("7", "".. g_world:GetAccumTime());
		kProjectile:SetParamValue("8", "".. (iCount + 1));
		
		local ptcl = g_world:ThrowRay(kProjectile:GetWorldPos(), Point3(0,0,-1), 100);
		if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
			--바닥을 찾지못하면 쏘는것을 멈춘다.
			return true;
		end

		ptcl:SetZ(ptcl:GetZ()+5);
		--월드에 붙을때 랜덤하게 회전시켜서 붙인다.
		local kRandPos = Point3(math.random(-10, 10), math.random(-10, 10), 0)
		g_world:AttachParticleSWithRotate("ef_Splash_Trap_dummy01", ptcl, math.random(5, 10)/10, kRandPos);
		g_world:AttachParticleSWithRotate("ef_Hammer_Crush_01", ptcl, 0.5, kRandPos);
	    
	end

	return true;
end

