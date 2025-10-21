TBL_ENCHANT_SHIFT_UI_LOC = {}
TBL_ENCHANT_SHIFT_UI_LOC[0] = { ["SLOT_X"] = 56,  ["SLOT_Y"] = 173, ["INS_X"] = 0,  ["ANI_X"] =  23, ["ANI_Y"] = -17, ["RST_X"] = 41, ["DEREG_BTN_X"] = 0,  ["REG_BTN_X"] = 27, ["PERCENT_T_X"] = 41, ["ARROW_X"] = 11, ["CV_TT"] = GetTT(1255):GetStr(), ["CV_COLOR"] = 0xFFFF3E3E }
TBL_ENCHANT_SHIFT_UI_LOC[1] = { ["SLOT_X"] = 23,  ["SLOT_Y"] = 241, ["INS_X"] = 0,  ["ANI_X"] =  23, ["ANI_Y"] = -17, ["RST_X"] = 41, ["DEREG_BTN_X"] = 0,  ["REG_BTN_X"] = 27, ["PERCENT_T_X"] = 41, ["ARROW_X"] = 11, ["CV_TT"] = "-2", ["CV_COLOR"] = 0xFFFF3E3E }
TBL_ENCHANT_SHIFT_UI_LOC[2] = { ["SLOT_X"] = 56,  ["SLOT_Y"] = 307, ["INS_X"] = 0,  ["ANI_X"] =  23, ["ANI_Y"] = -17, ["RST_X"] = 41, ["DEREG_BTN_X"] = 0,  ["REG_BTN_X"] = 27, ["PERCENT_T_X"] = 41, ["ARROW_X"] = 11, ["CV_TT"] = "-1", ["CV_COLOR"] = 0xFFFF3E3E }
TBL_ENCHANT_SHIFT_UI_LOC[3] = { ["SLOT_X"] = 187, ["SLOT_Y"] = 307, ["INS_X"] = 41, ["ANI_X"] = -18, ["ANI_Y"] = -17, ["RST_X"] = 0,  ["DEREG_BTN_X"] = 41, ["REG_BTN_X"] = 68, ["PERCENT_T_X"] = 0,  ["ARROW_X"] = 52, ["CV_TT"] = "0",  ["CV_COLOR"] = 0xFF00FFFF }
TBL_ENCHANT_SHIFT_UI_LOC[4] = { ["SLOT_X"] = 220, ["SLOT_Y"] = 241, ["INS_X"] = 41, ["ANI_X"] = -18, ["ANI_Y"] = -17, ["RST_X"] = 0,  ["DEREG_BTN_X"] = 41, ["REG_BTN_X"] = 68, ["PERCENT_T_X"] = 0,  ["ARROW_X"] = 52, ["CV_TT"] = "+1", ["CV_COLOR"] = 0xFF00FFFF }
TBL_ENCHANT_SHIFT_UI_LOC[5] = { ["SLOT_X"] = 187, ["SLOT_Y"] = 173, ["INS_X"] = 41, ["ANI_X"] = -18, ["ANI_Y"] = -17, ["RST_X"] = 0,  ["DEREG_BTN_X"] = 41, ["REG_BTN_X"] = 68, ["PERCENT_T_X"] = 0,  ["ARROW_X"] = 52, ["CV_TT"] = "+2", ["CV_COLOR"] = 0xFF00FF00 }

function SetSlotLocation(Slot)
	if Slot:IsNil() then
		return;
	end
	
	local index = Slot:GetBuildIndex();
	local SlotTBL = TBL_ENCHANT_SHIFT_UI_LOC[ index ];
	
	local kPos = Slot:GetLocation();
	kPos:SetX(SlotTBL["SLOT_X"]);
	kPos:SetY(SlotTBL["SLOT_Y"]);
	Slot:SetLocation(kPos);
	
	local kTemp = Slot:GetControl("IMG_SELECT");
	if false == kTemp:IsNil() then
		kPos = kTemp:GetLocation();
		kPos:SetX(SlotTBL["ANI_X"]);
		kPos:SetY(SlotTBL["ANI_Y"]);
		kTemp:SetLocation(kPos);
	end
	
	local kTemp = Slot:GetControl("IMG_SELECT_ANI");
	if false == kTemp:IsNil() then
		kPos = kTemp:GetLocation();
		kPos:SetX(SlotTBL["ANI_X"]);
		kPos:SetY(SlotTBL["ANI_Y"]);
		kTemp:SetLocation(kPos);
	end
			
	local kTemp = Slot:GetControl("ICN_RST");
	if false == kTemp:IsNil() then
		kPos = kTemp:GetLocation();
		kPos:SetX(SlotTBL["RST_X"]);
		kTemp:SetLocation(kPos);
	end
	
	local kTemp = Slot:GetControl("FRM_RESULT_CV");
	if false == kTemp:IsNil() then
		kPos = kTemp:GetLocation();
		kPos:SetX(SlotTBL["RST_X"]);
		kTemp:SetLocation(kPos);
		kTemp:SetStaticText(SlotTBL["CV_TT"]);
		kTemp:SetFontColor(SlotTBL["CV_COLOR"]);
	end

	kTemp = Slot:GetControl("IMG_CLOSE");
	if false == kTemp:IsNil() then
		kPos = kTemp:GetLocation();
		kPos:SetX(SlotTBL["RST_X"]);
		kTemp:SetLocation(kPos);
	end

	kTemp = Slot:GetControl("FRM_PER");
	if false == kTemp:IsNil() then
		kPos = kTemp:GetLocation();
		kPos:SetX(SlotTBL["PERCENT_T_X"]);
		kTemp:SetLocation(kPos);
	end

	local bVisible = index ~= 5;
	kTemp = Slot:GetControl("ICN_RATEUP");
	if false == kTemp:IsNil() then
		kPos = kTemp:GetLocation();
		kPos:SetX(SlotTBL["INS_X"]);
		kTemp:SetLocation(kPos);
		kTemp:Visible(bVisible);
	end

	kTemp = Slot:GetControl("BTN_DEREG");
	if false == kTemp:IsNil() then
		kPos = kTemp:GetLocation();
		kPos:SetX(SlotTBL["DEREG_BTN_X"]);
		kTemp:SetLocation(kPos);
		kTemp:Visible(bVisible);
	end

	kTemp = Slot:GetControl("BTN_REG");
	if false == kTemp:IsNil() then
		kPos = kTemp:GetLocation();
		kPos:SetX(SlotTBL["REG_BTN_X"]);
		kTemp:SetLocation(kPos);
		kTemp:Visible(bVisible);
	end

	kTemp = Slot:GetControl("SFRM_ARROW_SLOT");
	if false == kTemp:IsNil() then
		kPos = kTemp:GetLocation();
		kPos:SetX(SlotTBL["ARROW_X"]);
		kTemp:SetLocation(kPos);
		kTemp:Visible(bVisible);
	end
end

function SetSlotCVLocation(Slot)
	if Slot:IsNil() then
		return;
	end
	
	local index = Slot:GetBuildIndex();
	local SlotTBL = TBL_ENCHANT_SHIFT_UI_LOC[ index ];
	local CalcX = SlotTBL["SLOT_X"];
	if index < 3 then
		CalcX = CalcX + 41;
	end
	
	local kPos = Slot:GetLocation();
	kPos:SetX(CalcX);
	kPos:SetY(SlotTBL["SLOT_Y"]);
	Slot:SetLocation(kPos);
	Slot:SetStaticText(SlotTBL["CV_TT"]);
	Slot:SetFontColor(SlotTBL["CV_COLOR"]);
end

function OnCallHelpSelect(kWndName,iSpace)
	local kWnd = CallUI(kWndName)
	if kWnd:IsNil() then return end
	local kScript = kWnd:GetControl("SFRM_SCRIPT")
	if kScript:IsNil() then return end
	local kText = nil
	local kLoc = nil
	local kSize = nil
	local iY = 0
	for i=1,8 do
		kText = kScript:GetControl("FRM_TEXT"..i)
		if false==kText:IsNil() then
			kLoc = kText:GetLocation()
			if 0==iY then
				iY = kLoc:GetY()
			end
			kSize = kText:GetTextSize()
			kText:SetLocation( Point2(kLoc:GetX(),iY) )
			iY = iY + kSize:GetY() + iSpace
		end
	end
end
