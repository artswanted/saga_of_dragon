
-- 공통 : 화상
function Effect_Begin100011001(unit, iEffectNo, actarg)

	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin100011001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_R_MAX_MP)
	kEffectMgr:AddAbil(AT_R_MAX_MP, iAdd)
	unit:NftChangedAbil(AT_R_MAX_MP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	return 1
end

function Effect_End100011001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End100011001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iAdd = kEffect:GetAbil(AT_R_MAX_MP)
	kEffectMgr:AddAbil(AT_R_MAX_MP, 0-iAdd)
	unit:NftChangedAbil(AT_R_MAX_MP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)

	return 1	
end

function Effect_Tick100011001(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick100011001 Effect=" .. effect:GetEffectNo())
	local iAdd = effect:GetAbil(AT_MP)
	local iMP = unit:GetAbil(AT_MP)
	local iNewMHP = math.max(0, iMP+iAdd)
	
	if iNewMP ~= iMP then
		local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
		kPacket:PushGuid(unit:GetGuid())
		kPacket:PushShort(AT_MP)
		kPacket:PushInt(iNewMP)
		kPacket:PushGuid(effect:GetCaster())
		kPacket:PushInt(effect:GetEffectNo())
		unit:Send(kPacket, E_SENDTYPE_BROADALL)
		DeletePacket(kPacket)	
	
		unit:SetAbil(AT_MP, iNewHP)
	end
	return ECT_DOTICK		
end


