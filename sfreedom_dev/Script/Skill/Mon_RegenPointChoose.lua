-- skill

function Act_Mon_RegenPointChoose_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Act_Mon_RegenPointChoose_SetState(actor,action,kState)
	return	Act_Mon_Melee_SetState(actor,action,kState)
end

function Act_Mon_RegenPointChoose_OnEnter(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("Act_Mon_RegenPointChoose_OnEnter : " .. actor:GetAniSequenceID().. "\n",false,6482)
	local Effect = actor:GetAnimationInfoFromAniName("CASTING_EFFECT",action:GetSlotAnimName(0)):GetStr()
	if "" == Effect then
		Effect = "e_b_palis_skill_03_CylinderBeem"
	end
	
	local EffectScale = actor:GetAnimationInfoFromAniName("CASTING_EFFECT_SCALE",action:GetSlotAnimName(0)):GetStr()
	if ""==EffectScale then
		EffectScale = 1
	else
		EffectScale = tonumber(EffectScale)
	end
	
	if nil~=Effect then
		if action:GetActionParam() ~= AP_FIRE then
			local	ptTargetPos = 0;
			local	kPacket = action:GetParamAsPacket();
			if kPacket == nil or kPacket:IsNil() then
				ODS("[SkillError] Act_Mon_RegenPointChoose_OnEnter, kPacket is not valid. actor : "..actor:GetPilotGuid():GetString().." action:"..action:GetID().."\n")
				return	false;
			else
				local iTargetNum = kPacket:PopInt()
				for i=0, iTargetNum do
					ptTargetPos = kPacket:PopPoint3()
					if ptTargetPos:IsZero() then
						break
					end
					
					ODS("X: "..ptTargetPos:GetX()..", Y: "..ptTargetPos:GetY()..", Z: "..ptTargetPos:GetZ()..", Name: " .. Effect.."\n",false,6482)
					actor:AttachParticleToPointS(12200+i, ptTargetPos, Effect, EffectScale)
				end
				action:SetParamInt(12200,iTargetNum)
				
				ODS("Act_Mon_RegenPointChoose_OnEnter iTargetNum : ".. iTargetNum .."\n",false,6482)
			end
		end
	end

	return Act_Mon_Melee_OnEnter(actor, action)	
end
function Act_Mon_RegenPointChoose_OnCastingCompleted(actor,action)	
	Act_Mon_Melee_OnCastingCompleted(actor,action)
	Act_Mon_RegenPointDetachEffect_Casting(actor, action)
end

function Act_Mon_RegenPointChoose_Fire(actor,action)
	Act_Mon_Melee_Fire(actor,action)
end

function Act_Mon_RegenPointChoose_OnUpdate(actor, accumTime, frameTime)
	return Act_Mon_Melee_OnUpdate(actor, accumTime, frameTime)
end
function Act_Mon_RegenPointChoose_OnCleanUp(actor, action)	
	Act_Mon_Melee_OnCleanUp(actor, action)
end

function Act_Mon_RegenPointChoose_OnLeave(actor, action)

	Act_Mon_RegenPointDetachEffect_Casting(actor, action)
	
	return	Act_Mon_Melee_OnLeave(actor, action)
end

function Act_Mon_RegenPointChoose_DoDamage(actor,action)
	Act_Mon_Melee_DoDamage(actor,action)
end

function Act_Mon_RegenPointChoose_OnEvent(actor,textKey)
	return	Act_Mon_Melee_OnEvent(actor,textKey)
end

function Act_Mon_RegenPointChoose_OnTargetListModified(actor,action,bIsBefore)
	Act_Mon_Melee_OnTargetListModified(actor,action,bIsBefore)
end

function Act_Mon_RegenPointDetachEffect_Casting(actor,action)
	local	iTargetNum = action:GetParamInt(12200)
	ODS("Act_Mon_RegenPointDetachEffect_Casting : "..iTargetNum.."\n",false,6482)
	iTargetNum = 10
	for i=0, iTargetNum do
		actor:DetachFrom(12200+i)
	end
end