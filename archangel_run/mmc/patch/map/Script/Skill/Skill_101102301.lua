-- 함수 설명 ----------------------
--	Skill_PassiveXXXX	: Passive skill이 초기에 생성될 때 호출되는 함수
--	Skill_PCheckXXXX : 장비확인용, 확인이 필요할 때 호출되는 함수 (필요한 Skill만 구현하면 된다)
--		Passive 적용되어 야 할때 / Passive 해제되어야 할 때 모두 구현해야 한다.
--	Skill_BeginXXXX : Skill_PCheckXXXX가 구현되면 함께 구현되어야 한다.
--	Skill_EndXXXX : Skill_PCheckXXXX가 구현되면 함께 구현되어야 한다.

-- 전사 다운조작 3타 패시브 스킬
function Skill_Passive101102301(unit, skillno, actarg)
	return 0
end

function Skill_PCheck101102301(caster, skillno, actarg)
	return 0
end

function Skill_Begin101102301(caster, kSkillDef, actarg)
end

function Skill_End101102301(caster, kSkillDef, actarg)
end

