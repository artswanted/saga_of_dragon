local kGraphicSet_Wnd = nil
local kColorSet_Wnd = nil
local kFullscreenSetWnd = nil
local kFlyToCameraWnd = nil
local kSoundSet_Wnd = nil
local kEtcSet_Wnd = nil

--////////////////////////////////////////////////////////////////////////////////////
--Tab Set Table
local TabSet = {}
TabSet[1] = {Graphic = true, Sound = false, Etc = false}
TabSet[2] = {Graphic = false, Sound = true, Etc = false}
TabSet[3] = {Graphic = false, Sound = false, Etc = true}
--////////////////////////////////////////////////////////////////////////////////////
function Option_TabChange(kTopSelf, iTabNo)
	if 0 == iTabNo then
		iTabNo = Config_GetValue(HEADKEY_USER_UI, SUBKEY_LAST_OPTION)
	end
	
	--SFRM_OPTION
	--	SFRM_OPTION_TITLE
	--		SFRM_OPTION1 / CBTN_GRAPHIC
	--		SFRM_OPTION2 / CBTN_SOUND
	--		SFRM_OPTION3 / CBTN_ETC
	local kTitleForm = kTopSelf:GetControl("SFRM_OPTION_TITLE")
	local kBtnGraphic = kTitleForm:GetControl("SFRM_OPTION1"):GetControl("CBTN_GRAPHIC")
	local kBtnSound = kTitleForm:GetControl("SFRM_OPTION2"):GetControl("CBTN_SOUND")
	local kBtnEtc = kTitleForm:GetControl("SFRM_OPTION3"):GetControl("CBTN_ETC")
	
	--SFRM_OPTION
	--	SFRM_GRAPHIC
	--	SFRM_SOUND
	--	SFRM_ETC
	local kFrmGraphic = kTopSelf:GetControl("SFRM_GRAPHIC")
	local kFrmSound = kTopSelf:GetControl("SFRM_SOUND")
	local kFrmEtc = kTopSelf:GetControl("SFRM_ETC")
	
	local bGraphic = TabSet[iTabNo]["Graphic"]
	local bSound = TabSet[iTabNo]["Sound"]
	local bEtc = TabSet[iTabNo]["Etc"]
	
	kBtnGraphic:CheckState(bGraphic)
	kBtnSound:CheckState(bSound)
	kBtnEtc:CheckState(bEtc)
	
	kFrmGraphic:Visible(bGraphic)
	kFrmSound:Visible(bSound)
	kFrmEtc:Visible(bEtc)
	
	SysConfig_SetValue(HEADKEY_USER_UI, SUBKEY_LAST_OPTION, iTabNo)
end

--////////////////////////////////////////////////////////////////////////////////////
local GraphicSet = {HEADKEY_GRAPHIC1, HEADKEY_GRAPHIC2, HEADKEY_GRAPHIC3}
--Option Set
local QuickSetBtnSet = {}
QuickSetBtnSet[1] = {true, false, false}
QuickSetBtnSet[2] = {false, true, false}
QuickSetBtnSet[3] = {false, false, true}
local SubSetName = {}
SubSetName[1] = SUBKEY_WORLD_QUALITY
SubSetName[2] = SUBKEY_VIEW_DISTANCE
SubSetName[3] = SUBKEY_WORLD_SHADER
SubSetName[4] = SUBKEY_ANTI_ALIAS
SubSetName[5] = SUBKEY_GLOW_EFFECT
SubSetName[6] = SUBKEY_TEXTURE_FILTERING
SubSetName[7] = SUBKEY_TEXTURE_RES
SubSetName[8] = SUBKEY_DISPLAY_OTHERPLAYER_DAMAGE
SubSetName[9] = SUBKEY_REFRASH_RATE_SYNC
SubSetName[10] = SUBKEY_DISPLAY_MYPLAYER_DAMAGE
--////////////////////////////////////////////////////////////////////////////////////
function Quick_Graphic_Set(kTopSelf, iSetNo)
	if nil == kTopSelf then return end
	if kTopSelf:IsNil() then return end
	
	if 0 == iSetNo then
		iSetNo = Config_GetValue(HEADKEY_OPTION, SUBKEY_GRAPHIC_SET)
		ODS( iSetNo .. "\n")
	end
	Config_SetValue(HEADKEY_OPTION, SUBKEY_GRAPHIC_SET, iSetNo)
	
	kGraphicSet_Wnd = kTopSelf--Temp Save
	
	for kKey, kVal in pairs(SubSetName) do
		Option_SubSet(kTopSelf, kKey, 2, false)
	end
	
	--FRM_OPTION_GRAPHIC
	--
	--	SFRM_HIGH / BTN_HIGH
	--	SFRM_LOW / BTN_LOW
	--	SFRM_CUSTOM / BTN_CUSTOM
	local kBtnHigh = kTopSelf:GetControl("SFRM_HIGH"):GetControl("BTN_HIGH")
	local kBtnLow = kTopSelf:GetControl("SFRM_LOW"):GetControl("BTN_LOW")
	local kBtnCustom = kTopSelf:GetControl("SFRM_CUSTOM"):GetControl("BTN_CUSTOM")
	
	local bHigh = QuickSetBtnSet[iSetNo][1]
	local bLow = QuickSetBtnSet[iSetNo][2]
	local bCustom = QuickSetBtnSet[iSetNo][3]
	
	kBtnHigh:CheckState(bHigh)
	kBtnLow:CheckState(bLow)
	kBtnCustom:CheckState(bCustom)
	
end

--////////////////////////////////////////////////////////////////////////////////////
--iSet�� 0�� ��, 1�� ��, 2�� �ʱⰪ
--bCustom�� true�� Quick_Graphic_Set(kTopSelf, 2)�� �����Ѵ�. 
function Option_SubSet(kTopSelf, iSubSetNo, iSet, bCustom)
	--FRM_OPTION_GRAPHIC
	--	FRM_DETAIL_X / BTN_ON
	--	FRM_DETAIL_X / BTN_OFF
	
	local bChanged = false
	
	kGraphicSet_Wnd = kTopSelf--Temp Save
	
	local iVal = Config_GetValue(HEADKEY_OPTION, SUBKEY_GRAPHIC_SET)--Current Graphic Set?
	local iSaved = Config_GetValue(GraphicSet[iVal], SubSetName[iSubSetNo])--0 or 1
	if 2 == iSet then--Load it from Current Setting
		iSet = iSaved
	end
	
	bChanged = iSet ~= iSaved
	
	if bCustom and bChanged then
		for kKey, kVal in pairs(SubSetName) do
			local iCurSet = Config_GetValue(GraphicSet[iVal], kVal)
			Config_SetValue(GraphicSet[3], kVal, iCurSet)
		end
		Quick_Graphic_Set(kTopSelf, 3)
		iVal = 3
	end
	if 3 == iVal then --Save It at Custome Setting
		Config_SetValue(GraphicSet[iVal], SubSetName[iSubSetNo], iSet)
	end
	
	
	local kBtnOn = kTopSelf:GetControl("FRM_DETAIL_" .. iSubSetNo):GetControl("BTN_ON")
	local kBtnOff = kTopSelf:GetControl("FRM_DETAIL_" .. iSubSetNo):GetControl("BTN_OFF")
	if 1 == iSet then
		kBtnOn:CheckState(true)
		kBtnOff:CheckState(false)
	elseif 0 == iSet then
		kBtnOn:CheckState(false)
		kBtnOff:CheckState(true)
	end
	
	if g_renderMan ~= nil then
		g_renderMan:SetGraphicOption(SubSetName[iSubSetNo], iSet)
	end
end

--////////////////////////////////////////////////////////////////////////////////////
local GameSubSetName = {}
GameSubSetName[1] = SUBKEY_FULL_SCREEN
GameSubSetName[2] = SUBKEY_FLY_TO_CAMERA
GameSubSetName[3] = SUBKEY_RESOLUTION_WIDTH
GameSubSetName[4] = SUBKEY_RESOLUTION_HEIGHT
GameSubSetName[5] = SUBKEY_NIGHT_MODE

function GetCurrentResolutionWidth()
	local kWidth = Config_GetValue(HEADKEY_GAME, GameSubSetName[3])
	return kWidth
end

function GetCurrentResolutionHeight()
	local kHeight = Config_GetValue(HEADKEY_GAME, GameSubSetName[4])
	return kHeight
end

function Option_Game_Subset(kTopSelf, iSubSetNo, iSet)
	--FRM_OPTION_GRAPHIC
	--	FRM_GAME_DETAIL_X / BTN_ON
	--	FRM_GAME_DETAIL_X / BTN_OFF
	
	if 0 == iSet then--Load it from Current Setting
		iSet = Config_GetValue(HEADKEY_GAME, GameSubSetName[iSubSetNo])
	end
    if(2 == iSubSetNo) then
		kFlyToCameraWnd = kTopSelf
	else
		kFullscreenSetWnd = kTopSelf
	end

	local kBtnOn = kTopSelf:GetControl("BTN_ON")
	local kBtnOff = kTopSelf:GetControl("BTN_OFF")
	if 1 == iSet then
		kBtnOn:CheckState(true)
		kBtnOff:CheckState(false)
	elseif 2 == iSet then
		kBtnOn:CheckState(false)
		kBtnOff:CheckState(true)
	end
	
	Config_SetValue(HEADKEY_GAME, GameSubSetName[iSubSetNo], iSet)
end

Create_Screen_Option_Texts = { [0] = 90004009, [1] = 90004010, [2] = 90004011 }
function Create_Screen_Option( kList )
	if kList:IsNil() then return end

	kList:ClearAllListItem()
	for i = 0, 2 do
		local kItem = kList:AddNewListItemChar( GetTextW(Create_Screen_Option_Texts[i]):GetStr() )
		kItem:GetWnd():SetCustomDataAsInt( i )
	end
end

function Option_Game_ScreenChange(iSet)
	if iSet < 0 then -- Load it from Current Setting
		iSet = Config_GetValue(HEADKEY_GAME, SUBKEY_FULL_SCREEN)
	end
	Config_SetValue(HEADKEY_GAME, SUBKEY_FULL_SCREEN, iSet)
	Config_ApplyConfig()
end

--////////////////////////////////////////////////////////////////////////////////////
local ColorSetTable = {}
ColorSetTable[1] = {true, false, false, false, false, false, false, false, false, false, false, false}
ColorSetTable[2] = {false, true, false, false, false, false, false, false, false, false, false, false}
ColorSetTable[3] = {false, false, true, false, false, false, false, false, false, false, false, false}
ColorSetTable[4] = {false, false, false, true, false, false, false, false, false, false, false, false}
ColorSetTable[5] = {false, false, false, false, true, false, false, false, false, false, false, false}
ColorSetTable[6] = {false, false, false, false, false, true, false, false, false, false, false, false}
ColorSetTable[7] = {false, false, false, false, false, false, true, false, false, false, false, false}
ColorSetTable[8] = {false, false, false, false, false, false, false, true, false, false, false, false}
ColorSetTable[9] = {false, false, false, false, false, false, false, false, true, false, false, false}
ColorSetTable[10] = {false, false, false, false, false, false, false, false, false, true, false, false}
ColorSetTable[11] = {false, false, false, false, false, false, false, false, false, false, true, false}
ColorSetTable[12] = {false, false, false, false, false, false, false, false, false, false, false, true}
function Option_ColorSet(kTopSelf, iSetNo)
	if nil == kTopSelf then return end
	if kTopSelf:IsNil() then return end
	if 0 == iSetNo then
		iSetNo = Config_GetValue(HEADKEY_OPTION, SUBKEY_THEME_COLOR_SET)
	end
	Config_SetValue(HEADKEY_OPTION, SUBKEY_THEME_COLOR_SET, iSetNo)
	UI_ColorSet(iSetNo)
	
	kColorSet_Wnd = kTopSelf --Temp save
	
	--FRM_OPTION_GRAPHIC
	--	BTN_COLOR_1
	--	BTN_COLOR_2
	--	BTN_COLOR_3
	--	BTN_COLOR_4
	local kBtnColor1 = kTopSelf:GetControl("BTN_COLOR_1")
	local kBtnColor2 = kTopSelf:GetControl("BTN_COLOR_2")
	local kBtnColor3 = kTopSelf:GetControl("BTN_COLOR_3")
	local kBtnColor4 = kTopSelf:GetControl("BTN_COLOR_4")
	local kBtnColor5 = kTopSelf:GetControl("BTN_COLOR_5")
	local kBtnColor6 = kTopSelf:GetControl("BTN_COLOR_6")
	local kBtnColor7 = kTopSelf:GetControl("BTN_COLOR_7")
	local kBtnColor8 = kTopSelf:GetControl("BTN_COLOR_8")
	local kBtnColor9 = kTopSelf:GetControl("BTN_COLOR_9")
	local kBtnColor10 = kTopSelf:GetControl("BTN_COLOR_10")
	local kBtnColor11 = kTopSelf:GetControl("BTN_COLOR_11")
	local kBtnColor12 = kTopSelf:GetControl("BTN_COLOR_12")
	kBtnColor1:CheckState(ColorSetTable[iSetNo][1])
	kBtnColor2:CheckState(ColorSetTable[iSetNo][2])
	kBtnColor3:CheckState(ColorSetTable[iSetNo][3])
	kBtnColor4:CheckState(ColorSetTable[iSetNo][4])
	kBtnColor5:CheckState(ColorSetTable[iSetNo][5])
	kBtnColor6:CheckState(ColorSetTable[iSetNo][6])
    kBtnColor7:CheckState(ColorSetTable[iSetNo][7])
	kBtnColor8:CheckState(ColorSetTable[iSetNo][8])
	kBtnColor9:CheckState(ColorSetTable[iSetNo][9])
	kBtnColor10:CheckState(ColorSetTable[iSetNo][10])
	kBtnColor11:CheckState(ColorSetTable[iSetNo][11])
	kBtnColor12:CheckState(ColorSetTable[iSetNo][12])
end

function Option_Default()
	Config_DefaultConfig() --Default Config
	
	Quick_Graphic_Set(kGraphicSet_Wnd, 0)
	Option_ColorSet(kColorSet_Wnd, 0)
	OnUpdate_Sound_ValueControl(kSoundSet_Wnd, 1)
	OnUpdate_Sound_ValueControl(kSoundSet_Wnd, 2)
	OnUpdate_OptionETC_State(kEtcSet_Wnd)
	Option_Game_ScreenChange(kFullscreenSetWnd, -1)
	Option_Game_Subset(kFlyToCameraWnd, 2, 0)
	UpdateEtcConfig()
end

function Config_Save_Extern()

	Quick_Graphic_Set(kGraphicSet_Wnd, 0)
	Option_ColorSet(kColorSet_Wnd, 0)
	OnUpdate_Sound_ValueControl(kSoundSet_Wnd, 1)
	OnUpdate_Sound_ValueControl(kSoundSet_Wnd, 2)
	OnUpdate_OptionETC_State(kEtcSet_Wnd)
	Config_ApplyConfig()-- Apply Config
	Config_Save(true)-- Save Config.xml and Send Packet to Server
	SetBuildNameBoardforOptionChange()

	local bWide = IsWideScreen()
	if true == bWide then
		g_kSelectCameraMode = g_kArraySelectCameraMode["WIDE"];
		--ODS("Set Wide Camera\n", false, 3851)
	else
		g_kSelectCameraMode = g_kArraySelectCameraMode["NORMAL"];
		--ODS("Set Normal Camera\n", false, 3851)
	end
	
	local iShowNavi = Config_GetValue(HEADKEY_ETC, SUBKEY_NAVIGATION)
	if 0 == iShowNavi then
		CloseUI("SFRM_NAVI_MAP")
	else
		if nil ~= g_world then
			if"w_char_sel" ~= g_world:GetID() then
				CallNaviMap_WorldMap()
			end
		end
	end
	
	CloseUI("SFRM_OPTION")
end

function Config_Cancel_Extern()
	Config_CancelConfig()
	Config_Save()
	local iSetNo = Config_GetValue(HEADKEY_OPTION, SUBKEY_THEME_COLOR_SET)
	UI_ColorSet(iSetNo)
	
	local iShowNavi = Config_GetValue(HEADKEY_ETC, SUBKEY_NAVIGATION)
	if 0 == iShowNavi then
		CloseUI("SFRM_NAVI_MAP")
	else
		if nil ~= g_world then
			if"w_char_sel" ~= g_world:GetID() then
				CallNaviMap_WorldMap()
			end
		end
	end
	
	ODS( "Cancel Config\n")
	
	Quick_Graphic_Set(kGraphicSet_Wnd, 0)
	Option_ColorSet(kColorSet_Wnd, 0)
	OnUpdate_Sound_ValueControl(kSoundSet_Wnd, 1)
	OnUpdate_Sound_ValueControl(kSoundSet_Wnd, 2)
	OnUpdate_OptionETC_State(kEtcSet_Wnd)
	UpdateEtcConfig()
	
	CloseUI("SFRM_OPTION")
end


function OnClick_OptionETC_State(kSelf, kSubkey)
	if kSelf == nil then return end
	if kSelf:IsNil() then return end
	
	if kSelf:GetCheckState() == false then
		kSelf:CheckState(true)
		Config_SetValue(HEADKEY_ETC, kSubkey, 1)
	else
		kSelf:CheckState(false)
		Config_SetValue(HEADKEY_ETC, kSubkey, 0)
	end
end
function OnClick_OptionETC_ComboState(kOnBtn, kOffBtn, kSubKey)
	if kOnBtn == nil or kOffBtn == nil then return end
	if kOnBtn:IsNil() or kOffBtn:IsNil() then return end
	
	if kOnBtn:GetCheckState() == false then
		kOnBtn:CheckState(true)
		kOffBtn:CheckState(false)
		Config_SetValue(HEADKEY_ETC, kSubKey, 1)
	else
		kOnBtn:CheckState(false)
		kOffBtn:CheckState(true)
		Config_SetValue(HEADKEY_ETC, kSubKey, 0)
	end
end

function OnCall_OptionETC_State(kSelf, kSubkey)
	kSelf:CheckState( Config_GetValue(HEADKEY_ETC, kSubkey) )
end

g_kEtcFormSet = {}
g_kEtcFormSet[1] =	{ ["FRM"] = "FRM_ETC_ROW1_COL1", ["SUBKEY"] = "DENY_CHAT_WHISPER" }
g_kEtcFormSet[2] =	{ ["FRM"] = "FRM_ETC_ROW1_COL2", ["SUBKEY"] = "DENY_CHAT_PARTY" }
g_kEtcFormSet[3] =	{ ["FRM"] = "FRM_ETC_ROW2_COL1", ["SUBKEY"] = "DENY_CHAT_FRIEND" }
g_kEtcFormSet[4] =	{ ["FRM"] = "FRM_ETC_ROW2_COL2", ["SUBKEY"] = "DENY_CHAT_GUILD" }
g_kEtcFormSet[5] =	{ ["FRM"] = "FRM_ETC_ROW3_COL1", ["SUBKEY"] = "DENY_TRADE" }
g_kEtcFormSet[6] =	{ ["FRM"] = "FRM_ETC_ROW3_COL2", ["SUBKEY"] = "DENY_PARTY" }
--g_kEtcFormSet[7] =	{ ["FRM"] = "FRM_ETC_ROW4_COL1", ["SUBKEY"] = "DENY_FRIEND" }
g_kEtcFormSet[7] =	{ ["FRM"] = "FRM_ETC_ROW4_COL1", ["SUBKEY"] = "DENY_GUILD" }
g_kEtcFormSet[8] =	{ ["FRM"] = "FRM_ETC_ROW4_COL2", ["SUBKEY"] = "DENY_DUEL" }
g_kEtcFormSet[9] =	{ ["FRM"] = "FRM_ETC_ROW5_COL1", ["SUBKEY"] = "SHOW_NAME_NPC" }
g_kEtcFormSet[10] =	{ ["FRM"] = "FRM_ETC_ROW5_COL2", ["SUBKEY"] = "SHOW_NAME_PC" }
g_kEtcFormSet[11] =	{ ["FRM"] = "FRM_ETC_ROW6_COL1", ["SUBKEY"] = "AUTOITEM_MONEY" }
g_kEtcFormSet[12] =	{ ["FRM"] = "FRM_ETC_ROW6_COL2", ["SUBKEY"] = "AUTOITEM_EQUIP" }
g_kEtcFormSet[13] =	{ ["FRM"] = "FRM_ETC_ROW7_COL1", ["SUBKEY"] = "AUTOITEM_CONSUME" }
g_kEtcFormSet[14] =	{ ["FRM"] = "FRM_ETC_ROW7_COL2", ["SUBKEY"] = "AUTOITEM_ETC" }
g_kEtcFormSet[15] = { ["FRM"] = "FRM_ETC_ROW8_COL1", ["SUBKEY"] = "BATTLEOP_DISPLAY_HPBAR" }
g_kEtcFormSet[16] = { ["FRM"] = "FRM_ETC_ROW9_COL1", ["SUBKEY"] = "OFF_HELP" }
g_kEtcFormSet[17] = { ["FRM"] = "FRM_ETC_ROW8_COL2", ["SUBKEY"] = "DISPLAY_HELMET" }
g_kEtcFormSet[18] = { ["FRM"] = "FRM_ETC_ROW10", ["BTN1"] = "BTN_ON", ["BTN2"] = "BTN_OFF", ["SUBKEY"] = "SHOW_ALL_QUEST" }
g_kEtcFormSet[19] = { ["FRM"] = "FRM_ETC_ROW8_COL3", ["SUBKEY"] = "DISPLAY_PET_MPBAR" }
g_kEtcFormSet[20] = { ["FRM"] = "FRM_ETC_ROW11_COL1", ["SUBKEY"] = SUBKEY_DISPLAY_ACHIEVE_STATUS }
g_kEtcFormSet[21] = { ["FRM"] = "FRM_ETC_ROW12_COL1", ["SUBKEY"] = "NOTIFY_FRIEND" }
g_kEtcFormSet[22] = { ["FRM"] = "FRM_ETC_ROW12_COL2", ["SUBKEY"] = "NOTIFY_COUPLE" }
g_kEtcFormSet[23] = { ["FRM"] = "FRM_ETC_ROW13_COL1", ["SUBKEY"] = "NOTIFY_GUILD" }
g_kEtcFormSet[24] = { ["FRM"] = "FRM_ETC_ROW14_COL1", ["SUBKEY"] = "NAVIGATION_VISIBLE" }
g_kEtcFormSet[25] = { ["FRM"] = "FRM_ETC_ROW15_COL1", ["SUBKEY"] = SUBKEY_SIMPLY_TOOLTIP }

function OnUpdate_OptionETC_State(kEtcForm)
	if kEtcForm == nil then return end
	if kEtcForm:IsNil() then return end
	
	kEtcSet_Wnd = kEtcForm
	
	for kKey, kVal in pairs(g_kEtcFormSet) do
		local kSetFrm = kEtcForm:GetControl(kVal["FRM"])
		if not kSetFrm:IsNil() then
			if nil == kVal["BTN1"] then
				-- Check Btn
				local kChkBtn = kSetFrm:GetControl("BTN_CHECK")
				if not kChkBtn:IsNil() then
					OnCall_OptionETC_State(kChkBtn, kVal["SUBKEY"])
				end
			else
				-- Combo Btn
				local kOnBtn = kSetFrm:GetControl( kVal["BTN1"] )
				local kOffBtn = kSetFrm:GetControl( kVal["BTN2"] )
				if not kOnBtn:IsNil() and not kOffBtn:IsNil() then
					local iCurSet = Config_GetValue(HEADKEY_ETC, kVal["SUBKEY"])
					kOnBtn:CheckState( 0 ~= iCurSet )
					kOffBtn:CheckState( 0 == iCurSet )
				end
			end
		end
	end
end

--��ũ�ѽ� ���� �� ����
--��Ʈ true ���尪 0
--�µ� false ���尪 revert
kSoundSubkeySet = {}
kSoundSubkeySet[1] = SUBKEY_BGM_VOLUME
kSoundSubkeySet[2] = SUBKEY_EFFECT_VOLUME
kSoundSetWnd = {}
kSoundSetWnd[1] = "HSCRL_BGM_VOLUME"
kSoundSetWnd[2] = "HSCRL_EFFECT_VOLUME"
kSoundSetChkBtn = {}
kSoundSetChkBtn[1] = "BTN_MUTE_BGM"
kSoundSetChkBtn[2] = "BTN_MUTE_EFFECT"

function OnUpdate_Sound_ValueControl(kTopSelf, iSoundSet)
	if nil == kTopSelf then ODS("[OnUpdate_Sound_ValueControl] nil == kTopSelf\n") return end
	if kTopSelf:IsNil() then ODS("[OnUpdate_Sound_ValueControl] kTopSelf:IsNil()\n") return end
	
	kSoundSet_Wnd = kTopSelf
	local kSubkey = kSoundSubkeySet[iSoundSet]
	if nil == kSubkey then return end
	local kWnd = kTopSelf:GetControl( kSoundSetWnd[iSoundSet] )
	if kWnd:IsNil() then return end
	--local kChkBtn = kTopSelf:GetControl( kSoundSetChkBtn[iSoundSet] )
	--if kChkBtn:IsNil() then return end
	
	local iVal = Config_GetValue(HEADKEY_SOUND, kSubkey)
	kWnd:SetScrollCur( iVal )
	if iSoundSet == 1 then
		SetBGMVolume((kWnd:GetScrollCur() / kWnd:GetScrollMax()), false)
	elseif iSoundSet == 2 then
		SetEffectVolume((kWnd:GetScrollCur() / kWnd:GetScrollMax()), false)
	end
	
	OnUpdate_SoundChkBtn(kTopSelf, iSoundSet, iVal)
end

function OnUpdate_SoundChkBtn(kTopWnd, iSoundSet, iValue)
	if nil == kTopWnd or kTopWnd:IsNil() then return end
	local kChkBtn = kTopWnd:GetControl( kSoundSetChkBtn[iSoundSet] )
	if kChkBtn:IsNil() then return end
	
	if 0 == iValue then
		kChkBtn:CheckState(true)
	else
		kChkBtn:CheckState(false)
	end
end

function OnUpdate_Sound_SaveValue(kTopWnd, iSoundSet, iValue)
	local kSubkey = kSoundSubkeySet[iSoundSet]
	if nil == kSubkey then return end
	Config_SetValue(HEADKEY_SOUND, kSubkey, iValue)
	
	OnUpdate_SoundChkBtn(kTopWnd, iSoundSet, iValue)
end

function OnClick_Sound_MuteButton(kTopWnd, iSoundSet)
	if nil == kTopWnd or kTopWnd:IsNil() then return end
	local kChkBtn = kTopWnd:GetControl(kSoundSetChkBtn[iSoundSet])
	if nil == kChkBtn or kChkBtn:IsNil() then return end
	if false == kChkBtn:GetCheckState() then
		OnUpdate_Sound_SaveValue(kTopWnd, iSoundSet, 0)
		kChkBtn:CheckState(true)
	else
		OnUpdate_Sound_SaveValue(kTopWnd, iSoundSet, 75)
		kChkBtn:CheckState(false)
	end
	OnUpdate_Sound_ValueControl(kTopWnd, iSoundSet)
end

function Option_OnKeySet_Cancel()
-- Option -> ChangeKeys -> Cancel
	Config_CancelKeySet()
	Config_Save(false)
	QuickSlotMakeName()	
	CloseUI("SFRM_KEYSET")
end

function Option_OnKeySet_Accept()
-- Option -> ChangeKeys -> Accept
	Config_ApplyKeySet()
	Config_Save(true)
	QuickSlotMakeName()
	CloseUI("SFRM_KEYSET")
end


function IsWideScreen()
	local kScreenSize = GetScreenSize()
	return lwIsWideResolution(kScreenSize:GetX(), kScreenSize:GetY())
end
