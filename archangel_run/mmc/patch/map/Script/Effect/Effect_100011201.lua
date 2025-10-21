
-- 공통 : 실명
function Effect_Begin100011201(unit, iEffectNo, actarg)

	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin100011201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_R_HITRATE)
	kEffectMgr:AddAbil(AT_R_HITRATE, iAdd)
	unit:NftChangedAbil(AT_R_HITRATE, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_DODGE_RATE)
	kEffectMgr:AddAbil(AT_R_DODGE_RATE, iAdd)
	unit:NftChangedAbil(AT_R_DODGE_RATE, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_HITRATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_DODGE_RATE)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	

	return 1
end

function Effect_End100011201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End100011201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_R_HITRATE)
	kEffectMgr:AddAbil(AT_R_HITRATE, 0-iAdd)
	unit:NftChangedAbil(AT_R_HITRATE, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_DODGE_RATE)
	kEffectMgr:AddAbil(AT_R_DODGE_RATE, 0-iAdd)
	unit:NftChangedAbil(AT_R_DODGE_RATE, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_HITRATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_DODGE_RATE)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	

	return 1	
end

function Effect_Tick100011201(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick100011201 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK		
end


