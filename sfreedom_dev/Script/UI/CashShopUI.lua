g_CashShopTableEquipName = {70005, 51204, 51203, 51202, 70020, 70003, 70021, 70022, 70024, 70023, 70006, 70007, 70004, 51210}
g_CashShopTableBgIcon = {}
g_CashShopTableBgIcon[70005] = { UVIndex=7, SRC=2 }--얼굴
g_CashShopTableBgIcon[51204] = { UVIndex=3, SRC=4 }--성형
g_CashShopTableBgIcon[51203] = { UVIndex=2, SRC=4 }--헤어
g_CashShopTableBgIcon[51202] = { UVIndex=1, SRC=4 }--염색
g_CashShopTableBgIcon[70020] = { UVIndex=1, SRC=2 }--투구
g_CashShopTableBgIcon[70003] = { UVIndex=2, SRC=2 }--어깨
g_CashShopTableBgIcon[70021] = { UVIndex=3, SRC=2 }--상의
g_CashShopTableBgIcon[70022] = { UVIndex=5, SRC=2 }--하의
g_CashShopTableBgIcon[70024] = { UVIndex=4, SRC=2 }--장갑
g_CashShopTableBgIcon[70023] = { UVIndex=6, SRC=2 }--신발
g_CashShopTableBgIcon[70006] = { UVIndex=1, SRC=3 }--무기
g_CashShopTableBgIcon[70007] = { UVIndex=8, SRC=2 }--방패
g_CashShopTableBgIcon[70004] = { UVIndex=2, SRC=1 }--백팩
g_CashShopTableBgIcon[51210] = { UVIndex=15, SRC=2 }--풍선
g_CashShopIconSrc = {}
g_CashShopIconSrc[1] = { SRC="../Data/6_UI/info/ifIconBp.tga", U=6, V=6, W=240, H=240 }
g_CashShopIconSrc[2] = { SRC="../Data/6_UI/info/ifIconEtc.tga", U=3, V=5, W=120, H=200 }
g_CashShopIconSrc[3] = { SRC="../Data/6_UI/info/ifIconWp.tga", U=6, V=6, W=240, H=240 }
g_CashShopIconSrc[4] = { SRC="../Data/6_UI/info/infoStImg.tga", U=3, V=1, W=120, H=40 }
g_CashShopType={}
g_CashShopType["CASH"] 		= 0
g_CashShopType["MILEAGE"] 	= 1



function UI_Click_CashShop_Tab(wnd)
	if wnd:IsNil() == true then return end
	local Parent = wnd:GetParent()
	if Parent:IsNil() == true then return end
	
	local iLast = tonumber(string.sub(wnd:GetID():GetStr(), -1))

	for i=0,7 do
		local Btn = Parent:GetControl("CBTN_TAB"..i)
		if Btn:IsNil() == false and Btn:IsVisible() then
			if i~=iLast then
				Btn:CheckState(false)
				Btn:LockClick(false)
			end
		end
	end
	TopCategoryOnClick(Parent, iLast)
--[[	local iCount = 0	--소스코드로 옮김
	for i=0,10 do	--버튼들 돌자
		local wndBtn = Parent:GetControl("CBTN_SECOND_TAB"..i)
		if false==wndBtn:IsNil() then
			local ptSize = Point2(1,20)
			local ptPos = Point2(666,50)
			if iCount >= g_CashShopTableSecTitle[iLast]["cnt"] then
				wndBtn:SetStaticText("")
				wndBtn:SetClose(true)
			else
				wndBtn:SetClose(false)
				local iTTW = g_CashShopTableSecTitle[iLast]["ttw"]+i
				wndBtn:SetSize(Point2(200,ptSize:GetY()))	--임시로
				wndBtn:SetStaticTextW(GetTT(iTTW))
				ptSize:SetX(wndBtn:GetTextSize():GetX()+2)
				if 0==i then
					ptPos:SetX(52)
					wndBtn:LockClick(true)
					wndBtn:CheckState(true)
				else
					local wndPreBtn = Parent:GetControl("CBTN_SECOND_TAB"..i-1)
					if false==wndPreBtn:IsNil() then
						local posx = wndPreBtn:GetLocation():GetX()+wndPreBtn:GetSize():GetX()+13
						ptPos:SetX(posx)
					end
					wndBtn:CheckState(false)
				end
			end
			wndBtn:SetSize(ptSize)
			wndBtn:SetLocation(ptPos)
		end
		iCount=iCount+1
	end

	local bGift = (5 == iLast)

	Parent:GetControl("FRM_GIFT"):Visible(bGift)
	Parent:GetControl("FRM_GOODS"):Visible(false==bGift)]]
end

function UI_Click_CashShop_SecTab(wnd)
	if wnd:IsNil() == true then return end
	wnd:LockClick(true)
	if wnd:GetCheckState() == true then --선택된걸 또 선택
		return 
	else
		wnd:CheckState(true)
	end 

	local Parent = wnd:GetParent()
	if Parent:IsNil() == true then return end
	
	local Builder = Parent:GetControl("BLD_CBTN_SECOND_TAB")
	if Builder:IsNil() then
		return
	end
	
	local BuildCountPt = Builder:GetBuildCount()
	local BuildCount = BuildCountPt:GetX() * BuildCountPt:GetY()

	local iLast = wnd:GetBuildIndex()
	
	for i=0, BuildCount do
		local Btn = Parent:GetControl("CBTN_SECOND_TAB"..i)
		if Btn:IsNil() == false then
			if i~=iLast then
				Btn:CheckState(false)
				Btn:LockClick(true)
			end
		end
	end
end

function UI_CashShopEquipName(wnd)
	if wnd:IsNil() == true then return end
	local iIdx = wnd:GetParent():GetBuildIndex()
	iIdx = g_CashShopTableEquipName[iIdx+1]
	wnd:SetStaticTextW(GetTextW(iIdx))
	
	local BgIconInfo = g_CashShopTableBgIcon[ iIdx ]
	if BgIconInfo == nil then
		return
	end
	local IconSrc = g_CashShopIconSrc[ BgIconInfo.SRC ]
	if IconSrc == nil then
		return
	end
	
	wnd:ChangeImage(IconSrc.SRC)
	local Size = wnd:GetImgSize()
	Size:SetX(IconSrc.W)
	Size:SetY(IconSrc.H)
	wnd:SetImgSize(Size)	
	wnd:SetMaxUVIndex(IconSrc.U, IconSrc.V)
	wnd:SetUVIndex( BgIconInfo.UVIndex )
end

function UI_T_Y_ToCenter(wnd)
	if wnd:IsNil() == true then return end
	local text = wnd:GetStaticText()
	if nil==text or text:IsNil() then
		return
	end
	local kTextPos = wnd:GetTextPos()
	local kTxtSize = wnd:GetTextSize()
	kTextPos:SetY( (wnd:GetSize():GetY() - kTxtSize:GetY())/2 )
	wnd:SetTextPos( kTextPos )
end

function UI_CashShopUIInit(wnd)
	if wnd:IsNil() == true then return end

	--대분류탭 
	local Btn0 = wnd:GetControl("CBTN_TAB0") 
	UI_Click_CashShop_Tab(Btn0)
--[[	if false==Btn0:IsNil() then
		Btn0:CheckState(true)
		Btn0:LockClick(true)
	end]]

	for i=1,5 do
		local Btn = wnd:GetControl("CBTN_TAB"..i)
		if Btn:IsNil() == false then
			if i~=iLast then
				Btn:CheckState(false)
				Btn:LockClick(true)
			end
		end
	end
	------------------------------------------------
	
	-- 베스트 아이템
	local tab1 = wnd:GetControl("CBTN_LEFT_TAB1")
	if false==tab1:IsNil() then
		tab1:CheckState(false)
		tab1:LockClick(false)
	end

	local tab0 = wnd:GetControl("CBTN_LEFT_TAB0")
	if false==tab1:IsNil() then
		tab0:CheckState(true)
		tab0:LockClick(true)
	end

	local best = UISelf:GetControl("FRM_BEST")
	local rank = UISelf:GetControl("FRM_RANK")
	if false==best:IsNil() then
		best:Visible(true)
	end
	if false==rank:IsNil() then
		rank:Visible(false)
	end
	------------------------------------------------
end

function SetTextCashOrDP(kSelf, CashTextID, DPTextID)
	if(g_CashShopType["CASH"] ==  CurrentBuyType()) then 		
		kSelf:SetStaticTextW(GetTT(CashTextID))
	elseif(g_CashShopType["MILEAGE"] ==  CurrentBuyType()) then 		
		kSelf:SetStaticTextW(GetTT(DPTextID))
	end	
end

--------------------- Milage 계산에 관계되는 값( PgCashShop::CalcLocalAddBonus() ) ----------------------------
function Milage_100Nomalizer()
	local eLocale = GetLocale()
	if(eLocale == LOCALE.NC_TAIWAN) then
		return 100
	elseif(eLocale == LOCALE.NC_CHINA) then
		return 10000
	elseif(eLocale == LOCALE.NC_DEVELOP) then
		return 100
	elseif(eLocale == LOCALE.NC_SINGAPORE ) then	
		return 100
	elseif(eLocale == LOCALE.NC_THAILAND ) then
		return 100
	elseif(eLocale == LOCALE.NC_VIETNAM ) then
		return 100
	elseif(eLocale == LOCALE.NC_INDONESIA ) then
		return 100
	elseif(eLocale == LOCALE.NC_PHILIPPINES) then
		return 100
	elseif(eLocale == LOCALE.NC_JAPAN ) then
		return 1
	elseif(eLocale == LOCALE.NC_EU ) then
		return 100
	end
	return 1
end

function Milage_100UnitAmp()
	local eLocale = GetLocale()
	if(eLocale == LOCALE.NC_TAIWAN) then
		return 3
	elseif(eLocale == LOCALE.NC_CHINA) then
		return 300
	elseif(eLocale == LOCALE.NC_DEVELOP) then
		return 3
	elseif(eLocale == LOCALE.NC_SINGAPORE) then
		return 1
	elseif(eLocale == LOCALE.NC_THAILAND ) then
		return 1
	elseif(eLocale == LOCALE.NC_VIETNAM ) then
		return 1
	elseif(eLocale == LOCALE.NC_INDONESIA ) then
		return 1
	elseif(eLocale == LOCALE.NC_PHILIPPINES) then
		return 1
	elseif(eLocale == LOCALE.NC_JAPAN ) then
		return 0
	elseif(eLocale == LOCALE.NC_EU ) then
		return 1
	end
	return 1
end

function Milage_1000Nomalizer()
	local eLocale = GetLocale()
	if(eLocale == LOCALE.NC_TAIWAN) then
		return 1000
	elseif(eLocale == LOCALE.NC_CHINA) then
		return 100000
	elseif(eLocale == LOCALE.NC_DEVELOP) then
		return 1000
	elseif(eLocale == LOCALE.NC_SINGAPORE) then
		return 1000
	elseif(eLocale == LOCALE.NC_THAILAND ) then
		return 1000
	elseif(eLocale == LOCALE.NC_VIETNAM ) then
		return 1000
	elseif(eLocale == LOCALE.NC_INDONESIA ) then
		return 1000
	elseif(eLocale == LOCALE.NC_PHILIPPINES) then
		return 1000
	elseif(eLocale == LOCALE.NC_JAPAN ) then
		return 1
	elseif(eLocale == LOCALE.NC_EU ) then
		return 1000
	end		
	return 1
end

function Milage_1000UnitAmp()
	local eLocale = GetLocale()
	if(eLocale == LOCALE.NC_TAIWAN) then
		return 10
	elseif(eLocale == LOCALE.NC_CHINA) then
		return 1000
	elseif(eLocale == LOCALE.NC_DEVELOP) then
		return 10
	elseif(eLocale == LOCALE.NC_SINGAPORE) then
		return 0
	elseif(eLocale == LOCALE.NC_THAILAND ) then
		return 0
	elseif(eLocale == LOCALE.NC_VIETNAM ) then
		return 0
	elseif(eLocale == LOCALE.NC_INDONESIA ) then
		return 0
	elseif(eLocale == LOCALE.NC_PHILIPPINES) then
		return 0
	elseif(eLocale == LOCALE.NC_JAPAN ) then
		return 0
	elseif(eLocale == LOCALE.NC_EU ) then
		return 0
	end	
	return 1
end
-------------------------------------------------------------------------------------------------------------------

function CashShopItemMarkFileName(iIdx)
	local FileName = "../Data/6_ui/cashShop/csMark"..iIdx..".tga"
	return FileName
end

function CashShopInvViewIsBasket(Parent)
	if Parent:IsNil() then return false end
	
	local InvTapParent = Parent:GetControl("SFRM_CASH_TITLE");
	if InvTapParent:IsNil() then return false end
	
	local InvTap = InvTapParent:GetControl("BTN_TITLE_TAB2");
	if InvTap:IsNil() then return false end

	return InvTap:GetCheckState()
end

function CashShopInvSelectMarkerOnTick(self, parent)
	local FormName = "ICN_CASH_INV"
	local bIsViewBasket = CashShopInvViewIsBasket(parent);
	if bIsViewBasket then
		FormName = "FRM_BASKET_ICON"
	end
	
	local iBldIdx = self:GetBuildIndex()
	local UIIcon = parent:GetControl(FormName..iBldIdx)
	if not UIIcon:IsNil() then
		if not ExistPosInItem(UIIcon, bIsViewBasket) then
			self:CheckState(false)
		elseif(3 == GetShopType()) then -- 코스튭 조합
			local bVisible = false
			if(CostumeMix_IsInSlot(UIIcon)) then
				bVisible = true
			end
			local kCostumeMixCover = parent:GetControl("FRM_COSTUMEMIX_COVER"..iBldIdx)
			if(false == kCostumeMixCover:IsNil()) then				
				kCostumeMixCover:Visible(bVisible)
			end
		end
	end
end

function CashShopInvSelectMarkerOnLClick(self, parent)
	local FormName = "ICN_CASH_INV"
	local bIsViewBasket = CashShopInvViewIsBasket(parent)
	if bIsViewBasket then
		FormName = "FRM_BASKET_ICON"
	end
	
	local UIIcon = parent:GetControl(FormName..self:GetBuildIndex())
	if not UIIcon:IsNil() then
		if not bIsViewBasket then
			UIIcon:IconDoAction()
		end
	end
end

function CashShopInvSelectMarkerOnRClick(self, parent)
	local FormName = "ICN_CASH_INV"
	local bIsViewBasket = CashShopInvViewIsBasket(parent)
	if bIsViewBasket then
		FormName = "FRM_BASKET_ICON"
	end
	
	local UIIcon = parent:GetControl(FormName..self:GetBuildIndex())
	if not UIIcon:IsNil() then
		if bIsViewBasket then
			ConfirmBasketDeRegItem(UIIcon)
		elseif(3 == GetShopType()) then -- 코스튭 조합
			CostumeMixInsertSlotForMix(UIIcon)
		end
	end
end

function CashShopInvSelectMarkerOnOver(self, parent)
	local FormName = "ICN_CASH_INV"
	local bIsViewBasket = CashShopInvViewIsBasket(parent);
	if bIsViewBasket then
		FormName = "FRM_BASKET_ICON"
	end
	
	local cur = parent:GetControl("IMG_CURSOR")
	if false == cur:IsNil() then
		cur:Visible(true)
		local pos = self:GetLocation()
		pos:SetX(pos:GetX()+1)
		pos:SetY(pos:GetY()+1)
		cur:SetLocation(pos)
	end
	local UIIcon = parent:GetControl(FormName..self:GetBuildIndex())
	if not UIIcon:IsNil() then
		if bIsViewBasket then
			UIIcon:CallCashItemToolTip()
		else
			UIIcon:CallIconToolTip()
		end
	end
end

function CashShopInvMoveBtnCustomAction(self, parent)
	local bIsViewBasket = CashShopInvViewIsBasket(parent);
	if bIsViewBasket then
		BuyCashItemOnClick(parent, CIBT_BASKET_PART)
	elseif(3 == GetShopType()) then
		AddWarnDataTT(790733)
	else
		SelectItemMoveToInv(parent)
	end
end

function CashShopInvRenewBtnCustomAction(self, parent)
	local bIsViewBasket = CashShopInvViewIsBasket(parent);
	if bIsViewBasket then
		BuyCashItemOnClick(parent, CIBT_BASKET)
	else
		SelectItemReNew(parent)
	end
end

-------------------------------------------------------------------------------------------------------------------
g_CSRoulette = {}
g_CSRoulette["DUMMY_ROTATATION"] = 0	-- 유저가 선택하기 전까지 화살표가 회전하는데, 그 화살의 회전값(각도)을 저장하는 부분
g_CSRoulette["RADIAN"] = 0		-- 유저가 멈춤을 눌렀을때, 화살표의 회전이 천천히 감소할때 사용하는 회전값(라디안)
g_CSRoulette["STATE"] = "STOP"	-- STOP(멈춰진상태), ROLLING(룰렛이 도는중),WAIT_RESULT(결과에 멈출떄까지 천천히도는중)
g_CSRoulette["MAX_IDX"] = 10		-- 룰렛에 나눠진 수
g_CSRoulette["RESULT_IDX"] = 0	-- 결과 아이템이 있는 인덱스 
g_CSRoulette["DEG_UNIT"] = 360/g_CSRoulette["MAX_IDX"]	-- 룰렛에서 한칸의 각도
g_CSRoulette["CAN_CLOSE"] = false
g_CSRoulette["ACC_MOVED_DEG"] = 0
g_CSRoulette["TEMP"] = 0
g_CSRoulette["START_ACCTIME"] = 0
g_CSRoulette["AUTO_STOP_TIME"] = 5

function Roulette_SetCanClose(bClose)
	g_CSRoulette["CAN_CLOSE"] = bClose
	Roulette_DisbleCloseBtn(not bClose)
end

function Roulette_Init()
	g_CSRoulette["DUMMY_ROTATATION"] = 0
	g_CSRoulette["RADIAN"] = 0
	g_CSRoulette["STATE"] = "STOP"	
	Roulette_SetCanClose(false)
	--g_CSRoulette["CAN_CLOSE"] = false
	
	g_CSRoulette["ACC_MOVED_DEG"] = 0
	g_CSRoulette["TEMP"] = 0
	g_CSRoulette["START_ACCTIME"] = 0
	--g_CSRoulette["RESULT_IDX"] = 0
end

function Roulette_OnTick(kWnd)
	if(nil == kWnd) then return end
	if(true == kWnd:IsNil()) then return end
	
	if("STOP" == g_CSRoulette["STATE"]) then 
	
	elseif("ROLLING" == g_CSRoulette["STATE"]) then 
		local fAccelDeg = 30 *20* GetFrameTime()
		g_CSRoulette["DUMMY_ROTATATION"] = g_CSRoulette["DUMMY_ROTATATION"] + fAccelDeg
		kWnd:SetRotationDeg(g_CSRoulette["DUMMY_ROTATATION"])
		-- 소리 낼 만큼 이동했는가 
		g_CSRoulette["ACC_MOVED_DEG"] = g_CSRoulette["ACC_MOVED_DEG"]+fAccelDeg
		if(g_CSRoulette["START_ACCTIME"] ~= 0 and
		   GetAccumTime() - g_CSRoulette["START_ACCTIME"] > g_CSRoulette["AUTO_STOP_TIME"]) then
			Roulette_Decision()
			PlaySound("../Sound/U_Sound/button(click).wav")
			g_CSRoulette["START_ACCTIME"] = 0
		end
	elseif("WAIT_RESULT" == g_CSRoulette["STATE"]) then 
		local fAccelDeg = 15*DEGTORAD
		local fMaxIncDeg = 90*DEGTORAD	
		-- 회전 값 증가 계산		
		local fForce = Roulette_GetCalcForce()		
		local fApplyDeg = fForce*math.sin(g_CSRoulette["RADIAN"])
		
		-- 소리 낼 만큼 이동했는가 		
		g_CSRoulette["ACC_MOVED_DEG"] = g_CSRoulette["ACC_MOVED_DEG"]+ math.abs(fApplyDeg-g_CSRoulette["TEMP"])
		g_CSRoulette["TEMP"] = fApplyDeg
		
		kWnd:SetRotationDeg(fApplyDeg)
		g_CSRoulette["RADIAN"] = g_CSRoulette["RADIAN"] + fAccelDeg * GetFrameTime()
		if(fMaxIncDeg < g_CSRoulette["RADIAN"]) then
			g_CSRoulette["RADIAN"] = fMaxIncDeg
			g_CSRoulette["STATE"] = "STOP"
			ReqGetCSGachaResultItem()	-- 미리 정해진 최종 아이템을 받도록 요청

			local kRoulette = GetUIWnd("FRM_ROULETTE")
			if(nil == kRoulette) then return end
			if(true == kRoulette:IsNil()) then return end				
			local kBtn = kRoulette:GetControl("BTN_DECISION")
			if(nil == kBtn) then return end
			if(true == kBtn:IsNil()) then return end
			--[[
			if(false == ReqStartCSGachaRoulette()) then
				AddWarnDataTT(550005)
				Roulette_Close()
				return
			end]]
			Roulette_SetCanClose(true)
			--g_CSRoulette["CAN_CLOSE"] = true
			
			Roulette_BtnChange("AGAIN")
			--Roulette_Init()
		end
	end
	CSGacha_TickSound()
end

function Roulette_Close()
	if(true == g_CSRoulette["CAN_CLOSE"]) then
		Roulette_BtnChange("DEFAULT")
		CloseUI("FRM_ROULETTE")
		Roulette_SetCanClose(false)
		--g_CSRoulette["CAN_CLOSE"] = false
		return true
	end
	return false
end

function Roulette_DisbleCloseBtn(bDisable)
	local kRoulette = GetUIWnd("FRM_ROULETTE")
	if(nil == kRoulette) then return end
	if(true == kRoulette:IsNil()) then return end
	
	local kBtnClose = kRoulette:GetControl("BTN_CLOSE")
	if(nil == kBtnClose) then return end
	if(true == kBtnClose:IsNil()) then return end
	kBtnClose:Disable(bDisable)
end

function Roulette_Decision()
	local kRoulette = GetUIWnd("FRM_ROULETTE")
	if(nil == kRoulette) then return end
	if(true == kRoulette:IsNil()) then return end
	
	local kArrow = kRoulette:GetControl("FRM_ARROW")
	if(nil == kArrow) then return end
	if(true == kArrow:IsNil()) then return end
	
	local kBtn = kRoulette:GetControl("BTN_DECISION")
	if(nil == kBtn) then return end
	if(true == kBtn:IsNil()) then return end
	
	local kBtnStop = kRoulette:GetControl("BTN_DECISION_STOP")
	if(nil == kBtnStop) then return end
	if(true == kBtnStop:IsNil()) then return end
	
	local kBtnClose = kRoulette:GetControl("BTN_DECISION_AGAIN")
	if(nil == kBtnClose) then return end
	if(true == kBtnClose:IsNil()) then return end
	
	local kBtnReload = kRoulette:GetControl("BTN_RELOAD")
	if(nil == kBtnReload) then return end
	if(true == kBtnReload:IsNil()) then return end	

	if("AGAIN" == g_CSRoulette["STATE"]) then
		if(false == ReqStartCSGachaRoulette()) then
			CommonMsgBoxByTextTable(550005, true)
			--AddWarnDataTT(550005)
			Roulette_SetCanClose(true)
			--g_CSRoulette["CAN_CLOSE"] = true
			Roulette_Close()
		else
			Roulette_BtnChange("START")
			kArrow:SetRotationDeg(0)
			Roulette_Init()
			--cg_CSRoulette["CAN_CLOSE"] = false
		end
		kBtnReload:Disable(false)
		return 
	else
		kBtnReload:Disable(true)
	end	
	
	if(true == Roulette_Close()) then
		return
	end
	
	if("WAIT_RESULT" == g_CSRoulette["STATE"]) then
		--결과를 기다리는 중이라면 버튼을 눌러도 소용이 없다
		return
	end
	
	local fForce = Roulette_GetCalcForce()
	local fCugDeg = kArrow:GetRotationDeg()
	if("STOP" == g_CSRoulette["STATE"]) then
		-- 멈춰있는 상태임으로 룰렛을 돌린다
		g_CSRoulette["STATE"] = "ROLLING"
		Roulette_BtnChange("ROLLING")
		g_CSRoulette["START_ACCTIME"] = GetAccumTime()
		kBtnStop:Disable(false)
	elseif("ROLLING" == g_CSRoulette["STATE"]) then
		-- 돌아가고 있는 상태라면, 서서히 멈추게 한다
		--ODS("각도"..fCugDeg.."\n", false, 912)
		--Roulette_Init()	-- 변수값 초기화
		g_CSRoulette["STATE"] = "WAIT_RESULT"
		g_CSRoulette["DUMMY_ROTATATION"] = fCugDeg
		kArrow:SetRotationDeg(fCugDeg)				
		g_CSRoulette["TEMP"] = fCugDeg
		kBtnStop:Disable(true)
	end
	g_CSRoulette["RADIAN"]  = math.asin(fCugDeg/fForce)
end

function Roulette_GetCalcForce()
	--ODS("결과 idx값 :"..g_CSRoulette["RESULT_IDX"].."\n", false, 912)
	return g_CSRoulette["DEG_UNIT"]*g_CSRoulette["RESULT_IDX"] + 360*5	
end

function CallRoulette()
	if(5 <= GetCSGachaRemainReloadCnt()) then
		local strPrevState = g_CSRoulette["STATE"]
		Roulette_Init()
		if("WAIT_RESULT" == strPrevState) then
			g_CSRoulette["STATE"] = strPrevState
		end
		ActivateUI("FRM_ROULETTE", true)
		return true
	end
	return false
end

function RouletteItem_OnBuild(kWnd, iBuildIdx)
	local POS_TBL = {}
	POS_TBL[0] = {X=236, Y= 58}
	POS_TBL[1] = {X=339, Y= 94}
	POS_TBL[2] = {X=403, Y=181}
	POS_TBL[3] = {X=403, Y=293}
	POS_TBL[4] = {X=339, Y=382}
	POS_TBL[5] = {X=236, Y=414}
	POS_TBL[6] = {X=133, Y=382}
	POS_TBL[7] = {X= 69, Y=293}
	POS_TBL[8] = {X= 69, Y=181}
	POS_TBL[9] = {X=133, Y= 94}
	
	if(nil == kWnd) then return end
	if(true == kWnd:IsNil()) then return end
	local i=0	
	for i=0, table.getn(POS_TBL) do
		if(i == iBuildIdx) then
			 kWnd:SetLocation(Point2(POS_TBL[i].X, POS_TBL[i].Y))
			return 
		end
	end	
end

function Roulette_SetResultIdx(iIdx)
	if(iIdx < g_CSRoulette["MAX_IDX"] and 0 <= iIdx) then
		g_CSRoulette["RESULT_IDX"] = iIdx
		--ODS("설정됨"..iIdx.."\n",false ,912)
		return
	end	
	--ODS("설정 안됨\n",false ,912)
end

function Roulette_CanClose()
	return g_CSRoulette["CAN_CLOSE"]
end

function Roulette_Reload()
	local kRoulette = GetUIWnd("FRM_ROULETTE")
	if(nil == kRoulette) then return end
	if(true == kRoulette:IsNil()) then return end
	
	local kBtnReload = kRoulette:GetControl("BTN_RELOAD")
	if(nil == kBtnReload) then return end
	if(true == kBtnReload:IsNil()) then return end	
	
	if( "ROLLING" == g_CSRoulette["STATE"] or true == g_CSRoulette["CAN_CLOSE"]
		or "WAIT_RESULT" == g_CSRoulette["STATE"]) then
		-- 더이상 재설정 할수 없습니다 출력
		AddWarnDataTT(790627)
		return
	end
	if(false == DecCSGachaRemainReloadCnt()) then
		--CommonMsgBoxByTextTable(790627,true)
		AddWarnDataTT(790628)
		return
	end	
	
	ReqReloadCSGachaRoulette() -- 룰렛 재설정 요청
	PlaySoundByID("roulette_item")
	if(0 == GetCSGachaRemainReloadCnt()) then 
		kBtnReload:Disable(true)
	end
end

function Roulette_StartBtnDisable(bDisable)	
	local kCashShop = GetUIWnd("FRM_CASH_SHOP")
	if(true == kCashShop:IsNil()) then return end
	local kCSGacha = kCashShop:GetControl("SFRM_CASH_GACHA")
	if(true == kCSGacha:IsNil()) then return end
	local kBtn = kCSGacha:GetControl("BTN_START")
	if(true == kBtn:IsNil()) then return end
	kBtn:Disable(bDisable)
	local kArrow = kBtn:GetControl("FRM_ARROW")
	if(true == kArrow:IsNil()) then return end
	kArrow:Visible(not bDisable)
end

function Roulette_BtnChange(TypeStr)
	local kRoulette = GetUIWnd("FRM_ROULETTE")
	if(nil == kRoulette) then return end
	if(true == kRoulette:IsNil()) then return end
	
	local kArrow = kRoulette:GetControl("FRM_ARROW")
	if(nil == kArrow) then return end
	if(true == kArrow:IsNil()) then return end
	
	local kBtn = kRoulette:GetControl("BTN_DECISION")
	if(nil == kBtn) then return end
	if(true == kBtn:IsNil()) then return end
	
	local kBtnStop = kRoulette:GetControl("BTN_DECISION_STOP")
	if(nil == kBtnStop) then return end
	if(true == kBtnStop:IsNil()) then return end
	
	local kBtnClose = kRoulette:GetControl("BTN_DECISION_AGAIN")
	if(nil == kBtnClose) then return end
	if(true == kBtnClose:IsNil()) then return end
	
	local bStartVisible = true
	local bStopVisible  = false
	local bCloseVisible = false
	if("ROLLING" == TypeStr) then
		bStartVisible = false
		bStopVisible  = true
		bCloseVisible = false
	elseif("AGAIN" == TypeStr) then
		bStartVisible = false
		bStopVisible  = false
		bCloseVisible = true
	end	
	kBtn:Visible(bStartVisible)
	kBtnStop:Visible(bStopVisible)
	kBtnClose:Visible(bCloseVisible)
end

function OnClick_CSGachaPageBtn(kWnd)	
	if(nil == kWnd) then return end
	if(true == kWnd:IsNil()) then return end
	
	local kParent = kWnd:GetParent()	
	if(true == kParent:IsNil()) then return end
	
	local iPageNo = kWnd:GetCustomDataAsInt()
	
	local i=0
	for i=0, 3 do
		local kPageBtn = kParent:GetControl("CBTN_PAGE"..i);
		if(false == kPageBtn:IsNil()) then 
			kPageBtn:CheckState(false)			
		end
	end
	
	kWnd:CheckState(true)
	kWnd:LockClick(true)
	ShowCSGachaItemPage(iPageNo)
end

function CSGacha_SetRareUIPos(iRareSetCnt, iCurrentPage )
	-- RareUI를 얻어올수 있게 준비 후
	local kCashShop = GetUIWnd("FRM_CASH_SHOP")
	if(true == kCashShop:IsNil()) then return end
	local kCSGacha = kCashShop:GetControl("SFRM_CASH_GACHA")
	if(true == kCSGacha:IsNil()) then return end
	local kBldRareUI = kCSGacha:GetControl("BLD_RARE_ITEM")
	if(true == kBldRareUI:IsNil()) then return end
	
	local MAX_RARE_UI_NUM = CSGacha_MaxRareItemCntOnPage() -- 표현 가능한 UI 갯수 	
	
	local kRareLBtn = kCSGacha:GetControl("BTN_RARE_L")
	if(true == kRareLBtn:IsNil()) then return end
	local kRareRBtn = kCSGacha:GetControl("BTN_RARE_R")
	if(true == kRareRBtn:IsNil()) then return end
	
	local iMaxRarePage = GetCSGachaMaxRareItemPage()
	-- local bRareMoveBtnVisible = false
	-- if(MAX_RARE_UI_NUM  == iRareSetCnt and iCurrentPage == iMaxRarePage) then 
	-- -- 한번에 표현가능 갯수가 최대이고, 레어그룹 아이콘이 한페이지에 보여지지 않을만큼 많다면
		-- bRareMoveBtnVisible = true
	-- end
	
	local bLBtnVisible = false
	local bRBtnVisible = false
	if(0 < iCurrentPage and iCurrentPage < iMaxRarePage) then
	--현재 페이지가 0보다 크고, max보다 작으면 좌우페이지 보여줌
		bLBtnVisible = true
		bRBtnVisible = true
	elseif (0 == iCurrentPage and 0 < iMaxRarePage) then
	--현재 페이지가 0이고, MaxPage가 0보다 클때  우측만 보여줌
		bRBtnVisible = true
	elseif (iMaxRarePage == iCurrentPage and 0 < iMaxRarePage) then
	--현재 페이지가 maxpage이고, maxpage가 0보다 크면 좌측만 보여줌
		bLBtnVisible = true		
	end
	kRareLBtn:Visible(bLBtnVisible)
	kRareRBtn:Visible(bRBtnVisible)
	
	if(0 >= iRareSetCnt) then return end
	if(MAX_RARE_UI_NUM < iRareSetCnt) then iRareSetCnt = MAX_RARE_UI_NUM end
	
	local iUnitWidth = kBldRareUI:GetBuildGab():GetX() --110	-- 박스 X크기 + X_GAB -- 각 레어아이템 UI간 X위치 이격거리 
	local iRareTotalWidth = iUnitWidth*iRareSetCnt
	local iRareCenterX = iRareTotalWidth/2
	
	local iSpaceWidth = iUnitWidth * MAX_RARE_UI_NUM --550	-- 가용한 크기
	local iSpaceCenterX = iSpaceWidth/2
	local iSpaceBeginX = 58  --공간 시작점	
	local fRareBoxBeginX = iSpaceCenterX - iRareCenterX + iSpaceBeginX
	
	local iBldIdx = 0
	for iBldIdx=0, MAX_RARE_UI_NUM-1 do 		
		local RareUIName = "FRM_RARE_ITEM"..iBldIdx
		local kRareGroupUI =  kCSGacha:GetControl(RareUIName)		
		if(false == kRareGroupUI:IsNil()) then
			if(iBldIdx < iRareSetCnt) then
				kRareGroupUI:Visible(true)
				local kPos = kRareGroupUI:GetLocation()
				--fRareBoxBeginX을 시작으로 iUnitWidth(이격 길이) 만큼 출력
				local iNewX = iUnitWidth*(iBldIdx) + fRareBoxBeginX
				kPos:SetX(iNewX)
				kRareGroupUI:SetLocation(kPos)
			else
				kRareGroupUI:Visible(false)
			end
		end
	end
end

function CSGacha_MaxRareItemCntOnPage()
	return 5
end

function CSGacha_TickSound()
	if(g_CSRoulette["DEG_UNIT"] < g_CSRoulette["ACC_MOVED_DEG"]) then
		PlaySoundByID("roulette_tick")
		g_CSRoulette["ACC_MOVED_DEG"] = 0
	end
end

function CallCashShopPreviewEquipItemToolTip(kWnd)
	if(2 == GetShopType()) then
		CallCSGachaEquipItemToolTip(kWnd)
	else
		CallPreviewEquipItemToolTip(kWnd)
	end
end

function ClearPreviewEquip()
	if(2 == GetShopType()) then
		ClearCSGachaPreViewEquipItem()
	elseif(3 == GetShopType()) then
		CostumeMixClearPreViewEquipItem()
	end
	ClearPreviewEquipIcon()
end

function CSGachaBtn_Animation(kSelf, fInterTime)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end	
	if(0 == fInterTime) then return end
	
	local TimeCnt = kSelf:GetCustomDataAsFloat()
	TimeCnt= TimeCnt+GetFrameTime()
	if(fInterTime < TimeCnt) then 
		TimeCnt = 0
		local iNextIndex = kSelf:GetUVIndex()+1
		local iMod = kSelf:GetMaxUVIndex()+1
		iNextIndex = iNextIndex % iMod
		if(iNextIndex == 0) then  iNextIndex = 5 end	
		kSelf:SetUVIndex(iNextIndex)
	end	
	kSelf:SetCustomDataAsFloat(TimeCnt)
end

function CashShopBtnInCashShop(kSelf)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end
	
	if kSelf:GetCheckState() ~= true then
		local kParent = kSelf:GetParent()
		HideUI_For_CashShopPart(kParent, false)
		
		local kWnd = kSelf:GetControl("IMG_ANI")
		if(nil ~= kWnd:IsNil()) then
			kWnd:Visible(false)								
		end
		SetShopType(0)
		kSelf:CheckState(true)
		kSelf:LockClick(true)
	end
end

function GachaBtnInCashShop(kSelf)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end
	
	if kSelf:GetCheckState() ~= true then
		local kParent = kSelf:GetParent()
		HideUI_For_CashShopPart(kParent, true)
		
		local kWnd = kSelf:GetControl("IMG_ANI")
		if(nil ~= kWnd:IsNil()) then
			kWnd:Visible(false)
		end
		SetShopType(2)
		ReqCSGachaItemList()
		kSelf:CheckState(true)
		kSelf:LockClick(true)

		for i=1,7 do
			local line = kParent:GetControl("IMG_TOP_LINE"..i)
			if false==line:IsNil() then
				line:Visible(false)
			end
		end
	end
end

function CostumeMixBtnInCashShop(kSelf)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end
	
	if kSelf:GetCheckState() ~= true then
		local kParent = kSelf:GetParent()
		HideUI_For_CashShopPart(kParent, true)
		
		local kWnd = kSelf:GetControl("IMG_ANI")
		if(nil ~= kWnd:IsNil()) then
			kWnd:Visible(false)
		end
		SetShopType(3)
		CostumeMixReqResultItemList()
		kSelf:CheckState(true)
		kSelf:LockClick(true)
		
		ChatWndActivate(false) -- 채팅창 감춤

		for i=1,7 do
			local line = kParent:GetControl("IMG_TOP_LINE"..i)
			if false==line:IsNil() then
				line:Visible(false)
			end
		end
		
		local kTemp = GetUIWnd("FRM_CASH_SHOP")
		if(true == kTemp:IsNil()) then return end
		kTemp = kTemp:GetControl("SFRM_COSTUME_MIX")
		if(true == kTemp:IsNil()) then return end
		kTemp = kTemp:GetControl("SFRM_EXPECT_RESULT")
		if(true == kTemp:IsNil()) then return end
		kTemp = kTemp:GetControl("CBTN_RESULT_TAB0")
		if(true == kTemp:IsNil()) then return end
		CostumeMix_OnClick_ResultItemTab(kTemp, 0)
		
	end
end

function MileageBtnInCashShop(kSelf)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end
	
	if kSelf:GetCheckState() ~= true then
		local kParent = kSelf:GetParent()
		HideUI_For_CashShopPart(kParent, false)

		SetShopType(1) 
		kSelf:CheckState(true)
		kSelf:LockClick(true)		
	end	
end

function GiftBagBtnInCashShop(kSelf)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end
	
	if kSelf:GetCheckState() ~= true then
		SetShopType(0)
		local kParent = kSelf:GetParent()
		if not kParent:IsNil() then
			local kBuildIdx = kSelf:GetBuildIndex()
			local kTopLine
			for i = 0, 7 do
				kTopLine = kParent:GetControl("IMG_TOP_LINE" .. i )
				if(false == kTopLine:IsNil()) then
					if i == 0 then
						kTopLine:Visible( false )
					else
						kTopLine:Visible( true )
					end
				end
			end
		end
		HideUI_For_CashShopPart(kParent, false)
		
		kSelf:CheckState(true)
		kSelf:LockClick(true)					
		
		TopCategoryOnClick(kParent, 7)

		CallChatWindow(ECS_CASHSHOP)
	end
end


function HideUI_For_CashShopPart(kParent, bOffAllBuyBtn)
	if(true == kParent:IsNil()) then return end
	
	-- 캐시탭
	local CashTab = kParent:GetControl("BTN_CASH_SHOP")
	if(false == CashTab:IsNil()) then 
		CashTab:LockClick(false)
		CashTab:CheckState(false)
		local kWnd = CashTab:GetControl("IMG_ANI")
		if(nil ~= kWnd:IsNil()) then
			kWnd:Visible(true)
		end
	end
	
	-- 캐시 가차탭
	local kGacha = kParent:GetControl("BTN_CASH_GACHA")
	if(false == kGacha:IsNil()) then 
		kGacha:LockClick(false)
		kGacha:CheckState(false)
		local kWnd = kGacha:GetControl("IMG_ANI")
		if(nil ~= kWnd:IsNil()) then
			kWnd:Visible(true)
		end	
	end
	
	-- 코스튬 조합탭
	local kCostumeMix = kParent:GetControl("BTN_COSTUME_MIX")
	if(false == kCostumeMix:IsNil()) then 
		kCostumeMix:LockClick(false)
		kCostumeMix:CheckState(false)
		local kWnd = kCostumeMix:GetControl("IMG_ANI")
		if(nil ~= kWnd:IsNil()) then
			kWnd:Visible(true)
		end	
	end
	
	-- 마일리지 탭	
	local MileageTab = kParent:GetControl("BTN_MILEAGE_SHOP")
	if(false == MileageTab:IsNil()) then 
		MileageTab:LockClick(false)
		MileageTab:CheckState(false)	
	end
	
	-- 미리 보기 초기화
	ClearPreviewEquip()
	
	-- 선물함
	local kGift = kParent:GetControl("CBTN_TAB7")
	if(false == kGift:IsNil()) then
	kGift:LockClick(false)
		kGift:CheckState(false)			
	end		
			
	local kBuildIdx = UISelf:GetBuildIndex()
	local kTopLine
	for i = 0, 7 do
		kTopLine = kParent:GetControl("IMG_TOP_LINE" .. i )
		if(false == kTopLine:IsNil()) then
			if i == 0 then
				kTopLine:Visible( false )
			else
				kTopLine:Visible( true )
			end
		end
	end
	
	ChatWndActivate(true)
	CostumeMix_HideSeletedCover()
	CostumeMix_ClearSlotInfo()
	
	-- 모두 구입 버튼 보이기 여부
	local kTempWnd = kParent:GetControl("SFRM_INFO")
	if(true == kTempWnd:IsNil()) then return end
	kTempWnd = kTempWnd:GetControl("SFRM_BODY_BG")
	if(true == kTempWnd:IsNil()) then return end
	kTempWnd = kTempWnd:GetControl("SFRM_SHADOW")
	if(true == kTempWnd:IsNil()) then return end		
	local kBtnAllBuy = kTempWnd:GetControl("BTN_BUY_ALL")
	if(true == kBtnAllBuy:IsNil()) then return end	
	kBtnAllBuy:Visible(not bOffAllBuyBtn)
end

function OnCall_CashShopTopCbtn(kSelf)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end
	
	local kParent = kSelf:GetParent()
		if not kParent:IsNil() then
		for i = 0, 7 do
			kTopLine = kParent:GetControl("IMG_TOP_LINE" .. i )
			if(false == kTopLine:IsNil()) then
				if 0 == i then							
					kTopLine:Visible( false )
				else
					kTopLine:Visible( true )
				end
			end
		end
	end
end

function OnForcus_CashShopTopCbtn(kSelf)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end
	
	if kSelf:GetCheckState() ~= true then
		kSelf:CheckState(true)
		kSelf:LockClick(true)
		UI_Click_CashShop_Tab(kSelf)
		
		local kParent = kSelf:GetParent()
		if not kParent:IsNil() then
			local kBuildIdx = kSelf:GetBuildIndex()
			local kTopLine
			for i = 0, 7 do
				kTopLine = kParent:GetControl("IMG_TOP_LINE" .. i )
				if(false == kTopLine:IsNil()) then
					if kBuildIdx == i then							
						kTopLine:Visible( false )
					else
						kTopLine:Visible( true )
					end
				end
			end
		end
	end
end

--  코스튬 조합 룰렛 부분(룰렛부분만 있음!) 시작 ------------------------------------------------------------------------------------------------------------------------
function CostumeMix_GetSlotMaxCnt()
	local kTemp = GetUIWnd("FRM_CASH_SHOP")
	if(true == kTemp:IsNil()) then return end
	kTemp = kTemp:GetControl("SFRM_COSTUME_MIX")
	if(true == kTemp:IsNil()) then return end	
	kTemp = kTemp:GetControl("BLD_RESOURCE_SLOT")
	if(true == kTemp:IsNil()) then return end
	return kTemp:GetBuildNumberCount()
end

function CostumeMix_OnClick_ResultItemTab(kSelf, iBuildIdx)
	if(nil == kSelf) then return end
	if(true == kSelf:IsNil()) then return end
	
	--local iBuildIdx = kSelf:GetBuildIndex() -- UI Build로 만들어지지 않은 '점수별 예상'탭이 4 idx
	
	local kParent = kSelf:GetParent()
	if(true == kParent:IsNil()) then return end
	
	--일반, 고급, 스페셜 탭
	for i = 0, 4 do
		if(i ~= iBuildIdx) then
			local kTabBtn = kParent:GetControl("CBTN_RESULT_TAB"..i)		
			if(false == kTabBtn:IsNil()) then
				kTabBtn:CheckState(false)
				kTabBtn:LockClick(false)
			end
		end
	end
	
	local kItemListWnd = kParent:GetControl("FRM_ITEM_LIST")
	if(true == kItemListWnd:IsNil()) then return end
	
	local kPreviewRoulletTbl = kParent:GetControl("FRM_PREVIEW_ROULLET_TBL")
	if(true == kPreviewRoulletTbl:IsNil()) then return end
	local bItemListView = true
	if(5 == iBuildIdx) then
		bItemListView = false
	end	
	
	kItemListWnd:Visible(bItemListView)		 	  -- 아이템 결과 리스트쪽 UI와
	kPreviewRoulletTbl:Visible(not bItemListView) -- 예상 룰렛 테이블 UI를 감추거나 보여준다
	
	-- 점수별 예상 탭
	local kPreviewRoulletTblTab = kParent:GetControl("CBTN_ROULLET_TBL_TAB")
	if(false == kPreviewRoulletTblTab:IsNil()) then 
		kPreviewRoulletTblTab:CheckState(not bItemListView)
		kPreviewRoulletTblTab:LockClick(not bItemListView)
	end
	
	-- 하위 단일 아이템 보이기
	if(0 == iBuildIdx) then
		CostumeMixShowPart(11)	-- 노멀 탭
	elseif(1 == iBuildIdx) then	
		CostumeMixShowPart(12)	-- 고급 탭
	elseif(2 == iBuildIdx) then
		CostumeMixShowPart(13)	-- 스페셜 탭
	elseif(3 == iBuildIdx) then
		CostumeMixShowPart(14)	-- 유물 탭
	elseif(4 == iBuildIdx) then
		CostumeMixShowPart(15)	-- 레젠드 탭
	elseif(5 == iBuildIdx) then
		-- Point 룰렛 테이블보기
	end
	
	kSelf:CheckState(true)
	kSelf:LockClick(true)
	
	-- itemPage 번호 리셋 해야함 
	CostumeMixShowItemPage(0)
	-- local kBldItemPage = kParent:GetControl("BLD_PAGE")
	-- local iPageCnt = kBldItemPage:GetBuildNumberCount()
	-- for i=0, 3 do
		-- local kPageBtn = kParent:GetControl("CBTN_PAGE"..i);
		-- if(false == kPageBtn:IsNil()) then 
			-- kPageBtn:ClearCustomData()
			-- kPageBtn:SetStaticText("")
			
		-- end
	-- end
	
end

function CostumeMix_SetPreviewRoulletTbl(kTblWnd, iLegendCnt, iArtifactCnt, iSpclCnt, iAdvCnt, bDisable)
	if(nil == kTblWnd) then return end
	if(true == kTblWnd:IsNil()) then return end
	
	local iNormal_Idx = 1
	local iAdvUV_Idx  = 2
	local iSpclUV_Idx = 3
	local iArtifactUV_Idx = 6
	local iLegendUV_Idx = 7
	--local i??UV_Idx = 4
	local iGrayColor = 5
	
	local i = 1
	for i = 1, 10 do
		local kTblParts = kTblWnd:GetControl("IMG_ROULLET_TBL"..i)
		if(false == kTblParts:IsNil()) then 			
			if(true == bDisable) then	-- 모두 회색 처리 라면
				kTblParts:SetStaticText("")
				kTblParts:SetUVIndex(iGrayColor)
			elseif(0 < iLegendCnt) then 		-- 전설 갯수
				kTblParts:SetStaticTextW(GetTT(790714))
				kTblParts:SetUVIndex(iLegendUV_Idx)
				iLegendCnt = iLegendCnt-1
			elseif(0 < iArtifactCnt) then 		-- 유물 갯수
				kTblParts:SetStaticTextW(GetTT(790713))
				kTblParts:SetUVIndex(iArtifactUV_Idx)
				iArtifactCnt = iArtifactCnt-1
			elseif(0 < iSpclCnt) then 		-- 스페셜 갯수
				kTblParts:SetStaticTextW(GetTT(790703))
				kTblParts:SetUVIndex(iSpclUV_Idx)
				iSpclCnt = iSpclCnt-1
			elseif(0 < iAdvCnt) then 	-- 고급 갯수
				kTblParts:SetStaticTextW(GetTT(790702))
				kTblParts:SetUVIndex(iAdvUV_Idx)
				iAdvCnt = iAdvCnt-1
			else						-- 일반 갯수
				kTblParts:SetStaticTextW(GetTT(790701))
				kTblParts:SetUVIndex(iNormal_Idx)
			end
		end
	end
end

function CostumeMix_InitMainUI(kWnd, iGrade)
	kWnd:GetControl("SFRM_EXPECT_RESULT"):GetControl("CBTN_RESULT_TAB3"):Visible(iGrade>3)
	kWnd:GetControl("SFRM_EXPECT_RESULT"):GetControl("CBTN_RESULT_TAB4"):Visible(iGrade>4)

	local kNormalPoint = kWnd:GetControl("SFRM_NOMAL_POINT")
	local kAdvPoint = kWnd:GetControl("SFRM_ADV_POINT")
	local kSpclPoint = kWnd:GetControl("SFRM_SPCL_POINT")
	local kArtPoint = kWnd:GetControl("SFRM_ARTIFACT_POINT")
	local kLegndPoint = kWnd:GetControl("SFRM_LEGND_POINT")

	--Tooltip
	--285~ 		+19	
	local iAdd = 2 * (5-iGrade)
	local iY = 285 + (19 * (5-iGrade) /2) - iAdd
	local kLoc = kNormalPoint:GetLocation()
	kLoc:SetY(iY)	iY = iY + (19+iAdd)		kNormalPoint:SetLocation(kLoc)
	kLoc:SetY(iY)	iY = iY + (19+iAdd)		kAdvPoint:SetLocation(kLoc)
	kLoc:SetY(iY)	iY = iY + (19+iAdd)		kSpclPoint:SetLocation(kLoc)
	kLoc:SetY(iY)	iY = iY + (19+iAdd)		kArtPoint:SetLocation(kLoc)			kArtPoint:Visible(iGrade>3)
	kLoc:SetY(iY)	iY = iY + (19+iAdd)		kLegndPoint:SetLocation(kLoc)		kLegndPoint:Visible(iGrade>4)

	local kEtcWnd = kWnd:GetControl("FRM_ETC")
	local kNormalNotice = kEtcWnd:GetControl("SFRM_NORMAL_COLOR_NOTICE")
	local kAdvNotice = kEtcWnd:GetControl("SFRM_ADV_COLOR_NOTICE")
	local kSpclNotice = kEtcWnd:GetControl("SFRM_SPCL_COLOR_NOTICE")
	local kArtNotice = kEtcWnd:GetControl("SFRM_ARTIFACT_COLOR_NOTICE")
	local kLegndNotice = kEtcWnd:GetControl("SFRM_LEGND_COLOR_NOTICE")

	--중앙왼쪽 상태표시
	--190~322	+33
	iY = 190 + (33 * (5-iGrade) /2)
	kLoc = kNormalNotice:GetLocation()
	kLoc:SetY(iY)	iY = iY + 33		kNormalNotice:SetLocation(kLoc)
	kLoc:SetY(iY)	iY = iY + 33		kAdvNotice:SetLocation(kLoc)
	kLoc:SetY(iY)	iY = iY + 33		kSpclNotice:SetLocation(kLoc)
	kLoc:SetY(iY)	iY = iY + 33		kArtNotice:SetLocation(kLoc)		kArtNotice:Visible(iGrade>3)
	kLoc:SetY(iY)	iY = iY + 33		kLegndNotice:SetLocation(kLoc)		kLegndNotice:Visible(iGrade>4)
end

function CostumeMix_SetPreviewRoulletTbl_MainUI(iLegendCnt, iArtifactCnt, iSpclCnt, iAdvCnt)	-- 화면 가운데 있는 룰렛 테이블UI 설정
	local kTemp = GetUIWnd("FRM_CASH_SHOP")
	if(true == kTemp:IsNil()) then return end
	kTemp = kTemp:GetControl("SFRM_COSTUME_MIX")
	if(true == kTemp:IsNil()) then return end	
	kTemp = kTemp:GetControl("FRM_ROULLET_TBL")
	if(true == kTemp:IsNil()) then return end
	CostumeMix_SetPreviewRoulletTbl(kTemp, iLegendCnt, iArtifactCnt, iSpclCnt, iAdvCnt, false)
end

function CostumeMix_DisablePreviewRoulletTbl_MainUI()	-- 화면 가운데 있는 룰렛 테이블UI Disable 처리
	local kTemp = GetUIWnd("FRM_CASH_SHOP")
	if(true == kTemp:IsNil()) then return end
	kTemp = kTemp:GetControl("SFRM_COSTUME_MIX")
	if(true == kTemp:IsNil()) then return end	
	kTemp = kTemp:GetControl("FRM_ROULLET_TBL")
	if(true == kTemp:IsNil()) then return end
	CostumeMix_SetPreviewRoulletTbl(kTemp, 0, 0, 0, 0, true)
end

function CostumeMix_SetPreviewRoulletTbl_UnderTab(iLegendCnt, iArtifactCnt, iSpclCnt, iAdvCnt) -- 결과 아이템쪽 점수별 예상 탭을 누르면 나오는 룰렛 테이블UI 설정
	local kTemp = GetUIWnd("FRM_CASH_SHOP")
	if(true == kTemp:IsNil()) then return end
	kTemp = kTemp:GetControl("SFRM_COSTUME_MIX")
	if(true == kTemp:IsNil()) then return end
	kTemp = kTemp:GetControl("SFRM_EXPECT_RESULT")
	if(true == kTemp:IsNil()) then return end
	kTemp = kTemp:GetControl("FRM_PREVIEW_ROULLET_TBL")
	if(true == kTemp:IsNil()) then return end
	kTemp = kTemp:GetControl("FRM_ROULLET_TBL")
	if(true == kTemp:IsNil()) then return end
	CostumeMix_SetPreviewRoulletTbl(kTemp, iLegendCnt, iArtifactCnt, iSpclCnt, iAdvCnt, false)
end

function CostumeMix_SetRoulletTbl(iIdx, iGrade) -- 코스튬 룰렛의 각 아이템 부분의 배경을 조정
	--ODS("Idx:"..iIdx.." Grade:"..iGrade.."\n", false, 912)
	if(4 < iGrade or 1 > iGrade) then return end
	local kTblWnd = GetUIWnd("FRM_COSTUMEMIX_ROULETTE")
	if(true == kTblWnd:IsNil()) then return end
		
	local kTblParts = kTblWnd:GetControl("IMG_ROULLET_TBL"..iIdx+1)	--UI가 1번 부터 시작이다;
	if(false == kTblParts:IsNil()) then		
		kTblParts:SetUVIndex(iGrade)
	end		
end

function CostumeMix_MaxRareItemCntOnPage()
	return 6
end

function CostumeMix_SetRareUIPos(iRareSetCnt, iCurrentPage )
	-- RareUI를 얻어올수 있게 준비 후
	local kCashShop = GetUIWnd("FRM_CASH_SHOP")
	if(true == kCashShop:IsNil()) then return end
	local kTemp = kCashShop:GetControl("SFRM_COSTUME_MIX")
	if(true == kTemp:IsNil()) then return end
	
	local kTemp = kTemp:GetControl("SFRM_EXPECT_RESULT")
	if(true == kTemp:IsNil()) then return end	
	local kItemList = kTemp:GetControl("FRM_ITEM_LIST")
	if(true == kItemList:IsNil()) then return end
		
	local kBldRareUI = kItemList:GetControl("BLD_RARE_ITEM")
	if(true == kBldRareUI:IsNil()) then return end
	
	local MAX_RARE_UI_NUM = CostumeMix_MaxRareItemCntOnPage() -- 표현 가능한 UI 갯수 	
	
	local kRareLBtn = kItemList:GetControl("BTN_RARE_L")
	if(true == kRareLBtn:IsNil()) then return end
	local kRareRBtn = kItemList:GetControl("BTN_RARE_R")
	if(true == kRareRBtn:IsNil()) then return end
	
	local iMaxRarePage = CostumeMix_GetMaxRareItemPage()
	-- local bRareMoveBtnVisible = false
	-- if(MAX_RARE_UI_NUM  == iRareSetCnt and iCurrentPage == iMaxRarePage) then 
	-- -- 한번에 표현가능 갯수가 최대이고, 레어그룹 아이콘이 한페이지에 보여지지 않을만큼 많다면
		-- bRareMoveBtnVisible = true
	-- end
	
	local bLBtnVisible = false
	local bRBtnVisible = false
	if(0 < iCurrentPage and iCurrentPage < iMaxRarePage) then
	--현재 페이지가 0보다 크고, max보다 작으면 좌우페이지 보여줌
		bLBtnVisible = true
		bRBtnVisible = true
	elseif (0 == iCurrentPage and 0 < iMaxRarePage) then
	--현재 페이지가 0이고, MaxPage가 0보다 클때  우측만 보여줌
		bRBtnVisible = true
	elseif (iMaxRarePage == iCurrentPage and 0 < iMaxRarePage) then
	--현재 페이지가 maxpage이고, maxpage가 0보다 크면 좌측만 보여줌
		bLBtnVisible = true		
	end
	kRareLBtn:Visible(bLBtnVisible)
	kRareRBtn:Visible(bRBtnVisible)
	
	if(0 >= iRareSetCnt) then return end
	if(MAX_RARE_UI_NUM < iRareSetCnt) then iRareSetCnt = MAX_RARE_UI_NUM end
	
	local iUnitWidth = kBldRareUI:GetBuildGab():GetX() --110	-- 박스 X크기 + X_GAB -- 각 레어아이템 UI간 X위치 이격거리 
	local iRareTotalWidth = iUnitWidth*iRareSetCnt
	local iRareCenterX = iRareTotalWidth/2
	
	local iSpaceWidth = iUnitWidth * MAX_RARE_UI_NUM --550	-- 가용한 크기
	local iSpaceCenterX = iSpaceWidth/2
	local iSpaceBeginX = 52  --공간 시작점	
	local fRareBoxBeginX = iSpaceCenterX - iRareCenterX + iSpaceBeginX
	
	local iBldIdx = 0
	for iBldIdx=0, MAX_RARE_UI_NUM-1 do 		
		local RareUIName = "FRM_RARE_ITEM"..iBldIdx
		local kRareGroupUI =  kItemList:GetControl(RareUIName)		
		if(false == kRareGroupUI:IsNil()) then
			if(iBldIdx < iRareSetCnt) then
				kRareGroupUI:Visible(true)
				local kPos = kRareGroupUI:GetLocation()
				--fRareBoxBeginX을 시작으로 iUnitWidth(이격 길이) 만큼 출력
				local iNewX = iUnitWidth*(iBldIdx) + fRareBoxBeginX
				kPos:SetX(iNewX)
				kRareGroupUI:SetLocation(kPos)
			else
				kRareGroupUI:Visible(false)
			end
		end
	end
end

function CostumeMix_OnClick_PageBtn(kWnd)	
	if(nil == kWnd) then return end
	if(true == kWnd:IsNil()) then return end
	
	local kParent = kWnd:GetParent()	
	if(true == kParent:IsNil()) then return end
	
	local iPageNo = kWnd:GetCustomDataAsInt()
	
	local i=0
	for i=0, 3 do
		local kPageBtn = kParent:GetControl("CBTN_PAGE"..i);
		if(false == kPageBtn:IsNil()) then 
			kPageBtn:CheckState(false)			
		end
	end
	
	kWnd:CheckState(true)
	kWnd:LockClick(true)
	CostumeMixShowItemPage(iPageNo)
end

function CostumeMix_DisableStartBtn(bDisable)
	local kCashShop = GetUIWnd("FRM_CASH_SHOP")
	if(true == kCashShop:IsNil()) then return end
	local kTemp = kCashShop:GetControl("SFRM_COSTUME_MIX")
	if(true == kTemp:IsNil()) then return end
	local kBtn = kTemp:GetControl("BTN_START")
	if(true == kBtn:IsNil()) then return end
	kBtn:Disable(bDisable)
	local kArrow = kBtn:GetControl("FRM_ARROW")
	if(true == kArrow:IsNil()) then return end
	kArrow:Visible(not bDisable)
end

function CostumeMix_CallRoulette()
	if(5 <= CostumeMixGetRemainReloadCnt()) then
		local strPrevState = g_CSRoulette["STATE"]
		CostumeMixRoulette_Init()
		if("WAIT_RESULT" == strPrevState) then
			g_CSRoulette["STATE"] = strPrevState
		end		
		ActivateUI("FRM_COSTUMEMIX_ROULETTE", true)
		return true
	end
	return false
end

function CostumeMixRoulette_Reload()
	local kRoulette = GetUIWnd("FRM_COSTUMEMIX_ROULETTE")
	if(nil == kRoulette) then return end
	if(true == kRoulette:IsNil()) then return end
	
	local kBtnReload = kRoulette:GetControl("BTN_RELOAD")
	if(nil == kBtnReload) then return end
	if(true == kBtnReload:IsNil()) then return end	
	
	if( "ROLLING" == g_CSRoulette["STATE"] or true == g_CSRoulette["CAN_CLOSE"]
		or "WAIT_RESULT" == g_CSRoulette["STATE"]) then
		-- 더이상 재설정 할수 없습니다 출력
		AddWarnDataTT(790627)
		return
	end
	if(false == CostumeMixDecRemainReloadCnt()) then
		--CommonMsgBoxByTextTable(790627,true)
		AddWarnDataTT(790628)
		return
	end	
	
	CostumeMixReqRoulletReload() -- 룰렛 재설정 요청
	PlaySoundByID("roulette_item")
	if(0 == CostumeMixGetRemainReloadCnt()) then 
		kBtnReload:Disable(true)
	end
end

function  CostumeMixRoulette_Close()
	if(true == g_CSRoulette["CAN_CLOSE"]) then
		Roulette_BtnChange("DEFAULT")
		CloseUI("FRM_COSTUMEMIX_ROULETTE")
		CostumeMixRoulette_SetCanClose(false)
		return true
	end
	return false
end

function CostumeMixRoulette_DisbleCloseBtn(bDisable)
	local kRoulette = GetUIWnd("FRM_COSTUMEMIX_ROULETTE")
	if(nil == kRoulette) then return end
	if(true == kRoulette:IsNil()) then return end
	
	local kBtnClose = kRoulette:GetControl("BTN_CLOSE")
	if(nil == kBtnClose) then return end
	if(true == kBtnClose:IsNil()) then return end
	kBtnClose:Disable(bDisable)
end

function CostumeMixRoulette_SetCanClose(bClose)
	g_CSRoulette["CAN_CLOSE"] = bClose
	CostumeMixRoulette_DisbleCloseBtn(not bClose)
end

function CostumeMixRoulette_Decision()
	local kRoulette = GetUIWnd("FRM_COSTUMEMIX_ROULETTE")
	if(nil == kRoulette) then return end
	if(true == kRoulette:IsNil()) then return end
	
	local kArrow = kRoulette:GetControl("FRM_ARROW")
	if(nil == kArrow) then return end
	if(true == kArrow:IsNil()) then return end
	
	local kBtn = kRoulette:GetControl("BTN_DECISION")
	if(nil == kBtn) then return end
	if(true == kBtn:IsNil()) then return end
	
	local kBtnStop = kRoulette:GetControl("BTN_DECISION_STOP")
	if(nil == kBtnStop) then return end
	if(true == kBtnStop:IsNil()) then return end
	
	local kBtnReload = kRoulette:GetControl("BTN_RELOAD")
	if(nil == kBtnReload) then return end
	if(true == kBtnReload:IsNil()) then return end	
	
	if("AGAIN" == g_CSRoulette["STATE"]) then
		CostumeMixRoulette_BtnChange("START")
		kArrow:SetRotationDeg(0)
		CostumeMixRoulette_Init()
		--cg_CSRoulette["CAN_CLOSE"] = false
		kBtnReload:Disable(false)
		return 
	else
		kBtnReload:Disable(true)
	end	
	
	if(true == CostumeMixRoulette_Close()) then
		return
	end
	
	if("WAIT_RESULT" == g_CSRoulette["STATE"]) then
		--결과를 기다리는 중이라면 버튼을 눌러도 소용이 없다
		return
	end
	local fForce = Roulette_GetCalcForce()
	local fCugDeg = kArrow:GetRotationDeg()
	if("STOP" == g_CSRoulette["STATE"]) then
		-- 멈춰있는 상태임으로 룰렛을 돌린다
		g_CSRoulette["STATE"] = "ROLLING"
		CostumeMixRoulette_BtnChange("ROLLING")
		g_CSRoulette["START_ACCTIME"] = GetAccumTime()
		kBtnStop:Disable(false)
	elseif("ROLLING" == g_CSRoulette["STATE"]) then
		-- 돌아가고 있는 상태라면, 서서히 멈추게 한다
		--ODS("각도"..fCugDeg.."\n", false, 912)
		--Roulette_Init()	-- 변수값 초기화
		g_CSRoulette["STATE"] = "WAIT_RESULT"
		g_CSRoulette["DUMMY_ROTATATION"] = fCugDeg
		kArrow:SetRotationDeg(fCugDeg)				
		g_CSRoulette["TEMP"] = fCugDeg
		kBtnStop:Disable(true)
	end
	g_CSRoulette["RADIAN"]  = math.asin(fCugDeg/fForce)
end

function CostumeMixRoulette_BtnChange(TypeStr)
	local kRoulette = GetUIWnd("FRM_COSTUMEMIX_ROULETTE")
	if(nil == kRoulette) then return end
	if(true == kRoulette:IsNil()) then return end
	
	local kArrow = kRoulette:GetControl("FRM_ARROW")
	if(nil == kArrow) then return end
	if(true == kArrow:IsNil()) then return end
	
	local kBtn = kRoulette:GetControl("BTN_DECISION")
	if(nil == kBtn) then return end
	if(true == kBtn:IsNil()) then return end
	
	local kBtnStop = kRoulette:GetControl("BTN_DECISION_STOP")
	if(nil == kBtnStop) then return end
	if(true == kBtnStop:IsNil()) then return end

	local bStartVisible = true
	local bStopVisible  = false

	if("ROLLING" == TypeStr) then
		bStartVisible = false
		bStopVisible  = true		
	elseif("AGAIN" == TypeStr) then
		bStartVisible = false
		bStopVisible  = false		
	end	
	kBtn:Visible(bStartVisible)
	kBtnStop:Visible(bStopVisible)	
end

function CostumeMixRoulette_Init()
	g_CSRoulette["DUMMY_ROTATATION"] = 0
	g_CSRoulette["RADIAN"] = 0
	g_CSRoulette["STATE"] = "STOP"	
	CostumeMixRoulette_SetCanClose(false)
	--g_CSRoulette["CAN_CLOSE"] = false

	g_CSRoulette["ACC_MOVED_DEG"] = 0
	g_CSRoulette["TEMP"] = 0
	g_CSRoulette["START_ACCTIME"] = 0
	--g_CSRoulette["RESULT_IDX"] = 0
end

function CostumeMixRoulette_OnTick(kWnd)
	if(nil == kWnd) then return end
	if(true == kWnd:IsNil()) then return end
	
	if("STOP" == g_CSRoulette["STATE"]) then 
	
	elseif("ROLLING" == g_CSRoulette["STATE"]) then 
		local fAccelDeg = 30 *20* GetFrameTime()
		g_CSRoulette["DUMMY_ROTATATION"] = g_CSRoulette["DUMMY_ROTATATION"] + fAccelDeg
		kWnd:SetRotationDeg(g_CSRoulette["DUMMY_ROTATATION"])
		-- 소리 낼 만큼 이동했는가 
		g_CSRoulette["ACC_MOVED_DEG"] = g_CSRoulette["ACC_MOVED_DEG"]+fAccelDeg
		if(g_CSRoulette["START_ACCTIME"] ~= 0 and
		   GetAccumTime() - g_CSRoulette["START_ACCTIME"] > g_CSRoulette["AUTO_STOP_TIME"]) then
			CostumeMixRoulette_Decision()
			PlaySound("../Sound/U_Sound/button(click).wav")
			g_CSRoulette["START_ACCTIME"] = 0
		end
	elseif("WAIT_RESULT" == g_CSRoulette["STATE"]) then 
		local fAccelDeg = 15*DEGTORAD
		local fMaxIncDeg = 90*DEGTORAD	
		-- 회전 값 증가 계산		
		local fForce = Roulette_GetCalcForce()		
		local fApplyDeg = fForce*math.sin(g_CSRoulette["RADIAN"])
		
		-- 소리 낼 만큼 이동했는가 		
		g_CSRoulette["ACC_MOVED_DEG"] = g_CSRoulette["ACC_MOVED_DEG"]+ math.abs(fApplyDeg-g_CSRoulette["TEMP"])
		g_CSRoulette["TEMP"] = fApplyDeg
		
		kWnd:SetRotationDeg(fApplyDeg)
		g_CSRoulette["RADIAN"] = g_CSRoulette["RADIAN"] + fAccelDeg * GetFrameTime()
		if(fMaxIncDeg < g_CSRoulette["RADIAN"]) then
			g_CSRoulette["RADIAN"] = fMaxIncDeg
			g_CSRoulette["STATE"] = "STOP"
			CostumeMixReqGetResultItem()	-- 미리 정해진 최종 아이템을 받도록 요청

			local kRoulette = GetUIWnd("FRM_COSTUMEMIX_ROULETTE")
			if(nil == kRoulette) then return end
			if(true == kRoulette:IsNil()) then return end				
			local kBtn = kRoulette:GetControl("BTN_DECISION")
			if(nil == kBtn) then return end
			if(true == kBtn:IsNil()) then return end

			CostumeMixRoulette_SetCanClose(true)
			--CostumeMixRoulette_BtnChange("CLOSE")
		end
	end
	CSGacha_TickSound()
end

function CostumeMix_HideSeletedCover()
	local kCashShop = GetUIWnd("FRM_CASH_SHOP")
	if(true == kCashShop:IsNil()) then return end
	local kInfo = kCashShop:GetControl("SFRM_INFO")
	if(true == kInfo:IsNil()) then return end
	
	local kBldMixCover = kInfo:GetControl("BLD_COSTUMEMIX_COVER")
	if(true == kBldMixCover:IsNil()) then return end
	
	local iMax = kBldMixCover:GetBuildNumberCount()

	local i=0
	for i=0,iMax do
		local kMixCover = kInfo:GetControl("FRM_COSTUMEMIX_COVER"..i)
		if(false == kMixCover:IsNil()) then 
			kMixCover:Visible(false)
		end
	end
end
-- 코스튬 조합 룰렛 부분 끝 ------------------------------------------------------------------------------------------------------------------------

function OnCloseCashShop()
	SetShopType(0)
	CostumeMix_HideSeletedCover()
	CostumeMix_ClearSlotInfo()
--	ChatWndActivate(true)
--	ChatSetPreset(1)
end

function CheckActionForEnterCashShop()
	local kActor =GetMyActor()
	if(kActor == nil or kActor:IsNil()) then
		return false
	end
	-- 아래 액션은 응답이 필요한 액션이므로, 액션이 끝날때까지 캐시샵에 들어가면 안된다
	local kAction = kActor:GetAction()
	if(kAction == nil or kAction:IsNil()) then
		return false
	end	
	if kActor:IsRidingPet() then
		g_ChatMgrClient:Notice_Show(GetTextW(451001), 1)
		return false
	end
	if kActor:GetAbil(AT_DUEL) > 0 then
		g_ChatMgrClient:Notice_Show(GetTextW(201209), 1)
		return false
	end
	local kActionID = kAction:GetID()
--	local kNextActionID = kAction:GetNextActionName()
--	ODS("액션:"..kActionID.."\n", false, 912)
--	ODS("다음:"..kNextActionID.."\n", false, 912)
	if("a_Death Snatch"				== kActionID
		or "a_Wire Action"			== kActionID
		or "a_Claw Fishing"			== kActionID
		or "a_Wire Action_Finish"	== kActionID
		or "a_Death Snatch_Finish"	== kActionID
		or "a_Claw Fishing_Finish"	== kActionID
		) then
		return false
	end	
	return true
end

function ResetCashShopTab()
	local kCashShopUI = GetUIWnd("FRM_CASH_SHOP")
	local tab0 = kCashShopUI:GetControl("CBTN_LEFT_TAB0")
	if false==tab0:IsNil() then
		tab0:CheckState(true)
		tab0:LockClick(true)
	end
	local tab1 = kCashShopUI:GetControl("CBTN_LEFT_TAB1")
	if false==tab1:IsNil() then
		tab1:CheckState(false)
		tab1:LockClick(false)
	end
	UISelf:CheckState(true)
	UISelf:LockClick(true)
	local best = UISelf:GetParent():GetControl("FRM_BEST")
	local rank = UISelf:GetParent():GetControl("FRM_RANK")
	if false==best:IsNil() then
		best:Visible(true)
	end
	if false==rank:IsNil() then
		rank:Visible(false)
	end
end