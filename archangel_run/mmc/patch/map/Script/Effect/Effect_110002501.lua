
-- 트랩퍼 (건들지마라! : a_Dont touch me! ) : Level 1-10
function Effect_Begin110002501(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin110002501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	return 1
end
                                                                                                                          
function Effect_End110002501(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End110002501...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local iDamageEffectNo = kEffect:GetAbil(AT_EFFECTNUM1)
	InfoLog(9, "Effect_End110002501...iDamageEffectNo : " .. iDamageEffectNo ..")");
	--if iDamageEffectNo <= 0 then
	--InfoLog(5, "Effect_End110002501, Cannot Get AT_EFFECTNUM1")
	--	return 0
	--end

	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)

	local kPos = unit:GetPos()
	local kUnitArray = NewUNIT_PTR_ARRAY()	

	local iRange = kEffect:GetAbil(AT_DETECT_RANGE)
	kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_MONSTER, kUnitArray);
	kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_PLAYER, kUnitArray);
	InfoLog(9, "Effect_End110002501...Range : " .. iRange ..")");

	local iIndex = 0
	local target = kUnitArray:GetUnit(iIndex)
	local iMaxTarget = kEffect:GetAbil(AT_MAX_TARGETNUM);
	local iCount = kEffect:GetAbil(AT_COUNT);
	InfoLog(9, "Effect_End110002501...iMaxTarget : " .. iMaxTarget ..")");
	InfoLog(9, "Effect_End110002501...iCount : " .. iCount ..")");
	while target:IsNil() == false and iIndex < iMaxTarget do --최대 타겟만큼만 잡는다.
		if unit:IsEnemy(target, false) == true then	-- Enemy에게만 적용시킨다.
			-- 데미지 = 남아있는 hp * count의 피해
			local iDamage = unit:GetAbil(AT_HP) * iCount;
			InfoLog(9, "Effect_End110002501...Damage : " .. iDamage ..")");
			DoTickDamage(unit, target, iDamage, kEffect:GetEffectNo(), iDamageEffectNo, actarg);
		end
		iIndex = iIndex + 1
		target = kUnitArray:GetUnit(iIndex)
	end

	-- 현재 HP / MP를 effect에 세팅된 값으로 세팅한다.
	unit:SetHP(kEffect:GetAbil(AT_HP), E_SENDTYPE_BROADALL, unit:GetGuid())
	unit:SetMP(kEffect:GetAbil(AT_MP), E_SENDTYPE_BROADALL, unit:GetGuid())

	DeleteUNIT_PTR_ARRAY(kUnitArray);

	return 1
end

function Effect_Tick110002501(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick110002501 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
