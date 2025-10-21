function UI_QuickSlotIcon_OnDisplay(wnd)
		local kMyPilot = g_pilotMan:GetPlayerPilot();
		if kMyPilot:IsNil() == true then
			wnd:SetSize( Point2( wnd:GetSize():GetX(), 0 ) )
			return
		end

		local kMyActor = kMyPilot:GetActor();
		if kMyActor:IsNil() == true then
			wnd:SetSize( Point2( wnd:GetSize():GetX(), 0 ) )
			return
		end

		local kIcon = wnd:GetParent();
		local wndImg = kIcon:GetControl("FRM_COOTIME_IMG")

		local iRemainSkillCoolTime = kMyActor:GetRemainSkillCoolTimeInQuickSlot(kIcon:IconKey());
		
		if iRemainSkillCoolTime>0 then
			
			if iRemainSkillCoolTime>10 then
				wnd:SetFontColorRGBA(255,204,51, 255);
			else
				wnd:SetFontColorRGBA(255,100,128, 255);
			end
			
			local t = 90002
			if iRemainSkillCoolTime >= 3600 then
				t = t + 2
				iRemainSkillCoolTime = iRemainSkillCoolTime/3600
				iRemainSkillCoolTime = math.floor(iRemainSkillCoolTime)
			elseif iRemainSkillCoolTime >= 60 then
				t = t + 1
				iRemainSkillCoolTime = iRemainSkillCoolTime/60
				iRemainSkillCoolTime = math.floor(iRemainSkillCoolTime)
			end
			
			local fRemainSkillCoolTime = kMyActor:GetRemainSkillCoolTimeInQuickSlotFloat(kIcon:IconKey());
			local fTotalCoolTime = kMyActor:GetSkillTotalCoolTimeInQuickSlot(kIcon:IconKey());
			local fCoolRate = fRemainSkillCoolTime/fTotalCoolTime*40
			wnd:SetStaticText(""..iRemainSkillCoolTime .. GetTextW(t):GetStr());
			if fCoolRate < 0 then
				fCoolRate = 0
			elseif fCoolRate > 40 then
				fCoolRate = 40
			end
	
			if wndImg:IsNil() == false then
				wndImg:SetLocation( Point2(wndImg:GetLocation():GetX(), 40 - fCoolRate ) )
				wndImg:SetImgSize( Point2( wndImg:GetImgSize():GetX(), fCoolRate ) )
			end
			
		else
			wndImg:SetImgSize( Point2( wndImg:GetSize():GetX(), 0 ) )
			wnd:SetStaticText("");
		end
end

g_kQuickSlot = {}
g_kQuickSlot["MinSlotNum"] = 0
g_kQuickSlot["MaxSlotNum"] = 7
function GetQuickSlotWnd(idx) --0~7
	--at now Quickslot is 8
	if idx < g_kQuickSlot["MinSlotNum"] or idx > g_kQuickSlot["MaxSlotNum"] then
		return false
	end
	
	local wndParent = GetUIWnd("FormHero")
	if wndParent:IsNil() then ODS("can't find FormHero\n") return false, nil end

	local wnd = GetUIWnd("QuickInv")--wndParent:GetControl("QuickInv")
	if wnd:IsNil() then ODS("can't find QuickInv\n") return false, nil end

	local kFrmBg = wnd:GetControl("FRM_BG")
	if kFrmBg:IsNil() then ODS("can't find FRM_BG\n") return false, nil end
	
	local kFrmIcon = kFrmBg:GetControl("FRM_ICON")
	if kFrmIcon:IsNil() then ODS("can't find FRM_ICON\n") return false, nil end
	
	local kIconWnd = kFrmIcon:GetControl("QUICK" .. idx)
	if kIconWnd:IsNil() then ODS("can't find QUICK"..idx.."\n") return false, nil end
	
	return true, kIconWnd
end

g_kQuickSlot["QuikSlotSet"] = {}
g_kQuickSlot["QuikSlotSet"][0] = 1016
g_kQuickSlot["QuikSlotSet"][1] = 1017
g_kQuickSlot["QuikSlotSet"][2] = 1018
g_kQuickSlot["QuikSlotSet"][3] = 1019
g_kQuickSlot["QuikSlotSet"][4] = 1030
g_kQuickSlot["QuikSlotSet"][5] = 1031
g_kQuickSlot["QuikSlotSet"][6] = 1032
g_kQuickSlot["QuikSlotSet"][7] = 1033
g_kQuickSlot["QuikSlotSet"][8] = 1002
g_kQuickSlot["QuikSlotSet"][9] = 1003
g_kQuickSlot["QuikSlotSet"][10] = 1004
g_kQuickSlot["QuikSlotSet"][11] = 1005
g_kQuickSlot["QuikSlotSet"][12] = 1006
g_kQuickSlot["QuikSlotSet"][13] = 1007
g_kQuickSlot["QuikSlotSet"][14] = 1008
g_kQuickSlot["QuikSlotSet"][15] = 1009
function QuickSlotDoAction(idx)
	local bRet, kIconWnd = GetQuickSlotWnd(idx)
	if bRet then
		kIconWnd:IconDoAction()
		return true
	end
	
	return false
end

function QuickSlotDoBuild(kSelf, iIndex)
	if kSelf:IsNil() then ODS("[QuickSlotDoBuild] kSelf is nil\n") return end
	--end set name
	local iUKey = g_kQuickSlot["QuikSlotSet"][ iIndex ]
	local iKeyNo = UKeyToKey(iUKey)--From Current Key Setting
	local kName = GetKeynoToName(iKeyNo)	
	kSelf:GetParent():SetStaticText( kName )
end

function QuickSlotMakeName()
	for iKey = g_kQuickSlot["MinSlotNum"], g_kQuickSlot["MaxSlotNum"] do
		local bRet, kIconWnd = GetQuickSlotWnd(iKey)
		if bRet then
			local iUKey = g_kQuickSlot["QuikSlotSet"][iKey]
			local iKeyNo = UKeyToKey(iUKey) --From Current Key Setting
			local kName = GetKeynoToName(iKeyNo)
			--g_kQuickSlot["QuikSlotSet"][iKey]
			kIconWnd:SetStaticText( kName )
			--kIconWnd:SetStaticText()
		else
			ODS("Can't make quickslot text\n")
			return false
		end
	end
	
	return true
end

g_kQuickSlot["KeyNoToName"] = {}
g_kQuickSlot["KeyNoToName"][0]="NOKEY"--
g_kQuickSlot["KeyNoToName"][1]="ESC"--
g_kQuickSlot["KeyNoToName"][2]="1"--
g_kQuickSlot["KeyNoToName"][3]="2"--
g_kQuickSlot["KeyNoToName"][4]="3"--
g_kQuickSlot["KeyNoToName"][5]="4"--
g_kQuickSlot["KeyNoToName"][6]="5"--
g_kQuickSlot["KeyNoToName"][7]="6"--
g_kQuickSlot["KeyNoToName"][8]="7"--
g_kQuickSlot["KeyNoToName"][9]="8"--
g_kQuickSlot["KeyNoToName"][10]="9"--
g_kQuickSlot["KeyNoToName"][11]="0"--
g_kQuickSlot["KeyNoToName"][12]="-"--/* - on main keyboard */
g_kQuickSlot["KeyNoToName"][13]="="--
g_kQuickSlot["KeyNoToName"][14]="BSpc"--/* backspace */
g_kQuickSlot["KeyNoToName"][15]="Tab"--
g_kQuickSlot["KeyNoToName"][16]="Q"--
g_kQuickSlot["KeyNoToName"][17]="W"--
g_kQuickSlot["KeyNoToName"][18]="E"--
g_kQuickSlot["KeyNoToName"][19]="R"--
g_kQuickSlot["KeyNoToName"][20]="T"--
g_kQuickSlot["KeyNoToName"][21]="Y"--
g_kQuickSlot["KeyNoToName"][22]="U"--
g_kQuickSlot["KeyNoToName"][23]="I"--
g_kQuickSlot["KeyNoToName"][24]="O"--
g_kQuickSlot["KeyNoToName"][25]="P"--
g_kQuickSlot["KeyNoToName"][26]="["--
g_kQuickSlot["KeyNoToName"][27]="]"--
g_kQuickSlot["KeyNoToName"][28]="Ret"--/* Enter on main keyboard */
g_kQuickSlot["KeyNoToName"][29]="LCtrl"--
g_kQuickSlot["KeyNoToName"][30]="A"--
g_kQuickSlot["KeyNoToName"][31]="S"--
g_kQuickSlot["KeyNoToName"][32]="D"--
g_kQuickSlot["KeyNoToName"][33]="F"--
g_kQuickSlot["KeyNoToName"][34]="G"--
g_kQuickSlot["KeyNoToName"][35]="H"--
g_kQuickSlot["KeyNoToName"][36]="J"--
g_kQuickSlot["KeyNoToName"][37]="K"--
g_kQuickSlot["KeyNoToName"][38]="L"--
g_kQuickSlot["KeyNoToName"][39]=";"--
g_kQuickSlot["KeyNoToName"][40]="'"--
g_kQuickSlot["KeyNoToName"][41]="`"--/* accent grave */
g_kQuickSlot["KeyNoToName"][42]="LSft"--
g_kQuickSlot["KeyNoToName"][43]="\\"--
g_kQuickSlot["KeyNoToName"][44]="Z"--
g_kQuickSlot["KeyNoToName"][45]="X"--
g_kQuickSlot["KeyNoToName"][46]="C"--
g_kQuickSlot["KeyNoToName"][47]="V"--
g_kQuickSlot["KeyNoToName"][48]="B"--
g_kQuickSlot["KeyNoToName"][49]="N"--
g_kQuickSlot["KeyNoToName"][50]="M"--
g_kQuickSlot["KeyNoToName"][51]=","--
g_kQuickSlot["KeyNoToName"][52]="."--/* . on main keyboard */
g_kQuickSlot["KeyNoToName"][53]="/"--/* / on main keyboard */
g_kQuickSlot["KeyNoToName"][54]="RSft"--
g_kQuickSlot["KeyNoToName"][55]="*"--/* * on numeric keypad */
g_kQuickSlot["KeyNoToName"][56]="LMenu"--/* left Alt */
g_kQuickSlot["KeyNoToName"][57]="Spc"--
g_kQuickSlot["KeyNoToName"][58]="Cap"--
g_kQuickSlot["KeyNoToName"][59]="F1"--
g_kQuickSlot["KeyNoToName"][60]="F2"--
g_kQuickSlot["KeyNoToName"][61]="F3"--
g_kQuickSlot["KeyNoToName"][62]="F4"--
g_kQuickSlot["KeyNoToName"][63]="F5"--
g_kQuickSlot["KeyNoToName"][64]="F6"--
g_kQuickSlot["KeyNoToName"][65]="F7"--
g_kQuickSlot["KeyNoToName"][66]="F8"--
g_kQuickSlot["KeyNoToName"][67]="F9"--
g_kQuickSlot["KeyNoToName"][68]="F10"--
g_kQuickSlot["KeyNoToName"][69]="NLck"--
g_kQuickSlot["KeyNoToName"][70]="SLck"--/* Scroll Lock */
g_kQuickSlot["KeyNoToName"][71]="NUMPAD7"--
g_kQuickSlot["KeyNoToName"][72]="NUMPAD8"--
g_kQuickSlot["KeyNoToName"][73]="NUMPAD9"--
g_kQuickSlot["KeyNoToName"][74]="SUBTRACT"--/* - on numeric keypad */
g_kQuickSlot["KeyNoToName"][75]="NUMPAD4"--
g_kQuickSlot["KeyNoToName"][76]="NUMPAD5"--
g_kQuickSlot["KeyNoToName"][77]="NUMPAD6"--
g_kQuickSlot["KeyNoToName"][78]="ADD"--/* + on numeric keypad */
g_kQuickSlot["KeyNoToName"][79]="NUMPAD1"--
g_kQuickSlot["KeyNoToName"][80]="NUMPAD2"--
g_kQuickSlot["KeyNoToName"][81]="NUMPAD3"--
g_kQuickSlot["KeyNoToName"][82]="NUMPAD0"--
g_kQuickSlot["KeyNoToName"][83]="DECIMAL"--/* . on numeric keypad */
g_kQuickSlot["KeyNoToName"][84]="OEM_102"--/* <> or \| on RT 102-key keyboard (Non-U.S.) */
g_kQuickSlot["KeyNoToName"][85]="F11"--
g_kQuickSlot["KeyNoToName"][86]="F12"--
g_kQuickSlot["KeyNoToName"][87]="F13"--/*     (NEC PC98) */
g_kQuickSlot["KeyNoToName"][88]="F14"--/*     (NEC PC98) */
g_kQuickSlot["KeyNoToName"][89]="F15"--/*     (NEC PC98) */
g_kQuickSlot["KeyNoToName"][90]="KANA"--/* (Japanese keyboard)    */
g_kQuickSlot["KeyNoToName"][91]="ABNT_C1"--/* /? on Brazilian keyboard */
g_kQuickSlot["KeyNoToName"][92]="CONVERT"--/* (Japanese keyboard)    */
g_kQuickSlot["KeyNoToName"][93]="NOCONVERT"--/* (Japanese keyboard)    */
g_kQuickSlot["KeyNoToName"][94]="YEN"--/* (Japanese keyboard)    */
g_kQuickSlot["KeyNoToName"][95]="ABNT_C2"--/* Numpad . on Brazilian keyboard */
g_kQuickSlot["KeyNoToName"][96]="NUMPADEQUALS"--/* = on numeric keypad (NEC PC98) */
g_kQuickSlot["KeyNoToName"][97]="PREVTRACK"--/* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
g_kQuickSlot["KeyNoToName"][98]="AT"--/*     (NEC PC98) */
g_kQuickSlot["KeyNoToName"][99]="COLON"--/*     (NEC PC98) */
g_kQuickSlot["KeyNoToName"][100]="UNDERLINE"--/*     (NEC PC98) */
g_kQuickSlot["KeyNoToName"][101]="KANJI"--/* (Japanese keyboard)    */
g_kQuickSlot["KeyNoToName"][102]="STOP"--/*     (NEC PC98) */
g_kQuickSlot["KeyNoToName"][103]="AX"--/*     (Japan AX) */
g_kQuickSlot["KeyNoToName"][104]="UNLABELED"--/*(J3100) */
g_kQuickSlot["KeyNoToName"][105]="NEXTTRACK"--/* Next Track */
g_kQuickSlot["KeyNoToName"][106]="NUMPADENTER"--/* Enter on numeric keypad */
g_kQuickSlot["KeyNoToName"][107]="RCONTROL"--
g_kQuickSlot["KeyNoToName"][108]="MUTE"--/* Mute */
g_kQuickSlot["KeyNoToName"][109]="CALCULATOR"--/* Calculator */
g_kQuickSlot["KeyNoToName"][110]="PLAYPAUSE"--/* Play / Pause */
g_kQuickSlot["KeyNoToName"][111]="MEDIASTOP"--/* Media Stop */
g_kQuickSlot["KeyNoToName"][112]="VOLUMEDOWN"--/* Volume - */
g_kQuickSlot["KeyNoToName"][113]="VOLUMEUP"--/* Volume + */
g_kQuickSlot["KeyNoToName"][114]="WEBHOME"--/* Web home */
g_kQuickSlot["KeyNoToName"][115]="NUMPADCOMMA"--/* , on numeric keypad (NEC PC98) */
g_kQuickSlot["KeyNoToName"][116]="DIVIDE"--/* / on numeric keypad */
g_kQuickSlot["KeyNoToName"][117]="SYSRQ"--
g_kQuickSlot["KeyNoToName"][118]="RMENU"--/* right Alt */
g_kQuickSlot["KeyNoToName"][119]="PAUSE"--/* Pause */
g_kQuickSlot["KeyNoToName"][120]="HOME"--/* Home on arrow keypad */
g_kQuickSlot["KeyNoToName"][121]="UP"--/* UpArrow on arrow keypad */
g_kQuickSlot["KeyNoToName"][122]="PRIOR"--/* PgUp on arrow keypad */
g_kQuickSlot["KeyNoToName"][123]="LEFT"--/* LeftArrow on arrow keypad */
g_kQuickSlot["KeyNoToName"][124]="RIGHT"--/* RightArrow on arrow keypad */
g_kQuickSlot["KeyNoToName"][125]="END"--/* End on arrow keypad */
g_kQuickSlot["KeyNoToName"][126]="DOWN"--/* DownArrow on arrow keypad */
g_kQuickSlot["KeyNoToName"][127]="NEXT"--/* PgDn on arrow keypad */
g_kQuickSlot["KeyNoToName"][128]="INSERT"--/* Insert on arrow keypad */
g_kQuickSlot["KeyNoToName"][129]="DELETE"--/* Delete on arrow keypad */
g_kQuickSlot["KeyNoToName"][130]="LWIN"--/* Left Windows key */
g_kQuickSlot["KeyNoToName"][131]="RWIN"--/* Right Windows key */
g_kQuickSlot["KeyNoToName"][132]="APPS"--/* AppMenu key */
g_kQuickSlot["KeyNoToName"][133]="POWER"--/* System Power */
g_kQuickSlot["KeyNoToName"][134]="SLEEP"--/* System Sleep */
g_kQuickSlot["KeyNoToName"][135]="WAKE"--/* System Wake */
g_kQuickSlot["KeyNoToName"][136]="WEBSEARCH"--/* Web Search */
g_kQuickSlot["KeyNoToName"][137]="WEBFAVORITES"--/* Web Favorites */
g_kQuickSlot["KeyNoToName"][138]="WEBREFRESH"--/* Web Refresh */
g_kQuickSlot["KeyNoToName"][139]="WEBSTOP"--/* Web Stop */
g_kQuickSlot["KeyNoToName"][140]="WEBFORWARD"--/* Web Forward */
g_kQuickSlot["KeyNoToName"][141]="WEBBACK"--/* Web Back */
g_kQuickSlot["KeyNoToName"][142]="MYCOMPUTER"--/* My Computer */
g_kQuickSlot["KeyNoToName"][143]="MAIL"--/* Mail */
g_kQuickSlot["KeyNoToName"][144]="MEDIASELECT"--/* Media Select */
g_kQuickSlot["KeyNoToName"][145]="TOTAL_COUNT"--
