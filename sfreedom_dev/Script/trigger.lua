-- 트리거 트랩
function DoTrap_Script(trigger, actor)
	if trigger:IsNil() or actor:IsNil() or actor:IsUnitType(UT_PLAYER) == false then
		return
	end

	local kCurAction = actor:GetAction()
	if kCurAction:IsNil() or kCurAction:GetParam(100) == "Trap ReAction" or --현재 트랩 리액션 중이면 다시 전이시키지 않음
		actor:GetCanHit() == false or actor:IsGodTime() then
		return
	end
	
	if actor:IsRidingPet() then --펫에 탑승 중이라면 내려라
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end
	
	--actor:SetCanHit(false)
	actor:StartGodTime(1.5);
	
	local kReActionName = WideString()
	kReActionName = trigger:GetTriggerReactionSkillName(actor)
	local kReActionSkillNo = trigger:GetTriggerReactionSkillNo(actor)
	--local kReActionName = WideString("a_dmg")
	--local kReActionSkillNo = 100001901
	
	if kReActionName:GetStr() == "" then --트랩 스킬의 리액션 정보가 없다면 디폴트 리액션으로 "a_trap"을 사용
		kReActionName:Set("a_trap")
	end
	local kNextAction = actor:ReserveTransitAction(kReActionName:GetStr())
	if kNextAction:IsNil() or actor:IsMyActor() == false then
		return
	end
	
	SkillHelpFunc_SetReActionByTrapSkill(kNextAction, true) --이 액션이 브로드캐스팅 옵션이 꺼져있더라도 브로드캐스팅 해라.
	NET_C_M_REQ_TRIGGER(9,actor:GetPilotGuid(),trigger:GetTriggerSkillID()); --대미지/디버프 요청

	if kReActionSkillNo ~= 0 then --스킬의 리액션 번호가 없다면 액터 이펙트를 추가하지 않는다.
		actor:AddEffect(kReActionSkillNo, 0, actor:GetPilotGuid(), 0, 0, true)
	end
	
end
function DoTrap_OnEnter(trigger, actor)
	DoTrap_Script(trigger, actor)
end
function DoTrap_OnUpdate(trigger, actor)
	--DoTrap_Script(trigger, actor)
end
function DoTrap_OnLeave(trigger, nTrigger, actor)
end

-- Jump단을 설정할 수 있다.
function Trap_Jump_OnEnter(trigger, actor)
	if actor:IsMyActor() then
		UseCameraHeightAdjust(false)
	end
	actor:ReserveTransitAction("a_idle")
	actor:StartJump(trigger:GetParam())
	return true
end

function Trap_Jump_OnUpdate(trigger, actor)
	local action = actor:GetAction()
	if action:IsNil() == false then
		action:SetParam(119, "jump_trap")
	end
	return true
end

function Trap_Jump_OnLeave(trigger, nTrigger, actor)
	if actor:IsMyActor() then
		UseCameraHeightAdjust(true)
	end
end

-- XML에서 목표 물체, 높이 등등 PARAM으로 설정 할수 있는 TeleJump함수
function DoTeleJumpEx(trigger, actor)
	if trigger:IsNil() then
		return
	end
	if nil==actor or actor:IsNil() then
		return
	end
	if actor:GetAbil(AT_BEAR_EFFECT_RED) > 0
		or actor:GetAbil(AT_BEAR_EFFECT_BLUE) > 0 then
		return -- 러브러브 모드 곰을 업은 유저는 대점프를 탈 수 없다.
	end

	if actor:GetAbil(AT_HP) > 0 then
		local action = actor:GetAction()
		if action:IsNil() == false then
			action:SetParam(914, trigger:GetID())
		end
		local target_t = trigger:GetParamAsString()
		DoTeleJump(actor, target_t:GetStr(), trigger:GetParam(), trigger:GetIndex())
	end
end

function DoTeleJumpEx_OnEnter(trigger, actor)
	DoTeleJumpEx(trigger, actor)
end
function DoTeleJumpEx_OnUpdate(trigger, actor)
end
function DoTeleJumpEx_OnLeave(trigger, nTrigger, actor)
end



function ActivateEventEx(trigger, actor)

	if actor:IsMyActor() == false then
		return	
	end

	local	iEventID = trigger:GetParamIntFromParamMap("EventID")
	local	iQuestID = trigger:GetParamIntFromParamMap("QuestID");
	local	iMissionLevel = trigger:GetParamIntFromParamMap("MissionLevel");
	
	if iEventID == -1 then
	
		MessageBox("ActivateEventEx needs an EventID","ActivateEventEx Failed");
		return
	end
	
	ODS("ActivateEventEx iEventID: "..iEventID.." iQuestID:"..iQuestID.."\n");
	
	if iQuestID ~= -1 and ( not IsIngQuest(iQuestID)) then -- Check whether i'm on the quest or not
		return 
	end 
	
	if iMissionLevel ~= -1 then
	
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		if g_world:GetDifficultyLevel() ~= iMissionLevel then
			return
		end
	
	end

	GetEventScriptSystem():ActivateEvent(iEventID);

end

function ActivateEventEx_OnEnter(trigger, actor)
	ActivateEventEx(trigger, actor)
end
function ActivateEventEx_OnUpdate(trigger, actor)
end
function ActivateEventEx_OnLeave(trigger, nTrigger, actor)
end

--/////////////////////////////////// Quest Location //////////////////////////////////////
function DoSimpleLocation_OnEnter(kTrigger, kActor)
	--kTrigger:GetParam(트리거번호)
	--kTrigger:GetParam2(연관된 퀘스트)
	--kTrigger:GetParamAsString
	local sTriggerNo = kTrigger:GetParam()
	local sQuestID = kTrigger:GetParam2()
	if not IsIngQuest(sQuestID) then return end --Check Have a Quest
	kTrigger:SetEnable(false) --Disable me
	--send trigger packet
	local kPacket = NewPacket(12431)		-- PT_C_M_REQ_TRIGGER
	kPacket:PushInt(7)--TriggerType
	kPacket:PushInt(sTriggerNo) -- Trigger No
	Net_Send(kPacket)
	DeletePacket(kPacket)
end
function DoSimpleLocation_OnUpdate(kTrigger, kActor)
end
function DoSimpleLocation_OnLeave(kTrigger, kActor)
end

--/////////////////////////////////// Begin Quest Talk (DoQuestTalk) ///////////////////////////////////
function DoBeginQuestDialog_OnEnter(kTrigger, kActor)
	local sQuestID = kTrigger:GetParam()--Quest ID
	local sDialogNo = kTrigger:GetParam2()--Dialog ID
	if IsIngQuest(sQuestID) then
		return
	end
	if IsEndedQuest(sQuestID) then
		return
	end
	
	local Player = g_pilotMan:GetPlayerPilot()
	if Player:IsNil() == false then
		LockPlayerInputMove(2) -- for quest lock
	end
	
	g_kCanCancelQuestDialog = false --Can't cancel Quest Dialog
	kTrigger:SetEnable(false) --Disable me
	
	local kTempGuid = kTrigger:GetParamFromParamMap("NPC_GUID")
	local kNpcGuid = GUID(kTempGuid)
	GetQuestMan():SendDialogTrigger(kNpcGuid, sQuestID, sDialogNo)

end
function DoBeginQuestDialog_OnUpdate(kTrigger, kActor)
end
function DoBeginQuestDialog_OnLeave(kTrigger, kActor)
end
--/////////////////////////////////// Ing Quest Talk (DoQuestTalk) ///////////////////////////////////
function DoIngQuestDialog_OnEnter(kTrigger, kActor)
	local sQuestID = kTrigger:GetParam()--Quest ID
	local sNextDialog = kTrigger:GetParam2()--Dialog ID
	if not IsIngQuest(sQuestID) then
		return
	end
	
	local Player = g_pilotMan:GetPlayerPilot()
	if Player:IsNil() == false then
		LockPlayerInputMove(2) -- for quest lock
	end
	
	g_kCanCancelQuestDialog = false --Can't cancel Quest Dialog
	kTrigger:SetEnable(false) --Disable me
	
	local kTempGuid = kTrigger:GetParamFromParamMap("NPC_GUID")
	local kNpcGuid = GUID(kTempGuid)
	GetQuestMan():SendDialogTrigger(kNpcGuid, sQuestID, sNextDialog)
end
function DoIngQuestDialog_OnUpdate(kTrigger, kActor)
end
function DoIngQuestDialog_OnLeave(kTrigger, kActor)
end
--/////////////////////////////////// End Quest Talk (DoQuestTalk) ///////////////////////////////////
function DoEndQuestDialog_OnEnter(kTrigger, kActor)
	local sQuestID = kTrigger:GetParam()--Quest ID
	local sNextDialog = kTrigger:GetParam2()--Dialog ID
	if not IsIngQuest(sQuestID) then
		return
	end
	if not IsEndQuest(sQuestID) then--Is End able quest
		return
	end
	
	local Player = g_pilotMan:GetPlayerPilot()
	if Player:IsNil() == false then
		LockPlayerInputMove(2) -- for quest lock
	end
	
	g_kCanCancelQuestDialog = false --Can't cancel Quest Dialog
	kTrigger:SetEnable(false) --Disable me
	
	local kTempGuid = kTrigger:GetParamFromParamMap("NPC_GUID")
	local kNpcGuid = GUID(kTempGuid)
	GetQuestMan():SendDialogTrigger(kNpcGuid, sQuestID, sNextDialog)
end
function DoEndQuestDialog_OnUpdate(kTrigger, kActor)
end
function DoEndQuestDialog_OnLeave(kTrigger, kActor)
end

--/////////////////////////////////// Tutorial Talk (DoQuestTalk) ///////////////////////////////////
function DoTutorialHint_OnEnter(kTrigger, kActor)
	local sTTW = kTrigger:GetParam() --Quest Text Table No
	local sQuestID = kTrigger:GetParam2()--Dialog ID
	if not IsIngQuest(sQuestID) then
		return
	end
	--kTrigger:SetEnable(false) --Disable me
	local kWnd = ActivateUI("FRM_TUTORIAL_HINT")
	if kWnd:IsNil() then return end
	local kTextWnd = kWnd:GetControl("IMG_BG")
	if kTextWnd:IsNil() then return end
	kTextWnd:SetStaticTextW(GetTT(sTTW))
	kWnd:RefreshCalledTime()
end
function DoTutorialHint_OnUpdate(kTrigger, kActor)
end
function DoTutorialHint_OnLeave(kTrigger, kActor)
end

function DoMovingObject_OnEnter(kTrigger, kActor)
	UseCameraHeightAdjust(false)
	kActor:FreeMove(true)
	kActor:ApplyMovingObject_OnEnter(kTrigger)
end
function DoMovingObject_OnUpdate(kTrigger, kActor)
	kActor:ApplyMovingObject_OnEnter(kTrigger)
	--kActor:ApplyMovingObject_OnUpdate(kTrigger)
end
function DoMovingObject_OnLeave(kTrigger, kActor)
	--kActor:ApplyMovingObject_OnEnter(kTrigger)
	kActor:FreeMove(false)
	UseCameraHeightAdjust(false)
	kActor:ApplyMovingObject_OnLeave(kTrigger)
end

function DoTrigger_AutoUserItem_InPlayerInven(kTrigger, kActor)
	if nil == kTrigger or nil == kActor then return end
	
	local iUseItemNo = kTrigger:GetParam()
	
	local bRet = DoQuickUseItem(iUseItemNo)
	if false == bRet then
		local kItemName = GetItemName(iUseItemNo)
		if not kItemName:IsNil() then
			local kResultText = string.format( GetTT(700402):GetStr(), kItemName:GetStr() )
			GetChatMgrClient():AddLogMessage( WideString(kResultText), true )
		end
	end
end

g_GuardianInstallKeyDownTime = 0
function GuardianInstall_OnEnter(trigger, actor)
	if trigger:IsNil() or false == actor:IsMyActor() then
		return
	end

	OnEnterGuardianInstallTrigger(trigger:GetID(), trigger:GetTranslate())
end
function GuardianInstall_OnUpdate(trigger, actor)
	if g_pilotMan:IsLockMyInput() then
		return
	end

	local fNowTime = GetAccumTime()
	if fNowTime-g_GuardianInstallKeyDownTime>0.15 and KeyIsDown(KEY_SPACE) then
		if not IsOpenUIMode7() then
			return
		end

		g_GuardianInstallKeyDownTime = fNowTime
		if false==IsGuardianInstallStr(trigger:GetID()) then
			SetGuardianInstallLoc(trigger:GetID())
			CallUI("SFRM_INSTALL_GUARDIAN")
		end
	end
end
function GuardianInstall_OnLeave(trigger, nTrigger, actor)
	if false == actor:IsMyActor() then
		return
	end
	OnLeaveGuardianInstallTrigger()
end


g_RoadSign = {}
g_RoadSign["GetID"] = 0
g_RoadSign["OnEnterTime"] = 0
g_RoadSign["OpenMinimap"] = false
g_RoadSign["TEXT_NO"] = 0
g_RoadSign["DisplayTime"] = 4.0
g_RoadSign["AniScrTime"] = 0.0
function RoadSign_OnEnter(trigger, actor)
	if nil==trigger or trigger:IsNil() then
		return
	end

	local fAccumTime = GetAccumTime()
	if g_RoadSign["GetID"] == trigger:GetID() and fAccumTime-g_RoadSign["OnEnterTime"] <= 20 then
		return
	end

	g_RoadSign["OnEnterTime"] = 0
	g_RoadSign["TEXT_NO"] = trigger:GetParamIntFromParamMap("TEXT_NO")
	g_RoadSign["DisplayTime"] = trigger:GetParamIntFromParamMap("DisplayTime")
	if g_RoadSign["DisplayTime"] <= 0 then
		g_RoadSign["DisplayTime"] = 4.0
	end

	local bOpenMinimap = IsOpenMinimap()
	if bOpenMinimap then
		CallWorldMap(true)
	end
	g_RoadSign["OpenMinimap"] = bOpenMinimap

	
	RoadSignInit( ActivateUI("FRM_ROADSIGN") )
end

function RoadSign_OnUpdate(trigger, actor)
end

function RoadSign_OnLeave(trigger, nTrigger, actor)
	if nil==trigger or trigger:IsNil() then
		return
	end

	if g_RoadSign["OnEnterTime"] == 0 then
		g_RoadSign["OnEnterTime"] = GetAccumTime()
		g_RoadSign["GetID"] = trigger:GetID()
	end
end

--엘가 심장의 방 진입시 카메라 설정
function elga_room_of_heart_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraAdjustCameraInfo(Point3(0,0,0), Point3(0,0,0));
end

function elga_room_of_heart_OnUpdate(trigger, actor)
end

function elga_room_of_heart_OnLeave(trigger, nTrigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraAdjustCameraInfo(Point3(0,50,120), Point3(0,0,60));
end

--CameraAdjust 카메라 설정
function CameraAdjust_OnEnter(trigger, actor)
	local width = trigger:GetParamIntFromParamMap("IN_WIDTH")
	local zoom = trigger:GetParamIntFromParamMap("IN_ZOOM")
	local up = trigger:GetParamIntFromParamMap("IN_UP")
	local target_x = trigger:GetParamIntFromParamMap("IN_TARGET_X")
	local target_y = trigger:GetParamIntFromParamMap("IN_TARGET_Y")
	local target_z = trigger:GetParamIntFromParamMap("IN_TARGET_Z")

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraAdjustCameraInfo(Point3(width,zoom,up), Point3(target_x,target_y,target_z))
end

function CameraAdjust_OnUpdate(trigger, actor)
end

function CameraAdjust_OnLeave(trigger, nTrigger, actor)
	local width = trigger:GetParamIntFromParamMap("OUT_WIDTH")
	local zoom = trigger:GetParamIntFromParamMap("OUT_ZOOM")
	local up = trigger:GetParamIntFromParamMap("OUT_UP")
	local target_x = trigger:GetParamIntFromParamMap("OUT_TARGET_X")
	local target_y = trigger:GetParamIntFromParamMap("OUT_TARGET_Y")
	local target_z = trigger:GetParamIntFromParamMap("OUT_TARGET_Z")

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraAdjustCameraInfo(Point3(width,zoom,up), Point3(target_x,target_y,target_z))
end

g_NowTriggerName = ""
function Touch_Radar_OnEnter(trigger, actor)
	if actor:IsMyActor() then
		g_NowTriggerName = trigger:GetID()
		local Packet = NewPacket(15752)		-- PT_C_M_REQ_TRIGGER_ONENTER
		Packet:PushString(g_NowTriggerName)
		Net_Send(Packet)
		DeletePacket(Packet)
	end
end

function Touch_Radar_OnUpdate(trigger, actor)
end

function RadarTrigger_Leave(actor)
	local Packet = NewPacket(15754)		-- PT_C_M_REQ_TRIGGER_ONLEAVE
	Net_Send(Packet)
	DeletePacket(Packet)
	
	if actor:IsMyActor() then
		g_NowTriggerName = ""
	end
end

function Touch_Radar_OnLeave(trigger, nTrigger, actor)
	if false == nTrigger:IsNil() then
		if trigger:GetAddedType() == nTrigger:GetAddedType() then
			if false == trigger:GetDoOnLeaveScript() then
				return
			end
		end
	end
	RadarTrigger_Leave(actor)
end

g_NowKTHTriggerName = ""
function Touch_KingOfHill_OnEnter(trigger, actor)
	if actor:IsMyActor() then
		g_NowKTHTriggerName = trigger:GetID()
		local TopWnd = GetUIWnd("FRM_PVP_MAIN")
		if TopWnd:IsNil() then return end
		if false == TopWnd:IsVisible() then return end
		ActivateUI("FRM_STRONGHOLD_BG")
	end
end

function Touch_KingOfHill_OnUpdate(trigger, actor)
end

function Touch_KingOfHill_OnLeave(trigger, nTrigger, actor)
	if actor:IsMyActor() then
		g_NowKTHTriggerName = ""
		local Wnd = GetUIWnd("FRM_STRONGHOLD_BG")
		if Wnd:IsNil() then return end
		
		local TextWnd = Wnd:GetControl("FRM_TEXT")
		if false == TextWnd:IsNil() then
			TextWnd:SetStaticTextW(WideString(""))
		end
		Wnd:Close()
	end
end

function ScoreTrigger_OnEnter(trigger, actor)
	local Packet = NewPacket(15800)	-- PT_C_M_REQ_SCORE_TRIGGER_ONENTER
	Packet:PushString(trigger:GetID())
	Net_Send(Packet)
	
	DeletePacket(Packet)
end

function ScoreTrigger_OnUpdate(trigger, actor)
	return;
end

function ScoreTrigger_OnLeave(trigger, nTrigger, actor)
	return;
end
