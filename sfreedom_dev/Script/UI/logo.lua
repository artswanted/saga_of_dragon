g_iContractMetCnt=0

function UI_startLogo()
	-- todo
end

function UI_updateLogo()
	-- todo
end

function UI_endLogo()
	CloseUI("FRM_LOGO_IMG")
	ActivateUI("Cursor")	
	if (GetLocale() == LOCALE.NC_TAIWAN and 0 == g_iContractMetCnt) then
		UI_showContract(true)
		g_iContractMetCnt=1
	else
		UI_showLoginForm(true)
	end
	
	if (GetLocale() == LOCALE.NC_JAPAN) then
		LoadBgSound("../BGM/char_Select.BGM.mp3", 1.0)
	else
		LoadBgSound("../BGM/Akia.BGM.mp3", 1.0)
	end

	PlayBgSound(0)
	g_bLogined_User = false
end
