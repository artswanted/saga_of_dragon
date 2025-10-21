
-- 암살자 (섀도우 워크: a_Shadow Walk) : Lv 1-10 (Toggle)
function Effect_Begin104301301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin104301301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	-- 이동속도 감소
	local iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	-- 숨기
	unit:SetAbil(AT_UNIT_HIDDEN, 1)
	InfoLog(9, "Effect_Begin104301301 AT_UNIT_HIDDEN:" .. unit:GetAbil(AT_UNIT_HIDDEN))
	
	return 1
end
                                                                                                                          
function Effect_End104301301(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End104301301...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	-- 이동속도 감소
	local iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	-- 숨기
	unit:SetAbil(AT_UNIT_HIDDEN, 0)
	InfoLog(9, "Effect_End104301301 AT_UNIT_HIDDEN:" .. unit:GetAbil(AT_UNIT_HIDDEN))
	
	return 1
end

function Effect_Tick104301301(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick104301301 Effect=" .. effect:GetEffectNo())
	-- MP 소모
	local iNeedMP = effect:GetAbil(AT_MP)
	local iMP = unit:GetAbil(AT_MP)
	if iMP > 0-iNeedMP then
		unit:SetAbil(AT_MP, iMP+iNeedMP, true, false)
	else
		return ECT_MUSTDELETE
	end
	
	if unit:GetAbil(AT_UNIT_HIDDEN) == 0 then
		return ECT_MUSTDELETE
	end	

	return ECT_DOTICK
end
