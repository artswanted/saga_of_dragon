-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function MULTISTRAPE_ARROW_OnLoadingStart(kProjectile)
	return	true;
end
function MULTISTRAPE_ARROW_OnLoading(kProjectile)
	return	true;
end
function MULTISTRAPE_ARROW_OnFlyingStart(kProjectile)
	local fexValue =  1--tonumber(kProjectile:GetParamValue("exValue"));	
--배지어
	if(3 ~= kProjectile:GetMovingType()) then
		-- kProjectile:SetMovingType(2); -- 베지어 곡선으로 이동 한다.
		-- local kPos1 = kProjectile:GetFireStartPos();
		-- local kPos4 = kProjectile:GetTargetLoc();
		
-- --		kPos1:SetZ(kPos1:GetZ()+ math.random(0,5))
		-- kProjectile:SetParam(350,50); 		 --속력, 가속력, 중력
		
		-- local kPos2 = kProjectile:GetFireStartPos();
		-- local kPos3 = kProjectile:GetTargetLoc();
		-- kPos2:SetZ(kPos2:GetZ()+50);
		-- kPos2:SetX(kPos2:GetX()+100);
		
		-- --kPos2:SetZ(kPos2:GetZ() + math.random(100, 120));		
		-- -- kPos3:SetZ(kPos3:GetZ() - math.random(0, 30));
		
		-- -- kPos2:SetX(kPos2:GetX() + math.random(0, 50));
		-- -- kPos3:SetX(kPos3:GetX() - math.random(0, 50));
		
		-- -- kPos2:SetY(kPos2:GetY() + math.random(0, 50));
		-- -- kPos3:SetY(kPos3:GetY() - math.random(0, 50));

		-- local kDistance = kPos1:Distance(kPos4);
		-- local kDir = kProjectile:GetDirection();
		-- kDir:Multiply(-kDistance*0.20);
		-- kPos2:Add(kDir);

		-- kDir = kProjectile:GetDirection();
		-- kDir:Multiply(kDistance*0.40);
		-- kPos3:Add(kDir);
		
		-- kProjectile:SetParam_Bezier4SplineType(kPos1, kPos2, kPos3, kPos4);

-- 싸인 곡선
	-- kProjectile:SetMovingType(1)
	-- kProjectile:SetParam(350,50); 
	-- kProjectile:SetParam_SinCurveLineType(100, 45* 3.141592/180)

	end
-- 유도탄
--[[
	kProjectile:SetMovingType(3)		 		 --유도탄 타입
	kProjectile:SetParam(500,2000,0); 		 --속력, 가속력, 중력
]]	
--	kProjectile:SetParamValue("TIME", tostring(GetWorldAccumTime()))		
	return	true;
end
function MULTISTRAPE_ARROW_OnFlying(kProjectile)	

--[[ 이거 붙이고 12번 쏘면 데드락 됨
	local fTime = tonumber(kProjectile:GetParamValue("TIME"))
	if(GetWorldAccumTime() - fTime > 0.001) then
		local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
		local	kParentActor = kParentPilot:GetActor();
		kProjectile:SetParamValue("TIME", tostring(GetWorldAccumTime()))
		local kCurPos = kProjectile:GetWorldPos()
		kParentActor:AttachParticleToPoint(12993,kCurPos,"ef_Enchant_kat_01_l");
	end
]]
	return	true;
end

function MULTISTRAPE_ARROW_OnCollision(kProjectile, kCollideActor, kTargets)
	ODS("MULTISTRAPE_ARROW_OnCollision\n", false, 912)	
	if(3 ~= kProjectile:GetMovingType()) then	-- 올라가는 중이면 충돌 체크 하지 않음
		ODS("올라가는중\n", false, 912)
		return 0
	end

	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		ODS("파일롯이 nil\n", false, 912)
		return 0;
	end
	
	local	kParentActor = kParentPilot:GetActor();
	if kParentActor:IsNil() then
		ODS("액터가 nil\n", false, 912)
		return	0
	end
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
		ODS("액션이 nil\n", false, 912)
		return	0
	end
		
	local	kPos = kCollideActor:GetHitABVCenterPos()
	local	fRange = kAction:GetSkillRange(0,kParentActor)
	
	kParam = FindTargetParam(0, kParentActor)
	kParam:SetParam_1(kPos,kParentActor:GetLookingDir())
	kParam:SetParam_2(0,0,fRange,0)
	kParam:SetParam_3(true,FTO_NORMAL)

	if kParentActor:IsUnderMyControl() then
		kParentPilot = GetMyActor():GetPilot()
	end
	
	--local kCurPos = kProjectile:GetWorldPos()
	--kParentActor:AttachParticleToPoint(12992,kCurPos,"ef_base_dmg_ar_02");
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

function MULTISTRAPE_ARROW_OnTargetListModified(kProjectile)
	return SkillFunc_ProjectileHitOneTime(kProjectile)
end

-- 이거 return false의 의미가 멀까
function MULTISTRAPE_ARROW_OnArrivedAtTarget(kProjectile)
--	ODS("목표점 도착\n")
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())
	if(true == kParentPilot:IsNil() ) then return false end
		
	local	kParentActor = kParentPilot:GetActor();
	if(true == kParentActor:IsNil() ) then return false end
		
--	kProjectile:SetParamValue("TIME", tostring(GetWorldAccumTime()))
--	kParentActor:DetachFrom(12993);	 -- 파티클 쓸대 같이 쓸것 근데 12번 쏘면 다운됨

---------------------------------------------------------------------------------
--	if("ATTACK" ~= kProjectile:GetParamValue("STATE")) then 
	if(3 ~= kProjectile:GetMovingType()) then
		local 	kCurPos = kProjectile:GetWorldPos()
		
		local	kAction = kParentActor:GetAction();
		if(true == kAction:IsNil() ) then return false end
		
		local	kProjectileMan = GetProjectileMan();
		local	kDir = kProjectile:GetDirection();
		local	fMoveDistance = 30.0;
		
		local STD_INDEX = 1000
		local iTargetNum = tonumber(kParentActor:GetParam(STD_INDEX))
		if(nil == iTargetNum or 0 == iTargetNum) then 
			ODS("발사체: 타겟이 없다\n", false, 912)
			return false 
		end
		
		ODS("발사체: Target 갯수: "..iTargetNum.."\n", false, 912)
		
		----------- 타겟 찾음
		local iCurIndex = tonumber(kParentActor:GetParam("MULTISTRAPE_TARGET_INDEX"))
		if(nil == iCurIndex) then iCurIndex = 0 end
		iCurIndex = iCurIndex % iTargetNum
		kParentActor:SetParam("MULTISTRAPE_TARGET_INDEX", tostring(iCurIndex+1))
		---------------------------
		iCurIndex = STD_INDEX+iCurIndex
		
		local StrTargetGuid = kParentActor:GetParam(iCurIndex+1)
		if(nil == StrTargetGuid) then 
			ODS("발사체: 타겟 GUID가 없다\n", false, 912)
			return false 
		end
		local kTargetGuid = GUID(StrTargetGuid)		
		
		ODS("발사체 : Index:"..(iCurIndex+1).." 타겟 GUID:"..StrTargetGuid.."\n", false, 912)
		 
		local kTargetPilot = g_pilotMan:FindPilot(kTargetGuid)
		if(true == kTargetPilot:IsNil()) then return false end
		
		local kActorTarget = kTargetPilot:GetActor();
		if(true == kActorTarget:IsNil()) then return false end
		if(0 >= kActorTarget:GetAbil(AT_HP)) then 
			--이미 죽은놈 타겟 안함
			ODS("이미 죽었다\n", false, 912)
			return false 
		end
		
		local kTargetPos = kActorTarget:GetPos() 
		--local kTargetPos = kActorTarget:GetHitPoint()
		kTargetPos:Add(Point3(0,0,10))
		ODS("Index"..(iCurIndex+1).." X:"..kTargetPos:GetX().." Y:"..kTargetPos:GetY().." Z:"..kTargetPos:GetZ().."\n", false, 912)
		---------- 위치 얻음
		
		--local	kArrow = kProjectileMan:CreateNewProjectile("PROJECTILE_MULTISTRAPE_ARROW",kAction,kParentActor:GetPilotGuid());	
		local	kArrow = kProjectile
		
		kDir:Multiply(fMoveDistance);
		kCurPos:Add(kDir);	
		kArrow:SetWorldPos(kCurPos);		

		if kArrow:IsNil() == false then				
--[[			local iPentrationCount = kParentActor:GetAbil(AT_PENETRATION_COUNT);
			kArrow:SetPenetrationCount(iPentrationCount);-- 몇번을 관통할것인지 세팅			
			if iPentrationCount ~= 0 then
				kArrow:SetMultipleAttack(true);
			end
]]
			local kAEffect = kParentActor:GetAttackEffect(kAction:GetID())
			if false==kAEffect:IsNil() and ""~=kAEffect:GetEffectName() then
				kParentActor:AttachParticleS(421,kAEffect:GetNodeName(),kAEffect:GetEffectName(),kAEffect:GetScale())
			end
			
			--kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());				
			kArrow:SetMovingType(3)		 			 --유도탄 타입
			kArrow:SetParam(1000,0,0); 		 --속력, 가속력, 중력			
			--kArrow:SetParam(100,0,0)
			
			--kPos:Add(Point3(math.random(-150, 150),math.random(-150, 150),0))						
			--kArrow:SetTargetLoc(kPos);
			
			kArrow:SetTargetLoc(kTargetPos);			
			kArrow:SetTargetGuidFromServer(kTargetGuid)
			kArrow:Fire(true);
			
			kParentActor:AttachSound(2800,"MultiStripe_03")
			return true	-- 다시 세팅해서 날아갈수 있게
		end		
	else		
		--local kCurPos = kProjectile:GetWorldPos()
		--kParentActor:AttachParticleToPoint(12992,kCurPos,"ef_base_dmg_ar_02");		
		--MULTISTRAPE_ARROW_OnTargetListModified(kProjectile);
		--kParentActor:ClearIgnoreEffectList(); -- 이거 머하는거지	
		--return	false
	end	
------------------------------------------------------------------------------------	
end

function MULTISTRAPE_ARROW_OnAnimationEvent(kProjectile)
	return	true
end
