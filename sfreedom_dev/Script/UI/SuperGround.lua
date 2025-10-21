function FRM_GOGO_LEFT()
	CallUI("FRM_GOGO_LEFT")
end

function FRM_GOGO_RIGHT()
	CallUI("FRM_GOGO_EVENT")
end

function CallSuperGroundBossTimer(iRemainTime)
	ODS("CallSuperGroundBossTimer\n", false, 1509)
	g_iMissionTotalTimeLimit = iRemainTime;
	g_fMissionStartTimeLimit = GetCurrentTimeInSec()
	local kTopWnd = ActivateUI("FRM_TIMELIMIT")
end
function CloseSuperGroundBossTimer()
	CloseUI("FRM_TIMELIMIT")
end
function CallSuperGroundFailedUI()
	--[[local kFailedWnd = ActivateUI("FRM_MISSION_STAGE_FAILED")
	if kFailedWnd:IsNil() then return end
	local kTextWnd = kFailedWnd:GetControl("FRM_TEXT")
	if kTextWnd:IsNil() then return end
	kTextWnd:SetStaticText("...")]]--
	ActivateUI("FRM_SUPERGROUND_TIME_FAILED")
end

function OnMouseOverSuperGroundFloor(kTopUI, kUISelf, iCurFloor, bClear)
	local kShortBtn = kTopUI:GetControl("BTN_SHORT")
	if kShortBtn:IsNil() then return end
	local iSuperGroundNo = kShortBtn:GetCustomDataAsInt()
	local kToolTipTable = {}
	if iSuperGroundNo == 1030300 then -- 대미궁
		kToolTipTable[1] = {["FLOOR"] = 799122, ["NAME"] = 7900151, ["ICON"] = "../Data/6_UI/icon/msBossIcon_75.tga"}
		kToolTipTable[2] = {["FLOOR"] = 799123, ["NAME"] = 7900167, ["ICON"] = "../Data/6_UI/icon/msBossIcon_76.tga"}
		kToolTipTable[3] = {["FLOOR"] = 799124, ["NAME"] = 7900147, ["ICON"] = "../Data/6_UI/icon/msBossIcon_77.tga"}
		kToolTipTable[4] = {["FLOOR"] = 799125, ["NAME"] = 7900142, ["ICON"] = "../Data/6_UI/icon/msBossIcon_78.tga"}
		kToolTipTable[5] = {["FLOOR"] = 799126, ["NAME"] = 7900145, ["ICON"] = "../Data/6_UI/icon/msBossIcon_79.tga"}
		kToolTipTable[6] = {["FLOOR"] = 799127, ["NAME"] = 7900140, ["ICON"] = "../Data/6_UI/icon/msBossIcon_80.tga"}
		kToolTipTable[7] = {["FLOOR"] = 799128, ["NAME"] = 7900178, ["ICON"] = "../Data/6_UI/icon/msBossIcon_81.tga"}
		kToolTipTable[8] = {["FLOOR"] = 799129, ["NAME"] = 7900143, ["ICON"] = "../Data/6_UI/icon/msBossIcon_82.tga"}
		kToolTipTable[9] = {["FLOOR"] = 799130, ["NAME"] = 7900177, ["ICON"] = "../Data/6_UI/icon/msBossIcon_83.tga"}
		kToolTipTable[10] = {["FLOOR"] = 799131, ["NAME"] = 7900144, ["ICON"] = "../Data/6_UI/icon/msBossIcon_84.tga"}
		kToolTipTable[11] = {["FLOOR"] = 799132, ["NAME"] = 7900138, ["ICON"] = "../Data/6_UI/icon/msBossIcon_85.tga"}
		kToolTipTable[12] = {["FLOOR"] = 799133, ["NAME"] = 7900168, ["ICON"] = "../Data/6_UI/icon/msBossIcon_86.tga"}
		kToolTipTable[13] = {["FLOOR"] = 799134, ["NAME"] = 7900164, ["ICON"] = "../Data/6_UI/icon/msBossIcon_87.tga"}
		kToolTipTable[14] = {["FLOOR"] = 799135, ["NAME"] = 7900162, ["ICON"] = "../Data/6_UI/icon/msBossIcon_88.tga"}
		kToolTipTable[15] = {["FLOOR"] = 799136, ["NAME"] = 7900148, ["ICON"] = "../Data/6_UI/icon/msBossIcon_89.tga"}
		kToolTipTable[16] = {["FLOOR"] = 799137, ["NAME"] = 7900166, ["ICON"] = "../Data/6_UI/icon/msBossIcon_90.tga"}
		kToolTipTable[17] = {["FLOOR"] = 799138, ["NAME"] = 7900164, ["ICON"] = "../Data/6_UI/icon/msBossIcon_91.tga"}
		kToolTipTable[18] = {["FLOOR"] = 799139, ["NAME"] = 7900176, ["ICON"] = "../Data/6_UI/icon/msBossIcon_92.tga"}
		kToolTipTable[19] = {["FLOOR"] = 799140, ["NAME"] = 7900139, ["ICON"] = "../Data/6_UI/icon/msBossIcon_93.tga"}
		kToolTipTable[20] = {["FLOOR"] = 799141, ["NAME"] = 7900150, ["ICON"] = "../Data/6_UI/icon/msBossIcon_94.tga"}
		kToolTipTable[21] = {["FLOOR"] = 799142, ["NAME"] = 7900173, ["ICON"] = "../Data/6_UI/icon/msBossIcon_95.tga"}
		kToolTipTable[22] = {["FLOOR"] = 799143, ["NAME"] = 7900163, ["ICON"] = "../Data/6_UI/icon/msBossIcon_96.tga"}
		kToolTipTable[23] = {["FLOOR"] = 799144, ["NAME"] = 7900181, ["ICON"] = "../Data/6_UI/icon/msBossIcon_97.tga"}
		kToolTipTable[24] = {["FLOOR"] = 799145, ["NAME"] = 7900152, ["ICON"] = "../Data/6_UI/icon/msBossIcon_98.tga"}
		kToolTipTable[25] = {["FLOOR"] = 799146, ["NAME"] = 7900154, ["ICON"] = "../Data/6_UI/icon/msBossIcon_99.tga"}
		kToolTipTable[26] = {["FLOOR"] = 799147, ["NAME"] = 7900141, ["ICON"] = "../Data/6_UI/icon/msBossIcon_100.tga"}
		kToolTipTable[27] = {["FLOOR"] = 799148, ["NAME"] = 7900202, ["ICON"] = "../Data/6_UI/icon/msBossIcon_101.tga"}
		kToolTipTable[28] = {["FLOOR"] = 799149, ["NAME"] = 7900182, ["ICON"] = "../Data/6_UI/icon/msBossIcon_102.tga"}
		kToolTipTable[29] = {["FLOOR"] = 799150, ["NAME"] = 7900146, ["ICON"] = "../Data/6_UI/icon/msBossIcon_103.tga"}
		kToolTipTable[30] = {["FLOOR"] = 799151, ["NAME"] = 7900174, ["ICON"] = "../Data/6_UI/icon/msBossIcon_104.tga"}
		kToolTipTable[31] = {["FLOOR"] = 799152, ["NAME"] = 7900155, ["ICON"] = "../Data/6_UI/icon/msBossIcon_105.tga"}
		kToolTipTable[32] = {["FLOOR"] = 799153, ["NAME"] = 7900180, ["ICON"] = "../Data/6_UI/icon/msBossIcon_106.tga"}
		kToolTipTable[33] = {["FLOOR"] = 799154, ["NAME"] = 7900157, ["ICON"] = "../Data/6_UI/icon/msBossIcon_107.tga"}
		kToolTipTable[34] = {["FLOOR"] = 799155, ["NAME"] = 7900156, ["ICON"] = "../Data/6_UI/icon/msBossIcon_108.tga"}
		kToolTipTable[35] = {["FLOOR"] = 799156, ["NAME"] = 7900171, ["ICON"] = "../Data/6_UI/icon/msBossIcon_109.tga"}
		kToolTipTable[36] = {["FLOOR"] = 799157, ["NAME"] = 7900159, ["ICON"] = "../Data/6_UI/icon/msBossIcon_110.tga"}
		kToolTipTable[37] = {["FLOOR"] = 799158, ["NAME"] = 7900179, ["ICON"] = "../Data/6_UI/icon/msBossIcon_111.tga"}
		kToolTipTable[38] = {["FLOOR"] = 799159, ["NAME"] = 7900165, ["ICON"] = "../Data/6_UI/icon/msBossIcon_112.tga"}
		kToolTipTable[39] = {["FLOOR"] = 799160, ["NAME"] = 6041500, ["ICON"] = "../Data/6_UI/icon/msBossIcon_113.tga"}
		kToolTipTable[40] = {["FLOOR"] = 799161, ["NAME"] = 6023900, ["ICON"] = "../Data/6_UI/icon/msBossIcon_114.tga"}
		kToolTipTable[41] = {["FLOOR"] = 799162, ["NAME"] = 6009900, ["ICON"] = "../Data/6_UI/icon/msBossIcon_115.tga"}
		kToolTipTable[42] = {["FLOOR"] = 799163, ["NAME"] = 6018900, ["ICON"] = "../Data/6_UI/icon/msBossIcon_116.tga"}
		kToolTipTable[43] = {["FLOOR"] = 799164, ["NAME"] = 7900160, ["ICON"] = "../Data/6_UI/icon/msBossIcon_117.tga"}
		kToolTipTable[44] = {["FLOOR"] = 799165, ["NAME"] = 6017700, ["ICON"] = "../Data/6_UI/icon/msBossIcon_118.tga"}
		kToolTipTable[45] = {["FLOOR"] = 799166, ["NAME"] = 7900201, ["ICON"] = "../Data/6_UI/icon/msBossIcon_119.tga"}
		kToolTipTable[46] = {["FLOOR"] = 799167, ["NAME"] = 6044600, ["ICON"] = "../Data/6_UI/icon/msBossIcon_120.tga"}
		kToolTipTable[47] = {["FLOOR"] = 799168, ["NAME"] = 6015900, ["ICON"] = "../Data/6_UI/icon/msBossIcon_121.tga"}
		kToolTipTable[48] = {["FLOOR"] = 799169, ["NAME"] = 6104400, ["ICON"] = "../Data/6_UI/icon/msBossIcon_122.tga"}
		kToolTipTable[49] = {["FLOOR"] = 799170, ["NAME"] = 6009600, ["ICON"] = "../Data/6_UI/icon/msBossIcon_123.tga"}
		kToolTipTable[50] = {["FLOOR"] = 799171, ["NAME"] = 7900184, ["ICON"] = "../Data/6_UI/icon/msBossIcon_124.tga"}
	elseif iSuperGroundNo == 2030501 or iSuperGroundNo == 2030502 or iSuperGroundNo == 2030503 then
		kToolTipTable[-24] = {["FLOOR"] = 799196, ["NAME"] = 7900185, ["ICON"] = "../Data/6_UI/icon/msBossIcon_74.tga"}
		kToolTipTable[-23] = {["FLOOR"] = 799195, ["NAME"] = 7900133, ["ICON"] = "../Data/6_UI/icon/msBossIcon_73.tga"}
		kToolTipTable[-22] = {["FLOOR"] = 799194, ["NAME"] = 7900118, ["ICON"] = "../Data/6_UI/icon/msBossIcon_72.tga"}
		kToolTipTable[-21] = {["FLOOR"] = 799193, ["NAME"] = 7900129, ["ICON"] = "../Data/6_UI/icon/msBossIcon_71.tga"}
		kToolTipTable[-20] = {["FLOOR"] = 799192, ["NAME"] = 7900117, ["ICON"] = "../Data/6_UI/icon/msBossIcon_70.tga"}
		kToolTipTable[-19] = {["FLOOR"] = 799191, ["NAME"] = 7900130, ["ICON"] = "../Data/6_UI/icon/msBossIcon_69.tga"}
		kToolTipTable[-18] = {["FLOOR"] = 799190, ["NAME"] = 7900134, ["ICON"] = "../Data/6_UI/icon/msBossIcon_68.tga"}
		kToolTipTable[-17] = {["FLOOR"] = 799189, ["NAME"] = 7900128, ["ICON"] = "../Data/6_UI/icon/msBossIcon_67.tga"}
		kToolTipTable[-16] = {["FLOOR"] = 799188, ["NAME"] = 7900132, ["ICON"] = "../Data/6_UI/icon/msBossIcon_66.tga"}
		kToolTipTable[-15] = {["FLOOR"] = 799187, ["NAME"] = 7900105, ["ICON"] = "../Data/6_UI/icon/msBossIcon_65.tga"}
		kToolTipTable[-14] = {["FLOOR"] = 799186, ["NAME"] = 7900115, ["ICON"] = "../Data/6_UI/icon/msBossIcon_64.tga"}
		kToolTipTable[-13] = {["FLOOR"] = 799185, ["NAME"] = 7900124, ["ICON"] = "../Data/6_UI/icon/msBossIcon_63.tga"}
		kToolTipTable[-12] = {["FLOOR"] = 799184, ["NAME"] = 7900122, ["ICON"] = "../Data/6_UI/icon/msBossIcon_62.tga"}
		kToolTipTable[-11] = {["FLOOR"] = 799183, ["NAME"] = 7900123, ["ICON"] = "../Data/6_UI/icon/msBossIcon_61.tga"}
		kToolTipTable[-10] = {["FLOOR"] = 799182, ["NAME"] = 7900126, ["ICON"] = "../Data/6_UI/icon/msBossIcon_60.tga"}
		kToolTipTable[-9] = {["FLOOR"] = 799181, ["NAME"] = 7900120, ["ICON"] = "../Data/6_UI/icon/msBossIcon_59.tga"}
		kToolTipTable[-8] = {["FLOOR"] = 799180, ["NAME"] = 7900125, ["ICON"] = "../Data/6_UI/icon/msBossIcon_58.tga"}
		kToolTipTable[-7] = {["FLOOR"] = 799179, ["NAME"] = 7900121, ["ICON"] = "../Data/6_UI/icon/msBossIcon_57.tga"}
		kToolTipTable[-6] = {["FLOOR"] = 799178, ["NAME"] = 7900127, ["ICON"] = "../Data/6_UI/icon/msBossIcon_56.tga"}
		kToolTipTable[-5] = {["FLOOR"] = 799177, ["NAME"] = 7900116, ["ICON"] = "../Data/6_UI/icon/msBossIcon_55.tga"}
		kToolTipTable[-4] = {["FLOOR"] = 799176, ["NAME"] = 7900131, ["ICON"] = "../Data/6_UI/icon/msBossIcon_54.tga"}
		kToolTipTable[-3] = {["FLOOR"] = 799175, ["NAME"] = 7900112, ["ICON"] = "../Data/6_UI/icon/msBossIcon_53.tga"}
		kToolTipTable[-2] = {["FLOOR"] = 799174, ["NAME"] = 7900119, ["ICON"] = "../Data/6_UI/icon/msBossIcon_52.tga"}
		kToolTipTable[-1] = {["FLOOR"] = 799173, ["NAME"] = 7900111, ["ICON"] = "../Data/6_UI/icon/msBossIcon_51.tga"}
		kToolTipTable[0] = {["FLOOR"] = 799172, ["NAME"] = 7900101, ["ICON"] = "../Data/6_UI/icon/msBossIcon_50.tga"}
		kToolTipTable[1] = {["FLOOR"] = 799122, ["NAME"] = 7900102, ["ICON"] = "../Data/6_UI/icon/msBossIcon_40.tga"}
		kToolTipTable[2] = {["FLOOR"] = 799123, ["NAME"] = 7900103, ["ICON"] = "../Data/6_UI/icon/msBossIcon_41.tga"}
		kToolTipTable[3] = {["FLOOR"] = 799124, ["NAME"] = 7900113, ["ICON"] = "../Data/6_UI/icon/msBossIcon_42.tga"}
		kToolTipTable[4] = {["FLOOR"] = 799125, ["NAME"] = 7900104, ["ICON"] = "../Data/6_UI/icon/msBossIcon_43.tga"}
		kToolTipTable[5] = {["FLOOR"] = 799126, ["NAME"] = 7900114, ["ICON"] = "../Data/6_UI/icon/msBossIcon_44.tga"}
		kToolTipTable[6] = {["FLOOR"] = 799127, ["NAME"] = 7900108, ["ICON"] = "../Data/6_UI/icon/msBossIcon_45.tga"}
		kToolTipTable[7] = {["FLOOR"] = 799128, ["NAME"] = 7900107, ["ICON"] = "../Data/6_UI/icon/msBossIcon_46.tga"}
		kToolTipTable[8] = {["FLOOR"] = 799129, ["NAME"] = 7900109, ["ICON"] = "../Data/6_UI/icon/msBossIcon_47.tga"}
		kToolTipTable[9] = {["FLOOR"] = 799130, ["NAME"] = 7900106, ["ICON"] = "../Data/6_UI/icon/msBossIcon_48.tga"}
		kToolTipTable[10] = {["FLOOR"] = 799131, ["NAME"] = 6074100, ["ICON"] = "../Data/6_UI/icon/msBossIcon_49.tga"}
	else
		return
	end
	
	
	local kPos = kUISelf:GetLocation()
	kPos:SetY(kUISelf:GetTotalLocation():GetY()-21)
	kPos:SetX(kUISelf:GetTotalLocation():GetX()-10)
	OnTrueTowerCallToolTip( iCurFloor, kPos, bClear, kToolTipTable )
end
function OnMouseOutSuperGroundFloor(kTopUI, kUISelf, iCurFloor)
	CloseUI("FRM_TRUETW_FLOOR_TOOLTIP")
end
function ON_CALL_FROMEVENTSCRIPT_TRUETW(iCurFloor)
	UpdateTrueTWMiniMap(GetUIWnd("FRM_TRUE_TW_MINIMAP"), iCurFloor)
end

--/////////////////////////////////////////////////////////////
g_OnTickSuperGroundCurFloorUI = 0
function Call_SuperGroundFloor(iSuperGroundNo, iCurFloor, bCallLong)
	local bReverseFloor = false
	local iLongSuperGroundNo = iSuperGroundNo
	local iMinFloor = 1
	local iMaxFloor = 50
	local iTotalFloor = 50
	local iLongBaseIndex = 0
	local DisplayAccumFunc = function (iCurFloor)  return iCurFloor end
	local kShortFloorUIImgName = "../Data/6_ui/TrueTW/ttNplayBg01st.tga"
	if iSuperGroundNo == 1030300 then -- 대미궁
		iMinFloor = 1
		iMaxFloor = 50
		iTotalFloor = 50
	elseif iSuperGroundNo == 2030501 then -- 반클리프 1~10층
		iMinFloor = 1
		iMaxFloor = 10
		iTotalFloor = 35
		iLongBaseIndex = 25
		kShortFloorUIImgName = "../Data/6_ui/TrueTW/ttNplayBg02st.tga"
	elseif iSuperGroundNo == 2030502 then -- 반클리프 지하1~지하10층
		bReverseFloor = true
		iLongSuperGroundNo = 2030501
		iLongBaseIndex = 25
		iMinFloor = 1
		iMaxFloor = 10
		iTotalFloor = 35
		DisplayAccumFunc = function (iCurFloor)  return -(iCurFloor) end
		kShortFloorUIImgName = "../Data/6_ui/TrueTW/ttNplayBg02st.tga"
	elseif iSuperGroundNo == 2030503 then -- 반클리프 지하11~지하25층
		bReverseFloor = true
		iLongSuperGroundNo = 2030501
		iLongBaseIndex = 25
		iMinFloor = 1
		iMaxFloor = 15
		iTotalFloor = 35
		DisplayAccumFunc = function (iCurFloor)  return -(10 + iCurFloor) end
		kShortFloorUIImgName = "../Data/6_ui/TrueTW/ttNplayBg02st.tga"
	else
		return
	end
	if bCallLong then
		local kTopWnd = ActivateUI("FRM_SUPER_CUR_FLOOR_LONG_" .. iLongSuperGroundNo)
		if kTopWnd:IsNil() then return end
		kTopWnd:SetCustomDataAsInt(iCurFloor)
		
		local kShortBtn = kTopWnd:GetControl("BTN_SHORT")
		if not kShortBtn:IsNil() then kShortBtn:SetCustomDataAsInt(iSuperGroundNo) end
		local kFloorUI = kTopWnd:GetControl("FRM_MY_FLOOR")
		if not kFloorUI:IsNil() then
			--local kTargetTime = (iMaxFloor - iCurFloor) * 0.1
			--g_OnTickSuperGroundCurFloorUI = kTargetTime
			--kFloorUI:SetCustomDataAsFloat(kTargetTime)
			local kLoc = kFloorUI:GetLocation()
			if bReverseFloor then
				kFloorUI:SetCustomDataAsInt( DisplayAccumFunc(iCurFloor)+1 )
				kLoc:SetX(-31) kLoc:SetY(31 + (iTotalFloor*10) - (iTotalFloor*10) * (DisplayAccumFunc(iCurFloor)+iLongBaseIndex+1)/iTotalFloor)
			else
				kFloorUI:SetCustomDataAsInt( iCurFloor )
				kLoc:SetX(-31) kLoc:SetY(31 + (iTotalFloor*10) - (iTotalFloor*10) * (iCurFloor+iLongBaseIndex)/iTotalFloor)
			end
			kFloorUI:SetLocation(kLoc)
			kFloorUI:SetStaticText( DisplayAccumFunc(iCurFloor) .. "F" )
		end
		for iCur = 0, 50 do
			local kClearFloor = kTopWnd:GetControl( "FRM_CLEAR_FLOOR" .. iCur )
			local iLocalFloor = iCur - iLongBaseIndex + 1
			if not kClearFloor:IsNil() then
				if bReverseFloor then
					iLocalFloor = iLocalFloor - 1
					kClearFloor:Visible( iLocalFloor <= DisplayAccumFunc(iMinFloor) and iLocalFloor > DisplayAccumFunc(iMaxFloor) and iLocalFloor > DisplayAccumFunc(iCurFloor) )
					kClearFloor:SetCustomDataAsInt( iLocalFloor + 1 )
				else
					kClearFloor:Visible( iLocalFloor >= iMinFloor and iLocalFloor < iMaxFloor and iLocalFloor < iCurFloor )
					kClearFloor:SetCustomDataAsInt( iLocalFloor )
				end
			end
			local kFloor =  kTopWnd:GetControl( "FRM_CURSOR_BACK_FLOOR" .. iCur )
			if not kFloor:IsNil() then
				if bReverseFloor then
					kFloor:SetCustomDataAsInt( iLocalFloor + 1 )
				else
					kFloor:SetCustomDataAsInt( iLocalFloor )
				end
			end
		end
	else
		local kTopWnd = ActivateUI("FRM_SUPER_CUR_FLOOR_SHORT")
		if kTopWnd:IsNil() then return end
		kTopWnd:ChangeImage( kShortFloorUIImgName )
		kTopWnd:SetCustomDataAsInt(iCurFloor)
		local kShortBtn = kTopWnd:GetControl("BTN_SHORT")
		if not kShortBtn:IsNil() then kShortBtn:SetCustomDataAsInt(iSuperGroundNo) end
		local kFloorUI = kTopWnd:GetControl("FRM_MY_FLOOR")
		if not kFloorUI:IsNil() then
			local kLoc = kFloorUI:GetLocation()
			if iMinFloor == iCurFloor then
				if bReverseFloor then kLoc:SetX(-36) kLoc:SetY(38) else kLoc:SetX(-36) kLoc:SetY(78) end
			elseif iMinFloor < iCurFloor and iMaxFloor > iCurFloor then
				kLoc:SetX(-36) kLoc:SetY(58)
			elseif iMaxFloor == iCurFloor then
				if bReverseFloor then kLoc:SetX(-36) kLoc:SetY(78) else kLoc:SetX(-36) kLoc:SetY(38) end
			else
			end
			kFloorUI:SetLocation( kLoc )
			kFloorUI:SetStaticText( DisplayAccumFunc(iCurFloor) .. "F" )
			if bReverseFloor then
				kFloorUI:SetCustomDataAsInt( DisplayAccumFunc(iCurFloor)+1 )
			else
				kFloorUI:SetCustomDataAsInt( iCurFloor )
			end
		end
		local iMaxWndCount = 2
		for iCur = 0, iMaxWndCount do
			local iFloor = iCurFloor
			if iMinFloor == iCurFloor then
				if bReverseFloor then iFloor = iMaxWndCount - iCur + 1 else iFloor = iCur + 1 end
			elseif iMinFloor < iCurFloor and iMaxFloor > iCurFloor then
				if bReverseFloor then iFloor = (iCurFloor+(iMaxWndCount-iCur-1)) else iFloor = (iCurFloor+(iCur-1)) end
			elseif iMaxFloor == iCurFloor then
				if bReverseFloor then iFloor = (iCurFloor+(iMaxWndCount-iCur-2)) else iFloor = (iCurFloor+(iCur-2)) end
			else
			end
			local kClearFloor = kTopWnd:GetControl( "FRM_CLEAR_FLOOR" .. iCur )
			if not kClearFloor:IsNil() then
				kClearFloor:Visible( iFloor < iCurFloor )
				if bReverseFloor then
					kClearFloor:SetCustomDataAsInt( DisplayAccumFunc(iFloor)+1 )
				else
					kClearFloor:SetCustomDataAsInt( iFloor )
				end
				kClearFloor:SetStaticText( DisplayAccumFunc(iFloor) .. "F" )
				if iCurFloor == iFloor then
					kClearFloor:SetFontColorRGBA(0xFF, 0xF5, 0x68, 0xFF)
				else
					kClearFloor:SetFontColorRGBA(0xFF, 0xFF, 0xFF, 0xFF)
				end
			end
			local kFloor = kTopWnd:GetControl( "FRM_CURSOR_BACK_FLOOR" .. iCur )
			if not kFloor:IsNil() then
				kFloor:Visible(true)
				if bReverseFloor then
					kFloor:SetCustomDataAsInt( DisplayAccumFunc(iFloor)+1 )
				else
					kFloor:SetCustomDataAsInt( iFloor )
				end
				kFloor:SetStaticText( DisplayAccumFunc(iFloor) .. "F" )
				if iCurFloor == iFloor then
					kFloor:SetFontColorRGBA(0xFF, 0xF5, 0x68, 0xFF)
				else
					kFloor:SetFontColorRGBA(0xFF, 0xFF, 0xFF, 0xFF)
				end
			end
		end
	end
end

function OnTickSuperGroundCurFloorUI(kSelf)
	--[[
	local kTopWnd = kSelf:GetParent()
	local iTargetFloor = kTopWnd:GetCustomDataAsInt()
	local kTargetFloorUI = kTopWnd:GetControl("FRM_CURSOR_BACK_FLOOR"..iTargetFloor)
	if kTargetFloorUI:IsNil() then return end
	local fPercent = Bias(g_OnTickSuperGroundCurFloorUI / kSelf:GetCustomDataAsFloat(), 0.18)
	if 0 < g_OnTickSuperGroundCurFloorUI then
		g_OnTickSuperGroundCurFloorUI = g_OnTickSuperGroundCurFloorUI - GetFrameTime()
	else
		g_OnTickSuperGroundCurFloorUI = 0
		fPercent = 0
	end
	local kTargetPos = kTargetFloorUI:GetLocation()
	local kTargetY = kTargetPos:GetY() - 44
	local iLocalCur = math.floor(kTargetY * fPercent / 10) + iTargetFloor
	kTargetPos:SetY( 32 + kTargetY - kTargetY * fPercent )
	kTargetPos:SetX( kSelf:GetLocation():GetX() )
	kSelf:SetLocation(kTargetPos)
	kSelf:SetStaticText( iLocalCur .. "F" )]]
end
function OnTickSuperGroundRemainMonUI(kSelf)
	local kTopWnd = kSelf:GetParent()
	local kPos = kTopWnd:GetControl("FRM_MY_FLOOR"):GetLocation()
	kPos:SetY(kPos:GetY() + 26)
	kPos:SetX(kPos:GetX() - 37)
	kSelf:SetLocation(kPos)
	local kCountFrm = kSelf:GetControl("FRM_COUNT")
	if not kCountFrm:IsNil() then
		kCountFrm:SetStaticText( GetSG_RemainMonsterCount() )
	end
end

function OnCall_SGBossName(kSelf)
	-- Screen Size
 	SCREEN_SIZE_SetPosCenterX( kSelf, 0 )
	SCREEN_SIZE_SetPosCenterY( kSelf, 0 )
	SCREEN_SIZE_SetSize(kSelf)
	SCREEN_SIZE_SetSizeX( kSelf:GetControl("FRM_BLACK_TOP") )
	SCREEN_SIZE_SetImgSizeX( kSelf:GetControl("FRM_BLACK_TOP") )
	SCREEN_SIZE_SetSizeX( kSelf:GetControl("FRM_BLACK_BOTTOM") )
	SCREEN_SIZE_SetImgSizeX( kSelf:GetControl("FRM_BLACK_BOTTOM") )
	SCREEN_SIZE_SetSize( kSelf:GetControl("FRM_BLACK_BG") )
	SCREEN_SIZE_SetImgSize( kSelf:GetControl("FRM_BLACK_BG") )
	SCREEN_SIZE_SetPosCenterY( kSelf:GetControl("FRM_SG_BOSS_NAME"), -231 )
	SCREEN_SIZE_SetPosCenterY( kSelf:GetControl("FRM_SG_BOSS_IMG"), 21 )
end
function OnTick_SGBossName(kSelf)
	local iCallHpBarUI = kSelf:GetCustomDataAsInt()
	g_fSuperGroundBossNameUIAccumTime = g_fSuperGroundBossNameUIAccumTime + GetFrameTime()
	local iStepNo = 1
	local fStepTime1 = 1.0
	local fStepTime2 = 0.7
	local fStepTime3 = 2.0
	local fStepTime4 = 0.7
	local fStepTime5 = 0.7
	local fCurEleapsedPercent = 0.0
	if fStepTime1 > g_fSuperGroundBossNameUIAccumTime then
		iStepNo = 1
		fCurEleapsedPercent = g_fSuperGroundBossNameUIAccumTime / fStepTime1
	elseif fStepTime1 <= g_fSuperGroundBossNameUIAccumTime and fStepTime1+fStepTime2 > g_fSuperGroundBossNameUIAccumTime then
		iStepNo = 2
		fCurEleapsedPercent = (g_fSuperGroundBossNameUIAccumTime-fStepTime1) / fStepTime2
	elseif fStepTime1+fStepTime2 <= g_fSuperGroundBossNameUIAccumTime and fStepTime1+fStepTime2+fStepTime3 > g_fSuperGroundBossNameUIAccumTime then
		iStepNo = 3
		fCurEleapsedPercent = (g_fSuperGroundBossNameUIAccumTime-fStepTime1-fStepTime2) / fStepTime3
	elseif fStepTime1+fStepTime2+fStepTime3 <= g_fSuperGroundBossNameUIAccumTime and fStepTime1+fStepTime2+fStepTime3+fStepTime4 > g_fSuperGroundBossNameUIAccumTime then
		iStepNo = 4
		fCurEleapsedPercent = (g_fSuperGroundBossNameUIAccumTime-fStepTime1-fStepTime2-fStepTime3) / fStepTime4
	elseif fStepTime1+fStepTime2+fStepTime3+fStepTime4 <= g_fSuperGroundBossNameUIAccumTime and fStepTime1+fStepTime2+fStepTime3+fStepTime4+fStepTime5 > g_fSuperGroundBossNameUIAccumTime then
		iStepNo = 5
		fCurEleapsedPercent = (g_fSuperGroundBossNameUIAccumTime-fStepTime1-fStepTime2-fStepTime3-fStepTime4) / fStepTime5
	else
		iStepNo = 6
	end
	if iStepNo >= 1 and iStepNo <= 4 then
		CloseUI("FRM_MAIN_GAUGEBAR")
		if g_iSuperGroundBossNameUILastStep == 1 and iStepNo == 2 then
		elseif g_iSuperGroundBossNameUILastStep == 2 and iStepNo == 3 then
			g_pilotMan:GetPlayerActor():AttachSound(0, "CrossBlade", 0, 0, 0)
		elseif g_iSuperGroundBossNameUILastStep == 3 and iStepNo == 4 then
			g_pilotMan:GetPlayerActor():AttachSound(0, "ForgetPain", 0, 0, 0)
		end
	elseif iStepNo == 5 then
		if g_iSuperGroundBossNameUILastStep == 4 and iStepNo == 5 then
			--UI_CallMovieScene(false,2.0);
			--g_world:SetShowWorldFocusFilterColorAlpha(0xFF000000,0.5,0,1,true,true)
			g_bEnableShortCutKey = true
		end
	else
		UIOn()
		kSelf:Close()
		if 0 ~= iCallHpBarUI then
			ActivateUI("FRM_MAIN_GAUGEBAR")
			ActivateUI("FRM_BOSS_ATTR_INFO")
		end
		ActivateUI("FRM_WARNING")
	end
	g_iSuperGroundBossNameUILastStep = iStepNo
	
	local kNameWnd = kSelf:GetControl("FRM_SG_BOSS_NAME")
	local iScreenWidth = GetScreenSize():GetX() -- Screen Width
	if not kNameWnd:IsNil() then
		local kLoc = kNameWnd:GetLocation()
		local iBaseSizeX = kNameWnd:GetSize():GetX()
		local iBaseX = (iScreenWidth - iBaseSizeX) / 2
		if iStepNo == 2 then
			kLoc:SetX( iBaseX - ((iBaseSizeX+iBaseX)*(1.0-Bias(fCurEleapsedPercent, 0.18))) )
		elseif iStepNo == 3 then
			kLoc:SetX( iBaseX )
		elseif iStepNo == 4 then
			kLoc:SetX( iBaseX + ((iScreenWidth-iBaseX)*Bias(fCurEleapsedPercent, 0.18)) )
		else
			kLoc:SetX( -iBaseSizeX )
		end
		kNameWnd:SetLocation(kLoc)
	end
	local kImgWnd = kSelf:GetControl("FRM_SG_BOSS_IMG")
	if not kImgWnd:IsNil() then
		local kLoc = kImgWnd:GetLocation()
		local iBaseSizeX = kNameWnd:GetSize():GetX()
		local iBaseX = (iScreenWidth - iBaseSizeX) / 2
		if iStepNo == 2 then
			kLoc:SetX( iBaseX + ((iScreenWidth-iBaseX)*(1.0-Bias(fCurEleapsedPercent, 0.18))) )
		elseif iStepNo == 3 then
			kLoc:SetX( iBaseX )
		elseif iStepNo == 4 then
			kLoc:SetX( iBaseX - ((iBaseSizeX+iBaseX)*Bias(fCurEleapsedPercent, 0.18)) )
		else
			kLoc:SetX( iScreenWidth )
		end
		kImgWnd:SetLocation(kLoc)
	end
	local kTopBlackBG = kSelf:GetControl("FRM_BLACK_TOP")
	if not kTopBlackBG:IsNil() then
		local iMaxSize = 75
		local kImgSize = kTopBlackBG:GetImgSize()
		if iStepNo == 1 then
			kImgSize:SetY( iMaxSize * fCurEleapsedPercent )
		elseif iStepNo >= 2 and iStepNo <= 4 then
			kImgSize:SetY( iMaxSize )
		elseif iStepNo == 5 then
			kImgSize:SetY( iMaxSize * (1 - fCurEleapsedPercent) )
		else
			kImgSize:SetY( 0 )
		end
		kTopBlackBG:SetImgSize( kImgSize )
	end
	local kBottomBlackBG = kSelf:GetControl("FRM_BLACK_BOTTOM")
	if not kBottomBlackBG:IsNil() then
		local iMaxSize = 100
		local iBaseY = GetScreenSize():GetY()
		local kImgSize = kBottomBlackBG:GetImgSize()
		local kPos = kBottomBlackBG:GetLocation()
		if iStepNo == 1 then
			kImgSize:SetY( iMaxSize * fCurEleapsedPercent )
			kPos:SetY( iBaseY - iMaxSize * fCurEleapsedPercent )
		elseif iStepNo >= 2 and iStepNo <= 4 then
			kImgSize:SetY( iMaxSize )
		elseif iStepNo == 5 then
			kImgSize:SetY( iMaxSize * (1 - fCurEleapsedPercent) )
			kPos:SetY( iBaseY - iMaxSize * (1 - fCurEleapsedPercent) )
		else
			kImgSize:SetY( 0 )
			kPos:SetY( iBaseY )
		end
		kBottomBlackBG:SetImgSize( kImgSize )
		kBottomBlackBG:SetLocation( kPos )
	end
end
function CALL_SG_BOSS_NAMED_UI(iNo)
	local kTable = {}
	kTable[1] = {["GROUND_NAME"] = 851001, ["BOSS_NAME"] = 850001, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg001.dds", ["CALL_HP_UI"] = 1}
	kTable[2] = {["GROUND_NAME"] = 851002, ["BOSS_NAME"] = 850002, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg002.dds", ["CALL_HP_UI"] = 1}
	kTable[3] = {["GROUND_NAME"] = 851003, ["BOSS_NAME"] = 850003, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg003.dds", ["CALL_HP_UI"] = 1}
	kTable[4] = {["GROUND_NAME"] = 851004, ["BOSS_NAME"] = 850004, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg004.dds", ["CALL_HP_UI"] = 1}
	kTable[5] = {["GROUND_NAME"] = 851005, ["BOSS_NAME"] = 850005, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg005.dds", ["CALL_HP_UI"] = 1}
	kTable[6] = {["GROUND_NAME"] = 851006, ["BOSS_NAME"] = 850006, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg006.dds", ["CALL_HP_UI"] = 1}
	kTable[7] = {["GROUND_NAME"] = 851007, ["BOSS_NAME"] = 850007, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg007.dds", ["CALL_HP_UI"] = 1}
	kTable[8] = {["GROUND_NAME"] = 851008, ["BOSS_NAME"] = 850008, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg008.dds", ["CALL_HP_UI"] = 1}
	kTable[9] = {["GROUND_NAME"] = 851009, ["BOSS_NAME"] = 850009, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg009.dds", ["CALL_HP_UI"] = 1}
	kTable[10] = {["GROUND_NAME"] = 851010, ["BOSS_NAME"] = 850010, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg010.dds", ["CALL_HP_UI"] = 1}
	kTable[11] = {["GROUND_NAME"] = 851011, ["BOSS_NAME"] = 850011, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg011.dds", ["CALL_HP_UI"] = 1}
	kTable[12] = {["GROUND_NAME"] = 851012, ["BOSS_NAME"] = 850012, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg012.dds", ["CALL_HP_UI"] = 1}
	kTable[13] = {["GROUND_NAME"] = 851013, ["BOSS_NAME"] = 850013, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg013.dds", ["CALL_HP_UI"] = 1}
	kTable[14] = {["GROUND_NAME"] = 851014, ["BOSS_NAME"] = 850014, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg014.dds", ["CALL_HP_UI"] = 1}
	kTable[15] = {["GROUND_NAME"] = 851015, ["BOSS_NAME"] = 850015, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg015.dds", ["CALL_HP_UI"] = 1}
	kTable[16] = {["GROUND_NAME"] = 851016, ["BOSS_NAME"] = 850016, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg016.dds", ["CALL_HP_UI"] = 1}
	kTable[17] = {["GROUND_NAME"] = 851017, ["BOSS_NAME"] = 850017, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg017.dds", ["CALL_HP_UI"] = 1}
	kTable[18] = {["GROUND_NAME"] = 851018, ["BOSS_NAME"] = 850018, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg018.dds", ["CALL_HP_UI"] = 1}
	kTable[19] = {["GROUND_NAME"] = 851019, ["BOSS_NAME"] = 850019, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg019.dds", ["CALL_HP_UI"] = 1}
	kTable[20] = {["GROUND_NAME"] = 851020, ["BOSS_NAME"] = 850020, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg020.dds", ["CALL_HP_UI"] = 1}
	kTable[21] = {["GROUND_NAME"] = 851021, ["BOSS_NAME"] = 850021, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg021.dds", ["CALL_HP_UI"] = 1}
	kTable[22] = {["GROUND_NAME"] = 851022, ["BOSS_NAME"] = 850022, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg022.dds", ["CALL_HP_UI"] = 1}
	kTable[23] = {["GROUND_NAME"] = 851023, ["BOSS_NAME"] = 850023, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg023.dds", ["CALL_HP_UI"] = 1}
	kTable[24] = {["GROUND_NAME"] = 851024, ["BOSS_NAME"] = 850024, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg024.dds", ["CALL_HP_UI"] = 1}
	kTable[25] = {["GROUND_NAME"] = 851025, ["BOSS_NAME"] = 850025, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg025.dds", ["CALL_HP_UI"] = 1}
	kTable[26] = {["GROUND_NAME"] = 851026, ["BOSS_NAME"] = 850026, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg026.dds", ["CALL_HP_UI"] = 1}
	kTable[27] = {["GROUND_NAME"] = 851027, ["BOSS_NAME"] = 850027, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg027.dds", ["CALL_HP_UI"] = 1}
	kTable[28] = {["GROUND_NAME"] = 851028, ["BOSS_NAME"] = 850028, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg028.dds", ["CALL_HP_UI"] = 1}
	kTable[29] = {["GROUND_NAME"] = 851029, ["BOSS_NAME"] = 850029, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg029.dds", ["CALL_HP_UI"] = 1}
	kTable[30] = {["GROUND_NAME"] = 851030, ["BOSS_NAME"] = 850030, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg030.dds", ["CALL_HP_UI"] = 1}
	kTable[31] = {["GROUND_NAME"] = 851031, ["BOSS_NAME"] = 850031, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg031.dds", ["CALL_HP_UI"] = 1}
	kTable[32] = {["GROUND_NAME"] = 851032, ["BOSS_NAME"] = 850032, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg032.dds", ["CALL_HP_UI"] = 1}
	kTable[33] = {["GROUND_NAME"] = 851033, ["BOSS_NAME"] = 850033, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg033.dds", ["CALL_HP_UI"] = 1}
	kTable[34] = {["GROUND_NAME"] = 851034, ["BOSS_NAME"] = 850034, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg034.dds", ["CALL_HP_UI"] = 1}
	kTable[35] = {["GROUND_NAME"] = 851035, ["BOSS_NAME"] = 850035, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg035.dds", ["CALL_HP_UI"] = 1}
	kTable[36] = {["GROUND_NAME"] = 851036, ["BOSS_NAME"] = 850036, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg036.dds", ["CALL_HP_UI"] = 1}
	kTable[37] = {["GROUND_NAME"] = 851037, ["BOSS_NAME"] = 850037, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg037.dds", ["CALL_HP_UI"] = 1}
	kTable[38] = {["GROUND_NAME"] = 851038, ["BOSS_NAME"] = 850038, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg038.dds", ["CALL_HP_UI"] = 1}
	kTable[39] = {["GROUND_NAME"] = 851039, ["BOSS_NAME"] = 850039, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg039.dds", ["CALL_HP_UI"] = 1}
	kTable[40] = {["GROUND_NAME"] = 851040, ["BOSS_NAME"] = 850040, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg040.dds", ["CALL_HP_UI"] = 1}
	kTable[41] = {["GROUND_NAME"] = 851041, ["BOSS_NAME"] = 850041, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg041.dds", ["CALL_HP_UI"] = 1}
	kTable[42] = {["GROUND_NAME"] = 851042, ["BOSS_NAME"] = 850042, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg042.dds", ["CALL_HP_UI"] = 1}
	kTable[43] = {["GROUND_NAME"] = 851043, ["BOSS_NAME"] = 850043, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg043.dds", ["CALL_HP_UI"] = 1}
	kTable[44] = {["GROUND_NAME"] = 851044, ["BOSS_NAME"] = 850044, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg044.dds", ["CALL_HP_UI"] = 1}
	kTable[45] = {["GROUND_NAME"] = 851045, ["BOSS_NAME"] = 850045, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg045.dds", ["CALL_HP_UI"] = 1}
	kTable[46] = {["GROUND_NAME"] = 851046, ["BOSS_NAME"] = 850046, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg046.dds", ["CALL_HP_UI"] = 1}
	kTable[47] = {["GROUND_NAME"] = 851047, ["BOSS_NAME"] = 850047, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg047.dds", ["CALL_HP_UI"] = 1}
	kTable[48] = {["GROUND_NAME"] = 851048, ["BOSS_NAME"] = 850048, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg048.dds", ["CALL_HP_UI"] = 1}
	kTable[49] = {["GROUND_NAME"] = 851049, ["BOSS_NAME"] = 850049, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg049.dds", ["CALL_HP_UI"] = 1}
	kTable[50] = {["GROUND_NAME"] = 851050, ["BOSS_NAME"] = 850050, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg050.dds", ["CALL_HP_UI"] = 1}
	kTable[51] = {["GROUND_NAME"] = 851051, ["BOSS_NAME"] = 850051, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg051.dds", ["CALL_HP_UI"] = 1}
	kTable[52] = {["GROUND_NAME"] = 851052, ["BOSS_NAME"] = 850052, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg052.dds", ["CALL_HP_UI"] = 1}
	kTable[53] = {["GROUND_NAME"] = 851053, ["BOSS_NAME"] = 850053, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg053.dds", ["CALL_HP_UI"] = 1}
	kTable[54] = {["GROUND_NAME"] = 851054, ["BOSS_NAME"] = 850054, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg054.dds", ["CALL_HP_UI"] = 1}
	kTable[55] = {["GROUND_NAME"] = 851055, ["BOSS_NAME"] = 850055, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg055.dds", ["CALL_HP_UI"] = 1}
	kTable[56] = {["GROUND_NAME"] = 851056, ["BOSS_NAME"] = 850056, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg056.dds", ["CALL_HP_UI"] = 1}
	kTable[57] = {["GROUND_NAME"] = 851057, ["BOSS_NAME"] = 850057, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg057.dds", ["CALL_HP_UI"] = 1}
	kTable[58] = {["GROUND_NAME"] = 851058, ["BOSS_NAME"] = 850058, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg058.dds", ["CALL_HP_UI"] = 1}
	kTable[59] = {["GROUND_NAME"] = 851059, ["BOSS_NAME"] = 850059, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg059.dds", ["CALL_HP_UI"] = 1}
	kTable[60] = {["GROUND_NAME"] = 851060, ["BOSS_NAME"] = 850060, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg060.dds", ["CALL_HP_UI"] = 1}
	kTable[61] = {["GROUND_NAME"] = 851061, ["BOSS_NAME"] = 850061, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg061.dds", ["CALL_HP_UI"] = 1}
	kTable[62] = {["GROUND_NAME"] = 851062, ["BOSS_NAME"] = 850062, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg062.dds", ["CALL_HP_UI"] = 1}
	kTable[63] = {["GROUND_NAME"] = 851063, ["BOSS_NAME"] = 850063, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg063.dds", ["CALL_HP_UI"] = 1}
	kTable[64] = {["GROUND_NAME"] = 851064, ["BOSS_NAME"] = 850064, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg064.dds", ["CALL_HP_UI"] = 1}
	kTable[65] = {["GROUND_NAME"] = 851065, ["BOSS_NAME"] = 850065, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg065.dds", ["CALL_HP_UI"] = 1}
	kTable[66] = {["GROUND_NAME"] = 851066, ["BOSS_NAME"] = 850066, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg066.dds", ["CALL_HP_UI"] = 1}
	kTable[67] = {["GROUND_NAME"] = 851067, ["BOSS_NAME"] = 850067, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg067.dds", ["CALL_HP_UI"] = 1}
	kTable[68] = {["GROUND_NAME"] = 851068, ["BOSS_NAME"] = 850068, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg068.dds", ["CALL_HP_UI"] = 1}
	kTable[69] = {["GROUND_NAME"] = 851069, ["BOSS_NAME"] = 850069, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg069.dds", ["CALL_HP_UI"] = 1}
	kTable[70] = {["GROUND_NAME"] = 851070, ["BOSS_NAME"] = 850070, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg070.dds", ["CALL_HP_UI"] = 1}
	kTable[71] = {["GROUND_NAME"] = 851071, ["BOSS_NAME"] = 850071, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg071.dds", ["CALL_HP_UI"] = 1}
	kTable[72] = {["GROUND_NAME"] = 851072, ["BOSS_NAME"] = 850072, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg072.dds", ["CALL_HP_UI"] = 1}
	kTable[73] = {["GROUND_NAME"] = 851073, ["BOSS_NAME"] = 850073, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg073.dds", ["CALL_HP_UI"] = 1}
	kTable[74] = {["GROUND_NAME"] = 851074, ["BOSS_NAME"] = 850074, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg074.dds", ["CALL_HP_UI"] = 1}
	kTable[75] = {["GROUND_NAME"] = 851075, ["BOSS_NAME"] = 850075, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg075.dds", ["CALL_HP_UI"] = 1}
	kTable[76] = {["GROUND_NAME"] = 851076, ["BOSS_NAME"] = 850076, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg076.dds", ["CALL_HP_UI"] = 1}
	kTable[77] = {["GROUND_NAME"] = 851077, ["BOSS_NAME"] = 850077, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg077.dds", ["CALL_HP_UI"] = 1}
	kTable[78] = {["GROUND_NAME"] = 851078, ["BOSS_NAME"] = 850078, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg078.dds", ["CALL_HP_UI"] = 1}
	kTable[79] = {["GROUND_NAME"] = 851079, ["BOSS_NAME"] = 850079, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg079.dds", ["CALL_HP_UI"] = 1}
	kTable[80] = {["GROUND_NAME"] = 851080, ["BOSS_NAME"] = 850080, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg080.dds", ["CALL_HP_UI"] = 1}
	kTable[81] = {["GROUND_NAME"] = 851081, ["BOSS_NAME"] = 850081, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg081.dds", ["CALL_HP_UI"] = 1}
	kTable[82] = {["GROUND_NAME"] = 851082, ["BOSS_NAME"] = 850082, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg082.dds", ["CALL_HP_UI"] = 1}
	kTable[83] = {["GROUND_NAME"] = 851083, ["BOSS_NAME"] = 850083, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg083.dds", ["CALL_HP_UI"] = 1}
	kTable[84] = {["GROUND_NAME"] = 851084, ["BOSS_NAME"] = 850084, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg084.dds", ["CALL_HP_UI"] = 1}
	kTable[85] = {["GROUND_NAME"] = 851085, ["BOSS_NAME"] = 850085, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg085.dds", ["CALL_HP_UI"] = 1}
	kTable[86] = {["GROUND_NAME"] = 851086, ["BOSS_NAME"] = 850086, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg086.dds", ["CALL_HP_UI"] = 0}
	kTable[87] = {["GROUND_NAME"] = 851086, ["BOSS_NAME"] = 850087, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg087.dds", ["CALL_HP_UI"] = 1}
	kTable[88] = {["GROUND_NAME"] = 851087, ["BOSS_NAME"] = 850088, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg088.dds", ["CALL_HP_UI"] = 1}
	kTable[89] = {["GROUND_NAME"] = 851087, ["BOSS_NAME"] = 850089, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg089.dds", ["CALL_HP_UI"] = 1}
	kTable[90] = {["GROUND_NAME"] = 851087, ["BOSS_NAME"] = 850090, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg090.dds", ["CALL_HP_UI"] = 1}
	kTable[91] = {["GROUND_NAME"] = 851087, ["BOSS_NAME"] = 850091, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg091.dds", ["CALL_HP_UI"] = 0}
	kTable[92] = {["GROUND_NAME"] = 851087, ["BOSS_NAME"] = 850092, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg092.dds", ["CALL_HP_UI"] = 0}
	kTable[93] = {["GROUND_NAME"] = 851088, ["BOSS_NAME"] = 850093, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg093.dds", ["CALL_HP_UI"] = 1}
	kTable[94] = {["GROUND_NAME"] = 851088, ["BOSS_NAME"] = 850094, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg094.dds", ["CALL_HP_UI"] = 1}
	kTable[95] = {["GROUND_NAME"] = 851088, ["BOSS_NAME"] = 850095, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg095.dds", ["CALL_HP_UI"] = 1}
	kTable[96] = {["GROUND_NAME"] = 851089, ["BOSS_NAME"] = 850096, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg096.dds", ["CALL_HP_UI"] = 1}
	kTable[97] = {["GROUND_NAME"] = 851089, ["BOSS_NAME"] = 850097, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg097.dds", ["CALL_HP_UI"] = 1}
	kTable[98] = {["GROUND_NAME"] = 851089, ["BOSS_NAME"] = 850098, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg098.dds", ["CALL_HP_UI"] = 1}
	kTable[99] = {["GROUND_NAME"] = 851093, ["BOSS_NAME"] = 850099, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg099.dds", ["CALL_HP_UI"] = 1}
	kTable[100] = {["GROUND_NAME"] = 851093, ["BOSS_NAME"] = 850100, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg100.dds", ["CALL_HP_UI"] = 1}
	kTable[101] = {["GROUND_NAME"] = 851093, ["BOSS_NAME"] = 850101, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg101.dds", ["CALL_HP_UI"] = 1}
	kTable[102] = {["GROUND_NAME"] = 851094, ["BOSS_NAME"] = 850102, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg102.dds", ["CALL_HP_UI"] = 1}
	kTable[103] = {["GROUND_NAME"] = 851094, ["BOSS_NAME"] = 850103, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg103.dds", ["CALL_HP_UI"] = 1}
	kTable[104] = {["GROUND_NAME"] = 851094, ["BOSS_NAME"] = 850104, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg104.dds", ["CALL_HP_UI"] = 1}
	kTable[105] = {["GROUND_NAME"] = 851095, ["BOSS_NAME"] = 850105, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg105.dds", ["CALL_HP_UI"] = 1}
	kTable[106] = {["GROUND_NAME"] = 851095, ["BOSS_NAME"] = 850106, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg106.dds", ["CALL_HP_UI"] = 1}
	kTable[107] = {["GROUND_NAME"] = 851095, ["BOSS_NAME"] = 850107, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg107.dds", ["CALL_HP_UI"] = 1}
	kTable[108] = {["GROUND_NAME"] = 851095, ["BOSS_NAME"] = 850108, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg108.dds", ["CALL_HP_UI"] = 1}
	kTable[109] = {["GROUND_NAME"] = 851095, ["BOSS_NAME"] = 850109, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg109.dds", ["CALL_HP_UI"] = 1}
	kTable[110] = {["GROUND_NAME"] = 851096, ["BOSS_NAME"] = 850110, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg110.dds", ["CALL_HP_UI"] = 1}
	kTable[111] = {["GROUND_NAME"] = 851096, ["BOSS_NAME"] = 850111, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg053.dds", ["CALL_HP_UI"] = 1}
	kTable[112] = {["GROUND_NAME"] = 851096, ["BOSS_NAME"] = 850112, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg111.dds", ["CALL_HP_UI"] = 1}
	kTable[113] = {["GROUND_NAME"] = 851096, ["BOSS_NAME"] = 850113, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg112.dds", ["CALL_HP_UI"] = 1}
	kTable[114] = {["GROUND_NAME"] = 851096, ["BOSS_NAME"] = 850114, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg072.dds", ["CALL_HP_UI"] = 1}
	kTable[115] = {["GROUND_NAME"] = 851097, ["BOSS_NAME"] = 850115, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg113.dds", ["CALL_HP_UI"] = 1}
	kTable[116] = {["GROUND_NAME"] = 851097, ["BOSS_NAME"] = 850116, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg096.dds", ["CALL_HP_UI"] = 1}
	kTable[117] = {["GROUND_NAME"] = 851097, ["BOSS_NAME"] = 850117, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg097.dds", ["CALL_HP_UI"] = 1}
	kTable[118] = {["GROUND_NAME"] = 851097, ["BOSS_NAME"] = 850118, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg098.dds", ["CALL_HP_UI"] = 1}
	kTable[119] = {["GROUND_NAME"] = 851097, ["BOSS_NAME"] = 850119, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg114.dds", ["CALL_HP_UI"] = 1}
	kTable[120] = {["GROUND_NAME"] = 851098, ["BOSS_NAME"] = 850120, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg115.dds", ["CALL_HP_UI"] = 1}
	kTable[121] = {["GROUND_NAME"] = 851098, ["BOSS_NAME"] = 850121, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg116.dds", ["CALL_HP_UI"] = 1}
	kTable[122] = {["GROUND_NAME"] = 851098, ["BOSS_NAME"] = 850122, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg118.dds", ["CALL_HP_UI"] = 1}
	kTable[123] = {["GROUND_NAME"] = 851098, ["BOSS_NAME"] = 850123, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg093.dds", ["CALL_HP_UI"] = 1}
	kTable[124] = {["GROUND_NAME"] = 851098, ["BOSS_NAME"] = 850124, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg094.dds", ["CALL_HP_UI"] = 1}
	kTable[125] = {["GROUND_NAME"] = 851098, ["BOSS_NAME"] = 850125, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg095.dds", ["CALL_HP_UI"] = 1}
	kTable[126] = {["GROUND_NAME"] = 851096, ["BOSS_NAME"] = 850111, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg053.dds", ["CALL_HP_UI"] = 1}
	kTable[127] = {["GROUND_NAME"] = 851096, ["BOSS_NAME"] = 850114, ["IMG_PATH"] = "../Data/6_ui/boss/bossImg072.dds", ["CALL_HP_UI"] = 1}	
	
	if nil == kTable[iNo] then return end
	
	CloseUI("SFRM_BIG_MAP")
	CloseUI("SFRM_PROGRESS_MAP")
	local kTopWnd = ActivateUI("FRM_SG_FULL_BOSS_NAME")
	if kTopWnd:IsNil() then return end
	
	g_bEnableShortCutKey = false
	--g_world:SetShowWorldFocusFilterColorAlpha(0xFF000000,0,0.5,1,true,true)
	--UI_CallMovieScene(true,1.0);
	UIOff()
	g_iSuperGroundBossNameUILastStep = 0
	
	GetEventScriptSystem():ActivateEvent(10003)
	--Recall
	local kTopWnd = ActivateUI("FRM_SG_FULL_BOSS_NAME")
	g_fSuperGroundBossNameUIAccumTime = 0
	local kNameTopWnd = kTopWnd:GetControl("FRM_SG_BOSS_NAME")
	if not kNameTopWnd:IsNil() then
		local kMapNameWnd = kNameTopWnd:GetControl("FRM_MAP_NAME")
		if not kMapNameWnd:IsNil() then
			kMapNameWnd:SetStaticTextW( GetTT(kTable[iNo]["GROUND_NAME"]) )
		end
		local kNameWnd = kNameTopWnd:GetControl("FRM_NAME")
		if not kNameWnd:IsNil() then
			kNameWnd:SetStaticTextW( GetTT(kTable[iNo]["BOSS_NAME"]) )
		end
	end
	local kImgWnd = kTopWnd:GetControl("FRM_SG_BOSS_IMG")
	if not kImgWnd:IsNil() then
		kImgWnd:ChangeImage(kTable[iNo]["IMG_PATH"])
	end
	kTopWnd:SetCustomDataAsInt(kTable[iNo]["CALL_HP_UI"])
end

function sm(iSuperGroundNo, iMode)
	Net_SGMove(iSuperGroundNo, iMode)
end
function sf(iFloor)
	Net_SGFloor(iFloor, 1, true)
end

function CallClickHelpUI(kTop, iTextNo, iNo)
	CallClickHelpUICustom(kTop, iTextNo, iNo, 4,3, 11, 25,-1,-1,0)
end
function CallClickHelpUICustom(kTop, iTextNo, iNo, iW, iH, iIndex,iUV,iTransX,iTransY,iAliveTime)
	if kTop:IsNil() then return end
	local kTopPos = kTop:GetTotalLocation()
	local iX = kTopPos:GetX()
	if iTransX then iX = iX * iTransX end
	local iY = kTopPos:GetY()
	if iTransY then iY = iY * iTransY end
	Call_HelpBalloon(iX,iY, iW,iH, iIndex, iUV, iTextNo, "FRN_NEW_BALLOON_"..iNo,iAliveTime)
end
function CloseAllClickHelpUI()
	for iCur = 1, 12 do
		CloseUI("FRN_NEW_BALLOON_" .. iCur)
	end
end

function Call_HelpBalloon(iX, iY, iW, iH, iTargetIdx, iUV, iTextID, NewFrmID, iAliveTime)
	local iMaxCount = 100
	local iBaseSize = 27
	if iW <= 2 or iH <= 2 then return end
	if iW * iH > iMaxCount then return end
	local kTop = ActivateUI("FRM_HELP_BALLOON", false, NewFrmID)
	if kTop:IsNil() then return end
	local kSubWnd = nil
	
	kTop:SetAliveTime(iAliveTime)
	
	local iCurIndex = 0
	for iCurIndex = 0, iW * iH do
		local iXLine = math.mod(iCurIndex, iW)
		local iYLine = math.floor(iCurIndex / iW)
		local kBaseBlock = kTop:GetControl("FRM_BASE" .. iCurIndex)
		if kBaseBlock:IsNil() then return end
		
		kBaseBlock:Visible(true)
		local kPos = kBaseBlock:GetLocation()
		if 0 == iXLine and 0 == iYLine then -- left top
			kBaseBlock:SetUVIndex(7)
		elseif iXLine == (iW - 1) and 0 == iYLine then -- right top
			kBaseBlock:SetUVIndex(9)
		elseif iYLine == 0 then -- top
			kBaseBlock:SetUVIndex(8)
		elseif iYLine ~= 0 and iYLine ~= iH-1 and iXLine == 0 then -- left
			kBaseBlock:SetUVIndex(12)
		elseif iYLine ~= 0 and iYLine ~= iH-1 and iXLine == iW-1 then -- right
			kBaseBlock:SetUVIndex(14)
		elseif iYLine == iH-1 and iXLine == 0 then -- left bottom
			kBaseBlock:SetUVIndex(17)
		elseif iYLine == iH-1 and iXLine ~= 0 and iXLine ~= iW-1 then -- bottom
			kBaseBlock:SetUVIndex(18)
		elseif iYLine == iH-1 and iXLine == iW-1 then -- right bottom
			kBaseBlock:SetUVIndex(19)
		else
			kBaseBlock:SetUVIndex(13) -- center
		end
		if nil ~= iTargetIdx then
			if iTargetIdx == iCurIndex then
				kBaseBlock:SetUVIndex(iUV)
			end
		end
		kPos:SetX(iXLine*iBaseSize) kPos:SetY(iYLine*iBaseSize)
		kBaseBlock:SetLocation(kPos)
	end
	for iCurEmpty = iW*iH, iMaxCount do
		local kBaseBlock = kTop:GetControl("FRM_BASE" .. iCurEmpty)
		if not kBaseBlock:IsNil() then
			kBaseBlock:Visible(false)
		end
	end
	
	local kPos = kTop:GetLocation()
	local kSize = kTop:GetSize()
	kSize:SetX(iBaseSize * iW) kSize:SetY(iBaseSize * iH)
	if 0 > iX then
		iX = -iX
		kPos:SetX(iX - kSize:GetX() + iBaseSize)
	else
		kPos:SetX(iX)
	end
	if 0 > iY then
		iY = -iY
		kPos:SetY(iY - kSize:GetY() + iBaseSize)
	else
		kPos:SetY(iY)
	end
	kTop:SetLocation(kPos)
	kTop:SetSize(kSize)
	
	local kTextWnd = kTop:GetControl("FRM_TEXT")
	if not kTextWnd:IsNil() then
		kSize:SetX(kSize:GetX() - 37*2) kSize:SetY(kSize:GetY() - 27*2)
		kTextWnd:SetSize(kSize)
		kTextWnd:SetStaticTextW( GetTT(iTextID) )
		local kPos = kTextWnd:GetLocation()
		local kTextPos = kTextWnd:GetTextPos()
		local kTextSize = kTextWnd:GetTextSize()
		kPos:SetX(37) kPos:SetY(27)
		kTextPos:SetX(kSize:GetX() / 2) kTextPos:SetY( kSize:GetY()/2 - kTextSize:GetY()/2 )
		kTextWnd:SetTextPos( kTextPos )
		kTextWnd:SetLocation( kPos )
	end
end

function AddChildClickHelpUI(kParent, iTextNo, iNo)
	AddChildClickHelpUICustom(kParent, iTextNo, iNo, 4,3, 11, 25,-1,-1,0)
end
function AddChildClickHelpUICustom(kParent, iTextNo, iNo, iW, iH, iIndex,iUV,iTransX,iTransY,iAliveTime)
	if nil==kParent or kParent:IsNil() then return end
	local kParentPos = kParent:GetTotalLocation()
	local iX = kParentPos:GetX()
	if iTransX then iX = iX * iTransX end
	local iY = kParentPos:GetY()
	if iTransY then iY = iY * iTransY end
	AddChild_HelpBalloon(kParent,iX,iY, iW,iH, iIndex, iUV, iTextNo, "FRN_NEW_BALLOON_"..iNo,iAliveTime)
end
function AddChild_HelpBalloon(kParent, iX, iY, iW, iH, iTargetIdx, iUV, iTextID, NewFrmID, iAliveTime)
	if nil==kParent or kParent:IsNil() then return end
	local iMaxCount = 100
	local iBaseSize = 27
	if iW <= 2 or iH <= 2 then return end
	if iW * iH > iMaxCount then return end
	local kTop = AddChildUI(kParent,"FRM_HELP_BALLOON", NewFrmID,true)
	if kTop:IsNil() then return end
	local kSubWnd = nil
	
	kTop:SetAliveTime(iAliveTime)
	
	local iCurIndex = 0
	for iCurIndex = 0, iW * iH do
		local iXLine = math.mod(iCurIndex, iW)
		local iYLine = math.floor(iCurIndex / iW)
		local kBaseBlock = kTop:GetControl("FRM_BASE" .. iCurIndex)
		if kBaseBlock:IsNil() then return end
		
		kBaseBlock:Visible(true)
		local kPos = kBaseBlock:GetLocation()
		if 0 == iXLine and 0 == iYLine then -- left top
			kBaseBlock:SetUVIndex(7)
		elseif iXLine == (iW - 1) and 0 == iYLine then -- right top
			kBaseBlock:SetUVIndex(9)
		elseif iYLine == 0 then -- top
			kBaseBlock:SetUVIndex(8)
		elseif iYLine ~= 0 and iYLine ~= iH-1 and iXLine == 0 then -- left
			kBaseBlock:SetUVIndex(12)
		elseif iYLine ~= 0 and iYLine ~= iH-1 and iXLine == iW-1 then -- right
			kBaseBlock:SetUVIndex(14)
		elseif iYLine == iH-1 and iXLine == 0 then -- left bottom
			kBaseBlock:SetUVIndex(17)
		elseif iYLine == iH-1 and iXLine ~= 0 and iXLine ~= iW-1 then -- bottom
			kBaseBlock:SetUVIndex(18)
		elseif iYLine == iH-1 and iXLine == iW-1 then -- right bottom
			kBaseBlock:SetUVIndex(19)
		else
			kBaseBlock:SetUVIndex(13) -- center
		end
		if nil ~= iTargetIdx then
			if iTargetIdx == iCurIndex then
				kBaseBlock:SetUVIndex(iUV)
			end
		end
		kPos:SetX(iXLine*iBaseSize) kPos:SetY(iYLine*iBaseSize)
		kBaseBlock:SetLocation(kPos)
	end
	for iCurEmpty = iW*iH, iMaxCount do
		local kBaseBlock = kTop:GetControl("FRM_BASE" .. iCurEmpty)
		if not kBaseBlock:IsNil() then
			kBaseBlock:Visible(false)
		end
	end
	
	local kPos = kTop:GetLocation()
	kPos:SetX(iX)
	kPos:SetY(iY)
	kTop:SetLocation(kPos)

	local kSize = kTop:GetSize()
	kSize:SetX(iBaseSize * iW) kSize:SetY(iBaseSize * iH)
	kTop:SetSize(kSize)
	
	local kTextWnd = kTop:GetControl("FRM_TEXT")
	if not kTextWnd:IsNil() then
		kSize:SetX(kSize:GetX() - 37*2) kSize:SetY(kSize:GetY() - 27*2)
		kTextWnd:SetSize(kSize)
		kTextWnd:SetStaticTextW( GetTT(iTextID) )
		local kPos = kTextWnd:GetLocation()
		local kTextPos = kTextWnd:GetTextPos()
		local kTextSize = kTextWnd:GetTextSize()
		kPos:SetX(37) kPos:SetY(27)
		kTextPos:SetX(kSize:GetX() / 2) kTextPos:SetY( kSize:GetY()/2 - kTextSize:GetY()/2 )
		kTextWnd:SetTextPos( kTextPos )
		kTextWnd:SetLocation( kPos )
	end
end

function CALL_SUPER_GROUND_MAP_DESC(iTextID, iAliveTime)
	local kTop = ActivateUI("FRM_TRUTH_TOWER_DESC")
	if kTop:IsNil() then return end
	kTop:SetStaticTextW(GetTT(iTextID))
	if nil == iAliveTime then
		iAliveTime = 50000
	end
	kTop:SetAliveTime(iAliveTime)
end

function CALL_ROAD_SIGN(iBitDir) --8자리 숫자 (ex: 10020300) 자릿수는 방향을 의미하며 숫자값은 화살표 타입을 의미함
	local bVisibleDir = {}
	bVisibleDir[1] = math.floor(iBitDir / 10000000)
	bVisibleDir[2] = math.floor((iBitDir % 10000000) / 01000000)
	bVisibleDir[3] = math.floor((iBitDir % 01000000) / 00100000)
	bVisibleDir[4] = math.floor((iBitDir % 00100000) / 00010000)
	bVisibleDir[5] = math.floor((iBitDir % 00010000) / 00001000)
	bVisibleDir[6] = math.floor((iBitDir % 00001000) / 00000100)
	bVisibleDir[7] = math.floor((iBitDir % 00000100) / 00000010)
	bVisibleDir[8] = math.floor(iBitDir % 00000010)
	
	local kWndTop = ActivateUI("FRM_CONSTELLATION_ROADSIGN")
	if kWndTop:IsNil() == true then
		return
	end

	for i = 1, 8 do
		local kWndNormal = kWndTop:GetControl("FRM_ARROW_NORMAL_"..i)
		if kWndNormal:IsNil() == false then
			if bVisibleDir[i] == 1 then --1: Normal
				kWndNormal:Visible(true)
			else
				kWndNormal:Visible(false)
			end
		end
		local kWndBoss = kWndTop:GetControl("FRM_ARROW_BOSS_"..i)
		if kWndBoss:IsNil() == false then
			if bVisibleDir[i] == 2 then --2: Boss
				kWndBoss:Visible(true)
			else
				kWndBoss:Visible(false)
			end
		end
		local kWndBonus = kWndTop:GetControl("FRM_ARROW_BONUS_"..i)
		if kWndBonus:IsNil() == false then
			if bVisibleDir[i] == 3 then --3: Bonus
				kWndBonus:Visible(true)
			else
				kWndBonus:Visible(false)
			end
		end
	end
end

