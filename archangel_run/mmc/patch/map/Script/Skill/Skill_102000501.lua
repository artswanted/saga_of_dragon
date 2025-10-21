
-- Magician (힐은 셀프! : Self Heal) : Level 1-5
function Skill_Begin102000501(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire102000501(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult102000501(caster, kTargetArray, skillnum, result)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(4, "Skill_Fire102000501....SkillDef is NIl" ..skillnum)
		return -1
	end

	local iMaxMP = caster:GetAbil(AT_C_MAX_MP);
	local iDecPer = kSkillDef:GetAbil(AT_PERCENTAGE);
	local iDecMP = iMaxMP * iDecPer / ABILITY_RATE_VALUE;
	local iNowMP = caster:GetAbil(AT_MP);

	InfoLog(4, "Skill_Fire102000501....iMaxMP : "..iMaxMP.." iDecPer : "..iDecPer.." iDecMP : "..iDecMP.." iNowMP : "..iNowMP)
	if iNowMP < iDecMP then
		return 1
	end

	--caster:SetMP(iNowMP - iDecMP, E_SENDTYPE_BROADALL, caster:GetGuid())
	caster:SetAbil(AT_MP, iNowMP - iDecMP, true, false)

			
	local iAdd = kSkillDef:GetAbil(AT_MAGIC_ATTACK)
	local iHP = caster:GetAbil(AT_HP)
	local iMaxHP = caster:GetAbil(AT_C_MAX_HP)
	local iMagicAttack = caster:GetAbil(AT_C_MAGIC_ATTACK)
	local iNewHP = math.min(iMaxHP, iHP+iAdd + iMagicAttack)
	--InfoLog(9, "Skill_Fire102000501 Add:"..iAdd.."MagicAttack:"..iMagicAttack.."   NewHP:" ..iNewHP)
	if iNewHP == iHP then
		return 1
	end

	local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
	kPacket:PushGuid(caster:GetGuid())
	kPacket:PushShort(AT_HP)
	kPacket:PushInt(iNewHP)
	kPacket:PushGuid(caster:GetGuid())
	kPacket:PushInt(skillnum)
	caster:Send(kPacket, E_SENDTYPE_BROADALL)
	DeletePacket(kPacket)		

	caster:SetAbil(AT_HP, iNewHP, true)
	return 1
end

function Skill_Fail102000501(caster, kTargetArray, skillnum, result, arg)
	return false
end
