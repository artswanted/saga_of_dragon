
-- Fighter (갑옷부수기 : Armor Bread ) : Level 1-10
function Effect_Begin101000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 방어력 감소
  local iDec = kEffect:GetAbil(AT_PHY_DEFENCE)
  --InfoLog(9, "Effect_Begin101000701 iDec:" .. iDec .. ", Before(AT_C_PHY_DEFENCE):" .. unit:GetAbil(AT_C_PHY_DEFENCE))
  kEffectMgr:AddAbil(AT_PHY_DEFENCE, iDec)
	unit:NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_Begin101000701 iDec:" .. iDec .. ", After(AT_C_PHY_DEFENCE):" .. unit:GetAbil(AT_C_PHY_DEFENCE))
	return 1
end
                                                                                                                          
function Effect_End101000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End101000701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 방어력 감소
  local iDec = kEffect:GetAbil(AT_PHY_DEFENCE)
  kEffectMgr:AddAbil(AT_PHY_DEFENCE, 0-iDec)
	unit:NftChangedAbil(AT_PHY_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick101000701(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick101000701 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
