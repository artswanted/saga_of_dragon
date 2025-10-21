
-- 공통 : 동빙
function Effect_Begin100010301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin100010301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAdd = kEffect:GetAbil(AT_R_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MAGIC_DEFENCE)
	kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_NONE)
	
	unit:AddAbil(AT_FROZEN, 1)
	unit:SetAbil(AT_FROZEN_DMG_WAKE, 1)	-- Damage 받으면 깨어난다.
	unit:AddAbil(AT_CANNOT_ATTACK, 1)
	local iAdd = kEffect:GetAbil(AT_CANNOT_EQUIP)
	unit:AddAbil(AT_CANNOT_EQUIP, iAdd)
	unit:NftChangedAbil(AT_CANNOT_EQUIP, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_CANNOT_USEITEM)
	unit:AddAbil(AT_CANNOT_USEITEM, iAdd)
	unit:NftChangedAbil(AT_CANNOT_USEITEM, E_SENDTYPE_NONE)
		
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_PHY_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MAGIC_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_MOVESPEED)
	unit:SendAbiles(3, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	
	return 1
end

function Effect_End100010301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End100010301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_R_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MAGIC_DEFENCE)
	kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_NONE)
	
	unit:AddAbil(AT_FROZEN, -1)
	--unit:SetAbil(AT_FROZEN_DMG_WAKE, 1)	-- Damage 받으면 깨어난다.
	unit:AddAbil(AT_CANNOT_ATTACK, -1)
	local iAdd = kEffect:GetAbil(AT_CANNOT_EQUIP)
	unit:AddAbil(AT_CANNOT_EQUIP, 0-iAdd)
	unit:NftChangedAbil(AT_CANNOT_EQUIP, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_CANNOT_USEITEM)
	unit:AddAbil(AT_CANNOT_USEITEM, 0-iAdd)
	unit:NftChangedAbil(AT_CANNOT_USEITEM, E_SENDTYPE_NONE)
		
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_PHY_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MAGIC_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_MOVESPEED)
	unit:SendAbiles(3, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)	
	
	return 1	
end

function Effect_Tick100010301(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick100010301 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK	
end


