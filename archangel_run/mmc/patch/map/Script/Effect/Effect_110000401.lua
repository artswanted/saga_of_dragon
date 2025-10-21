-- 궁수 (모서리로 찍으세요 : a_Bleeding) : Level 1-10
function Effect_Begin110000401(unit, iEffectNo, actarg)
	return 1
end

function Effect_End110000401(unit, iEffectNo, actarg)

	return 1
end

function Effect_Tick110000401(unit, elapsedtime, kEffect, actarg)
	local iDec = kEffect:GetAbil(AT_HP)
	local iHP = unit:GetAbil(AT_HP)
	local iMin = math.max(0, iHP+iDec)
	InfoLog(9, "Effect_Tick110000401 HP:" .. iHP .. ", iDec:" .. iDec)
	--unit:SetAbil(AT_HP, iMin, false, false)

	local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
	kPacket:PushGuid(unit:GetGuid())
	kPacket:PushShort(AT_HP)
	kPacket:PushInt(iMin)
	kPacket:PushGuid(kEffect:GetCaster())
	kPacket:PushInt(kEffect:GetEffectNo())
	unit:Send(kPacket, E_SENDTYPE_BROADALL)
	DeletePacket(kPacket)
	unit:SetHP(iMin, E_SENDTYPE_NONE, kEffect:GetCaster())
	if iMin == 0 then
		return ECT_MUSTDELETE
	end 
	return ECT_DOTICK
end