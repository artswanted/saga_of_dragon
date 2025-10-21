-- transform

function Act_Transform_OnEnter(actor, action)
	actor:Stop()
	
	if actor:GetID() == "c_bruce" then
		action:SetParam(0, "0")
	end
	
	action = actor:GetAction();
	action:ClearGUIDContainer();

	return true
end

function Act_Transform_OnUpdate(actor, accumTime, frameTime)
	OriginalActor = actor:GetOriginalActor();
	if actor:IsAnimationDone() == true then
		return false
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	if actor:GetID() == "c_bumpercar" then
		
		action = actor:GetAction();
		
		actor:Walk(actor:IsToLeft(),200);
		actorTarget = g_world:FindTarget(actor, 2, 30.0)
		if actorTarget:IsNil() == false and action:CheckGUIDExist(actorTarget:GetPilotGuid())==false then
			action:AddNewGUID(actorTarget:GetPilotGuid());	--	때린 놈을 기억해뒀다가, 다시 때리지 않도록 한다.
			actorTarget:ReserveTransitAction("a_dmg")
			actorTarget:AddVelocity(Point3(0, 0, 300))
			
			if OriginalActor:IsNil() == false  and OriginalActor:IsMyActor() == true then
				Net_ReqDamage(actor, actorTarget, 1020)
			end
			
		end
		
	end
	
	if actor:GetID() == "c_bruce" then
		action = actor:GetAction()

		now = action:GetParam(0)
		now = frameTime + now

		if now > 1.0 and now < 1.6 then
			actorTarget = g_world:FindTargetWidely(actor:GetTranslate(), actor:GetLookingDir(), 2, 110.0, 110.0)
			if actorTarget:IsNil() == false and action:CheckGUIDExist(actorTarget:GetPilotGuid())==false then
				action:AddNewGUID(actorTarget:GetPilotGuid());	--	때린 놈을 기억해뒀다가, 다시 때리지 않도록 한다.
				actorTarget:ToLeft(not actor:IsToLeft())
				actorTarget:FindPathNormal()
				actorTarget:ReserveTransitAction("a_knock_back")
				actorTarget:AttachParticle(12, "p_ef_heart", "e_dmg")	
				actorTarget:SetShakeInPeriod(5, 200)
				QuakeCamera(0.5, 1.0)
				if OriginalActor:IsNil() == false and OriginalActor:IsMyActor() == true then
				
				--	Net_ReqDamage(actor, actorTarget, 1019)
				end
			end
		end

		action:SetParam(0, now)
	end

	return true
end

function Act_Transform_OnLeave(actor, action)
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end

	if actor:IsMyActor() == false then

		OriginalActor = actor:GetOriginalActor();
		if OriginalActor:IsNil() ==false then
			if OriginalActor:IsMyActor()==true then OriginalActor:SetCameraFocus();	end
			OriginalActor:SetTranslate(actor:GetTranslate());
		end
	
		g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
		return	true;
	end
	
	if actor:IsAnimationDone() == false then
		return false
	end

	OriginalActor = actor:GetOriginalActor();
	if OriginalActor:IsNil() ==false then
		if OriginalActor:IsMyActor()==true then OriginalActor:SetCameraFocus();	end
		OriginalActor:SetTranslate(actor:GetTranslate());
	end
	
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
	return false 
end

