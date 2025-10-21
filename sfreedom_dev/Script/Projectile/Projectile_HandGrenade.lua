-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function PROJECTILE_HandGrenade_OnLoadingStart(kProjectile)	
	return	true;
end
function PROJECTILE_HandGrenade_OnLoading(kProjectile)

	return	true;
end
function PROJECTILE_HandGrenade_OnFlyingStart(kProjectile)
	kProjectile:SetMovingType(2); -- 베지어 곡선으로 이동 한다.
	local kPos1 = kProjectile:GetFireStartPos();
	local kPos4 = kProjectile:GetTargetLoc();

	local fexValue = tonumber(kProjectile:GetParamValue("exValue"));
	kProjectile:SetSpeed(200 + fexValue * 1.3);

	local kPos2 = kProjectile:GetFireStartPos();
	local kPos3 = kProjectile:GetTargetLoc();
	kPos2:SetZ(kPos2:GetZ() + 40);
	kPos3:SetZ(kPos3:GetZ() + 50);

	local kDistance = kPos1:Distance(kPos4);
	local kDir = kProjectile:GetDirection();
	kDir:Multiply(-kDistance*0.15);
	kPos2:Add(kDir);

	kDir = kProjectile:GetDirection();
	kDir:Multiply(kDistance*0.40);
	kPos3:Add(kDir);

--	local kPosUp = kProjectile:GetUp();
	
--	local kDistance = kPos1:Distance(kPos4);
--	kPosUp:Multiply(40);
--	local kPos2 = kPos1;
--	local kPos3 = kPos4;

--	local kDir = kProjectile:GetDirection();
--	kDir:Multiply(-kDistance*0.2);
	
--	kPos2:Add(kPosUp);
--	kPos2:Add(kDir);
	
--	kPos3:Add(kPosUp);
--	kDir = kProjectile:GetDirection();
--	kDir:Multiply(-kDistance*0.5);
--	kPos3:Add(kDir);
	
	kProjectile:SetParam_Bezier4SplineType(kPos1, kPos2, kPos3, kPos4);
	return	true;
end
function PROJECTILE_HandGrenade_OnFlying(kProjectile)

	return	true;
end

function PROJECTILE_HandGrenade_OnCollision(kProjectile,kCollideActor,kTargets)
	
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

	if kProjectile:GetParamValue("action") == "a_WP Granade" then
		fRange = tonumber(kProjectile:GetParamValue("range"));
	end

	
	kParam = FindTargetParam();
	kParam:SetParam_1(kPos,kParentActor:GetLookingDir());
	kParam:SetParam_2(0,0,fRange,0);
	kParam:SetParam_3(true,FTO_NORMAL);

	if kProjectile:GetParamValue("action") == "a_HE Granade" then
		kParam:SetParam_3(true,FTO_NORMAL + FTO_DOWN);
	end

	if kParentActor:IsUnderMyControl() then
		kParentPilot = GetMyActor():GetPilot();
	end
	
	return kAction:FindTargetsEx2(kProjectile:GetParentActionNo(),TAT_SPHERE,kParam,kTargets,kTargets,kParentPilot);
	
end

function PROJECTILE_HandGrenade_OnTargetListModified(kProjectile)

	ODS("PROJECTILE_HandGrenade_OnTargetListModified\n");

	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	local	kTargetList = kProjectile:GetActionTargetList();
	
	if kParentPilot:IsNil() then
		kTargetList:ApplyActionEffects();
		return false;
	end

	local i = 0;
	
	if kTargetList:IsActionEffectApplied() then
		return false
	end
	
	local   bWeaponSoundPlayed = false;
	
	while i<kTargetList:size() do
		
		local	kActionTargetInfo = kProjectile:GetActionTargetInfo(i);
		local	kActionEffect = kActionTargetInfo:GetActionResult();
		
		if kActionEffect:IsMissed() == false then
		
			local	kTargetPilot =  g_pilotMan:FindPilot(kActionTargetInfo:GetTargetGUID());
			if kTargetPilot:IsNil() == false then

  				local	kTargetActor = kTargetPilot:GetActor()
  				local	kParentActor = kParentPilot:GetActor()

				local	kTargetGUID = kActionTargetInfo:GetTargetGUID();
				
				if bWeaponSoundPlayed == false then
                    bWeaponSoundPlayed = true;
                    -- 피격 소리 재생
					local actionName = GetSkillDef(kProjectile:GetParentActionNo()):GetActionName():GetStr()
					if kActionEffect:GetCritical() then
						actionName = "Critical_dmg"
					end
					kTargetActor:PlayWeaponSound(WST_HIT, kParentActor, actionName, 0, kActionTargetInfo);
                end					
                
				local	iSphereIndex = kActionTargetInfo:GetABVIndex();
				local pt = kTargetActor:GetABVShapeWorldPos(iSphereIndex);		

				local	kAction = kParentActor:GetAction();
				local   kHitTargetEffectID = kAction:GetScriptParam("HIT_TARGET_EFFECT_ID");
				local   kHitTargetEffectTargetNodeID = kAction:GetScriptParam("HIT_TARGET_EFFECT_TARGET_NODE");
				
				local kSoundID = "He_Granade_Bomb";
				if kHitTargetEffectID ~= "" and kHitTargetEffectTargetNodeID ~= "" then
					kTargetActor:AttachParticle(12,kHitTargetEffectTargetNodeID,kHitTargetEffectID);
				else
					if kProjectile:GetParamValue("action") == "a_HE Granade" then
						kTargetActor:AttachParticleToPoint(12 + tonumber(kProjectile:GetParamValue("GrenadeIndex")), pt, "ef_boom_05");
					elseif  kProjectile:GetParamValue("action") == "a_Flash Bang" then
						kParentActor:AttachParticleToPoint(22 + tonumber(kProjectile:GetParamValue("GrenadeIndex")), pt, "ef_Granade_Flashbang_02");
						kSoundID="FlashBang"
					elseif kProjectile:GetParamValue("action") == "a_WP Granade" then
						kParentActor:AttachParticleToPointS(32 + tonumber(kProjectile:GetParamValue("GrenadeIndex")),pt,"ef_Granade_WP_02", 2.0);
					elseif kProjectile:GetParamValue("action") == "a_Smoke Granade" then
						kParentActor:AttachParticleToPointS(42 + tonumber(kProjectile:GetParamValue("GrenadeIndex")),pt,"ef_CursedBomb_01", 1.0);
						kSoundID="smoke_granade"
					end

				end	

				kTargetActor:AttachSound(202 + tonumber(kProjectile:GetParamValue("GrenadeIndex")),kSoundID);

			--	kTargetActor:SetShakeInPeriod(5,200);
			end	
		end
		
		i=i+1;

	end

	kTargetList:ApplyActionEffects();

	return	false;

end

function PROJECTILE_HandGrenade_OnUpdate(kProjectile)
	
	if kProjectile:GetParamValue("ARRIVED") ~= "TRUE" then
		return true;
	end
	
	local	iCurrentTime = timeGetTime();
	local	iPrevTime = tonumber(kProjectile:GetParamValue("ARRIVED_TIME"));

	if (iCurrentTime - iPrevTime) > 90 then
		kProjectile:SetParamValue("ARRIVED_TIME",""..timeGetTime());
		local iRet = PROJECTILE_HandGrenade_HitOneTime(kProjectile,kParentPilot:GetActor());

		return iRet;
	end
	

	return	true;
end

function PROJECTILE_HandGrenade_OnArrivedAtTarget(kProjectile)
	PROJECTILE_HandGrenade_OnTargetListModified(kProjectile)

	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
	if kParentPilot:IsNil() then
		return false;
	end
	
	local	kParentActor = kParentPilot:GetActor();

	local kPos = kProjectile:GetWorldPos();

	if kProjectile:GetParamValue("action") == "a_HE Granade" then
		kParentActor:AttachParticleToPointS(12 + tonumber(kProjectile:GetParamValue("GrenadeIndex")), kPos,"ef_Splash_Trap_dummy01", 1.0);
		kParentActor:AttachSound(202 + tonumber(kProjectile:GetParamValue("GrenadeIndex")),"He_Granade_Bomb");
	elseif  kProjectile:GetParamValue("action") == "a_Flash Bang" then
		kParentActor:AttachParticleToPointS(22 + tonumber(kProjectile:GetParamValue("GrenadeIndex")), kPos,"ef_Granade_Flashbang_02", 1.0);
		kParentActor:AttachSound(202 + tonumber(kProjectile:GetParamValue("GrenadeIndex")),"FlashBang");
	elseif kProjectile:GetParamValue("action") == "a_WP Granade" then
		kParentActor:AttachParticleToPointS(32 + tonumber(kProjectile:GetParamValue("GrenadeIndex")), kPos,"ef_Granade_WP_02", 2.0);
		kParentActor:AttachSound(202 + tonumber(kProjectile:GetParamValue("GrenadeIndex")),"He_Granade_Bomb");
	elseif kProjectile:GetParamValue("action") == "a_Smoke Granade" then
		kParentActor:AttachParticleToPointS(42 + tonumber(kProjectile:GetParamValue("GrenadeIndex")), kPos,"ef_Splash_Trap_dummy01", 1.0);
		kParentActor:AttachSound(202 + tonumber(kProjectile:GetParamValue("GrenadeIndex")),"He_Granade_Bomb");
	end
	
	return	false;
end
function PROJECTILE_HandGrenade_OnAnimationEvent(kProjectile)

	return	true
end
