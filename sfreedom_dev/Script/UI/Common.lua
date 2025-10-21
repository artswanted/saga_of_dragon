
--[[
	enum InputBox Message
	1 = Guild Kick
	2 = Guild Req Join
	3 = Invite Party
	4 = Create Party
	5 = Rename Party
	6 = Invite Couple
]]
--//////////////  //////////////



function ProcessInputBoxMessage(iCommandType, kInputText)
	ODS("Command Type: "..iCommandType.." Text: "..kInputText:GetStr().."\n")
	if nil == iCommandType or 0 == iCommandType then return end
	if nil == kInputText or kInputText:IsNil() then 
		if 9 == iCommandType then
			RequestVendorCreate(kInputText)
		else
			return 
		end
	end
	
	if 1 == iCommandType then --Guild Kick
		Send_ReqKickMemberName(kInputText)
	elseif 2 == iCommandType then--Guild Invite
		Send_ReqJoinGuildName(kInputText)
	elseif 3 == iCommandType then--Invite Party
		Net_C_N_REQ_JOIN_PARTY_ByName(kInputText)
	elseif 4 == iCommandType then--Create Party
		if true == kInputText:FilterString(1, false) then
			CommonMsgBox(GetTT(700105):GetStr(), true)
			return
		end
		Net_C_N_REQ_CREATE_PARTY(kInputText)
	elseif 5 == iCommandType then--Rename Party
		if true == kInputText:FilterString(1, false) then
			CommonMsgBox(GetTT(700105):GetStr(), true)
			return
		end
		Net_PT_C_N_REQ_PARTY_RENAME(kInputText)
	elseif 6 == iCommandType then--Invite Couple
		Send_ReqCoupleByName(kInputText)
	elseif 7 == iCommandType then--Friend Add
		if true == kInputText:FilterString(3, false) then
			CommonMsgBox(GetTT(700105):GetStr(), true)
			return
		end	
		Community_Friend_AddFriendName(kInputText)
	elseif 8 == iCommandType then--Friend Group Add
		if true == kInputText:FilterString(3, false) then
			CommonMsgBox(GetTT(700105):GetStr(), true)
			return
		end	
		Community_Friend_AddGroupName(kInputText)
	elseif 9 == iCommandType then--Create Vendor
		RequestVendorCreate(kInputText)
		return
	end
end

--////////////// Pre Check Common Popup Command //////////////
function CheckPopupCommand(kActor, kCmdArray)
	local kNewCmdArray = {}
	local iCount = 0	
	if true == kActor:HaveActivate() then
		for kKey, kVal in pairs(kCmdArray) do
			local bCan = true
			if kVal == "QUEST" or kVal == "TACTICS_QUEST" then
				bCan = kActor:HaveQuest()
			elseif kVal == "DAILYQUEST_LV0" or kVal == "DAILYQUEST_LV1" or kVal == "DAILYQUEST_LV2" then
				bCan = kActor:HaveDailyQuest()
			elseif kVal == "TALK" then
				bCan = kActor:HaveTalk()
			elseif kVal == "TALK_ONLY" or kVal == "MISSION_EVENT" or kVal == "JAPAN_EVENT" then
				-- is can?
				if kVal == "TALK_ONLY" then
					bCan = kActor:HaveTalk()
				else
					bCan = true
				end
				if bCan then
					iCount = 1
					kNewCmdArray = {} -- only one
					kNewCmdArray[iCount] = kVal
					return kNewCmdArray, iCount
				end
			elseif kVal == "WARNING" then
				bCan = kActor:HaveWarning()
			elseif kVal == "GUILD" then
				bCan = AmIGuildOwner() or not HaveGuild()
			elseif kVal == "GUILD_INVENTORY" then
				if HaveGuildInventory() then
					bCan = true
				else
					bCan = false
				end
			elseif kVal == "ITEMCONVERT" then
				bCan = false
				if(true == kActor:IsEqualObjectName("c_Jewelry")) then
					bCan = true
				else
					 if(GetLocale() == LOCALE.NC_USA) -- 북미 이벤트 가토에게 아이템 변환기 메뉴추가 - 2009.08.20
						and (true == kActor:IsEqualObjectName("c_gatto")) then						
						 bCan = true
					end
				end
			elseif kVal == "EM_ADMINISTRATION" then
	--			bCan = AmIGuildOwner()
			elseif kVal == "SWORD_CLASS_HELP" or kVal == "MAGICIAN_CLASS_HELP" or kVal == "ARCHER_CLASS_HELP" or kVal == "THIEF_CLASS_HELP" then
				local kMyPilot = g_pilotMan:GetPlayerPilot()
				if not kMyPilot:IsNil() then
					local iClass = kMyPilot:GetAbil(AT_CLASS)
					bCan = (1 == iClass) or (2 == iClass) or (3 == iClass) or (4 == iClass)
				end
			elseif kVal == "SWORD_CP_SHOP" or kVal == "MAGICIAN_CP_SHOP" or kVal == "ARCHER_CP_SHOP" or kVal == "THIEF_CP_SHOP" 
					or  "SHAMAN_CP_SHOP" == kVal or "DOUBLE_FIGHTER_CP_SHOP" == kVal
					then
				if(GetLocale() == LOCALE.NC_NO_SET) then
					bCan = false
				else 
					bCan = true
				end
			elseif kVal == "MIXUPITEM" then
					bCan = false			-- 코스튬 조합 모든 나라 막음	
			elseif kVal == "MYHOME_TAX"
					or kVal == "MYHOME_INVITATION" then
					if(false == g_world:IsNil()) then
						local kHome = g_world:GetHome()
						if(false == kHome:IsNil()) then
							if(false == kHome:IsMyHome()) then
								bCan = false
							end
						end
					end
			elseif kVal == "MYHOME_AUCTION_MENU" then
				if true == IsMyhomeApt() then
					bCan = false
				end
				if(false == g_world:IsNil()) then
					local kHome = g_world:GetHome()
					if(false == kHome:IsNil()) then
						if(false == kHome:IsMyHome()) then
							bCan = false
						end
					end
				end
			elseif kVal == "MYHOME_REPAIR_DISCOUNT" then
				if IsRepairService() == false then
					bCan = false
				end
			elseif kVal == "MYHOME_ENCHANT_DISCOUNT" then
				if IsEnchantService() == false then
					bCan = false
				end
			--[[ -- 마이홈을 방문한 사람들은 누구나 버프를 받을수 있어야한다
			elseif kVal == "MYHOME_DECORATION" then
				bCan = false
				if(false == g_world:IsNil()) then
					local kHome = g_world:GetHome()
					if(false == kHome:IsNil()) then
						if(true == kHome:IsMyHome()) then
							bCan = true
						end
					end
				end
			]]
			elseif kVal == "MYHOME_SETTING" then
				bCan = false
				--if true == IsMyhomeApt() then
					if(false == g_world:IsNil()) then
						local kHome = g_world:GetHome()
						if(false == kHome:IsNil()) then
							if(true == kHome:IsMyHome()) then
								bCan = true
							end
						end
					end
				--end
			elseif kVal == "TOWN_APT_SELL" then
				-- if false == IsMyhomeApt() then
					-- bCan = false
				-- end
			elseif kVal == "MYHOME_SOCKET" then
				if GetLocale() == LOCALE.NC_TAIWAN 
				or GetLocale() == LOCALE.NC_THAILAND 
				or GetLocale() == LOCALE.NC_INDONESIA 
				or GetLocale() == LOCALE.NC_PHILIPPINES 
				or GetLocale() == LOCALE.NC_CHINA 
				or GetLocale() == LOCALE.NC_EU 
				or GetLocale() == LOCALE.NC_USA 
				or GetLocale() == LOCALE.NC_FRANCE 
				or GetLocale() == LOCALE.NC_GERMANY 
				or GetLocale() == LOCALE.NC_SINGAPORE	
				or GetLocale() == LOCALE.NC_RUSSIA then					
					bCan = false
				end
				-- if true == IsHomeBidding() then
					-- bCan = false
				-- end
			elseif kVal == "RARITY_BUILD_UP" then
				if(GetLocale() == LOCALE.NC_DEVELOP) then
					bCan = true
				else
					bCan = true
				end
			elseif kVal == "MYHOME_NPC_REMOVE" 
				or kVal == "MYHOME_NPC_MOVE" then
				if(false == g_world:IsNil()) then
					local kHome = g_world:GetHome()
					if(false == kHome:IsNil()) then
						if(false == kHome:IsMyHome()) then
							bCan = false
						end
					end
				end
			elseif kVal == "GEMSTORE_SHOP1" then
				bCan = IsNotEmptyDefGemStore(kActor:GetPilotGuid(), 1)
			elseif kVal == "GEMSTORE_SHOP2" then
				bCan = IsNotEmptyDefGemStore(kActor:GetPilotGuid(), 2)
			elseif kVal == "ITEM_TREE" then
				bCan = IsNotEmptyDefGemStore(kActor:GetPilotGuid(), 15)
			elseif kVal == "ITEM_SWAP" then
				bCan = IsNotEmptyDefGemStore(kActor:GetPilotGuid(), 16)
			elseif kVal == "ITEM_PLUS" then		
				bCan = IsNotEmptyDefGemStore(kActor:GetPilotGuid(), 17)				
			elseif kVal == "COLLECT_ANTIQUE" then
				bCan = true
			elseif kVal == "EXCHANGE_SOCKETCARD" then
				bCan = true
			elseif kVal == "EXCHANGE_GATHER" then
				bCan = true
			elseif kVal == "SHOP" then
				local ami = string.upper(kActor:GetID())
				if nil==ami then
					bCan = false
				else
					if (ami=="C_AMY") or (ami=="C_AMI") then
						bCan = false--일단 안보이게
						local eLocale = GetLocale()-- 활력포션때문에 쓰는 국가만 샵 표시
						if eLocale==LOCALE.NC_TAIWAN or eLocale==LOCALE.NC_THAILAND or
							eLocale==LOCALE.NC_JAPAN or eLocale==LOCALE.NC_EU or
							eLocale==LOCALE.NC_SINGAPORE or eLocale==LOCALE.NC_VIETNAM or
							eLocale==LOCALE.NC_RUSSIA or eLocale==LOCALE.NC_DEVELOP then
							bCan = true
						end
					end
				end
			end
		
			if true == bCan then
				iCount = iCount + 1
				kNewCmdArray[iCount] = kVal
			end
		end
	end
	
	return kNewCmdArray, iCount
end

--////////////// //////////////
g_kCommonPopup = {} -- Upper String Only 
--NPC MENU -- !!! included Variant/PgNpc.h !!!
g_kCommonPopup[1] = {["Name"]="QUEST", ["TTW"]=400539}
g_kCommonPopup[2] = {["Name"]="SHOP", ["TTW"]=400538}
g_kCommonPopup[3] = {["Name"]="GUILD", ["TTW"]=400540}
g_kCommonPopup[4] = {["Name"]="REPAIR", ["TTW"]=400608}
g_kCommonPopup[5] = {["Name"]="ENCHANT", ["TTW"]=400609}
g_kCommonPopup[6] = {["Name"]="INVSAFE", ["TTW"]=400582}
g_kCommonPopup[7] = {["Name"]="LOTTERY", ["TTW"]=400583}
g_kCommonPopup[8] = {["Name"]="SHINE_UP", ["TTW"]=400584}
g_kCommonPopup[9] = {["Name"]="COOKING", ["TTW"]=400585}
g_kCommonPopup[10] = {["Name"]="TALK", ["TTW"]=400586} -- same TALK_ONLY
g_kCommonPopup[11] = {["Name"]="WARNING", ["TTW"]=400588}
g_kCommonPopup[12] = {["Name"]="COIN", ["TTW"]=550004}
g_kCommonPopup[13] = {["Name"]="RARITY_UPGRADE", ["TTW"]=550006}
g_kCommonPopup[14] = {["Name"]="FRAN", ["TTW"]=550007}
g_kCommonPopup[15] = {["Name"]="CRAFT", ["TTW"]=1434}
g_kCommonPopup[16] = {["Name"]="TACTICS_QUEST", ["TTW"]=400839}
g_kCommonPopup[17] = {["Name"]="SWORD_CP_SHOP", ["TTW"]=401011}
g_kCommonPopup[18] = {["Name"]="MAGICIAN_CP_SHOP", ["TTW"]=401011}
g_kCommonPopup[19] = {["Name"]="ARCHER_CP_SHOP", ["TTW"]=401011}
g_kCommonPopup[20] = {["Name"]="THIEF_CP_SHOP", ["TTW"]=401011}
g_kCommonPopup[21] = {["Name"]="COUPON", ["TTW"]=1821}
g_kCommonPopup[22] = {["Name"]="SWORD_CLASS_HELP", ["TTW"]=401012}
g_kCommonPopup[23] = {["Name"]="MAGICIAN_CLASS_HELP", ["TTW"]=401012}
g_kCommonPopup[24] = {["Name"]="ARCHER_CLASS_HELP", ["TTW"]=401012}
g_kCommonPopup[25] = {["Name"]="THIEF_CLASS_HELP", ["TTW"]=401012}
g_kCommonPopup[26] = {["Name"]="TRADE_UNSEALINGSCROLL", ["TTW"]=400456}
g_kCommonPopup[27] = {["Name"]="DAILYQUEST_LV0", ["TTW"]=401023}
g_kCommonPopup[28] = {["Name"]="DAILYQUEST_LV1", ["TTW"]=401023}
g_kCommonPopup[29] = {["Name"]="DAILYQUEST_LV2", ["TTW"]=401023}
g_kCommonPopup[30] = {["Name"]="TRADE_SOULSTONE", ["TTW"]=400458}
g_kCommonPopup[31] = {["Name"]="POST", ["TTW"]=900}
g_kCommonPopup[32] = {["Name"]="EM_ADMINISTRATION", ["TTW"]=72006}
g_kCommonPopup[33] = {["Name"]="GEMSTORE", ["TTW"]=790120}
g_kCommonPopup[34] = {["Name"]="SOCKET_CREATE", ["TTW"]=790130}
g_kCommonPopup[35] = {["Name"]="ITEMCONVERT", ["TTW"]=790530}
g_kCommonPopup[36] = {["Name"]="MISSION_EVENT", ["TTW"]=509407}
g_kCommonPopup[37] = {["Name"]="EVENT_TW_EFFECTQUEST", ["TTW"]=700505}
g_kCommonPopup[38] = {["Name"]="MIXUPITEM", ["TTW"]=50801}
g_kCommonPopup[39] = {["Name"]="SOUL_STONE_TRADE", ["TTW"]=400458}
g_kCommonPopup[40] = {["Name"]="MYHOME_HELP", ["TTW"]=201097}			-- Start MyHomeMenu
g_kCommonPopup[41] = {["Name"]="MYHOME_DECORATION", ["TTW"]=201699}
g_kCommonPopup[42] = {["Name"]="MYHOME_INVITATION", ["TTW"]=201099}
g_kCommonPopup[43] = {["Name"]="MYHOME_TAX", ["TTW"]=201100}
g_kCommonPopup[44] = {["Name"]="MYHOME_AUCTION_MENU", ["TTW"]=201081}
g_kCommonPopup[45] = {["Name"]="MYHOME_AUCTION_INFO", ["TTW"]=201101}
g_kCommonPopup[46] = {["Name"]="MYHOME_INFO", ["TTW"]=201102}
g_kCommonPopup[47] = {["Name"]="TOWN_HELP", ["TTW"]=201103}
g_kCommonPopup[48] = {["Name"]="TOWN_BOARD", ["TTW"]=201719}
g_kCommonPopup[49] = {["Name"]="TOWN_APT_BUY", ["TTW"]=201690}
g_kCommonPopup[50] = {["Name"]="TOWN_MOVE", ["TTW"]=201106}
g_kCommonPopup[51] = {["Name"]="MYHOME_GO_TO_HOME", ["TTW"]=201110}
g_kCommonPopup[52] = {["Name"]="MYHOME_INVITE_LIST", ["TTW"]=201111}	-- End MyHomeMenu
g_kCommonPopup[53] = {["Name"]="SHARE_SAFE_INV", ["TTW"]=2920}
g_kCommonPopup[54] = {["Name"]="RANDOM_QUEST", ["TTW"]=406116}
g_kCommonPopup[55] = {["Name"]="CHRISTMAS_EVENT", ["TTW"]=460104}
g_kCommonPopup[56] = {["Name"]="HELP_TALK", ["TTW"]=372}
g_kCommonPopup[57] = {["Name"]="MYHOME_SIDEJOB", ["TTW"]=201600}
g_kCommonPopup[58] = {["Name"]="MYHOME_REPAIR_DISCOUNT", ["TTW"]=201605}
g_kCommonPopup[59] = {["Name"]="MYHOME_ENCHANT_DISCOUNT", ["TTW"]=201606}
g_kCommonPopup[60] = {["Name"]="MYHOME_NPC_MOVE", ["TTW"]=201654}
g_kCommonPopup[61] = {["Name"]="MYHOME_NPC_REMOVE", ["TTW"]=201655}
g_kCommonPopup[62] = {["Name"]="MYHOME_SOUL_GOLD", ["TTW"]=201618}
g_kCommonPopup[63] = {["Name"]="MYHOME_SOUL_COUNT", ["TTW"]=201618}
g_kCommonPopup[64] = {["Name"]="MYHOME_SOCKET", ["TTW"]=201623}
g_kCommonPopup[65] = {["Name"]="MYHOME_SETTING", ["TTW"]=201698}
g_kCommonPopup[66] = {["Name"]="MYHOME_ENCHANT_RATE", ["TTW"]=201613}
g_kCommonPopup[67] = {["Name"]="MYHOME_SIDE_JOB_BOARD", ["TTW"]=201631}
--68
--69
g_kCommonPopup[70] = {["Name"]="MARRY_IF_TALK", ["TTW"]=450124}			-- Start Marry MENU
g_kCommonPopup[71] = {["Name"]="MARRY_REQSEND", ["TTW"]=450125}
g_kCommonPopup[72] = {["Name"]="MARRY_MONEY", ["TTW"]=450117}
g_kCommonPopup[73] = {["Name"]="MARRY_MGS", ["TTW"]=450115}
g_kCommonPopup[74] = {["Name"]="MARRY_START", ["TTW"]=450114}			-- End Marry MENU
g_kCommonPopup[75] = {["Name"]="EVENT_QUEST_SYS", ["TTW"]=401439}
g_kCommonPopup[76] = {["Name"]="EVENT_MISSION", ["TTW"]=450150}
g_kCommonPopup[77] = {["Name"]="IGGACHA", ["TTW"]=790620}
g_kCommonPopup[78] = {["Name"]="BATTLE_SQUARE", ["TTW"]=460013}
g_kCommonPopup[79] = {["Name"]="SAVEPOS", ["TTW"]=2704}
g_kCommonPopup[80] = {["Name"]="TRANSPORT", ["TTW"]=2705}
g_kCommonPopup[81] = {["Name"]="HIDDEN_MSG", ["TTW"]=400988}
g_kCommonPopup[82] = {["Name"]="HIDDEN_OPEN", ["TTW"]=400989}
g_kCommonPopup[83] = {["Name"]="TACTICS_RANDOM_QUEST", ["TTW"]=406143}
g_kCommonPopup[84] = {["Name"]="SOUL", ["TTW"]=401230}
g_kCommonPopup[85] = {["Name"]="WANTED_QUEST", ["TTW"]=406155}
g_kCommonPopup[86] = {["Name"]="RARITY_BUILD_UP", ["TTW"]=1510}
g_kCommonPopup[87] = {["Name"]="RARITY_AMPLIFY", ["TTW"]=1511}
g_kCommonPopup[88] = {["Name"]="MON_CARD2_MIX", ["TTW"]=790480}
g_kCommonPopup[89] = {["Name"]="TOWN_APT_SELL", ["TTW"]=201728}
g_kCommonPopup[90] = {["Name"]="GEMHELP", ["TTW"]=5106}
g_kCommonPopup[91] = {["Name"]="TALK_ONLY", ["TTW"]=400586} -- same TALK
g_kCommonPopup[92] = {["Name"]="JAPAN_EVENT", ["TTW"]=400586}
--g_kCommonPopup[93] = {["Name"]="EXCHANGE_PET_EXP", ["TTW"]=387}
g_kCommonPopup[94] = {["Name"]="ENCHANT_SHIFT", ["TTW"]=1250}
g_kCommonPopup[95] = {["Name"]="GEMSTORE_SHOP1", ["TTW"]=790128}
g_kCommonPopup[96] = {["Name"]="GEMSTORE_SHOP2", ["TTW"]=790129}
g_kCommonPopup[97] = {["Name"]="GUILD_INVENTORY", ["TTW"]=401073}
g_kCommonPopup[98] = {["Name"]="SKILL_MASTER", ["TTW"]=401138}
g_kCommonPopup[99] = {["Name"]="EMPORIA_SHOP", ["TTW"]=70088}
g_kCommonPopup[100] = {["Name"]="EXPEDITION", ["TTW"]=710049}
g_kCommonPopup[200] = {["Name"]="HALLOWEEN_EVENT", ["TTW"]=460104}
g_kCommonPopup[201] = {["Name"]="EVENTSTORE", ["TTW"]=790530}
g_kCommonPopup[202] = {["Name"]="EVENT_SHOP", ["TTW"]=790542}
g_kCommonPopup[301] = {["Name"]="COMMUNITY_EVENT", ["TTW"]=302104}
g_kCommonPopup[302] = {["Name"]="EXCHANGE_ELUNIUM", ["TTW"]=310000}
g_kCommonPopup[303] = {["Name"]="VALKYRIE_STORE", ["TTW"]=310001}
g_kCommonPopup[304] = {["Name"]="EXCHANGE_PURESILVER_KEY", ["TTW"]=310010}
g_kCommonPopup[305] = {["Name"]="RAGNAROK_BLESS", ["TTW"]=310015}
g_kCommonPopup[306] = {["Name"]="ITEM_STORE", ["TTW"]=790530}
g_kCommonPopup[307] = {["Name"]="ITEM_TREE", ["TTW"]=50701}
g_kCommonPopup[308] = {["Name"]="BUFF", ["TTW"]=799032}
g_kCommonPopup[309] = {["Name"]="ITEM_SWAP", ["TTW"]=790128}
g_kCommonPopup[310] = {["Name"]="ITEM_PLUS", ["TTW"]=790129}

g_kCommonPopup[10001] = {["Name"]="JOB_SKILL_LEARN", ["TTW"]=799500} -- new number 10000
g_kCommonPopup[10002] = {["Name"]="JOBSKILL_SHOP", ["TTW"]=400538}
g_kCommonPopup[10004] = {["Name"]="JOB_SKILL3_CREATEITEM", ["TTW"]=799546}
g_kCommonPopup[10005] = {["Name"]="JOB_SKILL_SOUL_TRANSFER", ["TTW"]=799848}

g_kCommonPopup[10100] = {["Name"]="SOCKET_RESTORATION", ["TTW"]=790131}
g_kCommonPopup[10101] = {["Name"]="SOCKET_RESET", ["TTW"]=790132}
g_kCommonPopup[10102] = {["Name"]="SOCKET_MAKE", ["TTW"]=790103}

g_kCommonPopup[10103] = {["Name"]="ELEMENTHELP", ["TTW"]=799917}
g_kCommonPopup[10104] = {["Name"]="ELEMENT_STORE", ["TTW"]=799910}

g_kCommonPopup[10105] = {["Name"]="SOCKET_CARD_EXTRACTION", ["TTW"]=790163}

--PC MENU
g_kCommonPopup[100] = {["Name"]="TRADE", ["TTW"]=400700}
g_kCommonPopup[101] = {["Name"]="INVITE_PARTY", ["TTW"]=400510}
g_kCommonPopup[102] = {["Name"]="ADD_FRIEND", ["TTW"]=400508}
g_kCommonPopup[103] = {["Name"]="INVITE_PVP", ["TTW"]=400297}
g_kCommonPopup[104] = {["Name"]="VIEW_INFO", ["TTW"]=400272}
g_kCommonPopup[105] = {["Name"]="JOIN_PARTY", ["TTW"]=400579}
g_kCommonPopup[106] = {["Name"]="FOLLOW", ["TTW"]=400500}
g_kCommonPopup[107] = {["Name"]="JOIN_PARTYFIND", ["TTW"]=400579}
g_kCommonPopup[108] = {["Name"]="REQ_GUILD_GUID", ["TTW"]=400518}
g_kCommonPopup[109] = {["Name"]="REQ_GUILD_NAME", ["TTW"]=400518}
g_kCommonPopup[110] = {["Name"]="CANCEL_FOLLOW", ["TTW"]=986}
g_kCommonPopup[111] = {["Name"]="TAKE_PERSON", ["TTW"]=987}
g_kCommonPopup[112] = {["Name"]="CANCEL_TAKE_PERSON", ["TTW"]=988}
g_kCommonPopup[113] = {["Name"]="COPY_PCNAME_TO_CLIPBOARD", ["TTW"]=5019}
g_kCommonPopup[114] = {["Name"]="ORDER_CARD_VIEW", ["TTW"]=51024}
g_kCommonPopup[115] = {["Name"]="RECOMMEND", ["TTW"]=51025}
g_kCommonPopup[116] = {["Name"]="PVPLEAGUE_REGIST", ["TTW"]=560068}
g_kCommonPopup[117] = {["Name"]="PVPLEAGUE_MOVE", ["TTW"]=560069}
g_kCommonPopup[118] = {["Name"]="INTERACTIVE_EMOTION", ["TTW"]=790900}

g_kCommonPopup[119] = {["Name"]="ONE_ON_ONE", ["TTW"]=790004}
g_kCommonPopup[120] = {["Name"]="CHATWND_ONE_ON_ONE", ["TTW"]=790004}

--g_kCommonPopup[121] = {["Name"]="SHAMAN_CLASS_HELP", ["TTW"]=401012}
--g_kCommonPopup[122] = {["Name"]="DOUBLE_FIGHTER_CLASS_HELP", ["TTW"]=401012}
g_kCommonPopup[123] = {["Name"]="SHAMAN_CP_SHOP", ["TTW"]=401011}
g_kCommonPopup[124] = {["Name"]="DOUBLE_FIGHTER_CP_SHOP", ["TTW"]=401011}
g_kCommonPopup[125] = {["Name"]="TRADE_UNSEALINGSCROLL2", ["TTW"]=400456}
g_kCommonPopup[126] = {["Name"]="CRAFT2", ["TTW"]=1434}
g_kCommonPopup[127] = {["Name"]="PET_UPGRADE", ["TTW"]=3414}
g_kCommonPopup[128] = {["Name"]="CRAFT_ITEM_CHANGE", ["TTW"]=1434}

--Party UI Menu
g_kCommonPopup[1100] = {["Name"]="PARTY_LEAVE", ["TTW"]=400512}
g_kCommonPopup[1101] = {["Name"]="PARTY_KICK", ["TTW"]=400511}
g_kCommonPopup[1102] = {["Name"]="PARTY_GIVE_OWN", ["TTW"]=400560}
g_kCommonPopup[1103] = {["Name"]="PARTY_INVITE", ["TTW"]=400510}
g_kCommonPopup[1104] = {["Name"]="PARTY_OPTION", ["TTW"]=400565}
g_kCommonPopup[1105] = {["Name"]="PARTY_FOLLOW", ["TTW"]=400500}
g_kCommonPopup[1106] = {["Name"]="WHISPER", ["TTW"]=400048}
g_kCommonPopup[1107] = {["Name"]="PARTY_WARP", ["TTW"]=401300}
g_kCommonPopup[1108] = {["Name"]="PARTY_PEOPLE_CALL", ["TTW"]=401308}
g_kCommonPopup[1109] = {["Name"]="PARTY_SEE_INFO", ["TTW"]=400272}
g_kCommonPopup[1110] = {["Name"]="PARTY_TRADE", ["TTW"]=400700}
g_kCommonPopup[1111] = {["Name"]="PARTY_NAME_COPY_TO_CLIPBOARD", ["TTW"]=5019}
g_kCommonPopup[1112] = {["Name"]="PARTY_MYHOME_INVITE", ["TTW"]=201099}

g_kCommonPopup[1113] = {["Name"]="PARTY_FIND", ["TTW"]=401208}
g_kCommonPopup[1114] = {["Name"]="PEOPLE_FIND", ["TTW"]=401207}
g_kCommonPopup[1115] = {["Name"]="PARTYGROUND_WARP", ["TTW"]=799370}

--Couple UI Menu
g_kCommonPopup[2001] = {["Name"]="REQ_COUPLE", ["TTW"]=450037}
g_kCommonPopup[2002] = {["Name"]="REQ_INSTANCE_COUPLE", ["TTW"]=450062}
--Guild UI Menu
g_kCommonPopup[3001] = {["Name"]="GUILD_KICK", ["TTW"]=400623}
g_kCommonPopup[3002] = {["Name"]="GUILD_LEAVE", ["TTW"]=400449}
g_kCommonPopup[3003] = {["Name"]="GUILD_CHANGE_OWNER", ["TTW"]=400599}
g_kCommonPopup[3004] = {["Name"]="GUILD_SET_MASTER", ["TTW"]=400624}
g_kCommonPopup[3005] = {["Name"]="GUILD_UNSET_MASTER", ["TTW"]=400625}
g_kCommonPopup[3006] = {["Name"]="GUILD_MYHOME_INVITE"	, ["TTW"]=201099} 	-- 마이홈 초대
--ETC
g_kCommonPopup[3100] = {["Name"]="COLLECT_ANTIQUE", ["TTW"]=750000}			-- 골동품 수집
--g_kCommonPopup[3101] = {["Name"]="EXCHANGE_SOCKETCARD", ["TTW"]=759903}		-- 소켓카드 교환

--Community FriendPopup
g_kCommonPopup[4001] = {["Name"]="FRIEND_SEE_INFO"		, ["TTW"]=400272} 	-- 정보 보기
g_kCommonPopup[4002] = {["Name"]="FRIEND_INVITE_PARTY"	, ["TTW"]=400510} 	-- 파티초대
g_kCommonPopup[4003] = {["Name"]="FRIEND_TRADE"			, ["TTW"]=400700} 	-- 교환
g_kCommonPopup[4004] = {["Name"]="FRIEND_WHISPER"		, ["TTW"]=790000} 	-- 귓속말
g_kCommonPopup[4005] = {["Name"]="FRIEND_ONE_ON_ONE"	, ["TTW"]=790004} 	-- 1:1대화
g_kCommonPopup[4006] = {["Name"]="FRIEND_INVITE_GUILD"	, ["TTW"]=400518} 	-- 길드초대
g_kCommonPopup[4007] = {["Name"]="FRIEND_INVITE_COUPLE"	, ["TTW"]=450036} 	-- 커플신청	//CHN, 디폴트 두개 필요
g_kCommonPopup[4008] = {["Name"]="FRIEND_DELETE"		, ["TTW"]=400509} 	-- 친구삭제
g_kCommonPopup[4009] = {["Name"]="FRIEND_ISOLATE"		, ["TTW"]=790005} 	-- 차단하기
g_kCommonPopup[4010] = {["Name"]="FRIEND_CANCEL_ISOLATE", ["TTW"]=790007} 	-- 차단해제하기
g_kCommonPopup[4011] = {["Name"]="FRIEND_MYHOME_INVITE"	, ["TTW"]=201099} 	-- 마이홈 초대

g_kCommonPopup[4101] = {["Name"]="MYHOME_FC_MOVE"		, ["TTW"]=405022} 	-- 가구이동
g_kCommonPopup[4102] = {["Name"]="MYHOME_FC_DEL"		, ["TTW"]=405023} 	-- 가구삭제

g_kCommonPopup[4201] = {["Name"]="MYHOME_HOUSE_INFO"	, ["TTW"]=400272} 	-- 집정보, 위치 보기

g_kCommonPopup[4301] = {["Name"]="MYHOME_CHAT_WHISPER"		, ["TTW"]=790000} 	-- 마이홈 채팅 귓속말
g_kCommonPopup[4302] = {["Name"]="MYHOME_CHAT_ADD_FRIEND"	, ["TTW"]=400508} 	-- 마이홈 채팅 친구추가
g_kCommonPopup[4303] = {["Name"]="MYHOME_CHAT_INVITATION"	, ["TTW"]=201099} 	-- 마이홈 채팅 초대장 발급
g_kCommonPopup[4304] = {["Name"]="MYHOME_CHAT_ROOM_MASTER"	, ["TTW"]=201797} 	-- 마이홈 방장 인계
g_kCommonPopup[4305] = {["Name"]="MYHOME_CHAT_ROOMLIST_WHISPER"		, ["TTW"]=790000} 	-- 마이홈 채팅 귓속말
g_kCommonPopup[4306] = {["Name"]="MYHOME_CHAT_ROOMLIST_ADD_FRIEND"	, ["TTW"]=400508} 	-- 마이홈 채팅 친구추가
g_kCommonPopup[4307] = {["Name"]="MYHOME_CHAT_KICK_GUEST"	, ["TTW"]=201815} 	-- 마이홈 채팅 강제 퇴장

--Expedition UI Menu
g_kCommonPopup[4500] = {["Name"]="EXPEDITION_DESTROY", ["TTW"]=710055}
g_kCommonPopup[4501] = {["Name"]="EXPEDITION_LEAVE", ["TTW"]=710054}
g_kCommonPopup[4502] = {["Name"]="EXPEDITION_KICK", ["TTW"]=710053}
g_kCommonPopup[4503] = {["Name"]="EXPEDITION_GIVE_OWN", ["TTW"]=710052}
g_kCommonPopup[4504] = {["Name"]="EXPEDITION_KICK_2", ["TTW"]=710031}
g_kCommonPopup[4505] = {["Name"]="EXPEDITION_MOVE_SHIFT", ["TTW"]=710032}
g_kCommonPopup[4506] = {["Name"]="EXPEDITION_MOVE_TEAM1", ["TTW"]=710033}
g_kCommonPopup[4507] = {["Name"]="EXPEDITION_MOVE_TEAM2", ["TTW"]=710034}
g_kCommonPopup[4508] = {["Name"]="EXPEDITION_MOVE_TEAM3", ["TTW"]=710035}
g_kCommonPopup[4509] = {["Name"]="EXPEDITION_MOVE_TEAM4", ["TTW"]=710036}
g_kCommonPopup[4510] = {["Name"]="EXPEDITION_OPTION", ["TTW"]=710015}
g_kCommonPopup[4511] = {["Name"]="EXPEDITION_JOIN", ["TTW"]=710065}

--Element Dungeon UI Menu
g_kCommonPopup[4550] = {["Name"]="INFO_ELEMENT_DUNGEON", ["TTW"]=799934}
g_kCommonPopup[4551] = {["Name"]="INFO_USE_ELEMENT_STONE", ["TTW"]=799935}
g_kCommonPopup[4552] = {["Name"]="ENTER_ELEMENT_DUNGEON", ["TTW"]=799936}
g_kCommonPopup[4553] = {["Name"]="EXCHANGE_CARDBOX", ["TTW"]=799939}
g_kCommonPopup[4554] = {["Name"]="EXCHANGE_SOCKETCARD", ["TTW"]=759903}

--Exchange Gather UI Menu
g_kCommonPopup[4560] = {["Name"]="EXCHANGE_GATHER", ["TTW"]=799950}

--INTERACTIVE_EMOTION
g_kCommonPopup[5000] = {["Name"]="INTERACTIVE_WALTZ", ["TTW"]=790910, ["ActionNo"]=100006601 }

-- CHAT BLOCK
g_kCommonPopup[5100] = {["Name"]="DELETE_CHAT_BLOCK_LIST", ["TTW"]=404030}
g_kCommonPopup[5101] = {["Name"]="ADD_CHAT_BLOCK_LIST", ["TTW"]=404031}

--g_kCommonPopup[10001]~??	NPC메뉴로 사용중

g_kCommonPopup_Selected = 1 --Default
g_kCommonPopup_Count = 0--Count
CommonItemHeight = 24
g_kCommonPopup_Cur = {}
--EX: CallCommonPopup( {"SHOP", "QUEST", "REPAIR"}, ?, ?, ?)
g_NpcManu_OpenPos = Point3(0,0,0)
g_NpcManu_MapNo = 0
function CallMinimapMyHomePopupMenu()
	local kPopupItem = {}
	if CheckExistHaveHome() then
		kPopupItem[1] = "MYHOME_GO_TO_HOME"
	end
	kPopupItem[2] = "MYHOME_INVITE_LIST"
	
	local kGuid = g_pilotMan:GetPlayerPilotGuid()
	local kCursorPos = GetCursorPos()
	
	kCursorPos:SetX( kCursorPos:GetX()-129)
	
	CallCommonPopup(kPopupItem, kGuid, nil, kCursorPos)
end

--////////////// Process Common Popup Message by CmdName //////////////
function ProcessCommonPopupMessageCmdName(kCmdName, kOwnerGuid)
	--WriteToConsole("CommonPopup 메세지는 2차적으로 이곳으로 옵니다\n");
	if IsExistUniqueType("NO_DUPLICATE") == true then  return end 
	ODS("ProcessCommonPopupMessageCmdName kCmdName : "..kCmdName.."\n");
	
	local kMyPilot = g_pilotMan:GetPlayerPilot()
	if kMyPilot:IsNil() then return end
	
	--NPC
	if "QUEST" == kCmdName then 
		NET_C_M_REQ_TRIGGER(1, kOwnerGuid, TRIGGER_ACTION_CLICKED)
	elseif "TACTICS_QUEST" == kCmdName then	
		NET_C_M_REQ_TRIGGER(1, kOwnerGuid, TRIGGER_ACTION_CLICKED)
	elseif "REPAIR" == kCmdName then
		CallRepairUI(kOwnerGuid)
	elseif "ENCHANT_SHIFT" == kCmdName then
		OnCallHelpSelect("ENCHANT_SHIFT_SELECT", 11)
	elseif "ENCHANT" == kCmdName then
		CallItemPlusUI(kOwnerGuid)
	elseif "RARITY_UPGRADE" == kCmdName then
		CallRarityUpgradeUI(kOwnerGuid)
	elseif "GUILD" == kCmdName then
		if HaveGuild() then
			CallNpcFunctionUI(103, kOwnerGuid, "Elvis_normal", 400541)
		else
			CallNpcFunctionUI(103, kOwnerGuid, "Elvis_normal", 400617)
		end
	elseif "SHOP" == kCmdName then
		NPC_Shop(kOwnerGuid)
	elseif "EMPORIA_SHOP" == kCmdName then
		NPC_EmporiaShop(kOwnerGuid)
	elseif "COIN" == kCmdName then
		local bUseDescription = LOCALE.NC_USA == GetLocale()
		if bUseDescription and 10 >= kMyPilot:GetAbil(AT_LEVEL) then
			CallTextDialog(10701, kOwnerGuid)
		else
			NPC_Shop(kOwnerGuid)
		end
	elseif "JOBSKILL_SHOP" == kCmdName then
		NPC_JobSkillShop(kOwnerGuid)
	elseif "SWORD_CP_SHOP" == kCmdName then
		CallTextDialog(10021, kOwnerGuid)
	elseif "MAGICIAN_CP_SHOP" == kCmdName then
		CallTextDialog(10031, kOwnerGuid)
	elseif "ARCHER_CP_SHOP" == kCmdName then
		CallTextDialog(10041, kOwnerGuid)
	elseif "THIEF_CP_SHOP" == kCmdName then
		CallTextDialog(10051, kOwnerGuid)
	elseif "SHAMAN_CP_SHOP" == kCmdName then
		CallTextDialog(10025, kOwnerGuid)
	elseif "DOUBLE_FIGHTER_CP_SHOP" == kCmdName then
		CallTextDialog(10035, kOwnerGuid)
	elseif "COUPON" == kCmdName then
		CallTextDialog(10060, kOwnerGuid)
	elseif "SWORD_CLASS_HELP" == kCmdName then
		CallTextDialog(10070, kOwnerGuid)
	elseif "MAGICIAN_CLASS_HELP" == kCmdName then
		CallTextDialog(10080, kOwnerGuid)
	elseif "ARCHER_CLASS_HELP" == kCmdName then
		CallTextDialog(10090, kOwnerGuid)
	elseif "THIEF_CLASS_HELP" == kCmdName then
		CallTextDialog(10100, kOwnerGuid)
--	elseif "SHAMAN_CLASS_HELP" == kCmdName then
--		CallTextDialog(10100, kOwnerGuid)
--	elseif "DOUBLE_FIGHTER_CLASS_HELP" == kCmdName then
--		CallTextDialog(10100, kOwnerGuid)
	elseif "INVSAFE" == kCmdName then
		CallInvSafeUI(kOwnerGuid)
		--ActivateUI("SFRM_INV_SAFE")
	elseif "GEMSTORE" == kCmdName then
		SendReqGemStore(kOwnerGuid)
	elseif "GEMSTORE_SHOP1" == kCmdName then
		SendReqDefGemStore(kOwnerGuid, 1)
	elseif "GEMSTORE_SHOP2" == kCmdName then
		SendReqDefGemStore(kOwnerGuid, 2)
	elseif "EVENT_SHOP" == kCmdName then
		SetGemStoreType(16)
		local kWnd = CallUI("SFRM_COLLECT_ANTIQUE")
		InitCollectAntiqueWnd(kWnd)
		kWnd:SetOwnerGuid(kOwnerGuid)
	elseif "COLLECT_ANTIQUE" == kCmdName then
		SetGemStoreType(6)
		local kWnd = CallUI("SFRM_COLLECT_ANTIQUE")
		InitCollectAntiqueWnd(kWnd)
		kWnd:SetOwnerGuid(kOwnerGuid)
--	elseif "EXCHANGE_SOCKETCARD" == kCmdName then
--		CallUI("SFRM_EXCHANGE_SOCKETCARD")
	elseif "LOTTERY" == kCmdName then
		ActivateUI("SFRM_SHINESTONE_LOTTERY")
	elseif "SHINE_UP" == kCmdName then
		ActivateUI("SFRM_SHINESTONE_CHANGE")
	elseif "COOKING" == kCmdName then
		ActivateUI("SFRM_ITEM_MAKING_COOK")
	elseif "TALK" == kCmdName or "TALK_ONLY" == kCmdName then
		GetQuestMan():CallNpcTalk(kOwnerGuid)
	elseif "WARNING" == kCmdName then
		GetQuestMan():CallNpcWarning(kOwnerGuid)
	elseif "FRAN" == kCmdName then
		if 10 > kMyPilot:GetAbil(AT_LEVEL) then
			CallTextDialog(10001, kOwnerGuid)
		else
			FranYesNoMsgBox(kOwnerGuid)
		end
	elseif "SOUL" == kCmdName then
		CallTextDialog(11400, kOwnerGuid)
	elseif "SOCKET_CREATE" == kCmdName then	
		CallSocketSystemUI(kOwnerGuid, 1)
	elseif "SOCKET_RESTORATION" == kCmdName then
		CallSocketSystemUI(kOwnerGuid, 2)
	elseif "SOCKET_RESET" == kCmdName then
		CallSocketSystemUI(kOwnerGuid, 3)	
	elseif "SOCKET_CARD_EXTRACTION" == kCmdName then
		CallSocketSystemUI(kOwnerGuid, 4)				
	elseif "SOCKET_MAKE" == kCmdName then
		SetSocketNpc(kOwnerGuid)
	elseif "MARRY_IF_TALK" == kCmdName then
		ActivateUI("SFRM_SWEETHEART_MSG_IF")
	elseif "MARRY_REQSEND" == kCmdName then
		ActivateUI("SFRM_SWEETHEART_REQ")
	elseif "MARRY_MONEY" == kCmdName then
		SetMarryTotalMoney()
	elseif "MARRY_MGS" == kCmdName then
		ActivateUI("SFRM_SWEETHEART_MSG_MARRY")
	elseif "MARRY_START" == kCmdName then
		SetMarryState(2)
	elseif "MISSION_EVENT" == kCmdName then
		if true == CanQuestTalk(kOwnerGuid) then
			CallTextDialog(10400, kOwnerGuid)
		else
			CallTextDialog(10600, kOwnerGuid)
		end
	elseif "JAPAN_EVENT" == kCmdName then
		if true == CanQuestTalk(kOwnerGuid) then
			CallTextDialog(11500, kOwnerGuid)
		else
			CallTextDialog(11600, kOwnerGuid)
		end
	elseif "EVENT_TW_EFFECTQUEST" == kCmdName then
		Call_EventTaiwanQuestEffect(kOwnerGuid)
	elseif "ITEMCONVERT" == kCmdName then
		CallItemConvertSystemUI(kOwnerGuid)
    elseif "CRAFT" == kCmdName then
		if 8 > kMyPilot:GetAbil(AT_LEVEL) then
			CallTextDialog(10011, kOwnerGuid)
		else
			CallRarityUpgradeSelectUI(kOwnerGuid)
		end
    elseif "CRAFT2" == kCmdName then
		if 8 > kMyPilot:GetAbil(AT_LEVEL) then
			CallTextDialog(10015, kOwnerGuid)
		else
			CallRarityUpgradeSelectUI(kOwnerGuid)
		end
	elseif "CRAFT_ITEM_CHANGE" == kCmdName then
		if 8 > kMyPilot:GetAbil(AT_LEVEL) then
			CallTextDialog(10011, kOwnerGuid)
		else
			CallRarityUpgradeSelectUI(kOwnerGuid)
		end	
	elseif "TRADE_UNSEALINGSCROLL" == kCmdName then
		CallTextDialog(10200, kOwnerGuid)
	elseif "TRADE_UNSEALINGSCROLL2" == kCmdName then
		CallTextDialog(10205, kOwnerGuid)
	
	elseif "RARITY_BUILD_UP" == kCmdName then
		CallItemBuildUpSelectUI()
	elseif "RARITY_AMPLIFY" == kCmdName then
		OnCallHelpSelect("ITEM_RARITY_AMPLIFY_SELECT", 8)
	elseif "DAILYQUEST_LV0" == kCmdName then
		CallDailyQuestUI(kOwnerGuid, 401024)
	elseif "DAILYQUEST_LV1" == kCmdName then
		CallDailyQuestUI(kOwnerGuid, 401025)
	elseif "DAILYQUEST_LV2" == kCmdName then
		CallDailyQuestUI(kOwnerGuid, 401026)
	elseif "WANTED_QUEST" == kCmdName then		
		if IsCanBuildWantedQuest() then
			CallUI("FRM_ACCEPT_WANTED_QUEST")
		else
			if IsCanRunWantedQuest() then
				CallWantedQuestUI()
			end
		end
	elseif "RANDOM_QUEST" == kCmdName then
		if 24 > kMyPilot:GetAbil(AT_LEVEL) then
			CallTextDialog(11120, kOwnerGuid)
		else
			if IsCanBuildRandomQuest() then
				CallTextDialog(11100, kOwnerGuid)
			else
				if IsCanRunRandomQuest() then
					CallRandomQuestUI(true);
				else
					CallTextDialog(11110, kOwnerGuid)
				end
			end
		end
	elseif "TACTICS_RANDOM_QUEST" == kCmdName then
		if not HaveGuild() or 20 > kMyPilot:GetAbil(AT_LEVEL) then
			CallTextDialog(11330, kOwnerGuid)
		else
			if IsCanBuildTacticsRandomQuest() then
				CallTextDialog(11320, kOwnerGuid)
			else
				if IsCanRunTacticsRandomQuest() then
					CallRandomTacticsQuestUI(true);
				else
					CallTextDialog(11310, kOwnerGuid)
				end
			end
		end
	elseif "GUILD_INVENTORY" == kCmdName then	
		SendGuildInvOpen(1, kOwnerGuid)
	elseif "CHRISTMAS_EVENT" == kCmdName then
		SendReqRealmQuestInfo("2800")
	elseif "HALLOWEEN_EVENT" == kCmdName then
		SendReqRealmQuestInfo("297")
	elseif "HELP_TALK" == kCmdName then
		ActivateUI("FRM_HELP_MAIN")
	elseif "TRADE_SOULSTONE" == kCmdName then
		CallTextDialog(10300, kOwnerGuid)
	elseif "POST" == kCmdName then
		CallPostUI(kOwnerGuid)
	elseif "MIXUPITEM" == kCmdName then
		CallCostumeMixNpcMenu(kOwnerGuid)
	elseif "SOUL_STONE_TRADE" == kCmdName then
		UpdateSoulStoneTrade()
		RegistUIAction(GetUIWnd("FRM_TRADE_SOULSTONE"), "CloseUI")
	elseif "EVENT_QUEST_SYS" == kCmdName then
		CallEventQuestNpcTalk(kOwnerGuid, true)
	elseif "EVENT_MISSION" == kCmdName then
		CallTextDialog(11000, kOwnerGuid)
	elseif "HIDDEN_MSG" == kCmdName then
		ActivateUI("FRM_HIDDEN_TEXT")
	--[[elseif "HIDDEN_OPEN" == kCmdName then
		Net_ReqEnterHidden_Npc()	--]]
	elseif "SKILL_MASTER" == kCmdName then
		if IsOpenUIMode7() then
			CallUI("SFRM_DEFENCE_LEARNSKILL")
		end
	elseif "BATTLE_SQUARE" == kCmdName then
		CallBattleSquareChannelUI(kOwnerGuid)
	elseif "MYHOME_HELP" == kCmdName then
		if IsMyhomeApt() then
			CallTextDialog(11400, kOwnerGuid)
		else
			CallTextDialog(10800, kOwnerGuid)
		end
	elseif "MYHOME_DECORATION" == kCmdName then
			ReserveMyHomeUseDecoUIEvent();		
	elseif "MYHOME_INVITATION" == kCmdName then
		if CheckExistHaveHome() then
			if CheckMyHome() then
				OnCallMyhomeInviteLetter()
			end
		else
			CommonMsgBoxByTextTable(201055, true)
		end
	elseif "MYHOME_TAX" == kCmdName then
		if CheckMyHome() then
			ReserveMyHomeTaxPayEvent()
		end
	elseif "MYHOME_AUCTION_MENU" == kCmdName then
		if CheckExistHaveHome() then
			RegistUIAction(CallUI("SFRM_MYHOME_AUCTION_INFO"), "CloseUI")
		else
			CommonMsgBoxByTextTable(201055, true)
		end
	elseif "MYHOME_AUCTION_INFO" == kCmdName then

	elseif "MYHOME_SIDEJOB" == kCmdName then
		if CheckMyHome() then
			OnClickMyHomeSideJobOnMaid()
		end
	elseif "MYHOME_REPAIR_DISCOUNT" == kCmdName then
		OnClickRepairService()
	elseif "MYHOME_ENCHANT_DISCOUNT" == kCmdName then
		OnClickEnchantService()
	elseif "MYHOME_NPC_MOVE" == kCmdName then
		OnClickMyHomeSideJobNpcMove(kOwnerGuid)
	elseif "MYHOME_NPC_REMOVE" == kCmdName then
		OnClickMyHomeSideJobNpcRemove(kOwnerGuid)
	elseif "MYHOME_SOUL_GOLD" == kCmdName then
		OnClickMyHomeSideJobSoulGold(kOwnerGuid)
	elseif "MYHOME_SOUL_COUNT" == kCmdName then
		OnClickMyHomeSideJobSoulCount(kOwnerGuid)
	elseif "MYHOME_SOCKET" == kCmdName then
		OnClickMyHomesSocket()
	elseif "MYHOME_SETTING" == kCmdName then
		ReserveMyHomeOptionEvent() 
	elseif "MYHOME_ENCHANT_RATE" == kCmdName then
		OnClickMyHomeSideJobEnchantRate(kOwnerGuid)
	elseif "MYHOME_SIDE_JOB_BOARD" == kCmdName then
		OnSendSideJobBoardList()
	elseif "MYHOME_INFO" == kCmdName then
		
	elseif "TOWN_HELP" == kCmdName then
		CallTextDialog(10900, kOwnerGuid)
	elseif "TOWN_BOARD" == kCmdName then
		OnSendTown_HomeList()
	elseif "TOWN_APT_BUY" == kCmdName then
		OpenMyhomeBuy()
	elseif "TOWN_APT_SELL" == kCmdName then
		RegistUIAction(CallUI("SFRM_MYHOME_SELL"), "CloseUI")
	elseif "TOWN_MOVE" == kCmdName then
		OnCallOpenTownList()
	elseif "MYHOME_GO_TO_HOME" == kCmdName then
		OnClickGoToMyHome()
	elseif "MYHOME_INVITE_LIST" == kCmdName then
		OnClickOrtherHomeInvitation()
	elseif "SHARE_SAFE_INV" == kCmdName then
		CallInvShareSafeUI(kOwnerGuid)
	elseif "JOB_SKILL_LEARN" == kCmdName then
		CallJobSkillLearnUI()
	elseif "JOB_SKILL3_CREATEITEM" == kCmdName then
		if IsJobSkill3Learn() then
			CallUI("SFRM_JL3_ITEM_CREATE")
		else
			CommonMsgBoxByTextTable(799556,true)
		end
	elseif "JOB_SKILL_SOUL_TRANSFER" == kCmdName then
		CallUI("SFRM_CONFIRM_SOULABIL_TRANSFER")
		local kTargetPilot = g_pilotMan:FindPilot(kOwnerGuid)
		if kTargetPilot:IsNil() then return end
		g_NpcManu_OpenPos = kTargetPilot:GetPos()
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_NpcManu_MapNo = g_world:GetMapNo()
	elseif "ELEMENTHELP" == kCmdName then
		RegistUIAction(CallUI("SFRM_ELEMENT_STORE_HELP"), "CloseUI")
	elseif "ELEMENT_STORE" == kCmdName then
		SendReqDefGemStore(kOwnerGuid, 5)
	--Player
	elseif "INVITE_PARTY" == kCmdName then
		Net_C_N_REQ_JOIN_PARTY_ByGuid(kOwnerGuid)
	elseif "ADD_FRIEND" == kCmdName then
		g_FriendMgr:SendFriend_Command_AddByGuid(kOwnerGuid)
	elseif "INVITE_PVP" == kCmdName then
		OnClickPvP_ReqDuel(kOwnerGuid)
	elseif "TRADE" == kCmdName then
		Net_PT_C_M_REQ_EXCHANGE_ITEM_REQ(kOwnerGuid)
	elseif "VIEW_INFO" == kCmdName then
		Net_PT_C_M_REQ_VIEW_OTHER_EQUIP(kOwnerGuid)
	elseif "JOIN_PARTY" == kCmdName then
		Net_C_N_ANS_JOIN_PARTY_EX(true, kOwnerGuid)
	elseif "FOLLOW" == kCmdName then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local kActor = g_world:FindActor(kOwnerGuid)
		if nil ~= kActor and not kActor:IsNil() then
			GetMyActor():RequestFollowActor(kActor:GetPilotGuid(),1);
		end
	elseif "CANCEL_FOLLOW" == kCmdName then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local kActor = g_world:FindActor(kOwnerGuid)
		if nil ~= kActor and not kActor:IsNil() then
			GetMyActor():RequestFollowActor(kActor:GetPilotGuid(),4);
		end	
	elseif "TAKE_PERSON" == kCmdName then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local kActor = g_world:FindActor(kOwnerGuid)
		if nil ~= kActor and not kActor:IsNil() then
			GetMyActor():RequestFollowActor(kActor:GetPilotGuid(),5);
		end
	elseif "CANCEL_TAKE_PERSON" == kCmdName then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local kActor = g_world:FindActor(kOwnerGuid)
		if nil ~= kActor and not kActor:IsNil() then
			GetMyActor():RequestFollowActor(kActor:GetPilotGuid(),6, true);
		end
	elseif "JOIN_PARTYFIND" == kCmdName then
		Net_C_N_REQ_JOIN_PARTYFIND_ByGuid(kOwnerGuid)		
	elseif "REQ_GUILD_GUID" == kCmdName then
		Send_ReqJoinGuildGuid(kOwnerGuid)	
	elseif "COPY_PCNAME_TO_CLIPBOARD" == kCmdName then	-- PlayerName Copy UI 
		CopyPlayerNameToClipBoard(kOwnerGuid)
	elseif "ORDER_CARD_VIEW" == kCmdName then
		CallCharacterCardUI(kOwnerGuid)
	elseif "RECOMMEND" == kCmdName then
		RecommendTarget(kOwnerGuid)
		
	--Party UI
	elseif "PARTY_LEAVE" == kCmdName then
		OnClick_Community_PartyPopup("LEAVE", kOwnerGuid)
	elseif "PARTY_KICK" == kCmdName then
		OnClick_Community_PartyPopup("KICKOUT", kOwnerGuid)
	elseif "PARTY_GIVE_OWN" == kCmdName then
		OnClick_Community_PartyPopup("CHANGEMASTER", kOwnerGuid)
	elseif "PARTY_INVITE" == kCmdName then
		CallPartyInvite()
	elseif "PARTY_OPTION" == kCmdName then
		CallPartyOption()
	elseif "WHISPER" == kCmdName then
		ChatModeChangeGuid(kOwnerGuid)
	elseif "PARTY_FOLLOW" == kCmdName then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local kActor = g_world:FindActor(kOwnerGuid)
		if nil ~= kActor and not kActor:IsNil() then
			GetMyActor():RequestFollowActor(kActor:GetPilotGuid(),1);
		end
	elseif "PARTY_WARP" == kCmdName then
		CallPartyWarp(kOwnerGuid)
	elseif "PARTYGROUND_WARP" == kCmdName then
		CallPartyGroundWarp(kOwnerGuid)
	elseif "PARTY_PEOPLE_CALL" == kCmdName then
		CallPartyPeopleCall(kOwnerGuid)
	elseif "PARTY_SEE_INFO" == kCmdName then
		Net_PT_C_M_REQ_VIEW_OTHER_EQUIP(kOwnerGuid)
	elseif "PARTY_TRADE" == kCmdName then
		Net_PT_C_M_REQ_EXCHANGE_ITEM_REQ(kOwnerGuid)
	elseif "PARTY_NAME_COPY_TO_CLIPBOARD" == kCmdName then
		CopyPlayerNameToClipBoard(kOwnerGuid)
	elseif "PARTY_FIND" == kCmdName then
		if g_EpFindWindow == 1 then
			CallPartyListFind()
		else
			CallUI("SFRM_PARTY_PART_FIND")
		end
	elseif "PEOPLE_FIND" == kCmdName then		
		CallPartyPeopleFind()
	--Couple UI
	elseif "REQ_COUPLE" == kCmdName then
		Send_ReqCoupleByGuid(kOwnerGuid, false)
	elseif "REQ_INSTANCE_COUPLE" == kCmdName then
		Send_ReqCoupleByGuid(kOwnerGuid, true)
		
	--Guild UI
	elseif "GUILD_LEAVE" == kCmdName then
		Send_LeaveGuild()
	elseif "GUILD_KICK" == kCmdName then
		Send_GuildKickUser(kOwnerGuid)
	elseif "GUILD_CHANGE_OWNER" == kCmdName then
		ReqChangeOwner(kOwnerGuid)
		--Send_ReqChangeOwner(kOwnerGuid)
	elseif "GUILD_SET_MASTER" == kCmdName then
		SendReqChangeMemberGrade(kOwnerGuid, 1) -- Master Grade
	elseif "GUILD_UNSET_MASTER" == kCmdName then
		SendReqChangeMemberGrade(kOwnerGuid, 255) -- User Grade
		
	-- EmporiaUI
	elseif "EM_ADMINISTRATION" == kCmdName then
		EmporiaTalkAdministrator(kOwnerGuid)
		--CallTextDialog(10400, kOwnerGuid)
		--CallNpcFunctionUI(200, kOwnerGuid, "c_William", 72011)		
		
	--FriendPopUp Menu	
	elseif "FRIEND_SEE_INFO" == kCmdName then		
		Net_PT_C_M_REQ_VIEW_OTHER_EQUIP(kOwnerGuid)
	elseif "FRIEND_INVITE_PARTY" == kCmdName then		
		Net_C_N_REQ_JOIN_PARTY_ByGuid(kOwnerGuid)
	elseif "FRIEND_TRADE" == kCmdName then		
		Net_PT_C_M_REQ_EXCHANGE_ITEM_REQ(kOwnerGuid)
	elseif "FRIEND_WHISPER" == kCmdName then			
		FriendPopUpWhisper()
	elseif "FRIEND_ONE_ON_ONE" == kCmdName then		
		MToMChatNewDlg(kOwnerGuid)
	elseif "FRIEND_INVITE_GUILD" == kCmdName then		
		Send_ReqJoinGuildGuid(kOwnerGuid)	
	elseif "FRIEND_INVITE_COUPLE" == kCmdName then
		Send_ReqCoupleByGuid(kOwnerGuid, false)
	elseif "FRIEND_DELETE" == kCmdName then
		g_FriendMgr:SendFriend_Command_Delete(kOwnerGuid)
		g_FriendMgr:Friend_List_UpdateReq(true)
	elseif "FRIEND_MYHOME_INVITE" == kCmdName then
		SendMyhomeInvitebyFriend(kOwnerGuid)
	elseif "GUILD_MYHOME_INVITE" == kCmdName then
		SendMyhomeInvitebyGuild(kOwnerGuid)
	elseif "PARTY_MYHOME_INVITE" == kCmdName then
		SendMyhomeInvitebyParty(kOwnerGuid)
--[[	elseif "FRIEND_ISOLATE" == kCmdName then
		WriteToConsole("차단하기 선택됨\n")
		g_FriendMgr:SendFriend_Command_ChatStateChange(kOwnerGuid, 1)	
	elseif "FRIEND_CANCEL_ISOLATE" == kCmdName then
		WriteToConsole("차단해제 선택됨\n")
		g_FriendMgr:SendFriend_Command_ChatStateChange(kOwnerGuid, 0)
]]
	elseif "MYHOME_FC_MOVE" == kCmdName then
		SetMovingFurniture()
	elseif "MYHOME_FC_DEL" == kCmdName then
		RemoveFurniture()
	elseif "MYHOME_HOUSE_INFO" == kCmdName then
		OnCallTownMinimap()
		SavePickedHouse(kOwnerGuid)	--이때만 저장. OnCallTownMinimap뒤에 있어야 함
	elseif "IGGACHA" == kCmdName then 
		local kWnd = GetUIWnd("SFRM_INGAME_GACHA")
		if(true == kWnd:IsNil()) then
			ReqIGGachaList()
		end
		
	-- TransTower
	elseif "SAVEPOS" == kCmdName then
		local kUI = ActivateUI( "SFRM_TRANSTOWER_SAVEPOS", true )
		if false == kUI:IsNil() then
			kUI:SetOwnerGuid( kOwnerGuid )
		end
	elseif "TRANSPORT" == kCmdName then
		CallNpcTRANSPORT(kOwnerGuid)
	elseif "MON_CARD2_MIX" == kCmdName then
		OnCallMonsterCardMixerType2()
	elseif "GEMHELP" == kCmdName then
		RegistUIAction(CallUI("SFRM_LUCKY_STONE_NFY"), "CloseUI")
		
	--PvP League
	elseif "PVPLEAGUE_REGIST" == kCmdName then
		SendPvPLeague_QueryTeam()
	elseif "PVPLEAGUE_MOVE" == kCmdName then
		ActivateUI("SFRM_JOIN_LEAGUE_LOBBY")
		
	elseif "MYHOME_CHAT_WHISPER" == kCmdName then
		PopupMyhomeChatWhisper(kOwnerGuid)
	elseif "MYHOME_CHAT_ADD_FRIEND" == kCmdName then
		g_FriendMgr:SendFriend_Command_AddByGuid(kOwnerGuid)
	elseif "MYHOME_CHAT_INVITATION" == kCmdName then
		PopupMyhomeChatInvitation(kOwnerGuid)
	elseif "MYHOME_CHAT_ROOM_MASTER" == kCmdName then
		PopupMyhomeChatRoomMaster(kOwnerGuid)
	elseif "MYHOME_CHAT_KICK_GUEST" == kCmdName then
		PopupMyhomeChatKickGuest(kOwnerGuid)
	elseif "MYHOME_CHAT_ROOMLIST_WHISPER" == kCmdName then
		PopupMyhomeChatRoomListWhisper(kOwnerGuid)
	elseif "MYHOME_CHAT_ROOMLIST_ADD_FRIEND" == kCmdName then
		PopupMyhomeChatRoomListAddFriend(kOwnerGuid)
	elseif "EXCHANGE_PET_EXP" == kCmdName then
		local kUI = ActivateUI( "SFRM_CHANGE_EXP_POTION", false )
		if false == kUI:IsNil() then
			kUI:SetOwnerGuid( kOwnerGuid )
		end
	elseif "PET_UPGRADE" == kCmdName then
		PetUpgradeCall()
	elseif "INTERACTIVE_EMOTION" == kCmdName then
		--ODS("INTERACTIVE_EMOTION", false, 912)
		local kPopupItem = nil
		kPopupItem = {}
		kPopupItem[1] = "INTERACTIVE_WALTZ"
		
		-- 내 플레이어에게 대상의 GUID를 기억하게하고
		local kMyActor = g_pilotMan:GetPlayerActor()
		Set_InteractiveEmotion_TargetActor(kMyActor, kOwnerGuid, true);
		
		-- 대상 플레이어도, 내 GUID를 기억하게 한다
		local kTargetPilot = g_pilotMan:FindPilot(kOwnerGuid)
		if kTargetPilot:IsNil() then return end
		local kTargetActor = kTargetPilot:GetActor()
		if kTargetActor:IsNil() then return end
		Set_InteractiveEmotion_TargetActor(kTargetActor, g_pilotMan:GetPlayerPilotGuid(), false)
		
		if(kMyActor:GetAbil(AT_GENDER) == kTargetActor:GetAbil(AT_GENDER)) then
		-- 동성끼리는 할수 없음
			AddWarnDataTT(790904)
			return
		end

		if(nil ~= kPopupItem) then
			CallChatPopup(kPopupItem, GetTT(790900), g_kPopPos)
		end	
	elseif "EXPEDITION" == kCmdName then
		if false == kOwnerGuid:IsNil() then
			SetExpeditionNpcGuid(kOwnerGuid)	-- 그리고 원정대NPC의 GUID를 원정대 클래스에 저장한다.
			CallUI("FRM_EXPEDITION_INFO")		-- 원정대 NPC를 선택했을 때 모집공고를 보여준다.
		end
	elseif "EXPEDITION_DESTROY" == kCmdName then
		CallYesNoMsgBox(GetTT(720012), GUID(), MBT_EXPEDITION_DESTROY)	-- 원정대 해산을 선택했을 때 팝업창을 보여준다.
	elseif "EXPEDITION_LEAVE" == kCmdName then
		CallYesNoMsgBox(GetTT(720013), GUID(), MBT_EXPEDITION_LEAVE)	-- 원전대 탈퇴를 선택했을 때 팝업창을 보여준다.
	elseif "EXPEDITION_KICK" == kCmdName or "EXPEDITION_KICK_2" == kCmdName then
		ReqKickoutExpedition(kOwnerGuid)								-- 원정대 추방을 선택했을 때
	elseif "EXPEDITION_MOVE_SHIFT" == kCmdName then
		ReqMoveTeamExpedition(kOwnerGuid, 5)							-- 원정대원을 교체대기로 이동 시켰을 때
	elseif "EXPEDITION_MOVE_TEAM1" == kCmdName then
		ReqMoveTeamExpedition(kOwnerGuid, 1)							-- 1번팀으로 이동
	elseif "EXPEDITION_MOVE_TEAM2" == kCmdName then
		ReqMoveTeamExpedition(kOwnerGuid, 2)							-- 2번팀으로 이동
	elseif "EXPEDITION_MOVE_TEAM3" == kCmdName then
		ReqMoveTeamExpedition(kOwnerGuid, 3)							-- 3번팀으로 이동
	elseif "EXPEDITION_MOVE_TEAM4" == kCmdName then
		ReqMoveTeamExpedition(kOwnerGuid, 4)							-- 4번팀으로 이동
	elseif "EXPEDITION_OPTION" == kCmdName then
		if IsExpeditionMaster(g_pilotMan:GetPlayerPilotGuid()) then		-- 원정대 설정을 선택하면 설정창을 보여준다.
			CallUI("FRM_EXPEDITION_OPTION")
		end
	elseif "EXPEDITION_GIVE_OWN" == kCmdName then
		ReqChangeMasterExpedition(kOwnerGuid)							-- 원정대장 위임했을 때
	elseif "EXPEDITION_JOIN" == kCmdName then
		Send_C_N_REQ_JOIN_EXPEDITION(kOwnerGuid)						-- 원정대 가입신청
	elseif "INFO_ELEMENT_DUNGEON" == kCmdName then						-- 정령던전이란?
		OnClickDetailHelpViewMainByID(12, true)
	elseif "INFO_USE_ELEMENT_STONE" == kCmdName then					-- 정령석 합성 방법
		OnClickDetailHelpViewMainByID(13, true)
	elseif "ENTER_ELEMENT_DUNGEON" == kCmdName then						-- 정령던전 입장
		CallElementDungeonUI()
	elseif "EXCHANGE_CARDBOX" == kCmdName then							-- 카드상자 교환
		SendReqDefGemStore(kOwnerGuid, 8)
	elseif "EXCHANGE_SOCKETCARD" == kCmdName then						-- 소켓카드 교환
		CallUI("SFRM_EXCHANGE_SOCKETCARD")
	elseif "EXCHANGE_GATHER" == kCmdName then
		SendReqDefGemStore(kOwnerGuid, 9)
	elseif "EVENTSTORE" == kCmdName then
		SendReqEventStore(kOwnerGuid)
	elseif "COMMUNITY_EVENT" == kCmdName then
		CallUI("SFRM_COMMUNITY_EVENT")
	elseif "EXCHANGE_ELUNIUM" == kCmdName then
		CallUI("SFRM_EXCHANGE_ELUNIUM")
	elseif "VALKYRIE_STORE" == kCmdName then
		SendReqDefGemStore(kOwnerGuid, 12, true)
	elseif "EXCHANGE_PURESILVER_KEY" == kCmdName then
		CallUI("SFRM_EXCHANGE_PURESILVER_KEY")
	elseif "RAGNAROK_BLESS" == kCmdName then
		ReqRagnarokBless(kOwnerGuid)
	elseif "ITEM_STORE" == kCmdName then
		SendReqDefGemStore(kOwnerGuid, 14, true)
	elseif "ITEM_TREE" == kCmdName then
		SendReqDefGemStore(kOwnerGuid, 15, true)
	elseif "BUFF" == kCmdName then
		ReqRagnarokBless(kOwnerGuid)	
	elseif "ITEM_SWAP" == kCmdName then
		SendReqDefGemStore(kOwnerGuid, 16)
	elseif "ITEM_PLUS" == kCmdName then
		SendReqDefGemStore(kOwnerGuid, 17)	
	end
	--//
end

function CallMyHomeFurnitureMenu()
	local kPopupItem = {}
	kPopupItem[1] = "MYHOME_FC_MOVE"
	kPopupItem[2] = "MYHOME_FC_DEL"
	
	local kGuid = g_pilotMan:GetPlayerPilotGuid()
	local kCursorPos = GetCursorPos()
	
	CallCommonPopup(kPopupItem, kGuid, nil, kCursorPos)
end

--////////////// Process Common Popup Message by CmdName And OwnerName //////////////
function ProcessNamePopupMessageCmdName(kCmdName, kOwnerName, iCommandType)
	ODS("ProcessNamePopupMessageCmdName(kCmdName, kOwnerName)\n", false, 912)
	--Player 팝업 메뉴 클릭시 나오는 명령들 선택시 이곳으로 옴.
	if "INVITE_PARTY" == kCmdName then
		Net_C_N_REQ_JOIN_PARTY_ByName(WideString(kOwnerName))
	elseif "ADD_FRIEND" == kCmdName then
		g_FriendMgr:SendFriend_Command_AddByName(WideString(kOwnerName))
	elseif "REQ_GUILD_NAME" == kCmdName then
		Send_ReqJoinGuildName(WideString(kOwnerName))
	elseif "INTERACTIVE_WALTZ" == kCmdName then
		local kMyActor = g_pilotMan:GetPlayerActor()
		local StrTargetGuid = kMyActor:GetParam(INTERACTIVE_EMOTION_TARGET_GUID_INDEX)
		local kTargetGuid = GUID(StrTargetGuid)
		
		local iActionNo = g_kCommonPopup[iCommandType]["ActionNo"]
		--ODS("iActionNo:"..iActionNo.."\n", false, 912)
		Net_PT_C_M_REQ_INTERACTIVE_EMOTION_REQUEST(g_pilotMan:GetPlayerPilotGuid(), kTargetGuid, iActionNo)
		CommonMsgBoxByTextTable(790902, true, nil)
	elseif "ONE_ON_ONE" == kCmdName then
		if true == g_kGuid:IsNil() then
			return
		end
		MToMChatNewDlg(g_kGuid)
	elseif "CHATWND_ONE_ON_ONE" == kCmdName then
		if true == g_kChatLogGuid:IsNil() then
			return
		end
		ODS("CHATWND_ONE_ON_ONE\n",false,927)
		MToMChatNewDlg(g_kChatLogGuid)
	elseif "ADD_CHAT_BLOCK_LIST" == kCmdName then
		g_ChatMgrClient:Regist_ChatBlockUser(WideString(kOwnerName))
		g_ChatMgrClient:UpdateChatBlockList(GetUIWnd("SFRM_COMMUNITY"):GetControl("FRM_BLOCK"))
	elseif "DELETE_CHAT_BLOCK_LIST" == kCmdName then
		g_ChatMgrClient:UnRegist_ChatBlockUser(WideString(kOwnerName))
		g_ChatMgrClient:UpdateChatBlockList(GetUIWnd("SFRM_COMMUNITY"):GetControl("FRM_BLOCK"))
	end
end

function ChatModeChangeGuid(kGuid, bFocus)
	local kName = g_ChatMgrClient:GuidToName(kGuid)
	if true == kName:IsNil() then 
		local kPilot = g_pilotMan:FindPilot(kGuid)
		if true == kPilot:IsNil() then
			return
		end
		
		kName = kPilot:GetName()
		if true == kName:IsNil() then
			return
		end
	end
	ChatModeChangeWhisper(kName:GetStr(), bFocus)
end

function ChatModeChangeWhisper(kName, bFocus)
	if kName == nil or string.len(kName) == 0 then
		return
	end
	local kMyName = g_pilotMan:GetPlayerPilot():GetName():GetStr()
	if kName == kMyName then
		return
	end
	local kWnd = GetUIWnd("ChatBar")
	if true == kWnd:IsNil() then
		kWnd = CallUI("ChatBar")
	end
	g_ChatMgrClient:CheckChatCommand(WideString("/MODE 4 "..kName))
	
	if bFocus then
		OnEnter_FocusEdit(GetUIWnd("ChatBar"))
		local kEditWnd = kChatWnd:GetControl("EDT_CHAT")
		if kEditWnd:IsNil() then return end
		kEditWnd:SetEditFocus(false)
	end
end

g_kChatLogGuid = 0
function ProcessChatUserPopup(UISelf)
	local kName = UISelf:GetCustomDataAsStr()
	if true == kName:IsNil() then
		return
	end
	local kMyName = g_pilotMan:GetPlayerPilot():GetName():GetStr()
	if kMyName == kName:GetStr() then
		return
	end
	g_kChatLogGuid = g_ChatMgrClient:Name2Guid_Find_ByName(kName)
	local index = 1
	local kPopupItem = {}
	kPopupItem[index] = "INVITE_PARTY"
	index = index + 1
	if false == IsMyFriend(g_kChatLogGuid) then
		kPopupItem[index] = "ADD_FRIEND"
		index = index + 1
	end
	if (true  == HaveGuild()) and (true  == AmIGuildMaster())	and (false == IsMyGuildMemeber(g_kChatLogGuid))	then
		kPopupItem[index] = "REQ_GUILD_NAME"
		index = index + 1
	end
	kPopupItem[index] = "CHATWND_ONE_ON_ONE"
	index = index + 1
	-- 유저 차단 메뉴 팝업.
	kPopupItem[index] = "ADD_CHAT_BLOCK_LIST"
	index = index + 1
	
	CallChatPopup(kPopupItem, kName)
	ODS("ProcessChatUserPopup(UISelf)\n", false, 912)
end

g_kGuid = 0
function ProcessNotifyUserPopup(UISelf)
	local kName = UISelf:GetCustomDataAsStr()
	if true == kName:IsNil() then
		return
	end
	local kMyName = g_pilotMan:GetPlayerPilot():GetName():GetStr()
	if kMyName == kName:GetStr() then
		return
	end
	g_kGuid = g_ChatMgrClient:GetNotifyGuid()
	local index = 1
	local kPopupItem = {}
	kPopupItem[index] = "INVITE_PARTY"
	index = index + 1
	if false == IsMyFriend(g_kGuid) then
		kPopupItem[index] = "ADD_FRIEND"
		index = index + 1
	end
	if (true  == HaveGuild()) and (true  == AmIGuildMaster()) and (false == IsMyGuildMemeber(g_kGuid)) 	then
		kPopupItem[index] = "REQ_GUILD_NAME"
		index = index + 1
	end
	kPopupItem[index] = "ONE_ON_ONE"

	CallChatPopup(kPopupItem, kName)
	ODS("ProcessChatUserPopup(UISelf)\n", false, 912)
end

function CallChatPopup(kCmdArray, kName, kPopPos)
	local kWnd = CallUI("FRM_NAME_CMD_POPUP")
	if nil == kWnd or kWnd:IsNil() then return end
	local kList = kWnd:GetControl("QUEST_LIST")
	if nil == kList or kList:IsNil() then return end
	kList:ClearAllListItem()
	g_kCommonPopup_Cur = {}
	
	kWnd:SetStaticText(kName:GetStr())
	
	--Step 1.
	local iCount = 0
	for kKey, kVal in pairs(kCmdArray) do
		local iTTW, iNo = 0, 0
		for kSetKey, kSetVal in pairs(g_kCommonPopup) do
			if kSetVal["Name"] == kVal then
				iNo = kSetKey
				iTTW = kSetVal["TTW"]
				iCount = iCount + 1
				
				g_kCommonPopup_Cur[iCount] = iNo--Save
			end
		end
		
		if 0 ~= iTTW and 0 ~= iNo  then
			local kNewItem = kList:AddNewListItemChar("")
			if nil ~= kNewItem and not kNewItem:IsNil() then
				local kItemWnd = kNewItem:GetWnd()
				if nil ~= kItemWnd and not kItemWnd:IsNil() then
					local kBtn = kItemWnd:GetControl("BTN_1")
					if nil ~= kBtn and not kBtn:IsNil() then
						kBtn:SetStaticTextW(GetTT(iTTW))
						kBtn:SetCustomDataAsInt(iNo)
					end
				end
			end
		end
	end
	
	if 0 == iCount then
		kWnd:Close()
	end	
	
	--Step 2.
	local kBG1 = kWnd:GetControl("FRM_BG1")
	if nil == kBG1 or kBG1:IsNil() then return end
	local kBG2 = kWnd:GetControl("FRM_BG2")
	if nil == kBG2 or kBG2:IsNil() then return end
	local kWndSize = kWnd:GetSize()
	local kSize = kList:GetSize()
	local kBgSize1 = kBG1:GetSize()
	local kBgSize2 = kBG2:GetSize()
	local iItemH = CommonItemHeight
	local kCursorPos = GetCursorPos()
	kSize:SetY(iItemH * iCount)
	kBgSize1:SetY(iItemH * iCount + 6)
	kBgSize2:SetY(iItemH * iCount + 8)
	kWndSize:SetY(kBgSize2:GetY() + 19)
	kList:SetSize(kSize)
	kBG1:SetSize(kBgSize1)
	kBG2:SetSize(kBgSize2)
	kWnd:SetSize(kWndSize)
	
	if nil ~= kPopPos then
		--ODS("kPopPos\n", false, 912)
		kWnd:SetLocation(kPopPos)
	else
		--ODS("kCursorPos\n", false, 912)
		kWnd:SetLocation(kCursorPos)
	end	
	
	g_kCommonPopup_Count = iCount
	CommonPopup_CurPos(kWnd, 1)--Init Pos
	RegistUIAction(kWnd, "CloseUI")	
end

--////////////// Call Common Popup //////////////
g_kPopPos = nil
function CallCommonPopup(kCmdArray, kGuid, kActor, kPopPos)
--	WriteToConsole("CommonPopup최초 호출시 CallCommonPopup이 호출됩니다\n");
	if IsExistUniqueType("NO_DUPLICATE") == true then  return end -- 중복 팝업 금지.
	if GetEventScriptSystem():IsNowActivate() then return end
	
	local kWnd = CallUI("FRM_COMMON_POPUP")
	if nil == kWnd or kWnd:IsNil() then return end
	local kList = kWnd:GetControl("QUEST_LIST")
	if nil == kList or kList:IsNil() then return end
	kList:ClearAllListItem()
	g_kCommonPopup_Cur = {}
	
	--Step 1.
	local iCount = 0
	for kKey, kVal in pairs(kCmdArray) do
		local iTTW, iNo = 0, 0
		for kSetKey, kSetVal in pairs(g_kCommonPopup) do
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			if GATTR_BATTLESQUARE == g_world:GetAttr() then	-- 배틀스퀘어에서 따라가기 팝업창 삭제 (2010. 06. 28 조현건)
				if kSetVal["Name"] ~= "FOLLOW" and
				kSetVal["Name"] ~= "PARTY_FOLLOW" then
					if kSetVal["Name"] == kVal then
						iNo = kSetKey
						iTTW = kSetVal["TTW"]
						iCount = iCount + 1
						
						g_kCommonPopup_Cur[iCount] = iNo--Save
					end				
				end
			else				
				if kSetVal["Name"] == kVal then
					iNo = kSetKey
					iTTW = kSetVal["TTW"]
					iCount = iCount + 1
					
					g_kCommonPopup_Cur[iCount] = iNo--Save
				end
			end
			
		end
		
		if 0 ~= iTTW and 0 ~= iNo  then
			local kNewItem = kList:AddNewListItemChar("")
			if nil ~= kNewItem and not kNewItem:IsNil() then
				local kItemWnd = kNewItem:GetWnd()
				if nil ~= kItemWnd and not kItemWnd:IsNil() then
					local kBtn = kItemWnd:GetControl("BTN_1")
					if nil ~= kBtn and not kBtn:IsNil() then
						kBtn:SetStaticTextW(GetTT(iTTW))
						kBtn:SetCustomDataAsInt(iNo)
						
						if nil ~= kGuid and not kGuid:IsNil() then-- use this or Step 4.
							kBtn:SetOwnerGuid(kGuid)
						end
					end
				end
			end
		end
	end
	
	if 0 == iCount then
		kWnd:Close()
	end
	
	--Step 2.
	local kBG1 = kWnd:GetControl("FRM_BG1")
	if nil == kBG1 or kBG1:IsNil() then return end
	local kBG2 = kWnd:GetControl("FRM_BG2")
	if nil == kBG2 or kBG2:IsNil() then return end
	local kWndSize = kWnd:GetSize()
	local kSize = kList:GetSize()
	local kBgSize1 = kBG1:GetSize()
	local kBgSize2 = kBG2:GetSize()
	local iItemH = CommonItemHeight
	kSize:SetY(iItemH * iCount)
	kBgSize1:SetY(iItemH * iCount + 6)
	kBgSize2:SetY(iItemH * iCount + 8)
	kWndSize:SetY(kBgSize2:GetY() + 19)
	kList:SetSize(kSize)
	kBG1:SetSize(kBgSize1)
	kBG2:SetSize(kBgSize2)
	kWnd:SetSize(kWndSize)
	
	--Step 3.
	if nil ~= kGuid and not kGuid:IsNil() then
		kWnd:SetOwnerGuid(kGuid)
	end
	
	--Step 4.
	if nil ~= kActor and not kActor:IsNil() then
		--Set Actor Name
		local kPilot = kActor:GetPilot()
		if nil ~= kPilot and not kPilot:IsNil() then
			local kPilotName = kPilot:GetName()
			kPilotName:EraseCRLF()
			kWnd:SetStaticTextW(kPilotName)
		end
		kWnd:AdjustToOwnerPos() --Only Actor is not nil
	else
		kWnd:SetStaticText("")
	end
	if nil ~= kPopPos then
		g_kPopPos = kPopPos		
		kWnd:SetLocation(kPopPos)
	else
		g_kPopPos = kWnd:GetLocation()
	end
	
	g_kCommonPopup_Count = iCount
	CommonPopup_CurPos(kWnd, 1)--Init Pos

	RegistUIAction(kWnd, "CloseUI")
end

function ProcessNamePopupMessage(iCommandType, kOwnerName)
	if nil == iCommandType or 0 == iCommandType then return end
	if nil == kOwnerName or 0 == string.len(kOwnerName) then return end
	local kPopupSet = g_kCommonPopup[iCommandType]
	if nil == kPopupSet then return end
	local kCmdName = kPopupSet["Name"]
	ProcessNamePopupMessageCmdName(kCmdName, kOwnerName, iCommandType)
end

--////////////// Process Common Popup Message //////////////
function ProcessCommonPopupMessage(iCommandType, kOwnerGuid)
--	WriteToConsole("모든 CommonPopUp메세지는 1차적으로 이곳으로 옵니다. ProcessCommonPopupMessage()\n");
	if nil == iCommandType or 0 == iCommandType then return end
	if nil == kOwnerGuid or kOwnerGuid:IsNil() then return end
	if IsExistUniqueType("NO_DUPLICATE") == true then  return end 

	local kPopupSet = g_kCommonPopup[iCommandType]
	if nil == kPopupSet then return end
	
	local kFunc = kPopupSet["Name"]
	ProcessCommonPopupMessageCmdName(kFunc, kOwnerGuid)
end

--////////////// Key Event UP //////////////
function OnUp_CommonPopup(kTopWnd)
	if nil == kTopWnd or kTopWnd:IsNil() then return end
	
	local iCur = g_kCommonPopup_Selected
	iCur = iCur - 1
	if 0 >= iCur then
		iCur = g_kCommonPopup_Count
	end
	
	CommonPopup_CurPos(kTopWnd, iCur)
end

--////////////// Key Event DOWN //////////////
function OnDown_CommonPopup(kTopWnd)
	if nil == kTopWnd or kTopWnd:IsNil() then return end
	
	local iCur = g_kCommonPopup_Selected
	iCur = iCur + 1
	if g_kCommonPopup_Count < iCur then
		iCur = 1
	end
	
	CommonPopup_CurPos(kTopWnd, iCur)
end

--////////////// Key Event Enter //////////////
function OnEnter_CommonPopup(kTopWnd)
--	WriteToConsole("키보드로 선택된 경우 이게 호출됩니다\n")
	if nil == kTopWnd or kTopWnd:IsNil() then return end
	local kOwnerGuid = kTopWnd:GetOwnerGuid()
	local iCommandNo = g_kCommonPopup_Cur[g_kCommonPopup_Selected]
	kTopWnd:Close()
	ProcessCommonPopupMessage(iCommandNo, kOwnerGuid)
end

function OnEnter_CommonPopupToName(kTopWnd)
	ODS("OnEnter_CommonPopupToName(kTopWnd)\n", false, 912)
	if nil == kTopWnd or kTopWnd:IsNil() then return end
	local iCommandNo = g_kCommonPopup_Cur[g_kCommonPopup_Selected]
	local Text = kTopWnd:GetStaticText():GetStr()
	kTopWnd:Close()
	ProcessNamePopupMessage(iCommandNo, Text)
end

--////////////// CommonPopup_CurPos //////////////
function CommonPopup_CurPos(kTopWnd, iCur)
	if nil == kTopWnd or kTopWnd:IsNil() then return end
	local kCurWnd = kTopWnd:GetControl("BTN_CUR")
	if nil == kCurWnd or kCurWnd:IsNil() then return end
	
	local kPos = kCurWnd:GetLocation()
	kPos:SetY( (iCur-1) * CommonItemHeight + 18 )
	kCurWnd:SetLocation(kPos)
	
	g_kCommonPopup_Selected = iCur
end

function SetTextOnThisUI(iTextID1, iTextID2)
	
	-- is exist useable UI ?
	if( nil == UISelf ) then	
		return 
	end	
	if( true == UISelf:IsNil() ) then
		return
	end
	
	-- add Text and set Text to current UI
	local Text1 = GetTT(iTextID1)
	local Text2 = GetTT(iTextID2)
	Text1:Add( Text2:GetStr() )
	UISelf:SetStaticTextW( Text1 )
end

function UVAnimation(kSelf, fInterTime, iUVIndexMin, iUVIndexMax, iIncreseIndex)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end	
	if(0 == fInterTime) then return end
	
	if(nil == iUVIndexMax or nil == iUVIndexMin or nil == iIncreseIndex) then
		iUVIndexMax = kSelf:GetMaxUVIndex()
		iUVIndexMin = 1
		iIncreseIndex = 1
	else
		if(0 >= iUVIndexMax or 0 >= iUVIndexMin or 0 >= iIncreseIndex) then 
			iUVIndexMax = kSelf:GetMaxUVIndex()
			iUVIndexMin = 1
			iIncreseIndex = 1
		end
	end
	
	local TimeCnt = kSelf:GetCustomDataAsFloat()
	TimeCnt= TimeCnt+GetFrameTime()
	if(fInterTime < TimeCnt) then 
		TimeCnt = 0
		local iNextIndex = kSelf:GetUVIndex()+1
		local iMod = iUVIndexMax+1
		iNextIndex = iNextIndex % iMod
		if(iNextIndex == 0) then  iNextIndex = iUVIndexMin end	
		kSelf:SetUVIndex(iNextIndex)
	end	
	kSelf:SetCustomDataAsFloat(TimeCnt)
end

function UVAnimationRevers(kSelf, fInterTime, iUVIndexMax, iUVIndexMin, iDecreseIndex, bLoop)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end	
	if(0 == fInterTime) then return end
	
	if(nil == iUVIndexMax or nil == iUVIndexMin or nil == iIncreseIndex) then
		iUVIndexMax = kSelf:GetMaxUVIndex()
		iUVIndexMin = 1
		iIncreseIndex = 1
	else
		if(0 >= iUVIndexMax or 0 >= iUVIndexMin or 0 >= iIncreseIndex) then 
			iUVIndexMax = kSelf:GetMaxUVIndex()
			iUVIndexMin = 1
			iIncreseIndex = 1
		end
	end
	
	local TimeCnt = kSelf:GetCustomDataAsFloat()
	TimeCnt= TimeCnt+GetFrameTime()
	if(fInterTime < TimeCnt) then 
		TimeCnt = 0
		local iNextIndex = kSelf:GetUVIndex()-1
		if(iNextIndex <= iUVIndexMin) then  
			if( bLoop == true ) then
				iNextIndex = iUVIndexMax 
			else
				iNextIndex = iUVIndexMin
			end
		end	
		kSelf:SetUVIndex(iNextIndex)
	end	
	kSelf:SetCustomDataAsFloat(TimeCnt)
end

function UVAnimationReversFlowTime(kSelf, fInterTime, iUVIndexMax, iUVIndexMin, iDecreseIndex, bLoop)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end
	if(0 == fInterTime) then return end
	local kReentry = kSelf:GetControl("DMY_STARTTIME")
	if(true == kReentry:IsNil()) then return end
	
	if(nil == iUVIndexMax or nil == iUVIndexMin or nil == iIncreseIndex) then
		iUVIndexMax = kSelf:GetMaxUVIndex()
		iUVIndexMin = 1
		iIncreseIndex = 1
	else
		if(0 >= iUVIndexMax or 0 >= iUVIndexMin or 0 >= iIncreseIndex) then 
			iUVIndexMax = kSelf:GetMaxUVIndex()
			iUVIndexMin = 1
			iIncreseIndex = 1
		end
	end
	
	local fNowTime = GetAccumTime()
	local fReentryTime = kReentry:GetCustomDataAsFloat()
	kReentry:SetCustomDataAsFloat(fNowTime)
	
	local TimeCnt = kSelf:GetCustomDataAsFloat()
	TimeCnt= TimeCnt + (fNowTime-fReentryTime)
	if(fInterTime < TimeCnt) then
		local iNextIndex = kSelf:GetUVIndex()-math.floor(TimeCnt)
		TimeCnt = 0
		if(iNextIndex <= iUVIndexMin) then  
			if( bLoop == true ) then
				iNextIndex = iUVIndexMax 
			else
				iNextIndex = iUVIndexMin
			end
		end	
		kSelf:SetUVIndex(iNextIndex)
	end	
	kSelf:SetCustomDataAsFloat(TimeCnt)
end

function UVAnimationBearTimer(kSelf, BearTeam, fInterTime, iUVIndexMin, iUVIndexMax)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end	
	if(0 == fInterTime) then return end
	
	if(nil == iUVIndexMax or nil == iUVIndexMin) then
		iUVIndexMax = kSelf:GetMaxUVIndex()
		iUVIndexMin = 1
	else
		if(0 >= iUVIndexMax or 0 >= iUVIndexMin) then 
			iUVIndexMax = kSelf:GetMaxUVIndex()
			iUVIndexMin = 1
		end
	end
	
	local TimeCnt = kSelf:GetCustomDataAsFloat()
	TimeCnt= TimeCnt+GetFrameTime()
	if(fInterTime < TimeCnt) then 
		TimeCnt = 0
		local iNextIndex = kSelf:GetUVIndex() + 1
		if(iNextIndex > iUVIndexMax) then  
			iNextIndex = iUVIndexMax
			if TEAM_RED == BearTeam then
				g_Start_Bear_Timer_Red = false
			elseif TEAM_BLUE == BearTeam then
				g_Start_Bear_Timer_Blue = false
			end
		end	
		kSelf:SetUVIndex(iNextIndex)
	end	
	kSelf:SetCustomDataAsFloat(TimeCnt)
end

kWndArray		=	{}
kWndArray[ 1]	=	{["NAME"] = "SFRM_ITEM_PLUS_UPGRADE",		["TTW"] = 1208	}
kWndArray[ 2]	=	{["NAME"] = "SFRM_ITEM_RARITY_UPGRADE", 	["TTW"] = 1482	}
kWndArray[ 3]	=	{["NAME"] = "SFRM_ENCHANT_SHIFT",			["TTW"] = 1363	}
kWndArray[ 4]	=	{["NAME"] = "SFRM_ITEM_RARITY_BUILDUP",		["TTW"] = 1550	}
kWndArray[ 5]	=	{["NAME"] = "SFRM_ITEM_RARITY_AMPLIFY",		["TTW"] = 1551	}
kWndArray[ 6]	=	{["NAME"] = "SFRM_SOULABIL_TRANSFER",		["TTW"] = 799906}
kWndArray[ 7]	=	{["NAME"] = "SFRM_SOCKET_SYSTEM",			["TTW"] = 790110}
kWndArray[ 8]	=	{["NAME"] = "SFRM_EXCHANGE_SOCKETCARD",		["TTW"] = 759904}
kWndArray[ 9]	=	{["NAME"] = "SFRM_COLLECT_ANTIQUE",			["TTW"] = 750008}
kWndArray[10]	=	{["NAME"] = "SFRM_REPAIR_USE_ITEM",			["TTW"] = 1223	}
kWndArray[11]	=	{["NAME"] = "SFRM_MONSTER_CARD",			["TTW"] = 759996}
kWndArray[12]	=	{["NAME"] = "SFRM_CASH_OUT_BUY",			["TTW"] = 5113	}
kWndArray[13]	=	{["NAME"] = "SFRM_EXCHANGE_ELUNIUM",		["TTW"] = 310003}
kWndArray[14]	=	{["NAME"] = "SFRM_EXCHANGE_PURESILVER_KEY",	["TTW"] = 310011}

function CheckAbleSortBtn() -- 인벤토리 정렬/소트 불가
	for kKey, kVal in pairs(kWndArray) do
		local Wnd = GetUIWnd(kVal["NAME"])
		if Wnd:IsNil() == false then
			if Wnd:IsVisible() == true then
				AddWarnDataTT(kVal["TTW"])
				return false
			end
		end
	end
	return true
end

g_TargetIcnAniTime = 0
g_TargetIcnAniTime2 = 0
function InitTargetIcnAniTime()  -- 골동품, 소켓카드 교환 창 애니메이션 용
	g_TargetIcnAniTime = GetAccumTime()
	g_TargetIcnAniTime2 = GetAccumTime()
end

function ClickListTab(kSelf)
	if nil==kSelf or kSelf:IsNil() then return end

	local kMain = kSelf:GetParent():GetParent()
	local kList = kMain:GetParent()
	if kList:IsNil() then return end
	ODS("ClickListTab\n")
	local kItem = kList:ListFirstItem()
	while false == kItem:IsNil() do
		kItem:GetWnd():GetControl("SFRM_ITEM"):GetControl("BTN_SELECT"):GetControl("IMG_SELECT"):Visible(false)
		kItem = kList:ListNextItem(kItem)
	end
	
	kSelf:GetControl("IMG_SELECT"):Visible(true)
end
