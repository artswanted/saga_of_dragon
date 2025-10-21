
-- 시야 늘려주는 버프
function Effect_Begin2601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin2601...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 시야 증가
	local iAdd = kEffect:GetAbil(AT_DETECT_RANGE)
	kEffectMgr:AddAbil(AT_DETECT_RANGE, iAdd)
	unit:NftChangedAbil(AT_DETECT_RANGE, E_SENDTYPE_NONE)

	iAdd = kEffect:GetAbil(AT_CHASE_RANGE)
	kEffectMgr:AddAbil(AT_CHASE_RANGE, iAdd)
	unit:NftChangedAbil(AT_CHASE_RANGE, E_SENDTYPE_NONE)
	
	return 1
end
                                                                                                                          
function Effect_End2601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End2601...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 시야 감소
	local iAdd = kEffect:GetAbil(AT_DETECT_RANGE)
	kEffectMgr:AddAbil(AT_DETECT_RANGE, 0-iAdd)
	unit:NftChangedAbil(AT_DETECT_RANGE, E_SENDTYPE_NONE)

	iAdd = kEffect:GetAbil(AT_CHASE_RANGE)
	kEffectMgr:AddAbil(AT_CHASE_RANGE, 0-iAdd)
	unit:NftChangedAbil(AT_CHASE_RANGE, E_SENDTYPE_NONE)

	InfoLog(5, "Effect_End2601...Decrease(" .. iAdd ..")")

	return 1
end

function Effect_Tick2601(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick2601 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
