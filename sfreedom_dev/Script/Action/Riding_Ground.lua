-- Idle

function Act_Riding_Ground_OnEnter(actor, action)
--	CheckNil(actor == nil);
--	CheckNil(actor:IsNil());
--	CheckNil(action == nil);
--	CheckNil(action:IsNil());
	
--	action:SetDoNotBroadCast(true);	
--	actor:MakePetActionQueue(-1)

	local kMyPilot = actor:GetPilot()
	if kMyPilot:IsNil() then return true end
	local kMyUnit = kMyPilot:GetUnit()
	if kMyUnit:IsNil() then return true end
	local kCallerPilot = g_pilotMan:FindPilot(kMyUnit:GetCaller())
	if kCallerPilot:IsNil() then return true end
	local kCallerActor = kCallerPilot:GetActor()
	if kCallerActor:IsNil() then return true end
	
--	actor:SetMovingDir(kCallerActor:GetMovingDir());
	--actor:FindPathNormal();

	action:SetSlot(17)
	actor:PlayCurrentSlot(false)

	return true
end

function Act_Riding_Ground_OnUpdate(actor, accumTime, frameTime)
	local kMyPilot = actor:GetPilot()
	if kMyPilot:IsNil() then return true end
	local kMyUnit = kMyPilot:GetUnit()
	if kMyUnit:IsNil() then return true end
	local kCallerPilot = g_pilotMan:FindPilot(kMyUnit:GetCaller())
	if kCallerPilot:IsNil() then return true end
	local kCallerActor = kCallerPilot:GetActor()
	if kCallerActor:IsNil() then return true end
	
	local action = actor:GetAction()
	if action == nil or action:IsNil() then return true end

	local kCallerAction = kCallerActor:GetAction()
	if kCallerAction:IsNil() then return true end
	
	if kCallerActor:IsRidingPet() == false then
		kCallerActor:MountPet()
	end

	if actor:IsAnimationDone() and kCallerAction:GetID() ~= "a_rp_walk" and kCallerAction:GetID() ~= "a_rp_jump" then
		action:SetSlot(17)
		actor:PlayCurrentSlot(false)
	end

	if actor:IsMeetFloor() == false or (kCallerAction:GetID() ~= "a_rp_idle" and kCallerAction:GetID() ~= "a_rp_walk") then
		local kWnd = GetUIWnd("SFRM_TRANSTOWER")
		if kWnd:IsNil() == false then
			local kWndByPet = kWnd:GetControl("FRM_CALLER_IS_PET") --펫스킬에 의해 순간이동 UI 창이 떠있다면
			if kWndByPet:IsNil() == false and kWndByPet:IsVisible() == true then
				kWnd:Close() --창을 지워준다
			end
		end
	end


	return true
end
function Act_Riding_Ground_OnCleanUp(actor, action)
end

function Act_Riding_Ground_OnLeave(actor, action)
	local nextActionName = action:GetID()
	if nextActionName ~= "a_trace_ground" and nextActionName ~= "a_trace_ground2" and
		nextActionName ~= "a_call_trans" and nextActionName ~= "a_pet_melee_01" and
		nextActionName ~= "a_pet_melee_02" then
		return false
	end
	return true
end
