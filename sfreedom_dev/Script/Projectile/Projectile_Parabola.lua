-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function PROJECTILE_Parabola_OnLoadingStart(kProjectile)	
	return	true;
end
function PROJECTILE_Parabola_OnLoading(kProjectile)

	return	true;
end
function PROJECTILE_Parabola_OnFlyingStart(kProjectile)
	local UseFlyingStart = kProjectile:GetParamValue("USE_FLYING_START")	--Z축을 조절해 보자
	if nil~=UseFlyingStart and "FALSE"==UseFlyingStart then
		return true
	end

	kProjectile:SetMovingType(2); -- 베지어 곡선으로 이동 한다.
	local kPos1 = kProjectile:GetFireStartPos();
	local kPos4 = kProjectile:GetTargetLoc();
	

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	kPos4:SetZ(kPos4:GetZ()+20)	--약간 위로 올려서
	local ptcl = g_world:ThrowRay(kPos4, Point3(0,0,-1), 200) --바닥좌표 검사하자

	if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then	--바닥이 없으면
		ptcl:SetX(kPos4:GetX())
		ptcl:SetY(kPos4:GetY())
		ptcl:SetZ(kPos4:GetZ()-20)	--위에서 약간 올려줬음
	else
		kPos4:SetX(ptcl:GetX())
		kPos4:SetY(ptcl:GetY())
		kPos4:SetZ(ptcl:GetZ())
	end

	kProjectile:SetTargetLoc(kPos4)

	local kPos2 = kProjectile:GetFireStartPos();
	local kPos3 = kProjectile:GetTargetLoc();

	local fZ = kProjectile:GetParamValue("Z_FACTOR")	--Z축을 조절해 보자
	if nil==fZ or ""==fZ then
		fZ=1.0
	else
		fZ = tonumber(fZ)
	end
	ODS("fZ: "..fZ.."\n",false,6482)

	kPos2:SetZ(kPos2:GetZ() + 40*fZ)
	kPos3:SetZ(kPos3:GetZ() + 50*fZ)

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
function PROJECTILE_Parabola_OnFlying(kProjectile)

	return	true;
end

function PROJECTILE_Parabola_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_Parabola_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end

function PROJECTILE_Parabola_OnUpdate(kProjectile)
	
	if kProjectile:GetParamValue("ARRIVED") ~= "TRUE" then
		return true;
	end
	
	local	iCurrentTime = timeGetTime();
	local	iPrevTime = tonumber(kProjectile:GetParamValue("ARRIVED_TIME"));

	if (iCurrentTime - iPrevTime) > 90 then
		kProjectile:SetParamValue("ARRIVED_TIME",""..timeGetTime());
		local iRet = PROJECTILE_Parabola_HitOneTime(kProjectile,kParentPilot:GetActor());

		return iRet;
	end
	

	return	true;
end

function PROJECTILE_Parabola_OnArrivedAtTarget(kProjectile)
	return	PROJECTILE_MASSIVE_DEFAULT_OnArrivedAtTarget(kProjectile)
end
function PROJECTILE_Parabola_OnAnimationEvent(kProjectile)

	return	true
end
