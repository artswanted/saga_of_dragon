
-- 트랩퍼 (집중 : a_Concentrate) : Level 1-10 PASSIVE
function Effect_Begin110002301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin110002301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	--Critical 증가
	local iAdd = kEffect:GetAbil(AT_R_CRITICAL_RATE)
	InfoLog(9, "Effect_End110002301 iAdd:" .. iAdd ..", Before:" .. unit:GetAbil(AT_C_CRITICAL_RATE))
	kEffectMgr:AddAbil(AT_R_CRITICAL_RATE, iAdd)
	unit:NftChangedAbil(AT_R_CRITICAL_RATE, E_SENDTYPE_NONE)
	InfoLog(9, "Effect_End110002301 After:" .. unit:GetAbil(AT_C_CRITICAL_RATE))

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_CRITICAL_RATE)
	unit:SendAbiles(1, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	
	return 1
end
                                                                                                                          
function Effect_End110002301(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_End110002301 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End110002301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	--Critical 감소
	local iAdd = kEffect:GetAbil(AT_R_CRITICAL_RATE)
	kEffectMgr:AddAbil(AT_R_CRITICAL_RATE, 0-iAdd)
	unit:NftChangedAbil(AT_R_CRITICAL_RATE, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_CRITICAL_RATE)
	unit:SendAbiles(1, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	
	return 1
end

function Effect_Tick110002301(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick110002301 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
