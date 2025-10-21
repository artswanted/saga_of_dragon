function Act_Mon_Target_Loc_OnCheckCanEnter(actor, action)
end

function Act_Mon_Target_Loc_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local kPacket = action:GetParamAsPacket()
	local iPCount = kPacket:PopInt()
	action:SetParamInt(1,iPCount)
	
	if iPCount == 0 then
		return false
	end
	
	local i = 0
	while i < iPCount do
		action:AddNewGUID(kPacket:PopGuid())
		i = i + 1
	end
	

	local	prevAction = actor:GetAction()
	if prevAction:GetID() ~= "a_jump" then
		actor:Stop()
	end
	
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_Target_Loc_OnCastingCompleted(actor,action)
	else
		if( Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
		
	    ODS("Act_Mon_Target_Loc_OnEnter Start Casting\n");
		
	end
	
	return true
end
function Act_Mon_Target_Loc_OnCastingCompleted(actor,action)	

	Act_Mon_Melee_SetState(actor,action,"FIRE")
	Act_Mon_Target_Loc_Fire(actor,action);
	
end

function Act_Mon_Target_Loc_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--GetActionResultCS(action, actor)

	-- 타겟을 바라보도록 한다.

end

function Act_Mon_Target_Loc_OnUpdate(actor, accumTime, frameTime)


	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if actor:IsAnimationDone() == true then
			return false
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Target_Loc_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	
	return true
end

function Act_Mon_Target_Loc_OnCleanUp(actor, action)
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	-- action:DeleteParamPacket()
end

function Act_Mon_Target_Loc_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	ODS("Act_Mon_Target_Loc_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
	if action:GetActionType()=="EFFECT" or
		actionID == "a_freeze" or
		actionID == "a_reverse_gravity" or
		actionID == "a_blow_up" or
		actionID == "a_blow_up_small" or
		actionID == "a_knock_back" or
		actionID == "a_die" or
		actionID == "a_idle" or
		actionID == "a_run_right" or
		actionID == "a_run_left" or
		actionID == "a_handclaps" then
		return true
	end
	return false 
end

function Act_Mon_Target_Loc_LoadToWeapon(kActor, kAction)
	
	local	kProjectileMan = GetProjectileMan()
	local	kProjectileID = kActor:GetAnimationInfo("PROJECTILE_ID")
	if nil == kProjectileID then
		return nil
	end
	
	local	fProjetileScale = tonumber(kActor:GetAnimationInfo("PROJECTILE_SCALE"))
	if nil  == fProjetileScale then
		fProjetileScale = 1.0
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID, kAction, kActor:GetPilotGuid())
	if kNewArrow:IsNil() then
		return nil
	end
	
	local	kTargetNodeID = kActor:GetAnimationInfo("FIRE_START_NODE")
	if nil == kTargetNodeID then
		kTargetNodeID = "p_ef_heart"
	end
	
	local	fProjectileSpeed = kActor:GetAnimationInfo("PROJECTILE_SPEED")
	if nil ~= fProjectileSpeed then
		fProjectileSpeed = tonumber(fProjectileSpeed)
		kNewArrow:SetSpeed(fProjectileSpeed)
	end
	
	kNewArrow:SetScale(fProjetileScale*kNewArrow:GetScale())
	kNewArrow:LoadToHelper(kActor,kTargetNodeID)
	
	return kNewArrow
end

function Act_Mon_Target_Loc_OnEvent(actor,textKey)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	local	action = actor:GetAction()
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if textKey == "hit" or textKey == "fire" then
	
		local iPCount = action:GetParamInt(1)
		local i = 0
		while i < iPCount do
			local kGuid = action:GetGUIDInContainer(i)
			local kPilot = g_pilotMan:FindPilot(kGuid)
			if kPilot ~= nil and kPilot:IsNil() == false then
				local kArrow = Act_Mon_Target_Loc_LoadToWeapon(actor,action)
				if kArrow ~= nil and kArrow:IsNil() == false then
					kArrow:SetTargetLoc(kPilot:GetActor():GetNodeTranslate("p_ef_head") )
					kArrow:SetTargetGuidFromServer(kGuid)
					kArrow:Fire()
				end
			end
			i = i + 1
		end
		action:ClearTargetList()
	end
	
	return true;
end

function Act_Mon_Target_Loc_OnTargetListModified(actor,action,bIsBefore)
end
