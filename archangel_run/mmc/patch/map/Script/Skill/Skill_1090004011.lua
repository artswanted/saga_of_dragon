-- 메이지 (콰그마이어 효과 : a_Quagmire_Fire) : Level 1-10
function Skill_Begin1090004011(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090004011(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult1090004011(caster, kTargetArray, skillnum, result)

	--InfoLog(9, "Skill_Fire1090004011 --")
	local kSkillDef = GetSkillDef(skillnum)
	--InfoLog(9, "Skill_Fire1090004011 10")
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire1090004011....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	--InfoLog(9, "Skill_Fire1090004011 20")
	local iEffectPer = kSkillDef:GetAbil(AT_PERCENTAGE)
	--InfoLog(9, "Skill_Fire1090004011 21")
	local iIndex = 0
	--InfoLog(9, "Skill_Fire1090004011 22")
	local target = kTargetArray:GetUnit(iIndex)
	--InfoLog(9, "Skill_Fire1090004011 30")
	while target:IsNil() == false do
		--InfoLog(9, "Skill_Fire1090004011 31")
		local iRandValue = RAND(ABILITY_RATE_VALUE)
		--InfoLog(9, "Skill_Fire1090004011 32")
		if iRandValue < iEffectPer then
			--InfoLog(9, "Skill_Fire1090004011 33")
			local aresult = result:GetResult(target:GetGuid(), true)
			--InfoLog(9, "Skill_Fire1090004011 34")
			if aresult:IsNil() == false and aresult:GetInvalid() == false then
				--InfoLog(9, "Skill_Fire1090004011 35")
			--DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
				target:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
				--InfoLog(9, "Skill_Fire1090004011 35")
			end
		end

		iIndex = iIndex + 1
		target = kTargetArray:GetUnit(iIndex)
	end
	
	--InfoLog(9, "Skill_Fire1090004011 99")
	return iIndex
end

function Skill_Fail1090004011(caster, kTargetArray, skillnum, result, arg)
	--local iIndex = 0
	--local target = kTargetArray:GetUnit(iIndex)
	--while target:IsNil() == false do
	--	caster:SyncRandom(ABILITY_RATE_VALUE)
	--	caster:SyncRandom(ABILITY_RATE_VALUE)

	--	iIndex = iIndex + 1
	--	target = kTargetArray:GetUnit(iIndex)
	--end
	return false
end

function Skill_CanFire1090004011(caster, kTargetArray, skillnum, actarg)
	InfoLog(9, "Skill_CanFire1090004011 --")
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() == true then
		return false
	end
	
	--local kEffectMgr = target:GetEffectMgr()
	--local kEffect = kEffectMgr:GetEffect(kSkillDef:GetEffectNo(), true)
	--if kEffect:IsNil() == false then
		-- 이미 효과를 받았으므로 또 받지 않도록 한다.
	--	return false
	--end	
	
	if Skill_CheckRange(caster, kTargetArray, kSkillDef, actarg) == false then
		return false
	end
	
	return true
end

function Skill_CanReserve1090004011(caster, kTargetArray, skillnum, actarg)
	InfoLog(9, "Skill_CanReserve1090004011 --")
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end



function Skill_Begin1090004012(caster, skillnum, iStatus,arg)
	return Skill_Begin1090004011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090004012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1090004012(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1090004012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1090004012(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1090004013(caster, skillnum, iStatus,arg)
	return Skill_Begin1090004011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090004013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1090004013(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1090004013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1090004013(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1090004014(caster, skillnum, iStatus,arg)
	return Skill_Begin1090004011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090004014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1090004014(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1090004014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1090004014(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1090004015(caster, skillnum, iStatus,arg)
	return Skill_Begin1090004011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090004015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1090004015(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1090004015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1090004015(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1090004016(caster, skillnum, iStatus,arg)
	return Skill_Begin1090004011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090004016(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1090004016(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1090004016(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1090004016(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1090004017(caster, skillnum, iStatus,arg)
	return Skill_Begin1090004011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090004017(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1090004017(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1090004017(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1090004017(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1090004018(caster, skillnum, iStatus,arg)
	return Skill_Begin1090004011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090004018(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1090004018(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1090004018(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1090004018(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1090004019(caster, skillnum, iStatus,arg)
	return Skill_Begin1090004011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090004019(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1090004019(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1090004019(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1090004019(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_Begin1090004020(caster, skillnum, iStatus,arg)
	return Skill_Begin1090004011(caster, skillnum, iStatus,arg)
end

function Skill_Fire1090004020(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fire1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_Fail1090004020(caster, kTargetArray, skillnum, result, arg)
	return Skill_Fail1090004011(caster, kTargetArray, skillnum, result, arg)
end

function Skill_CanFire1090004020(caster, kTargetArray, skillnum, actarg)
	return Skill_CanFire1090004011(caster, kTargetArray, skillnum, actarg)
end

function Skill_CanReserve1090004020(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserve1090004011(caster, kTargetArray, skillnum, actarg)
end

