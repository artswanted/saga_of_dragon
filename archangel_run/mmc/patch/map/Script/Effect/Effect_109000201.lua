
-- 사냥꾼 (비웃지마라! : Condor Attack) : Level 1-10
function Effect_Begin109000201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin109000201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- Damage 증폭
	-- 무기 Damage의 %로 Damgae가 증가
	local kInventory = unit:GetInventory()
	if kInventory:IsNil()== true then
		InfoLog(5, "Effect_Begin109000201 : unit has no Inventory ")
		return 0
	end
	local kItem = kInventory:GetItem(IT_FIT, EQUIP_POS_WEAPON)
	if kItem:IsNil()== true then
		kEffect:SetValue(0)
		return 0
	end
	local iBase = kItem:GetAbil(AT_PHY_ATTACK)
	if iBase == 0 then
		kEffect:SetValue(0)
	else
		local iAddDmg = math.floor(kEffect:GetAbil(AT_R_PHY_ATTACK) * iBase / ABILITY_RATE_VALUE)
		kEffect:SetValue(iAddDmg)
		
		kEffectMgr:AddAbil(AT_PHY_ATTACK_MAX, iAddDmg)
		unit:NftChangedAbil(AT_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
		kEffectMgr:AddAbil(AT_PHY_ATTACK_MIN, iAddDmg)
		unit:NftChangedAbil(AT_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
		local kActarg = NewActArg()
		kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_PHY_ATTACK_MAX)
		kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_PHY_ATTACK_MIN)
		unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
		DeleteActArg(kActarg)		
	end	
	return 1
end

function Effect_End109000201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End109000201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- Damage 증폭
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
		DeleteActArg(kActarg)		
	end
	return 1
end

function Effect_Tick109000201(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick109000201 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

