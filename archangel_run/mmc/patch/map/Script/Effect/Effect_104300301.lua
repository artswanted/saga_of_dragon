
--  도둑 공통(클로 블로킹 : a_Claw Blocking) : Level 1-10 PASSIVE
function Effect_Begin104300301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin104300301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local iAdd = kEffect:GetAbil(AT_BLOCK_RATE)
	kEffectMgr:AddAbil(AT_BLOCK_RATE, iAdd)
	unit:NftChangedAbil(AT_BLOCK_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	return 1
end
                                                                                                                          
function Effect_End104300301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End104300301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_BLOCK_RATE)
	kEffectMgr:AddAbil(AT_BLOCK_RATE, iAdd)
	unit:NftChangedAbil(AT_BLOCK_RATE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	
	return 1
end

function Effect_Tick104300301(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick104300301 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
