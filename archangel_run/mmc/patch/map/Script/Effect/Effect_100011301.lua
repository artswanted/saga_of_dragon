
-- 숨기 금지
function Effect_Begin100011301(unit, iEffectNo, actarg)
	return 1
end

function Effect_End100011301(unit, iEffectNo, actarg)
	return 1
end

function Effect_Tick100011301(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick100011301 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
