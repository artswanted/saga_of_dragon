function UI_RefreshSkillIconPet(wnd, iSkillNo, iResNo, bGray)
	if true==wnd:IsNil() then return end

	if wnd:IsNil() == true then
		return
	end

	wnd:SetCustomDataAsInt(iSkillNo)

	wnd:RegistScript("ON_DISPLAY", "UISelf:DrawIcon()")
	wnd:RegistScript("ON_R_BTN_DOWN", "UISelf:IconDoAction()")
	wnd:RegistScript("ON_MOUSE_OVER", "UISelf:CallIconToolTip()")
	wnd:RegistScript("ON_MOUSE_OUT", "CloseToolTip()")
	wnd:RegistScript("ON_L_BTN_DOWN", "UISelf:IconToCursor()")

	wnd:SetIconKeyAndResNo(iSkillNo, iResNo)
	wnd:SetGrayScale(bGray)
end
