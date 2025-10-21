g_EffectEscapeKey = false

function InitEffectEscapeKeyDown()
	g_EffectEscapeKey = false
end

function IsEffectEscapeKeyDown()
	if false == g_EffectEscapeKey then
		if KeyIsDown(KEY_RIGHT) then
			return false
		end
	end

	if true == g_EffectEscapeKey then
		if KeyIsDown(KEY_LEFT) then
			return false
		end
	end

	if KeyIsDown(KEY_LEFT) or KeyIsDown(KEY_RIGHT) then
		g_EffectEscapeKey = not g_EffectEscapeKey
		return true
	end

	return false
end

function Net_EffectEscapeKeyDown(iEffectID)
	if nil~=iEffectID and iEffectID>0 then
		local packet = NewPacket( 13108 ) -- PT_C_M_REQ_EFFECT_ESCAPE_KEYDOWN
		packet:PushInt(iEffectID)
		Net_Send( packet )
		DeletePacket( packet )
	end
end

function ProcessEffectEscapeKeyDown(iEffectID)
	if IsEffectEscapeKeyDown() then
		Net_EffectEscapeKeyDown(iEffectID)
	end
end
