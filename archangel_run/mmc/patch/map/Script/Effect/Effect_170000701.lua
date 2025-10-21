
-- 닌자( 격! 일섬!: a_Rolling Log Play) : Level 1-10 PASSIVE
function Effect_Begin170000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin170000701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local iDeleteEffectNo = kEffect:GetAbil(AT_EFFECTNUM1);
	unit:DeleteEffect(iDeleteEffectNo);
	InfoLog(5, "Effect_Begin170000701 DeleteEffect"..iDeleteEffectNo.."" )
	return 1
end
                                                                                                                          
function Effect_End170000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End170000701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	return 1
end

function Effect_Tick170000701(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick170000701 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
