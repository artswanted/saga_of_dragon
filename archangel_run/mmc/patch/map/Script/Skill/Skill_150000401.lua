-- 함수 설명 ----------------------
--	Skill_PassiveXXXX	: Passive skill이 초기에 생성될 때 호출되는 함수
--	Skill_PCheckXXXX : 장비확인용, 확인이 필요할 때 호출되는 함수 (필요한 Skill만 구현하면 된다)
--		Passive 적용되어 야 할때 / Passive 해제되어야 할 때 모두 구현해야 한다.
--	Skill_BeginXXXX : Skill_PCheckXXXX가 구현되면 함께 구현되어야 한다.
--	Skill_EndXXXX : Skill_PCheckXXXX가 구현되면 함께 구현되어야 한다.

-- 저격수 (관통 : a_Thrust) : Level 1-10 PASSIVE
function Skill_Passive150000401(unit, skillno, actarg)
	if unit:CheckPassive(skillno, actarg) == false then
		return -1
	end
	--Skill_CS_Passive150000401(unit, iSkillID)
	
	return Skill_PCheck150000401(unit, skillno, actarg)
end

function Skill_PCheck150000401(caster, skillno, actarg)
	local bOn = false
	local kSkillDef = GetSkillDef(skillno)
	if kSkillDef:IsNil() == true then
		InfoLog(5, "Skill_PCheck150000401 : Cannot Get SkillDef:" .. skillno)
		return -1
	end
	local iEffectNo = kSkillDef:GetEffectNo()
	local kEffectMgr = caster:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, true)
	if kEffect:IsNil() == false then
		InfoLog(9, "Skill_PCheck150000401 11")
		bOn = true	-- 이미 존재하는 Effect 이다.
	end
	if caster:CheckPassive(skillno, actarg) == false then
		if bOn == true then
			Skill_End150000401(caster, kSkillDef, actarg)
		end
		return -1
	end

	-- 무기 착용 제한은 CheckPassive에서 이미 했다.
	if bOn == false	then
		Skill_Begin150000401(caster, kSkillDef, actarg)
	end
	return 0
end

function Skill_Begin150000401(caster, kSkillDef, actarg)
	--InfoLog(9, "Skill_Begin150000401 --")
	local iEffectNo = kSkillDef:GetEffectNo()
	
	local iAdd = kSkillDef:GetAbil(AT_PHY_ATTACK)
	local kEffect = caster:AddEffect(kSkillDef:GetEffectNo(), iAdd, actarg, caster)
	if kEffect:IsNil() == false then
		kEffect:SetActArgInt(ACTARG_WEAPONLIMIT, kSkillDef:GetAbil(AT_WEAPON_LIMIT))
	end
end

function Skill_End150000401(caster, kSkillDef, actarg)
	--InfoLog(9, "Skill_End150000401 --")
	local iEffectNo = kSkillDef:GetEffectNo()
	caster:DeleteEffect(iEffectNo)
end

