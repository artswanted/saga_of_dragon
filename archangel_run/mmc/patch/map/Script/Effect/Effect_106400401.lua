
-- Paladin (기원의 오라 : a_Prayer) : Level 1-10
function Effect_Begin106400401(unit, iEffectNo, actarg)
	--[[
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin106400401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 물리공격력 감소
	local iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	-- 마법공격력 감소
	iAdd = kEffect:GetAbil(AT_R_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_PHY_ATTACK_MIN)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_MAGIC_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_R_MAGIC_ATTACK_MIN)	
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	]]
	
	return 1
end
                                                                                                                          
function Effect_End106400401(unit, iEffectNo, actarg)
	--[[
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End106400401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 물리공격력 감소
	local iAdd = kEffect:GetAbil(AT_R_PHY_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_PHY_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_ATTACK_MIN, E_SENDTYPE_NONE)
	-- 마법공격력 감소
	iAdd = kEffect:GetAbil(AT_R_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_R_MAGIC_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_PHY_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_PHY_ATTACK_MIN)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_MAGIC_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_R_MAGIC_ATTACK_MIN)	
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)		
	]]

	return 1
end

function Effect_Tick106400401(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick106400401 Effect=" .. effect:GetEffectNo())
	local iNeedMP = effect:GetAbil(AT_MP)
	local iMP = unit:GetAbil(AT_MP)
	local iNewMP = math.max(0, iMP-iNeedMP)
	InfoLog(9, "Effect_Tick106400401 NeedMP:" .. iNeedMP .. ", NewMP:" .. iNewMP)
	unit:SetAbil(AT_MP, iNewMP, true, false)	
	if iNewMP<=0 then
		return ECT_MUSTDELETE
	end
	
	return ECT_DOTICK
end
