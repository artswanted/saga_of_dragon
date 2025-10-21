
-- 닌자 (환영분신 : a_Shadow Copy ) : Level 1-5
function Effect_Begin170000601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin170000601...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_BARRIER_100PERECNT_COUNT);
	kEffectMgr:AddAbil(AT_BARRIER_100PERECNT_COUNT, iAdd);
	unit:NftChangedAbil(AT_BARRIER_100PERECNT_COUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);
	
	kEffectMgr:AddAbil(AT_PREV_BARRIER_100PERECNT_COUNT, iAdd);
	unit:NftChangedAbil(AT_PREV_BARRIER_100PERECNT_COUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);

	kEffectMgr:AddAbil(AT_SHADOW_COPY_USE, 1);
	unit:NftChangedAbil(AT_SHADOW_COPY_USE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);
	InfoLog(5, "Effect_Begin170000601...AT_SHADOW_COPY_USE : " .. unit:GetAbil(AT_SHADOW_COPY_USE)..")");

	
	return 1
end

function Effect_End170000601(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End170000601...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	kEffectMgr:AddAbil(AT_BARRIER_100PERECNT_COUNT, 0-unit:GetAbil(AT_BARRIER_100PERECNT_COUNT));
	unit:NftChangedAbil(AT_BARRIER_100PERECNT_COUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);
	
	kEffectMgr:AddAbil(AT_PREV_BARRIER_100PERECNT_COUNT, 0-unit:GetAbil(AT_PREV_BARRIER_100PERECNT_COUNT));
	unit:NftChangedAbil(AT_PREV_BARRIER_100PERECNT_COUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);

	kEffectMgr:AddAbil(AT_SHADOW_COPY_USE, 0-unit:GetAbil(AT_SHADOW_COPY_USE));
	unit:NftChangedAbil(AT_SHADOW_COPY_USE, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);
	InfoLog(5, "Effect_End170000601...AT_SHADOW_COPY_USE : " .. unit:GetAbil(AT_SHADOW_COPY_USE)..")");

	return 1
end

function Effect_Tick170000601(unit, elapsedtime, effect, actarg)
	--InfoLog(9, "Effect_Tick170000601...1");
	if unit:GetAbil(AT_BARRIER_100PERECNT_COUNT) ~= unit:GetAbil(AT_PREV_BARRIER_100PERECNT_COUNT) then --숫자의 변화가 있을 경우
		--InfoLog(9, "Effect_Tick170000601...2");
		local iValue = unit:GetAbil(AT_BARRIER_100PERECNT_COUNT);
		local iPrevValue = unit:GetAbil(AT_BARRIER_100PERECNT_COUNT);

		local kEffectMgr = unit:GetEffectMgr();
		--InfoLog(9, "Effect_Tick170000601...5 Value : " ..iValue.." Prev Value : "..iPrevValue.."");
		kEffectMgr:AddAbil(AT_PREV_BARRIER_100PERECNT_COUNT, iValue - iPrevValue);
		unit:NftChangedAbil(AT_PREV_BARRIER_100PERECNT_COUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);
	end

	if unit:GetAbil(AT_BARRIER_100PERECNT_COUNT) <= 0 then
		--InfoLog(9, "Effect_Tick170000601...6");
		return ECT_MUSTDELETE
	end
	--InfoLog(9, "Effect_Tick170000601...7");
	return ECT_DOTICK
end
