function UI_showContract(bShow)
	if bShow == true then
		CallUI("LoginBg")
		CallUI("SFRM_CONTRACT")
		--CallUI("AuthFailDlg")
	else
		CloseUI("LoginBg")
		CloseUI("LoginDlg")
		CloseUI("AuthFailDlg")
		CloseUI("FRM_SERVER_BG")
		CloseUI("SFRM_CONTRACT")
	end	
end

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
		CloseUI("SFRM_CONTRACT")
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
function OnDisconnectFlush()
	--called c++
	--g_renderMan:CleanUpScene()
	g_bAddedUIScene = false
	--g_iPlayerLockCount = 0
	--g_renderMan:AddScene("s_login")
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
