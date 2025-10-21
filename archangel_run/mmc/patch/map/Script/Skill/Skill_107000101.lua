
-- 메이지 (엄마손은 약손 : Heal) : Level 1-10
function Skill_Begin107000101(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire107000101(caster, kTargetArray, skillnum, result, arg)
	--InfoLog(9, "Skill_Fire107000101 --")
	--GetSkillResult107000101(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire107000101....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iMagicPower = caster:GetAbil(AT_C_MAGIC_ATTACK)
	--InfoLog(9, "Skill_Fire107000101 MagicPower:" .. iMagicPower)
	local iHeal = iMagicPower * GET_RATE_VALUE(kSkillDef:GetAbil(AT_MAGIC_DMG_PER)) / 100
	--InfoLog(9, "Skill_Fire107000101 iHeal:" .. iHeal)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local iMaxHP = target:GetAbil(AT_C_MAX_HP)
		local iHP = target:GetAbil(AT_HP)
		local iNewHP = math.floor(math.min(iMaxHP, iHP+iHeal))
		target:SetAbil(AT_HP, iNewHP)
		
		local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
		kPacket:PushGuid(target:GetGuid())
		kPacket:PushShort(AT_HP)
		kPacket:PushInt(iNewHP)
		kPacket:PushGuid(caster:GetGuid())
		kPacket:PushInt(skillnum)
		target:Send(kPacket, E_SENDTYPE_BROADALL)
		DeletePacket(kPacket)
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	return iIndex
end

function Skill_Fail107000101(caster, kTargetArray, skillnum, result, arg)
	return false
end
