g_bNewRaceLoadingImg = false
g_bFullScreen = false

function SetNewRaceLoadingImg( bShow )
	g_bNewRaceLoadingImg = bShow
end
function GetNewRaceLoadingImg()
	return g_bNewRaceLoadingImg
end

function UI_SetLoadingImage(mapNumber, bNoProgress)
	local parent = GetUIWnd("FRM_LOADING_IMG")
	if parent:IsNil() then
		return false
	end
	
	ODS("Loading of "..mapNumber.." map\n")
	
	local loadingWnd2 = parent:GetControl("LOADING_IMG_FIELD")
	local loadingWnd3 = parent:GetControl("LOADING_IMG_010503")
	local loadingWnd4 = parent:GetControl("LOADING_IMG_020200")
	
	if loadingWnd2:IsNil() == false then
		loadingWnd2:Visible(false)
	end
	
	if loadingWnd3:IsNil() == false then
		loadingWnd3:Visible(false)
	end
	
	if loadingWnd4:IsNil() == false then
		loadingWnd4:Visible(false)
	end
	
	local loadingWnd = nil
	if mapNumber == 010503 or mapNumber == 9010503 then
		loadingWnd = parent:GetControl("LOADING_IMG_010503")
	elseif mapNumber == 020200 or mapNumber == 9020200 then
		loadingWnd = parent:GetControl("LOADING_IMG_020200")
	else 
		loadingWnd = parent:GetControl("LOADING_IMG_FIELD")
	end

	if loadingWnd:IsNil() == true then
		ODS("Loading for "..mapNumber.." doesn't exist\n")
		return false
	end
	
	loadingWnd:Visible(true)
	if bNoProgress == nil or bNoProgress == false then
	    UI_SetLoadingProgress(0)
		SetBriefingLoadUI(MapNo, 0)
	end
end

function UI_SetShowPartyLoadingProgress(bShow, iNum)
	local parent = GetUIWnd("FRM_LOADING_IMG")
	if parent:IsNil() then
		return false
	end
	
	local iShowNum = 0
	if bShow == true and iNum > 0 then
		iShowNum = iNum
	end

	
	for i = 1, 3 do
		local bgWnd = parent:GetControl("LOADING_PROGRESS_"..i.."BG")
		if bgWnd:IsNil() == false then
			if iShowNum > i then
				bgWnd:IsVisible(false)
			else
				bgWnd:IsVisible(true)
			end
		end
		
		local barWnd = parent:GetControl("LOADING_PROGRESS_"..i)
		if barWnd:IsNil() == false then
			if iShowNum > i then
				barWnd:IsVisible(false)
			else
				barWnd:IsVisible(true)
			end
		end		
	end		
end

function UI_SetLoadingProgress(per)
	local parent = GetUIWnd("FRM_LOADING_IMG")
	if parent:IsNil() then
		return true
	end
	local wnd = parent:GetControl("BAR_LOADING_PROGRESS")
	if wnd:IsNil() == true then
		ODS("parent:GetControl[BAR_LOADING_PROGRESS] ����\n")
		return true
	end		
	local size = wnd:GetImgSize()
	size:SetX(847 * per / 100)
	wnd:SetImgSize(size)
	g_renderMan:Draw()
	ODS("UI_SetLoadingProgress : " .. per .. " Time : " .. GetAccumTime() ..  "\n")
	return true
end

function UI_SetPartyLoadingProgress(iNum, per)
	local parent = GetUIWnd("FRM_LOADING_IMG")
	if parent:IsNil() or iNum == 0 or iNum > 3 then
		return true
	end
	
	local wnd = parent:GetControl("BAR_LOADING_PROGRESS"..iNum)
	if wnd:IsNil() == true then
		ODS("parent:GetControl[BAR_LOADING_PROGRESS"..iNum.."] ����\n")
		return true
	end		
	
	local size = wnd:GetImgSize()
	size:SetX(847 * per / 100)
	wnd:SetImgSize(size)
	g_renderMan:Draw()
	return true
end

function UI_SetFullScreenLoadingResolution(UISelf)
	if UISelf:IsNil() then
		return
	end

	local kScreenSize = GetScreenSize()
	local kPos = UISelf:GetLocation()
	kPos:SetX( (kScreenSize:GetX() - UISelf:GetSize():GetX() ) / 2 )
	kPos:SetY( (kScreenSize:GetY() - UISelf:GetSize():GetY() ) / 2 )
	UISelf:SetLocation( kPos )
end

function UI_SetFullScreenLoadingImg(img)
	local kWnd = GetUIWnd("FRM_BACKGROUND")
	if kWnd:IsNil() then
		return
	end

	local kImg = kWnd:GetControl("FRM_IMG")
	if kImg:IsNil() then
		return
	end

	ODS("Set Loading image: " .. img .. "\n")
	kImg:SetDefaultImgName(img)
	UI_SetFullScreenLoadingResolution(kWnd)
end

function UI_SetFullScreenLoading(img)
	UI_SetFullScreenLoadingImg(img)
	g_bFullScreen = true

	local kWnd = GetUIWnd("FRM_LOADING_IMG")
	if kWnd:IsNil() then
		return
	end

	local kText = kWnd:GetControl("FRM_TEXT")
	if kText:IsNil() == false then
		kText:Visible(false)
	end

	local kProgress = kWnd:GetControl("BAR_LOADING_PROGRESS")
	if kProgress:IsNil() == false then
		kProgress:Visible(false)
	end
end

function UI_FullScreenHideLogo()
	local kWnd = GetUIWnd("FRM_LOADING_IMG")
	if kWnd:IsNil() then
		return
	end
	local kLogo = kWnd:GetControl("GAME_LOGO")
	if kLogo:IsNil() == false then
		kLogo:Visible(false)
	end
end