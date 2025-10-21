function OnClick_QuestInfoButton(kSelf)
	if nil == kSelf or kSelf:IsNil() then return end
	local kParent = kSelf:GetParent()
	local iCustomInt = kParent:GetCustomDataAsInt()
	local kTopWnd = kParent:GetParent()
	OnItemSelected_From_QuestInfo(kTopWnd, iCustomInt);
end

function OnClick_WantedQuestButton(kSelf)
	if nil == kSelf or kSelf:IsNil() then return end
	local kParent = kSelf:GetParent()
	local iCustomInt = kSelf:GetCustomDataAsInt()
	OnItemSelected_From_QuestInfo(kParent, iCustomInt);
end

function QuestNextDialog(kTop)
	local kTemp = kTop:GetControl("FRM_MAIN")
	if kTemp:IsNil() then return false end
	kTemp = kTemp:GetControl("FRM_TEXT_BG")
	if kTemp:IsNil() then return false end
	kTemp = kTemp:GetControl("FRM_TEXT")	
	if kTemp:IsNil() then return false end
	if kTemp:IsFinishedTextDraw() then	
		local bClose = GetQuestMan():NextQuestDialog(kTop)
		if bClose then
			kTop:Close()
		end
	else
		kTemp:SkipTDrawInterval()
	end
	return true
end

function SCREEN_SIZE_SetPosCenterX(kWnd, iAddX)
	if not kWnd:IsNil() then
		local kScreenSize = GetScreenSize()
		local kPos = kWnd:GetLocation()
		kPos:SetX( (kScreenSize:GetX() - kWnd:GetSize():GetX() ) / 2 + iAddX )
		kWnd:SetLocation( kPos )
	end
end
function SCREEN_SIZE_SetPosCenterY(kWnd, iAddY)
	if not kWnd:IsNil() then
		local kScreenSize = GetScreenSize()
		local kPos = kWnd:GetLocation()
		kPos:SetY( (kScreenSize:GetY() - kWnd:GetSize():GetY() ) / 2 + iAddY )
		kWnd:SetLocation( kPos )
	end
end
function SCREEN_SIZE_SetPosX(kWnd, iAddX)
	if not kWnd:IsNil() then
		local kScreenSize = GetScreenSize()
		local kPos = kWnd:GetLocation()
		kPos:SetX( kScreenSize:GetX() +  iAddX )
		kWnd:SetLocation( kPos )
	end
end
function SCREEN_SIZE_SetPosY(kWnd, iAddY)
	if not kWnd:IsNil() then
		local kScreenSize = GetScreenSize()
		local kPos = kWnd:GetLocation()
		kPos:SetY( kScreenSize:GetY() +  iAddY )
		kWnd:SetLocation( kPos )
	end
end
function SCREEN_SIZE_SetPosXY(kWnd, iAddX, iAddY)
	if not kWnd:IsNil() then
		local kScreenSize = GetScreenSize()
		local kPos = kWnd:GetLocation()
		kPos:SetX( kScreenSize:GetX() +  iAddX )
		kPos:SetY( kScreenSize:GetY() +  iAddY )
		kWnd:SetLocation( kPos )
	end
end
function SCREEN_SIZE_SetSize(kWnd)
	if not kWnd:IsNil() then
		local kScreenSize = GetScreenSize()
		kWnd:SetSize( kScreenSize )
	end
end
function SCREEN_SIZE_SetSizeX(kWnd)
	if not kWnd:IsNil() then
		local kScreenSize = GetScreenSize()
		local kSize = kWnd:GetSize()
		kSize:SetX( kScreenSize:GetX() )
		kWnd:SetSize( kSize )
	end
end
function SCREEN_SIZE_SetSizeY(kWnd)
	if not kWnd:IsNil() then
		local kScreenSize = GetScreenSize()
		local kSize = kWnd:GetSize()
		kSize:SetY( kScreenSize:GetY() )
		kWnd:SetSize( kSize )
	end
end
function SCREEN_SIZE_SetImgSize(kWnd)
	if not kWnd:IsNil() then
		local kScreenSize = GetScreenSize()
		kWnd:SetImgSize( kScreenSize )
	end
end
function SCREEN_SIZE_SetImgSizeX(kWnd)
	if not kWnd:IsNil() then
		local kScreenSize = GetScreenSize()
		local kSize = kWnd:GetImgSize()
		kSize:SetX( kScreenSize:GetX() )
		kWnd:SetImgSize( kSize )
	end
end

function OnCall_QuestFull_IMG_UPPER(kTop)
	local kScreenSize = GetScreenSize()
	fMovieStartTime = GetAccumTime() 
	local kSize = kTop:GetImgSize()
	kSize:SetY(1)
	kSize:SetX( kScreenSize:GetX() )
	kTop:SetImgSize( kSize )
end
function OnDisplay_QuestFull_IMG_UPPER(kTop)
	local kScreenSize = GetScreenSize()
	local kSize = kTop:GetImgSize()
	local fw = (GetAccumTime() - fMovieStartTime)/0.4*60
	if fw > 60 then
		kSize:SetY(60)
		kTop:SetImgSize( kSize )
		return
	end
	kSize:SetY(fw)
	kTop:SetImgSize( kSize )
end
function OnCall_QuestFull_IMG_LOWER(kTop)
	local kScreenSize = GetScreenSize()
	fMovieStartTime1 = GetAccumTime() 
	local kSize = kTop:GetImgSize()
	kSize:SetY(1)
	kSize:SetX( kScreenSize:GetX() )
	kTop:SetImgSize( kSize )
	local kPos = kTop:GetLocation()
	kPos:SetY( kScreenSize:GetY() )
	kTop:SetLocation( kPos )
	
	bUpOK = false
	kTop:GetParent():GetControl("FRM_QUEST_REQ"):Visible(false)
	kTop:GetParent():GetControl("IMG_TITLE"):Visible(false)
	kTop:GetParent():GetControl("FRM_MAIN"):Visible(false)
	kTop:GetParent():GetControl("IMG_TITLE_BG_NORMAL"):Visible(false)
	kTop:GetParent():GetControl("FRM_BG_SET_NORMAL"):Visible(false)
	kTop:GetParent():GetControl("FRM_3DMODEL_NORMAL"):Visible(false)
	kTop:GetParent():GetControl("IMG_TITLE_BG_SCENARIO"):Visible(false)
	kTop:GetParent():GetControl("FRM_BG_SET_SCENARIO"):Visible(false)
	kTop:GetParent():GetControl("FRM_3DMODEL_SCENARIO"):Visible(false)

	kTop:GetParent():GetControl("IMG_LEFT"):Visible(false)
	kTop:GetParent():GetControl("IMG_RIGHT"):Visible(false)
end
function OnDisplay_QuestFull_IMG_LOWER(kTop)
	local iTargetSize = 60
	local kScreenSize = GetScreenSize()
	local kSize = kTop:GetImgSize()
	local fw = (GetAccumTime() - fMovieStartTime1)/0.3*iTargetSize
	if fw > iTargetSize then
		kSize:SetY(iTargetSize)
		kTop:SetImgSize( kSize )
		local kPos = kTop:GetLocation()
		kPos:SetY(kScreenSize:GetY()-iTargetSize)
		kTop:SetLocation( kPos )
		
		if bUpOK == false then
			local bool bScenario = g_kFullScreenUIShow["SCENARIO"]
			kTop:GetParent():GetControl("FRM_QUEST_REQ"):Visible( g_kFullScreenUIShow["INFO"] )
			kTop:GetParent():GetControl("IMG_TITLE"):Visible( g_kFullScreenUIShow["TITLE"] )
			kTop:GetParent():GetControl("FRM_MAIN"):Visible( g_kFullScreenUIShow["CONTENTS"] )
			kTop:GetParent():GetControl("IMG_TITLE_BG_NORMAL"):Visible( g_kFullScreenUIShow["TITLE"] and not bScenario )
			kTop:GetParent():GetControl("FRM_BG_SET_NORMAL"):Visible( g_kFullScreenUIShow["CONTENTS"] and not bScenario )
			kTop:GetParent():GetControl("FRM_3DMODEL_NORMAL"):Visible( g_kFullScreenUIShow["FACE"] and not bScenario )
			kTop:GetParent():GetControl("IMG_TITLE_BG_SCENARIO"):Visible( g_kFullScreenUIShow["TITLE"] and bScenario )
			kTop:GetParent():GetControl("FRM_BG_SET_SCENARIO"):Visible( g_kFullScreenUIShow["CONTENTS"] and bScenario )
			kTop:GetParent():GetControl("IMG_LEFT"):Visible( g_kFullScreenUIShow["CONTENTS"] and bScenario )
			kTop:GetParent():GetControl("IMG_RIGHT"):Visible( g_kFullScreenUIShow["CONTENTS"] and bScenario )
			kTop:GetParent():GetControl("FRM_3DMODEL_SCENARIO"):Visible( g_kFullScreenUIShow["FACE"] and bScenario )
			bUpOK = true
		end
		return
	end
	kSize:SetY(fw)
	kTop:SetImgSize( kSize )
	
	local kPos = kTop:GetLocation()
	kPos:SetY(kScreenSize:GetY()-fw)
	kTop:SetLocation( kPos )
end

function OnCreate_QuestFullScreenDialog(kTop)
	if kTop == nil or kTop:IsNil() then return end
	-- Screen size
	local kScreenSize = GetScreenSize()
	--local kOrgSize = kTop:GetSize()
	--if kOrgSize:GetX() ~= kScreenSize:GetX() or kOrgSize:GetY() ~= kScreenSize:GetY() then
		SCREEN_SIZE_SetSize(kTop)
		
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("FRM_QUEST_MENU"), 0 )
		--pass y
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("FRM_GUILD_MENU"), 0 )
		--pass y
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("FRM_EDIT"), 0 )
		SCREEN_SIZE_SetPosCenterY( kTop:GetControl("FRM_EDIT"), -60 )
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("FRM_GUILD_EMBLEM_SELECT"), 0 )
		SCREEN_SIZE_SetPosCenterY( kTop:GetControl("FRM_GUILD_EMBLEM_SELECT"), -60 )
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("SFRM_GUILD_BUY_SKILL"), 12 )
		SCREEN_SIZE_SetPosCenterY( kTop:GetControl("SFRM_GUILD_BUY_SKILL"), -60 )
		
		-- Screen BG
		SCREEN_SIZE_SetSize( kTop:GetControl("IMG_BLACK_BG_NORMAL") )
		SCREEN_SIZE_SetImgSize( kTop:GetControl("IMG_BLACK_BG_NORMAL") )
		SCREEN_SIZE_SetSize( kTop:GetControl("IMG_BLACK_BG_SCENARIO") )
		SCREEN_SIZE_SetImgSize( kTop:GetControl("IMG_BLACK_BG_SCENARIO") )
		-- Screen Title
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("IMG_TITLE_BG_NORMAL"), 0 )
		--SCREEN_SIZE_SetPosY( kTop:GetControl("IMG_TITLE_BG_NORMAL"), -732 )
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("IMG_TITLE_BG_SCENARIO"), 0 )
		--SCREEN_SIZE_SetPosY( kTop:GetControl("IMG_TITLE_BG_SCENARIO"), -732 )
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("IMG_TITLE"), 0  )
		--SCREEN_SIZE_SetPosY( kTop:GetControl("IMG_TITLE"), -734 )
		
		--3D Model
		SCREEN_SIZE_SetSize( kTop:GetControl("FRM_MODEL") )
		-- NPC Face
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("FRM_3DMODEL_NORMAL"), 256 )
		SCREEN_SIZE_SetPosY( kTop:GetControl("FRM_3DMODEL_NORMAL"), -572 )
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("FRM_3DMODEL_SCENARIO"), 256 )
		SCREEN_SIZE_SetPosY( kTop:GetControl("FRM_3DMODEL_SCENARIO"), -743 )
		
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("FRM_BG_SET_NORMAL"), 0 )
		SCREEN_SIZE_SetPosY( kTop:GetControl("FRM_BG_SET_NORMAL"), -261 )
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("FRM_BG_SET_SCENARIO"), 0 )
		SCREEN_SIZE_SetPosY( kTop:GetControl("FRM_BG_SET_SCENARIO"), -332 )
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("FRM_MAIN"), 0 )
		SCREEN_SIZE_SetPosY( kTop:GetControl("FRM_MAIN"), -261 )
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("IMG_ENTER"), 0 )
		SCREEN_SIZE_SetPosY( kTop:GetControl("IMG_ENTER"), -48 )
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("IMG_SKIP"), 358 )
		SCREEN_SIZE_SetPosY( kTop:GetControl("IMG_SKIP"), -48 )
		
		SCREEN_SIZE_SetPosCenterX( kTop:GetControl("FRM_QUEST_REQ"), -328 )
		SCREEN_SIZE_SetPosY( kTop:GetControl("FRM_QUEST_REQ"), -700 )
	--end
end

function OnCall_QuestFullScreenDialog()
	CloseUI("FRM_TUTORIAL_HINT")
	if GetEventScriptSystem():IsNowActivate() == false then
		LockPlayerInputMove(2) -- for quest lock
		LockPlayerInput(2)
		UIOff()
	end
	
	--g_world:SetShowWorldFocusFilter(true, "../Data/5_Effect/9_Tex/EF_blackBG.tga", 0.718)
	SetActorNameVisible(false)
	g_bEnableShortCutKey = false--Disable Short Cut Key
end

g_EnableSkipButton = false
function OnClose_QuestFullScreenDialog()
	g_EnableSkipButton = false
	if GetEventScriptSystem():IsNowActivate() == false then
		UnLockPlayerInputMove(2) -- for quest lock
		UnLockPlayerInput(2)
		UIOn()
	end
	
	
	if true == g_bCanUpdateLevelUp then
		UpdateLevelUI()
	end
	
	--g_world:SetShowWorldFocusFilter(false)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:ClearDrawActorFilter()
	GetQuestMan():ClearRecentQuestInfo()
	SetActorNameVisible(true)
	CloseUI("FRM_QUEST_REQ")
	CloseUI("FRM_COMPLETE_INFO")
	CloseToolTip()
	g_kCanCancelQuestDialog = true--Can Cancel FullScreen Quest Dialog
	g_bEnableShortCutKey = true--Enable Short Cut Key
	
	--[[
	local kPacket = NewPacket(12429)--PT_C_M_REQ_CANCEL_QUEST_TALK
	Net_Send(kPacket)
	DeletePacket(kPacket)
	CloseToolTip()
	]]
	
	OnShowMiniQuestList()
	UpdateGuildUI(GetUIWnd("SFRM_COMMUNITY"))
	ActivateUI("QuestOut")
	RefreshPartyStateWnd()
	OnUpdate_Community_PartyList()
end

g_kCanCancelQuestDialog = true
function IsCanCancelFullScreenQuestDialog()
	return g_kCanCancelQuestDialog
end

g_fQuestTime = 5400
function OnDisplay_QuestTimer()
	local fFrameTime = GetFrameTime()
	local kSelf = UISelf
	local kMin10 = kSelf:GetControl("FRM_MIN10")
	local kMin01 = kSelf:GetControl("FRM_MIN01")
	local kSec10 = kSelf:GetControl("FRM_SEC10")
	local kSec01 = kSelf:GetControl("FRM_SEC01")
	
	g_fQuestTime = g_fQuestTime - fFrameTime
	
	if 0 < g_fQuestTime then
		local iMin = math.floor(g_fQuestTime / 60)
		local iSec = math.floor(g_fQuestTime - (iMin*60))
		local iMin10 = math.floor(iMin / 10)
		if 10 < iMin10 then iMin10 = 10 end
		local iMin01 = math.floor(iMin - (iMin10*10))
		local iSec10 = math.floor(iSec / 10)
		local iSec01 = math.floor(iSec - (iSec10*10))
		kMin10:SetUVIndex(iMin10+1)
		kMin01:SetUVIndex(iMin01+1)
		kSec10:SetUVIndex(iSec10+1)
		kSec01:SetUVIndex(iSec01+1)
	end
end

g_kSFRM_QUEST_COMPLETE_Item = {}
g_kSFRM_QUEST_COMPLETE_Count = 0
function Add_SFRM_QUEST_COMPLETE_Item(String, bIgnoreSave)
	if GetEventScriptSystem():IsNowActivate() then return end
	if nil == String then return end
	local kWnd = GetUIWnd("SFRM_QUEST_COMPLETE")
	if kWnd:IsNil() then
		kWnd = ActivateUI("SFRM_QUEST_COMPLETE")
		if kWnd:IsNil() then return end
		for kKey, kVal in pairs(g_kSFRM_QUEST_COMPLETE_Item) do
			Add_SFRM_QUEST_COMPLETE_Item( kVal, true )
		end
	end
	local kList = kWnd:GetControl("LST_REWARD_TEXT")
	if kList:IsNil() then return end
	
	local kMiddleImg = kWnd:GetControl("IMG_BG2")
	if kMiddleImg:IsNil() then return end
	local kBottomImg = kWnd:GetControl("IMG_BG3")
	if kBottomImg:IsNil() then return end
	
	kList:AddNewListItemChar(String)
	if not bIgnoreSave then
		g_kSFRM_QUEST_COMPLETE_Count = g_kSFRM_QUEST_COMPLETE_Count + 1
		g_kSFRM_QUEST_COMPLETE_Item[g_kSFRM_QUEST_COMPLETE_Count] = String
	end
	
	--Adjust wnd size
	--local iLimitMinY = 61
	local iItemCnt = kList:GetListItemCount()
	local iMidY = iItemCnt * 21 + 21
	--[[if iMidY < iLimitMinY then iMidY = iLimitMinY end
	if iMidY > iLimitMinY then
		iMidY = iMidY + (iLimitMinY - (iMidY % iLimitMinY))
	end]]
	local kMidSize = kMiddleImg:GetSize()
	kMidSize:SetY(iMidY)
	kMiddleImg:SetSize(kMidSize)
	kMiddleImg:SetImgSize(kMidSize)
	
	local kBottomPos = kBottomImg:GetLocation()
	kBottomPos:SetY(78 + iMidY)
	kBottomImg:SetLocation(kBottomPos)
end
function End_SFRM_QUEST_COMPLETE()
	CloseUI("SFRM_QUEST_COMPLETE")
end
function End_After_SFRM_QUEST_COMPLETE()
	g_kSFRM_QUEST_COMPLETE_Item = {}
	g_kSFRM_QUEST_COMPLETE_Count = 0
	if GetQuestMan():ResumeNextQuestTalk() then
		UnLockPlayerInputMove(25) -- for Next quest lock
		UnLockPlayerInput(25)
	end
end

--//
g_kSelectiveQuestList = {}
g_kSelectiveQUestList_Cur = 0
g_kSelectiveQUestList_Max = 0

function OnInit_SelectiveQuestList(kGuid)
	g_kSelectiveQuestList = {}
	g_kSelectiveQUestList_Cur = 0
	g_kSelectiveQUestList_Max = 0
	g_kSelectiveQUestList_NpcGuid = kGuid
end

function OnAdd_SelectiveQuestList(iAddPos, iQuestID)
	if 0 > iAddPos then return end
	g_kSelectiveQuestList[iAddPos] = iQuestID
	
	if iAddPos > g_kSelectiveQUestList_Max then --maximmum
		g_kSelectiveQUestList_Max = iAddPos
	end
end

function HighlightSelectiveQuestList(iQuestID)
	local kTopWnd = GetUIWnd("SELECTIVE_QUEST_LIST")
	if kTopWnd:IsNil() then return end
	local kCursor = kTopWnd:GetControl("BTN_CUR")
	if kCursor:IsNil() then return end
    local kList = kTopWnd:GetControl("QUEST_LIST")
    if kList:IsNil() then return end
	
	for i = 0, g_kSelectiveQUestList_Max do	
		if g_kSelectiveQuestList[i] == iQuestID then
			g_kSelectiveQUestList_Cur = i
			break
		end
	end

	local kLimitYPos = 200
	local kTopPos = g_kSelectiveQUestList_Cur * 25
	if kLimitYPos < kTopPos then
		kList:SetDisplayStartPos(kTopPos - kLimitYPos)
		kTopPos = kLimitYPos
	else
		kList:SetDisplayStartPos(0)
	end
	
	local kPos = kCursor:GetLocation()
	kPos:SetY( 64 + kTopPos )
	kCursor:SetLocation(kPos)
end

-- 
function Move_SelectiveQuestList(iDirection)
	local kTopWnd = GetUIWnd("SELECTIVE_QUEST_LIST")
	if kTopWnd:IsNil() then return end
	local kCursor = kTopWnd:GetControl("BTN_CUR")
	if kCursor:IsNil() then return end
    local kList = kTopWnd:GetControl("QUEST_LIST")
    if kList:IsNil() then return end
	
	if 0 == iDirection then--Reset
		g_kSelectiveQUestList_Cur = 0
	elseif nil == g_kSelectiveQuestList[g_kSelectiveQUestList_Cur+iDirection] then --nil check
		if 0 > iDirection then
			g_kSelectiveQUestList_Cur = g_kSelectiveQUestList_Max
		elseif 0 < iDirection then
			g_kSelectiveQUestList_Cur = 0
		end
	else
		g_kSelectiveQUestList_Cur = g_kSelectiveQUestList_Cur+iDirection
	end
	kNext = g_kSelectiveQuestList[g_kSelectiveQUestList_Cur]
	
	local kLimitYPos = 200
	local kTopPos = g_kSelectiveQUestList_Cur * 25
	if kLimitYPos < kTopPos then
		kList:SetDisplayStartPos(kTopPos - kLimitYPos)
		kTopPos = kLimitYPos
	else
		kList:SetDisplayStartPos(0)
	end
	
	local kPos = kCursor:GetLocation()
	kPos:SetY( 64 + kTopPos )
	kCursor:SetLocation(kPos)
end

function OnEnter_SelectiveQuestList()
	local iQuestID = g_kSelectiveQuestList[g_kSelectiveQUestList_Cur]
	OnQuestItemSelected_From_SelectiveQuestList(iQuestID, g_kSelectiveQUestList_NpcGuid)
	CloseUI("SELECTIVE_QUEST_LIST")
end

g_kFullScreenUIShow = {}
g_kFullScreenUIShow["FACE"] = true
g_kFullScreenUIShow["TITLE"] = true
g_kFullScreenUIShow["CONTENTS"] = true
function OnSet_FullScreenUIShow(kName, bShow)
	g_kFullScreenUIShow[kName] = bShow
end

-- 
function OnCloseMiniQuest()
	CloseUI("FORM_MY_QUEST_LIST")
end
--
function OnRecoveryMiniQuest(bCheckBoss)
	if 0 ~= Get_MiniQuestList_ShowCount() then --mini quest information visible
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		if bCheckBoss and g_world:GetMapNo()==9020200 then--is not boss
			return
		end
		if g_world:IsHaveWorldAttr(GATTR_FLAG_MYHOME) then --myhome
			return
		end
		OnShowMiniQuestList()
	end
end

function InitSoulStoneTradeTable()
	AddSoulStoneTradeItem(1,51100110,60000960,2)
	AddSoulStoneTradeItem(1,51100120,60000960,2)
	AddSoulStoneTradeItem(1,51100125,60000960,2)
	AddSoulStoneTradeItem(1,51100130,60000960,3)
	AddSoulStoneTradeItem(1,51100135,60000960,3)
	AddSoulStoneTradeItem(1,51100140,60000960,3)
	AddSoulStoneTradeItem(1,51100150,60000960,3)
	AddSoulStoneTradeItem(2,51100160,60000960,3)
	AddSoulStoneTradeItem(2,51100155,60000960,3)
	AddSoulStoneTradeItem(2,51100165,60000960,4)
	AddSoulStoneTradeItem(2,51100170,60000960,4)
	AddSoulStoneTradeItem(2,51100180,60000960,4)
	AddSoulStoneTradeItem(2,51100190,60000960,4)
	AddSoulStoneTradeItem(2,51100200,60000960,4)
	AddSoulStoneTradeItem(2,51100210,60000960,4)
	AddSoulStoneTradeItem(2,51100220,60000960,4)
	AddSoulStoneTradeItem(2,51100295,60000960,4)
	AddSoulStoneTradeItem(2,51100235,60000960,5)
	AddSoulStoneTradeItem(2,51100245,60000960,5)
	AddSoulStoneTradeItem(2,51100255,60000960,5)
	AddSoulStoneTradeItem(2,51100265,60000960,5)
	AddSoulStoneTradeItem(2,51100275,60000960,5)
	AddSoulStoneTradeItem(2,51100285,60000960,5)
end
function GetSoulTradeLocationText(iLocationNo)
	if iLocationNo == 1 then
		return 700
	elseif iLocationNo == 2 then
		return 701
	end
	return 0
end

function CheckNfyClickState(kSelf)
	for i = 0, 1 do
		local Child = kSelf:GetControl("FRM_ITEM"..i)
		if not Child:IsNil() then
			if Child:IsVisible() then
				return
			end
		end
	end
	kSelf:Close()
end

function CloseCheckNewQuestNfy(bIsScenarioQuest)
	local kWnd = GetUIWnd("FRM_NEW_QUEST_NOTICE")
	if kWnd:IsNil() then
		CheckNewQuestNfy(2, 0)
		return
	end
	
	if bIsScenarioQuest then
		TickQuestNfyScriptFunc( kWnd:GetControl("FRM_ITEM" .. 0) )
	else
		TickQuestNfyScriptFunc( kWnd:GetControl("FRM_ITEM" .. 1) )
	end
end

function TickQuestNfyScriptFunc(kSelf)
	if kSelf == nil or kSelf:IsNil() then
		return
	end

	local QuestID = kSelf:GetCustomDataAsInt()
	local CallType = kSelf:GetParent():GetCustomDataAsInt()
	
	if kSelf:GetBuildIndex() == 0 then
		kSelf:Visible(OnTickCheckQuestModify(CallType, QuestID, true))
		local kQuestListUI = GetUIWnd("SFRM_NEW_QUEST")
		if nil ~= kQuestListUI and false == kQuestListUI:IsNil() then
			if 4 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(3)
			elseif 1 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(2) 
			elseif 2 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(1)
			end	
		else
			if 1 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(2) 
			elseif 2 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(1)
			elseif 3 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(4)
			elseif 4 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(3)				
			end		
		end
	else
		kSelf:Visible(OnTickCheckQuestModify(CallType, QuestID, false))
		local kQuestListUI = GetUIWnd("SFRM_NEW_QUEST")
		if nil ~= kQuestListUI and false == kQuestListUI:IsNil() then
			if 2 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(1)
			elseif 3 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(4)
			elseif 4 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(3)	
			end	
		else
			if 1 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(2) 
			elseif 2 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(1)
			elseif 3 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(4)
			elseif 4 == kSelf:GetUVIndex() then
				kSelf:SetUVIndex(3)				
			end		
		end

	end	
end

function CallQuestNfyScriptFunc(kSelf)
	local QuestID = kSelf:GetCustomDataAsInt()
	local CallType = kSelf:GetParent():GetCustomDataAsInt()
	local bIsScenarioQuest = false
	if kSelf:GetBuildIndex() == 0 then
		bIsScenarioQuest = true
	end
	CloseCheckNewQuestNfy(not bIsScenarioQuest)
	CallQuestNfyUI(CallType, QuestID, bIsScenarioQuest)
	if true == CheckRemoteAcceptQuest(CallType, QuestID, bIsScenarioQuest) then
		kSelf:Visible(true)
	else
		kSelf:Visible(false)
	end
end

function CallEventQuestNpcTalk(kOwnerGuid, bFirst)
	if IsNowEventQuest() then
		local kText = GetEventQuest_NpcTalkPrologue()
		if not bFirst then
			--[[if GetLocale() == LOCALE.NC_TAIWAN then
				kText = GetTT(450219)
			else
				kText = GetTT(450228)
			end--]]
			kText = GetTT(450219)
		end
		--now in event
--		if GetLocale() == LOCALE.NC_TAIWAN then
			CallNpcFunctionUI2(1000, kOwnerGuid, "Tw_event_girl_normal", kText)
--		else
--			CallNpcFunctionUI2(1000, kOwnerGuid, "Colin_normal", kText)
--		end
	else
		--not in event
--		if GetLocale() == LOCALE.NC_TAIWAN then
			CallNpcFunctionUI(1001, kOwnerGuid, "Tw_event_girl_normal", 450217)
--		else
--			CallNpcFunctionUI(1001, kOwnerGuid, "Colin_normal", 450226)
--		end
	end
end

function OnCallAchieveStatus(wnd)
	if nil==wnd or wnd:IsNil()then return end
	if nil==g_world then return end
	
	local attr = g_world:GetAttr()
	ODS("OnCallAchieveStatus: " .. attr .. "\n")
	if (GATTR_BOSS == attr)
	or (GATTR_FLAG_BOSS == attr) then
		wnd:Close()
	end
end

function OnClickAchieveBook()
	local wnd = CallUI("FRM_BOOK")
	if false==wnd:IsNil() then
		UI_Click_Book_Tab(wnd:GetControl("CBTN_TAB1"))
	end
end

function OnDblClick_QuestIngItem(kUISelf)
	local iQuestID = kUISelf:GetParent():GetCustomDataAsInt()
	if true == IsCanShowIngQuestList(iQuestID) then
		Show_IngQuestInterface(iQuestID)
	else
		-- Soul Quest UI
	end
end
