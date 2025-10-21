-- 트랩퍼 ( 스모크 그레네이드 : a_Smoke Granade) : Level 1-5
function Effect_Begin110002901(unit, iEffectNo, actarg)
	return 1
end

function Effect_End110002901(unit, iEffectNo, actarg)
	return 1
end

function Effect_Tick110002901(unit, elapsedtime, effect, actarg)
	return ECT_DOTICK
end

-- 트랩퍼 ( 스모크 그레네이드 설치: a_Smoke Granade Active) : Level 1-5

function Effect_Begin1100029001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin1100029001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	--방어력 증가
	local iAdd = kEffect:GetAbil(AT_R_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	--마법 방어력 감소
	iAdd = kEffect:GetAbil(AT_R_MAGIC_DEFENCE)
	kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	return 1
end

function Effect_End1100029001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End1100029001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	--방어력 감소
	local iAdd = kEffect:GetAbil(AT_R_PHY_DEFENCE)
	kEffectMgr:AddAbil(AT_R_PHY_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	--마법 방어력 증가
	iAdd = kEffect:GetAbil(AT_R_MAGIC_DEFENCE)
	kEffectMgr:AddAbil(AT_R_MAGIC_DEFENCE, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick1100029001(unit, elapsedtime, effect, actarg)
return ECT_DOTICK
end