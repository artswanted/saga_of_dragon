
function World_Portal(trigger, actor)
	World_Portal_Action( trigger, actor, 0 )
	return true
end

function World_Rag_Portal(trigger, actor, ModeNo)
	World_Portal_Action( trigger, actor, ModeNo )
	return true
end

function World_Portal_Action( trigger, actor, iIndex )
	if nil == trigger or trigger:IsNil() or nil == actor or actor:IsNil() then
		return
	end
	if nil == iIndex then
		iIndex = 0
	end
	
	ODS("World_Portal_Action : "..trigger:GetID().." num : " .. iIndex .. "\n");
	local action = actor:ReserveTransitAction("a_portal")
	if action:IsNil() == false then
		action:SetParam( 1, trigger:GetID())
		action:SetParam( 2, iIndex )
	end	
end

function World_ScrollPortal_Action( ctype, actor, posX, posY )
	local action = actor:ReserveTransitAction("a_teleport01")	
	if action:IsNil() == false then	
		action:SetParam( 1, posX )
		action:SetParam( 2, posY )
		action:SetParam( 3, ctype )
		action:BroadCast(actor:GetPilot());
	end
end

-- iInStep
-- -1 : NextStage
-- 0 : INFO
-- 1~6 : Level
function World_Mission_Portal( trigger, actor, iInStep, packettemp, MutatorCount, MutatorPacket)
	if trigger:IsActiveTime() == false then
		AddWarnDataTT(803, true)
		return false
	end
	
	local kPilot = actor:GetPilot()
	if kPilot:IsNil() == false and kPilot:IsAlive() == false then
		return false --�÷��̾ �׾��ִٸ� �̼� ���� �Ұ�...
	end

	if iInStep == nil then
		iInStep = 0
	end

	if MutatorCount == nil then
		MutatorCount = 0
	end
	ODS("Mutators count: " .. MutatorCount .. "\n")

	ODS("World_Mission_Portal trigger : "..trigger:GetID().." iInStep: "..iInStep.."\n");
	
	local packet = NewPacket(13517)	--PT_C_M_REQ_MISSION_TRIGGER_ACTION
	packet:PushString(trigger:GetID())
	packet:PushInt( iInStep )
	if 0 >= iInStep then			--  �̼� ��Ż�� ������ ���� ���ش�.
		packet:PushInt(0)
	else
		packet:PushPacket(packettemp)
	end
	
	packet:PushInt( GetMutatotSelectedCount() )
	packet:PushPacket( GetMutatorPacket() ) -- packet have list whit mutator No

	Net_Send(packet)
	DeletePacket(packet)
	
	if iInStep == 0 then
		local iMissionNum = trigger:GetParam()
		if iMissionNum >= 0 then
			local StringPath = GetMissionImgPath(iMissionNum)
			g_szMissionBGFileName = StringPath
			ODS(g_szMissionBGFileName .. "\n")
			g_iMissionDiscription = tonumber(trigger:GetParamFromParamMap("MISSION_DISCRIPTION_TEXT_NUM"))
		end
	end	
end

function World_Chaos_Portal( iInStep, packettemp )
	if iInStep == nil then
		iInStep = 0
	end
	
	local packet = NewPacket(13522)	--PT_C_M_REQ_MISSION_CHAOS_ACTION
	--packet:PushString(trigger:GetID())
	local iMissionNo = GetMissionNo()
	packet:PushInt( iMissionNo )
	packet:PushInt( iInStep )
	if 0 == iInStep then			--  �̼� ��Ż�� ������ ���� ���ش�.
		packet:PushInt(0)
	else
		packet:PushPacket(packettemp)
	end
	Net_Send(packet)
	DeletePacket(packet)
	--[[
	if iInStep == 0 then
		local iMissionNum = trigger:GetParam()
		if iMissionNum >= 0 then
			local StringPath = GetMissionImgPath(iMissionNum)
			g_szMissionBGFileName = StringPath
			ODS(g_szMissionBGFileName .. "\n")
			g_iMissionDiscription = tonumber(trigger:GetParamFromParamMap("MISSION_DISCRIPTION_TEXT_NUM"))
		end
	end--]]	
end

function World_MissionEvent_Portal( trigger, actor, iInStep, iSelect )
	if iInStep == nil then
		iInStep = 0
	end
	
	ODS("World_Mission_Portal trigger : "..trigger:GetID().." iInStep: "..iInStep.."\n");
	
	local packet = NewPacket(5001)	--PT_C_M_REQ_TRIGGER_ACTION
	packet:PushString(trigger:GetID())
	packet:PushInt( iInStep )
	packet:PushInt( iSelect )
	Net_Send(packet)
	DeletePacket(packet)
	
	if iInStep == 0 then
		local iMissionNum = trigger:GetParam()
		if iMissionNum >= 0 then
			local StringPath = GetMissionImgPath(iMissionNum)
			g_szMissionBGFileName = StringPath
			ODS(g_szMissionBGFileName .. "\n")
			g_iMissionDiscription = tonumber(trigger:GetParamFromParamMap("MISSION_DISCRIPTION_TEXT_NUM"))
		end
	end
	
end

function World_Mission_Portal_Next_Stage(trigger, actor)
	World_Mission_Portal( trigger, actor, -1 )
end

function World_Mission_DirectPortal(trigger, actor)
	local packettemp = NewPacket()
	packettemp:PushInt(0)
	World_Mission_Portal( trigger, actor, trigger:GetParam2(), packettemp )
	DeletePacket(packettemp)
end

-- function World_Dungeon_Portal(trigger, actor) CPP���� ó��

function OnDungeonEnter(kSelf)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	CreateEnterIndunParty()
	World_Portal(g_world:GetTriggerByID(kSelf:GetCustomDataAsStr():GetStr()), GetMyActor())
	kSelf:CloseParent()
end

function OnRagDungeonEnter(TriggerID, ModeNo)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	CreateEnterIndunParty()
	World_Rag_Portal(g_world:GetTriggerByID(TriggerID:GetStr()), GetMyActor(), ModeNo)
	CloseUI("FRM_ENTER_RAGNAROK_GROUND")
end

function OnCall_DungeonEnterParty(kSelf, iMapNo)
	if nil==kSelf or kSelf:IsNil() then
		return
	end
	local kPos = kSelf:GetLocation()
	kPos:SetX(kPos:GetX() + kSelf:GetSize():GetX())
	kPos:SetY(kPos:GetY() + 35)
	
	local TopWnd = CallUI("SFRM_DUNGEON_PARTY")
	if TopWnd:IsNil() then
		return
	end
	TopWnd:SetLocation(kPos)

	local ColorWnd = TopWnd:GetControl("SFRM_COLOR")
	if false == ColorWnd:IsNil() then
		ColorWnd:SetSize( Point2(ColorWnd:GetSize():GetX(), 290) )
		local ShadowWnd = ColorWnd:GetControl("SFRM_SHADOW")
		if false == ShadowWnd:IsNil() then
			ShadowWnd:SetSize( Point2(ShadowWnd:GetSize():GetX(), 290) )
		end
	end
	
	local ListWnd = TopWnd:GetControl("LST_PARTY")
	if false == ListWnd:IsNil() then
		ListWnd:SetSize( Point2(ListWnd:GetSize():GetX(), 280) )
	end

	local EnterBtn = TopWnd:GetControl("BTN_ENTER")
	if false == EnterBtn:IsNil() then
		EnterBtn:SetLocation( Point2(EnterBtn:GetLocation():GetX(), 392) )
	end
	local RefreshBtn = TopWnd:GetControl("BTN_REFRESH")
	if false == RefreshBtn:IsNil() then
		RefreshBtn:SetLocation( Point2(RefreshBtn:GetLocation():GetX(), 392) )
	end
	
	TopWnd:SetSize( Point2(TopWnd:GetSize():GetX(), 433) )
	
	local kContMapNo = NewPacket()
	kContMapNo:PushInt(1)	--MapNo Count
	kContMapNo:PushInt(iMapNo)
	TopWnd:GetControl("BTN_REFRESH"):SetCustomDataAsPacket(kContMapNo)
	DeletePacket(kContMapNo)

	Send_Refresh_DungeonPartyList(TopWnd)
end

function Send_Refresh_DungeonPartyList(kWnd)
	if nil==kWnd or kWnd:IsNil() then
		return
	end
	
	kWnd:GetControl("LST_PARTY"):ClearAllListItem()
	kWnd:GetControl("FRM_LOADING"):Visible(true)

	local kContMapNo = kWnd:GetControl("BTN_REFRESH"):GetCustomDataAsPacket()
	local kGuid = GUID()
	kGuid:Generate()
	
	kWnd:SetOwnerGuid(kGuid)

	local packet = NewPacket(21251)	--PT_C_M_REQ_INDUN_PARTY_LIST
	packet:PushGuid(kGuid)
	packet:PushInt(1) -- Dungeon Type : 1-Default, 2-Constellation
	packet:PushPacket(kContMapNo)
	Net_Send(packet)
	DeletePacket(packet)
end

function OnCall_MissionEnterParty(kSelf)
	if nil==kSelf or kSelf:IsNil() then
		return
	end
	local kPos = kSelf:GetLocation()
	kPos:SetX(kPos:GetX() + kSelf:GetSize():GetX() - 83)
	kPos:SetY(kPos:GetY() + 108)
	
	local TopWnd = ActivateUI("SFRM_DUNGEON_PARTY")
	if TopWnd:IsNil() then
		return
	end
	TopWnd:SetLocation(kPos)

	local ColorWnd = TopWnd:GetControl("SFRM_COLOR")
	if false == ColorWnd:IsNil() then
		ColorWnd:SetSize( Point2(ColorWnd:GetSize():GetX(), 290 - 52) )
		local ShadowWnd = ColorWnd:GetControl("SFRM_SHADOW")
		if false == ShadowWnd:IsNil() then
			ShadowWnd:SetSize( Point2(ShadowWnd:GetSize():GetX(), 290 - 52) )
		end
	end
	
	local ListWnd = TopWnd:GetControl("LST_PARTY")
	if false == ListWnd:IsNil() then
		ListWnd:SetSize( Point2(ListWnd:GetSize():GetX(), 280 - 52) )
	end

	local EnterBtn = TopWnd:GetControl("BTN_ENTER")
	if false == EnterBtn:IsNil() then
		EnterBtn:SetLocation( Point2(EnterBtn:GetLocation():GetX(), 392 - 52) )
	end
	local RefreshBtn = TopWnd:GetControl("BTN_REFRESH")
	if false == RefreshBtn:IsNil() then
		RefreshBtn:SetLocation( Point2(RefreshBtn:GetLocation():GetX(), 392 - 52) )
	end
	
	TopWnd:SetSize( Point2(TopWnd:GetSize():GetX(), 433 - 52) )
	
	Send_Refresh_DungeonPartyList(TopWnd)
end

function World_Portal_ByClassNo(trigger, actor)	

	local	iClassNo = actor:GetAbil(AT_CLASS)
	if iClassNo <= 0 then
		ODS("[World_Portal_ByClassNo] iClassNo Error : "..iClassNo.."\n")
		return false
	end
	
	if false == trigger:IsHavePortalAccess( iClassNo ) then
		ODS("[World_Portal_ByClassNo] ClassNo(" .. iClassNo .. ") Error!! \n")
		return false
	end
				
	ODS("World_Portal_ByClassNo trigger : "..trigger:GetID().." iClassNo:"..iClassNo.." \n");
	World_Portal_Action( trigger, actor, iClassNo )
	return true
end

function DoTeleportAction(trigger, actor, eventType)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local target_t = trigger:GetTeleportID()
	local target_pos = g_world:GetObjectPosByName(target_t:GetStr())
	--ODS("ID : " .. trigger:GetID() .. " TargetName : ".. target_t:GetStr() .. " X : " .. target_pos:GetX() .. " Y : " .. target_pos:GetY() .. "\n", false, 3851)
	if target_pos:IsZero() == true then
		local iTT = trigger:GetErrorMsgID()
		if 0 ~= iTT then
			AddWarnDataTT(iTT)
		end
		return false
	end
	
	if actor:IsMyActor() and actor:IsNowFollowing() == false then
		local action = nil
		if(6 == eventType or 7 == eventType) then
			action = actor:ReserveTransitAction("a_teleport_rocket")
		else
			action = actor:ReserveTransitAction("a_teleport")
		end
		if( action:IsNil() ) then return false end
		
		action:SetParamAsPoint(1, target_pos)
		action:SetParamInt(100, eventType)
		action:SetParam(911, target_t:GetStr())
		action:SetParam(914, trigger:GetID())
		if trigger:GetParam() > 0 then
			action:SetParamInt(915, trigger:GetParam())
		end
		if trigger:GetParam2() > 0 then
			action:SetParamInt(916, trigger:GetParam2())
		end
	end

	return true
end

---------------------------------------------------------------
--	Map_Puppet_PlayAnimation
---------------------------------------------------------------
function Map_Puppet_PlayAnimation(trigger, actor)

	local	kPuppetName = trigger:GetParamFromParamMap("PUPPET_NAME");
	local	kPuppetAnimationID = tonumber(trigger:GetParamFromParamMap("PUPPET_ANIMATION"));
	
	if kPuppetAnimationID == nil then
		return
	end
	
	local	kPuppet = GetPuppet(kPuppetName)
	if kPuppet:IsNil() then
		return
	end
	
	kPuppet:PlayAnimation(kPuppetAnimationID);

end

function Map_Puppet_PlayAnimation_OnEnter(trigger, actor)
	Map_Puppet_PlayAnimation(trigger,actor)
end
function Map_Puppet_PlayAnimation_OnUpdate(trigger, actor)
end
function Map_Puppet_PlayAnimation_OnLeave(trigger, actor)
end
---------------------------------------------------------------
--	Map_WorldEventTrigger
---------------------------------------------------------------
function Map_WorldEventTrigger_OnEnter(trigger, actor)

	OnWorldEvent_PhysXTriggerEnter(trigger,actor);

end
function Map_WorldEventTrigger_OnUpdate(trigger, actor)
end
function Map_WorldEventTrigger_OnLeave(trigger, actor)
end

function Map_WorldEventTrigger(trigger, actor)

	OnWorldEvent_PhysXTriggerAction(trigger,actor);
	
	return true
end

---------------------------------------------------------------
--	Map_EnterDungeon
---------------------------------------------------------------

function Map_EnterDungeon(trigger, actor)
	DoTeleportAction(trigger, actor, 1)
	return true
end
function Map_EnterDungeon_OnEnter(trigger, actor)
	DoTeleportAction(trigger, actor, 1)
	return true
end
function Map_EnterDungeon_OnUpdate(trigger, actor)
end
function Map_EnterDungeon_OnLeave(trigger, actor)
end


---------------------------------------------------------------
--	Map_EnterDungeon
---------------------------------------------------------------

function Party_Map_EnterDugeon(trigger, actor)
	local kWndRoadSign = GetUIWnd("FRM_CONSTELLATION_ROADSIGN")
	if kWndRoadSign:IsNil() == false then
		kWndRoadSign:Close()
	end

	DoTeleportAction(trigger, actor, 3)
	return true
end
function Party_Map_EnterDugeon_OnEnter(trigger, actor)
	DoTeleportAction(trigger, actor, 3)
	return true
end
function Party_Map_EnterDugeon_OnUpdate(trigger, actor)
end
function Party_Map_EnterDugeon_OnLeave(trigger, actor)
end

function Enter_Constellation(trigger, actor)
	if nil == trigger or trigger:IsNil() or nil == actor or actor:IsNil() then
		return
	end
	GetEventScriptSystem():ActivateEvent(20000001)
end

function Enter_Constellation_Boss(trigger, actor)
	if nil == trigger or trigger:IsNil() or nil == actor or actor:IsNil() then
		return
	end
	ReqEnterConstellation_Boss()
end

---------------------------------------------------------------
--	Map_Teleport
---------------------------------------------------------------

function Map_Teleport(trigger, actor)
	DoTeleportAction(trigger, actor, 0)
	return true
end
function Map_Teleport_OnEnter(trigger, actor)
	DoTeleportAction(trigger, actor, 0)
	return true
end
function Map_Teleport_OnUpdate(trigger, actor)
end
function Map_Teleport_OnLeave(trigger, actor)
end

---------------------------------------------------------------
--	Map_Teleport ( Not Fade-In/Out )
---------------------------------------------------------------

function Map_Telewarp(trigger, actor)
	DoTeleportAction(trigger, actor, 5)
	return true
end
function Map_Telewarp_OnEnter(trigger, actor)
	DoTeleportAction(trigger, actor, 5)
	return true
end
function Map_Telewarp_OnUpdate(trigger, actor)
end
function Map_Telewarp_OnLeave(trigger, actor)
end

---------------------------------------------------------------
--	Touch_Teleport
---------------------------------------------------------------

function Touch_Teleport_Party(trigger, actor)
	DoTeleportAction(trigger, actor, 8)
	return true
end
function Touch_Teleport_Party_OnEnter(trigger, actor)
	DoTeleportAction(trigger, actor, 8)
	return true
end
function Touch_Teleport_Party_OnUpdate(trigger, actor)
end
function Touch_Teleport_Party_OnLeave(trigger, actor)
end

---------------------------------------------------------------
--	Touch_Teleport
---------------------------------------------------------------

function Touch_Teleport(trigger, actor)
	DoTeleportAction(trigger, actor, 2)
	return true
end
function Touch_Teleport_OnEnter(trigger, actor)
	DoTeleportAction(trigger, actor, 2)
	return true
end
function Touch_Teleport_OnUpdate(trigger, actor)
end
function Touch_Teleport_OnLeave(trigger, actor)
end

---------------------------------------------------------------
--	Map_Teleport_Hint
---------------------------------------------------------------

function Map_Teleport_Hint_OnEnter(trigger, actor)
	local iCount = trigger:GetParam()
	local kSkipPosName = trigger:GetParamFromParamMap("SKIP_POS")
	iCount = iCount - 1
	if 0 == iCount then
		local kWnd = ActivateUI("FRM_TUTORIAL_SKIP_POS")
		if nil ~= kWnd and not kWnd:IsNil() then
			kWnd:SetCustomDataAsStr(kSkipPosName)
		end
	else
		trigger:SetParam(iCount)
	end
	actor:AttachParticle(289, "char_root", "e_portal")
	return true
end
function Map_Teleport_Hint_OnUpdate(trigger, actor)
	actor:Walk(DIR_UP, 50)
end
function Map_Teleport_Hint_OnLeave(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local target_t = trigger:GetParamAsString()
	local target_pos = g_world:GetObjectPosByName(target_t:GetStr())
	
	if target_pos:IsZero() == true then
		return false
	end
	actor:SetTranslate(target_pos)
	actor:StartJump(0)
end

---------------------------------------------------------------
--	Map_TeleRocket
---------------------------------------------------------------

function Map_TeleRokect(trigger, actor)
	if actor:IsRidingPet() then		
		return false
	end
	DoTeleportAction(trigger, actor, 6)
	-- DoPartyMemberTeleMoveIn(6)
	return true
end
function Map_TeleRokect_OnEnter(trigger, actor)
	if actor:IsRidingPet() then		
		return false
	end
	DoTeleportAction(trigger, actor, 6)
	--DoPartyMemberTeleMoveIn(6)
	return true
end
function Map_TeleRokect_OnUpdate(trigger, actor)
end
function Map_TeleRokect_OnLeave(trigger, actor)
end
----------------------------------------------------------------
function CallNationWarUI(trigger, actor)
	CallUI("MapMove_NationWar")
end

function CallBossUI(trigger, actor)
--	MessageBox("��")
	CallUI("MapMove_Boss")
end

function Pickup_DropBox(dropbox)
	dropbox = ToDropBox(dropbox)

	if dropbox:IsNil() == true then
		return true
	end
	
	local	kMyActor= GetMyActor();
	if kMyActor:IsNil() then
		return	true
	end
	
	local	kMyPos = kMyActor:GetPos();
	local	kBoxPos = dropbox:GetPos();
	local	fDistance = kBoxPos:Distance(kMyPos);
	ODS("The distance is "..fDistance.."\n");
	if fDistance>GetItemPickUpLimitDistance() then
		ODS("The box is too far from you\n");
		return	true
	end

	-- ���ǰ�?
	if not dropbox:IsMine() then
	--	MessageBox("�� �������� �ƴմϴ� ����~������", "����")
		return true
	--else
	--	MessageBox("�� �������� �½��ϴ� ����~������", "����")
	end

	-- �ڽ� ������ ��û�ϴ� ��Ŷ�� ������.
	boxGuid = dropbox:GetGuid()
	packet = NewPacket(12362) 	-- ��� ������ ��������
	packet:PushGuid(boxGuid)
	Net_Send(packet)
	DeletePacket(packet)

	return true
end

function Pick_PC()

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	actor = g_world:PickActor()
	if actor:IsNil() then
		return false
	end

	MessageBox("Pick_PC : "..actor:GetID());

	pilot_guid = actor:GetPilotGuid();

	Net_C_N_REQ_JOIN_PARTY(pilot_guid, PRJC_ByCharGuid);

end

function TargetMarkOn()
	GetSkillTargetMan():SetDrawTargetMark(true)
end

function TargetMarkOff()
	GetSkillTargetMan():SetDrawTargetMark(false)
end

function TargetAreaDisplayOn()
	GetSkillTargetMan():EnableTargetAreaDisplay(true)
end

function TargetAreaDisplayOff()
	GetSkillTargetMan():EnableTargetAreaDisplay(false)
end

function tadon()
	TargetAreaDisplayOn()
end

function tadoff()
	TargetAreaDisplayOff()
end

function World_Hidden_Portal( trigger, actor )	
	ODS("World_Mission_Portal trigger : "..trigger:GetID().."\n");

	local iValue = 0
	local packet = NewPacket(5001)	--PT_C_M_REQ_TRIGGER_ACTION
	packet:PushString(trigger:GetID())
	packet:PushInt( iValue )	
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_ReqEnterHidden_Npc()
	local kMyActor = g_pilotMan:GetPlayerActor()
	if false == kMyActor:IsNil() then
		local kTrigger = kMyActor:GetCurrentTrigger()
		if false == kTrigger:IsNil() then
			World_Hidden_Portal( kTrigger, kMyActor )
		else
			--g_ChatMgrClient:Notice_Show_ByTextTableNo(18995)--��Ż�� ������ϴ�.
		end
	end
end

function World_SuperGround_Portal( trigger, actor )
	CallSuperGroundEnterUI(trigger:GetParam(), trigger:GetID())
	--[[
	local iValue = 0
	local packet = NewPacket(5001)	--PT_C_M_REQ_TRIGGER_ACTION
	packet:PushString(trigger:GetID())
	packet:PushInt( iValue )
	Net_Send(packet)
	DeletePacket(packet)
	]]
end

function World_SuperGround_Portal_NextFloor( trigger, actor )
	local iValue = trigger:GetParam()
	--ODS("Param = "..iValue.."\n", false, 3851);
	local packet = NewPacket(5001)	--PT_C_M_REQ_TRIGGER_ACTION
	packet:PushString(trigger:GetID())
	packet:PushInt( iValue )
	Net_Send(packet)
	DeletePacket(packet)
end

function World_RagnarokGround_Portal( trigger, actor )
	local iValue = trigger:GetRagUI()
	CallRagUI(iValue, trigger:GetID())
end

function World_JobSkill( trigger, actor, iSkillNo )
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	if not actor:IsMeetFloor() then
		AddWarnDataStr(GetTextW(240), 2, true)
		return
	end
	
	local packet = NewPacket(5001)	--PT_C_M_REQ_TRIGGER_ACTION
	packet:PushString(trigger:GetID())
	packet:PushInt( iSkillNo )	
	Net_Send(packet)
	DeletePacket(packet)
end

TBL_REQ_ADD_UNIT_GUID = {}
TBL_REQ_ADD_UNIT_GUID["elga_3"]	= { TYPE=1,		GUID=nil }
TBL_REQ_ADD_UNIT_GUID["elga_2"]	= { TYPE=2,		GUID=nil }

function RegAddUnitGuid(iType, kGuid)
	if nil==iType or nil==kGuid then
		return
	end

	for kKey, kVal in pairs(TBL_REQ_ADD_UNIT_GUID) do
		if iType == kVal.TYPE then
			ODS("RegAddUnitGuid\n",false,6482)
			kVal.GUID = kGuid
		end
	end
end

function GetReqAddUnitGuid(kName)
	if nil~=kName then
		local kUnit = TBL_REQ_ADD_UNIT_GUID[kName]
		if nil~=kUnit then
			return kUnit.GUID
		end
	end
end

---------------------------------------------------------------
--	AddWarningMsgFormTT
---------------------------------------------------------------
function AddWarningMsgFormTT(trigger, actor)
	return true
end
function AddWarningMsgFormTT_OnEnter(trigger, actor)
	local iWarnMsg = trigger:GetParam()
	if 0 ~= iWarnMsg then
		AddWarnDataTT(iWarnMsg)
	end
	return true
end
function AddWarningMsgFormTT_OnUpdate(trigger, actor)
end
function AddWarningMsgFormTT_OnLeave(trigger, actor)
end


---------------------------------------------------------------
--	Progress Map
---------------------------------------------------------------
function Progress_Sensor_OnEnter(kTrigger, kActor)
	if kTrigger:IsNil() then
		return false
	end
	
	if kActor:IsNil() or kActor:IsMyActor() == false then
		return false
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
--	if g_world:GetCurProgress() == kTrigger:GetProgressID() then
--		return true
--	end
	
	g_world:SetCurProgress(kTrigger:GetProgressID())

	return true
end

function Progress_Sensor_OnUpdate(kTrigger, kActor)
	return true
end

function Progress_Sensor_OnLeave(kTrigger, kNextTrigger, actor)
	return true
end

---------------------------------------------------------------
--	Race Event Check Point
---------------------------------------------------------------
function Race_Check_Point_OnEnter(kTrigger, kActor)
	if kActor:IsMyActor() then
		local packet = NewPacket(21730)	--PT_C_M_NFY_RACE_CHECK_POINT
		packet:PushString(kTrigger:GetID())
		Net_Send(packet)
		DeletePacket(packet)
	end
	
	return true
end

function Race_Check_Point_OnUpdate(kTrigger, kActor)
	return true
end

function Race_Check_Point_OnLeave(kTrigger, kNextTrigger, actor)
	return true
end
