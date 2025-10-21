
-- 공통 : 중독
function Effect_Begin100010601(unit, iEffectNo, actarg)
	--[[
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin100010601...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	local iAdd = kEffect:GetAbil(AT_CANNOT_CASTSKILL)
	unit:AddAbil(AT_CANNOT_CASTSKILL, iAdd)
	unit:NftChangedAbil(AT_CANNOT_CASTSKILL, E_SENDTYPE_SELF)
		
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_PHY_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MAGIC_DEFENCE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_R_MOVESPEED)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_R_ATTACK_SPEED)
	unit:SendAbiles(4, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	]]
	
	return 1
end

function Effect_End100010601(unit, iEffectNo, actarg)

	return 1	
end

function Effect_Tick100010601(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick100010601 Effect=" .. effect:GetEffectNo())
	local iAdd = effect:GetAbil(AT_HP)
	local iHP = unit:GetAbil(AT_HP)
	local iNewHP = math.max(1, iHP+iAdd)
	
	if iNewHP ~= iHP then
		local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
		kPacket:PushGuid(unit:GetGuid())
		kPacket:PushShort(AT_HP)
		kPacket:PushInt(iNewHP)
		kPacket:PushGuid(effect:GetCaster())
		kPacket:PushInt(effect:GetEffectNo())
		unit:Send(kPacket, E_SENDTYPE_BROADALL)
		DeletePacket(kPacket)	
	
		--unit:SetAbil(AT_HP, iNewHP)
		unit:SetHP(iNewHP, E_SENDTYPE_NONE, effect:GetCaster())
	end
	-- HP==1이면 멈추어야 한다.
	if iNewHP <= 1 then
		return ECT_MUSTDELETE
	end
	return ECT_DOTICK	
end


