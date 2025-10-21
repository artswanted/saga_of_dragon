--	DoFile("UI/Auction.lua")
-- ref ECostCharge in PgMargetDef.h
MG_NORMAL 	= 0
MG_GOOD		= 1
MG_HISTORY	= 2

function SetSearchItemGrade(UISelf, Level)
	local GradeTxtWnd = UISelf:GetParent():GetParent():GetControl("SFRM_ITEM_LANK")
	if GradeTxtWnd:IsNil() then
		return
	end
	GradeTxtWnd:SetStaticTextW(UISelf:GetStaticText())
	GradeTxtWnd:SetCustomDataAsInt(Level)
	UISelf:GetParent():Visible(false)
end

function SetSearchItemDisplayGrade(UISelf, Level)
	local GradeTxtWnd = UISelf:GetParent():GetParent():GetControl("SFRM_ITEM_GRADE")
	if GradeTxtWnd:IsNil() then
		return
	end
	GradeTxtWnd:SetStaticTextW(UISelf:GetStaticText())
	GradeTxtWnd:SetCustomDataAsInt(Level)
	UISelf:GetParent():Visible(false)
end

function SetSearchJobLimit(UISelf, Type)
	local JobLimitTextWnd = UISelf:GetParent():GetParent():GetControl("SFRM_ITEM_CLASS")
	if JobLimitTextWnd:IsNil() then
		return
	end
	JobLimitTextWnd:SetStaticTextW(UISelf:GetStaticText())
	JobLimitTextWnd:SetCustomDataAsInt(Type)
	UISelf:GetParent():Visible(false)
end

g_AuctionRadio = {}
g_AuctionRadio[1] = { [1]="CBTN_ITEM",		[2]="CBTN_SELLER"		}
g_AuctionRadio[2] = { [1]="CBTN_GOLD_SELL",	[2]="CBTN_CASH_SELL"	}
g_AuctionRadio[3] = { [1]="CBTN_SHOP_STYLE1", [2]="CBTN_SHOP_STYLE2", [3]="CBTN_SHOP_STYLE3" }
function AuctionRadioAction(ParentWnd, Type, Number)
	if false ~= ParentWnd:IsNil() then 
		return 
	end

	for	Key, Val in pairs(g_AuctionRadio[Type]) do
		local	Child = ParentWnd:GetControl(Val)
		if false == Child:IsNil() then
			Child:CheckState(false)
		end
	end
	
	local Child = ParentWnd:GetControl(g_AuctionRadio[Type][Number])
	if false == Child:IsNil() then
		Child:CheckState(true)
	end
end

g_auItemType		= {}
g_auItemType[1] 	= {["Key"]=1, 	["TTW"]=400069,	["MEMO"]="Equip"}
g_auItemType[1][1] 	= {["Key"]=20, 	["TTW"]=70020, 	["MEMO"]="Helmet"}
g_auItemType[1][2] 	= {["Key"]=3, 	["TTW"]=70003, 	["MEMO"]="Shoulder"}
g_auItemType[1][3] 	= {["Key"]=21, 	["TTW"]=70021, 	["MEMO"]="tunic"}
g_auItemType[1][4] 	= {["Key"]=22, 	["TTW"]=70022, 	["MEMO"]="Pants"}
g_auItemType[1][5] 	= {["Key"]=24, 	["TTW"]=70024, 	["MEMO"]="Glove"}
g_auItemType[1][6] 	= {["Key"]=23, 	["TTW"]=70023, 	["MEMO"]="boot"}
g_auItemType[1][7] 	= {["Key"]=27, 	["TTW"]=70026, 	["MEMO"]="kickball"}
g_auItemType[1][8] 	= {["Key"]=7, 	["TTW"]=70007, 	["MEMO"]="Shield"}
g_auItemType[1][9] 	= {["Key"]=6, 	["TTW"]=70006, 	["MEMO"]="Weapon"}
g_auItemType[1][10] 	= {["Key"]=4, 	["TTW"]=70004, 	["MEMO"]="BackPack"}
g_auItemType[1][11] = {["Key"]=8, 	["TTW"]=70008, 	["MEMO"]="Necklace"}
g_auItemType[1][12] = {["Key"]=9, 	["TTW"]=70009, 	["MEMO"]="Earring"}
g_auItemType[1][13] = {["Key"]=10, 	["TTW"]=70010, 	["MEMO"]="Ring"}
g_auItemType[1][14] = {["Key"]=11, 	["TTW"]=70011, 	["MEMO"]="Belt"}
g_auItemType[1][15] = {["Key"]=0, 	["TTW"]=400452, 	["MEMO"]="ETC"}
g_auItemType[2] 	= {["Key"]=2, 	["TTW"]=403060,	["MEMO"]="Consume"}
g_auItemType[3] 	= {["Key"]=3, 	["TTW"]=403061,	["MEMO"]="Etc"}
g_auItemType[4] 	= {["Key"]=4, 	["TTW"]=403062,	["MEMO"]="Cash"}
g_auItemType[5] 	= {["Key"]=91, 	["TTW"]=403266,	["MEMO"]="CashTrade"}
g_auItemType[6] 	= {["Key"]=92, 	["TTW"]=403267,	["MEMO"]="GoldTrade"}
--g_auItemType[8] = {["Key"]=93, ["TTW"]=403266, ["MEMO"]="CashTrade"}

--각 국가별 필요없는 카타고리 탭 제거
function IsCashTrade()
	local eLocale = GetLocale()
	return eLocale == LOCALE.NC_DEVELOP
	or eLocale == LOCALE.NC_TAIWAN
	or eLocale == LOCALE.NC_CHINA
end

if not IsCashTrade() then
	g_auItemType[5] = nil
	g_auItemType[6] = nil
end

function	Auction_Search_Item_Type_Init(UISelf)
	if false ~= UISelf:IsNil() then
		return
	end
	
	local	Child = UISelf:GetControl("FRM_SEARCH")
	if false ~= Child:IsNil() then 
		return 
	end
	
	local	ListWnd = Child:GetControl("LIST_AUCTION_ITEM_TYPE")
	if false ~= ListWnd:IsNil() then 
		return
	end
	ListWnd:ClearAllListItem()
	for Key, Val in pairs(g_auItemType) do
		local Item = ListWnd:AddNewListItemChar("")
		if false == Item:IsNil() then
			local ItemWnd = Item:GetWnd()
			local TextWnd = ItemWnd:GetControl("CBTN_SELECT")

			if false == TextWnd:IsNil() then
				TextWnd:SetStaticText(GetTT(g_auItemType[Key]["TTW"]):GetStr())
				TextWnd:SetCustomDataAsInt(g_auItemType[Key]["Key"])
			end
			for SKey, SVal in pairs(g_auItemType[Key]) do
				if "Key" ~= SKey and "TTW" ~= SKey and "MEMO" ~= SKey then
					local SItem = ItemWnd:AddNewTreeItemChar(GetTT(g_auItemType[Key][SKey]["TTW"]):GetStr())
					if false == SItem:IsNil() then
						local SItemWnd = SItem:GetWnd()
						SItemWnd:SetCustomDataAsInt(g_auItemType[Key][SKey]["Key"])
					end
				end
			end
		end
	end
end

function MainActionBtnState()
	local UIAuction = GetUIWnd("FRM_AUCTION_MAIN")
	if UIAuction:IsNil() then
		return
	end
	local UIActBtn = UIAuction:GetControl("BTN_ACTION")
	if UIActBtn:IsNil() then
		return
	end
	UIActBtn:Disable(false)
end

function Auction_Tree_Reset(kSelf)
	ODS("CHECK\n")
	local UIParent = kSelf:GetParent()
	local iMax = UIParent:GetTreeItemCount()
	if 0 < iMax then
		local ATRItem = UIParent:TreeFirstItem()
		while false == ATRItem:IsNil() do
			local ATRItemWnd = ATRItem:GetWnd()
			ATRItemWnd:CheckState(false)
			ATRItemWnd:LockClick(false)
			ATRItem = UIParent:TreeNextItem(ATRItem)		
		end
	end
	kSelf:CheckState(true)
	kSelf:LockClick(true)
end

function Auction_List_Reset(UIParent)
	local ItemCnt = UIParent:GetListItemCount()
	if 0 < ItemCnt then
		local ATRItem = UIParent:ListFirstItem()
		while false == ATRItem:IsNil() do
			local ATRItemWnd = ATRItem:GetWnd()
			Auction_Tree_Reset(ATRItemWnd)
			local ATRBtnWnd = ATRItemWnd:GetControl("CBTN_SELECT")
			if false == ATRBtnWnd:IsNil() then
				ATRBtnWnd:CheckState(false)
				ATRItemWnd:CollapseTree()
			end
			ATRItem = UIParent:ListNextItem(ATRItem)		
		end
	end
end

function Auction_Order_Arrow(UISelf)
	local Child = UISelf:GetControl("FRM_VIEW_ALIGN")
	if false == Child:IsNil() then
		local UVIndex = Child:GetUVIndex()
		if 1 == UVIndex then
			Child:SetUVIndex(2)
		else
			Child:SetUVIndex(1)
		end
	end
end

function Auction_SetCurrentShopStyle(kUISelf, eECC_TYPE)
	if(nil == kUISelf) then return end
	if(true == kUISelf:IsNil()) then return end		
	
	local kParent = kUISelf:GetParent()
	local kSyle1 = kParent:GetControl("CBTN_SHOP_STYLE1")
	if( nil == kSyle1 ) then return end
	if(true == kSyle1:IsNil()) then return end		
	
	local kSyle2 = kParent:GetControl("CBTN_SHOP_STYLE2")
	if( nil == kSyle2 ) then return end
	if(true == kSyle2:IsNil()) then return end		
	
	local kSyle3 = kParent:GetControl("CBTN_SHOP_STYLE3")
	if( nil == kSyle3 ) then return end
	if(true == kSyle3:IsNil()) then return end		
		
	SetCurrentMarketGrade(eECC_TYPE);
end


function Auction_Blink_Tick( kUI )
	if kUI:IsNil() == false then				
		local blinkTime = kUI:GetCustomDataAsInt();
				
		kUI:SetCustomDataAsInt(blinkTime-1)
		if blinkTime > 0 then
			blinkTime = blinkTime-1
		end
		
		if( blinkTime%2 == 0) then
			kUI:SetUVIndex(5)
		else
			kUI:SetUVIndex(6)
		end
		
		if kUI:GetCustomDataAsInt() == 0 then 
			kUI:Visible(false)
		end		
		
	end
end

function Auction_TreeItemClick( kSelf )
	Auction_List_Reset(kSelf:GetParent():GetParent())
	kSelf:CheckState(true)
	local Data = kSelf:GetCustomDataAsInt()
	if Data < 91 then
		SelectItemType(0, Data)
	else
		SelectItemType(Data, 0)
	end
	kSelf:GetParent():ExpandTree()
end

function Auction_ItemClassDrop( kSelf )
	if kSelf:IsNil() then return end
	
	local Child = kSelf:GetControl("SFRM_ITEM_CLASS_DROP")
	if false == Child:IsNil() then
		Child:Visible(not Child:IsVisible())
	end
end

function Auction_ItemDisplayGradeDrop( kSelf )
	if kSelf:IsNil() then return end
	
	local Child = kSelf:GetControl("SFRM_ITEM_GRADE_DROP")
	if false == Child:IsNil() then
		Child:Visible(not Child:IsVisible())
	end
end

function Auction_ItemLankDrop( kSelf )
	if kSelf:IsNil() then return end
	
	local Child = kSelf:GetControl("SFRM_ITEM_LANK_DROP")
	if false == Child:IsNil() then
		Child:Visible(not Child:IsVisible())
	end
end

function Auction_SearchClick()
	local kMain = GetUIWnd("FRM_AUCTION_MAIN")
	if kMain:IsNil() then return end
	
	local kSearch = kMain:GetControl("FRM_SEARCH")
	if kSearch:IsNil() then return end
	
	local kBtn = kMain:GetControl("BTN_SEARCH")
	if kBtn:IsNil() then return end
	
	SearchItemList(kSearch, kBtn)
end

function Auction_ShopStyleClick(kWnd, iType)
	local iNumber = 0
	local Grade = 0
	
	if IsChangeShop(iType) then
		if 0 == iType then
			iNumber = 1
			Grade = MG_NORMAL
		elseif 1 == iType then
			iNumber = 2
			Grade = MG_GOOD
		elseif 2 == iType then
			iNumber = 3
			Grade = MG_HISTORY
		end
		
		if 0 ~= iNumber then
			AuctionRadioAction(UISelf:GetParent(), 3, iNumber)
			SetCurrentMarketGrade(Grade)
			UpdateRegList(kWnd)
		end
	else
		FixGradeRadioBtn()
	end
end
	
function Call_MarketAction()
	local UIMain = GetUIWnd("FRM_AUCTION_MAIN")
	if UIMain:IsNil() then return end
	
	local UIAction = UIMain:GetControl("BTN_ACTION")
	if UIAction:IsNil() then return end
	
	if false == UIAction:IsDisable() then
		MarketAction(UIAction)
	end
end

function OpenMarketCloseUI()
	CloseUI("SFRM_CONFIRM_OPEN")
	CloseUI("SFRM_OPEN_MARKET_BUY")
end

-- 노점 UI [Vendor System] --
INV_COUNT = 23
function CloseVendor()
	ResetSelectArticle()
	ResetVendorSelect()
	CloseUI("SFRM_VENDOR_INV")
	CloseUI("SFRM_VENDOR_BUY")
	CloseUI("SFRM_CALCULATOR")
end

function ResetVendorSelect()
	local kMain = GetUIWnd("SFRM_VENDOR_INV")
	if false == kMain:IsNil() then
		local kShadow = kMain:GetControl("SFRM_INV_SHADOW")
		if false == kShadow:IsNil() then
			local i = 0
			for i=0, INV_COUNT do
				local kSlot = kShadow:GetControl("BTN_SLOT_SELECT" ..i )
				if false == kSlot:IsNil() then
					kSlot:CheckState(false)
				end
			end
		end
	end
end

function Selectitem(kWnd)
	local i = 0
	for i=0, INV_COUNT do
		local kSlot = kWnd:GetParent():GetControl("BTN_SLOT_SELECT" ..i )
		if kSlot:GetBuildIndex() == kWnd:GetBuildIndex() then
			local kItem = kWnd:GetParent():GetControl("FRM_BASKET_ICON" ..i )
			if not kItem:IsNil() then
				SetPrice(kItem)
			end
		else
			kSlot:CheckState(false)
		end
	end
end

function VendorInvSelectMarkerOnOver(self, parent)
	local FormName = "FRM_BASKET_ICON"
	local UIIcon = parent:GetControl(FormName..self:GetBuildIndex())
	if not UIIcon:IsNil() then
		OnOverVendorItemToolTip(UIIcon)
	end
end

function RequestVendorEnter()
	local packet = NewPacket(16183)
	local kGuid = g_pilotMan:GetPlayerPilotGuid()
	packet:PushGuid(kGuid)
	Net_Send(packet)
	DeletePacket(packet)
end