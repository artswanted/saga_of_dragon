
-- Wizard (미러 이미지 : a_Mirror Image ) : Level 1-5
function Effect_Begin109002001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin109002001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_BARRIER_100PERECNT_COUNT);
	kEffectMgr:AddAbil(AT_BARRIER_100PERECNT_COUNT, iAdd);
	unit:NftChangedAbil(AT_BARRIER_100PERECNT_COUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);
	
	kEffectMgr:AddAbil(AT_PREV_BARRIER_100PERECNT_COUNT, iAdd);
	unit:NftChangedAbil(AT_PREV_BARRIER_100PERECNT_COUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);
	
	--인형 효과 추가
	local iNewEffectNo = kEffect:GetAbil(AT_EFFECTNUM1);
	unit:AddEffect(iNewEffectNo, iAdd, actarg, unit);
	
	return 1
end

function Effect_End109002001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End109002001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	kEffectMgr:AddAbil(AT_BARRIER_100PERECNT_COUNT, 0-unit:GetAbil(AT_BARRIER_100PERECNT_COUNT));
	unit:NftChangedAbil(AT_BARRIER_100PERECNT_COUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);
	
	kEffectMgr:AddAbil(AT_PREV_BARRIER_100PERECNT_COUNT, 0-unit:GetAbil(AT_PREV_BARRIER_100PERECNT_COUNT));
	unit:NftChangedAbil(AT_PREV_BARRIER_100PERECNT_COUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);

	--인형 효과 삭제
	local iNewEffectNo = kEffect:GetAbil(AT_EFFECTNUM1);
	unit:DeleteEffect(iNewEffectNo);	
		
	return 1
end

function Effect_Tick109002001(unit, elapsedtime, effect, actarg)
	InfoLog(9, "Effect_Tick109002001...1");
	if unit:GetAbil(AT_BARRIER_100PERECNT_COUNT) ~= unit:GetAbil(AT_PREV_BARRIER_100PERECNT_COUNT) then --숫자의 변화가 있을 경우
		InfoLog(9, "Effect_Tick109002001...2");
		local iValue = unit:GetAbil(AT_BARRIER_100PERECNT_COUNT);
		local iPrevValue = unit:GetAbil(AT_BARRIER_100PERECNT_COUNT);

		local kEffectMgr = unit:GetEffectMgr();
		InfoLog(9, "Effect_Tick109002001...5 Value : " ..iValue.." Prev Value : "..iPrevValue.."");
		kEffectMgr:AddAbil(AT_PREV_BARRIER_100PERECNT_COUNT, iValue - iPrevValue);
		unit:NftChangedAbil(AT_PREV_BARRIER_100PERECNT_COUNT, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);

		--인형 개수 변화
		local iNewEffectNo = effect:GetAbil(AT_EFFECTNUM1);
		unit:AddEffect(iNewEffectNo, iValue, actarg, unit);
		InfoLog(9, "Effect_Tick109002001...5 " ..iValue.."");
	end

	if unit:GetAbil(AT_BARRIER_100PERECNT_COUNT) <= 0 then
		InfoLog(9, "Effect_Tick109002001...6");
		return ECT_MUSTDELETE
	end
	InfoLog(9, "Effect_Tick109002001...7");
	return ECT_DOTICK
end


function Effect_Begin1090020001(unit, iEffectNo, actarg)
	return 1;
end

function Effect_End1090020001(unit, iEffectNo, actarg)
	return 1;
end

function Effect_Tick1090020001(unit, elapsedtime, effect, actarg)
	return ECT_DOTICK;
end