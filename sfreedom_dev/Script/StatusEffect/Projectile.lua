function Effect_Projectile_Begin(actor, iEffectID, iEffectKey)

	local TargetId = actor:GetStatusEffectParam(iEffectID, "PROJECTILE_TARGET_ID")
	if false == TargetId:IsNil() then
		TargetId = tonumber(TargetId:GetStr())
	end
	
	local kTargetActor = g_pilotMan:FindActorByClassNo(TargetId)
	if false == kTargetActor:IsNil() then
		actor:SetStatusEffectParam(iEffectID, "TARGET_GUID", kTargetActor:GetPilotGuid():GetString())
	end
	
	local ActionNo = tonumber(actor:GetStatusEffectParam(iEffectID, "PROJECTILE_ACTION_ID"):GetStr())
	local kActionGuid = actor:CreateTempActionByActionNo(ActionNo)
	if false == kActionGuid:IsNil() then
		actor:SetStatusEffectParam(iEffectID, "ACTION_GUID", kActionGuid:GetString())
	end
	
end


function Effect_Projectile_End(actor, iEffectID, iEffectKey)

	local ActionGuid = GUID(actor:GetStatusEffectParam(iEffectID, "ACTION_GUID"):GetStr())
	if false == ActionGuid:IsNil() then
		actor:RemoveTempAction(ActionGuid)
	end
	
end


function Effect_Projectile_OnTick(actor, iEffectID, fAccumTime)

	local ActionNo = tonumber(actor:GetStatusEffectParam(iEffectID, "PROJECTILE_ACTION_ID"):GetStr())
	local ProjectileId = actor:GetStatusEffectParam(iEffectID, "PROJECTILE_ID")
	local TargetGuid = GUID(actor:GetStatusEffectParam(iEffectID, "TARGET_GUID"):GetStr())
	local ActionGuid = GUID(actor:GetStatusEffectParam(iEffectID, "ACTION_GUID"):GetStr())
	if ProjectileId:IsNil() or TargetGuid:IsNil() or ActionGuid:IsNil() then
		return
	end	

	local kSkillDef = GetSkillDef(ActionNo)
	if kSkillDef:IsNil() then
		return
	end

	local TargetActor = g_pilotMan:FindPilot( TargetGuid ):GetActor()
	if TargetActor:IsNil() then
		return
	end
	
	local Action = actor:GetTempAction(ActionGuid)
	if Action:IsNil() then
		return
	end
	
	local kProjectileMan = GetProjectileMan()
	local	kNewArrow = kProjectileMan:CreateNewProjectile(ProjectileId:GetStr(), Action, actor:GetPilotGuid())
	if kNewArrow:IsNil() then
		return
	end
	
	local iPentrationCount = kSkillDef:GetAbil(AT_PENETRATION_COUNT);
	kNewArrow:SetPenetrationCount(iPentrationCount);-- 몇번을 관통할것인지 세팅
	if iPentrationCount ~= 0 then
		kNewArrow:SetMultipleAttack(true);
	end

	local 	kTargetNodeID = actor:GetStatusEffectParam(iEffectID, "FIRE_START_NODE")
	if kTargetNodeID:IsNil() then
		kTargetNodeID = "p_ef_mouth"
	else
		kTargetNodeID = kTargetNodeID:GetStr()
	end
	
	local 	kEndNodeID = actor:GetStatusEffectParam(iEffectID, "TARGET_END_NODE")
	if false == kEndNodeID:IsNil() then
		kNewArrow:SetTargetEndNode(kEndNodeID:GetStr())
	end

	kNewArrow:LoadToHelper(actor,kTargetNodeID)	--	장전
	kNewArrow:SetTargetGuidFromServer(TargetGuid)
	kNewArrow:Fire(true)
	
end
