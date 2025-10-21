function Init_01503( kGround )
	return true
end

function Ready_01503( kGround )
--	InfoLog( 9, "9010503 Ready Ground")
--	kGround:ActivateMonsterGenGroup(false, 1)
	return true
end

function Update_01503( kGround )
--	InfoLog( 9, "9010503 Ready Tick!!")
	return true
end

function Start_01503( kGround )
  kGround:ActivateMonsterGenGroup(false, 1)
--	local Parel = kGround:GetBossMonster(M_PARELBOSS)
--	Parel:SetAbil( AT_MONSTER_APPEAR,1, false)
--	Parel:SetState(US_IDLE)
	return true
end

function Ready_01503_RecvStart( kGround, kUnit )
	return true
end

