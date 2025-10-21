
-- Wizard ( 완벽한 결계 : a_Perfect Sanctuary) : Level 1-10
function Effect_Begin109001901(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin109001901...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	kEffectMgr:AddAbil(AT_CANNOT_DAMAGE, 1)
	unit:NftChangedAbil(AT_CANNOT_DAMAGE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin109001901 Cannot Damage On")

	kEffectMgr:AddAbil(AT_CANNOT_CASTSKILL, 1)
	unit:NftChangedAbil(AT_CANNOT_CASTSKILL, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin109001901 Cannot CastSkill On")

	return 1
end
                                                                                                                          
function Effect_End109001901(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin109001901...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	kEffectMgr:AddAbil(AT_CANNOT_DAMAGE, -1)
	unit:NftChangedAbil(AT_CANNOT_DAMAGE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin109001901 Cannot Damage Off")

	kEffectMgr:AddAbil(AT_CANNOT_CASTSKILL, -1)
	unit:NftChangedAbil(AT_CANNOT_CASTSKILL, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	InfoLog(9, "Effect_Begin109001901 Cannot CastSkill Off")

	
	
	return 1
end

function Effect_Tick109001901(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick109001901 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
