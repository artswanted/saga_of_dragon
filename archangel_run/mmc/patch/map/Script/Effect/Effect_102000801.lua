
-- 마법사 (정신일도하사불성 : Concentration) : Level 1-10
function Effect_Begin102000801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin102000801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_INT)
	--InfoLog(9, "Effect_Begin102000801 AT_INT:" .. iAdd)
	--InfoLog(9, "Effect_Begin102000801 Before(AT_INT):" .. unit:GetAbil(AT_C_INT))
	kEffectMgr:AddAbil(AT_INT, iAdd)
	unit:NftChangedAbil(AT_INT, E_SENDTYPE_NONE)
	--InfoLog(9, "Effect_Begin102000801 After(AT_INT):" .. unit:GetAbil(AT_C_INT))	
	iAdd = kEffect:GetAbil(AT_R_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_INT)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MAGIC_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_MAGIC_ATTACK_MIN)	
	unit:SendAbiles(3, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_End102000801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End102000801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local iAdd = kEffect:GetAbil(AT_INT)
	kEffectMgr:AddAbil(AT_INT, 0-iAdd)
	unit:NftChangedAbil(AT_INT, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_INT)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MAGIC_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_MAGIC_ATTACK_MIN)	
	unit:SendAbiles(3, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_Tick102000801(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick102000801 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

