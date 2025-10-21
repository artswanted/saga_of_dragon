
-- Fighter (미련 곰퉁이 : Bear Hank ) : Level 1-10
function Effect_Begin101000601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000601...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- STR 증가
	local iStr = kEffect:GetAbil(AT_STR)
	kEffectMgr:AddAbil(AT_STR, iStr)
	unit:NftChangedAbil(AT_STR, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	-- Damage 증폭
	-- 무기 Damage의 %로 Damgae가 증가
	local kInventory = unit:GetInventory()
	if kInventory:IsNil()== true then
		--InfoLog(5, "Effect_Begin101000601 : unit has no Inventory ")
		return 0
	end
	local kItem = kInventory:GetItem(IT_FIT, EQUIP_POS_WEAPON)
	if kItem:IsNil()== true then
		--InfoLog(9, "Effect_Begin101000601 31")
		kEffect:SetValue(0)
		return 0
	end
	--InfoLog(9, "Effect_Begin101000601 40")
	local iBase = kItem:GetAbil(AT_PHY_ATTACK_MAX)
	if iBase == 0 then
		--InfoLog(9, "Effect_Begin101000601 41")
		kEffect:SetValue(0)
	else
		local iAddDmg = math.floor(kEffect:GetAbil(AT_R_PHY_ATTACK_MAX) * iBase / ABILITY_RATE_VALUE)
		kEffect:SetValue(iAddDmg)		
		if iAddDmg > 0 then
			kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, iAddDmg)
			unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
			kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, iAddDmg)
			unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
			
			local kActarg = NewActArg()
			kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_PHY_ATTACK_MAX)
			kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_PHY_ATTACK_MIN)
			unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
		end
	end
	return 1
end
                                                                                                                          
function Effect_End101000601(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_End101000601 --")
	local kEffectMgr = unit:GetEffectMgr()
	--InfoLog(9, "Effect_End101000601 01")
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	--InfoLog(9, "Effect_End101000601 02")
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101000601...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--InfoLog(9, "Effect_End101000601 10")
	-- STR 증가
  local iStr = kEffect:GetAbil(AT_STR)
	kEffectMgr:AddAbil(AT_STR, 0-iStr)
	--InfoLog(9, "Effect_End101000601 iStr:" .. iStr .. ", Before:AT_C_STR:" .. unit:GetAbil(AT_C_STR))
	unit:NftChangedAbil(AT_STR, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_End101000601 Aftger:AT_C_STR:" .. unit:GetAbil(AT_C_STR))
	local iAddDmg = kEffect:GetValue()
	if iAddDmg > 0 then
		kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, 0-iAddDmg)
		unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
		kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, 0-iAddDmg)
		unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
			
		local kActarg = NewActArg()
		kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_PHY_ATTACK_MAX)
		kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_PHY_ATTACK_MIN)
		unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	end
	return 1
end

function Effect_Tick101000601(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick101000601 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
