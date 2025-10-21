function Net_PT_C_M_REQ_EXCHANGE_ITEM_REQ(kGuid)
	ODS("Net_PT_C_M_REQ_EXCHANGE_ITEM_REQ 시작\n")
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	if g_world:IsHaveWorldAttr(GATTR_FLAG_NOEXCHANGEITEM) then
		AddWarnDataTT(400727)
		return
	end

	-- 안전거품 사용시 교환 안되도록 처리부분
	if g_pilotMan ~= nil then
		local kMyPilot = g_pilotMan:GetPlayerPilot()
		if nil ~= kMyPilot and false == kMyPilot:IsNil() then
			if false == kMyPilot:IsAlive() then	--죽은 상태에서 교환 불가
					AddWarnDataTT(400728)
				return
			end
			local MyUnit = kMyPilot:GetUnit()
			if nil ~= MyUnit and not MyUnit:IsNil() then
				if MyUnit:IsEffect(402001) == true then
					AddWarnDataTT(400842)
					return
				end
			end			
		end
		
		local kTargetPilot = g_pilotMan:FindPilot(kGuid)
		if nil ~= kTargetPilot and false == kTargetPilot:IsNil() then	
			if false == kTargetPilot:IsAlive() then	--죽은 놈이랑 교환 불가
					AddWarnDataTT(400729)
				return
			end
			local MyUnit = kTargetPilot:GetUnit()
			if nil ~= MyUnit and not MyUnit:IsNil() then
				if MyUnit:IsEffect(402001) == true then
					AddWarnDataTT(400841)
					return
				end
			end
		else
			AddWarnDataTT(400668)
			return
		end
	end	


	local wndTrade = GetUIWnd("SFRM_TRADE")
	local wndMsg = GetUIWnd("SFRM_MSG_TRADE")
	local wndMsgR = GetUIWnd("SFRM_MSG_TRADE_REQUEST")
	if wndTrade:IsNil() == false or wndMsg:IsNil() == false or wndMsgR:IsNil() == false then
		AddWarnDataTT(400717)	-- 현재 거래중
		return
	end

	if GetTradeGuid():IsNil() == false then	--거래 GUID가 있으면
		AddWarnDataTT(400717)	-- 현재 거래중
		return
	end
	
	local wndPlus = GetUIWnd("SFRM_ITEM_PLUS_UPGRADE")
	if false == wndPlus:IsNil() then
	    AddWarnDataTT(1208)	-- 현재 인챈트 중
		return
    end
    
    local wndPlus = GetUIWnd("SFRM_ITEM_RARITY_UPGRADE")
	if false == wndPlus:IsNil() then
	    AddWarnDataTT(1408)	-- 현재 세공
		return
    end
    
    local wndCook = GetUIWnd("SFRM_ITEM_MAKING_COOK")
	if false == wndCook:IsNil() then
	    AddWarnDataTT(1612)	-- 현재 요리중
		return
    end

	local wndPos = GetUIWnd("SFRM_POST")
	if false == wndPos:IsNil() then
	    AddWarnDataTT(957)	-- 현재 우편함 열려있음
		return
    end
	
	
	if kGuid:IsNil() == false then
		local packet = NewPacket(13210)
		packet:PushGuid(kGuid)
		Net_Send(packet)
		DeletePacket(packet)
	end
end

function Net_PT_M_C_NFY_EXCHANGE_ITEM_REQ(kPacket)
	ODS("Net_PT_M_C_NFY_EXCHANGE_ITEM_REQ 시작\n")
	-- 트레이드 GUID가 있으면 여기서 튕기자. 지금 거래중이라는 뜻이다. 
	if kPacket:IsNil() == false then
		local kGuid = kPacket:PopGuid()	-- 상대 GUID
		local iType = kPacket:PopInt()	-- 여러가지 결과 중
		local kTradeGuid = kPacket:PopGuid()	-- 거래 GUID

		if 0 ~= Config_GetValue("ETC", "DENY_TRADE") then
			NewTradeStart(kTradeGuid, kGuid)
			Net_PT_C_M_NFY_EXCHANGE_ITEM_ANS(false)			
			AddWarnDataTT(400726)
			ClearTradeInfo()			
			return
		end
	
		ODS("iType = " .. iType .. "\n")
		if kGuid:IsNil() == false then
			if g_pilotMan ~= nil then
				local kTargetPilot = g_pilotMan:FindPilot(kGuid)
				if kTargetPilot:IsNil() == false then
					local kID = kTargetPilot:GetName()
					if iType == 1 then -- TR_REQUEST
						local kMsg = CallUI("SFRM_MSG_TRADE")
						if kMsg:IsNil() == false then
							kMsg:GetControl("SFRM_COLOR"):GetControl("SFR_SDW"):SetStaticText(kID:GetStr() .. GetTextW(400711):GetStr())

							ODS("ID = " .. kID:GetStr() .. " kTradeGuid : " .. kTradeGuid:GetString() .. "\n")
							NewTradeStart(kTradeGuid, kGuid)
							
							if IsExistUniqueType("NPC_MENU") == true then
								-- NPC 메뉴가 열려있는 상태
								Net_PT_C_M_NFY_EXCHANGE_ITEM_ANS(false)
								ClearTradeInfo()
							end
							
							local wndPlus = GetUIWnd("SFRM_ITEM_PLUS_UPGRADE")
							if false == wndPlus:IsNil() then
							   	-- 현재 인챈트 중
								Net_PT_C_M_NFY_EXCHANGE_ITEM_ANS(false)
								ClearTradeInfo()
						    end
    
						    local wndCook = GetUIWnd("SFRM_ITEM_MAKING_COOK")
							if false == wndCook:IsNil() then
							   	-- 현재 요리 중
								Net_PT_C_M_NFY_EXCHANGE_ITEM_ANS(false)
								ClearTradeInfo()
						    end
						    
						    local wndRare = GetUIWnd("SFRM_ITEM_RARITY_UPGRADE")
							if false == wndRare:IsNil() then
							    Net_PT_C_M_NFY_EXCHANGE_ITEM_ANS(false)								
							    ClearTradeInfo()	-- 현재 세공
						    end
							
							--local wndCostume = GetUIWnd("SFRM_COSTUME_MIXER")
							--if false == wndCostume:IsNil() then								
							 --   Net_PT_C_M_NFY_EXCHANGE_ITEM_ANS(false)								
							 --   ClearTradeInfo()	-- 현재 코스튬 조합중
						    --end
							
							 local wndRare = GetUIWnd("SFRM_GUILD_INVENTORY")
							if false == wndRare:IsNil() then
							    Net_PT_C_M_NFY_EXCHANGE_ITEM_ANS(false)								
							    ClearTradeInfo()	-- 길드금고 열려 있는 중
						    end

						    local wndPost = GetUIWnd("SFRM_POST")
							if false == wndPost:IsNil() then
							    Net_PT_C_M_NFY_EXCHANGE_ITEM_ANS(false)
							    ClearTradeInfo()	-- 현재 우편함 열려있음
						    end
						end
					elseif iType == 4 then --TR_SUCCESS
						NewTradeStart(kTradeGuid, kGuid)
						local kPilot = g_pilotMan:FindPilot(kGuid)
						if kPilot:IsNil() == false then
							local kName = kPilot:GetName()
							local kMsg = WideString(string.format(GetTextW(400722):GetStr(), kName:GetStr()))
							g_ChatMgrClient:AddLogMessage(kMsg)
							local wndReq = ActivateUI("SFRM_MSG_TRADE_REQUEST")
							if wndReq:IsNil() == false then
								local wndColor = wndReq:GetControl("SFRM_COLOR")
								if wndColor:IsNil() == false then
									local wndSdw = wndColor:GetControl("SFRM_SDW")
									if wndSdw:IsNil() == false then
										wndSdw:SetStaticTextW(kMsg)
									end
								end
							end
						end
					else
						AddWarnDataTT(400710+iType)
						--[[if iType == 2 then	-- TR_ACCEPT
					elseif iType == 3 then	-- TR_REJECT
					elseif iType == 4 then	-- TR_SUCCESS
					elseif iType == 5 then	-- TR_NO_INVEN
					elseif iType == 6 then	-- TR_FAIL
					elseif iType == 7 then	-- TR_ON_TRADEING
					elseif iType == 8 then]]--	-- TR_TRADED_IN_3SEC
	
					end
				end
			end
		end
	end
end

function Net_PT_C_M_NFY_EXCHANGE_ITEM_ANS(bOK)
	ODS("Net_PT_C_M_NFY_EXCHANGE_ITEM_ANS 시작\n")
	local kTradeGuid = GetTradeGuid()
	if kTradeGuid:IsNil() == true then
		ODS("kTradeGuid:IsNil() == true \n")
	end
	if bOK == true then
		ODS("Trade Accept\n")
	else
		ODS("Trade Reject\n")
	end

	ODS(" Guid : " .. kTradeGuid:GetString() ..  "\n")
	local packet = NewPacket(13212)
	packet:PushGuid(kTradeGuid)
	packet:PushBool(bOK)
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_PT_M_C_NFY_EXCHANGE_ITEM(kPacket)
	ODS("Net_PT_M_C_NFY_EXCHANGE_ITEM 시작\n")
	if kPacket:IsNil() == true then
		return
	end
	CloseUI("SFRM_MSG_TRADE")
	CloseUI("SFRM_MSG_TRADE_REQUEST")
	
	local kTradeGuid = kPacket:PopGuid()
	local Guid = kPacket:PopGuid()
	local iResult = kPacket:PopInt() --뜰까 말까 혹
	if 6 == iResult or 3 == iResult then	-- TR_FAIL TR_REJECT
		ClearTradeInfo()				-- 거래가 실패했으므로 초기화
		AddWarnDataTT(400710+iResult)
		CloseUI("SFRM_TRADE")
		ODS("거래실패\n")
	elseif 2 == iResult then
		local wndTrade = ActivateUI("SFRM_TRADE")
		if wndTrade:IsNil() == true then
			return
		end

		local wndC1 = wndTrade:GetControl("SFRM_COLOR1")
		if wndC1:IsNil() == false then
			local kMyPilot = GetMyPilot()
			if kMyPilot:IsNil() == false then
				local kName = kMyPilot:GetName():GetStr()
				wndC1:SetStaticText(kName)
			end
		end

		local wndC2 = wndTrade:GetControl("SFRM_COLOR2")
		if wndC2:IsNil() == false then
			local kOtherPilot = g_pilotMan:FindPilot(Guid)
			if kOtherPilot:IsNil() == false then
				local kName = kOtherPilot:GetName():GetStr()
				wndC2:SetStaticText(kName)
			end
		end
	end
end

function Net_PT_C_M_NFY_EXCHANGE_ITEM_QUIT()
	local packet = NewPacket(13219)
	packet:PushGuid(GetTradeGuid())
	Net_Send(packet)
	DeletePacket(packet)
end
 
function Net_PT_C_M_NFY_EXCHANGE_ITEM_READY(wnd)
	if wnd:IsNil() == true then
		return
	end
	
	--고정 버튼이 눌러져서 고정이어야만 동작하자
	local wndReady = wnd:GetParent():GetControl("CBTN_READY")
	if wndReady:IsNil() == true then
		return
	end

	if wndReady:GetCheckState() == false then	-- 확인 상태가 아니면 취소 
		Net_PT_C_M_NFY_EXCHANGE_ITEM_QUIT()
		return
	end
	wnd:LockClick(true)
	wnd:SetClose(true)

	local packet = NewPacket(13216)
	packet:PushGuid(GetTradeGuid())
	Net_Send(packet)
	DeletePacket(packet)
end

function Close_SFRM_MSG_TRADE(szName)
	CloseUI(szName)
	Net_PT_C_M_NFY_EXCHANGE_ITEM_ANS(false)
--	ClearTradeInfo()
end
