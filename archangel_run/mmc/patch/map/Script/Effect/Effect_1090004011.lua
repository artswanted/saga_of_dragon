
-- 메이지 (콰그마이어 효과 : a_Quagmire_Fire) : Level 1-10
function Effect_Begin1090004011(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin1090004011...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAdd = kEffect:GetAbil(AT_R_HITRATE)
	InfoLog(9, "Effect_Begin1090004011 Before:AT_C_HITRATE:" .. unit:GetAbil(AT_C_HITRATE))
	kEffectMgr:AddAbil(AT_R_HITRATE, iAdd)
	unit:NftChangedAbil(AT_R_HITRATE, E_SENDTYPE_NONE)
	InfoLog(9, "Effect_Begin1090004011 After:AT_C_HITRATE:" .. unit:GetAbil(AT_C_HITRATE))
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	InfoLog(9, "Effect_Begin1090004011 Before:AT_C_MOVESPEED:" .. unit:GetAbil(AT_C_MOVESPEED))
	kEffectMgr:AddAbil(AT_R_MOVESPEED, iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_NONE)
	InfoLog(9, "Effect_Begin1090004011 After:AT_C_MOVESPEED:" .. unit:GetAbil(AT_C_MOVESPEED))
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_HITRATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MOVESPEED)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)

	return 1
end

function Effect_End1090004011(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_End1090004011 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End1090004011...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAdd = kEffect:GetAbil(AT_R_HITRATE)
	kEffectMgr:AddAbil(AT_R_HITRATE, 0-iAdd)
	unit:NftChangedAbil(AT_R_HITRATE, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_HITRATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MOVESPEED)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_Tick1090004011(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick1090004011 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
