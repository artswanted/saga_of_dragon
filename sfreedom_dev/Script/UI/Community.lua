--g_FriendMgr

--================================
g_kCommunity = {}
g_kCommunity["FriendList"] = {}--lwFriendItem array
g_kCommunity["GroupList"] = {}--lwWString array
g_kCommunity.GroupCount = 0

g_kCommunity["SelectItemText"] = nil--use Popup menu
g_kCommunity.iGroupShowMode = 0
g_kCommunity.iTempGroupShowMode = 0

g_kCommunity["ClickedItemWnd"] = nil

g_kCommunity["PartyParam"] = {}

g_kCommunity["ShowToolTip"] = false

g_kCommunity["FriendSortType"] = 0
g_kCommunity["FriendSortTypeDESC"] = false
--================================
eCommunity = {}
eCommunity.EMCT_AddAccept = 10
eCommunity.EMCT_AddReject = 11

function SetFriendMemberSortType(iType)
	if iType == g_kCommunity["FriendSortType"] then
		g_kCommunity["FriendSortTypeDESC"] = not g_kCommunity["FriendSortTypeDESC"]
	else
		g_kCommunity["FriendSortType"] = iType
		g_kCommunity["FriendSortTypeDESC"] = false
	end
end
--////////////////////////////////////// Update_FriendList ////////////////////////////
function SortFriendMember_ClassASC(lhs, rhs)
	if nil==lhs or lhs:IsNil() then return false end
	if nil==rhs or rhs:IsNil() then return false end
	if lhs:Class() > rhs:Class() then		return false		end
	if lhs:Class() < rhs:Class() then 		return true		 	end
	return false
end

function SortFriendMember_ClassDESC(lhs, rhs)
	if nil==lhs or lhs:IsNil() then return false end
	if nil==rhs or rhs:IsNil() then return false end
	if lhs:Class() > rhs:Class() then		return true			end
	if lhs:Class() < rhs:Class() then 		return false		end
	return false
end

function SortFriendMember_LevelASC(lhs, rhs)
	if nil==lhs or lhs:IsNil() then return false end
	if nil==rhs or rhs:IsNil() then return false end
	if lhs:Level() > rhs:Level() then		return false		end
	if lhs:Level() < rhs:Level() then 		return true		 	end
	return false
end

function SortFriendMember_LevelDESC(lhs, rhs)
	if nil==lhs or lhs:IsNil() then return false end
	if nil==rhs or rhs:IsNil() then return false end
	if lhs:Level() > rhs:Level() then		return true			end
	if lhs:Level() < rhs:Level() then 		return false		end
	return false
end

function SortFriendMember_NameASC(lhs, rhs)
	if nil==lhs or lhs:IsNil() then return false end
	if nil==rhs or rhs:IsNil() then return false end
	local left  = lhs:CharacterName():GetStr()
	local right = rhs:CharacterName():GetStr()
	if string.lower(left) > string.lower(right) then 		return false		end
	if string.lower(left) < string.lower(right) then 		return true			end
	return false
end

function SortFriendMember_NameDESC(lhs, rhs)
	if nil==lhs or lhs:IsNil() then return false end
	if nil==rhs or rhs:IsNil() then return false end
	local left  = lhs:CharacterName():GetStr()
	local right = rhs:CharacterName():GetStr()
	if string.lower(left) > string.lower(right) then 		return true			end
	if string.lower(left) < string.lower(right) then 		return false		end
	return false
end

function SortFriendMember_LocationASC(lhs, rhs)
	if nil==lhs or lhs:IsNil() then return false end
	if nil==rhs or rhs:IsNil() then return false end
	local left  = GetMapNameW(lhs:Location()):GetStr()
	local right = GetMapNameW(rhs:Location()):GetStr()
	if string.lower(left) > string.lower(right) then 		return false		end
	if string.lower(left) < string.lower(right) then 		return true			end
	return false
end

function SortFriendMember_LocationDESC(lhs, rhs)
	if nil==lhs or lhs:IsNil() then return false end
	if nil==rhs or rhs:IsNil() then return false end
	local left  = GetMapNameW(lhs:Location()):GetStr()
	local right = GetMapNameW(rhs:Location()):GetStr()
	if string.lower(left) > string.lower(right) then 		return true			end
	if string.lower(left) < string.lower(right) then 		return false		end
	return false
end

function Update_FriendList(lwui)
	local iFriendCnt = g_FriendMgr:Friend_Count()
	ODS("Update Friend list count "..iFriendCnt.."\n")
	
	g_kCommunity["FriendList"] = {}
	for iVal=1, iFriendCnt do
		local iArrayLoc = iVal - 1
		
		local kFriendItem = g_FriendMgr:FriendItem_ByAt(iArrayLoc)
		if false == kFriendItem:IsNil() then
			ODS("닐체크 통과 mode:"..g_kCommunity.iGroupShowMode.."\n")
			if(1 == g_kCommunity.iGroupShowMode) then	--show on line friend
				if (true == kFriendItem:IsOnline()) then
					g_kCommunity["FriendList"][iVal] = kFriendItem
					ODS("온라인만보기아이템추가\n")
				end
			else										--show on, off line friend
				g_kCommunity["FriendList"][iVal] = kFriendItem
				ODS("전체보기아이템추가\n")
			end
			
		end	
		ODS("for 루프 끝\n")
		--g_kCommunity["FriendList"][iVal] = g_FriendMgr:FriendItem_ByAt(iArrayLoc)
	end
	
	ODS("Update Friend group list\n")
	g_kCommunity["GroupList"] = {}
	g_kCommunity.GroupCount = 0
	for kKey, kVal in pairs(g_kCommunity["FriendList"]) do
		--검색
		local bFind = false
		for kKey2, kVal2 in pairs(g_kCommunity["GroupList"]) do
			ODS("같은지 검사 : " .. kVal:GroupName():GetStr() .. " " .. kVal2:GetStr())
			if true == kVal:GroupName():IsEqual(kVal2) then
				ODS(" 같다")
				bFind = true
			end
			ODS("\n")
		end
		if false == bFind then--없으면 추가
			g_kCommunity.GroupCount = g_kCommunity.GroupCount + 1
			g_kCommunity["GroupList"][g_kCommunity.GroupCount] = kVal:GroupName()
			ODS("없으면 추가 : " .. kVal:GroupName():GetStr() .. "\n")
		end
	end
	 
	g_kCommunity.GroupCount = g_kCommunity.GroupCount + 1
	g_kCommunity["GroupList"][g_kCommunity.GroupCount] = GetTextW(400502)
	
--	if true == OnDisplayPvP_Community(1) then
--		return
--	end
	---------------------------------------------
	local kTop = lwui--GetUIWnd("SFRM_COMMUNITY")
	if kTop == nil or kTop:IsNil() == true then 
      ODS("[lwui] kTop is nil\n") 
      kTop = GetUIWnd("SFRM_COMMUNITY")
      if kTop:IsNil() == true then
         ODS("[FriendList] kTop is nil\n") 
         return 
      end
   end
	
	local kFriendFrm = kTop:GetControl("FRM_COMMUNITY")
	if kFriendFrm:IsNil() then ODS("[FriendList] FriendForm is nil\n") return end
	
	local kFriendList = kFriendFrm:GetControl("LST_FRIEND_LIST")
	if kFriendList:IsNil() then ODS("[FriendList] FriendList is nil\n") return end
	
	kFriendList:ClearAllListItem()--clear list
	
	--Sort
	if 1 == g_kCommunity["FriendSortType"] then
		if true == g_kCommunity["FriendSortTypeDESC"] then
			table.sort(g_kCommunity["FriendList"],SortFriendMember_ClassDESC)
			ODS("SortFriendMember_ClassDESC\n")
		else
			table.sort(g_kCommunity["FriendList"],SortFriendMember_ClassASC)
			ODS("SortFriendMember_ClassASC\n")
		end
	elseif 2 == g_kCommunity["FriendSortType"] then
		if true == g_kCommunity["FriendSortTypeDESC"] then
			table.sort(g_kCommunity["FriendList"],SortFriendMember_LevelDESC)
			ODS("SortFriendMember_LevelDESC\n")
		else
			table.sort(g_kCommunity["FriendList"],SortFriendMember_LevelASC)
			ODS("SortFriendMember_LevelASC\n")
		end
	elseif 3 == g_kCommunity["FriendSortType"] then
		if true == g_kCommunity["FriendSortTypeDESC"] then
			table.sort(g_kCommunity["FriendList"],SortFriendMember_NameDESC)
			ODS("SortFriendMember_NameDESC\n")
		else
			table.sort(g_kCommunity["FriendList"],SortFriendMember_NameASC)
			ODS("SortFriendMember_NameASC\n")
		end
	elseif 4 == g_kCommunity["FriendSortType"] then
		if true == g_kCommunity["FriendSortTypeDESC"] then
			table.sort(g_kCommunity["FriendList"],SortFriendMember_LocationDESC)
			ODS("SortFriendMember_LocationDESC\n")
		else
			table.sort(g_kCommunity["FriendList"],SortFriendMember_LocationASC)
			ODS("SortFriendMember_LocationASC\n")
		end
	end
	
	OnDisplay_Community_FriendList_ByGroup(kTop, kFriendFrm, kFriendList)
	--[[if g_kCommunity.iGroupShowMode == 1 then
		OnDisplay_Community_FriendList_ByGroup(kTop, kFriendFrm, kFriendList)
	else
		OnDisplay_Community_FriendList_ByOnOff(kTop, kFriendFrm, kFriendList)
	end--]]
end

--//////////////////////////////////////// OnDisplay_Community_FriendList_ByOnOff //////////////////////////
function OnDisplay_Community_FriendList_ByOnOff(kTop, kFriendFrm, kFriendList)
	local iCountLogin = g_FriendMgr:CountOfLogin(true)
	local iCountLogout = g_FriendMgr:CountOfLogin(false)
	
	--On line	400527
	local kGroup = GetTT(400527)
	local kGroupItem = kFriendList:AddNewListItem(kGroup):GetWnd()
	if kGroupItem:IsNil() then ODS("Group Item is nil\n") return end
	kGroupItem:ClearAllListItem()
	
	local kBgFrm = kGroupItem:GetControl("SFRM_BG")
	if kBgFrm:IsNil() then ODS("[FriendList] Bg is nil\n") return end
	local kResultName = WideString(kGroup:GetStr() .. " (" .. iCountLogin .. ")")
	kBgFrm:SetStaticTextW(kResultName)
	for kKey, kFriend in pairs(g_kCommunity["FriendList"]) do
		if kFriend:IsOnline() then
			Add_Friend_Community(kGroupItem, kFriend, kKey, true)
		end
	end
	
	
	--Off line	400528
	local kGroup = GetTT(400528)
	local kGroupItem = kFriendList:AddNewListItem(kGroup):GetWnd()
	if kGroupItem:IsNil() then ODS("Group Item is nil\n") return end
	
	local kBgFrm = kGroupItem:GetControl("SFRM_BG")
	if kBgFrm:IsNil() then ODS("[FriendList] Bg is nil\n") return end
	local kResultName = WideString(kGroup:GetStr() .. " (" .. iCountLogout .. ")")
	kBgFrm:SetStaticTextW(kResultName)
	for kKey, kFriend in pairs(g_kCommunity["FriendList"]) do
		if kFriend:IsOffline() then
			Add_Friend_Community(kGroupItem, kFriend, kKey, false)
		end
	end
end

--///////////////////////////////// OnDisplay_Community_FriendList_ByGroup /////////////////////////////////
function OnDisplay_Community_FriendList_ByGroup(kTop, kFriendFrm, kFriendList)
	
	for kKey, kFriend in pairs(g_kCommunity["FriendList"]) do
		--local kGroup = kFriend:GroupName()
		local kGroup = GetTT(0)
		
		if kGroup:IsNil() then
			--two ways (Accepter[] or Requester[])
			if kFriend:IsRequester() then
				kGroup = GetTT(0)--Wait for acept agree add friend
			--elseif kFriend:IsAccepter() then
				--kGroup = GetTT(400501)--wait for accept
			else
				kGroup = GetTT(0)--no group
			end
		end
		
		ODS("Group List : "..kGroup:GetStr().."\n")
		
		local kGroupItem = kFriendList:ListFindItemW(kGroup)
		if kGroupItem:IsNil() then
			ODS("Add FriendGroup : " .. kGroup:GetStr() .. "\n")
			local kListItem = kFriendList:AddNewListItem(kGroup)
			if kListItem:IsNil() then ODS("kFriendList:AddNewListItem(kGroup) failed\n") return end
			kGroupItem = kListItem:GetWnd()
			if kGroupItem:IsNil() then ODS("Group Item is nil\n") return end
			kGroupItem:ClearAllTreeItem()
			
			local kBgFrm = kGroupItem:GetControl("SFRM_BG")
			if kBgFrm:IsNil() then ODS("[FriendList] Bg is nil\n") return end
			--local kEditBg = kGroupItem:GetControl("FRM_EDIT_NAME")
			--if kEditBg:IsNil() then ODS("[FriendList] FRM_EDIT_NAME is nil\n ") return end
			--local kEditName = kEditBg:GetControl("EDT_GROUPNAME")
			--if kEditName:IsNil() then ODS("[FriendList] EDT_GROUPNAME") return end
			--kEditName:SetEditTextW(kGroup)
			
			local kTotal = g_FriendMgr:CountOfGroup(kFriend:GroupName(), false)
			local kLogin = g_FriendMgr:CountOfGroup(kFriend:GroupName(), true)
			
			local kResultName = WideString(kGroup:GetStr() .. " (" .. kLogin .. "/" .. kTotal .. ")")
			--kBgFrm:SetStaticTextW(kResultName)
			
			kBgFrm:SetCustomDataAsInt(kKey)
			
			--Edit Group Form
			--[[local kEditFrm = kGroupItem:GetControl("FRM_EDIT_NAME")
			if kEditFrm:IsNil() then ODS("[FriendList] FRM_EDIT_NAME is nil()\n") return end
			kEditFrm:SetCustomDataAsInt(kKey)--]]
			kGroupItem:SetInvalidate(true)
		else
			kGroupItem = kGroupItem:GetWnd()
		end
		
		Add_Friend_Community(kGroupItem, kFriend, kKey, true)
		
	end--for
	
end

--////////////////////////////////// Add_Friend_Community ////////////////////////////////
function Add_Friend_Community(kGroupItem, kFriend, kKey, bExpand)
	if kGroupItem:IsNil() then ODS("kGroupItem is nil\n") return end
	--ExpandTree
	--AddNewTreeItem
	local kFriendName = kFriend:CharacterName()
	if kFriendName:IsNil() then
		return
	end--if
	
	ODS("Add Friend Item: "..kFriendName:GetStr().."\n")
	local kFriendItem = kGroupItem:AddNewTreeItem(GetTT(0)):GetWnd()
	if kFriendItem:IsNil() then ODS("FriendItem is nil\n") return end
	kFriendItem:ClearAllTreeItem()
	
	ODS("kKey is : " .. kKey .. "\n")
	kFriendItem:SetCustomDataAsInt(kKey)
	
	if false == kFriend:SetMyHomeStatus(kFriendItem) then
		return
	end
	
	local kOffline = kFriend:IsOffline()
	
	local iClass = kFriend:Class()
	local kClassFrm = kFriendItem:GetControl("IMG_CLASS")
	if kClassFrm:IsNil() then ODS("Class is nil") return end
	if iClass ~= 0 and (false == kOffline) then
		SetMiniClassIconIndex(kClassFrm, iClass)
	else
		kClassFrm:Visible(false)				
	end
	
	local iLevel = kFriend:Level()
	local kLevelFrm = kFriendItem:GetControl("FRM_LEVEL")
	if kLevelFrm:IsNil() then ODS("Level is nil") return end
	if iLevel ~= 0 and (false == kOffline) then
		kLevelFrm:SetStaticText(iLevel)
	else
		kLevelFrm:Visible(false)				
	end

	local iCharacterName = kFriend:CharacterName()
	local kCharacterNameFrm = kFriendItem:GetControl("FRM_NAME")
	if kCharacterNameFrm:IsNil() then ODS("CharacterName is nil") return end
	if iCharacterName ~= 0 then	
		kCharacterNameFrm:SetStaticTextW(iCharacterName)
	else
		kCharacterNameFrm:Visible(false)
	end
	
	local iChannel = kFriend:Channel()
	local kChannelFrm = kFriendItem:GetControl("FRM_CHANNEL")
	if kChannelFrm:IsNil() then ODS("Channel is nil") return end
	if iChannel ~= 0 and (false == kOffline) then
		kChannelFrm:SetStaticText( "<".. string.format(GetTextW(401058):GetStr(), iChannel) ..">" )
	else
		kChannelFrm:Visible(false)
	end
	
	local iLocation = kFriend:Location()
	local kLocationFrm = kFriendItem:GetControl("FRM_LOCATION")
	kLocationFrm:ClearCustomData()
	if kLocationFrm:IsNil() then ODS("Location is nil") return end
	if true == kFriend:IsRequester() then
		kLocationFrm:Visible(false)
	elseif true == kFriend:IsAccepter() then
		local kWait = kFriendItem:GetControl("BTN_WAIT")
		if kWait:IsNil() then ODS("Wait is nil") return end
		kWait:Visible(true)
	elseif iLocation ~= 0 and (false == kOffline) then
		local CutStr = WideString("..."); 
		local LocationName
		if 9000000 > iLocation then
			LocationName = GetTextW(200158)
		else
			LocationName = GetMapNameW(iLocation)
		end
		kLocationFrm:SetCustomDataAsStr(LocationName:GetStr())
		SetCutedTextLimitLength(kLocationFrm,LocationName,CutStr,kLocationFrm:GetWidth())
	else
		local kResultText = WideString("<" .. GetTextW(401059):GetStr() .. ">")
		kLocationFrm:SetStaticTextW(kResultText)
	end	

	local iState = kFriend:IsLinkStatus()
	local kStateFrm = kFriendItem:GetControl("FRM_STATE")
	if kStateFrm:IsNil() then ODS("State is nil") return end
	if iState ~= 0 and (false == kOffline) then
		kStateFrm:SetStaticText("")
	end
	
--[[	--kFriend:IsLinkStatus
	local kStautsImg = kFriendItem:GetControl("IMG_STATUS")
	if kStautsImg:IsNil() then ODS("StatusImg is nil") return end
	local iLinkStatus = kFriend:IsLinkStatus()
	ODS("IsLinkStatus : " .. iLinkStatus .. "\n")
	if iLinkStatus ~= 0 then
		kStautsImg:SetUVIndex(kFriend:IsLinkStatus())
	else
		kStautsImg:SetUVIndex(4)
	end
--]]	
	--Status set
	if kFriend:IsRequester() then		
		kFriendItem:GetControl("BTN_ACCEPT"):Visible(true)
		kFriendItem:GetControl("BTN_REJECT"):Visible(true)
	elseif kFriend:IsOnline() then
		local wndListen = kFriendItem:GetControl("IMG_LISTEN")
		if wndListen:IsNil() == false then
			wndListen:Visible(true)
			wndListen:SetEnable(true)
			if kFriend:IsBlock_HeCanListen() == false or kFriend:IsBlock_HeCanTalk() == false then
				wndListen:SetUVIndex(1)
			else
				wndListen:SetUVIndex(2)
			end
			kFriendItem:GetControl("BTN_PARTY_INV"):Visible(true)
			kFriendItem:GetControl("BTN_PARTY_INV"):SetEnable(true)
			kFriendItem:GetControl("BTN_MAN_TO_MAN"):Visible(true)
			kFriendItem:GetControl("BTN_MAN_TO_MAN"):SetEnable(true)
		end

	end
	--kFriendItem:GetControl("FRM_NICKNAME"):SetStaticTextW(kFriendName)
	--kFriendItem:GetControl("IMG_TALK")
	--kFriendItem:GetControl("IMG_LISTEN")
	if bExpand then --Expand ?
		kGroupItem:CollapseTree()
		kGroupItem:ExpandTree()--Expanded
	end
end

--//////////////////////////////////////////////////////////////////


--g_FriendMgr
--/////////////////////////////// Community_Friend_AddFriend ///////////////////////////////////
function Community_Friend_AddFriend(kEdit)
	if nil == kEdit then
		ODS("[Friend_AddFriend]kEdit is nil\n")
		return
	end
	local kEditText = kEdit:GetEditText()
	if 0 ~= kEditText:Length() then
		g_FriendMgr:SendFriend_Command_AddByName(kEditText)
		g_FriendMgr:Friend_List_UpdateReq(true)
	end
end

function Community_Friend_AddFriendName(kEdit)
	if nil == kEdit then
		ODS("[Friend_AddFriend]kEdit is nil\n")
		return
	end
	local kEditText = kEdit
	if 0 ~= kEditText:Length() then
		g_FriendMgr:SendFriend_Command_AddByName(kEditText)
		g_FriendMgr:Friend_List_UpdateReq(true)
	end
end

function Community_Friend_AddGroup(kEdit)
	if nil == kEdit then
		ODS("[Friend_AddGroup]kEdit is nil\n")
		return
	end
	if 12 <= g_kCommunity.GroupCount then
		AddWarnDataTT(400597)
		return
	end
	local kEditText = kEdit:GetEditText()
	if 0 ~= kEditText:Length() then
		g_FriendMgr:SendFriend_Command_AddGroup(kEditText)
	end
end

function Community_Friend_AddGroupName(kEdit)
	if nil == kEdit then
		ODS("[Friend_AddGroup]kEdit is nil\n")
		return
	end
	if 12 <= g_kCommunity.GroupCount then
		AddWarnDataTT(400597)
		return
	end
	local kEditText = kEdit
	if 0 ~= kEditText:Length() then
		g_FriendMgr:SendFriend_Command_AddGroup(kEditText)
	end
end

--///////////////////////////////////// Community_Friend_Popup ///////////////////////////////////////////
function Community_Friend_Popup(kSelf)--Open popup menu Friend
	local kPopupItem = {}	
	--선택된 친구의 Guid얻기
	g_kCommunity["SelectItemText"] = kSelf:GetCustomDataAsInt()
	local iKey = g_kCommunity["SelectItemText"]
	local kCharacterName = g_kCommunity["FriendList"][iKey]:CharacterName()
--	local kGroupName = g_kCommunity["FriendList"][iKey]:GroupName()
	local kFriendGuid = g_kCommunity["FriendList"][iKey]:Guid()	
	local kFriendItem = g_kCommunity["FriendList"][iKey]
	
--[[	
	--local kFriendItem = g_kCommunity["FriendList"][iKey]
	local iKey = g_kCommunity["SelectItemText"]
	local kCharacterName = kFriendItem:CharacterName()
	local kGroupName = kFriendItem:GroupName()
	local kFriendGuid = kFriendItem:Guid()
	]]	
	
	local kMyActor = GetMyActor()
	local kMyGuid = kMyActor:GetPilotGuid()	

	if kFriendItem:IsOnline() then		
		--kPopupItem[1] = "FRIEND_SEE_INFO"	
		--내가 파티장이고, 대상이 파티가 없으면(이건안됨)
		if( (true == IsPartyMaster(kMyGuid) and true == HaveIParty())
			or 
			false == HaveIParty() ) then
			kPopupItem[2] = "FRIEND_INVITE_PARTY"
		end
		
		--kPopupItem[3] = "FRIEND_TRADE"
		kPopupItem[4] = "FRIEND_WHISPER"
		kPopupItem[5] = "FRIEND_ONE_ON_ONE"
		
		--if not CHINA locale, can use couple invite
		if (LOCALE.NC_CHINA ~= GetLocale()) then
			if (false == IamHaveCouple()) then 
				kPopupItem[6] = "FRIEND_INVITE_COUPLE"
			end	
		end	
		
		--if  false == IsHaveGuild(kFriendGuid) and 
		if (true  == HaveGuild()) and (true  == AmIGuildMaster()) 	then
			kPopupItem[7] = "FRIEND_INVITE_GUILD"
		end	
	end
	kPopupItem[8] = "FRIEND_DELETE"	
	if CheckExistHaveHome() then
		kPopupItem[9] = "FRIEND_MYHOME_INVITE"	
	end
--[[	
	if( true == kFriendItem:IsBlock_HeCanTalk()) then 
		kPopupItem[9] = "FRIEND_CANCEL_ISOLATE"			
		ODS("AAAAAAAAAAAAA\n")
	else
	ODS("BBBBBBBBBBBBBB\n")
		kPopupItem[9] = "FRIEND_ISOLATE"
	end	
]]
	local kPos = GetCursorPos()	
	CallCommonPopup(kPopupItem, kFriendGuid, nil, kPos)
--[[	if kSelf:IsNil() then ODS("FriendPopup kSelf is nil\n") return end
	
	local kPopupGroup = GetUIWnd("SFRM_GROUP_POPUP")
	if not kPopupGroup:IsNil() then CloseUI("SFRM_GROUP_POPUP") end	
	
	local kWnd = CallUI("SFRM_FRIEND_POPUP")
	if not kWnd:IsNil() then
		local kPos = GetCursorPos()
		kWnd:SetLocation(kPos)--Set Popup location
		g_kCommunity["SelectItemText"] = kSelf:GetCustomDataAsInt() --kSelf:GetStaticText() -- saved selected item text
	end--]]
end

--///////////////////////////////////// Community_Friend_LocationToolTip ///////////////////////////////////////////
function Community_Friend_LocationToolTip(kSelf)
	if kSelf:IsNil() then ODS("LocationToolTip kSelf is nil\n") return end
	
	local iKey = kSelf:GetCustomDataAsInt()
	if nil == g_kCommunity["FriendList"][iKey] then return end
	local kFriendItem = g_kCommunity["FriendList"][iKey]
	if kFriendItem:IsNil() then return end
	
	local iLocation = kFriendItem:Location()
	
	if kFriendItem:IsOnline() then
		if iLocation ~= 0 then
			local kLocationStr = GetAppendTextW(GetTT(400270), GetMapNameW(iLocation))
		    CallToolTipText(0, kLocationStr, GetCursorPos(), "ToolTip2", 11)
		else
			-- PvP
			CallToolTipText(0, GetTextW(400300), GetCursorPos(), "ToolTip2", 11)
		end
		g_kCommunity["ShowToolTip"] = true
	end
end

function Community_Friend_CloseToolTip()
    CloseToolTip(nil, 11)
--	if g_kCommunity["ShowToolTip"] then
--		CloseToolTip()
--	end
end

--///////////////////////////////////// Community_Friend_GroupPopup ///////////////////////////////////////////
function Community_Friend_GroupPopup(kSelf)--Open popup menu group
	if 2 == g_kCommunity.iGroupShowMode then --group menu lock at only show on/off line user
		return
	end
	
	if kSelf:IsNil() then ODS("GroupPopup kSelf is nil\n") return end
	--CallUI("DMY_FULLSCREEN")--Call FullScreen Dummy
	
	
	g_kCommunity["ClickedItemWnd"] = kSelf:GetParent() --temp save
	if g_kCommunity["ClickedItemWnd"]:IsNil() then ODS("[Community_Friend_GroupPopup] GetParent() is nil\n") return end
	
	local kPopupFriend = GetUIWnd("SFRM_FRIEND_POPUP")
	if not kPopupFriend:IsNil() then CloseUI("SFRM_FRIEND_POPUP") end
	
	local kWnd = CallUI("SFRM_GROUP_POPUP")
	if not kWnd:IsNil() then
		local kPos = GetCursorPos()
		kWnd:SetLocation(kPos)--Set Popup location
		g_kCommunity["SelectItemText"] = kSelf:GetCustomDataAsInt()--kSelf:GetStaticText() -- saved selected item text
	end
end

--///////////////////////////////////// OnClick_Community_FriendPopup ///////////////////////////////////////////
function OnClick_Community_FriendPopup(kCommand)
	--OnClick_FullScreenDummy()
	
	if nil == g_kCommunity["SelectItemText"] then
		ODS("[FriendPopup] g_kCommunity[SelectItemText] is nil\n")
		return 
	end
	ODS("FriendPopup Command: ".. kCommand.."\n")
	ODS("Selected item key is : ".. g_kCommunity["SelectItemText"] .. "\n")
	
	local iKey = g_kCommunity["SelectItemText"]
	local kCharacterName = g_kCommunity["FriendList"][iKey]:CharacterName()
	local kGroupName = g_kCommunity["FriendList"][iKey]:GroupName()
	local kGuid = g_kCommunity["FriendList"][iKey]:Guid()
	--
	if "PARTY" == kCommand then
		Net_C_N_REQ_JOIN_PARTY_ByGuid(kGuid)
	elseif "WHISPER" == kCommand then
		g_ChatMgrClient:CheckChatCommand(WideString("/MODE 4 "..kCharacterName:GetStr()))
		-- OnClick_ChatMode_Change(4)--Whisper
		
		-- local kChatWnd = GetUIWnd("ChatBar")
		-- if kChatWnd:IsNil() then ODS("ChatBar is nil") return end
		-- local kWhisperBg = kChatWnd:GetControl("SRM_WSP_BG")
		-- if kWhisperBg:IsNil() then ODS("SRM_WSP_BG is nil") return end
		-- local kNameEdit = kWhisperBg:GetControl("EDT_WSP")
		-- if kNameEdit:IsNil() then ODS("EDT_WSP is nil") return end
		
		-- kNameEdit:SetEditTextW(kCharacterName)
		
		-- local kEditWnd = kChatWnd:GetControl("EDT_CHAT")
		-- if kEditWnd:IsNil() then ODS("EDT_CHAT is nil\n") end
		
		-- --OnEnter_FocusEdit(GetUIWnd("ChatBar"))
		
		-- kEditWnd:SetEditFocus(false)
		
	elseif "MODIFYGROUP" == kCommand then
		if nil ~= g_kCommunity["SelectItemText"] then
			local kWnd = CallUI("SFRM_EDIT_GROUP")
			if not kWnd:IsNil() then
				--Call Group Select box
				local kFrnWnd = kWnd:GetControl("FRM_GROUPNAME")
				kFrnWnd:SetStaticTextW(kGroupName)
				kWnd:SetCustomDataAsInt(iKey)
				g_FriendMgr:Friend_List_UpdateReq(true)
			end
		else
			g_FriendMgr:AddEventMessage(700023)
		end
	elseif "DELFRIEND" == kCommand then
		g_FriendMgr:SendFriend_Command_Delete(kGuid)
		g_FriendMgr:Friend_List_UpdateReq(true)
	elseif "SEND_COUPLE" == kCommand then
		Send_ReqCoupleByGuid(kGuid, false)
	else
		ODS("Can't find Friend Popup menu command\n")
	end
	CloseUI("SFRM_FRIEND_POPUP")
end

--////////////////////////////////////// OnClick_Community_GroupPopup //////////////////////////////////////////
function OnClick_Community_GroupPopup(kCommand)
	--OnClick_FullScreenDummy()
	if nil == g_kCommunity["SelectItemText"] then
		ODS("[GroupPopup] g_kCommunity[SelectItemText] is nil\n")
		return
	end
	ODS("GroupPopup Command: ".. kCommand.."("..g_kCommunity["SelectItemText"]..")\n")
	
	local iKey = g_kCommunity["SelectItemText"]
	local kGroupName = g_kCommunity["FriendList"][iKey]:GroupName()
	--
	if "MODIFYGROUP" == kCommand then
		if nil ~= g_kCommunity["ClickedItemWnd"] then
			local wndItem = g_kCommunity["ClickedItemWnd"]
			if not wndItem:IsNil() and wndItem:GetStaticText():GetStr() ~= GetTT(400502):GetStr() then
				local kEditName = g_kCommunity["ClickedItemWnd"]:GetControl("FRM_EDIT_NAME")
				kEditName:Visible(true)
				local edt = kEditName:GetControl("EDT_GROUPNAME")
				if false == edt:IsNil() then
					edt:SetEditTextW(kEditName:GetParent():GetStaticText())
				end
			end
		end
	elseif "DELGROUP" == kCommand then
		g_FriendMgr:SendFriend_Command_DeleteGroup(kGroupName) 
	else
		ODS("Can't find Friend Popup menu command\n")
	end
end

--/////////////////////////////////// OnClick_Community_Close /////////////////////////////////////////////
function OnClick_Community_Close()
	--CloseUI("DMY_FULLSCREEN")
	CloseUI("SFRM_EDIT_GROUP")
	CloseUI("FRM_FRIEND_GROUPLIST")
	CloseUI("SFRM_PARTY_OPTION")
	CloseUI("SFRM_COMMON_EDIT")
	CloseUI("SFRM_COUPLE_FIND")
	CloseUI("SFRM_PROPLE_SEARCH")
	CloseUI("SFRM_SWEETHEART_IF")
	CloseUI("SFRM_PARTY_PEOPLE_FIND")
	--OnClick_FullScreenDummy()
end

--[[/////////////////////////////////// OnClick_FullScreenDummy /////////////////////////////////////////////
function OnClick_FullScreenDummy()
	--All popup menu close
	CloseUI("SFRM_FRIEND_POPUP")
	CloseUI("SFRM_GROUP_POPUP")
	CloseUI("SFRM_PARTY_POPUP")
end]]--

--//////////////////////////////////// OnEsc_CloseCommunity ////////////////////////////////////////////
function OnEsc_CloseCommunity()
	--CloseUI("DMY_FULLSCREEN")
	
	local kPopupFriend = GetUIWnd("SFRM_FRIEND_POPUP")
	if not kPopupFriend:IsNil() then CloseUI("SFRM_FRIEND_POPUP") end
	
	local kPopupGroup = GetUIWnd("SFRM_GROUP_POPUP")
	if not kPopupGroup:IsNil() then CloseUI("SFRM_GROUP_POPUP") end
	
	local kPopupCommon = GetUIWnd("FRM_COMMON_POPUP")
	if not kPopupCommon:IsNil() then CloseUI("FRM_COMMON_POPUP") end
	
	local kCommunity = GetUIWnd("SFRM_COMMUNITY")
	if not kCommunity:IsNil() then 
		CloseUI("SFRM_COMMUNITY")
	else
		CallUI("SFRM_COMMUNITY")
	end
	
	CloseToolTip()
	
	return true
end
--//////////////////////////////////// OnClick_Community_ShowGroup ////////////////////////////////////////////
eCommunity.kGroupBtnSet = {}
eCommunity.kGroupBtnSet[1] = {true, false}
eCommunity.kGroupBtnSet[2] = {false, true}
function OnClick_Community_ShowGroup(kParent, iMode)
	if nil == kParent then
		ODS("FRM_COMMUNITY is nil\n")
		return
	end
	
	if 0 == iMode then
		iMode = Config_GetValue(HEADKEY_USER_UI, SUBKEY_FRIEND_SHOWGROUP)
	end
	
	ODS("Community ShowMode is ".. iMode.."\n")
	
	--[[local kBtnGroup = kParent:GetControl("BTN_SHOWGROUP")
	if kBtnGroup:IsNil() then ODS("BTN_SHOWALL is nil\n") return end
	local kBtnOnline = kParent:GetControl("BTN_ONOFFLINE")
	if kBtnOnline:IsNil() then ODS("BTN_ONOFFLINE is nil\n") return end
	
	if nil == eCommunity.kGroupBtnSet[iMode] then
		if nil ~= eCommunity.kGroupBtnSet[1] then
			iMode = 1
		end
	end
	
	kBtnGroup:CheckState(eCommunity.kGroupBtnSet[iMode][1])
	kBtnOnline:CheckState(eCommunity.kGroupBtnSet[iMode][2])--]]
	
	--g_kCommunity.iGroupShowMode = iMode
	
	Config_SetValue(HEADKEY_USER_UI, SUBKEY_FRIEND_SHOWGROUP, iMode)
	
	Update_FriendList(kParent:GetParent())
end

--//////////////////////////////////// OnOk_EditGroupName ////////////////////////////////////////////
function OnOk_EditGroupName(kTopBg)
	if kTopBg:IsNil() then ODS("[OnCancel_EditGroupName] is nil\n") return end
	
	local iKey = kTopBg:GetCustomDataAsInt()
	--local iKey = g_kCommunity["SelectItemText"]
	local kOldGroupName = g_kCommunity["FriendList"][iKey]:GroupName()
	
	local kEdit = kTopBg:GetControl("EDT_GROUPNAME")
	if kEdit:IsNil() then ODS("EDT_GROUPNAME is nil\n") return end
	local kGroupName= kEdit:GetEditText()
	ClearEditFocus()
	local kTT = GetTT(400502)
	
	if kGroupName:IsEqual(kTT) == false then
		ODS(kGroupName:GetStr() .. " " .. kTT:GetStr())
		
		if kGroupName:FilterString(3, false) then
			AddWarnDataTT(700144)
			return
		end
		
		if 0>=kTopBg:GetParent():GetTreeItemCount() then
			local kResultName = WideString(kGroupName:GetStr() .. " (0/0)")
			--kTopBg:GetParent():GetControl("SFRM_BG"):SetStaticTextW(kResultName)
			g_kCommunity["FriendList"][iKey]:SetGroupName(kGroupName:GetStr())
			g_FriendMgr:ChangeGroupName(kOldGroupName, kGroupName)
		else
			g_FriendMgr:SendFriend_Command_ModifyGroupName(kOldGroupName, kGroupName)
			g_FriendMgr:Friend_List_UpdateReq(true)
		end
	end
	
	kTopBg:Visible(false)
end
--//////////////////////////////////// OnClick_Community_WaitCannel ////////////////////////////////////////////
function OnClick_Community_WaitCannel(kTop)
	if kTop:IsNil() == false then
		local iKey = kTop:GetCustomDataAsInt()
		local kGuid = g_kCommunity["FriendList"][iKey]:Guid()
		g_FriendMgr:SendFriend_Command_Delete(kGuid)
		g_FriendMgr:Friend_List_UpdateReq(true)		
	end
end
--//////////////////////////////////// OnClick_Community_AcceptFriend ////////////////////////////////////////////
function OnClick_Community_AcceptFriend(kTop)
	if kTop:IsNil() == false then
		local wndaccpt = kTop:GetControl("BTN_ACCEPT")
		if wndaccpt:IsNil() == false then
			wndaccpt:Visible(false)
		end
		wndaccpt = kTop:GetControl("BTN_REJECT")
		if wndaccpt:IsNil() == false then
			wndaccpt:Visible(false)
		end
	end
	local iKey = kTop:GetCustomDataAsInt()
	local kGuid = g_kCommunity["FriendList"][iKey]:Guid()
	g_FriendMgr:Friend_Accept(kGuid)
end
--//////////////////////////////////// OnClick_Community_RejectFriend ////////////////////////////////////////////
function OnClick_Community_RejectFriend(kTop)
	if kTop:IsNil() == false then
		local wndaccpt = kTop:GetControl("BTN_ACCEPT")
		if wndaccpt:IsNil() == false then
			wndaccpt:Visible(false)
		end
		wndaccpt = kTop:GetControl("BTN_REJECT")
		if wndaccpt:IsNil() == false then
			wndaccpt:Visible(false)
		end
	end
	local iKey = kTop:GetCustomDataAsInt()
	
	local kGuid = g_kCommunity["FriendList"][iKey]:Guid()
	g_FriendMgr:Friend_Reject(kGuid)
end

--//////////////////////////////////// OnCancel_EditGroupName ////////////////////////////////////////////
function OnCancel_EditGroupName(kTopBg)
	if kTopBg:IsNil() then ODS("[OnCancel_EditGroupName] is nil\n") return end
	kTopBg:Visible(false)
end

--//////////////////////////////////// OnClick_Community_ModifyGroup ////////////////////////////////////////////
function OnClick_Community_ModifyGroup(kTop)
	ODS("OnClick_Community_ModifyGroup()\n")
	if kTop:IsNil() then ODS("[OnClick_Community_ModifyGroup] kTop is nil\n") return end

	local kFrmNewGroup = kTop:GetControl("SFRM_COLOR"):GetControl("SFR_SDW"):GetControl("SRM_WSP_BG")
	if kFrmNewGroup:IsNil() then ODS("[OnClick_Community_ModifyGroup] FRM_GROUPNAME is nil \n") return end
	
	local kNewGroup = kFrmNewGroup:GetStaticText()
	local iGroupKey = kFrmNewGroup:GetCustomDataAsInt()
	
	local iKey = kTop:GetCustomDataAsInt()
	ODS("OnClick_Community_ModifyGroup  iKey : "..iKey.."\n")
	if nil==g_kCommunity["FriendList"][iKey] then return end
	if nil == g_kCommunity["GroupList"][iGroupKey] then return end
	
	local kGroupName = g_kCommunity["GroupList"][iGroupKey]
	local kGuid = g_kCommunity["FriendList"][iKey]:Guid()
	
	g_FriendMgr:SendFriend_Command_ModifyGroup(kGuid, kGroupName)
	
	CloseUI("SFRM_EDIT_GROUP")
end

--//////////////////////////////////// OnClick_Community_ShowGroupList ////////////////////////////////////////////
function OnClick_Community_ShowGroupList()
	Update_FriendList()
	
	local kWnd = CallUI("FRM_FRIEND_GROUPLIST", true)
	if kWnd:IsNil() then ODS("FRM_FRIEND_GROUPLIST is nil\n") return end
	local kList = kWnd:GetControl("LST_FRIEND_GROUPLIST")
	if kList:IsNil() then ODS("LST_FRIEND_GROUPLIST is nil\n") return end
	
	kList:ClearAllListItem()
				
	for kKey, kVal in pairs(g_kCommunity["GroupList"]) do
		if not kVal:IsNil() then
			local kItem = kList:ListFindItemW(kVal)
			if kItem:IsNil() then
				ODS("AddNewListItem : " .. kVal:GetStr().."("..kKey..")".."\n")
				local kStrName = kVal:GetStr()
				if kVal:Length() > 10 then
					kStrName = string.sub(kStrName, 1,10)
					kStrName = kStrName .. ".."
				end
				local kItem = kList:AddNewListItemChar(kStrName)
				if kItem:IsNil() then ODS("[..ShowGroupList] add failed item is nil") return end
				local kItemWnd = kItem:GetWnd()
				if kItemWnd:IsNil() then ODS("[..ShowGroupList] Item Wnd is nil") return end
				kItemWnd:SetCustomDataAsInt(kKey)
			end
		end
	end
	
	local listcount = kList:GetListItemCount()
	if 0<listcount then
		kList:SetSize(Point2(kList:GetSize():GetX(), 22*(kList:GetListItemCount())))
	else
		kWnd:Close()
	end
end

--//////////////////////////////////// OnClick_Community_ChangeGroup ////////////////////////////////////////////
function OnClick_Community_ChangeGroup(kSelf)
	local iKey = kSelf:GetCustomDataAsInt()
	local kGroupName = g_kCommunity["GroupList"][iKey]
	
	ODS("ChageGroup Clicked : "..iKey.."\n")
	
	local kWnd = GetUIWnd("SFRM_EDIT_GROUP")
	if kWnd:IsNil() then ODS("[OnClick_Community_ChangeGroup] SFRM_EDIT_GROUP is nil\n") return end
	local kGroupFrm = kWnd:GetControl("SFRM_COLOR"):GetControl("SFR_SDW"):GetControl("SRM_WSP_BG")
	if kGroupFrm:IsNil() then ODS("[OnClick_Community_ChangeGroup] FRM_GROUPNAME is nil\n") return end

	local kStrName = kGroupName:GetStr()
	local iCnt = kGroupName:Length()

	if iCnt > 10 then
		kStrName = string.sub(kStrName, 1,10)
		kStrName = kStrName .. ".."
	end

	kGroupFrm:SetStaticText(kStrName)
	kGroupFrm:SetCustomDataAsInt(iKey)
	
	CloseUI("FRM_FRIEND_GROUPLIST")
end


--//////////////////////////////////// OnClick_Tab_Community ////////////////////////////////////////////
g_kCommunity["CommunityTabSet"] = {}
g_kCommunity["CommunityTabSet"][1] = { true, false, false, false, false }
g_kCommunity["CommunityTabSet"][2] = { false, true, false, false, false }
--g_kCommunity["CommunityTabSet"][3] = { false, false, true, false }
g_kCommunity["CommunityTabSet"][4] = { false, false, false, true, false }
g_kCommunity["CommunityTabSet"][5] = { false, false, false, false, true }
function OnClick_Tab_Community(kBGFrm, iSetNo)
	if kBGFrm:IsNil() then return end
	
	if 0 == iSetNo then
		iSetNo = Config_GetValue(HEADKEY_USER_UI, SUBKEY_LAST_COMMUNITY)
	end
	
	local kVal = g_kCommunity["CommunityTabSet"][iSetNo]
	if nil == kVal then
		kVal = g_kCommunity["CommunityTabSet"][1]
		iSetNo = 1
	end
	
	local kBtnCommunity = kBGFrm:GetControl("CBTN_COMMUNITY")
	if kBtnCommunity:IsNil() then return end
	local kBtnParty = kBGFrm:GetControl("CBTN_PARTY")
	if kBtnParty:IsNil() then return end
	--local kBtnGuild = kBGFrm:GetControl("CBTN_GUILD")
	--if kBtnGuild:IsNil() then return end
	local kBtnCouple = kBGFrm:GetControl("CBTN_COUPLE")
	if kBtnCouple:IsNil() then return end
	local BtnBlock = kBGFrm:GetControl("CBTN_BLOCK")
	if BtnBlock:IsNil() then return end
	
	kBtnCommunity:CheckState( kVal[1] )
	kBtnParty:CheckState( kVal[2] )
	--kBtnGuild:CheckState( kVal[3] )
	kBtnCouple:CheckState( kVal[4] )
	BtnBlock:CheckState( kVal[5] )
	
	local kTopFrm = kBGFrm:GetParent()
	if kTopFrm:IsNil() then return end
	local kFriendFrm = kTopFrm:GetControl("FRM_COMMUNITY")
	if kFriendFrm:IsNil() then return end
	local kPartyFrm = kTopFrm:GetControl("FRM_PARTY")
	if kPartyFrm:IsNil() then return end
	--local kGuildFrm = kTopFrm:GetControl("FRM_GUILD")
	--if kGuildFrm:IsNil() then return end
	local kCoupleFrm = kTopFrm:GetControl("FRM_COUPLE")
	if kCoupleFrm:IsNil() then return end
	local BlockFrm = kTopFrm:GetControl("FRM_BLOCK")
	if BlockFrm:IsNil() then return end
	
	kFriendFrm:Visible( kVal[1] )
	kPartyFrm:Visible( kVal[2] )
	--kGuildFrm:Visible( kVal[3] )
	kCoupleFrm:Visible( kVal[4] )
	BlockFrm:Visible( kVal[5] )
	
	if 1 == iSetNo then
		SortFriendMember(kTopFrm,0)		
	elseif 3 == iSetNo then
		SortGuildMember(kTopFrm,0)
	end
	
	if iSetNo == 5 then
		local blockwnd = BlockFrm:GetControl("SFRM_COLORBOX4"):GetControl("EDT_CHAT")						
		blockwnd:SetEditText("")
		SetEditFocus(blockwnd:GetParent():GetID():GetStr(), blockwnd:GetID():GetStr())
		blockwnd:SetEditFocus(true)
	else
		ClearEditFocus()
	end

	Config_SetValue(HEADKEY_USER_UI, SUBKEY_LAST_COMMUNITY, iSetNo)
end

function OnClick_StatusButton(wnd, TextNo)
	if wnd:IsNil() == true or TextNo == 0 then
		return
	end
	local iStatus = TextNo - 20
	if iStatus == g_FriendMgr:MyLinkStatus() then	--이전 상태와 같으면 구태여 업데할 필요 없음
		return
	end

	local drop = wnd:GetParent()
	if drop:IsNil() == false then
		local frm = drop:GetParent()
		if frm:IsNil() == false then
			frm:SetSize(Point2(87, 24))
			local stat = frm:GetControl("BTN_STATUS")
			if stat:IsNil() == false then
				stat:SetStaticTextW(GetTextW(TextNo))
				g_FriendMgr:SendFriend_Command_LinkStateChange(iStatus)
			end
		end
		local dsize = drop:GetSize()
		dsize:SetY(0)
		drop:SetSize(dsize)
		drop:Visible(false)
	end
end

function Community_Friend_Click_ChatStatue(ui, i)
	if ui:IsNil() == true then
		return
	end
	local kItem = ui:GetParent()
	if kItem:IsNil() == true then
		return
	end
	local iKey = kItem:GetCustomDataAsInt()
	local kGuid = g_kCommunity["FriendList"][iKey]:Guid()
	if kGuid:IsNil() == true then
		return
	end	

	g_FriendMgr:SendFriend_Command_ChatStateChange(kGuid, i)
end


function FriendPopUpWhisper()			
	local iKey = g_kCommunity["SelectItemText"]
	local kCharacterName = g_kCommunity["FriendList"][iKey]:CharacterName()
	local kGroupName = g_kCommunity["FriendList"][iKey]:GroupName()
	local kGuid = g_kCommunity["FriendList"][iKey]:Guid()	
	g_ChatMgrClient:CheckChatCommand(WideString("/MODE 4 "..kCharacterName:GetStr()))
end

function SetTempFriendViewMode(iMode)
	-- 0 : all view / 1 : view online
	if(iMode<0) then return end
	if(iMode>1) then return end
	g_kCommunity.iTempGroupShowMode = iMode
end

function ViewFriendItemAccodingViewMode()
	g_kCommunity.iGroupShowMode = g_kCommunity.iTempGroupShowMode	
	local kWnd = GetUIWnd("SFRM_COMMUNITY")
	if (nil == kWnd) then return end
	if (true == kWnd:IsNil()) then return end
	local kFrmCommWnd = kWnd:GetControl("FRM_COMMUNITY")
	if (nil == kFrmCommWnd) then return end
	if (true == kFrmCommWnd:IsNil()) then return end
	OnClick_Community_ShowGroup(kFrmCommWnd, g_kCommunity.iGroupShowMode)
end

function GetCurrentViewMode()
	return g_kCommunity.iGroupShowMode
end

function Community_Friend_MyhomeGo(kSelf)
	g_kCommunity["SelectItemText"] = kSelf:GetCustomDataAsInt()
	local iKey = g_kCommunity["SelectItemText"]
	local kFriendGuid = g_kCommunity["FriendList"][iKey]:Guid()	
	SendMyhomeGobyFriend(kFriendGuid)
end

function Community_Guild_MyhomeGo(kSelf)
	local kGuid = kSelf:GetOwnerGuid()
	SendMyhomeGobyGuild(kGuid)
end

function Community_Party_MyhomeGo(kSelf)
	local kGuid = kSelf:GetOwnerGuid()
	SendMyhomeGobyParty(kGuid)
end

function Community_Couple_MyhomeGo(kSelf)
	local kGuid = kSelf:GetOwnerGuid()
	SendMyhomeGobyCouple(kGuid)
end


function SortFriendMember(kCommunityTop,iMode)
	if (nil==kCommunityTop) or (true==kCommunityTop:IsNil()) then return end
	
	SetFriendMemberSortType(iMode)
	Update_FriendList(kCommunityTop)
end

function SortGuildMember(kCommunityTop,iMode)
	if (nil==kCommunityTop) or (true==kCommunityTop:IsNil()) then return end
	
	SetGuildMemberSortType(iMode)
	UpdateGuildUI(kCommunityTop)
end 

function RegistChatBlock(EditWnd)
	if EditWnd:IsNil() then
		return
	end
	
	local BlockName = EditWnd:GetEditText_TextBlockApplied()
	local InputText = EditWnd:GetEditText()
	EditWnd:SetEditText("")
	
	if true == InputText:FilterString(1, false) then
			CommonMsgBox(GetTT(401355):GetStr(), true)
		return
	end				
	
	g_ChatMgrClient:Regist_ChatBlockUser(BlockName)
end

function Community_ChatBlock_Popup(Self)
	local PopupItem = {}
	
	local BlockName = Self:GetCustomDataAsStr()
	PopupItem[0] = "DELETE_CHAT_BLOCK_LIST"
	
	local Pos = GetCursorPos()
	
	CallChatPopup(PopupItem, BlockName, Pos)
end

--//////////////////////////////////// Community_Event ////////////////////////////////////////////
function OnClick_Community_Event(Self)
	local List = Self:GetParent()
	local Item = List:ListFirstItem()
	while false==Item:IsNil() do
		local ItemWnd = Item:GetWnd()
		ItemWnd:GetControl("IMG_OX_OVER"):SetUVIndex(1)
		ItemWnd:GetControl("IMG_OX_OVER"):Visible(false)

		Item = List:ListNextItem(Item)
	end
	Self:GetControl("IMG_OX_OVER"):SetUVIndex(3)
	Self:GetControl("IMG_OX_OVER"):Visible(true)
end

g_SelectedCommunityEventNo = 0
function GetSelectedCommunityEventNo()
	return g_SelectedCommunityEventNo
end

function Net_Send_Event_Exit()
	local packet = NewPacket(8226)--PT_C_M_REQ_DEFAULT_MAP_MOVE
	Net_Send(packet)
	DeletePacket(packet)
end

g_fCommunityEventRefreshTime = 0.0
g_CommunityEventTopWnd = false
g_BattleSquareTopWnd = false
g_CommunityEventType = ""
g_CommunityEventMode = ""
function CallSetEventList()
	if g_CommunityEventTopWnd == true then
		SetEventList(GetUIWnd("SFRM_COMMUNITY_EVENT"), g_CommunityEventType, g_CommunityEventMode)
	end
	if g_BattleSquareTopWnd == true then
		SetEventList(GetUIWnd("SFRM_COMMUNITY_EVENT"), g_CommunityEventType, g_CommunityEventMode)
	end
end

function GetClassNoByUVIdx(idx)
	if idx > 28 then
		return idx+22
	end
	return idx
end
