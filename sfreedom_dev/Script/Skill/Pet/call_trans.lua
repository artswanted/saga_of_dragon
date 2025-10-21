function Act_Pet_Call_Trans_OnCheckCanEnter(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local kCallerActor = actor:GetMountTargetPet()
	if kCallerActor:IsNil() then
		return false
	end
	
	local kCallerAction = kCallerActor:GetAction()
	if kCallerAction:IsNil() then
		return false
	end
	
	if actor:IsMeetFloor() == false or (kCallerAction:GetID() ~= "a_rp_idle" and kCallerAction:GetID() ~= "a_rp_walk") then
		return false
	end
	
	return true
end

function Act_Pet_Call_Trans_OnEnter(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kCallerActor = actor:GetCallerActor()
	if kCallerActor:IsNil() or kCallerActor:IsMyActor() == false then
		return false
	end
	if kCallerActor:IsRidingPet() == false then --펫에 탑승중이 아니라면 스킬 사용 불가!!
		g_ChatMgrClient:Notice_Show(GetTextW(451005), 1)
		return false
	end
	action:SetNextActionName("a_riding_ground")	
	CallPetTRANSPORT(actor:GetPilotGuid())

	return true
end

function Act_Pet_Call_Trans_OnCastingCompleted(actor, action)
end

function Act_Pet_Call_Trans_OnFindTarget(actor,action,kTargets)
end

function Act_Pet_Call_Trans_OnUpdate(actor, accumTime, frameTime)
	return false
end

function Act_Pet_Call_Trans_OnCleanUp(actor)
	return true
end

function Act_Pet_Call_Trans_OnLeave(actor, action)
	return true
end

function Act_Pet_Call_Trans_OnEvent(actor,textKey)
	return true
end

