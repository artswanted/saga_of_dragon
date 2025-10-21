
-- 압둘라 단체 힐하기 : a_mon_melee_03
function Skill_Begin6013905(caster, skillnum, iStatus,arg, TailPacket)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6013905(caster, kTargetArray, skillnum, result, actarg)
	-- GetSkillResult6013905(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire6013905....SkillDef is NIl : " ..skillnum)
		return -1
	end

	local iAdd = kSkillDef:GetAbil(AT_HP)
	InfoLog(9, "Skill_Fire6013905 HP Add:" .. iAdd)
	
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		InfoLog(9, "Skill_Fire6013905 HP 20")
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false then
			InfoLog(9, "Skill_Fire6013905 HP 30")
			local iHP = target:GetAbil(AT_HP)
			local iMaxHP = target:GetAbil(AT_C_MAX_HP)
			local iNewHP = math.min(iMaxHP, iHP+iAdd)
			InfoLog(9, "Skill_Fire6013905 HP iHP:" .. iHP .. ", NewHP:" .. iNewHP)
			if iNewHP ~= iHP then
				InfoLog(9, "Skill_Fire6013905 HP 40")
				local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
				kPacket:PushGuid(target:GetGuid())
				kPacket:PushShort(AT_HP)
				kPacket:PushInt(iNewHP)
				kPacket:PushGuid(caster:GetGuid())
				kPacket:PushInt(skillnum)
				target:Send(kPacket, E_SENDTYPE_BROADALL)
				DeletePacket(kPacket)	
				target:SetHP(iNewHP, E_SENDTYPE_NONE, caster:GetGuid())
			end
		end
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	InfoLog(9, "Skill_Fire6013905 HP 99")
	return iIndex
end

function Skill_Fail6013905(caster, kTargetArray, skillnum, result, arg)
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


function Skill_CanReserve6013905(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

