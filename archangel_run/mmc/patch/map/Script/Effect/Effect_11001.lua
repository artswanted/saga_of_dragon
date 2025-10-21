
-- 미션 패널티
function Effect_Begin11001(unit, iEffectNo, actarg)
	
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin11001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local penaltyHP = kEffect:GetAbil(AT_PENALTY_HP_RATE)
	if 0 ~= penaltyHP then
		local iHP = unit:GetAbil(AT_HP)
		iHP = iHP + (iHP*penaltyHP/10000)
		unit:SetAbil(AT_HP,iHP,true)
	end
	
	local penaltyMP = kEffect:GetAbil(AT_PENALTY_MP_RATE)
	if 0 ~= penaltyMP then
		local iMP = unit:GetAbil(AT_MP)
		iMP = iMP + (iMP*penaltyMP/10000)
		unit:SetAbil(AT_MP,iMP,true)
	end
	
	local penaltyExp = kEffect:GetAbil(AT_ADD_EXPERIENCE_RATE)
	if 0 ~= penaltyExp then
		unit:AddAbil(AT_ADD_EXPERIENCE_RATE,penaltyExp)
	end
	return 1
end

function Effect_End11001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End11001...Cannot GetEffect(" .. iEffectNo ..")")
		return 0
	end
	
	local penaltyExp = kEffect:GetAbil(AT_ADD_EXPERIENCE_RATE)
	if 0 ~= penaltyExp then
		unit:AddAbil(AT_ADD_EXPERIENCE_RATE, -penaltyExp )
	end
	return 1
end

function Effect_Tick11001(unit, elapsedtime, effect, actarg)
	return ECT_DOTICK
end

