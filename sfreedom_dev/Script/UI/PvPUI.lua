
PvPModeTable = {}
PvPModeTable[PVP_MODE_TEAM]			= 400352
PvPModeTable[PVP_MODE_PERSONAL]		= 400351

PvPTypeTable = {}
PvPTypeTable[PVP_TYPE_ALL]			= 400379
PvPTypeTable[PVP_TYPE_DM]			= 400361
PvPTypeTable[PVP_TYPE_KTH]			= 400363
PvPTypeTable[PVP_TYPE_ANNIHILATION] = 400362
PvPTypeTable[PVP_TYPE_DESTROY]		= 400364
PvPTypeTable[PVP_TYPE_WINNERS]		= 400365
PvPTypeTable[PVP_TYPE_WINNERS_TEAM] = 400366
PvPTypeTable[PVP_TYPE_ACE]          = 400367
PvPTypeTable[PVP_TYPE_LOVE]			= 400368

PVP_DEFAULT_MAXPLAYER 				= 8
PVP_DEFAULT_TIMELIMIT				= 3
PVP_DEFAULT_POINTLIMIT				= 2000
PVP_DEFAULT_ROUNDCOUNT				= 3
PVP_LOVE_DEFAULT_ROUNDCOUNT			= 5
PVP_DEFAULT_MAXPLAYER_RANKMODE		= 8

PvPAttrTable = {}
PvPAttrTable["GROUNDNAME"] = ""
PvPAttrTable["GROUNDNO"] = 0
PvPAttrTable["TYPE"] = 0
PvPAttrTable["MODE"] = 0
PvPAttrTable["LIMITLEVEL"] = 0
PvPAttrTable["MAXROUND"] = 0
PvPAttrTable["MAXUSER"] = 0
PvPAttrTable["GAMETIEM"] = 0
PvPAttrTable["GAMEPOINT"] = 0

g_MinimapAlramAniCount = {} -- 미니맵 알람 애니메이션용
for i = 0, 19 do
	g_MinimapAlramAniCount[i] = 0
end

-----------------------------------------------------------------------------------

function GetPvPAttribute( lwStr )
	return PvPAttrTable[lwStr]
end

function OnClickPvP()
	if false == GetUIWnd("FRM_USER_STOPPER"):IsNil() then
		return
	end

	if IsJoinPvP() then
		local actor = g_pilotMan:GetPlayerActor()
		if false == actor:IsNil() then
			if actor:IsRidingPet() then
				g_ChatMgrClient:Notice_Show(GetTextW(451001), 1)
				return
			end
			if actor:GetAbil(AT_DUEL) > 0 then
				g_ChatMgrClient:Notice_Show(GetTextW(201209), 1)
				return
			end
			
			-- 안전거품  막기
			if g_pilotMan:IsLockMyInput() and actor:IsMyActor() then
				g_ChatMgrClient:Notice_Show( GetTextW(400858), 1 )
				return
			end

			local kPacket = NewPacket(30750)--PT_C_M_REQ_PVP_SELECTOR
			Net_Send(kPacket)
			DeletePacket(kPacket)
		end
		local SystemMenu = GetUIWnd("SFRM_SYSTEMMENU")
		if false == SystemMenu:IsNil() then
			local Btn_Pvp = SystemMenu:GetControl("BTN_PVP")
			if false == Btn_Pvp:IsNil() then
				Btn_Pvp:SetTwinkle(false)
			end
		end
	end
end

function OnClickPvP_ExitRoom()
	local kPacket = NewPacket(30711)--PT_C_PM_REQ_EXIT_ROOM
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function GetPvPTypeTextTable( ePvPType )
	return PvPTypeTable[ePvPType]
end

function GetPvPModeTextTable( ePvPMode )
	return PvPModeTable[ePvPMode]
end

-- Init
function OnInit_CreateRoom(kSelf)
	local iGndNo = GetRandomPvPGround( false, PVP_TYPE_DM ) -- 맵이름
	OnSelectPvP_Ground( iGndNo, kSelf )
	OnSelectPvP_Mode( PVP_MODE_TEAM, kSelf )
	OnSelectPvP_Type( PVP_TYPE_DM, kSelf)
	
	local InitLevel = false
	if false == IsExerciseType() then
	-- Ranking Mode
		local iRoomLevelMax = GetRankingModeLevel()
		local iRoomLevelMin = GetRankingModeLevelMin()
		if 0 < iRoomLevelMax then
			OnSelectPvP_RankingModeLevel( iRoomLevelMin, iRoomLevelMax, kSelf )
			InitLevel = true
		end
	end
	
	if false == InitLevel then
		OnSelectPvP_Level( 3, kSelf )
	end
	
	if false == IsExerciseType() then
		OnSelectPvP_MaxPlayer( PVP_DEFAULT_MAXPLAYER_RANKMODE, kSelf )
	else
		OnSelectPvP_MaxPlayer( PVP_DEFAULT_MAXPLAYER, kSelf )
	end
	
	OnSelectPvP_RoundCount( PVP_DEFAULT_ROUNDCOUNT, kSelf )
	
	local kOptionWnd = kSelf:GetControl("SFRM_OPTION")
	if false == kOptionWnd:IsNil() then
		local kOption = kOptionWnd:GetControl("CBTN_HANDYCAP")--핸디캡
		if false == kOption:IsNil() then
			kOption:CheckState(false)
		end
		
		kOption = kOptionWnd:GetControl("CBTN_USE_ITEM")--아이템 사용
		if false == kOption:IsNil() then
			kOption:CheckState(true)
		end
		
		kOption = kOptionWnd:GetControl("CBTN_DISABLE_DASHJUMP")-- 대쉬 점프 금지
		if false == kOption:IsNil() then
			kOption:CheckState(false)
		end
		
		kOption = kOptionWnd:GetControl("CBTN_ALLOW_OBSERVER")-- 관객 허용
		if false == kOption:IsNil() then
			kOption:CheckState(true)
		end
	end
end

function GetCreateWnd(bInLobby)
	if bInLobby == true then
		local kWnd = GetUIWnd("FRM_PVP_CREATEROOM_BODY")
		if false == kWnd:IsNil() then
			return kWnd:GetControl("FRM_PVP_CREATEROOM")
		end
	else
		return GetUIWnd("FRM_PVP_ROOM_ATTR")
	end
	return UIWnd()
end

function OnSelectPvP_Ground(kVal, kWnd)
	if kWnd == nil then
		kWnd = GetCreateWnd(true)
	end
	
	if false == kWnd:IsNil() then
		local kControl = kWnd:GetControl("SFRM_TAB_MAPNAME")
		if false == kControl:IsNil() then
			PvPAttrTable["GROUNDNO"] = kVal			-- 맵번호
			PvPAttrTable["GROUNDNAME"] = GetPvPGroundName(kVal)	-- 맵이름
			kControl:SetStaticTextW( PvPAttrTable["GROUNDNAME"] )
			kControl:SetCustomDataAsInt(kVal)
			
			local kImgControl = kWnd:GetControl("IMG_MAP_PREVIEW")
			if false == kImgControl:IsNil() then
				local kImgPath = GetPreviewImgPath(kVal)
				
				if false == IsExerciseType() then
					kImgPath = "../Data/6_UI/pvp/pvMapBg.tga"
					kImgControl:ChangeImage(kImgPath)
				else
					kImgControl:ChangeImage(kImgPath:GetStr())
				end			
			end
--[[			
			local kTypeControl = kWnd:GetControl("SFRM_TAB_GAMETYPE")
			if false == kTypeControl:IsNil() then
			
				local kType = PVP_TYPE_DM
				kType = kTypeControl:GetCustomDataAsInt()
				
				if 0 == kType then
					OnSelectPvP_Type(GetPvPType( kVal, kType, true),kWnd)
				else
					OnSelectPvP_Type(GetPvPType( kVal, kType, false),kWnd)
				end
			end
--]]
			return true
		end
	end
	return false
end

function OnSelectPvP_Type(kVal, kWnd)
	if kWnd == nil then
		kWnd = GetCreateWnd(true)
	end

	if false == kWnd:IsNil() then
		local kControl = kWnd:GetControl("SFRM_TAB_GAMETYPE")
		if false == kControl:IsNil() then
			PvPAttrTable["TYPE"] = kVal --게임종류
			local kTypeName = GetTextW(GetPvPTypeTextTable(kVal)):GetStr()	
			kControl:SetStaticText(kTypeName)
			kControl:SetCustomDataAsInt(kVal)
			
			if PVP_TYPE_KTH == kVal then
				OnSelectPvP_Limit( 0, PVP_DEFAULT_POINTLIMIT, kWnd )
			elseif PVP_TYPE_DM == kVal
				or PVP_TYPE_ANNIHILATION == kVal then
				OnSelectPvP_Limit( PVP_DEFAULT_TIMELIMIT, 0, kWnd )
			elseif PVP_TYPE_LOVE == kVal then
				OnSelectPvP_Limit( 0, 0, kWnd )
			else
				OnSelectPvP_Limit( PVP_DEFAULT_TIMELIMIT, PVP_DEFAULT_POINTLIMIT, kWnd )
			end
			
			local iMode = PvPAttrTable["MODE"]
			if PVP_MODE_TEAM ~= iMode then
				OnSelectPvP_Mode(iMode, kWnd)
			end
			
			local bVisibleMode = false
			local bVisibleRound = true
			
			if PVP_TYPE_DM == kVal then
				bVisibleMode = true
				bVisibleRound = false
			end
			
			if PVP_TYPE_KTH == kVal then
				bVisibleRound = false
				OnSelectPvP_RoundCount(1, kWnd)
			elseif PVP_TYPE_DM == kVal
				or PVP_TYPE_ANNIHILATION == kVal then
				OnSelectPvP_RoundCount(PVP_DEFAULT_ROUNDCOUNT, kWnd)
			elseif PVP_TYPE_LOVE == kVal then
				OnSelectPvP_RoundCount(PVP_LOVE_DEFAULT_ROUNDCOUNT, kWnd)
			end
			
			local kModeBtn = kWnd:GetControl("CBTN_GAMEMODE")
			if false == kModeBtn:IsNil() then
				if true == IsExerciseType() then
					kModeBtn:Visible(bVisibleMode)
				end				
			end
			
			local kRoundBtn = kWnd:GetControl("CBTN_ROUNDCOUNT")
			if false == kRoundBtn:IsNil() then
				kRoundBtn:Visible(bVisibleRound)
			end
			return true
		end
	end
	return false
end

function OnSelectPvP_Mode(kVal, kWnd)
	if kWnd == nil then
		kWnd = GetCreateWnd(true)
	end
	
	if false == kWnd:IsNil() then
		local kControl = kWnd:GetControl("SFRM_TAB_GAMEMODE")
		if false == kControl:IsNil() then
			if PVP_MODE_TEAM == kVal then
			elseif PVP_MODE_PERSONAL == kVal then
				local iType = PvPAttrTable["TYPE"]
				if PVP_TYPE_DM ~= iType then
					kVal = PVP_MODE_TEAM
				end
			else
				kVal = PVP_MODE_TEAM
			end
			local kModeName = GetTextW(GetPvPModeTextTable(kVal)):GetStr()
			kControl:SetStaticText(kModeName)
			kControl:SetCustomDataAsInt(kVal)
			PvPAttrTable["MODE"] = kVal
			return true
		end
	end
	return false
end

function OnSelectPvP_Limit(kTime, kPoint, kWnd)
	if kWnd == nil then
		kWnd = GetCreateWnd(true)
	end
	
	if false == kWnd:IsNil() then
		local TimeLimitBtn = kWnd:GetControl("CBTN_TIMELIMIT")
		if false == TimeLimitBtn:IsNil() then
			if PVP_TYPE_LOVE == PvPAttrTable["TYPE"] then
				TimeLimitBtn:Visible(false)
			else
				TimeLimitBtn:Visible(true)
			end
		else
			return false
		end
		local kControl = kWnd:GetControl("SFRM_TAB_TIMELIMIT")
		if false == kControl:IsNil() then
			if PVP_TYPE_KTH == PvPAttrTable["TYPE"] then
				kControl:SetStaticText(kPoint)
				kControl:SetCustomDataAsInt(kPoint)
				PvPAttrTable["GAMEPOINT"] = kPoint
			elseif PVP_TYPE_DM == PvPAttrTable["TYPE"]
				or PVP_TYPE_ANNIHILATION == PvPAttrTable["TYPE"] then
				kControl:SetStaticText(kTime)
				kControl:SetCustomDataAsInt(kTime)
				PvPAttrTable["GAMETIME"] = kTime
			elseif PVP_TYPE_LOVE == PvPAttrTable["TYPE"] then
				kControl:SetStaticText( GetTextW(3331):GetStr() )
				PvPAttrTable["GAMETIME"] = kTime
			else
				kControl:SetStaticText(kTime.." / "..kPoint)
				kControl:SetCustomDataAsInt(kVal)
				PvPAttrTable["GAMETIME"] = kTime
				PvPAttrTable["GAMEPOINT"] = kPoint
			end
			return true
		end
	end
	return false
end

function OnSelectPvP_Level(kVal, kWnd)
	if kWnd == nil then
		kWnd = GetCreateWnd(true)
	end

	if false == kWnd:IsNil() then
		local kControl = kWnd:GetControl("SFRM_TAB_LEVELLIMIT")
		if false == kControl:IsNil() then
			local MyLevel = GetMyActorValue(AT_LEVEL)
			local iLevelMin = MyLevel - kVal
			local iLevelMax = MyLevel + kVal
			if 0 == kVal then
				kControl:SetStaticText(GetTextW(3331):GetStr())
			else
				kControl:SetStaticText(iLevelMin .. " ~ " .. iLevelMax)
			end
			kControl:SetCustomDataAsInt(kVal)
			PvPAttrTable["LIMITLEVEL"] = kVal
			return true
		end
	end
	return false
end

function OnSelectPvP_RankingModeLevel(iLevelMin, iLevelMax, kWnd)
	if kWnd == nil then
		kWnd = GetCreateWnd(true)
	end
	
	if false == kWnd:IsNil() then
		local kControl = kWnd:GetControl("SFRM_TAB_LEVELLIMIT")
		if false == kControl:IsNil() then
			kControl:SetStaticText(iLevelMin .. " ~ " .. iLevelMax)
			kControl:SetCustomDataAsInt(iLevelMax)
			return true
		end
	end
	return false
end

function OnSelectPvP_MaxPlayer(kVal, kWnd)
	if kWnd == nil then
		kWnd = GetCreateWnd(true)
	end
	
	if false == kWnd:IsNil() then
		local kControl = kWnd:GetControl("SFRM_TAB_MAXPLAYER")
		if false == kControl:IsNil() then
			kControl:SetStaticText(kVal)
			kControl:SetCustomDataAsInt(kVal)
			PvPAttrTable["MAXUSER"] = kVal
			return true
		end
	end
	return false
end

function OnSelectPvP_RoundCount(kVal, kWnd)
	if kWnd == nil then
		kWnd = GetCreateWnd(true)
	end
	
	if false == kWnd:IsNil() then
		local kControl = kWnd:GetControl("SFRM_TAB_ROUNDCOUNT")
		if false == kControl:IsNil() then
			local iType = PvPAttrTable["TYPE"]
			if PVP_TYPE_DM == iType
			or PVP_TYPE_KTH == iType then
				kVal = 1 --사투전, 점령전은 라운드 1밖에 지원하지 않는다.
			end
			kControl:SetStaticText(kVal)
			kControl:SetCustomDataAsInt(kVal)
			PvPAttrTable["MAXROUND"] = kVal
			return true
		end
	end
	return false
end

function CallPvPCountDown( iRemainTimeSec )
	local kUI = ActivateUI("FRM_PVP_COUNTDOWN")
	if false == kUI:IsNil() then
		local kImg = kUI:GetControl("IMG_COUNT_DOWN")
		if false == kImg:IsNil() then
			if iRemainTimeSec <= 0 then
				ODS("CallPvPCountDown : Few RemainTimeSec<" .. iRemainTimeSec .. ">\n")
				iRemainTimeSec = 1
			elseif iRemainTimeSec > 6 then
				ODS("CallPvPCountDown : Over RemainTimeSec<" .. iRemainTimeSec .. ">\n")
				iRemainTimeSec = 6
			elseif 4 > iRemainTimeSec then
				PlaySoundByID( "PVP_Count_0" .. iRemainTimeSec ) 
			end
			kImg:SetUVIndex( iRemainTimeSec )
		end		
	end
end
 
function UpdatePvPCountDown(kSelf)
	if false == kSelf:IsNil() then
		local iNowIndex = kSelf:GetUVIndex()
		if 1 < iNowIndex then
			local iRemainTimeSec = iNowIndex - 1
			kSelf:SetUVIndex( iRemainTimeSec )
			if 4 > iRemainTimeSec then
				PlaySoundByID( "PVP_Count_0" .. iRemainTimeSec )
			end
		else
			kSelf:CloseParent()
			if false==IsUIWnd("FRM_PVP_START") then
				CallUI("FRM_PVP_START")
			end
		end
	else
		ODS("UpdatePvPCountDown UI Is Nil")
	end
end

g_PvP_BigNum = 3
function CallPvPCountDown_2( iRemainTimeSec )
	if iRemainTimeSec <= 0 then
		iRemainTimeSec = 1
	end
				
	if iRemainTimeSec > g_PvP_BigNum then
		local kUI = ActivateUI("FRM_PVP_COUNTDOWN_SMALL")
		if false == kUI:IsNil() then
			kUI:SetCustomDataAsInt(iRemainTimeSec)
			UpdatePvPCountDown_small(kUI)
		end
	else
		CloseUI("FRM_PVP_COUNTDOWN_SMALL")
		CallPvPCountDown( iRemainTimeSec )
	end
end

function UpdatePvPCountDown_small(kSelf)
	local iSec = kSelf:GetCustomDataAsInt()
	kSelf:SetCustomDataAsInt(iSec-1)
	if iSec <= g_PvP_BigNum then
		CallPvPCountDown_2(iSec)
		return
	end
	
	local iTenNum = math.floor(iSec / 10)
	local iOneNum = math.floor(iSec % 10) + 1
	
	local kTen = kSelf:GetControl("FRM_TEN")
	if iTenNum > 0 then
		kTen:Visible(true)
		kTen:SetUVIndex(iTenNum + 1)
	else
		kTen:Visible(false)
	end
	
	local kOne = kSelf:GetControl("FRM_ONE")
	kOne:SetUVIndex(iOneNum)
end

function OnClickPvP_AskOutBoxBtn(kBtn)
	if false == kBtn:IsNil() then
		local kPacket = kBtn:GetCustomDataAsPacket()
		if false == kPacket:IsNil() then
			if false == kPacket:Empty() then
				Net_Send(kPacket)
				DeletePacket(kPacket)
				return true
			end
		end
	end
	return false
end

function OnInitPvP_Community(kSelf)
    if kSelf:IsNil() then return end
    
	local iNo = 1
	local kFrm
	while iNo < 3 do
	    kFrm = kSelf:GetControl("CBTN_" .. iNo )
   		if false == kFrm:IsNil() then
	    	kFrm:CheckState(false)
		end
	    kFrm = kSelf:GetControl("FRM_" .. iNo)
	    if false == kFrm:IsNil() then
	        kFrm:Visible(false)
	    end
	    iNo = iNo + 1
	end
	
 	local kParent = kSelf:GetParent()
	if false == kParent:IsNil() then
		local kFrame = kParent:GetControl("FRM_COMMUNITY_FRAME")
		if false == kFrame:IsNil() then
			kFrame:Visible(false)
		end
	end
end

-- iSetNo(0전체,1친구,2길드)
function OnClickPvP_Tab_Community( kSelf, iSetNo )
    if kSelf:IsNil() then return end
    
    local kParent = kSelf:GetParent()
    if kParent:IsNil() then return end
    
    local kCloseWnd = kParent:GetControl("CBTN_" .. iSetNo )
	if kCloseWnd:IsNil() then  return end
	
	SetPvpTabListMode( iSetNo )
	
	local iNo = 0
    while iNo < 3 do
        if iNo ~= iSetNo then
			kCloseWnd = kParent:GetControl("CBTN_" .. iNo )
   			if false == kCloseWnd:IsNil() then
	    		kCloseWnd:CheckState(false)
	    		kCloseWnd:LockClick(false)
			end
			
			kCloseWnd = kParent:GetControl("FRM_" .. iNo )
			if false == kCloseWnd:IsNil() then
	    		kCloseWnd:Visible(false)
			end
		else
		    kCloseWnd = kParent:GetControl("CBTN_" .. iNo )
   			if false == kCloseWnd:IsNil() then
   			    kCloseWnd:CheckState(true)
	    		kCloseWnd:LockClick(true)
			end
		    kCloseWnd = kParent:GetControl("FRM_" .. iNo )
			if false == kCloseWnd:IsNil() then
	    		kCloseWnd:Visible(true)
			end
		end
		iNo = iNo + 1
	end
	
	local kFrame = kParent:GetControl("FRM_COMMUNITY_FRAME")
	if false == kFrame:IsNil() then
		kFrame:Visible(false)
	end
	
	local kBtnTogether = kParent:GetControl("BTN_TOGETHER")
	if false == kBtnTogether:IsNil() then
		kBtnTogether:Disable( false )
		--kBtnTogether:Disable( 0 == iSetNo )
	end
	
--	OnDisplayPvP_Community(iSetNo)
end

function OnClickPvP_CommunityItem( kSelf )
	local kParent = kSelf:GetParent()
	if false == kParent:IsNil() then
		kParent:SetSelect( kSelf:GetOwnerGuid() )
	end
	return
	--[[
	local bRoom = false
	if false == kSelf:IsNil() then
		local kWnd = GetUIWnd("FRM_PVP_LOBBY")
		if kWnd:IsNil() then	
			kWnd = GetUIWnd("FRM_PVP_ROOM_ATTR")
			bRoom = true
			if kWnd:IsNil() then
				return false
			end
		end
		
		local kFrame = kWnd:GetControl("FRM_COMMUNITY_FRAME")
		if false == kFrame:IsNil() then
			local kPacket = kSelf:GetCustomDataAsPacket()
			kFrame:SetCustomDataAsPacket(kPacket)
			DeletePacket(kPacket)
				
			local pos = kSelf:GetTotalLocation()
			if true == bRoom then
				pos:SetX(pos:GetX()-726)
				pos:SetY(pos:GetY()-130)
			end
			pos:SetX(pos:GetX()+3)
			pos:SetY(pos:GetY()-3)
			kFrame:SetLocation(pos)
			kFrame:Visible(true)
			return true
		end
	end
	return false	]]--
end

function OnDisplayPvP_Community(iMode)
--	iMode = 1(Friend), 2(Guild)
	local kStatus = GetContentsStatus()
	
	local kWnd = nil
	if STATUS_LOBBY == kStatus then
		kWnd = GetUIWnd("FRM_PVP_LOBBY")
	elseif STATUS_ROOM == kStatus then
		kWnd = GetUIWnd("FRM_PVP_ROOM_ATTR")
	else
		return false
	end
	
	if false == kWnd:IsNil() then
		kWnd = kWnd:GetControl("FRM_COMMUNITY")
	end

	if false == kWnd:IsNil() then
		local kForm = kWnd:GetControl("FRM_" .. iMode )
		if 0 == iMode then--All
			return
		elseif 2 == iMode then-- Guild
			UpdateGuildUI(kForm)
			return
		end
		
		if false == kForm:IsNil() then
			local iSetMode = GetPvpTabListMode()
			local kListWnd = kForm:GetControl("LST_LIST"..iSetMode)
			if false == kListWnd:IsNil() then
				kListWnd:ClearAllListItem()
				
				for kKey, kFriendItem in pairs(g_kCommunity["FriendList"]) do
					local kNewItem = kListWnd:AddNewListItem(kFriendItem:CharacterName()):GetWnd()
					kNewItem:SetEnable(false)
					
					local iLinkStatus = kFriendItem:IsLinkStatus()
					local iLoc = kFriendItem:Location()
					
					local kStautsImg = kNewItem:GetControl("IMG_STATUS")
					if false == kStautsImg:IsNil() then
						if iLinkStatus ~= 0 then
							kStautsImg:SetUVIndex(iLinkStatus)
						else
							kStautsImg:SetUVIndex(4)
						end
					end
					
					if kFriendItem:IsOnline() then
						if iLoc == 1 then
							kNewItem:SetEnable(true)
							kNewItem:GetControl("FRM_POS"):SetStaticTextW(GetTextW(200001))
							
							if STATUS_LOBBY == kStatus then
								local kPacket = NewPacket(30730) -- PT_C_T_REQ_WITH_PVP
								kPacket:PushGuid(kFriendItem:Guid())
								kNewItem:SetCustomDataAsPacket(kPacket)
								DeletePacket(kPacket)
							else
								local kPacket = NewPacket(30732) -- PT_C_T_REQ_INVITE_PVP
								kPacket:PushSize_t(1)
								kPacket:PushGuid(kFriendItem:Guid())
								kNewItem:SetCustomDataAsPacket(kPacket)
								DeletePacket(kPacket)
							end
						else
							if GATTR_PVP == GetMapAttrByNo(iLoc) then
								kNewItem:SetEnable(true)
								kNewItem:GetControl("FRM_POS"):SetStaticTextW(GetTextW(200002))
								
								local kPacket = NewPacket(30730) -- PT_C_T_REQ_WITH_PVP
								kPacket:PushGuid(kFriendItem:Guid())
								kNewItem:SetCustomDataAsPacket(kPacket)
								DeletePacket(kPacket)
							else
								kNewItem:GetControl("FRM_POS"):SetStaticTextW(GetTextW(200004))
								kNewItem:SetCustomDataAsInt(200116)
							end
						end									
					else
						kNewItem:GetControl("FRM_POS"):SetStaticTextW(GetTextW(200004))
						kNewItem:SetCustomDataAsInt(200116)
					end
				end	
			end
		end
	end
	return true
end

function OnClickPvP_WithGame(kSelf)
	if false == kSelf:IsNil() then
		local kParent = kSelf:GetParent()
		if false == kParent:IsNil() then
			local iSetMode = GetPvpTabListMode()
			local kControl = kParent:GetControl("FRM_" .. iSetMode)
			if false == kControl:IsNil() then
				kControl = kControl:GetControl("LST_LIST" .. iSetMode)
				if false == kControl:IsNil() then
					local kSelectCharacterGuid = kControl:GetSelectGuid()
					if false == kSelectCharacterGuid:IsNil() then
						local kPacket = NewPacket(30730) -- PT_C_T_REQ_WITH_PVP
						kPacket:PushGuid(kSelectCharacterGuid)
						Net_Send(kPacket)
						DeletePacket(kPacket)
					end
				end
			end
		end
	end
end

function OnClickPvP_InvateGame(kSelf)
	if false == kSelf:IsNil() then
		local kParent = kSelf:GetParent()
		if false == kParent:IsNil() then
			local iSetMode = GetPvpTabListMode()
			local kControl = kParent:GetControl("FRM_" .. iSetMode)
			if false == kControl:IsNil() then
				kControl = kControl:GetControl("LST_LIST" .. iSetMode)
				if false == kControl:IsNil() then
					local kSelectCharacterGuid = kControl:GetSelectGuid()
					if false == kSelectCharacterGuid:IsNil() then
						local kPacket = NewPacket(30732) -- PT_C_T_REQ_INVITE_PVP
						kPacket:PushSize_t(1)
						kPacket:PushGuid(kSelectCharacterGuid)
						Net_Send(kPacket)
						DeletePacket(kPacket)
					end
				end
			end
		end
	end
end

function GetQuickTypeWnd()
	local kLobbyWnd = GetUIWnd("FRM_PVP_LOBBY")
	if false == kLobbyWnd:IsNil() then
		local kWnd = kLobbyWnd:GetControl("SFRM_MENUBOX")
		if false == kWnd:IsNil() then
			return kWnd:GetControl("SFRM_DROP_BG")
		end
	end
	return UIWnd()
end

function GetQuickType()
	local kWnd = GetQuickTypeWnd()
	if false == kWnd:IsNil() then
		return kWnd:GetCustomDataAsShort()
	end
	return 0
end

function CallPvP_QuickType(kSelf)
	if false == kSelf:IsNil() then
		kSelf:ClearAllListItem()
		local kNowQuickType = GetQuickType()

		if kNowQuickType ~= PVP_TYPE_ALL then
			local kWnd = kSelf:AddNewListItemChar( GetTextW(GetPvPTypeTextTable(PVP_TYPE_ALL)):GetStr() ):GetWnd()
			if false == kWnd:IsNil() then
				kWnd:SetCustomDataAsShort(PVP_TYPE_ALL)
			end
		end
		
		if kNowQuickType ~= PVP_TYPE_DM then
			local kWnd = kSelf:AddNewListItemChar( GetTextW(GetPvPTypeTextTable(PVP_TYPE_DM)):GetStr() ):GetWnd()
			if false == kWnd:IsNil() then
				kWnd:SetCustomDataAsShort(PVP_TYPE_DM)
			end
		end
		
		if kNowQuickType ~= PVP_TYPE_KTH then
			local kWnd = kSelf:AddNewListItemChar( GetTextW(GetPvPTypeTextTable(PVP_TYPE_KTH)):GetStr() ):GetWnd()
			if false == kWnd:IsNil() then
				kWnd:SetCustomDataAsShort(PVP_TYPE_KTH)
			end
		end
		
		if kNowQuickType ~= PVP_TYPE_LOVE then
			local kWnd = kSelf:AddNewListItemChar( GetTextW(GetPvPTypeTextTable(PVP_TYPE_LOVE)):GetStr() ):GetWnd()
			if false == kWnd:IsNil() then
				kWnd:SetCustomDataAsShort(PVP_TYPE_LOVE)
			end
		end
		
		if kNowQuickType ~= PVP_TYPE_WINNERS then
			local kWnd = kSelf:AddNewListItemChar( GetTextW(GetPvPTypeTextTable(PVP_TYPE_WINNERS)):GetStr() ):GetWnd()
			if false == kWnd:IsNil() then
				kWnd:SetCustomDataAsShort(PVP_TYPE_WINNERS)
			end
		end
	end
end

function OnClickPvP_QuickType(kSelf)
	if false == kSelf:IsNil() then
		local kWnd = GetQuickTypeWnd()
		if false == kWnd:IsNil() then
			local kType = kSelf:GetCustomDataAsShort()
			kWnd:SetCustomDataAsShort(kType)
			kWnd:SetStaticTextW(GetTextW(GetPvPTypeTextTable(kType)))
		end
	end
end

function ClosePvP_ExileBox(kOwnerGuid)
	local kBoxUI = GetUIWnd("FRM_PVP_INROOM_INFO")
	if false == kBoxUI:IsNil() then
	    if true == kOwnerGuid:IsEqual(kBoxUI:GetOwnerGuid()) then
			kBoxUI:Close()
	    end
	end
	
	kBoxUI = GetUIWnd("BOX_PVP_EXILE")
	if false == kBoxUI:IsNil() then
	    local kBtn = kBoxUI:GetControl("BTN_OK")
	    if false == kBtn:IsNil() then
	        if true == kOwnerGuid:IsEqual(kBtn:GetOwnerGuid()) then
	            kBoxUI:Close()
	        end
	    end

	end
end

--[[
function OnScrollPvP_HandyCap( kSelf )
	if false == kSelf:IsNil() then
		local kToolTip = ActivateUI("TOOLTIP_PVP_HANDYCAP")
		if false == kToolTip:IsNil() then
			local iValue = kSelf:GetScrollCur()
			kToolTip:SetStaticText(iValue)
		end
	end
end]]--

function OnClickPvP_RoomItem( kSelf )
	if false == kSelf:IsNil() then
		local kParent = kSelf:GetParent()
		local iRoomNo = kSelf:GetCustomDataAsInt()
		if false == kParent:IsNil() then
			local kParentOfParent = kParent:GetParent()
			if false == kParentOfParent:IsNil() then
				local kFrame = kParentOfParent:GetControl("FRM_CLICK_FRAME")
				if false == kFrame:IsNil() then
					kFrame:Visible(true)
					local total = kSelf:GetTotalLocation()
					local MaxScreen = Point2(1600, 1200)
					MaxScreen:IncX(-GetScreenSize():GetX())
					MaxScreen:IncY(-GetScreenSize():GetY())
					MaxScreen:SetX(MaxScreen:GetX()/2)
					MaxScreen:SetY(MaxScreen:GetY()/2)
					total:IncX(MaxScreen:GetX())
					total:IncY(MaxScreen:GetY())
					kFrame:SetLocation(total)
					kFrame:SetCustomDataAsInt(iRoomNo)
				end
			end
		end
		return true
	end
	return false
end

function OnDoubleClickPvP_RoomItem( kSelf )
	if true == OnClickPvP_RoomItem( kSelf ) then
		OnClickPvP_JoinRoom("", kSelf:GetCustomDataAsInt() )
	end
end

function InitPvP_LobbyRoomFrame( kFrame )
	if false == kFrame:IsNil() then
		local kPos = kFrame:GetLocation()
		kPos:SetX(64)
		kPos:SetY(0)
		kFrame:SetLocation(kPos)
		kFrame:Visible(false)
	end
end

function OnCall_FRM_PVP_MAIN( kUI, gametype )
	if gametype == PVP_TYPE_ANNIHILATION then
		local kControl = CallUI("FRM_MODE_EXPLANATION")
		if false == kControl:IsNil() then
			kControl:SetStaticTextW( GetTextW(200138) )
		end
	elseif gametype == PVP_TYPE_KTH then
	elseif gametype == PVP_TYPE_LOVE then
	else
		local kControl = CallUI("FRM_MODE_EXPLANATION")
		if false == kControl:IsNil() then
			kControl:SetStaticTextW( GetTextW(200137) )
		end
	end
end

function OnSet_PvPSlotHP( kUIWnd, iMaxHP, iNowHP )
	local kHPBar = kUIWnd:GetControl("BAR_HP")
	local iHP = kHPBar:GetBarNow()
	if iHP > iNowHP then
		OnSetPvpDamageTwinkle(kHPBar, 50, 1000);
	end
	if false == kHPBar:IsNil() then
		kHPBar:BarMax( iMaxHP )
		kHPBar:BarNow( iNowHP )
	end
	local kDeadUI = kUIWnd:GetControl("FRM_DEADMARK")
	if false == kDeadUI:IsNil() then
		if 0 == iNowHP or (0 == iHP and 0 ~= iNowHP) then
			OnCallPvPUserSlotChange(kUIWnd, 0.3)
		end
		OnSetPvpDieUserSlot(kUIWnd, 0 == iNowHP)
	end
end

function CallPvPKillCount( iKillCount )
	local iUVIndex = 0
	if 1 == iKillCount then
		iUVIndex = 1
		PlaySoundByID("PVP_First_Kill")
	elseif 2 == iKillCount then
		iUVIndex = 2
		PlaySoundByID("PVP_Double_Kill")
	elseif 3 == iKillCount then
		iUVIndex = 3
		PlaySoundByID("PVP_Triple_Kill")
	elseif 4 == iKillCount then
		iUVIndex = 4
		PlaySoundByID("PVP_Excellent")
	elseif 6 == iKillCount then
		iUVIndex = 5
		PlaySoundByID("PVP_Best_Player")
	elseif 10 == iKillCount then
		iUVIndex = 6
		PlaySoundByID("PVP_Unbelievable")
	elseif 15 == iKillCount then
		iUVIndex = 7
		PlaySoundByID("PVP_Oh_My_God")
	end
	
	if 0 < iUVIndex then
		local kUI = ActivateUI("FRM_PVP_KILL_COUNT")
		if false == kUI:IsNil() then
			kUI:SetUVIndex(iUVIndex)
			kUI:SetCustomDataAsInt(1)
			
			local kPos = kUI:GetLocation()
			kPos:SetX(300)
			kUI:SetLocation(kPos)
			
--			kUI:SetSizeScale( 3.0, false )
			kUI:SetMaxAlpha(0.0)
		else
			CheckNil( false )
		end
	end
end

function UpdatePvPKillCount( kSelf )
	local scrSize = GetScreenSize()
	if false == kSelf:IsNil() then
		local kAdd = (scrSize:GetX() - 1024	)/2
		local iState = kSelf:GetCustomDataAsInt()
		local dwLiveTimeMS = kSelf:GetLiveTime( false )
		
		if 1 == iState then
			local kPos = kSelf:GetLocation()
			
			if 200 <= dwLiveTimeMS then
				kSelf:SetCustomDataAsInt(2)
				kSelf:SetMaxAlpha( 1.0 )
--				kSelf:SetSizeScale( 1.0, false )
				kPos:SetX(300 + kAdd)
			else
				local fRate = dwLiveTimeMS / 200
				kSelf:SetMaxAlpha( fRate )
--				kSelf:SetSizeScale( 3.0 - ( 2.0 * fRate ) )
				kPos:SetX( (900+kAdd) - ( 600 * fRate ) )
			end
			
			kSelf:SetLocation(kPos)
		elseif 2 == iState then
			if 2650 <= dwLiveTimeMS then
				kSelf:SetCustomDataAsInt(3)
			end
		elseif 3 == iState then
			local kPos = kSelf:GetLocation()
			local fRate = (3000 - dwLiveTimeMS) / 350 + 0.1
			if fRate < 0.0 then
				fRate = 0.0
			end

			kSelf:SetMaxAlpha( fRate )
			kPos:SetX( ( 300 * fRate ) )
			kSelf:SetLocation(kPos)
		end
	else
		CheckNil( false )
	end
end

function CallPvPMessage( wstrMsg )
	local kUI = ActivateUI( "FRM_PVP_MESSAGE" )
	if false == kUI:IsNil() then
		local kText = kUI:GetControl("FRM_MSG")
		if false == kText:IsNil() then
			kText:SetStaticTextW( wstrMsg )
			kText:Visible(false)
		else
			CheckNil( false )
		end
		
		local kImg = kUI:GetControl("IMG_BG")
		if false == kImg:IsNil() then
			kImg:Visible(false)
		end
		kUI:SetCustomDataAsInt( 3000 )
	else
		CheckNil( false )
	end
end

function UpdatePvPMessage( kSelf )
	if false == kSelf:IsNil() then
		local iValue = kSelf:GetCustomDataAsInt()
		if 0 < iValue then
			local dwLiveTimeMS = kSelf:GetLiveTime( false )
			if dwLiveTimeMS >= iValue then
			
				local kText = kSelf:GetControl("FRM_MSG")
				if false == kText:IsNil() then
					kText:Visible(true)
				end
				
				local kImg = kSelf:GetControl("IMG_BG")
				if false == kImg:IsNil() then
					kImg:Visible(true)
				end
				
				kSelf:SetCustomDataAsInt( 0 )
			end
		end	
	else
		CheckNil( false )
	end
end

--[[
function UpdatePvPLobbyIDBtn( kUI )
	if false == kUI:IsNil() then
		if true == kUI:IsClickLock() then
			local iCtmValue = kUI:GetCustomDataAsInt()
			if 1 < iCtmValue then
				kUI:SetCustomDataAsInt( iCtmValue - 1 )
			else
				kUI:SetClickLock( false )
				kUI:ClearCustomData()
			end
		end
	end
end
]]

function OnClickPvP_RankingPage( kBtn )
	if false == kBtn:IsNil() then
		local iPage = kBtn:GetCustomDataAsInt()
		if 0 > iPage then
			return
		end
		
		local kPageControl = kBtn:GetParent()
		if kPageControl:IsNil() then
			return
		end
		
		local kRankingUI = kPageControl:GetParent()
		if false == kRankingUI:IsNil() then
			local kUI = kRankingUI:GetControl("SFRM_MIDDLE")
			if false == kUI:IsNil() then
				local kList = kUI:GetControl("LST_LIST")
				kList:SetDisplayStartItem( iPage * 15 )
				
				local kPrevBtn = kPageControl:GetControl("BTN_TO_PREV")
				if false == kPrevBtn:IsNil() then
					if 5 > iPage then
						kPrevBtn:SetEnable(false)
					else
						kPrevBtn:SetEnable(true)
						kPrevBtn:SetCustomDataAsInt( iPage - 5 )
					end
				end
				
				local kNextBtn = kPageControl:GetControl("BTN_TO_NEXT")
				if false == kNextBtn:IsNil() then
					if 0 == iPage then
						kNextBtn:SetEnable(true)
						kNextBtn:SetCustomDataAsInt( 5 )
					elseif 5 > iPage  then
						kNextBtn:SetEnable(true)
						kNextBtn:SetCustomDataAsInt( 6 )					
					else
						kNextBtn:SetEnable(false)
					end
				end
				
				local iStartPage = 0
				if 4 < iPage then
					iStartPage = 5
				end
				
				local iLastPage = 6
				for i = 0, 5 do
					local kButton = kPageControl:GetControl("BTN_NUM_" .. i)
					if false == kButton:IsNil() then
						local iSetPage = iStartPage + i
						if iLastPage >= iSetPage then
							kButton:SetStaticText( iSetPage + 1 )
							kButton:SetCustomDataAsInt( iSetPage )
							
							if iPage == iSetPage then
								kButton:SetEnable(false)
								kButton:SetFontColorRGBA(247,234,150,255)--f7ea96
							else
								kButton:SetEnable(true)
								kButton:SetFontColorRGBA(255,255,255,255)
							end
						else
							kButton:SetStaticText("")
							kButton:SetEnable(false)
						end
					end
				end
			end
		end
	
	end
end
------------------------------------------------------------------------------->
------------------------------------------------------------------------------->
function PVP_START_UI_OnCall(kUI)
	SetUISteer(kUI
			, GetFrameTime()
			, ReCalcUIPos(256+60, true), ReCalcUIPos(192, false)
			, ReCalcUIPos(256+60, true), ReCalcUIPos(192, false)
			, 0,300,300
			, 1,1,0
			, 1,1,0)	
end

function PVP_START_UI_OnTick(kUI)
	local ExecutedTime = SteeringUI_OnTick(kUI)
	
	local Inc = 0.3		-- Inc 초 마다 깜빡임
	local BeginTime =0
	local EndTime = 2
	local bHide = false	
	local Cnt = 0
	for i = BeginTime,EndTime,Inc do
		if( ExecutedTime > i and i+Inc > ExecutedTime ) then
		
			if(Cnt%2 == 0) then		
				bHide = false
			else
				bHide = true
			end			
--[[			
			if(false == bHide) then  bHide = true
				ODS("감춤\n")
			else 
				bHide = false
				ODS("보임\n")
			end		
]]
			SetUISteer(kUI
			, ExecutedTime+GetFrameTime()
			, ReCalcUIPos(256+60, true), ReCalcUIPos(192, false)
			, ReCalcUIPos(256+60, true), ReCalcUIPos(192, false)
			, 0,0,0
			, 1,1,0
			, 1,1,0, bHide)
		end
		Cnt = Cnt+1
	end
	
	local MoveTime = EndTime
	if(ExecutedTime > MoveTime and MoveTime+Inc > ExecutedTime ) then
		SetUISteer(kUI
		, ExecutedTime+GetFrameTime()
		, ReCalcUIPos(256+60, true), ReCalcUIPos(192, false)
		, ReCalcUIPos(-512, true), ReCalcUIPos(192, false)
		, 0,300,300
		, 1,1,0
		, 1,0,-3, false)
	end
	if(kUI:GetLocation():GetX() < -256) then 
		kUI:Close()

		if( CheckNil(g_world == nil) ) then return end
		if( CheckNil(g_world:IsNil()) ) then return end
							
		local TopWnd = GetUIWnd("FRM_PVP_MAIN")
		if false == TopWnd:IsNil() then
			local SubWnd = TopWnd:GetControl("FRM_ROUND")
			if false == SubWnd:IsNil() then
				local Num10Wnd = SubWnd:GetControl("IMG_NUM10")
				if false == Num10Wnd:IsNil() then
					local Num01Wnd = SubWnd:GetControl("IMG_NUM01")
					if false == Num01Wnd:IsNil() then
						local Num10 = Num10Wnd:GetUVIndex()
						local Num01 = Num01Wnd:GetUVIndex()
						if( 1 == Num10 and 2 == Num01 ) then
							if true == g_world:IsHaveWorldAttr(GATTR_FLAG_PVP) then
								AddWarnDataTT(749993) -- PVP 지역에서는 옵션증폭이 적용 안된다는 시스템 메시지
							end
							return
						else
							return
						end
					end
				end
			end
		end
		if true == g_world:IsHaveWorldAttr(GATTR_FLAG_PVP) then
			AddWarnDataTT(749993) -- PVP 지역에서는 옵션증폭이 적용 안된다는 시스템 메시지
		end
	end
	
end

function PVP_GAMESET_HEAD_OnCall(kUI)
	if nil == kUI then return end
	if true == kUI:IsNil() then return end	
	
	local fRevise = 0
	local fReviser1 = 0.1953125*(GetScreenSize():GetX() - 1024)
	fRevise = fReviser1*(1024/GetScreenSize():GetX())

	SetUISteer(kUI
		, GetFrameTime()
		, ReCalcUIPos(-kUI:GetWidth()+fRevise+60, true), ReCalcUIPos(192, false)
		, ReCalcUIPos(261+fRevise, true), ReCalcUIPos(192, false)
		, 0,510,510
		, 1,1,0
		, 1,1,0
		)
end

function PVP_GAMESET_HEAD_OnTick(kUI)
	if nil == kUI then return end
	if true == kUI:IsNil() then return end	
		
	local fRevise = 0
	local fReviser1 = 0.1953125*(GetScreenSize():GetX() - 1024)
	fRevise = fReviser1*(1024/GetScreenSize():GetX())
	
	local ExecutedTime = SteeringUI_OnTick(kUI)
	if( ExecutedTime > 1.2 and 1.3 > ExecutedTime ) then
		SetUISteer(kUI
		, ExecutedTime+GetFrameTime()
		, ReCalcUIPos(261+fRevise, true), ReCalcUIPos(192, false)
		, ReCalcUIPos(-1024+fRevise, true), ReCalcUIPos(192, false)
		, 0,500,35
		, 1,10,8
		, 1,0,-20)
	end
end

function PVP_GAMESET_TAIL_OnCall(kUI)
	if nil == kUI then return end
	if true == kUI:IsNil() then return end	
	
	local fRevise = 0
	
	local fReviser1 = -0.0390625*(GetScreenSize():GetX() - 1024)
	fRevise = fReviser1*(1024/GetScreenSize():GetX())

	SetUISteer(kUI
		, GetFrameTime()
		, ReCalcUIPos(1024+kUI:GetWidth()+fRevise, true), ReCalcUIPos(192, false)
		, ReCalcUIPos(549+fRevise, true), ReCalcUIPos(192, false)
		, 0,600,600
		, 1,1,0
		, 1,1,0
		)
end

function PVP_GAMESET_TAIL_OnTick(kUI)
	if nil == kUI then return end
	if true == kUI:IsNil() then return end	
	
	local fRevise = 0
	local fReviser1 = -0.0390625*(GetScreenSize():GetX() - 1024)
	fRevise = fReviser1*(1024/GetScreenSize():GetX())
	
	local ExecutedTime = SteeringUI_OnTick(kUI)
	if( ExecutedTime > 1.2 and 1.3 > ExecutedTime ) then
		SetUISteer(UISelf
			, ExecutedTime+GetFrameTime()
			, ReCalcUIPos(549+fRevise, true), ReCalcUIPos(192, false)
			, ReCalcUIPos(1824, true), ReCalcUIPos(192, false)
			, 0,500,35
			, 1,10,8
			, 1,0,-20)
	end
end

-- PVP Time Over 

function PVP_TIMEOVER_HEAD_OnCall(kUI)
	if nil == kUI then return end
	if true == kUI:IsNil() then return end	
	
	local fRevise = 0
	local fReviser1 = 0.1302083*(GetScreenSize():GetX() - 1024)
	fRevise = fReviser1*(1024/GetScreenSize():GetX())
	
	SetUISteer(kUI
		, GetFrameTime()
		, ReCalcUIPos(-kUI:GetWidth()+60+fRevise, true), ReCalcUIPos(192, false)
		, ReCalcUIPos(256+25-16+60+fRevise, true), ReCalcUIPos(192, false)		
		, 0,600,600
		, 1,1,0
		, 1,1,0
		)
end

function PVP_TIMEOVER_HEAD_OnTick(kUI)
	if nil == kUI then return end
	if true == kUI:IsNil() then return end	
	
	local fRevise = 0
	local fReviser1 = 0.1302083*(GetScreenSize():GetX() - 1024)
	fRevise = fReviser1*(1024/GetScreenSize():GetX())
	
	local ExecutedTime = SteeringUI_OnTick(kUI)
	if( ExecutedTime > 1.2 and 1.3 > ExecutedTime ) then
		SetUISteer(kUI
		, ExecutedTime+GetFrameTime()
		, ReCalcUIPos(265+60+fRevise, true), ReCalcUIPos(192, false)
		, ReCalcUIPos(-1024, true), ReCalcUIPos(192, false)
		, 0,500,35
		, 1,10,8
		, 1,0,-20)
	end
end

function PVP_TIMEOVER_TAIL_OnCall(kUI)
	if nil == kUI then return end
	if true == kUI:IsNil() then return end	
	
	local fRevise = 0
	local fReviser1 = -0.078125*(GetScreenSize():GetX() - 1024)
	fRevise = fReviser1*(1024/GetScreenSize():GetX())
	
	SetUISteer(kUI
		, GetFrameTime()
		, ReCalcUIPos(1024+60+fRevise, true), ReCalcUIPos(192, false)
		, ReCalcUIPos(506+60+fRevise, true), ReCalcUIPos(192, false)
		, 0,600,600
		, 1,1,0
		, 1,1,0
		)
end

function PVP_TIMEOVER_TAIL_OnTick(kUI)
	if nil == kUI then return end
	if true == kUI:IsNil() then return end	
	
	local fRevise = 0
	local fReviser1 = -0.078125*(GetScreenSize():GetX() - 1024)
	fRevise = fReviser1*(1024/GetScreenSize():GetX())
	
	local ExecutedTime = SteeringUI_OnTick(kUI)
	if( ExecutedTime > 1.2 and 1.3 > ExecutedTime ) then
		SetUISteer(UISelf
			, ExecutedTime+GetFrameTime()
			, ReCalcUIPos(506+60+fRevise, true), ReCalcUIPos(192, false)
			, ReCalcUIPos(1824+60, true), ReCalcUIPos(192, false)
			, 0,500,35
			, 1,10,8
			, 1,0,-20)
	end
end

function CheckJoinLevel( kUIWnd )
	if nil == kUIWnd then
		return false
	end
	
	if kUIWnd:IsNil() then
		return false
	end
	
	local iLimitLevel = kUIWnd:GetCustomDataAsShort()
	
	local pilot = g_pilotMan:GetPlayerPilot()
	if false == pilot:IsNil() then
		if iLimitLevel <= pilot:GetAbil(AT_LEVEL) then
			return true
		end
	end
	
	g_ChatMgrClient:Notice_Show( GetTextW(460018), 1 )
	return false
end

function OnToolTip_PvPType( kUIWnd )

	if false == kUIWnd:IsNil() then
		local iType = kUIWnd:GetCustomDataAsInt()
		if PVP_TYPE_DM == iType then
			CallToolTipText(0, GetTextW(400292), GetUIWnd("Cursor"):GetLocation(), "ToolTip2", 11 )
		elseif PVP_TYPE_ANNIHILATION == iType then
			CallToolTipText(0, GetTextW(400293), GetUIWnd("Cursor"):GetLocation(), "ToolTip2", 11 )
		elseif PVP_TYPE_KTH == iType then
			CallToolTipText(0, GetTextW(200166), GetUIWnd("Cursor"):GetLocation(), "ToolTip2", 11 )
		elseif PVP_TYPE_LOVE == iType then
			CallToolTipText(0, GetTextW(400371), GetUIWnd("Cursor"):GetLocation(), "ToolTip2", 11 )
		end
	end
	
end


function OnCall_RegistTeam()
	local kMyActor = GetMyActor()
	local kMyGuid = kMyActor:GetPilotGuid()	
	if( false == HaveIParty() ) then
		--파티상태에서만 사용할 수 있는 메뉴 입니다.
		CommonMsgBoxByTextTable( 560050, true )
	elseif( false == IsPartyMaster(kMyGuid) ) then
		--파티장만 사용할 수 있는 메뉴 입니다.
		CommonMsgBoxByTextTable( 560051, true )
	else
		CallUI("SFRM_REGIST_TEAM", true)
	end
end

function Recv_PT_N_C_NFY_PVPLEAGUE_NOTICE_MSG( eMessageType, szLevel )
	local kMsg;
	if( 0 == eMessageType ) then
		--파티상태에서만 사용할 수 있는 메뉴 입니다.
		kMsg = GetTextW(560054)
	elseif( 1 == eMessageType ) then
		CallUI("SFRM_INVITE_LEAGUE", true)
		return
	elseif( 2 == eMessageType ) then
		kMsg = WideString(string.format(GetTextW(560077):GetStr(),szLevel))
	elseif( 3 == eMessageType ) then
		kMsg = GetTextW(560078)
	elseif( 4 == eMessageType ) then
		kMsg = WideString(string.format(GetTextW(560079):GetStr(),szLevel))
	end
	
	Notice_Show( kMsg, false )
end

function OnTick_InviteLeague(kSelf)
	if( false == kSelf:IsNil() )then
		local kMinuete = kSelf:GetControl("MINUETE")
		local kSecond = kSelf:GetControl("SECOND")
		if( false == kMinuete:IsNil() and false == kSecond:IsNil() ) then
			local iMinuete = kMinuete:GetCustomDataAsInt()
			local iSecond = kSecond:GetCustomDataAsInt()
			
			iSecond = iSecond-1
			if( 0 > iSecond-1 ) then
				iSecond = 59
				iMinuete = iMinuete-1
				if( 0 > iMinuete-1 ) then
					iMinuete = 0
					return false
				end
			end
			kMinuete:SetCustomDataAsInt(iMinuete)
			kSecond:SetCustomDataAsInt(iSecond)
			
			kSelf:SetStaticText(GetTextW(560004):GetStr()..tostring(iMinuete)..GetTextW(176):GetStr().." "..tostring(iSecond)..GetTextW(177):GetStr())
			return true
		end
	end
	return false
end

function SendPvPLeague_QueryTeam()
	local kMyActor = GetMyActor()
	local kMyGuid = kMyActor:GetPilotGuid()
	local kPacket = NewPacket(33024)--PT_C_N_REQ_QUERY_PVPLEAGUE_TEAM
	kPacket:PushGuid(kMyGuid)
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function CallPvPDropMenu(self, str)
	if nil==self or self:IsNil() or ""==str then
		return
	end

	local modelist = CallUI(str)
	if false==modelist:IsNil() then
		local kCursorPos = self:GetTotalLocation()
		kCursorPos:SetX(kCursorPos:GetX() - modelist:GetSize():GetX())
		kCursorPos:SetY(kCursorPos:GetY() + 25)
		modelist:SetLocation(kCursorPos)
	end
end

function OnBuildSlotPvPAniUI(kSelf)
	if( nil == kSelf ) then return end
	if( true == kSelf:IsNil() ) then return end
	
	local ptScreenSize = GetScreenSize()
	local iImgBgWidth = 272
	
	local iBuildIndex = kSelf:GetBuildIndex()
	local kPos = kSelf:GetLocation()
	if iBuildIndex > 4 then	-- 우측 부분
		kSelf:ChangeImage( "../Data/6_ui/Pvp/pvWdwRAni.tga" )
		local iMoveX = 0;
		if ( iBuildIndex - 5 ) > 3 then
			iMoveX = 3 * 15
		else
			iMoveX = ( iBuildIndex - 5 ) * 15
		end	
		local iPosX = (ptScreenSize:GetX()-iImgBgWidth)+iMoveX -- 오른쪽 끝에서 부터 위치를 계산하도록 수정
		kPos:SetX( iPosX )
	else					-- 좌측 부분
		local iMoveX = 0;
		if iBuildIndex > 3 then
			iMoveX = 3 * 15
		else
			iMoveX = iBuildIndex * 15
		end
		kPos:SetX( kPos:GetX() - iMoveX )
	end
	kSelf:SetLocation(kPos)
end


function OnBuildSlotPvPUI(kSelf)
	if( nil == kSelf ) then return end
	if( true == kSelf:IsNil() ) then return end
	
	local ptScreenSize = GetScreenSize()
	local iImgBgWidth = 272
	
	local iBuildIndex = kSelf:GetBuildIndex()
	
	local kRankImg = kSelf:GetControl("IMG_RANK")
	kRankImg:SetUVIndex(iBuildIndex+1)
	
	if iBuildIndex > 4 then	-- 우측 부분
		kSelf:ChangeImage( "../Data/6_UI/pvp/pvWdwR.tga" )
		
		kFrmPos = kSelf:GetLocation()
		kFrmPos:SetX(771)
		kSelf:SetLocation(kFrmPos)
		
		local kFrmName = kSelf:GetControl("FRM_NAME")
		local kFrmPos = kFrmName:GetLocation()
		kFrmPos:SetX(60)
		kFrmName:SetLocation(kFrmPos)
		kFrmName:SetFontFlag( kFrmName:GetFontFlag() + 32 )
		
		kFrmPos = kRankImg:GetLocation()
		kFrmPos:SetX(134)
		kRankImg:SetLocation(kFrmPos)
			
		local kForm = kSelf:GetControl("IMG_KILL")
		if false == kForm:IsNil() then
			kFrmPos = kForm:GetLocation()
			kFrmPos:SetX( 170 )
			kForm:SetLocation(kFrmPos)
			kFrmPos = kForm:GetTextPos()
			kFrmPos:SetX(50)
			kForm:SetTextPos(kFrmPos)						
			kForm:SetFontFlag(kForm:GetFontFlag() + 32)
		end
		
		kForm = kSelf:GetControl("IMG_DEATH")
		if false == kForm:IsNil() then
			kFrmPos = kForm:GetLocation()
			kFrmPos:SetX( 210 )
			kForm:SetLocation(kFrmPos)
			kFrmPos = kForm:GetTextPos()
			kFrmPos:SetX(50)
			kForm:SetTextPos(kFrmPos)						
			kForm:SetFontFlag(kForm:GetFontFlag() + 32)
		end
		
		kForm = kSelf:GetControl("FRM_KILL")
		if false == kForm:IsNil() then
			kFrmPos = kForm:GetLocation()
			kFrmPos:SetX( 190 )
			kForm:SetLocation(kFrmPos)
		end
	
		kForm = kSelf:GetControl("FRM_DEATH")
		if false == kForm:IsNil() then
			kFrmPos = kForm:GetLocation()
			kFrmPos:SetX( 230 )
			kForm:SetLocation(kFrmPos)
		end
		
		kForm = kSelf:GetControl("FRM_DEADMARK")
		if false == kForm:IsNil() then
			kFrmPos = kForm:GetLocation()
			kFrmPos:SetX( 137 )
			kForm:SetLocation(kFrmPos)
		end
		
		kForm = kSelf:GetControl("BAR_HP")
		if false == kForm:IsNil() then
			kForm:ChangeImage( "../Data/6_UI/pvp/pvHpbarR.tga", true )
			kFrmPos = kForm:GetLocation()
			kFrmPos:SetX( 2 )
			kForm:SetLocation(kFrmPos)
			kForm:SetBarReverse(true)
		end
		
		kForm = kSelf:GetControl("BAR_MP")
		if false == kForm:IsNil() then
			kForm:ChangeImage( "../Data/6_UI/pvp/pvMpbarR.tga", true )
			kFrmPos = kForm:GetLocation()
			kFrmPos:SetX( 2 )
			kForm:SetLocation(kFrmPos)
			kForm:SetBarReverse(true)
		end
		
		kForm = kSelf:GetControl("FRM_TOTALPOINT")
		if false == kForm:IsNil() then
			kFrmPos = kForm:GetLocation()
			kFrmPos:SetX(210)
			kForm:SetLocation(kFrmPos)
		end
		
	else					-- 좌측 부분
		local kForm = kSelf:GetControl("IMG_KILL")
		if false == kForm:IsNil() then
			kFrmPos = kForm:GetLocation()
			kFrmPos:SetX( kFrmPos:GetX() )
			kForm:SetLocation(kFrmPos)
		end
		
		kForm = kSelf:GetControl("IMG_DEATH")
		if false == kForm:IsNil() then
			kFrmPos = kForm:GetLocation()
			kFrmPos:SetX( kFrmPos:GetX() )
			kForm:SetLocation(kFrmPos)
		end
		
		kForm = kSelf:GetControl("FRM_KILL")
		if false == kForm:IsNil() then
			kFrmPos = kForm:GetLocation()
			kFrmPos:SetX( 41 )
			kForm:SetLocation(kFrmPos)
		end
	
		kForm = kSelf:GetControl("FRM_DEATH")
		if false == kForm:IsNil() then
			kFrmPos = kForm:GetLocation()
			kFrmPos:SetX( 81 )
			kForm:SetLocation(kFrmPos)
		end
		
		kForm = kSelf:GetControl("FRM_TOTALPOINT")
		if false == kForm:IsNil() then
			kFrmPos = kForm:GetLocation()
			kFrmPos:SetX(60)
			kForm:SetLocation(kFrmPos)
		end

		kFrmPos = kSelf:GetLocation()
		kFrmPos:SetX(-17)
		kSelf:SetLocation(kFrmPos)
		
	end
end

function OnSelectPvPType(Self)
	local TopWnd = GetUIWnd("FRM_PVP_CREATEROOM_BODY")
	if TopWnd:IsNil() then return end
	local Wnd = TopWnd:GetControl("FRM_PVP_CREATEROOM")
	if Wnd:IsNil() then return end
	
	local iGndNo = GetRandomPvPGround( false, Self:GetCustomDataAsInt() )
	OnSelectPvP_Ground( iGndNo, Wnd )
	
	OnSelectPvP_MaxPlayer( PVP_DEFAULT_MAXPLAYER, Wnd )
	
	local SelectType = UISelf:GetCustomDataAsInt()
	OnSelectPvP_Type( SelectType, Wnd )
	
	local LimitWnd = Wnd:GetControl("IMG_TAB_TIMELIMIT")
	if LimitWnd:IsNil() then return end

	if PVP_TYPE_KTH == SelectType then
		LimitWnd:SetStaticText(GetTextW(200163):GetStr())
	elseif PVP_TYPE_DM == SelectType
		or PVP_TYPE_ANNIHILATION == SelectType
		or PVP_TYPE_LOVE == SelectType then
		LimitWnd:SetStaticText(GetTextW(400325):GetStr())
	else
	end
end

function OnSelectPvPType_InRoom(Self)
	PvPAttrTable["TYPE"] = Self:GetCustomDataAsInt() -- 게임 타입
	SetEditPvP_Type( PvPAttrTable["TYPE"] )

	PvPAttrTable["GROUNDNO"] = GetRandomPvPGround( false, PvPAttrTable["TYPE"] ) -- 맵 번호

	PvPAttrTable["GROUNDNAME"] = GetPvPGroundName( PvPAttrTable["GROUNDNO"] ) -- 맵 이름

	if PVP_TYPE_KTH == PvPAttrTable["TYPE"]
		or PVP_TYPE_ANNIHILATION == PvPAttrTable["TYPE"]
		or PVP_TYPE_LOVE == PvPAttrTable["TYPE"] then
		PvPAttrTable["MODE"] = PVP_MODE_TEAM -- 섬멸전, 점령전, 깃발전은 팀모드만 가능
	end

	if PVP_TYPE_DM == PvPAttrTable["TYPE"] or PVP_TYPE_KTH == PvPAttrTable["TYPE"] then
		PvPAttrTable["MAXROUND"] = 1
	elseif PVP_TYPE_LOVE == PvPAttrTable["TYPE"] then
		PvPAttrTable["MAXROUND"] = 5
	else
		PvPAttrTable["MAXROUND"] = 3
	end

	PvPAttrTable["MAXUSER"] = PVP_DEFAULT_MAXPLAYER
	
	SetEditPvP_Ground( PvPAttrTable["GROUNDNO"] )
	SetEditPvP_Mode( PvPAttrTable["MODE"] )
	SetEditPvP_RoundCount( PvPAttrTable["MAXROUND"] )

	if PVP_TYPE_KTH == PvPAttrTable["TYPE"] then
		PvPAttrTable["GAMETIME"] = 0
		PvPAttrTable["GAMEPOINT"] = PVP_DEFAULT_POINTLIMIT
		SetEditPvP_Point( PvPAttrTable["GAMEPOINT"] )
	elseif PVP_TYPE_DM == PvPAttrTable["TYPE"] 
		or PVP_TYPE_ANNIHILATION == PvPAttrTable["TYPE"] then
		PvPAttrTable["GAMETIME"] = PVP_DEFAULT_TIMELIMIT
		PvPAttrTable["GAMEPOINT"] = 0
		SetEditPvP_Time( PvPAttrTable["GAMETIME"] )
	elseif PVP_TYPE_LOVE == PvPAttrTable["TYPE"] then
		PvPAttrTable["GAMETIME"] = 0
		PvPAttrTable["GAMEPOINT"] = 0
		SetEditPvP_Time( PvPAttrTable["GAMETIME"] )
	else
		PvPAttrTable["GAMETIME"] = PVP_DEFAULT_TIMELIMIT
		PvPAttrTable["GAMEPOINT"] = PVP_DEFAULT_POINTLIMIT
		SetEditPvP_Time( PvPAttrTable["GAMETIME"] )
		SetEditPvP_Point( PvPAttrTable["GAMEPOINT"] )
	end
	
	local Wnd = GetUIWnd("FRM_PVP_ROOM_ATTR")
	if false == Wnd:IsNil() then
		local LimitWnd = Wnd:GetControl("IMG_TAB_TIMELIMIT")
		if false == LimitWnd:IsNil() then
			if PVP_TYPE_KTH == PvPAttrTable["TYPE"] then
				LimitWnd:SetStaticText(GetTextW(200163):GetStr())
			elseif PVP_TYPE_DM == PvPAttrTable["TYPE"] 
				or PVP_TYPE_ANNIHILATION == PvPAttrTable["TYPE"]
				or PVP_TYPE_LOVE == PvPAttrTable["TYPE"] then
				LimitWnd:SetStaticText(GetTextW(400325):GetStr())
			else
			end
		end
	end
end

g_IsChageStrongholdTeam = false
g_NowStrongholdTeam = 0
function OnUpdate_StrongHold_Bar(Self)
	local OldPoint = GetEntityPointByTriggerID(g_NowKTHTriggerName)
	local NowPoint = OldPoint
	if NowPoint < 0 then
		NowPoint = NowPoint * -1
	end
	local NowLevel = GetLevelByPoint(NowPoint)
	local MaxLevelPoint = GetPointByLevel(4)
	local RedBarWnd = Self:GetControl("BAR_RED")
	local BlueBarWnd = Self:GetControl("BAR_BLUE")
	local ArrowWnd = Self:GetControl("FRM_ARROW")

	local PointRate = math.floor(NowPoint / MaxLevelPoint * 100)
	if false == ArrowWnd:IsNil() then
		local BarSize = RedBarWnd:GetSize()
		local BarPosX = math.floor(BarSize:GetX() * PointRate / 100)
		if OldPoint > 0 then
			BarPosX = 125 + BarPosX
		else
			BarPosX = 125 - BarPosX
		end
		ArrowWnd:SetLocation( Point2( BarPosX, ArrowWnd:GetLocation():GetY() ) )
	end
	
	if false == RedBarWnd:IsNil() or false == BlueBarWnd:IsNil() then
		if OldPoint > 0 then
			RedBarWnd:Visible(true)
			BlueBarWnd:Visible(false)
			RedBarWnd:BarMax(MaxLevelPoint)
			RedBarWnd:BarNow(NowPoint)
		else
			RedBarWnd:Visible(false)
			BlueBarWnd:Visible(true)
			BlueBarWnd:BarMax(MaxLevelPoint)
			BlueBarWnd:BarNow(NowPoint)
		end
	end

	local TextWnd = Self:GetControl("FRM_TEXT")
	if false == TextWnd:IsNil() then
		if OldPoint > 0 then -- RED TEAM
			local TextPos = TextWnd:GetTextPos()
			if TextPos:GetX() > 64 then
				TextWnd:SetTextPos(Point2(TextPos:GetX() - 10, 3))
			else
				TextWnd:SetTextPos(Point2(64, 3))
			end
			if 0 ~= NowLevel then
				TextWnd:SetStaticText( GetTT(70100):GetStr().." "..GetTT(799359):GetStr().."."..NowLevel )
			else
				TextWnd:SetStaticText( GetTT(70100):GetStr().." "..GetTT(200164):GetStr() )
			end
		elseif OldPoint < 0 then -- BLUE TEAM
			local TextPos = TextWnd:GetTextPos()
			if TextPos:GetX() < 192 then
				TextWnd:SetTextPos(Point2(TextPos:GetX() + 10, 3))
			else
				TextWnd:SetTextPos(Point2(192, 3))
			end
			if 0 ~= NowLevel then
				TextWnd:SetStaticText( GetTT(70101):GetStr().." "..GetTT(799359):GetStr().."."..NowLevel )
			else
				TextWnd:SetStaticText( GetTT(70101):GetStr().." "..GetTT(200164):GetStr() )
			end
		end
	end
end

g_Start_Bear_Timer_Red = false
g_Start_Bear_Timer_Blue = false
function SetLoveBearTimer( Team, Time, bStart )	
	if Time > 0 then
		Time = 6 - Time
	end
	
	local RedWnd = GetUIWnd("FRM_RED_BEAR_TIMER")
	if RedWnd:IsNil() then return end
	local RedTimerWnd = RedWnd:GetControl("FRM_TIMER")
	if RedTimerWnd:IsNil() then return end
	
	local BlueWnd = GetUIWnd("FRM_BLUE_BEAR_TIMER")
	if BlueWnd:IsNil() then return end
	local BlueTimerWnd = BlueWnd:GetControl("FRM_TIMER")
	if BlueTimerWnd:IsNil() then return end
	
	if TEAM_RED == Team then
		g_Start_Bear_Timer_Red = bStart
	elseif TEAM_BLUE == Team then
		g_Start_Bear_Timer_Blue = bStart
	end
	
	if Time < 0 then	-- 시간이 음수로 들어오면 곰이 본진으로 돌아간 것임
		if TEAM_RED == Team then
			RedTimerWnd:SetUVIndex(7)
			RedWnd:GetControl("FRM_BEAR"):Visible(false)
		elseif TEAM_BLUE == Team then
			BlueTimerWnd:SetUVIndex(6)
			BlueWnd:GetControl("FRM_BEAR"):Visible(false)
		end
	elseif Time > 0 then
		if TEAM_RED == Team then
			RedTimerWnd:SetUVIndex(Time)
			RedWnd:GetControl("FRM_BEAR"):Visible(true)
		elseif TEAM_BLUE == Team then
			BlueTimerWnd:SetUVIndex(Time)
			BlueWnd:GetControl("FRM_BEAR"):Visible(true)
		end
	end
end

g_TotalPointWarning_R = false	-- 승리포인트 20% 이하. 경고 1단계
g_TotalPointWarning_B = false	-- 승리포인트 20% 이하. 경고 1단계
function OnUpdate_Team_TotalPoint(Team, Point)
	local TopWnd = GetUIWnd("FRM_PVP_MAIN")
	if TopWnd:IsNil() then return end

	local PointWnd = TopWnd:GetControl("FRM_POINT")
	if PointWnd:IsNil() then return end	

	local TotalPointRate = math.floor(Point / PvPAttrTable["GAMEPOINT"] * 100)
	
	g_TotalPointWarning_R = false
	g_TotalPointWarning_B = false
	
	if Team == 1 then -- RED TEAM
		local FrmBarWnd = PointWnd:GetControl("FRM_RED")
		if FrmBarWnd:IsNil() then return end
		local FrmTextWnd = FrmBarWnd:GetControl("FRM_TEXT")
		if FrmTextWnd:IsNil() then return end
		local AniBarWnd = FrmBarWnd:GetControl("BAR_RED")
		if AniBarWnd:IsNil() then return end
		FrmTextWnd:SetStaticText(Point)
		
		if TotalPointRate <= 20 then
			g_TotalPointWarning_R = true
			g_TotalPointWarning_B = false
		end

		OnSetPvpDamageTwinkle(AniBarWnd, 50, 300)
		AniBarWnd:BarMax(PvPAttrTable["GAMEPOINT"])
		AniBarWnd:BarNow(Point)
	elseif Team == 2 then -- BLUE TEAM
		local FrmBarWnd = PointWnd:GetControl("FRM_BLUE")
		if FrmBarWnd:IsNil() then return end
		local FrmTextWnd = FrmBarWnd:GetControl("FRM_TEXT")
		if FrmTextWnd:IsNil() then return end
		local AniBarWnd = FrmBarWnd:GetControl("BAR_BLUE")
		if AniBarWnd:IsNil() then return end
		FrmTextWnd:SetStaticText(Point)
		
		if TotalPointRate <= 20 then
			g_TotalPointWarning_R = false
			g_TotalPointWarning_B = true
		end
		
		OnSetPvpDamageTwinkle(AniBarWnd, 50, 300)
		AniBarWnd:BarMax(PvPAttrTable["GAMEPOINT"])
		AniBarWnd:BarNow(Point)
	end
end

function SetRoomBaseInfo(Mode, Type, GndNo, Point, Time, MaxUser)
	PvPAttrTable["MODE"] = Mode
	PvPAttrTable["TYPE"] = Type
	PvPAttrTable["GROUNDNO"] = GndNo
	PvPAttrTable["GAMEPOINT"] = Point
	PvPAttrTable["GAMETIME"] = Time
	PvPAttrTable["MAXUSER"] = MaxUser
end

function PlayWarnSoundKTH()
	PlaySound("../Sound/Object_Sound\Macro_Fail02.wav")
end