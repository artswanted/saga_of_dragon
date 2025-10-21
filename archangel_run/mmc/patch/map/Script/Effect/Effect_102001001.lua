
-- a_ThreeWay_Cast : 확산포 캐스팅
function Effect_Begin102001001(unit, iEffectNo, actarg)
	InfoLog(9, "Effect_Begin102001001 --")
	unit:SetAbil(AT_SKILL_EXTENDED, 1, true, true)
	return 1
end

function Effect_End102001001(unit, iEffectNo, actarg)
	InfoLog(9, "Effect_End102001001 --")
	unit:SetAbil(AT_SKILL_EXTENDED, 0, true, true)	
	return 1
end

function Effect_Tick102001001(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick102001001 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end

