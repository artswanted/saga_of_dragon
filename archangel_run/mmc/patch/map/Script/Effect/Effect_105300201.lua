
-- 기사 (패링 : a_Parring) : Level 1-5
function Effect_Begin105300201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin105300201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- BlockRate 증가
	local iAdd = kEffect:GetAbil(AT_BLOCK_RATE)
	InfoLog(9, "Effect_Begin105300201 Add:" .. iAdd .. ", Before[AT_BLOCK_RATE:" .. unit:GetAbil(AT_C_BLOCK_RATE))
	kEffectMgr:AddAbil(AT_BLOCK_RATE, iAdd)
	unit:NftChangedAbil(AT_BLOCK_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin105300201 After[AT_BLOCK_RATE:" .. unit:GetAbil(AT_C_BLOCK_RATE))	
	return 1
end
                                                                                                                          
function Effect_End105300201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End105300201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- BlockRate 증가
	local iAdd = kEffect:GetAbil(AT_BLOCK_RATE)
	kEffectMgr:AddAbil(AT_BLOCK_RATE, 0-iAdd)
	unit:NftChangedAbil(AT_BLOCK_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	return 1
end

function Effect_Tick105300201(unit, elapsedtime, effect, actarg)

	local kInventory = unit:GetInventory()
	if kInventory:IsNil()== true then
		InfoLog(5, "Effect_Tick105300201 : unit has no Inventory ")
		return ECT_MUSTDELETE
	end
	local kItem = kInventory:GetItem(IT_FIT, EQUIP_POS_WEAPON)
	if kItem:IsNil()== true then
		InfoLog(9, "Effect_Tick105300201 : unit has no weapon")
		return ECT_MUSTDELETE
	end
	if kItem:CheckWeaponLimit(effect:GetValue()) == false then
		InfoLog(9, "Effect_Tick105300201 : WeaponLimit!!!")
		return ECT_MUSTDELETE
	end
	
	return ECT_DOTICK
end
