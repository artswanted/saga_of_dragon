CONSTELLATION_PATH				= "../Data/6_ui/constellation/"
CONSTELLATION_ACTIVE			= "ctOn_"
CONSTELLATION_OVER				= "ctOver_"
CONSTELLATION_OVER_NOTACTIVE	= "ctDisable_"
CONSTELLATION_SELECT			= "ctClick_"
CONSTELLATION_STAR_ANI			= "ctStarani_"
CONSTELLATION_MAX_FILE_INDEX	= 10

g_IsMouseOverConstellationEnterBtn = false
g_FakeRefreshConstellationTime = 0.0
g_FakeRefreshConstellationPartyList = false
g_RefreshConstellationPartyTime = 0.0
g_SelectedConstellationPosition	= 1
g_SelectedConstellationDifficulty = 1
g_ConstellationMinLevel = 0
g_ConstellationMaxLevel = 0
g_ConstellationUI = {}
g_ConstellationUI[1] = { ["X"] = 39, ["Y"] = 238, ["W"] = 293, ["H"] = 163 }
g_ConstellationUI[2] = { ["X"] = 59, ["Y"] = 146, ["W"] = 324, ["H"] = 182 }
g_ConstellationUI[3] = { ["X"] = 167, ["Y"] = 102, ["W"] = 248, ["H"] = 192 }
g_ConstellationUI[4] = { ["X"] = 291, ["Y"] = 81, ["W"] = 169, ["H"] = 186 }
g_ConstellationUI[5] = { ["X"] = 407, ["Y"] = 76, ["W"] = 105, ["H"] = 174 }
g_ConstellationUI[6] = { ["X"] = 513, ["Y"] = 76, ["W"] = 105, ["H"] = 174 }
g_ConstellationUI[7] = { ["X"] = 565, ["Y"] = 81, ["W"] = 169, ["H"] = 185 }
g_ConstellationUI[8] = { ["X"] = 611, ["Y"] = 102, ["W"] = 246, ["H"] = 192 }
g_ConstellationUI[9] = { ["X"] = 643, ["Y"] = 147, ["W"] = 322, ["H"] = 181 }
g_ConstellationUI[10] = { ["X"] = 694, ["Y"] = 239, ["W"] = 290, ["H"] = 162 }
g_ConstellationUI[11] = { ["X"] = 374, ["Y"] = 244, ["W"] = 278, ["H"] = 160 }

g_Constellation_StarAni = {}
g_Constellation_StarAni[1] = { ["X"] = 39, ["Y"] = 259, ["W"] = 450, ["H"] = 90 }
g_Constellation_StarAni[2] = { ["X"] = 101, ["Y"] = 158, ["W"] = 450, ["H"] = 90 }
g_Constellation_StarAni[3] = { ["X"] = 213, ["Y"] = 103, ["W"] = 450, ["H"] = 90 }
g_Constellation_StarAni[4] = { ["X"] = 321, ["Y"] = 75, ["W"] = 450, ["H"] = 90 }
g_Constellation_StarAni[5] = { ["X"] = 422, ["Y"] = 65, ["W"] = 450, ["H"] = 90 }
g_Constellation_StarAni[6] = { ["X"] = 517, ["Y"] = 66, ["W"] = 450, ["H"] = 90 }
g_Constellation_StarAni[7] = { ["X"] = 613, ["Y"] = 80, ["W"] = 450, ["H"] = 90 }
g_Constellation_StarAni[8] = { ["X"] = 723, ["Y"] = 104, ["W"] = 450, ["H"] = 90 }
g_Constellation_StarAni[9] = { ["X"] = 831, ["Y"] = 159, ["W"] = 450, ["H"] = 90 }
g_Constellation_StarAni[10] = { ["X"] = 893, ["Y"] = 262, ["W"] = 450, ["H"] = 90 }
g_Constellation_StarAni[11] = { ["X"] = 409, ["Y"] = 273, ["W"] = 200, ["H"] = 600 }

g_Result_Ani = {}
g_Result_Ani[0] = { ["ANI_X"] =  27, ["ANI_Y"] = -17, ["RST_X"] = 41}
g_Result_Ani[1] = { ["ANI_X"] =  27, ["ANI_Y"] = -17, ["RST_X"] = 41}
g_Result_Ani[2] = { ["ANI_X"] =  27, ["ANI_Y"] = -17, ["RST_X"] = 41}
g_Result_Ani[3] = { ["ANI_X"] =  27, ["ANI_Y"] = -17, ["RST_X"] = 41}
g_Result_Ani[4] = { ["ANI_X"] =  27, ["ANI_Y"] = -17, ["RST_X"] = 41}
g_Result_Ani[5] = { ["ANI_X"] =  27, ["ANI_Y"] = -17, ["RST_X"] = 41}
g_Result_Ani[6] = { ["ANI_X"] =  27, ["ANI_Y"] = -17, ["RST_X"] = 41}
g_Result_Ani[7] = { ["ANI_X"] =  27, ["ANI_Y"] = -17, ["RST_X"] = 41}
g_Result_Ani[8] = { ["ANI_X"] =  27, ["ANI_Y"] = -17, ["RST_X"] = 41}

g_ConstellationUIMouseOver = {}
g_ConstellationUIMouseOver[1] = false
g_ConstellationUIMouseOver[2] = false
g_ConstellationUIMouseOver[3] = false
g_ConstellationUIMouseOver[4] = false
g_ConstellationUIMouseOver[5] = false
g_ConstellationUIMouseOver[6] = false
g_ConstellationUIMouseOver[7] = false
g_ConstellationUIMouseOver[8] = false
g_ConstellationUIMouseOver[9] = false
g_ConstellationUIMouseOver[10] = false
g_ConstellationUIMouseOver[11] = false
g_ConstellationUIMouseOver[12] = false

g_ConstellationUIAnimationState = 0
g_ConstellationUIAnimationFrame = 0


function Constellation__ClearRecord()
	local wnd = GetUIWnd("FRM_CONSTELLATION_RESULT")
	if wnd:IsNil() == true then
		wnd = CallUI("FRM_CONSTELLATION_RESULT")
		if wnd:IsNil() == true then
			return
		end
	end
end


function InitConstellationUI(Self)
	g_SelectedConstellationPosition	= 1
	g_SelectedConstellationDifficulty = 1
end

function SetConstellationUI_StarAni(Self)
	if true == Self:IsNil() then
		return
	end
	
	local index = Self:GetCustomDataAsInt()
	if index < GetMinConstellationDungeon() or index > GetMaxConstellationDungeon() then
		return
	end
	
	if nil == g_Constellation_StarAni[index] then
		return
	end
	
	local fileNo = index - 1
	if fileNo < CONSTELLATION_MAX_FILE_INDEX then
		Self:ChangeImage(CONSTELLATION_PATH..CONSTELLATION_STAR_ANI.."0"..fileNo..".tga")
		Self:SetSize(Point2(g_Constellation_StarAni[index]["W"]/5, g_Constellation_StarAni[index]["H"]))
		Self:SetMaxUVIndex(5, 1)
	else
		Self:ChangeImage(CONSTELLATION_PATH..CONSTELLATION_STAR_ANI..fileNo..".tga")
		Self:SetSize(Point2(g_Constellation_StarAni[index]["W"], g_Constellation_StarAni[index]["H"]/5))
		Self:SetMaxUVIndex(1, 5)
	end
	
	Self:SetLocation(Point2(g_Constellation_StarAni[index]["X"], g_Constellation_StarAni[index]["Y"]))
	Self:SetImgSize(Point2(g_Constellation_StarAni[index]["W"], g_Constellation_StarAni[index]["H"]))
end

function SetConstellationUI(Self)
	if true == Self:IsNil() then
		return
	end
	
	local index = Self:GetCustomDataAsInt()
	if index < GetMinConstellationDungeon() or index > GetMaxConstellationDungeon() then
		return
	end
	
	if nil == g_ConstellationUI[index] then
		return
	end
	
	Self:SetLocation(Point2(g_ConstellationUI[index]["X"], g_ConstellationUI[index]["Y"]))
	Self:SetSize(Point2(g_ConstellationUI[index]["W"], g_ConstellationUI[index]["H"]))
	
	local fileNo = index - 1
	local Wnd = Self:GetControl("FRM_ACTIVE")
	if false == Wnd:IsNil() then
		if fileNo < CONSTELLATION_MAX_FILE_INDEX then
			Wnd:ChangeImage(CONSTELLATION_PATH..CONSTELLATION_ACTIVE.."0"..fileNo..".tga")
		else
			Wnd:ChangeImage(CONSTELLATION_PATH..CONSTELLATION_ACTIVE..fileNo..".tga")
		end
		Wnd:SetImgSize(Point2(g_ConstellationUI[index]["W"], g_ConstellationUI[index]["H"]))
		Wnd:SetSize(Point2(g_ConstellationUI[index]["W"], g_ConstellationUI[index]["H"]))
	end
	Wnd = Self:GetControl("FRM_OVER")
	if false == Wnd:IsNil() then
		if fileNo < CONSTELLATION_MAX_FILE_INDEX then
			Wnd:ChangeImage(CONSTELLATION_PATH..CONSTELLATION_OVER.."0"..fileNo..".tga")
		else
			Wnd:ChangeImage(CONSTELLATION_PATH..CONSTELLATION_OVER..fileNo..".tga")
		end
		Wnd:SetImgSize(Point2(g_ConstellationUI[index]["W"], g_ConstellationUI[index]["H"]))
		Wnd:SetSize(Point2(g_ConstellationUI[index]["W"], g_ConstellationUI[index]["H"]))
	end
	Wnd = Self:GetControl("FRM_OVER_NOTACTIVE")
	if false == Wnd:IsNil() then
		if fileNo < CONSTELLATION_MAX_FILE_INDEX then
			Wnd:ChangeImage(CONSTELLATION_PATH..CONSTELLATION_OVER_NOTACTIVE.."0"..fileNo..".tga")
		else
			Wnd:ChangeImage(CONSTELLATION_PATH..CONSTELLATION_OVER_NOTACTIVE..fileNo..".tga")
		end
		Wnd:SetImgSize(Point2(g_ConstellationUI[index]["W"], g_ConstellationUI[index]["H"]))
		Wnd:SetSize(Point2(g_ConstellationUI[index]["W"], g_ConstellationUI[index]["H"]))
	end
	Wnd = Self:GetControl("FRM_SELECT")
	if false == Wnd:IsNil() then
		if fileNo < CONSTELLATION_MAX_FILE_INDEX then
			Wnd:ChangeImage(CONSTELLATION_PATH..CONSTELLATION_SELECT.."0"..fileNo..".tga")
		else
			Wnd:ChangeImage(CONSTELLATION_PATH..CONSTELLATION_SELECT..fileNo..".tga")
		end
		Wnd:SetImgSize(Point2(g_ConstellationUI[index]["W"], g_ConstellationUI[index]["H"]))
		Wnd:SetSize(Point2(g_ConstellationUI[index]["W"], g_ConstellationUI[index]["H"]))
	end
end

function AnimateConstellationUI(Self, gradationMin, gradationMax)
	if true == Self:IsNil() then
		return
	end
	
	if 0 >= g_ConstellationUIAnimationState then
		return
	end
	
	local SelectWnd = Self:GetControl("FRM_SELECT")
	if true == SelectWnd:IsNil() then
		return
	end
	
	local ActiveWnd = Self:GetControl("FRM_ACTIVE")
	if true == ActiveWnd:IsNil() then
		return
	end
	
	local index = Self:GetCustomDataAsInt()
	
	if 1 == g_ConstellationUIAnimationState then
		if index == GetMaxConstellationDungeon() - 1 then
			SelectWnd:SetMaxAlpha(math.sin((g_ConstellationUIAnimationFrame+gradationMax-2)/(index/2)))
		else
			for i = -gradationMin, gradationMax do
				if index == g_ConstellationUIAnimationFrame + i then
					if i < 0 then
						SelectWnd:SetMaxAlpha( 1 - (math.abs(i)/gradationMin) )
					else
						SelectWnd:SetMaxAlpha( 1 - (i/gradationMax) )
					end
					return
				end
			end
		end
	elseif 2 == g_ConstellationUIAnimationState then
		SelectWnd:SetMaxAlpha(0)
	elseif 3 == g_ConstellationUIAnimationState then
		local bVisible = false
		if ConstellationEnable(index, 1) then
			bVisible = IsInDay_Constellation(index)
		end
		
		if true == bVisible then
			ActiveWnd:SetMaxAlpha(math.sin(g_ConstellationUIAnimationFrame/GetMaxConstellationDungeon()))
			local StarAniWnd = Self:GetParent():GetControl("FRM_STAR_ANI_"..index)
			if false == StarAniWnd:IsNil() then
				StarAniWnd:SetMaxAlpha(math.sin(g_ConstellationUIAnimationFrame/GetMaxConstellationDungeon()))
			end
		end
	elseif 4 == g_ConstellationUIAnimationState then -- use cash item
		local bVisible = false
		if ConstellationEnable(index, 1) then
			if IsInDay_Constellation(index) then
				return;
			end
			bVisible = true
		end
		
		if true == bVisible then
			ActiveWnd:SetMaxAlpha(math.sin(g_ConstellationUIAnimationFrame/GetMaxConstellationDungeon()))
			local StarAniWnd = Self:GetParent():GetControl("FRM_STAR_ANI_"..index)
			if false == StarAniWnd:IsNil() then
				StarAniWnd:SetMaxAlpha(math.sin(g_ConstellationUIAnimationFrame/GetMaxConstellationDungeon()))
			end
		end
	elseif 5 == g_ConstellationUIAnimationState then -- use cash item
		local bVisible = false
		if ConstellationEnable(index, 1) then
			bVisible = IsInDay_Constellation(index)
		else
			return
		end
		
		if false == bVisible then
			ActiveWnd:SetMaxAlpha(1 - math.sin(g_ConstellationUIAnimationFrame/GetMaxConstellationDungeon()))
			local StarAniWnd = Self:GetParent():GetControl("FRM_STAR_ANI_"..index)
			if false == StarAniWnd:IsNil() then
				StarAniWnd:SetMaxAlpha(1 - math.sin(g_ConstellationUIAnimationFrame/GetMaxConstellationDungeon()))
			end
		end
	end
end

function CountConstellationUIAnimationFrame(ParentWnd)
	if 0 == g_ConstellationUIAnimationState then
		return
	elseif 1 == g_ConstellationUIAnimationState then
		if g_ConstellationUIAnimationFrame > (GetMaxConstellationDungeon() + 4) then
			g_ConstellationUIAnimationState = 2
			g_ConstellationUIAnimationFrame = 0
		end
	elseif 2 == g_ConstellationUIAnimationState then
		if g_ConstellationUIAnimationFrame > 1 then
			g_ConstellationUIAnimationState = 3
			g_ConstellationUIAnimationFrame = 0
			ResetConstellation()
		end
	elseif 3 == g_ConstellationUIAnimationState then
		if g_ConstellationUIAnimationFrame > GetMaxConstellationDungeon() then
			g_ConstellationUIAnimationState = -1
			g_ConstellationUIAnimationFrame = 0
			Init_ConstellationUI(ParentWnd)
		end
	elseif 4 == g_ConstellationUIAnimationState then
		if g_ConstellationUIAnimationFrame > GetMaxConstellationDungeon() then
			g_ConstellationUIAnimationState = -1
			g_ConstellationUIAnimationFrame = 0
		end
	elseif 5 == g_ConstellationUIAnimationState then
		if g_ConstellationUIAnimationFrame > GetMaxConstellationDungeon() + 1 then
			g_ConstellationUIAnimationState = -1
			g_ConstellationUIAnimationFrame = 0
		end
	end
	g_ConstellationUIAnimationFrame = g_ConstellationUIAnimationFrame + 1
end

function MouseOverConstellation(Self)
	if true == Self:IsNil() then
		return
	end
	
	if true == g_IsMouseOverConstellationEnterBtn then
		return
	end
	
	local ActiveWnd = Self:GetControl("FRM_ACTIVE")
	if true == ActiveWnd:IsNil() then
		return
	end
	
	local index = Self:GetCustomDataAsInt()
	
	local bVisible = false
	local szPath = ActiveWnd:GetDefaultImgName()
	local Alpha = GetAlphaFromImgAtCursor(szPath:GetStr(), ActiveWnd)
	if (Alpha >= 100) then
		bVisible = true
	end
	
	local OverWnd = Self:GetControl("FRM_OVER")
	if true == OverWnd:IsNil() then
		return
	end
	
	local OverNotActiveWnd = Self:GetControl("FRM_OVER_NOTACTIVE")
	if true == OverNotActiveWnd:IsNil() then
		return
	end
	
	if true == bVisible then
		if 0.5 <= ActiveWnd:GetMaxAlpha() then
			if 0 == OverWnd:GetMaxAlpha() then
				OverWnd:SetMaxAlpha(1)
				if( index == 1 or index == 2 ) then
					PlaySoundByID("cha-cursor01")
				elseif( index == 3 or index == 4 ) then
					PlaySoundByID("cha-cursor02")
				elseif( index == 5 or index == 6 ) then
					PlaySoundByID("cha-cursor03")
				elseif( index == 7 or index == 8 ) then
					PlaySoundByID("cha-cursor04")
				elseif( index == 9 or index == 10 ) then
					PlaySoundByID("cha-cursor05")
				elseif( index == 11 ) then
					PlaySoundByID("cha-cursor06")
				end
			end
		else
			OverNotActiveWnd:SetMaxAlpha(1)
		end
		
		UpdateConstellationInfo(Self:GetCustomDataAsInt(), 1)
		
		g_ConstellationUIMouseOver[index] = true
	else
		if 0.5 <= ActiveWnd:GetMaxAlpha() then
			OverWnd:SetMaxAlpha(0)
		else
			OverNotActiveWnd:SetMaxAlpha(0)
		end
		
		g_ConstellationUIMouseOver[index] = false
	end
end

function IsMouseOverConstellationUI()
	for i = 1, GetMaxConstellationDungeon() - 1 do
		if true == g_ConstellationUIMouseOver[i] then
			return true
		end
	end
	return false
end

function ResetConstellation()
	local ParentWnd = GetUIWnd("FRM_CONSTELLATION_DUNGEON")
	if true == ParentWnd:IsNil() then
		return
	end
	
	for i = GetMinConstellationDungeon(), GetMaxConstellationDungeon() do
		local Wnd = ParentWnd:GetControl("FRM_CONSTELLATION_"..i)
		if false == Wnd:IsNil() then
			local SelectWnd = Wnd:GetControl("FRM_SELECT")
			if false == SelectWnd:IsNil() then
				SelectWnd:SetMaxAlpha(0)
			end
			local OverWnd = Wnd:GetControl("FRM_OVER")
			if false == OverWnd:IsNil() then
				OverWnd:SetMaxAlpha(0)
			end
		end
	end
end

function ClickConstellation(Self)
	if true == Self:IsNil() then
		return
	end
	
	if true == g_IsMouseOverConstellationEnterBtn then
		return
	end
	
	local index = Self:GetCustomDataAsInt()
	if index < GetMinConstellationDungeon() or index > GetMaxConstellationDungeon() then
		return
	end
	
	local ActiveWnd = Self:GetControl("FRM_ACTIVE")
	if true == ActiveWnd:IsNil() then
		return
	end
	
	if 0.5 > ActiveWnd:GetMaxAlpha() then
		return
	end
	
	local bClick = false
	local szPath = ActiveWnd:GetDefaultImgName()
	local Alpha = GetAlphaFromImgAtCursor(szPath:GetStr(), ActiveWnd)
	if (Alpha >= 100) then
		bClick = true
	end
	
	if false == bClick then
		return
	end
	
	g_SelectedConstellationPosition = index
	
	local ParentWnd = Self:GetParent()
	if true == ParentWnd:IsNil() then
		return
	end
	
	for i = GetMinConstellationDungeon(), GetMaxConstellationDungeon() do
		local Wnd = ParentWnd:GetControl("FRM_CONSTELLATION_"..i)
		if false == Wnd:IsNil() then
			local SelectWnd = Wnd:GetControl("FRM_SELECT")
			if false == SelectWnd:IsNil() then
				if i == index then
					SelectWnd:SetMaxAlpha(1)
					CallDifficultyButtonSet()
					PlaySoundByID("cha-cursor07")
				else
					SelectWnd:SetMaxAlpha(0)
				end
			end
		end
	end
end

function CallDifficultyButtonSet()
	if false == GetDifficultyLegend(g_SelectedConstellationPosition) then
		ActivateUI("FRM_BTNSET_DIFFICULTY")
	else
		ActivateUI("FRM_BTNSET_DIFFICULTY_LEGEND")		
	end
end

function ReqEnterConstellationDungeon()
	ReqEnterConstellation(g_SelectedConstellationPosition, g_SelectedConstellationDifficulty)
end

function SetSelectedConstellationDungeon(Position, Difficulty)
	g_SelectedConstellationPosition = Position
	g_SelectedConstellationDifficulty = Difficulty
end

function SetConstellationInfo(MinLevel, MaxLevel)
	g_ConstellationMinLevel = MinLevel
	g_ConstellationMaxLevel = MaxLevel
end

function CallConstellationStartUI()
	--GetEventScriptSystem():ActivateEvent(20000010+g_SelectedConstellationPosition)
end

function CallConstellationExit()
	local ExitWnd = nil
	if IsDefenceGameMode() then
		ExitWnd = ActivateUI("SFRM_CONSTELLATION_DEFENCE_EXIT")
	else
		ExitWnd = ActivateUI("SFRM_CONSTELLATION_EXIT")
	end
	
	if false == ExitWnd:IsNil() then
		local DiffWnd = ExitWnd:GetControl("FRM_DIFFICULTY")
		if false == DiffWnd:IsNil() then
			DiffWnd:SetStaticTextW(GetTextW(798812 + g_SelectedConstellationDifficulty))
			if 1 == g_SelectedConstellationDifficulty then
				DiffWnd:SetFontColor(0xFF2cbeff)
			elseif 2 == g_SelectedConstellationDifficulty then
				DiffWnd:SetFontColor(0xFFf95fff)
			elseif 3 == g_SelectedConstellationDifficulty then
				DiffWnd:SetFontColor(0xFFffc258)
			elseif 4 == g_SelectedConstellationDifficulty then
				DiffWnd:SetStaticTextW(GetTextW(50304))
				DiffWnd:SetFontColor(0xFFffc258)	
			end
		end
		local LevelWnd = ExitWnd:GetControl("FRM_LEVEL")
		if false == LevelWnd:IsNil() then
			LevelWnd:SetStaticText(GetTT(798818):GetStr() .. " " .. g_ConstellationMinLevel .. "~" .. g_ConstellationMaxLevel)
		end
	end
	return ExitWnd
end

function CallConstellationMission()
	if g_bConstellationMission or g_world:IsHaveWorldAttr(GATTR_FLAG_CONSTELLATION) then
		ActivateUI("SFRM_CONSTELLATION_MISSION")
		ReqConstellationMission()
	end
end

function CallConstellationUI()
	CallConstellationExit()
	CallConstellationMission()
	ActivateUI("SFRM_CONSTELLATION_TIMER")
	ShowUI_RestMonsterNum(g_RestMonsterNum)
end

function CloseConstellationUI()
	CloseUI("SFRM_CONSTELLATION_DEFENCE_EXIT")
	CloseUI("SFRM_CONSTELLATION_EXIT")
	CloseUI("SFRM_CONSTELLATION_MISSION")
	CloseUI("SFRM_CONSTELLATION_TIMER")
	CloseUI("FRM_REST_MON_NUM")
end

g_iConstellationResultDeadCountTime = 15
function UI_SetConstellationResultCountTimeSec(iTime)
	g_iConstellationResultDeadCountTime = iTime
end

function UI_GetConstellationResultCountTimeSec()
	local iTime = g_iConstellationResultDeadCountTime
	g_iConstellationResultDeadCountTime = 15
	return iTime
end

function AnimateConstellationStartUI(Self)
	local iDelta = GetAccumTime() - g_StartUITimeStamp
	local ms = Self:GetControl("IMG_START")
	local msLoc = ms:GetLocation()
	
	if g_StartUIState == 1 then
		if iDelta > 0.3 then
			g_StartUIState = g_StartUIState + 1
			g_StartUITimeStamp = GetAccumTime()
		else
			local iX = 1023 - iDelta/0.3*652
			msLoc:SetX(iX)
			ms:SetLocation(msLoc)
		end
	elseif g_StartUIState == 2 then
		if iDelta > 3.3 then
			g_StartUIState = g_StartUIState + 1
			g_StartUITimeStamp = GetAccumTime()
		end
	elseif g_StartUIState == 3 then
		if iDelta > 0.3 then
			g_StartUIState = g_StartUIState + 1
			g_StartUITimeStamp = GetAccumTime()
		else
			local iX = 270 - iDelta/0.3*500
			msLoc:SetX(iX)
			ms:SetLocation(msLoc)
		end
	elseif g_StartUIState == 4 then
		Self:Close()
	end
end

ptConstellationResultCenter = Point2(0, 0)
ptConstellationResultTitleCenter = Point2(0, 0)
fCRTimeUpdate = 0
fCRTimeUpdateInterval = 0.01
fCRDelta = 35
fCRDelayTime = 30

function InitConstellationResultSlide(kWnd)
	if kWnd:IsNil() then
		return false
	end

	ptConstellationResultCenter = kWnd:GetLocation()
	local kWndTitle = CallUI("FRM_CONSTELLATION_RESULT_TITLE")
	if kWndTitle:IsNil() then
		return false
	end
	ptConstellationResultTitleCenter = Point2(kWndTitle:GetLocation():GetX(), ptConstellationResultCenter:GetY() - 50)

	local ptResolution = kWnd:GetResolutionSize()
	local ptkWndTitleSize = kWndTitle:GetSize()

	kWnd:SetLocation(Point2(ptConstellationResultCenter:GetX(), ptConstellationResultCenter:GetY() + (ptResolution:GetY() - ptConstellationResultCenter:GetY())))
	kWndTitle:SetLocation(Point2(ptConstellationResultTitleCenter:GetX(), -ptkWndTitleSize:GetY()))
	PlaySoundByID("c1_uizoomin")
	
	fCRDelayTime = 30

	return true
end

function UpdateConstellationResultSlide(kWnd, fAccumTime)
	if kWnd:IsNil() then
		return false
	end
	local kWndTitle = GetUIWnd("FRM_CONSTELLATION_RESULT_TITLE")
	if kWndTitle:IsNil() then
		kWndTitle = CallUI("FRM_CONSTELLATION_RESULT_TITLE")
		if kWndTitle:IsNil() then
			return false
		end
	end
	
	if fAccumTime < (fCRTimeUpdate + fCRTimeUpdateInterval) then
		return false
	end
	fCRTimeUpdate = fAccumTime
	
	if kWnd:GetLocation():GetY() > ptConstellationResultCenter:GetY() then
		kWnd:SetLocation(Point2(kWnd:GetLocation():GetX(), kWnd:GetLocation():GetY() - fCRDelta))
		if kWnd:GetLocation():GetY() < ptConstellationResultCenter:GetY() then
			kWnd:SetLocation(Point2(kWnd:GetLocation():GetX(), ptConstellationResultCenter:GetY()))
			PlaySoundByID("c4_tender_number")
		end
	elseif fCRDelayTime > 0 then
		fCRDelayTime = fCRDelayTime - 1
	else
		if kWndTitle:GetLocation():GetY() < ptConstellationResultTitleCenter:GetY() then
			kWndTitle:SetLocation(Point2(kWndTitle:GetLocation():GetX(), kWndTitle:GetLocation():GetY() + fCRDelta))
			if kWndTitle:GetLocation():GetY() > ptConstellationResultTitleCenter:GetY() then
				kWndTitle:SetLocation(Point2(kWndTitle:GetLocation():GetX(), ptConstellationResultTitleCenter:GetY()))
			end
		else
			PlaySoundByID("c4_tender_number")
			return true
		end
	end
	
	return false
end

g_iRoadSignUVCurIndex = 0
g_iRoadSignUVDir = 1
g_iRoadSignUVInitIndex = {}
g_iRoadSignUVInitIndex[1] = 1
g_iRoadSignUVInitIndex[2] = 5
g_iRoadSignUVInitIndex[3] = 9
g_iRoadSignUVInitIndex[4] = 13
g_iRoadSignUVInitIndex[5] = 17
g_iRoadSignUVInitIndex[6] = 21
g_iRoadSignUVInitIndex[7] = 25
g_iRoadSignUVInitIndex[8] = 29

g_iRoadSignLocation = {}
g_iRoadSignLocation[1] = {["x"] = -45, ["y"] = -206}
g_iRoadSignLocation[2] = {["x"] = 70, ["y"] = -153}
g_iRoadSignLocation[3] = {["x"] = 117, ["y"] = -45}
g_iRoadSignLocation[4] = {["x"] = 70, ["y"] = 64}
g_iRoadSignLocation[5] = {["x"] = -45, ["y"] = 117}
g_iRoadSignLocation[6] = {["x"] = -159, ["y"] = 64}
g_iRoadSignLocation[7] = {["x"] = -206, ["y"] = -45}
g_iRoadSignLocation[8] = {["x"] = -159, ["y"] = -153}


function InitConstellationRoadSign(self)
	g_iRoadSignUVCurIndex = 0
	g_iRoadSignUVDir = 1

	for i = 1, 8 do
		local kWndNormal = self:GetControl("FRM_ARROW_NORMAL_"..i)
		if kWndNormal:IsNil() == false then
			kWndNormal:SetUVIndex(g_iRoadSignUVInitIndex[i])
			
			local Location = kWndNormal:GetLocation()
			Location:SetX(g_iRoadSignLocation[i]["x"])
			Location:SetY(g_iRoadSignLocation[i]["y"])
			kWndNormal:SetLocation(Location)
		end
		local kWndBoss = self:GetControl("FRM_ARROW_BOSS_"..i)
		if kWndBoss:IsNil() == false then
			kWndBoss:SetUVIndex(g_iRoadSignUVInitIndex[i])
			
			local Location = kWndBoss:GetLocation()
			Location:SetX(g_iRoadSignLocation[i]["x"])
			Location:SetY(g_iRoadSignLocation[i]["y"])
			kWndBoss:SetLocation(Location)
		end
		local kWndBonus = self:GetControl("FRM_ARROW_BONUS_"..i)
		if kWndBonus:IsNil() == false then
			kWndBonus:SetUVIndex(g_iRoadSignUVInitIndex[i])
			
			local Location = kWndBonus:GetLocation()
			Location:SetX(g_iRoadSignLocation[i]["x"])
			Location:SetY(g_iRoadSignLocation[i]["y"])
			kWndBonus:SetLocation(Location)
		end
	end
	

end

function UpdateConstellationRoadSign(self)
	if g_iRoadSignUVDir == 1 then
		if g_iRoadSignUVCurIndex == 3 or g_iRoadSignUVCurIndex > 3 then
			g_iRoadSignUVDir = -1
		end
	else
		if g_iRoadSignUVCurIndex == 0 or g_iRoadSignUVCurIndex < 0 then
			g_iRoadSignUVDir = 1
		end
	end
	g_iRoadSignUVCurIndex = g_iRoadSignUVCurIndex + g_iRoadSignUVDir

	if self == nil or self:IsNil() == true then
		return
	end

	for i = 1, 8 do
		local kWndNormal = self:GetControl("FRM_ARROW_NORMAL_"..i)
		if kWndNormal:IsNil() == false then
			kWndNormal:SetUVIndex(g_iRoadSignUVCurIndex + g_iRoadSignUVInitIndex[i])
		end
		local kWndBoss = self:GetControl("FRM_ARROW_BOSS_"..i)
		if kWndBoss:IsNil() == false then
			kWndBoss:SetUVIndex(g_iRoadSignUVCurIndex + g_iRoadSignUVInitIndex[i])
		end
		local kWndBonus = self:GetControl("FRM_ARROW_BONUS_"..i)
		if kWndBonus:IsNil() == false then
			kWndBonus:SetUVIndex(g_iRoadSignUVCurIndex + g_iRoadSignUVInitIndex[i])
		end
	end
end

function CallConstellationPartyListToolTip(guid)
	if nil == guid or guid:IsNil() then return end
	local ToolTipStr = GetConstellationPartyListToolTip(guid)
	if nil ~= ToolTipStr then
		CallToolTipText(0, ToolTipStr, GetCursorPos(), "ToolTip2", 11)
	else
		CloseToolTip(nil, 11)
	end
end

kCMTAlphaStep = 1 --1: FadeIn, 2: Stay, 0: FadeOut
kCMTAlphaDelay = 30
function InitConstellationMinimapThumbnail(kTopWnd)
	if kTopWnd:IsNil() == true then
		return
	end
	PlaySoundByID("button_UI_Close")

	local ptImgSize = g_world:GetMapThumbnailImgSize()
	local strImgName = g_world:GetMapThumbnailImgName()
	kTopWnd:SetSize(ptImgSize)
	kTopWnd:SetImgSize(ptImgSize)
	kTopWnd:ChangeImage(strImgName:GetStr())

	kCMTAlphaStep = 1
	kCMTAlphaDelay = 30
	kTopWnd:SetMaxAlpha(0)
	kTopWnd:SetScaleCenter(Point2F(0.5, 0.5))
	kTopWnd:SetScale(1.2)
	kTopWnd:SetAlignX(50)
	kTopWnd:SetAlignY(25)
	kTopWnd:VAlign()
end

function SetItemTreeTime(iSeletedItemNo,iOrderIdx)
	if IsGemStoreItemTree(iSeletedItemNo,iOrderIdx) then
		ItemTreeClear()
		GetEventTimer():Add("SET_ITEM_TREE", 0.1, "SetGemStoreItemTree(" .. tostring(iSeletedItemNo) .. "," .. tostring(iOrderIdx) .. ")", 1, false)
	end
end

function UpdateConstellationMinimapThumbnail(kTopWnd)
	if kTopWnd:IsNil() == true then
		return false
	end
	local fScale = kTopWnd:GetScale()
	local kCurAlpha = kTopWnd:GetMaxAlpha()
	if kCMTAlphaStep == 1 then --1: FadeIn, 2: Stay, 0: FadeOut
		kCurAlpha = kCurAlpha + 0.12
		fScale = fScale - 0.02
		if (kCurAlpha > 1 or kCurAlpha == 1) and (fScale == 1 or fScale < 1) then
			kCurAlpha = 1
			kCMTAlphaStep = 2
		end
		kTopWnd:SetScale(fScale)
	elseif kCMTAlphaStep == 2 then
		kCMTAlphaDelay = kCMTAlphaDelay - 1
		if kCMTAlphaDelay == 0 or kCMTAlphaDelay < 0 then
			kCMTAlphaStep = 0
		end
	else
		kCurAlpha = kCurAlpha - 0.1
		if kCurAlpha == 0 or kCurAlpha < 0 then
			kTopWnd:Close()
			return true
		end
		fScale = fScale - 0.02
		kTopWnd:SetScale(fScale)
		
	end
	kTopWnd:SetMaxAlpha(kCurAlpha)
	return false
end

