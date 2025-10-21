
-- 메이지 (감염 : a_Disease) : Level 1-10
function Effect_Begin107000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin107000701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local iPer = kEffect:GetAbil(AT_PERCENTAGE2)
	if RAND(ABILITY_RATE_VALUE) < iPer then
		kEffect:SetValue(0) -- Effect 실패
	else
		kEffect:SetValue(1) -- Effect 성공
		-- MaxHP 감소
		local iAdd = kEffect:GetAbil(AT_R_MAX_HP)
		kEffectMgr:AddAbil(AT_R_MAX_HP, iAdd)
		unit:NftChangedAbil(AT_R_MAX_HP, E_SENDTYPE_NONE)
		-- 현재 Hp 감소
		local iHp = unit:GetAbil(AT_HP)
		local iMaxHp = unit:GetAbil(AT_C_MAX_HP)
		if iHp > iMaxHp then -- MaxHP가 현재 Hp보다 작을 경우 현재 HP도 줄인다
			unit:SetHP(iMaxHp, E_SENDTYPE_NONE, kEffect:GetCaster())
		end

		-- MaxMP 감소
		local iAdd = kEffect:GetAbil(AT_R_MAX_MP)
		kEffectMgr:AddAbil(AT_R_MAX_MP, iAdd)
		unit:NftChangedAbil(AT_R_MAX_MP, E_SENDTYPE_NONE)

		-- 현재 Mp 감소
		local iMp = unit:GetAbil(AT_MP)
		local iMaxMp = unit:GetAbil(AT_C_MAX_MP)
		if iMp > iMaxMp then -- MaxMp가 현재 Mp보다 작을 경우 현재 Mp도 줄인다
			unit:SetMP(iMaxMp, E_SENDTYPE_NONE, kEffect:GetCaster())
		end
		
		local kActarg = NewActArg()
		kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_MAX_HP)
		kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MAX_MP)
		unit:SendAbiles(2, kActarg, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
		DeleteActArg(kActarg)
		local kActarg2 = NewActArg()
		kActarg2:SetInt(ACT_ARG_CUSTOMDATA1, AT_HP)
		kActarg2:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_MP)
		unit:SendAbiles(2, kActarg2, E_SENDTYPE_SELF)
		DeleteActArg(kActarg2)		
	end
	return 1
end
                                                                                                                          
function Effect_End107000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End107000701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iValue = kEffect:GetValue()
	-- Effect가 성공(GetValue() > 0) 일때 만 효과를 다시 돌린다.
	if iValue > 0 then
		-- MaxHP 감소
		local iAdd = kEffect:GetAbil(AT_R_MAX_HP)
		kEffectMgr:AddAbil(AT_R_MAX_HP, 0-iAdd)
		unit:NftChangedAbil(AT_R_MAX_HP, E_SENDTYPE_NONE)
		-- MaxMP 감소
		local iAdd = kEffect:GetAbil(AT_R_MAX_MP)
		kEffectMgr:AddAbil(AT_R_MAX_MP, 0-iAdd)
		unit:NftChangedAbil(AT_R_MAX_MP, E_SENDTYPE_NONE)
		
		local kActarg = NewActArg()
		kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_MAX_HP)
		kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MAX_MP)
		unit:SendAbiles(2, kActarg, E_SENDTYPE_SELF+E_SENDTYPE_EFFECTABIL)
		DeleteActArg(kActarg)	
	end
	return 1
end

function Effect_Tick107000701(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick107000701 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
