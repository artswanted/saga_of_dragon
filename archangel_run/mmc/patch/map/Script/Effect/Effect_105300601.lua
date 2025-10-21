
-- 기사 (리벤지 : a_Revenge) : Level 1-5
function Effect_Begin105300601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin107000701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 반사가 일어날 확률
	local iAdd = kEffect:GetAbil(AT_DMG_REFLECT_HITRATE)
	InfoLog(9, "Effect_Begin105300601 Before(AT_DMG_REFLECT_HITRATE):" .. unit:GetAbil(AT_C_DMG_REFLECT_HITRATE))
	kEffectMgr:AddAbil(AT_DMG_REFLECT_HITRATE, iAdd)
	unit:NftChangedAbil(AT_C_DMG_REFLECT_HITRATE, E_SENDTYPE_NONE)
	InfoLog(9, "Effect_Begin105300601 After(AT_DMG_REFLECT_HITRATE):" .. unit:GetAbil(AT_C_DMG_REFLECT_HITRATE))
	-- 반사되는 반사량
	local iAdd = kEffect:GetAbil(AT_DMG_REFLECT_RATE)
	InfoLog(9, "Effect_Begin105300601 Before(AT_DMG_REFLECT_RATE):" .. unit:GetAbil(AT_C_DMG_REFLECT_RATE))
	kEffectMgr:AddAbil(AT_DMG_REFLECT_RATE, iAdd)
	unit:NftChangedAbil(AT_C_DMG_REFLECT_RATE, E_SENDTYPE_NONE)
	InfoLog(9, "Effect_Begin105300601 After(AT_DMG_REFLECT_RATE):" .. unit:GetAbil(AT_C_DMG_REFLECT_RATE))

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_DMG_REFLECT_HITRATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_DMG_REFLECT_RATE)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end
                                                                                                                          
function Effect_End105300601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End105300601...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 반사가 일어날 확률
	local iAdd = kEffect:GetAbil(AT_DMG_REFLECT_HITRATE)
	InfoLog(9, "Effect_End105300601 Before(AT_DMG_REFLECT_HITRATE):" .. unit:GetAbil(AT_C_DMG_REFLECT_HITRATE))
	kEffectMgr:AddAbil(AT_DMG_REFLECT_HITRATE, 0-iAdd)
	unit:NftChangedAbil(AT_C_DMG_REFLECT_HITRATE, E_SENDTYPE_NONE)
	InfoLog(9, "Effect_End105300601 After(AT_DMG_REFLECT_HITRATE):" .. unit:GetAbil(AT_C_DMG_REFLECT_HITRATE))
	-- 반사되는 반사량
	local iAdd = kEffect:GetAbil(AT_DMG_REFLECT_RATE)
	InfoLog(9, "Effect_End105300601 Before(AT_DMG_REFLECT_RATE):" .. unit:GetAbil(AT_C_DMG_REFLECT_RATE))
	kEffectMgr:AddAbil(AT_DMG_REFLECT_RATE, 0-iAdd)
	unit:NftChangedAbil(AT_C_DMG_REFLECT_RATE, E_SENDTYPE_NONE)
	InfoLog(9, "Effect_End105300601 After(AT_DMG_REFLECT_RATE):" .. unit:GetAbil(AT_C_DMG_REFLECT_RATE))

	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_DMG_REFLECT_HITRATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_DMG_REFLECT_RATE)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_Tick105300601(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick105300601 Effect=" .. effect:GetEffectNo())
	local iNeedMP = effect:GetAbil(AT_MP)
	local iMP = unit:GetAbil(AT_MP)
	if (iMP + iNeedMP) < 0 then
		return ECT_MUSTDELETE
	end
	unit:SetAbil(AT_MP, iMP+iNeedMP, true, false)
	return ECT_DOTICK
end
