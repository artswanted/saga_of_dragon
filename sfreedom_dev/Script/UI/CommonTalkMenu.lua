g_kNpcCommandMenu = {}
--[[
g_kNpcCommandMenu[102] = {--Common
	OnAdd = function(kTopWnd)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		AddTalkMenuItem(kBtnList, 400542, -1, false)
		ResizeTalkMenuWnd(kMenuWnd, kBtnList)
	end;
}]]--
g_kNpcCommandMenu[103] = {--Guild Command
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		if not HaveGuild() then
			AddTalkMenuItem(kBtnList, 400543, 1, kGuid, false) --Create
			AddTalkMenuItem(kBtnList, 516664, 11200, kGuid, false) --Help
		else
			if AmIGuildOwner() then
				AddTalkMenuItem(kBtnList, 516664, 11200, kGuid, false) --Help
				--AddGuildMasterCommand(kBtnList, 400544, 12)--Member Grade
				--AddGuildMasterCommand(kBtnList, 400545, 8)--Grade name, etc
				--AddTalkMenuItem(kBtnList, 71005, 16, kGuid, false)--SiegeWar
				AddTalkMenuItem(kBtnList, 400803, 20, kGuid, false)--Guild Lv
				AddTalkMenuItem(kBtnList, 400807, 22, kGuid, false)--Guild Skill
				if HaveGuildInventory() then
					AddTalkMenuItem(kBtnList, 401088, 23, kGuid, false)--Guild Inventory Supervision
				else
					AddTalkMenuItem(kBtnList, 401089, 24, kGuid, false)--Guild Inventory Create Y/N ?
				end				
				AddTalkMenuItem(kBtnList, 400546, 14, kGuid, false)--Destroy
			end
		end
		AddTalkMenuItem(kBtnList, 400542, -1, kGuid, false)--Close
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}
g_kNpcCommandMenu[104] = {--Destroy Y / N
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		AddTalkMenuItem(kBtnList, 400556, 15, kGuid, false)--Destroy Y
		AddTalkMenuItem(kBtnList, 400557, 0, kGuid, false)--Back Menu
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}
g_kNpcCommandMenu[105] = {--PreCreateGuild
	OnAdd = function(kTopWnd, kGuid)
		local kEditWnd = kTopWnd:GetControl("FRM_EDIT")
		if kEditWnd:IsNil() then return end
		
		kEditWnd:Visible(true)
	end;
}
g_kNpcCommandMenu[106] = {--Create Y / N
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		AddTalkMenuItem(kBtnList, 400556, 12, kGuid, false)--Create Y
		AddTalkMenuItem(kBtnList, 400557, 0, kGuid, false)--Back Menu
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}
g_kNpcCommandMenu[107] = {--LvUp Y / N
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		AddTalkMenuItem(kBtnList, 400556, 21, kGuid, false)--LvUp Y
		AddTalkMenuItem(kBtnList, 400557, 0, kGuid, false)--Back Menu
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}
g_kNpcCommandMenu[108] = {--Buy Guild Skill
	OnAdd = function(kTopWnd, kGuid)
		local kSkillBuyWnd = kTopWnd:GetControl("SFRM_GUILD_BUY_SKILL")
		if kSkillBuyWnd:IsNil() then return end
		kSkillBuyWnd:Visible(true)
		UpdateGuildSkillBuyUI(kSkillBuyWnd)
	end;
}

g_kNpcCommandMenu[109] = {
	OnAdd = function(kTopWnd, kGuid)
	
	end;
}

g_kNpcCommandMenu[110] = {--Select Guild Mark
	OnAdd = function(kTopWnd, kGuid)
		SetTempGuildEmblem(-1)
		local kGuildMarkWnd = kTopWnd:GetControl("FRM_GUILD_EMBLEM_SELECT")
		if kGuildMarkWnd:IsNil() then return end
		kGuildMarkWnd:Visible(true)
		kGuildMarkWnd:SetOwnerGuid(kGuid)
		
		local kSelectedWnd = kGuildMarkWnd:GetControl("IMG_OVER1")
		if not kSelectedWnd:IsNil() then
			kSelectedWnd:Visible(false)
		end
		
	end;
}

g_kNpcCommandMenu[114] = {--Guild Inventory Create Y / N?
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		AddTalkMenuItem(kBtnList, 400556, 25, kGuid, false)--Create Yes
		AddTalkMenuItem(kBtnList, 400557, 0, kGuid, false)--Back Menu
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}

g_kNpcCommandMenu[1000] = {--TW Event Quest
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		AddTalkMenuItem(kBtnList, 450220, 1001, kGuid, false)--Next
		AddTalkMenuItem(kBtnList, 450221, 1000, kGuid, false)--Next
		AddTalkMenuItem(kBtnList, 450222, 0, kGuid, false)--Close
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}
--[[
g_kNpcCommandMenu[1001] = {--TW Event Quest
	OnAdd = function(kTopWnd, kGuid)
		--local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		--AddTalkMenuItem(kBtnList, 915, 0, kGuid, false)--Close
		--CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}
g_kNpcCommandMenu[1002] = {--TW Event Quest
	OnAdd = function(kTopWnd, kGuid)
		--local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		--AddTalkMenuItem(kBtnList, 915, 0, kGuid, false)--Close
		--CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}]]

g_kNpcCommandMenu[2000] = {--CP
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		AddTalkMenuItem(kBtnList, 10, 2001, kGuid, false)--CP Help
		AddTalkMenuItem(kBtnList, 10, 2002, kGuid, false)--CP
		AddTalkMenuItem(kBtnList, 400542, 0, kGuid, false)--Close
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}

g_kNpcCommandMenu[2001] = {--CP
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		AddTalkMenuItem(kBtnList, 10, 2000, kGuid, false)--Back
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}
--///////////////////////////////////////////////////////////////////////
g_kNpcCommandMenu[201] = {--엠포리아 기본
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		AddTalkMenuItem(kBtnList, 509360, 30, kGuid, false)--Back
		if  CheckEmporiaGuildOwner() then
			AddTalkMenuItem(kBtnList, 509361, 31, kGuid, false)--Back
			AddTalkMenuItem(kBtnList, 509362, 32, kGuid, false)--Back
			AddTalkMenuItem(kBtnList, 509363, 33, kGuid, false)--Back
		end
		--AddTalkMenuItem(kBtnList, 509364, 34, kGuid, false)--Back
		AddTalkMenuItem(kBtnList, 500003, 35, kGuid, false)--Back
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}

g_kNpcCommandMenu[202] = {--어디?
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		AddTalkMenuItem(kBtnList, 500001, 52, kGuid, false)--Back
		AddTalkMenuItem(kBtnList, 500003, 35, kGuid, false)--Back
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}

g_kNpcCommandMenu[203] = {--생성
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		for idx = 1, 4 do
			if not EmporiaAddonBuildingCreated(idx) then
				local bState = not EmporiaAddonBuildingUseable(idx)
				AddTalkMenuItem(kBtnList, 509364 + idx, 35 + idx, kGuid, bState)--Back
			end
		end
		AddTalkMenuItem(kBtnList, 500001, 52, kGuid, false)--Back
		AddTalkMenuItem(kBtnList, 500003, 35, kGuid, false)--Back
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}

g_kNpcCommandMenu[204] = {--관리
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		--AddTalkMenuItem(kBtnList, 509370, 55, kGuid)--Back
		AddTalkMenuItem(kBtnList, 509371, 51, kGuid, false)--Back
		AddTalkMenuItem(kBtnList, 500001, 52, kGuid, false)--Back
		AddTalkMenuItem(kBtnList, 500003, 35, kGuid, false)--Back
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}

g_kNpcCommandMenu[205] = {--삭제
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		for idx = 1, 4 do
			if EmporiaAddonBuildingCreated(idx) then
				AddTalkMenuItem(kBtnList, 509364 + idx, 40 + idx, kGuid, false)--Back
			end
		end
		AddTalkMenuItem(kBtnList, 500001, 32, kGuid, false)--Back
		AddTalkMenuItem(kBtnList, 500003, 35, kGuid, false)--Back
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}

g_kNpcCommandMenu[206] = {--유지
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		for idx = 1, 4 do
			if EmporiaAddonBuildingCreated(idx) then
				AddTalkMenuItem(kBtnList, 509364 + idx, 45 + idx, kGuid, false)--Back
			end
		end
		AddTalkMenuItem(kBtnList, 500001, 32, kGuid, false)--Back
		AddTalkMenuItem(kBtnList, 500003, 35, kGuid, false)--Back
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}

g_kNpcCommandMenu[207] = {--설정
	OnAdd = function(kTopWnd, kGuid)
		local kMenuWnd, kBtnList = CallMenuWnd(kTopWnd)
		local bState = EmporiaEntryOpenState()
		AddTalkMenuItem(kBtnList, 509372, 53, kGuid, bState)--Back
		AddTalkMenuItem(kBtnList, 509373, 54, kGuid, not bState)--Back
		AddTalkMenuItem(kBtnList, 500001, 52, kGuid, false)--Back
		AddTalkMenuItem(kBtnList, 500003, 35, kGuid, false)--Back
		CommonMenuSetter(kMenuWnd, kBtnList)
	end;
}
--///////////////////////////////////////////////////////////////////////

--///////////////////////////////////////////////////////////////////////
function CommonMenuSetter(kMenuWnd, kBtnList)
	ResizeTalkMenuWnd(kMenuWnd, kBtnList)
	OnDefault_CommonTalkMenuCursor(kMenuWnd)
end

function CallMenuWnd(kTopWnd)
	local kMenuWnd = kTopWnd:GetControl("FRM_GUILD_MENU")
	if kMenuWnd:IsNil() then return end
	
	local kBtnList = kMenuWnd:GetControl("LIST_COMMAND")
	if kBtnList:IsNil() then return end
	kBtnList:ClearAllListItem()
	return kMenuWnd, kBtnList
end
function AddTalkMenuItem(kList, iTTW, iCommand, kGuid, bDisable)
	if nil == kList then return end
	if kList:IsNil() then return end
	
	local kText = GetTT(iTTW)
	local emporia_idx = iTTW - 509364
	if 0 < emporia_idx and emporia_idx <= 5 then	--엠포리아 구조물인 경우 태그 붙이기
		local kGrade = GetTT(71999)
		kGrade:ReplaceStr("#GRADE#", GetEmporiaBuildingGradeStr(emporia_idx) )
		kText = WideString( kGrade:GetStr() .. GetTT(iTTW):GetStr() )
	end
	
	local kItem = kList:AddNewListItem(kText)
	if kItem:IsNil() then return end
	local kWnd = kItem:GetWnd()
	if kWnd:IsNil() then return end
	
	kWnd:Disable(bDisable)
	kWnd:SetCustomDataAsInt(iCommand)--Set Command
	if kGuid ~= nil then
		kWnd:SetOwnerGuid(kGuid)
	end
end
function ResizeTalkMenuWnd(kMenuWnd, kBtnList)
	kMenuWnd:Visible(true)
	local kListSize = kBtnList:GetSize()
	kListSize:SetY( kBtnList:GetListItemCount() * 30 )
	kBtnList:SetSize(kListSize)
	
	local kTopFrmSize = kMenuWnd:GetSize()
	kTopFrmSize:SetY( kListSize:GetY() + 23 )
	kMenuWnd:SetSize(kTopFrmSize)
	SCREEN_SIZE_SetPosCenterY( kMenuWnd, -60 )
end
function OffAllSubMenu(kTopWnd)
	local kMenuWnd = kTopWnd:GetControl("FRM_GUILD_MENU")
	if not kMenuWnd:IsNil() then kMenuWnd:Visible(false) end
	local kQuestMenuWnd = kTopWnd:GetControl("FRM_QUEST_MENU")
	if not kQuestMenuWnd:IsNil() then kQuestMenuWnd:Visible(false) end
	local kEditWnd = kTopWnd:GetControl("FRM_EDIT")
	if not kEditWnd:IsNil() then kEditWnd:Visible(false) end
	local kSkillBuyWnd = kTopWnd:GetControl("SFRM_GUILD_BUY_SKILL")
	if not kSkillBuyWnd:IsNil() then kSkillBuyWnd:Visible(false) end
	local kEmblemWnd = kTopWnd:GetControl("FRM_GUILD_EMBLEM_SELECT")
	if not kEmblemWnd:IsNil() then kEmblemWnd:Visible(false) end
end

--///////////////////////////////////////////////////////////////////////
function ShowNpcTalkMenu(kTopWnd, iMenuSet, kGuid)
	if kTopWnd == nil or kTopWnd:IsNil() then return end
	
	OffAllSubMenu(kTopWnd)
	
	if nil == g_kNpcCommandMenu[iMenuSet] then
		return false
	end
	
	g_kNpcCommandMenu[iMenuSet].OnAdd(kTopWnd, kGuid)
	return true
end

--///////////////////////////////////////////////////////////////////////
function OnCommonTalkMenuSpecFunction(iCommand, kGuid)
	CloseUI("FRM_QUEST_FULLSCREEN") --Close
	
	-- Spec Function
	if iCommand == -1 then
		
	elseif iCommand == -2 then
		FranYesNoMsgBox(kGuid)
	elseif iCommand == -3 then
		CallRarityUpgradeSelectUI(kGuid)
	elseif iCommand == -4 then
		SendReqGemStore(kGuid)
	elseif iCommand == -5 then
		CallEventViewUI()
		--RegistUIAction(ActivateUI("SFRM_EVENT_COUPON"), "CloseUI")
	elseif iCommand == -6 then
		SendEventNameCoupon()
	elseif iCommand == -7 then
		RegistUIAction(ActivateUI("FRM_TRADE_UNSEALINGSCROLL"), "CloseUI")
	elseif iCommand == -8 then
		UpdateSoulStoneTrade()
		RegistUIAction(GetUIWnd("FRM_TRADE_SOULSTONE"), "CloseUI")
	elseif iCommand == -9 then
		Net_ReqEnterMission_Npc()
	elseif iCommand == -10 then
		NET_C_M_REQ_TRIGGER(1, kGuid, TRIGGER_ACTION_CLICKED)
	elseif iCommand == -11 then
		NPC_Shop(kGuid)
	elseif iCommand == -12 then
		Net_ReqEnterMission_Npc()
	elseif iCommand == -13 then
		CallUI("FRM_ACCEPT_RANDOM_QUEST")
	elseif iCommand == -14 then
		CallUI("FRM_ACCEPT_TACTICS_RANDOM_QUEST")
	elseif IsMapNo(-iCommand) then
		SendReqNpcTalkMapNove(kGuid, -iCommand)
	else
	end
end

--///////////////////////////////////////////////////////////////////////
function OnCommonTalkMenuCommand(iCommand, kGuid)
	ODS("OnGuildMasterCommand : " .. iCommand .. "\n")
	--parse master command
	
	if iCommand < 0 then
		OnCommonTalkMenuSpecFunction(iCommand, kGuid)
		return
	end
	
	if iCommand == 0 then -- Guild NPC Menu
		CloseUI("FRM_QUEST_FULLSCREEN") --Close
		--CallNpcFunctionUI(103, kGuid, "Elvis_normal", 400826)
	--Create--------------------------
	elseif iCommand == 1 then --Create
		if not HaveGuild() then
			Send_CreateGuildTest()
		else
			CallNpcFunctionUI(102, kGuid, "Elvis_normal", 400547)
		end
	elseif iCommand == 12 then
		Send_CreateGuild()
	elseif iCommand == 8 then
	--Destroy-------------------------
	elseif iCommand == 14 then--Destroy Y/N
		CallNpcFunctionUI(104, kGuid, "Elvis_normal", 400555)
	elseif iCommand == 15 then--Destroy Y
		Send_DestroyGuild()
	--LvUp----------------------------
	elseif iCommand == 20 then--LvUp Y/N
		local iIntroText = GetGuildLevelIntroText(GetGuildLevel()+1)
		if 0 == iIntroText then
			CallNpcFunctionUI(102, kGuid, "Elvis_normal", 400819)
		else
			CallNpcFunctionUI(107, kGuid, "Elvis_normal", iIntroText)
		end
		
	elseif iCommand == 21 then--LvUp Y
		Send_ReqGuildLvUp()
	--Skill---------------------------
	elseif iCommand == 22 then--Skill UI
		CallNpcFunctionUI(108, kGuid, "Elvis_normal", 400825)
		
	--Guild Inventory
	elseif iCommand == 23 then -- Guild Inventory Supervision
		CloseUI("FRM_QUEST_FULLSCREEN") --Close		
		CallGuildInvSuperVision()
	elseif iCommand == 24 then -- Guild Inventory Create Y/N?
		CallNpcFunctionUI(114, kGuid, "Elvis_normal", 401079)--QDT_Guild_Inventory_Create
	elseif iCommand == 25 then -- Guild Inventory Create Yes
		Send_ReqGuildInventoryCreate()
	--Emporia
	elseif iCommand == 30 then
		CallNpcFunctionUI(202, kGuid, "c_William", 60519)		
	elseif iCommand == 31 then
		CallNpcFunctionUI(203, kGuid, "c_William", 509375)		
	elseif iCommand == 32 then
		CallNpcFunctionUI(204, kGuid, "c_William", 509376)		
	elseif iCommand == 33 then
		CallNpcFunctionUI(207, kGuid, "c_William", 509379)		
	elseif iCommand == 34 then
		BuyEmporiaTeleCard(iCommand, kGuid)
	elseif iCommand == 35 then
		CallTextDialog(10501, kGuid)
	elseif iCommand <= 36 then
		EmporiaAddonBuildingConstruct(1, kGuid)
	elseif iCommand <= 37 then
		EmporiaAddonBuildingConstruct(2, kGuid)
	elseif iCommand <= 38 then
		EmporiaAddonBuildingConstruct(3, kGuid)
	elseif iCommand <= 39 then
		EmporiaAddonBuildingConstruct(4, kGuid)
	elseif iCommand <= 40 then
		EmporiaAddonBuildingConstruct(5, kGuid)
	elseif iCommand <= 41 then
		EmporiaAddonBuildingDestruct(1, kGuid)
	elseif iCommand <= 42 then
		EmporiaAddonBuildingDestruct(2, kGuid)
	elseif iCommand <= 43 then
		EmporiaAddonBuildingDestruct(3, kGuid)
	elseif iCommand <= 44 then
		EmporiaAddonBuildingDestruct(4, kGuid)
	elseif iCommand <= 45 then
		EmporiaAddonBuildingDestruct(5, kGuid)
	elseif iCommand <= 46 then
		EmporiaAddonBuildingMaintenancePayment(1, kGuid)
	elseif iCommand <= 47 then
		EmporiaAddonBuildingMaintenancePayment(2, kGuid)
	elseif iCommand <= 48 then
		EmporiaAddonBuildingMaintenancePayment(3, kGuid)
	elseif iCommand <= 49 then
		EmporiaAddonBuildingMaintenancePayment(4, kGuid)
	elseif iCommand <= 50 then
		EmporiaAddonBuildingMaintenancePayment(5, kGuid)
	elseif iCommand == 51 then
		CallNpcFunctionUI(206, kGuid, "c_William", 509378)
	elseif iCommand == 52 then
		CallNpcFunctionUI(201, kGuid, "c_William", 509374)	
	elseif iCommand == 53 then
		EmporiaEntryOpenToOtherUser(2, kGuid)
	elseif iCommand == 54 then
		EmporiaEntryOpenToOtherUser(1, kGuid)
	elseif iCommand == 55 then
		CallNpcFunctionUI(205, kGuid, "c_William", 509377)
	elseif iCommand == 1000 then
		CallEventQuestNpcTalk(kGuid, false)
	elseif iCommand == 1001 then
		CheckEventQuestEnd(kGuid)
	else
		CallTextDialog(iCommand, kGuid)
	end
	
	-- elseif iCommand == 30 then
		-- CallNpcFunctionUI(201, kGuid, "c_William", 72007)
	
	-- elseif iCommand == 31 then
		-- CallNpcFunctionUI(202, kGuid, "c_William", 72008)
		
	-- elseif iCommand == 32 then
		-- CallNpcFunctionUI(200, kGuid, "c_William", 1436)
		
	-- elseif iCommand == 33 then	--기부함 설치
		-- local kText = GetTextW(72012)
		-- kText:Add("%%")
		-- kText:Add(GetTextW(72009):GetStr())
		-- kText:Add("\n")
		-- kText:Add(GetTextW(72004):GetStr())--주당 관리비 :
		-- kText:Add(GetEmporiaFuncPrice(EMPORIA_CONTRIBUTE))
		-- kText:Add(" ")
		-- kText:Add(GetTextW(72013):GetStr())-- 길드 경험치
		-- CallNpcFunctionUI2(203, kGuid, "c_William", kText)
	-- elseif iCommand == 34 then	--기부함  유지고고고
		-- KeepEmporiaFunc(EMPORIA_CONTRIBUTE)
	-- elseif iCommand == 35 then	--상점 열기
		-- NPC_Shop(kGuid)
		-- CloseUI("FRM_QUEST_FULLSCREEN") --Close
	-- else
		-- --CallNpcFunctionUI(iCommand, kGuid, "Elvis_normal", 400541)
		-- CallTextDialog(iCommand, kGuid)
	-- end
	
end

function OnDefault_CommonTalkMenuCursor(kUI)
	local kCursor = kUI:GetControl("FRM_CURSOR")
	if kCursor:IsNil() then
		return
	end
	local kList = kUI:GetControl("LIST_COMMAND")
	if kList:IsNil() then
		return
	end
	local iCount = kList:GetListItemCount()
	if 0 < iCount then
		local kItem = kList:ListFirstItem()
		while not kItem:IsNil() do
			if true == kItem:GetWnd():IsDisable() then
				kItem = kList:ListNextItem(kItem)
			else
				kCursor:SetOwnerGuid(kItem:GetWnd():GetOwnerGuid())
				kCursor:SetCustomDataAsInt(kItem:GetWnd():GetCustomDataAsInt())
				local kPos = kList:GetLocation()
				kPos:SetX(kPos:GetX() + kItem:GetWnd():GetLocation():GetX() - 6)
				kPos:SetY(kPos:GetY() + kItem:GetWnd():GetLocation():GetY() - 4)
				kCursor:SetLocation(kPos)
				break
			end
		end
	end
end
function OnUp_CommonTalkMenuCursor(kUI)
	local kParent = kUI:GetParent()
	if kParent:IsNil() then
		return
	end
	local kList = kParent:GetControl("LIST_COMMAND")
	if kList:IsNil() then
		return
	end
	local iCount = kList:GetListItemCount()
	local kGuid = kUI:GetOwnerGuid()
	local kKey = kUI:GetCustomDataAsInt()
	local kSelectPos = kUI:GetLocation()
	if 0 < iCount then
		local kItem = kList:ListFirstItem()
		if kItem:IsNil() then
			return
		end
		if kGuid:IsEqual(kItem:GetWnd():GetOwnerGuid()) and kKey == kItem:GetWnd():GetCustomDataAsInt() then
			return
		end
		while not kItem:IsNil() do
			if true == kItem:GetWnd():IsDisable() then
				kItem = kList:ListNextItem(kItem)
			else
				local kItemGuid = kItem:GetWnd():GetOwnerGuid()
				local kItemKey = kItem:GetWnd():GetCustomDataAsInt()
				if kGuid:IsEqual(kItemGuid) and kKey == kItemKey then
					kUI:SetLocation(kSelectPos)
					return
				end	
				kUI:SetOwnerGuid(kItemGuid)
				kUI:SetCustomDataAsInt(kItemKey)
				kSelectPos:SetX(kList:GetLocation():GetX() + kItem:GetWnd():GetLocation():GetX() - 6)
				kSelectPos:SetY(kList:GetLocation():GetY() + kItem:GetWnd():GetLocation():GetY() - 4)
				kItem = kList:ListNextItem(kItem)
			end
		end
	end
end

function OnDown_CommonTalkMenuCursor(kUI)
	local kParent = kUI:GetParent()
	if kParent:IsNil() then
		return
	end
	local kList = kParent:GetControl("LIST_COMMAND")
	if kList:IsNil() then
		return
	end
	local iCount = kList:GetListItemCount()
	local kGuid = kUI:GetOwnerGuid()
	local kKey = kUI:GetCustomDataAsInt()
	local kSelectItem = false
	if 0 < iCount then
		local kItem = kList:ListFirstItem()
		while not kItem:IsNil() do
			if true==kItem:GetWnd():IsDisable() then
				kItem = kList:ListNextItem(kItem)
			else
				local kItemGuid = kItem:GetWnd():GetOwnerGuid()
				local kItemKey = kItem:GetWnd():GetCustomDataAsInt()
				if kSelectItem then
					kUI:SetOwnerGuid(kItemGuid)
					kUI:SetCustomDataAsInt(kItemKey)
					local kPos = kList:GetLocation()
					kPos:SetX(kPos:GetX() + kItem:GetWnd():GetLocation():GetX() - 6)
					kPos:SetY(kPos:GetY() + kItem:GetWnd():GetLocation():GetY() - 4)
					kUI:SetLocation(kPos)
					return
				else
					if kGuid:IsEqual(kItemGuid) and kKey == kItemKey then
						kSelectItem = true
					end			
				end
				kItem = kList:ListNextItem(kItem)
			end
		end
	end
end

function OnOverCommonTalkItem(kUI)
	local kList = kUI:GetParent()
	local kParent = kList:GetParent()
	local kOver = kParent:GetControl("FRM_CURSOR")
	if kOver:IsNil() then
		return
	end
	kOver:SetOwnerGuid(kUI:GetOwnerGuid())
	kOver:SetCustomDataAsInt(kUI:GetCustomDataAsInt())
	local kPos = kList:GetLocation()
	kPos:SetX(kPos:GetX() + kUI:GetLocation():GetX() - 6)
	kPos:SetY(kPos:GetY() + kUI:GetLocation():GetY() - 4)
	kOver:SetLocation(kPos)
end

function OnDefault_CommonQTalkMenuCursor(kUI)
	local kCursor = kUI:GetControl("FRM_CURSOR")
	if kCursor:IsNil() then
		return
	end
	local kList = kUI:GetControl("LIST_COMMAND")
	if kList:IsNil() then
		return
	end
	local iCount = kList:GetListItemCount()
	if 0 < iCount then
		local kItem = kList:ListFirstItem()
		if kItem:IsNil() then
			return
		end
		kCursor:SetCustomDataAsInt(kItem:GetWnd():GetCustomDataAsInt())
		local kPos = kList:GetLocation()
		kPos:SetX(kPos:GetX() + kItem:GetWnd():GetLocation():GetX() - 6)
		kPos:SetY(kPos:GetY() + kItem:GetWnd():GetLocation():GetY() - 4)
		kCursor:SetLocation(kPos)
	end
end

function OnUp_CommonQTalkMenuCursor(kUI)
	local kParent = kUI:GetParent()
	if kParent:IsNil() then
		return
	end
	local kList = kParent:GetControl("LIST_COMMAND")
	if kList:IsNil() then
		return
	end
	local iCount = kList:GetListItemCount()
	local kKey = kUI:GetCustomDataAsInt()
	local kSelectPos = kList:GetLocation()
	if 0 < iCount then
		local kItem = kList:ListFirstItem()
		if kItem:IsNil() then
			return
		end
		if kKey == kItem:GetWnd():GetCustomDataAsInt() then
			return
		end
		while not kItem:IsNil() do
			local kItemKey = kItem:GetWnd():GetCustomDataAsInt()
			if kKey == kItemKey then
				kUI:SetLocation(kSelectPos)
				return
			end	
			kUI:SetCustomDataAsInt(kItemKey)
			kSelectPos:SetX(kList:GetLocation():GetX() + kItem:GetWnd():GetLocation():GetX() - 6)
			kSelectPos:SetY(kList:GetLocation():GetY() + kItem:GetWnd():GetLocation():GetY() - 4)
			kItem = kList:ListNextItem(kItem)
		end
	end
end

function OnDown_CommonQTalkMenuCursor(kUI)
	local kParent = kUI:GetParent()
	if kParent:IsNil() then
		return
	end
	local kList = kParent:GetControl("LIST_COMMAND")
	if kList:IsNil() then
		return
	end
	local iCount = kList:GetListItemCount()
	local kKey = kUI:GetCustomDataAsInt()
	local kSelectItem = false
	if 0 < iCount then
		local kItem = kList:ListFirstItem()
		while not kItem:IsNil() do
			local kItemKey = kItem:GetWnd():GetCustomDataAsInt()
			if kSelectItem then
				kUI:SetCustomDataAsInt(kItemKey)
				local kPos = kList:GetLocation()
				kPos:SetX(kPos:GetX() + kItem:GetWnd():GetLocation():GetX() - 6)
				kPos:SetY(kPos:GetY() + kItem:GetWnd():GetLocation():GetY() - 4)
				kUI:SetLocation(kPos)
				return
			else
				if kKey == kItemKey then
					kSelectItem = true
				end			
			end
			kItem = kList:ListNextItem(kItem)
		end
	end
end

function OnOverCommonQTalkItem(kUI)
	local kList = kUI:GetParent()
	local kParent = kList:GetParent()
	local kOver = kParent:GetControl("FRM_CURSOR")
	if kOver:IsNil() then
		return
	end
	kOver:SetCustomDataAsInt(kUI:GetCustomDataAsInt())
	local kPos = kList:GetLocation()
	kPos:SetX(kPos:GetX() + kUI:GetLocation():GetX() - 6)
	kPos:SetY(kPos:GetY() + kUI:GetLocation():GetY() - 4)
	kOver:SetLocation(kPos)
end

function OnClickCommonQTalkItem(kUI)
	local kParent = kUI:GetParent()
	if kParent:IsNil() then
		return
	end
	local kList = kParent:GetControl("LIST_COMMAND")
	if kList:IsNil() then
		return
	end
	local iCount = kList:GetListItemCount()
	local kKey = kUI:GetCustomDataAsInt()
	if 0 < iCount then
		local kItem = kList:ListFirstItem()
		while not kItem:IsNil() do
			local kItemKey = kItem:GetWnd():GetCustomDataAsInt()
			if kKey == kItemKey then
				OnItemSelected_From_QuestInfo(kItem:GetWnd(), kKey)
				return
			end	
			kItem = kList:ListNextItem(kItem)
		end
	end
end






















