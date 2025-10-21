-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function PROJECTILE_TWOACTION_LAST_ARROW_OnLoadingStart(kProjectile)

	return	true;
end
function PROJECTILE_TWOACTION_LAST_ARROW_OnLoading(kProjectile)

	return	true;
end
function PROJECTILE_TWOACTION_LAST_ARROW_OnFlyingStart(kProjectile)

	return	true;
end
function PROJECTILE_TWOACTION_LAST_ARROW_OnFlying(kProjectile)

	return	true;
end

function PROJECTILE_TWOACTION_LAST_ARROW_OnCollision(kProjectile,kCollideActor,kTargets)
	ODS("PROJECTILE_TWOACTION_LAST_ARROW_OnCollision\n")
	
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

		ODS("iPentrationCount : "..iPentrationCount.."\n")
	
	local	kAction = kParentActor:GetAction();
	if kAction:IsNil() then
		return	0
	end
	ODS("들어옴 : ", false, 912)
	
	local strGUID = kProjectile:GetParamValue("IGNORE_GUID")
	--[[
	-- 충돌 체크 하지 않아야 할 타겟이 이라면
	
	if(nil ~= strGUID) then
		local kTargetGUID = kCollideActor:GetPilotGuid()
		if(kTargetGUID:GetString() == strGUID) then
			ODS("충돌 체크 하지 않음 GUID:"..strGUID.."\n",false, 912)
			return 0	-- 충돌 체크 하지 않는다
		end
	end
	ODS("충돌체크 됨\n",false, 912)
	--]]
	
	local	kPos = kCollideActor:GetHitABVCenterPos();
	local	fRange = 30;
	
	kParam = FindTargetParam();
	kParam:SetParam_1(kPos,kParentActor:GetLookingDir());
	kParam:SetParam_2(0,0,fRange,0);
	kParam:SetParam_3(true,FTO_NORMAL);

	if kParentActor:IsUnderMyControl() then
		kParentPilot = GetMyActor():GetPilot();
	end
	kAction:FindTargetsEx2(kProjectile:GetParentActionNo(),TAT_SPHERE,kParam,kTargets,kTargets,kParentPilot);
	
	local i = 0;
	local kTargetList = kTargets --kAction:GetTargetList();
	local iTargetCount = kTargetList:size();
	while i<iTargetCount do
		local   kTargetInfo = kTargetList:GetTargetInfo(i);
		local   kTargetGUID = kTargetInfo:GetTargetGUID();
		if(kTargetGUID:GetString() == strGUID) then
			ODS("충돌 체크 하지 않음 GUID:"..strGUID,false, 912)			
			kTargetList:DeleteTargetInfo(GUID(strGUID))
			break
		end		
		i = i + 1;
	end
	ODS("\n타겟갯수:"..kTargetList:size().."\n",false, 912)
	
	return kTargetList:size()
end

function PROJECTILE_TWOACTION_LAST_ARROW_OnTargetListModified(kProjectile)

	return SkillFunc_ProjectileHitOneTime(kProjectile)

end


function PROJECTILE_TWOACTION_LAST_ARROW_OnArrivedAtTarget(kProjectile)	
	PROJECTILE_TWOACTION_LAST_ARROW_OnTargetListModified(kProjectile);	

	return	false;
end
function PROJECTILE_TWOACTION_LAST_ARROW_OnAnimationEvent(kProjectile)

	return	true
end
