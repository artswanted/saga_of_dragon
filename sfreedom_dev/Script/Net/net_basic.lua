g_bAddedUIScene = false
g_iMapMoveStage = 0
g_bMovingToSameMap = false	--	같은 맵으로 이동했을때
g_MissionWallGroup = PG_PHYSX_GROUP_MISSION_WALL_0		-- 미션맵 벽 통과용
--g_bEliteWarningUICallReserve = false
g_SafeModeTargetFollowGuid = nil
g_szMapMoveCompleteNextActionName = ""
g_CntMapLoadingElapsTime =0	-- 맵로딩 시간을 제기 위한 변수 
g_VisitFirstMap = false		-- 이 맵에 처음 왔는가? C코드 안에서 셋팅함

function DoFileInitLua()
	if g_bInitScript == false then
		DoFile("init.lua")
		DoFile("Net/net.lua")
		g_bInitScript = true
	end
end

-------------------------------------------------- 이동 결과
g_kMapMoveCause = 0
function Net_MapMove(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_VisitFirstMap = false

	local OldMapAttr = g_world:GetAttr()
	
	Clear_FireLoveRemainSMS() 	-- 사랑의 불꽃은 맵이동을 하면 모두 닫아버린다.
	
	g_NpcManu_MapNo = 0
	g_kMapMoveCause = 0
	if false == g_world:IsHaveWorldAttr(GATTR_INSTANCE) then
		misdiff = 0
	end
	g_CntMapLoadingElapsTime = GetLocalTime()	-- 맵 로딩 직전에 시간을 기억한다
ODS("Net_MapMove 1\n", false, 1509);
	CallMapMove(true)
	
	g_iMapMoveStage = 1
	g_bLogined_User = false	--don't open inventory for not login user <070610 Naturallaw>
	GetQuestMan():ClearNPCQuestInfo() --all saved previous quest info clear <070727 Naturallaw>
	SaveMiniIngQuestSnapInfo()
	BalloonEmoticonSwitch(true)

	CloseAllUI()

ODS("Net_MapMove 2\n");

	g_kMapMoveCause = packet:PopByte()
	local iOldMapNo = g_mapNo
	g_mapNo = packet:PopInt()  -- 갈 맵 번호 SGroundKey::iGroundNo
    local kGroundGuid = packet:PopGuid()	-- 갈맵의 GUID
    local kMapAttr = packet:PopInt()	-- 갈맵의 타입
    
ODS("Net_MapMove 3\n");
	g_playerInfo.guidPilot = packet:PopGuid()			-- CharacterGUID
	g_pilotMan:SetPlayerPilotGuid( g_playerInfo.guidPilot )
    
    
ODS("Net_MapMove 4\n");
	SetBriefingLoadUI(g_mapNo, -1)
	ClearAllWorks();
	
	g_bMovingToSameMap = SetNowGroundkey(0,g_mapNo,kGroundGuid) == false;
	if g_bMovingToSameMap then
		ODS("g_bMovingToSameMap : TRUE\n")
	else
		ODS("g_bMovingToSameMap : FALSE\n");
	end

	local bMoveMyhomeToMyhome = (OldMapAttr == GATTR_MYHOME and iOldMapNo == g_mapNo)
	
    -- 갈 맵의 번호와 현재 맵번호가 같다면 맵로딩을 하지 않는다. 
	if g_bMovingToSameMap == false or bMoveMyhomeToMyhome then
		
		--StopBgSound()
		
		if OldMapAttr == GATTR_MYHOME then
			g_ChatMgrClient:SetChatStation(ECS_COMMON)
		end
		
		local LoadingImageID = 0
		local bRidingPet = false --펫에 탑승한 상태로 맵을 이동하나
		local MyActor = g_pilotMan:GetPlayerActor()
		if MyActor:IsNil() == false then
			if MyActor:IsRidingPet() and g_kMapMoveCause == MMET_RidingPet_Transport then
				bRidingPet = true
			end
			local trigger = MyActor:GetCurrentTrigger()
			LoadingImageID = trigger:GetLoadingImageID()
		end
		
		g_renderMan:RemoveAllScene("UIScene")	-- UIScene을 제외한 모든 Scene을 삭제.

		if g_bAddedUIScene == false then
			ReleaseAllControl() -- 지금까지 로딩했던 모든 UI를 릴리즈 한다.
			g_bSkill_Window_Initialized = false
			g_renderMan:CleanUpScene()
			g_renderMan:AddScene("UIScene")
			g_bAddedUIScene = true
		end

		--g_renderMan:InvalidateSceneContainer()	-- UIScene은 즉시 추가해야함.
		
		CallUI("FRM_LOADING_IMG")
 		--UI_SetLoadingImage(g_mapNo)
		LoadingImage(g_mapNo, kMapAttr, LoadingImageID, bRidingPet, iOldMapNo)
		
		UI_SetLoadingProgress(0)
		
		--StartScreenUpdate()
	ODS("Net_MapMove 5\n");
		UI_SetLoadingProgress(20)

		-- 월드를 생성한다.
		g_world_tmp = g_renderMan:AddSceneByNo(g_mapNo)
		if g_world_tmp == nil then
			-- 클라가 맵이 없을 경우 실패할 수 있음
			MessageBox("[Net_MapMove] Failed to create Scene : " .. g_mapNo .. " .")
			g_CntMapLoadingElapsTime =0 -- 맵 로딩시간 카운트 초기화
			return true 
		end
		g_world = ToWorld(g_world_tmp)
		g_world:SetUpdateWorld(true)
		g_world:SetMapNo(g_mapNo)
		g_world:SetAttr(kMapAttr)
		
		g_world:RecursiveDetachGlowMapByGndAttr();
		g_worldID = g_world:GetID()
		
		 
		UpdateProgress(kMapAttr, 50)
		-- 사용자의 입력을 막는다.
		LockPlayerInputMove(3) -- for map moving lock
		LockPlayerInput(3)
		-- 캐릭터들을 맵에 추가한다.
		g_world:Recv_PT_M_C_ADD_UNIT(packet)
		
		--g_world:Recv_PT_M_C_UNIT_POS_CHANGE(g_playerInfo.guidPilot, kTargetPos)
		UpdateProgress(kMapAttr, 80)
	ODS("ChangeMapInfo Start\n");
		ChangeMapInfo(g_mapNo)
		discord_activity_set_state(GetMapNameW(g_mapNo))
		discord_update_activity()
	else
		g_world:RemoveAllObject(true)
		
		-- 캐릭터들을 맵에 추가한다.
		g_world:Recv_PT_M_C_ADD_UNIT(packet)
	end
	
	g_world:InitSmallAreaInfo(packet)
	g_world:SetActivaingWorldMap(false)
	g_world:SetUpdateWorld(true)
	

	if kMapAttr == GATTR_EMPORIABATTLE then
		UI_SetLoadingProgress(100)
		UpdateBriefingTick(g_mapNo, 0)
	else
		-- PT_C_M_NFY_MAPLOADED
		ReqMapMoveComplate()
	end

	UseCameraHeightAdjust(true)	--점프할 때 카메라가 따라오지 않도록
	--CallLinkageUI();
	--StopBgSound()
	return true
end

function ReqMapMoveComplate()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local packet2 = NewPacket(49)
	packet2:PushByte(g_kMapMoveCause)
	packet2:PushInt(GetNowGroundNo())
	packet2:PushGuid(GetNowGroundGuid())
	Net_Send(packet2)
	DeletePacket(packet2)
	ODS("ChangeMapInfo Finished\n");
	g_iMapMoveStage = 2
	
	g_kMapMoveCause = 0

	ClearOtherPartyName()
	lwActivateDailyReward()
	
	ODS("ReqMapMoveComplate WorldAttr: " .. g_world:GetAttr() .. "\n")
	if g_world:IsHaveWorldAttr(GATTR_DEFAULT) then
		misdiff = 0	
	end
	if g_world:IsHaveWorldAttr(GATTR_EXPEDITION_LOBBY) then 	-- 현재 위치가 원정대 로비이고
		if IsInExpedition() then								-- 원정대에 속해 있다면
			ClearNotHaveKeyItemMemberList()						-- 원정대 메달 미소지자 리스트 초기화
			ActivateUI("FRM_EXPEDITION_REGISTER")				-- 원정대 등록창 오픈
		end
	end
	if g_world:IsHaveWorldAttr(GATTR_EXPEDITION_GROUND) then	-- 원정대 던전 내부이고
		if IsInExpedition() then								-- 원정대에 속해 있다면
			OnCall_ExpeditionMainBar()							-- 미니정보창 콜
			UpdateExpeditionMainbar()							-- 업데이트
		end
	end
	if g_world:IsHaveWorldAttr(GATTR_FLAG_EMPORIABATTLE)		-- 엠포리아, 배틀스퀘어로 진입하면
		or g_world:IsHaveWorldAttr(GATTR_FLAG_BATTLESQUARE) then
		AddWarnDataTT(749993)									-- 옵션증폭 적용 안된다는 시스템 메시지 출력
	end
end

function UpdateProgress( kMapAttr, Progress )
	UI_SetLoadingProgress(Progress)
end

function UpdateBriefingTick(MapNo, Progress)
	local kMainUI = ActivateUI("FRM_LOADING_HELP_BTN_BAR")
	if kMainUI:IsNil() then
		ReqMapMoveComplate()
	end
	
	if true == SetBriefingLoadUI( MapNo, Progress ) then
		kMainUI:SetCustomDataAsInt(MapNo)
		kMainUI:SetOwnerState(Progress)
	end
end

function NextBriefingTick(kWnd)
	local kMainUI = GetUIWnd("FRM_LOADING_HELP_BTN_BAR")
	if kMainUI:IsNil() then
		ReqMapMoveComplate()
	end
	
	if false==kWnd:IsNil() and 1==kWnd:GetCustomDataAsInt() then
		ReqMapMoveComplate()
		return
	end
	
	UpdateBriefingTick(kMainUI:GetCustomDataAsInt(), kMainUI:GetOwnerState() + 1);
end

function PrevBriefingTick()
	local kMainUI = GetUIWnd("FRM_LOADING_HELP_BTN_BAR")
	if kMainUI:IsNil() then
		ReqMapMoveComplate()
	end
	
	UpdateBriefingTick(kMainUI:GetCustomDataAsInt(), kMainUI:GetOwnerState() - 1);
end

function Net_MapMoveComplete(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_iMapMoveStage = 3
	g_bLogined_User = true	--open inventory for login user <070610 Naturallaw>
	ODS("Net_MapMoveComplete\n")
	
	if g_world:IsHaveWorldAttr(GATTR_EMPORIABATTLE) then
		CloseUI("FRM_LOADING_HELP_BTN_BAR")
	else
		UI_SetLoadingProgress(95)
	end
	g_iMapMoveStage = 4
	
	local kMyPilot = g_pilotMan:GetPlayerPilot();	
	if kMyPilot:IsNil() == true then		
		return
	end
	
	CloseBriefingOBJUI()
	UI_MapMoveComplete()
		
	g_MissionWallGroup = PG_PHYSX_GROUP_MISSION_WALL_0 -- 벽 그룹 초기화
	local kMyActor = kMyPilot:GetActor();
	if kMyActor:IsNil() == false then
		kMyActor:ResetActiveGrp()
	end

	OnRecoveryMiniQuest(true)
	RefreshPartyStateWnd()
	DrawMonsterKillUI()
	RefreshRepairAlarmUI()
	GetMyCharacterCardInfo()
	
--	g_pilotMan:SetLockMyInput(false)
--	g_iPlayerLockCount = 0
	
	GetQuestMan():Load()
	
	g_world:Recv_PT_M_C_NFY_MAPLOADED(packet)

	CMP_STATE_CHANGE(0)

--	if g_bEliteWarningUICallReserve then
--		g_bEliteWarningUICallReserve = false
--		CallUI("FRM_WARNING")
--	end
	if 0==GetNowMailCount() then
		REQ_MAIL_RECV()	--요청
	end
	
	
	if g_bMovingToSameMap == false then
	
		--	맵 이동시 발생되는 헬프 시스템 호출
		GetHelpSystem():ActivateByConditionString("MapMove",g_world:GetID());
		--	맵 이동시 발생되는 이벤트 스크립트 호출
		if 0 ~= g_world:GetEventScriptIDOnEnter() then
			if true == g_world:CheckEventScriptOnEnter() then
				GetEventScriptSystem():ActivateEvent(g_world:GetEventScriptIDOnEnter(),g_world:GetAccumTime(),0.1);
			end
		end
	end
	
	
	-- 안전거품 사용시 맵이동시 동일하게 키입력 못하도록 막아야 한다.
	local Menu = false
	local MyActor = GetMyActor()
	local MyPilot = MyActor:GetPilot()
	if nil ~= MyPilot and not MyPilot:IsNil() then
		local MyUnit = MyPilot:GetUnit()
		if nil ~= MyUnit and not MyUnit:IsNil() then
			Menu = MyUnit:IsEffect(402001)
			if Menu == true then
				LockPlayerInput(100) -- for effect lock
			end
		end
	end	
	if Menu == false then
		if true == IsFollow() then
			Menu = true
		end
	end
	if Menu == true then
		local kGuid = GetFollowGuid()
		if nil ~= kGuid and not kGuid:IsNil() then
			--MyActor:ResponseFollowActor(kGuid, 1);
			MyActor:SetStartFollow(kGuid)
			local	kPilot = g_pilotMan:FindPilot(kGuid)
			if nil ~= kPilot and not kPilot:IsNil() then
				local actor = kPilot:GetActor()
				if nil ~= actor and not actor:IsNil() then					
					actor:AddFollowingMeActor(MyActor:GetPilotGuid())
				end
			end			
		end		

	end
	CheckMapMoveNextAction()
	MToMMapMoveReCheck()
	CSNotifyCheck()
	CheckUsingExpCard()
	CoupleSweetHeartUI()
	LuckyStarEventUI()
	UpdateCheckDrawLimitBreakBar()
	
	if false==g_world:IsMapMoveCompleteFade() then
		g_world:SetShowWorldFocusFilter(false)
	else
		g_world:MapMoveCompleteFade()
	end
	BalloonEmoticonSwitch(false)
	CallMapMove(false)

	CallUI("FRM_USER_STOPPER")
--	UnLockPlayerInputMove(3) -- for map moving lock
--	UnLockPlayerInput(3)
	if false==GetTradeGuid():IsNil() then
		Net_PT_C_M_NFY_EXCHANGE_ITEM_QUIT()
		ClearTradeInfo()
	end
	--맵 로딩시간에 든 값을 체크하여 넣는다
	local LodingElapsTime = GetLocalTime() - g_CntMapLoadingElapsTime
	CallMacroCheckUI_IfNotComplete(LodingElapsTime) -- if not complete MacroCheckUI then call MacroCheckUI
end

function CheckMapMoveNextAction()
	local actor = GetMyActor()	
	local prevAction = actor:GetAction()
	if g_szMapMoveCompleteNextActionName == "Chaos" or g_szMapMoveCompleteNextActionName == "Hidden" then
		local action = actor:ReserveTransitAction("a_teleport01_out")	
		if action:IsNil() == false then	
			action:SetParam( 3, g_szMapMoveCompleteNextActionName )
			action:BroadCast(actor:GetPilot())	
		end				
	end
	g_szMapMoveCompleteNextActionName = ""
end
--[[
-- 이건 지울거임..............
-- Lock Count
g_iPlayerLockCount = 0
function LockPlayerInput()
	local Player = g_pilotMan:GetPlayerPilot()
	if Player:IsNil() == false then
		if 0 == g_iPlayerLockCount then
			--MessageBox("Lock")
			Player:GetActor():ReserveTransitAction("a_lock_move")
			Player:GetActor():ClearActionState()
			Player:GetActor():SetDirection(DIR_NONE)
			--g_pilotMan:LockMyInput(true)
		end
		g_iPlayerLockCount = g_iPlayerLockCount + 1
		return true
	end
	return false
end

function UnLockPlayerInput(bForceUnlock)
	--g_pilotMan:LockMyInput(false)
	local Player = g_pilotMan:GetPlayerPilot()
	if Player:IsNil() == false then
		ODS("UnLockPlayerInput \n")
		local bUnlock = false
		if 0 < g_iPlayerLockCount then
			g_iPlayerLockCount = g_iPlayerLockCount - 1
			if 0 == g_iPlayerLockCount or bForceUnlock then
				bUnlock = true
			end
		end
		
		if bUnlock then
			--MessageBox("unlock")
			Player:GetActor():ReserveTransitAction("a_unlock_move")
			g_iPlayerLockCount = 0
		end
	else
		ODS("Player:IsNil() == false \n")
	end
end
]]
function LockGlobalHotKey(bLock)
	g_bEnableShortCutKey = not bLock
end

--[[
function Net_PT_M_C_NFY_PLAY_OPENING(packet)
	ODS("Net_PT_M_C_NFY_PLAY_OPENING \n")
	if true == g_world:Recv_PT_M_C_NFY_PLAY_OPENING(packet) then
		CloseUI("FRM_LOADING_IMG")
		EndScreenUpdate()
	else
--		오프닝 실패했으니까 그냥 끝났다고 보낸다.
		local packet = NewPacket(67)	--PT_C_M_REQ_END_OPENING
		Net_Send(packet)
		DeletePacket(packet)
	end
end
]]--

function Net_ReqPing(packet)
	local dwTime = packet:PopInt()
	
	local kMyActor = GetMyActor()
	if kMyActor:IsNil() then
		-- Map에 입장하기 전이다
		local kPos = Point3(0, 0, 0)
		local kResPacket = NewPacket(12371)
		kResPacket:PushInt(dwTime)
		kResPacket:PushPoint3(kPos)
		Net_Send(kResPacket)
		DeletePacket(kResPacket)
		return
	end
	
	local kPos = kMyActor:GetPos()
	local kResPacket = NewPacket(12371)
	kResPacket:PushInt(dwTime)
	kPos:SetZ(kPos:GetZ()-25)
	kResPacket:PushPoint3(kPos)
	Net_Send(kResPacket)
	DeletePacket(kResPacket)
	
end

