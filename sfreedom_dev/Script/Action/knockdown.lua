-- sit_down 

function Act_Knockdown_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if( CheckNil(nil==g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	if actor:IsRidingPet() then --라이딩펫: 탑승 중 피격당하면 강제 하차→피격 액션 진행
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end

	actor:Stop()
	actor:BackMoving(true)
	actor:ToLeft(not actor:IsToLeft())
	action:SetParamFloat(0, g_world:GetAccumTime())
	return true
end

function Act_Knockdown_OnUpdate(actor, accumTime, frameTime)
	action = actor:GetAction()
	curSlot = action:GetCurrentSlot()

	-- knockback
	if curSlot == 0 then
		if accumTime - action:GetParamFloat(0) <= 0.5 then
			dir = DIR_RIGHT
			if actor:IsToLeft() == true then
				dir = DIR_LEFT
			end
			actor:Walk(dir, 130)
		elseif accumTime - action:GetParamFloat(0) >= 0.8 and 
			actor:IsMeetFloor() == true then
			action:SetParam(1, "D")
			actor:PlayNext()
			actor:Stop()
		end
	-- handspring
	elseif actor:IsAnimationDone() == true then
		return false
	end

	return true
end

function Act_Knockdown_OnCleanUp(actor, action)
end

function Act_Knockdown_OnLeave(actor, action)
	if action:GetID() == "a_die" or
		actor:GetAction():GetParam(1) == "D" then
		actor:ToLeft(not actor:IsToLeft())
		actor:BackMoving(false)
		return true
	end
		
	return false
end
