g_currentInvPage = 0
g_maxInvPage = 2

function CheckPage(kSelf, iPageNo)
    if(nil == kSelf) then return end
    local kParent = kSelf:GetParent():GetParent()
    g_currentInvPage = iPageNo
	for i=0, g_maxInvPage do
		local kBtnPage = kSelf:GetControl("BTN_PAGE"..i)
		if false == kBtnPage:IsNil() then
			local kCheckBtn = kBtnPage:GetControl("CBTN_PAGE"..i)
            if false == kCheckBtn:IsNil() then
                if iPageNo == i then
                    kCheckBtn:CheckState(true)
                    kCheckBtn:LockClick(true)
                else
                    kCheckBtn:CheckState(false)
                    kCheckBtn:LockClick(false)
                end
            end
		end
	end
    SetInvViewSize(GetUIWnd("Inv"), g_currentInvPage)
end
