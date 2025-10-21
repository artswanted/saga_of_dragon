g_mapNo = 0
g_world = nil
g_worldID = nil
g_createdActor = nil
g_BullDogActor = nil
g_ExtraActor = {}
g_playerInfo = {}
g_UIStatus = MENU_SELECT
local g_CharRoot = "char_spawn_1" -- 예전과 달리 스폰포인트가 하나이다.
local g_ActorUpdateSetting = 0
local g_iMaxCharacter = 8

local g_CameraActor = nil
g_SceneType = 0
g_CreateType = 0 		--0: Human / 1:Dragon

local g_iClassMarkNum = 9812341
local g_bIsCreateMode = false

g_CharDefault = {}
g_CharNowRot = {}
g_CharDefault["POS"] = {}
g_CharDefault["RAD"] = {}
g_CharNowRot["NUM"] = {}
g_CharNowRot["RAD"] = {}
g_IsNowRotating = {}

g_EnableCreateCount = 4

function GetRotPos(fRad, wnd)
	if wnd:IsNil() == true then
		return Point2(0,0)
	end
	local ptSize = wnd:GetSize()
	local pos = Point2(math.sin(fRad)*390+485-ptSize:GetX()/2, math.cos(fRad)*-390+384-ptSize:GetY()/2)
	return pos
end

function GetRotPosByIndex(iIdx, wnd)
	if wnd:IsNil() == true then
		return Point2(0,0)
	end
	local fRad = math.rad(iIdx*-22.5 - 56)
	return GetRotPos(fRad, wnd)
end

function Reset_IsNowRotating(bValue)
	for i = 0, table.getn(g_IsNowRotating) - 1 do
		g_IsNowRotating[i] = bValue
	end
end

function Get_IsNowRotating(bValue)
	for i = 0, table.getn(g_IsNowRotating) - 1 do
		if g_IsNowRotating[i] ~= bValue then
			ODS(i.."번째가 다름\n")
			return false
		end
	end

	return true
end


function CreateBullDog()
	RemoveBullDog()
	g_BullDogActor = g_selectStage:AddNewActor(20000, 0, "char_root_select_01")
	if nil==g_BullDogActor and true==g_BullDogActor:IsNil() then
		return
	end
	g_BullDogActor:ResetAnimation()
	g_BullDogActor:ReserveTransitAction("a_BullDog_Idle")
end

function RemoveBullDog()
	if g_BullDogActor == nil or g_BullDogActor:IsNil() then
		return
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:RemoveActorOnNextUpdate(g_BullDogActor:GetPilotGuid())
	g_BullDogActor = nil
end

--------------------------------------------------
function InitializeCharacterSelect(world)
	g_world = world
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_ActorUpdateSetting = GetVariableActorUpdate()
	SetVariableActorUpdate(false)
	
	g_selectStage = world:SetSelectStage(true)
	g_selectStage:AddBaseItemSetFromTableData()

	-- 성별에 따른 디폴트 (타입, Gender, Slot위치)
	-- 0,헤어 컬러   2,헤어스타일   4,얼굴

	-- 남자 전사
	g_selectStage:SetFirstExposedSlot(0, 1, 2)
	g_selectStage:SetFirstExposedSlot(2, 1, 3)
	g_selectStage:SetFirstExposedSlot(4, 1, 8)
	
	--초기화
	LuckyStarEventInit()
	SetCurrentMarketGrade(MG_NORMAL)
	AchieveStatusInit()
	InitializeSelectCamera()
	g_LoginToMapMoveComplete = false
end
-----------------------Camare---------------------------
 local g_kCameraActor = nil

function InitializeSelectCamera()
	local guid = GUID("123")
	guid:Generate()
	local bWide = IsWideScreen()
	if true == bWide then
		g_kSelectCameraMode = g_kArraySelectCameraMode["WIDE"];
	else
		g_kSelectCameraMode = g_kArraySelectCameraMode["NORMAL"];
	end
	local pilot = g_pilotMan:NewPilot(guid, 5000000, 0) -- ca_CharactorSelect
	if pilot:IsNil() == false then
		pilot:SetAbil(AT_HIDE, 1)
		g_kCameraActor = pilot:GetActor()
		g_kCameraActor:SetHideShadow(true)
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, g_kCameraActor, Point3(0,0,0), 9)
		g_world:SetCameraModeMovie2(g_kCameraActor, 2, g_kSelectCameraMode["CAMERA_MOVE_SELECT"], "SelectCameraAniDone()")
	end
end

function SetSelectCameraMove(iSeqNo)
	if g_kCameraActor:IsNil() == false then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetCameraModeMovie2(g_kCameraActor, 2, iSeqNo, "SelectCameraAniDone()")
	end
end

function MoveCamera_Crete()
	local iSeqNo = g_kSelectCameraMode["CAMERA_MOVE_SELECT"]
	if g_CreateType == 0 then
		iSeqNo = g_kSelectCameraMode["CAMERA_MOVE_CREATE_HUMAN_IN"]
	elseif g_CreateType == 1 then
		iSeqNo = g_kSelectCameraMode["CAMERA_MOVE_CREATE_DRAGON_IN"]
	end
	SetSelectCameraMove(iSeqNo)
end

function MoveCamera_Select()
	local iSeqNo = g_kSelectCameraMode["CAMERA_MOVE_SELECT"]
	if g_CreateType == 0 then
		iSeqNo = g_kSelectCameraMode["CAMERA_MOVE_CREATE_HUMAN_OUT"]
	elseif g_CreateType == 1 then
		iSeqNo = g_kSelectCameraMode["CAMERA_MOVE_CREATE_DRAGON_OUT"]
	end
	SetSelectCameraMove(iSeqNo)
end
--------------------------------------------------
function Net_PickCharacter(world)

	if g_bIsCreateMode == true then
		return true
	end
	
	if MENU_REALM_COMBINE == g_UIStatus then
		return false
	end
	
	local actor = world:PickActor(1) -- ObjectGroupType,  1 = Player
	if actor:IsNil() then
		return false
	end
		
	local oldSelectedActor = g_selectStage:GetSelectedActor()
	local newSelectedActor = actor:GetPilotGuid()
	local iSlot = g_selectStage:GetSpawnSlot(newSelectedActor)
	if -1 == iSlot then
		return false
	end
	
	if oldSelectedActor:GetString() == newSelectedActor:GetString() then
		return false
	end
	
	local bResult = SelectCharactor(actor)
	
	return bResult
end

function SelectCharactor(actor)
		
	local kDeleteUI = GetUIWnd("DeleteConfirm")
	if false == kDeleteUI:IsNil() then
		--ODS("kDeleteUI is Nil!!!\n", false, 3851)
		return true
	end
	
	local oldSelectedActor = g_selectStage:GetSelectedActor()
	local newSelectedActor = actor:GetPilotGuid()
	
	if oldSelectedActor:IsNil() == false then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local oldActor = g_world:FindActor(oldSelectedActor);
		if oldActor:IsNil() == false then
			if oldActor:IsDBState(CIDBS_NeedRestore) then
				oldActor:ReserveTransitAction("a_sleep")
			else
				oldActor:ReserveTransitAction("a_intro_select_back")
			end
			oldActor:DetachFrom(49023853, true)
		end
	end
	
	local selectedSlot = g_selectStage:GetSpawnSlot(newSelectedActor)
	-- 선택한 Actor가 SpawnSlot을 가지고 있지 않으면 무시 
	if selectedSlot == -1 then
		return true
	end
	-- 새로 선택된 캐릭터 액션/정보
	local bCanStart = true
	g_selectStage:SetSelectedActor(selectedSlot-1)
	if actor:IsDBState(CIDBS_NeedRestore) then
		actor:ReserveTransitAction("a_sleep")
		bCanStart = false
	else
		actor:ReserveTransitAction("a_intro_up")
	end

	local kCharMenu = GetUIWnd("SFRM_CHAR_MENU")
	if kCharMenu:IsNil() == false then
		local kButtonStart = kCharMenu:GetControl("BTN_START_GAME")
		local kRestoreInfo = kCharMenu:GetControl("FRM_CHAR_RESTORE_INFO")
		if kCharMenu:IsNil() == false then
			kButtonStart:Disable(bCanStart == false)
		end
		if kRestoreInfo:IsNil() == false then
			kRestoreInfo:Visible(bCanStart == false)
		end
	end

	--새로 캐릭터 생성된 직후엔 UI 그려주지 말자. SelectUI()에서 호출해준다.
	if false == g_bIsCreateMode then
		g_selectStage:SetCharInfo()
	end
	
	local iClass = actor:GetAbil(AT_BASE_CLASS)
	if 5 > iClass then
		actor:AttachParticleWithRotate(49023853, "p_pt_ride", "eff_char_select00",0.4, true)
	else
		actor:AttachParticleWithRotate(49023853, "char_root", "eff_char_select00",0.4, true)
	end
	
	return true
end

--======================== Character Select =======================
function OnClick_CharacterSelect_StartGame()
	SetVariableActorUpdate(g_ActorUpdateSetting)
	Net_SelectCharacter()
	ClearTradeInfo()
	return true
end

--------------------------------------------------
function nop()
end

--------------------------------------------------
function Net_SelectCharacter()
	if g_selectStage == nil then	    
		return false
	end

	local selectedActor = g_selectStage:GetSelectedActor()
	if selectedActor:IsNil() == false then	--임시다. 캐선창이 완료되면 이 IF문은 지우자
		if true == g_selectStage:CheckNameConflict(selectedActor) then
			AddWarnDataStr(GetTextW(50519), 0)  -- 이름을 변경해야 접속이 가능
			return false
		end
		local bClose = SendSelectCharacter(selectedActor)
		return true
	end
	
	local pilot = g_pilotMan:FindPilot(selectedActor)
	if pilot:IsNil() then
		AddWarnDataStr(GetTextW(600014), 0)  -- 캐릭터 선택해주세요
		return false
	end

	return true
end
--------------------------------------------------
function Net_ReqMapJoin(world)
	packet = NewPacket(33)					--PT_C_S_REQ_ENTER_MAP_SERVER
	packet:PushInt(g_playerInfo.recentMapNo)
	Net_Send(packet)
	DeletePacket(packet)

	return true
end

--------------------------------------------------
g_bIsFirstJoin = true
function Net_AddReadyCharacter(world, packet)
	g_world = world
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	world:Recv_AddReadyCharacter(packet)
	
	g_selectStage:ReqEnableExtendSlot()	--확장 슬롯 갯수 요청
	
	for iIdx = 0, g_iMaxCharacter do
		DrawSpawnCharacter(iIdx+1)
	end
	CreateBullDog()	--1번슬롯용 개
	
	if g_bIsFirstJoin then
		CloseAllUI(true)
		OnSelectUI()
		g_bIsFirstJoin = false
	end
	PlayBgSound(0)
	
	g_selectStage:OnCallCharList()
	if true==g_bIgnoreCombine then return end
	g_selectStage:OnCallDeleteWait()
	
	local kSelectGuid = g_selectStage:GetSelectedActor()
	local spawnPilot = g_pilotMan:FindPilot(kSelectGuid)
	if spawnPilot:IsNil() == false then
		local spawnActor = spawnPilot:GetActor()
		if spawnActor:IsNil() == false then
			SelectCharactor(spawnActor)
			return true
		end
	end
	
end

function DrawSpawnCharacter(iIdx) -- 슬롯의 캐릭터 그리기
	local spawnActorGuid = g_selectStage:GetSpawnActor(iIdx)
	local spawnPilot = g_pilotMan:FindPilot(spawnActorGuid)
	if spawnPilot:IsNil() == false then
		local spawnActor = spawnPilot:GetActor()
		if spawnActor:IsNil() == false then
			spawnActor:LockBidirection(false)
			spawnActor:SetHideShadow(false)
			spawnActor:InitPhysical(false)
			spawnActor:DetachFrom(g_iClassMarkNum)
			spawnActor:DetachFrom(g_iClassMarkNum+1)
			spawnActor:FindPathNormal()
			spawnActor:SeeFront(true, true)
			spawnActor:SetMouseOverScript("Actor_MouseOver")
			spawnActor:SetMouseOutScript("Actor_MouseOut")
			if spawnActor:IsDBState(CIDBS_NeedRestore) then
				spawnActor:ReserveTransitAction("a_sleep")
			else
				spawnActor:ReserveTransitAction("a_intro_idle")
			end
		end
	end
end

---------------------------------------------------------------
function Net_ReqPrepareCreateCharacter()
	--PT_C_S_REQ_PRE_CREATE_CHARACTER
	local oldSelectedActor = g_selectStage:GetSelectedActor()
	if oldSelectedActor:IsNil() == false then
		local pilot = g_pilotMan:FindPilot(oldSelectedActor)
		if pilot:IsNil() == false then
			local spawnSlot = g_selectStage:GetSpawnSlot(oldSelectedActor)
			if spawnSlot ~= -1 then
				if( CheckNil(g_world == nil) ) then return false end
				if( CheckNil(g_world:IsNil()) ) then return false end
				local loc = g_world:FindSpawnLoc(g_CharRoot)
				pilot:GetActor():SetTranslate(loc,true)		
				pilot:GetActor():SetHideShadow(false)	
				local kAction = pilot:GetActor():ReserveTransitAction("a_intro_idle")
				kAction:SetSlot(spawnSlot-1)
			end
		end
	end
	
	--CloseUI("GoBack")

	packet = NewPacket(4097)			
	Net_Send(packet)
	DeletePacket(packet)
end
---------------------------------------------------------------
function Net_RecvPrepareCreateCharacter()
	--ODS("Net_RecvPrepareCreateCharacter\n", 0, 9999)
	-- Check Count
	if g_selectStage:GetSpawnSlotCount() >= g_iMaxCharacter then
		AddWarnDataStr(GetTextW(600012), 0)      -- 더이상 캐릭 생성 불가
		return false
	end
	
	-- 새로운 캐릭터를 생성한다.
	if nil ~= g_CreateType then
		if g_CreateType == 0 then
			g_createdActor = g_selectStage:AddNewActor(1, 1, "char_root_create_1")
		elseif g_CreateType == 1 then
			g_createdActor = g_selectStage:AddNewActor(CT_SHAMAN, 1, "char_root_create_2")
		end
	else
		g_createdActor = g_selectStage:AddNewActor(1, 1, "char_root_create_1")
	end
	
	if g_createdActor:IsNil() == false then
		g_createdActor:GetPilot():SetAbil(AT_IDLEACTION_TYPE, 101)
		g_selectStage:SetNewActor(g_createdActor)
		g_selectStage:SetDefaultFiveElement()
		g_createdActor:ClearActionState()
		g_createdActor:ReserveTransitAction("a_idle")
		local kAddPos = Point3(100,0,0)
		if g_CreateType == 1 then
			kAddPos = Point3(-100,0,0)
		end
		local	kTargetPos = g_selectStage:GetNewActor():GetPos():_Add(kAddPos);
		g_createdActor:LookAt(kTargetPos)	
		
		local iClass = g_createdActor:GetAbil(AT_CLASS)
		g_selectStage:EquipBaseItemSet(iClass, 1)	--기본 장비
		if iClass == 51 then
			-- 용족 소환사
			g_selectStage:EquipBaseItemBySeq(2, 4)	--기본 장비
			g_selectStage:EquipBaseItemBySeq(4, 5)	--기본 장비
			g_selectStage:EquipBaseItemBySeq(0, 5)
		else
			g_selectStage:EquipBaseItemBySeq(2, 3)		--머리카락 
			g_selectStage:EquipBaseItemBySeq(4, 8)		--얼굴
			g_selectStage:EquipBaseItemBySeq(0, 2)
		end
		
		local baseItemSet = g_selectStage:GetBaseItemSet(0)
		if baseItemSet:IsNil() == false then
			baseItemSet:SetExposedSlot(5)	-- Hair Color의 경우 5개의 슬롯을 가진다.
		end
	end
	
	local userNameControl = GetUserNameControl()
	if userNameControl ~= nil then
		if userNameControl:IsNil() == false then
			userNameControl:SetEditText("")
		end	
	end
end

function Play_Actor_Animation(Style)
	local pRotateWnd = GetUIWnd("CharacterRotate")
	if pRotateWnd:IsNil() then
		return false
	end
	
	local pCharacterWnd = pRotateWnd:GetControl("FRM_CHARACTER")
	if not pCharacterWnd:IsNil() then
		pCharacterWnd:SetRenderModelActorTargetAnimation("CreateCharacter", "Character", Style)
	end
end

function Play_Actor_Animation2(Style)
	local Actor = g_selectStage:GetNewActor()
	if nil ~= Actor and false == Actor:IsNil() then
		Actor:ResetAnimation()
		Actor:ReserveTransitAction(Style)
	end
end

function Net_RecvPrepareCreateCharacter_OnDisplay()
	-- 새로운 캐릭터를 생성한다.
	local iRaceSetNo = 0	--종족별 세트타입
	if nil ~= g_CreateType then
		if g_CreateType == 0 then
			g_createdActor = g_selectStage:AddNewActor(1, 1, "char_root_create_1")
			iRaceSetNo = 10000
		elseif g_CreateType == 1 then
			g_createdActor = g_selectStage:AddNewActor(CT_SHAMAN, 1, "char_root_create_2")
			iRaceSetNo = 20000
		end
	else
		g_createdActor = g_selectStage:AddNewActor(1, 1, "char_root_create_1")
	end

	if g_createdActor:IsNil() == false then
		g_createdActor:GetPilot():SetAbil(AT_IDLEACTION_TYPE, 101)			
		g_selectStage:SetNewActor(g_createdActor)			
		g_selectStage:SetDefaultFiveElement()
		g_createdActor:ReserveTransitAction("a_idle")
		g_createdActor:ClearActionState()
		
		local kAddPos = Point3(100,0,0)
		if g_CreateType == 1 then
			kAddPos = Point3(-100,0,0)
		end
		local	kTargetPos = g_selectStage:GetNewActor():GetPos():_Add(kAddPos);
		g_createdActor:LookAt(kTargetPos)
		g_createdActor:DetachFrom(g_iClassMarkNum)

		local iClass = g_createdActor:GetAbil(AT_CLASS)
		g_selectStage:EquipBaseItemSet(iClass, 1)	--기본 장비
		if iClass == 51 then
			-- 용족 소환사
			g_selectStage:EquipBaseItemBySeq(2, 4)	--기본 장비
			g_selectStage:EquipBaseItemBySeq(4, 5)	--기본 장비
			g_selectStage:EquipBaseItemBySeq(0, 5)
		else
			g_selectStage:EquipBaseItemBySeq(2, 3)		--머리카락 
			g_selectStage:EquipBaseItemBySeq(4, 8)		--얼굴
			g_selectStage:EquipBaseItemBySeq(0, 2)
		end
			
		local baseItemSet = g_selectStage:GetBaseItemSet_Detail(0, iRaceSetNo, 3)
		if baseItemSet:IsNil() == false then
			baseItemSet:SetExposedSlot(5)	-- Hair Color의 경우 5개의 슬롯을 가진다.
		end
	end

	local serNameControl = GetUserNameControl()
	if userNameControl ~= nil then
		if userNameControl:IsNil() == false then
			userNameControl:SetEditText("")
		end
	end
end

local g_bIsCancelInCreateLevel = 0
function CancelInCreate()
	local actor = g_selectStage:GetNewActor()
	if actor:IsNil() == false then
		if(nil ~= g_world) then
			g_world:RemoveObject(actor:GetPilotGuid())
			--g_world:SetMainCameraStatus("select_camera")
		end
	end
	OffCreateUI()
end
function CancelInCreate_OnDisplay()
	if g_bIsCancelInCreateLevel == 1 then
		g_bIsCancelInCreateLevel = 2
		--ActivateUI("FRM_CHAR_SEL_IN")

		local actor = g_selectStage:GetNewActor()
		if actor:IsNil() == false then
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			g_world:RemoveObject(actor:GetPilotGuid())
			--g_world:SetMainCameraStatus("select_camera")
		end
	elseif g_bIsCancelInCreateLevel == 2 then
		g_bIsCancelInCreateLevel = 0
	end
end

local g_bIsOkInCreateLevel = 0

function OkInCreate()
	local userName = GetUserName()
	if CompNum(userName:Length(), 0) == 1  then
		Net_ReqCreateCharacter(userName)
	else
		AddWarnDataStr(GetTextW(600013), 0)
	end
	OffCreateUI()
end
function OkInCreate_OnDisplay()
	if g_bIsOkInCreateLevel == 1 then
		--ODS("OkInCreate_OnDisplay1\n", 0, 3851)
		-- Step 1
		g_bIsOkInCreateLevel = 2

		-- 생성창의 캐릭을 지운다.
		local newActor = g_selectStage:GetNewActor()
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:RemoveObject(newActor:GetPilotGuid())
		
		g_selectStage:ResetSpawnSlot()
		-- 캐릭터 리스트를 다시 요청한다.
		packet = NewPacket(4132)	-- PT_C_S_REQ_CHARACTER_LIST
		Net_Send(packet)
		DeletePacket(packet)

	elseif g_bIsOkInCreateLevel == 2 then
		--ODS("OkInCreate_OnDisplay1\n", 0, 9999)
		g_bIsOkInCreateLevel = 0
	end
end
---------------------------------------------------------------
function Net_RecvCreateCharacter(world, packet)
	local errCode = packet:PopInt()
	--ODS("RecvCreateCharacter : " .. errCode .. "\n", false, 3851)

	if 100 == errCode then
		AddWarnDataStr(GetTextW(600011), 0)      -- 이미 사용중
	elseif 101 == errCode then
		AddWarnDataStr(GetTextW(600017), 0)      -- 사용할 수 없는 단어(욕설)
	elseif 102 == errCode  then
		AddWarnDataStr(GetTextW(600017), 0)      -- 알 수 없는 클래스
	elseif 103 == errCode  then
		AddWarnDataStr(GetTextW(600035), 0)      -- 중복확인을 먼저 하고 생성해야 합니다.
	elseif 104 == errCode  then
		AddWarnDataStr(GetTextW(600037), 0)      -- 사용 할 수 없는 문자가 들어있습니다.
	elseif 105 == errCode  then
		AddWarnDataStr(GetTextW(600038), 0)      -- 이름이 너무 깁니다.
	elseif 106 == errCode  then
		AddWarnDataStr(GetTextW(600024), 0)      -- 캐릭터를 생성 할 수 없습니다.(서버에서 캐릭터 생성기능 막았을 때)
	elseif 108 == errCode  then
		AddWarnDataStr(GetTextW(791520), 0)      -- Can't create drakan CCR_CANT_CREATE_DRAKAN
	elseif 200 == errCode  then
		AddWarnDataStr(GetTextW(600012), 0)      -- 더이상 캐릭 생성 불가
	elseif 250 == errCode  then
		AddWarnDataStr(GetTextW(600012), 0)      -- 알 수 없는 Guid
	elseif 107 == errCode then	
		AddWarnDataStr(GetTextW(600025), 0)      -- 이름이 너무 짧습니다
	elseif 400 == errCode then	
		AddWarnDataStr(GetTextW(600105), 0)      -- 점핑 이벤트 캐릭터를 만들수 없습니다.
	elseif 401 == errCode then	
		AddWarnDataStr(GetTextW(600106), 0)      -- 더 이상 점핑 이벤트 캐릭터를 만들수 없습니다.
	elseif 2 == errCode then
		local errPos = packet:PopInt()
		--ODS("Error Pos : " .. errPos .."\n")
	elseif 1 == errCode then 		-- 생성 성공 CCR_SUCCESS
		g_SceneType = 1
		MoveCamera_Select()
		LoadingSelectScene()
		local kClass = g_selectStage:GetNewActor():GetPilot():GetAbil(AT_CLASS)
		if (kClass == CT_SHAMAN and kClass == CT_DOUBLE_FIGHTER) then
			g_selectStage:SetDrakanCreateItemNo(g_selectStage:GetDrakanCreateItemNo() - 1)
		end
	end

    local wnd = GetUIWnd("StartGame")
    if wnd:IsNil() == false then
        UISelf:SetClose(false)
    end
	
	local iExtendSlot = g_selectStage:GetEnableExtendSlot()
	SetLockedExtendSlot(iExtendSlot)
	
	return true
end
---------------------------------------------------------------
function Net_ReqDeleteCharacter(ConfirmNumber)
	-- PT_C_S_REQ_DELETE_CHARACTER(4115)

	local selectedActorGuid = g_selectStage:GetSelectedActor()
	if selectedActorGuid:IsNil() then
		return true
	end

	packet = NewPacket(4115)				--PT_C_S_REQ_PRE_CREATE_CHARACTER
	packet:PushGuid(selectedActorGuid)
	packet:PushWString(ConfirmNumber)
	Net_Send(packet)
	DeletePacket(packet)
	CloseUI("DeleteConfirm")

end
---------------------------------------------------------------
function Net_ReqCreateCharacter(actorName)
	-- PT_C_S_REQ_CREATE_CHARACTER 4105

	local actor = g_selectStage:GetNewActor()
	if actor == nil or
		actor:IsNil() == true then
		return true
	end

	local pilot = g_pilotMan:FindPilot(actor:GetPilotGuid())
	if pilot:IsNil() then
		return true
	end

	local iGender = pilot:GetAbil(AT_GENDER)
	local iClass = pilot:GetAbil(AT_CLASS)

	local iBodyFiveElement = pilot:GetAbil(AT_BODY_FIVE_ELEMENT_TYPE)

	--ODS("Class = " .. iClass .. "\n")

	packet = NewPacket(4105)--PT_C_S_REQ_CREATE_CHARACTER
	packet:PushWString(actorName)
	
	packet:PushChar(iGender)
	packet:PushInt(iClass)
	packet:PushInt(iBodyFiveElement)
	
	packet:PushInt(actor:GetDefaultItem(2))
	packet:PushInt(actor:GetDefaultItem(1))
	packet:PushInt(actor:GetDefaultItem(4))
	packet:PushInt(actor:GetDefaultItem(2^21))
	packet:PushInt(actor:GetDefaultItem(2^24))
	packet:PushInt(actor:GetDefaultItem(2^22))
	packet:PushInt(actor:GetDefaultItem(2^23))

	Net_Send(packet)
	DeletePacket(packet)
end
---------------------------------------------------------------
function Net_ReqCheckName(actorName)
	packet = NewPacket(4101)
	packet:PushWString(actorName)

	--ODS("actor Name : " .. actorName:GetStr() .. "\n")

	Net_Send(packet)
	DeletePacket(packet)
end

---------------------------------------------------------------

function RefreshNewCharacter(iGender, iClass)
	local createdActor = g_selectStage:GetNewActor()
	if createdActor:IsNil() == false then
		local pilot = g_pilotMan:FindPilot(createdActor:GetPilotGuid())
		if pilot:IsNil() == false then
			local iOldGender = pilot:GetAbil(AT_GENDER)
			local iOldClass = pilot:GetAbil(AT_CLASS)
			local bRefresh = true

			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			g_world:RemoveObject(createdActor:GetPilotGuid())
			local kSpawnName = "char_root_create_1"
			if g_CreateType == 0 then
				kSpawnName = "char_root_create_1"
			elseif g_CreateType == 1 then
				kSpawnName = "char_root_create_2"
			end
			g_createdActor = g_selectStage:AddNewActor(iClass, iGender, kSpawnName)
			if g_createdActor:IsNil() == false then
				g_createdActor:GetPilot():SetAbil(AT_IDLEACTION_TYPE, 101)
				g_selectStage:SetNewActor(g_createdActor)
				g_selectStage:SetDefaultFiveElement()
				g_createdActor:DetachFrom(g_iClassMarkNum)
				
			end
			iOldClass = -1
				
			g_selectStage:GetNewActor():AttachParticle(456, "char_root", "ef_select")

			if iOldClass ~= iClass then
				if iGender == 1 then
					if iClass == 1 then
						-- 남자 전사
						g_selectStage:EquipBaseItemBySeq(2, 3)
						g_selectStage:EquipBaseItemBySeq(4, 8) 
						g_selectStage:EquipBaseItemBySeq(0, 2)
					elseif iClass == 2 then
						-- 남자 법사
						g_selectStage:EquipBaseItemBySeq(2, 2)
						g_selectStage:EquipBaseItemBySeq(4, 5) 
						g_selectStage:EquipBaseItemBySeq(0, 4)					
					elseif iClass == 3 then
						-- 남자 궁수 
						g_selectStage:EquipBaseItemBySeq(2, 5)
						g_selectStage:EquipBaseItemBySeq(4, 6) 
						g_selectStage:EquipBaseItemBySeq(0, 8)					
					elseif iClass == 4 then
						-- 남자 도적
						g_selectStage:EquipBaseItemBySeq(2, 6)
						g_selectStage:EquipBaseItemBySeq(4, 7)
						g_selectStage:EquipBaseItemBySeq(0, 17)
					elseif iClass == 51 then
						-- 용족 소환사
						g_selectStage:EquipBaseItemBySeq(2, 4)
						g_selectStage:EquipBaseItemBySeq(4, 5)
						g_selectStage:EquipBaseItemBySeq(0, 5)
					elseif iClass == 52 then
						-- 용족 격투가(쌍둥이)
						g_selectStage:EquipBaseItemBySeq(2, 7)		--머리형태
						g_selectStage:EquipBaseItemBySeq(4, 1)		--얼굴
						g_selectStage:EquipBaseItemBySeq(0, 9)		--머리색깔
					end
				elseif iGender == 2 then
					if iClass == 1 then
						-- 여자 전사
						g_selectStage:EquipBaseItemBySeq(2, 3)
						g_selectStage:EquipBaseItemBySeq(4, 5) 
						g_selectStage:EquipBaseItemBySeq(0, 12)
					elseif iClass == 2 then
						-- 여자 법사
						g_selectStage:EquipBaseItemBySeq(2, 1)
						g_selectStage:EquipBaseItemBySeq(4, 6)
						g_selectStage:EquipBaseItemBySeq(0, 14)
					elseif iClass == 3 then
						-- 여자 궁수
						g_selectStage:EquipBaseItemBySeq(2, 7)
						g_selectStage:EquipBaseItemBySeq(4, 0)
						g_selectStage:EquipBaseItemBySeq(0, 15)
					elseif iClass == 4 then
						-- 여자 도적
						g_selectStage:EquipBaseItemBySeq(2, 6)
						g_selectStage:EquipBaseItemBySeq(4, 2)
						g_selectStage:EquipBaseItemBySeq(0, 16)
					elseif iClass == 51 then
						-- 용족 소환사
						g_selectStage:EquipBaseItemBySeq(2, 1)
						g_selectStage:EquipBaseItemBySeq(4, 5)
						g_selectStage:EquipBaseItemBySeq(0, 2)
					elseif iClass == 52 then
						-- 용족 격투가(쌍둥이)
						g_selectStage:EquipBaseItemBySeq(2, 1)		--머리형태
						g_selectStage:EquipBaseItemBySeq(4, 0)		--얼굴
						g_selectStage:EquipBaseItemBySeq(0, 5)		--머리색깔
					end
				end
			end
			
			if bRefresh == true then
				g_selectStage:EquipBaseItemSet(iClass, 1)	-- 첫번째 셋트
				g_selectStage:ClearTestParts()
			end
			local kWnd = GetUIWnd("FRM_BACK_IMAGE")
			if kWnd:IsNil() == false then
				local kMainWnd = kWnd:GetControl("FRM_BACK_IMAGE2")
				if kMainWnd:IsNil() == false then
					local kHair = kMainWnd:GetControl("FRM_HAIR_SELECT")
					if kHair:IsNil() == false then
						g_selectStage:CallSelectStyle_Hair(kHair)
					end
					local kFace = kMainWnd:GetControl("FRM_FACE_SELECT")
					if kFace:IsNil() == false then
						g_selectStage:CallSelectStyle_Face(kFace)
					end
				end
			end
		end
	end
end

function OnSelectRaceUI()
	local iDefaultSlot = 4
	local iExtendSlot = g_selectStage:GetEnableExtendSlot()
	if g_selectStage:GetSpawnSlotCount() >= iDefaultSlot+iExtendSlot then
		AddWarnDataStr(GetTextW(600012), 1)
		return false
	end
	CloseUI("SFRM_SERVER_INFO")
	CloseUI("SFRM_CHARACTOR_INFO")
	CloseUI("FRM_LOGO")
	ActivateUI("FRM_CHOICE_RACE")
	SetCharSelectMenu(MENU_SELECT_RACE)
	ClosePremiumUI()
end


MENU_SELECT = 1				--캐릭터 선택
MENU_CREATE = 2				--캐릭터 생성
MENU_SELECT_RACE = 3		--캐릭터 생성 - 종족 선택
MENU_REALM_COMBINE = 4		--렐름 통합

function SetCharSelectMenu(iSelectType)
	local bSelect = false
	if iSelectType == MENU_SELECT then
		bSelect = true
	else
		bSelect = false
	end
	local kWnd = ActivateUI("SFRM_CHAR_MENU")
	if kWnd:IsNil() == false then
		local kBtn = kWnd:GetControl("BTN_BACK")
		if kBtn:IsNil() == false then
			if iSelectType == MENU_REALM_COMBINE then
				kBtn:Visible(true)
			else
				kBtn:Visible(bSelect)
			end
		end
		kBtn = kWnd:GetControl("BTN_CREATE_CHARACTOR")
		if kBtn:IsNil() == false then
			if iSelectType == MENU_REALM_COMBINE then
				kBtn:Visible(false)
			else
				kBtn:Visible(bSelect)
			end
			kBtn:GetControl("IMG_JUMPING_EVENT"):Visible(g_pilotMan:IsJumpingEvent())
		end
		kBtn = kWnd:GetControl("BTN_DELETE_CHARACTOR")
		if kBtn:IsNil() == false then
			if iSelectType == MENU_REALM_COMBINE then
				kBtn:Visible(false)
			else
				kBtn:Visible(bSelect)
			end
		end
		kBtn = kWnd:GetControl("BTN_OPTION")
		if kBtn:IsNil() == false then
			if iSelectType == MENU_REALM_COMBINE then
				kBtn:Visible(false)
			else
				kBtn:Visible(bSelect)
			end
		end
		kBtn = kWnd:GetControl("BTN_SHOW_SCENARIO")
		if kBtn:IsNil() == false then
			if iSelectType == MENU_REALM_COMBINE then
				kBtn:Visible(false)
			else
				kBtn:Visible(bSelect)
			end
		end
		kBtn = kWnd:GetControl("BTN_START_GAME")
		if kBtn:IsNil() == false then
			if iSelectType == MENU_REALM_COMBINE then
				kBtn:Visible(false)
			else
				kBtn:Visible(bSelect)
			end
		end
		kBtn = kWnd:GetControl("BTN_CREATE_OK")
		if kBtn:IsNil() == false then
			if iSelectType == MENU_SELECT then
				kBtn:Visible(false)
			elseif iSelectType == MENU_CREATE then
				kBtn:Visible(true)
			elseif iSelectType == MENU_SELECT_RACE then
				kBtn:Visible(false)
			elseif iSelectType == MENU_REALM_COMBINE then
				kBtn:Visible(false)
			end
		end
		kBtn = kWnd:GetControl("BTN_CREATE_CANCLE")
		if kBtn:IsNil() == false then
			if iSelectType == MENU_SELECT then
				kBtn:Visible(false)
			elseif iSelectType == MENU_CREATE then
				kBtn:Visible(true)
			elseif iSelectType == MENU_SELECT_RACE then
				kBtn:Visible(false)
			elseif iSelectType == MENU_REALM_COMBINE then
				kBtn:Visible(false)
			end
		end
		kBtn = kWnd:GetControl("BTN_RACE_CANCLE")
		if kBtn:IsNil() == false then
			if iSelectType == MENU_SELECT then
				kBtn:Visible(false)
			elseif iSelectType == MENU_CREATE then
				kBtn:Visible(false)
			elseif iSelectType == MENU_SELECT_RACE then
				kBtn:Visible(true)
			elseif iSelectType == MENU_REALM_COMBINE then
				kBtn:Visible(false)
			end
		end
			
		kBtn = kWnd:GetControl("BTN_CREATE_VIEW_STORY")
		if kBtn:IsNil() == false then
			if iSelectType == MENU_REALM_COMBINE then
				kBtn:Visible(false)
			elseif(GetLocale() == LOCALE.NC_EU
				or GetLocale() == LOCALE.NC_FRANCE 
				or GetLocale() == LOCALE.NC_GERMANY)	then
				if iSelectType == MENU_SELECT then
					kBtn:Visible(false)
				elseif iSelectType == MENU_CREATE then
					kBtn:Visible(true)
				elseif iSelectType == MENU_SELECT_RACE then
					kBtn:Visible(false)
				end
			else
				kBtn:Visible(false)
			end
		end
	end
	g_UIStatus = iSelectType
end

function OnSelectUI()
	if true == g_bIsCreateMode then
		return
	end
	-- 캐릭터 선택 버튼 누를시 채널 UI를 호출 안하게 한것을 해제.
	IgnoreChannelUI(true, false)
	
	ActivateUI("FRM_OUTLINE_2")
	
	local iDeleteSlot = 0
	if g_selectStage ~= nil then
		local selActorGuid = g_selectStage:GetSelectedActor()
		local selActor = nil
		local pilot = g_pilotMan:FindPilot(selActorGuid)
		if pilot:IsNil() == false then
			selActor = pilot:GetActor()
			selActor:ReserveTransitAction("a_intro_up")
		end
		iDeleteSlot = g_selectStage:GetDeleteSlotCount()
	end
	if true==g_bIgnoreCombine then
		iDeleteSlot = 0
	end
	if 0 == iDeleteSlot then
		SetCharSelectMenu(MENU_SELECT)
		ActivateUI("SFRM_CHARACTOR_INFO")
		ActivateUI("FRM_LOGO")
		CloseUI("FRM_CHOICE_RACE")
		g_selectStage:SetCharInfo()
		g_selectStage:SetServerInfo()
		ActivatePremiumUI()
	else
		SetCharSelectMenu(MENU_REALM_COMBINE)
	end
end

function OffSelectUI()
	CloseUI("ToolTip2")
	
	CloseUI("SFRM_CHAR_MENU")
	CloseUI("SFRM_SERVER_INFO")
	CloseUI("SFRM_CHARACTOR_INFO")
	CloseUI("FRM_LOGO")
	ClosePremiumUI()
end

function OnCreateUI()
	SetCharSelectMenu(MENU_CREATE)
	ActivateUI("FRM_OUTLINE_2")
	ActivateUI("FRM_BACK_IMAGE")
	SetSelectClassCard()
	ActivateUI("FRM_EQUIP_GROUP")
	g_selectStage:SetServerInfo()
	local kRotate = ActivateUI("CharacterRotate")
end

function OffCreateUI()
	CloseUI("CharacterRotate")
	CloseUI("SelectClass")
	CloseUI("ToolTip2")
	CloseUI("FRM_BACK_IMAGE")
	--CloseUI("FRM_EMOTION_GROUP")
	--신규 캐릭터 생성
	--g_world:SetCamera("select_camera")
	CloseUI("SFRM_CHAR_MENU")
	CloseUI("SFRM_SERVER_INFO")
	CloseUI("FRM_EQUIP_GROUP")
end

function SetSelectClassCard()

	local bCreateNewRace = false
	if g_CreateType == 0 then
		bCreateNewRace = false
	elseif g_CreateType == 1 then
		bCreateNewRace = true
	end
	local kMainWnd = ActivateUI("SelectClass")
	if nil~=kMainWnd and kMainWnd:IsNil() == false then
		local kCard_fighter = kMainWnd:GetControl("SelectFighter")
		if kCard_fighter:IsNil() == false then
			kCard_fighter:Visible(not bCreateNewRace)
		end
		local kCard_Magician = kMainWnd:GetControl("SelectMagician")
		if kCard_Magician:IsNil() == false then
			kCard_Magician:Visible(not bCreateNewRace)
		end
		local kCard_Archer = kMainWnd:GetControl("SelectArcher")
		if kCard_Archer:IsNil() == false then
			kCard_Archer:Visible(not bCreateNewRace)
		end
		local kCard_Thief = kMainWnd:GetControl("SelectThief")
		if kCard_Thief:IsNil() == false then
			kCard_Thief:Visible(not bCreateNewRace)
		end
		local kCard_Summoner = kMainWnd:GetControl("SelectSummoner")
		if kCard_Summoner:IsNil() == false then
			kCard_Summoner:Visible(bCreateNewRace)
		end
		local kCard_Twins = kMainWnd:GetControl("SelectTwins")
		if kCard_Twins:IsNil() == false then
			kCard_Twins:Visible(bCreateNewRace)
		end
		local kCard_NewRace1 = kMainWnd:GetControl("SelectNewRace1")
		if kCard_NewRace1:IsNil() == false then
			kCard_NewRace1:Visible(bCreateNewRace)
		end
		local kCard_NewRace2 = kMainWnd:GetControl("SelectNewRace2")
		if kCard_NewRace2:IsNil() == false then
			kCard_NewRace2:Visible(bCreateNewRace)
		end
	end
	
end

function RefreshCustomControl()
	-- Hair
	local SubControl = GetUIWnd("FRM_HAIR_SET")
	if SubControl:IsNil() == true then
		return false
	else
		SubControl:VOnCall()
	end

	-- Hair Color
	local SubControl = GetUIWnd("FRM_HAIR_COLOR_SET")
	if SubControl:IsNil() == true then
		return false
	else
		SubControl:VOnCall()
	end
	-- Face
	local SubControl = GetUIWnd("FRM_FACE_SET")
	if SubControl:IsNil() == true then
		return false
	else
		SubControl:VOnCall()
	end
end

---------------------------------------------------------------
function GetUserNameControl()
	local CreateCharacterForm = GetUIWnd("FRM_BACK_IMAGE")
	if CreateCharacterForm:IsNil() == true then return nil end
	return CreateCharacterForm:GetControl("editUserName")
end
---------------------------------------------------------------
function GetUserName()
	local userNameControl = GetUserNameControl()
	if userNameControl == nil or userNameControl:IsNil() then
		return WideString("")
	end
	
	return userNameControl:GetEditText()
end
---------------------------------------------------------------
function GetConfirmNumber()
	local DeleteForm = GetUIWnd("DeleteConfirm")
	local DeleteNoControl = DeleteForm:GetControl("SFRM_EDIT_CONFIRM_NUM")
	local DeleteNoControlShd = DeleteNoControl:GetControl("SFRM_EDIT_CONFIRM_SHADOWBOX")
	local DeleteConfirmForm = DeleteNoControlShd:GetControl("editConfirmNum")
	
	if DeleteConfirmForm:IsNil() then
		return WideString("")
	end
	
	return DeleteConfirmForm:GetEditText()
end

function CleanUpCharSelectScene(world)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetSelectStage(false)
end

function LoadingSelectScene()
	if g_SceneType == 0 then
		OffSelectUI()
		Net_RecvPrepareCreateCharacter_OnDisplay()
	elseif g_SceneType == 1 then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local packet = NewPacket(4132)
		Net_Send(packet)
		DeletePacket(packet)
		local actor = g_selectStage:GetNewActor()
		g_world:RemoveObject(actor:GetPilotGuid())
		g_selectStage:ClearNewActor()
		g_world:SetMainCameraStatus("select_camera")
		OffCreateUI()
	elseif g_SceneType == 2 then
		CancelInCreate()
	end
	return true
end

function SelectClassInit(kSelf, IsPushed, iClassNo)
	SelectClassCardEvent(kSelf, "FRM_PUSH_IMG", IsPushed)
	SelectClassCardEvent(kSelf, "FRM_OVER_IMG", false)
	SelectClassCardEvent(kSelf, "FRM_FRONT_IMG", not IsPushed)

	local kWnd = kSelf:GetControl("IMG_JUMPING_EVENT")
	if false==kWnd:IsNil() then
		kWnd:Visible( g_pilotMan:IsCreateJumpingCharClass(iClassNo) )
	end
end

function SelectClassCardEvent(UISelf, FrmName, IsVisible)
	local Temp = UISelf:GetControl(FrmName)
	if Temp:IsNil() then
		return
	end
	Temp:Visible(IsVisible)
end

function SelectClassPushEvent(UISelf, Type)
	SelectClassCardEvent(UISelf, "FRM_PUSH_IMG", true)
	SelectClassCardEvent(UISelf, "FRM_FRONT_IMG", false)
	SelectClassCardEvent(UISelf, "FRM_OVER_IMG", false)
	local wndSelectClass = UISelf:GetParent()
	if Type ~= 1 then
		local UITemp = wndSelectClass:GetControl("SelectFighter")
		if UITemp:IsNil() then
			return
		end
		SelectClassCardEvent(UITemp, "FRM_PUSH_IMG", false)
		SelectClassCardEvent(UITemp, "FRM_FRONT_IMG", true)
	end
	if Type ~= 2 then
		local UITemp = wndSelectClass:GetControl("SelectMagician")
		if UITemp:IsNil() then
			return
		end 
		SelectClassCardEvent(UITemp, "FRM_PUSH_IMG", false)
		SelectClassCardEvent(UITemp, "FRM_FRONT_IMG", true)
	end
	if Type ~= 3 then
		local UITemp = wndSelectClass:GetControl("SelectArcher")
		if UITemp:IsNil() then
			return
		end
		SelectClassCardEvent(UITemp, "FRM_PUSH_IMG", false)
		SelectClassCardEvent(UITemp, "FRM_FRONT_IMG", true)
	end
	if Type ~= 4 then
		local UITemp = wndSelectClass:GetControl("SelectThief")
		if UITemp:IsNil() then
			return
		end
		SelectClassCardEvent(UITemp, "FRM_PUSH_IMG", false)
		SelectClassCardEvent(UITemp, "FRM_FRONT_IMG", true)
	end
	-- New Race--
	if Type ~= 51 then
		local UITemp = wndSelectClass:GetControl("SelectSummoner")
		if UITemp:IsNil() then
			return
		end
		SelectClassCardEvent(UITemp, "FRM_PUSH_IMG", false)
		SelectClassCardEvent(UITemp, "FRM_FRONT_IMG", true)
	end
	
	if Type ~= 52 then
		local UITemp = wndSelectClass:GetControl("SelectTwins")
		if UITemp:IsNil() then
			return
		end
		SelectClassCardEvent(UITemp, "FRM_PUSH_IMG", false)
		SelectClassCardEvent(UITemp, "FRM_FRONT_IMG", true)
	end
	--미구현
	--[[if Type ~= 51 then
		local UITemp = wndSelectClass:GetControl("SelectNewRace1")
		if UITemp:IsNil() then
			return
		end
		SelectClassCardEvent(UITemp, "FRM_PUSH_IMG", false)
		SelectClassCardEvent(UITemp, "FRM_FRONT_IMG", true)
	end
	
	if Type ~= 51 then
		local UITemp = wndSelectClass:GetControl("SelectNewRace2")
		if UITemp:IsNil() then
			return
		end
		SelectClassCardEvent(UITemp, "FRM_PUSH_IMG", false)
		SelectClassCardEvent(UITemp, "FRM_FRONT_IMG", true)
	end ]]
	
	local Class = Type
	
	local actor = g_selectStage:GetNewActor()
	if actor:IsNil() == false then
		g_selectStage:EquipBaseItemSet(Class, 1)
		local iGender = g_selectStage:GetNewActor():GetPilot():GetAbil(AT_GENDER)
		RefreshNewCharacter(iGender, Class)
		CheckGender(iGender)
		g_selectStage:GetNewActor():GetPilot():SetAbil(AT_CLASS, Class)
		local kAddPos = Point3(100,0,0)
		if g_CreateType == 1 then
			kAddPos = Point3(-100,0,0)
		end
		local	kTargetPos = g_selectStage:GetNewActor():GetPos():_Add(kAddPos);
		g_selectStage:GetNewActor():LookAt(kTargetPos)		
	end
	local kParentWnd = UISelf:GetParent();
	if(true == kParentWnd:IsNil()) then return end 
	kParentWnd:SetCustomDataAsInt(Type)
	--ODS(Type.."\n")
end


function SelectCameraAniDone()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local iSeqID = g_world:GetCameraMove_ID()
	--[[if nil~=g_CameraActor and g_CameraActor:IsNil() == false then
		g_world:RemoveObject(g_CameraActor:GetPilotGuid())
	end]]
	if iSeqID == g_kSelectCameraMode["CAMERA_MOVE_CREATE_HUMAN_IN"] then
		SetSelectCameraMove(g_kSelectCameraMode["CAMERA_MOVE_CREATE_HUMAN"])
		g_bIsCreateMode = true
		OnCreateUI()
	elseif iSeqID == g_kSelectCameraMode["CAMERA_MOVE_CREATE_DRAGON_IN"] then
		SetSelectCameraMove(g_kSelectCameraMode["CAMERA_MOVE_CREATE_DRAGON"])
		g_bIsCreateMode = true
		OnCreateUI()
	elseif iSeqID == g_kSelectCameraMode["CAMERA_MOVE_CREATE_HUMAN_OUT"] then
		SetSelectCameraMove(g_kSelectCameraMode["CAMERA_MOVE_SELECT"])
		g_bIsCreateMode = false;
		OnSelectUI()
		CallSelectCharactorUI()
	elseif iSeqID == g_kSelectCameraMode["CAMERA_MOVE_CREATE_DRAGON_OUT"] then
		SetSelectCameraMove(g_kSelectCameraMode["CAMERA_MOVE_SELECT"])
		g_bIsCreateMode = false;	
		OnSelectUI()
		CallSelectCharactorUI()
	end
end
	
function SelectGender(kSelf, iGender)
	if true == kSelf:GetCheckState() then
		return 
	end 
	local strCheckFalse = "SelectFeMale"
	if iGender == 2 then
		strCheckFalse = "SelectMale"
	end
	
	local UITemp = kSelf:GetParent():GetControl(strCheckFalse)			
	if not UITemp:IsNil() then
		kSelf:CheckState(true)
		kSelf:LockClick(true)
		UITemp:LockClick(false)
		UITemp:CheckState(false)
	
		RefreshNewCharacter(iGender, g_selectStage:GetNewActor():GetPilot():GetAbil(AT_CLASS))
		RefreshCustomControl()
		PlaySound("../Sound/U_Sound/button(gender).wav")
		g_selectStage:AddActor("CreateCharacter", "Character")
		
		local kAddPos = Point3(100,0,0)
		if g_CreateType == 1 then
			kAddPos = Point3(-100,0,0)
		end
		local	kTargetPos = g_selectStage:GetNewActor():GetPos():_Add(kAddPos);
		g_selectStage:GetNewActor():LookAt(kTargetPos)		
		
		local pChild = GetUIWnd("CharacterRotate")
		if false == pChild:IsNil() then
			pChild:VOnCall()
		end					
	end	
	CheckGender(iGender)
end

function CheckGender(iGender)
	local kMain = GetUIWnd("FRM_BACK_IMAGE")
	if nil==kMain or kMain:IsNil() then
		return false
	end
	local kSelf = kMain:GetControl("SelectGender")
	if nil==kSelf or kSelf:IsNil() then
		return false
	end
	local bSelectMale = true
	if iGender == 2 then
		bSelectMale = false
	end
	local kMale = kSelf:GetControl("SelectMale")
	local kFeMale = kSelf:GetControl("SelectFemale")
	
	kMale:SetEnable(true)
	kFeMale:SetEnable(true)
	
	kMale:CheckState(bSelectMale)
	kMale:LockClick(bSelectMale)
	kFeMale:CheckState(not bSelectMale)
	kFeMale:LockClick(not bSelectMale)
	
end

function TestEquipItem(iTestType)
	if nil==g_createdActor or g_createdActor:IsNil() then
		return false
	end
	
	local iClassNo = g_createdActor:GetAbil(AT_BASE_CLASS)
	local kArrClassItems = g_kCharSelect_Test_Wear[iClassNo]
	if nil==kArrClassItems then
		return false
	end
	
	local iGender = g_createdActor:GetAbil(AT_GENDER)
	local kArrEquipType = kArrClassItems[iTestType][iGender]
	if nil==kArrEquipType then
		return false
	end
	
	g_createdActor:SetAbil(AT_START_SET_NO, iTestType)
	
	g_createdActor:AttachParticleS(49023851, "char_root", "eff_magic_select00",0.4)
	g_createdActor:AttachParticle(49023851, "char_root", "ef_select")
	
	g_selectStage:ClearTestParts()
	for kKey, kVal in pairs(kArrEquipType) do
		--격투가일때, 풋브레이커 아이템 있으면 신발 장착하지 않도록.
		--if 0 ~= kVal then
			if 52 == iClassNo then
				if  kKey == "SHOSE" then
					if kArrEquipType["SHIELED"] == 0 then
						g_createdActor:EquipItem(kArrEquipType[kKey], false)
					end
				else
					g_createdActor:EquipItem(kArrEquipType[kKey], false)
				end
			else
				g_createdActor:EquipItem(kArrEquipType[kKey], false)
			end
		--end
	end
end

function GetCreateType()
	return g_CreateType
end

function SelectCharactorSlot(iSelectSlot)
	if MENU_REALM_COMBINE == g_UIStatus then
		return false
	end
	local kSelectGuid = g_selectStage:GetSpawnActor(iSelectSlot)
	local spawnPilot = g_pilotMan:FindPilot(kSelectGuid)
	if spawnPilot:IsNil() == false then
		local spawnActor = spawnPilot:GetActor()
		if spawnActor:IsNil() == false then
			SelectCharactor(spawnActor)
			return true
		end
	end
	return false
end

local g_kKeyboardSelect = {}
g_kKeyboardSelect[1] = { ["RIGHT"]=4, ["LEFT"]=5, ["UP"]=6, ["DOWN"]=0 }
g_kKeyboardSelect[2] = { ["RIGHT"]=5, ["LEFT"]=3, ["UP"]=8, ["DOWN"]=0 }
g_kKeyboardSelect[3] = { ["RIGHT"]=2, ["LEFT"]=0, ["UP"]=8, ["DOWN"]=0 }
g_kKeyboardSelect[4] = { ["RIGHT"]=0, ["LEFT"]=1, ["UP"]=7, ["DOWN"]=0 }
g_kKeyboardSelect[5] = { ["RIGHT"]=1, ["LEFT"]=2, ["UP"]=6, ["DOWN"]=0 }
g_kKeyboardSelect[6] = { ["RIGHT"]=7, ["LEFT"]=8, ["UP"]=7, ["DOWN"]=1 }
g_kKeyboardSelect[7] = { ["RIGHT"]=0, ["LEFT"]=6, ["UP"]=0, ["DOWN"]=4 }
g_kKeyboardSelect[8] = { ["RIGHT"]=6, ["LEFT"]=3, ["UP"]=0, ["DOWN"]=2 }

local INPUT_RIGHT = 0
local INPUT_LEFT = 1
local INPUT_UP = 2
local INPUT_DOWN = 3
local g_bOldKeyDown = false;

function SelectbyKeyboard_Right(world, bKeyDown)
	if false==g_bOldKeyDown and
	 true==bKeyDown then
		SelectbyKeyboard(INPUT_RIGHT)
	end
	g_bOldKeyDown = bKeyDown;
	return true
end

function SelectbyKeyboard_Left(world, bKeyDown)
	if false==g_bOldKeyDown and
	 true==bKeyDown then
		SelectbyKeyboard(INPUT_LEFT)
	end
	g_bOldKeyDown = bKeyDown;
	return true
end

function SelectbyKeyboard_Up(world, bKeyDown)
	if false==g_bOldKeyDown and
	 true==bKeyDown then
		SelectbyKeyboard(INPUT_UP)
	end
	g_bOldKeyDown = bKeyDown;
	return true
end

function SelectbyKeyboard_Down(world, bKeyDown)
	if false==g_bOldKeyDown and
	 true==bKeyDown then
		SelectbyKeyboard(INPUT_DOWN)
	end
	g_bOldKeyDown = bKeyDown;
	return true
end

function SelectbyKeyboard(iInputType)
	local bEnd = false
	local iCurSlot = g_selectStage:GetSelectedSlot()+1
	if nil==g_kKeyboardSelect[iCurSlot] then
		return
	end
	while false == bEnd do
		local iSelectSlot = 0
		if INPUT_RIGHT == iInputType then
			iSelectSlot = g_kKeyboardSelect[iCurSlot]["RIGHT"]
		elseif INPUT_LEFT == iInputType then
			iSelectSlot = g_kKeyboardSelect[iCurSlot]["LEFT"]
		elseif INPUT_UP == iInputType then
			iSelectSlot = g_kKeyboardSelect[iCurSlot]["UP"]
		elseif INPUT_DOWN == iInputType then
			iSelectSlot = g_kKeyboardSelect[iCurSlot]["DOWN"]
		end
		if 0==iSelectSlot then
			bEnd = true
		else
			bEnd = SelectCharactorSlot(iSelectSlot)
		end
		if false == bEnd then
			iCurSlot = iSelectSlot
		end
	end
end

function SetLockedExtendSlot(iUnLockedExtendSlot)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:DetachAllParticle()
	for iIdx = 0, (g_iMaxCharacter-1) do
		local kSlotName = nil
		if 3==iIdx or 4==iIdx then
			kSlotName = "char_root_select_0"..(iIdx+1).." 01"	--띄어쓰기 한거임.
		else
			kSlotName = "char_root_select_0"..iIdx+1
		end
		local kSlotPos = g_world:GetObjectWorldPosByName(kSlotName)
		if iIdx < 4 + iUnLockedExtendSlot then
			local SpawnGuid = g_selectStage:GetSpawnActor(iIdx+1)
			if nil==SpawnGuid or true == SpawnGuid:IsNil() then
				g_world:AttachParticle("EF_Character_UnLock1001_01", kSlotPos)
			end
		else
			g_world:AttachParticle("EF_Character_Lock1001_01", kSlotPos)
		end
	end
	CallSelectCharactorUI()
end


function ActivatePremiumUI()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	ClosePremiumUI()
	if GATTR_MYHOME==g_world:GetAttr() then
		return
	end

	if nil~=g_pilotMan and g_pilotMan:IsPremiumService() then
		if IsUIWnd("FRM_PARTY_STATE") and IsInParty() then
			ActivateUI("BTN_SMALL_PREMIUM")
		else
			ActivateUI("BTN_BIG_PREMIUM")
		end
	end
end

function ClosePremiumUI()
	CloseUI("BTN_BIG_PREMIUM")
	CloseUI("BTN_SMALL_PREMIUM")
end

function PremiumReminAlram(fRemainTime)
	GetEventTimer():DelGlobalEvent("PREMIUM_REMAIN_ALRAM")

	fRemainTime = fRemainTime - (5*60)--5분 알람
	if fRemainTime > 0 then
		GetEventTimer():AddGlobalEvent("PREMIUM_REMAIN_ALRAM", fRemainTime, "CallUI('FRM_PREMIUM_ALRAM')", 1, false)
	end
end

function CloseSelectCharactorUI()
	for iSlot=1, 8 do
		local kActorPoint = g_selectStage:GetActorPosToScreen(iSlot)
		local kSelectUI = CallUI("FRM_SELECT_"..iSlot)
		if nil~=kSelectUI and false==kSelectUI:IsNil() then
			--ODS("-----------------Close "..iSlot.."------------------\n", false, 3851)
			kSelectUI:Close()
		end
	end
end

function CallSelectCharactorUI()
	for iSlot=1, 8 do
		local kSelectUI = ActivateUI("FRM_SELECT_"..iSlot)
		if nil~=kSelectUI and false==kSelectUI:IsNil() then
			local kActorPoint = g_selectStage:GetActorPosToScreen(iSlot)
			local kSize = kSelectUI:GetSize()
			kSelectUI:SetLocation( Point2(kActorPoint:GetX()-(kSize:GetX()/2), kActorPoint:GetY()-kSize:GetY()) )
			kSelectUI:Visible(true)
		end
	end
end

function SelectCharactorUI(iIndex)
	local oldSelectedActor = g_selectStage:GetSelectedActor()
	local newSelectedActor = g_selectStage:GetSpawnActor(iIndex)
	if oldSelectedActor:GetString() == newSelectedActor:GetString() then
		return false
	end
	SelectCharactorSlot(iIndex)
end

function Send_PT_C_S_REQ_CREATE_CHARACTER(bNotUseJumpingEvent)
	if true==bNotUseJumpingEvent then
		g_pilotMan:NotUseJumpingEvent()
	end
	PlaySoundByID( "cha-creation" )
	g_selectStage:Send_PT_C_S_REQ_CREATE_CHARACTER()
	--SetEditFocus("FRM_BACK_IMAGE", "editUserName")
	GetEventTimer():Add("SET_EDIT_FOCUS_CREATE_CHARACTER", 0.1, "SetEditFocus('FRM_BACK_IMAGE', 'editUserName')", 1, false)
end

function CreateDrakanMsgBox(kClass, kDrakanCreateItemNo)
	if kDrakanCreateItemNo == 0 then
		ODS("Drakan item no eq 0\n")
		return true
	end
	if (kClass ~= CT_SHAMAN and kClass ~= CT_DOUBLE_FIGHTER) then
		ODS("Not a drakan create: " .. kClass .. "\n")
		return true
	end

	ODS("CreateCharacter Drakan\n")
	local kText = GetTT(600108)
	kText:ReplaceStr("#COUNT#", g_selectStage:GetDrakanCreateItemCount() - 1 )
	CallYesNoMsgBox(kText, GUID(), MBT_CREATE_DRAKAN_CHAR)
	return false
end

function CreateCharacter(bCreateDrakanSilent)
	ODS("CreateCharacter\n")
	local kDrakanCreateItemNo = g_selectStage:GetDrakanCreateItemNo()
	local kClass = g_selectStage:GetNewActor():GetPilot():GetAbil(AT_CLASS)
	if false == bCreateDrakanSilent then
		if false == CreateDrakanMsgBox(kClass, kDrakanCreateItemNo) then
			return
		end
	end

	local iJumpingCreateCount = g_pilotMan:GetJumpingCreateCharCount()
	if iJumpingCreateCount > 0 then
		if true==GetUserName():IsNil() then
			g_ChatMgrClient:Notice_Show( GetTextW(600013), 0 )
			return
		end

		local kText = GetTT(600100)
		
		local kEtcText = nil
		if iJumpingCreateCount == 1 then
			kText:ReplaceStr("#CLASS#", GetTT(30000+g_pilotMan:GetCreateJumpingCharClass(1)):GetStr() )
			kText:ReplaceInt("#LEVEL#", g_pilotMan:GetCreateJumpingCharLevel(1))
		end

		local kRemainText = nil
		local iRemainCount = g_pilotMan:GetJumpingRemainCount()
		if iRemainCount > 0 then
			kRemainText = GetTT(600102)
			kRemainText:ReplaceInt("#REMAIN#", iRemainCount)
		else
			kRemainText = GetTT(600104)
		end
		kText:Add("\n")
		kText:Add( kRemainText:GetStr() )
		kText:Add("\n")
		kText:Add( GetTT(600107):GetStr() )

		ClearEditFocus()
		CallYesNoMsgBox(kText, GUID(), MBT_CREATE_JUMPING_CHAR_EVENT)
	else
		Send_PT_C_S_REQ_CREATE_CHARACTER(true)
	end
end

function DoAction_JumpingClassBtn(kSelf)
	if kSelf==nil then
		return
	end
	local iJumpingCreateCount = g_pilotMan:GetJumpingCreateCharCount()
	local iIndex = kSelf:GetCustomDataAsInt()
	if iJumpingCreateCount<iIndex then
		CreateCharacter(true)
	else
		g_pilotMan:SetJumpingSelectIdx(iIndex)
		Send_PT_C_S_REQ_CREATE_CHARACTER(false)
	end
	kSelf:GetParent():CloseParent()
end

function AddJumpingClassMenuItem(kList, kText, iCommand, kGuid, bDisable)
	if nil == kList then return end
	if kList:IsNil() then return end
	
	local kItem = kList:AddNewListItem(kText)
	if kItem:IsNil() then return end
	local kWnd = kItem:GetWnd()
	if kWnd:IsNil() then return end
	
	kWnd:Disable(bDisable)
	kWnd:SetCustomDataAsInt(iCommand)--Set Command
	if kGuid ~= nil then
		kWnd:SetOwnerGuid(kGuid)
	end
end