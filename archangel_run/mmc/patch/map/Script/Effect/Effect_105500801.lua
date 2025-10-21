
-- Paladin (바리케이드 : a_Barricade) : Level 1-10
function Effect_Begin105500801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin105500801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 물리 전 방어력
	local iPhy = kEffect:GetAbil(AT_PHY_DMG_DEC)
	--InfoLog(9, "Effect_Begin102000701 before:PHY:".. iPhy .. ", DEC:" ..unit:GetAbil(AT_PHY_DMG_DEC).." DEC_C:" ..unit:GetAbil(AT_C_PHY_DMG_DEC))
	kEffectMgr:AddAbil(AT_PHY_DMG_DEC, iPhy)
	unit:NftChangedAbil(AT_PHY_DMG_DEC, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_Begin102000701 after:DEC:".. unit:GetAbil(AT_PHY_DMG_DEC).." DEC_C:" ..unit:GetAbil(AT_C_PHY_DMG_DEC))
	-- 마법 전 방어력
	local iMagic = kEffect:GetAbil(AT_MAGIC_DMG_DEC)
	--InfoLog(9, "Effect_Begin102000701 before:MAGIC:".. iMagic .. ", DEC:" .. unit:GetAbil(AT_MAGIC_DMG_DEC).."DEC_C:" ..unit:GetAbil(AT_C_MAGIC_DMG_DEC))
	kEffectMgr:AddAbil(AT_MAGIC_DMG_DEC, iMagic)
	unit:NftChangedAbil(AT_MAGIC_DMG_DEC, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_Begin102000701 after:MAGIC:".. unit:GetAbil(AT_MAGIC_DMG_DEC).." DEC_C:" ..unit:GetAbil(AT_C_MAGIC_DMG_DEC))	

	-- 이동속도 감소
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	--InfoLog(9, "Effect_Begin102000701 Before(AT_C_MOVESPEED):" .. unit:GetAbil(AT_C_MOVESPEED) .. ", iAdd:" .. iAdd)	
	kEffectMgr:AddAbil(AT_R_MOVESPEED, iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_Begin102000701 After(AT_C_MOVESPEED):" .. unit:GetAbil(AT_C_MOVESPEED))

	--무기 교환 불가
	kEffectMgr:AddAbil(AT_CANNOT_EQUIP, 1)
	unit:NftChangedAbil(AT_CANNOT_EQUIP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	--아이템 사용 불가
	kEffectMgr:AddAbil(AT_CANNOT_USEITEM, 1)
	unit:NftChangedAbil(AT_CANNOT_USEITEM, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	--데미지 모션 하지 않음
	kEffectMgr:AddAbil(AT_DAMAGEACTION_TYPE, 101)
	unit:NftChangedAbil(AT_DAMAGEACTION_TYPE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	return 1
end
                                                                                                                          
function Effect_End105500801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End105500801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 물리 전 방어력
	local iPhy = kEffect:GetAbil(AT_PHY_DMG_DEC)
	--InfoLog(9, "Effect_End105500801 before:PHY:".. iPhy .. ", DEC:" ..unit:GetAbil(AT_PHY_DMG_DEC))
	kEffectMgr:AddAbil(AT_PHY_DMG_DEC, 0-iPhy)
	unit:NftChangedAbil(AT_PHY_DMG_DEC, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_End105500801 after:DEC:".. unit:GetAbil(AT_C_PHY_DMG_DEC))
	-- 마법 전 방어력
	local iMagic = kEffect:GetAbil(AT_MAGIC_DMG_DEC)
	--InfoLog(9, "Effect_End105500801 before:MAGIC:".. iMagic .. ", DEC:" .. unit:GetAbil(AT_MAGIC_DMG_DEC))	
	kEffectMgr:AddAbil(AT_MAGIC_DMG_DEC, 0-iMagic)
	unit:NftChangedAbil(AT_MAGIC_DMG_DEC, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_End105500801 after:MAGIC:".. unit:GetAbil(AT_MAGIC_DMG_DEC))	
	
	-- 이동속도 증가
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	--InfoLog(9, "Effect_Begin102000701 Before(AT_C_MOVESPEED):" .. unit:GetAbil(AT_C_MOVESPEED) .. ", iAdd:" .. iAdd)	
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	--InfoLog(9, "Effect_Begin102000701 After(AT_C_MOVESPEED):" .. unit:GetAbil(AT_C_MOVESPEED))

	--무기 교환 가능
	kEffectMgr:AddAbil(AT_CANNOT_EQUIP, -1)
	unit:NftChangedAbil(AT_CANNOT_EQUIP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	--아이템 사용 가능
	kEffectMgr:AddAbil(AT_CANNOT_USEITEM, -1)
	unit:NftChangedAbil(AT_CANNOT_USEITEM, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	kEffectMgr:AddAbil(AT_DAMAGEACTION_TYPE, -101)
	unit:NftChangedAbil(AT_DAMAGEACTION_TYPE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	
	return 1
end

function Effect_Tick105500801(unit, elapsedtime, effect, actarg)
	local iNeed = effect:GetAbil(AT_MP)
	local iMP = unit:GetAbil(AT_MP)

	--NeedMP값이 -로 세팅되어있다.
	if -iNeed > iMP then
		return ECT_MUSTDELETE
	end

	--InfoLog(9, "Effect_Begin102000701 NeedMP : " .. iNeed.."   C_MP:"..iMP)
		
	unit:SetAbil(AT_MP, iMP+iNeed, true, false)
	return ECT_DOTICK
end
