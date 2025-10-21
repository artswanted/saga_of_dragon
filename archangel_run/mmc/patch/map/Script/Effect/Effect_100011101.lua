
-- 공통 : 심한 화상
function Effect_Begin100011101(unit, iEffectNo, actarg)

	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin100011101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_R_MAX_MP)
	local iMp = unit:GetAbil(AT_MP);
	local iMaxMp = unit:GetAbil(AT_C_MAX_MP);

	InfoLog(5, "Effect_Begin100011101  Add : "..iAdd.." iMp : " .. iMp.. " iMaxMp : "..iMaxMp)
		
	local iValue = iMp + (iMaxMp * iAdd / ABILITY_RATE_VALUE);
	local iNewHp = math.max(0, iValue);

	InfoLog(5, "Effect_Begin100011101  iValue : "..iValue.." iNewHp : " .. iNewHp)

	unit:SetMP(iNewHp, E_SENDTYPE_NONE, kEffect:GetCaster())

	return 1
end

function Effect_End100011101(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End100011101...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	return 1	
end

function Effect_Tick100011101(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick100011101 Effect=" .. effect:GetEffectNo())
	local iAdd = effect:GetAbil(AT_MP)
	local iMP = unit:GetAbil(AT_MP)
	local iNewMHP = math.max(0, iMP+iAdd)

	InfoLog(5, "Effect_Tick100011101  iAdd : "..iAdd.." iMP : " .. iMP.." iNewMHP : "..iNewMHP)
	
	-- 나중에 MP 빠지는것을 클라이언트에 연출로 보여줘야하는 일이 생기면 그때 추가
--[[	if iNewMP ~= iMP then
		local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
		kPacket:PushGuid(unit:GetGuid())
		kPacket:PushShort(AT_MP)
		kPacket:PushInt(iNewMP)
		kPacket:PushGuid(effect:GetCaster())
		kPacket:PushInt(effect:GetEffectNo())
		unit:Send(kPacket, E_SENDTYPE_BROADALL)
		DeletePacket(kPacket)	
]]--
		unit:SetMP(iNewMHP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL, effect:GetCaster())
--	end
	return ECT_DOTICK		
end


