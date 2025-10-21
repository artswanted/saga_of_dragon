
-- lavalon(BOSS) -- Lavalon(BOSS) lv_lavalon_breath_l
function Effect_Begin6000902(lavalon, iEffectNo, actarg)
	return Effect_Begin6000900(lavalon, iEffectNo, actarg)
end

function Effect_End6000902(lavalon, iEffectNo, actarg)
	return Effect_End6000900(lavalon, iEffectNo, actarg)
end

function Effect_Tick6000902(lavalon, elapsedtime, effect, actarg)
	Effect_Tick6000900( lavalon, elapsedtime, effect, actarg)
end
