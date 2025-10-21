
function RunningEvent_ClearRecord()
	local wnd = GetUIWnd("FRM_RUNNING_RANKING")
	if wnd:IsNil() == true then
		wnd = CallUI("FRM_RUNNING_RANKING")
		if wnd:IsNil() == true then
			return
		end
	end

	for i=0,3 do
		local Form = wnd:GetControl("FRM_RUNNING_EVENT_RANKING".. i)
		if Form:IsNil() == false then
			Form:Visible(false)
		end
	end
end

function RunningEvent_UpdateRecord(Rank, MyRank, Name, wPos, UpDateRecordTime, bUpdate)
	
	local wnd = GetUIWnd("FRM_RUNNING_RANKING")
	if wnd:IsNil() == true then
		wnd = CallUI("FRM_RUNNING_RANKING")
		if wnd:IsNil() == true then
			return
		end
	end

	local Form = wnd:GetControl("FRM_RUNNING_EVENT_RANKING".. Rank-1)
	if Form:IsNil() then
		return
	end
	Form:Visible(true)
	if bUpdate == false then
		return
	end

	local RankingTOP = Form:GetControl("RANKINGTOP")
	if RankingTOP:IsNil() then
		return
	end
	
	local RankingLowMin10 = Form:GetControl("RANKINGLOW10")
	if RankingLowMin10:IsNil() then
		return
	end	
	
	local RankingLowMin1 = Form:GetControl("RANKINGLOW1")	
	if RankingLowMin1:IsNil() then
		return
	end	
	
	if MyRank > 99 then
		MyRank = 99
	end
	
	if UpDateRecordTime > 5999990 then
		UpDateRecordTime = 5999990
	end	
	
	if Rank < 4 then
		RankingTOP:SetUVIndex(Rank)
		RankingLowMin10:Visible(false)
		RankingLowMin1:Visible(false)

	elseif Rank == 4 then
		RankingTOP:Visible(false)
		if MyRank < 10 then
			RankingLowMin10:Visible(false)
		else
			RankingLowMin10:Visible(true)
		end 
		RankingLowMin1:Visible(true)
		local UserMin10 = math.floor(MyRank / 10) + 1
		local UserMin1 = math.floor(MyRank % 10) + 1
		RankingLowMin1:SetUVIndex(UserMin1)
		RankingLowMin10:SetUVIndex(UserMin10)
	end
	
	local UpDateUserID = Form:GetControl("USER_ID")
	if UpDateUserID:IsNil() == false then
		UpDateUserID:SetStaticTextW( Name )
	end	

	if wPos > 10 then
		wPos = 10
	end
	
	local RacePos = Form:GetControl("IMG_USER_POS")
	local RaceArrive = Form:GetControl("IMG_USER_ARRIVE")
	if wPos > 0 then
		if RacePos:IsNil() == false then
			wPos = ((wPos - 1) * 6) + 1
			if wPos < 1 then
				wPos = 0
			end
			RacePos:Visible(true)
			RacePos:SetTickInterval(300)
			RacePos:RefreshCalledTime()
			RacePos:RefreshLastTickTime()
			RacePos:SetUVIndex(wPos)
		end
		if RaceArrive:IsNil() == false then
			RaceArrive:Visible(false)
		end
	elseif wPos < 0 then
		if RacePos:IsNil() == false then
			RacePos:Visible(false)
		end
		if RaceArrive:IsNil() == false then
			wPos = 1
			RaceArrive:Visible(true)
			RaceArrive:SetTickInterval(300)
			RaceArrive:RefreshCalledTime()
			RaceArrive:RefreshLastTickTime()
			RaceArrive:SetUVIndex(wPos)
		end
	end


	--UpDateRecordTime 1/1000
	local dwMin = math.floor(UpDateRecordTime / 60000)
	local dwSec = math.floor((UpDateRecordTime % 60000) / 1000)
	local dwMilliSec = math.floor((UpDateRecordTime % 1000) / 10)

	local kWndMin10 = Form:GetControl("FRM_MIN10")
	if kWndMin10:IsNil() == false then
		if UpDateRecordTime == 0 then
			kWndMin10:Visible(false)
		else
			kWndMin10:Visible(true)
			kWndMin10:SetUVIndex(math.floor(dwMin / 10) + 1)
		end
	end
	
	local kWndMin1 = Form:GetControl("FRM_MIN1")
	if kWndMin1:IsNil() == false then
		if UpDateRecordTime == 0 then
			kWndMin1:Visible(false)
		else
			kWndMin1:Visible(true)
			kWndMin1:SetUVIndex(math.floor(dwMin % 10) + 1)
		end
	end
	
	local kWndSec10 = Form:GetControl("FRM_SEC10")
	if kWndSec10:IsNil() == false then
		if UpDateRecordTime == 0 then
			kWndSec10:Visible(false)
		else
			kWndSec10:Visible(true)
			kWndSec10:SetUVIndex(math.floor(dwSec / 10) + 1)
		end
	end
	
	local kWndSec1 = Form:GetControl("FRM_SEC1")
	if kWndSec1:IsNil() == false then
		if UpDateRecordTime == 0 then
			kWndSec1:Visible(false)
		else
			kWndSec1:Visible(true)
			kWndSec1:SetUVIndex(math.floor(dwSec % 10) + 1)
		end
	end
	
	local kWndMilliSec10 = Form:GetControl("FRM_SEC1000")
	if kWndMilliSec10:IsNil() == false then
		if UpDateRecordTime == 0 then
			kWndMilliSec10:Visible(false)
		else
			kWndMilliSec10:Visible(true)
			kWndMilliSec10:SetUVIndex(math.floor(dwMilliSec / 10) + 1)
		end
	end
	
	local kWndMilliSec1 = Form:GetControl("FRM_SEC100")
	if kWndMilliSec1:IsNil() == false then
		if UpDateRecordTime == 0 then
			kWndMilliSec1:Visible(false)
		else
			kWndMilliSec1:Visible(true)
			kWndMilliSec1:SetUVIndex(math.floor(dwMilliSec % 10) + 1)
		end
	end
	
	local TimerSep = Form:GetControl("IMG_TIMER_SEP")
	if TimerSep:IsNil() == false then
		if UpDateRecordTime == 0 then
			TimerSep:Visible(false)
		else
			TimerSep:Visible(true)
		end
	end	
end

g_iRaceAccumTime = 0
g_iRaceRecentTime = 0
g_bRaceStopTime = false
function Set_Running_Event_Timer()
	g_iRaceAccumTime = 0
	g_iRaceRecentTime = GetCurrentTimeInSec()*1000
	g_bRaceStopTime = false
	CallUI("FRM_RUNNING_EVENT_TIMER")
end

function Stop_Running_Event_Timer()
	g_bRaceStopTime = true
end

function RunningEvent_UR_SetMoveSlide(iRank, bVisible) --중간 순위 연출
	local wnd = GetUIWnd("FRM_RUNNING_RANKING")
	if wnd:IsNil() == true then
		wnd = CallUI("FRM_RUNNING_RANKING")
	end
	if wnd:IsNil() == true then
		return
	end
	
	local kRankElem = wnd:GetControl("FRM_RUNNING_EVENT_RANKING".. iRank-1)
	if kRankElem:IsNil() then
		return
	end
	if bVisible == true then
		kRankElem:SetCustomDataAsBYTE(1)
	else
		kRankElem:SetCustomDataAsBYTE(0)
	end
end

function RunningEvent_UR_MoveSlide() --중간 순위 연출
	local wnd = GetUIWnd("FRM_RUNNING_RANKING")
	if wnd:IsNil() == true then
		wnd = CallUI("FRM_RUNNING_RANKING")
	end
	if wnd:IsNil() == true then
		return
	end

	local kBuild = wnd:GetControl("BLD_RUNNIG_SLOT")
	if kBuild:IsNil() then
		return
	end
	local YGab = kBuild:GetBuildGab():GetY()
	for i=0,3 do
		local kRankElem = wnd:GetControl("FRM_RUNNING_EVENT_RANKING".. i)
		if false == kRankElem:IsNil() then
			local pos = kRankElem:GetLocation()
			local size = kRankElem:GetSize()
			local ptStart = Point2(1 - (size:GetX() * (i + 1)), YGab * (i + 1) )
			if kRankElem:GetCustomDataAsBYTE() == 0 then
				ptStart:SetX(1)
			end
			local ptEnd = Point2(1, YGab * (i + 1))
			if i == 3 then
				ptStart:SetY(ptStart:GetY() + 23)
				ptEnd:SetY(ptEnd:GetY() + 23)
			end
			kRankElem:SetupMove(ptStart, ptEnd, 0.1 * (i + 1))
		end
	end
end

function DisplayRunningEventTimer(wnd)
	if wnd:IsNil() == true then
		return
	end

	if g_bRaceStopTime == false then
		local fNow = GetCurrentTimeInSec()*1000
		g_iRaceAccumTime = g_iRaceAccumTime + fNow - g_iRaceRecentTime
		g_iRaceRecentTime = fNow
	end
	if g_iRaceAccumTime > 5999580 then
		g_iRaceAccumTime = 5999590
	end
	

	local iDelta = math.floor(g_iRaceAccumTime*0.1)
	local iSec100 = iDelta%10 + 1
	local iSec1000 = math.floor((iDelta%100)/10) + 1	
	iDelta = math.floor(g_iRaceAccumTime*0.001)
	local iSec1 = iDelta%10 + 1
	local iSec10 = math.floor((iDelta%60)/10) + 1
	local iMin1 = math.floor(iDelta/60)%10 + 1
	local iMin10 = math.floor(iDelta/600)%10 + 1	
	
	local wnd100sec = wnd:GetControl("FRM_SEC100")
	if wnd100sec:IsNil() == false then
		wnd100sec:SetUVIndex(iSec100)
	end

	local wnd1000sec = wnd:GetControl("FRM_SEC1000")
	if wnd1000sec:IsNil() == false then
		wnd1000sec:SetUVIndex(iSec1000)
	end
	
	local wnd1sec = wnd:GetControl("FRM_SEC1")
	if wnd1sec:IsNil() == false then
		wnd1sec:SetUVIndex(iSec1)
	end

	local wnd10sec = wnd:GetControl("FRM_SEC10")
	if wnd10sec:IsNil() == false then
		wnd10sec:SetUVIndex(iSec10)
	end

	local wnd1min = wnd:GetControl("FRM_MIN1")
	if wnd1min:IsNil() == false then
		wnd1min:SetUVIndex(iMin1)
	end

	local wnd10min = wnd:GetControl("FRM_MIN10")
	if wnd10min:IsNil() == false then
		wnd10min:SetUVIndex(iMin10)
	end

	local kPathDefault = "../Data/6_ui/mission/msNumTmY.tga"
	local kPathsDefault = "../Data/6_ui/mission/msNumTmYs2.tga"
	local kPath = "../Data/6_ui/mission/msNumTmY.tga"
	local kPaths = "../Data/6_ui/mission/msNumTmYs2.tga"
	
	local iChangeColorTime = 180
	if true == IsDefenceMode() then
		iChangeColorTime = 10
		kPath = "../Data/6_ui/mission/msNumTmR.tga"
		kPaths = "../Data/6_ui/mission/msNumTmRs.tga"
	end
	
	if iDelta <= iChangeColorTime then	
		if true == IsDefenceMode() and 1 < iDelta then
			ActivateUI("FRM_DEFENCE_TIMELIMIT")
		end
		wnd100sec:ChangeImage(kPaths)
		wnd100sec:SetInvalidate(true)
		wnd1000sec:ChangeImage(kPaths)
		wnd1000sec:SetInvalidate(true)
		wnd10sec:ChangeImage(kPath)
		wnd10sec:SetInvalidate(true)
		wnd1sec:ChangeImage(kPath)
		wnd1sec:SetInvalidate(true)
		wnd1min:ChangeImage(kPath)
		wnd1min:SetInvalidate(true)
		wnd10min:ChangeImage(kPath)
		wnd10min:SetInvalidate(true)
	else
		wnd100sec:ChangeImage(kPathsDefault)
		wnd100sec:SetInvalidate(true)
		wnd1000sec:ChangeImage(kPathsDefault)
		wnd1000sec:SetInvalidate(true)
		wnd10sec:ChangeImage(kPathDefault)
		wnd10sec:SetInvalidate(true)
		wnd1sec:ChangeImage(kPathDefault)
		wnd1sec:SetInvalidate(true)
		wnd1min:ChangeImage(kPathDefault)
		wnd1min:SetInvalidate(true)
		wnd10min:ChangeImage(kPathDefault)
		wnd10min:SetInvalidate(true)	
	end
	
end

function RunningEvent_NotifyTime(dwRemainTime, iTTNo1, iTTNo2)
	local dwTimeInSec = math.floor(dwRemainTime / 1000)
	local Minute =  math.floor(dwTimeInSec / 60)
	local Second = dwTimeInSec % 60

	local str1 = ""
	local str2 = string.format(GetTextW(315):GetStr(), Minute)
	local str3 = string.format(GetTextW(316):GetStr(), Second)
	local str4 = " "..GetTextW(iTTNo2):GetStr()
	if iTTNo1 > 0 then
		str1 = GetTextW(iTTNo1):GetStr().." "
	end

	if 0 == Minute then
		g_ChatMgrClient:AddLogMessage(WideString(str1..str3..str4), true, 0, 12)
	elseif 0 == Second then
		g_ChatMgrClient:AddLogMessage(WideString(str1..str2..str4), true, 0, 12)
	else
		g_ChatMgrClient:AddLogMessage(WideString(str1..str2.." "..str3..str4), true, 0, 12)
	end
end


function RunningEvent_DisplayWaitTimer(wnd, dwRemainTime)
	if wnd:IsNil() == true then
		return
	end

	local iDelta = math.floor(dwRemainTime / 1000)
	
	local iSec1 = iDelta%10 + 1	-- 1더하는 것은 그림 index가 1부터 시작하기 때문에
	local iSec10 = math.floor((iDelta%60)/10) + 1
	local iMin1 = math.floor(iDelta/60)%10 + 1
	local iMin10 = 0
	if wnd:GetControl("FRM_HOUR1"):IsNil() then
		iMin10 = math.floor(iDelta/600)%10 + 1
	else
		iMin10 = math.floor((iDelta/60)%60)/10 + 1
	end
	
	local wnd1sec = wnd:GetControl("FRM_SEC1")
	if wnd1sec:IsNil() == false then
		wnd1sec:SetUVIndex(iSec1)
	end
	local wnd10sec = wnd:GetControl("FRM_SEC10")
	if wnd10sec:IsNil() == false then
		wnd10sec:SetUVIndex(iSec10)
	end
	local wnd1min = wnd:GetControl("FRM_MIN1")
	if wnd1min:IsNil() == false then
		wnd1min:SetUVIndex(iMin1)
	end
	local wnd10min = wnd:GetControl("FRM_MIN10")
	if wnd10min:IsNil() == false then
		wnd10min:SetUVIndex(iMin10)
	end
end

g_dwRunningEventBeginTime = 0
function RunningEvent_SetBeginTimeNfy(dwMilliSec)
	g_dwRunningEventBeginTime = dwMilliSec
	CallUI("FRM_RACE_EVENT_WAIT_BEGIN_TIMER")
end

g_dwRunningEventFinishTime = 0
function RunningEvent_SetFinishTimeNfy(dwMilliSec)
	g_dwRunningEventFinishTime = dwMilliSec
	CallUI("FRM_RACE_EVENT_WAIT_FINISH_TIMER")
end