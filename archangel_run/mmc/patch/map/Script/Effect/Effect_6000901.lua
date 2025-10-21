
-- lavalon(BOSS) -- Lavalon(BOSS) lv_lavalon_breath_l
function Effect_Begin6000901(lavalon, iEffectNo, actarg)
	return Effect_Begin6000900(lavalon, iEffectNo, actarg)
end

function Effect_End6000901(lavalon, iEffectNo, actarg)
	return Effect_End6000900(lavalon, iEffectNo, actarg)
end

function Effect_Tick6000901(lavalon, elapsedtime, effect, actarg)
	Effect_Tick6000900( lavalon, elapsedtime, effect, actarg)
end
