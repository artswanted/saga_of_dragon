g_SelectMapUITable = {}
g_SelectMapUITable["CursorSet"] = {}
g_SelectMapUITable["TotalCount"] = 0
g_SelectMapUITable["Current"] = 1
g_SelectMapUITable["Trigger"] = nil

function	SelectMapUI_OpenUI( kTrigger, kActor )
	--Open UI
	local	kSelectMapUI = ActivateUI("SFRM_SELECT_MAP");
	if kSelectMapUI:IsNil() then
		return
	end

	local	kMapNameControl = kSelectMapUI:GetControl("SFRM_COLOR"):GetControl("SFRM_SDW"):GetControl("SFRM_MAP_NAME");
	if false == kMapNameControl:IsNil() then
		--Title
		local	iTitleTextID = tonumber(kTrigger:GetParamFromParamMap("TitleTextID"));
		if nil ~= iTitleTextID then
			local	kTitleText = GetTextW(iTitleTextID)
			kMapNameControl:SetStaticTextW(kTitleText)
		else
			kMapNameControl:SetStaticText("")
		end
	end
	
	-- Clear All List Item
	local	kListControl = kSelectMapUI:GetControl("MapList")
	if true == kListControl:IsNil() then
		kSelectMapUI:Close()
		return
	end

	local bIsHomeTown = (kTrigger:GetParamFromParamMap("IsHomeTown")=="TRUE")	--홈타운 포탈이냐?

	local kTitle = kSelectMapUI:GetControl("SFRM_TITLE")
	if false==kTitle:IsNil() then
		local kBtn = kTitle:GetControl("SFRM_TAB_BG"):GetControl("CBTN_TITLE")
		if false==kBtn:IsNil() then
			local kTTW = 301
			if true==bIsHomeTown then
				kTTW = 308
			end
			kBtn:SetStaticTextW(GetTextW(kTTW))
		end
	end
	
	kListControl:ClearAllListItem()
	g_SelectMapUITable["CursorSet"] = {}
	g_SelectMapUITable["TotalCount"] = 0
	g_SelectMapUITable["Current"] = 0
	g_SelectMapUITable["Trigger"] = nil
	g_SelectMapUITable["IsHomeTown"] = bIsHomeTown
	
	-- 리스트 추가하기
	local iLastIndex = 0
	local iIndex = 1
	if bIsHomeTown then	--홈타운 포탈이냐?
		iIndex = 0	--0베이스
		local iTownCount = GetHomeTownCount()
		while 0<iTownCount do
			SelectMapUI_AddHomeTownItem( kListControl, GetHomeTownNoAt(iIndex), iIndex + 1 )
			iTownCount = iTownCount -1
			iIndex = iIndex + 1
		end
	else
		while true == kTrigger:IsHavePortalAccess( iIndex ) do
			if true == kTrigger:IsPortalAccess( iIndex ) then
				if true == SelectMapUI_AddMapItem( kListControl, kTrigger:GetPortalAccessName(iIndex), iIndex ) then
					iLastIndex = iIndex
				end
			end
			iIndex = iIndex + 1
		end
	end
	
	g_SelectMapUITable["Trigger"] = kTrigger:GetID()
--	if 2 > g_SelectMapUITable["TotalCount"] then
--		kSelectMapUI:Close()
--		World_Portal_Action( kTrigger, kActor, iLastIndex )
--	end
end

function SelectMapUI_AddHomeTownItem( kListControl, iMapNo, index )
	local kListItem = kListControl:AddNewListItem( GetMapNameW(iMapNo) )
	if kListItem:IsNil() == false then
		local wnd = kListItem:GetWnd()
		if wnd:IsNil() == false then
			kListItem:GetWnd():SetCustomDataAsInt(index)
			local kCheckBtn = wnd:GetControl("CBTN_CHECK")
			if nil ~= kCheckBtn and not kCheckBtn:IsNil() then
				kCheckBtn:CheckState(false)
				g_SelectMapUITable["TotalCount"] = g_SelectMapUITable["TotalCount"] + 1
				local iCur = g_SelectMapUITable["TotalCount"]
				g_SelectMapUITable["CursorSet"][iCur] = kCheckBtn
				return true
			end
		end
	end
	return false
end

function SelectMapUI_AddMapItem( kListControl, iNameID, index )
	local kListItem = kListControl:AddNewListItem( GetTextW(iNameID) )
	if kListItem:IsNil() == false then
		local wnd = kListItem:GetWnd()
		if wnd:IsNil() == false then
			kListItem:GetWnd():SetCustomDataAsInt(index)
			local kCheckBtn = wnd:GetControl("CBTN_CHECK")
			if nil ~= kCheckBtn and not kCheckBtn:IsNil() then
				kCheckBtn:CheckState(false)
				g_SelectMapUITable["TotalCount"] = g_SelectMapUITable["TotalCount"] + 1
				local iCur = g_SelectMapUITable["TotalCount"]
				g_SelectMapUITable["CursorSet"][iCur] = kCheckBtn
				return true
			end
		end
	end
	return false
end

function OnClick_SelectMapItem(kCheckBtn)
	if nil == kCheckBtn or kCheckBtn:IsNil() then return end
	if kCheckBtn:GetCheckState() then
		return
	end
	
	local kItemTop = kCheckBtn:GetParent()
	if nil == kItemTop or kItemTop:IsNil() then return end
	local kListWnd = kItemTop:GetParent()
	if nil == kListWnd or kListWnd:IsNil() then return end
	
	OnUpdate_SelectMapResetCursor(kListWnd)
	
	--ODS("FRM_MAPLIST_ELE SetCustomDataAsInt : "..UISelf:GetParent():GetCustomDataAsInt().."\n");
	kCheckBtn:CheckState(true)
	kListWnd:SetCustomDataAsInt( kItemTop:GetCustomDataAsInt() )
end

function OnUpdate_SelectMapResetCursor(kListWnd)
	if nil == kListWnd or kListWnd:IsNil() then return end
	
	local kListItem = kListWnd:ListFirstItem();
	while kListItem:IsNil() == false do
		kListItem:GetWnd():GetControl("CBTN_CHECK"):CheckState(false)
		kListItem = kListWnd:ListNextItem(kListItem);
	end
end

--[[function OnUpdate_GetSelectMapItem(kListWnd, iCur)
	if nil == kListWnd or kListWnd:IsNil() then return end
	if nil == iCur or 0 == iCur then iCur = 1 end
	
	OnUpdate_SelectMapResetCursor(kListWnd)
	
	local iCnt = 1
	local kListItem = kListWnd:ListFirstItem();
	while kListItem:IsNil() == false do
		--kListItem:GetWnd():GetControl("CBTN_CHECK"):CheckState(false)
		if iCnt == iCur then
			local kItemWnd = kListItem:GetWnd()
			if nil ~= kItemWnd and not kItemWnd:IsNil() then
				local kCheckBtn = kItemWnd:GetControl("CBTN_CHECK")
				if nil ~= kCheckBtn and not kCheckBtn:IsNil() then
					kCheckBtn:CheckState(true)
					kListWnd:SetCustomDataAsInt( kItemWnd:GetCustomDataAsInt() )
				end
			end
			return
		end
		kListItem = kListWnd:ListNextItem(kListItem);
		iCnt = iCnt + 1
	end
end
]]

function OnKeyDown_SelectMapMove(iIncVal)
	if nil == iIncVal then return end
	local iCur = g_SelectMapUITable["Current"]
	iCur = iCur + iIncVal
	if nil == g_SelectMapUITable["CursorSet"][iCur] then
		if 0 < iIncVal then
			iCur = 1
		elseif 0 > iIncVal then
			iCur = g_SelectMapUITable["TotalCount"]
		end
	end
	local kCurCheckBtn = g_SelectMapUITable["CursorSet"][iCur]
	if nil == kCurCheckBtn or kCurCheckBtn:IsNil() then return end
	g_SelectMapUITable["Current"] = iCur
	
	OnClick_SelectMapItem(kCurCheckBtn)
end

function OnClick_EnterSelectMap( kTopSelf )
	if nil == kTopSelf or kTopSelf:IsNil() then return end
	local kList = kTopSelf:GetControl("MapList")
	if nil == kList or kList:IsNil() then return end
	local index = kList:GetCustomDataAsInt()
	
	if 0 == index then
		-- 좀!!! 이동지역을 선택하고 눌러라
		g_ChatMgrClient:Notice_Show_ByTextTableNo(112)
		return
	end
	
	local actor = GetMyActor()
	if false == actor:IsNil() then
		if nil ~= g_SelectMapUITable["Trigger"] then
			if g_SelectMapUITable["IsHomeTown"] then
				OnClickHomeTown(index-1)	--0베이스임
			else
				local action = actor:ReserveTransitAction("a_unlock_move")
				action = actor:ReserveTransitAction("a_portal")
				if action:IsNil() == false then
					action:SetParam(1, g_SelectMapUITable["Trigger"])
					action:SetParam(2, index)
				end
			end
			g_SelectMapUITable["Trigger"] = nil
		else
			ODS("[OnClick_EnterSelectMap] tirgger is nil\n")
		end
	end
	CloseUI("SFRM_SELECT_MAP")
end
