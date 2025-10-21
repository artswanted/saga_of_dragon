function Act_KeepingWater_OnCheckCanEnter( actor, action )
	return	true
end

function Act_KeepingWater_OnEnter( actor, action )
	ODS("Act_KeepingWater_OnEnter\n")
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	-- if( CheckNil(action == nil) ) then return false end
	-- if( CheckNil(action:IsNil()) ) then return false end
	
	actor:FreeMove(true)
	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	return true
end

function Act_KeepingWater_OnUpdate( actor, accumTime, frameTime )
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local action = actor:GetAction()
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local packet = action:GetParamAsPacket()
	if packet~=nil and packet:IsNil() == false then
	
		ODS("Act_KeepingWater_OnUpdate Recv CustomPacket\n")
		
		local bBool = packet:PopBool()
		if true == bBool then
			-- 회복 연출 파티클을 붙여야 한다.
			actor:AttachParticle(999, "char_root", "ef_PVP_ManaStone_boom_01_char_root")
			
			-- 애니메이션을 리셋해야한다.
			action:SetSlot(0)
		else
			-- 처맞았네..
			action:SetSlot(2)
		end
		
		actor:ResetAnimation()
		actor:PlayCurrentSlot()
		action:DeleteParamPacket()
		
	elseif true == actor:IsAnimationDone() then
		local currentSlot = action:GetCurrentSlot()
		if 0 == currentSlot then
			actor:PlayNext()
		elseif 2 == currentSlot then
			action:SetSlot(0)
			actor:ResetAnimation()
			actor:PlayCurrentSlot()
		else
			return false
		end
	end
	
	return true
end

function Act_KeepingWater_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	actor:ReserveTransitAction("a_keeping_water")
end

function Act_KeepingWater_OnLeave(actor, action)
	return true
end
