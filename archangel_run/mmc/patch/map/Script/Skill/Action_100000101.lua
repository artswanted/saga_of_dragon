
-- Action (a_Resurrection_01)
function Action_Fire11101(caster, skillnum, iStatus,arg)
	InfoLog(9, "Action_Fire11101 --")
	-- 살아 날수 있는가 검사한다.
	local iAlive = caster:GetAbil(AT_REVIVED_BY_OTHER)
	if iAlive > 0 then
		caster:SetAbil(AT_REVIVED_BY_OTHER, 0)
		
		local iHP = caster:GetAbil(AT_HP_RESERVED)
		local iMP = caster:GetAbil(AT_MP_RESERVED)
		caster:Alive(4, E_SENDTYPE_BROADALL, iHP, iMP)
		caster:SetAbil(AT_HP_RESERVED, 0)
		caster:SetAbil(AT_MP_RESERVED, 0)
	end

	return 0	-- EActionR_Success
end

-- Action (a_run)
function Action_Fire100005426(caster, skillnum, iStatus,arg)
	if caster:GetAbil(AT_UNLOCK_HIDDEN_MOVE) > 0 then
		-- 움직이면 Hidden 상태 해제
		caster:SetAbil(AT_UNIT_HIDDEN, 0)
	end
	return 0	-- EActionR_Success
end

-- Action (a_dash)
function Action_Fire100005427(caster, skillnum, iStatus,arg)
	if caster:GetAbil(AT_UNLOCK_HIDDEN_MOVE) > 0 then
		-- 움직이면 Hidden 상태 해제
		caster:SetAbil(AT_UNIT_HIDDEN, 0)
	end

	return 0	-- EActionR_Success
end
