
-- Wizard ( MP ÀüÀÌ : a_MP Transition) : Level 1-10
function Effect_Begin109001701(unit, iEffectNo, actarg)
	return 1
end
                                                                                                                          
function Effect_End109001701(unit, iEffectNo, actarg)
	return 1
end

function Effect_Tick109001701(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick109001701 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
