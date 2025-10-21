
--  도둑 (더블어택 : a_Double Attack) : Level 1-10 PASSIVE
function Effect_Begin104000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin104000701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local iAdd = kEffect:GetAbil(AT_PERCENTAGE)
	unit:AddAbil(AT_CLAW_DBL_ATTK_RATE, iAdd)

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_CLAW_DBL_ATTK_RATE)
	unit:SendAbiles(1, kActarg, E_SENDTYPE_BROADALL)
	DeleteActArg(kActarg)
	
	return 1
end
                                                                                                                          
function Effect_End104000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End104000701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local iAdd = kEffect:GetAbil(AT_PERCENTAGE)
	unit:AddAbil(AT_CLAW_DBL_ATTK_RATE, 0-iAdd)
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_CLAW_DBL_ATTK_RATE)
	unit:SendAbiles(1, kActarg, E_SENDTYPE_BROADALL)
	DeleteActArg(kActarg)

	return 1
end

function Effect_Tick104000701(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick104000701 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
