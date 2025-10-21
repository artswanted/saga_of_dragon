
g_TrueTowerLastClearFloor = {}
g_TrueTowerLastClearFloor["START_TIME"] = 0
g_TrueTowerLastClearFloor["BLINK_TIME"] = 0
g_TrueTowerLastClearFloor["BLINK_FLOOR"] = 0
g_TrueTowerLastClearFloor["LAST_VISIBLE"] = 0
g_TrueTowerLastClearFloor["ACCUMTIME"] = 0
g_TrueTowerLastClearFloor["BLINK_DELAY_TIME"] = 0
function InitTrueTWMiniMap(kUISelf)
	g_TrueTowerLastClearFloor = {}
	g_TrueTowerLastClearFloor["START_TIME"] = 0
	g_TrueTowerLastClearFloor["BLINK_TIME"] = 0
	g_TrueTowerLastClearFloor["BLINK_FLOOR"] = 0
	g_TrueTowerLastClearFloor["LAST_VISIBLE"] = 0
	g_TrueTowerLastClearFloor["ACCUMTIME"] = 0
	g_TrueTowerLastClearFloor["BLINK_DELAY_TIME"] = 0
	UpdateTrueTWMiniMap(kUISelf, 0)
end

function UpdateTrueTWMiniMap(kUISelf, iClearFloor)
	if kUISelf:IsNil() then return end
	
	g_TrueTowerLastClearFloor["START_TIME"] = GetAccumTime()
	g_TrueTowerLastClearFloor["BLINK_TIME"] = 3.0
	g_TrueTowerLastClearFloor["BLINK_FLOOR"] = iClearFloor
	g_TrueTowerLastClearFloor["LAST_VISIBLE"] = true
	g_TrueTowerLastClearFloor["ACCUMTIME"] = 0
	g_TrueTowerLastClearFloor["BLINK_DELAY_TIME"] = 0.03
	
	local iLastFloor = 10
	for iCur=1, iLastFloor do
		UpdateTrueTWClearFloorUI(kUISelf, iCur, iClearFloor >= iCur)
		
		local kCrystalUI = kUISelf:GetControl("FRM_MAP_LAST")
		if not kCrystalUI:IsNil() then
			kCrystalUI:Visible( 9 <= iClearFloor )
		end
		local kCursur = kUISelf:GetControl("FRM_MAP_OVER_" .. iCur)
		if not kCursur:IsNil() then
			kCursur:Visible(false)
		end
	end
end

function UpdateTrueTWClearFloorUI(kTopUI, iFloor, bClear)
	local kClearUI = kTopUI:GetControl("FRM_MAP_CLEAR_" .. iFloor)
	if not kClearUI:IsNil() then
		kClearUI:Visible( bClear )
	end
end

function OnTickTrueTWMiniMapBlink(kUISelf)
	if (GetAccumTime() - g_TrueTowerLastClearFloor["START_TIME"]) <= g_TrueTowerLastClearFloor["BLINK_TIME"] then
		g_TrueTowerLastClearFloor["ACCUMTIME"] = g_TrueTowerLastClearFloor["ACCUMTIME"] + GetFrameTime()
		if g_TrueTowerLastClearFloor["BLINK_DELAY_TIME"] <= g_TrueTowerLastClearFloor["ACCUMTIME"] then
			g_TrueTowerLastClearFloor["LAST_VISIBLE"] = not g_TrueTowerLastClearFloor["LAST_VISIBLE"]
			g_TrueTowerLastClearFloor["ACCUMTIME"] = 0
			g_TrueTowerLastClearFloor["BLINK_DELAY_TIME"] = g_TrueTowerLastClearFloor["BLINK_DELAY_TIME"] + 0.003
		end
		UpdateTrueTWClearFloorUI(kUISelf, g_TrueTowerLastClearFloor["BLINK_FLOOR"], g_TrueTowerLastClearFloor["LAST_VISIBLE"])
	else
		if g_TrueTowerLastClearFloor["START_TIME"] ~= 0 then
			UpdateTrueTWClearFloorUI(kUISelf, g_TrueTowerLastClearFloor["BLINK_FLOOR"], true)
			g_TrueTowerLastClearFloor["START_TIME"] = 0
			g_TrueTowerLastClearFloor["BLINK_TIME"] = 0
			g_TrueTowerLastClearFloor["BLINK_FLOOR"] = 0
		end
	end
end

function OnMouseOverTrueTowerFloor(kTopUI, iCurFloor, bClear)
	local kToolTipTable = {}
	kToolTipTable[1] = {["FLOOR"] = 600505, ["NAME"] = 6023900, ["ICON"] = "../Data/6_UI/icon/msBossIcon_31.tga"}
	kToolTipTable[2] = {["FLOOR"] = 600506, ["NAME"] = 6041500, ["ICON"] = "../Data/6_UI/icon/msBossIcon_32.tga"}
	kToolTipTable[3] = {["FLOOR"] = 600507, ["NAME"] = 6104400, ["ICON"] = "../Data/6_UI/icon/msBossIcon_33.tga"}
	kToolTipTable[4] = {["FLOOR"] = 600508, ["NAME"] = 6017700, ["ICON"] = "../Data/6_UI/icon/msBossIcon_34.tga"}
	kToolTipTable[5] = {["FLOOR"] = 600509, ["NAME"] = 6018900, ["ICON"] = "../Data/6_UI/icon/msBossIcon_35.tga"}
	kToolTipTable[6] = {["FLOOR"] = 600510, ["NAME"] = 6044600, ["ICON"] = "../Data/6_UI/icon/msBossIcon_36.tga"}
	kToolTipTable[7] = {["FLOOR"] = 600511, ["NAME"] = 6009900, ["ICON"] = "../Data/6_UI/icon/msBossIcon_39.tga"}
	kToolTipTable[8] = {["FLOOR"] = 600512, ["NAME"] = 6015900, ["ICON"] = "../Data/6_UI/icon/msBossIcon_37.tga"}
	kToolTipTable[9] = {["FLOOR"] = 600513, ["NAME"] = 6009600, ["ICON"] = "../Data/6_UI/icon/msBossIcon_38.tga"}
	kToolTipTable[10] = {["FLOOR"] = 600514, ["NAME"] = 6126100, ["ICON"] = "../Data/6_UI/icon/msBossIcon_39.tga"}
	for iCur=1, 10 do
		local kCursor = kTopUI:GetControl("FRM_MAP_OVER_" .. iCur)
		if not kCursor:IsNil() then
			kCursor:Visible(iCurFloor == iCur)
			
			if iCurFloor == iCur then
				local kPos = kCursor:GetLocation()
				kPos:SetY(kCursor:GetLocation():GetY())
				kPos:SetX(kCursor:GetTotalLocation():GetX())
				OnTrueTowerCallToolTip( iCur, kPos, bClear, kToolTipTable )
			end
		end
	end -- end for
end
function OnTrueTowerCallToolTip(iCurFloor, kPos, bClear, kToolTipTable)
	if nil == kToolTipTable[iCurFloor] then return end
	local kToolTip = ActivateUI("FRM_TRUETW_FLOOR_TOOLTIP")
	if not kToolTip:IsNil() then
		kPos:SetX(kPos:GetX() - kToolTip:GetSize():GetX())
		kToolTip:SetLocation(kPos)
		local kFloor = kToolTip:GetControl("FRM_FLOOR")
		if not kFloor:IsNil() then
			if bClear then
				local kTempStr = GetTT( kToolTipTable[iCurFloor]["FLOOR"] )
				kTempStr:Add( " " .. GetTT(600504):GetStr() )
				kFloor:SetStaticTextW( kTempStr )
			else
				kFloor:SetStaticTextW( GetTT( kToolTipTable[iCurFloor]["FLOOR"] ) )
			end
		end
		local kName = kToolTip:GetControl("FRM_NAME")
		if not kName:IsNil() then
			local kDefString = GetDefString( kToolTipTable[iCurFloor]["NAME"])
			if nil ~= kDefString then
				kName:SetStaticTextW( WideString(kDefString) )
			end
		end
		local kIconUI = kToolTip:GetControl("FRM_ICON")
		if not kIconUI:IsNil() then
			kIconUI:ChangeImage(kToolTipTable[iCurFloor]["ICON"], false)
			kIconUI:SetSizeScale(0.7)
		end
		local kClearImg = kToolTip:GetControl("FRM_CLEAR")
		if not kClearImg:IsNil() then
			kClearImg:Visible(bClear)
		end
	end
end
function OnMouseOutTrueTowerFloor(kUISelf)
	kUISelf:Visible(false)
	CloseUI("FRM_TRUETW_FLOOR_TOOLTIP")
end
function ON_CALL_FROMEVENTSCRIPT_TRUETW(iCurFloor)
	UpdateTrueTWMiniMap(GetUIWnd("FRM_TRUE_TW_MINIMAP"), iCurFloor)
end
