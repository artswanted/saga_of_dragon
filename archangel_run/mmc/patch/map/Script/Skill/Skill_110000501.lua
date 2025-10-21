-- 함수 설명 ----------------------
--	Skill_PassiveXXXX	: Passive skill이 초기에 생성될 때 호출되는 함수
--	Skill_PCheckXXXX : 장비확인용, 확인이 필요할 때 호출되는 함수 (필요한 Skill만 구현하면 된다)
--		Passive 적용되어 야 할때 / Passive 해제되어야 할 때 모두 구현해야 한다.
--	Skill_BeginXXXX : Skill_PCheckXXXX가 구현되면 함께 구현되어야 한다.
--	Skill_EndXXXX : Skill_PCheckXXXX가 구현되면 함께 구현되어야 한다.

-- 사냥꾼 (어드밴스드 보우 마스터리 : a_Adv Bow Mastery) : Level 1-10 PASSIVE
function Skill_Passive110000501(unit, skillno, actarg)
	if unit:CheckPassive(skillno, actarg) == false then
		return -1
	end
	
	return Skill_PCheck110000501(unit, skillno, actarg)
end

function Skill_PCheck110000501(caster, skillno, actarg)
	local bOn = false
	local kSkillDef = GetSkillDef(skillno)
	if kSkillDef:IsNil() == true then
		InfoLog(5, "Skill_PCheck110000501 : Cannot Get SkillDef:" .. skillno)
		return -1
	end
	local iEffectNo = kSkillDef:GetEffectNo()
	local kEffectMgr = caster:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, true)
	if kEffect:IsNil() == false then
		bOn = true	-- 이미 존재하는 Effect 이다.
	end
	
	if caster:CheckPassive(skillno, actarg) == false then
		if bOn == true then
			Skill_End110000501(caster, kSkillDef, actarg)
		end
		return -1
	end
	
	if bOn == false then
		Skill_Begin110000501(caster, kSkillDef, actarg)
	end

	return 0
end

function Skill_Begin110000501(caster, kSkillDef, actarg)
	local iEffectNo = kSkillDef:GetEffectNo()
	local kEffect = caster:AddEffect(iEffectNo, 0, actarg, caster)
	if kEffect:IsNil() == false then
		kEffect:SetActArgInt(ACTARG_WEAPONLIMIT, kSkillDef:GetAbil(AT_WEAPON_LIMIT))
	end
end

function Skill_End110000501(caster, kSkillDef, actarg)
	local iEffectNo = kSkillDef:GetEffectNo()
	caster:DeleteEffect(iEffectNo)
end

