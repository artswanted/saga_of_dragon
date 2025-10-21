-- spawn
DOMINATION_EFFECTNO = 5872100
DOMINATION_DEFALUT_LIGHTNING_PATH = "../Data/5_Effect/9_Tex/Fx_Lightningblue_1x4.dds"
DOMINATION_RED_LIGHTNING_PATH = "../Data/5_Effect/9_Tex/efx_lightning_allinone_red.dds"
DOMINATION_BLUE_LIGHTNING_PATH = "../Data/5_Effect/9_Tex/efx_lightning_allinone_blue.dds"
DOMINATION_RED_HAND_EFFECT = "efx_com_skill_domination_trans_01_red_hand"
DOMINATION_BLUE_HAND_EFFECT = "efx_com_skill_domination_trans_01_blue_hand"
DOMINATION_RED_TARGET_EFFECT = "efx_com_skill_domination_trans_01_red_target"
DOMINATION_BLUE_TARGET_EFFECT = "efx_com_skill_domination_trans_01_blue_target"

DOMINATION_LIGHTNING_START_NODE = "Bip01 R Hand"
DOMINATION_LIGHTNING_END_NODE_1 = "p_ef_head"
DOMINATION_LIGHTNING_END_NODE_2 = "p_ef_heart"

function Skill_Domination_OnCheckCanEnter(actor, action)
	return SkillFunc_OnCheckCanEnter(actor, action)
end

function Skill_Domination_OnEnter(actor, action)
	--ODS("Skill_Domination_OnEnter\n",false,6482)
	if SkillFunc_OnEnter(actor, action)  then
		if action:GetActionParam() ~= AP_CASTING then
			Skill_Domination_OnCastingCompleted(actor, action);

			actor:AttachSound(12784,"domination_trans_01_start")
			local kEffect = actor:GetPilot():GetUnit():GetEffect( DOMINATION_EFFECTNO )
			if false == kEffect:IsNil() then
				local kCasterPilot = g_pilotMan:FindPilot( kEffect:GetCaster() )
				if kCasterPilot:IsNil() == false then
					local	kTargetActor = kCasterPilot:GetActor();
					if kTargetActor:IsNil() == false then
						actor:LookAt(kTargetActor:GetPos(),true,true)
						action:SetParam(22, kEffect:GetCaster():GetString())

						local kActionResult = NewActionResultVector()
						local kResult = kActionResult:GetResult(kEffect:GetCaster(), false)
						action:AddTarget(kEffect:GetCaster(), kTargetActor:GetHitObjectABVIndex(), kResult)
						DeleteActionResultVector(kActionResult)
					end
				end
			end
		end
		return true
	end
	return false
end

function Skill_Domination_OnCastingCompleted(actor, action)
	Skill_ChainLightning_Fire(actor,action)
	actor:SetParam("lightning", "false")
	action:SetParam(20,"")	-- EndNode
	action:SetParamFloat(21,0)
	return SkillFunc_OnCastingCompleted(actor, action)
end

function Skill_Domination_GetTarget(action)
	local kTargetGuid = GUID(action:GetParam(22))
	if nil~=kTargetGuid and false==kTargetGuid:IsNil() then
		return g_pilotMan:FindPilot( kTargetGuid ):GetActor()
	end
	return nil
end

function Skill_Domination_OnUpdate(actor, accumTime, frameTime)
	local kEffect = actor:GetPilot():GetUnit():GetEffect( DOMINATION_EFFECTNO )
	if true == kEffect:IsNil() then
		--ODS("Skill_Domination_OnUpdate Not Effect\n",false,6482)
		return false
	end

	if actor:IsHide() then
		return false
	end

	if "true"==actor:GetParam("lightning") then
		local 	action = actor:GetAction()
		if IsShowTargetLightning(actor, action) then
			local fChangeTime = action:GetParamFloat(21)-frameTime;
			action:SetParamFloat(21,fChangeTime);
			if fChangeTime < 0 then
				action:SetParam(20, "")
			end

			local kEndNode = action:GetParam(20)
			if nil==kEndNode or ""==kEndNode then
				if 1==math.random(0, 1) then
					action:SetParam(20, DOMINATION_LIGHTNING_END_NODE_1)
				else
					action:SetParam(20, DOMINATION_LIGHTNING_END_NODE_2)
				end
				kEndNode = action:GetParam(20)
				action:SetParamFloat(21,0.14);

				-- 가디언에게 이펙트 붙이기
				local kTarget = Skill_Domination_GetTarget(action)
				if nil~=kTarget and false==kTarget:IsNil() then
					kTarget:AttachSound(12785,"domination_trans_01_spell")
					if 1==actor:GetAbil(AT_TEAM) then
						if false == kTarget:IsAttachParticleSlot(789655) then
							kTarget:AttachParticle(789655, DOMINATION_LIGHTNING_END_NODE_2, DOMINATION_RED_TARGET_EFFECT)
						end
					else
						if false == kTarget:IsAttachParticleSlot(789656) then
							kTarget:AttachParticle(789656, DOMINATION_LIGHTNING_END_NODE_2, DOMINATION_BLUE_TARGET_EFFECT)
						end
					end
				end
			end

			Skill_ChainLightning_UpdatePlanePos(actor, action, DOMINATION_LIGHTNING_START_NODE, kEndNode)
		end
	end

	return SkillFunc_OnUpdate(actor, actor:GetAction(), accumTime, frameTime)	
end

function Skill_Domination_ReleasePlane(actor)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local   iGroupNum = actor:GetAction():GetParamInt(2);
	if iGroupNum ~= -1 then
		local   kMan = GetLinkedPlaneGroupMan();
		kMan:ReleaseLinkedPlaneGroup(iGroupNum);
		actor:GetAction():SetParamInt(2,-1);
		actor:GetAction():ClearTargetList();
	end
end

function Skill_Domination_OnCleanUp(actor, action)
	Skill_Domination_ReleasePlane(actor)
	actor:DetachFrom(789654)
	
	local	kCurrentAction = actor:GetAction();
	if nil~=kCurrentAction and false==kCurrentAction:IsNil() then
		local kTarget = Skill_Domination_GetTarget(kCurrentAction)
		if nil~=kTarget and false==kTarget:IsNil() then
			kTarget:DetachFrom(789655)
			kTarget:DetachFrom(789656)
		end
	end
	return SkillFunc_OnCleanUp(actor, action)
end

function Skill_Domination_OnTargetListModified(actor, action, bIsBefore)
	return SkillFunc_OnTargetListModified(actor, action, bIsBefore)
end

function Skill_Domination_OnLeave(actor, action)
	--ODS("Skill_Domination_OnLeave\n",false,6482)
	return SkillFunc_OnLeave(actor, action)
end

function Skill_Domination_OnEvent(actor,textKey)
	if "effpos" == textKey or "hit" == textKey  then
		local action = actor:GetAction()
		if IsShowTargetLightning(actor, action) then
			local kLightingEffect = DOMINATION_BLUE_LIGHTNING_PATH
			local kHandEffect = DOMINATION_BLUE_HAND_EFFECT
			if 1==actor:GetAbil(AT_TEAM) then
				kLightingEffect = DOMINATION_RED_LIGHTNING_PATH
				kHandEffect = DOMINATION_RED_HAND_EFFECT
			end
			Skill_ChainLightning_CreateLinkedPlane(actor, action, kLightingEffect)
			local   iGroupNum = action:GetParamInt(2);
			if iGroupNum ~= -1 then
				local   kMan = GetLinkedPlaneGroupMan();
				kGroup = kMan:GetLinkedPlaneGroup(iGroupNum);
				--ODS("GetTextureCoordinateUpdateDelay: "..kGroup:GetTextureCoordinateUpdateDelay().."\n",false,6482)
				kGroup:SetTextureCoordinateUpdateDelay(0.022)
			end
		
			actor:SetParam("lightning", "true")
			actor:AttachParticle(789654, DOMINATION_LIGHTNING_START_NODE, kHandEffect)	--플레이어 손에 이펙트 붙이기
		end
		return true
	end
	return SkillFunc_OnEvent(actor,textKey)
end

function Skill_Domination_OnFindTarget(actor,action,kTargets)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	--ODS("Skill_Domination_OnFindTarget\n",false,6482)
	return kTargets:size()
end