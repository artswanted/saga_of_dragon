
-- Magician (깜짝놀랬지 : Transformation) : Level 1-5
function Effect_Begin102000201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin102000201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--unit:SetState(US_FROZEN)
	local iSpeed = unit:GetAbil(AT_MOVESPEED)
	--InfoLog(9, "Effect_Begin102000201 Before AT_C_MOVESPEED:" .. unit:GetAbil(AT_C_MOVESPEED))
	kEffectMgr:AddAbil(AT_MOVESPEED, 0-iSpeed)
	unit:NftChangedAbil(AT_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_Begin102000201 After AT_C_MOVESPEED:" .. unit:GetAbil(AT_C_MOVESPEED))
	kEffect:SetValue(iSpeed)
	
	unit:SetAbil(AT_FROZEN, unit:GetAbil(AT_FROZEN)+1, false)
	InfoLog(9, "Effect_Begin102000201 AT_FROZEN:" .. unit:GetAbil(AT_FROZEN))
	unit:SetAbil(AT_FROZEN_DMG_WAKE, unit:GetAbil(AT_FROZEN_DMG_WAKE)+1, false)
	InfoLog(9, "Effect_Begin102000201 AT_FROZEN:" .. unit:GetAbil(AT_FROZEN))
	return 1
end

function Effect_End102000201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End102000201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--unit:SetState(US_IDLE)
	local iSpeed = kEffect:GetValue()
	InfoLog(9, "Effect_End102000201 Before AT_C_MOVESPEED:" .. unit:GetAbil(AT_C_MOVESPEED))
	kEffectMgr:AddAbil(AT_MOVESPEED, iSpeed)
	unit:NftChangedAbil(AT_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_End102000201 After AT_C_MOVESPEED:" .. unit:GetAbil(AT_C_MOVESPEED))
	local iValue = unit:GetAbil(AT_FROZEN)
	if iValue > 0 then
		unit:SetAbil(AT_FROZEN, iValue-1, false)
	else
		unit:SetAbil(AT_FROZEN, 0, false)
	end
	InfoLog(9, "Effect_End102000201 AT_FROZEN:" .. unit:GetAbil(AT_FROZEN))	
	
	iValue = unit:GetAbil(AT_FROZEN_DMG_WAKE)
	if iValue > 0 then
		unit:SetAbil(AT_FROZEN_DMG_WAKE, unit:GetAbil(AT_FROZEN_DMG_WAKE)-1, false)
	else
		unit:SetAbil(AT_FROZEN_DMG_WAKE, 0, false)
	end
	return 1
end

function Effect_Tick102000201(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick102000201 Effect=" .. effect:GetEffectNo())
	local iAbil = unit:GetAbil(AT_FROZEN)
	if iAbil <= 0 then
		return ECT_MUSTDELETE
	end
	return ECT_DOTICK
end

