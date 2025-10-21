
-- 병아리 안전모 아이템 효과
function Effect_Begin12101(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_Begin12101 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin12011...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 공격불가 / 피격불가
	--InfoLog(9, "AT_CANNOT_DAMAGE Before=" .. unit:GetAbil(AT_CANNOT_DAMAGE))
	unit:AddAbil(AT_CANNOT_DAMAGE, 1)
	--InfoLog(9, "AT_CANNOT_DAMAGE After=" .. unit:GetAbil(AT_CANNOT_DAMAGE))
	--InfoLog(9, "AT_CANNOT_ATTACK Before=" .. unit:GetAbil(AT_CANNOT_ATTACK))
	unit:AddAbil(AT_CANNOT_ATTACK, 1)
	--InfoLog(9, "AT_CANNOT_ATTACK Before=" .. unit:GetAbil(AT_CANNOT_ATTACK))
	return 1
end

function Effect_End12101(unit, iEffectNo, actarg)
	--InfoLog(5, "Effect_End12101 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End12101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 공격불가 / 피격불가
	unit:AddAbil(AT_CANNOT_DAMAGE, -1)
	unit:AddAbil(AT_CANNOT_ATTACK, -1)
	return 1
end

function Effect_Tick12101(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick12101 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
