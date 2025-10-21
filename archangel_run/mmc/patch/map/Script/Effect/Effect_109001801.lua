
-- Wizard ( 배리어 : a_Barrier) : Level 1-10
function Effect_Begin109001801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin109001801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end


	local iAdd = kEffect:GetAbil(AT_BARRIER_AMOUNT);	
	kEffectMgr:AddAbil(AT_BARRIER_AMOUNT, iAdd);
	unit:NftChangedAbil(AT_BARRIER_AMOUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);
	
	return 1
end
                                                                                                                          
function Effect_End109001801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin109001801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	-- 남아있는 배리어를 0 으로 만들어 버린다.
	kEffectMgr:AddAbil(AT_BARRIER_AMOUNT, 0-unit:GetAbil(AT_BARRIER_AMOUNT));
	unit:NftChangedAbil(AT_BARRIER_AMOUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);

	return 1
end

function Effect_Tick109001801(unit, elapsedtime, effect, actarg)
	local iBarrierAmount = unit:GetAbil(AT_BARRIER_AMOUNT);
	InfoLog(5, "Effect_Begin109001801 iBarrierAmount : "..iBarrierAmount);
	if iBarrierAmount <= 0 then
		return ECT_MUSTDELETE;
	end
	
	return ECT_DOTICK
end
