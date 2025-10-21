-- Default 보스 관련 함수

-- Boss의 HP설정
function SetBossHP_Default( kGround, Boss )
	
--	local iPlayerNum = kGround:GetPlayerCount()
	local iPlayerNum = 1
	if 0 < iPlayerNum then
		local iNewHP = Boss:GetAbil(AT_MAX_HP) * iPlayerNum
--		InfoLog(7, "Set Boss HP : " ..iNewHP .. " PlayerNum : " .. iPlayerNum )
		if iNewHP < 0 then
			iNewHP = iNewHP * -1
		end
		Boss:SetAbil( AT_HP, iNewHP, true, true )
		Boss:SetAbil( AT_C_MAX_HP, iNewHP, true, true )
	end
	
	local iGage = Boss:GetAbil(AT_HP_GAGE)
	Boss:SetAbil(AT_MANUAL_DIEMOTION, 101)
	Boss:SetAbil(AT_CURRENT_ACTION, 0, false)
	Boss:SetAbil(AT_C_HP_GAGE, iGage-1, false)
end

-- 보스의 정보를 유저한테 보낸다.
function SendBossMonster( kGround, Boss )
	local emptyGuid = GUID()
	emptyGuid:Empty()
	
	local kPacket = NewPacket( PT_M_C_NFY_BOSSMONSTER )	
	kPacket:PushGuid( Boss:GetGuid() )
	kGround:Broadcast( kPacket, Boss, emptyGuid )
	DeletePacket( kPacket )
end
