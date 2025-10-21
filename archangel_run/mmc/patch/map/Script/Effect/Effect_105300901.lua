
-- 기사 (HP회복력 향상 : a_HP Recovery) : Level 1-10 PASSIVE
function Effect_Begin105300901(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin105300901...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- Max HP 증가
	InfoLog(9, "Effect_Begin105300901 Before:AT_C_MAX_HP:" .. unit:GetAbil(AT_C_MAX_HP))
	local iAdd = kEffect:GetAbil(AT_MAX_HP)
	kEffectMgr:AddAbil(AT_MAX_HP, iAdd)
	unit:NftChangedAbil(AT_MAX_HP, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin105300901 After:AT_C_MAX_HP:" .. unit:GetAbil(AT_C_MAX_HP))
	-- 현재 HP도 Maxp HP가 증가되는 만큼 증가
	--InfoLog(9, "Effect_Begin105300901 Before:AT_HP:" .. unit:GetAbil(AT_HP))
	--local iHP = math.max(unit:GetAbil(AT_HP) + iAdd, unit:GetAbil(AT_C_MAX_HP))
	--unit:SetHP(iHP, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL, kEffect:GetCaster());
	--InfoLog(9, "Effect_Begin105300901 After:AT_HP:" .. unit:GetAbil(AT_HP))

	-- 자동회복되는 양 증가
	local iAdd = kEffect:GetAbil(AT_HP_RECOVERY)
	InfoLog(9, "Effect_Begin105300901 Before:AT_HP_RECOVERY:" .. unit:GetAbil(AT_C_HP_RECOVERY) .. ", Added:" .. iAdd)	
	kEffectMgr:AddAbil(AT_HP_RECOVERY, iAdd)
	unit:NftChangedAbil(AT_HP_RECOVERY, E_SENDTYPE_NONE)
	InfoLog(9, "Effect_Begin105300901 After:AT_HP_RECOVERY:" .. unit:GetAbil(AT_C_HP_RECOVERY))
	--Portion 효과 증가 (HP)
	local iAdd = kEffect:GetAbil(AT_HP_POTION_ADD_RATE)
	InfoLog(9, "Effect_Begin105300901 Before:AT_C_HP_POTION_ADD:" .. unit:GetAbil(AT_C_HP_POTION_ADD_RATE) .. ", Added:" .. iAdd)	
	kEffectMgr:AddAbil(AT_HP_POTION_ADD_RATE, iAdd)
	unit:NftChangedAbil(AT_HP_POTION_ADD_RATE, E_SENDTYPE_NONE)
	InfoLog(9, "Effect_After105300901 After:AT_C_HP_POTION_ADD:" .. unit:GetAbil(AT_C_HP_POTION_ADD_RATE))
	--Portion 효과 증가 (MP)
	local iAdd = kEffect:GetAbil(AT_MP_POTION_ADD_RATE)
	InfoLog(9, "Effect_Begin105300901 Before:AT_C_MP_POTION_ADD:" .. unit:GetAbil(AT_C_MP_POTION_ADD_RATE) .. ", Added:" .. iAdd)	
	kEffectMgr:AddAbil(AT_MP_POTION_ADD_RATE, iAdd)
	unit:NftChangedAbil(AT_MP_POTION_ADD_RATE, E_SENDTYPE_NONE)
	InfoLog(9, "Effect_Begin105300901 After:AT_C_MP_POTION_ADD:" .. unit:GetAbil(AT_C_MP_POTION_ADD_RATE))

	return 1
end
                                                                                                                          
function Effect_End105300901(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End105300901...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- Max HP 증가
	local iAdd = kEffect:GetAbil(AT_MAX_HP)
	kEffectMgr:AddAbil(AT_MAX_HP, 0-iAdd)
	unit:NftChangedAbil(AT_MAX_HP, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	-- HP가 Max HP보다 클 경우 감소 시킨다.
	--local iHP = math.min(unit:GetAbil(AT_HP), unit:GetAbil(AT_C_MAX_HP))
	--unit:SetHP(iHP, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL, kEffect:GetCaster());
	-- 자동회복되는 양 증가
	local iAdd = kEffect:GetAbil(AT_HP_RECOVERY)
	kEffectMgr:AddAbil(AT_HP_RECOVERY, 0-iAdd)
	unit:NftChangedAbil(AT_HP_RECOVERY, E_SENDTYPE_NONE)	
	--Portion 효과 증가 (HP)
	local iAdd = kEffect:GetAbil(AT_R_HP_POTION_ADD)
	kEffectMgr:AddAbil(AT_R_HP_POTION_ADD, 0-iAdd)
	unit:NftChangedAbil(AT_R_HP_POTION_ADD, E_SENDTYPE_NONE)	
	--Portion 효과 증가 (MP)
	local iAdd = kEffect:GetAbil(AT_R_MP_POTION_ADD)
	kEffectMgr:AddAbil(AT_R_MP_POTION_ADD, 0-iAdd)
	unit:NftChangedAbil(AT_R_MP_POTION_ADD, E_SENDTYPE_NONE)
end

function Effect_Tick105300901(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick105300901 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
