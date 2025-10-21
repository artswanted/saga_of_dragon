g_iBookNaviCenter = 1
g_bBookNaviMin = false;
g_bBookNaviMax = false;
g_kLeftMapList = {}
g_kRightMapList = {}
g_iBookNowMap = g_mapNo

BOOK_PATH = "../Data/6_ui/book/"
function UI_Set_Book_MapNavi(wnd, mapID, Cont, Grnd, bNeedClearList)
	if wnd:IsNil() then return end
	if nil==mapID then
		return
	end

	if nil==bNeedClearList then
		bNeedClearList=true
	end

	local posTbl = nil
	local grndTbl = nil
	local contTbl = nil

	local first = ""
	local pmap = mapID
	if 0<mapID then	--맵번호만으로 구하기
		local pmap = FindPMAP_NO(mapID)
		if 0<pmap then
			mapID = pmap
		end
		posTbl = g_GroundPos[mapID]
		if nil==posTbl then
			return 
		else			
			grndTbl = g_Ground[posTbl["CONT"]]
			if nil==grndTbl then
				return
			else
				contTbl = g_Continent[grndTbl["CONT"]]
				if nil==contTbl then
					return
				end
			end
			first = posTbl["CONT"]
		end
		for i=1,2 do
			local btn = wnd:GetControl("FRM_PAGE2"):GetControl("BTN_CONT"..i)
			if false==btn:IsNil() then
				local bSame = (contTbl["BTN"]==("BTN_CONT"..i))
				btn:LockClick(bSame)
				btn:CheckState(bSame)			
			end
		end

	elseif nil~=Cont and ""~=Cont then
		contTbl = g_Continent[Cont]	--북의 대륙버튼을 눌렀을 때
		if nil~=contTbl and nil~=contTbl["1ST"] then
			mapID=contTbl["1ST"]
			if false==IsOpenGroundByNo(mapID) then	--첫 마을도 안가봤으면
				mapID = FindFirstMapInCont(Cont)
			end
		end
	elseif nil~=Grnd and ""~=Grnd then
		grndTbl = g_Ground[Grnd]
		contTbl = g_Continent[grndTbl["CONT"]]
		first = Grnd
	else
		return
	end
	if nil==contTbl then 
		return 
	end

	local lst = wnd:GetControl("LST_CONT")
	if lst:IsNil() then return end

	local item = nil
	if bNeedClearList then
		lst:ClearAllListItem()
	else
		item = lst:ListFirstItem()
	end
	for i=1,10 do
		if nil~=contTbl[i] then
			if bNeedClearList then
				item = lst:AddNewListItem(GetTextW(contTbl[i]["NAME"]))
			elseif item:IsNil() then
				break;
			end
			if false==item:IsNil() then
				UI_T_Y_ToCenter(item:GetWnd())
				item:GetWnd():SetCustomDataAsStr(contTbl[i]["GROUND"])
				if ""==first then
					first = contTbl[i]["GROUND"]
					item:GetWnd():CheckState(true)
					item:GetWnd():LockClick(true)
				else
					local bLock = (first == contTbl[i]["GROUND"])
					item:GetWnd():CheckState(bLock)
					item:GetWnd():LockClick(bLock)
				end
			end
			if false==bNeedClearList then
				item = lst:ListNextItem(item)
			end
		end
	end

	local groundName = ""
	if nil~=posTbl then
		groundName = posTbl["CONT"]
	else
		groundName = first
	end
	if ""==groundName then 
		return 
	end

	local img = wnd:GetControl("IMG_LEFT_PAGE")
	if false==img:IsNil() then 
		img:SetStaticTextW(GetTextW(g_Ground[groundName]["TITLE_TTW"]))
	end

	lst = wnd:GetControl("LST_GROUND")
	if lst:IsNil() then
		return 
	end
	lst:ClearAllListItem()

	local clicked = nil

	for index, value in pairs(g_GroundList[groundName]) do
		local item = lst:AddNewListItem(GetMapNameW(value["MAP_NO"]))
		if false==item:IsNil() then
			if "VILLAGE"==value["TYPE"] then
				item:GetWnd():ButtonChangeImage(BOOK_PATH.."bkBtLst2_03.tga",260,96)
			elseif "DUNGEON"==value["TYPE"] then
				item:GetWnd():ButtonChangeImage(BOOK_PATH.."bkBtLst2_02.tga",260,96)
			end

			UI_T_Y_ToCenter(item:GetWnd())
			item:GetWnd():SetCustomDataAsInt(value["MAP_NO"])

			if 0==mapID then
				mapID = value["MAP_NO"]
			end

			if mapID==value["MAP_NO"] then
				clicked = item:GetWnd()
			end

			item:GetWnd():Disable(false==IsOpenGroundByNo(value["MAP_NO"]))
		end
	end
	if nil~=clicked and false==clicked:IsNil() then
		UI_GroundEleOnClick(clicked)
	end
end

function UI_ContEleOnClick(wnd)
	if wnd:IsNil() then return end
	if wnd:GetCheckState() then return end

	local list = wnd:GetParent()
	if list:IsNil() then return end

	UI_Set_Book_MapNavi(list:GetParent(), 0, "", wnd:GetCustomDataAsStr():GetStr(), false)
end

function UI_GroundEleOnClick(wnd)
	if wnd:IsNil() then return end
	if wnd:GetCheckState() then return end

	local mapno = wnd:GetCustomDataAsInt()
	if 0==mapno then return end

	local list = wnd:GetParent()
	if list:IsNil() then return end

	local item = list:ListFirstItem()
	local custom = 0
	while false==item:IsNil() do
		custom = item:GetWnd():GetCustomDataAsInt()
		item:GetWnd():CheckState(mapno==custom)
		item:GetWnd():LockClick(mapno==custom)

		item = list:ListNextItem(item)
	end

	local iType = UI_Get_MapType(mapno)

	local parent = list:GetParent():GetControl("FRM_PAGE2"):GetControl("IMG_RIGHT_PAGE")

	if false==parent:IsNil() then
		for i=0,2 do
			parent:GetControl("FRM_PAGE"..i):Visible(false)
			if i==iType then
				SetWorldPage(parent:GetControl("FRM_PAGE"..iType), mapno, iType)
			end
		end
	end
end

function UI_Click_Book_Cont_Btn(wnd)	--선택한 대륙의 첫번째 맵번호를 찾자
	if wnd:IsNil() == true then return end
	if wnd:GetCheckState() == true then --선택된걸 또 선택
		return 
	end

	local Parent = wnd:GetParent()
	if Parent:IsNil() == true then return end

	for i=1,2 do
		local btn = Parent:GetControl("BTN_CONT"..i)
		if false==btn:IsNil() then
			btn:LockClick(false)
			btn:CheckState(false)			
		end
	end

	wnd:CheckState(true)
	wnd:LockClick(true)
		
	UI_Set_Book_MapNavi(Parent:GetParent(), 0, wnd:GetCustomDataAsStr():GetStr(), "", true)
end

function FindFirstMapInCont(cont)
	local contTbl = g_Continent[cont]
	if nil==contTbl then return 0 end

	for i=1,10 do
		if nil==contTbl[i] then return 0 end
		local gName = contTbl[i]["GROUND"]
		local grndTbl = g_GroundList[gName]
		if nil~=grndTbl then
			for j=1,20 do
				if true==IsOpenGroundByNo( grndTbl["MAP_NO"] ) then
					return grndTbl["MAP_NO"]
				end
			end
		else
			break
		end
	end

	return 0
end

function FindCustomBossByMapNo(no)
	local posTbl = g_GroundPos[no]
	if nil==posTbl then return 0 end

	if nil~=posTbl["PMAP_NO"] then
		posTbl = g_GroundPos[posTbl["PMAP_NO"]]
	end
	
	if nil~=posTbl["BOSS"] then
		return posTbl["BOSS"]
	end

	return 0
end

function FindPMAP_NO(no)
	local posTbl = g_GroundPos[no]
	if nil==posTbl then return 0 end

	if nil~=posTbl["PMAP_NO"] then
		return posTbl["PMAP_NO"]
	end

	return 0
end

function UI_Set_Book_MapNavi_Icon(UIParent, MapNo, idx, End, Calc, UserPos)
	local InfoTbl = g_GroundPos[MapNo]
	local NextMapNo = 0
	if nil ~= InfoTbl then
		if Calc < 0 then
			NextMapNo = InfoTbl["PREV"]
		else
			NextMapNo = InfoTbl["NEXT"]
		end
		
		if  idx == 0 or idx == 3  then
			if nil ~= InfoTbl["MAX"] and InfoTbl["MAX"] == true then
				g_bBookNaviMax = true
			elseif nil ~= InfoTbl["MIN"] and InfoTbl["MIN"] == true then
				g_bBookNaviMin = true
			end
		end	
	end

	if idx >= 0 and idx <= 3 then
		local wndIcon = UIParent:GetControl("FRM_MAPICON" .. idx)
		if false == wndIcon:IsNil() then
			wndIcon:Visible(false)
			if nil~=InfoTbl then
				wndIcon:Visible(true)
				wndIcon:ChangeImage(g_MapIconPath[InfoTbl["TYPE"]])

				wndIcon:SetStaticText(GetMapNameW(MapNo):GetStr())
				local iLocalMapNo = MapNo
				wndIcon:SetCustomDataAsInt(iLocalMapNo)	--맵번호 저장
			end
		end
	end
	if idx ~= End then
		UI_Set_Book_MapNavi_Icon(UIParent, NextMapNo, idx + Calc, End, Calc, false)
	else
		return
	end
end

function UI_Build_Book_Tab(wnd)
	local Index = wnd:GetBuildIndex()
	local iTTID = 0
	if Index == 5 then
		iTTID = 49001
		local GetLoc = wnd:GetLocation()
		GetLoc:SetY(534)
		wnd:SetLocation(GetLoc)
	else
		iTTID = Index + 3000
	end
	wnd:SetStaticTextW(GetTextW(iTTID)) 
end

BOOK_TAB_COUNT = 6

function UI_Bool_Tab_Init(wnd,iIndex)
	if wnd:IsNil() == true then return end
	for i=0,BOOK_TAB_COUNT do	--버튼들 초기화
		local Btn = wnd:GetControl("CBTN_TAB"..i)
		if false==Btn:IsNil() then
			Btn:CheckState(i==iIndex)
			Btn:LockClick(i==iIndex)
		end
	end
end

function UI_CallBookAchievent()
	local kBookWnd = UI_Click_Book_Tab(CallUI("FRM_BOOK"):GetControl("CBTN_TAB1"))
	if nil~=kBookWnd and false==kBookWnd:IsNil() then
		local kWnd = kBookWnd:GetControl("FRM_PAGE1"):GetControl("CBTN_TAB2")
		UI_Click_Book_Medal_Sub(kWnd)
	end
end

function UI_Click_Book_Tab(wnd)
	if wnd:IsNil() == true then return nil end
	wnd:LockClick(true)
	if wnd:GetCheckState() == true then --선택된걸 또 선택
		return nil
	else
		wnd:CheckState(true)
	end 
	--새로띄울 페이지의 버튼은 방금 클릭한걸로
	--닫힐 페이지의 버튼은 원래대로
	local iLast = wnd:GetBuildIndex()
	local iNow = string.sub(wnd:GetID():GetStr(), 9)	--제일 마지막 한글자
	if nil==iNow then iNow = 0 end
	iNow = tonumber(iNow)
	local Parent = wnd:GetParent()
	if Parent:IsNil() == true then return nil end

	for i=0,BOOK_TAB_COUNT do	--버튼들 초기화
		local Btn = Parent:GetControl("CBTN_TAB"..i)
--		local Frm = Parent:GetControl("FRM_PAGE"..i)
		local name = "FRM_BOOK"
		if i>0 then
			name = name..i
		end

		if Btn:IsNil() == false then
			if i~=iLast then
				Btn:CheckState(false)
				Btn:LockClick(false)
			end
		end
		CloseUI(name)
	end

	local kBookId = "FRM_BOOK"
	if 0<iLast then
		kBookId = "FRM_BOOK"..iLast
	end
	local kNewPage = ActivateUI(kBookId)
	if kNewPage:IsNil() then return nil end

	for i=0,BOOK_TAB_COUNT do	--버튼들 초기화
		local Btn = kNewPage:GetControl("CBTN_TAB"..i)

		if Btn:IsNil() == false then
			Btn:CheckState(i==iLast)
			Btn:LockClick(i==iLast)
		end
	end

	if iLast == 5 then
		OnClickBookRankPage(kNewPage)
		return kNewPage
	end

	local wndLeft = kNewPage:GetControl("IMG_LEFT_PAGE")
	if wndLeft:IsNil() then
		return kNewPage
	end

	if (iLast==2 or iLast==3)  and nil~=g_GroundPos[g_mapNo] then
		local NowCont = g_GroundPos[g_mapNo]["CONT"]	--현재 내가 있는 대륙
		local Frm = kNewPage:GetControl("FRM_PAGE"..iLast)
		for i,value in pairs(g_Continent) do
			local btn = Frm:GetControl(i)
			if false==btn:IsNil() then
				btn:LockClick(NowCont == i)
				btn:CheckState(NowCont == i)
				if NowCont == i then
					wndLeft:GetControl("FRM_MAP_SUCC"):SetStaticText(UI_Init_Book_Cont_Rate(btn:GetID():GetStr()))	--월드 성취율
				end
			end
		end		
	end
	return kNewPage
end

function UI_Book_OnCall(wnd)
	if wnd:IsNil() then return end
	for i=0,BOOK_TAB_COUNT do
		local kName = "FRM_BOOK"
		if 0<i then kName = kName..i end
		if wnd:GetID():GetStr()~=kName then
			CloseUI(kName)
		end
	end
	UI_Click_Book_Tab(wnd:GetControl("CBTN_TAB0"))
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetShowWorldFocusFilter(true, "../Data/5_Effect/9_Tex/EF_blackBG.tga", 0.8) 
end

function UI_Book_OnClose(iNo)
	for i=0,BOOK_TAB_COUNT do
		local kName = "FRM_BOOK"
		if 0<i and i~=iNo then
			CloseUI(kName)
		end
	end
	CloseToolTip()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetShowWorldFocusFilter(false) 
	OnCloseBookUI()
end

function UI_Init_Book_Cont_Rate(id)
	local kTable = g_GroundList[id]
	local iCnt = 0
	for i, value in pairs(kTable) do
		if IsOpenGroundByNo(value["MAP_NO"]) then
			iCnt = iCnt + 1
		end
	end
	
	local total = table.getn(kTable)	-- 0 base
	if 0==total then
		total = 1
	end

	local rate = math.floor(iCnt/total*100)
	return tostring(rate).."%"
end

function UI_Click_Book_MapIcon(wnd, targetUnVisible)
	if UI_Check_Book_Quest_Page() then return end
	if wnd:IsNil() == true then return end
	local Parent = wnd:GetParent()
	if Parent:IsNil() == true then return end

	local iCustom = wnd:GetCustomDataAsInt()	--맵번호
	if 0==iCustom then
		iCustom = g_mapNo
	end

	if nil==g_GroundPos[iCustom]["CANDI0"] and not IsOpenGroundByNo(iCustom) then
		AddWarnDataTT(3077)
--		Parent:GetControl("IMG_MAP"):Visible(false)
		return
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local bIsChaos = false
	if g_world:IsHaveWorldAttr(GATTR_FLAG_CHAOS_F) then	--카오스면
		bIsChaos = true
		kCandi = nil
		for i=0,3 do
			local kCandi = g_GroundPos[9018200]["CANDI"..i]	--기본맵들 중에 하나 골라서 보여주자
			if nil~=kCandi and IsOpenGroundByNo(kCandi) then
				iCustom = kCandi
			end
		end
		if nil==iCustom then	--기본맵들 중에도 하나도 없으면
			iCustom = 9018200	--진짜 답없음;
		end
	end

	local wndTarget = Parent:GetControl("IMG_TARGET_POS")
	if wndTarget:IsNil() then return end
	wndTarget:Visible(false)

	local iType = UI_Get_MapType(iCustom)

	local page2 = Parent:GetParent():GetControl("FRM_PAGE2")

	--Parent:GetControl("IMG_MAP"):Visible(false)
	if false == page2:IsVisible() or 0>=iCustom then
		Parent:GetControl("IMG_MAP"):Visible(true)
		SetBookMiniMap(Parent:GetControl("IMG_MAP"), iCustom)
		SetWorldPage(page2:GetControl("IMG_RIGHT_PAGE"):GetControl("FRM_PAGE"..iType),iCustom, iType)
		return
	end

	local kCandiList = Parent:GetParent():GetControl("SFRM_LIST_BOX")
	if kCandiList:IsNil() then return end
	kCandiList:SetCustomDataAsInt(0)
	local MapList = kCandiList:GetControl("LST_MAP_CANDI")
	if MapList:IsNil() == true then return end
	MapList:ClearAllListItem()
	if nil~=g_GroundPos[iCustom]["CANDI0"] then	--후보지들이 있다면
		kCandiList:Visible(true)
		for i=0,9 do 	--일단 한 10개?
			local CandiMap = g_GroundPos[iCustom]["CANDI"..i]
			if nil~=CandiMap and 0<CandiMap then
				local wndEle = MapList:AddNewListItemChar(GetMapNameW(CandiMap):GetStr()):GetWnd()
				if false==wndEle:IsNil() then
					wndEle:SetCustomDataAsInt(CandiMap)
				end
			else
				break
			end
		end
		local ListSize = MapList:GetSize()
		ListSize:SetY(22*MapList:GetListItemCount())
		MapList:SetSize(ListSize)
		MapList:GetParent():SetSize(Point2(ListSize:GetX()+4, ListSize:GetY()+4))
		MapList:GetParent():SetLocation(GetCursorPos())
		Parent:GetParent():GetControl("SFRM_LIST_BOX"):SetCustomDataAsInt(iCustom)
		return
	else
		kCandiList:Visible(false)
	end

	for i=0,2 do
		page2:GetControl("IMG_RIGHT_PAGE"):GetControl("FRM_PAGE"..i):Visible(false)
	end

	local myPos = Point2(wnd:GetLocation():GetX()+17, wnd:GetLocation():GetY())
	wndTarget:SetLocation(myPos)
	wndTarget:SetCustomDataAsInt(0)
	if false==targetUnVisible then
		wndTarget:Visible(true)
	end

	local wndRight = Parent:GetParent():GetControl("FRM_PAGE2"):GetControl("IMG_RIGHT_PAGE")
	if wndRight:IsNil() == true then return end

	iType = UI_Get_MapType(iCustom)
	for i=0,2 do
		local wndPage = wndRight:GetControl("FRM_PAGE"..i)
		if false == wndPage:IsNil() then
			wndPage:Visible(i==iType)
			if i==iType then
				Parent:GetControl("IMG_MAP"):Visible(true)
				if SetWorldPage(wndPage, iCustom, iType) then
				    SetBookMiniMap(Parent:GetControl("IMG_MAP"), iCustom)
                end
				wndTarget:SetCustomDataAsInt(iCustom)
			end
		end
	end
end

function UI_Click_Book_MapEle(wnd)
	if wnd:IsNil() == true then return end
	local Parent = wnd:GetParent():GetParent()	--스태틱 폼
	if Parent:IsNil() == true then return end

	local wndLeft = Parent:GetParent():GetControl("IMG_LEFT_PAGE")
	if wndLeft:IsNil() == true then return end

	local iCustom = wnd:GetCustomDataAsInt()	--실제 맵번호
	if false==IsOpenGroundByNo(iCustom) then
		AddWarnDataTT(3077)
		return
	end
	local iParentCustom = Parent:GetCustomDataAsInt()	--Def번호

	local iType = UI_Get_MapType(iParentCustom)
	for i=0,3 do
		local wndIcon = wndLeft:GetControl("FRM_MAPICON"..i)
		local IconCustom = wndIcon:GetCustomDataAsInt()
		if false == wndIcon:IsNil() and iParentCustom == IconCustom then	--같은걸 찾아서
			local IconPos = wndIcon:GetLocation()
			wndLeft:GetControl("IMG_TARGET_POS"):SetLocation(Point2(IconPos:GetX()+17, IconPos:GetY()))
			wndLeft:GetControl("IMG_TARGET_POS"):Visible(true)
			wndLeft:GetControl("IMG_TARGET_POS"):SetCustomDataAsInt(iParentCustom)
			local wndRight = Parent:GetParent():GetControl("FRM_PAGE2"):GetControl("IMG_RIGHT_PAGE")
			if wndRight:IsNil() == true then return end

			for i=0,2 do
				local wndPage = wndRight:GetControl("FRM_PAGE"..i)
				if false == wndPage:IsNil() then
					wndPage:Visible(i==iType)
					if i==iType then
						if SetWorldPage(wndPage, iCustom, iType) then
						    SetBookMiniMap(wndLeft:GetControl("IMG_MAP"), iCustom)
                        end
						wndLeft:GetControl("IMG_MAP"):Visible(true)
					end
				end
			end
			break
		end
	end
end


function UI_Get_MapType(iNo)
	local Type = "DEFAULT"
	local iType = 0
	if nil~=g_GroundPos[iNo] then
		local pmap = g_GroundPos[iNo]["PMAP_NO"]
		if nil~=pmap then
			iNo = pmap
		end
		
		if nil == g_GroundPos[iNo] then
			return 0
		end
		local key = g_GroundPos[iNo]["KEY"]
		local cont = g_GroundPos[iNo]["CONT"]
		
		if nil == g_GroundList[cont][key] then
			return 0
		end
		
		Type = g_GroundList[cont][key]["TYPE"]
	end

	if Type == "DEFAULT" then
		return 0
	elseif Type == "VILLAGE" then
		return 1
	elseif Type == "BOSS" or Type == "DUNGEON" then
		return 0
	end

	return 0
end

function UI_Click_Book_Help(wnd)
	if wnd:IsNil() == true then return end
	wnd:LockClick(true)
	if wnd:GetCheckState() == true then --선택된걸 또 선택
		return 
	else
		wnd:CheckState(true)
	end 
	local Parent = wnd:GetParent()
	if Parent:IsNil() == true then return end
	
	local iLast = wnd:GetBuildIndex()
	
	for i=0,5 do
		local Btn = Parent:GetControl("CBTN_TAB"..i)
--		local Frm = Parent:GetControl("FRM_PAGE"..i)
		if Btn:IsNil() == false then
			if i~=iLast then
				Btn:CheckState(false)
				Btn:LockClick(false)
			end
		end
--[[		if Frm:IsNil() == false then
			if i==iLast then
				Frm:Visible(true)
			else
				Frm:Visible(false)
			end
		end]]
	end

	Parent:GetControl("IMG_RIGHT_PAGE"):SetStaticTextW(GetTextW(3060+iLast))
end

function UI_Click_Book_Medal(wnd)
	if wnd:IsNil() == true then return end
	wnd:LockClick(true)
	if wnd:GetCheckState() == true then --선택된걸 또 선택
		return 
	else
		wnd:CheckState(true)
	end 

	local Parent = wnd:GetParent()
	if Parent:IsNil() == true then return end
	
	local iLast = tonumber(string.sub(wnd:GetID():GetStr(), -1))
	
	for i=0,2 do
		local Btn = Parent:GetControl("CBTN_MEDAL"..i)
--		local Frm = Parent:GetControl("FRM_PAGE"..i)
		if Btn:IsNil() == false then
			if i~=iLast then
				Btn:CheckState(false)
				Btn:LockClick(false)
			end
		end
--[[		if Frm:IsNil() == false then
			if i==iLast then
				Frm:Visible(true)
			else
				Frm:Visible(false)
			end
		end]]
	end
end

function UI_Click_Book_Medal_Sub(wnd)
	if wnd:IsNil() == true then return end
	wnd:LockClick(true)
	if wnd:GetCheckState() == true then --선택된걸 또 선택
		return 
	else
		wnd:CheckState(true)
	end 
	local Parent = wnd:GetParent()
	if Parent:IsNil() == true then return end
	
	local iLast = wnd:GetBuildIndex()
	
	local ExpPage = Parent:GetControl("FRM_PAGE")
	local List = Parent:GetControl("LST_MEDAL")
	local EquipWnd = Parent:GetControl("FRM_EQUIP_BOOK_MEDAL")
	if ExpPage:IsNil() or List:IsNil() or EquipWnd:IsNil() then
		return
	end
	
	local Builder = Parent:GetControl("BLD_TYPE")
	if Builder:IsNil() then
		return
	end
	
	local BuildCountPt = Builder:GetBuildCount()
	local BuildCount = BuildCountPt:GetX() * BuildCountPt:GetY()
	
	for i=1, BuildCount do
		local Btn = Parent:GetControl("CBTN_TAB"..i)
		if Btn:IsNil() == false then
			if i~=iLast then
				Btn:CheckState(false)
				Btn:LockClick(false)
			else
				if 1 == iLast then
					ExpPage:Visible(true)
					List:Visible(false)
					EquipWnd:Visible(false)
				else
					ExpPage:Visible(false)
					List:Visible(true)
					EquipWnd:Visible(true)
					SetAchievePage(wnd:GetParent(), iLast - 1)
				end
			end
		end
	end
end

function UI_Init_Book_Medal_Sub(wnd)
	if wnd:IsNil() == true then return end

	local Builder = wnd:GetControl("BLD_TYPE")
	if Builder:IsNil() then
		return
	end
	
	local BuildCountPt = Builder:GetBuildCount()
	local BuildCount = BuildCountPt:GetX() * BuildCountPt:GetY()

	for i=1, BuildCount do
		local Btn = wnd:GetControl("CBTN_TAB"..i)
		if Btn:IsNil() == false then
			local bFirst = (1==i)
			if bFirst then
				local HelpWnd = wnd:GetControl("FRM_PAGE")
				if not HelpWnd:IsNil() then
					HelpWnd:Visible(true)
				end
				
				local ListWnd = wnd:GetControl("LST_MEDAL")
				if not ListWnd:IsNil() then
					ListWnd:Visible(false)
				end

				local EquipWnd = wnd:GetControl("FRM_EQUIP_BOOK_MEDAL")
				if not EquipWnd:IsNil() then
					EquipWnd:Visible(false)
				end
			end

			Btn:CheckState(bFirst)
			Btn:LockClick(bFirst)
		end
	end

	wnd:GetControl("BTN_BOOK_GET_ACHIEVE_ITEM"):Visible(false)
end

function UI_Click_Book_Quest_Tab(wnd, iType)
	for i=0,1 do
		local btn = wnd:GetControl("CBTN_TAB"..i)
		if false == btn:IsNil() then
			btn:CheckState(iType==i)
			btn:LockClick(iType==i)
			if iType==i then
			end
		end
	end
	SetQuestPage(wnd, iType)
end

function UI_Click_Book_Item(wnd)
	if wnd:IsNil() == true then return end
	wnd:LockClick(true)
	if wnd:GetCheckState() == true then --선택된걸 또 선택
		return 
	else
		wnd:CheckState(true)
	end 
	local Parent = wnd:GetParent()
	if Parent:IsNil() == true then return end
	
	local iLast = wnd:GetBuildIndex()
	
	-------------------------------------------------------------------------
	--미개방 컨텐츠 #NOT_OPEN#(개방시 삭제) 북의 '아이템의 등급' 메뉴
	if(GetLocale() ~= LOCALE.NC_DEVELOP) then	
		for i=0,4 do
			local iNum = i
			local Btn = Parent:GetControl("CBTN_LEFT_TAB"..i)
			if( 2 <= iNum) then iNum = iNum + 1 end
			local Frm = Parent:GetControl("FRM_PAGE"..iNum)
			
			
			if Btn:IsNil() == false then
				if i~=iLast then
					Btn:CheckState(false)
					Btn:LockClick(false)
				end
			end
			 if Frm:IsNil() == false then
				if 1==iNum then
					 OnClickSetBestItem(Frm)
				elseif 3==iNum then				
					local CutStr = WideString("...");
					local TxtFrm6 = Frm:GetControl("FRM_TEXT6")
					if TxtFrm6:IsNil() == false then
						TxtFrm6:SetStaticTextW(GetTT(3123))
						--SetCutedTextLimitLength( TxtFrm6, TxtFrm6:GetStaticText(), CutStr, TxtFrm6:GetWidth() )
					end
					local TxtFrm8 = Frm:GetControl("FRM_TEXT8")				
					if TxtFrm8:IsNil() == false then
						TxtFrm8:SetStaticTextW(GetTT(3173))
						SetCutedTextLimitLength( TxtFrm8, TxtFrm8:GetStaticText(), CutStr, TxtFrm8:GetWidth() )
					end
				end
				
				 if i==iLast then
					 Frm:Visible(true)
				 else
					 Frm:Visible(false)
				 end
			 end
		end
		---------------------------------------------------------------------------------------
	else
	for i=0,5 do
		local Btn = Parent:GetControl("CBTN_LEFT_TAB"..i)
		local Frm = Parent:GetControl("FRM_PAGE"..i)
		if Btn:IsNil() == false then
			if i~=iLast then
				Btn:CheckState(false)
				Btn:LockClick(false)
			end
		end
		if Frm:IsNil() == false then
			if 1==i then
				OnClickSetBestItem(Frm)
			end
				if i==3 then
				local TxtFrm6 = Frm:GetControl("FRM_TEXT6")
				local TxtFrm8 = Frm:GetControl("FRM_TEXT8")				
				TxtFrm6:SetStaticTextW(GetTT(3172))
				TxtFrm8:SetStaticTextW(GetTT(3173))				
				local CutStr = WideString("...");
				SetCutedTextLimitLength( TxtFrm6, TxtFrm6:GetStaticText(), CutStr, TxtFrm6:GetWidth() )
				SetCutedTextLimitLength( TxtFrm8, TxtFrm8:GetStaticText(), CutStr, TxtFrm8:GetWidth() )
			end
			
			if i==iLast then
				Frm:Visible(true)
			else
				Frm:Visible(false)
			end
		end
	end
end
	-------------------------------------------------------------------------
	
end

function UI_Check_Book_Quest_Page()
	local book = GetUIWnd("FRM_BOOK3")
	if book:IsNil() then return false end

	if book:GetControl("CBTN_TAB3"):GetCheckState() then--퀘스트 버튼
		return true
	end

	if book:GetControl("FRM_PAGE3"):IsVisible() then
		return true
	end

	return false
end

function UI_Clear_Book_QuestInfo(wnd)
	if wnd:IsNil() then
		return
	end

	local TitleForm = wnd:GetControl("IMG_RIGHT_PAGE")
	if TitleForm:IsNil() then
		return
	end
	TitleForm:SetStaticText("")

	local CommentList = wnd:GetControl("LST_QUEST_COMMENT")
	if CommentList:IsNil() then
		return
	end
	CommentList:ClearAllListItem()
	
	local NpcIcon = wnd:GetControl("ICN_NPC")
	if NpcIcon:IsNil() then
		return
	end
	
	NpcIcon:Visible(false)
end

TBL_Explanation_Location = {}
TBL_Explanation_Location[0] = { ["X"] = 48, ["Y"] = 11 }
TBL_Explanation_Location[1] = { ["X"] = 308, ["Y"] = 28 }
TBL_Explanation_Location[2] = { ["X"] =48, ["Y"] = 44 }
TBL_Explanation_Location[3] = { ["X"] = 48, ["Y"] = 60 }
TBL_Explanation_Location[4] = { ["X"] = 48, ["Y"] = 76 }
TBL_Explanation_Location[5] = { ["X"] = 48, ["Y"] = 92 }
TBL_Explanation_Location[6] = { ["X"] = 48, ["Y"] = 108 }
TBL_Explanation_Location[7] = { ["X"] = 48, ["Y"] = 124 }
TBL_Explanation_Location[8] = { ["X"] = 48, ["Y"] = 156 }
TBL_Explanation_Location[9] = { ["X"] = 48, ["Y"] = 220 }
TBL_Explanation_Location[10] = { ["X"] = 48, ["Y"] = 284 }

function UI_Build_Book_ToolTipExpButton(wnd)
	if wnd:IsNil() then return end
	local BuildIndex = wnd:GetBuildIndex()
	local Location = wnd:GetLocation()
	Location:SetX(TBL_Explanation_Location[BuildIndex]["X"])
	Location:SetY(TBL_Explanation_Location[BuildIndex]["Y"])
	wnd:SetLocation(Location)
	wnd:SetStaticText(""..(BuildIndex + 1))
end

function UI_Click_Book_ToolTipExplanation(wnd)
	if wnd:IsNil() then return end
	local Parent = wnd:GetParent()

	local Child = Parent:GetControl("FRM_EXPLANATION")
	if Child:IsNil() then return end
	
	Child:SetStaticTextW(GetTT(3126 + wnd:GetBuildIndex()));
end

function UI_Click_Book_SoulTabChange(wnd, Page)
	local Parent = wnd:GetParent():GetParent()
	for idx = 0, 2 do
		local Child = Parent:GetControl("FRM_SUB"..idx)
		if not Child:IsNil() then
			if idx == Page then
				Child:Visible(true)
			else
				Child:Visible(false)
			end
		end
	end
end

function OnOverImportanceQuestGroup(wnd, over)
	local pEx = wnd:GetControl("IMG_EX");
	if not pEx:IsNil() then
		local iUV = pEx:GetUVIndex();
		if 3 ~= iUV then
			if true == over then
				pEx:SetUVIndex(2);
			else
				pEx:SetUVIndex(1);
			end
		end
	end
	local pSelect = wnd:GetControl("IMG_SELECT");
	if not pSelect:IsNil() then
		local iUV = pSelect:GetUVIndex();
		if 3 ~= iUV then
			if true == over then
				pSelect:SetUVIndex(2);
			else
				pSelect:SetUVIndex(1);
			end
		end
	end
end

function OnClickImportanceQuestGroup(wnd)
	local bIsChangeValue = not wnd:IsExpandTree();
	if true == bIsChangeValue then
		wnd:ExpandTree();
	else
		wnd:CollapseTree();
	end
	
	local pEx = wnd:GetControl("IMG_EX");
	if not pEx:IsNil() then
		if true == bIsChangeValue then
			pEx:SetUVIndex(3);
		else
			pEx:SetUVIndex(1);
		end
	end
end

function OnClickQuestViewListChange(wnd)
	if wnd:GetCheckState() then
		return
	end
	local UIParent = wnd:GetParent();
	if UIParent:IsNil() then
		return
	end
	
	local QuestList = UIParent:GetControl("LST_QUEST");
	local ImportanceQuestList = UIParent:GetControl("LST_IMPORTANCE_QUEST");
	
	if QuestList:IsNil() or ImportanceQuestList:IsNil() then
		return
	end
	
	local IsVisibleNormal = QuestList:IsVisible();
	QuestList:Visible(not IsVisibleNormal);
	ImportanceQuestList:Visible(IsVisibleNormal);

	for i=1,2 do
		local btn = UIParent:GetControl("BTN_QUEST_TYPE_CHANGE"..i)
		if false==btn:IsNil() then
			btn:LockClick(false)
			btn:CheckState(false)			
		end
	end

	wnd:LockClick(true)
	wnd:CheckState(true)
end

function AchieveUserNotifyUISizeSetter(Parent)
	if Parent:IsNil() then return end

	local iScreenX = GetScreenSize():GetX()
	Parent:SetSize( Point2(iScreenX, Parent:GetSize():GetY()) )
	Parent:SetLocation( Point2(0,0) )
	
	local BackGround = Parent:GetControl("SFRM_BACKGROUND")
	if BackGround:IsNil() then return end
	BackGround:SetSize( Point2(iScreenX+12, BackGround:GetSize():GetY()) )
	BackGround:SetLocation( Point2(-6,0) )
	
	local Border = Parent:GetControl("FRM_BORDER")
	if Border:IsNil() then return end
	Border:SetSize( Point2(iScreenX, Border:GetSize():GetY()) )
	
	local Achievement = Border:GetControl("FMA_FIRST_ACHIEVEMENT")
	if Achievement:IsNil() then return end
	Achievement:SetSize( Point2(iScreenX - 30, Achievement:GetSize():GetY()) )
	Achievement:SetupMove(Point2(iScreenX - 30,0), Point2(-(iScreenX - 30),0), 15)
	
	local BtnGreetMsg = Parent:GetControl("BTN_GREET_MSG")
	if BtnGreetMsg:IsNil() then return end
	BtnGreetMsg:SetLocation( Point2(iScreenX-BtnGreetMsg:GetSize():GetX(), BtnGreetMsg:GetLocation():GetY()) )
end

function AchieveUserNotifyTestPacket()
	local kPacket = NewPacket(16304);
	kPacket:PushInt();
	kPacket:PushWString(WideString("엄마친구동생아들의딸"));
	AddTestPacket(kPacket);
end

function ItemDisplayGradeExplainPageSwitch(Parent, PageNo)
	if Parent:IsNil() then return end
	
	local Page0 = Parent:GetControl("FRM_PAGE0");
	local Page1 = Parent:GetControl("FRM_PAGE1");
	
	if Page0:IsNil() or Page1:IsNil() then return end
	
	Page0:Visible( PageNo == 0 );
	Page1:Visible( PageNo == 1 );
end

function ItemDisplayExplainBuildPage0TO0(kSelf)
	local Index = kSelf:GetBuildIndex();
	
	local Text1 = kSelf:GetControl("FRM_TEXT1");
	local Text2 = kSelf:GetControl("FRM_TEXT2");
	local Text3 = kSelf:GetControl("FRM_TEXT3");

	if Text1:IsNil() or Text2:IsNil() or Text3:IsNil() then return end
	
	if Index == 4 then
		Text1:SetStaticText(GetTT(403278):GetStr())
		Text3:SetStaticText(GetTT(406117):GetStr())
	elseif Index > 4 then
		Text1:SetStaticText(GetTT(3314 + Index):GetStr())
		Text3:SetStaticText(GetTT(3331 + Index):GetStr())
	else
		Text1:SetStaticText(GetTT(3315 + Index):GetStr())
		Text3:SetStaticText(GetTT(3332 + Index):GetStr())
	end
	if Index == 1 then
		Text3:SetTextPos(Point2(82, -7))
	end
	if Index == 0 then
		Text2:SetStaticText(GetTT(3331):GetStr())			
	else
		Text2:SetStaticText(ChangeIndexToSysEmoFont(Index):GetStr())
	end
end

function ItemDisplayExplainBuildPage0TO1(kSelf)
	local Index = kSelf:GetBuildIndex();
	kSelf:SetStaticText(ChangeIndexToSysEmoFont(Index+1):GetStr()..GetTT(3156):GetStr())
end

TBL_DISPLAY_GET_RATE = {}
TBL_DISPLAY_GET_RATE[0] = { ["WP"]=3321, ["AM"]=3321, ["AC"]=3321, ["RT"]=3324 }
TBL_DISPLAY_GET_RATE[1] = { ["WP"]=3322, ["AM"]=3321, ["AC"]=3322, ["RT"]=3325 }
TBL_DISPLAY_GET_RATE[2] = { ["WP"]=3322, ["AM"]=3321, ["AC"]=3322, ["RT"]=3325 }
TBL_DISPLAY_GET_RATE[3] = { ["WP"]=3322, ["AM"]=3321, ["AC"]=3322, ["RT"]=3325 }
TBL_DISPLAY_GET_RATE[4] = { ["WP"]=3321, ["AM"]=3321, ["AC"]=3321, ["RT"]=406117 }
TBL_DISPLAY_GET_RATE[5] = { ["WP"]=3321, ["AM"]=3321, ["AC"]=3321, ["RT"]=3326 }
TBL_DISPLAY_GET_RATE[6] = { ["WP"]=3321, ["AM"]=3321, ["AC"]=3321, ["RT"]=3327 }

function ItemDisplayExplainBuildPage1(kSelf)
	local Index = kSelf:GetBuildIndex();
	
	local Text1 = kSelf:GetControl("FRM_TEXT1");
	local Text2 = kSelf:GetControl("FRM_TEXT2");
	local Text3 = kSelf:GetControl("FRM_TEXT3");
	local Text4 = kSelf:GetControl("FRM_TEXT4");
	local Text5 = kSelf:GetControl("FRM_TEXT5");

	if Text1:IsNil() or Text2:IsNil() or Text3:IsNil() or Text4:IsNil() or Text5:IsNil() then return end

	if Index == 4 then
		Text1:SetStaticText(GetTT(403278):GetStr())
	elseif Index > 4 then
		Text1:SetStaticText(GetTT(3314 + Index):GetStr())
	else
		Text1:SetStaticText(GetTT(3315 + Index):GetStr())
	end

	Text2:SetStaticText(GetTT(TBL_DISPLAY_GET_RATE[Index]["WP"]):GetStr())
	Text3:SetStaticText(GetTT(TBL_DISPLAY_GET_RATE[Index]["AM"]):GetStr())
	Text4:SetStaticText(GetTT(TBL_DISPLAY_GET_RATE[Index]["AC"]):GetStr())
	Text5:SetStaticText(GetTT(TBL_DISPLAY_GET_RATE[Index]["RT"]):GetStr())
end

function ItemExplainPageBuildButtonText(kSelf)
	local Index = kSelf:GetBuildIndex();
	----------------------------------------------------------------------------
	--미개방 컨텐츠 #NOT_OPEN#(개방시 삭제) 북의 '아이템의 등급' 메뉴
	if(GetLocale() ~= LOCALE.NC_DEVELOP) then  
		if Index == 2 then
			kSelf:SetStaticTextW(GetTextW(3328))
		else
			kSelf:SetStaticTextW(GetTextW(UISelf:GetBuildIndex()+3175))		 
		end
	----------------------------------------------------------------------------
	else 
	if Index == 3 then
		kSelf:SetStaticTextW(GetTextW(3328))
	elseif Index > 3 then
		kSelf:SetStaticTextW(GetTextW(UISelf:GetBuildIndex()+3174))
	else
		kSelf:SetStaticTextW(GetTextW(UISelf:GetBuildIndex()+3175))
	end
end
end


g_markOrder = {}
g_markOrder["BTN_EXCLAM"] = {14,19,9,21,23,16,12,41,42,32}
g_markOrder["BTN_QUESTION"] = {13,18,8,20,22,15,10,43,31}

function UI_Book_Explain_Mark(wnd)
	if wnd:IsNil() then return end
	if wnd:GetCheckState() then
		return
	end
	local prnt = wnd:GetParent()
	if prnt:IsNil() then return end

	local lst = prnt:GetControl("LST_MARK")
	if lst:IsNil() then return end
	lst:ClearAllListItem()

	local ltext = prnt:GetControl("FRM_MARK_EXPLAN")
	if ltext:IsNil() then return end

	local add = -1
	local otherbtnName = "BTN_EXCLAM"
	local otherbtn = prnt:GetControl(otherbtnName)
	local name = wnd:GetID():GetStr()
	if name=="BTN_QUESTION" then
		add = 9
		ltext:SetStaticTextW(GetTextW(425))
	elseif name =="BTN_EXCLAM" then
		otherbtn = prnt:GetControl("BTN_QUESTION")
		ltext:SetStaticTextW(GetTextW(424))
	else
		SetBreak()
	end

	wnd:LockClick(true)
	wnd:CheckState(true)
	if false==otherbtn:IsNil() then
		otherbtn:LockClick(false)
		otherbtn:CheckState(false)
	end

	for i=1,9 do
		local item = lst:AddNewListItemChar(tostring(i))
		local itemwnd = item:GetWnd()
		if false==itemwnd:IsNil() then
			itemwnd:SetStaticText("")
			local textwnd = itemwnd:GetControl("FRM_TEXT")
			if false==textwnd:IsNil() then 
				textwnd:SetStaticTextW(GetTextW(430+add+i))
			end
			local img = itemwnd:GetControl("FRM_IMG")
			if false==img:IsNil() then
				img:SetUVIndex(g_markOrder[name][i])
			end
		end
	end
	if -1 == add then
		local item = lst:AddNewListItemChar(tostring(i))
		local itemwnd = item:GetWnd()
		if false==itemwnd:IsNil() then
			itemwnd:SetStaticText("")
			local textwnd = itemwnd:GetControl("FRM_TEXT")
			if false==textwnd:IsNil() then 
				textwnd:SetStaticTextW(GetTextW(430+add+10))
			end
			local img = itemwnd:GetControl("FRM_IMG")
			if false==img:IsNil() then
				img:SetUVIndex(g_markOrder[name][10])
			end
		end
	end
end
