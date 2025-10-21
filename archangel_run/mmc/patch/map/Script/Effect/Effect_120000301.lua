
-- Wizard ( 체인 라이트닝: a_Chain Lighting) : Level 1-10
function Effect_Begin120000301(unit, iEffectNo, actarg)
	--local kEffectMgr = unit:GetEffectMgr()
	--local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	--if kEffect:IsNil() == true then
	--	InfoLog(5, "Effect_Begin120000101...Cannot GetEffect(" .. iEffectNo ..")");
	--	return 0
	--end
	-- MAX_HP 증가
	--local iAdd = kEffect:GetValue(AT_MAX_HP)
	--kEffectMgr:AddAbil(AT_MAX_HP, iAdd)
	--unit:NftChangedAbil(AT_MAX_HP, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	return 1
end
                                                                                                                          
function Effect_End120000301(unit, iEffectNo, actarg)
	--local kEffectMgr = unit:GetEffectMgr()
	--local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	--if kEffect:IsNil() == true then
	--	InfoLog(5, "Effect_End106300601...Cannot GetEffect(" .. iEffectNo ..")");
	--	return 0
	--end
	-- MAX_HP 증가
	--local iAdd = kEffect:GetAbil(AT_MAX_HP)
	--kEffectMgr:AddAbil(AT_MAX_HP, 0-iAdd)
	--unit:NftChangedAbil(AT_MAX_HP, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
	--local iMaxHP = unit:GetAbil(AT_C_MAX_HP)
	-- 현재HP값이 MAX_HP를 넘지 않도록 조정
	--if unit:GetAbil(AT_HP) > iMaxHP then
	--	unit:SetAbil(AT_HP, iMaxHP, true, false)
	--end
	return 1
end

function Effect_Tick120000301(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick120000301 Effect=" .. effect:GetEffectNo())
	local iActionID = unit:GetActionID()
	if iActionID ~= effect:GetValue() then
		-- 체인라이트닝 발사가 중지되었다면
		return ECT_MUSTDELETE
	end
	local iDecHP = effect:GetAbil(AT_HP)
	local iHP = unit:GetAbil(AT_HP)
	local iNewHP = math.max(0, iHP + iDecHP)
	if iNewHP <= 0 then
		return ECT_MUSTDELETE
	end
	local iDecMP = effect:GetAbil(AT_MP)
	local iMP = unit:GetAbil(AT_MP)
	local iNewMP = math.max(0, iMP + iDecMP)
	if iNewMP <= 0 then
		return ECT_MUSTDELETE
	end
	InfoLog(9, "Effect_Tick120000301 DecHP:" .. iDecHP .. ", NewHP:" .. iNewHP)
	InfoLog(9, "Effect_Tick120000301 DecMP:" .. iDecMP .. ", NewMP:" .. iNewMP)
	--unit:SetAbil(AT_HP, iNewHP, true, false)
	unit:SetHP(iNewHP, E_SENDTYPE_NONE, effect:GetCaster())
	unit:SetAbil(AT_MP, iNewMP, true, false)	
	return ECT_DOTICK
end
