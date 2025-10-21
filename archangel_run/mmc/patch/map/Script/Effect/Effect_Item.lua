
-- ItemEffect 에 의한 효과
function Effect_Begin12001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin12001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iCount = 0
	local kActarg = NewActArg()
	local iAbilArray = { AT_R_MAX_HP, AT_R_MAX_MP, AT_STR, AT_INT, AT_CON, AT_DEX }
	local iChangedArray = {}
	for index=1, table.getn(iAbilArray) do
		local iAbil = iAbilArray[index]
		local iAdd = kEffect:GetAbil(iAbil)
		if iAdd ~= 0 then
			kEffectMgr:AddAbil(iAbil, iAdd)
			unit:NftChangedAbil(iAbil, E_SENDTYPE_NONE)
			kActarg:SetInt(ACT_ARG_CUSTOMDATA1+iCount, iAbil)
			iCount = iCount + 1
		end
	end
	if iCount > 0 then
		unit:SendAbiles(iCount, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	end

	DeleteActArg(kActarg)	
	return 1
end

function Effect_End12001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin12001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iCount = 0
	local kActarg = NewActArg()
	local iAbilArray = { AT_R_MAX_HP, AT_R_MAX_MP, AT_R_STR, AT_R_INT, AT_R_CON, AT_R_DEX }
	local iChangedArray = {}
	for index=1, table.getn(iAbilArray) do
		local iAbil = iAbilArray[index]
		local iAdd = kEffect:GetAbil(iAbil)
		if iAdd ~= 0 then
			kEffectMgr:AddAbil(iAbil, 0-iAdd)
			unit:NftChangedAbil(iAbil, E_SENDTYPE_NONE)
			kActarg:SetInt(ACT_ARG_CUSTOMDATA1+iCount, iAbil)
			iCount = iCount + 1
		end
	end
	if iCount > 0 then
		unit:SendAbiles(iCount, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	end

	DeleteActArg(kActarg)	
	return 1
end

function Effect_Tick12001(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick12001 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK	
end

