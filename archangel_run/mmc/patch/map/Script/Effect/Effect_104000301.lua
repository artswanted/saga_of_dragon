
-- Thief (독 살포 : Venom) : Level 1-5
function Effect_Begin104000301(unit, iEffectNo, actarg)
	return 1
end

function Effect_End104000301(unit, iEffectNo, actarg)
	return 1
end

function Effect_Tick104000301(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick104000301 Effect=" .. effect:GetEffectNo())
	local iHP = unit:GetAbil(AT_HP)
	local iDec = effect:GetAbil(AT_HP)
	local iNew = iHP + iDec
	InfoLog(9, "Effect_Tick104000301 iHP:" .. iHP .. ", iDec:" .. iDec)
	
	local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
	kPacket:PushGuid(unit:GetGuid())
	kPacket:PushShort(AT_HP)
	kPacket:PushInt(iNew)
	kPacket:PushGuid(effect:GetCaster())
	kPacket:PushInt(effect:GetEffectNo())
	unit:Send(kPacket, E_SENDTYPE_BROADALL)
	DeletePacket(kPacket)	
	if iNew <= 0 then
		-- HP가 0이 되려고 하면 Effect는 종료 되어야 한다.
		--unit:SetAbil(AT_HP, 0, true, true)
		--unit:SetTarget(effect:GetCaster())
		unit:SetHP(0, E_SENDTYPE_NONE, effect:GetCaster())
		return ECT_MUSTDELETE
	end

	--unit:SetAbil(AT_HP, iNew, true, true)	-- iDec값은 음수값이므로
	unit:SetHP(iNew, E_SENDTYPE_NONE, effect:GetCaster())
	return ECT_DOTICK
end

