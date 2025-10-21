-- Emporia Battle

g_EM_StartTime = 0
g_EM_TargetLoc = 0

function EMBattleResultCall( kUI )
	if kUI:IsNil() == false and g_world:IsNil() == false then
		g_EM_StartTime = g_world:GetAccumTime()	
		
		local kPos = kUI:GetLocation()
		g_EM_TargetLoc = kUI:GetLocation()
	end
end

function EMBattleResultDisplay( kUI )
	if kUI:IsNil() == false and g_world:IsNil() == false then
		local TimeGap = g_world:GetAccumTime() - g_EM_StartTime
		if TimeGap >= 0.2 then
			kUI:SetLocation(g_EM_TargetLoc)
		else
			local kPos = Point2( g_EM_TargetLoc:GetX() / ( 0.2 / TimeGap ), g_EM_TargetLoc:GetY() )
			kUI:SetLocation( kPos )
		end
	end
end

function EMBattleResultClose( kUI )
	if kUI:IsNil() == false then
		local iWin = kUI:GetCustomDataAsInt()
		if iWin > 0 then
			GetEventTimer():Add("EmporiaBattleResultClose", 0.3, "ActivateUI('FRM_EMBATTLE_WIN')", 1, false)
		else
			GetEventTimer():Add("EmporiaBattleResultClose", 0.3, "ActivateUI('FRM_EMBATTLE_LOSE')", 1, false)
		end
		
		kUI:SetLocation(g_EM_TargetLoc)
	end
end

function EMSubCoreTick( kUI )
	if kUI:IsNil() == false then
		local iTickCount = kUI:GetCustomDataAsInt()
		if iTickCount > 0 then
			iTickCount = iTickCount - 1
			
			local kImg = kUI:GetControl("IMG_AT")
			if false == kImg:IsNil() then
				local kUVIndex = kImg:GetUVIndex()
				if kUVIndex > 2 then
					kUVIndex = kUVIndex - 2
					kUI:SetFontColorRGBA(255,255,255,255)
				elseif iTickCount > 0 then
					kUVIndex = kUVIndex + 2
					kUI:SetFontColorRGBA(255,0,0,255)
				end
				
				kImg:SetUVIndex( kUVIndex )
			end
			
			kUI:SetCustomDataAsInt( iTickCount )
		end
	end
end

function EMRebirthCall( iRebirthTime, kType )
	ODS("리버스 시간 : "..iRebirthTime.."\n")
	local kUI = ActivateUI("FRM_EMBATTLE_REBIRTHTIME")
	if kUI:IsNil() == false then
		
		local kPos = kUI:GetLocation()
		
		if WAR_TYPE_PROTECTDRAGON == kType then
			kPos:SetY( 250 )
		else
			kPos:SetY( 185 )
		end
		
		kUI:SetLocation( kPos )
		
		local kCntUI = kUI:GetControl("IMG_REBIRTH_CNT")
		if kCntUI:IsNil() == false then
			iRebirthTime = iRebirthTime + 1
			kCntUI:SetStaticText( iRebirthTime )
			EMRebirthTick( kCntUI )
		end

		local kNONE = kUI:GetControl("FRM_ATK_TEAM_COLOR_NONE")
		local kATK = kUI:GetControl("FRM_ATK_TEAM_COLOR_ATK")
		local kDEF = kUI:GetControl("FRM_ATK_TEAM_COLOR_DEF")
		if not kNONE:IsNil() and not kATK:IsNil() and not kDEF:IsNil() then
			kNONE:Visible(true)
			kATK:Visible(false)
			kDEF:Visible(false)
		end
	end
end

function EMRebirthAtkColorChange( TeamNo )
	local kUI = ActivateUI("FRM_EMBATTLE_REBIRTHTIME")
	if kUI:IsNil() == false then
		local kNONE = kUI:GetControl("FRM_ATK_TEAM_COLOR_NONE")
		local kATK = kUI:GetControl("FRM_ATK_TEAM_COLOR_ATK")
		local kDEF = kUI:GetControl("FRM_ATK_TEAM_COLOR_DEF")
		if not kNONE:IsNil() and not kATK:IsNil() and not kDEF:IsNil() then
			kNONE:Visible(false);	
			kATK:Visible(false);	
			kDEF:Visible(false);	
			if TeamNo == 0 then
				kNONE:Visible(true)
			elseif TeamNo == 1 then
				kATK:Visible(true)
			else
				kDEF:Visible(true)
			end
		end
	end
end

function EMRebirthTick( kUI )
	if kUI:IsNil() == false then
		local iTime = tonumber(kUI:GetStaticText():GetStr())
		local kParent = kUI:GetParent()
		local kATK = kParent:GetControl("FRM_ATK_TEAM_COLOR_ATK")
		local kDEF = kParent:GetControl("FRM_ATK_TEAM_COLOR_DEF")

		local iEftTime = iTime - 1;
		if iEftTime > 0 then
			iEftTime = iEftTime - 1
		elseif iEftTime == 0 then
			iEftTime = 9			
		else
			iEftTime = 8
		end

		if iTime > 0 then
			iTime = iTime - 1
		else
			iTime = 9
		end

		if iTime == 0 then
			if false == kParent:IsNil() then
				local kHeart = kParent:GetControl("FRM_EMBATTLE_REBIRTHTIME_DUMMY")
				if false == kHeart:IsNil() then
					local Eft = kHeart:GetControl("IMG_REBIRTH_EFT")
					if false == Eft:IsNil() then
						Eft:Visible(true)
					end
				end
			end				
		end
		kUI:SetStaticText( iTime )
		kUI:SetUVIndex(10 - iTime)
		if not kATK:IsNil() and not kDEF:IsNil() then
			kATK:SetUVIndex(10 - iEftTime)
			kDEF:SetUVIndex(10 - iEftTime)
		end
	end
end

function EMReburthTickHeart(kUI)
	if kUI:IsNil() or (not kUI:IsVisible()) then
		return
	end
	kUI:SetSizeScale(kUI:GetSizeScale()+0.2, true)
	kUI:SetMaxAlpha(kUI:GetMaxAlpha()-0.05)
	if 3.5 < kUI:GetSizeScale() then
		kUI:Visible(false)
		kUI:SetSizeScale(1.0, true)
		kUI:SetMaxAlpha(1.0)
	end
end

function EMKillMsgTick( kUI )
	if kUI:IsNil() == false then
		local iCount = kUI:GetListItemCount()
		if iCount > 0 then
			local kItem = kUI:ListFirstItem()
			if kItem:IsNil() == false then
				local kItemWnd = kItem:GetWnd()
				local fStartTime = kItemWnd:GetCustomDataAsFloat()
				if (GetAccumTime() - fStartTime) > 6.0 then
					kUI:ListDeleteItem(kItem)
				end
			end
		end
	end
end

function OnClick_EmporiaReserve( kGUID )
	local kUI = ActivateUI("FRM_EMPORIA_BATTLE_RESERVER", true)
	if false == kUI:IsNil() then
		kUI:SetOwnerGuid( kGUID )
		ClearEditFocus()
		local kEdit = kUI:GetControl("EDT_EXP")
		if false == kEdit:IsNil() then
			kEdit:SetEditFocus(true)
		end
	end
end

function OnClick_EmporiaChallengeBattle( kGUID )
	local kUI = ActivateUI("FRM_EMPORIA_CHALLENGE_BATTLE", true)
	if false == kUI:IsNil() then
		kUI:SetOwnerGuid( kGUID )
		ClearEditFocus()
		local kEdit = kUI:GetControl("EDT_EXP")
		if false == kEdit:IsNil() then
			kEdit:SetEditFocus(true)
		end
	end
end

function RefreshEmporiaJoinBtn( kEmporiaJoinBtnUI, kEmporiaState )
	if false == kEmporiaJoinBtnUI:IsNil() then
		if EMPORIA_KEY_MINE == kEmporiaState then
			kEmporiaJoinBtnUI:SetStaticTextW(GetTextW(400460))
			kEmporiaJoinBtnUI:Disable(false)
		elseif EMPORIA_KEY_BATTLERESERVE == kEmporiaState or EMPORIA_KEY_BATTLECONFIRM == kEmporiaState then
			kEmporiaJoinBtnUI:SetStaticTextW(GetTextW(400445))
			kEmporiaJoinBtnUI:Disable(true)
		elseif EMPORIA_KEY_BATTLE_ATTACK == kEmporiaState or EMPORIA_KEY_BATTLE_DEFENCE == kEmporiaState then
			kEmporiaJoinBtnUI:SetStaticTextW(GetTextW(400445))
			kEmporiaJoinBtnUI:Disable(false)
		else
			kEmporiaJoinBtnUI:SetStaticTextW(GetTextW(400460))
			kEmporiaJoinBtnUI:Disable(true)
		end
	end
end

function EmporiaBattleBigDragonHPBG(kParent)
	local HpBar = kParent:GetControl("BAR_HP")
	local HpBg2 = kParent:GetControl("IMG_HP_BG2")
	if not HpBar:IsNil() and not HpBg2:IsNil() then
		local Percent = HpBar:GetBarCurrent() / HpBar:GetBarMax() * 100
		HpBg2:SetUVIndex( Percent / 25 );
		if Percent < 25 then
			HpBg2:Visible(false)
		else
			HpBg2:Visible(true)
		end
	end

end

function OnClickEmporiaBattleHelp(kSelf)
	if kSelf:IsNil() then return end
	
	local kParent = kSelf:GetParent();
	local kBuilder = kParent:GetControl("BLD_BUTTON");
	
	if kBuilder:IsNil() then return end
	
	local BuildCountPt = kBuilder:GetBuildCount();
	local BuildCount = BuildCountPt:GetX() * BuildCountPt:GetY();
	
	for idx = 1, BuildCount do
		local kButton = kParent:GetControl("CBTN_HELP"..(idx - 1));
		local kContents = kParent:GetControl("FRM_HELP_CONTENTS_"..(idx - 1));
	
		if kButton:IsNil() or kContents:IsNil() then return end
		
		if kSelf:GetBuildIndex() == (idx - 1) then
			kButton:CheckState(true);
			kButton:LockClick(true);
			kContents:Visible(true);
		else
			kButton:LockClick(false);
			kButton:CheckState(false);
			kContents:Visible(false);
		end
	end
end

g_EMTab = {}
g_EMTab[EMUI_ENTRANCE_BOARD] = "FRM_ENTRANCE_BOARD";
g_EMTab[EMUI_STATE_EMPORIA_STATUSBOARD] = "FRM_STATUSBOARD";
g_EMTab[EMUI_STATE_EMPORIA_HELP] = "FRM_EMPORIA_HELP";

function OnClickEmporiaBattleTab(kBuildIdx)
	local kParent = GetUIWnd("FRM_GUILDWAR");
	if kParent:IsNil() then
		kParent = CallUI("FRM_GUILDWAR")
		if kParent:IsNil() then return end
	end
	
	local kBuilder = kParent:GetControl("BLD_TAB_BTN");	
	if kBuilder:IsNil() then return end
	
	local BuildCountPt = kBuilder:GetBuildCount();
	local BuildCount = BuildCountPt:GetX() * BuildCountPt:GetY();
	
	for idx = 1, BuildCount do
		local index = idx - 1
		local kButton = kParent:GetControl("CBTN_TAB"..index);
		local kContents = kParent:GetControl(g_EMTab[index]);
	
		if kButton:IsNil() or kContents:IsNil() then return end
		
		if kBuildIdx == index then
			kParent:SetCustomDataAsInt(index)
			kButton:CheckState(true);
			kButton:LockClick(true);
			kContents:Visible(true);
			
			if EMUI_STATE_EMPORIA_STATUSBOARD == index then
				Call_EmporiaStatusBoard(kContents)
			elseif EMUI_ENTRANCE_BOARD == index then
				Call_GuildEntranceBoard()
			end
		else
			kButton:LockClick(false);
			kButton:CheckState(false);
			kContents:Visible(false);
		end
	end
end

function OnClickEmporiaChallengeBattle(kSelf)
	if nil==kSelf or kSelf:IsNil() then return end
	
	local kParent = kSelf:GetParent()
	if false == kParent:IsNil() then
		local kEditUI = kParent:GetControl("EDT_EXP")
		if false == kEditUI:IsNil() then
			local kText = kEditUI:GetEditText()
			if kText:Length() > 0 then
				Net_ReqEmporiaReserve( kParent:GetOwnerGuid(), kText:GetStr() * 10000 )
				UISelf:CloseParent()
			end
		end
	else
		UISelf:CloseParent()
	end
end

function Net_AddGuildMoney(iMoney)
	local kPlayer = g_pilotMan:GetPlayerUnit()
	if kPlayer:IsNil() then return end
	
	local kPacket = NewPacket(50101)	-- PT_C_N_REQ_GUILD_COMMAND
	kPacket:PushByte(47)
	kPacket:PushInt(iMoney)
	Net_Send(kPacket)
	DeletePacket(kPacket)
end