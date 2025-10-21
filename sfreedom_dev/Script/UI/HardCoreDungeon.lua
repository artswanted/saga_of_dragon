function SetHardCoreDungeonVoteResultUI( kUISelf )
	
	if nil == kUISelf then
		return
	end
	
	if kUISelf:IsNil() then
		return
	end
	
	local iMyIndex = kUISelf:GetCustomDataAsSize_t()
	if 0 < iMyIndex then
	
		local kMyUI = kUISelf:GetControl( "SFRM_VOTE_" .. iMyIndex )
		if false == kMyUI:IsNil() then
		
			local bSelected = false
			local iOK = 0
		
			local kControl = kMyUI:GetControl("IMG_OK")
			if false == kControl:IsNil() then
				if 3 == kControl:GetUVIndex() then
					bSelected = true
					iOK = 1
				end
			end
		
			if false == bSelected then
				kControl = kMyUI:GetControl("IMG_CANCEL")
				if false == kControl:IsNil() then
					if 3 == kControl:GetUVIndex() then
						bSelected = true
						iOK = 0
					end
				end
			end
		
			if true == bSelected then
		
				SetHardCoreDungeonVoteResult( iOK, 402007 )
			
			else
		
				CommonMsgBoxByTextTable( 402005, true )
			
			end
		end
	else
		iMyIndex = 1
		local kUI = kUISelf:GetControl( "SFRM_VOTE_" .. iMyIndex )
		
		while false == kUI:IsNil() do
		
			if "" ~= kUI:GetStaticText():GetStr() then
			
				local bIsVote = false
				
				local kControl = kUI:GetControl("IMG_OK")
				if false == kControl:IsNil() then
					if 3 == kControl:GetUVIndex() then
						bIsVote = true
					end
				end
			
				if false == bIsVote then
					kControl = kUI:GetControl("IMG_CANCEL")
					if false == kControl:IsNil() then
						if 3 ~= kControl:GetUVIndex() then
							CommonMsgBoxByTextTable( 402009, true )
							return
						end
					end
				end
			end
			
			iMyIndex = iMyIndex + 1
			kUI = kUISelf:GetControl( "SFRM_VOTE_" .. iMyIndex )
		end
		
		SetHardCoreDungeonVoteResult( 1, 402007 )
		
	end
end

function SetHardCoreDungeonVoteResult( iOK, iMsg )

	local kIfUI = ActivateUI("BOX_HCD_IF", true)
	if false == kIfUI:IsNil() then
			
		local kMsg = kIfUI:GetControl("FRM_MESSAGE")
		if false == kMsg:IsNil() then
			kMsg:SetStaticTextW( GetTextW(iMsg) )
		end
				
		local kBtn = kIfUI:GetControl("BTN_OK")
		if false == kBtn:IsNil() then
			kBtn:SetCustomDataAsInt( iOK )
		end
				
	end
end

function OnVoteUp( kUISelf )
	if nil == kUISelf then
		return
	end
	
	if kUISelf:IsNil() then
		return
	end
	
	if true == kUISelf:GetEnable() then
		if 1 == kUISelf:GetUVIndex() then
			kUISelf:SetUVIndex(2)
		end
	end
end

function OnVoteOut( kUISelf )
	if nil == kUISelf then
		return
	end
	
	if kUISelf:IsNil() then
		return
	end
	
	if true == kUISelf:GetEnable() then
		if 2 == kUISelf:GetUVIndex() then
			kUISelf:SetUVIndex(1)
		end
	end
end

function OnVoteDown( kUISelf, kOtherUIName )
	if nil == kUISelf then
		return
	end
	
	if kUISelf:IsNil() then
		return
	end
	
	if true == kUISelf:GetEnable() then
		if 2 == kUISelf:GetUVIndex() then
			kUISelf:SetUVIndex(3)
			
			local kParent = kUISelf:GetParent()
			if false == kParent:IsNil() then
				local kOtherUI = kParent:GetControl( kOtherUIName )
				if false == kOtherUI:IsNil() then
					if true == kOtherUI:GetEnable() then
						kOtherUI:SetUVIndex(1)
					end
				end
			end
		end
	end
end

function OnVoteIF( kUISelf )
	if nil == kUISelf then
		return
	end
	
	if kUISelf:IsNil() then
		return
	end
	
	local kVoteUIWnd = GetUIWnd( "SFRM_HCD_VOTE" )
	if false == kVoteUIWnd:IsNil() then
		local iMode = kUISelf:GetCustomDataAsInt()
		if 1 == iMode then
		
			local kBtn = kVoteUIWnd:GetControl("BTN_OK")
			if false == kBtn:IsNil() then
				kBtn:SetEnable(false)
			end
			
			kBtn = kVoteUIWnd:GetControl("BTN_CANCEL")
			if false == kBtn:IsNil() then
				kBtn:SetEnable(false)
			end
			
			Net_HardCoreDungeonVoteResultSend(true)
			
		else
		
			Net_HardCoreDungeonVoteResultSend(false)
			kVoteUIWnd:Close()
			
		end
	end
	
	kUISelf:CloseParent()
end

function OnVoteState( kUISelf )
	if nil == kUISelf then
		return
	end
	
	if kUISelf:IsNil() then
		return
	end
	
	local byState = kUISelf:GetCustomDataAsBYTE()
	if 1 < byState then
		local iText = 402100 + byState
		CallToolTipText(0, GetTextW(iText), GetUIWnd("Cursor"):GetLocation(), "ToolTip2", 11 )
	end
end

function OnTickMan( kUISelf )
	if nil == kUISelf then
		return
	end
	
	if kUISelf:IsNil() then
		return
	end
	
	local uv_index = kUISelf:GetUVIndex() + 1
	if 8 < uv_index then
		uv_index = 1
	end
	kUISelf:SetUVIndex(uv_index)
end

function OnTickHCDTime( kUISelf, bMsg )
	if nil == kUISelf then
		return
	end
	
	if kUISelf:IsNil() then
		return
	end
	
	local kControl = kUISelf:GetControl("IMG_DOT")
	if true == kControl:IsNil() then
		ODS("Not Fount Control 'IMG_DOT'\n")
		return
	end
	
	local bVisible = kControl:IsVisible()
	kControl:Visible( not bVisible )
	if true == bVisible then
		return
	end
	
	local iRemainTimeSec = kUISelf:GetCustomDataAsInt()
	if 0 < iRemainTimeSec then
		iRemainTimeSec = iRemainTimeSec - 1
		kUISelf:SetCustomDataAsInt( iRemainTimeSec )
	end
	
	-- Message
	if bMsg and 30 >= iRemainTimeSec then
		if 		30 == iRemainTimeSec 
			or	20 == iRemainTimeSec
			or	10 == iRemainTimeSec
			or	5 == iRemainTimeSec then
			
			local kStr = string.gsub( GetTextW(402012):GetStr(), "#SEC#", iRemainTimeSec )
			Notice_Show( WideString(kStr), 1 )
		end
	end
	
	--> Hour
	local iHour = iRemainTimeSec / 3600
	iHour = iHour - (iHour % 1)
	
	kControl = kUISelf:GetControl("IMG_HOUR_10")
	if false == kControl:IsNil() then
		local iTenHour = (iHour / 10) + 1
		if 10 < iTenHour then
			iTenHour = 10
		end
		kControl:SetUVIndex( iTenHour )
	end
	
	kControl = kUISelf:GetControl("IMG_HOUR_1")
	if false == kControl:IsNil() then
		kControl:SetUVIndex( (iHour % 10) + 1 )
	end
	--< End Hour
	
	iRemainTimeSec = iRemainTimeSec % 3600
	
	--> Min
	local iMin = iRemainTimeSec / 60
	iMin = iMin - (iMin % 1)
	
	kControl = kUISelf:GetControl("IMG_MIN_10")
	if false == kControl:IsNil() then
		kControl:SetUVIndex( (iMin / 10) + 1 )
	end
	
	kControl = kUISelf:GetControl("IMG_MIN_1")
	if false == kControl:IsNil() then
		kControl:SetUVIndex( (iMin % 10) + 1 )
	end
	--< End Min
	
	--> Sec
	local iSec = iRemainTimeSec % 60
	iSec = iSec - (iSec % 1)
	
	kControl = kUISelf:GetControl("IMG_SEC_10")
	if false == kControl:IsNil() then
		kControl:SetUVIndex( (iSec / 10) + 1 )
	end
	
	kControl = kUISelf:GetControl("IMG_SEC_1")
	if false == kControl:IsNil() then
		kControl:SetUVIndex( (iSec % 10) + 1 )
	end
	
end

function OnTickHCDMsg( kSelf )
	local iRand = Random(3)
	local iMsg = 402300 + iRand
	g_ChatMgrClient:AddEventMessage(iMsg, true, 0, 11)
end
