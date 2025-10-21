local g_kChat = {}
--UI Set
g_kChat.kChatTable = {}
g_kChat.kChatTable[1] = {true, false, false, false, false, false, false}
g_kChat.kChatTable[2] = {false, true, false, false, false, false, false}
g_kChat.kChatTable[3] = {false, false, true, false, false, false, false}
g_kChat.kChatTable[4] = {false, false, false, true, false, false, false}
g_kChat.kChatTable[5] = {false, false, false, false, true, false, false}
g_kChat.kChatTable[6] = {false, false, false, false, false, true, false}
g_kChat.kChatTable[7] = {false, false, false, false, false, false, true}
--Filter set
--[[채팅 필터<070619 육정범>
	CT_Error			= 0,//에러 이벤트 타입
	CT_None				= 1,//
	CT_Normal			= 2//일반대화
	CT_Party			= 3,//파티대화
	CT_Whisper_ByName		= 4,//귓말 (이름으로)
	CT_Friend			= 5,//친구대화
	CT_Guild			= 6,//길드대화
	CT_TEAM				= 7,//TeamChat
	CT_Max				= 8,//
	CT_Whisper_ByGuid		=9,//귓말(Guid로)
	CT_Event			=10,//친구, 파티초대, 길드 등에 이벤트 메시지
	CT_Event_System			=11,//서버/클라이언트 시스템 이벤트 메시지(서버가 종료되었습니다, 연결이 종료 되었습니다.)
	CT_Event_Game			=12,//게임 메시지(공격, 스킬, ..)
	CT_Notice			=13,//공지
	CT_Battle			=14,//전투
	CT_Item				=15,//아이템 습득/파기 메시지
	CT_Trade			=39, // 거래
]]--

g_kChat.kChatModeSet = {}
g_kChat.kChatModeSet[7] = {true, false, 400273}--Team
g_kChat.kChatModeSet[6] = {true, false, 400156}--Guild
g_kChat.kChatModeSet[5] = {true, false, 400154}--Friend
g_kChat.kChatModeSet[4] = {false, true, 400157}--Whisper
g_kChat.kChatModeSet[3] = {true, false, 400155}--Party
--g_kChat.kChatModeSet[2] = {true, false, 400153}--All

--g_kChat.kChatFilter[ nTabNo ] = { ChatTypeNo, ... }
g_kChat.kChatFilter = {}
--g_kChat.kChatFilter[1] = {CT_Normal, CT_Friend, CT_Party, CT_Guild, CT_Battle, CT_Item, CT_Gold, CT_Exp, CT_Whisper_ByName, CT_TEAM, CT_Whisper_ByGuid, CT_Event, CT_Event_System, CT_Event_Game, CT_Error, CT_Notice, CT_MegaPhone_CH, CT_MegaPhone_SV, CT_ERROR_NOTICE,CT_NORMAL_NOTICE, CT_NOTICE1, CT_NOTICE2, CT_NOTICE3, CT_LOVE_SMS} --All
g_kChat.kChatFilter[1] = {CT_Normal, CT_TEAM, CT_Friend, CT_Whisper_ByGuid, CT_Whisper_ByName, CT_Notice, CT_MegaPhone_CH, CT_MegaPhone_SV, CT_LOVE_SMS} --Chating
g_kChat.kChatFilter[2] = {CT_Guild, CT_TEAM, CT_Friend, CT_Whisper_ByGuid, CT_Whisper_ByName,CT_Notice, CT_MegaPhone_CH, CT_MegaPhone_SV, CT_LOVE_SMS} --Party Message
g_kChat.kChatFilter[3] = {CT_Party, CT_TEAM, CT_Friend, CT_Whisper_ByGuid, CT_Whisper_ByName,CT_Notice, CT_MegaPhone_CH, CT_MegaPhone_SV, CT_LOVE_SMS} --Guild Message
g_kChat.kChatFilter[4] = {CT_Trade, CT_Friend, CT_Notice, CT_MegaPhone_CH, CT_MegaPhone_SV, CT_LOVE_SMS} --Trade
g_kChat.kChatFilter[5] = {CT_Event, CT_Event_System, CT_Event_Game, CT_Error, CT_Notice, CT_ERROR_NOTICE,CT_NORMAL_NOTICE, CT_NOTICE1, CT_NOTICE2, CT_NOTICE3, CT_LOVE_SMS} --System

g_kChat.kChatLogWnd = {}
g_kChat.kChatLogWnd[1] = {["LIST"]="LIST_CHAT_NORMAL",["BTN"]="BTN_CHATFILTER_CHAT" }
g_kChat.kChatLogWnd[2] = {["LIST"]="LIST_CHAT_GUILD",["BTN"]="BTN_CHATFILTER_GUILD" }
g_kChat.kChatLogWnd[3] = {["LIST"]="LIST_CHAT_PARTY",["BTN"]="BTN_CHATFILTER_PARTY" }
g_kChat.kChatLogWnd[4] = {["LIST"]="LIST_CHAT_TRADE",["BTN"]="BTN_CHATFILTER_TRADE" }
g_kChat.kChatLogWnd[5] = {["LIST"]="LIST_CHAT_SYSTEM",["BTN"]="BTN_CHATFILTER_SYSTEM"}

local g_kChatOption = {}
g_kChatOption.kChatMode = {}
g_kChatOption.kChatMode[1] = {["IDX"] = HEADKEY_CHATMODE_NORMAL, ["MODE"] = ChatFilter_Normal, ["LIST"] = "LIST_CHAT_NORMAL", ["SFRM"] = "SFRM_COLOR_NORMAL"}
g_kChatOption.kChatMode[2] = {["IDX"] = HEADKEY_CHATMODE_PARTY, ["MODE"] = ChatFilter_Party, ["LIST"] = "LIST_CHAT_PARTY", ["SFRM"] = "SFRM_COLOR_PARTY"}
g_kChatOption.kChatMode[3] = {["IDX"] = HEADKEY_CHATMODE_GUILD, ["MODE"] = ChatFilter_Guild, ["LIST"] = "LIST_CHAT_GUILD", ["SFRM"] = "SFRM_COLOR_GUILD"}
g_kChatOption.kChatMode[4] = {["IDX"] = HEADKEY_CHATMODE_TRADE, ["MODE"] = ChatFilter_Trade, ["LIST"] = "LIST_CHAT_TRADE", ["SFRM"] = "SFRM_COLOR_TRADE"}
g_kChatOption.kChatMode[5] = {["IDX"] = HEADKEY_CHATMODE_SYSTEM, ["MODE"] = ChatFilter_System, ["LIST"] = "LIST_CHAT_SYSTEM", ["SFRM"] = "SFRM_COLOR_SYSTEM"}

--g_kChatOption.kSystemInfo = {}
--g_kChatOption.kSystemInfo[1] = {["IDX"] = , ["VALUE"]}
--g_kChatOption.kSystemInfo[2] = {["IDX"] = , ["VALUE"]}
--g_kChatOption.kSystemInfo[3] = {["IDX"] = , ["VALUE"]}

g_kChatOption.kFilter = {}
g_kChatOption.kFilter[1] = {["IDX"] = SUBKEY_FILTER_NORMAL, ["FILTER"] = CT_Normal, ["BTN"] = "BTN_NORMAL"}
g_kChatOption.kFilter[2] = {["IDX"] = SUBKEY_FILTER_PARTY, ["FILTER"] = CT_Party, ["BTN"] = "BTN_PARTY"}
g_kChatOption.kFilter[3] = {["IDX"] = SUBKEY_FILTER_GUILD, ["FILTER"] = CT_Guild, ["BTN"] = "BTN_GUILD"}
g_kChatOption.kFilter[4] = {["IDX"] = SUBKEY_FILTER_TRADE, ["FILTER"] = CT_Trade, ["BTN"] = "BTN_TRADE"}
g_kChatOption.kFilter[5] = {["IDX"] = SUBKEY_FILTER_SYSTEM, ["FILTER"] = 0, ["BTN"] = "BTN_SYSTEM"}
g_kChatOption.kFilter[6] = {["IDX"] = SUBKEY_FILTER_SYSTEM_DAMAGE, ["FILTER"] = CT_Battle, ["BTN"] = "BTN_SYSTEM_DAMAGE"}
g_kChatOption.kFilter[7] = {["IDX"] = SUBKEY_FILTER_SYSTEM_ITEM, ["FILTER"] = CT_Item, ["BTN"] = "BTN_SYSTEM_ITEM"}
g_kChatOption.kFilter[8] = {["IDX"] = SUBKEY_FILTER_SYSTEM_GOLD, ["FILTER"] = CT_Gold, ["BTN"] = "BTN_SYSTEM_GOLD"}
g_kChatOption.kFilter[9] = {["IDX"] = SUBKEY_FILTER_SYSTEM_EXP, ["FILTER"] = CT_Exp, ["BTN"] = "BTN_SYSTEM_EXP"}

--UI                    
g_kChat.bShowHistory = true
g_kChat.bShowSysHistory = true

g_NotifyPartyFunction = true

function Init_LogWnd_FilterSet()
	--Filter Set
	g_ChatMgrClient:ChatFilter_ModeClear()
	for kKey, kVal in pairs(g_kChat.kChatFilter) do
		for kKey2, kVal2 in pairs(kVal) do
			local kWndName = g_kChat.kChatLogWnd[kKey]["LIST"]
			g_ChatMgrClient:ChatFilter_ModeAdd(kKey, kVal2, kWndName)
		end
	end
	SetSysoutOption(GetUIWnd("SFRM_CHAT_OPTION"), 1)
	CloseUI("SysChatOut")
end

--////////////////////////////////////  ////////////////////////////////////////////
function Init_ChatFilter()
	for idxChatMode = 1, 4 do
		for idxFilter = 1, 5 do
			local iSet = Config_GetValue(g_kChatOption.kChatMode[idxChatMode]["IDX"], g_kChatOption.kFilter[idxFilter]["IDX"])
			if 0 == iSet then
				iSet = Config_GetDefaultValue(g_kChatOption.kChatMode[idxChatMode]["IDX"], g_kChatOption.kFilter[idxFilter]["IDX"])
				Config_SetValue(g_kChatOption.kChatMode[idxChatMode]["IDX"], g_kChatOption.kFilter[idxFilter]["IDX"], iSet)
			end
			
			if SUBKEY_FILTER_SYSTEM == g_kChatOption.kFilter[idxFilter]["IDX"] then
			else
				if 2 == iSet then
					g_ChatMgrClient:ChatFilter_ModeAdd(g_kChatOption.kChatMode[idxChatMode]["MODE"], g_kChatOption.kFilter[idxFilter]["FILTER"], g_kChatOption.kChatMode[idxChatMode]["LIST"])
				else
					g_ChatMgrClient:ChatFilter_ModeDel(g_kChatOption.kChatMode[idxChatMode]["MODE"], g_kChatOption.kFilter[idxFilter]["FILTER"], g_kChatOption.kChatMode[idxChatMode]["LIST"])
				end
			end
		end
	end
	
	for idxFilter = 6, 9 do
		iSet = Config_GetValue(HEADKEY_CHATMODE_SYSTEM, g_kChatOption.kFilter[idxFilter]["IDX"])
		if 0 == iSet then
			iSet = Config_GetDefaultValue(ChatFilter_System, g_kChatOption.kFilter[idxFilter]["IDX"])
			Config_SetValue(HEADKEY_CHATMODE_SYSTEM, g_kChatOption.kFilter[idxFilter]["IDX"], iSet)
		end
		if 2 == iSet then
			g_ChatMgrClient:ChatFilter_ModeAdd(ChatFilter_System, g_kChatOption.kFilter[idxFilter]["FILTER"], "LIST_CHAT_SYSTEM")
			SystemFilter_Modify(g_kChatOption.kFilter[idxFilter]["FILTER"], 0)
		else
			g_ChatMgrClient:ChatFilter_ModeDel(ChatFilter_System, g_kChatOption.kFilter[idxFilter]["FILTER"], "LIST_CHAT_SYSTEM")
			SystemFilter_Modify(g_kChatOption.kFilter[idxFilter]["FILTER"], 1)
		end
	end
	
	SetSysoutOption(GetUIWnd("SFRM_CHAT_OPTION"), 1)
	
	--ODS("Init_ChatFilter()\n", false, 927)
end
--////////////////////////////////////  ////////////////////////////////////////////
function ChatOption_Default(kParent)	--채팅 필터 옵션 초기화(현재UI, 채팅모드 번호)  
	for idxChatMode = 1, 4 do
		local kChatMode = kParent:GetControl(g_kChatOption.kChatMode[idxChatMode]["SFRM"])
		kChatMode = kChatMode:GetControl("SFRM_COLOR3")
		for idxFilter = 1, 5 do
			local kFilter = kChatMode:GetControl(g_kChatOption.kFilter[idxFilter]["BTN"])
			iSet = Config_GetDefaultValue(g_kChatOption.kChatMode[idxChatMode]["IDX"], g_kChatOption.kFilter[idxFilter]["IDX"])
			Config_SetValue(g_kChatOption.kChatMode[idxChatMode]["IDX"], g_kChatOption.kFilter[idxFilter]["IDX"], iSet)
			if 2 == iSet then
				kFilter:CheckState(true)
			elseif 1 == iSet then
				kFilter:CheckState(false)
			else
				kFilter:Disable(true)
			end
			if SUBKEY_FILTER_SYSTEM == g_kChatOption.kFilter[idxFilter]["IDX"] then
			else
				if 2 == iSet then
					g_ChatMgrClient:ChatFilter_ModeAdd(g_kChatOption.kChatMode[idxChatMode]["MODE"], g_kChatOption.kFilter[idxFilter]["FILTER"], g_kChatOption.kChatMode[idxChatMode]["LIST"])
				elseif 1 == iSet then
					g_ChatMgrClient:ChatFilter_ModeDel(g_kChatOption.kChatMode[idxChatMode]["MODE"], g_kChatOption.kFilter[idxFilter]["FILTER"], g_kChatOption.kChatMode[idxChatMode]["LIST"])
				end
			end
		end
	end	
	
	local kSystemOption = kParent:GetParent():GetControl("SFRM_SYSTEM_BG_SHADOW"):GetControl("SFRM_WINDOW_FILTER")
	kSystemOption = kSystemOption:GetControl("SFRM_COLOR3")
	for idxFilter = 6, 9 do
		local kFilter = kSystemOption:GetControl(g_kChatOption.kFilter[idxFilter]["BTN"])
		iSet = Config_GetDefaultValue(HEADKEY_CHATMODE_SYSTEM, g_kChatOption.kFilter[idxFilter]["IDX"])
		Config_SetValue(HEADKEY_CHATMODE_SYSTEM, g_kChatOption.kFilter[idxFilter]["IDX"], iSet)
		if 2 == iSet then
			kFilter:CheckState(true)
		elseif 1 == iSet then
			kFilter:CheckState(false)
		else
			kFilter:Disable(true)
		end
		if 2 == iSet then
			g_ChatMgrClient:ChatFilter_ModeAdd(ChatFilter_System, g_kChatOption.kFilter[idxFilter]["FILTER"], "LIST_CHAT_SYSTEM")
			SystemFilter_Modify(g_kChatOption.kFilter[idxFilter]["FILTER"], 0)
		else
			g_ChatMgrClient:ChatFilter_ModeDel(ChatFilter_System, g_kChatOption.kFilter[idxFilter]["FILTER"], "LIST_CHAT_SYSTEM")
			SystemFilter_Modify(g_kChatOption.kFilter[idxFilter]["FILTER"], 1)
		end
	end
	
	SetSysoutOption(GetUIWnd("SFRM_CHAT_OPTION"), 0)
	g_ChatMgrClient:SetSysChatOutSnap(true)
	Config_SetValue(HEADKEY_USER_UI, SUBKEY_CHATLOG_SHOW, 1)
	ChatSetPreset(1)
	if false == g_AutoHideOption then
		GetEventTimer():Add("ChatOutHide", 3.0, "OnOver_ChatHistory(GetUIWnd('ChatOut'), false)", 1, false)
	end
end

function SetSysoutOption(kParent, Default)
	local iSet
	if 0 == Default then
		iSet = Config_GetDefaultValue(HEADKEY_SYSTEMOUT_OPTION, SUBKEY_SYSTEMOUT_WINDOW)
		Config_SetValue(HEADKEY_SYSTEMOUT_OPTION, SUBKEY_SYSTEMOUT_WINDOW, iSet)
	else
		iSet = Config_GetValue(HEADKEY_SYSTEMOUT_OPTION, SUBKEY_SYSTEMOUT_WINDOW)
	end 
	local BntOn = kParent:GetControl("SFRM_SYSTEM_BG_SHADOW"):GetControl("SFRM_SYSTEM_WINDOW"):GetControl("SFRM_COLOR3"):GetControl("BTN_ON")
	local BtnOff = kParent:GetControl("SFRM_SYSTEM_BG_SHADOW"):GetControl("SFRM_SYSTEM_WINDOW"):GetControl("SFRM_COLOR3"):GetControl("BTN_OFF")
	if 2 == iSet then
		GetUIWnd("SysChatOut"):Visible(true)
		g_ChatMgrClient:SetSysOutHide(false)
		if 2 == Default then
			BtnOff:CheckState(false)
		elseif 0 == Default then
			BntOn:CheckState(true)
			BtnOff:CheckState(false)
		else
			CloseUI("SFRM_CHAT_OPTION")
		end
	else
		GetUIWnd("SysChatOut"):Visible(false)
		g_ChatMgrClient:SetSysOutHide(true)
		if 2 == Default then
			BntOn:CheckState(false)
		elseif 0 == Default then
			BtnOff:CheckState(true)
			BntOn:CheckState(false)
		else
			CloseUI("SFRM_CHAT_OPTION")
		end
	end
	if 0 == Default then
		iSet = Config_GetDefaultValue(HEADKEY_SYSTEMOUT_OPTION, SUBKEY_SYSTEMOUT_EXTEND)
		Config_SetValue(HEADKEY_SYSTEMOUT_OPTION, SUBKEY_SYSTEMOUT_EXTEND, iSet)
	else
		iSet = Config_GetValue(HEADKEY_SYSTEMOUT_OPTION, SUBKEY_SYSTEMOUT_EXTEND)
	end
	local BtnBasic = kParent:GetControl("SFRM_SYSTEM_BG_SHADOW"):GetControl("SFRM_WINDOW_EXTEND"):GetControl("SFRM_COLOR3"):GetControl("BTN_BASIC")
	local BtnExtend = kParent:GetControl("SFRM_SYSTEM_BG_SHADOW"):GetControl("SFRM_WINDOW_EXTEND"):GetControl("SFRM_COLOR3"):GetControl("BTN_EXTEND")
	if 2 == iSet then
		if 2 == Default then
			BtnExtend:CheckState(false)
		elseif 0 == Default then
			BtnBasic:CheckState(true)
			BtnExtend:CheckState(false)
		end
		SysOutSetPreset(1)
	else
		if 2 == Default then
			BtnBasic:CheckState(false)
		elseif 0 == Default then
			BtnExtend:CheckState(true)
			BtnBasic:CheckState(false)
		end
		SysOutSetPreset(2)
	end
	if 0 == Default then
		iSet = Config_GetDefaultValue(HEADKEY_SYSTEMOUT_OPTION, SUBKEY_SYSTEMOUT_HIDE)
		Config_SetValue(HEADKEY_SYSTEMOUT_OPTION, SUBKEY_SYSTEMOUT_HIDE, iSet)
	else
		iSet = Config_GetValue(HEADKEY_SYSTEMOUT_OPTION, SUBKEY_SYSTEMOUT_HIDE)
	end
	BntOn = kParent:GetControl("SFRM_CHATHIDE_BG_SHADOW"):GetControl("BTN_ON")
	BtnOff = kParent:GetControl("SFRM_CHATHIDE_BG_SHADOW"):GetControl("BTN_OFF")
	if 2 == iSet then
		if 2 == Default then
			BtnOff:CheckState(false)
		elseif 0 == Default then
			BntOn:CheckState(true)
			BtnOff:CheckState(false)
		end
		AutoHide(false)
	else
		if 2 == Default then
			BntOn:CheckState(false)
		elseif 0 == Default then
			BtnOff:CheckState(true)
			BntOn:CheckState(false)
		end
		AutoHide(true)
	end
end
--////////////////////////////////////  ////////////////////////////////////////////
function ChatOption_CheckFromSystem(kParent)	-- 채팅 필터 옵션 - 시스템 버튼 클릭 시 호출
	local kSystem = kParent:GetControl("BTN_SYSTEM")
	if true == kSystem:IsNil() then
		return true
	end
	local kDamage = kParent:GetControl("BTN_SYSTEM_DAMAGE")
	if true == kDamage:IsNil() then
		return true
	end
	local kItem = kParent:GetControl("BTN_SYSTEM_ITEM")
	if true == kItem:IsNil() then
		return true
	end
	local kGold = kParent:GetControl("BTN_SYSTEM_GOLD")
	if true == kGold:IsNil() then
		return true
	end
	local kExp = kParent:GetControl("BTN_SYSTEM_EXP")
	if true == kExp:IsNil() then
		return true
	end
	
	for idxChatMode = 1, 5 do
--		if kParent:GetParent():GetID() ==  then
		if g_kChatOption.kChatMode[idxChatMode]["SFRM"] == kParent:GetParent():GetID():GetStr() then
			local kWndName = g_kChatOption.kChatMode[idxChatMode]["LIST"]
			local kKey = g_kChatOption.kChatMode[idxChatMode]["IDX"]
			local kMode = g_kChatOption.kChatMode[idxChatMode]["MODE"]	
			if true == kSystem:GetCheckState() then		-- 상위 시스템 버튼이 꺼졌을 때,
				kDamage:CheckState(false)
				Config_SetValue(kKey, SUBKEY_FILTER_SYSTEM_DAMAGE, 1)
				g_ChatMgrClient:ChatFilter_ModeDel(kMode, CT_Battle, kWndName)
				kItem:CheckState(false)
				Config_SetValue(kKey, SUBKEY_FILTER_SYSTEM_ITEM, 1)
				g_ChatMgrClient:ChatFilter_ModeDel(kMode, CT_Item, kWndName)
				kGold:CheckState(false)
				Config_SetValue(kKey, SUBKEY_FILTER_SYSTEM_GOLD, 1)
				g_ChatMgrClient:ChatFilter_ModeDel(kMode, CT_Gold, kWndName)
				kExp:CheckState(false)
				Config_SetValue(kKey, SUBKEY_FILTER_SYSTEM_EXP, 1)	--하위 시스템버튼 모두 OFF.
				g_ChatMgrClient:ChatFilter_ModeDel(kMode, CT_Exp, kWndName)
			else	-- 상위 시스템 버튼이 켜졌을 때,
				if not kDamage:GetCheckState() 
				and not kItem:GetCheckState() 
				and not kGold:GetCheckState()
				and not kExp:GetCheckState() then	--하위 시스템 버튼이 모두 Off면 전부 On.
					kDamage:CheckState(true)
					Config_SetValue(kKey, SUBKEY_FILTER_SYSTEM_DAMAGE, 2)
					g_ChatMgrClient:ChatFilter_ModeAdd(kMode, CT_Battle, kWndName)
					kItem:CheckState(true)
					Config_SetValue(kKey, SUBKEY_FILTER_SYSTEM_ITEM, 2)
					g_ChatMgrClient:ChatFilter_ModeAdd(kMode, CT_Item, kWndName)
					kGold:CheckState(true)
					Config_SetValue(kKey, SUBKEY_FILTER_SYSTEM_GOLD, 2)
					g_ChatMgrClient:ChatFilter_ModeAdd(kMode, CT_Gold, kWndName)
					kExp:CheckState(true)
					Config_SetValue(kKey, SUBKEY_FILTER_SYSTEM_EXP, 2)
					g_ChatMgrClient:ChatFilter_ModeAdd(kMode, CT_Exp, kWndName)
				end
			end
			return true
		end
	end
end
--////////////////////////////////////  ////////////////////////////////////////////
function ChatOption_CheckFromSystemElement(kParent)	-- 채팅 필터 옵션 - 하위 시스템 버튼 클릭 시 호출(데미지/아이템/골드/경험치)
	for idxChatMode = 1, 5 do
		if kParent:GetParent():GetID():GetStr() == g_kChatOption.kChatMode[idxChatMode]["SFRM"] then
			kWndName = g_kChatOption.kChatMode[idxChatMode]["LIST"]
			kKey = g_kChatOption.kChatMode[idxChatMode]["IDX"]
			kMode = g_kChatOption.kChatMode[idxChatMode]["MODE"]
			local kSystem = kParent:GetControl("BTN_SYSTEM")
			if not kSystem:IsNil() then
				local iCheckCount = 0
				for idxFilter = 5, 9 do
					local iSet = Config_GetValue(g_kChatOption.kChatMode[idxChatMode]["IDX"], g_kChatOption.kFilter[idxFilter]["IDX"])
					if 2 == iSet then
						iCheckCount = iCheckCount+1
					end
				end
				--버튼이 눌린 시점에(눌린 버튼은 상태가 바뀌기 전)
				if 0 == iCheckCount then	-- 하위 시스템 버튼이 전부 꺼져 있다면,
					kSystem:CheckState(false)
					Config_SetValue(kKey, SUBKEY_FILTER_SYSTEM, 1)	-- 상위 시스템 버튼 Off.
				elseif 1 <= iCheckCount then	-- 하위 시스템 버튼이 하나라도 켜져 있다면,
					kSystem:CheckState(true)
					Config_SetValue(kKey, SUBKEY_FILTER_SYSTEM, 2)	-- 상위 시스템 버튼 On
				end
				return true
			end
		end
	end
end
--////////////////////////////////////  ////////////////////////////////////////////
function ChatMode_FilterSet(kTopSelf, iSetNo)
	local kSet = g_kChat.kChatFilter[iSetNo]
	if nil == kSet then
		iSetNo = Config_GetValue(HEADKEY_USER_UI, SUBKEY_LAST_CHAT_TAB)
		kSet = g_kChat.kChatFilter[iSetNo]
		if nil == kSet then
			iSetNo = Config_GetValue(HEADKEY_USER_UI, SUBKEY_DEFAULT_CHAT_TAB)
			kSet = g_kChat.kChatFilter[iSetNo]
		end
	end
	Config_SetValue(HEADKEY_USER_UI, SUBKEY_LAST_CHAT_TAB, iSetNo)
	g_kSaveChatTapNo = iSetNo
	--UI Set
	local kParent = kTopSelf:GetParent()
	if not kParent:IsNil() then
		for idx = 1, 4 do
			local kWnd = kParent:GetControl(g_kChat.kChatLogWnd[idx]["LIST"])
			local kBtn = kTopSelf:GetControl(g_kChat.kChatLogWnd[idx]["BTN"])
			if nil == kWnd or kWnd:IsNil() or nil == kBtn or kBtn:IsNil() then
				return
			end
			kWnd:Visible(g_kChatOutSet ~= 0 and g_kChat.kChatTable[iSetNo][idx])
			kBtn:CheckState(g_kChat.kChatTable[iSetNo][idx])
		end
	end
end

--////////////////////////////////////  ////////////////////////////////////////////
function OnEnter_FocusEdit(kTop)
	if kTop:IsNil() then ODS("[OnEnter_FocusEdit] kTop is Nil\n") return end
	
	--[[
	local kMyPilot = g_pilotMan:GetPlayerPilot()
	if kMyPilot ~= nil and not kMyPilot:IsNil() then
		local kMyActor = kMyPilot:GetActor()
		if kMyActor ~= nil and not kMyActor:IsNil() then
			--kMyActor:ReserveTransitAction("a_idle")
		end
	end
	]]
	
	local kEdit = kTop:GetControl("EDT_CHAT")
	g_ChatMgrClient:ToggleConsecutiveChat()
	if false == kEdit:IsFocus() then
		ClearEditFocus()
		kEdit:SetEditFocus(false)
		OnDelTimer_HideChatBar()
		return true
	end
	return false
end

--////////////////////////////////////  ////////////////////////////////////////////
function OnAddTimer_HideChatBar()
	if IsSingleMode() then return end
	--GetEventTimer():Add("ChatBarHide", 30.0, "CloseChatBar()", 1, false)
end

function OnDelTimer_HideChatBar()
	if IsSingleMode() then return end
	GetEventTimer():Del("ChatBarHide")
end

function CloseChatBar()
	local kChatBarWnd = GetUIWnd("ChatBar")
	if kChatBarWnd:IsNil() then return end
	local kChatEditWnd = kChatBarWnd:GetControl("EDT_CHAT")
	if kChatEditWnd:IsNil() then return end
	
	local bCloseAble = true
	bCloseAble = bCloseAble and (STATUS_ROOM ~= GetContentsStatus())
	bCloseAble = bCloseAble and (not kChatEditWnd:IsFocus())
	if bCloseAble then
		kChatBarWnd:Close()
	else
		GetEventTimer():Add("ChatBarHide", 30.0, "CloseChatBar()", 1, false)
	end
end

--////////////////////////////////////  ////////////////////////////////////////////
function OnEnter_SendChat(kSelf)
	if kSelf:IsNil() then ODS("[OnEnter_SendChat] kSelf is nil\n") return end
	if true == kSelf:IsFocus() then
		
		local kChat = kSelf:GetEditText_TextBlockApplied()
		kSelf:SetEditText("")
		
		-- local kTop = GetUIWnd("ChatBar")
		-- if kTop:IsNil() then ODS("[OnEsc_CancalName] kTop is nil\n") return false end
		-- local kWspBG = kTop:GetControl("SRM_WSP_BG")
		-- if kWspBG:IsNil() then ODS("[OnEsc_CancalName] SRM_WSP_BG is nil\n") return false end
		-- local kNameEdit = kWspBG:GetControl("EDT_WSP")
		-- if kNameEdit:IsNil() then ODS("[OnEsc_CancalName] EDT_WSP is nil\n") return false end	
		-- local kName = kNameEdit:GetEditText()
		
		if true == g_ChatMgrClient:SendChatStyleString(kChat, true) then
			local isToggle = g_ChatMgrClient:GetToggleConsecutiveChat()
			if true ~= isToggle then
				ClearEditFocus()
				g_ChatMgrClient:ToggleConsecutiveChatUI()
			end
			g_ChatMgrClient:SendChat_InputNow(false)
			OnAddTimer_HideChatBar()
		end
		return true
	end
	return false
end

--////////////////////////////////////  ////////////////////////////////////////////
function OnEsc_CancalName()
	local kTop = GetUIWnd("ChatBar")
	if kTop:IsNil() then ODS("[OnEsc_CancalName] kTop is nil\n") return false end
	local kWspBG = kTop:GetControl("SRM_WSP_BG")
	if kWspBG:IsNil() then ODS("[OnEsc_CancalName] SRM_WSP_BG is nil\n") return false end
	local kNameEdit = kWspBG:GetControl("EDT_WSP")
	if kNameEdit:IsNil() then ODS("[OnEsc_CancalName] EDT_WSP is nil\n") return false end
	
	kNameEdit:SetEditText("")
	ClearEditFocus()
	OnAddTimer_HideChatBar()
	return true
end

--////////////////////////////////////  ////////////////////////////////////////////
function OnEsc_CancelChat()
	local kTop = GetUIWnd("ChatBar")
	if kTop:IsNil() then ODS("[OnEsc_CancelChat] kTop is nil\n") return false end
	local kEditChat = kTop:GetControl("EDT_CHAT")
	if kEditChat:IsNil() then ODS("[OnEsc_CancelChat] kSelf is nil\n") return false end
	
	g_ChatMgrClient:SendChat_InputNow(false)
	kEditChat:ResetTypingHistory()
	kEditChat:SetEditText("")
	ClearEditFocus()
	OnAddTimer_HideChatBar()
	return true
end

--////////////////////////////////////  ////////////////////////////////////////////
g_kChatHistoryLock = false
function SetLock_ChatHistory( bLock )
	g_kChatHistoryLock = bLock
end

function OnOver_SysChatHistory(kTop, bVisible)
	if true == g_kChatHistoryLock then
		return false
	end
	g_kChat.bShowSysHistory = bVisible
	kTop:GetControl("SYSCHAT_BG"):Visible(bVisible)
	return true
end

function OnOver_ChatHistory(kTop, bVisible)
	if true == g_kChatHistoryLock then
		return false
	end
	g_kChat.bShowHistory = bVisible
	kTop:GetControl("CHAT_BG"):Visible(bVisible)
	return true
end
--[[
				GetUIWnd("ChatBar"):GetControl("SRM_WSP_BG"):Visible(false)
				GetUIWnd("ChatBar"):GetControl("SFRM_TELLBTN_BG"):Visible(true)
				wnd = GetUIWnd("ChatBar"):GetControl("SFRM_TELLBTN_BG"):GetControl("BTN_TELL_TYPE")
				if wnd:IsNil() == false then
					wnd:SetStaticTextW(GetTextW(400153))
				end
				UISelf:GetParent():Close() 
				----------------------------------------------------------
				wnd = GetUIWnd("ChatBar"):GetControl("SFRM_TELLBTN_BG"):GetControl("BTN_TELL_TYPE")
				if wnd:IsNil() == false then
					wnd:SetStaticTextW(GetTextW(400157))
				end
				UISelf:GetParent():Close() 
				GetUIWnd("ChatBar"):GetControl("SRM_WSP_BG"):Visible(true)
				GetUIWnd("ChatBar"):GetControl("SFRM_TELLBTN_BG"):Visible(false)
]]
--////////////////////////////////////  ////////////////////////////////////////////
function OnClick_ChatMode_Change(iMode)
	local kChatBar = GetUIWnd("ChatBar")
	if kChatBar:IsNil() then ODS("Can't find ChatBar ui\n") return end
	
	local kWspBG = kChatBar:GetControl("SRM_WSP_BG")
	if kWspBG:IsNil() then ODS("can't find SRM_WSP_BG ui\n") return end
	local kTellBtnBG = kChatBar:GetControl("SFRM_TELLBTN_BG")
	if kTellBtnBG:IsNil() then ODS("can't find SFRM_TELLBTN_BG\n") return end
	
	local kSet = g_kChat.kChatModeSet[iMode]
	if nil == kSet then
		iMode = 2
		kSet = g_kChat.kChatModeSet[iMode]
	end
	
	ODS("CALL CHATMODE " .. iMode .. "\n", false, 1158)
	kTellBtnBG:Visible(kSet[1])
	kWspBG:Visible(kSet[2])
	
	local kTellBtn = kTellBtnBG:GetControl("BTN_TELL_TYPE")
	if not kTellBtn:IsNil() then
		kTellBtn:SetStaticTextW(GetTextW(kSet[3]))
	end
	ODS("CALL CHATMODE " .. iMode .. "\n", false, 1158)
	
	g_ChatMgrClient:ChatMode_Set(iMode)
	
	CloseUI("SFRM_TELL_TYPE")
end

--////////////////////////////////////  ////////////////////////////////////////////
--[[
g_kChat.LogWindowSet = {}
g_kChat.LogWindowSet[1] = true
g_kChat.LogWindowSet[2] = false
function OnClick_Chat_Show(bMode)
	local iMode = 0
	if 0 == iMode then
		iMode = Config_GetValue(HEADKEY_USER_UI, SUBKEY_CHATLOG_SHOW)
		if 0 == iMode then
			iMode = Config_GetValue(HEADKEY_USER_UI, SUBKEY_DEFAULT_CHATLOG_SHOW)
		end
	end
	
	if bMode then
		if 1 == iMode then
			iMode = 2
		elseif 2 == iMode then
			iMode = 1
		end
	end
	
	local kWnd = GetUIWnd("ChatOut")
	if g_kChat.LogWindowSet[iMode] then
		if kWnd:IsNil() then
			kWnd = ActivateUI("ChatOut")
		end
		kWnd:Visible(true)
	else
		if not kWnd:IsNil() then
			kWnd:Visible(false)
		end
	end
	
	Config_SetValue(HEADKEY_USER_UI, SUBKEY_CHATLOG_SHOW, iMode)
end
]]

function CloseCurrentMiniMap()
	ODS("CloseCurrentMiniMap\n");
	CloseUI("FRM_MINIMAP")
end

function ShowCurrentMiniMap(currentMinimap)
	local UIParent = GetUIWnd("FRM_MINIMAP")
	if true == UIParent:IsNil() then
		UIParent = ActivateUI("FRM_MINIMAP")
		if true == UIParent:IsNil() then
			return
		end
	end
	
	ShowCurrentMinimap2(UIParent, currentMinimap)
end
	
function ShowCurrentMinimap2(UIParent, currentMinimap)
	if UIParent:IsNil() then
		return
	end

	if currentMinimap == 0 or currentMinimap == nil then
		currentMinimap = Config_GetValue(HEADKEY_USER_UI, SUBKEY_CURRENT_MINIMAP)
	else
		Config_SetValue(HEADKEY_USER_UI, SUBKEY_CURRENT_MINIMAP, currentMinimap)
	end
	
	--[[if GATTR_BATTLESQUARE == g_world:GetAttr() then
		currentMinimap = 2
	end]]
	
	ODS("ShowCurrentMiniMap:"..currentMinimap.."\n");
	
	if currentMinimap == 1 then
		local pMiniMap = UIParent:GetControl("SFRM_MINIMAP")
		if false == pMiniMap:IsNil() then
			pMiniMap:Visible(true)
			pMiniMap:SetShowMiniMap(MINIMAP_MAIN, true)
		end
		local pMoney = UIParent:GetControl("SFRM_SUB_MENU")
		if false == pMoney:IsNil() then
			local kPos = pMoney:GetLocation()
			local kSize = UIParent:GetSize()
			kPos:SetY(191)
			kSize:SetY(250)
			pMoney:SetLocation(kPos)
			UIParent:SetSize(kSize)
		end
		local pTitle = UIParent:GetControl("SFRM_TITLEBAR")
		if false == pTitle:IsNil() then
			local pClose = pTitle:GetControl("BTN_CLOSE")
			if false == pClose:IsNil() then pClose:Visible(false) end
			
			local kOpen = pTitle:GetControl("BTN_OPEN")
			if false == kOpen:IsNil() then kOpen:Visible(true) end
		end
		
		local pFrmMission = GetUIWnd("FRM_MISSION_ENTER")
		if pFrmMission:IsNil() == false then
			pFrmMission:Visible(false)
		end
	elseif currentMinimap == 2 then
		local pMiniMap = UIParent:GetControl("SFRM_MINIMAP")
		if false == pMiniMap:IsNil() then
			pMiniMap:Visible(false)
			pMiniMap:SetShowMiniMap(MINIMAP_MAIN, false)
		end
		local pMoney = UIParent:GetControl("SFRM_SUB_MENU")
		if false == pMoney:IsNil() then
			local kPos = pMoney:GetLocation()
			local kSize = UIParent:GetSize()
			kPos:SetY(32)
			kSize:SetY(62)
			pMoney:SetLocation(kPos)
			UIParent:SetSize(kSize)
		end
		local pTitle = UIParent:GetControl("SFRM_TITLEBAR")
		if false == pTitle:IsNil() then
			local pClose = pTitle:GetControl("BTN_CLOSE")
			if false == pClose:IsNil() then pClose:Visible(true) end
			
			local kOpen = pTitle:GetControl("BTN_OPEN")
			if false == kOpen:IsNil() then kOpen:Visible(false) end
		end
		
		local pFrmMission = CallUI("FRM_MISSION_ENTER")
		if pFrmMission:IsNil() == false then
			local pBtnMission = pFrmMission:GetControl("BTN_MISSION_ENTER")
			if pBtnMission:IsNil() == false then --현재 맵에 미션 트리거가 있는/없는 경우에 미션버튼을 보임/숨김
				local kPos = pFrmMission:GetLocation()
				kPos:SetX(kPos:GetX() - 4)
				kPos:SetY(62)
				pFrmMission:SetLocation(kPos)
			
				if( CheckNil(g_world == nil) ) then return false end
				if( CheckNil(g_world:IsNil()) ) then return false end
				local kMissionTrigID = g_world:GetMissionTrigIDOnThisMap()
				if kMissionTrigID:IsNil() then
					pBtnMission:ClearCustomData()
					pBtnMission:Visible(false)
				else
					pBtnMission:SetCustomDataAsStr(kMissionTrigID:GetStr())
					pBtnMission:Visible(true)
				end
			end
		end
	end
	ShowRestExpIcon(IsMapFillRestExp(g_mapNo)) -- 휴식 경험치가 차는 맵이면 UI를 휴식경험치가 차는 UI를 띄어준다
end

g_MiniMapFlag = false
function MiniMapOpenToClose(Parent, bIsOpen)
	if not g_MiniMapFlag then
		local UIMap = Parent:GetControl("SFRM_MINIMAP")
		if UIMap:IsNil() ~= true then
			UIMap:Visible(bIsOpen)
			UIMap:SetShowMiniMap(MINIMAP_MAIN, bIsOpen)
		end
		local UIGold = Parent:GetControl("SFRM_SUB_MENU")
		if UIGold:IsNil() ~= true then
			local kPos = UIGold:GetLocation()
			local kSize = Parent:GetSize()
			if bIsOpen then
				kPos:SetY(191)
				kSize:SetY(250)		
			else
				kPos:SetY(32)
				kSize:SetY(62)
			end
			UIGold:SetLocation(kPos)
			Parent:SetSize(kSize)		
		end
	end
	if bIsOpen then
		g_MiniMapFlag = true
	else
		g_MiniMapFlag = false
	end
end

function MakeHotKeyText(iText, iKey)
	local szText = WideString(GetTextW(iText):GetStr() .. GetTextW(422000):GetStr() .. GetTextW(UKeyToKey(iKey) + 420000):GetStr())
	return szText
end

--///////////////////////// ChangeChatOutSize /////////////////////////
g_kChatOutSet = 1
g_kChatOutPreset = {}
g_kChatOutPreset[0] = {["MAIN_H"]=21, ["BG_H"]=0, ["LST_H"]=0}
g_kChatOutPreset[1] = {["MAIN_H"]=120, ["BG_H"]=98, ["LST_H"]=87}
g_kChatOutPreset[2] = {["MAIN_H"]=170, ["BG_H"]=148, ["LST_H"]=137}--Diff 50
g_kChatOutPreset[3] = {["MAIN_H"]=220, ["BG_H"]=198, ["LST_H"]=187}--DIff 50
g_kChatOutPreset[4] = {["MAIN_H"]=270, ["BG_H"]=248, ["LST_H"]=237}--DIff 50
g_kChatOutPreset[5] = {["MAIN_H"]=320, ["BG_H"]=298, ["LST_H"]=287}--Diff 50
function ChangeChatOutSize(iIncVal)
	if g_ChatMgrClient:CheckChatOut() then
		return
	end
	
    local iIndex = 0
	if 0 == iIncVal then
		g_kChatOutSet = Config_GetValue(HEADKEY_USER_UI, SUBKEY_CHATLOG_SHOW)
		if 0 == g_kChatOutSet then
			g_kChatOutSet = Config_GetValue(HEADKEY_USER_UI, SUBKEY_DEFAULT_CHATLOG_SHOW)
		end
	end
	
	iIndex = g_kChatOutSet + iIncVal
		
	if 0 < iIncVal then
		if nil == g_kChatOutPreset[iIndex] then
			iIndex = 0
		end
	elseif 0 > iIncVal then
		if nil == g_kChatOutPreset[iIndex] then
			iIndex = 5
		end
	end
	
	Config_SetValue(HEADKEY_USER_UI, SUBKEY_CHATLOG_SHOW, iIndex)
	
	--local kSizePreset = g_kChatOutPreset[g_kChatOutSet]
	ChatSetPreset(iIndex)
	if false == g_AutoHideOption then
		GetEventTimer():Add("ChatOutHide", 3.0, "OnOver_ChatHistory(GetUIWnd('ChatOut'), false)", 1, false)
	end
end

function ChatSetPreset(iIndex)
	g_kChatOutSet = iIndex
	local kSizePreset = g_kChatOutPreset[g_kChatOutSet]
	
	ActivateUI("ChatOut")
	
	--Start change size
	local kTopWnd = GetUIWnd("ChatOut")
	local kChatBar = GetUIWnd("ChatBar")
	if nil == kTopWnd or kTopWnd:IsNil() then return end
	if nil == kChatBar or kChatBar:IsNil() then return end
	local kDummy = kTopWnd:GetControl("DMY_CHAT")
	if nil == kDummy or kDummy:IsNil() then return end
	local kTopMenu = kTopWnd:GetControl("CHAT_BG")
	if nil == kTopMenu or kTopMenu:IsNil() then return end
	local kBGWnd = kTopMenu:GetControl("SFRM_CHATOUT_BG")
	
	for idx = 1, 4 do
		local kList = kTopWnd:GetControl(g_kChat.kChatLogWnd[idx]["LIST"])
		if nil == kList or kList:IsNil() then return end
		ListChangeSize(kSizePreset["LST_H"], kList)
		kList:Visible(iIndex ~= 0 and g_kSaveChatTapNo == idx);
	end
	
	--Bg
	local kBGSize = kBGWnd:GetSize()
	kBGSize:SetY( kSizePreset["BG_H"] )
	kBGWnd:SetSize(kBGSize)

	--MainWnd
	local kTopPos = kTopWnd:GetLocation()
	local kStandard = kChatBar:GetLocation()
	local kTopSize = kTopWnd:GetSize()
	kTopPos:SetY( kStandard:GetY() - kSizePreset["MAIN_H"] )
	kTopSize:SetY( kSizePreset["MAIN_H"] )
	kTopWnd:SetSize(kTopSize)
	kTopWnd:SetLocation(kTopPos)

	--Dummy
	local kDmySize = kDummy:GetSize()
	kDmySize:SetY( kSizePreset["MAIN_H"] )
	kDummy:SetSize(kDmySize)
	
	OnOver_ChatHistory(kTopWnd, true)
	
	local kSysOut = GetUIWnd("SysChatOut")
	if nil == kSysOut or kSysOut:IsNil() then return end
	
	if true == g_ChatMgrClient:GetSysChatOutSnap() then
		local kSysOutPos = kSysOut:GetLocation()
		local kSysOutSize = kSysOut:GetSize()
		kSysOutPos:SetY(kTopPos:GetY() - kSysOutSize:GetY()-5)
		kSysOut:SetLocation(kSysOutPos)
	end
end

g_kSysOutExtendSet = 1
g_kSysOutExtendPreset = {}
g_kSysOutExtendPreset[1] = {["BG_H"] = 80, ["LST_H"] = 68}
g_kSysOutExtendPreset[2] = {["BG_H"] = 150, ["LST_H"] = 135}
function SysOutSetPreset(iIndex)
	g_kChatOutSet = iIndex
	local kSizePreset = g_kSysOutExtendPreset[g_kChatOutSet]
	
	ActivateUI("SysChatOut")
	
	local kTopWnd = GetUIWnd("SysChatOut")
	local kDummy = kTopWnd:GetControl("SYS_DMY_CHAT")
	local kTopMenu = kTopWnd:GetControl("SYSCHAT_BG")
	local kBGWnd = kTopMenu:GetControl("SFRM_CHATOUT_BG")
	local kChatWnd = GetUIWnd("ChatOut")
	
	local kList = kTopWnd:GetControl(g_kChat.kChatLogWnd[5]["LIST"])
	local kListPos = kList:GetLocation()
	ListChangeSize(kSizePreset["LST_H"], kList)
	kListPos:SetY(kListPos:GetY())
	kList:SetLocation(kListPos)
	kList:Visible(true)
	
	local kBGSize = kBGWnd:GetSize()
	kBGSize:SetY(kSizePreset["BG_H"])
	kBGWnd:SetSize(kBGSize)
	
	local kTopSize = kTopWnd:GetSize()
	local kTopPos = kTopWnd:GetLocation()
	kTopSize:SetY(kSizePreset["BG_H"]+14)
	kTopWnd:SetSize(kTopSize)
	kTopWnd:SetLocation(kTopPos)
	if true == g_ChatMgrClient:GetSysChatOutSnap() then
		local kStandard = kChatWnd:GetLocation()
		kTopPos:SetY(kStandard:GetY() - kSizePreset["BG_H"] - 19)
		kTopWnd:SetLocation(kTopPos)
	end
	
	local kDmySize = kDummy:GetSize()
	kDmySize:SetY(kSizePreset["BG_H"])
	kDummy:SetSize(kDmySize)
	
	OnOver_ChatHistory(kTopWnd, true)
	
	if true == g_ChatMgrClient:GetSysOutHide() then
		GetUIWnd("SysChatOut"):Visible(false)
	end
	if false == g_AutoHideOption then
		GetEventTimer():Add("SysChatOutHide", 3.0, "OnOver_SysChatHistory(GetUIWnd('SysChatOut'), false)", 1, false)
	end
end

function ListChangeSize(kListH, kListWnd)
	local kListSize = kListWnd:GetSize()
	kListSize:SetY( kListH )
	kListWnd:SetSize( kListSize )
	kListWnd:MoveMiddleBtnEndPos()
end

function OpenCoinUI(wnd, bOpen)
	if true==wnd:IsNil() then return end
	local pa = UISelf:GetParent()
	pa:GetControl("IMG_OPEN_BG"):Visible(false==bOpen)
	pa:GetControl("BTN_HEAD_OPEN"):Visible(false==bOpen)
	pa:GetControl("IMG_CLOSE_BG"):Visible(bOpen)
	pa:GetControl("BTN_HEAD_CLOSE"):Visible(bOpen)

	pa:GetControl("IMG_BG"):Visible(bOpen)

	for i=1,3 do
		local coin = pa:GetControl("FRM_COIN"..i)
		if false==coin:IsNil() then
			coin:Visible(bOpen)
		end
	end
end

g_bCanUpdateLevelUp = false
function UpdateLevelUI( bIsBattleLv )
	local kFrmHero =  GetUIWnd("FormHero")
	if false == kFrmHero:IsNil() then
		local kFrmHPMPBg = kFrmHero:GetControl("SFRM_HPMP_BG")
		if false == kFrmHPMPBg:IsNil() then
			local kFrmLv = kFrmHPMPBg:GetControl("FRM_LV")
			if false == kFrmLv:IsNil() then
				if not bIsBattleLv then
					-- Do Levelup UI Update
					local kTwinkle = kFrmLv:GetControl("IMG_TWINKLE")
					if not kTwinkle:IsNil() then
						kTwinkle:Visible(true)
					end
					kFrmLv:SetTwinkle(60000, 150)
					
					local kClickImg = CallUI("FRM_LVUPCLICK")
					if not kClickImg:IsNil() then
						kClickImg:SetAliveTime(60000)
					end
				end
				
				DrawPlayerLv( kFrmLv:GetControl("FRM_LV_TEXT") )
				
				if not bIsBattleLv then
					if not GetQuestMan():IsFullQuestDialog() then
						g_ChatMgrClient:AddEventMessage(700027,  true, 2, 11)
					end
				end
				
				g_bCanUpdateLevelUp = false
				return true
			end
		end
	end
	
	g_bCanUpdateLevelUp = true
	return false
end

function SendChatEmotion(iTextNo, MotionID)
	local iChatMode = g_ChatMgrClient:ChatMode_Get()
	g_ChatMgrClient:ChatMode_Set(CT_Emotion)	--채팅타입을 감정표현으로
	g_ChatMgrClient:SendChatW(GetTT(iTextNo))
	g_ChatMgrClient:ChatMode_Set(iChatMode)
	SendEmotion(MotionID)
end

g_fEffectTime_InvenEffect = 0
function SetGetItemEffect()
	local kTopWnd = GetUIWnd("QuickMenu")
	if kTopWnd:IsNil() then return end
	local kEffectWnd = kTopWnd:GetControl("IMG_EFFECT")
	if kEffectWnd:IsNil() then return end
	kEffectWnd:Visible(true)
	g_fEffectTime_InvenEffect = GetAccumTime()
	g_pilotMan:GetPlayerActor():AttachSound(0, "WoodenBox_Get", 1.0, 80, 100)
end

function OnTick_InvenEffect(kSelf)
	if kSelf:IsNil() then return end
	local fDiff = GetAccumTime() - g_fEffectTime_InvenEffect
	local fTime = 1.2;
	if fTime < fDiff then
		DrawMonsterKillUI()
		kSelf:Visible(false)
		return
	end
	local iIndex = fDiff / fTime * 16
	kSelf:SetUVIndex( iIndex+1 )
end

function OnClickRePosition()
	local actor = g_pilotMan:GetPlayerActor()
	if false == actor:IsNil() then
		local curAction = actor:GetAction()
		CheckNil(nil==curAction)
		CheckNil(curAction:IsNil())
		if(curAction:GetID() ~= "a_jump") then
			actor:ReserveTransitAction("a_Reposition")
		end
	end
end
--[[
local g_kMainUIIcon = {}
g_kMainUIIcon[1] = {["STATE"]=false } -- UserInfo Icon
g_kMainUIIcon[2] = {["STATE"]=false } -- Inven Icon
g_kMainUIIcon[3] = {["STATE"]=false } -- SKill Icon
g_kMainUIIcon[4] = {["STATE"]=false } -- Quest Icon
g_kMainUIIcon[5] = {["STATE"]=false } -- Community Icon
g_kMainUIIcon[6] = {["STATE"]=false } -- Book Icon

function SetMainUIIconBlink(index, bSet)
	if (index < 1) or (table.getn(g_kMainUIIcon) < index) then 
		return
	end
	
	if (false == bSet) then
		g_kMainUIIcon[index]["STATE"]= false
	else
		g_kMainUIIcon[index]["STATE"]= true
	end
end

function OnTick_MainUIIcon(kWnd, index)
	if (index < 1) or (table.getn(g_kMainUIIcon) < index) then 
		return
	end
	if (false == g_kMainUIIcon[index]["STATE"]) then return end
	if (nil == kWnd) then return end
	if (true == kWnd:IsNil()) then return end
	
	local TimeCnt = kWnd:GetCustomDataAsFloat()
	TimeCnt= TimeCnt+GetFrameTime()
	if(1 < TimeCnt) then 
		TimeCnt = 0
		local CurIndex = kWnd:GetUVIndex() + 1
		CurIndex = CurIndex % 3
		if(CurIndex == 0) then  CurIndex = 1 end	
		kWnd:SetUVIndex(CurIndex)
	end	
	kWnd:SetCustomDataAsFloat(TimeCnt)
end
]]
function SetSkillIconBlink_InMainUI(bSet)
	local kWnd = GetUIWnd("QuickMenu"):GetControl("BTN_MAIN3")
	if (nil == kWnd) then return end
	if (true == kWnd:IsNil()) then return end
	
	local kOutLineWnd = kWnd:GetControl("IMG_OUTLINE")
	
	if (false == bSet) then
--		g_kMainUIIcon[3]["STATE"]= false
		kWnd:OffTwinkleSelf()
		if (nil ~= kOutLineWnd) then
			if (true ~= kOutLineWnd:IsNil()) then
				kOutLineWnd:OffTwinkleSelf()
				kOutLineWnd:Visible(false)
			end
		end
	else
		--g_kMainUIIcon[3]["STATE"]= true
		kWnd:SetTwinkle(9900000, 777)
		if (nil ~= kOutLineWnd) then
			if (true ~= kOutLineWnd:IsNil()) then
				kOutLineWnd:SetTwinkle(9900000, 777)
				kOutLineWnd:Visible(true)
			end
		end
	end	
end

function ClearChatBar()
-- 채팅바의 내용을 없애고 포커스를 잃게함
	local kTop = GetUIWnd("ChatBar")
	if(nil == kTop) then return end
	local kEdit = kTop:GetControl("EDT_CHAT")
	if(nil == kEdit) then return end
	kEdit:SetEditText("")
	ClearEditFocus()
	g_ChatMgrClient:SendChat_InputNow(false)
end

function SetStatusBtnBlink(bSet)
	local kWnd = GetUIWnd("FRM_STATUS_UP")
	if (nil == kWnd) then return end
	if (true == kWnd:IsNil()) then return end
	
	local kImg = kWnd:GetControl("IMG_BTN")
	
	if (false == bSet) then
		kWnd:OffTwinkleSelf()
		if (nil ~= kImg) then
			if (true ~= kImg:IsNil()) then
				kImg:OffTwinkleSelf()
			end
		end
	else
		kWnd:SetTwinkle(9900000, 777)
		if (nil ~= kImg) then
			if (true ~= kImg:IsNil()) then
				kImg:SetTwinkle(9900000, 777)
			end
		end
	end	
end

function GetStrColorInCharInfoUI(iType)
	if(0 == iType) then 
		return "{C=0xFFF104BC/}"
	end
end

g_RestExpOn = false
function SetRestXpMode(bMode)
	local bIsChange = g_RestExpOn ~= bMode
	g_RestExpOn = bMode
	local kWnd = GetUIWnd("Bg_XP_Bar")
	if(true == kWnd:IsNil()) then ODS("안됨1\n",false, 912) return end
	
	local kXpWnd = kWnd:GetControl("XP")
	if(true == kXpWnd:IsNil()) then  ODS("안됨2\n",false, 912) return end
	
	local kRestXpWnd = kWnd:GetControl("REST_XP_BAR")
	if(true == kRestXpWnd:IsNil()) then  ODS("안됨3\n",false, 912) return end
		
	local kRestXpLimitWnd = kWnd:GetControl("REST_XP_LIMIT_BAR")
	if(true == kRestXpLimitWnd:IsNil()) then  ODS("안됨4\n",false, 912) return end
	
	if(bMode) then
		kRestXpWnd:Visible(true)
		kRestXpLimitWnd:Visible(true)
		kXpWnd:Visible(false)
		if(bIsChange) then
			Notice_Show( GetTextW(799024), 1 )
		end
	else
		kRestXpWnd:Visible(false)
		kRestXpLimitWnd:Visible(false)
		kXpWnd:Visible(true)
	end
end

g_bIsRestIconVisible = false
function ShowRestExpIcon(bShow)
	local kWnd = GetUIWnd("FRM_MINIMAP")
	if(true == kWnd:IsNil()) then return end
	
	local kIcon = kWnd:GetControl("SFRM_RESTEXP")
	if(true == kIcon:IsNil()) then return end
	kIcon:Visible(bShow)
	g_bIsRestIconVisible = bShow
end

function NoticeRestExpUI_OnClose()
	local kMyPlayer  = GetMyActor()
	if(false == kMyPlayer:IsNil()) then
		local kStr = GetRestExpToString():GetStr()
		local iExp = tonumber(kStr)
		if(0 < iExp) then
			kStr = kStr.."%"
			Notice_Show(WideString(string.format(GetTextW(799023):GetStr(),kStr), 6, true))
		end
	end
end

function ChatWndActivate(bActivate)
	if(bActivate) then
		ActivateUI("ChatOut")
		ActivateUI("ChatBar")
	else
		local kChatToolTip = GetUIWnd("FRM_CHAT_TOOLTIP")
		if kChatToolTip:IsNil() == false then
			kChatToolTip:Close()
			if false == g_AutoHideOption then
				GetEventTimer():Add("ChatOutHide", 2.0, "OnOver_ChatHistory(GetUIWnd('ChatOut'), false)", 1, false)
			end
		end	
		CloseUI("ChatOut")
		CloseUI("ChatBar")
	end
end

function CallXPBarTooltip()
	if( 9018100 == g_mapNo )then
		CallToolTipText(0, GetTextW(799025), GetUIWnd("Cursor"):GetLocation()) 
	else
		CallToolTipText(0, GetTextW(799020), GetUIWnd("Cursor"):GetLocation()) 
	end
end

function ChatOption_CheckSystemOut(Filter, List, Mode)
	if 0 == Mode then
		local iSet = Config_GetValue(HEADKEY_CHATMODE_SYSTEM, SUBKEY_FILTER_SYSTEM_DAMAGE)
		if 2 == iSet then
			g_ChatMgrClient:ChatFilter_ModeAdd(Filter, CT_Battle, List)
		else
			g_ChatMgrClient:ChatFilter_ModeDel(Filter, CT_Battle, List)
		end
		iSet = Config_GetValue(HEADKEY_CHATMODE_SYSTEM, SUBKEY_FILTER_SYSTEM_ITEM)
		if 2 == iSet then
			g_ChatMgrClient:ChatFilter_ModeAdd(Filter, CT_Item, List)
		else
			g_ChatMgrClient:ChatFilter_ModeDel(Filter, CT_Item, List)
		end
		iSet = Config_GetValue(HEADKEY_CHATMODE_SYSTEM, SUBKEY_FILTER_SYSTEM_GOLD)
		if 2 == iSet then
			g_ChatMgrClient:ChatFilter_ModeAdd(Filter, CT_Gold, List)
		else
			g_ChatMgrClient:ChatFilter_ModeDel(Filter, CT_Gold, List)
		end
		iSet = Config_GetValue(HEADKEY_CHATMODE_SYSTEM, SUBKEY_FILTER_SYSTEM_EXP)
		if 2 == iSet then
			g_ChatMgrClient:ChatFilter_ModeAdd(Filter, CT_Exp, List)
		else
			g_ChatMgrClient:ChatFilter_ModeDel(Filter, CT_Exp, List)
		end
		g_ChatMgrClient:ChatFilter_ModeAdd(Filter, CT_Event, List)
		g_ChatMgrClient:ChatFilter_ModeAdd(Filter, CT_Event_System, List)
		g_ChatMgrClient:ChatFilter_ModeAdd(Filter, CT_Event_Game, List)
		g_ChatMgrClient:ChatFilter_ModeAdd(Filter, CT_Notice, List)
	else
		g_ChatMgrClient:ChatFilter_ModeDel(Filter, CT_Battle, List)
		g_ChatMgrClient:ChatFilter_ModeDel(Filter, CT_Item, List)
		g_ChatMgrClient:ChatFilter_ModeDel(Filter, CT_Gold, List)
		g_ChatMgrClient:ChatFilter_ModeDel(Filter, CT_Exp, List)
		g_ChatMgrClient:ChatFilter_ModeDel(Filter, CT_Event, List)
		g_ChatMgrClient:ChatFilter_ModeDel(Filter, CT_Event_System, List)
		g_ChatMgrClient:ChatFilter_ModeDel(Filter, CT_Event_Game, List)
		g_ChatMgrClient:ChatFilter_ModeDel(Filter, CT_Notice, List)
	end
end

function SystemFilter_Modify(Type, Mode)
	if 0 == Mode then
		local iSet = Config_GetValue(HEADKEY_CHATMODE_NORMAL, SUBKEY_FILTER_SYSTEM)
		if 2 == iSet then
			g_ChatMgrClient:ChatFilter_ModeAdd(ChatFilter_Normal, Type, "LIST_CHAT_NORMAL")
		else
			g_ChatMgrClient:ChatFilter_ModeDel(ChatFilter_Normal, Type, "LIST_CHAT_NORMAL")			
		end
		iSet = Config_GetValue(HEADKEY_CHATMODE_PARTY, SUBKEY_FILTER_SYSTEM)
		if 2 == iSet then
			g_ChatMgrClient:ChatFilter_ModeAdd(ChatFilter_Party, Type, "LIST_CHAT_PARTY")
		else
			g_ChatMgrClient:ChatFilter_ModeDel(ChatFilter_Party, Type, "LIST_CHAT_PARTY")
		end
		iSet = Config_GetValue(HEADKEY_CHATMODE_GUILD, SUBKEY_FILTER_SYSTEM)
		if 2 == iSet then
			g_ChatMgrClient:ChatFilter_ModeAdd(ChatFilter_Guild, Type, "LIST_CHAT_GUILD")
		else
			g_ChatMgrClient:ChatFilter_ModeDel(ChatFilter_Guild, Type, "LIST_CHAT_GUILD")
		end
		iSet = Config_GetValue(HEADKEY_CHATMODE_TRADE, SUBKEY_FILTER_SYSTEM)
		if 2 == iSet then
			g_ChatMgrClient:ChatFilter_ModeAdd(ChatFilter_Trade, Type, "LIST_CHAT_TRADE")
		else
			g_ChatMgrClient:ChatFilter_ModeDel(ChatFilter_Trade, Type, "LIST_CHAT_TRADE")
		end
	else
		g_ChatMgrClient:ChatFilter_ModeDel(ChatFilter_Normal, Type, "LIST_CHAT_NORMAL")
		g_ChatMgrClient:ChatFilter_ModeDel(ChatFilter_Party, Type, "LIST_CHAT_PARTY")
		g_ChatMgrClient:ChatFilter_ModeDel(ChatFilter_Guild, Type, "LIST_CHAT_GUILD")
		g_ChatMgrClient:ChatFilter_ModeDel(ChatFilter_Trade, Type, "LIST_CHAT_TRADE")
	end
end

g_AutoHideOption = false
function AutoHide(bHide)
	g_AutoHideOption = bHide
	if	false == g_AutoHideOption then
		GetEventTimer():Add("SysChatOutHide", 3.0, "OnOver_SysChatHistory(GetUIWnd('SysChatOut'), false)", 1, false)
		GetEventTimer():Add("ChatOutHide", 3.0, "OnOver_ChatHistory(GetUIWnd('ChatOut'), false)", 1, false)
	else
		GetEventTimer():Del("ChatOutHide")
		GetEventTimer():Del("SysChatOutHide")
		GetUIWnd("SysChatOut"):GetControl("SYSCHAT_BG"):Visible(true)
		GetUIWnd("ChatOut"):GetControl("CHAT_BG"):Visible(true)
	end
end

function DefaultChatWndPos()
	local ChatOut = GetUIWnd("ChatOut")
	local SysChatOut = GetUIWnd("SysChatOut")
	local QuickSlotEx = GetUIWnd("QuickInvEx")
	local QuickSlot = GetUIWnd("QuickInv")
	
	local ChatOutPos = ChatOut:GetLocation()
	local QuickSlotPos = QuickSlot:GetLocation()
	local QuickSlotExPos = QuickSlotEx:GetLocation()
	local ChatOutSize = ChatOut:GetSize()
	ChatOutPos:SetX(0)
	if true == QuickSlotEx:IsVisible() then
		ChatOutPos:SetY(QuickSlotExPos:GetY() - ChatOutSize:GetY() - 32)
	else
		ChatOutPos:SetY(QuickSlotPos:GetY() - ChatOutSize:GetY() - 27)
	end
	ChatOut:SetLocation(ChatOutPos)

	local kSizePreset = g_kSysOutExtendPreset[g_kChatOutSet]	
	local SysChatOutPos = SysChatOut:GetLocation()
	SysChatOutPos:SetX(0)
	SysChatOutPos:SetY(ChatOutPos:GetY() - kSizePreset["BG_H"] - 19)
	SysChatOut:SetLocation(SysChatOutPos)
end

function NotifyPartyFunction()
	if true == g_NotifyPartyFunction and not IsInParty() then
		g_ChatMgrClient:AddEventMessage(799382, true, 0, 11)
		GetUIWnd("FRM_MINIMAP"):GetControl("IMG_HELP_PARTY_ARROW"):Visible(true)
		GetUIWnd("FRM_MINIMAP"):GetControl("SFRM_PARTY_FIND_LIST"):Visible(true)
		g_NotifyPartyFunction = false
	end	
end

function CheckPVPModeEnter()
	if IsInExpedition() then
		CallYesNoMsgBox( GetTT(720050), GUID(), MBT_EXPEDITION_PVPMODE_ENTER )
	else
		if(true == IsOnMacroCheck()) then  
			return	
		end
		OnClickPvP()
	end
end