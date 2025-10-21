g_bNeedManyRealmUI = true
function UI_showLoginForm(bShow)
	if bShow == true then
		CallUI("LoginBg")
		CallUI("LoginDlg")
		--CallUI("AuthFailDlg")
	else
		CloseUI("LoginBg")
		CloseUI("LoginDlg")
		CloseUI("AuthFailDlg")
		CloseUI("FRM_SERVER_BG")
	end	
end

function OnClick_TryLogin(kSelf)
	local parent = kSelf:GetParent()
	local id_wnd = parent:GetControl("ID")
	local pw_wnd = parent:GetControl("PW")
	local save_wnd = parent:GetControl("CBTN_ID_SAVE")
	local save_wnd_pw = parent:GetControl("CBTN_PW_SAVE")

	local strID = id_wnd:GetEditText()
	local strPW = pw_wnd:GetEditText()
	local bCheck = save_wnd:GetCheckState()
	local bCheckPw = save_wnd_pw:GetCheckState()
	
	if 0 == strID:Length() then
		CommonMsgBox(GetTextW(108):GetStr(), true)
		return
	end
	if 0 == strPW:Length() then
		CommonMsgBox(GetTextW(109):GetStr(), true)
		return
	end
	
	ClearEditFocus()
	TryLogin(strID, strPW, true, bCheck, bCheckPw)
	kSelf:SetClose(true)
end

function Input_TryConnectToSS(packet)
	NetConnectToSwitch(packet)
	return true
end


--======================Login Window======================
g_kLoginDlg = {}
g_kLoginDlg["LastEdit"] = 0
function OnKey_Set_Tab(iVal)
	g_kLoginDlg["LastEdit"] = iVal
end

function OnKey_Login_ExternTab()
	local kTop = GetUIWnd("LoginDlg")
	local kIDWnd = GetUIWnd("LoginDlg"):GetControl("ID")
	local kPWWnd = GetUIWnd("LoginDlg"):GetControl("PW")
	
	if false == kIDWnd:IsFocus() and false == kPWWnd:IsFocus() then
		if 0 < kIDWnd:GetEditText():Length() then
			kPWWnd:SetEditFocus(true)
		else
			kIDWnd:SetEditFocus(true)
		end
		
		return true
	end
	
	return false
end

function OnKey_Login_Tab()
	if 0 == g_kLoginDlg.LastEdit then
		g_kLoginDlg.LastEdit = 2
	end
	
	local kTop = GetUIWnd("LoginDlg")
	local kIDWnd = GetUIWnd("LoginDlg"):GetControl("ID")
	local kPWWnd = GetUIWnd("LoginDlg"):GetControl("PW")
	
	if 2 == g_kLoginDlg.LastEdit then
		kIDWnd:SetEditFocus(true)
	elseif 1 == g_kLoginDlg.LastEdit then
		kPWWnd:SetEditFocus(true)
	end
end

function OnKey_Login_Enter(kSelf)
	if 0 == g_kLoginDlg.LastEdit then
		g_kLoginDlg.LastEdit = 2
	end
	
	local kTop = GetUIWnd("LoginDlg")
	local kIDWnd = GetUIWnd("LoginDlg"):GetControl("ID")
	local kPWWnd = GetUIWnd("LoginDlg"):GetControl("PW")
	
	if true == kSelf:IsFocus() then
		if 2 == g_kLoginDlg.LastEdit then
			--Login Check
			local kPW = kIDWnd:GetEditText()
			local kID = kPWWnd:GetEditText()
			if 0 < kPW:Length() and 0 < kID:Length() then --length check
				ClearEditFocus()
				SendScriptEvent("LoginDlg", "TRY_LOGIN", "ON_L_BTN_UP")
				return true
			end
		end
		OnKey_Login_Tab()
		return true
	end
	return false
end

function OnKey_Login_Esc()
	local kLoginDlg = GetUIWnd("LoginDlg")
	if kLoginDlg:IsNil() then ODS("Can't find LoginDlg\n") return end
	
	local kIdWnd = kLoginDlg:GetControl("ID")
	if kIdWnd:IsNil() then ODS("can't find LoginDlg/ID\n") return end
	kIdWnd:SetEditText("")
	
	local kPwWnd = kLoginDlg:GetControl("PW")
	if kPwWnd:IsNil() then ODS("can't find LoginDlg/PW\n") return end
	kPwWnd:SetEditText("")
	
	
	g_kLoginDlg.LastEdit = 0
	ClearEditFocus()
end

--========================== Flush Login Script ==========================
function OnDisconnectFlush(bClearAuthData)
	--called c++
	--g_renderMan:CleanUpScene()
	g_bAddedUIScene = false
	--g_renderMan:AddScene("s_login")
	if nil ~= bClearAuthData and true==bClearAuthData then
		DeleteSelectBasketItem(true)
	end
	UI_endLogo()
end

--========================== Channel ==========================
function OnClick_Channel_BackLogin(UIParent)
	DisconnectLoginServer()
	UIParent:Close()
	CloseUI("FRM_SERVER_BG")
	CloseUI("FRM_SV_CH_SELECT_TITLE")
	CloseUI("FRM_DEFAULT_CHANNEL")
	CallUI("LoginBg")
	CallUI("LoginDlg")
	return true
end

function OnCreate_RePos_ChannelCard(kWnd)
	if (nil == kWnd) then return end
	if (true == kWnd:IsNil()) then return end
	
	local kParentWnd = kWnd:GetParent()
	if (nil == kParentWnd) then return end
	if (true == kParentWnd:IsNil()) then return end
	
	local BldCnt = kWnd:GetBuildCount()
	local BldCntX = BldCnt:GetX()
	local Location = kWnd:GetLocation()	
	local HalfParentWidth = kParentWnd:GetWidth()/2	
	local HalfGabX = kWnd:GetBuildGab():GetX()/2
	-- 첫 렐름 카드의 위치를 구한다
	local FirstCardPosX = HalfParentWidth - HalfGabX * BldCntX
	Location:SetX(FirstCardPosX)
	kWnd:SetLocation(Location)
end

function OnBuild_Realm(kWnd, iRealmCnt)
	local index = kWnd:GetCustomDataAsShort()
	if(IsNeedManyRealmUI(iRealmCnt)) then
		kWnd:ChangeImage("../Data/6_ui/server/svBcd0"..(index)..".tga")
	else
		kWnd:ChangeImage("../Data/6_ui/server/svBcard0"..(index)..".tga")
	end
end

function ChangeRealmImgInChannelUI(kWnd,iRealmNo)
	if (nil == kWnd) then return end
	if (true == kWnd:IsNil()) then return end
	
	if( iRealmNo < 7) then
		kWnd:ChangeImage("../Data/6_ui/server/svCardBig.tga")
	elseif( iRealmNo < 13) then
		kWnd:ChangeImage("../Data/6_ui/server/svCardBig2.tga")
	elseif( iRealmNo < 19) then
		kWnd:ChangeImage("../Data/6_ui/server/svCardBig3.tga")
	end
	
	local UVIndex = iRealmNo % 6
	if(0 == UVIndex) then
		UVIndex = 6
	end 	
	kWnd:SetUVIndex(UVIndex)
end

function NewRealmMarkDisplay(kWnd)
	if (nil == kWnd) then return end
	if (true == kWnd:IsNil()) then return end
	local TimeCnt = kWnd:GetCustomDataAsFloat()
	TimeCnt= TimeCnt+GetFrameTime()
	if(0.1 < TimeCnt) then 
		TimeCnt = 0
		local CurIndex = kWnd:GetUVIndex() + 1
		CurIndex = CurIndex % kWnd:GetMaxUVIndex()	
		if(CurIndex == 0) then  CurIndex = 1 end	
		kWnd:SetUVIndex(CurIndex)
	end	
	kWnd:SetCustomDataAsFloat(TimeCnt)
end

function CheckRealmDisplayLimit(iRealmCnt)
	if( iRealmCnt <= 4) then
		return 4
	elseif(iRealmCnt <= 6)	then
		return 6
	elseif(iRealmCnt <= 8)	then
		return 8	
	else
		 if( iRealmCnt%2 == 0) then 	--짝수
			return iRealmCnt/2
		else							--홀수
			return iRealmCnt/2+1
		 end
	end
end

function UseContentsConfig()
-- 서비스중, 서비스 중지에 대한 Contents_Config.ini 정보를 따를것인지 여부
--	return true		-- 따름
	return false	-- 따르지 않음
end

function IsNeedManyRealmUI(iRealmCnt)
	 if(8 < iRealmCnt) then
		g_bNeedManyRealmUI = true
	 else
		g_bNeedManyRealmUI = false
	end
	return g_bNeedManyRealmUI
end