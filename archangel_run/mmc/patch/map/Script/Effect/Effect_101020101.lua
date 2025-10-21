
-- 공통 : Stunned
function Effect_Begin101020101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin101020101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iSpeed = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, iSpeed)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	-- 임시적으로 MoveSpeed를 0으로 변경할 수는 있지만,
	-- MoveSpeed를 올려주는 Effect받으면, 그만큼 올라갈 수 있다.
	unit:AddAbil(AT_FROZEN, 1)
	-- Old Value 저장하고
	unit:SetAbil(AT_FROZEN_DMG_WAKE, 0)	-- Damage 받아도 깨어 날 수 없다.
	--InfoLog(9, "Effect_Begin101020101 AT_FROZEN:" .. unit:GetAbil(AT_FROZEN))
	unit:AddAbil(AT_CANNOT_ATTACK, 1)
	local iAdd = kEffect:GetAbil(AT_CANNOT_EQUIP)
	unit:AddAbil(AT_CANNOT_EQUIP, iAdd)
	unit:NftChangedAbil(AT_CANNOT_EQUIP, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_CANNOT_USEITEM)
	unit:AddAbil(AT_CANNOT_USEITEM, iAdd)
	unit:NftChangedAbil(AT_CANNOT_USEITEM, E_SENDTYPE_NONE)

	return 1
end

function Effect_End101020101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End101020101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	--unit:SetState(US_IDLE)
	local iSpeed = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-iSpeed)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	-- 임시적으로 MoveSpeed를 0으로 변경할 수는 있지만,
	-- MoveSpeed를 올려주는 Effect받으면, 그만큼 올라갈 수 있다.
	unit:AddAbil(AT_FROZEN, -1)
	-- Old Value 저장하고
	unit:SetAbil(AT_FROZEN_DMG_WAKE, 1)
	unit:AddAbil(AT_CANNOT_ATTACK, -1)
	local iAdd = kEffect:GetAbil(AT_CANNOT_EQUIP)
	unit:AddAbil(AT_CANNOT_EQUIP, 0-iAdd)
	unit:NftChangedAbil(AT_CANNOT_EQUIP, E_SENDTYPE_NONE)
	iAdd = kEffect:GetAbil(AT_CANNOT_USEITEM)
	unit:AddAbil(AT_CANNOT_USEITEM, 0-iAdd)
	unit:NftChangedAbil(AT_CANNOT_USEITEM, E_SENDTYPE_NONE)
	return 1	
end

function Effect_Tick101020101(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick101020101 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

