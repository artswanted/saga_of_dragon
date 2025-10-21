
-- 성기사 (바디 액티베이션 : a_Body Activation) : Level 1-5 PASSIVE
function Effect_Begin105400001(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_Begin105400001 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin105400001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--Max HP 증가
	local iAdd = kEffect:GetAbil(AT_R_MAX_HP)
	kEffectMgr:AddAbil(AT_R_MAX_HP, iAdd)
	unit:NftChangedAbil(AT_R_MAX_HP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_MAX_HP)
	unit:SendAbiles(1, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)

	return 1
end
                                                                                                                          
function Effect_End105400001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End105400001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--Max HP 감소
	local iAdd = kEffect:GetAbil(AT_R_MAX_HP)
	kEffectMgr:AddAbil(AT_R_MAX_HP, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAX_HP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_MAX_HP)
	unit:SendAbiles(1, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)

	return 1
end

function Effect_Tick105400001(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick105400001 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
