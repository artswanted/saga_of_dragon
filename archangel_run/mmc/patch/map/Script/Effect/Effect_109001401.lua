
-- Wizard (썬더 브레이크 : a_Thunder Break) : Level 1-5
function Effect_Begin109001401(unit, iEffectNo, actarg)
	return 1
end
                                                                                                                          
function Effect_End109001401(unit, iEffectNo, actarg)
	return 1
end

function Effect_Tick109001401(unit, elapsedtime, effect, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(effect:GetEffectNo(), false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Tick109001401...Cannot GetEffect(" .. effect:GetEffectNo() ..")");
		return 0
	end
	
	local iSkillNo = kEffect:GetAbil(AT_MON_SKILL_01)
	if iSkillNo <= 0 then
		InfoLog(5, "Effect_Tick109001401, Cannot Get AT_MON_SKILL_01 " ..  effect:GetEffectNo())
		return ECT_MUSTDELETE
	end

	local kGround = actarg:Get(ACTARG_GROUND)
	if kGround == nil then
		InfoLog(5, "Effect_Tick109001401, Ground is Nil ")
		return ECT_MUSTDELETE
	end

	kGround = Ground(kGround)

	if kGround:IsNil() then
		InfoLog(5, "Effect_Tick109001401, Ground is Nil ")
		return ECT_MUSTDELETE
	end

	local kPos = unit:GetPos()
	local kUnitArray = NewUNIT_PTR_ARRAY()	

	local iRange = effect:GetAbil(AT_DETECT_RANGE)
	local kUnitType = unit:GetUnitType()
	
	if UT_PLAYER == kUnitType then
		kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_MONSTER, kUnitArray);
	end

	-- 현재 Effect는 몬스터도 사용한다.
	kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_PLAYER, kUnitArray);

	local kSkillDef = GetSkillDef(iSkillNo)
	if kSkillDef:IsNil() then
		InfoLog(5, "Effect_Tick109001401....SkillDef is NIl" ..iSkillNo)
		return ECT_MUSTDELETE
	end

	local iIndex = 0
	local target = kUnitArray:GetUnit(iIndex)
	local iPercent = kSkillDef:GetAbil(AT_MAGIC_DMG_PER)
	local iMaxTarget = effect:GetAbil(AT_MAX_TARGETNUM);
	while target:IsNil() == false and iIndex < iMaxTarget do --최대 타겟만큼만 잡는다.
		if unit:IsEnemy(target, false) == true then	-- Enemy에게만 적용시킨다.
			-- 데미지 마법 공격력의 %		
			local iMagic = unit:GetAbil(AT_C_MAGIC_ATTACK) * iPercent / ABILITY_RATE_VALUE
			if 0~=iMagic then
				DoTickDamage(unit, target, iMagic, effect:GetEffectNo(), kSkillDef:GetEffectNo(), actarg);
			end
		end
		iIndex = iIndex + 1
		target = kUnitArray:GetUnit(iIndex)
	end

	DeleteUNIT_PTR_ARRAY(kUnitArray);

	return ECT_DOTICK
end

-- Wizard (썬더 브레이크<효과> : a_Thunder Break) : Level 1-5
function Effect_Begin1090014001(unit, iEffectNo, actarg)
	return 1
end
function Effect_End1090014001(unit, iEffectNo, actarg)
	return 1
end

function Effect_Tick1090014001(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick1090014001 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
