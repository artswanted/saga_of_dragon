
function Effect_ChainLightning_Begin(actor, iEffectID, iEffectKey)
	
	local	kEffectDef = GetEffectDef(iEffectID)
	if kEffectDef:IsNil() then
		return
	end
	
	local TargetActor = g_pilotMan:FindActorByClassNo( kEffectDef:GetAbil(AT_CLASS) )
	if false == TargetActor:IsNil() then
		actor:SetStatusEffectParam(iEffectID, "TARGET_GUID", TargetActor:GetPilotGuid():GetString())
	end

	local ActionGuid = actor:CreateTempActionByActionNo( kEffectDef:GetAbil(AT_MON_SKILL_01) )
	if false == ActionGuid:IsNil() then
		actor:SetStatusEffectParam(iEffectID, "ACTION_GUID", ActionGuid:GetString())
	end
	
	local action = actor:GetTempAction(ActionGuid)
	if action:IsNil() then
		return
	end

	action:SetParam(22, TargetActor:GetPilotGuid():GetString())

	local kActionResult = NewActionResultVector()
	local kResult = kActionResult:GetResult(TargetActor:GetPilotGuid(), false)
	action:AddTarget(TargetActor:GetPilotGuid(), TargetActor:GetHitObjectABVIndex(), kResult)
	DeleteActionResultVector(kActionResult)
	
	local 	kLinkEffect = actor:GetStatusEffectParam(iEffectID, "LINK_EFFECT_ID")
	if kLinkEffect:IsNil() then
		kLinkEffect = DOMINATION_BLUE_LIGHTNING_PATH
	else
		kLinkEffect = kLinkEffect:GetStr()
	end

	action:SetParamInt(2,-1)    --  Plane 새로 생성
	Skill_ChainLightning_CreateLinkedPlane(actor, action, kLinkEffect)
	local   iGroupNum = action:GetParamInt(2);
	if iGroupNum ~= -1 then
		local   kMan = GetLinkedPlaneGroupMan();
		kGroup = kMan:GetLinkedPlaneGroup(iGroupNum);
		kGroup:SetTextureCoordinateUpdateDelay(0.022)
	end

	local 	kStartNode = actor:GetStatusEffectParam(iEffectID, "FIRE_START_NODE")
	if kStartNode:IsNil() then
		kStartNode = "p_ef_star"
	else
		kStartNode = kStartNode:GetStr()
	end

	local 	kStartEffect = actor:GetStatusEffectParam(iEffectID, "START_EFFECT_ID")
	if false == kStartEffect:IsNil() then
		actor:AttachParticle(689653, kStartNode, kStartEffect:GetStr())	--시작 이펙트 붙이기
	end

end


function Effect_ChainLightning_End(actor, iEffectID, iEffectKey)

	local ActionGuid = GUID(actor:GetStatusEffectParam(iEffectID, "ACTION_GUID"):GetStr())
	local action = actor:GetTempAction(ActionGuid)
	if action:IsNil() then
		return
	end

	local   iGroupNum = action:GetParamInt(2);
	if iGroupNum ~= -1 then
		local   kMan = GetLinkedPlaneGroupMan();
		kMan:ReleaseLinkedPlaneGroup(iGroupNum);
		action:SetParamInt(2,-1);
		action:ClearTargetList();
	end

	-- 노드에 붙인 이펙트 제거
	actor:DetachFrom(689653)

	local TargetGuid = GUID(actor:GetStatusEffectParam(iEffectID, "TARGET_GUID"):GetStr())
	local TargetActor = g_pilotMan:FindPilot( TargetGuid ):GetActor()
	if false == TargetActor:IsNil() then
		TargetActor:DetachFrom(689654)
	end

	--임시 액션 삭제
	if false == ActionGuid:IsNil() then
		actor:RemoveTempAction(ActionGuid)
	end
	
end


function Effect_ChainLightning_OnTick(actor, iEffectID, fAccumTime)

	local	kEffectDef = GetEffectDef(iEffectID)
	if kEffectDef:IsNil() then
		return
	end

	local TargetGuid = GUID(actor:GetStatusEffectParam(iEffectID, "TARGET_GUID"):GetStr())
	local ActionGuid = GUID(actor:GetStatusEffectParam(iEffectID, "ACTION_GUID"):GetStr())
	if TargetGuid:IsNil() or ActionGuid:IsNil() then
		return
	end	

	local TargetActor = g_pilotMan:FindPilot( TargetGuid ):GetActor()
	if TargetActor:IsNil() then
		return
	end

	local action = actor:GetTempAction(ActionGuid)
	if action:IsNil() then
		return
	end

	local 	kStartNode = actor:GetStatusEffectParam(iEffectID, "FIRE_START_NODE")
	if kStartNode:IsNil() then
		kStartNode = "p_ef_star"
	else
		kStartNode = kStartNode:GetStr()
	end
	
	local 	kEndNode = actor:GetStatusEffectParam(iEffectID, "FIRE_END_NODE")
	if kEndNode:IsNil() then
		kEndNode = "p_ef_star"
	else
		kEndNode = kEndNode:GetStr()
	end

	local 	kEndEffect = actor:GetStatusEffectParam(iEffectID, "END_EFFECT_ID")
	if false == kEndEffect:IsNil() then
		-- 타겟에게 이펙트 붙이기
		if false == TargetActor:IsAttachParticleSlot(689654) then
			TargetActor:AttachParticle(689654, kEndNode, kEndEffect:GetStr())
		end
	end
	
	

	Skill_ChainLightning_UpdatePlanePos(actor, action, kStartNode, kEndNode)
	
end
