
-- 저격수 (아트로핀 : a_Atropine) : Level 1
function Effect_Begin150000901(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin150000901...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iNewEffect = 0;	
	local iUseCount = unit:GetAbil(AT_ATROPINE_USE_COUNT); -- 타겟의 현재 아트로핀 누적 횟수를 얻어 온다.

	--InfoLog(9, "Effect_Begin150000901.. Atropine Use Count( " .. iUseCount .." )");

	if iUseCount == 0 then
		iNewEffect = kEffect:GetAbil(AT_EFFECTNUM1); -- 첫번째 사용
	elseif iUseCount == 1 then
		iNewEffect = kEffect:GetAbil(AT_EFFECTNUM2); -- 두번째 사용
	elseif iUseCount == 2 then
		iNewEffect = kEffect:GetAbil(AT_EFFECTNUM3); -- 세번째 사용
	elseif iUseCount >= 3 then
		iNewEffect = kEffect:GetAbil(AT_EFFECTNUM4); -- 네번째 사용
	end

	if iNewEffect ~= 0 then
		--InfoLog(9, "Effect_Begin150000901 AddEffect : "..iNewEffect);
		unit:AddEffect(iNewEffect, 0, actarg, unit);
	end

	return 1
end
                                                                                                                          
function Effect_End150000901(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End150000901...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	return 1
end

function Effect_Tick150000901(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick150000901 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end


function Effect_Begin1500009101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	local iAddCount = kEffect:GetAbil(AT_COUNT);

	--아트로핀 카운터를 증가 시킨다.
	kEffectMgr:AddAbil(AT_ATROPINE_USE_COUNT, iAddCount);
	unit:NftChangedAbil(AT_ATROPINE_USE_COUNT, E_SENDTYPE_NONE);

	local iDeleteEffect = kEffect:GetAbil(AT_EFFECTNUM1);
	if iDeleteEffect ~= 0 then
		unit:DeleteEffect(iDeleteEffect);
	end

	local iUseCount = unit:GetAbil(AT_ATROPINE_USE_COUNT);

	if iUseCount >= 3 then -- 세번째 / 네번째 사용 할때
		local iAdd = kEffect:GetAbil(AT_R_MAX_HP)
		kEffectMgr:AddAbil(AT_R_MAX_HP, iAdd)
		unit:NftChangedAbil(AT_R_MAX_HP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
		
		-- 현재 Hp 감소
		local iHp = unit:GetAbil(AT_HP)
		local iMaxHp = unit:GetAbil(AT_C_MAX_HP)

		--InfoLog(5, "Effect_Begin1500009101 iMaxHp : "..iMaxHp)
		if iHp > iMaxHp then -- MaxHP가 현재 Hp보다 작을 경우 현재 HP도 줄인다
			unit:SetHP(iMaxHp, E_SENDTYPE_BROADALL, kEffect:GetCaster())
		end
	end
	
	return 1;
end

function Effect_End1500009101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	local iAddCount = kEffect:GetAbil(AT_COUNT);
	
	local iUseCount = unit:GetAbil(AT_ATROPINE_USE_COUNT); -- 타겟의 현재 아트로핀 누적 횟수를 얻어 온다.

	if iUseCount >= 3 then -- 세번째 / 네번째 사용
		local iAdd = kEffect:GetAbil(AT_R_MAX_HP)
		kEffectMgr:AddAbil(AT_R_MAX_HP, 0-iAdd)
		unit:NftChangedAbil(AT_R_MAX_HP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
		--InfoLog(5, "Effect_End1500009001...UseCount(" .. iUseCount..")");
	end	


	--아트로핀 카운터를 없앤다.
	kEffectMgr:AddAbil(AT_ATROPINE_USE_COUNT, -iAddCount);
	unit:NftChangedAbil(AT_ATROPINE_USE_COUNT, E_SENDTYPE_NONE);
	--InfoLog(9, "Effect_End1500009001...UseCount(" .. iUseCount..")"..": ".. iAddCount.."");
	
	return 1;
end

function Effect_Tick1500009101(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick1500009001 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

function Effect_Begin1500009201(unit, iEffectNo, actarg)
	return Effect_Begin1500009101(unit, iEffectNo, actarg)
end

function Effect_End1500009201(unit, iEffectNo, actarg)
	return Effect_End1500009101(unit, iEffectNo, actarg)
end

function Effect_Tick1500009201(unit, elapsedtime, effect, actarg)
	return Effect_Tick1500009101(unit, elapsedtime, effect, actarg)
end

function Effect_Begin1500009301(unit, iEffectNo, actarg)
	return Effect_Begin1500009101(unit, iEffectNo, actarg)
end

function Effect_End1500009301(unit, iEffectNo, actarg)
	return Effect_End1500009101(unit, iEffectNo, actarg)
end

function Effect_Tick1500009301(unit, elapsedtime, effect, actarg)
	return Effect_Tick1500009101(unit, elapsedtime, effect, actarg)
end

function Effect_Begin1500009401(unit, iEffectNo, actarg)
	return Effect_Begin1500009101(unit, iEffectNo, actarg)
end

function Effect_End1500009401(unit, iEffectNo, actarg)
	return Effect_End1500009101(unit, iEffectNo, actarg)
end

function Effect_Tick1500009401(unit, elapsedtime, effect, actarg)
	return Effect_Tick1500009101(unit, elapsedtime, effect, actarg)
end