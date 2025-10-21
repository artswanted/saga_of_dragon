function HelpSystem_OnActivateObject(kHelpSystem, kHelpObject)
	if kHelpObject:IsNil() then return end
		
	if 0 ~= kHelpObject:GetCallEventID() then
		--Only Call Event Script
		local bEventScriptTalk = GetQuestMan():IsEventScriptDialog()
		local bQuestTalk = GetQuestMan():IsFullQuestDialog()
		if false == bEventScriptTalk and
			false == bQuestTalk then
			GetEventScriptSystem():ActivateEvent( kHelpObject:GetCallEventID() );
		end
	end
	
	if 0 ~= kHelpObject:GetMessageTTID() then
		local iMinKeyHelp = 1
		local iMaxKeyHelp = 18
		local iHelpType = kHelpObject:GetUIType()
		if 0 == iHelpType then
			OnCall_HelpUI(kHelpSystem, kHelpObject) -- Normal Help
		elseif iMinKeyHelp <= iHelpType and 17 >= iHelpType then
			OnCall_ControlHelpUI(kHelpSystem, kHelpObject) -- Key Control Help, Quest Mark
		elseif 18 == iHelpType then
			OnCall_HelpUI2(kHelpSystem, kHelpObject)
		else
			MessageBox("invalid help Type :" .. iHelpType)
		end
	end
	if 0 ~= kHelpObject:GetHelperMsgTTID() then
		TalkHelper( kHelpObject:GetHelperMsgTTID() )
	end
end

function OnCall_HelpUI(kHelpSystem, kHelpObject)
	CloseUI("FRM_HELP_CONTROL")
	local	HelpWnd = GetUIWnd("FRM_HELP_MINI")
	if HelpWnd:IsNil() == true then
		HelpWnd = CallUI("FRM_HELP_MINI")
	end
	--RegistUIAction(HelpWnd, "CloseUI")
	HelpWnd:SetAliveTime(18000)
	HelpWnd:RefreshCalledTime()
	
	local CWndIcon = HelpWnd:GetControl("FRM_BIG_ICON")
	if CWndIcon:IsNil() == false then
		local CRelatedID = kHelpObject:GetRelatedUIName()
		if string.len(CRelatedID) ~= 0 then
			CWndIcon:SetCustomDataAsStr(CRelatedID);
			CWndIcon:SetUVIndex(1)
		else
			CWndIcon:SetCustomDataAsStr(" ");
			CWndIcon:SetUVIndex(3)
		end
	end
	local CWndTitle = HelpWnd:GetControl("FRM_HELP_TITLE")
	if not CWndTitle:IsNil() then
		CWndTitle:SetStaticTextW(GetTT(kHelpObject:GetTitleTTID()))
	end
	local CWndContents = HelpWnd:GetControl("FRM_HELP_CONTENTS")
	if not CWndContents:IsNil() then
		CWndContents:SetStaticTextW(GetTT(kHelpObject:GetMessageTTID()))
	end
end

function OnCall_ControlHelpUI(kHelpSystem, kHelpObject)
	CloseUI("FRM_HELP_MINI")
	local kHelpWnd = GetUIWnd("FRM_HELP_CONTROL")
	if kHelpWnd:IsNil() then
		kHelpWnd = CallUI("FRM_HELP_CONTROL")
	end
	
	--RegistUIAction(kHelpWnd, "CloseUI")
	local iTitleTextID = kHelpObject:GetTitleTTID()
	kHelpWnd:SetStaticTextW( GetTT(iTitleTextID) )
	kHelpWnd:SetAliveTime(36000)
	kHelpWnd:RefreshCalledTime()
	
	local kContensText = kHelpWnd:GetControl("FRM_CONTENTS")
	if not kContensText:IsNil() then
		local iMsgTextID = kHelpObject:GetMessageTTID()
		kContensText:SetStaticTextW( GetTT(iMsgTextID) )
		local kPos = kContensText:GetLocation()
		local iY = (85 - kContensText:GetTextSize():GetY()) / 2
		kPos:SetY( 35 + iY )
		kContensText:SetLocation(kPos)
	end
	
	local iCurVisibleHelp = kHelpObject:GetUIType()
	local iMaxControlHelp = 12
	local iMaxHelp = iMaxControlHelp + 6
	for iCur = 1, iMaxControlHelp do -- Key Control Visible / Invisible
		local kControlHelpWnd = kHelpWnd:GetControl( "IMG_" .. iCur )
		if not kControlHelpWnd:IsNil() then
			local bIsVisible = iCur == iCurVisibleHelp
			kControlHelpWnd:Visible( bIsVisible )
		end
	end
	
	local kQuestMarkUI = kHelpWnd:GetControl("IMG_QuestMark")
	if not kQuestMarkUI:IsNil() then -- Quest Mark Visible / Invisible
		local bVisible = (iMaxControlHelp < iCurVisibleHelp and iMaxHelp >= iCurVisibleHelp)
		kQuestMarkUI:SetUVIndex( iCurVisibleHelp - iMaxControlHelp )
		kQuestMarkUI:Visible( bVisible )
	end
end

function OnTick_ControlHelp(kSelf)
	if nil == kSelf then return end
	if kSelf:IsNil() then return end
	if not kSelf:IsVisible() then return end
	g_kCurHelpTime = g_kCurHelpTime + GetFrameTime()
	if 0.4 < g_kCurHelpTime then
		local iNextUV = kSelf:GetUVIndex() + 1
		if kSelf:GetMaxUVIndex() < iNextUV then iNextUV = 1 end
		kSelf:SetUVIndex(iNextUV)
		g_kCurHelpTime = 0
	end
end

function OpenBookPage(iPage)
	local kBook = ActivateUI("FRM_BOOK"..iPage)
	if kBook:IsNil() then
		return kBook
	end

	UI_Bool_Tab_Init(kBook, iPage)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetShowWorldFocusFilter(true, "../Data/5_Effect/9_Tex/EF_blackBG.tga", 0.8) 
	return kBook
end

function OpenBookPagePage(iPage, iPage2)
	local kBook = OpenBookPage(iPage)
	if nil==kBook then return nil end

	local kPage = kBook:GetControl("FRM_PAGE"..iPage2)
	if kPage:IsNil() then return nil end
	return kPage
end

function DirectOpenUI(OpenID)
	if OpenID == nil or string.len(OpenID) == 0 then
		return
	end
	
	if OpenID == "OPEN_BOOK_ACHIEVE" then
		OpenBookPage(1)
--		OnCallDetailAchievementView()
	elseif OpenID == "OPEN_INV" then
		ActivateUI("Inv")
	elseif OpenID == "OPEN_COMMUNITY" then
		ActivateUI("SFRM_COMMUNITY")
	elseif OpenID == "OPEN_SKILL" then
		ActivateUI("SFRM_SKILL_WINDOW")
	elseif OpenID == "OPEN_CASH_SHOP" then
		if(true == IsOnMacroCheck()) then  
			return	
		end
		CallCashShop() 
	elseif OpenID == "OPEN_BOOK" then
		ActivateUI("FRM_BOOK")
	elseif OpenID == "OPEN_BOOK_SOUL" then
		local kPage = OpenBookPagePage(4,4)
		if nil==kPage then return end
		UI_Click_Book_Item(kPage:GetControl("CBTN_LEFT_TAB4"))
	elseif OpenID == "OPEN_BOOK_ENCHANT" then
		local kPage = OpenBookPagePage(4,4)
		if nil==kPage then return end
		UI_Click_Book_Item(kPage:GetControl("CBTN_LEFT_TAB3"))
	elseif OpenID == "OPEN_BOOK_ITEM_GRADE" then
		local kPage = OpenBookPagePage(4,4)
		if nil==kPage then return end
		UI_Click_Book_Item(kPage:GetControl("CBTN_LEFT_TAB2"))
	elseif OpenID == "OPEN_BOOK_RECOMMAND_ITEM" then
		local kPage = OpenBookPagePage(4,4)
		if nil==kPage then return end
		UI_Click_Book_Item(kPage:GetControl("CBTN_LEFT_TAB1"))
	elseif OpenID == "OPEN_BOOK_EXPLANIN_ITEM" then
		local kPage = OpenBookPagePage(4,4)
		if nil==kPage then return end
		UI_Click_Book_Item(kPage:GetControl("CBTN_LEFT_TAB0"))
	elseif OpenID == "OPEN_CHAR_CARD" then
		local kInfo = ActivateUI("CharInfo")
		if kInfo:IsNil() then
			return
		end
		SetMyCharacterCard() 
		ChangeInfoTab(kInfo:GetControl("CBTN_TAB_2"), 2)
	elseif OpenID == "OPEN_PET" then
		local kInfo = ActivateUI("CharInfo")
		if kInfo:IsNil() then
			return
		end

		local kTab = kInfo:GetControl("CBTN_TAB_1")
		if kTab:IsNil() then
			return
		end
		
		local kPilot = g_pilotMan:GetPlayerPilot();
		if kPilot:IsNil() == false then
			local kUnit = kPilot:GetUnit()
			local kPetGuid = kUnit:GetSelectedPet()
			ChangeInfoTab(kTab, 1) 
			ViewPetEquip(g_pilotMan:GetPlayerPilotGuid())
			SetPetInfoToUI(kTab:GetParent():GetControl("FRM_PET_INFO"), kUnit:GetSelectedPet())
		end
	elseif OpenID == "OPEN_QUEST" then
		ActivateUI("FORM_MY_QUEST_LIST")
	elseif OpenID == "OPEN_WORLD_MAP" then
		CallWorldMap(false)
	elseif OpenID == "OPEN_MINI_MAP" then
		CallWorldMap(false)
	elseif OpenID == "OPEN_AUCTION" then
		if(true == IsOnMacroCheck()) then  
			return	
		end
		if IsExistUniqueType("NO_DUPLICATE") == true then return end
		local wndTrade = GetUIWnd("SFRM_TRADE")
		if false == wndTrade:IsNil() then
			return 
		end
		ClearChatBar()				
		CallOpenMarket()
	elseif OpenID == "OPEN_PVP" then
		if(true == IsOnMacroCheck()) then  
			return	
		end
		OnClickPvP()
	end
end

function OnCall_CreateNewRace()
	local kMain = CallUI("FRM_HELP_MINI")
	if nil==kMain or true==kMain:IsNil() then
		return
	end
	local kContents = kMain:GetControl("FRM_HELP_CONTENTS")
	kContents:SetStaticText( GetTextW(791521):GetStr() )
end









