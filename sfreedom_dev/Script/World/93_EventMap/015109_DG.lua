ODS("DoFile_015109\n",false,1509)
g_iStage = 0
g_fBossOpeningTime = 0.0

function Init_015109(world)
	ODS("Init_015109\n",false,1509)
	g_iStage = 0
end

function Ready_015109(world)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if 0~=g_fBossOpeningTime then
		return
	end
	ODS("Ready_015109\n",false,1509)
	LockPlayerInputMove(8) -- for lock
	LockPlayerInput(8)
	CloseAllUI()

	CallUI("FRM_Lost")
	g_iStage = 1
	g_fBossOpeningTime = 10.0
	g_fParelNowTime = g_world:GetAccumTime()

	-- ESC Script 등록
	RegistESCScript( "OnEsc_015109Opening" )
	StopBgSound()	--배경음 끄기
end

function CreateOpeningActor_015109()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	ODS("CreateOpeningActor\n", false, 1509)
	local guid = GUID("123")
	guid:Generate();
	
	local pilot = g_pilotMan:NewPilot(guid, 6105100, 0,"NPC")
	
	if pilot == nil or pilot:IsNil() == true then
		return true
	end
	
	local kActor = pilot:GetActor()
	if kActor == nil or kActor:IsNil() == true then
		return true
	end
	
	if IsSingleMode() == false then
		g_pilotMan:InsertPilot(guid,pilot);
	end

	local	kSpawnPos = Point3(0, 0, 4000)
	
	kActor:FreeMove(true)
	kActor:SetUseLOD(false)
	kActor:SetUseSmoothShow(false)
	g_world:AddActor(guid, pilot:GetActor(), kSpawnPos, 6)
	
	pilot:GetActor():ClearReservedAction()
	local kAction = pilot:GetActor():ReserveTransitAction("a_opening_movie")
	kAction:SetSlot(0)
	kActor:ResetAnimation();

	pilot:GetActor():ClearActionState()
	pilot:SetAbil(AT_HP, 1000000);
	pilot:SetAbil(AT_UNIT_SIZE,2);
	pilot:SetAbil(AT_DAMAGEACTION_TYPE,0);
	pilot:SetAbil(AT_NOT_SEE_PLAYER, 0)
	pilot:SetAbil(AT_DAMAGEACTION_TYPE, 0)	
	
	g_world:SetCameraModeMovie2(kActor, 2, 1000000, "nop()")
end

function RemoveOpeningActor_010509()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kActor = g_pilotMan:FindActorByClassNo(6105100);
	
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	g_world:RemoveActorOnNextUpdate(kActor:GetPilotGuid())
	g_world:RemoveCameraAM()
	
	ODS("RemoveOpeningActor\n", false, 1509)
	return true
end

function Update_015109()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if 0 == g_iStage then
		return false
	end

	local fDeltaTime = g_world:GetAccumTime() - g_fParelNowTime

	if 1 == g_iStage then
	    CallUI("FRM_MOVIE_IN")
		
		CreateOpeningActor_015109()

		CallMapMove(true)
		g_fParelNowTime = g_world:GetAccumTime()
		g_iStage = 2
		ODS("Update_015109 g_iStage = 2 " ..fDeltaTime.."\n",false,1509)
	elseif 2 == g_iStage and fDeltaTime > 1 then
		g_iStage = 3
		ODS("Update_015109 g_iStage = 3 " ..fDeltaTime.."\n",false,1509)
	elseif 3 == g_iStage and fDeltaTime > 4 then
		--g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 0.0, 1.0, 3.0, true )
		g_iStage = 4
		ODS("Update_015109 g_iStage = 4 " ..fDeltaTime.."\n",false,1509)
	elseif 4 == g_iStage and fDeltaTime >= g_fBossOpeningTime  then
		OnEsc_015109Opening()
		ODS("Update_015109 OnEsc_015109Opening " ..fDeltaTime.."\n",false,1509)
	elseif 5 == g_iStage then
		ODS("Update_015109 g_iStage = 5 " ..fDeltaTime.."\n",false,1509)
		g_iStage = 0
	    CloseUI("FRM_MOVIE_IN")
		UI_DefaultActivate( true )
		UnLockPlayerInputMove(8) -- for  lock
		UnLockPlayerInput(8)
		CallMapMove(false)
		g_world:SetCameraZoom(1.0) -- 줌최대뒤로
		ClearESCScript()
		return false
	end
	--ODS("Delta : "..fDeltaTime.."\n",false,1509)
	return true
end

function Start_015109(world)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	ODS("Start_015109\n",false,1509)
	OnRecoveryMiniQuest(false)

	-- 사운드
	PlayBgSound(0)
	--g_world:DetachSound("Mutisha_Opening")
	g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 1.0, 0.0, 3.0, false )
	g_iStage = 5
	
	g_world:SetCameraMode(1, GetMyActor())
	g_world:SetCameraFixed(true) -- 카메라 고정
	RemoveOpeningActor_010509()
end

function OnEsc_015109Opening()
	g_iStage = 5
	ODS("OnEsc_015109Opening\n", false, 1509)
	local kGroundGuid = GUID()
	kGroundGuid:Set("00000000-0000-0000-0000-000000000000")
	local packet = NewPacket(49)	--PT_C_M_NFY_MAPLOADED
	packet:PushByte(1)
	packet:PushInt(g_mapNo)		-- SGroundKey::WriteToPacket() 1/2
	packet:PushGuid(kGroundGuid)	-- SGroundKey::WriteToPacket() 2/2
	Net_Send(packet)
	DeletePacket(packet)

	Start_015109()
end
