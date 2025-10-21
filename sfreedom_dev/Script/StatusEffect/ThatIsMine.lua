function Effect_Mine_Begin(actor, iEffectID, iEffectKey)
	return
end

function Effect_Mine_End(actor, iEffectID, iEffectKey)
	return
end

function Effect_Mine_OnTick(actor, iEffectID, fAccumTime)
	if actor:GetCallerGuid():IsNil() then return end
	local bEq = actor:GetCallerGuid():IsEqual(g_pilotMan:GetPlayerPilotGuid())
	if bEq then
		--ODS("Effect_Mine_OnTick MyPet\n", false, 1509)
	else
		return
	end
	local	kEffectDef = GetEffectDef(iEffectID);
	if kEffectDef:IsNil() then
		return
	end

	local iSkill = kEffectDef:GetAbil(AT_MON_SKILL_01)
	local kSkillDef = GetSkillDef(iSkill)
	if kSkillDef:IsNil() then
		return
	end

	actor:PickUpNearItem(kSkillDef:GetAbil(AT_ATTACK_RANGE), kSkillDef:GetAbil(AT_MAX_LOOTNUM))
end
