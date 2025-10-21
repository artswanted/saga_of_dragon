
-- 닌자 (더블 점프 : a_Double Jump) : Level 1 PASSIVE
function Effect_Begin170000201(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin170000201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	kEffectMgr:AddAbil(AT_DOUBLE_JUMP_USE, 1);
	unit:NftChangedAbil(AT_DOUBLE_JUMP_USE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);
	
	return 1
end
                                                                                                                          
function Effect_End170000201(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_End170000201 --")
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End170000201...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	kEffectMgr:AddAbil(AT_DOUBLE_JUMP_USE, -1);
	unit:NftChangedAbil(AT_DOUBLE_JUMP_USE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);
	
	return 1
end

function Effect_Tick170000201(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick170000201 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
