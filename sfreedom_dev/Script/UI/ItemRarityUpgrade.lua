function SelectSoulCraftByIndex( Index )
	local type = Index
	wnd = ItemRarityUpgradeWnd(UISelf)
	if 6==type then
		type = 0
	end
	wnd:SetAttachElement(type)
	local kGuid = UISelf:GetParent():GetCustomDataAsGuid()
	UISelf:CloseParent()
	local lwUp = CallRarityUpgradeUI( kGuid )
	if false==lwUp:IsNil() then
		local lwTitle = lwUp:GetControl("SFRM_TITLE")
		if false==lwTitle:IsNil() then
			local lwTitleC = lwTitle:GetControl("SFRM_COLOR")
			if false==lwTitleC:IsNil() then
				local lwTitleS = lwTitleC:GetControl("SFRM_TITLE_SDW")
				if false==lwTitleS:IsNil() then
					local iTTW = 1434
					if 7==type then
						iTTW = 1446
					end
					lwTitleS:SetStaticTextW(GetTextW(iTTW))
				end
			end
		end
	end
end 

function Process_BuildUp(kSelf)

	kSelf:GetParent():GetControl("IMG_RESULT"):Visible(true)
	local wndMark = kSelf:GetParent():GetControl("IMG_RESULT")
	local iState = kSelf:GetCustomDataAsInt()
	
	local TimeCnt = kSelf:GetControl("FRM_DELAY"):GetCustomDataAsFloat()
	TimeCnt = TimeCnt+GetFrameTime()
	kSelf:GetControl("FRM_DELAY"):SetCustomDataAsFloat(TimeCnt)
	
	local AniSpeed = kSelf:GetParent():GetCustomDataAsFloat()
	AniSpeed = AniSpeed - 0.0008
	kSelf:GetParent():SetCustomDataAsFloat(AniSpeed)
	
	if (TimeCnt < AniSpeed) then
		return
	end
	
	kSelf:GetControl("FRM_DELAY"):SetCustomDataAsFloat(GetFrameTime())
	
	local iMaxIndex = kSelf:GetControl("FRM_MAX_INDEX"):GetCustomDataAsInt()
	iMaxIndex = iMaxIndex-1
	iMaxIndex = 180 + (iMaxIndex*64)
	local wndMark = kSelf:GetParent():GetControl("IMG_RESULT")
	local iState = kSelf:GetCustomDataAsInt()
	
	PlaySound("../Sound/U_Sound/button(class).wav")
	if 1 == iState then
		local kPos = wndMark:GetLocation()
		kPos:IncX(64)
		
		if (kPos:GetX() > iMaxIndex )then
			kSelf:SetCustomDataAsInt(2)
		else
			wndMark:SetLocation(kPos)
		end
	end
	
	if 2 == iState then
		local kPos = wndMark:GetLocation()
		kPos:IncX(-64)
		
		if (180 > kPos:GetX()) then
			kSelf:SetCustomDataAsInt(1)
		else
			wndMark:SetLocation(kPos)
		end
	end   
end
