
-- 공통 : 심각한 중독
function Effect_Begin100010701(unit, iEffectNo, actarg)
	

	return 1
end

function Effect_End100010701(unit, iEffectNo, actarg)

	return 1	
end

function Effect_Tick100010701(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick100010701 Effect=" .. effect:GetEffectNo())
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


