
-- 투사 (그로우 바디 : a_Grow Body) : Level 1-10
function Effect_Begin106300601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin106300601...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- MAX_HP 증가
	local iAdd = kEffect:GetAbil(AT_MAX_HP)
	InfoLog(9, "Effect_Begin106300601 Before,AT_MAX_HP:" .. unit:GetAbil(AT_C_MAX_HP))
	kEffectMgr:AddAbil(AT_MAX_HP, iAdd)
	unit:NftChangedAbil(AT_MAX_HP, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin106300601 After,AT_MAX_HP:" .. unit:GetAbil(AT_C_MAX_HP))
	return 1
end
                                                                                                                          
function Effect_End106300601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End106300601...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- MAX_HP 증가
	local iAdd = kEffect:GetAbil(AT_MAX_HP)
	kEffectMgr:AddAbil(AT_MAX_HP, 0-iAdd)
	unit:NftChangedAbil(AT_MAX_HP, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	local iMaxHP = unit:GetAbil(AT_C_MAX_HP)
	-- 현재HP값이 MAX_HP를 넘지 않도록 조정
	if unit:GetAbil(AT_HP) > iMaxHP then
		--unit:SetAbil(AT_HP, iMaxHP, true, false)
		unit:SetHP(iMaxHP, E_SENDTYPE_SELF, kEffect:GetCaster())
	end
	return 1
end

function Effect_Tick106300601(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick106300701 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
