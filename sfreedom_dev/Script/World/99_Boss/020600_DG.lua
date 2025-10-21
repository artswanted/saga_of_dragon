g_iStage = 0
g_fBossOpeningTime = 0.0

function Init_020600(world)
	g_iStage = 0
end

function Ready_020600(world)
	LockPlayerInputMove(8) -- for aram lock
	LockPlayerInput(8)
	CloseAllUI()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	CallUI("FRM_Aram")
	g_iStage = 1
	g_fBossOpeningTime = 25
	g_fParelNowTime = g_world:GetAccumTime()

	-- ESC Script 등록
	RegistESCScript( "PT_C_M_REQ_INDUN_START" )
	StopBgSound()	--배경음 끄기
end

function AramCreateOpeningActor()
	local guid = GUID("123")
	guid:Generate();
	
	local pilot = g_pilotMan:NewPilot(guid, 6000931, 0,"NPC")
	
	if pilot == nil or pilot:IsNil() == true then
		return true
	end
	
	local kActor = pilot:GetActor()
	if kActor == nil or kActor:IsNil() == true then
		return true
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if IsSingleMode() == false then
		g_pilotMan:InsertPilot(guid,pilot);
	end

	local	kSpawnPos = Point3(0, 0, -1000)
	
	kActor:FreeMove(true)
	kActor:SetUseLOD(false)
	kActor:SetUseSmoothShow(false)
	g_world:AddActor(guid, pilot:GetActor(), kSpawnPos, 6)
	
	-- local kTime = g_world:GetAccumTime()
	-- kActor:SetAnimationStartTime(kTime)
	
	--pilot:SetName(kName);
	pilot:GetActor():ClearReservedAction()
	local kAction = pilot:GetActor():ReserveTransitAction("a_opening_movie")
	kAction:SetSlot(0)
	kActor:ResetAnimation();
	--kActor:PlayCurrentSlot();

	pilot:GetActor():ClearActionState()
	pilot:SetAbil(AT_HP, 1000000);
	pilot:SetAbil(AT_UNIT_SIZE,2);
	pilot:SetAbil(AT_DAMAGEACTION_TYPE,0);
	pilot:SetAbil(AT_NOT_SEE_PLAYER, 0)
	pilot:SetAbil(AT_DAMAGEACTION_TYPE, 0)	
	
	g_world:SetCameraModeMovie2(kActor, 2, 1000081, "nop()")
end

function AramRemoveOpeningActor()
	local kActor = g_pilotMan:FindActorByClassNo(6000931);
	
	if kActor == nil or kActor:IsNil() then
		return	true
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:RemoveActorOnNextUpdate(kActor:GetPilotGuid())
	g_world:RemoveCameraAM()
	
	return true
end

function Update_020600()
	if 0 == g_iStage then
		return false
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local fDeltaTime = g_world:GetAccumTime() - g_fParelNowTime

	if 1 == g_iStage then
	    CallUI("FRM_MOVIE_IN")
		
		-- g_world:SetCameraModeByName("B_aram_Mutisha_opening 01","Camera01")
		-- g_world:SetStartAnimationByName("B_aram_Mutisha_opening 01",0)
		-- g_world:AttachSound("Mutisha_Opening", GetCameraTrn(true), 1,10000,10000)

		AramCreateOpeningActor()
		--Act_Mutisha_Die_CreateEngingActor()

		CallMapMove(true)
		g_fParelNowTime = g_world:GetAccumTime()
		g_iStage = 2
	elseif 2 == g_iStage and fDeltaTime > 1 then
		g_iStage = 3
	elseif 3 == g_iStage then
		g_iStage = 4
	elseif 4 == g_iStage and fDeltaTime >= g_fBossOpeningTime  then
--		g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 0.0, 1.0, 0.5, true )
		PT_C_M_REQ_INDUN_START()
	elseif 5 == g_iStage then
		g_iStage = 0
	end

	return true
end

function Start_020600(world)
	OnRecoveryMiniQuest(false)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	-- 사운드
	PlayBgSound(0)
	--g_world:DetachSound("Mutisha_Opening")
	g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 1.0, 0.0, 3.0, false )
	g_iStage = 5
	
	--g_world:DetachObjectByName("B_aram_Mutisha_opening 01")
	g_world:SetCameraMode(1, GetMyActor())
	g_world:SetCameraFixed(true) -- 카메라 고정
	AramRemoveOpeningActor()

	CloseUI("FRM_MOVIE_IN")
	UI_DefaultActivate( true )
	UnLockPlayerInputMove(8) -- for aram lock
	UnLockPlayerInput(8)
	CallMapMove(false)
	g_world:SetCameraZoom(1.0) -- 줌최대뒤로
end
