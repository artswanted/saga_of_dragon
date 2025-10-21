function Net_PT_M_C_NFY_MAP_MOVING( packet )
	local iGroundNo = packet:PopInt()
	local kAttr = GetMapAttrByNo( iGroundNo )
	
	g_iNextCount = 0 -- NextMiniCard
	g_iNowCardNum = 0 --01BASE
	g_iNowCheckedCardNum = 0 -- 보이는 카드 기준
				
	local kUI = CallUI("FRM_INDUN_JOIN_REQUEST", true)
	if false == kUI:IsNil() then
		local kText = kUI:GetControl("SFRM_TEXT")
		if false == kText:IsNil() then
			if kAttr == GATTR_MISSION 
			or kAttr == GATTR_BOSS
			or kAttr == GATTR_CHAOS_MISSION
			or kAttr == GATTR_INSTANCE
			then
				kText:SetStaticTextW(GetTextW(19001))
			else
				kText:SetStaticTextW(GetTextW(18999))
			end
		end
	end
	
	LockPlayerInputMove(3) -- for map moving process lock
	LockPlayerInput(3)

	CloseMissionEnterUI()
end

function Net_PT_M_C_NFY_FAILED_ENTER_INDUN(packet)
	local kType = packet:PopByte()
	
	CloseUI("FRM_INDUN_JOIN_REQUEST")
	CloseUI("FRM_MISSION_JOIN_REQUEST")
	
	if kType == PORTAL_WAIT_FAILED then
		CommonMsgBoxByTextTable( 10, true )
	elseif kType == PORTAL_WAIT_TIMEOUT then
		CommonMsgBoxByTextTable( 155, true )
	end
	UnLockPlayerInputMove(3) -- for map moving process lock
	UnLockPlayerInput(3)
end

function Net_Req_C_C_NFY_CANCEL_ENTER_INDUN()
	packet = NewPacket(12552)
	packet:PushGuid(g_playerInfo.guidPilot)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_M_C_NFY_WILLBE_CLOSE_INDUN(packet)
	leftMin = packet:PopInt()
	SiegeMsgBox(GetTextW(19002):GetStr())
end

function Net_PT_M_C_NFY_USER_MAPMOVE_CHECK(packet)
	ODS("Net_PT_M_C_NFY_USER_MAPMOVE_CHECK 시작 \n")
	iReturn = packet:PopInt()
	ODS("iReturn : " .. iReturn .. "\n")
	if iReturn ~= 0 then
		if iReturn == 1 then	-- 레벨 제한 more
			iLevel = packet:PopShort()
			SiegeMsgBox(GetTextW(19006):GetStr() .. tostring(iLevel) .. GetTextW(19007):GetStr())
		elseif iReturn == 2 then	-- less
			iLevel = packet:PopShort()
			SiegeMsgBox(GetTextW(19006):GetStr() .. tostring(iLevel) .. GetTextW(19008):GetStr())
		elseif iReturn == 3 then
			iItemCount = packet:PopInt()
			wstrTotal = WideString("")
			for i = 1, iItemCount do
				iItemNum = packet:PopInt()
				ODS("iItemNum : " .. iItemNum .. "\n")
				wstrName = GetItemName(iItemNum)
				wstrTotal = GetAppendTextW(wstrTotal, wstrName)
				szNum = tostring(packet:PopInt())
				wstrTotal = GetAppendTextW(wstrTotal, WideString(" " .. szNum .. GetTextW(50413):GetStr() .. "\n"))
			end
			SiegeMsgBox(GetTextW(19004):GetStr() .. wstrTotal:GetStr() .. GetTextW(19005):GetStr())
		elseif iReturn == 4 then
			SiegeMsgBox(GetTextW(19009):GetStr())
		end
	else
		CallUI("FRM_INDUN_JOIN_REQUEST")
	end
	ODS("Net_PT_M_C_NFY_USER_MAPMOVE_CHECK 끝 \n")
end

function PT_C_M_REQ_INDUN_START()
	g_iStage = 4
	local kPacket = NewPacket(13201) -- PT_C_M_REQ_INDUN_START
	Net_Send(kPacket)
	DeletePacket(kPacket)
	return true
end

function PT_C_M_REQ_INDUN_START2()
	g_iStage = 4
	local kPacket = NewPacket(13204) -- PT_C_M_REQ_INDUN_START2
	Net_Send(kPacket)
	DeletePacket(kPacket)
	return true
end