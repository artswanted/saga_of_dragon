
-- 위자드 (MP 포션 극대화 : a_Addition MP Potion Effect) : Level 1-10 PASSIVE
function Effect_Begin109001101(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_Begin109001101 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin109001101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	--Portion 효과 증가 (MP)
	local iAdd = kEffect:GetAbil(AT_MP_POTION_ADD_RATE)
	kEffectMgr:AddAbil(AT_MP_POTION_ADD_RATE, iAdd)
	unit:NftChangedAbil(AT_MP_POTION_ADD_RATE, E_SENDTYPE_NONE)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_MP_POTION_ADD_RATE)
	unit:SendAbiles(1, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)

	return 1
end
                                                                                                                          
function Effect_End109001101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End109001101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	--Portion 효과 감소 (MP)
	local iAdd = kEffect:GetAbil(AT_MP_POTION_ADD_RATE)
	kEffectMgr:AddAbil(AT_MP_POTION_ADD_RATE, 0-iAdd)
	unit:NftChangedAbil(AT_MP_POTION_ADD_RATE, E_SENDTYPE_NONE)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_MP_POTION_ADD_RATE)
	unit:SendAbiles(1, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)

	return 1
end

function Effect_Tick109001101(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick109001101 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
