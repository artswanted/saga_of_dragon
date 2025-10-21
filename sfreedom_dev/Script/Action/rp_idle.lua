function Act_RP_Idle_OnCheckCanEnter(actor, action)

	return	true;
end
function Act_RP_Idle_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end	
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end	
	
	local actionID = action:GetID()
	if actor:IsCheckMeetFloor() and actor:IsMeetFloor() and actor:IsMyActor() then
		local kDir = actor:GetDirection()
		if kDir ~= 0 then
			actor:ReserveTransitAction("a_rp_walk", kDir)
			return false
		end
	end
	
	action:SetSlot(1)
	actor:ResetAnimation();	

	local kActorPet = actor:GetMountTargetPet()
	if kActorPet:IsNil() then
		return true
	end
	local kPetAction = kActorPet:GetAction()
	if kPetAction:IsNil() then
		return true
	end
	if kPetAction:GetID() == "a_riding_ground" then
		kPetAction:SetSlot(17)
		kActorPet:PlayCurrentSlot(false)
	end
	
	return true
end

function Act_RP_Idle_OnUpdate(actor, accumTime, frameTime)
	return true
end

function Act_RP_Idle_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end	
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end	
	
	local nextActionName = action:GetID()
	
	if nextActionName ~= "a_idle" and
		nextActionName ~= "a_act" and
		nextActionName ~= "a_walk" and
		nextActionName ~= "a_walk_left" and
		nextActionName ~= "a_walk_right" and
		nextActionName ~= "a_walk_up" and
		nextActionName ~= "a_walk_down" and
		nextActionName ~= "a_run" and
		nextActionName ~= "a_run_left" and
		nextActionName ~= "a_run_right" and
		nextActionName ~= "a_run_up" and
		nextActionName ~= "a_run_down" and
		nextActionName ~= "a_dmg" and
		nextActionName ~= "a_jump" and
		nextActionName ~= "a_trap" and
		nextActionName ~= "a_rp_walk" and
		nextActionName ~= "a_rp_jump"
		and action:GetID() ~= "a_class_promotion_dragonian"
		and action:GetID() ~= "a_class_promotion_human"
	then
		return false
	end
	
	if nextActionName == "a_rp_jump" then
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() then
			return true
		end
		local kPetAction = kActorPet:GetAction()
		if kPetAction:IsNil() then
			return true
		end
		
		if kPetAction:GetID() ~= "a_riding_ground" then
			return false
		end
	end
	return true
end

function Act_RP_Idle_OnCleanUp(actor, action)
end

function Act_RP_Idle_OnClearUpRun(actor)
end
