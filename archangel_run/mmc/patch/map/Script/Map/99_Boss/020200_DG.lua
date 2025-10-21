function Init_020200( kGround )
	return true
end

--function Ready_020200( kGround )
--    kGround:ActivateMonsterGenGroup(false, 1)
--	return true
--end

--function Update_020200( kGround )
--	return true
--end

function Start_020200( kGround )
--	local kDecorator = kGround:GetDecorator()
--	if kDecorator:IsNil() == false then
--		kDecorator:ReserveStage(1)
--	end
	return true
end

function Ready_020200_RecvStart( kGround, kUnit )
	return true
end

