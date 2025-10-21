
-- 레인저 (카모플라쥬 : a_Camouflage) : Level 1-5
function Effect_Begin110001701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin110001701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- Hidden
	unit:SetAbil(AT_UNIT_HIDDEN, 1)
	unit:SetAbil(AT_UNLOCK_HIDDEN_MOVE, 1)
	InfoLog(9, "Effect_Begin110001701 :" .. unit:GetAbil(AT_UNLOCK_HIDDEN_MOVE))
	
	local kPos = unit:GetPos()
	kEffect:SetActArgInt(ACT_ARG_CUSTOMDATA1, math.floor(kPos:GetX()))
	kEffect:SetActArgInt(ACT_ARG_CUSTOMDATA1+1, math.floor(kPos:GetY()))
	kEffect:SetActArgInt(ACT_ARG_CUSTOMDATA1+2, math.floor(kPos:GetZ()))	
	return 1
end

function Effect_End110001701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End110001701...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- Hidden
	unit:SetAbil(AT_UNIT_HIDDEN, 0)
	unit:SetAbil(AT_UNLOCK_HIDDEN_MOVE, 0)

	return 1
end

function Effect_Tick110001701(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick110001001 Effect=" .. effect:GetEffectNo())
	-- MP소모
	local iDec = effect:GetAbil(AT_MP)
	InfoLog(9, "Effect_Tick110001701 MP:" .. iDec)
	local iMP = unit:GetAbil(AT_MP)
	if iDec ~= 0 then
		local iMin = math.max(0, iMP+iDec)
		unit:SetAbil(AT_MP, iMin, true, false)
		if iMin <= 0 then
			return ECT_MUSTDELETE
		end		
	end

	if unit:GetAbil(AT_UNIT_HIDDEN) <= 0 then
		--InfoLog(9, "Effect_Tick110001701 ECT_MUSTDELETE")
		return ECT_MUSTDELETE
	end
	local kPos = unit:GetPos()
	if math.floor(kPos:GetX()) ~= effect:GetActArgInt(ACT_ARG_CUSTOMDATA1) then
		InfoLog(9, "Effect_Tick110001701 PosX[" .. math.floor(kPos:GetX()) .. "], ActArgX[" .. effect:GetActArgInt(ACT_ARG_CUSTOMDATA1) .. "]")
		return ECT_MUSTDELETE
	end
	if math.floor(kPos:GetY()) ~= effect:GetActArgInt(ACT_ARG_CUSTOMDATA1+1) then
		return ECT_MUSTDELETE
	end
	if math.floor(kPos:GetZ()) ~= effect:GetActArgInt(ACT_ARG_CUSTOMDATA1+2) then
		return ECT_MUSTDELETE
	end
	
	--InfoLog(9, "Effect_Tick110001701 ECT_DOTICK")

	return ECT_DOTICK
end
