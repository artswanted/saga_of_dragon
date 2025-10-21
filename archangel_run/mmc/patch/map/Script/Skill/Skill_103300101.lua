-- 함수 설명 ----------------------
--	Skill_PassiveXXXX	: Passive skill이 초기에 생성될 때 호출되는 함수
--	Skill_PCheckXXXX : 장비확인용, 확인이 필요할 때 호출되는 함수 (필요한 Skill만 구현하면 된다)
--		Passive 적용되어 야 할때 / Passive 해제되어야 할 때 모두 구현해야 한다.
--	Skill_BeginXXXX : Skill_PCheckXXXX가 구현되면 함께 구현되어야 한다.
--	Skill_EndXXXX : Skill_PCheckXXXX가 구현되면 함께 구현되어야 한다.

--  궁수 공통(투사무기 숙력 : Bow Mastery) : Level 1-10 PASSIVE
function Skill_Passive103300101(unit, skillno, actarg)
	if unit:CheckPassive(skillno, actarg) == false then
		InfoLog(9, "Skill_Passive103300101 19")
		return -1
	end
	
	return Skill_PCheck103300101(unit, skillno, actarg)
end

function Skill_PCheck103300101(caster, skillno, actarg)
	InfoLog(9, "Skill_PCheck103300101 --")
	local bOn = false
	local kSkillDef = GetSkillDef(skillno)
	if kSkillDef:IsNil() == true then
		InfoLog(5, "Skill_PCheck103300101 : Cannot Get SkillDef:" .. skillno)
		return -1
	end
	local iEffectNo = kSkillDef:GetEffectNo()
	local kEffectMgr = caster:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, true)
	if kEffect:IsNil() == false then
		InfoLog(9, "Skill_PCheck103300101 bOn true")
		bOn = true	-- 이미 존재하는 Effect 이다.
	end
	
	if caster:CheckPassive(skillno, actarg) == false then
		if bOn == true then
			Skill_End103300101(caster, kSkillDef, actarg)
		end
		return -1
	end
	
	-- 무기 착용 제한은 CheckPassive에서 이미 했다.
	Skill_Begin103300101(caster, kSkillDef, actarg)
	return 0
end

function Skill_Begin103300101(caster, kSkillDef, actarg)
	local iEffectNo = kSkillDef:GetEffectNo()
	local kEffect = caster:AddEffect(iEffectNo, 0, actarg, caster)
	if kEffect:IsNil() == false then
		kEffect:SetActArgInt(ACTARG_WEAPONLIMIT, kSkillDef:GetAbil(AT_WEAPON_LIMIT))
	end
end

function Skill_End103300101(caster, kSkillDef, actarg)
	local iEffectNo = kSkillDef:GetEffectNo()
	caster:DeleteEffect(iEffectNo)
end

