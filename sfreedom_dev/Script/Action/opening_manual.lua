-- Idle

function Common_ManualOpening_Enter(actor)
	if( CheckNil(nil == actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	if( CheckNil(nil == g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	g_world:SetCameraMode(CMODE_FOLLOW, actor)
	CloseUI("FRM_LOADING_IMG")
	CallUI("FRM_MOVIE_IN")
	--g_pilotMan:SetLockMyInput(true)
	if GATTR_ELEMENT_GROUND_BOSS==g_world:GetAttr() then
		LockPlayerInput(30)	--EPLT_ManualOpening
	end
end

function Common_ManualOpening_Leave(actor)
	if( CheckNil(nil == actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end

	if( CheckNil(nil == g_world) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end

	if GATTR_ELEMENT_GROUND_BOSS==g_world:GetAttr() then
		UnLockPlayerInput(30)	--EPLT_ManualOpening
	end

--	CallEnergyGaugeBig(actor)
    CloseUI("FRM_MOVIE_IN")
	local MyActor = GetMyActor()

	CheckNil(nil == MyActor);
	CheckNil(MyActor:IsNil());

	g_world:SetCameraMode(CMODE_FOLLOW, MyActor)
		
	if false == MyActor:IsNil() then
		MyActor:ReserveTransitAction("a_idle")
	end
--	g_pilotMan:SetLockMyInput(false)
--	g_bEliteWarningUICallReserve = true
	
	if g_iMapMoveStage == 2 then
		local kGroundGuid = GUID()
		kGroundGuid:Set("00000000-0000-0000-0000-000000000000")
		--kGroundGuid:MsgBox("Common_ManualOpening_Leave")
		local packet = NewPacket(49)	--PT_C_M_NFY_MAPLOADED
		packet:PushByte(1)
		packet:PushInt(g_mapNo)		-- SGroundKey::WriteToPacket() 1/2
		packet:PushGuid(kGroundGuid)	-- SGroundKey::WriteToPacket() 2/2
		Net_Send(packet)
		DeletePacket(packet)
	elseif g_iMapMoveStage == 5 then
		g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 1.0, 0.0, 1.0, false, false )
	end
	
--	CallUI("FRM_LOADING_IMG")
--	UI_SetLoadingImage(g_mapNo,true)
end

function Common_ManualOpening_End(actor,action)
	if( CheckNil(nil == actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	if( CheckNil(nil == action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	if( CheckNil(nil == g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

    g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 0.0, 1.0, 1.5, true, false )
	if action ~= nil and action:IsNil() == false then
		action:SetParamInt(81,1)
		action:SetSlot(1)
	end
	if action ~= nil then
		if action:GetParam(15) == "AnimDone" then
			actor:ResetAnimation();
			actor:PlayCurrentSlot();
		end
	end
end

function Act_Opening_Manual_OnEnter(actor, action)
	if( CheckNil(nil == actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	if( CheckNil(nil == action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	if( CheckNil(nil == g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local actorID = actor:GetID()
	local loc = actor:GetNodeWorldPos("char_root")
	loc:SetZ(loc:GetZ() + 0.1)
	action:SetParamInt(82, 0)

    Common_ManualOpening_Enter(actor)
	local zoom = actor:GetAnimationInfoFromAniName("CAMERA_ZOOM_OUT", action:GetCurrentSlotAnimName())
	if false==zoom:IsNil() then
		zoom = tonumber(zoom:GetStr())
		action:SetParamFloat(1, g_world:GetCameraZoom())	--일단 저장
		g_world:SaveCameraPos()
		g_world:SetCameraZoomMax(zoom)
		g_world:SetCameraZoom(1.0)--줌 아웃이니까 최대한 뒤로 땡긴다
		action:SetParamFloat(22,g_world:GetAccumTime())	--줌 아웃된 시간 저장
		action:SetParamFloat(23,1)--현재 줌아웃 된 값 저장
		action:SetParamInt(82, 1)--줌 기능 사용함

		local zoom_in = actor:GetAnimationInfoFromAniName("CAMERA_ZOOM_IN", action:GetCurrentSlotAnimName())
		if false==zoom_in:IsNil() then
			zoom_in = tonumber(zoom_in:GetStr())
			action:SetParamFloat(24, zoom_in/zoom)
		end
	end

	actor:SetCanHit(false)
	if actor:GetAbil(AT_MONSTER_TYPE) == 1 then -- 공중형이다.
		local fHeight = actor:GetAbil(AT_HEIGHT)
		loc:SetZ(loc:GetZ() - fHeight)
	end
	actor:AttachParticleToPoint(50, loc, "e_ef_monster_openning")
	
	action:SetParamFloat(11,g_world:GetAccumTime()); -- Idle 은 Loop 애니여서 IsAnimationDone() 이 무의미해진다. 따라서 흐른 시간으로 종료처리해야한다. 시작시간을 기록.
	if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then	--	Opening 애니가 없을 경우 Idle01 애니로 전환한다.
		action:SetSlot(1);
	end

	local MyActor = GetMyActor();
	if( CheckNil(nil == MyActor) ) then return false end
	if( CheckNil(MyActor:IsNil()) ) then return false end
	
	MyActor:SetMovingDelta(Point3(0,0,0))
	MyActor:SetMovingDir(Point3(0,0,0))

	if EMGRADE_ELITE == actor:GetAbil(AT_GRADE) then
		action:SetParamFloat(9, -0.8)
		actor:LockBidirection(false)
	end

	action:SetParam(15, "")

	return true
end

function Act_Opening_Manual_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(nil == actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	if( CheckNil(nil == g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local action = actor:GetAction()

	if( CheckNil(nil == action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	if action:GetParamInt(81) == 1 then
	    if true == g_world:IsEndShowWorldFocusFilter() then
	        return false
	    end
	    return true
	else
		local zoom_out = action:GetParamInt(82)
		if 0~=zoom_out then
			local zoom_in = action:GetParamFloat(24)
			local zoomspeed = tonumber(actor:GetAnimationInfo("CAMERA_ZOOM_IN_SPEED"))
			if nil~=zoomspeed and 0<zoomspeed then
				local nowzoom = action:GetParamFloat(23)-frameTime*zoomspeed
				--ODS("zoom_out: "..zoom_out..", zoom_in: "..zoom_in..", now: "..nowzoom..", time: "..frameTime*zoomspeed.."\n",false,6482)
				if nowzoom < zoom_in then
					nowzoom = zoom_in
				end
				g_world:SetCameraZoom(nowzoom)
				action:SetParamFloat(23, nowzoom)
			else
				g_world:SetCameraZoom(zoom_in)
				action:SetParamFloat(23, zoom_in)
			end
		end
	end
	
	if false==actor:IsAnimationLoop() then
		if actor:IsAnimationDone() == true and action:GetParam(15) == "" then
			action:SetParam(15, "AnimDone")
			Common_ManualOpening_End(actor,action)
		end
	else
		local fOpenTime = action:GetAbil(AT_MON_OPENING_TIME)
		if fOpenTime==0 then
			fOpenTime = 3000	--기본값
		end
		fOpenTime = fOpenTime/1000.0
		if action:GetParamFloat(11)+fOpenTime <= g_world:GetAccumTime() then
			action:SetParam(15, "AnimDone")
			Common_ManualOpening_End(actor,action)
		end
	end

	local fRot = action:GetParamFloat(9)
	if fRot~=0 then
		actor:SetRotation(fRot, Point3(0,0,1))
		action:SetParamFloat(9,0)
	end

	if KeyIsDown(KEY_ESC) then
	    Common_ManualOpening_End(actor,action)
	end

	return true
end
function Act_Opening_Manual_OnCleanUp(actor, action)
    Common_ManualOpening_Leave(actor)
	if( CheckNil(nil == actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	actor:SetCanHit(true)
end

function Act_Opening_Manual_OnLeave(actor, action)
	if( CheckNil(nil == actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end

--	CheckNil(nil == action);
--	if( CheckNil(action:IsNil()) ) then return false end

--	local nextActionID = action:GetID()
	local prevAction = actor:GetAction()

	CheckNil(nil == prevAction);
	CheckNil(prevAction:IsNil());

	actor:DetachFrom(50)
	actor:RestoreLockBidirection()
	
	if 0~=prevAction:GetParamInt(82) then
		g_world:RestoreCameraPos()
		g_world:SetCameraZoom(prevAction:GetParamFloat(1))
	end
	return true
end

function Act_Opening_Manual_OnEvent(actor,textKey)
	if( CheckNil(nil == actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	if textKey == "effect_00001" or textKey == "effect_00002" then	--마할카 도끼
		QuakeCamera(0.3, 0.7)
	elseif textKey == "mahalka_01" then	-- 마할카 괴성
		QuakeCamera(0.7, 0.7)
	elseif textKey == "DeathMaster_01" then
		QuakeCamera(0.7, 1.0)
	elseif textKey == "cam" then
		local QuakeTime = actor:GetAnimationInfo("QUAKE_TIME")
		if nil ~= QuakeTime and "" ~= QuakeTime then
			local QuakeFactor = actor:GetAnimationInfo("QUAKE_FACTOR")	
			if  nil == QuakeFactor then
				QuakeFactor = 1.0
			else
				QuakeFactor = tonumber(QuakeFactor)
			end
			QuakeCamera(QuakeTime, QuakeFactor)
		end
	elseif textKey == "opening_end" then
		local action = actor:GetAction() 
	
		CheckNil(nil == action);
		if( CheckNil(action:IsNil()) ) then return false end

		if nil~=action and false == action:IsNil() then
			Common_ManualOpening_End(actor, actor:GetAction())
			action:SetParam(15, "opening_end")
		end
	end
end
