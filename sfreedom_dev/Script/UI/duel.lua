--szControlName: FRM_DUEL_START_COUNTDOWN, FRM_DUEL_FINISH_COUNTDOWN, FRM_DUEL_AREAOUT_COUNTDOWN 셋중 하나가 되겠지.
--iRemainTimeSec: 값의 범위는 1~6이 되겠지
function CallDuelCountDown( szControlName, iRemainTimeSec ) 
	local kUI = ActivateUI(szControlName)
	if false == kUI:IsNil() then
		local kImg = kUI:GetControl("IMG_COUNT_DOWN")
		if false == kImg:IsNil() then
			if iRemainTimeSec <= 0 then
				ODS("CallPvPCountDown : Few RemainTimeSec<" .. iRemainTimeSec .. ">\n")
				iRemainTimeSec = 1
			elseif iRemainTimeSec > 6 then
				ODS("CallPvPCountDown : Over RemainTimeSec<" .. iRemainTimeSec .. ">\n")
				iRemainTimeSec = 6
			elseif 4 > iRemainTimeSec then
				PlaySoundByID( "PVP_Count_0" .. iRemainTimeSec ) 
			end
			kImg:SetUVIndex( iRemainTimeSec )
		end		
	end
end

function CancelDuelCountDown( szControlName )
	CloseUI(szControlName)
end

function IsDuelCounting( szControlName )
	return IsVisible( szControlName )
end

function UpdateDuelStartCountDown(kSelf)
	if false == kSelf:IsNil() then
		local iNowIndex = kSelf:GetUVIndex()
		if 1 < iNowIndex then
			local iRemainTimeSec = iNowIndex - 1
			kSelf:SetUVIndex( iRemainTimeSec )
			if 4 > iRemainTimeSec then
				PlaySoundByID( "PVP_Count_0" .. iRemainTimeSec )
			end
		else --1 다음엔 무엇을 할건가
			CloseUI("FRM_DUEL_START_COUNTDOWN")
		end
	end
end

function UpdateDuelFinishCountDown(kSelf)
	if false == kSelf:IsNil() then
		local iNowIndex = kSelf:GetUVIndex()
		if 1 < iNowIndex then
			local iRemainTimeSec = iNowIndex - 1
			kSelf:SetUVIndex( iRemainTimeSec )
			if 4 > iRemainTimeSec then
				PlaySoundByID( "PVP_Count_0" .. iRemainTimeSec )
			end
		else --1 다음엔 무엇을 할건가
			CloseUI("FRM_DUEL_FINISH_COUNTDOWN")
		end
	end
end

function UpdateDuelAreaOutCountDown(kSelf)
	if false == kSelf:IsNil() then
		local iNowIndex = kSelf:GetUVIndex()
		if 1 < iNowIndex then
			local iRemainTimeSec = iNowIndex - 1
			kSelf:SetUVIndex( iRemainTimeSec )
			if 4 > iRemainTimeSec then
				PlaySoundByID( "PVP_Count_0" .. iRemainTimeSec )
			end
		else --1 다음엔 무엇을 할건가
			CloseUI("FRM_DUEL_AREAOUT_COUNTDOWN")
		end
	end
end

g_iRemainSec = 15
function InitAnsBox(kSelf)
	if kSelf:IsNil() then
		return
	end
	g_iRemainSec = 15

	local kWndBg = kSelf:GetControl("SFRM_BG")
	if kWndBg:IsNil() then
		return
	end
	local kWndBgColor = kWndBg:GetControl("SFRM_BG_COLOR")
	local kMsg1 = kSelf:GetControl("FRM_MESSAGE")
	local kMsg2 = kSelf:GetControl("FRM_MESSAGE2")
	
	if kWndBgColor:IsNil() == false and kMsg1:IsNil() == false and kMsg2:IsNil() == false then
		local kTextSize1 = kMsg1:GetTextSize()
		local kTextSize2 = kMsg2:GetTextSize()
		local kTextPos1 = kMsg1:GetTextPos()
		local kTextPos2 = kMsg2:GetTextPos()
		kTextPos1:SetY( (kWndBgColor:GetSize():GetY() - kTextSize1:GetY() - kTextSize2:GetY() ) / 2 )
		kTextPos2:SetY( kTextPos1:GetY() + kTextSize1:GetY() )
		kMsg1:SetTextPos(kTextPos1)
		kMsg2:SetTextPos(kTextPos2)
	end
end

function UpdateRemainAnsBox(kSelf)
	if g_iRemainSec > 0 then
		g_iRemainSec = g_iRemainSec - 1
	else
		local kBtnCancel = UISelf:GetControl("BTN_CANCEL")
		if kBtnCancel:IsNil() == false then
			OnClickPvP_AskOutBoxBtn(kBtnCancel)
			UISelf:Close()
		end
	end
		
	local kMsg2 = kSelf:GetControl("FRM_MESSAGE2")
	if kMsg2:IsNil() == false then
		kMsg2:SetStaticText( string.format(GetTextW(201221):GetStr(), g_iRemainSec ) )
	end
end

