
-- Thief (찾아보시지 : Shadow Walk) : Level 1-10
function Effect_Begin104000101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin104000101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	-- 이동속도가 Effect에서 정의된 절대값으로 변경된다.
	local iSpeed = 0-unit:GetAbil(AT_C_MOVESPEED)+kEffect:GetAbil(AT_MOVESPEED)
	kEffect:SetValue(iSpeed)
	kEffectMgr:AddAbil(AT_MOVESPEED, iSpeed)
	unit:NftChangedAbil(AT_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	-- MoveSpeed 변경 못하게 해 주고
	kEffectMgr:AddAbil(AT_FIXED_MOVESPEED, 1)
	-- AutoHeal Stop
	kEffectMgr:AddAbil(AT_ENABLE_AUTOHEAL, -1)
	-- 숨는 속성 살리고
	kEffectMgr:SetAbil(AT_UNIT_HIDDEN, 1)
	return 1
end

function Effect_End104000101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End104000101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	-- MoveSpeed 변경 못하게 했던 것을 먼저 풀어주고
	kEffectMgr:AddAbil(AT_FIXED_MOVESPEED, -1)
	unit:NftChangedAbil(AT_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	-- Restore MoveSpeed
	local iSpeed = kEffect:GetValue()
	kEffectMgr:AddAbil(AT_MOVESPEED, 0-iSpeed)
	-- MoveSpeed 변경 못하게 해 주고
	kEffectMgr:AddAbil(AT_FIXED_MOVESPEED, -1)
	-- AutoHeal Stop
	kEffectMgr:AddAbil(AT_ENABLE_AUTOHEAL, 1)
	-- 숨는 속성 살리고
	kEffectMgr:SetAbil(AT_UNIT_HIDDEN, 0)
	return 1
end

function Effect_Tick104000101(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick104000101 Effect=" .. effect:GetEffectNo())
	-- Hidden 상태가 그대로 유지되고 있는지 확인
	if unit:GetAbil(AT_UNIT_HIDDEN) <= 0 then
		-- Hidden 상태가 풀렸으므로 Effect는 종료되어야 한다.
		return ECT_MUSTDELETE
	end
	-- MP used
	local iMP = unit:GetAbil(AT_MP)
	local iDec = effect:GetAbil(AT_NEED_MP)
	if iMP < iDec then
		-- MP가 모자르므로 Effect 종료
		return ECT_MUSTDELETE	-- Self Delete
	end
	unit:SetAbil(AT_MP, iMP-iDec, true)
	--InfoLog(9, "Effect_Tick104000101 MP:" .. iMP-iDec)
	return ECT_DOTICK
end

