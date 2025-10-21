
g_bPostOpenInv = false

function OnClickPostCheckBtn(kWnd)
	if nil==kWnd or kWnd:IsNil() then return end
	
	if 1 == kWnd:GetBuildIndex() then
		local kParent = kWnd:GetParent()
		local inv = GetUIWnd("Inv")
		if inv:IsNil() then
			inv = CallUI("Inv")
			g_bPostOpenInv = true
		end
		if inv:IsNil() then return end
		local kPos = kParent:GetLocation()
		local kNewPos = Point2()
		kNewPos:SetX(kPos:GetX()+kParent:GetSize():GetX()+30)
		kNewPos:SetY(kPos:GetY())
		inv:SetLocation(kNewPos)
		
		ClearPostSendItem()
	else
		if true==g_bPostOpenInv then
			g_bPostOpenInv = false
			CloseUI("Inv")
		end
	end
end

function CloseMailUI()
	if true==g_bPostOpenInv then
		g_bPostOpenInv = false
		CloseUI("Inv")
	end
	CloseToolTip()
	ClosePostUI()
end

function OnClickPostAllSelect(kWnd)
	if nil==kWnd or kWnd:IsNil() then return end
	local bCheckState = not kWnd:GetCheckState()
	
	local kParent = kWnd:GetParent()
	if kParent:IsNil() then return end
	
	local Builder = kParent:GetControl("BLD_LETTER")
	if Builder:IsNil() then return end
	local cnt = Builder:GetBuildCount():GetY() - 1
	for i=0,cnt do
		local kLetter = kParent:GetControl("FRM_LETTER"..i)
		if false==kLetter:IsNil() then 
			local kCBtn = kLetter:GetControl("CBTN_CHECK")
			if false==kCBtn:IsNil() and true==kCBtn:IsVisible() then 
				kCBtn:CheckState(bCheckState)
			end
		end
	end	
end

function ClearPostAllSelect()
	local kWnd = GetUIWnd("SFRM_POST")
	if nil==kWnd or kWnd:IsNil() then return end
	
	local kPost = kWnd:GetControl("FRM_POST0")
	if nil==kPost or kPost:IsNil() then return end
	if false==kPost:IsVisible() then return end
	
	local kCheck = kPost:GetControl("BTN_ALL")
	if nil==kCheck or kCheck:IsNil() then return end
	kCheck:CheckState(false)
end

g_OnClickPostSendItem = false
function OnLBtnDownPostSendItem()
	g_OnClickPostSendItem = true
end

function OnLBtnUpPostSendItem(wnd)
	if nil==wnd or wnd:IsNil() then return end
	
	if false == g_OnClickPostSendItem then
		wnd:CursorToIcon()
	end
	g_OnClickPostSendItem = false
end