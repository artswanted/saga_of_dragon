function InitBSNotice()
	Add_BSNotice(799233)
	Add_BSNotice(460080)
	Add_BSNotice(460081)
	Add_BSNotice(460082)
	Add_BSNotice(460083)
	Add_BSNotice(460095)
	Add_BSNotice(460096)
end

g_iBSTimeSecValue = 0
g_bBattleSquareNowGame = 0

function CallBattleSquareTimer(iRemainTimeSec, bNowGame)
	local kTimer = ActivateUI("FRM_BS_TIMER")
	g_iBSTimeSecValue = iRemainTimeSec * 100 + 100
	if g_bBattleSquareNowGame ~= bNowGame then
		local kTitle = kTimer:GetControl("FRM_TITLE")
		if false == kTitle:IsNil() then
			if true == bNowGame then
				kTitle:SetStaticTextW(GetTT(799232))
			else
				kTitle:SetStaticTextW(GetTT(799231))
			end
		end
	end
	g_bBattleSquareNowGame = bNowGame
end
function UpdateBattleSquareTimer(kTopWnd)
	if nil == kTopWnd then return end
	if kTopWnd:IsNil() then return end
	
	if 1 > g_iBSTimeSecValue then
		g_iBSTimeSecValue = 0
	else
		g_iBSTimeSecValue = g_iBSTimeSecValue - (GetFrameTime() * 100)
	end
	
	local iMin = math.mod(g_iBSTimeSecValue / 6000, 60)
	local iSec = math.mod(g_iBSTimeSecValue / 100, 60)
	local iMSec = math.mod(g_iBSTimeSecValue, 60)
	
	SetNumberToControl(iMin, kTopWnd:GetControl("FRM_MIN10"), kTopWnd:GetControl("FRM_MIN1"), true)
	SetNumberToControl(iSec, kTopWnd:GetControl("FRM_SEC10"), kTopWnd:GetControl("FRM_SEC1"), false)
	--SetNumberToControl(iMSec, kTopWnd:GetControl("FRM_SEC1000"), kTopWnd:GetControl("FRM_SEC100"), false)
end

function UpdateBattleSquareTeamPoint(UITeam, iPoint)
	if UITeam:IsNil() then
		return
	end
	SetNumberToControl(iPoint, UITeam:GetControl("FRM_COUNT10"), UITeam:GetControl("FRM_COUNT1"), false)
end

function SetNumberToControl(iValue, kTen, kOne, bSkipEnable)
	if kTen:IsNil() then return end
	if kOne:IsNil() then return end
	
	local iTen = iValue / 10
	local iOne = math.mod(iValue, 10)
	if true == bSkipEnable and 1 > iTen then
		kTen:SetUVIndex( 11 )
	else
		kTen:SetUVIndex( iTen + 1 )
	end
	kOne:SetUVIndex( iOne + 1 )
end

--/////////////////////////////////////
function Attach_BSSoundGameSet(kSoundName)
	local kMyActor = g_pilotMan:GetPlayerActor()
	if not kMyActor:IsNil() then
		kMyActor:AttachSound(100, kSoundName, 1.0, 0.0, 0.0)
	end
end
function Check_BSTimeCount(iDiffTime)
	if -10 == iDiffTime or 10 == iDiffTime then
		Attach_BSSoundGameSet("PVP_Count_10")
	elseif -9 == iDiffTime or 9 == iDiffTime then
		Attach_BSSoundGameSet("PVP_Count_09")
	elseif -8 == iDiffTime or 8 == iDiffTime then
		Attach_BSSoundGameSet("PVP_Count_08")
	elseif -7 == iDiffTime or 7 == iDiffTime then
		Attach_BSSoundGameSet("PVP_Count_07")
	elseif -6 == iDiffTime or 6 == iDiffTime then
		Attach_BSSoundGameSet("PVP_Count_06")
	elseif -5 == iDiffTime or 5 == iDiffTime then
		Attach_BSSoundGameSet("PVP_Count_05")
	elseif -4 == iDiffTime or 4 == iDiffTime then
		Attach_BSSoundGameSet("PVP_Count_04")
	elseif -3 == iDiffTime or 3 == iDiffTime then
		Attach_BSSoundGameSet("PVP_Count_03")
	elseif -2 == iDiffTime or 2 == iDiffTime then
		Attach_BSSoundGameSet("PVP_Count_02")
	elseif -1 == iDiffTime or 1 == iDiffTime then
		Attach_BSSoundGameSet("PVP_Count_01")
		if 0 > iDiffTime then
			GetEventTimer():Add("BS_READY_SOUND", 1, "Attach_BSSoundGameSet('PVP_READY')", 1, false)
			GetEventTimer():Add("BS_START_SOUND", 1.4, "Attach_BSSoundGameSet('PVP_Battle_START')", 1, false)
		end
	else
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		if GATTR_BATTLESQUARE == g_world:GetAttr() then
			Attach_BSSoundGameSet("PVP_Battle_Alarm")
		else
			Attach_BSSoundGameSet("PVP_Battle_Alarm")
		end
	end
end

--/////////////////////////////////////
function BSKillMsgTick( UISelf )
	if UISelf:IsNil() == false then
		local iCount = UISelf:GetListItemCount()
		if iCount > 0 then
			local kItem = UISelf:ListFirstItem()
			if kItem:IsNil() == false then
				local kItemWnd = kItem:GetWnd()
				local fStartTime = kItemWnd:GetCustomDataAsFloat()
				if (GetAccumTime() - fStartTime) > 7.0 then
					UISelf:ListDeleteItem(kItem)
				end
			end
		end
	end
end

function CallBSMainUI(bObserverMode)
	ActivateUI("FRM_BS_INFO")
	ActivateUI("FRM_BS_TEAM_RED_POINT")
	ActivateUI("FRM_BS_TEAM_BLUE_POINT")
	ActivateUI("FRM_BS_TEAM_RED_MEDAL")
	ActivateUI("FRM_BS_TEAM_BLUE_MEDAL")
	
	if true == bObserverMode then
		ActivateUI("FRM_BS_CENTER_NOTICE")
		CloseUI("FRM_BS_MY_POINT")
		CloseUI("FRM_BS_WIN_TEAM_ITEM")
		CloseUI("FRM_BS_MY_ITEM")
	else
		CloseUI("FRM_BS_CENTER_NOTICE")
		ActivateUI("FRM_BS_MY_POINT")
		ActivateUI("FRM_BS_WIN_TEAM_ITEM")
		ActivateUI("FRM_BS_MY_ITEM")
	end
end
function CloseBSMainUI()
	CloseUI("FRM_BS_TEAM_RED_POINT")
	CloseUI("FRM_BS_TEAM_BLUE_POINT")
	CloseUI("FRM_BS_TEAM_RED_MEDAL")
	CloseUI("FRM_BS_TEAM_BLUE_MEDAL")
	CloseUI("FRM_BS_CENTER_NOTICE")
	CloseUI("FRM_BS_MY_POINT")
	CloseUI("FRM_BS_WIN_TEAM_ITEM")
	CloseUI("FRM_BS_MY_ITEM")
end

g_kBSReviceTimer = 0
function SetBSHeartVisible(kTopWnd, bVisible)
	if false == kTopWnd:IsNil() then
		local kHeart = kTopWnd:GetControl("FRM_REBIRTHTIME_DUMMY")
		if false == kHeart:IsNil() then
			local Eft = kHeart:GetControl("IMG_REBIRTH_EFT")
			if false == Eft:IsNil() then
				Eft:Visible( bVisible )
			end
		end
	end
end
function CallBSReviveTimer(kTopWnd)
	if kTopWnd:IsNil() then return end
	g_kBSReviceTimer = GetAccumTime()
	SetBSHeartVisible(kTopWnd, false)
end
function BSReviveTimerTick(UISelf)
	if 0 == g_kBSReviceTimer then
		ODS( GetAccumTime() .. " Timer None \n", false, 998 )
		return
	end
	if UISelf:IsNil() == false then
		local fTime = GetAccumTime() - g_kBSReviceTimer
		local iTime = math.floor(fTime)
		local iBaseTime = 9
		if iBaseTime > iTime then
			UISelf:SetStaticText(iBaseTime - iTime)
			UISelf:SetUVIndex(iTime + 1)
		else
			UISelf:SetStaticText(0)
			UISelf:SetUVIndex(10)
			SetBSHeartVisible(UISelf:GetParent(), true)
		end
	end
end
function BSRebirthTickHeart(UISelf)
	if UISelf:IsNil() or (not UISelf:IsVisible()) then
		return
	end
	UISelf:SetSizeScale(UISelf:GetSizeScale()+0.2, true)
	UISelf:SetMaxAlpha(UISelf:GetMaxAlpha()-0.05)
	if 3.5 < UISelf:GetSizeScale() then
		UISelf:Visible(false)
		UISelf:SetSizeScale(1.0, true)
		UISelf:SetMaxAlpha(1.0)
		g_kBSReviceTimer = 0
	end
end

g_kBSRewardBonusMessage = {}
g_kBSRewardItem = {}
function ClearBSReward()
	g_kBSRewardBonusMessage = {}
	g_kBSRewardItem = {}
end
function AddBSBonusRewardMsg(iNo, kMessage)
	g_kBSRewardBonusMessage[iNo] = kMessage
end
function AddBSRewardItem(iNo, iItemNo, iCount, iTotalCount)
	g_kBSRewardItem[iNo] = {}
	g_kBSRewardItem[iNo]["ITEM"] = iItemNo
	g_kBSRewardItem[iNo]["COUNT"] = iCount
	g_kBSRewardItem[iNo]["TOTAL_COUNT"] = iTotalCount
end
function Call_BSGameResult(iKill, iDeath, iPoint, iAliveTime)
	local kResultUI = GetUIWnd("FRM_BS_RESULT_ITEM")
	if kResultUI:IsNil() then
		kResultUI = ActivateUI("FRM_BS_RESULT_ITEM")
	end
	if kResultUI:IsNil() then return end

	local kTopWnd = kResultUI:GetControl("FRM_BS_EXIT_RESULT")
	if kTopWnd:IsNil() then return end
	kTopWnd:Visible(true)
	local kKill = kTopWnd:GetControl("FRM_KILL")
	if not kKill:IsNil() then
		kKill:SetStaticText( tostring(iKill) )
	end
	local kDeath = kTopWnd:GetControl("FRM_DEATH")
	if not kDeath:IsNil() then
		kDeath:SetStaticText( tostring(iDeath) )
	end
	local kPoint = kTopWnd:GetControl("FRM_POINT")
	if not kPoint:IsNil() then
		kPoint:SetStaticText( tostring(iPoint) )
	end
end

function Call_BSExit()
	local kResultUI = GetUIWnd("FRM_BS_RESULT_ITEM")
	if kResultUI:IsNil() then
		kResultUI = ActivateUI("FRM_BS_RESULT_ITEM")
	end
	if kResultUI:IsNil() then return end

	local kTopWnd = kResultUI:GetControl("SFRM_BS_EXIT")
	if kTopWnd:IsNil() then return end
	kTopWnd:Visible(true)
end

function Call_BSGameEnd(bWin, bDraw, iKill, iDeath, iPoint)
	Attach_BSSoundGameSet('PVP_Game_SET')
	local iTime = 1.0
	--GetEventTimer():Add("BS_END_SHOW_GAME_SET", 1.0, "ActivateUI('FRM_BATTLE_SQUARE_GAME_SET')", 1, false)
	local TimerUI = GetUIWnd("FRM_BS_TIMER")
	if not TimerUI:IsNil() then
		TimerUI:Close()
	end
	if true == bDraw then
		GetEventTimer():Add("BS_END_SHOW_GAME_SET", iTime, "ActivateUI('FRM_BATTLE_SQUARE_GAME_DRAW')", 1, false)
	else
		if true == bWin then
			GetEventTimer():Add("BS_END_SHOW_GAME_SET", iTime, "ActivateUI('FRM_BATTLE_SQUARE_GAME_WIN')", 1, false)
		else
			GetEventTimer():Add("BS_END_SHOW_GAME_SET", iTime, "ActivateUI('FRM_BATTLE_SQUARE_GAME_LOSE')", 1, false)
		end
	end
	local iRewardTime = iTime
	for kKey, kVal in pairs(g_kBSRewardItem) do
		iTime = iTime + 0.5
		GetEventTimer():Add("BS_END_SHOW_REWARD"..kKey, iTime, "Call_BSReward(".. kKey .. ", "..kVal["ITEM"]..", "..kVal["COUNT"]..", "..kVal["TOTAL_COUNT"]..")", 1, false)
		GetEventTimer():Add("BS_END_SHOW_REWARD_SOUND"..kKey, iTime - 0.1, "Attach_BSSoundGameSet('PVP_Item')", 1, false)
	end
	
	iTime = iTime + 1
	local iBaseSec = 60
	local iAliveTime = (iBaseSec - iTime - iRewardTime) * 1000
	GetEventTimer():Add("BS_END_SHOW_REWARD_POINT", iTime, "Call_BSGameResult(".. iKill .. "," .. iDeath .. "," .. iPoint .. "," .. iAliveTime ..")", 1, false)
	
	for kKey, kVal in pairs(g_kBSRewardBonusMessage) do
		iTime = iTime + 0.2
		GetEventTimer():Add("BS_END_SHOW_BONUS_REWARD_MSG"..kKey, iTime, "GetChatMgrClient():AddLogMessage( g_kBSRewardBonusMessage["..kKey.."], true, 1 )", 1, false)
	end
	iTime = iTime + 2
	GetEventTimer():Add("BS_END_SHOW_EXIT", iTime, "Call_BSExit()", 1, false)
	iTime = iTime + 2
	GetEventTimer():Add("BS_END_SHOW_EXIT_SYS_INV", iTime, "CheckSystemInventory()", 1, false)
end
