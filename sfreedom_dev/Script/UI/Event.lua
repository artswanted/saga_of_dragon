-- 테스트 하는 코드
-- CommonMsgBoxByTextTable(UISelf:GetBuildIndex()+2359, true)
-- CommonMsgBox((UISelf:GetBuildIndex()+1)^6, true)
-- local Cost = Parent:GetParent():GetControl("SFRM_LS_COSTMONEY")
-- if false==Cost:IsNil() then
	-- Cost:SetCustomDataAsInt( (UISelf:GetBuildIndex()+1)^6 )
-- end
-- 빌드는 0부터 시작하고 SelectStar인덱스 저장은 +1해서 함

g_LuckyStar = {}
g_LuckyStar["IconAni"] = true
g_LuckyStar["BackPopup"] = ""
g_LuckyStar["SelectStar"] = 0
g_LuckyStar["AutoPopup"] = true
g_LuckyStar["IconAniTick"] = 0.0

function LuckyStarUI_Clear()
	g_LuckyStar["IconAni"] = true
	g_LuckyStar["BackPopup"] = ""
	g_LuckyStar["SelectStar"] = 0
	g_LuckyStar["AutoPopup"] = true
	g_LuckyStar["IconAniTick"] = 0.0
end

function LuckyStarUI_SelectStarClear() 
	g_LuckyStar["SelectStar"] = 0
end

function LuckyStarUI_IconVisible(bValue)
	local kIconUI = GetUIWnd("BTN_LUCKYSTAR_ICON")
	if true == kIconUI:IsNil() then return end
	
	kIconUI:Visible(bValue)
end

function LuckyStarUI_Visible()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if false == GetUIWnd("FRM_CHAR_LIST"):IsNil() then return false end
	if g_world:IsHaveWorldAttr(GATTR_FLAG_MYHOME) then return false end
	
	return true
end

function LuckyStarUI_Animation()
	g_LuckyStar["IconAniTick"] = 3
end

function LuckyStarUI_NextEventTimeToolTip()	
	if true==LuckyStarNextEventTime():IsNil() then return end
	
	local ToolTipText = GetTextW(2374):GetStr() .. "\n" .. LuckyStarNextEventTime():GetStr()
	CallMutableToolTipText(WideString(ToolTipText), GetUIWnd("Cursor"):GetLocation())
end

function OnBuildStarIcon(UISelf, Index)
	if true == UISelf:IsNil() then return end
	
	local ImgPath = string.format("../Data/6_ui/event/evtLsB%02d.tga", Index)
	UISelf:ButtonChangeImage(ImgPath)
end

function OnTick_LuckyStar(UISelf)
	if(true == UISelf:IsNil()) then return end
	
	g_LuckyStar["IconAniTick"] = g_LuckyStar["IconAniTick"] - GetFrameTime()
	
	
	if true == g_LuckyStar["IconAni"] and g_LuckyStar["IconAniTick"] > 0 then
		UVAnimation(UISelf, 0.2)
	else
		UISelf:SetUVIndex(1)
	end
end

function LuckyStarIconAni(bValue)
	g_LuckyStar["IconAni"] = bValue	
end

function LuckyStarUI_CheckAutoPopup(UISelf)
	if(true == UISelf:IsNil()) then return end
	
	g_LuckyStar["AutoPopup"] = IsLuckyStarAutoPopup()
	UISelf:CheckState(g_LuckyStar["AutoPopup"])	
end

function LuckyStarUI_UpdateAutopopState()
	if false==g_LuckyStar["AutoPopup"] then
		g_LuckyStar["AutoPopup"] = true
	else
		g_LuckyStar["AutoPopup"] = false
	end
	
	LuckyStarAutoPopup(g_LuckyStar["AutoPopup"])
end

function LuckyStarUI_CheckSelectStar( UISelf )	
	if true==UISelf:IsNil() then return end
	
	local UIParent = UISelf:GetParent()	
	if true==UIParent:IsNil() then return end
	for i=0,11 do
		local UIBtn = UIParent:GetControl("CHK_STAR"..i)
		if false==UIBtn:IsNil() and UISelf:GetBuildIndex() ~= i then
			UIBtn:CheckState(false)
		end
	end
	
	if true==UISelf:GetCheckState() then
		g_LuckyStar["SelectStar"] = 0
	else
		g_LuckyStar["SelectStar"] = UISelf:GetBuildIndex()+1
	end
end

--행운성 선택
function LuckyStarUI_ReAckOK(UIMain)
	if true==UIMain:IsNil() then return end
	
	if 0==g_LuckyStar["SelectStar"] then
		CommonMsgBoxByTextTable(2331, true, UIMain)
		return
	end

	local UIAsk = ActivateUI("SFRM_LS_ASK_SEND")
	if true==UIAsk:IsNil() then 
		LuckyStarPopupClose(UIMain)
		return 
	end
	
	g_LuckyStar["BackPopup"] = UIMain:GetID():GetStr()
	UIMain:Close()
end

function LuckyStarUI_ReAskInfoText(UISelf)
	if 0 < g_LuckyStar["SelectStar"] and g_LuckyStar["SelectStar"] < 13 then
		local str = string.format(GetTT(2346):GetStr(), GetTT(2358+g_LuckyStar["SelectStar"]):GetStr())
		UISelf:SetStaticText( str )
	end	
end

function LuckyStarUI_Back()		
	if nil == g_LuckyStar["BackPopup"] then
		return 
	end	
	
	ActivateUI(g_LuckyStar["BackPopup"])
end

function LuckyStarUI_Send() 
	local EventGuid = GetLuckyStarEventGuid()
	if true==EventGuid:IsNil() then return end
	if nil==g_LuckyStar["BackPopup"] then return end
	if 0==g_LuckyStar["SelectStar"] then return end
		
	local kPacket
	if "SFRM_LS_SELECTPOP"==g_LuckyStar["BackPopup"] then
		kPacket = NewPacket(19205)--PT_C_M_REQ_LUCKYSTAR_ENTER	
	elseif "SFRM_LS_CHANGEPOP"==g_LuckyStar["BackPopup"] then
		kPacket = NewPacket(19208)--PT_C_M_REQ_LUCKYSTAR_CHANGE_STAR
	end
	if Nil==kPacket then return end
	
	kPacket:PushGuid(EventGuid)
	kPacket:PushInt(g_LuckyStar["SelectStar"])
	Net_Send(kPacket)
	DeletePacket(kPacket)
	
	local UIBack = GetUIWnd(g_LuckyStar["BackPopup"])
	LuckyStarPopupClose(UIBack)
end

function LuckyStarUI_ChangeStar(UIMain)	
	ActivateUI("SFRM_LS_CHANGEPOP")	

	if false==UIMain:IsNil() then 
		UIMain:Close()
	end
end

function LuckyStarUI_IsOpenPopup()
	if false == GetUIWnd("SFRM_LS_INFOPOP"):IsNil() then return true end
	if false == GetUIWnd("SFRM_LS_SELECTPOP"):IsNil() then return true end
	if false == GetUIWnd("SFRM_LS_CHANGEPOP"):IsNil() then return true end
	if false == GetUIWnd("SFRM_LS_ASK_SEND"):IsNil() then return true end
	if false == GetUIWnd("SFRM_LS_RESULT_1"):IsNil() then return true end
	if false == GetUIWnd("SFRM_LS_RESULT_2"):IsNil() then return true end
	if false == GetUIWnd("SFRM_LS_RESULT_CUSTOM"):IsNil() then return true end
	
	return false
end

function LuckyStarUI_PopupOpen()
	if true == LuckyStarUI_IsOpenPopup() then return end
	
	LuckyStarPopupOpen()
end