-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function PROJECTILE_TWOACTION_ARROW_OnLoadingStart(kProjectile)

	return	true
end
function PROJECTILE_TWOACTION_ARROW_OnLoading(kProjectile)

	return	true
end
function PROJECTILE_TWOACTION_ARROW_OnFlyingStart(kProjectile)

	return	true
end
function PROJECTILE_TWOACTION_ARROW_OnFlying(kProjectile)

	return	true
end

function PROJECTILE_TWOACTION_ARROW_OnCollision(kProjectile,kCollideActor,kTargets)
	ODS("PROJECTILE_TWOACTION_ARROW_OnCollision\n")
	
	--	타겟잡기
	
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		return 0
	end
	
	local	kParentActor = kParentPilot:GetActor()
	if kParentActor:IsNil() then
		return	0
	end

	local iPentrationCount = kProjectile:GetPenetrationCount()
	if iPentrationCount == nil then
		iPentrationCount = 0	
	end

	if iPentrationCount ~= 0 then
		kTargets:clear()
		kTargets:SetActionEffectApplied(false)
	end	

		ODS("iPentrationCount : "..iPentrationCount.."\n")
	
	local	kAction = kParentActor:GetAction()
	if kAction:IsNil() then
		return	0
	end
		
	local	kPos = kCollideActor:GetHitABVCenterPos()
	local	fRange = 30
	
	kParam = FindTargetParam()
	kParam:SetParam_1(kPos,kParentActor:GetLookingDir())
	kParam:SetParam_2(0,0,fRange,0)
	kParam:SetParam_3(true,FTO_NORMAL)

	if kParentActor:IsUnderMyControl() then
		kParentPilot = GetMyActor():GetPilot()
	end

	local iFindTargetType = TAT_SPHERE
	if iPentrationCount ~= 0 then
		fRange = kAction:GetSkillRange(0,kParentActor)
		kParam:SetParam_1(kPos,kParentActor:GetLookingDir())
		kParam:SetParam_2(fRange,30,0,iPentrationCount + 1)
		kParam:SetParam_3(true,FTO_NORMAL)	
		iFindTargetType = TAT_BAR
	end	
	
	--- 발사체 생성	
	local iProjectleNum = kAction:GetAbil(AT_PROJECTILE_NUM);
	local iFireDeg = kAction:GetAbil(AT_MON_PROJECTILE_ANGLE);
	local kIgnoreTargetGUID = kCollideActor:GetPilotGuid() -- 지금 맞은 타겟은 충돌 체크 되면 안된다
	if(nil ~= kIgnoreTargetGUID) then
	ODS("최초 설정           GUID:"..kIgnoreTargetGUID:GetString().."\n",false, 912)
	end
	ODS("넘"..iProjectleNum.." 각:"..iFireDeg.."\n",false, 912)
	PROJECTILE_TWOACTION_ARROW_CreateLastArrow(kParentActor, kAction, kProjectile:GetWorldPos(), iProjectleNum-1, iFireDeg, kIgnoreTargetGUID)
	
	---
	return kAction:FindTargetsEx2(kProjectile:GetParentActionNo(),iFindTargetType,kParam,kTargets,kTargets,kParentPilot)
end

function PROJECTILE_TWOACTION_ARROW_OnTargetListModified(kProjectile)

	return SkillFunc_ProjectileHitOneTime(kProjectile)

end


function PROJECTILE_TWOACTION_ARROW_OnArrivedAtTarget(kProjectile)	
	PROJECTILE_TWOACTION_ARROW_OnTargetListModified(kProjectile)	
	
	local	kParentPilot = g_pilotMan:FindPilot(kProjectile:GetParentPilotGuid())

	if kParentPilot:IsNil() then
		return 0
	end
	
	local	kParentActor = kParentPilot:GetActor()
	if kParentActor:IsNil() then
		return	0
	end
	
	local	kAction = kParentActor:GetAction()
	if kAction:IsNil() then
		return	0
	end
	--[[
	-- 발사체 생성	테스트
	local iProjectleNum = kAction:GetAbil(AT_PROJECTILE_NUM);
	local iFireDeg = 180 --kAction:GetAbil(AT_MON_PROJECTILE_ANGLE);
	local kIgnoreTargetGUID = nil	
	PROJECTILE_TWOACTION_ARROW_CreateLastArrow(kParentActor, kAction, kProjectile:GetWorldPos(), iProjectleNum-1, iFireDeg, kIgnoreTargetGUID)
	]]
	return	false
end
function PROJECTILE_TWOACTION_ARROW_OnAnimationEvent(kProjectile)

	return	true
end

function PROJECTILE_TWOACTION_ARROW_CreateLastArrow(actor,action, kPos, iArrowCnt, fRangeDeg, kIgnoreTargetGUID)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	if(iArrowCnt == 0) then iArrowCnt = 1 end
	local strGUID = ""
	if(nil ~= kIgnoreTargetGUID) then 
		strGUID = kIgnoreTargetGUID:GetString()
	end
--	local	kProjectileMan = GetProjectileMan()
	
	-- 회전축 구하기
	local kRotateAxis = Point3(0,0,1)
	local fRotateAngle = (fRangeDeg/iArrowCnt)*DEGTORAD
		
	
	local	iAttackRange = action:GetSkillRange(0,actor) 
	if(0 == iAttackRange) then
		iAttackRange = 144-30
	elseif(30 < iAttackRange) then
		iAttackRange = iAttackRange-30
	end
	
	local fSpeed = 200
	local fAccel = 10
	if(1 == (iArrowCnt%2)) then -- 홀수이면 전방으로 나가는것 하나
		local	kArrow = PROJECTILE_TWOACTION_ARROW_LoadToWeapon(actor,action)
		kArrow:SetWorldPos(kPos)
		local	kFireTarget = actor:GetLookingDir()
		kFireTarget:Multiply(iAttackRange)
		kFireTarget:Add(kPos)
		kArrow:SetTargetLoc(kFireTarget)
		kArrow:SetParamValue("IGNORE_GUID",strGUID)
		kArrow:SetCollisionCheckSec(0)
--		kArrow:SetParam(fSpeed,fAccel); 		 --속력, 가속력, 중력
		kArrow:Fire(true)
	end
	local	i = 1
	local iTemp =1
	while i<iArrowCnt do
		
		local	kArrow = PROJECTILE_TWOACTION_ARROW_LoadToWeapon(actor,action)
		kArrow:SetWorldPos(kPos)
		local	kFireTarget = actor:GetLookingDir()
		kFireTarget:Multiply(iAttackRange)
		kFireTarget:Rotate(kRotateAxis,fRotateAngle*(-iTemp))				
		kFireTarget:Add(kPos)
		kArrow:SetTargetLoc(kFireTarget)	
		kArrow:SetParamValue("IGNORE_GUID",strGUID)
		kArrow:SetCollisionCheckSec(0)
	--	kArrow:SetParam(fSpeed,fAccel); 		 --속력, 가속력, 중력
		kArrow:Fire(true)--	발사!
		
		kArrow = PROJECTILE_TWOACTION_ARROW_LoadToWeapon(actor,action)
		kArrow:SetWorldPos(kPos)
		kFireTarget = actor:GetLookingDir()
		kFireTarget:Multiply(iAttackRange)
		kFireTarget:Rotate(kRotateAxis,fRotateAngle*(iTemp))
		kFireTarget:Add(kPos)
		kArrow:SetTargetLoc(kFireTarget)
		kArrow:SetParamValue("IGNORE_GUID",strGUID)
		kArrow:SetCollisionCheckSec(0)
	--	kArrow:SetParam(fSpeed,fAccel); 		 --속력, 가속력, 중력
		kArrow:Fire(true)--	발사!
		
		iTemp=iTemp+1
		i=i+2
	end	
end


function PROJECTILE_TWOACTION_ARROW_LoadToWeapon(actor,action)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local	kProjectileMan = GetProjectileMan()
	local	kProjectileID = "PROJECTILE_TWOACTION_LAST_ARROW"--이거 이름 스크립트에서 얻어오게할것 --action:GetScriptParam("PROJECTILE_ID")
	
	if kProjectileID == "" then
		kProjectileID = actor:GetEquippedWeaponProjectileID()
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid())
	local	kTargetNodeID = action:GetScriptParam("ATTACH_NODE_ID")
	
	kNewArrow:LoadToHelper(actor,kTargetNodeID)	--	장전
	return	kNewArrow
end