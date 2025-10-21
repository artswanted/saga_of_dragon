
-- 압둘라 스스로 힐하기 : a_mon_melee_02
function Skill_Begin6013904(caster, skillnum, iStatus,arg, TailPacket)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6013904(caster, kTargetArray, skillnum, result, actarg)
	-- GetSkillResult6013904(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire6013904....SkillDef is NIl : " ..skillnum)
		return -1
	end
	local iAdd = kSkillDef:GetAbil(AT_HP)
	local iHP = caster:GetAbil(AT_HP)
	local iMaxHP = caster:GetAbil(AT_C_MAX_HP)
	local iNewHP = math.min(iMaxHP, iHP+iAdd)
	if iNewHP ~= iHP then
		local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
		kPacket:PushGuid(caster:GetGuid())
		kPacket:PushShort(AT_HP)
		kPacket:PushInt(iNewHP)
		kPacket:PushGuid(caster:GetGuid())
		kPacket:PushInt(skillnum)
		caster:Send(kPacket, E_SENDTYPE_BROADALL)
		DeletePacket(kPacket)	

		caster:SetHP(iNewHP, E_SENDTYPE_NONE, caster:GetGuid())
	end
	return iIndex
end

function Skill_Fail6013904(caster, kTargetArray, skillnum, result, arg)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		caster:SyncRandom(ABILITY_RATE_VALUE)
		caster:SyncRandom(ABILITY_RATE_VALUE)
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return false
end


function Skill_CanReserve6013904(caster, kTargetArray, skillnum, actarg)
	InfoLog(9, "Skill_CanReserve6013904 --")
	local iHP = caster:GetAbil(AT_HP)
	local iMaxHP = caster:GetAbil(AT_C_MAX_HP)
	if iMaxHP < iHP * 2 then	-- HP가 50% 이하일 때만 허용하자.
		InfoLog(9, "Skill_CanReserve6013904 return false")
		return false
	end

	return true
end

