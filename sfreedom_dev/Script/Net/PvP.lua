
function Net_JoinPvP( iMode )

	local player = g_pilotMan:GetPlayerUnit()
	if IsInExpedition() then
		Notice_Show( GetTextW(720051), 1 )
		return
	end
	if false == player:IsNil() then
		local kPartyGuid = player:GetPartyGuid()
		if false == kPartyGuid:IsNil() and 4 ~= iMode then
			Notice_Show( GetTextW(19013), 1 )
			return
		end
	end
	
	local kPacket = NewPacket(30700)--PT_C_M_REQ_JOIN_LOBBY
	kPacket:PushInt( iMode )
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function Net_ChangePvPLobby( iLobbyID )
	local kPacket = NewPacket(30722)--PT_C_T_REQ_CHANGE_LOBBY
	kPacket:PushInt(iLobbyID)
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function Net_PT_PM_C_NFY_GAME_LOADING(kPacket)
	local kTopUI = CallUI("FRM_PVP_WAIT",true)
	local kAniBar = kTopUI:GetControl("BAR_TIMER")
	kAniBar:SetStartTime(15000)
	kTopUI:SetStaticText(GetTextW(400381):GetStr())
	LockPlayerInputMove(5) -- for pvp enter lock
	LockPlayerInput(5)
end

function Net_PT_M_C_REQ_DUEL_PVP(kPacket)
	local kReqCharGuid = kPacket:PopGuid()
	local kReqName = kPacket:PopWString()
	
	if 0 ~= Config_GetValue("ETC", "DENY_DUEL") then --결투자동거부 옵션이 설정되어 있다면
		local kPacket = NewPacket(30603) --PT_C_M_ANS_DUEL_PVP
		kPacket:PushGuid(kReqCharGuid)
		kPacket:PushBool(false)
		Net_Send(kPacket)
		DeletePacket(kPacket)
		return
	end
			
	local kWnd = GetUIWnd("BOX_PVP_ANSOUT")
	if false == kWnd:IsNil() then
		-- 이미 신청을 받고 있는 중이네.
		local kPacket = NewPacket(30605) --PT_C_M_ERROR_DUEL_PVP
		kPacket:PushGuid(kReqCharGuid)
		Net_Send(kPacket)
		DeletePacket(kPacket)
		return
	end
	
	kWnd = CallUI("BOX_PVP_ANSOUT", true)
	if kWnd:IsNil() then
		-- UI를 열 수 없으면  거절한거나 다름없다.
		local kPacket = NewPacket(30603) --PT_C_M_ANS_DUEL_PVP
		kPacket:PushGuid(kReqCharGuid)
		kPacket:PushBool(false)
		Net_Send(kPacket)
		DeletePacket(kPacket)
		return
	end
		
	local kMsgWnd = kWnd:GetControl("FRM_MESSAGE")
	if kReqName:IsNil() == false and false == kMsgWnd:IsNil() then
		--GetClassName //guid로 클래스 이름(TT) 얻기
		local kPilot = g_pilotMan:FindPilotByName(kReqName)
		if kPilot ~= nil and kPilot:IsNil() == false then
			local kGuid = kPilot:GetGuid()
			if kGuid:IsNil() == false then
				local kClassName = GetClassName(kGuid)
				local iLv = kPilot:GetAbil(AT_LEVEL)
				if kClassName:IsNil() == false and iLv ~= 0 then
					kMsgWnd:SetStaticText( string.format(GetTextW(200120):GetStr(), kReqName:GetStr(), kClassName:GetStr(), iLv) )
				end
			end
		end
		--kMsgWnd:SetStaticText( string.format(GetTextW(200120):GetStr(), kReqName:GetStr()) )
	end
	InitAnsBox(kWnd)
	
	local kOkBtn = kWnd:GetControl("BTN_OK")
	if false == kOkBtn:IsNil() then
		local kPacket = NewPacket(30603) --PT_C_M_ANS_DUEL_PVP
		kPacket:PushGuid(kReqCharGuid)
		kPacket:PushBool(true)
		kOkBtn:SetCustomDataAsPacket(kPacket)
		DeletePacket(kPacket)
	end
		
	local kCancelBtn = kWnd:GetControl("BTN_CANCEL")
	if false == kCancelBtn:IsNil() then
		local kPacket = NewPacket(30603) --PT_C_M_ANS_DUEL_PVP
		kPacket:PushGuid(kReqCharGuid)
		kPacket:PushBool(false)
		kCancelBtn:SetCustomDataAsPacket(kPacket)
		DeletePacket(kPacket)
	end	
end

function Net_PT_M_C_ANS_DUEL_PVP(kPacket)
	local bResult = kPacket:PopByte()
	local kWnd = GetUIWnd("BOX_PVP_ASKOUT")
	if false == kWnd:IsNil() then
		CloseUI("BOX_PVP_ASKOUT")
	end
	

	if bResult == 0 then --거절 받았을 경우
		local kName = kPacket:PopWString()		
		local kOutText = WideString(string.format(GetTextW(200122):GetStr(), kName:GetStr()))
		g_ChatMgrClient:AddLogMessage(kOutText, false, 2)
		CommonMsgBox( string.format(GetTextW(200122):GetStr(), kName:GetStr()),true )
	else
		--local kTargetID = kPacket:PopGuid()
	end
end

function Net_PT_M_C_NFY_DUEL_START_COUNT(kPacket)
	local bySec = kPacket:PopByte()
	CallDuelCountDown("FRM_DUEL_START_COUNTDOWN", bySec)
end

function Net_PT_M_C_NFY_DUEL_END_COUNT(kPacket)
	local bySec = kPacket:PopByte()
	CallDuelCountDown("FRM_DUEL_FINISH_COUNTDOWN", bySec)
end

function Net_PT_M_C_NFY_DUEL_OUT_COUNT(kPacket)
	local bySec = kPacket:PopByte()
	CallDuelCountDown("FRM_DUEL_AREAOUT_COUNTDOWN", bySec)
	g_ChatMgrClient:Notice_Show(GetTextW(201206), 1) --결투지역을 벗어났다. 5초안에 드가라

end

function Net_PT_M_C_NFY_DUEL_BEGIN(kPacket) --결투가 시작되면 PC 발밑에 각각 이펙트 붙여줌
	local stName1 = kPacket:PopWString()
	local stName2 = kPacket:PopWString()
	local kActor1 = nil
	local kActor2 = nil

	--CallUI("FRM_DUEL_START")
	local kPilot1 = g_pilotMan:FindPilotByName(stName1)
	if kPilot1:IsNil() == false then
		kActor1 = kPilot1:GetActor()
		if kActor1 ~= nil and kActor1:IsNil() == false then
			kActor1:AttachParticle(4444, "char_root", "e_ef_colorshadow_pvp_red")
			if kActor1:IsMyActor() then
				CallUI("FRM_DUEL_START")
			end
		end
	end
	local kPilot2 = g_pilotMan:FindPilotByName(stName2)
	if kPilot2:IsNil() == false then
		kActor2 = kPilot2:GetActor()
		if kActor2 ~= nil and kActor2:IsNil() == false then
			kActor2:AttachParticle(4444, "char_root", "e_ef_colorshadow_pvp_blue")
			if kActor2:IsMyActor() then
				CallUI("FRM_DUEL_START")
			end
		end
	end
	
	if stName1:IsNil() == false and stName2:IsNil() == false then
		local kOutText = WideString( string.format(GetTextW(201214):GetStr(), stName1:GetStr(), stName2:GetStr()) )
		g_ChatMgrClient:Notice_Show(kOutText, 0) --"a님과 b님의 결투가 시작되었습니다."
		if true == g_bOutputLog_Duel then
			if kActor1 ~= nil and kActor1:IsNil() == false and
			   kActor2 ~= nil and kActor2:IsNil() == false then
				StartOutputLog_PvP()
				SetOutputLog_InsertTarget( kActor1:GetPilotGuid() )
				SetOutputLog_InsertTarget( kActor2:GetPilotGuid() )
			else
				g_ChatMgrClient:Notice_Show("Not Found Duel Players", 0)
				EndOutputLog_PvP()
			end
		end
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

end

function Net_PT_M_C_NFY_DUEL_FINISH(kPacket) --결투가 종료되면 PC 발밑에 각각 이펙트 떼어줌
	local byRes = kPacket:PopByte() --1이면 1번 유저가 승자, 2이면 2번 유저가 승자다
	local byResCause = kPacket:PopByte()
	local kIDPlayer1 = kPacket:PopGuid()
	local kIDPlayer2 = kPacket:PopGuid()
	local stName1 = "Player 1"
	local stName2 = "Player 2"
	local kActor1 = nil
	local kActor2 = nil
	
	--게임이 이미 끝났다면.. 카운트다운 연출들은 모두 취소해라
	if IsDuelCounting("FRM_DUEL_FINISH_COUNTDOWN") then
		CancelDuelCountDown("FRM_DUEL_FINISH_COUNTDOWN")
	end
	if IsDuelCounting("FRM_DUEL_TIMEOUT_COUNTDOWN") then
		CancelDuelCountDown("FRM_DUEL_TIMEOUT_COUNTDOWN")
	end
	
	local kPilot1 = g_pilotMan:FindPilot(kIDPlayer1)
	if kPilot1:IsNil() == false then
		kActor1 = kPilot1:GetActor()
		if kActor1:IsNil() == false then
			kActor1:DetachFrom(4444)
		end
		stName1 = kPilot1:GetName():GetStr()
	end
	local kPilot2 = g_pilotMan:FindPilot(kIDPlayer2)
	if kPilot2:IsNil() == false then
		kActor2 = kPilot2:GetActor()
		if kActor2:IsNil() == false then
			kActor2:DetachFrom(4444)
		end
		stName2 = kPilot2:GetName():GetStr()
	end

	ODS("Duel " .. kIDPlayer1:GetString() .. " " .. kIDPlayer2:GetString() .. "\n")
	
	if kActor1 ~= nil and kActor2 ~= nil and kActor1:IsNil() == false and kActor2:IsNil() == false then
		local kWinnerActor = nil
		local kLoserActor = nil
		if byRes == 1 then
			kWinnerActor = kActor1
			kLoserActor = kActor2
		elseif byRes == 2 then
			kWinnerActor = kActor2
			kLoserActor = kActor1
		end
		
		if kWinnerActor ~= nil and kLoserActor ~= nil then
			if (kWinnerActor:IsMyActor() or kLoserActor:IsMyActor()) and byResCause == 0 then --내가 결투를 한 당사자일 경우
				kWinnerActor:ReserveTransitAction("a_duel_victory")
				kLoserActor:ReserveTransitAction("a_duel_slowmotion")
			else --내가 관전자일 경우 슬로우모션 없이 일반 액션으로 전환
				kWinnerActor:ReserveTransitAction("a_victory")
				kLoserActor:ReserveTransitAction("a_breakdown")
			end
		end
		
		
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if kActor1 ~= nil and kActor1:IsNil() == false then
		kActor1:SetDuelWinnerTitle() --결투 승리자 타이틀을 갱신
		if kActor1:IsMyActor() then
			if byRes == 1 then
				CallUI("FRM_DUEL_WIN")
			elseif byRes == 2 then
				CallUI("FRM_DUEL_LOSE")
			elseif byRes == 3 then
				CallUI("FRM_DUEL_DRAW")
			end
			PlaySoundByID("PVP_Game_SET")
		end
	end
	if kActor2 ~= nil and kActor2:IsNil() == false then
		kActor2:SetDuelWinnerTitle() --결투 승리자 타이틀을 갱신
		if kActor2:IsMyActor() then
			if byRes == 1 then
				CallUI("FRM_DUEL_LOSE")
			elseif byRes == 2 then
				CallUI("FRM_DUEL_WIN")
			elseif byRes == 3 then
				CallUI("FRM_DUEL_DRAW")
			end
			PlaySoundByID("PVP_Game_SET")
		end
	end
	
	local kOutText = nil
	if byRes == 1 then
		if byResCause == 2 then
			kOutText = WideString( string.format(GetTextW(201217):GetStr(), stName2) )
		else
			kOutText = WideString( string.format(GetTextW(201215):GetStr(), stName1) )
		end
	elseif byRes == 2 then
		if byResCause == 2 then
			kOutText = WideString( string.format(GetTextW(201217):GetStr(), stName1) )
		else
			kOutText = WideString( string.format(GetTextW(201215):GetStr(), stName2) )
		end
	elseif byRes == 3 then
		if byResCause == 4 then
			kOutText = WideString( string.format(GetTextW(201218):GetStr(), stName1, stName2) )
		else
			kOutText = WideString( string.format(GetTextW(201216):GetStr(), stName1, stName2) )
		end
	end
	if kOutText ~= nil then
		g_ChatMgrClient:Notice_Show(kOutText, 0)
	end
	
	if true == g_bOutputLog_Duel then
		EndOutputLog_PvP()
	end
	
end

function Net_PT_M_C_ERROR_DUEL_PVP(kPacket)
	local kWnd = GetUIWnd("BOX_PVP_ANSOUT")
	local iMessage = kPacket:PopInt()
	if false == kWnd:IsNil() then
		CloseUI("BOX_PVP_ANSOUT")
		if( 200136 == iMessage or 200135 == iMessage) then
			CommonMsgBox(string.format(GetTextW(iMessage):GetStr(), PVP_LimitLevel()), true)
		else
			CommonMsgBoxByTextTable(iMessage, true)
		end
	end
end

function Net_PT_M_C_ERROR_INPARTY_PVP(kPacket)
	local kWnd = GetUIWnd("BOX_PVP_ASKOUT")
	local kName = kPacket:PopWString()
	local iMessage = kPacket:PopInt()
	if false == kWnd:IsNil() then
		CloseUI("BOX_PVP_ASKOUT")
		CommonMsgBox(string.format(GetTextW(iMessage):GetStr(), kName:GetStr()), true)
	end
end

function Net_PT_T_C_ANS_INVITE_PVP(kPacket)
	local kName = kPacket:PopWString()
	local iRoomIndex = kPacket:PopInt()
	local iRoomNo = kPacket:PopUInt()
	local kCharGuid = kPacket:PopGuid()
	
	kWnd = ActivateUI("BOX_PVP_ASKOUT", true)
	if false == kWnd:IsNil() then
		local kMsgWnd = kWnd:GetControl("FRM_MESSAGE")
		if false == kMsgWnd:IsNil() then
			kMsgWnd:SetStaticText( string.format(GetTextW(200119):GetStr(), kName:GetStr(), iRoomNo) )
		end
	
		local kOkBtn = kWnd:GetControl("BTN_OK")
		if false == kOkBtn:IsNil() then
			local kPacket = NewPacket(30709) --PT_C_T_REQ_JOIN_ROOM
			kPacket:PushInt(iRoomIndex)
			kPacket:PushSize_t(0)
			kPacket:PushBool(true)
			kOkBtn:SetCustomDataAsPacket(kPacket)
			DeletePacket(kPacket)
		end
		
		local kCancelBtn = kWnd:GetControl("BTN_CANCEL")
		if false == kCancelBtn:IsNil() then
			local kPacket = NewPacket(30753) --PT_C_T_REQ_INVATE_FAIL
			kPacket:PushGuid(kCharGuid)
			kCancelBtn:SetCustomDataAsPacket(kPacket)
			DeletePacket(kPacket)
		end		
	end
end

function DoKingHillUp01_OnEnter(trigger, actor)
	DoHillUp(1)
end

function DoKingHillUp01_OnLeave(trigger, actor)
	DoHillOut(1)
end

function DoKingHillUp01_OnUpdate(trigger, actor)
	DoHillUp(1)
end

function DoKingHillUp02_OnEnter(trigger, actor)
	DoHillUp(2)
end

function DoKingHillUp02_OnLeave(trigger, actor)
	DoHillOut(2)
end

function DoKingHillUp02_OnUpdate(trigger, actor)
	DoHillUp(2)
end

function SetMyDuelTitle(iWins)
	g_pilotMan:GetPlayerActor():SetAbil(1105, iWins)
	g_pilotMan:GetPlayerActor():SetDuelWinnerTitle()
end