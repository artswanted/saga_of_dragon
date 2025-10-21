
-- 메이지 (인스탄스 힐 : a_Instance Heal) : Level 1-5
function Skill_Begin107000401(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire107000401(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult107000401(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire107000401....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local iMin = caster:GetAbil(AT_C_MAGIC_ATTACK_MIN)
	local iPower = iMin + RAND(caster:GetAbil(AT_C_MAGIC_ATTACK_MAX) - iMin + 1)
	local iSkillPower = kSkillDef:GetAbil(AT_MAGIC_ATTACK)
	local iSkillPRate = kSkillDef:GetAbil(AT_MAGIC_DMG_PER)
	if iSkillPRate == 0 then
		iSkillPRate = ABILITY_RATE_VALUE
	end
	iPower = iPower * iSkillPRate / ABILITY_RATE_VALUE + iSkillPower
	iPower = iPower * kSkillDef:GetAbil(AT_R_MAGIC_ATTACK) / ABILITY_RATE_VALUE
	InfoLog(9, "Skill_Fire107000401 HealPower=" .. iPower)
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	local kUnitArray = NewUNIT_PTR_ARRAY()
	local iDetectRange = kSkillDef:GetAbil(AT_ATTACK_RANGE)
	InfoLog(9, "Skill_Fire107000301 DetectRange:" .. iDetectRange)
	kGround:GetUnitInRange(caster:GetPos(), iDetectRange, caster:GetUnitType(), kUnitArray)
	local iIndex = 0
	local target = kUnitArray:GetUnit(iIndex)
	while target:IsNil() == false do
		InfoLog(9, "Skill_Fire107000301 iIndex:" .. iIndex)
		if caster:IsEnemy(target, false) == false then	-- Enemy가 아닐때만...
			local aresult = result:GetResult(target:GetGuid(), true)
			local iHP = target:GetAbil(AT_HP)
			local iMax = target:GetAbil(AT_C_MAX_HP)
			local iNewHP = math.min(iMax, iHP+iPower)
			if iHP ~= iNewHP then
				target:SetAbil(AT_HP, iNewHP, false, false)
			end
			
			InfoLog(9, "Skill_Fire107000301 60 iIndex:" .. iIndex)
			local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
			kPacket:PushGuid(target:GetGuid())
			kPacket:PushShort(AT_HP)
			kPacket:PushInt(iNewHP)
			kPacket:PushGuid(caster:GetGuid())
			kPacket:PushInt(skillnum)
			target:Send(kPacket, E_SENDTYPE_BROADALL)
			DeletePacket(kPacket)
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)			
	end
	
	return iIndex
end

function Skill_Fail107000401(caster, kTargetArray, skillnum, result, arg)
	return false
end
