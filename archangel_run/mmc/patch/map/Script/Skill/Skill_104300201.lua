
-- Thief (스트립 웨폰 : a_Strip Weapon) : Level 1-5
function Skill_Begin104300201(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire104300201(caster, kTargetArray, skillnum, result, arg)
	--GetSkillResult104300201(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(8, "Skill_Fire102000201....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	local iPercent = kSkillDef:GetAbil(AT_PERCENTAGE)
	local iIndex = 0
	local target = kTargetArray:GetUnit(iIndex)
	while target:IsNil() == false do
		local aresult = result:GetResult(target:GetGuid(), true)
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			if target:IsUnitType(UT_PLAYER) == true then
				if iPercent > RAND(ABILITY_RATE_VALUE) then
					-- Player이면 Weapon을 다시 인벤에 넣어 버린다.
					target:UnEquipItem(EQUIP_POS_WEAPON, kGround)
				else
					aresult:SetMissed(true)
				end
			else
				if iPercent * 2 > RAND(ABILITY_RATE_VALUE) then	-- 확률은 두배
				-- Monster라면 공격력을 20% 줄어든다.
					target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
				else
					aresult:SetMissed(true)
				end
			end
		end
		
		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	return iIndex
end

function Skill_Fail104300201(caster, kTargetArray, skillnum, result, arg)
	return false
end
