UISelf = nil

function UI_SetEnv(wnd)
	UISelf = wnd
end

g_LoginToMapMoveComplete = false
function UI_LoginToMapMoveComplete()
	if g_LoginToMapMoveComplete then return end

	g_LoginToMapMoveComplete = true
	
	local eLocale = GetLocale()
	
	if eLocale == LOCALE.NC_KOREA then
		if IsPlayerPlayTime() then
			AddWarnDataStr(GetTT(799017), 0, false)
		end
	end
end

function UI_MapMoveComplete() -- net.lua에서 옮겨옴.
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if g_iMapMoveStage ~= 4 then
		return
	end
	g_iMapMoveStage = 5
	ODS("UI_MapMoveComplete\n")
	
	local kLoadingUI = GetUIWnd("FRM_LOADING_IMG")
	if false == kLoadingUI:IsNil() then
		kLoadingUI:Close()
	else
		g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 1.0, 0.0, 1.0, false, false )
	end
	EndScreenUpdate()
	
	-- Game UI
  	PlayBgSound(0)

	if true == g_world:IsHaveWorldAttr(GATTR_FLAG_NOMINIMAP) then
		UI_DefaultActivate(true)
		-- 튜토리얼 맵은 미니맵을 끈다.
		if	g_world:GetMapNo() == 9910100 then
			CloseCurrentMiniMap()
		end
		
	else
		UI_DefaultActivate(false)
		CallUI("FRM_TOWN_NAME")
	end
	
	if GetVisitFirstMap() then
		g_MovieMgr:ScenarioMovie(g_world:GetMapNo())
	end
	
	CheckSystemInventory()
--	CheckSafeFoam()
	OnSendBuild_HomeList()

	ODS("SetLockMyInput fasle " .. GetAccumTime() .. "\n") 
	
	UI_LoginToMapMoveComplete()
	ODS("UI_MapMoveComplete END\n")
end

function UI_DefaultActivate( bCloseMiniMap )
	UI_BaseActivate()
	
	local bDefenceMission = IsDefenceMode() or IsDefenceMode7() or IsDefenceMode8()
	if not bDefenceMission then
		ActivateUI("FRM_PARTY_STATE")
	end
	
	if nil == bCloseMiniMap or false ==  bCloseMiniMap then
	    CallPlayTime("BTN_TIMER")
		if( nil ~= g_world ) then 
			if( false == g_world:IsNil() ) then
				if( true == g_world: IsExistMiniMapData() ) then 
					ShowCurrentMiniMap(0)
				end
			end
		end
		
	else
		CloseCurrentMiniMap() -- 이거 해주는게 더 안전하지 않을까.
	end
	--휴식 경험치 ------------------
	SetRestXpMode(g_RestExpOn)

	if(true == g_bNoticeRestExp and true == g_bIsRestIconVisible) then
		CallUI("FRM_RESERVE_NOTICE_REST_EXP")
	end
	
	if true == g_NotifyPartyFunction then
		CallUI("FRM_NOTICE_PARTY_FUNCTION")
	end
	
	g_bNoticeRestExp = false
	--------------------------------
	CallRandomQuestBtn()
	CallRandomTacticsQuestBtn()
	ActivatePremiumUI()
	
	local iNextTimeInSec = GetNextIngameNoticeShowTimeInSec()
	GetEventTimer():Add("NOTICE_INGAME", iNextTimeInSec, "CallUI('FRM_NOTICE_INGAME')", 1, false)

end

function CheckActiveStatusUI()
	local wnd = GetUIWnd("FRM_STATUS_UP")
	if nil~=wnd and false==wnd:IsNil() then
		if not IsViewActiveStatusUI() then
			wnd:Close()
		end
	end
end

function IsViewActiveStatusUI()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kMyPlayer = g_pilotMan:GetPlayerPilot()

	if g_world:IsHaveWorldAttr(GATTR_BATTLESQUARE) then return false end
	if g_world:IsHaveWorldAttr(GATTR_FLAG_MYHOME) then return false end
	if kMyPlayer:GetAbil(AT_STATUS_BONUS) < 1 then return false end
	if IsStrategyDefenceMode() then return false end
	
	return true
end

g_kSaveChatTapNo = 1
function UI_BaseActivate()
	ActivateUI("Cursor")
	ActivateUI("FormHero")
	ActivateUI("QuickInv")
	ActivateUI("SFRM_SYSTEMMENU")
	ActivateUI("QuickMenu")
	ActivateUI("Bg_XP_Bar")
	ActivateUI("FRM_BUFF")
	
	if IsViewActiveStatusUI() then 	-- Status Bonus가 존재하면
		ActivateUI("FRM_STATUS_UP")						-- Active Status UI를 호출함
	else
		CloseUI("FRM_STATUS_UP")
	end
	
	local chatStat = ECS_COMMON
	if nil~=g_world and g_world:IsHaveWorldAttr(GATTR_FLAG_MYHOME) then	--마이홈
		chatStat = ECS_MYHOME
	end

	if chatStat ~= ECS_MYHOME then
		g_ChatMgrClient:SetChatStation(chatStat)
		CallChatWindow(g_kSaveChatTapNo)
	end
	ReCallQuickSlotEx()
	--ActivateUI("FRM_FRAN")
	UI_GreateField()	
end

function UI_GreateField()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local WorldAttr = g_world:GetAttr()

	if GATTR_PVP == WorldAttr then
	elseif GATTR_MISSION == WorldAttr then
	elseif GATTR_BOSS == WorldAttr then
	elseif GATTR_EMPORIABATTLE == WorldAttr then
	else
	end
	
	local iShowNavi = Config_GetValue(HEADKEY_ETC,SUBKEY_NAVIGATION)
	if 0 ~= iShowNavi then
		CallNaviMap_WorldMap()
	end
end

g_ListMoveRange = 0
function UI_ChatActivate()
	local kChatBar = ActivateUI("ChatBar")
	local kDMYChatBar = ActivateUI("DMY_ChatBar")
	local State = GetContentsStatus()
	local kMoveRange = 0
	
	if false == kChatBar:IsNil() then
		
		local kPos = kChatBar:GetLocation()-- 위치 조정하기
		local kSize = kChatBar:GetSize()-- 크기 조정하기
		if STATUS_ROOM == State then
			kPos:SetX(53)
			kPos:SetY(695)
			kMoveRange = 609 - kSize:GetX()
			kSize:SetX(609)
		else
			kPos:SetX(0)
			kPos:SetY(669)
			kMoveRange = 482 - kSize:GetX()
			kSize:SetX(482)
		end
		kChatBar:SetLocation(kPos)
		kChatBar:SetSize(kSize)
		
		-- ChatListUp버튼
		local kBtnC = kChatBar:GetControl("BTN_TEXT_COLOR")
		if false == kBtnC:IsNil() then
			kPos = kBtnC:GetLocation()
			kPos:SetX(kPos:GetX() + kMoveRange)
			kBtnC:SetLocation(kPos)
		end
		local kBtnEC = kChatBar:GetControl("BTN_EMOTICON")
		if false == kBtnEC:IsNil() then
			kPos = kBtnEC:GetLocation()
			kPos:SetX(kPos:GetX() + kMoveRange)
			kBtnEC:SetLocation(kPos)
		end
		local kBtnET = kChatBar:GetControl("BTN_EMOTION")
		if false == kBtnET:IsNil() then
			kPos = kBtnET:GetLocation()
			kPos:SetX(kPos:GetX() + kMoveRange)
			kBtnET:SetLocation(kPos)
		end
		-- if false == kForm:IsNil() then
			-- -- 위치 조정
			-- if 0~= kMoveRange then
				-- kPos = kForm:GetLocation()
				-- kPos:SetX(kPos:GetX()+kMoveRange)
				-- kForm:SetLocation(kPos)
			-- end
			
			-- local kListUpBtn = kForm:GetControl("Chat_List_Up")
			-- if false == kListUpBtn:IsNil() then
				-- -- 잠그기 / 풀기
				-- if STATUS_ROOM == State then
					-- kListUpBtn:SetClose(true)
				-- else
					-- kListUpBtn:SetClose(false)
				-- end
			-- end
		-- end

		-- 입력창
		local kEditBG = kChatBar:GetControl("SFRM_EDT_BG")
		if false == kEditBG:IsNil() then
			-- 크기 조정
			if 0~= kMoveRange then
				kSize = kEditBG:GetSize()
				kSize:SetX(kSize:GetX()+kMoveRange)
				kEditBG:SetSize(kSize)
				local kEditShadowBG = kEditBG:GetControl("SFRM_EDT_BG_SHADOW")
				if false == kEditShadowBG:IsNil() then
					kEditShadowBG:SetSize(kSize)
				end
			end
		end
	end
	
	if true ~= g_ChatMgrClient:CheckChatOut() then
		if 0 ~= g_ListMoveRange then
			kMoveRange = kMoveRange + g_ListMoveRange
			g_ListMoveRange = 0
		end
		UI_ChatListActivate(State, false, kMoveRange)
	else
		g_ListMoveRange = kMoveRange
	end
end

function UI_ChatListActivate(State, bIgnore, kMoveRange)
	local bIsReset = false
	local kPos = Point2()
	local bPvPChat = false
	local iDefaultChatMode = 2
	
	if STATUS_ROOM == State then
		kPos:SetX(52)
		kPos:SetY(575)
		ChatSetPreset(1)
		bPvPChat = true				
		bIsReset = true
	elseif STATUS_PLAYING == State then
		ChatSetPreset(1)
		iDefaultChatMode = 7
		kPos:SetX(2)
		kPos:SetY(g_kChatOutPreset[g_kChatOutSet]["MAIN_Y"])
		bIsReset = true
	else
		ChatSetPreset(1)
		kPos:SetX()
		kPos:SetY(g_kChatOutPreset[g_kChatOutSet]["MAIN_Y"])
		bIsReset = true
	end
	
	if true == bIsReset or true == bIgnore then
		SetLock_ChatHistory( bPvPChat )
		local kChatOutWnd = ActivateUI("ChatOut")
		if false == kChatOutWnd:IsNil() then
			kChatOutWnd:Visible(true)
			
			local kChatBG = kChatOutWnd:GetControl("CHAT_BG")
			if false == kChatBG:IsNil() then
				kChatBG:Visible(true)
				local kButton = kChatBG:GetControl("BTN_DEEXPAND")
				if false == kButton:IsNil() then
					local kPos = kButton:GetLocation()
					kPos:SetX(kPos:GetX() + kMoveRange)
					kButton:SetLocation(kPos)				
					kButton:SetClose(bPvPChat)
				end
				kButton = kChatBG:GetControl("BTN_EXPAND")
				if false == kButton:IsNil() then
					local kPos = kButton:GetLocation()
					kPos:SetX(kPos:GetX() + kMoveRange)
					kButton:SetLocation(kPos)				
					kButton:SetClose(bPvPChat)
				end
					
				if 0 ~= kMoveRange then
					-- 크기 조정
					local kSize = kChatOutWnd:GetSize()
					kSize:SetX(kSize:GetX() + kMoveRange)
					kChatOutWnd:SetSize(kSize)
					
					kSize = kChatBG:GetSize()
					kSize:SetX(kSize:GetX() + kMoveRange)
					kChatBG:SetSize(kSize)
					
					local kSFRM_CHATOUT_BG = kChatBG:GetControl("SFRM_CHATOUT_BG")
					kSFRM_CHATOUT_BG:SetSize(kSize)
					
					kButton = kChatBG:GetControl("BTN_CALL_CHAT_DLG")
					if false==kButton:IsNil() then
						local kPos = kButton:GetLocation()
						kPos:SetX( kPos:GetX() + kMoveRange )
						kButton:SetLocation(kPos)
					end
				end
			end
			if true ~= bIgnore then		
				kChatOutWnd:SetLocation(kPos)
				OnClick_ChatMode_Change(iDefaultChatMode)
			end			
		end
	end
end

function CallChatMenu(self)
	if self:IsNil() then return end
	
	local kPos = self:GetTotalLocation()
	
	local kStatus = GetContentsStatus()
	local kMenu = CallUI("SFRM_TELL_TYPE")
	if false == kMenu:IsNil() then
		local kSize = kMenu:GetSize()
		kPos:SetY( kPos:GetY() - kSize:GetY() )
		kMenu:SetLocation(kPos)
	end
	
end
--//////////////////////////////////////////////////////////////////////////////////////////////////////
ColorSet = {}
--[1]Basic BG,[2]Light BG,[3]Dark BG,[4]Allow,[5]Line1,[6]Line2,[7]Quickslot highlight,[8]Basic Text,[9]Level Text,[10]QuickSolt Text,[11]InvenStatic,[12]DarkLine1,[13]DarkLine2
ColorSet[1] = {0xF7EA96,0xFBF0A7,0xE7D87B,0xFF9700,0xDDC96C,0xFCF4C2,0xFEF7CC,0x4D3413,0xF7EA96,0xFAE26D,0x61605C,0x17130E,0x504948}
ColorSet[2] = {0xFFC5C5,0xFFD4D4,0xF5AAAA,0xDC5C95,0xE69D9D,0xFFDDDD,0xFFE3E3,0x602424,0xFFC5C5,0xFFB3B3,0x61605C,0x17130E,0x504948}
ColorSet[3] = {0xCBF0F6,0xDFF7FB,0xAAE3ED,0x8669E2,0xA5D4DC,0xEBFCFF,0xE8FCFF,0x194047,0xCBF0F6,0xAEE5FA,0x61605C,0x17130E,0x504948}
ColorSet[4] = {0xF6F6F6,0xFFFFFF,0xE1E1E1,0x1BB3B1,0xDCDCDC,0xFFFFFF,0xFFFFFF,0x43403C,0xF6F6F6,0xEBEBEB,0x61605C,0x17130E,0x504948}
-- Add new Color by Feribell
ColorSet[5] = {0xe5ff94,0xcfe784,0xcfe784,0xFF9700,0xddfe77,0xddfe77,0xFEF7CC,0x4D3413,0xe5ff94,0xFAE26D,0x61605C,0x17130E,0x504948}
ColorSet[6] = {0xc2bdff,0x9c95f2,0x9c95f2,0xFF9700,0x9c95f2,0x9c95f2,0xFEF7CC,0x4D3413,0xc2bdff,0xFAE26D,0x61605C,0x17130E,0x504948}
ColorSet[7] = {0xff8383,0xdc6767,0xdc6767,0xFF9700,0xdc6767,0xdc6767,0xFEF7CC,0x4D3413,0xff8383,0xFAE26D,0x61605C,0x17130E,0x504948}
ColorSet[8] = {0xed99ff,0xd489e4,0xd489e4,0xFF9700,0xd489e4,0xd489e4,0xFEF7CC,0x4D3413,0xed99ff,0xFAE26D,0x61605C,0x17130E,0x504948}
ColorSet[9] = {0x7fce81,0x74bd75,0x74bd75,0xFF9700,0x74bd75,0x74bd75,0xFEF7CC,0x4D3413,0x7fce81,0xFAE26D,0x61605C,0x17130E,0x504948}
ColorSet[10] = {0xffbe21,0xe0a71d,0xe0a71d,0xFF9700,0xe0a71d,0xe0a71d,0xFEF7CC,0x4D3413,0xffbe21,0xFAE26D,0x61605C,0x17130E,0x504948}
ColorSet[11] = {0xaffeff,0x7bebec,0x7bebec,0xFF9700,0x7bebec,0x7bebec,0xFEF7CC,0x4D3413,0xaffeff,0xFAE26D,0x61605C,0x17130E,0x504948}
ColorSet[12] = {0xffb085,0xec8953,0xec8953,0xFF9700,0xec8953,0xec8953,0xFEF7CC,0x4D3413,0xffb085,0xFAE26D,0x61605C,0x17130E,0x504948}

local iCnt = 0
for kKey, kCur in pairs(ColorSet) do
	iCnt = iCnt + 1
end
ColorSet['Cnt'] = iCnt

local DefaultUIColorSet = 1
function UI_ColorSet(idx)
	--local iColorSetSize = 5
	
	if 1 > idx or ColorSet.Cnt < idx then
		idx = DefaultUIColorSet
	end
	
	for kKey, kVal in pairs(ColorSet[idx]) do
		RegUIColor(kKey, kVal)-- Idx, Color
	end
	--[[
	for i = iColorSetSize*idx + 1, iColorSetSize*idx + iColorSetSize do
		RegUIColor(i - iColorSetSize*idx, ColorSet[i])
	end
	]]--
	ChangeUIColorSet()--Accept Color Set
end

function UI_ChannelChanging( iTextTableNo, nChannel )
	local kChannelChangeUI = ActivateUI("FRM_CHANNEL_CHANGING", true)
	if false == kChannelChangeUI:IsNil() then
		kChannelChangeUI:SetStaticTextW( GetTextW(iTextTableNo) )
		local kAniBar = kChannelChangeUI:GetControl("BAR_TIMER")
		if false == kAniBar:IsNil() then
			kChannelChangeUI:SetCustomDataAsShort(nChannel)
			kAniBar:SetStartTime(10000) -- Max 10초
		end
	end
end

function UI_ChannelChangingClose( nChannelNo )
	if 0 == nChannelNo then
		ClearAutoLogin()
	elseif 0 < nChannelNo then
		g_ChatMgrClient:Notice_Show( WideString(string.format(GetTT(156):GetStr(),nChannelNo)), 1 )
	end
end

function CommonMsgBox(sz, bModal, ty)
	CloseToolTip()
	ODS("CommonMsgBox : " .. sz .. "\n")
	-- 공용으로 쓸 메세지 박스
	local wndmsg = CloneAutoCallUI("SFRM_MSG_COMMON", bModal)
	if wndmsg:IsNil() then
		return
	end
	
	local sdw = wndmsg:GetControl("SFRM_COLOR"):GetControl("SFR_SDW")
	if not sdw:IsNil() then
	
		sdw:SetStaticText(sz)
		local kTextPos = sdw:GetTextPos()
		if nil ~= ty then
			-- Default + Y
			kTextPos:SetY( 30 + ty )
		else
			-- Auto center height align
			local kTxtSize = sdw:GetTextSize()
			kTextPos:SetY( (75 - kTxtSize:GetY())/2 )
		end
		sdw:SetTextPos( kTextPos )
	end
	RegistUIAction(wndmsg, "CloseUI")
end

function CommonMsgBoxCancel(sz, kOKPacket, bModal, ty)
	CloseToolTip()
	ODS("CommonCancelMsgBox : " .. sz .. "\n")
	-- 공용으로 쓸 메세지 박스
	local wndmsg = CallUI("SFRM_MSG_COMMON_CANCEL", bModal)
	if wndmsg:IsNil() == false then
		if false == kOKPacket:Empty() then
			local kBtn = wndmsg:GetControl("BTN_OK")
			if false == kBtn:IsNil() then
				kBtn:SetCustomDataAsPacket(kOKPacket)
			end			
		end	
		
		local sdw = wndmsg:GetControl("SFRM_COLOR"):GetControl("SFR_SDW")
		if not sdw:IsNil() then
		
			sdw:SetStaticText(sz)
			local kTextPos = sdw:GetTextPos()
		    if nil ~= ty then
				-- Default + Y
				kTextPos:SetY( 30 + ty )
			else
				-- Auto center height align
				local kTxtSize = sdw:GetTextSize()
				kTextPos:SetY( (75 - kTxtSize:GetY())/2 )
		    end
			sdw:SetTextPos( kTextPos )
		end
	end
end

function QuitMsgBox_NC( kStr, bModal )
	CloseToolTip()
	local wndmsg = GetUIWnd("")
	if IsVisible("SFRM_MIDDLE_MSG_COMMON_QUIT") then
		wndmsg = GetUIWnd("SFRM_MIDDLE_MSG_COMMON_QUIT")
	else
		wndmsg = CallUI("SFRM_MIDDLE_MSG_COMMON_QUIT", bModal)
	end
	if not wndmsg:IsNil() then
		local sdw = wndmsg:GetControl("SFRM_COLOR"):GetControl("SFR_SDW")
		if not sdw:IsNil() then
			sdw:SetStaticText(kStr)

			local Size = sdw:GetSize()
			local kTextPos = sdw:GetTextPos()
			local kTextSize = sdw:GetTextSize()
			kTextPos:SetY((Size:GetY()-kTextSize:GetY())/2)
			sdw:SetTextPos(kTextPos)
		end
	end
end
-- 공용으로 쓰는 메세지 박스(텍스트 테이블 번호로
function CommonMsgBoxByTextTable( iSzNo, bModal )
	CloseToolTip()
	local wndmsg = GetUIWnd("")
	if IsVisible("SFRM_MSG_COMMON") then
		wndmsg = GetUIWnd("SFRM_MSG_COMMON")
	else
		wndmsg = CallUI("SFRM_MSG_COMMON", bModal)
	end
	if not wndmsg:IsNil() then
		local sdw = wndmsg:GetControl("SFRM_COLOR"):GetControl("SFR_SDW")
		if not sdw:IsNil() then
			sdw:SetStaticTextW(GetTextW(iSzNo))
			
			local Size = sdw:GetSize()
			local kTextPos = sdw:GetTextPos()
			local kTextSize = sdw:GetTextSize()
			kTextPos:SetY((Size:GetY()-kTextSize:GetY())/2)
			sdw:SetTextPos(kTextPos)
		end
	end
end

function CommonMsgBoxByTextTable( iSzNo, bModal, kMainUI )
	CloseToolTip()
	local wndmsg = GetUIWnd("")
	if IsVisible("SFRM_MSG_COMMON") then
		wndmsg = GetUIWnd("SFRM_MSG_COMMON")
	else
		wndmsg = CallUI("SFRM_MSG_COMMON", bModal)
	end
	if not wndmsg:IsNil() then
		local sdw = wndmsg:GetControl("SFRM_COLOR"):GetControl("SFR_SDW")
		if not sdw:IsNil() then
			sdw:SetStaticTextW(GetTextW(iSzNo))
			
			local Size = sdw:GetSize()
			local kTextPos = sdw:GetTextPos()
			local kTextSize = sdw:GetTextSize()
			kTextPos:SetY((Size:GetY()-kTextSize:GetY())/2)
			sdw:SetTextPos(kTextPos)
		end
	end
	
	if not wndmsg:IsNil() and nil ~= kMainUI and not kMainUI:IsNil() then
		local mainPos = kMainUI:GetLocation()
		local mainSize = kMainUI:GetSize()
		
		local wndPos = wndmsg:GetLocation()
		local wndSize = wndmsg:GetSize()
		
		wndPos:SetX( mainPos:GetX()+(mainSize:GetX()/2) - (wndSize:GetX()/2) )
		wndPos:SetY( mainPos:GetY()+(mainSize:GetY()/2) - (wndSize:GetY()/2) )
		
		wndmsg:SetLocation( wndPos )
	end
end

function CommonMsgBoxByText( kStr, bModal )
	CloseToolTip()
	local wndmsg = GetUIWnd("")
	if IsVisible("SFRM_MSG_COMMON") then
		wndmsg = GetUIWnd("SFRM_MSG_COMMON")
	else
		wndmsg = CallUI("SFRM_MSG_COMMON", bModal)
	end
	if not wndmsg:IsNil() then
		local sdw = wndmsg:GetControl("SFRM_COLOR"):GetControl("SFR_SDW")
		if not sdw:IsNil() then
			sdw:SetStaticTextW(kStr)
			
			local Size = sdw:GetSize()
			local kTextPos = sdw:GetTextPos()
			local kTextSize = sdw:GetTextSize()
			kTextPos:SetY((Size:GetY()-kTextSize:GetY())/2)
			sdw:SetTextPos(kTextPos)
		end
		return wndmsg
	end
	return nil
end

function CommonMiddleMsgBoxByText( kStr, bModal )
	CloseToolTip()
	local wndmsg = GetUIWnd("")
	if IsVisible("SFRM_MIDDLE_MSG_COMMON") then
		wndmsg = GetUIWnd("SFRM_MIDDLE_MSG_COMMON")
	else
		wndmsg = CallUI("SFRM_MIDDLE_MSG_COMMON", bModal)
	end
	if not wndmsg:IsNil() then
		local sdw = wndmsg:GetControl("SFRM_COLOR"):GetControl("SFR_SDW")
		if not sdw:IsNil() then
			sdw:SetStaticTextW(kStr)

			local Size = sdw:GetSize()
			local kTextPos = sdw:GetTextPos()
			local kTextSize = sdw:GetTextSize()
			kTextPos:SetY((Size:GetY()-kTextSize:GetY())/2)
			sdw:SetTextPos(kTextPos)
		end
	end
end

function CallLockMsg( kMsg, bLock )
	local wndMsg = ActivateUI( "SFRM_MSG_LOCK", true )
	if not wndMsg:IsNil() then
		local control = wndMsg:GetControl("SFRM_MEMO")
		if not control:IsNil() then
			control:SetStaticText( kMsg ) 
		end
		
		control = wndMsg:GetControl("BTN_OK")
		if not control:IsNil() then
			control:Visible( not bLock ) 
		end
	end
end

function MsgOkByText( kStr, kGuid, eBoxType )
	CloseToolTip()
	local wndmsg = ActivateUI("MSG_OK_BOX", true)
	if not wndmsg:IsNil() then
		local sdw = wndmsg:GetControl("FRM_MESSAGE")
		if not sdw:IsNil() then
			sdw:SetStaticTextW(kStr)

			local Size = sdw:GetSize()
			local kTextPos = sdw:GetTextPos()
			local kTextSize = sdw:GetTextSize()
			kTextPos:SetY((Size:GetY()-kTextSize:GetY())/2)
			sdw:SetTextPos(kTextPos)
		end
		wndmsg:MsgBoxOwnerGuid(kGuid)
		wndmsg:MsgBoxType(eBoxType)
		return wndmsg
	end
	return nil
end

function CallWorldMap( bBig )
	if	g_world:IsNil() then
	    AddWarnDataTT(665)
	    return
	end
	
	if	true == g_world:IsHaveWorldAttr(GATTR_FLAG_MYHOME) or
		true == g_world:IsHaveWorldAttr(GATTR_FLAG_CHAOS_F) then
	    AddWarnDataTT(665)
	    return
	end
	
	if true == bBig then
		local PvPRoomWnd = GetUIWnd("FRM_PVP_BACKGROUND")
		if false == PvPRoomWnd:IsNil() then
			if PvPRoomWnd:IsVisible() then
				AddWarnDataTT(98)
				return
			end
		end
		g_RoadSign["OpenMinimap"] = false
		if g_world:IsHaveWorldAttr(GATTR_BATTLESQUARE) then
			OnCallBattleSquareMiniMap()
		else
			OnCallCenterMiniMap(GUID(""))
		end

	else
		if g_world:IsHaveWorldAttr(GATTR_HOMETOWN_F) then
			OnCallTownMinimap()
		else
			CallNewWorldMap()
		end
	end
end

function IsOpenMinimap()
	return false==GetUIWnd("SFRM_BIG_MAP"):IsNil()
end
function OnKey_Post_Enter(parent, kName)
	if parent:IsNil() then
		return false
	end
	ClearEditFocus()
	local kEdt = parent:GetControl(kName)
	if false == kEdt:IsNil() then
		kEdt:SetEditFocus(true)
	end
	return true
end

function CommonNetBox( sz, kOKPacket, kCanclePacket )
	local kBox = ActivateUI("SFRM_MSG_COMMON_YESNO",true)
	if kBox:IsNil() == false then
		if false == kOKPacket:Empty() then
			local kBtn = kBox:GetControl("BTN_OK")
			if false == kBtn:IsNil() then
				kBtn:SetCustomDataAsPacket(kOKPacket)
			end
			
		end
		
		if false == kCanclePacket:Empty() then
			local kBtn = kBox:GetControl("BTN_CANCLE")
			if false == kBtn:IsNil() then
				kBtn:SetCustomDataAsPacket(kCanclePacket)
			end
		end
		
		local sdw = kBox:GetControl("SFRM_COLOR"):GetControl("SFR_SDW")
		if false == sdw:IsNil() then
			sdw:SetStaticText(sz)
			local kTextPos = sdw:GetTextPos()
		    if nil ~= ty then
				-- Default + Y
				kTextPos:SetY( 5 + ty )
			else
				-- Auto center height align
				local kTxtSize = sdw:GetTextSize()
				kTextPos:SetY( (sdw:GetSize():GetY() - kTxtSize:GetY())/2 )
		    end
			sdw:SetTextPos( kTextPos )
		end
		return true
	end
	return false
end

function CommonNetSummonPartyMemberBox( sz, kName, kOKPacket, kCanclePacket )	
	local kBox = ActivateUI("SFRM_MSG_SUMMONPARTYMEMBER_YESNO",true)
	if kBox:IsNil() == false then		
		kSummonPartyMemberName = kName
		if false == kOKPacket:Empty() then
			local kBtn = kBox:GetControl("BTN_OK")
			if false == kBtn:IsNil() then
				kBtn:SetCustomDataAsPacket(kOKPacket)
			end
			
		end
		
		if false == kCanclePacket:Empty() then
			local kBtn = kBox:GetControl("BTN_CANCLE")
			if false == kBtn:IsNil() then
				kBtn:SetCustomDataAsPacket(kCanclePacket)
			end
		end
		
		local sdw = kBox:GetControl("SFRM_COLOR1"):GetControl("SFR_SDW1")
		if false == sdw:IsNil() then
			sdw:SetStaticText(sz)
			--[[local kTextPos = sdw:GetTextPos()
		    if nil ~= ty then
				-- Default + Y
				kTextPos:SetY( 5 + ty )
			else
				-- Auto center height align
				local kTxtSize = sdw:GetTextSize()
				kTextPos:SetY( (sdw:GetSize():GetY() - kTxtSize:GetY())/2 )
		    end
			sdw:SetTextPos( kTextPos )--]]
		end
		return true
	end
	return false
end

function Net_SendUIPacket( kUI )
	local kPacket = kUI:GetCustomDataAsPacket()
	if false == kPacket:Empty() then
		Net_Send(kPacket)
		DeletePacket(kPacket)
		return true
	end
	return false
end

function UI_CallMovieScene(bIn, fTime)
	if 0>=fTime then
		fTime = 2
	end
	if bIn then
		CallUI("FRM_MOVIE_IN")
	else
		CallUI("FRM_MOVIE_OUT")
	end

	fMovieTime = fTime
end

function UI_CloseMovieScene(fTime)
	if 0>=fTime then
		fTime = 2
	end
	if false==GetUIWnd("FRM_MOVIE_IN"):IsNil() then
		CallUI("FRM_MOVIE_OUT")
		fMovieTime = fTime
	end
end

function UI_SHOPBTN_OnFocus(wnd, iIndex)
	if wnd:IsNil() or 0==iIndex then return end
	ChangeShopViewGroup(iIndex)
	for i=1,4 do 
		local btn = wnd:GetControl("CBTN_TAB"..i)
		if false == btn:IsNil() then
			btn:CheckState(i==iIndex)
			btn:LockClick(i==iIndex)
		end
	end

	local wndDrop = wnd:GetControl("SFRM_SHOP"):GetControl("SFRM_ITEM_BG"):GetControl("SFRM_SDW"):GetControl("FRM_FILTER"):GetControl("SFRM_DROP")
	if wndDrop:IsNil() == false then
		wndDrop:GetParent():SetSize(Point2(87, 24))
		wndDrop:Visible(false)
	end

	OnShopPageView(wnd:GetControl("SFRM_SHOP"):GetControl("SFRM_ITEM_BG"), 1 )
	ShopPageNumSetting(wnd:GetControl("SFRM_SHOP"):GetControl("SFRM_ITEM_BG") )
end

function UI_SHOPDROPBTN_OnFocus(wnd, szOrder, iText)
	if wnd:IsNil() or 0==iIndex then return end
	ChangeShopViewSubOrder(szOrder)
	wnd:GetParent():SetSize(Point2(87, 24))
	wnd:Visible(false)
	wnd:GetParent():GetControl("BTN_FILTER"):SetStaticTextW(GetTextW(iText))

	OnShopPageView(wnd:GetParent():GetParent():GetParent(), 1 )
	ShopPageNumSetting(wnd:GetParent():GetParent():GetParent())
end

function UI_Equip_RegScript(wnd, bOther)
	if wnd:IsNil() then return end
	wnd:RegistScript("ON_DISPLAY", "UISelf:DrawIcon()")
	wnd:RegistScript("ON_MOUSE_OVER", "UISelf:CallIconToolTip()")
	wnd:RegistScript("ON_MOUSE_OUT", "CloseToolTip()")
	if bOther then
		return
	end
	wnd:RegistScript("ON_L_BTN_DOWN", "UISelf:IconToCursor()")
	wnd:RegistScript("ON_L_BTN_UP", "UISelf:CursorToIcon()")
end

--[[
function CallEnergyGaugeBig(kActor)
	if kActor:IsNil() then return end

	local strUI = ""
	local iGrade = kActor:GetAbil(AT_GRADE)
	if iGrade == EMGRADE_ELITE then
		strUI = "FRM_MAIN_GAUGEBAR"
	elseif iGrade == EMGRADE_BOSS then
		strUI = "FRM_BOSS_GAUGEBAR"
	end

	CallUI(strUI)
end
]]
function WhenTimeHasComeThenClose(kSelf)
	local Time = kSelf:GetCustomDataAsInt()
	ODS(Time.."\n")
	kSelf:SetCustomDataAsInt(Time-1)
	if(0 > Time) then
		kSelf:Close()
	end
end

function SetReadyScaleEffect(kUI)
-- 임시적인 pvp 레디 UI 이펙트 최초 호출 
	if(nil == kUI)  		then  return end
	if(true == kUI:IsNil()) then  return end				
	if(not kUI:IsVisible()) then  return end
	kUI:SetSizeScale(4, false)
	local Delta = 0.5;
	kUI:SetCustomDataAsFloat(-Delta)
	kUI:SetMaxAlpha(Delta)
end

function ProcessReadyScaleEffect(kUI)
-- 임시적인 pvp 레디 UI 이펙트
	if(nil == kUI)  		then  return end
	if(true == kUI:IsNil()) then  return end				
	if(not kUI:IsVisible()) then  return end
	local FixSize = 1.2
	if(kUI:GetSizeScale() <= 1) then
		kUI:SetSizeScale(FixSize,false)
		kUI:SetMaxAlpha(1)
		PlaySoundByID("Button_READY")
		ODS("들어오고 있음\n")
		return
	end
	
	if(kUI:GetSizeScale() == FixSize) then 
		ODS("걸림\n")
		return 
	end
	
	local Delta = kUI:GetCustomDataAsFloat()
	kUI:SetSizeScale(kUI:GetSizeScale()+Delta, false)
	kUI:SetMaxAlpha(kUI:GetMaxAlpha()+0.05)	
	
	if(kUI:GetSizeScale() <= 1) then
		kUI:SetCustomDataAsInt(1)
	end
end

function SetUISteer(kUI
			, FrameTime
			, BeginX,BeginY
			, EndX,EndY
			, Spd,Accel,MaxSpd
			, BeginScale,EndScale, IncScale
			, BeginAlpha,EndAlpha, IncAlpha
			, bBlink
			)
	if nil == kUI then return end
	if true == kUI:IsNil() then return end	
	kUI:ClearCustomData()
	BeginX = math.floor(BeginX)
	BeginY = math.floor(BeginY)
	EndX = math.floor(EndX)
	EndY = math.floor(EndY)
	
	local kPacket = NewPacket(0);	
	-- 저장할 프레임 시간
	kPacket:PushFloat(FrameTime)
	
	--최초 위치
	kPacket:PushInt(BeginX) kPacket:PushInt(BeginY)
	--목적 위치	
	kPacket:PushInt(EndX) kPacket:PushInt(EndY)

	--현재 속력
	kPacket:PushFloat(Spd)
	--가속력
	kPacket:PushFloat(Accel)
	--최대속력
	kPacket:PushFloat(MaxSpd)
	
	--방향
	local DirX, DirY
	DirX, DirY = Vec2MakeDirection(BeginX,BeginY, EndX,EndY)
	kPacket:PushFloat(DirX)
	kPacket:PushFloat(DirY)
	
	--최초 스케일	
	kPacket:PushFloat(BeginScale)
	--목적 스케일	
	kPacket:PushFloat(EndScale)
	--스케일 증감값
	IncScale = ModifyIncVal(BeginScale, EndScale, IncScale)
	kPacket:PushFloat(IncScale)

	
	--최초 알파
	if(0 > BeginAlpha) then
		BeginAlpha = 0
	elseif(1 < BeginAlpha) then 
		BeginAlpha = 1
	end
	kPacket:PushFloat(BeginAlpha)
	--목적 알파	--ODS("Before EndAlpha :"..EndAlpha.."\n")
	if(0 > EndAlpha) then
		EndAlpha = 0
	elseif(1 < EndAlpha) then 
		EndAlpha = 1
	end
	kPacket:PushFloat(EndAlpha)	
	--알파 증감값	--	ODS("Before IncAlpha :"..IncAlpha.."\n")--	ODS("EndAlpha :"..EndAlpha.."\n")
	IncAlpha = ModifyIncVal(BeginAlpha, EndAlpha, IncAlpha)
	kPacket:PushFloat(IncAlpha)--	ODS("End IncAlpha :"..IncAlpha.."\n")
	-- 깜빡임
	kPacket:PushBool(bBlink)

	kUI:SetCustomDataAsPacket(kPacket)
	
	--하위는 테스트. 지울것
	local CurPos = Point2(BeginX, BeginY)
	kUI:SetLocation(CurPos)
	kUI:SetSizeScale(BeginScale)
	if(true == bBlink) then 
		kUI:SetMaxAlpha(0)
	else
		kUI:SetMaxAlpha(1)
		kUI:SetAlpha(BeginAlpha)
	end
	DeletePacket(kPacket)
end
--AccTime =0
function SteeringUI_OnTick(kUI)	
	if nil == kUI then return end
	if true == kUI:IsNil() then return end	
		
	local kPacket = kUI:GetCustomDataAsPacket()	
	if nil == kPacket then return end
	if true == kPacket:IsNil() then return end
	
	-- 실행된 시간 계산
	local AccExecutedTime = kPacket:PopFloat()
	AccExecutedTime = AccExecutedTime+GetFrameTime()
	
	--최초 위치
	local BeginX = kPacket:PopInt()	
	local BeginY = kPacket:PopInt()
	--목적 위치	
	local EndX = kPacket:PopInt() 
	local EndY = kPacket:PopInt()	
	
	--현재 속력
	local Spd = kPacket:PopFloat()
	--가속력
	local Accel = kPacket:PopFloat()	
	--최대속력
	local MaxSpd = kPacket:PopFloat()
		
	--방향
	local DirX = kPacket:PopFloat()
	local DirY = kPacket:PopFloat()	
	
	--최초 스케일
	local BeginScale = kPacket:PopFloat()	
	--목적 스케일	
	local EndScale = kPacket:PopFloat()
	--스케일 증감
	local IncScale = kPacket:PopFloat()	
	
	--최초 알파
	local BeginAlpha = kPacket:PopFloat()
	--목적 알파
	local EndAlpha = kPacket:PopFloat()	
	--증가 알파 
	local IncAlpha = kPacket:PopFloat()
	--깜빡임
	local bBlink = kPacket:PopBool()
	

	
--	local CurPos = kUI:GetLocation() -- SetLocation
--	local CurScale = kUI:GetSizeScale() -- SetSizeScale
	--CurAlpha -- SetAlpha
	
	--증가할 속도를 구함
--	AccTime  = AccTime+GetFrameTime()
--	ODS("FrameTime: "..GetFrameTime().."\n")
--	ODS("TempAccTime: "..AccTime.."\n")
	Spd = Spd+(Accel*GetFrameTime())
	if(Spd > MaxSpd) then Spd = MaxSpd end

	-- 현재 속력 보다 거리가 작다면, 현재 위치는 목표위치가 됨
	local Dist = Vec2DistToVec2(BeginX,BeginY, EndX,EndY)
	if(BeginX ~= EndX or BeginY ~= EndY) then
		if Spd > Dist then
			--CurPos:SetX(EndX)	CurPos:SetY(EndY)
			BeginX = EndX
			BeginY = EndY
		else
			local VelX = DirX*Spd
			local VelY = DirY*Spd
			--CurPos:SetX(CurPos:GetX() + VelX)			CurPos:SetY(CurPos:GetY() + VelY)
			BeginX = BeginX + VelX
			BeginY = BeginY + VelY
		end
	end 
	
	--Alpha	
	if(EndAlpha - BeginAlpha < 0) then 		
		BeginAlpha = BeginAlpha+(IncAlpha*GetFrameTime())	
		if(EndAlpha > BeginAlpha) then
			BeginAlpha = EndAlpha
		end
	else
		BeginAlpha = BeginAlpha+(IncAlpha*GetFrameTime())	
		if(EndAlpha < BeginAlpha) then
			BeginAlpha = EndAlpha
		end
	end
	--ODS("BeginAlpha :"..BeginAlpha.."\n")
	
	--Scale	
	if(EndScale - BeginScale < 0) then
		BeginScale = BeginScale+(IncScale*GetFrameTime())
		if(EndScale > BeginScale) then
			BeginScale = EndScale
		end
	elseif(EndScale - BeginScale > 0) then
		BeginScale = BeginScale+(IncScale*GetFrameTime())
		if(EndScale < BeginScale) then
			BeginScale = EndScale
		end
	end
	
	SetUISteer(kUI
			, AccExecutedTime
			, BeginX, BeginY
			, EndX,EndY
			, Spd,Accel,MaxSpd
			, BeginScale,EndScale,IncScale
			, BeginAlpha,EndAlpha,IncAlpha
			)
	return AccExecutedTime
end

function Vec2Length(x,y)
	local DblX = x*x
	local DblY = y*y
	return math.sqrt(DblX+DblY)
end

function Vec2DistToVec2(FromX, FromY, ToX, ToY)
	local x = ToX - FromX
	local y = ToY - FromY
	return Vec2Length(x,y)
end

function Vec2Nomalize(x, y)
	local dblX = x*x
	local dblY = y*y
	local VectorLength =  math.sqrt(dblX+dblY)
	local nomalX = x/VectorLength
	local nomalY = y/VectorLength
	return nomalX, nomalY
end

function Vec2MakeDirection(FromX,FromY, ToX,ToY)
	local DirX = ToX - FromX
	local DirY = ToY - FromY
	return Vec2Nomalize(DirX, DirY)
end

function ModifyIncVal(Begin, End, IncVal)
	if( End ~= Begin) then
	--ODS("분기1\n")
		if( End - Begin < 0) then
			--ODS("분기2\n")
			if(IncVal > 0) then
			--ODS("분기3\n")
				IncVal = IncVal*-1
			end
		else
		--ODS("분기4\n")
			if(IncVal < 0) then 		
	--		ODS("분기5\n")
				IncVal = IncVal*-1
			end
		end
	else
--	ODS("분기6\n")
		IncVal = 0
	end
	return IncVal
end

function OnStockShopPageButton(wnd, iValue)
	if wnd:IsNil() or 0==iValue then
		return
	end
	local wndText = wnd:GetControl("FRM_TEXT")
	if false==wndText:IsNil() then
		local iData = wndText:GetCustomDataAsInt()
		if 0<iValue then
			if iData>=2 then
				return
			end
		else
			if 0>=iData then
				wndText:SetStaticText("1 ~ "..(iData+1)*30)
				return
			end
		end
		iData = iData + iValue
		wndText:SetCustomDataAsInt(iData)			
		local iMin = iData*30 + 1
		wndText:SetStaticText(iMin.." ~ "..(iData+1)*30)
		local bgicon = wnd:GetControl("SFRM_ITEM_BG"):GetControl("SFRM_SDW"):GetControl("FRM_ICON")
		if false==bgicon:IsNil() then
			local iTotal = 29
			local wndBld = bgicon:GetControl("BLD_ICON")
			if false==wndBld:IsNil() then
				local ptCount = wndBld:GetBuildCount()
				iTotal = ptCount:GetX()*ptCount:GetY()
			end
			for i=0,iTotal do
				local wIcon = bgicon:GetControl("InvIcon"..i)
				if false==wIcon:IsNil() then
					wIcon:SetIconKey(wIcon:GetBuildIndex()+iMin-1)
				end
			end
		end
	end
end

function ShopPageNumSetting(wnd)
	if wnd:IsNil() then
		return
	end
	
	local wndPageNum = wnd:GetControl("FRM_PAGE_NUM")
	if wndPageNum:IsNil() then
		return
	end
	
	local iMaxGoods = GetShopGoodsViewSize()
	local iMaxPageDivide = iMaxGoods / 36
	local iMaxPageResidual = iMaxPageDivide % 1
	local iMaxPage = iMaxPageDivide - iMaxPageResidual
	if 0 < iMaxPageResidual then
		iMaxPage = iMaxPage + 1
	end
	
	if 0 == iMaxPage then
		iMaxPage = 1
	end
	local iCurPage = wndPageNum:GetCustomDataAsInt()
	
	if iCurPage > iMaxPage then
		iCurPage = iMaxPage
	end
	-- Only Use JobSkillShop---
	local UIShop = wnd:GetParent()
	if false == UIShop:IsNil() then
		local CheckBtn_AlreadyLearn = UIShop:GetControl("CBTN_FILTER_ALREADY_LEARN")
		if false == CheckBtn_AlreadyLearn:IsNil() then
			if true == CheckBtn_AlreadyLearn:GetCheckState() then
				LearnFilter_JobSkillShop(true, iPage)
				ODS("Enter LearnFilter_JobSkillShop", false, 912)
				return;
			end
			ODS("CheckBtn_AlreadyLearn is false", false, 912)
		end
		ODS("CheckBtn_AlreadyLearn is null", false, 912)
	end
	---------------------------
	OnShopPageView(wnd, iCurPage)
	wndPageNum:SetStaticText(""..iCurPage.." / "..iMaxPage)
end

function OnShopPageView(wnd, iPage)

	if wnd:IsNil() or 0 >= iPage then
		return
	end
	
	local wndPageNum = wnd:GetControl("FRM_PAGE_NUM")
	if wndPageNum:IsNil() then
		return
	end
	
	local iMaxGoods = GetShopGoodsViewSize()
	local iMaxPageDivide = iMaxGoods / 36
	local iMaxPageResidual = iMaxPageDivide % 1
	local iMaxPage = iMaxPageDivide - iMaxPageResidual
	if 0 < iMaxPageResidual then
		iMaxPage = iMaxPageDivide + 1
	end
	if 1 > iMaxPage then
		iMaxPage = 1
	end
	if iPage > iMaxPage then
		iPage = iMaxPage
	end
	
	local iMin = (iPage -1) * 36
	local wndIcon = wnd:GetControl("SFRM_SDW"):GetControl("FRM_ICON")
	if wndIcon:IsNil() then
		return
	end
	
	local iTotal = 36
	local wndBldIcon = wndIcon:GetControl("BLD_ICON")
	if wndBldIcon:IsNil() == false then
		local ptCount = wndBldIcon:GetBuildCount()
		iTotal = ptCount:GetX() * ptCount:GetY()
	end
	
	for i=0, iTotal-1 do
		local wIcon = wndIcon:GetControl("InvIcon"..i)
		if false == wIcon:IsNil() then
			wIcon:SetIconKey(wIcon:GetBuildIndex()+iMin)
			
		end
	end
	wndPageNum:SetCustomDataAsInt(iPage)
end

function OnShopPageBack(wnd)
	if wnd:IsNil() then
		return
	end
	
	local wndPageNum = wnd:GetControl("FRM_PAGE_NUM")
	if wndPageNum:IsNil() then
		return
	end

	local iCurPage = wndPageNum:GetCustomDataAsInt()
	OnShopPageView(wnd, iCurPage - 1)
end

function OnShopPageNext(wnd)
	if wnd:IsNil() then
		return
	end
	
	local wndPageNum = wnd:GetControl("FRM_PAGE_NUM")
	if wndPageNum:IsNil() then
		return
	end

	local iCurPage = wndPageNum:GetCustomDataAsInt()
	OnShopPageView(wnd, iCurPage + 1)
end

function OnDisplayFireLove(kWnd)
	local kParent = kWnd:GetParent()
	local AliveTime = kParent:GetAliveTime()
	local fExecutedTime = GetWorldAccumTime() - kWnd:GetCustomDataAsFloat()
	fExecutedTime = fExecutedTime*1000
	local fRatio = fExecutedTime / AliveTime
	--ODS(fExecutedTime.."/"..AliveTime.."\n")
	
	fRatio = fRatio*2
	if(fRatio > 1) then
		fRatio = 1+(1-fRatio)		
	end	
	ODS("비율:"..fRatio.."\n")	
	if( fRatio <= 0 ) then 
		fRatio = 0		
		--kWnd:Visible(false)
	elseif(fRatio >=0.5 ) then
		fRatio = 0.5
		--kWnd:Visible(true)
	else
		--kWnd:Visible(true)
	end
	kWnd:SetAlpha(fRatio)	
	--ODS("alpha"..kWnd:GetAlpha().."\n")	
--[[
	if( fExecutedTime > 2500 and false == TempSwitch) then
		TempSwitch  = true
		kWnd:SetCustomDataAsFloat(GetWorldAccumTime())
	elseif(false == TempSwitch) then
		kWnd:SetAlpha(0.00001)
		return
	end
]]
	--[[
	
	fRatio = fRatio*2
	if(fRatio > 1) then
		fRatio = 1+(1-fRatio)		
	end
	--ODS("비율:"..fRatio.."\n")	
	if( fRatio <= 0 ) then 
		fRatio = 0		
		kWnd:Visible(false)
	elseif(fRatio >=0.5 ) then
		fRatio = 0.5
	end
	kWnd:SetAlpha(fRatio)
	]]
end

function PilotDieCloseUI()
	--UseItemCustomType() 참고
	--Recv_PT_M_C_NFY_ITEM_CHANGE() 참고
	CloseUI("SFRM_ITEM_MAKING_COOK") 	--요리하기
	CloseUI("SFRM_TELEPORT") 			--순간이동카드
	CloseUI("SFRM_SEAL_PROCESS") 		--봉인 주문서
	CloseUI("SFRM_FIRE_OF_LOVE_SMS") 	--사랑의 불꽃
	CloseUI("SFRM_LUCKY_CHANGER") 		--크레프트 행운권
	CloseUI("SFRM_SMS") 				--확성기
	CloseUI("FRM_EVT_SMS")				--사랑의 카드
	CloseUI("SFRM_GAMBLE") 				--가차
	CloseUI("SFRM_INV_SAFE")			--창고카드
end

g_bNoticeRestExp = false
function NoticeRestExp(bShow)
	g_bNoticeRestExp = bShow
end

function ReCalcUIPos(iVal, bWidth)
	local iResult = 0
	local fRate = 0	
	
	local iScreenCurSize =0
	local iScreenOrignSize = 0
	if(bWidth) then
		 iScreenCurSize = GetScreenSize():GetX()
		 iScreenOrignSize = 1024
	else
		iScreenCurSize = GetScreenSize():GetY()
		iScreenOrignSize = 768
	end
	fRate = iVal/iScreenOrignSize
	iResult = math.floor(iScreenCurSize*fRate) -- math.floor() -소수점 자리 버림 / math.ceil() -소수점 자리 올림
	return iResult
end

function IsAllShowUI()
	if false==GetUIWnd("FRM_MOVIE_IN"):IsNil() then return false end
	if false==GetUIWnd("FRM_MOVIE_OUT"):IsNil() then return false end
	if false==GetUIWnd("FRM_MISSION_STAGE_START"):IsNil() then return false end
	if false==GetUIWnd("FRM_CONSTELLATION_START"):IsNil() then return false end

	return true
end

function AlignUICenterR(wnd, x, y, add)
	if nil==wnd or wnd:IsNil() then
		return
	end
	
	if nil==add then add = 1 end

	local hSize = wnd:GetSize():GetY()*0.5
	local ypos = y/768*GetScreenSize():GetY() + (GetScreenSize():GetY() - 768)/2
	ypos = ypos * add
	local xpos = x/1024*GetScreenSize():GetX()
	
	wnd:SetLocation(Point2(xpos, ypos))
end

function AlignUICenterL(wnd, x, y, add)
	if nil==wnd or wnd:IsNil() then
		return
	end

	if nil==add then add = 1 end

	local hSize = wnd:GetSize():GetY()*0.5
	local ypos = y/768*GetScreenSize():GetY() + (GetScreenSize():GetY() - 768)/2
	ypos = ypos * add
	local xpos = GetScreenSize():GetX()/2 - (512-x)
	
	wnd:SetLocation(Point2(xpos, ypos))
end

--기준폼 왼쪽/오른쪽에 폼을 일정위치만큼 떨어져서 붙임
--이때 기준폼과 벽(해상도크기)에 공간이 없어 폼을 표시할 수 없으면 그 반대편에 출력됨
--예)기준폼과 오른쪽벽에 공간이 없으면 기준폼 왼쪽에 폼 출력
function DoUIAutoPosition(kSelf, kSrcForm, iSpace, bRight)
	if kSelf:IsNil() or nil==kSrcForm then
		return
	end
	kSrcForm = GetUIWnd(kSrcForm)
	if kSrcForm:IsNil() then
		return
	end
	
	if nil==iSpace then
		iSpace = 0
	end

	local kLoc = kSrcForm:GetLocation()
	local iSizeX = kSelf:GetSize():GetX()
	local iLeft = kLoc:GetX() - iSizeX - iSpace
	local iRight = kLoc:GetX() + kSrcForm:GetSize():GetX() + iSpace
	local iPosX = iRight
	if nil==bRight or bRight then
		iPosX = iRight
		if iRight+iSizeX > GetScreenSize():GetX() then
			iPosX = iLeft
		end
	else
		iPosX = iLeft
		if iLeft < 0 then
			iPosX = iRight
		end
	end
	
	kLoc:SetX(iPosX)
	kSelf:SetLocation(kLoc)
end

function ClassPromotionUIOnCall(kSelf)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end	
	
	local kNewPacket = NewPacket()
	kNewPacket:PushFloat( GetAccumTime() )
	kSelf:SetCustomDataAsPacket(kNewPacket) -- UI 호출 시간
	DeletePacket( kNewPacket )
	
	local kMain = kSelf:GetControl("FRM_MAIN")
	if( true == kMain:IsNil() ) then  return end
	
	local kMyActor = GetMyActor()
	if( true == kMyActor:IsNil() ) then return end
	local iClass = kMyActor:GetAbil(AT_CLASS)
	
	local kResult = GetTextW(401900)
	if( kResult:IsNil() ) then return end
	
	local iTT = 30000 + iClass
	local kClassName = GetTextW(iTT)
	if( kClassName:IsNil() ) then return end
	
	kResult:Add( kClassName:GetStr() )
	kMain:SetStaticTextW( kResult )
end

function ClassPromotionUIOnTick(kSelf)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end		
	local kMain = kSelf:GetControl("FRM_MAIN")
	if( true == kMain:IsNil() ) then  return end	
	local kWing = kSelf:GetControl("FRM_WING")
	if( true == kWing:IsNil() ) then  return end
	
	local fTargetTime = 0.15 	-- Scale 변환을 몇초후에 완료 할 것인가?
	--local fAdjustScale = 1.5 	-- 임시 변수
	
	local kPacket = kSelf:GetCustomDataAsPacket()
	local fCallTime = kPacket:PopFloat()					-- UI 호출 시간을 얻어와
	local fElapsed = GetAccumTime() - fCallTime 	-- 얼마만큼 시간이 흘렀는지 파악한 후	
	
	--메인 UI  Scale 설정
	local fMainScaleBegin = 1.066666666666667
	local fMainScaleEnd = 0.8666666666666667
		
	local fPer = fElapsed/fTargetTime					-- MainUI Scale 보간이 몇 %나 완료 되었는지 계산해서
	if(1 < fPer) then fPer = 1 end	
	
	local fScale = fMainScaleEnd - fMainScaleBegin
	fScale = (fScale * fPer) + fMainScaleBegin	-- 지금 시간에 적용할 MainUI의 Scale을 구한 후
	kMain:SetScale(fScale)								-- MainUI에 세팅하고
	
	--날개 UI Scale 설정
	local fWingScaleBegin = 0.3333333333333333
	local fWingScaleEnd = 0.9333333333333333
	
	fScale = fWingScaleEnd - fWingScaleBegin			
	fScale = (fScale * fPer) + fWingScaleBegin			-- 지금 시간에 적용할 WingUI의 Scale을 구한 후	
	kWing:SetScale(fScale)										-- WingUI에 세팅하고
	
	-- Packet을 이용한 Data 저장
	local kNewPacket = NewPacket()
	kNewPacket:PushFloat( fCallTime )
	kSelf:SetCustomDataAsPacket(kNewPacket) -- UI 호출 시간을 다음틱에 쓸수 있게 다시 저장
	DeletePacket( kNewPacket )
end

function MsgBoxOk(kSelf)
	if(nil == kSelf) then return end
	local kMsgType = kSelf:GetParent():GetMsgBoxType()
	ODS("MsgBox OK, Type: " .. kMsgType)
	if kMsgType == MBT_CREATE_DRAKAN_CHAR then
		CreateCharacter(true)
		return
	end

	kSelf:OnOk()
	return
end

function MsgBoxCancel(kSelf)
	if(nil == kSelf) then return end
	local kMsgType = kSelf:GetParent():GetMsgBoxType()
	ODS("MsgBox Cancel: " .. kMsgType)
	if kMsgType == MBT_CREATE_DRAKAN_CHAR then
		return
	end

	kSelf:OnCancel()
	return
end