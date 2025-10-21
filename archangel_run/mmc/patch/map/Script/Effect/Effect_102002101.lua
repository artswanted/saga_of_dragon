
-- a_ThreeWay_Cast : 확산포 캐스팅
function Effect_Begin102002101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin102002101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 마법공격력 증가
	local iAdd = kEffect:GetAbil(AT_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_MAGIC_ATTACK_MAX, iAdd)
	unit:NftChangedAbil(AT_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_MAGIC_ATTACK_MIN, iAdd)
	unit:NftChangedAbil(AT_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_MAGIC_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_MAGIC_ATTACK_MIN)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	
	return 1
end

function Effect_End102002101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End102002101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 마법공격력 증가
	local iAdd = kEffect:GetAbil(AT_MAGIC_ATTACK_MAX)
	kEffectMgr:AddAbil(AT_MAGIC_ATTACK_MAX, 0-iAdd)
	unit:NftChangedAbil(AT_MAGIC_ATTACK_MAX, E_SENDTYPE_NONE)
	kEffectMgr:AddAbil(AT_MAGIC_ATTACK_MIN, 0-iAdd)
	unit:NftChangedAbil(AT_MAGIC_ATTACK_MIN, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_MAGIC_ATTACK_MAX)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_MAGIC_ATTACK_MIN)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_Tick102002101(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick102001001 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

