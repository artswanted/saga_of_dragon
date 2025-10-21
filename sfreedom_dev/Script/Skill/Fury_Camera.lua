function CreateFuryCamera(actor, fCamDistance)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	if LOCALE.NC_DEVELOP == GetLocale()
	or LOCALE.NC_KOREA == GetLocale() then
		GetMyActor():AttachSound(98751, "defence-monsterin")
		return false
	end

	if nil==fCamDistance or 0==fCamDistance then
		fCamDistance = 180
	end

	local guid = GUID("123")
	guid:Generate();
	
	local pilot = g_pilotMan:NewPilot(guid, 1000041, 0, "NPC")
	
	if pilot == nil or pilot:IsNil() == true then
		return false
	end
	
	local kActor = pilot:GetActor()
	if kActor == nil or kActor:IsNil() == true then
		return false
	end
	kActor:SetHide(true)
	
	if IsSingleMode() == false then
		g_pilotMan:InsertPilot(guid,pilot);
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:AddActor(guid, pilot:GetActor(), actor:GetPos(), 6)
	
	pilot:GetActor():ClearReservedAction()
	local kAction = pilot:GetActor():ReserveTransitAction("a_fury_camera")
	kAction:SetSlot(0)
	kActor:ResetAnimation()
	
	kAction:SetParam(104,actor:GetPilotGuid():GetString())
	kAction:SetParamFloat(114,fCamDistance)
end

function RemoveFuryCamera(actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
end

function Act_Fury_Camera_OnCheckCanEnter(actor,action)
	return	true
end

function Act_Fury_Camera_OnEnter(actor, action)
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	action:SetParamInt(99,0)
	return Act_Mon_Melee_OnEnter(actor, action)
end

function Act_Fury_Camera_OnCastingCompleted(actor,action)	
	return Act_Mon_Melee_OnCastingCompleted(actor,action)	
end

function Act_Fury_Camera_OnUpdate(actor, accumTime, frameTime)
	--Act_Mon_Melee_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(99)
	if 0==iState then
		--SlowMotion
		local	Type = 0
		local	From = 0.004
		local	To = 1
		local	Time = 2.7
		g_world:SetSlowMotion(Type, From, To, Time)
		action:SetParamFloat(0,Time)
		action:SetParamFloat(1,g_world:GetAccumTime());	--	Start Time
		
		--Set Camera
		g_world:SetCameraMode(9,GetMyActor());
		
		local	fMoveTime = 0.6
		local	fCamDistance = action:GetParamFloat(114)		--거리
		local	fCamMoveUp = -40		--위에서
		local	fHorizRotAngle = 0
		local	fVertRotAngle = math.atan(GetCameraTrn():GetZ()/GetCameraTrn():GetY())*180/3.141592;	

		local	kEventCam = g_world:GetCameraModeEvent()
		if kEventCam:IsNil() then
			return	false
		end

		local	kGuid = action:GetParam(104)		--분노하는 Unit Guid
		kEventCam:SetTargetActor(GUID(kGuid));		
		kEventCam:StartMoveCamera(fMoveTime,fCamDistance,fCamMoveUp,fHorizRotAngle,fVertRotAngle);
		
		--NextState
		action:SetParamInt(99,1)
	elseif 1==iState then
		local	kEventCam = g_world:GetCameraModeEvent()
		if kEventCam:IsNil() then
			return	false
		end
		
		local 	fElapsedTime = accumTime - action:GetParamFloat(1)
		if fElapsedTime >= action:GetParamFloat(0) and kEventCam:GetState() == 2 then
			action:SetParamInt(99,2)
		end
	elseif 2==iState then
		local	WaitTime = 0.7
		action:SetParamFloat(0,WaitTime)
		action:SetParamFloat(1,g_world:GetAccumTime());	--	Start Time
		
		action:SetParamInt(99,5)
	elseif 5==iState then
		local 	fElapsedTime = accumTime - action:GetParamFloat(1)
		if fElapsedTime >= action:GetParamFloat(0) then
			action:SetParamInt(99,6)
		end
	elseif 6==iState then
		local	kActor = GetMyActor()
		if kActor == nil or kActor:IsNil() then
			return	false
		end
		
		local	fMoveTime = 0.8
		local	fCamDistance = math.sqrt(GetCameraTrn():GetY()*GetCameraTrn():GetY()+GetCameraTrn():GetZ()*GetCameraTrn():GetZ());
		local	fCamMoveUp = GetCameraRot():GetZ();
		local	fHorizRotAngle = 0
		local	fVertRotAngle = math.atan(GetCameraTrn():GetZ()/GetCameraTrn():GetY())*180/3.141592;	

		local	kEventCam = g_world:GetCameraModeEvent()
		if kEventCam:IsNil() then
			return	false
		end

		kEventCam:SetTargetActor(kActor:GetPilotGuid());		
		kEventCam:StartMoveCamera(fMoveTime,fCamDistance,fCamMoveUp,fHorizRotAngle,fVertRotAngle);
		
		action:SetParamInt(99,7)
	elseif 7==iState then
		local	kEventCam = g_world:GetCameraModeEvent()
		if kEventCam:IsNil() then
			return	false
		end

		if kEventCam:GetState() == 2 then
			action:SetParamInt(99,8)
		end
	elseif 8==iState then
		local	kActor = GetMyActor()
		if kActor == nil or kActor:IsNil() then
			return	false
		end
		
		g_world:SetCameraMode(1,kActor);
		return false
	end

	return true
end

function Act_Fury_Camera_OnCleanUp(actor, action)
	return Act_Mon_Melee_OnCleanUp(actor, action)
end

function Act_Fury_Camera_OnLeave(actor, action)
	RemoveFuryCamera(actor)
	return Act_Mon_Melee_OnLeave(actor, action)
end

function Act_Fury_Camera_OnEvent(actor,textKey)
	return Act_Mon_Melee_OnEvent(actor, textKey)
end

function Act_Fury_Camera_OnTargetListModified(actor,action,bIsBefore)
	return Act_Mon_Melee_OnTargetListModified(actor,action,bIsBefore)
end
