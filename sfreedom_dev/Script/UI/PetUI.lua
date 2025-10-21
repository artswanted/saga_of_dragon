g_UseItemForChangeColor = true
g_UseItemForChangeName = true

--if GetLocale() == LOCALE.NC_??????? then
--	g_UseItemForChangeColor = false
--	g_UseItemForChangeName = false
--end

g_PetItemSetInfo = {}
g_PetItemSetInfo[0] = {["TTW"]=70006}	--무기
g_PetItemSetInfo[1] = {["TTW"]=70008}	--목걸이
g_PetItemSetInfo[2] = {["TTW"]=70009}	--귀걸이
g_PetItemSetInfo[3] = {["TTW"]=70010}	--반지

if GetLocale() == LOCALE.NC_CHINA 
	or GetLocale() == LOCALE.NC_JAPAN 
	or GetLocale() == LOCALE.NC_THAILAND 
	or GetLocale() == LOCALE.NC_INDONESIA 
	or GetLocale() == LOCALE.NC_PHILIPPINES
	or GetLocale() == LOCALE.NC_TAIWAN then
		g_UseItemForChangeColor = true
		g_UseItemForChangeName = true
end

function IsPetAlive(kPilot)
	if nil==kPilot or kPilot:IsNil() then
		kPilot = g_pilotMan:GetPlayerPilot()
	end
	local kUnit = kPilot:GetUnit()
	if kUnit:IsNil() then return false end
	local kPetGuid = kUnit:GetSelectedPet()
	local kPetPilot = g_pilotMan:FindPilot(kPetGuid)
	if kPetPilot:IsNil() then return false end	--진짜 펫 찾자
	
	return kPetPilot:IsAlive()
end

function CallPetRenameChk(bUseItem)
	local kPlayerPilot = g_pilotMan:GetPlayerPilot()
	if false == kPlayerPilot:IsNil() then
		local bCheck = false
		local iTTW = 403078
		if bUseItem then
			bCheck = CheckPetNameChange(bUseItem)
			iTTW = 161
		else
			local iHaveMoney = kPlayerPilot:GetAbil64(AT_MONEY)
			bCheck = (false == iHaveMoney:IsBig( GetPetRenamePayInt64() ) )
		end
		if false == bCheck then
			CommonMsgBoxByTextTable( iTTW, true )
			return
		end

		local kPopUp = ActivateUI( "FRM_RENAME_PET_CHECK", true)
		if false == kPopUp:IsNil() then
			local kOKBtn = kPopUp:GetControl("BTN_OK")
			if false == kOKBtn:IsNil() then
				kOKBtn:SetCustomDataAsInt(0)
			end
			
			local kMsg = kPopUp:GetControl("FRM_MESSAGE")
			if false == kMsg:IsNil() then
				if bUseItem then
					kMsg:SetStaticText(  GetTextW(7511):GetStr() )
				else
					kMsg:SetStaticText( string.format( GetTextW(7500):GetStr(), GetPetRenamePay():GetStr() ))
				end
			end
			
			local kPacket = NewPacket()
			kPacket:PushByte(7)		-- IT_FIT
			kPacket:PushByte(26)	-- EQUIP_POS_PET
			kPopUp:SetCustomDataAsPacket( kPacket )
			DeletePacket( kPacket )
		end
	end
end

function CallPetDyeingChk(iData, bUstItem)
	local kPlayerPilot = g_pilotMan:GetPlayerPilot()
	if false == kPlayerPilot:IsNil() then

		local iRet = CheckPetColorChange(iData, bUstItem)
		if 0 == iRet then
			CommonMsgBoxByTextTable( 159, true )
		elseif 1 == iRet then
			CommonMsgBoxByTextTable( 160, true )		
		else
			if false==bUstItem then
				local iHaveMoney = kPlayerPilot:GetAbil64(AT_MONEY)
				if true == iHaveMoney:IsBig( GetPetDyeingPayInt64() ) then
					-- 돈이 모자라다
					CommonMsgBoxByTextTable( 403078, true )
				else
					local kPopUp = ActivateUI( "FRM_PET_COLOR_CHANGE_CHECK", true )
					if false == kPopUp:IsNil() then
						if nil==iData then iData = 0 end
						kPopUp:SetCustomDataAsInt(iData)

						local kMsg = kPopUp:GetControl("FRM_MESSAGE")
						if false == kMsg:IsNil() then
							kMsg:SetStaticText( string.format( GetTextW(7507):GetStr(), GetPetDyeingPay():GetStr() ))
						end
					end
				end
			else	--아이템을 사용하는것이면
				local kPopUp = ActivateUI( "FRM_PET_COLOR_CHANGE_CHECK", true )
				if false == kPopUp:IsNil() then
					if nil==iData then iData = 0 end
					kPopUp:SetCustomDataAsInt(iData)

					local kMsg = kPopUp:GetControl("FRM_MESSAGE")
					if false == kMsg:IsNil() then
						kMsg:SetStaticText( GetTextW(7510):GetStr() )
					end
				end
			end
		end
	end
end

--랜덤 염색 기능은 게임머니로 사용불가. (g_UseItemForChangeColor는 체크 하지 않고 오로지 true로 간주함)
function CallPetDyeingRandColorChk(iData)
	local kPlayerPilot = g_pilotMan:GetPlayerPilot()
	if false == kPlayerPilot:IsNil() then

		local iRet = CheckPetColorChange(iData, true)
		if 0 == iRet then
			CommonMsgBoxByTextTable( 159, true )
		elseif 1 == iRet then
			CommonMsgBoxByTextTable( 160, true )		
		else
			local kPopUp = ActivateUI( "FRM_PET_RAND_COLOR_CHANGE_CHECK", true )
			if false == kPopUp:IsNil() then
				if nil==iData then iData = 0 end
				kPopUp:SetCustomDataAsInt(iData)

				local kMsg = kPopUp:GetControl("FRM_MESSAGE")
				if false == kMsg:IsNil() then
					kMsg:SetStaticText( string.format( GetTextW(7519):GetStr(), GetPetDyeingRandColorMax() ))
				end
			end
		end
	end
end

function CallPetRename( kUI )
	ODS("CallPetRename\n", false, 1509)
	local kParent = kUI:GetParent()
	if false == kParent:IsNil() then
		local iMode = kUI:GetCustomDataAsInt()
		if 1 == iMode then
			local kPacket = kParent:GetCustomDataAsPacket()
			if false==kPacket:IsNil() and false==kPacket:Empty() then
				Net_Send(kPacket)
				DeletePacket(kPacket)
			else
			end
			CloseUI("SFRM_RENAME_PET")
		else
			local kRenameUI = ActivateUI("SFRM_RENAME_PET", true)
			if false == kRenameUI:IsNil() then
				kParent:CopyCustomData(kRenameUI)
			end
		end
		
		kParent:Close()
	end
end

function CheckPetRename( kUI, bUseItem )
	if false == kUI:IsNil() then
		local kEdit = kUI:GetControl("EDIT_NAME")
		
		if false == kEdit:IsNil() then
			local bResult = CheckPetNameFiltering(kEdit:GetEditText())
			if 1 == bResult then
				CommonMsgBoxByTextTable( 51065, true )
			elseif 2 == bResult then
				CommonMsgBoxByTextTable( 158, true )
			else
			    kUI:Close()
				local kPopUp = ActivateUI( "FRM_RENAME_PET_CHECK", true)
				if false == kPopUp:IsNil() then
				
					local kOKBtn = kPopUp:GetControl("BTN_OK")
					if false == kOKBtn:IsNil() then
						kOKBtn:SetCustomDataAsInt(1)
					end
				
					local kMsg = kPopUp:GetControl("FRM_MESSAGE")
					if false == kMsg:IsNil() then
						kMsg:SetStaticText( string.format( GetTextW(7502):GetStr(), kEdit:GetEditText():GetStr() ))
					end
					
					local kUIPacket = kUI:GetCustomDataAsPacket()
					Net_PT_C_M_REQ_PET_RENAME(kUIPacket, kEdit:GetEditText(), kPopUp, bUseItem)
					DeletePacket( kUIPacket )
				end
			end
		end
	end
end

function UI_PetColorClick(wnd, bInit)
	if true==wnd:IsNil() then return end
	local wndLoc = wnd:GetLocation()
	local kTotalLoc = wnd:GetTotalLocation()
	local kClickLoc = GetCursorPos()
	kClickLoc:IncX(-kTotalLoc:GetX())
	kClickLoc:IncY(-kTotalLoc:GetY())
	local xpos = math.floor(kClickLoc:GetX()/20)
	local ypos = math.floor(kClickLoc:GetY()/20)
	local iIndex = xpos + ypos*4 + 1	--0이면 기본. 1베이스
	if nil~=bInit and true==bInit then	--펫 기본 어빌을 갖고오자
		local kPlayer = g_pilotMan:GetPlayerUnit()
		if false==kPlayer:IsNil() then
			local kPetGuid = kPlayer:GetSelectedPet()
			if false==kPetGuid:IsNil() then
				local kPet = g_pilotMan:FindPilot(kPetGuid)
				if false==kPet:IsNil()  then
					iIndex = kPet:GetAbil(AT_COLOR_INDEX)
					iIndex = iIndex - 1
					if 1>=iIndex then iIndex = 0 end
					xpos = math.floor(iIndex%4)
					ypos = math.floor(iIndex/4)
				end
			end
		end
		iIndex = iIndex + 1
	end
	local kSelect = wnd:GetParent():GetControl("FRM_COLOR_SELECT")
	if false==kSelect:IsNil() then
		kSelect:SetLocation(Point2(xpos*20+wndLoc:GetX(), ypos*20+wndLoc:GetY()))
	end
	wnd:GetParent():SetCustomDataAsInt(iIndex)
end

function UI_PetInfoTab(wnd, iIdx)
	if wnd:IsNil() then return end
	local kTbl = {}
	kTbl[0]={["BTN"]="CBTN_INFO", ["FRM"]="FRM_INFO"}
	kTbl[1]={["BTN"]="CBTN_SKILL", ["FRM"]="FRM_SKILL"}

	local iN = table.getn(kTbl)
	local kParent = wnd:GetParent()
	if kParent:IsNil() then return end
	for i=0,iN do
		local bThisTab = (iIdx==i)
		local kBtn = kParent:GetControl(kTbl[i]["BTN"])
		local kFrm = kParent:GetControl(kTbl[i]["FRM"])
		if false==kBtn:IsNil() then
			kBtn:SetClickLock(bThisTab)
			kBtn:CheckState(bThisTab)
		end

		if false==kFrm:IsNil() then
			kFrm:Visible(bThisTab)
		end
	end
end

function UI_SwitchToRidingPetInfo(kWndNormalPet, kWndRidingPet, bThisTab, bRidingPet) --펫 정보탭이 활성화 되었을 때 일반펫이냐 라이딩펫이냐에 따라 내부 폼을 선택해준다. (라이딩펫은 스탯표시를 안해줌)
--UI롤백. 현재 사용하지 않음
	local bShowNormalPetWnd = false
	local bShowRidingPetWnd = false
	
	if bThisTab then
		if bRidingPet and kWndRidingPet:IsNil() == false then
			bShowRidingPet = true
			bShowNormalPet = false
		else
			bShowRidingPet = false
			bShowNormalPet = true
		end
	else
		bShowRidingPet = false
		bShowNormalPet = false
	end

	if kWndNormalPet:IsNil() == false then
		kWndNormalPet:Visible(bShowNormalPet)
	end
	if kWndRidingPet:IsNil() == false then
		kWndRidingPet:Visible(bShowRidingPet)
	end
end

function UI_ShowHidePetEquipSlot(wnd, bShow)
	if wnd:IsNil() then
		return
	end
	
	local kWndDisable = wnd
	for i=0,4 do
		kWndDisable = wnd:GetControl("IMG_COSTUM_ICON_BG"..i)
		if kWndDisable:IsNil() == false then
			kWndDisable:Visible(bShow)
			kWndDisable = kWndDisable:GetControl("IMG_COSTUM")
			if kWndDisable:IsNil() == false then
				kWndDisable:Visible(bShow)
			end			
		end
		
		kWndDisable = wnd:GetControl("IMG_COSTUM_ICON_BG2_"..i)
		if kWndDisable:IsNil() == false then
			kWndDisable:Visible(bShow)
		end
	end
	
	for i = 0,7 do
		local kWndIcon = wnd:GetControl("FRM_EQUIP_ICON"..i)
		if kWndIcon:IsNil() == false then
			kWndIcon:Visible(bShow)
		end
	end
end

function UI_SetPosBeside(wnd, szName)
	if wnd:IsNil() then return end
	local kEquip = GetUIWnd("CharInfo")
	if kEquip:IsNil() then return end
	local kLoc = kEquip:GetLocation()
	local kSize = kEquip:GetSize()
	kLoc:IncX(kSize:GetX()-3)
	kLoc:IncY(kSize:GetY()-wnd:GetSize():GetY())
	wnd:SetLocation(kLoc)
end

function UI_PetInfo_Popup(wnd)
	if wnd:IsNil() then return end
	local kPop = wnd:GetParent():GetControl("SFRM_PETINFO_POPUP")--CallUI("SFRM_PETINFO_POPUP")
	if kPop:IsNil() then return end
	kPop:Visible(true)
	local kLoc = wnd:GetLocation()--wnd:GetTotalLocation()

	kLoc:IncX(-114)
	kLoc:IncY(20)

	kPop:SetLocation(kLoc)
end
