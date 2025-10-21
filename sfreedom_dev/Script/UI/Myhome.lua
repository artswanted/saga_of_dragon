-- 마이홈 옵션 체크 버튼 UI 이름
g_kMyHomeOptionTbl = {}
g_kMyHomeOptionTbl[1] =	{ ["FRM"] = "CBTN_PUBLIC_HOME" }
g_kMyHomeOptionTbl[2] =	{ ["FRM"] = "CBTN_PUBLIC_FRIEND" }
g_kMyHomeOptionTbl[3] =	{ ["FRM"] = "CBTN_PUBLIC_COUPLE" }
g_kMyHomeOptionTbl[4] =	{ ["FRM"] = "CBTN_PUBLIC_GUILD"}

function MyHome_Selected_Tab_Change(ParentWnd, iTabNo)
	if ParentWnd:IsNil() == true then return end
	
	for idx = 1, 6 do
		local ChildWnd = ParentWnd:GetControl("CBTN_TAB"..idx)
		if ChildWnd:IsNil() ~= true then
			if iTabNo == idx then
				ChildWnd:CheckState(true)
			else
				ChildWnd:CheckState(false)
			end
		end
	end
end

function MyHome_Selected_Slot_Change(ParentWnd, iPlus)
	if ParentWnd:IsNil() == true then return end
	
	local ChildWnd = ParentWnd:GetControl("FRM_ITEM_SLOT")
	if ChildWnd:IsNil() ~= true then
		local idx = tonumber(ChildWnd:GetStaticText():GetStr()) + iPlus
		if 1 > idx then
			idx = 9
		elseif 9 < idx then
			idx = 1
		end
		ChildWnd:SetStaticText(tostring(idx))
	end
end

function MyHome_Tab_Inv_Set(UISelf, iTabNo)
	if UISelf:IsNil() == true then return end
	
	local TabParentWnd = UISelf:GetControl("FRM_HOME_ITEM_BAG_TAB")
	if TabParentWnd:IsNil() ~= true then
		MyHome_Selected_Tab_Change(TabParentWnd, iTabNo)
	end
	
	local InvParentWnd = UISelf:GetControl("FRM_HOME_ITEM_BAG")
	if InvParentWnd:IsNil() ~= true then
		local ChildWnd = InvParentWnd:GetControl("FRM_ITEM_SLOT")
		if ChildWnd:IsNil() ~= true then
			ChildWnd:SetStaticText(tostring(1)) -- 숫자를 초기 지정 슬롯으로
		end	
	end
end

function MyHome_Option_ChkBtn_OnOff(kParentWnd, iOptionNum, bOn)
	-- 테이블 범위 체크 
	if (table.getn(g_kMyHomeOptionTbl) < iOptionNum) then return end
	if kParentWnd:IsNil() == true then return end
	
	local kChkBtn = g_kMyHomeOptionTbl[iOptionNum]["FRM"]
	local kOnBtn = kChkBtn.."_ON"
	local kOffBtn = kChkBtn.."_OFF"

	kParentWnd:GetControl(kOnBtn):CheckState(bOn)
	kParentWnd:GetControl(kOffBtn):CheckState(not bOn)

	if iOptionNum == 1 then
		for i = 2, 4 do
			local kOnBtn = kParentWnd:GetControl(g_kMyHomeOptionTbl[i]["FRM"].."_ON")
			if kOnBtn:IsNil() then return end
			local kOffBtn = kParentWnd:GetControl(g_kMyHomeOptionTbl[i]["FRM"].."_OFF")
			if kOffBtn:IsNil() then return end
			kOnBtn:Disable(bOn);
			kOffBtn:Disable(bOn);
		end	
	end	
end

function MyHome_DecoBuf_Particle(iCnt)
	if(0 >= iCnt) then return end
	local kMyActor = GetMyActor()	
	if nil==kMyActor or kMyActor:IsNil() then
		return
	end	
	-- 데코레이션 아이템 버프 받을때 파티클 붙임
	kMyActor:AttachParticleWithRotate(10002, "char_root", "ef_Myroom_Buf_01_B", 1, false)	
	if( 1 < iCnt) then	
		--kMyActor:AttachParticleWithRotate(10001, "char_root", "ef_Myroom_Buf_02_G", 1.2, false)
		kMyActor:AttachParticleWithRotate(10001, "char_root", "ef_Myroom_Buf_02_G", 1, false)
	end
	if( 2 < iCnt) then
		--kMyActor:AttachParticleWithRotate(10002, "char_root", "ef_Myroom_Buf_01_B", 1.4, false)
		kMyActor:AttachParticleWithRotate(10003, "char_root", "ef_Myroom_Buf_03_Y", 1, false)
	end
	if( 3 < iCnt) then
		kMyActor:AttachParticleWithRotate(10000, "char_root", "ef_Myroom_Buf_04_R", 1, false)
		--kMyActor:AttachParticleWithRotate(10003, "char_root", "ef_Myroom_Buf_03_Y", 1.6, false)
	end	
end

function MyHome_GetTaxAxis()
	if(GetLocale() == LOCALE.NC_TAIWAN) then -- 대만은 사용하지 않음
		return 0
	end
	local iGoldUnit = 10000	-- 골드 단위
	return 300*iGoldUnit	--300 골드
end

function MyHome_GetUnderTaxAxisPayment()
	if(GetLocale() == LOCALE.NC_TAIWAN) then -- 대만은 사용하지 않음
		return 0
	end
	local iGoldUnit = 10000	-- 골드 단위
	return 30*iGoldUnit		-- 30 골드
end

g_HomeChatColor = {}
g_HomeChatColor[0] = 4294938185
g_HomeChatColor[1] = 4286316774
g_HomeChatColor[2] = 4278208230
g_HomeChatColor[3] = 4278229162
g_HomeChatColor[4] = 4278224444
g_HomeChatColor[5] = 4291925370
g_HomeChatColor[6] = 4291788031
g_HomeChatColor[7] = 4284781567
g_HomeChatColor[8] = 4281193885
g_HomeChatColor[9] = 4289258535
g_HomeChatColor[10] = 4294952688
g_HomeChatColor[11] = 4292328191
g_HomeChatColor[12] = 4289907455
g_HomeChatColor[13] = 4288804351
g_HomeChatColor[14] = 4289396676
g_HomeChatColor[15] = 4283249683
g_HomeChatColor[16] = 4294945458
g_HomeChatColor[17] = 4294948494
g_HomeChatColor[18] = 4294966428
g_HomeChatColor[19] = 4292607866

function SetMyhomeChatColor(idx)
	local ChatWnd = GetUIWnd("SFRM_MYHOME_CHAT_ALL_CHAT")
	if ChatWnd:IsNil() == true then
		ChatWnd = GetUIWnd("SFRM_MYHOME_CHAT_ROOM_CHAT")
	end
	
	if ChatWnd:IsNil() == true then
		return
	end
	
	SetMyhomeChatFontColor(g_HomeChatColor[idx])
	local Btn = ChatWnd:GetControl("BTN_TEXT_COLOR")
	if false == Btn:IsNil() then
		local pChild = Btn:GetControl("FRM_COLOR_VIEW")
		if false == pChild:IsNil() then
			pChild:VOnCall()
		end
	end		
end

function AddMyhomeEmoticon(idx)
	
	local ChatWnd = GetUIWnd("SFRM_MYHOME_CHAT_ALL_CHAT")
	if ChatWnd:IsNil() == true then
		ChatWnd = GetUIWnd("SFRM_MYHOME_CHAT_ROOM_CHAT")
	end
	if ChatWnd:IsNil() == true then
		return
	end
	local pShd = ChatWnd:GetControl("SFRM_BODY_SHD2")
	if false == pShd:IsNil() then
		local pEditWnd = pShd:GetControl("MEDT_COMMENT_WRITE")
		if false == pEditWnd:IsNil() then
			local curText = pEditWnd:GetEditText()
			local iNeedLength = 2;
			if idx < 10 then
				iNeedLength = 2
			elseif idx < 100 then
				iNeedLength = 3
			elseif idx < 1000 then
				iNeedLength = 4
			else
				SetBreak()
			end
				
			if (curText:Length() + iNeedLength) <= pEditWnd:GetEditLimitLength() then
				ODS("edit control has "..pEditWnd:GetEditLimitLength().." cur: "..curText:Length().."\n", 0, 9999)
				pEditWnd:SetEditText(pEditWnd:GetEditText():GetStr().."#"..idx)
				pEditWnd:SetEditFocus(true)
			end
		end
	end
end

function CallMyHomeChatMemberPopup(kGuid)
	if true == g_pilotMan:IsMyPlayer(kGuid) then
		return
	end
	local kPopupItem = {}
	kPopupItem[1] = "MYHOME_CHAT_WHISPER"
	kPopupItem[2] = "MYHOME_CHAT_ADD_FRIEND"
	kPopupItem[3] = "MYHOME_CHAT_INVITATION"
	if true == IsMyRoomMasterChatRoom() then
		kPopupItem[4] = "MYHOME_CHAT_ROOM_MASTER"
		kPopupItem[5] = "MYHOME_CHAT_KICK_GUEST"
	end
	local kCursorPos = GetCursorPos()

	CallCommonPopup(kPopupItem, kGuid, nil, kCursorPos)
	ODS("CallMyHomeChatMemberPopup\n",false,987)
end

function CallMyHomeChatRoomListPopup(kGuid)
	local kPopupItem = {}
	kPopupItem[1] = "MYHOME_CHAT_ROOMLIST_WHISPER"
	kPopupItem[2] = "MYHOME_CHAT_ROOMLIST_ADD_FRIEND"
	
	local kCursorPos = GetCursorPos()

	CallCommonPopup(kPopupItem, kGuid, nil, kCursorPos)
	ODS("CallMyHomeChatRoomListPopup\n",false,987)
end

--DoFile("UI/Myhome.lua")