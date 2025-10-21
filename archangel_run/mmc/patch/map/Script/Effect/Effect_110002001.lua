
-- a_Rapidly Shot_Cast : 난사 캐스트 Level 1-5
function Effect_Begin110002001(unit, iEffectNo, actarg)
	unit:SetAbil(AT_SKILL_EXTENDED, 1, true, true)
	return 1
end

function Effect_End110002001(unit, iEffectNo, actarg)
	--InfoLog(9, "Effect_End102001001 --")
	unit:SetAbil(AT_SKILL_EXTENDED, 0, true, true)	
	return 1
end

function Effect_Tick110002001(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick102001001 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

