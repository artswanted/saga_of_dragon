-- /////////////////////// Net_PT_C_N_REQ_PVP_LIST ///////////////////////
g_iDirection = 1 --���� 1, ������ 2
g_iMissionMapCountLua = 4
MAX_MINICARD_NUM = 4
g_iNextCount = 0 -- NextMiniCard
g_iNowCardNum = 0 --01BASE
g_iNowCheckedCardNum = 0 -- ���̴� ī�� ����
g_szMissionBGFileName = "../Data/6_ui/mission/ms010100_"

g_iMissionTimeLimit = 0
g_iMissionTotalTimeLimit = 0
g_fMissionStartTimeLimit = 0
g_fMissionStartTime = 0

g_iMissionBonusTimeLimit = 0
g_iMissionBonusTotalTimeLimit = 0
g_fMissionBonusStartTimeLimit = 0
g_fMissionBonusStartTime = 0

g_iMissionTime = 0
g_iMissionTimeAttack = false
g_iMissionTotalTime = 0
g_bConstellationMission = false
misdiff = 0
mismember = 0
g_iMissionSenseScore = 0
g_iMissionAbilityScore = 0
g_iMissionTotalScore = 0
m_bMissionScoreTime = false
m_bMissionScoreAttack = false
MissionUpdateScoreAniType = 1
g_iSecChange = 0
g_iMissionStage = 0
g_iMissionStageCount = 0
g_iMissionWave = 0
g_ObjectGuid1 = nil
g_ObjectGuid2 = nil
g_RestMonsterNum = 0

g_IsLevel5Clear = false

g_mode6_SelectSuccessStep1Time = 0.3
g_mode6_SelectSuccessStep2Time = 2.3
g_mode6_SelectSuccessStep3Time = 0.3
g_mode6_SelectSuccessDelayTime = 1.3
g_mode6_StartNextStageDelayTime = (g_mode6_SelectSuccessStep1Time+g_mode6_SelectSuccessStep2Time+g_mode6_SelectSuccessStep3Time) + g_mode6_SelectSuccessDelayTime

--���潺7 ���� �������� �����̿�
g_mode7_wndTen = nil
g_mode7_wndOne = nil
g_mode7_fDeadTime = 0.0
g_mode7_iTenNum = 0.0
g_mode7_iOneNum = 0.0
g_mode7_fDeadCount = 0

--���潺7 �����
g_DMode7EnterDelayTime = 0
g_F7EnterTime = 0
g_Defence7Enter = false
g_Defence7ModeExpStr = WideString("")
g_OldFatigue = 0

--���潺7 ����Ʈ �������
g_UsingCopyMachine = false

function GuardianDetectRangeEffectName()
	return "ef_Dont_timer_01_char_root"
end
function GuardianDetectRangeEffectSize()
	return 85.0
end

function InitMission()
	g_iMissionStage = 0
	g_iMissionWave = 0
	g_iMissionSenseScore = 0
	g_iMissionTotalScore = 0	
	g_iMissionAbilityScore = 0
	MissionUpdateSense(g_iMissionSenseScore)
	MissionUpdateAbility(g_iMissionTotalScore)
	MissionUpdateTotalScore(g_iMissionAbilityScore)
	MissionUpdateScoreAniType = 1
	g_ObjectGuid1 = nil
	g_ObjectGuid2 = nil
	MissionMgrInit()
end

function Net_PT_M_C_ANS_MISSION_INFO(packet)
	ODS("Net_PT_M_C_ANS_MISSION_INFO\n")

	local iType = Recv_PT_M_C_ANS_MISSION_INFO(packet)
	
	local PlayerPilot = g_pilotMan:GetPlayerPilot();
	if IsInParty() and false == IsPartyMaster(PlayerPilot:GetGuid()) then
		return;
	end
	
	g_iNextCount = 0 -- NextMiniCard
	g_iNowCardNum = 0 --01BASE
	g_iNowCheckedCardNum = 0 -- ���̴� ī�� ����	
	
	InitMission()
	
	local MissionTopWnd 
	
	if 1 == iType then
		MissionTopWnd = GetUIWnd("FRM_CREATE_EVENT_ITEM")		
	else
		MissionTopWnd = GetUIWnd("FRM_CREATE_SCENARIO_ITEM")
	end
	
	local MissionWnd
	
	for k = 0,1 do		
		if k == 0 then
			MissionWnd = MissionTopWnd:GetControl("FRM_CREATE_FIELD_SCENARIO")	
		else
			MissionWnd = MissionTopWnd:GetControl("FRM_CREATE_FIELD")
		end
		
		if false == MissionWnd:IsNil() then
			for i=0,3 do
				for j=0,1 do
					local indexplus = 0
					if 1 ~= iType and 0 ~= k and 0 ~= i then
						indexplus = 1
					end
					local imgCard
					if j == 0 then 
						imgCard = MissionWnd:GetControl("FRM_CARD"..i) 
					else
						imgCard = MissionWnd:GetControl("FRM_BG_ITEM_BG"):GetControl("FRM_CARD"..i)
					end
					if imgCard:IsNil() == false then
				
						local ImgWnd = imgCard:GetControl("IMG_BG")

						if ImgWnd:IsNil() == false then							
							local ii = i + 1 + indexplus
							ImgWnd:ChangeImage(g_szMissionBGFileName..ii..".tga")
							ImgWnd:SetInvalidate(true)
						end
						
						local ImgKey = ImgWnd:GetControl("IMG_KEY")

						if ImgKey:IsNil() == false then
							ImgKey:SetUVIndex(i+1)
						end
						
						local ImgWnd = imgCard:GetControl("IMG_BG1")

						if ImgWnd:IsNil() == false then
							local ii = i + 2 + indexplus
							ImgWnd:ChangeImage(g_szMissionBGFileName..ii..".tga")
							ImgWnd:SetInvalidate(true)
						end
						
						local ImgKey = ImgWnd:GetControl("IMG_KEY")

						if ImgKey:IsNil() == false then
							ImgKey:SetUVIndex(i+1)
						end						
						
						local PartyPeople = imgCard:GetControl("FRM_PARTY_PEOPLE")
						if PartyPeople:IsNil() == false then
							PartyPeople:SetCustomDataAsInt(i+1)
						end						
					end
				end
			end	
			local ImgText = MissionWnd:GetControl("IMG_TEXT_BG")
			if ImgText:IsNil() == false then
				ImgText:SetStaticText(GetTextW(g_iMissionDiscription):GetStr())
			end

			local NotHaveArcadeText = MissionWnd:GetControl("FRM_BG_EVENT_NOT_HAVE_ARCADE")
			if NotHaveArcadeText:IsNil() == false then
				local TextWnd = NotHaveArcadeText:GetControl("FRM_TEXT")
				if TextWnd:IsNil() == false then
					TextWnd:SetStaticText(GetTextW(g_iMissionDiscription):GetStr())
				end
			end
			
			if MissionTopWnd:IsNil() == false then
				local ImgEasy = MissionTopWnd:GetControl("FRM_EASY")
				if ImgEasy:IsNil() == false then				
					if 4 == iType then
						ImgEasy:Visible(true)
					else
						ImgEasy:Visible(false)
					end
				end	
				
				
				local FrmField = MissionTopWnd:GetControl("FRM_CREATE_FIELD")
				if FrmField:IsNil() == false then
					for k=0,3 do
						local FrmText = FrmField:GetControl("FRM_ITEMTEXT"..k)
						if FrmText:IsNil() == false then
							local FrmVisibleText3 = FrmText:GetControl("FRM_ITEM_TEXT3")
							local FrmVisibleText4 = FrmText:GetControl("FRM_ITEM_TEXT4")
							
							if FrmVisibleText3:IsNil() == false then
								if 4 == iType then
									FrmVisibleText3:Visible(false)
								else
									FrmVisibleText3:Visible(true)
								end
							end
							if FrmVisibleText4:IsNil() == false then
								if 4 == iType then
									FrmVisibleText4:Visible(false)
								else
									FrmVisibleText4:Visible(true)
								end
							end																
						end
					end
					
					local FrmBgItem = FrmField:GetControl("FRM_BG_ITEM")
					if FrmBgItem:IsNil() == false then
						local FrmText = FrmBgItem:GetControl("FRM_TEXT")
						if FrmText:IsNil() == false then
							if 4 == iType then
								FrmText:SetStaticText(GetTextW(400954):GetStr())
							else
								FrmText:SetStaticText(GetTextW(400949):GetStr())
							end
						end
					end					
					
				end
				
			end
		end
	end

	
--[[
	local MissionWnd = GetUIWnd("FRM_CREATE_FIELD_SCENARIO")	
	
	if MissionWnd:IsNil() then
		return false
	end
	

	for i=0,3 do
	    local imgCard = MissionWnd:GetControl("FRM_CARD"..i)
	    if imgCard:IsNil() == false then
	
		    local ImgWnd = imgCard:GetControl("IMG_BG")

		    if ImgWnd:IsNil() == false then
	    		local ii = i + 1
    			ImgWnd:ChangeImage(g_szMissionBGFileName..ii..".tga")
			    ImgWnd:SetInvalidate(true)
		    end
		end
	end

	local ImgText = MissionWnd:GetControl("IMG_TEXT_BG")
	if ImgText:IsNil() == false then
	    ImgText:SetStaticText(GetTextW(g_iMissionDiscription):GetStr())
	end
]]--
	g_iMissionMapCountLua = GetMissionMapCount()
	
	MissionScoreTypeClear()
	m_bMissionScoreTime = false
	m_bMissionScoreAttack = false
	
	return true
end

function OnSelectMissionDiff( kSelf, bScenario )
	
	local kParent = kSelf:GetParent()
	if false == kParent:IsNil() then
	
		local kGrandParent = kParent:GetParent()
		if false == kGrandParent:IsNil() then
		
			local iDiff = 0
			if bScenario then
				if kGrandParent:GetControl("FRM_CARD0"):GetControl("IMG_BG"):GetControl("IMG_CLEAR"):IsVisible() then
					iDiff = 6
				else
					iDiff = 5
				end
			else		
				iDiff = kParent:GetCustomDataAsInt() + 1
			end
			
			local CardWnd = kGrandParent:GetControl("FRM_CARD"..kParent:GetCustomDataAsInt())
			if false == IsGrayDiff(CardWnd) then
				return
			end
				
			if iDiff > 0 then
				SetScenarioMissionMapCount(GetMissionNo(), iDiff)
				Net_ReqEnterMission( iDiff, 0, 0, 0, 0, GetMutatotSelectedCount(), GetMutatorPacket() )
				kGrandParent:CloseParent()
				return true
			end
		end	
	end
	return false
end

function IsGrayDiff(CardWnd)
	local ImgBg = CardWnd:GetControl("IMG_BG")
	if false == ImgBg:IsNil() then
		if false == CheckScenarioMissionEnter() then
			return false
		end
	--	if true == ImgBg:IsGrayScale() then
			--if GetUIWnd("FRM_GAMING_RANDOM"):IsNil() == false then
	--			AddWarnDataTT(400225)
	--			CallMissionRetryUI()
				--CardWnd:GetParent():CloseParent()
	--			return false
			--end		
	--	end
	end
	return true
end

function CallDefenceModEnter()
	g_IsLevel5Clear = false
	
	local PlayerPilot = g_pilotMan:GetPlayerPilot();
	if false == IsInParty() or true == IsPartyMaster(PlayerPilot:GetGuid()) then
			local TopWnd = GetUIWnd("FRM_CREATE_SCENARIO_ITEM")
		if true == TopWnd:IsNil() then return end

		local DefenceWnd = TopWnd:GetControl("FRM_CREATE_FIELD_DEFENCE")
		if DefenceWnd:IsNil() == true then return end

		local CardWnd = DefenceWnd:GetControl("FRM_CARD0")
		if CardWnd:IsNil() == true then return end
		
		local ImgBg = CardWnd:GetControl("IMG_BG")
		if ImgBg:IsNil() == true then return end
		
		if ImgBg:IsGrayScale() then
			AddWarnDataTT(400225)
			return
		end
			
		local ScenarioWnd = TopWnd:GetControl("FRM_CREATE_FIELD_SCENARIO")
		if true == ScenarioWnd:IsNil() then return end
		
		local ScenarioCard = ScenarioWnd:GetControl("FRM_CARD0")
		if true == ScenarioCard:IsNil() then return end
		
		g_IsLevel5Clear = IsClearDiff(ScenarioCard)
	end
	local bDefenceMode6 = false
	local bDefenceMode7 = false
	bDefenceMode6 = IsOpenDefenseModeTable(1,GetMissionNo())
	bDefenceMode7 = IsOpenDefenseModeTable(2,GetMissionNo())
	if not bDefenceMode6 and not bDefenceMode7 then
		return
	else
		if bDefenceMode6 and bDefenceMode7 then
			ActivateUI("FRM_CREATE_DEFENCE")
		elseif bDefenceMode6 then
			if IsInParty() == true then
				local kPlayerPilot = g_pilotMan:GetPlayerPilot();
				if kPlayerPilot:IsNil() == true or IsPartyMaster(kPlayerPilot:GetGuid()) == false then
					return
				end
			end
			OnEnterDefenceMode6()
		elseif bDefenceMode7 then
			if IsInParty() == true then
				local kPlayerPilot = g_pilotMan:GetPlayerPilot();
				if kPlayerPilot:IsNil() == true or IsPartyMaster(kPlayerPilot:GetGuid()) == false then
					return
				end
			end
			OnEnterDefenceMode7()
		end
	end
end

function IsOpenDefanceMode(iMode)
	if 6==iMode and true == IsHaveArcadeMode() then
		if IsMissionComplatedQuest(iMode) then
			return true
		else
			return false
		end
	end
	
	if 8==iMode then
		return false
		--return IsOpenDefenseModeTable(4,GetMissionNo())
	elseif 7==iMode and (not IsOpenDefenseModeTable(2,GetMissionNo()) or not IsMissionComplatedQuest(iMode)) then
		return false
	end
	return true
end

function SetDefanceImgPath(kWnd,iMode)
	if nil==kWnd or kWnd:IsNil() then
		return
	end
	
	local kImg = kWnd:GetControl("IMG_BG")
	if nil==kImg or kImg:IsNil() then
		return
	end
	
	local kPath = "../Data/6_ui/mission/"
	if 7==iMode then
		kPath = kPath .. "msDefense7Img_"
	elseif 8==iMode then
		kPath = kPath .. "msDefense8Img_"
	else
		kPath = kPath .. "msDefenseImg_"
	end	
	kPath = kPath .. GetMissionNo() .. ".tga"

	local bOpen = IsOpenDefanceMode(iMode)
	kImg:ChangeImage(kPath)
	kImg:SetGrayScale( not bOpen )
	
	local kClearQuest = kWnd:GetControl("FRM_CLEARQUEST")
	if false==kClearQuest:IsNil() then
		kClearQuest:Visible(not bOpen and not IsMissionComplatedQuest(iMode))
	end
	kImg:SetInvalidate(true)
end

function SetDefanceInfoText(kWnd,iMode)
	if nil==kWnd or kWnd:IsNil() then
		return
	end
	
	if 8==iMode then
		kWnd:GetControl("FRM_TEXT"):SetStaticText(GetTT(401145):GetStr())
		--SetMissionLevelingText(kWnd:GetControl("FRM_TEXT"),iMode)
	elseif 7==iMode then
		local iMin = tonumber(GetEnterDefenceMinLevel(GetMissionNo(),8))
		local iMax = tonumber(GetEnterDefenceMaxLevel(GetMissionNo(),8))
		local text = ''
		if iMax >= 1000 then
			text = string.format(GetTextW(401146):GetStr(), iMin)
		else
			text = string.format(GetTextW(401177):GetStr(), iMin, iMax)
		end
		kWnd:GetControl("FRM_TEXT"):SetStaticText(text)
	elseif 6==iMode then
		kWnd:GetControl("FRM_TEXT"):SetStaticText(GetTT(400958):GetStr())
	end
end

function SetEnterDefanceUI(kWnd,iMode)
	if nil==kWnd or kWnd:IsNil() then
		return
	end
	
	SetDefanceImgPath(kWnd,iMode)
	SetDefanceInfoText(kWnd,iMode)
	kWnd:GetControl("IMG_OVER"):Visible(false)
end

function Send_PT_C_M_REQ_DEFENCE_PARTYLIST()
	local packet = NewPacket(13511)
	Net_Send(packet)
	DeletePacket(packet)
end

function OnCall_FRM_CREATE_DEFENCE(kSelf)
	if nil==kSelf or kSelf:IsNil() then
		return
	end

	g_DMode7EnterDelayTime = 0
	g_F7EnterTime = 0
	g_Defence7Enter = false

	SetEnterDefanceUI( kSelf:GetControl("FRM_CREATE_FIELD_DEFENCE"),6 )
	SetEnterDefanceUI( kSelf:GetControl("FRM_CREATE_FIELD_F7"),7 )
	SetEnterDefanceUI( kSelf:GetControl("FRM_CREATE_FIELD_F8"),8 )
	Send_PT_C_M_REQ_DEFENCE_PARTYLIST()
end

function MissionSelectedKeyDown(UISelf, CardNum)
	local CardWnd = UISelf:GetControl("FRM_CARD"..CardNum)
	if false == CardWnd:IsNil() then	
		if false == IsGrayDiff(CardWnd) then
			return
		end
		--[[if true == CardWnd:IsGrayScale() then 
			AddWarnDataTT(400802)
			return 
		end--]]
--		iMsnNum = UISelf:GetCustomDataAsInt()
--		if iMsnNum == 0 then
--			ODS("iMsnNum == 0\n")
--		end

		local iDiff = CardWnd:GetCustomDataAsInt() + 1
		Net_ReqEnterMission( iDiff, 0, 0, 0, 0, GetMutatotSelectedCount(), GetMutatorPacket() )
		CardWnd:GetParent():CloseParent()			
	end
	
	InitMission()
end

function Net_ReqEnterMission_Npc()
	local kMyActor = g_pilotMan:GetPlayerActor()
	if false == kMyActor:IsNil() then
		local kTrigger = kMyActor:GetCurrentTrigger()
		if false == kTrigger:IsNil() then
			World_Mission_Portal( kTrigger, kMyActor, 0, 0, 0 )
		else
			--g_ChatMgrClient:Notice_Show_ByTextTableNo(18995)--��Ż�� ������ϴ�.
		end
	end
end

function Net_ReqEnterMission_Event_Npc( iSelect )
	local kMyActor = g_pilotMan:GetPlayerActor()
	if false == kMyActor:IsNil() then
		local kTrigger = kMyActor:GetCurrentTrigger()
		if false == kTrigger:IsNil() then
			World_MissionEvent_Portal( kTrigger, kMyActor, 0, iSelect )
		else
			--g_ChatMgrClient:Notice_Show_ByTextTableNo(18995)--��Ż�� ������ϴ�.
		end
	end
end

function Net_ReqEnterMission( iDiff, iType, iCustomDataAsInt1, iCustomDataAsInt2, iCustomDataAsInt3, iMutatorCount, MutatorPacket )

	if nil == iMutatorCount then
		iMutatorCount = 0
	end

	local loadingUI = GetUIWnd("FRM_MISSION_JOIN_REQUEST")
	if loadingUI:IsNil() == false then
        return	    
	end 
	
	local packettemp = NewPacket()
	packettemp:PushInt(iType)				
	if 1 == iType then
		packettemp:PushInt(iCustomDataAsInt1)
	elseif 2 == iType then
		packettemp:PushInt(iCustomDataAsInt1)
		packettemp:PushInt(iCustomDataAsInt2)
		packettemp:PushInt(iCustomDataAsInt3)
	end	
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if g_world:IsHaveWorldAttr( GATTR_FLAG_MISSION ) then
	
		local packet = NewPacket(13403) --PT_C_M_REQ_ENTER_MISSION
		ODS("PT_C_M_REQ_ENTER_MISSION Start\n")
		packet:PushInt(iDiff)		
		
		packet:PushPacket(packettemp)
		
		Net_Send(packet)
		ODS("PT_C_M_REQ_ENTER_MISSION\n End")
		DeletePacket(packet)
	else
		if 0 ~= iType and 2 == iDiff then	
			-- ī����
			ODS("World_Chaos_Portal\n End")
			World_Chaos_Portal(iDiff, packettemp)
		else	
			local kMyActor = g_pilotMan:GetPlayerActor()
			if false == kMyActor:IsNil() then
				local kTrigger = kMyActor:GetCurrentTrigger()
				if false == kTrigger:IsNil() then
					World_Mission_Portal( kTrigger, kMyActor, iDiff, packettemp, iMutatorCount, MutatorPacket )
				else --Ʈ���Ÿ� ã�� �� ����
					local kMissionTrigID = g_world:GetMissionTrigIDOnThisMap()
					if kMissionTrigID:IsNil() then
						g_ChatMgrClient:Notice_Show_ByTextTableNo(18995)--��Ż�� ������ϴ�.
					else --Ʈ���Ű� �ƴ� �ٷΰ��� UI�� Ŭ�� ���� ��� ���� ���� Ʈ���Ÿ� ã�ƶ�
						local kMissionTrig = g_world:GetTriggerByID(kMissionTrigID:GetStr())
						if kMissionTrig:IsNil() == false then
							World_Mission_Portal( kMissionTrig, kMyActor, iDiff, packettemp, iMutatorCount, MutatorPacket )
						end
					end
				end
			end
		end
	end
	
	DeletePacket(packettemp)
	
--[[
	if IsInParty() == true then
		local kPlayerPilot = g_pilotMan:GetPlayerPilot();
		if kPlayerPilot:IsNil() == true or IsPartyMaster(kPlayerPilot:GetGuid()) == false then
			ODS("������ �ƴ�\n")
			return false;
		end
	end
	]]--

end

function Net_PT_M_C_NFY_MISSION_STATE(packet)	-- ���� �̼Ǹʿ� ������ ���	
	CloseUI("FRM_MISSION_JOIN_REQUEST")
	ODS("Net_PT_M_C_NFY_MISSION_STATE\n")
	
	LockPlayerInputMove(3)
	LockPlayerInput(3)

	local MisNum = packet:PopInt()
	SetMissionNo(MisNum)
	local misname = GetDefString( MisNum )	-- int���� char�� �ص� �ɵ�.
	if nil ~= misname then
		ODS("�̼� ���� : " .. misname .. "\n")
	else
		misname = " "
	end
	misdiff = packet:PopInt()
	mismember = packet:PopSize_t()
	
	if g_world~= nil and g_world:IsNil() == false then
		g_world:SetDifficultyLevel(misdiff)
		local misnameW = GetMapNameW( g_world:GetMapNo() )
		if nil~=misnameW and false==misnameW:IsNil() then
			misname = misnameW:GetStr()
		end
	end
	
	g_iMissionTime = packet:PopInt()
	g_iMissionTimeAttack = packet:PopBool()
	g_iMissionTotalTime = packet:PopInt()		
	--g_iMissionMapCountLua = CalcMissionCount(MisNum, misdiff)
	g_iMissionMapCountLua = packet:PopByte()
	g_iNextCount = packet:PopByte()
	g_bConstellationMission = packet:PopBool()
	SetMissionCountMap(g_iMissionMapCountLua)
	
	
	if true == IsDefenceGameMode() then
		-- ��ȣ�� Guid
		g_ObjectGuid1 = packet:PopGuid()
		
		if true == IsDefenceMode8() then
			g_ObjectGuid2 = packet:PopGuid()
		end
		
		if 0 ~= g_iMissionStage or 0 ~= g_iMissionWave then
			g_iMissionWave = g_iMissionWave+1
			Net_PT_M_C_NFY_REST_MONSTER_NUM(packet)	
		end

		MissionUpdateUIScore()
		UnLockPlayerInputMove(3)
		UnLockPlayerInput(3)
		if not IsDefenceMode() then
			UnLockPlayerInput(1)
		end
		return true
	end
	
	if 1 == misdiff or 2 == misdiff then
		CallRankTop(GetMissionKey(GetMissionNo()), misdiff-1)
	end
	
	local wndInfo = ActivateUI("FRM_GAMING_RANDOM")
	if	wndInfo:IsNil() == true then
		return false
	end
	wndInfo:SetCustomDataAsInt(0)
	InitMiniCard(wndInfo, MisNum)
	SetMiniCard(wndInfo, g_iNextCount)
	
	if (misdiff ~= 5 and misdiff ~= 6) then
		ActivateUI("FRM_MISSION_SCORE")
		if true == GetMissionExpUI(MisNum) then
			ActivateUI("FRM_MISSION_EXP")
		end
	end
	
	ODS("�̼� ���� : " .. misname .. " ���̵� : " .. misdiff .. "�̼� ��ȣ : " ..MisNum.. " g_iMissionMapCountLua : " .. g_iMissionMapCountLua .. "\n", false, 1509)
	local frmBG = wndInfo:GetControl("FRM_BG");
	if frmBG:IsNil() == false then
	    frmBG:SetStaticText(GetTextW(400218):GetStr() .. misname)
	end
	
	if g_iMissionTotalTime > 999000 then
		g_iMissionTotalTime = 999000
	end
	
	for i = 0,3 do	-- �ϴ� ����
	    local wndBG = wndInfo:GetControl("FRM_BG"); 
	    if wndBG:IsNil() == false then
		    local star = wndBG:GetControl("FRM_STAR".. tostring(i))
		    if star:IsNil() == false then
		    	star:Visible(false)
		    end
		end
	end
	
    local wndBG = wndInfo:GetControl("FRM_BG")
    if wndBG:IsNil() == false then

		local diff = wndBG:GetControl("FRM_DIFF")
		if diff:IsNil() == false then
			diff:Visible(true)
			diff:SetStaticText("{C=0xFFf7ea96/T=FTcts_13/}" .. GetTextW(400970):GetStr() .. "{C=0xFFFFFFFF/T=FTcts_13/}" .. GetTextW(400680+misdiff-1):GetStr())
		end			

	    local member = wndBG:GetControl("FRM_MEMBER")
	    if member:IsNil() == false then
    		member:SetStaticText(GetTextW(400219):GetStr() .. tostring(mismember) .. GetTextW(400221):GetStr())
    	end
	end
	--UnLockPlayerInput()
	g_fMissionStartTime = GetCurrentTimeInSec()	
		
	if (misdiff == 5 or misdiff == 6) and (g_iMissionTimeAttack == true) then
		if g_iNextCount ~= 0 then
			ActivateUI("FRM_MISSION_SCENARIO_TIMER")
		end
		--local wndTimer = ActivateUI("FRM_MISSION_SCENARIO_TIMER")
		local wndTimer = GetUIWnd("FRM_MISSION_SCENARIO_TIMER")
		if wndTimer:IsNil() == false then
			wndTimer:SetCustomDataAsInt(iMissionTime)
		end
	elseif (misdiff ~= 5 and misdiff ~= 6) then
		local wndTimer = ActivateUI("FRM_MISSION_TIMER")
		if wndTimer:IsNil() == false then
			wndTimer:SetCustomDataAsInt(iMissionTime)
		end
		
		--ActivateUI("FRM_MISSION_SCORE")
	end
	
	--local wndTimer = ActivateUI("FRM_MISSION_TIMER")
	--if wndTimer:IsNil() == false then
	--	wndTimer:SetCustomDataAsInt(iMissionTime)
	--end	
	
	Net_PT_M_C_NFY_REST_MONSTER_NUM(packet)	
	MissionUpdateUIScore()
	UnLockPlayerInputMove(3)
	UnLockPlayerInput(3)
end

function DisplayMissionScenarioTimer(wnd)
	if wnd:IsNil() == true then
		return
	end

	local fNow = GetCurrentTimeInSec()*1000
	
	local iMissionTime;
	
	iMissionTime = g_iMissionTotalTime - (g_iMissionTime + fNow - g_fMissionStartTime*1000)
	if iMissionTime < 0 then
		iMissionTime = 0;
	end

	local iDelta = math.floor(iMissionTime*0.1)
	local iSec100 = iDelta%10 + 1
	local iSec1000 = math.floor((iDelta%100)/10) + 1	
	iDelta = math.floor(iMissionTime*0.001)
	local iSec1 = iDelta%10 + 1
	local iSec10 = math.floor((iDelta%60)/10) + 1
	local iMin1 = math.floor(iDelta/60)%10 + 1
	local iMin10 = math.floor(iDelta/600)%10 + 1	
	
	local wnd100sec = wnd:GetControl("FRM_SEC100")
	if wnd100sec:IsNil() == false then
		wnd100sec:SetUVIndex(iSec100)
	end

	local wnd1000sec = wnd:GetControl("FRM_SEC1000")
	if wnd1000sec:IsNil() == false then
		wnd1000sec:SetUVIndex(iSec1000)
	end
	
	local wnd1sec = wnd:GetControl("FRM_SEC1")
	if wnd1sec:IsNil() == false then
		wnd1sec:SetUVIndex(iSec1)
	end

	local wnd10sec = wnd:GetControl("FRM_SEC10")
	if wnd10sec:IsNil() == false then
		wnd10sec:SetUVIndex(iSec10)
	end

	local wnd1min = wnd:GetControl("FRM_MIN1")
	if wnd1min:IsNil() == false then
		wnd1min:SetUVIndex(iMin1)
	end

	local wnd10min = wnd:GetControl("FRM_MIN10")
	if wnd10min:IsNil() == false then
		wnd10min:SetUVIndex(iMin10)
	end

	local kPathDefault = "../Data/6_ui/mission/msNumTmY.tga"
	local kPathsDefault = "../Data/6_ui/mission/msNumTmYs2.tga"
	local kPath = "../Data/6_ui/mission/msNumTmY.tga"
	local kPaths = "../Data/6_ui/mission/msNumTmYs2.tga"
	
	local iChangeColorTime = 180
	if true == IsDefenceMode() then
		iChangeColorTime = 10
		kPath = "../Data/6_ui/mission/msNumTmR.tga"
		kPaths = "../Data/6_ui/mission/msNumTmRs.tga"
	end
	
	if iDelta <= iChangeColorTime then	
		if true == IsDefenceMode() and 1 < iDelta then
			ActivateUI("FRM_DEFENCE_TIMELIMIT")
		end
		wnd100sec:ChangeImage(kPaths)
		wnd100sec:SetInvalidate(true)
		wnd1000sec:ChangeImage(kPaths)
		wnd1000sec:SetInvalidate(true)
		wnd10sec:ChangeImage(kPath)
		wnd10sec:SetInvalidate(true)
		wnd1sec:ChangeImage(kPath)
		wnd1sec:SetInvalidate(true)
		wnd1min:ChangeImage(kPath)
		wnd1min:SetInvalidate(true)
		wnd10min:ChangeImage(kPath)
		wnd10min:SetInvalidate(true)
	else
		wnd100sec:ChangeImage(kPathsDefault)
		wnd100sec:SetInvalidate(true)
		wnd1000sec:ChangeImage(kPathsDefault)
		wnd1000sec:SetInvalidate(true)
		wnd10sec:ChangeImage(kPathDefault)
		wnd10sec:SetInvalidate(true)
		wnd1sec:ChangeImage(kPathDefault)
		wnd1sec:SetInvalidate(true)
		wnd1min:ChangeImage(kPathDefault)
		wnd1min:SetInvalidate(true)
		wnd10min:ChangeImage(kPathDefault)
		wnd10min:SetInvalidate(true)	
	end
	
end

function MissionUpdateSense(iSense)
	if iSense >= 9999999 then
		iSense = 9999999
	end
	MissionUpdateScoreAniType = 1
	g_iMissionSenseScore = iSense
	DisPlayMissionUpdateScore(0, iSense)
end

function MissionUpdateAbility(iAbility)
	if iAbility >= 9999999 then
		iAbility = 9999999
	end
	MissionUpdateScoreAniType = 2
	g_iMissionAbilityScore = iAbility
	DisPlayMissionUpdateScore(1, iAbility)	
end

function MissionUpdateTotalScore(iTotal)
	if iTotal >= 9999999 then
		iTotal = 9999999
	end	
	g_iMissionTotalScore = iTotal
	DisPlayMissionUpdateScore(2, iTotal)
end

function MissionUpdateUIScore()	
	DisPlayMissionUpdateScore(0, g_iMissionSenseScore)
	DisPlayMissionUpdateScore(1, g_iMissionAbilityScore)
	DisPlayMissionUpdateScore(2, g_iMissionTotalScore)
end

function MissionUpdateScoreAni()
	UpdateMissionScoreStart(MissionUpdateScoreAniType)
end

function DisplayMissionScore(wnd)
	if wnd:IsNil() == true then
		return
	end
	
	local wndScore = ActivateUI("FRM_MISSION_SCORE")
	
	if wndScore:IsNil() == false then
		local wndScore1000 = 0
		local wndScore100 = 0
		local wndScore10 = 0
		local wndScore1 = 0
		
		local iScore = 0
		
		local bVisable = false
		local font = 0
				
		for i = 0, 2 do
			wndScore1000 = 0
			wndScore100 = 0
			wndScore10 = 0
			wndScore1 = 0		
			if i == 0 then
				iScore = g_iMissionSenseScore
				wndScore100 = wndScore:GetControl("FRM_SENSE_100")
				wndScore10 = wndScore:GetControl("FRM_SENSE_10")
				wndScore1 = wndScore:GetControl("FRM_SENSE_1")			
				
				--[[if g_iMissionSenseScore >= 9999999 then
					g_iMissionSenseScore = 9999999
					bVisable = true
					
					if m_bMissionScoreAttack == false then
						--UpdateMissionScoreStart(2)
						m_bMissionScoreAttack = true
					end
				end--]]		
			
			elseif i == 1 then
				iScore = g_iMissionTotalScore
				wndScore100 = wndScore:GetControl("FRM_ABILITY_100")
				wndScore10 = wndScore:GetControl("FRM_ABILITY_10")
				wndScore1 = wndScore:GetControl("FRM_ABILITY_1")			
			elseif i == 2 then
				iScore = g_iMissionAbilityScore
				wndScore100 = wndScore:GetControl("FRM_TOTALSCORE_100")
				wndScore10 = wndScore:GetControl("FRM_TOTALSCORE_10")
				wndScore1 = wndScore:GetControl("FRM_TOTALSCORE_1")				
			end
			
			if i == 0 then
				if iScore > 9999999 then
					iScore = 9999999
				end			
			else
				if iScore > 9999999 then
					iScore = 9999999
				end
			end
		
			if wndScore100:IsNil() == false then
				if iScore == 0 then
					wndScore100:SetUVIndex(iScore+1)
				else
					wndScore100:SetUVIndex((iScore%1000)/100+1)
				end
			end
			if wndScore10:IsNil() == false then
				if iScore == 0 then
					wndScore10:SetUVIndex(iScore+1)
				else			
					wndScore10:SetUVIndex((iScore%100)/10+1)
				end
			end
			if wndScore1:IsNil() == false then
				if iScore == 0 then
					wndScore1:SetUVIndex(iScore+1)
				else						
					wndScore1:SetUVIndex(iScore%10+1)
				end
			end	
		end
	end
end


function DisplayMissionTimer(wnd)
	if wnd:IsNil() == true then
		return
	end

	local fNow = GetCurrentTimeInSec()*1000
	local iMissionTime = g_iMissionTime + fNow - g_fMissionStartTime*1000--GetFrameTime()*1000
	local iDelta = math.floor(iMissionTime*0.1)
	local iSec100 = iDelta%10 + 1
	local iSec1000 = math.floor((iDelta%100)/10) + 1
	iDelta = math.floor(iMissionTime*0.001)
	local iSec1 = iDelta%10 + 1	-- 1���ϴ� ���� �׸� index�� 1���� �����ϱ� ������
	local iSec10 = math.floor((iDelta%60)/10) + 1
	local iMin1 = math.floor(iDelta/60)%10 + 1
	local iMin10 = 0
	if wnd:GetControl("FRM_HOUR1"):IsNil() then
		iMin10 = math.floor(iDelta/600)%10 + 1
	else
		iMin10 = math.floor((iDelta/60)%60)/10 + 1
	end
	local iHour1 = math.floor(iDelta/3600)%10 + 1
	local iHour10 = math.floor(iDelta/3600)/10 + 1

	
	local wnd1sec = wnd:GetControl("FRM_SEC1")
	if wnd1sec:IsNil() == false then
		wnd1sec:SetUVIndex(iSec1)
	end

	local wnd10sec = wnd:GetControl("FRM_SEC10")
	if wnd10sec:IsNil() == false then
		wnd10sec:SetUVIndex(iSec10)
	end

	local wnd100sec = wnd:GetControl("FRM_SEC100")
	if wnd100sec:IsNil() == false then
		wnd100sec:SetUVIndex(iSec100)
	end

	local wnd1000sec = wnd:GetControl("FRM_SEC1000")
	if wnd1000sec:IsNil() == false then
		wnd1000sec:SetUVIndex(iSec1000)
	end

	local wnd1min = wnd:GetControl("FRM_MIN1")
	if wnd1min:IsNil() == false then
		wnd1min:SetUVIndex(iMin1)
	end

	local wnd10min = wnd:GetControl("FRM_MIN10")
	if wnd10min:IsNil() == false then
		wnd10min:SetUVIndex(iMin10)
	end
	
	local wnd1hour = wnd:GetControl("FRM_HOUR1")
	if wnd1hour:IsNil() == false then
		wnd1hour:SetUVIndex(iHour1)
	end
	
	local wnd10hour = wnd:GetControl("FRM_HOUR10")
	if wnd10hour:IsNil() == false then
		wnd10hour:SetUVIndex(iHour10)
	end

end

function Net_PT_C_M_REQ_EXIT_MISSION()	-- ������ ������ ��
	CloseUI("FRM_GAMING_RANDOM")
	if Get_CMP_STATE() == 0 or Get_CMP_STATE() >= 12 then
		Net_RecentMapMove()
		PartyStateModify()
		CloseUI("FRM_MISSION_DEFENCE_TIMER")
		CloseUI("FRM_MISSION_SCENARIO_TIMER")
		CloseUI("FRM_MISSION_TIMER")
		CloseUI("FRM_MISSION_SCORE")
		CloseUI("FRM_MISSION_EXP")
		CloseUI("SFRM_MISSION_STAGE_DATA")
		CloseUI("SFRM_MISSION_ADD_EXP")
		CloseUI("FRM_DEFENCE_MINICARD")
		CloseUI("FRM_MISSION_DEFENCE_HP_BAR")
	else 
	    ODS("Get_CMP_STATE : " .. Get_CMP_STATE() .. "\n")
	end
   ODS("Net_PT_C_M_REQ_EXIT_MISSION \n")
end

function Net_PT_N_C_NFY_MISSION_RESULT(packet)	-- �̼� Ŭ����. ���� �ڵ�󿡼� �� �ذ�.
	-- PT_N_C_NFY_MISSION_RESULT
	ODS("Net_PT_N_C_NFY_MISSION_RESULT\n")

	local wnd = CallUI("SFRM_RESULT_INFO_BG")
	if wnd:IsNil() == true then
		return 
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local iMemberNum = packet:PopInt()			-- ��� ���� char�� �ص� �ɵ�

	local item = {}
	local info = {}

	for i = 0, iMemberNum-1 do
		ODS("Net_PT_N_C_NFY_MISSION_RESULT For Loop\n")
		local MemberGuid = packet:PopGuid()--GUID �����̶� �̸� �˾ƿ;� ��
      local CharGuid = packet:PopGuid()--GUID �����̶� �̸� �˾ƿ;� ��
		CharGuid:ODS()

		info.iNormal = packet:PopInt()
		info.iStrngth = packet:PopInt()
		info.iCrack = packet:PopInt()

		info.iGetXP = packet:PopInt()
		info.iTotalXP = packet:PopInt()

		info.iLv = packet:PopInt()
		info.szName = packet:PopWString()
      
		if info.szName:Length() <= 0 then
			if CharGuid:IsNil() == false then
				local playerActor = g_world:FindActor(CharGuid)
				if playerActor:IsNil() == false then
					local playerPilot = playerActor:GetPilot()
					if playerPilot:IsNil() == false then
						info.szName = playerPilot:GetName()
					end
				end
			else
				ODS("CharGuid:IsNil() == true \n")
			end
		end

      if info.iLv <= 0 then
         if CharGuid:IsNil() == false then
            local playerActor = g_world:FindActor(CharGuid)
            if playerActor:IsNil() == false then
               info.iLv = playerActor:GetAbil(AT_LEVEL)
            end
         end
      end
		
		local iItemNum = packet:PopInt() --int	������ ����
		ODS(info.szName:GetStr() .. "���� ���� ������ ���� " .. iItemNum .."��\n")
		
		if iItemNum > 0 then
			ODS("���� �����۹�ȣ�� ")
			for j = 0, iItemNum-1 do
				item[j] = packet:PopInt()
				ODS(item[j].." ")
			end
			ODS("\n")
		end

		local wndInfo = wnd:GetControl("SFRM_RESULT_INFO"..i)
		if wndInfo:IsNil() == false	then
			wndInfo:Visible(true)
			local title = wndInfo:GetControl("SFRM_TITLE"):GetControl("SFRM_COLOR"):GetControl("SFRM_SDW")
			if title:IsNil() == false then
				title:SetStaticText(GetTextW(400222):GetStr() .. info.iLv .. " " .. info.szName:GetStr())
			end
			local sdw = wndInfo:GetControl("SFRM_COLOR"):GetControl("SFRM_SDW")
			if sdw:IsNil() == false then
				local normal = sdw:GetControl("FRM_NORMAL"):GetControl("FRM_NUM")
				if normal:IsNil() == false then
					normal:SetStaticText(tostring(info.iNormal))
				end
				local strnth = sdw:GetControl("FRM_STRENGTH"):GetControl("FRM_NUM")
				if strnth:IsNil() == false then
					strnth:SetStaticText(tostring(info.iStrngth))
				end
				local clack = sdw:GetControl("FRM_CRACK"):GetControl("FRM_NUM")
				if clack:IsNil() == false then
					clack:SetStaticText(tostring(info.iCrack))
				end
				local totalexp = sdw:GetControl("SFRM_TOTAL_EXP"):GetControl("FRM_NUM")
				if totalexp:IsNil() == false then
					totalexp:SetStaticText(GetTextW(400223):GetStr() .. info.iGetXP .. GetTextW(400216):GetStr() )
				end
				local clearexp = sdw:GetControl("SFRM_CLEAR_EXP"):GetControl("FRM_NUM")
				if clearexp:IsNil() == false then
					clearexp:SetStaticText(GetTextW(400223):GetStr() .. info.iTotalXP .. GetTextW(400216):GetStr() )
				end
			end
			sdw = wndInfo:GetControl("SFRM_ITEM_COLOR"):GetControl("SFRM_ITEM_SDW")
			if sdw:IsNil() == false then
				for k = 0,3 do
					local kIcon = sdw:GetControl("IMG_BONUS" .. k)
					if	kIcon:IsNil() == false then
						kIcon:SetCustomDataAsInt(item[k])
						DrawMissionRewardItem(kIcon)
					end
				end
			end
		end
	end

	g_world:RemoveAllGradeMonster(1)
	g_world:RemoveAllGradeMonster(2)
	ODS("Net_PT_N_C_NFY_MISSION_RESULT ��\n")
	LockPlayerInputMove(4) -- for mission result lock
	LockPlayerInput(4)
end

function Net_PT_M_C_NFY_COMPLETE_SECTION(packet)
	ODS("Net_PT_M_C_NFY_COMPLETE_SECTION \n")	
end

function Net_PT_M_C_NFY_REJECT_STAGE_MAP_MOVE(packet)
	ODS("Net_PT_M_C_NFY_REJECT_STAGE_MAP_MOVE \n")
	local iMessage = packet:PopInt()
	AddWarnDataTT(400232+iMessage)
end

function Net_PT_M_C_NFY_COMPLETE_SECTION(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	ODS("Net_PT_M_C_NFY_COMPLETE_SECTION \n")
	ODS("���� �ϳ� �ϼ�")
	local kMyActor = GetMyActor()
	if kMyActor:IsNil() == false then
		g_MissionWallGroup = g_MissionWallGroup + 1
		ODS("g_MissionWallGroup : " .. g_MissionWallGroup .. "\n")
		kMyActor:SetActiveGrp(g_MissionWallGroup, false)	-- ������ Ǯ��
		if g_world:IsNil() == false then
			g_world:RemoveCameraWall("c_wall_section_" .. g_MissionWallGroup)
		end
		g_MissionWallGroup = g_MissionWallGroup + 1			-- �ϳ� �� ����
		if false == IsDefenceGameMode() then
			CallUI("FRM_GOGO")
		end
	end
end

function ALiveMonsterCountUI(bShow)
	if nil == bShow then
		bShow = false
	end
	
	local kUIName = "FRM_REST_MON_NUM"
	if bShow then
		ActivateUI(kUIName)
	else
		CloseUI(kUIName)
	end
end

function Net_PT_M_C_NFY_REST_MONSTER_NUM(packet)
    ODS("Net_PT_M_C_NFY_REST_MONSTER_NUM \n")
    local iNum = packet:PopSize_t()    --���������� ���� ���������� �����ϱ�.
	ShowUI_RestMonsterNum(iNum)
end

function ShowUI_RestMonsterNum(iNum)
	g_RestMonsterNum = iNum

	--AddWarnDataStr(WideString(GetTT(400239):GetStr() .. " " .. iNum))

	local eMapAttr = 0
	if nil ~= g_world then
		eMapAttr = g_world:GetAttr()
	end

	local wnd = ActivateUI("FRM_REST_MON_NUM")
	if wnd:IsNil() then
		return
	end

	local NumText = tostring(iNum)
	local Loc = wnd:GetLocation()
	if true == IsDefenceMode() then
		if g_bConstellationMission then
			Loc:SetY(191)
		else
			Loc:SetY(220)
		end
	elseif (IsDefenceMode7() or IsDefenceMode8()) then
		iNum = 0
		CheckActiveStatusUI()
	elseif GATTR_CONSTELLATION_GROUND == eMapAttr or GATTR_CONSTELLATION_BOSS == eMapAttr then
		Loc:SetY(152)
	else
		Loc:SetY(254)
	end

	if (false==GetUIWnd("SFRM_CONSTELLATION_DEFENCE_EXIT"):IsNil() or false==GetUIWnd("SFRM_CONSTELLATION_EXIT"):IsNil()) and GetUIWnd("SFRM_CONSTELLATION_TIMER"):IsNil() then
		Loc:SetY(Loc:GetY() - 53)
	end

	wnd:SetLocation(Loc)
	wnd:SetStaticText(NumText)
	
    if 0==iNum and GATTR_MISSION==eMapAttr and false==IsDefenceMode() then
		if false == ((g_iNowCheckedCardNum+1) >= g_iMissionMapCountLua) then
			if misdiff < 5 then
				CallUI("FRM_GOGO")
			end
		end
	    CloseUI("FRM_REST_MON_NUM")
    end
end

function Net_PT_M_C_NFY_STAGECLEAR_GOGO()
	if false == ((g_iNowCheckedCardNum+1) >= g_iMissionMapCountLua) then
		if false == IsDefenceGameMode() then
			CallUI("FRM_GOGO")
		end
	end
end

function Net_PT_C_M_NFY_TOUCH_SECTION_TRIGGER_OnEnter(trigger, actor)
	ODS("PT_C_M_NFY_TOUCH_SECTION_TRIGGER \n")
	if trigger:IsEnable() then
		local packet = NewPacket(13420)
		packet:PushInt(trigger:GetParam())
		Net_Send(packet)
		DeletePacket(packet)
		trigger:SetEnable(false)
	end
	return true
end
function Net_PT_C_M_NFY_TOUCH_SECTION_TRIGGER_OnUpdate(trigger, actor)
end
function Net_PT_C_M_NFY_TOUCH_SECTION_TRIGGER_OnLeave(trigger, actor)
end

function TurnOffCameraWall_OnEnter(trigger, actor)
	local i1WallNum = trigger:GetParam()
	local i2WallNum = trigger:GetParam2()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	if i1WallNum >= 0 then
		g_world:RemoveCameraWall("c_wall_section_" .. i1WallNum)
	end

	if i2WallNum >= 0 then
		g_world:RemoveCameraWall("c_wall_section_" .. i2WallNum)
	end
end
function TurnOffCameraWall_OnUpdate(trigger, actor)
end
function TurnOffCameraWall_OnLeave(trigger, actor)
end

function TurnOnCameraWall_OnEnter(trigger, actor)
	local i1WallNum = trigger:GetParam()
	local i2WallNum = trigger:GetParam2()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	if i1WallNum >= 0 then
		g_world:RecoverCameraWall("c_wall_section_" .. i1WallNum)
	end

	if i2WallNum >= 0 then
		g_world:RecoverCameraWall("c_wall_section_" .. i2WallNum)
	end
end
function TurnOnCameraWal_OnUpdate(trigger, actor)
end
function TurnOnCameraWall_OnLeave(trigger, actor)
end

function GetRewardItemOpenSoundName()
	return "Item_Give"
end

function GetQuickPage()
	return g_iQuickPage;
end
function ChageQuickSlot(wndPage, i)
	if wndPage:IsNil() == false then
		--local iPage = tonumber(wndPage:GetStaticText():GetStr())
		--iPage = iPage + i
		
		g_iQuickPage = ChangeQuickSlotViewPage(i)
		
		if g_iQuickPage > QUICKSLOT_PAGE_COUNT then
			g_iQuickPage = g_iQuickPage-QUICKSLOT_PAGE_COUNT
		elseif 1 > g_iQuickPage then
			g_iQuickPage = QUICKSLOT_PAGE_COUNT+g_iQuickPage
		end
		wndPage:SetStaticText(tostring(g_iQuickPage))
	end
end

function InitMiniCard(wnd, MisNum)
	ODS("\t== InitMiniCard ==\n", false, 1509)
	local StringPath = GetMissionImgPath(MisNum)
	g_szMissionBGFileName = StringPath

	if wnd:IsNil() == true or g_iNextCount ~= 0 then
		return false
	end
	
	for i=1,MAX_MINICARD_NUM do
		local ii = i - 1
		local wndCard = wnd:GetControl("FRM_CARD_BG"..ii)
		if wndCard:IsNil() == false then
			wndCard:Visible(true) 
		end
	end

 	if g_iNextCount == 0 then
		InitMission()
		ODS("BBBBBBBBBBBBBBBBBBBBB\n")
	end
	
	if misdiff == 5 or misdiff == 6 then
		return true
	end
	CallUI("FRM_MISSION_START")
	ODS("AAAAAAAAAAAAAAAAAAAA"..g_iNextCount.."\n")
	return true
end

function CallMissionRankingRewardItem(kText)
	local RewardItemNo1 = MissionRankingRewardItem( GetMissionNo(), misdiff, 1 )
	local RewardItemNo2 = MissionRankingRewardItem( GetMissionNo(), misdiff, 2 )
	local RewardItemNo3 = MissionRankingRewardItem( GetMissionNo(), misdiff, 3 )
	
--	if (RewardItemNo1 ~= 0) and (RewardItemNo2 ~= 0) and (RewardItemNo3 ~= 0) then
		DisplayMissionRankingRewardItem( RewardItemNo1, RewardItemNo2, RewardItemNo3, kText )
--	end
end

function DisplayMissionRankingRewardItem( RewardItemNo1, RewardItemNo2, RewardItemNo3, kText )	
	CallUI( "FRM_MISSION_RANKING_REWARD_ITEM" )
	
	local TopWnd = GetUIWnd( "FRM_MISSION_RANKING_REWARD_ITEM" )
	if TopWnd:IsNil() == false then
--		local RewardIcon1 = TopWnd:GetControl( "ICN_ITEM1" )
--		local RewardIcon2 = TopWnd:GetControl( "ICN_ITEM2" )
--		local RewardIcon3 = TopWnd:GetControl( "ICN_ITEM3" )
		local Rank = TopWnd:GetControl( "FRM_RANK" )
		
--		if RewardIcon1:IsNil() == true then return end
--		if RewardIcon2:IsNil() == true then return end
--		if RewardIcon3:IsNil() == true then return end
		if Rank:IsNil() == true then return end
		
--		RewardIcon1:SetCustomDataAsInt( RewardItemNo1 )
--		RewardIcon2:SetCustomDataAsInt( RewardItemNo2 )
--		RewardIcon3:SetCustomDataAsInt( RewardItemNo3 )
		Rank:SetStaticText(kText)
		
		local parent = GetUIWnd("FRM_MISSION_START")
		if false==parent:IsNil() then
			local pos = TopWnd:GetLocation()
			pos:SetY(parent:GetLocation():GetY()+170)
			TopWnd:SetLocation(pos)
		end
	end
end

function SetMiniCard(wnd, iNow)	
	if nil==wnd or wnd:IsNil() then
		return
	end
	ODS("\t== SetMiniCard ("..iNow.. "/" .. g_iMissionMapCountLua..") ==\n",false,6482)
	for i=1,MAX_MINICARD_NUM do
		local ii = i - 1
		
		local wndCard = nil
		wndCard = wnd:GetControl("FRM_CARD_BG"..ii)
		if wndCard:IsNil() == false then
			wndCard:GetControl("IMG_CARD"):Visible(i<=g_iMissionMapCountLua)
			local kPath = g_szMissionBGFileName..i.."s.tga"
			if i >= g_iMissionMapCountLua then
				kPath = "../Data/6_ui/mission/msIngCardBoss.tga"
			else
				if iNow<ii then
					kPath = "../Data/6_ui/mission/msIngCardQst.tga"
				end
			end
			wndCard:GetControl("IMG_CARD"):ChangeImage(kPath) 
			wndCard:SetInvalidate(true)

			local iUVIdx = 4
			if ii==iNow or (iNow>=MAX_MINICARD_NUM and i==MAX_MINICARD_NUM) then
				iUVIdx = 3
			end			
			wndCard:GetControl("IMG_CARD"):GetControl("IMG_CARD_STATE"):SetUVIndex( iUVIdx )
		end
	end
end


function Net_PT_M_C_NFY_MISSION_TIME_LIMIT(packet)
	g_iMissionTotalTimeLimit = packet:PopInt()
	g_fMissionStartTimeLimit = GetCurrentTimeInSec()
	
	CallUI("FRM_TIMELIMIT")
end

function DisplayMissionTimeLimit(wnd)
	if wnd:IsNil() == true then
		return
	end

	local fNow = GetCurrentTimeInSec()*1000
	
	local iMissionTime;
	
	iMissionTime = g_iMissionTotalTimeLimit - (g_iMissionTimeLimit + fNow - g_fMissionStartTimeLimit*1000)
	if iMissionTime < 0 then
		iMissionTime = 0;
	end
	

	local iDelta = math.floor(iMissionTime*0.1)
	local iSec100 = iDelta%10 + 1
	local iSec1000 = math.floor((iDelta%100)/10) + 1	
	iDelta = math.floor(iMissionTime*0.001)
	local iSec1 = iDelta%10 + 1
	local iSec10 = math.floor((iDelta%60)/10) + 1
	local iMin1 = math.floor(iDelta/60)%10 + 1
	local iMin10 = math.floor(iDelta/600)%10 + 1	
	
	local wnd100sec = wnd:GetControl("FRM_SEC100")
	if wnd100sec:IsNil() == false then
		wnd100sec:SetUVIndex(iSec100)
	end

	local wnd1000sec = wnd:GetControl("FRM_SEC1000")
	if wnd1000sec:IsNil() == false then
		wnd1000sec:SetUVIndex(iSec1000)
	end
	
	local wnd1sec = wnd:GetControl("FRM_SEC1")
	if wnd1sec:IsNil() == false then
		wnd1sec:SetUVIndex(iSec1)
	end

	local wnd10sec = wnd:GetControl("FRM_SEC10")
	if wnd10sec:IsNil() == false then
		wnd10sec:SetUVIndex(iSec10)
	end

	local wnd1min = wnd:GetControl("FRM_MIN1")
	if wnd1min:IsNil() == false then
		wnd1min:SetUVIndex(iMin1)
	end

	local wnd10min = wnd:GetControl("FRM_MIN10")
	if wnd10min:IsNil() == false then
		wnd10min:SetUVIndex(iMin10)
	end

	local kPathDefault = "../Data/6_ui/mission/msNumRs.tga"
	local kPathsDefault = "../Data/6_ui/mission/msNumTmRs.tga"	
	local kPath = "../Data/6_ui/mission/msNumRs.tga"
	local kPaths = "../Data/6_ui/mission/msNumTmRs.tga"
	
	
	
	if iDelta <= 60 then
		if g_iSecChange ~= iSec1 then
			PlaySoundByID("Mission_Clock")
		end
	end
	
	g_iSecChange = iSec1
	
	if iDelta <= 180 then	
		wnd100sec:ChangeImage(kPaths)
		wnd100sec:SetInvalidate(true)
		wnd1000sec:ChangeImage(kPaths)
		wnd1000sec:SetInvalidate(true)
		wnd10sec:ChangeImage(kPath)
		wnd10sec:SetInvalidate(true)
		wnd1sec:ChangeImage(kPath)
		wnd1sec:SetInvalidate(true)
		wnd1min:ChangeImage(kPath)
		wnd1min:SetInvalidate(true)
		wnd10min:ChangeImage(kPath)
		wnd10min:SetInvalidate(true)
	else
		wnd100sec:ChangeImage(kPathsDefault)
		wnd100sec:SetInvalidate(true)
		wnd1000sec:ChangeImage(kPathsDefault)
		wnd1000sec:SetInvalidate(true)
		wnd10sec:ChangeImage(kPathDefault)
		wnd10sec:SetInvalidate(true)
		wnd1sec:ChangeImage(kPathDefault)
		wnd1sec:SetInvalidate(true)
		wnd1min:ChangeImage(kPathDefault)
		wnd1min:SetInvalidate(true)
		wnd10min:ChangeImage(kPathDefault)
		wnd10min:SetInvalidate(true)	
	end
	
	local kBossAttrWnd = GetUIWnd("FRM_BOSS_ATTR_INFO")
	if nil~=kBossAttrWnd and false==kBossAttrWnd:IsNil() then
		local kAttrPos = kBossAttrWnd:GetLocation()
		local kAttrSize = kBossAttrWnd:GetSize()
		if 0 ~= kAttrPos:GetY() then
			wnd:SetLocation( Point2(wnd:GetLocation():GetX(), kAttrPos:GetY()+kAttrSize:GetY()) )
		else
			wnd:SetLocation( Point2(wnd:GetLocation():GetX(), 106) )
		end
	else
		wnd:SetLocation( Point2(wnd:GetLocation():GetX(), 106) )
	end
end

function IsDefenceGameMode()
	if IsDefenceMode() or IsDefenceMode7() or IsDefenceMode8() then
		return true
	end
	
	return false
end

function IsStrategyDefenceMode()
	if IsDefenceMode7() then return true end
	if IsDefenceMode8() then return true end
	return false
end

function IsDefenceMode7()
	if (8 == misdiff) then
		return true
	end
	
	return false
end

function IsDefenceMode8()
	if (9 == misdiff) then
		return true
	end
	
	return false
end

function IsDefenceMode()
	if (7 == misdiff) then
		return true
	end
	
	return false
end

function IsLastStage()
	if (true == IsDefenceMode()) then
		if( g_iMissionStage == g_iMissionMapCountLua ) then
			return true
		end
	else
		return true
	end
	
	return false
end

function Start_Defence_Mission(iMissionNo, iMissionLv, iStageNo, iStageCount, iDirection, bSelectSuccess, iSuccessCount)
	if 0==iMissionNo then
		return
	end

	g_iMissionStage = iStageNo
	g_iMissionStageCount = iStageCount
	g_iDirection = iDirection
	g_iMissionWave = 0
	misdiff = iMissionLv
	if g_world~= nil and g_world:IsNil() == false then
		g_world:SetDifficultyLevel(misdiff)
	end

	if (g_iMissionStage == 0) then --ù��° ���������� ��� �ʱ�ȭ
		MissionMgrInit()
		CloseUI("FRM_STATUS_UP")
		if false==IsDefenceMode() then
			CloseUI("FRM_PARTY_STATE")
		end
	end

	if IsDefenceMode7() then
		Start_Defence_F7(iMissionNo, bSelectSuccess)
	else
		Start_Defence_F6(iMissionNo, bSelectSuccess, iSuccessCount)
	end
end

function Start_Defence_F6(iMissionNo, bSelectSuccess, iSuccessCount)
	if (g_iMissionStage == 0) then --ù��° ���������� �����ϴ� ������.
		if 1 == g_iDirection then
			iMissionNo = (iMissionNo * 10)+1
			GetEventScriptSystem():ActivateEvent(iMissionNo)
		else
			iMissionNo = (iMissionNo * 10)+2
			GetEventScriptSystem():ActivateEvent(iMissionNo)
		end
		return
	end
	
	local SelectTopWnd = ActivateUI("FRM_MISSION_SELECT_DIRECTION")
	if SelectTopWnd:IsNil() == false then
		local SelectWnd = SelectTopWnd:GetControl("IMG_MAIN")
		if SelectWnd:IsNil() == false then
			if bSelectSuccess == true then
				SelectWnd:SetCustomDataAsInt(1)
			else
				SelectWnd:SetCustomDataAsInt(0)
			end
		end
		SelectWnd = SelectTopWnd:GetControl("IMG_BG")
		if SelectWnd:IsNil() == false then
			SelectWnd:SetCustomDataAsInt(iSuccessCount)
		end
		
		SelectWnd = SelectTopWnd:GetControl("FRM_TEXT_TITLE")
		if SelectWnd:IsNil() == false then
			SelectWnd:SetCustomDataAsFloat(GetAccumTime())
		end
		SelectTopWnd:SetCustomDataAsInt(1)
	end

	GetEventTimer():Add("DEFENCE_F6_START_NEXTSTAGE", g_mode6_StartNextStageDelayTime, "StartNextStage()", 1, false)
end

function Start_Defence_F7(iMissionNo, bSelectSuccess)
	if (g_iMissionStage == 0) then --ù��° ���������� �����ϴ� ������.
		if GetMyActor():GetAbil(AT_TEAM) == 2 then
			iMissionNo = (iMissionNo * 100)+2
			GetEventScriptSystem():ActivateEvent(iMissionNo)
		else
			iMissionNo = (iMissionNo * 100)+1
			GetEventScriptSystem():ActivateEvent(iMissionNo)
		end
		return
	elseif ( g_iMissionStage == 10 ) then -- 11��° ���������� ����Ʈ ������ ���� ����
		CallPointCopyMachine(g_iMissionStage) -- �Լ� ������ ����ó�� �Ǿ�����.
	end
	
	local iNowStage = g_iMissionStage+1
	StartNextStage()
	CallDefenceMissionReward(iNowStage,g_iMissionStageCount)
end

function Send_PT_C_M_NFY_DEFENCE_STAGE()
	local packet = NewPacket(13471)--PT_C_M_NFY_DEFENCE_STAGE
	Net_Send(packet)
end

function Send_PT_C_M_REQ_DEFENCE_SELECT_CLOSE()
	local kMissionSelectUI = GetUIWnd("FRM_MISSION_SELECT_DIRECTION")
	if kMissionSelectUI:IsNil() == true then
		return
	end

	local kBtnItemLeft = kMissionSelectUI:GetControl("BTN_USE_ITEM_LEFT")
	local kBtnItemRight = kMissionSelectUI:GetControl("BTN_USE_ITEM_RIGHT")
	if kBtnItemLeft:IsNil() == true or kBtnItemRight:IsNil() == true then
		return
	end
	
	local kTextLeft = kBtnItemLeft:GetControl("FRM_L_COUNT_INFALLIBLE_COUNT")
	local kTextRight = kBtnItemRight:GetControl("FRM_R_COUNT_INFALLIBLE_COUNT")
	if kTextLeft:IsNil() == true or kTextRight:IsNil() == true then
		return
	end

	if kTextLeft:GetCustomDataAsInt() > 0 or kTextRight:GetCustomDataAsInt() > 0 then
		return
	end
	
	local packet = NewPacket(13489)--PT_C_M_REQ_DEFENCE_SELECT_CLOSE
	Net_Send(packet)
end

function Set_Defence_Timer(StageTime)
	g_iMissionTotalTime = StageTime
	CallUI("FRM_MISSION_DEFENCE_TIMER")
end

g_CommunityEventWaitTime = 0
function Set_CommunityEvent_Wait_Timer(WaitTime)
	g_CommunityEventWaitTime = WaitTime / 1000
	CallUI("FRM_COMMUNITY_EVENT_WAIT_TIMER")
end

function MakeWaitTimeText()
	local Minute =  math.floor((g_CommunityEventWaitTime - g_NowEventTime)/ 60)
	local Second = (g_CommunityEventWaitTime - g_NowEventTime) % 60
	
	local str1 = GetTextW(302108):GetStr().." "
	local str2 = string.format(GetTextW(315):GetStr(), Minute)
	local str3 = string.format(GetTextW(316):GetStr(), Second)
	local str4 = " "..GetTextW(302109):GetStr()

	local AllStr = str1..str2..str3..str4
	local AllStr2 = str1..str2..str4
	if 0 == Second then
		g_ChatMgrClient:AddLogMessage(WideString(AllStr2), true, 0, 12)
	else
		g_ChatMgrClient:AddLogMessage(WideString(AllStr), true, 0, 12)
	end
end

function Add_Defence_Time(iAddTime)
	g_iMissionTotalTime = g_iMissionTotalTime + iAddTime
	CloseUI("FRM_DEFENCE_TIMELIMIT")
end

function Start_Defence_Wave( bIsLastWave, iDirection )
	g_iMissionWave = g_iMissionWave+1
	if (g_iMissionWave == 1 or bIsLastWave == true) then
		Send_PT_C_M_NFY_DEFENCE_WAVE()
		return
	else
		if IsStrategyDefenceMode() then
			ActivateUI("FRM_MODE7_NEXTWAVE")
		else
			if 1 == iDirection then
				ActivateUI("FRM_DEFENCE_LEFT")
			else
				ActivateUI("FRM_DEFENCE_RIGHT")
			end
		end
		PlaySoundByID("defence-monsterin")
		return
	end
end

function DefenceMonsterZenEffect(bDir)
	if g_world == nil then
		return
	end
	
	local pos

	if IsStrategyDefenceMode() then
		if true == bDir then
			pos = g_world:FindTriggerLoc("B_Mon_7")
		else
			pos = g_world:FindTriggerLoc("B_Mon_2")
		end
		fScale = 0.8
	else
		if true == bDir then
			pos = g_world:FindSpawnLoc("char_spawn_Left")
		else
			pos = g_world:FindSpawnLoc("char_spawn_Right")
		end
		fScale = 1.0
	end
	pos:SetZ(pos:GetZ()+50)
	
	local ptcl = g_world:ThrowRay(pos, Point3(0,0,-1), 500);
	if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
		return
	end
	ptcl:SetZ(ptcl:GetZ()+5);
	g_world:AttachParticleSWithRotate("ef_MonGroup_Create", ptcl, fScale, Point3(0,0,0));					
end

function Send_PT_C_M_NFY_DEFENCE_WAVE()
	local kPacket = NewPacket(13473)--PT_C_M_NFY_DEFENCE_WAVE
	Net_Send(kPacket)

	CloseUI("FRM_DEFENCE_LEFT")
	CloseUI("FRM_DEFENCE_RIGHT")
	CloseUI("FRM_MODE7_NEXTWAVE")
end

function End_Defence_Stage()
	local iMissionNo = GetMissionNo()
	if IsStrategyDefenceMode() then
		iMissionNo = (iMissionNo * 100)+3
	else
		iMissionNo = (iMissionNo * 10)+4
	end
	GetEventScriptSystem():ActivateEvent(iMissionNo);
end

function End_Defence_Mission()
	CloseUI("FRM_MISSION_DEFENCE_TIMER")
	local iMissionNo = GetMissionNo()
	if IsStrategyDefenceMode() then
		if GetMyActor():GetAbil(AT_TEAM) == 2 then
			iMissionNo = (iMissionNo * 100)+5
		else
			iMissionNo = (iMissionNo * 100)+4
		end
	else
		iMissionNo = (iMissionNo * 10)+3
	end
	GetEventScriptSystem():ActivateEvent(iMissionNo);
end

function Send_PT_C_M_NFY_DEFENCE_ENDSTAGE()
	local kPacket = NewPacket(13475)--PT_C_M_NFY_DEFENCE_ENDSTAGE
	Net_Send(kPacket)
end

function Send_PT_C_M_NFY_DEFENCE_DIRECTION(kDirection)
	local kPacket = NewPacket(13479)--PT_C_M_NFY_DEFENCE_DIRECTION(
	kPacket:PushByte(kDirection)
	Net_Send(kPacket)
end

function CallCommunityEventMiniCard(EventNo)
	local TimerWnd = GetUIWnd("BTN_TIMER")
	if false == TimerWnd:IsNil() then
		TimerWnd:Close()
	end

	local wndInfo = ActivateUI("FRM_COMMUNITY_EVENT_MINICARD")
	if	wndInfo:IsNil() == true then
		return false
	end
	
	if 0 ~= EventNo then
		wndInfo:SetCustomDataAsInt(EventNo)
	end
	
	local MisNum = GetMissionNo()
	local misname = GetMapNameW( g_mapNo ):GetStr()	-- int���� char�� �ص� �ɵ�.
	if nil ~= misname then
		ODS("�̼� ���� : " .. misname .. "�ʹ�ȣ : " .. g_mapNo .. "\n")
	else
		misname = " "
	end

	local frmBG = wndInfo:GetControl("FRM_BG");
	if frmBG:IsNil() == true then
		return false
	end	
	frmBG:Visible(true)
	frmBG:SetStaticText(GetTextW(400218):GetStr() .. misname)
	
	local member = frmBG:GetControl("FRM_MEMBER")
	if member:IsNil() == true then
    	return false
    end
	member:Visible(true)
	
	local diff = frmBG:GetControl("FRM_DIFF")
	if diff:IsNil() == true then
    	return false
    end
	diff:Visible(true)
	diff:SetStaticText("{C=0xFFf7ea96/T=FTcts_13/}" .. GetTextW(400970):GetStr() .. "{C=0xFFfffbc7/T=FTcts_13/}" .. GetTextW(302106):GetStr())
	
	for i = 0,3 do	-- �� �� ����
	    if frmBG:IsNil() == false then
		    local star = frmBG:GetControl("FRM_STAR".. tostring(i))
		    if star:IsNil() == false then
		    	star:Visible(false)
		    end
		end
	end
	
	CloseCurrentMiniMap() 
	
	return true
end

function CallDefenceMiniCard()
	if g_bConstellationMission then
		return
	end

	if IsDefenceMode7() then
		ActivateUI("FRM_DEFENCE_STAGE_INFO")
		return
	end

	local wndInfo = ActivateUI("FRM_DEFENCE_MINICARD")
	if	wndInfo:IsNil() == true then
		return false
	end
	wndInfo:SetCustomDataAsInt(0)
	
	local MisNum = GetMissionNo()
	local misname = GetDefString( g_mapNo )	-- int���� char�� �ص� �ɵ�.
	if nil ~= misname then
		ODS("�̼� ���� : " .. misname .. "�ʹ�ȣ : " .. g_mapNo .. "\n")
	else
		misname = " "
	end
	ODS("�̼� ���� : " .. misname .. "�ʹ�ȣ : " .. g_mapNo .. "\n")
	
	ODS("�̼� ���� : " .. misname .. " ���̵� : " .. misdiff .. "�̼� ��ȣ : " ..MisNum.. " g_iMissionMapCountLua : " .. g_iMissionMapCountLua .. "\n", false, 1)
	local frmBG = wndInfo:GetControl("FRM_BG");
	if frmBG:IsNil() == true then
		return false
	end	
	frmBG:Visible(true)
	frmBG:SetStaticText(GetTextW(400218):GetStr() .. misname)
	
	local member = frmBG:GetControl("FRM_MEMBER")
	if member:IsNil() == true then
    	return false
    end
	member:Visible(true)
	member:SetStaticText(GetTextW(400219):GetStr() .. tostring(mismember) .. GetTextW(400221):GetStr())
	
	local diff = frmBG:GetControl("FRM_DIFF")
	if diff:IsNil() == true then
    	return false
    end
	diff:Visible(true)
	diff:SetStaticText("{C=0xFFf7ea96/T=FTcts_13/}" .. GetTextW(400970):GetStr() .. "{C=0xFFfffbc7/T=FTcts_13/}" .. GetTextW(400956):GetStr())
	
	for i = 0,3 do	-- �� �� ����
	    if frmBG:IsNil() == false then
		    local star = frmBG:GetControl("FRM_STAR".. tostring(i))
		    if star:IsNil() == false then
		    	star:Visible(false)
		    end
		end
	end
	
	return true
end

function StartNextStage()
	g_iMissionWave = g_iMissionWave+1
	CallDefenceRewardText()	
	SetMissionStageData(g_iMissionStage+1, g_iMissionStageCount+1)

	if IsStrategyDefenceMode() then
		ActivateUI("FRM_MODE7_NEXTWAVE")
	else
		if 1 == g_iDirection then
			ActivateUI("FRM_DEFENCE_LEFT")
		else
			ActivateUI("FRM_DEFENCE_RIGHT")
		end
	end
	CloseUI("FRM_MISSION_SELECT_DIRECTION")
	SetResultStage()
end

function SetMissionStageData(iStageNo, iStageCount)
	if g_bConstellationMission then
		local kTopWnd = ActivateUI("FRM_CONSTELLATION_START")
		local kTextWnd = kTopWnd:GetControl("FRM_DUNGEON_START_NAME")
		local vStrText = GetTT(798824)
		vStrText:ReplaceStr("#STAGE_NO#", tostring(iStageNo))
		kTextWnd:SetStaticText(vStrText:GetStr())

		kTopWnd = CallConstellationExit()
		if kTopWnd:IsNil() then
			return
		end
		kTextWnd = kTopWnd:GetControl("FRM_TEXT_STAGE")

		vStrText = GetTT(400961)
		vStrText = "{C=0xFFFFF799/}".. vStrText:GetStr() .. " : {C=0xFFFFFFFF/}" .. tostring(iStageNo) .. " / " .. tostring(iStageCount)
		kTextWnd:SetStaticText(vStrText)

		CallConstellationMission()
		return
	end

	local kTopWnd = ActivateUI("FRM_MISSION_STAGE_START")
	if kTopWnd:IsNil() then
		return
	end

	local kTextWnd = kTopWnd:GetControl("FRM_TEXT")
	if kTextWnd:IsNil() then
		return
	end

	local vStrText = GetTT(400962)
	vStrText:ReplaceStr("#STAGE_NO#", tostring(iStageNo))
	kTextWnd:SetStaticText(vStrText:GetStr())


	kTopWnd = ActivateUI("SFRM_MISSION_STAGE_DATA")
	if kTopWnd:IsNil() then
		return
	end

	kTextWnd = kTopWnd:GetControl("FRM_TEXT_STAGE")
	if kTextWnd:IsNil() then
		return
	end

	vStrText = "{C=0xFF00FFFF/}" .. tostring(iStageNo) .. "{C=0xFFFFFFFF/} / " .. tostring(iStageCount)
	kTextWnd:SetStaticText(vStrText)
end

function Close_FRM_MISSION_STAGE_START()
	if false==IsDefenceGameMode() then
		return
	end

	CloseUI("IMG_DEFENCE_REWARD_STAGE")
	Send_PT_C_M_NFY_DEFENCE_STAGE()
	if IsStrategyDefenceMode() and ( GetHideDamageStageNo() == g_iMissionStage ) then
		AddWarnDataTT(401176)
	end
end

function SetDefenceBonusExp(iTargetScore)
	local WndExp = GetUIWnd("SFRM_MISSION_ADD_EXP")
	if WndExp:IsNil() == true then
		return
	end
	
	local WndTop = WndExp:GetControl("FRM_EXP_NUMBER")
	if WndTop:IsNil() == true then
		return
	end
	
	local iMaxNum = 8	
	if (0 == iTargetScore) then
		for index = 0,iMaxNum-1 do
			local WndNumber = WndTop:GetControl("FRM_"..index)
			if WndNumber:IsNil() == false then
				WndNumber:SetUVIndex(1)
				if index ~= 0 then
					WndNumber:Visible(false)
				end
				WndNumber:SetInvalidate(true)
			end
		end
		return
	end
	
	local TimeCnt = GetFrameTime()
	WndTop:SetCustomDataAsFloat(TimeCnt)
	
	local WndBG = WndTop:GetControl("IMG_BG1")
	if WndBG:IsNil() == false then
		WndBG:SetCustomDataAsInt(1)
	end
	
	for i = 0,iMaxNum-1 do
		local iReversIndex = iMaxNum-i-1
		
		if 0 == iReversIndex then
			WndNumber = WndTop:GetControl("FRM_0")
			if WndNumber:IsNil() == false then
				WndNumber:SetCustomDataAsInt(iTargetScore+1)
			end
		else
			local iMultyNum = Math_Pow(10, iReversIndex)
			if 0 >= iMultyNum then
				iMultyNum = 1
			end
			
			local iScore = math.floor(iTargetScore/iMultyNum)+1
			iTargetScore = math.floor(iTargetScore % iMultyNum)
			local WndNumber = WndTop:GetControl("FRM_"..iReversIndex)
			if WndNumber:IsNil() == false then
				WndNumber:SetCustomDataAsInt(iScore)
			end
		end
	end
	
end

function StartDefenceBonusExp(WndTop, iInterTime, iMaxNum)
	if WndTop:IsNil() == true then
		return
	end
	
	local WndBG = WndTop:GetControl("IMG_BG1")
	if WndBG:IsNil() == true then
		return
	end
	
	if 1 ~= WndBG:GetCustomDataAsInt() then
		return
	end
	
	if 0 >= iInterTime or 0 >= iMaxNum then
		return
	end
	
	local TimeCnt = WndTop:GetCustomDataAsFloat()
	TimeCnt = TimeCnt+GetFrameTime()
	
	local iShowIndex = iMaxNum-1
	
	--Calculate Show Index
	for i = 0,iMaxNum-1 do
		local iReversIndex = iMaxNum-i-1
		WndNumber = WndTop:GetControl("FRM_"..iReversIndex)
		if WndNumber:IsNil() == false then
			if 1 == WndNumber:GetCustomDataAsInt() and iReversIndex >= iShowIndex then
				iShowIndex = iReversIndex-1
			end
		end
	end
	
	--Update Number
	for i = 0,iMaxNum-1 do
		if(i*iInterTime < TimeCnt) then 
			WndNumber = WndTop:GetControl("FRM_"..i)
			if WndNumber:IsNil() == false then
				if iShowIndex >= i then
					WndNumber:GetControl("FRM_UPDATE"):Visible(true)
				end
				if iShowIndex < i and WndNumber:GetCustomDataAsInt() == 1 then
					WndNumber:Visible(false)
				else
					WndNumber:Visible(true)
				end
			end
			
			if iMaxNum-1 == i then
				WndBG:SetCustomDataAsInt(2)
				WndTop:SetCustomDataAsFloat(GetFrameTime())
				return
			end
		end
	end
	
	WndTop:SetCustomDataAsFloat(TimeCnt)
end

function StopDefenceBonusExp(WndTop, iInterTime, iMaxNum)
	if WndTop:IsNil() == true then
		return
	end
	
	local WndBG = WndTop:GetControl("IMG_BG1")
	if WndBG:IsNil() == true then
		return
	end
	
	if 2 ~= WndBG:GetCustomDataAsInt() then
		return
	end
	
	if 0 >= iInterTime or 0 >= iMaxNum then
		return
	end
	
	local TimeCnt = WndTop:GetCustomDataAsFloat()
	TimeCnt = TimeCnt+GetFrameTime()
	
	--Stop Number
	for i = 0,iMaxNum-1 do
		if(i*iInterTime < TimeCnt) then 
			WndNumber = WndTop:GetControl("FRM_"..i)
			if WndNumber:IsNil() == false then
				local WndUpdate = WndNumber:GetControl("FRM_UPDATE")
				if WndUpdate:IsNil() == false then
					if WndUpdate:Visible() == true then
						WndUpdate:Visible(false)
					else
						WndNumber:SetUVIndex(WndNumber:GetCustomDataAsInt())
					end
				end
				if iMaxNum-1 == i then
					WndBG:SetCustomDataAsInt(0)
				end
			end
		end
	end

	WndTop:SetCustomDataAsFloat(TimeCnt)
end

g_iScoreAniLastIndex = 0
function UpdateDefenceExpAni(kSelf, bNotAllUpdate)
	if kSelf:IsNil() == true then
		return
	end
	
	local WndNumber = kSelf:GetParent()
	if WndNumber:IsNil() == true then
		return
	end
	
	--ODS("Index: " .. WndNumber:GetBuildIndex() .. ", Last: " .. kSelf:GetCustomDataAsInt() .. "\n")
	--[[�ӵ� ������ ���� ����
	if nil~=bNotAllUpdate or bNotAllUpdate then
		if kSelf:GetCustomDataAsInt() then
			return
		end
	end]]
	
	-- ODS("Index: " .. WndNumber:GetBuildIndex() .. ", Last: " .. g_iScoreAniLastIndex .. "\n")
	if nil~=bNotAllUpdate or bNotAllUpdate then
		if WndNumber:GetBuildIndex()>=g_iScoreAniLastIndex then
			return
		end
	end

	local iIndex = WndNumber:GetUVIndex() + 1
	if iIndex >= 11 then
		iIndex = 1
	end
	if iIndex % 3 == 0 then
		PlaySoundByID("defence-getpointcount")
	end
	WndNumber:SetUVIndex(iIndex)
end

g_OldDefencePoint = 0
function SetDefencePoint(WndExp, iTargetScore)
	--local WndExp = GetUIWnd("SFRM_MISSION_ADD_EXP")
	if WndExp:IsNil() == true then
		return
	end
	
	local WndTop = WndExp:GetControl("FRM_EXP_NUMBER")
	if WndTop:IsNil() == true then
		return
	end
	if iTargetScore < 0 then
		iTargetScore = 0
	end

	if g_OldDefencePoint<999999 and 999999<=iTargetScore then
		AddWarnDataTT(401178)
	end
	g_OldDefencePoint = iTargetScore
	
	local iMaxNum = 6
	local iMaxScore = 9
	for i = 0,iMaxNum-2 do
		iMaxScore = iMaxScore * 10 + 9
	end
	--ODS("iMaxScore: " .. iMaxScore .. "\n")
	if iTargetScore > iMaxScore then
		iTargetScore = iMaxScore
	end
	
	if (0 == iTargetScore) then
		for index = 0,iMaxNum-1 do
			local WndNumber = WndTop:GetControl("FRM_"..index)
			if WndNumber:IsNil() == false then
				WndNumber:SetUVIndex(1)
				if index ~= 0 then
					WndNumber:Visible(false)
				end
				WndNumber:SetInvalidate(true)
			end
		end
		return
	end
	ODS(iMaxNum.."\n")
	local TimeCnt = GetFrameTime()
	WndTop:SetCustomDataAsFloat(TimeCnt)
	
	local WndBG = WndTop:GetControl("IMG_BG1")
	if WndBG:IsNil() == false then
		WndBG:SetCustomDataAsInt(1)
	end
	
	g_iScoreAniLastIndex = iMaxNum-1
	for i = 0,iMaxNum-1 do
		local iReversIndex = iMaxNum-i-1
		local iOldScore = 0
		local iScore = 0
		if 0 == iReversIndex then
			WndNumber = WndTop:GetControl("FRM_0")
			if WndNumber:IsNil() == false then
				iScore = iTargetScore+1
				iOldScore = WndNumber:GetCustomDataAsInt()
				WndNumber:SetCustomDataAsInt(iScore)
			end
		else
			local iMultyNum = Math_Pow(10, iReversIndex)
			if 0 >= iMultyNum then
				iMultyNum = 1
			end
			
			iScore = math.floor(iTargetScore/iMultyNum)+1
			iTargetScore = math.floor(iTargetScore % iMultyNum)
			local WndNumber = WndTop:GetControl("FRM_"..iReversIndex)
			if WndNumber:IsNil() == false then
				iOldScore = WndNumber:GetCustomDataAsInt()
				WndNumber:SetCustomDataAsInt(iScore)
				ODS("FRM_"..iReversIndex..", Score:"..iScore.."\n")
			end
		end

		if g_iScoreAniLastIndex-iReversIndex<2 and iOldScore==iScore then
			g_iScoreAniLastIndex = iReversIndex
		end
	end

	--[[for i = 0,iMaxNum-1 do
		
		local iSet = 0
		if i<g_iScoreAniLastIndex then
			iSet = 1
		end
		ODS("Index: " .. i .. ", Last: " .. g_iScoreAniLastIndex .. ", iSet: " .. iSet .. "\n")
		WndNumber:GetControl("FRM_UPDATE"):SetCustomDataAsInt(iSet)
	end]]
end

function Math_Pow(kNum, kCount)
	local kResult = kNum
	for i = 1,kCount-1 do
		kResult = kResult*kNum
	end
	return kResult
end

function SetDefenceEnterImg(iMissionNo)

	local WndTop = GetUIWnd("FRM_CREATE_SCENARIO_ITEM")
	if true == WndTop:IsNil() then
		return
	end
	
	local WndDefence = WndTop:GetControl("FRM_CREATE_FIELD_DEFENCE")
	if true == WndDefence:IsNil() then
		return
	end	
	
	local WndCard = WndDefence:GetControl("FRM_CARD0")
	if true == WndCard:IsNil() then
		return
	end
	
	local WndImg = WndCard:GetControl("IMG_BG")
	if true == WndImg:IsNil() then
		return
	end
	
	
	local kPath = "../Data/6_ui/mission/msDefenseImg_" .. iMissionNo .. ".tga"
	WndImg:ChangeImage(kPath)
	WndImg:SetInvalidate(true)
end

function SetDefenceModeEnterImg(kWnd, iMode)
	if nil==kWnd or kWnd:IsNil() then
		return
	end
	
	local kImg = kWnd:GetControl("IMG_BG")
	if nil==kImg or kImg:IsNil() then
		return
	end
	
	local kPath = "../Data/6_ui/mission/"
	if 7==iMode then
		kPath = kPath .. "msDefense7Img_"
	elseif 8==iMode then
		kPath = kPath .. "msDefense8Img_"
	else
		kPath = kPath .. "msDefenseImg_"
	end	
	kPath = kPath .. GetMissionNo() .. ".tga"

	kImg:ChangeImage(kPath)
	kImg:SetInvalidate(true)
end

function SetResultStage()
	local WndTop = GetUIWnd("SFRM_MISSION_ADD_EXP")
	if true == WndTop:IsNil() then
		return
	end
	
	local WndText = WndTop:GetControl("FRM_TEXT_EXPLAIN")
	if true == WndText:IsNil() then
		return
	end
	
	local iResultStage = GetDefenceResultStage(g_iMissionStage+1)
	local kMsg = string.format(GetTextW(400960):GetStr(), tonumber(iResultStage))
	WndText:SetStaticText(kMsg)
end

function SetCamerModeDefault()
	if g_world ~= nil then
		g_world:SetCameraMode(1,GetMyActor())
	end
end

function SetResultPointText(iPoint)
	local WndExp = GetUIWnd("FRM_POINT_RESULT")
	if WndExp:IsNil() == true then
		return
	end

	local kMsg = GetTextW(401193):GetStr().." "..iPoint
	WndExp:SetStaticText(kMsg)
	WndExp:Visible(true)
end

function SetDefenceResultExpText(iBonusExp)
	local WndExp = GetUIWnd("FRM_DEFENCE_RESULT_EXP")
	if WndExp:IsNil() == true then
		return
	end

	local kLoc = WndExp:GetLocation()
	local ConstellationUI = GetUIWnd("FRM_CONSTELLATION_RESULT")
	if false == ConstellationUI:IsNil() then
		kLoc:SetY(ConstellationUI:GetLocation():GetY() + ConstellationUI:GetSize():GetY() + 10)
	else
		kLoc:SetY(255)
	end
	WndExp:SetLocation(kLoc)

	local kMsg = string.format(GetTextW(400947):GetStr(), tonumber(iBonusExp))	
	WndExp:SetStaticText(kMsg)
	WndExp:Visible(true)
end

function CallDefenceRewardText()
	local iNowStage = g_iMissionStage+1
	local iResultStage = GetDefenceResultStage(iNowStage)
	if iResultStage == iNowStage then
		local kWnd = ActivateUI("IMG_DEFENCE_REWARD_STAGE")
		local kLoc = kWnd:GetLocation()
		if g_bConstellationMission then
			kLoc:SetY(348)
		else
			kLoc:SetY(248)
		end
		kWnd:SetLocation(kLoc)
	end
end


function CallSelectItemToolTip()
	local iNeedItemCount = UISelf:GetCustomDataAsInt()
	local kItemNo = GetMissionCashItemNo()
	kItemName = GetDefString(kItemNo)
	local kMsg = string.format(GetTextW(400944):GetStr(), kItemName,iNeedItemCount)
	if iNeedItemCount == 1 then
		kMsg = kMsg .. GetTextW(400946):GetStr()
	else
		kMsg = kMsg .. GetTextW(400945):GetStr()
	end
	if IsEventMission() == true then
		kMsg = kMsg.."\n\n"..GetTextW(750049):GetStr()
	end
	
	CallToolTipText(0, WideString(kMsg), GetUIWnd("Cursor"):GetLocation()) 
end

function GetDefenceStageNo()
	return g_iMissionStage+1
end

function OnTick_SelectDirection()
	local MissionTopWnd = GetUIWnd("FRM_MISSION_SELECT_DIRECTION")
	if MissionTopWnd:IsNil() == false then
		local iState = MissionTopWnd:GetCustomDataAsInt()
		if iState == 0 then
			return
		end
		
		if iState == 1 then
			local kWnd = MissionTopWnd:GetControl("FRM_TEXT_TITLE")
			local StartUITimeStamp = kWnd:GetCustomDataAsFloat()
			local iDelta = GetAccumTime() - StartUITimeStamp
			local iCalc = 0
			
			if iDelta > g_mode6_SelectSuccessDelayTime then
				iState = iState + 1
				StartUITimeStamp = GetAccumTime()
				
				local WndMain = MissionTopWnd:GetControl("IMG_MAIN")
				if WndMain:IsNil() == false then
					local iSelectSuccess = WndMain:GetCustomDataAsInt()
					if (iSelectSuccess == 1 ) then
						local SuccessWnd = CallUI("FRM_MISSION_SELECT_SUCCESS")
						if (SuccessWnd:IsNil() == false) then
							local TextWnd = SuccessWnd:GetControl("IMG_RIGHT")
							if (TextWnd:IsNil() == false) then
								local WndBg = MissionTopWnd:GetControl("IMG_BG")
								if WndBg:IsNil() == false then
									local iSuccessCount = WndBg:GetCustomDataAsInt()
									TextWnd:SetStaticText(tostring(iSuccessCount) .. GetTextW(400972):GetStr())
								end
							end
						end
					else
						CallUI("FRM_MISSION_SELECT_FAILED")
						local MissionFailedWnd = GetUIWnd("FRM_MISSION_SELECT_FAILED")
						if MissionFailedWnd:IsNil() == false then
							local Wnd = MissionFailedWnd:GetControl("IMG_RIGHT")
							if MissionFailedWnd:IsNil() == false then
								local kItemNo = GetMissionCashItemNo()
								kItemName = GetDefString(kItemNo)
								Wnd:SetStaticText(string.format(GetTextW(400974):GetStr(), kItemName))
							end
						end
					end					
				end
			else
				iCalc = iDelta/0.03*10
			end
			Move_SelectDirection(kWnd, iCalc)
			kWnd = MissionTopWnd:GetControl("IMG_MAIN")
			Move_SelectDirection(kWnd, iCalc)
			kWnd = MissionTopWnd:GetControl("BTN_LEFT")
			Move_SelectDirection(kWnd, iCalc)
			kWnd = MissionTopWnd:GetControl("BTN_RIGHT")
			Move_SelectDirection(kWnd, iCalc)
			kWnd = MissionTopWnd:GetControl("FRM_SLECT_TIMER")
			Move_SelectDirection(kWnd, iCalc)
			kWnd = MissionTopWnd:GetControl("BTN_USE_ITEM_LEFT")
			Move_SelectDirection(kWnd, iCalc)
			kWnd = MissionTopWnd:GetControl("BTN_USE_ITEM_RIGHT")
			Move_SelectDirection(kWnd, iCalc)
			kWnd = MissionTopWnd:GetControl("BTN_GO")
			Move_SelectDirection(kWnd, iCalc)
			kWnd = MissionTopWnd:GetControl("FRM_TEXT_CORRECT_CHAIN")
			Move_SelectDirection(kWnd, iCalc)
			kWnd = MissionTopWnd:GetControl("FRM_TEXT_TITLE")
			Move_SelectDirection(kWnd, iCalc)
			kWnd = MissionTopWnd:GetControl("FRM_TEXT_EXPLAIN")
			Move_SelectDirection(kWnd, iCalc)
		end
		
		MissionTopWnd:SetCustomDataAsInt(iState)
	end
end

function Move_SelectDirection(kWnd, iCalc)
	if kWnd:IsNil() == true then
		return
	end

	local msLoc = kWnd:GetLocation()	
	local iY = msLoc:GetY()
	iY = iY-iCalc
	msLoc:SetY(iY)
	kWnd:SetLocation(msLoc)
end

function IsClearDiff(CardWnd)
	if true == IsHaveArcadeMode() then
		local ImgBg = CardWnd:GetControl("IMG_BG")
		if false == ImgBg:IsNil() then
			local ImgClear = ImgBg:GetControl("IMG_CLEAR")
			if false == ImgClear:IsNil() then
				if true == ImgClear:IsVisible() then
					return true
				else
					ODS("ImgClear:Visible() false\n", false, 1234)
				end
			else
				ODS("ImgClear:IsNil()\n", false, 1234)
			end
		else
			ODS("ImgBg:IsNil()\n", false, 1234)
		end
	else
		return true
	end
	return false
end

function DisplayHpGaugeBar(kSelf)
	if false==kSelf or kSelf:IsNil() then
		return
	end
	
	local kAniBar = kSelf:GetControl("BAR_HP")
	if kAniBar:IsNil() then
	    return
	end
--[[	
	local kBG = kSelf:GetControl("BG")
	if kBG:IsNil() then
	   return
	end
	
	local iIndex = kAniBar:GetUVIndex()-1
	if iIndex < 1 then
	    kBG:Visible(false)
	else
	    kBG:Visible(true)
		kBG:SetUVIndex(iIndex)
	end
	--]]

	if nil == g_world then
		return
	end

	local MyActor = GetMyActor()
	if MyActor:IsNil() == false then
		local kTargetActor = nil
		if 2 == MyActor:GetAbil(AT_TEAM) and nil ~= g_ObjectGuid2 then
			kTargetActor = g_world:FindActor(g_ObjectGuid2)
		elseif nil ~= g_ObjectGuid1 then
			kTargetActor = g_world:FindActor(g_ObjectGuid1)
		end		

		if nil ~= kTargetActor then
			local iMaxHP = kTargetActor:GetAbil(AT_C_MAX_HP)
			local iNowHP = kTargetActor:GetAbil(AT_HP)
			
			local iHP = kAniBar:GetBarNow()
			if false == kAniBar:IsNil() then
				kAniBar:BarMax( iMaxHP )
				kAniBar:BarNow( iNowHP )
				
				local fHPPer = 0.0
				if 0 ~= iMaxHP then
					fHPPer = math.floor(iNowHP)/iMaxHP
				end
				
				local iValueHP = fHPPer*100
				if iValueHP < 0 then
					iValueHP = 0
				end

				local kMsg = string.format(GetTextW(400999):GetStr(), iValueHP)
				kAniBar:SetStaticText(kMsg)

				local uvIndex = math.floor(fHPPer * 10 +1)
				if uvIndex < 1 then
					uvIndex = 1
				elseif 10 < uvIndex then
					uvIndex = 10
				end
				kAniBar:SetUVIndex(uvIndex)
			end
		end
	end
end

--����( Elga) 3�� ������ �� �����ð� ǥ�ÿ�
function DisplayElgaRoomOfHeart(kSelf)
	if false==kSelf or kSelf:IsNil() then
		return
	end
	
	local kAniBar = kSelf:GetControl("BAR_HP")
	if kAniBar:IsNil() then
	    return
	end
	
	local kTargetGuid = GetReqAddUnitGuid("elga_3")
	if nil==kTargetGuid or kTargetGuid:IsNil() then
		return
	end

	local kTargetPilot = g_pilotMan:FindPilot(kTargetGuid)
	if nil == kTargetPilot then
		return
	end

	local kTarget = kTargetPilot:GetUnit()
	if nil == kTarget then
		return
	end
	
	local kEffect = kTarget:FindEffect( 3542525 );
	if false == kEffect:IsNil() then
		local iMax = kEffect:GetDurationTime()
		local iEndRemainTime = kEffect:GetEndTime()-GetServerElapsedTime32(false);
		local iNow = iMax-iEndRemainTime
		if iNow>iMax then
			iNow = iMax
		end
		kAniBar:BarMax( iMax )
		kAniBar:BarNow( iNow )
	else
		kAniBar:BarMax( 1 )
		kAniBar:BarNow( 1 )
	end
end

--[[
7 : ���潺���
8 : �������潺
9 : ���� �������潺
]]
function OnEnterDefenceMode(iGameLevel)
	if not IsOpenDefanceMode(iGameLevel-1) then
		AddWarnDataTT(400225)
		return
	end
	
	iDiff = iGameLevel
	
	local iMissionNo = GetMissionNo()
	
	if 0 >= iMissionNo then
		return
	end
	
	if Enter_DefenceMission(iMissionNo) == false then
		return
	end
	
	Net_ReqEnterMission( iDiff, 0 ) -- note: for defence mode mutator not used 

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if not g_world:IsHaveWorldAttr( GATTR_FLAG_MISSION ) then
		InitMission()
		CloseMissionEnterUI()
	end
end

function CloseMissionEnterUI()
	CloseUI("FRM_CREATE_DEFENCE")
	CloseUI("FRM_CREATE_SCENARIO_ITEM")
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetShowWorldFocusFilter(false, "../Data/5_Effect/9_Tex/EF_blackBG.tga", 0.5)
end

function OnEnterDefenceMode6()
	OnEnterDefenceMode(7)
end

function CheckEnterDefenceMode7()
	if IsInParty() then
		if IsPartyMaster( g_pilotMan:GetPlayerPilotGuid() ) then
			if false == IsMissionComplatedQuest(7) then
				return
			end
			if false == g_Defence7Enter then
				g_Defence7Enter = true
				Send_PT_C_M_REQ_ENTER_DEFENCE()
			end
		else
			AddWarnDataTT(80027)
		end
	else
		OnEnterDefenceMode7()
	end
end

function InitDefenceMode7DelayTime()
	g_DMode7EnterDelayTime = GetAccumTime()
	g_F7EnterTime = 6
end

function DelayDefenceMode7()
	if 0 ~= g_F7EnterTime then
		if GetAccumTime() - g_DMode7EnterDelayTime >= 1 then
			g_DMode7EnterDelayTime = GetAccumTime()
			g_F7EnterTime = g_F7EnterTime - 1
			local text = WideString(string.format(GetTextW(750026):GetStr(), tonumber(g_F7EnterTime)))
			g_ChatMgrClient:AddLogMessage(text, true)
			if 0 == g_F7EnterTime then
				g_DMode7EnterDelayTime = 0
				if IsInParty() and IsPartyMaster(g_pilotMan:GetPlayerPilotGuid()) then
					g_Defence7Enter = false
					OnEnterDefenceMode(8)
				end
			end
		end
	end
end

function OnEnterDefenceMode7()
	OnEnterDefenceMode(8)
end

function OnEnterDefenceMode8()
	if true then return end

	if IsInParty() then
		OnEnterDefenceMode(9)
	else
		AddWarnDataTT(22002)
	end
end

function LocalCheck_IsOpenDeffencMode8()
	return false
end

--���潺 ����϶� �Ϲ�UI ���°�������Ȯ��
function IsOpenUIMode7()
	if false==GetUIWnd("FRM_MY_INFO_NEXT"):IsNil() then return false end
	if false==GetUIWnd("FRM_MISSION_STAGE_SUCCESS"):IsNil() then return false end
	if false==GetUIWnd("FRM_MISSION_STAGE_FAILED"):IsNil() then return false end

	return true
end

function CallMissionStart()
	if IsStrategyDefenceMode() then
		CallUI("FRM_DEFENCE_MISSION_F7_START")
	end
end

function CallMissionSelectMapUI(index)
	local wnd = CallUI("FRM_SELECT_DIFFICULTY")
	if true == wnd:IsNil() then return end
	
	wnd:SetCustomDataAsInt(index)
	
	local QuestCount = RecommendMissionQuestCount(index)
	local AchievementCount = RecommendMissionAchievementCount(index)
	
	local RecommendWnd = wnd:GetControl("FRM_RECOMMEND_1")
	if RecommendWnd:IsNil() == false then
		local MissionWnd = RecommendWnd:GetControl("IMG_MISSION")
		local AchievementWnd = RecommendWnd:GetControl("IMG_ACHIEVEMENT")
		if MissionWnd:IsNil() == false then
			if QuestCount == 1 then
				MissionWnd:Visible(true)
			else
				MissionWnd:Visible(false)
			end
		end
		if AchievementWnd:IsNil() == false then
			if AchievementCount == 1 then
				AchievementWnd:Visible(true)
			else
				AchievementWnd:Visible(false)
			end
		end
	end
	RecommendWnd = wnd:GetControl("FRM_RECOMMEND_2")
	if RecommendWnd:IsNil() == false then
		local MissionWnd = RecommendWnd:GetControl("IMG_MISSION")
		local AchievementWnd = RecommendWnd:GetControl("IMG_ACHIEVEMENT")
		if MissionWnd:IsNil() == false then
			if QuestCount == 2 then
				MissionWnd:Visible(true)
			else
				MissionWnd:Visible(false)
			end
		end
		if AchievementWnd:IsNil() == false then
			if AchievementCount == 2 then
				AchievementWnd:Visible(true)
			else
				AchievementWnd:Visible(false)
			end
		end
	end
	RecommendWnd = wnd:GetControl("FRM_RECOMMEND_3")
	if RecommendWnd:IsNil() == false then
		local MissionWnd = RecommendWnd:GetControl("IMG_MISSION")
		local AchievementWnd = RecommendWnd:GetControl("IMG_ACHIEVEMENT")
		if MissionWnd:IsNil() == false then
			if QuestCount > 2 then
				MissionWnd:Visible(true)
			else
				MissionWnd:Visible(false)
			end
		end
		if AchievementWnd:IsNil() == false then
			if AchievementCount > 2 then
				AchievementWnd:Visible(true)
			else
				AchievementWnd:Visible(false)
			end
		end
	end
	
	local textwnd = wnd:GetControl("FRM_CONTENT")
	local TradeBtn = wnd:GetControl("BTN_TRADE")
	local bShowExchangeSoulStoneBtn = not IsEventMission()
	if textwnd:IsNil() == false then
		local iText = 0
		if 0 == index then
			iText = 302205
			if TradeBtn:IsNil() == false then
				TradeBtn:Visible(false)
			end
			local TopWnd = GetUIWnd("FRM_CREATE_SCENARIO_ITEM")
			if false == TopWnd:IsNil() then
				local Toploc = TopWnd:GetLocation()
				wnd:SetLocation(Point2(Toploc:GetX() + 210, Toploc:GetY() + 237))
			end
		else
			iText = 302205
			if TradeBtn:IsNil() == false then
				TradeBtn:Visible(true)
			end
			local TopWnd = GetUIWnd("FRM_CREATE_SCENARIO_ITEM")
			if false == TopWnd:IsNil() then
				local Toploc = TopWnd:GetLocation()
				wnd:SetLocation(Point2(Toploc:GetX() + 420, Toploc:GetY() + 237))
			end
		end
		if bShowExchangeSoulStoneBtn == false then
			if TradeBtn:IsNil() == false then
				TradeBtn:Visible(false)
			end
		end
	
		local strContent = GetTextW(iText):GetStr()
		local iRequiredItemNo = GetMissionRequiredItemNo(index)
		if iRequiredItemNo > 0 then
			local strRequiredItem = string.format(GetTT(302212):GetStr(), GetItemName(iRequiredItemNo):GetStr())
			strContent = strRequiredItem.."\n"..strContent
			
			if false == CheckHaveMissionEnterItem(index) then
				AddWarnDataStr(WideString(strRequiredItem), 1, true)
			end
		end		
		
		textwnd:SetStaticText(strContent)
	end
	CreateEnterIndunParty()
end

function CallMissionSelectMap(UISelf)
	if true == UISelf:IsNil() then
		return
	end
	
	local index = UISelf:GetBuildIndex()
	local CardWnd = UISelf:GetParent():GetControl("FRM_CARD"..index)
	if false == CardWnd:IsNil() then	
		local ImgBg = CardWnd:GetControl("IMG_BG")
		if false == ImgBg:IsNil() then	
			if ImgBg:IsGrayScale() then
				AddWarnDataTT(400225)
				return
			end		
		end

		CallMissionSelectMapUI(index)
	end	
end

function SelectMissionMapUIInit(UISelf)
	if false == UISelf:IsNil() then
		for i=0,5 do
			local SelectMapNum = UISelf:GetControl("BTN_MAP_CURSOR"..i)
			if false == SelectMapNum:IsNil() then
				SelectMapNum:CheckState(false)
			end			
		end
	end
end

function SetMissionMapSelectImage(UISelf)
	g_szMissionBGFileName = GetMissionImgPath( GetMissionNo() )
	if false == UISelf:IsNil() then
		for i=0,3 do
			local CardWnd = UISelf:GetControl("FRM_CARD"..i)	
			if false == CardWnd:IsNil() then
				local ImgWnd = CardWnd:GetControl("IMG_BG")
				if ImgWnd:IsNil() == false then							
					local ii = i + 1
					ImgWnd:ChangeImage(g_szMissionBGFileName..ii..".tga")
					ImgWnd:SetInvalidate(true)
				end	
			end
		end	
		
		for j=0,5 do
			local SelectMapNum = UISelf:GetControl("BTN_MAP_CURSOR"..j)
			if false == SelectMapNum:IsNil() then
				local Text = SetMissionSelectMapName(UISelf:GetCustomDataAsInt(), j)
				TextPos = SelectMapNum:GetTextPos()
				TextPos:SetY(20)
				if Text:Length() >= 14 then
					TextPos:SetY(TextPos:GetY() - 6)
				end
				SelectMapNum:SetTextPos(TextPos)
				SelectMapNum:SetStaticText(Text:GetStr())				
			end	
		end
	end
	SelectMissionMapUIInit(UISelf)
end

function SeleceMissionMapOver(UISelf, Index)
	if true == UISelf:IsNil() then
		return
	end	
	
	local wnd = UISelf:GetControl("BTN_MAP_CURSOR"..Index)
	if false == wnd:IsNil() then
		wnd:SetFontColor(0xFFfff568)
	end
end

function SeleceMissionMapOut(UISelf, Index)
	if true == UISelf:IsNil() then
		return
	end	
	local disableWnd = UISelf:GetControl("BTN_MAP_CURSOR"..Index)
	if false == disableWnd:IsNil() then
		disableWnd:SetFontColor(0xFF00ff00)
	end		
end

function SeleceMissionMapCheckByHotKey(UISelf, Index)
	if true == UISelf:IsNil() then
		return
	end	
	
	local CheckSelectMapCount = SelectMissionMapCount(UISelf, Index)
	if 3 > CheckSelectMapCount then
		local disableWnd = UISelf:GetControl("BTN_MAP_CURSOR"..Index)
		if false == disableWnd:IsNil() then		
			if true == disableWnd:GetCheckState() then
				disableWnd:CheckState(false)
				return
			else
				disableWnd:CheckState(true)
			end
		end
	else
		CommonMsgBox(GetTT(401188):GetStr(), true)
		-- ���̻� ������ �� ����.
	end	
	
	
end

function SeleceMissionMapCheck(UISelf, Index)
	if true == UISelf:IsNil() then
		return
	end	
	
	local CheckSelectMapCount = SelectMissionMapCount(UISelf, Index)
	if 3 > CheckSelectMapCount then
		return
	end	
	
	local disableWnd = UISelf:GetControl("BTN_MAP_CURSOR"..Index)
	if false == disableWnd:IsNil() then		
		if true == disableWnd:GetCheckState() then
			disableWnd:CheckState(false)
			return
		else
			disableWnd:CheckState(true)
		end
	end
	
	CommonMsgBox(GetTT(401188):GetStr(), true)
	-- ���̻� ������ �� ����.
end

function SelectMissionMapCount(UISelf, Index)
	if true == UISelf:IsNil() then
		return 0
	end
	
	local CheckSelectMapCount = 0	
	for i=0,5 do
		local SelectMapNum = UISelf:GetControl("BTN_MAP_CURSOR"..i)
		if false == SelectMapNum:IsNil() then
			if true == SelectMapNum:GetCheckState() and (Index ~= i) then
				CheckSelectMapCount = CheckSelectMapCount + 1
			end
		end
	end	
	
	return CheckSelectMapCount
end

function MissionSelectedKeyDownJoin(UISelf, iSelectMapCount)
	local iDiff = UISelf:GetCustomDataAsInt() + 1
	Net_ReqEnterMission( iDiff, 1, iSelectMapCount, 0, 0, 0, GetMutatotSelectedCount(), GetMutatorPacket() ) -- todo: mutator
	SetMissionCountMap(iSelectMapCount + 1)
	UISelf:Close()
	InitMission()
end

function MissionSelectedKeyDownSelectJoin(UISelf)
	if true == UISelf:IsNil() then
		return
	end
	
	local Value = {}
	local index = 0
	for i=0,5 do
		local SelectMapNum = UISelf:GetControl("BTN_MAP_CURSOR"..i)
		if false == SelectMapNum:IsNil() then
			if true == SelectMapNum:GetCheckState() then
				Value[index] = i + 1
				index = index + 1
			end
		end
	end	
	
	local iDiff = UISelf:GetCustomDataAsInt() + 1
	Net_ReqEnterMission( iDiff, 2, Value[0], Value[1], Value[2], GetMutatotSelectedCount(), GetMutatorPacket() ) -- todo : mutator
	SetMissionCountMap(index + 1)
	UISelf:Close()
	InitMission()
end

function Net_PT_M_C_NFY_MISSION_BONUS_TIME_LIMIT(packet)
	g_iMissionBonusTotalTimeLimit = packet:PopInt()
	g_fMissionBonusStartTimeLimit = GetCurrentTimeInSec()
	
	ActivateUI("FRM_BONUS_TIMELIMIT")
	ActivateUI("FRM_BONUS_MISSION_START")
end

function DisplayMissionBonusTimeLimit(wnd)
	if wnd:IsNil() == true then
		return
	end

	local fNow = GetCurrentTimeInSec()*1000
	
	local iMissionTime;
	
	iMissionTime = g_iMissionBonusTotalTimeLimit - (g_iMissionBonusTimeLimit + fNow - g_fMissionBonusStartTimeLimit*1000)
	if iMissionTime < 0 then
		iMissionTime = 0;
	end
	

	local iDelta = math.floor(iMissionTime*0.1)
	local iSec100 = iDelta%10 + 1
	local iSec1000 = math.floor((iDelta%100)/10) + 1	
	iDelta = math.floor(iMissionTime*0.001)
	local iSec1 = iDelta%10 + 1
	local iSec10 = math.floor((iDelta%60)/10) + 1
	local iMin1 = math.floor(iDelta/60)%10 + 1
	local iMin10 = math.floor(iDelta/600)%10 + 1	
	
	local wnd100sec = wnd:GetControl("FRM_SEC100")
	if wnd100sec:IsNil() == false then
		wnd100sec:SetUVIndex(iSec100)
	end

	local wnd1000sec = wnd:GetControl("FRM_SEC1000")
	if wnd1000sec:IsNil() == false then
		wnd1000sec:SetUVIndex(iSec1000)
	end
	
	local wnd1sec = wnd:GetControl("FRM_SEC1")
	if wnd1sec:IsNil() == false then
		wnd1sec:SetUVIndex(iSec1)
	end

	local wnd10sec = wnd:GetControl("FRM_SEC10")
	if wnd10sec:IsNil() == false then
		wnd10sec:SetUVIndex(iSec10)
	end

	local wnd1min = wnd:GetControl("FRM_MIN1")
	if wnd1min:IsNil() == false then
		wnd1min:SetUVIndex(iMin1)
	end

	local wnd10min = wnd:GetControl("FRM_MIN10")
	if wnd10min:IsNil() == false then
		wnd10min:SetUVIndex(iMin10)
	end

	local kPathDefault = "../Data/6_ui/mission/msNumRs.tga"
	local kPathsDefault = "../Data/6_ui/mission/msNumTmRs.tga"	
	local kPath = "../Data/6_ui/mission/msNumRs.tga"
	local kPaths = "../Data/6_ui/mission/msNumTmRs.tga"
	
	wnd100sec:ChangeImage(kPathsDefault)
	wnd100sec:SetInvalidate(true)
	wnd1000sec:ChangeImage(kPathsDefault)
	wnd1000sec:SetInvalidate(true)
	wnd10sec:ChangeImage(kPathDefault)
	wnd10sec:SetInvalidate(true)
	wnd1sec:ChangeImage(kPathDefault)
	wnd1sec:SetInvalidate(true)
	wnd1min:ChangeImage(kPathDefault)
	wnd1min:SetInvalidate(true)
	wnd10min:ChangeImage(kPathDefault)
	wnd10min:SetInvalidate(true)		
end

function ALiveMonsterRegenCountUI(bShow)
	if nil == bShow then
		bShow = false
	end
	
	local iCount = 0
	local kUIName = "FRM_REST_MON_REGEN_NUM"
	
	if bShow and nil~=kUIName then
		local wnd = ActivateUI(kUIName)
		if wnd:IsNil() == false then
			wnd:SetStaticText(tostring(iCount))
			wnd:SetCustomDataAsInt(iCount)
			
			local packet = NewPacket(13525) --PT_C_M_REQ_GENMONSTER_GROUP_NUM
			Net_Send(packet)
			DeletePacket(packet)			
		end
	else
		CloseUI(kUIName)
	end
end

function ALiveMonsterRegenCountUpdate(iCount)
	local wnd = GetUIWnd("FRM_REST_MON_REGEN_NUM")
	if wnd:IsNil() == false then
		if 0 <= iCount then
			wnd:SetStaticText(tostring(iCount))
			wnd:SetCustomDataAsInt(iCount)
		end
	end
end

function Net_PT_M_C_ANS_GENMONSTER_GROUP_NUM(packet)
    local iNum = packet:PopInt()
	ALiveMonsterRegenCountUpdate(iNum)
end

function CallPointCopyMachine(StageNo)
	local eLocale = GetLocale()
	--if -- ����Ʈ �����Ⱑ ��� �Ǵ� ������ �ּ�ó��
		--eLocale == LOCALE.NC_KOREA
		--or eLocale == LOCALE.NC_TAIWAN
		--or eLocale == LOCALE.NC_CHINA
		--or eLocale == LOCALE.NC_SINGAPORE
		--or eLocale == LOCALE.NC_THAILAND
		--or eLocale == LOCALE.NC_VIETNAM
		--or eLocale == LOCALE.NC_INDONESIA
		--or eLocale == LOCALE.NC_PHILIPPINES
		--or eLocale == LOCALE.NC_JAPAN
		--or eLocale == LOCALE.NC_FRANCE
		--or eLocale == LOCALE.NC_GERMANY
		--or eLocale == LOCALE.NC_USA
		--or eLocale == LOCALE.NC_RUSSIA
		--or eLocale == LOCALE.NC_EU
	--then return end

	if 10 == StageNo then -- 11�������� �� ������ ���� ����
		--GetEventScriptSystem():ActivateEvent(90001)
		ActivateUI("FRM_POINT_COPY_MACHINE")
		SetMonsterKillButtonText()
	end
end

function SetUsingCopyMachine(Value)
	g_UsingCopyMachine = Value
end

function RefreshFatigue(FatigueForm)
	if FatigueForm:IsNil() then return end
	if false == IsChangeFatigue() then return end

	g_Defence7ModeExpStr = CallDefenceFatigueWindow(FatigueForm)

	local FatigueImg = FatigueForm:GetControl("IMG_BAR")
	if FatigueImg:IsNil() then return end

	local Fatigue = GetDefenceFatigue()
	
	g_OldFatigue = Fatigue
	
	local MaxFatigue = GetMaxDefenceFatigue()
	local Rate = Fatigue / MaxFatigue
	if Fatigue >= 1200 then
		FatigueImg:SetUVIndex(1)
	elseif Fatigue >= 800 then
		FatigueImg:SetUVIndex(2)
	elseif Fatigue >= 400 then
		FatigueImg:SetUVIndex(3)
	else
		FatigueImg:SetUVIndex(4)
	end
	FatigueImg:SetImgSize( Point2(Rate*FatigueImg:GetSize():GetX(), 76) )
end

function IsChangeFatigue()
	local Fatigue = GetDefenceFatigue()
	if g_OldFatigue ~= Fatigue then
		return true
	else
		return false
	end
end

g_NewPoint = {}
g_NewPoint["FRM_CURRENT_POINT"] = 0
g_NewPoint["FRM_NEXT_POINT"] = 0

g_F7AnimationIndex = {}
g_F7AnimationIndex[0] = {}
g_F7AnimationIndex[1] = {}
g_F7AnimationIndex[2] = {}
g_F7AnimationIndex[3] = {}
g_F7AnimationIndex[4] = {}
g_F7AnimationIndex[5] = {}
g_F7AnimationIndex[0]["FRM_CURRENT_POINT"] = 0
g_F7AnimationIndex[1]["FRM_CURRENT_POINT"] = 1
g_F7AnimationIndex[2]["FRM_CURRENT_POINT"] = 2
g_F7AnimationIndex[3]["FRM_CURRENT_POINT"] = 3
g_F7AnimationIndex[4]["FRM_CURRENT_POINT"] = 4
g_F7AnimationIndex[5]["FRM_CURRENT_POINT"] = 5
g_F7AnimationIndex[0]["FRM_NEXT_POINT"] = 0
g_F7AnimationIndex[1]["FRM_NEXT_POINT"] = 1
g_F7AnimationIndex[2]["FRM_NEXT_POINT"] = 2
g_F7AnimationIndex[3]["FRM_NEXT_POINT"] = 3
g_F7AnimationIndex[4]["FRM_NEXT_POINT"] = 4
g_F7AnimationIndex[5]["FRM_NEXT_POINT"] = 5

g_StartCPAnimation = {}
g_StartCPAnimation[0] = {}
g_StartCPAnimation[1] = {}
g_StartCPAnimation[2] = {}
g_StartCPAnimation[3] = {}
g_StartCPAnimation[4] = {}
g_StartCPAnimation[5] = {}
g_StartCPAnimation[0]["FRM_CURRENT_POINT"] = false
g_StartCPAnimation[1]["FRM_CURRENT_POINT"] = false
g_StartCPAnimation[2]["FRM_CURRENT_POINT"] = false
g_StartCPAnimation[3]["FRM_CURRENT_POINT"] = false
g_StartCPAnimation[4]["FRM_CURRENT_POINT"] = false
g_StartCPAnimation[5]["FRM_CURRENT_POINT"] = false
g_StartCPAnimation[0]["FRM_NEXT_POINT"] = false
g_StartCPAnimation[1]["FRM_NEXT_POINT"] = false
g_StartCPAnimation[2]["FRM_NEXT_POINT"] = false
g_StartCPAnimation[3]["FRM_NEXT_POINT"] = false
g_StartCPAnimation[4]["FRM_NEXT_POINT"] = false
g_StartCPAnimation[5]["FRM_NEXT_POINT"] = false

function InitCopyPointState(WndName)
	g_NewPoint[WndName] = 0
	for i = 0, 5 do
		g_F7AnimationIndex[i][WndName] = i
		g_StartCPAnimation[i][WndName] = false
	end
end

function UpdateCopyPointNumber(Self)
	if nil == Self or Self:IsNil() then return end
	
	local WndName = Self:GetID():GetStr()
	
	InitCopyPointState(WndName)

	if "FRM_CURRENT_POINT" == WndName
	or "FRM_NEXT_POINT" == WndName then
		g_NewPoint[WndName] = Self:GetCustomDataAsInt()
		
		for i = 0, 5 do
			g_StartCPAnimation[i][WndName] = true
		end
	end
end

function StopAnimateCopyPoint(Self, digit)
	if nil == Self or Self:IsNil() then return end
	
	local WndName = Self:GetID():GetStr()
	
	if "FRM_CURRENT_POINT" == WndName
	or "FRM_NEXT_POINT" == WndName then
		if 0 == digit
		or false == g_StartCPAnimation[digit - 1][WndName] then
			local Point = Self:GetControl("FRM_POINT"..digit)
			if nil == Point or Point:IsNil() then return end
			
			local NewPoint = g_NewPoint[WndName]
			if 0 < NewPoint then
				for i = 1, digit do
					NewPoint = NewPoint / 10
				end
				NewPoint = NewPoint % 10
			end
			Point:SetUVIndex(NewPoint + 1)
			g_StartCPAnimation[digit][WndName] = false
		end
	end
end

function AnimateCopyPointAll(Self, fInterTime)
	if nil == Self or Self:IsNil() then return end

	local WndName = Self:GetID():GetStr()

	for digit = 0, 5 do
		if g_StartCPAnimation[digit][WndName] then
			AnimateCopyPoint(Self, fInterTime, digit)
			if digit == 5 then
				PlaySoundByID("defence-getpointcount")
			end
		end
	end
end

function AnimateCopyPoint(Self, fInterTime, digit)
	if nil == Self or Self:IsNil() then return end
	if(0 == fInterTime) then return end

	local WndName = Self:GetID():GetStr()
	
	local Point = Self:GetControl("FRM_POINT"..digit)
	if nil == Point or Point:IsNil() then return end
	
	if 9 < g_F7AnimationIndex[digit][WndName] then
		g_F7AnimationIndex[digit][WndName] = 0
		StopAnimateCopyPoint(Self, digit)
		return
	end

	local TimeCnt = Point:GetCustomDataAsFloat()
	TimeCnt= TimeCnt+GetFrameTime()
	if(fInterTime < TimeCnt) then 
		TimeCnt = 0
		g_F7AnimationIndex[digit][WndName] = g_F7AnimationIndex[digit][WndName] + 1
		Point:SetUVIndex(g_F7AnimationIndex[digit][WndName])
	end
	Point:SetCustomDataAsFloat(TimeCnt)
end

g_CopyMaxAniStart = false
g_CopyMaxVisible = true

function UpdateCopyMax(Self, fInterTime)
	if Self:IsNil() then return end
	
	if g_CopyMaxAniStart then
		local TimeCnt = Self:GetCustomDataAsFloat()
		TimeCnt= TimeCnt+GetFrameTime()
		if(fInterTime < TimeCnt) then 
			TimeCnt = 0
			if g_CopyMaxVisible then			 -- MAX�� ����������
				AnimateCopyPointMax(Self, false) -- ����
				g_CopyMaxVisible = false
			else
				AnimateCopyPointMax(Self, true)  -- MAX�� ���������� Ŵ
				g_CopyMaxVisible = true
			end
		end
		Self:SetCustomDataAsFloat(TimeCnt)
	end
end

function StartAnimateCopyMax()
	local Wnd = GetUIWnd("FRM_POINT_COPY_MACHINE")
	if Wnd:IsNil() then return end
	local Sub = Wnd:GetControl("FRM_CURRENT_POINT")
	if Sub:IsNil() then return end
	local MaxWnd = Sub :GetControl("FRM_MAX")
	if MaxWnd:IsNil() then return end
	
	local Count = GetCopyPointChargeCount()
	local MaxCount = GetMaxCopyPointChargeCount()
	if MaxCount == Count then
		AnimateCopyPointMax(MaxWnd, true)
		g_CopyMaxAniStart = true
	else
		AnimateCopyPointMax(MaxWnd, false)
		g_CopyMaxAniStart = false
	end
end

function AnimateCopyPointMax(Self, bOn)
	if Self:IsNil() then return end
	if bOn then
		Self:SetStaticTextW(GetTT(750041))
	else
		Self:SetStaticText("")
	end
end

function CancleDefence7Enter()
	if 0 ~= g_F7EnterTime
	and IsPartyMaster( g_pilotMan:GetPlayerPilotGuid() ) then
		Send_PT_C_M_REQ_CANCLE_DEFENCE()
	end
end

function InitF7EnterTime()
	g_F7EnterTime = 0
end

g_Defence7_fDeadCount = 60
function SetDefence7_DeadCount(iCnt)
	g_Defence7_fDeadCount = iCnt
end