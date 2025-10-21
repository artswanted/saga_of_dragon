-- Riding 

function Act_Riding_OnEnter(actor, action)
	if actor:IsMeetFloor() == false then
		return false
	end

	pet = actor:GetMyPet()
	if pet:IsNil() == true then
		return false
	end

	actor:RideMyPet(not actor:IsRiding())
	pet:AttachParticle(289, "char_root", "e_special_transform")
	return true
end

function Act_Riding_OnUpdate(actor, accumTime, frameTime)
	if actor:IsRiding() == false then
		actor:ReserveTransitAction("a_idle")
	end
	return false
end
function Act_Riding_OnCleanUp(actor, action)
end

function Act_Riding_OnLeave(actor, action)
	if actor:IsRiding() == true then
		if action:GetID() == "a_ride_idle" then
			return true
		end
	elseif action:GetID() == "a_idle" then
		actor:ClearActionState()
		return true
	end
	return false
end

function Act_Riding_Idle_OnEnter(actor, action)
	local kCurAction = actor:GetAction()

	--right = actor:GetActionState("a_run_right")
	--left = actor:GetActionState("a_run_left")

	-- 0으로 같지 않으면 달리는 액션으로!
	--if right ~= left then
	--	actor:ReserveTransitAction("a_ride_run")
	--	return false
	--end

	if kCurAction:IsNil() == false then
		curActionID = kCurAction:GetID()
		if curActionID == "a_ride_run" or
			(curActionID == "a_idle" and kCurAction:GetParam(19) == "RideMyPet") then
			return true
		end
	end

	return false
end

function Act_Riding_Idle_OnUpdate(actor, accumTime, frameTime)
	pet = actor:GetMyPet()
	if pet:IsNil() == false then
		actor:SetTranslate(pet:GetTranslate())
	end
	
	return true 
end
function Act_Riding_Idle_OnCleanUp(actor, action)
end

function Act_Riding_Idle_OnLeave(actor, action)
	local actionID = action:GetID()
	local pet = actor:GetMyPet()

	if actionID == "a_ride_run" or
		(actionID == "a_idle" and pet:GetAction():GetParam(19) == "RideMyPet") then
		return true
	else
		-- Pet의 Actor를 가져온다.
		if pet:IsNil() == false then
			if action:GetEnable() == true then
				pet:ReserveTransitAction(actionID)
				if actionID == "a_run_right" or
					actionID == "a_run_left" then
					actor:ReserveTransitAction("a_ride_run")
				end 
			else
				pet:UntransitAction(actionID)
			end
		end
		return false
	end
end

function Act_Riding_Run_OnEnter(actor, action)
	if actor:GetAction():GetID() == "a_ride_idle" then
		return true
	end
	return false
end

function Act_Riding_Run_OnUpdate(actor, accumTime, frameTime)
	pet = actor:GetMyPet()
	if pet:IsNil() == false then
		actor:SetTranslate(pet:GetTranslate())
	end

	return true
end
function Act_Riding_Run_OnCleanUp(actor, action)
end

function Act_Riding_Run_OnLeave(actor, action)
	actionID = action:GetID()

	if actionID == "a_ride_idle" or
		actionID == "a_ride" then
		return true
	else
		-- Pet의 Actor를 가져온다.
		pet = actor:GetMyPet()
		if pet:IsNil() == false then
			if action:GetEnable() == true then
				pet:ReserveTransitAction(actionID)
			else
				pet:UntransitAction(actionID)
				if actionID == "a_run_right" or
					actionID == "a_run_left" then
					actor:ReserveTransitAction("a_ride_idle")
				end 
			end
		end
		return false
	end
end
