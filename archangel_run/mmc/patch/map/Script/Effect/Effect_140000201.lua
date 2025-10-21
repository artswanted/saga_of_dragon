
-- Trapper ( 건들지마라!: a_Don’t Touch Me) : Level 1
function Effect_Begin140000201(unit, iEffectNo, actarg)
	--local kEffectMgr = unit:GetEffectMgr()
	--local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	--if kEffect:IsNil() == true then
	--	InfoLog(5, "Effect_Begin140000201...Cannot GetEffect(" .. iEffectNo ..")");
	--	return 0
	--end
	--명중률 증가
	--local iAdd = kEffect:GetAbil(AT_PENETRATION_RATE)
	--kEffectMgr:AddAbil(AT_PENETRATION_RATE, iAdd)
	--unit:NftChangedAbil(AT_PENETRATION_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end
                                                                                                                          
function Effect_End140000201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End140000201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iSkillNo = kEffect:GetAbil(AT_NORMAL_SKILL_ID)
	if iSkillNo <= 0 then
		InfoLog(5, "Effect_End140000201, Cannot Get AT_NORMAL_SKILL_ID")
		return -1
	end
	
	local kNilTarget = Unit()
	local kAResult = NewActionResultVector()
	unit:SkillFire(iSkillNo, kNilTarget, kAResult, actarg, true)

	DeleteActionResultVector(kAResult)	
	
	return 1
end

function Effect_Tick140000201(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick140000201 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
