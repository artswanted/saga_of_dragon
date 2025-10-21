
--[[
--//////////////////////////////////// OnClick_Invite_Party ////////////////////////////////////////////
function OnClick_Invite_Party(kPartyFrm)
	if kPartyFrm:IsNil() then ODS("[OnClick_Invite_Party] kPartyFrm is nil\n") end
	
	local kEditName = kPartyFrm:GetControl("EDT_JOIN_ID")
	if kEditName:IsNil() then ODS("EDT_JOIN_ID is nil") end
	
	local kEditStr = kEditName:GetEditText()
	if not kEditStr:IsNil() then
		Net_C_N_REQ_JOIN_PARTY_ByName(kEditStr)
	end
end
]]

--//////////////////////////////////// CallPartyInvite ////////////////////////////////////////////
function CallPartyJoinInvite(kGuid)
	Net_C_N_REQ_JOIN_PARTY_ByGuid(kGuid)
end
--//////////////////////////////////// CallPartyInvite ////////////////////////////////////////////
function CallPartyJoin(kGuid)
	Net_C_N_ANS_JOIN_PARTY_EX(true, kGuid)
end
--//////////////////////////////////// OnUpdate_Community_PartyList ////////////////////////////////////////////
function OnUpdate_Community_PartyList()
	local kWnd = GetUIWnd("SFRM_COMMUNITY")
	if not kWnd:IsNil() then
		local kPartyFrm = kWnd:GetControl("FRM_PARTY")
		if not kPartyFrm:IsNil() then
			RefreshPartyInfoWnd(kPartyFrm)
		end
	end
end

--//////////////////////////////////// Community_Party_People_Popup ////////////////////////////////////////////
function Community_Party_People_Popup(kSelf)
	if kSelf:IsNil() then ODS("Community_Party_People_Popup kSelf is nil\n") return end
	if not IsInParty() then return end
	
	local kGuid = kSelf:GetOwnerGuid()
	local kMyGuid = g_pilotMan:GetPlayerPilotGuid()
	local bIamMaster = IsPartyMaster(kMyGuid)
	local bMaster = IsPartyMaster(kGuid)	
		
	local kPartyCmdArray = {}

	if bIamMaster and not bMaster then
		kPartyCmdArray[1] = "INVITE_PARTY"
		kPartyCmdArray[2] = "VIEW_INFO"
		kPartyCmdArray[3] = "WHISPER"		
	end
		
	local kCursorPos = GetCursorPos()
	CallCommonPopup(kPartyCmdArray, kGuid, nil, kCursorPos)	
	--CallCommonPopup(kPartyCmdArray, PeopleFindOwnerGuid, nil, kCursorPos)		
end
--//////////////////////////////////// Community_Party_Part_Popup ////////////////////////////////////////////
function Community_Party_Part_Popup(kSelf)
	if kSelf:IsNil() then ODS("Community_Party_Part_Popup kSelf is nil\n") return end
	if IsInParty() then return end
	
	local kGuid = kSelf:GetOwnerGuid()
	
	local kPartyCmdArray = {}
	
	--kPartyCmdArray[1] = "JOIN_PARTY"
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if GATTR_BATTLESQUARE ~= g_world:GetAttr() then
		kPartyCmdArray[1] = "JOIN_PARTYFIND"	
	end
	
	local kCursorPos = GetCursorPos()
	CallCommonPopup(kPartyCmdArray, kGuid, nil, kCursorPos)	
end
--//////////////////////////////////// Community_Party_Popup ////////////////////////////////////////////
function Community_Party_Popup(kSelf)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if kSelf:IsNil() then ODS("Party Popup kSelf is nil\n") return end
	if not IsInParty() then return end
	
	local kGuid = kSelf:GetOwnerGuid()
	
	if IsInPartyMemberGuid(kGuid) == false then
		return
	end
	--[[
	--CallUI("DMY_FULLSCREEN")--Call FullScreen Dummy
	
	local kPopupGroup = GetUIWnd("SFRM_PARTY_POPUP")
	if not kPopupGroup:IsNil() then CloseUI("SFRM_PARTY_POPUP") end
	
	local kWnd = CallUI("SFRM_PARTY_POPUP")
	if not kWnd:IsNil() then
		local kPos = GetCursorPos()
		kWnd:SetLocation(kPos)--Set Popup location
		kWnd:SetOwnerGuid(kGuid)
	end
	]]
	
	local kMyGuid = g_pilotMan:GetPlayerPilotGuid()
	local bMine = kMyGuid:IsEqual(kGuid)
	local bIamMaster = IsPartyMaster(kMyGuid)
	local bMaster = IsPartyMaster(kGuid)
	
	local kPartyCmdArray = {}
	if bMine then
		kPartyCmdArray[1] = "PARTY_LEAVE"
	end
	if bIamMaster and not bMaster then
	
		if g_world:IsHaveWorldAttr(GATTR_MISSION) then 
			if GetLocale() == LOCALE.NC_SINGAPORE then
				if false == ((g_iNowCheckedCardNum+1) >= g_iMissionMapCountLua) then
					kPartyCmdArray[2] = "PARTY_KICK"
				end
			end
		else
			kPartyCmdArray[2] = "PARTY_KICK"
		end
		kPartyCmdArray[3] = "PARTY_GIVE_OWN"
	end
	if bIamMaster and bMine then
		kPartyCmdArray[5] = "PARTY_OPTION"
	end
	
	if bMine == false then
		local MyActor = GetMyActor()
		if not MyActor:IsNowFollowing() then
			local kPartyActor = g_world:FindActor(kGuid)
			if nil ~= kPartyActor and not kPartyActor:IsNil() and GetMyActor():CheckCanFollow(kPartyActor:GetPilotGuid(),false) then
				kPartyCmdArray[6] = "PARTY_FOLLOW"
			end
		else
			kPartyCmdArray[6] = "CANCEL_FOLLOW"
		end	
		--kPartyCmdArray[6] = "PARTY_FOLLOW";
		kPartyCmdArray[7] = "ADD_FRIEND";
		kPartyCmdArray[8] = "PARTY_WARP"
		if true == bMaster then
			kPartyCmdArray[9] = "PARTYGROUND_WARP"
		end
		kPartyCmdArray[10]  = "WHISPER"
		if not g_pilotMan:FindPilot(kGuid):IsNil() then
			kPartyCmdArray[11] = "PARTY_SEE_INFO"
			kPartyCmdArray[12] = "PARTY_TRADE"
		end
		kPartyCmdArray[13] = "PARTY_NAME_COPY_TO_CLIPBOARD"
	end
	
	if CheckExistHaveHome() and not bMine then
		kPartyCmdArray[14] = "PARTY_MYHOME_INVITE"	
	end
	
	local kCursorPos = GetCursorPos()
	CallCommonPopup(kPartyCmdArray, kGuid, nil, kCursorPos)
	
	CloseToolTip(nil, 11)
end

--//////////////////////////////////// OnClick_Community_PartyPopup ////////////////////////////////////////////
function OnClick_Community_PartyPopup(kCommand, kOwnerGuid)
	--OnClick_FullScreenDummy()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	ODS("Request Kick user : " .. kOwnerGuid:GetString() .."\n")
	if kOwnerGuid:IsNil() then ODS("[OnClick_Community_PartyPopup] kOwnerGuid is nil\n") return end
	
	if "LEAVE" == kCommand
		or "KICKOUT" == kCommand then
		CheckCanKick(kOwnerGuid, kCommand)
	elseif "CHANGEMASTER" == kCommand then
		if not IsCanPartyArea() or (g_world:IsHaveWorldAttr(GATTR_FLAG_MISSION+GATTR_FLAG_SUPER))  then 
			g_ChatMgrClient:AddEventMessage(700034, true, 1, 11)
			return
		end
		SendReqChangeMaster(kOwnerGuid)
	elseif "WHISPER" == kCommand then
		ChatModeChangeGuid(kOwnerGuid, true)
	end
end

--[[
--//////////////////////////////////// OnDie_StartPartyMemberDie ////////////////////////////////////////////
function OnDie_Party_StartMemberDie(kSelf, fTime)
	if nil == kSelf then return end
	if kSelf:IsNil() then return end
	
	local kCntDownFrm = kSelf:GetControl("IMG_COUNTDOWN")
	if kCntDownFrm:IsNil() then return end
	
	local kGuid = kSelf:GetOwnerGuid()
	local kStrGuid = kGuid:GetString()
	local fCurTime = GetAccumTime()
	local fEleapsedTime = fCurTime - fTime
	
	if 15 <= fEleapsedTime then
		return
	end
	
	g_kCommunity["PartyParam"][kStrGuid] = fTime
	
	kCntDownFrm:Visible(true)
end
function OnTick_Party_UpdateDieCounter(kSelf)
	local kGuid = kSelf:GetParent():GetOwnerGuid()
	local kStrGuid = kGuid:GetString()
	local fOldTime = g_kCommunity["PartyParam"][kStrGuid]
	if kOldTime == nil then return end
	
	local iCurrent = math.floor(((fOldTime+15) - GetAccumTime())/15 * 10)
	
	--local iTime = 10 - (fOldTime - GetAccumTime())
	if 0 >= iCurrent then
		UISelf:Visible(false)
		return
	end
	
	UISelf:GetControl("IMG_COUNTDOWN"):SetUVIndex(iCurrent)
end
]]

--////////////////////////////////  ////////////////////////////////
g_kListAttribute = 0
g_kAttribute = 0
g_kContinent = 0
g_kArea = 0
g_kLevel = 0
g_MaxMember = 0
g_fPartyPeopleRefreshTime = 0
g_fPartyPartRefreshTime = 0
PeopleFindOwnerGuid = 0
PartFindOwnerGuid = 0
PartListOwnerGuid = 0
SearchPeopleFindOwnerGuid = 0
g_fSummonPartyMemberTime = 0
g_fSummonPartyMemberCount = 15
kSummonPartyMemberName = 0
g_fPartyListSearchRefreshTime = 0
g_fPartyPeopleListSearchRefreshTime = 0
g_fPartyPromoteTime = 0
g_CountTime = 0
g_kDefaultLevelLimit = 1
g_kLevelLimit3 = 3
g_kLevelLimit5 = 5
g_kLevelLimit10 = 10

g_PartyListSortByName = 0
g_PartyListSortByAttr = 0
g_PartyListSortByState = 0

g_kGuidTransTowerForPartyMove = 0

g_ClassFilter = 30000
g_LevelFilter = 0

g_PartyState = false

g_bIsPraiveRegist = false

g_UICurPartyOption = {}
g_UICurPartyOption[1] = 0x0--Exp
g_UICurPartyOption[2] = 0x0--Item
g_UICurPartyOption[3] = 0x0--Public Title
g_PartyOptionEnum = {}
g_PartyOptionEnum[1] = {}	--Exp
g_PartyOptionEnum[1][0x00000000] = 0xFFFF0000
g_PartyOptionEnum[1][0x00000001] = 1	--POE_LikeSolo
g_PartyOptionEnum[1][0x00000003] = 2	--POE_CalcShare(Default)
g_PartyOptionEnum[2] = {}	--Item
g_PartyOptionEnum[2][0x00000000] = 0xFF00FFFF
g_PartyOptionEnum[2][0x00010000] = 1	--POI_LikeSolo
g_PartyOptionEnum[2][0x00030000] = 2	--POI_Order
g_PartyOptionEnum[2][0x00030000] = 2	--POI_Order
g_PartyOptionEnum[3] = {}	--Public Title
g_PartyOptionEnum[3][0x00000000] = 0x00FFFFFF
g_PartyOptionEnum[3][0x01000000] = 1	--POT_Public
g_PartyOptionEnum[3][0x03000000] = 2	--POT_Private
function PartyOption_Bit2Int(iOptionSet, Bit)
	if nil == g_PartyOptionEnum[iOptionSet] then return end
	return g_PartyOptionEnum[iOptionSet][Bit]
end
function PartyOption_Int2Bit(iOptionSet, Int)
	if nil == g_PartyOptionEnum[iOptionSet] then return end
	for kKey, kVal in pairs(g_PartyOptionEnum[iOptionSet]) do
		if kVal == Int then
			g_UICurPartyOption[iOptionSet] = kKey
			return kKey
		end
	end
end

--////////////////////////////////  ////////////////////////////////
function OnCall_Community_PartyFindInit(kTop)			
	g_kListAttribute =0
	g_kAttribute = 1
	g_kContinent = 1
	g_kArea = 0
	g_fPartyPeopleRefreshTime = 0
	g_fPartyPartRefreshTime = 0
	PeopleFindOwnerGuid = 0
	SearchPeopleFindOwnerGuid = 0
	PartFindOwnerGuid = 0
	PartListOwnerGuid = 0
	
	g_PartyListSortByName = PLS_NAMEGREATER
	g_PartyListSortByAttr = PLS_ATTRGREATER
	g_PartyListSortByState = PLS_STATELESS
	
	SetPartyListSortMethod(PLS_STATELESS)
end

function OnCall_Community_PartyOptionInit(kTop)		
	g_pilotMan:GetPlayerPilot():GetAbil(AT_LEVEL)
	g_kAttribute = 1
	g_kContinent = 1
	g_kArea = 0
	g_kLevel = g_kDefaultLevelLimit
	g_fPartyPeopleRefreshTime = 0
	g_fPartyPartRefreshTime = 0
	PeopleFindOwnerGuid = 0
	SearchPeopleFindOwnerGuid = 0
	PartFindOwnerGuid = 0
	PartListOwnerGuid = 0
	
	OnCall_Community_PartyOption(kTop, 2)
--[[	OnClick_Community_PartyOption(kTop:GetControl("FRM_ITEM"), 3, 1)
	OnClick_Community_PartyOption(kTop:GetControl("FRM_ITEM"), 2, 1)
	kTop:GetControl("FRM_ITEM"):GetControl("SFRM_BG_INPUT_LEVEL"):SetStaticText(tostring(g_kLevel))
	--]]
end
--////////////////////////////////  ////////////////////////////////
function OnCall_Community_PartyOption(kTop, iSetNo)
	if false == IsInParty() then
		g_UICurPartyOption[1] = 0x00000001
		g_UICurPartyOption[2] = 0x00030000
		g_UICurPartyOption[3] = 0x01000000
	else
		g_UICurPartyOption[1] = GetPartyOptionExp()
		g_UICurPartyOption[2] = GetPartyOptionItem()
		g_UICurPartyOption[3] = GetPartyOptionPublicTitle()
	end
	
	if nil == kTop then return end
	if kTop:IsNil() then return end
	
	local kForm = {}
	kForm[1] = kTop:GetControl("FRM_EXP")
	kForm[2] = kTop:GetControl("FRM_ITEM")
	local kCheck = {}
	kCheck[1] = kTop:GetControl("CBTN_EXP")
	kCheck[2] = kTop:GetControl("CBTN_ITEM")
	
	local kCurForm = kForm[iSetNo]
	local kCurCheck = kCheck[iSetNo]
	if nil == kCurForm then return end
	if kCurForm:IsNil() then return end
	if nil == kCurCheck then return end
	if kCurCheck:IsNil() then return end
	
	for kKey, kVal in pairs(kForm) do
		if not kVal:IsNil() then
			kVal:Visible(false)--All Visible false
		end
	end
	for kKey, kVal in pairs(kCheck) do
		if not kVal:IsNil() then
			kVal:CheckState(false)
		end
	end
	
	kCurForm:Visible(true)	
	kCurCheck:CheckState(true)
	
	local kCurOption = PartyOption_Bit2Int(iSetNo, g_UICurPartyOption[iSetNo])
	OnClick_Community_PartyOption(kCurForm, iSetNo, kCurOption)		
end

--////////////////////////////////  ////////////////////////////////
function OnClick_Community_PartyOption(kTop, iOptionSet, iOptionNo)
	if nil == kTop then return end
	if kTop:IsNil() then return end
	
	local kCheck = {}
	local kCheck1 = {}
	local kCheck2 = {}
		
	kCheck[1] = kTop:GetControl("CHK_OPTION1")
	kCheck[2] = kTop:GetControl("CHK_OPTION2")
	kCheck[3] = kTop:GetControl("CHK_OPTION3")
	
	kCheck1[1] = kTop:GetControl("CHK_OPTION4")
	kCheck1[2] = kTop:GetControl("CHK_OPTION5")
	
	kCheck2[1] = kTop:GetControl("CHK_PARTYSTATE")
	
	--local kCurCheck = kCheck[iOptionNo]	
	local kCurCheck
	
	if 2 == iOptionSet then
		kCurCheck = kCheck[iOptionNo]
	elseif 3 == iOptionSet then
		kCurCheck = kCheck1[iOptionNo]
	elseif 4 == iOptionSet then
		kCurCheck = kCheck2[iOptionNo]
	end
	
	if nil == kCurCheck then return end
	if kCurCheck:IsNil() then return end
	--[[
	for kKey, kWnd in pairs(kCheck) do--Set selected all
		if not kWnd:IsNil() then
			kWnd:CheckState(false)
		end
	end
	kCurCheck:CheckState(true)--Set selected check btn
	]]	
	
	if 2 == iOptionSet then
		for kKey, kWnd in pairs(kCheck) do--Set selected all
			if not kWnd:IsNil() then
				kWnd:CheckState(false)
			end
		end	
	elseif 3 == iOptionSet then
		for kKey, kWnd in pairs(kCheck1) do--Set selected all
			if not kWnd:IsNil() then
				kWnd:CheckState(false)
			end
		end	
	end
	if 4 ~= iOptionSet then
		kCurCheck:CheckState(true)
	end
	
	if 4 == iOptionSet then
		for kKey, kWnd in pairs(kCheck2) do
			if not kWnd:IsNil() then
				if false == g_PartyState then
					g_PartyState = true
					kWnd:CheckState(true)
				else
					g_PartyState = false
					kWnd:CheckState(false)
				end
			end
		end
	end

	--[[
	if 4 <= iOptionNo then 
		kCheck[4]:CheckState(false)
		kCheck[5]:CheckState(false)		
	elseif 3 >= iOptionSet then 
		kCheck[1]:CheckState(false)
		kCheck[2]:CheckState(false)
		kCheck[2]:CheckState(false)
	end	
	kCheck[iOptionNo]:CheckState(true);
	]]
	
	g_UICurPartyOption[iOptionSet] = PartyOption_Int2Bit(iOptionSet, iOptionNo)
end

--//////////////////////////////// CallPartyCreate ////////////////////////////////
function CallPartyCreate()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if not IsCanPartyArea() or g_world:IsHaveWorldAttr(GATTR_FLAG_MISSION+GATTR_FLAG_SUPER) or g_world:IsHaveWorldAttr(GATTR_FLAG_PUBLIC_CHANNEL)  then
		g_ChatMgrClient:AddEventMessage(700034, true, 1, 11)
		return
	end
	
	if not IsInParty() then
		--Create
		-- CallInputBox(GetTT(700101), 4, true, 15)		
		CallUI("SFRM_PARTY_OPTION", false)	
		RefreshPartyTitleWnd()
		--[[
		if IsPartyMaster( g_pilotMan:GetPlayerPilotGuid() ) then
			CallUI("SFRM_PARTY_OPTION", true)
		else
			CommonMsgBox(GetTT(700093):GetStr(), true)			
		end
		]]
	else
		--CommonMsgBox(GetTT(700056):GetStr(), true)
		CallPartyOption()
	end
	
end

--//////////////////////////////// CallPartyRename ////////////////////////////////
function CallPartyRename()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if not IsCanPartyArea() or (g_world:IsHaveWorldAttr(GATTR_FLAG_MISSION+GATTR_FLAG_SUPER))  then
		g_ChatMgrClient:AddEventMessage(700034, true, 1, 11)
		return
	end
	
	if not IsInParty() then
		CommonMsgBox(GetTT(22002):GetStr(), true)
	else
		if IsPartyMaster( g_pilotMan:GetPlayerPilotGuid() ) then
			--Rename
			CallInputBox(GetTT(700102), 5, true, 12);
		else
			CommonMsgBox(GetTT(700093):GetStr(), true)
		end
	end
end

--//////////////////////////////// CallPartyLeave////////////////////////////////
function CallPartyLeave()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if not IsCanPartyArea() or (g_world:IsHaveWorldAttr(GATTR_FLAG_MISSION+GATTR_FLAG_SUPER))  then
		g_ChatMgrClient:AddEventMessage(700034, true, 1, 11)
		return
	end
	
	if IsInParty() then
		SendReqLeaveParty()
	else
		CommonMsgBox(GetTT(22002):GetStr(), true)
	end
end

--//////////////////////////////// CallPartyInvite ////////////////////////////////
function CallPartyInvite()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if not IsCanPartyArea() or g_world:IsHaveWorldAttr(GATTR_FLAG_MISSION+GATTR_FLAG_SUPER) or g_world:IsHaveWorldAttr(GATTR_FLAG_PUBLIC_CHANNEL)  then
		g_ChatMgrClient:AddEventMessage(700034, true, 1, 11)
		return
	end
	
	if not IsInParty() then
		CallInputBox(GetTT(400271), 3, true, 10);
	else
		if IsPartyMaster( g_pilotMan:GetPlayerPilotGuid() ) then
			if true == IsUIWnd("SFRM_HCD_VOTE") then
				g_ChatMgrClient:AddEventMessage(402200, true, 1, 11)
			else
				CallInputBox(GetTT(400271), 3, true, 10)
			end
		else
			CommonMsgBox(GetTT(700093):GetStr(), true)
		end
	end
end

--//////////////////////////////// CallPartyOption ////////////////////////////////
function CallPartyOption()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if not IsCanPartyArea() or (g_world:IsHaveWorldAttr(GATTR_FLAG_MISSION+GATTR_FLAG_SUPER))  then
		g_ChatMgrClient:AddEventMessage(700034, true, 1, 11)
		return
	end
	
	if IsInParty() then
		if IsPartyMaster( g_pilotMan:GetPlayerPilotGuid() ) then
			if true == IsUIWnd("SFRM_HCD_VOTE") then
				g_ChatMgrClient:AddEventMessage(402202, true, 1, 11)
			else
				CallUI("SFRM_PARTY_OPTION", false)
				RefreshPartyTitleWnd()
			end
		else
			CommonMsgBox(GetTT(700093):GetStr(), true)
		end
	else
		CommonMsgBox(GetTT(22002):GetStr(), true)
	end
end

--//////////////////////////////// CallPartyItemLocation ////////////////////////////////
function CallPartyItemLocation(kCharGuid)
	if nil == kCharGuid or kCharGuid:IsNil() then return end
	local iGndNo = GetPartyMemberLocation(kCharGuid)
	if 0 ~= iGndNo then
		local kLocationStr = GetAppendTextW(GetTT(400270), GetMapNameW(iGndNo))
		if IsPartyMaster(kCharGuid) then
			kLocationStr = GetAppendTextW(kLocationStr, GetTT(14))
			local kTemp = WideString("[ ")
			kLocationStr = GetAppendTextW(kLocationStr, kTemp)
			kLocationStr = GetAppendTextW(kLocationStr, GetPartyOptionWStr())
			local kTemp = WideString(" ]")
			kLocationStr = GetAppendTextW(kLocationStr, kTemp)
		end
		CallToolTipText(0, kLocationStr, GetCursorPos(), "ToolTip2", 11)
	else
		CloseToolTip(nil, 11)
	end
end

--//////////////////////////////// CallPartyFind ////////////////////////////////
function CallPartyFind()
	if IsInParty() then
		CallPartyPeopleFind()
	else
		--CallUI("SFRM_PARTY_PART_FIND", false)
		--Send_RequestPartyList()
		CallPartyListFind()
	end
end

function CallPartyPeopleFind()
	CallUI("SFRM_PARTY_PEOPLE_FIND", false)
end

function CallPartyListFind()
	CallUI("SFRM_PARTY_PART_FIND")
	
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("CBTN_ITEM"):CheckState(true)
	SetPartyListInit()
	local iArea = SetPartyFindAttribute(0)	
	OnPartyFind_Continent(g_kContinent)			
	SetPartyFindListInit()
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_ALL"):CheckState(true)
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_LIST_ALL"):CheckState(true)
end

function CallPartyMiniMapMenu()
	local kPartyCmdArray = {}
	kPartyCmdArray[1] = "PARTY_FIND"

	local kCursorPos = GetCursorPos()
	kCursorPos:SetX( kCursorPos:GetX() - 129)
	
	CallCommonPopup(kPartyCmdArray, GetMyActor():GetPilotGuid(), nil, kCursorPos)	
end

--//////////////////////////////// OnSelectParty_Level ////////////////////////////////
function OnSelectParty_MaxMember(kVal)
	g_MaxMember = kVal
	GetUIWnd("SFRM_PARTY_OPTION"):GetControl("FRM_ITEM"):GetControl("SFRM_BG_MAX_MEMBER"):SetStaticText("  "..g_MaxMember.." "..GetTT(1986):GetStr())
end

function OnSelectParty_Continent(kVal)
	g_kContinent = kVal
	
	local iTTW = 700
	if 1== g_kContinent then			--  바람대륙
		iTTW = 700
	elseif 2== g_kContinent then		--  혼돈대륙
		iTTW = 701
	elseif 3 == g_kContinent then		-- 고요의 섬
		iTTW = 703		
	end
	
	GetUIWnd("SFRM_PARTY_OPTION"):GetControl("FRM_ITEM"):GetControl("SFRM_BG_INPUT_CONTINENT"):SetStaticText(GetTT(iTTW):GetStr())
end

function OnSelectParty_Area(kVal)
	g_kArea = kVal
	GetUIWnd("SFRM_PARTY_OPTION"):GetControl("FRM_ITEM"):GetControl("SFRM_BG_INPUT_AREA"):SetStaticText(GetTT(kVal):GetStr())
end

function OnSelectParty_Level(kVal)
	g_kLevel = kVal
	local PlayerLevel = g_pilotMan:GetPlayerPilot():GetAbil(AT_LEVEL)
	if 0 == kVal then
		GetUIWnd("SFRM_PARTY_OPTION"):GetControl("FRM_ITEM"):GetControl("SFRM_BG_INPUT_LEVEL"):SetStaticText(GetTextW(700552):GetStr())
	else
		GetUIWnd("SFRM_PARTY_OPTION"):GetControl("FRM_ITEM"):GetControl("SFRM_BG_INPUT_LEVEL"):SetStaticText("-"..tostring(kVal).." ~ ".."+"..tostring(kVal))
	end
end

--//////////////////////////////// Party Find ////////////////////////////////
function OnPartyFind_Continent(kVal)
	g_kContinent = kVal
	
	local iTTW = 700
	if 1== g_kContinent then			--  바람대륙
		iTTW = 700
	elseif 2== g_kContinent then		--  혼돈대륙
		iTTW = 701
	elseif 3 == g_kContinent then		-- 고요의 섬
		iTTW = 703
	end
	
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("SFRM_BG_INPUT_CONTINENT"):SetStaticText(GetTT(iTTW):GetStr())
end

function OnPartyFind_Area(kVal)
	g_kArea = kVal
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("SFRM_BG_INPUT_AREA"):SetStaticText(GetTT(kVal):GetStr())
end
--//////////////////////////////// CallRefreshPartyPeopleFindWnd ////////////////////////////////
function CallRefreshPartyPeopleFindWnd(UISelf)
	RefreshPartyPeopleFindWnd(UISelf)
end

--//////////////////////////////// CallSend_RequestPartyList ////////////////////////////////
function CallSend_RequestPartyList()
	--Send_RequestPartyList()
	
	Send_RequestPartyListInfo(g_kAttribute, g_kContinent, g_kArea)
end

function CallSend_RequestPartyPeopleList()
	Send_RequestPartyPeopleListInfo(g_ClassFilter, g_LevelFilter)
end

function PartyFindList_Cursor(kWnd)
	local kListWnd = kWnd
	
	if nil == kListWnd or kListWnd:IsNil() then return end				
	
	local kListItem = kListWnd:ListFirstItem()
	
	while kListItem:IsNil() == false do				
		kListItem:GetWnd():GetControl("IMG_BG_LINE"):Visible(false)
		kListItem = kListWnd:ListNextItem(kListItem)
	end

	UISelf:GetParent():GetControl("IMG_BG_LINE"):SetUVIndex(3)
	UISelf:GetParent():GetControl("IMG_BG_LINE"):Visible(true)
end

--//////////////////////////////// CallPartyPeopleClass ////////////////////////////////
function CallPartyPeopleClass(kCharGuid)
	if nil == kCharGuid or kCharGuid:IsNil() then return end
	local ClassName = GetClassName(kCharGuid)
	if nil ~= ClassName then
		CallToolTipText(0, ClassName, GetCursorPos(), "ToolTip2", 11)
	else
		CloseToolTip(nil, 11)
	end
end

function CallPartyWarp(kCharGuid)
	if nil == kCharGuid or kCharGuid:IsNil() then return end
	Net_PT_C_M_REQ_MOVETOPARTYMEMBER(kCharGuid)
end

function CallPartyGroundWarp(kCharGuid)
	if nil == kCharGuid or kCharGuid:IsNil() then return end
	Net_PT_C_M_REQ_MOVETOPARTYMASTERGROUND(kCharGuid)
end

function CallPartyPeopleCall(kCharGuid)
	if nil == kCharGuid or kCharGuid:IsNil() then return end
	Net_PT_C_M_REQ_SUMMONPARTYMEMBER(kCharGuid)	
end

function SetPartyAttribute(iAttribute)
	g_kAttribute = iAttribute

	GetUIWnd("SFRM_PARTY_OPTION"):GetControl("FRM_ITEM"):GetControl("BTN_FIELD"):CheckState(false)
	GetUIWnd("SFRM_PARTY_OPTION"):GetControl("FRM_ITEM"):GetControl("BTN_MISSION"):CheckState(false)
	GetUIWnd("SFRM_PARTY_OPTION"):GetControl("FRM_ITEM"):GetControl("BTN_INDUN"):CheckState(false)
	GetUIWnd("SFRM_PARTY_OPTION"):GetControl("FRM_ITEM"):GetControl("BTN_HIDDEN"):CheckState(false)
	GetUIWnd("SFRM_PARTY_OPTION"):GetControl("FRM_ITEM"):GetControl("BTN_CHAOS"):CheckState(false)
	
	local iArea = SetPartyOptionArea(g_kAttribute, g_kContinent)
	OnSelectParty_Area(iArea)	
end

function SetPartyFindAttribute(iAttribute)
	g_kAttribute = iAttribute

	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_ALL"):CheckState(false)
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_FIELD"):CheckState(false)
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_MISSION"):CheckState(false)
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_INDUN"):CheckState(false)
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_HIDDEN"):CheckState(false)
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_CHAOS"):CheckState(false)
	
	local iArea = SetPartyOptionArea(g_kAttribute, g_kContinent, true)
	OnPartyFind_Area(iArea)	
end

function SetPartyFindListInit()
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_LIST_ALL"):CheckState(false)
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_LIST_FIELD"):CheckState(false)
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_LIST_MISSION"):CheckState(false)
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_LIST_INDUN"):CheckState(false)
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_LIST_HIDDEN"):CheckState(false)
	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_ITEM"):GetControl("BTN_LIST_CHAOS"):CheckState(false)
end

function SetPartyFindListAttribute(iAttribute)
	g_kListAttribute = iAttribute
	
	SetPartyFindListInit()	
	PartyListViewRefresh(0, iAttribute)
end

function GetSelectedListAttribute()
	return g_kListAttribute
end
--//////////////////////////////////// CreateParty /////////////////////////////////////////
function CreateParty()
	local kParent = GetUIWnd("SFRM_PARTY_OPTION")
	local kEditBox = kParent:GetControl("EDT_CHARNAME_1")
	if nil == kEditBox or kEditBox:IsNil() then return end
	local kInputText = kEditBox:GetEditText()
	
	local kEditBox1 = kParent:GetControl("EDT_CHARNAME_2")
	if nil == kEditBox1 or kEditBox1:IsNil() then return end
	local kInputText1 = kEditBox1:GetEditText()	
	
	local kIsParty = IsInParty()
	
	--g_kContinent = GetUIWnd("SFRM_PARTY_OPTION"):GetControl("FRM_ITEM"):GetControl("SFRM_BG_INPUT_CONTINENT"):GetCustomDataAsInt()
	--g_kArea = GetUIWnd("SFRM_PARTY_OPTION"):GetControl("FRM_ITEM"):GetControl("SFRM_BG_INPUT_AREA"):GetCustomDataAsInt()
	--g_kLevel = GetUIWnd("SFRM_PARTY_OPTION"):GetControl("FRM_ITEM"):GetControl("SFRM_BG_INPUT_LEVEL"):GetCustomDataAsInt()
				
	if not kIsParty then 
		if true == kInputText:FilterString(1, false) then
			CommonMsgBox(GetTT(700105):GetStr(), true)
			return
		end
		if true == g_ChatMgrClient:CheckChatTag(kInputText) then	-- Check Chat Tag
			CommonMsgBox(GetTT(700105):GetStr(), true)
			return
		end
		
		if true == kInputText1:FilterString(1, false) then
			CommonMsgBox(GetTT(401355):GetStr(), true)
			return
		end
		if true == g_ChatMgrClient:CheckChatTag(kInputText1) then	-- Check Chat Tag
			CommonMsgBox(GetTT(401355):GetStr(), true)
			return
		end
		
		SetPartyOptionNew(g_kAttribute, g_kContinent, g_kArea, g_MaxMember, kInputText1)
		SetPartyOption(g_UICurPartyOption[1], g_UICurPartyOption[2], g_UICurPartyOption[3], 0, g_PartyState)
		Net_C_N_REQ_CREATE_PARTY(kIsInParty, kInputText)
	else
		if true == kInputText:FilterString(1, false) then
			CommonMsgBox(GetTT(700105):GetStr(), true)
			return
		end
		if true == kInputText1:FilterString(1, false) then
			CommonMsgBox(GetTT(401355):GetStr(), true)
			return
		end		
		
		SetPartyOption1(g_UICurPartyOption[1], g_UICurPartyOption[2], g_UICurPartyOption[3], 0, g_PartyState)
		SetPartyOption2(g_kAttribute, g_kContinent, g_kArea, g_MaxMember, kInputText1)
		Send_RequestChangeOption(kInputText)
	end
	ActivateUI("FRM_PARTY_STATE")
	--kSelf:CloseParent()
end

function CreateEnterIndunParty()
	local kIsParty = IsInParty()
	if kIsParty then
		return
	end
	
	-- SetPartyOptionNew(0, 0, 0, WideString(""))
	-- SetPartyOption(0, 0, 0, 0, false)
	Net_C_N_REQ_CREATE_PARTY(kIsParty, GeneratePartyName())
end

function PartyStateCloseBtnOver(kWnd)
	if nil==kWnd or kWnd:IsNil() then return end
	if not IsInParty() then return end
	
	local kOwner = kWnd:GetOwnerGuid()
	if false==IsInPartyMemberGuid(kOwner) then return end
	
	local kMyGuid = g_pilotMan:GetPlayerPilotGuid()
	local bMine = kMyGuid:IsEqual(kOwner)
	local bIamMaster = IsPartyMaster(kMyGuid)
	
	local bCanKick_Leave = CheckCanKick_Leave()

	if bMine then
		CloseToolTip()
		if bCanKick_Leave then
			CallMutableToolTipText(GetTT(400512), GetUIWnd("Cursor"):GetLocation())
		else
			CallMutableToolTipText(GetTT(401364), GetUIWnd("Cursor"):GetLocation())
		end
		return
	end

	if not bCanKick_Leave then
		local WalkImg = kWnd:GetControl("IMG_WALK")
		if false == WalkImg:IsNil() then
			bCanKick_Leave = WalkImg:IsVisible()
		end
	end
	
	if bIamMaster then
		CloseToolTip()
		if bCanKick_Leave then
			CallMutableToolTipText(GetTT(400511), GetUIWnd("Cursor"):GetLocation())
		else
			CallMutableToolTipText(GetTT(401363), GetUIWnd("Cursor"):GetLocation())
		end
		return
	end
end


function CallPartyCloseBtnOver(kSelf)
	if nil==kSelf or kSelf:IsNil() then return end

	if not IsInParty() then return end

	local kOwner = kSelf:GetOwnerGuid()
	if false==IsInPartyMemberGuid(kOwner) then return end

	local kMyGuid = g_pilotMan:GetPlayerPilotGuid()
	local bMine = kMyGuid:IsEqual(kOwner)
	local bIamMaster = IsPartyMaster(kMyGuid)

	if not bMine then
		CallPartyItemLocation(kOwner)
	end

	if bMine or bIamMaster then
		kSelf:GetControl("BTN_CLOSE"):Visible(true)
	end
end

function CallPartyCloseBtnOut(kSelf)
	CloseToolTip()

	if nil==kSelf or kSelf:IsNil() then return end

	if not IsInParty() then return end

	local kOwner = kSelf:GetOwnerGuid()
	if false==IsInPartyMemberGuid(kOwner) then return end

	local kMyGuid = g_pilotMan:GetPlayerPilotGuid()
	local bMine = kMyGuid:IsEqual(kOwner)

	if not bMine then
		kSelf:GetControl("BTN_CLOSE"):Visible(false)
	end
end

function CallPartyListToolTip(kCharGuid)
	if nil == kCharGuid or kCharGuid:IsNil() then return end
	local ToolTipStr = GetPartyListToolTip(kCharGuid)
	if nil ~= ToolTipStr then
		CallToolTipText(0, ToolTipStr, GetCursorPos(), "ToolTip2", 11)
	else
		CloseToolTip(nil, 11)
	end
end

function CallPartyListStateToolTip(kCharGuid)
	if nil == kCharGuid or kCharGuid:IsNil() then return end
	local ToolTipStr = GetPartyListStateToolTip(kCharGuid)
	if nil ~= ToolTipStr then
		CallToolTipText(0, ToolTipStr, GetCursorPos(), "ToolTip2", 11)
	else
		CloseToolTip(nil, 11)
	end
end

function OnSelectFindPartyClass(kVal)
	g_ClassFilter = kVal
	local iTTW = kVal;
	GetUIWnd("SFRM_PARTY_PEOPLE_FIND"):GetControl("FRM_FITER"):GetControl("FRM_CLASS_FILTER"):GetControl("SFRM_BG_COLORBOX"):SetStaticText(GetTT(iTTW):GetStr())
end

function OnSelectFindPartyLevel(kVal)
	g_LevelFilter = kVal
	if 0 == g_LevelFilter then
		GetUIWnd("SFRM_PARTY_PEOPLE_FIND"):GetControl("FRM_FITER"):GetControl("FRM_LEVEL_FILTER"):GetControl("SFRM_BG_COLORBOX"):SetStaticText(GetTT(30000):GetStr())
		return
	end
	local lValue = kVal - 10
	if 0 == lValue then
		lValue = 1
	end
	GetUIWnd("SFRM_PARTY_PEOPLE_FIND"):GetControl("FRM_FITER"):GetControl("FRM_LEVEL_FILTER"):GetControl("SFRM_BG_COLORBOX"):SetStaticText("Lv"..lValue.." ~ ".."Lv"..kVal)
end

function PrivateRegist(kEdit)
	if kEdit:IsNil() then
		return
	end
	if IsInParty() then
		return
	end
	local kContents = kEdit:GetEditText_TextBlockApplied()
	local kInputText = kEdit:GetEditText()
	kEdit:SetEditText("")
	
	if true == kInputText:FilterString(1, false) then
			CommonMsgBox(GetTT(401355):GetStr(), true)
		return
	end				
	
	local bRet = Send_PrivateRegist(kContents)
	if bRet then
		CloseUI("SFRM_PRIVATE_REGIST")
	end
end

function PartyFindPeople_Cursor(kWnd)
	local kListWnd = kWnd
	if nil == kListWnd or kListWnd:IsNil() then return end

	local kListItem = kListWnd:ListFirstItem()
	
	while kListItem:IsNil() == false do
		kListItem:GetWnd():GetControl("IMG_BG_LINE"):Visible(false)
		kListItem = kListWnd:ListNextItem(kListItem)
	end
	UISelf:GetParent():GetControl("IMG_BG_LINE"):SetUVIndex(3)
	UISelf:GetParent():GetControl("IMG_BG_LINE"):Visible(true)
end

function FindPartyPeopleInvite(kGuid)
	if kGuid:IsNil() then
		return
	end
	Net_C_N_REQ_JOIN_PARTY_ByGuid(kGuid)
end

function PartyJoinWndButtonPosModify(kOKBtn, kCancelBtn, kRefuseBtn)
	local OKPos = kOKBtn:GetLocation()
	local CancelPos = kCancelBtn:GetLocation()
	OKPos:SetX(OKPos:GetX() - 48)
	CancelPos:SetX(CancelPos:GetX() - 48)
	kOKBtn:SetLocation(OKPos)
	kCancelBtn:SetLocation(CancelPos)
	kRefuseBtn:Visible(true)
end

function CallPartyInvitePopupMenu()
	if PeopleFindownerGuid == g_pilotMan:GetPlayerPilotGuid() then return end
	local kPopupItem = {}
	kPopupItem[1] = "INVITE_PARTY"
	
	local kCursorPos = GetCursorPos()
	kCursorPos:SetX( kCursorPos:GetX() )
	
	CallCommonPopup(kPopupItem, PeopleFindOwnerGuid, nil, kCursorPos)
end

function MissionCompletePartyStateChange(bState)
	if IsPartyMaster( g_pilotMan:GetPlayerPilotGuid() ) and GetPartyOptionState()~=bState then
		g_PartyState = bState
		
		local TopWnd = GetUIWnd("FRM_PARTY_STATE")
		if TopWnd:IsNil() then return end
		local SubWnd = TopWnd:GetControl("FRM_BREAK_IN")
		if SubWnd:IsNil() then return end
		local CheckBoxWnd = SubWnd:GetControl("CHK_BREAK_IN")
		if CheckBoxWnd:IsNil() then return end

		if true == bState then
			Send_PT_C_M_REQ_JOIN_PARTY_REFUSE(true)
		else
			Send_PT_C_M_REQ_JOIN_PARTY_REFUSE(false)
		end
	end
end

function Set_PrivateRegist(kVal)
	g_bIsPraiveRegist = kVal
end

--//////////////////////////////// Expedition ////////////////////////////////

g_EpFindWindow = 1
g_EpRegisterWindowState = 1
g_EpContinent = 0
g_EpLevelFilter = 0
g_EpCurrSelectTeam = 1
g_EpOptionItem = 0
g_EpOptionPublicTitle = 0

g_UICurExpeditionOption = {}
g_UICurExpeditionOption[1] = 0x0--Exp
g_UICurExpeditionOption[2] = 0x0--Item
g_UICurExpeditionOption[3] = 0x0--Public Title
g_ExpeditionOptionEnum = {}
g_ExpeditionOptionEnum[1] = {}	--Exp
g_ExpeditionOptionEnum[1][0x00000000] = 0xFFFF0000
g_ExpeditionOptionEnum[1][0x00000001] = 1	--EOE_LikeSolo
g_ExpeditionOptionEnum[1][0x00000003] = 2	--EOE_CalcShare(Default)
g_ExpeditionOptionEnum[2] = {}	--Item
g_ExpeditionOptionEnum[2][0x00000000] = 0xFF00FFFF
g_ExpeditionOptionEnum[2][0x00010000] = 1	--EOI_LikeSolo
g_ExpeditionOptionEnum[2][0x00030000] = 2	--EOI_Order
g_ExpeditionOptionEnum[3] = {}	--Public Title
g_ExpeditionOptionEnum[3][0x00000000] = 0x00FFFFFF
g_ExpeditionOptionEnum[3][0x01000000] = 1	--EOT_Public
g_ExpeditionOptionEnum[3][0x03000000] = 2	--EOT_Private

g_fExpeditionRenameTime = 0
g_fExpeditionUserListSearchRefreshTime = 0
g_fExpeditionListSearchRefreshTime = 0
g_fExpeditionKeyItemRefreshTime = 0

g_ExpeditionGuid = 0
g_ExpeditionNpcGuid = 0

function ExpeditionOption_Bit2Int(iOptionSet, Bit)
	if nil == g_ExpeditionOptionEnum[iOptionSet] then return end
	return g_ExpeditionOptionEnum[iOptionSet][Bit]
end

function ExpeditionOption_Int2Bit(iOptionSet, Int)
	if nil == g_ExpeditionOptionEnum[iOptionSet] then return end
	for kKey, kVal in pairs(g_ExpeditionOptionEnum[iOptionSet]) do
		if kVal == Int then
			g_UICurExpeditionOption[iOptionSet] = kKey
			return kKey
		end
	end
end

--/////////////////////////////////////////////////////////
-- START (EXPEDITION OPTION)
--/////////////////////////////////////////////////////////
function OnCall_ExpeditionOptionInit()
	local MyGuid = g_pilotMan:GetPlayerPilotGuid()
	if IsInExpedition(MyGuid) then
		return
	end
	g_EpLevelFilter = g_kLevel

	g_UICurExpeditionOption[1] = g_UICurPartyOption[1]
	g_UICurExpeditionOption[2] = g_UICurPartyOption[2]
	g_UICurExpeditionOption[3] = g_UICurPartyOption[3]
end

function ExpeditionOption()
	g_EpLevelFilter = GetExpeditionOptionLevel()

	g_UICurExpeditionOption[1] = GetExpeditionOptionExp()
	g_UICurExpeditionOption[2] = GetExpeditionOptionItem()
	g_UICurExpeditionOption[3] = GetExpeditionOptionPublic()
end

function SetDefaultExpeditionOption()
	local g_EpOptionItem = ExpeditionOption_Bit2Int(2, g_UICurExpeditionOption[2])
	local g_EpOptionPublicTitle = ExpeditionOption_Bit2Int(3, g_UICurExpeditionOption[3])
	-- Init ItemOption
	OnClick_ExpeditionOption(GetUIWnd("FRM_EXPEDITION_OPTION"):GetControl("SFRM_COLOR_BOX2"), 2, g_EpOptionItem)
	-- Init PublicTitleOption
	OnClick_ExpeditionOption(GetUIWnd("FRM_EXPEDITION_OPTION"):GetControl("SFRM_COLOR_BOX2"), 3, g_EpOptionPublicTitle)
end

function OnSelectExpeditionLimitLevel(Val)
	g_EpLevelFilter = Val
	if 0 == Val then
		GetUIWnd("FRM_EXPEDITION_OPTION"):GetControl("SFRM_COLOR_BOX2"):GetControl("FRM_LIMIT_LEVEL"):SetStaticText(GetTextW(700552):GetStr())
	else
		GetUIWnd("FRM_EXPEDITION_OPTION"):GetControl("SFRM_COLOR_BOX2"):GetControl("FRM_LIMIT_LEVEL"):SetStaticText("-"..tostring(Val).." ~ ".."+"..tostring(Val))
	end
end

function OnClick_ExpeditionOption(uiTop, iOptionSet, iOptionNo)
	if nil == uiTop then return end
	if uiTop:IsNil() then return end
	
	local Check1 = {}
	local Check2 = {}
	
	-- ITEM
	Check1[1] = uiTop:GetControl("CHK_OPTION3")
	Check1[2] = uiTop:GetControl("CHK_OPTION4")
	-- PUBLIC TITLE
	Check2[1] = uiTop:GetControl("CHK_OPTION1")
	Check2[2] = uiTop:GetControl("CHK_OPTION2")

	local CurCheck
	
	if 2 == iOptionSet then				-- ITEM
		CurCheck = Check1[iOptionNo]
	elseif 3 == iOptionSet then			-- PUBLIC TITLE
		CurCheck = Check2[iOptionNo]
	end
	
	if nil == CurCheck then return end
	if CurCheck:IsNil() then return end
	
	if 2 == iOptionSet then				-- ITEM
		Check1[1]:CheckState(false)
		Check1[2]:CheckState(false)
		CurCheck:CheckState(true)
	elseif 3 == iOptionSet then			-- PUBLIC TITLE
		Check2[1]:CheckState(false)
		Check2[2]:CheckState(false)
		CurCheck:CheckState(true)
	end

	g_UICurExpeditionOption[iOptionSet] = ExpeditionOption_Int2Bit(iOptionSet, iOptionNo)
end

function ExpeditionName_Change_Apply(EditBox)
	if nil == EditBox or EditBox:IsNil() then
		return
	end

	local InputText = EditBox:GetEditText()
	if true == InputText:FilterString(1, false) then
		CommonMsgBox(GetTT(700105):GetStr(), true)
		return
	end
	Send_C_N_REQ_RENAME_EXPEDITION(InputText)
end

function ExpeditionOption_Change_Apply()
	if IsExpeditionMaster(g_pilotMan:GetPlayerPilotGuid()) then
		Send_C_N_REQ_CHANGEOPTION_EXPEDITION(g_UICurExpeditionOption[1], g_UICurExpeditionOption[2], g_UICurExpeditionOption[3], g_EpLevelFilter)
	end
	CloseUI("FRM_EXPEDITION_OPTION")
end
--/////////////////////////////////////////////////////////
-- END (EXPEDITION OPTION)
--/////////////////////////////////////////////////////////

--/////////////////////////////////////////////////////////
-- START (EXPEDITION REGISTER)
--/////////////////////////////////////////////////////////
function CreateExpedition()
	if IsInExpedition() then
		CommonMsgBox(GetTT(720001):GetStr(), true)
		return
	end
	
	if not IsPartyMaster(g_pilotMan:GetPlayerPilotGuid()) then
		CommonMsgBox(GetTT(720007):GetStr(), true)
		return
	end
	
	OnCall_ExpeditionOptionInit()

	SetExpeditionOption(g_UICurExpeditionOption[1], g_UICurExpeditionOption[2], g_UICurExpeditionOption[3], g_EpLevelFilter)
	--패킷보내기
	Send_C_N_REQ_CREATE_EXPEDITION()
end

function ResetExpeditionListOver()
	local TopWnd = GetUIWnd("FRM_EXPEDITION_REGISTER")
	if TopWnd:IsNil() then return end
	local BodyWnd = TopWnd:GetControl("FRM_BODY")
	if BodyWnd:IsNil() then return end
	local BoxWnd1 = BodyWnd:GetControl("SFRM_COLOR_BOX2")
	if BoxWnd1:IsNil() then return end
	local BoxWnd2 = BodyWnd:GetControl("SFRM_COLOR_BOX3")
	if BoxWnd2:IsNil() then return end
	local List1 = BoxWnd1:GetControl("LST_TEAM_SLOT_LIST")
	local List2 = BoxWnd2:GetControl("LST_TEAM_SLOT_LIST")
	
	if false == List1:IsNil() then
		local Item1 = List1:ListFirstItem()
		while false == Item1:IsNil() do
			Item1:GetWnd():GetControl("SFRM_LISTOVER"):Visible(false)
			Item1 = List1:ListNextItem(Item1)
		end
	end
	if false == List2:IsNil() then
		local Item2 = List2:ListFirstItem()
		while false == Item2:IsNil() do
			Item2:GetWnd():GetControl("SFRM_LISTOVER"):Visible(false)
			Item2 = List2:ListNextItem(Item2)
		end
	end
end

function OnSelectExpeditionTeam(Wnd, Val)
	g_EpCurrSelectTeam = Val

	if nil == Wnd then return end
	if Wnd:IsNil() then return end

	for i = 1, 4, 1 do
		Wnd:GetControl("CBTN_EXPEDITION_TEAM"..i):CheckState(false)
	end
	
	Wnd:GetControl("CBTN_EXPEDITION_TEAM"..g_EpCurrSelectTeam):CheckState(true)

	UpdateExpeditionMemberInfo(Wnd:GetControl("SFRM_COLOR_BOX2"), Val)
	UpdateExpeditionMemberInfo(Wnd:GetControl("SFRM_COLOR_BOX3"), 5)
	
	if GetAccumTime() > g_fExpeditionKeyItemRefreshTime then
		Send_C_M_REQ_REFRESH_NEED_ITEM_INFO()
		g_fExpeditionKeyItemRefreshTime = GetAccumTime() + 1.3
	end
end

function ExpeditionTeamButtonState(uiSelf, Val)
	local MaxTeam = GetMaxTeamNumber()
	if MaxTeam >= Val then
		uiSelf:Disable(false)
	else
		uiSelf:Disable(true)
	end
end

function ExpeditionRegisterButtonState()
	local ExpeditionNpcGuid = GetExpeditionNpcGuid()
	if nil == ExpeditionNpcGuid then return end
	if CheckExpeditionDailyQuest() then
		UISelf:SetStaticTextW(GetTT(710004))
		UISelf:Disable(false)
	else
		UISelf:SetStaticTextW(GetTT(710003))
		UISelf:Disable(true)
	end
end

function ShowExpeditionInfo()
	local Wnd = GetUIWnd("FRM_EXPEDITION_REGISTER")
	if Wnd:IsNil() then return end
	local HeadWnd = Wnd:GetControl("FRM_HEAD")
	if HeadWnd:IsNil() then return end
	--NAME
	local uiSelf = HeadWnd:GetControl("SFRM_LINE_BOX")
	if uiSelf:IsNil() then return end
	ShowExpeditionName(uiSelf)
	--LOCATION
	uiSelf = HeadWnd:GetControl("SFRM_AU_BOX")
	ShowExpeditionArea(uiSelf)
	--MEMBER NUMBER
	uiSelf = HeadWnd:GetControl("SFRM_AU_BOX2")
	ShowExpeditionCurrMemNum(uiSelf)
end

function UpdateExpeditionInfo()
	local Wnd = GetUIWnd("FRM_EXPEDITION_REGISTER")
	if Wnd:IsNil() then return end
	local BodyWnd = Wnd:GetControl("FRM_BODY")
	if BodyWnd:IsNil() then return end
	-- 등록창의 원정대 리스트 갱신(현재 선택된 팀과 대기팀)
	UpdateExpeditionMemberInfo(BodyWnd:GetControl("SFRM_COLOR_BOX2"), g_EpCurrSelectTeam)
	UpdateExpeditionMemberInfo(BodyWnd:GetControl("SFRM_COLOR_BOX3"), 5)
end

function UpdateExpeditionMyTeamInfo()
	local Wnd = GetUIWnd("FRM_EXPEDITION_REGISTER")
	if Wnd:IsNil() then return end
	local BodyWnd = Wnd:GetControl("FRM_BODY")
	if BodyWnd:IsNil() then return end
	-- 등록창의 원정대 리스트 갱신(내 팀과 대기팀)
	local MyTeam = GetMemberTeamNumber(g_pilotMan:GetPlayerPilotGuid())
	if MyTeam == 5 then	-- 내 팀이 교체대기 팀이면
		OnSelectExpeditionTeam(BodyWnd, g_EpCurrSelectTeam)
	else
		g_EpCurrSelectTeam = MyTeam
		OnSelectExpeditionTeam(BodyWnd, g_EpCurrSelectTeam)
	end
end

function DestroyExpeditionMsgBox()
	if IsExpeditionMaster(g_pilotMan:GetPlayerPilotGuid()) then
		CallYesNoMsgBox(GetTT(720012), GUID(), MBT_EXPEDITION_DESTROY)
	else
		CallYesNoMsgBox(GetTT(720013), GUID(), MBT_EXPEDITION_LEAVE)
	end
end

function ReqMoveTeamExpedition(Guid, Val)
	if IsExpeditionMaster(g_pilotMan:GetPlayerPilotGuid()) then
		Send_C_N_REQ_TEAM_MOVE_EXPEDITION(Guid, Val)
	end
end

function ReqChangeMasterExpedition(Guid)
	if IsExpeditionMaster(g_pilotMan:GetPlayerPilotGuid()) then
		Send_C_N_REQ_CHANGEMASTER_EXPEDITION(Guid)
	end
end

function ReqKickoutExpedition(OwnerGuid)
	if IsExpeditionMaster(g_pilotMan:GetPlayerPilotGuid()) then
		CallYesNoMsgBox(GetTT(720042), OwnerGuid, MBT_EXPEDITION_KICK_MEMBER)
	end
end

function MinimizeRegisterWindow(WindowState)
	if WindowState <= 0 or WindowState >= 4 then return end

	local TopWindow = GetUIWnd("FRM_EXPEDITION_REGISTER")
	if TopWindow:IsNil() then return end
	local BackGroundWnd = TopWindow:GetControl("SFRM_BLACK_BG")
	if BackGroundWnd:IsNil() then return end
	local TopWnd = TopWindow:GetControl("FRM_TOP")
	if TopWnd:IsNil() then return end
	local HeadWnd = TopWindow:GetControl("FRM_HEAD")
	if HeadWnd:IsNil() then return end
	local BodyWnd = TopWindow:GetControl("FRM_BODY")
	if BodyWnd:IsNil() then return end
	local TailWnd = TopWindow:GetControl("FRM_TAIL")
	if TailWnd:IsNil() then return end
	if 1 == WindowState then
		HeadWnd:Visible(true)
		BodyWnd:Visible(true)
		TailWnd:Visible(true)
		TopWindow:SetSize(Point2(TopWindow:GetSize():GetX(), TopWnd:GetSize():GetY() + HeadWnd:GetSize():GetY() + BodyWnd:GetSize():GetY() + TailWnd:GetSize():GetY()))
		BackGroundWnd:SetSize(Point2(BackGroundWnd:GetSize():GetX(), TopWnd:GetSize():GetY() + HeadWnd:GetSize():GetY() + BodyWnd:GetSize():GetY() + TailWnd:GetSize():GetY()))
		TailWnd:SetLocation(Point2(TailWnd:GetLocation():GetX(), BodyWnd:GetLocation():GetY() + BodyWnd:GetSize():GetY()))
		if GetScreenSize():GetY() <= TopWindow:GetSize():GetY() + TopWindow:GetLocation():GetY() then
			TopWindow:SetLocation(Point2(TopWindow:GetLocation():GetX(), GetScreenSize():GetY() - TopWindow:GetSize():GetY()))
		end
	elseif 2 == WindowState then
		HeadWnd:Visible(true)
		BodyWnd:Visible(false)
		TailWnd:Visible(true)
		TopWindow:SetSize(Point2(TopWindow:GetSize():GetX(), TopWnd:GetSize():GetY() + HeadWnd:GetSize():GetY() + TailWnd:GetSize():GetY()))
		BackGroundWnd:SetSize(Point2(BackGroundWnd:GetSize():GetX(), TopWnd:GetSize():GetY() + HeadWnd:GetSize():GetY() + TailWnd:GetSize():GetY()))
		TailWnd:SetLocation(Point2(TailWnd:GetLocation():GetX(), BodyWnd:GetLocation():GetY()))
	elseif 3 == WindowState then
		HeadWnd:Visible(false)
		BodyWnd:Visible(false)
		TailWnd:Visible(true)
		TopWindow:SetSize(Point2(TopWindow:GetSize():GetX(), TopWnd:GetSize():GetY() + TailWnd:GetSize():GetY()))
		BackGroundWnd:SetSize(Point2(BackGroundWnd:GetSize():GetX(), TopWnd:GetSize():GetY() + TailWnd:GetSize():GetY()))
		TailWnd:SetLocation(Point2(TailWnd:GetLocation():GetX(), HeadWnd:GetLocation():GetY()))
	end
end
--/////////////////////////////////////////////////////////
-- END (EXPEDITION REGISTER)
--/////////////////////////////////////////////////////////

--/////////////////////////////////////////////////////////
-- START (EXPEDITION FIND WINDOW)
--/////////////////////////////////////////////////////////
function OnCall_FindWindow(Wnd, Val)
	g_EpFindWindow = Val
	
	if g_EpFindWindow == 1 then
		OnCall_PartyFind(Wnd)
		OffCall_ExpeditionFind(Wnd)
	elseif( g_EpFindWindow == 2 ) then
		OnCall_ExpeditionFind(Wnd)
		OffCall_PartyFind(Wnd)
	end
end

function OnCall_PartyFind(Wnd)
	Wnd:GetControl("CBTN_CONTINENT"):Visible(true)
	Wnd:GetControl("CBTN_AREA"):Visible(true)
	Wnd:GetControl("BTN_OK"):Visible(true)
	Wnd:GetControl("BTN_CANCEL"):Visible(true)
	Wnd:GetControl("BTN_PREV_PAGE"):Visible(true)
	Wnd:GetControl("BTN_NEXT_PAGE"):Visible(true)
	Wnd:GetControl("FRM_PAGE"):Visible(true)
	Wnd:GetControl("FRM_ITEM"):Visible(true)

	-- CHECKSTATE
	Wnd:GetControl("CBTN_ITEM"):CheckState(true)
end

function OnCall_ExpeditionFind(Wnd)
	Wnd:GetControl("BTN_OK2"):Visible(true)
	Wnd:GetControl("BTN_CANCEL2"):Visible(true)
	Wnd:GetControl("BTN_PREV_PAGE2"):Visible(true)
	Wnd:GetControl("BTN_NEXT_PAGE2"):Visible(true)
	Wnd:GetControl("FRM_PAGE2"):Visible(true)
	Wnd:GetControl("FRM_EX_ITEM"):Visible(true)
	
	-- CHECKSTATE
	Wnd:GetControl("CBTN_EX_ITEM"):CheckState(true)
end

function OffCall_PartyFind(Wnd)
	Wnd:GetControl("CBTN_CONTINENT"):Visible(false)
	Wnd:GetControl("CBTN_AREA"):Visible(false)
	Wnd:GetControl("BTN_OK"):Visible(false)
	Wnd:GetControl("BTN_CANCEL"):Visible(false)
	Wnd:GetControl("BTN_PREV_PAGE"):Visible(false)
	Wnd:GetControl("BTN_NEXT_PAGE"):Visible(false)
	Wnd:GetControl("FRM_PAGE"):Visible(false)
	Wnd:GetControl("FRM_ITEM"):Visible(false)

	-- CHECKSTATE
	Wnd:GetControl("CBTN_ITEM"):CheckState(false)
end

function OffCall_ExpeditionFind(Wnd)
	Wnd:GetControl("BTN_OK2"):Visible(false)
	Wnd:GetControl("BTN_CANCEL2"):Visible(false)
	Wnd:GetControl("BTN_PREV_PAGE2"):Visible(false)
	Wnd:GetControl("BTN_NEXT_PAGE2"):Visible(false)
	Wnd:GetControl("FRM_PAGE2"):Visible(false)
	Wnd:GetControl("FRM_EX_ITEM"):Visible(false)
	
	-- CHECKSTATE
	Wnd:GetControl("CBTN_EX_ITEM"):CheckState(false)
end

function OnExpeditionFind_Continent(Val)
	g_EpContinent = Val
	
	local iTTW = 0
	if 0 == g_EpContinent then			--	전체
		iTTW = 30000
	elseif 1 == g_EpContinent then		--  바람대륙
		iTTW = 700
	elseif 2 == g_EpContinent then		--  혼돈대륙
		iTTW = 701
	end

	GetUIWnd("SFRM_PARTY_PART_FIND"):GetControl("FRM_EX_ITEM"):GetControl("SFRM_BG_INPUT_CONTINENT"):SetStaticText(GetTT(iTTW):GetStr())
end

function LuaCall_ExpeditionUserList()
	local Wnd = GetUIWnd("FRM_EXPEDITION_INVITATION")
	if Wnd:IsNil() then return end
	UpdateExpeditionUserList(Wnd)
end
--/////////////////////////////////////////////////////////
-- END (EXPEDITION FIND WINDOW)
--/////////////////////////////////////////////////////////

--/////////////////////////////////////////////////////////
-- START (EXPEDITION MAIN BAR)
--/////////////////////////////////////////////////////////
function OnCall_ExpeditionMainBar()
	local MaxTeam = GetMaxTeamNumber()
	if MaxTeam >= 1 then
		local MemberNum = GetExpeditionTeamMemberNum(1)
		if MemberNum == 0 then
			CloseUI("FRM_EXPEDITION_MAIN_ITEM_HEAD")
		else
			ActivateUI("FRM_EXPEDITION_MAIN_ITEM_HEAD")
		end
	end
	if MaxTeam >= 4 then
		local MemberNum = GetExpeditionTeamMemberNum(4)
		if MemberNum == 0 then
			CloseUI("FRM_EXPEDITION_MAIN_ITEM_SUB3")
		else
			ActivateUI("FRM_EXPEDITION_MAIN_ITEM_SUB3")
		end
	end
	if MaxTeam >= 3 then
		local MemberNum = GetExpeditionTeamMemberNum(3)
		if MemberNum == 0 then
			CloseUI("FRM_EXPEDITION_MAIN_ITEM_SUB2")
		else
			ActivateUI("FRM_EXPEDITION_MAIN_ITEM_SUB2")
		end
	end
	if MaxTeam >= 2 then
		local MemberNum = GetExpeditionTeamMemberNum(2)
		if MemberNum == 0 then
			CloseUI("FRM_EXPEDITION_MAIN_ITEM_SUB1")
		else
			ActivateUI("FRM_EXPEDITION_MAIN_ITEM_SUB1")
		end
	end
	ExpeditionMainReLocation()
end

function UpdateExpeditionMainbar()
	-- 미니 정보창의 팀 총인원 수 갱신
	local Val = 1
	local Wnd1 = GetUIWnd("FRM_EXPEDITION_MAIN_ITEM_HEAD")
	if Wnd1:IsNil() then return end
	
	local SubWnd1 = Wnd1:GetControl("FRM_MAIN_TEAM_NUMBER")
	if SubWnd1:IsNil() then return end
	
	local MemberNum = GetExpeditionTeamMemberNum(Val)
	UpdateExpeditionMainBarInfo(Wnd1, Val)
	SubWnd1:SetStaticText(Val..GetTT(400105):GetStr().."("..MemberNum..")")
	
	local MaxTeam = GetMaxTeamNumber()
	for i = 2, MaxTeam, 1 do
		local Wnd2 = GetUIWnd("FRM_EXPEDITION_MAIN_ITEM_SUB"..(i - 1))
		if Wnd2:IsNil() then return end

		local SubWnd2 = Wnd2:GetControl("FRM_MAIN_TEAM_NUMBER")
		if SubWnd2:IsNil() then return end
		
		local MemberNum2 = GetExpeditionTeamMemberNum(i)
		UpdateExpeditionMainBarInfo(Wnd2, i)
		SubWnd2:SetStaticText(i..GetTT(400105):GetStr().."("..MemberNum2..")")
	end
end

function OnSelectExpeditionMainArwBtn(Wnd, Val)
	if nil == Wnd then return end
	if Wnd:IsNil() then return end
	
	local Self1 = Wnd:GetControl("BTN_MAIN_ARW_UP")
	local Self2 = Wnd:GetControl("BTN_MAIN_ARW_DOWN")
	
	if Val == 1 then
		Self1:Visible(false)
		Self2:Visible(true)
	else
		Self2:Visible(false)
		Self1:Visible(true)
	end
end

function ExpeditionMainReLocation()
	local Parent = GetUIWnd("FRM_EXPEDITION_MAIN_ITEM_HEAD")
	if not Parent:IsNil() then
		Parent:ReLocationSnapChild()
	end
end

function ExpeditionMainBarClear()
	local Wnd1 = GetUIWnd("FRM_EXPEDITION_MAIN_ITEM_HEAD"):GetControl("LST_MAIN_TEAM_LIST")
	Wnd1:ClearAllListItem()
	Wnd1:CloseParent()
	local Wnd2 = GetUIWnd("FRM_EXPEDITION_MAIN_ITEM_SUB1"):GetControl("LST_MAIN_TEAM_LIST")
	Wnd2:ClearAllListItem()
	Wnd2:CloseParent()
	local Wnd3 = GetUIWnd("FRM_EXPEDITION_MAIN_ITEM_SUB2"):GetControl("LST_MAIN_TEAM_LIST")
	Wnd3:ClearAllListItem()
	Wnd3:CloseParent()
	local Wnd4 = GetUIWnd("FRM_EXPEDITION_MAIN_ITEM_SUB3"):GetControl("LST_MAIN_TEAM_LIST")
	Wnd4:ClearAllListItem()
	Wnd4:CloseParent()
end
--/////////////////////////////////////////////////////////
-- END (EXPEDITION MAIN BAR)
--/////////////////////////////////////////////////////////

--/////////////////////////////////////////////////////////
-- Community_Expedition_Popup & ect
--/////////////////////////////////////////////////////////
function Community_Expedition_Popup(uiSelf)
	if uiSelf:IsNil() then ODS("Expedition Popup uiSelf is nil\n") return end
	if not IsInExpedition() then return end
	
	local Guid = uiSelf:GetOwnerGuid()
	
	if IsInExpeditionMemberGuid(Guid) == false then
		return
	end
	
	local MyGuid = g_pilotMan:GetPlayerPilotGuid()	-- 내 GUID
	local bMine = MyGuid:IsEqual(Guid)				-- 내 GUID와 미니파티창 오너의 GUID와 같으면 참(내 미니창 선택)
	local bIamMaster = IsExpeditionMaster(MyGuid)	-- 내가 원정대장이면 참
	local bMaster = IsExpeditionMaster(Guid)		-- 미니파티창의 오너가 대장이면 참
	
	local ExpeditionCmdArray = {}
	if bMine then	-- 내 창을 선택
		ExpeditionCmdArray[1] = "EXPEDITION_LEAVE"
		if bIamMaster then	-- 내가 마스터
			ExpeditionCmdArray[2] = "EXPEDITION_OPTION"
		end
	else			-- 다른 파티원의 창을 선택
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		if bIamMaster then	-- 내가 원정대장이면
			if g_world:IsHaveWorldAttr(GATTR_EXPEDITION_GROUND) then 
				ExpeditionCmdArray[1] = "EXPEDITION_KICK"
			end
			ExpeditionCmdArray[2] = "EXPEDITION_GIVE_OWN"
		end
		local MyActor = GetMyActor()
		if not MyActor:IsNowFollowing() then
			local PartyActor = g_world:FindActor(Guid)
			if nil ~= PartyActor and not PartyActor:IsNil() and GetMyActor():CheckCanFollow(PartyActor:GetPilotGuid(),false) then
				ExpeditionCmdArray[3] = "PARTY_FOLLOW"
			end
		else
			ExpeditionCmdArray[3] = "CANCEL_FOLLOW"
		end	
		ExpeditionCmdArray[4] = "ADD_FRIEND";
		ExpeditionCmdArray[5]  = "WHISPER"
		if not g_pilotMan:FindPilot(Guid):IsNil() then
			ExpeditionCmdArray[6] = "PARTY_SEE_INFO"
			ExpeditionCmdArray[7] = "PARTY_TRADE"
		end
		ExpeditionCmdArray[8] = "PARTY_NAME_COPY_TO_CLIPBOARD"
	end

	local CursorPos = GetCursorPos()
	CallCommonPopup(ExpeditionCmdArray, Guid, nil, CursorPos)
	
	CloseToolTip(nil, 11)
end

function Register_Expedition_Popup(uiSelf)
	if uiSelf:IsNil() then return end
	if not IsInExpedition() then return end
	
	local Guid = uiSelf:GetOwnerGuid()
	if IsInExpeditionMemberGuid(Guid) == false then
		return
	end
	
	local MyGuid = g_pilotMan:GetPlayerPilotGuid()	-- 내 GUID
	local bMine = MyGuid:IsEqual(Guid)				-- 내 GUID와 미니파티창 오너의 GUID와 같으면 참(내 미니창 선택)
	local bIamMaster = IsExpeditionMaster(MyGuid)	-- 내가 원정대장이면 참
	local bMaster = IsExpeditionMaster(Guid)		-- 미니파티창의 오너가 대장이면 참
	
	if bIamMaster == false then return end			-- 원정대장만 팝업창 뜸
	
	local ExpeditionCmdArray = {}
	
	if bMine then
	else
		ExpeditionCmdArray[1] = "EXPEDITION_KICK_2"
	end
	
	local TeamNum = GetMemberTeamNumber(Guid);
	local MaxTeam = GetMaxTeamNumber()

	if TeamNum ~= 5 then
		ExpeditionCmdArray[2] = "EXPEDITION_MOVE_SHIFT"
	end
	if MaxTeam >= 1 then
		if TeamNum ~= 1 then
			ExpeditionCmdArray[3] = "EXPEDITION_MOVE_TEAM1"
		end
	end
	if MaxTeam >= 2 then
		if TeamNum ~= 2 then
			ExpeditionCmdArray[4] = "EXPEDITION_MOVE_TEAM2"
		end
	end
	if MaxTeam >= 3 then
		if TeamNum ~= 3 then
			ExpeditionCmdArray[5] = "EXPEDITION_MOVE_TEAM3"
		end
	end
	if MaxTeam >= 4 then
		if TeamNum ~= 4 then
			ExpeditionCmdArray[6] = "EXPEDITION_MOVE_TEAM4"
		end
	end

	local CursorPos = GetCursorPos()
	CallCommonPopup(ExpeditionCmdArray, Guid, nil, CursorPos)
	
	CloseToolTip(nil, 11)
end

function ExpeditionStateCloseBtnOver(Wnd)
	if Wnd:IsNil() then return end

	local Owner = Wnd:GetOwnerGuid()
	local MyGuid = g_pilotMan:GetPlayerPilotGuid()
	local bMine = MyGuid:IsEqual(Owner)
	local bIamMaster = IsExpeditionMaster(MyGuid)

	if bIamMaster then
		if bMine then
			CloseToolTip()
			CallMutableToolTipText(GetTT(710054), GetUIWnd("Cursor"):GetLocation())
		else
			CloseToolTip()
			CallMutableToolTipText(GetTT(710053), GetUIWnd("Cursor"):GetLocation())
		end
	else
		CloseToolTip()
		CallMutableToolTipText(GetTT(710054), GetUIWnd("Cursor"):GetLocation())
	end
	
end

function ExpeditionStateCloseBtnDown(Wnd)
	if Wnd:IsNil() then return end
	
	local OwnerGuid = Wnd:GetOwnerGuid()
	local MyGuid = g_pilotMan:GetPlayerPilotGuid()
	local bMine = MyGuid:IsEqual(OwnerGuid)
	local bIamMaster = IsExpeditionMaster(MyGuid)

	if bIamMaster then
		if bMine then
			CallYesNoMsgBox(GetTT(720013), MyGuid, MBT_EXPEDITION_LEAVE)
		else
			CallYesNoMsgBox(GetTT(720042), OwnerGuid, MBT_EXPEDITION_KICK_MEMBER)
		end
	else
		CallYesNoMsgBox(GetTT(720013), MyGuid, MBT_EXPEDITION_LEAVE)
	end
end

function AllCloseExpeditionWindow()
	GetUIWnd("FRM_EXPEDITION_OPTION"):Close()
	GetUIWnd("FRM_EXPEDITION_INVITATION"):Close()
	GetUIWnd("FRM_EXPEDITION_REGISTER"):Close()
	GetUIWnd("FRM_EXPEDITION_MAIN_ITEM_HEAD"):Close()
	GetUIWnd("FRM_EXPEDITION_MAIN_ITEM_SUB1"):Close()
	GetUIWnd("FRM_EXPEDITION_MAIN_ITEM_SUB2"):Close()
	GetUIWnd("FRM_EXPEDITION_MAIN_ITEM_SUB3"):Close()
end

function OnSelectExpeditionTeam2(Wnd, Val)
	g_EpCurrSelectTeam = Val

	if nil == Wnd then return end
	if Wnd:IsNil() then return end

	for i = 1, 4, 1 do
		Wnd:GetControl("CHK_TEAM"..i):CheckState(false)
	end
	
	Wnd:GetControl("CHK_TEAM"..g_EpCurrSelectTeam):CheckState(true)
end

function CallExpeditionItemLocation(CharGuid)
	if nil == CharGuid or CharGuid:IsNil() then return end
	local iGndNo = GetPartyMemberLocation(CharGuid)
	if 0 ~= iGndNo then
		local LocationStr = GetAppendTextW(GetTT(400270), GetMapNameW(iGndNo))
		if IsPartyMaster(CharGuid) then
			LocationStr = GetAppendTextW(LocationStr, GetTT(14))
			local Temp = WideString("[ ")
			LocationStr = GetAppendTextW(LocationStr, Temp)
			LocationStr = GetAppendTextW(LocationStr, GetExpeditionOptionWStr())
			local Temp = WideString(" ]")
			LocationStr = GetAppendTextW(LocationStr, Temp)
		end
		CallToolTipText(0, LocationStr, GetCursorPos(), "ToolTip2", 11)
	else
		CloseToolTip(nil, 11)
	end
end

function CallExpeditionJoinPopupMenu(Wnd)
	local OwnerGuid = Wnd:GetOwnerGuid()
	if nil == OwnerGuid then return end
	local PopupItem = {}
	PopupItem[1] = "EXPEDITION_JOIN"
	
	local CursorPos = GetCursorPos()
	CursorPos:SetX( CursorPos:GetX() )
	
	CallCommonPopup(PopupItem, OwnerGuid, nil, CursorPos)
end

function CallExpeditionCloseBtnOver(Self)
	if nil==Self or Self:IsNil() then return end

	if not IsInExpedition() then return end

	local Owner = Self:GetOwnerGuid()
	if false==IsInExpeditionMemberGuid(Owner) then return end

	local MyGuid = g_pilotMan:GetPlayerPilotGuid()
	local bMine = MyGuid:IsEqual(Owner)
	local bIamMaster = IsPartyMaster(MyGuid)

	if not bMine then
		CallExpeditionItemLocation(Owner)
	end

	if bMine or bIamMaster then
		Self:GetControl("BTN_CLOSE"):Visible(true)
	end
end

function CallExpeditionCloseBtnOut(Self)
	CloseToolTip()

	if nil==Self or Self:IsNil() then return end

	if not IsInExpedition() then return end

	local Owner = Self:GetOwnerGuid()
	if false==IsInExpeditionMemberGuid(Owner) then return end

	local MyGuid = g_pilotMan:GetPlayerPilotGuid()
	local bMine = MyGuid:IsEqual(Owner)

	if not bMine then
		Self:GetControl("BTN_CLOSE"):Visible(false)
	end
end

function ReqUnRegistPrivate()
	if true == g_bIsPraiveRegist then
		g_bIsPraiveRegist = false
		SendUnRegistPrivate()
	end
end

g_BoolRecvPartyList = false
g_fRecvPartyListRefreshTime = 0
function StartRecvPartyList(bRecv)
	g_BoolRecvPartyList	= bRecv
	g_fRecvPartyListRefreshTime = GetAccumTime() + 3.0
	
	local TopWnd = GetUIWnd("SFRM_PARTY_PART_FIND")
	if true == TopWnd:IsNil() then return end
	local SubWnd = TopWnd:GetControl("FRM_ITEM")
	if true == SubWnd:IsNil() then return end
	local LoadWnd = SubWnd:GetControl("FRM_LOADING")
	if true == LoadWnd:IsNil() then return end 
	
	if true == bRecv then
		LoadWnd:Visible(false)
	else
		LoadWnd:Visible(true)
	end
end
