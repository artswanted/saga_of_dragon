-- Creep Idle
creepVel = 100.0

function Act_Creep_Idle_OnEnter(actor, action)
	return true
end

function Act_Creep_Idle_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())	
	local right = actor:GetActionState("a_run_right")
	local left = actor:GetActionState("a_run_left")

	if right ~= left then
		actor:SetNextActionName("a_creep_move")
		return false
	end

	return true
end

function Act_Creep_Idle_OnLeave(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	if action:GetID() == "a_creep_down" then
		actor:ReserveTransitAction("a_creep_up")
	elseif action:GetID() == "a_creep_up" then
		return true
	end
	return false
end


-- Creep Move

function Act_Creep_Move_OnEnter(actor, action)
	return true
end

function Act_Creep_Move_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	local right = actor:GetActionState("a_run_right")
	local left = actor:GetActionState("a_run_left")

	if right > left then
		actor:Walk(false, actor:GetAbil(AT_C_MOVESPEED))
	elseif right < left then
		actor:Walk(true, actor:GetAbil(AT_C_MOVESPEED))	
	else
		actor:SetNextActionName("a_creep_idle")
		return false
	end

	return true
end

function Act_Creep_Move_OnLeave(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	if action:GetID() == "a_creep_down" then
		actor:ReserveTransitAction("a_creep_up")
	elseif action:GetID() == "a_creep_up" then
		return true
	end
	return false
end



-- Creep Down 

function Act_Creep_Down_OnEnter(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	if actor:IsRidingPet() then --라이딩펫: 탑승 중 피격당하면 강제 하차→피격 액션 진행
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end
	actor:Stop()
	actor:SetCreep(true)
	return true
end

function Act_Creep_Down_OnUpdate(actor, accumTime, frameTime)
	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end

function Act_Creep_Down_OnLeave(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	if actor:IsAnimationDone() == false then
		return false
	end
	return true
end

-- Creep up 

function Act_Creep_Up_OnEnter(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	actor:Stop()
	actor:SetCreep(false)
	return true
end

function Act_Creep_Up_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end

function Act_Creep_Up_OnCleanUp(actor, action)
end

function Act_Creep_Up_OnLeave(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	if actor:IsAnimationDone() == false then
		return false
	end
	return true
end


