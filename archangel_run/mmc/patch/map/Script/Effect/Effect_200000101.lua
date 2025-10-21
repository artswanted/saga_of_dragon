
--a_Guild_Power Action
function Effect_Begin200000101(unit, iEffectNo, actarg)
	return Effect_Begin105300801(unit, iEffectNo, actarg)
end
                                                                                                                          
function Effect_End200000101(unit, iEffectNo, actarg)
	return Effect_End105300801(unit, iEffectNo, actarg)
end

function Effect_Tick200000101(unit, elapsedtime, effect, actarg)
	return Effect_Tick105300801(unit, elapsedtime, effect, actarg)
end
