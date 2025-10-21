
--[[
--///////////////////////////////////////////////////////////////////////	Common YES / NO
function CallGuild_CommonYesNo(iTTW, iCommand)
	if nil == iTTW or nil == iCommand then return end
	local kWnd = CallUI("SFRM_GUILD_MASTER_YES_NO")
	if kWnd:IsNil() then return end
	local kForm = kWnd:GetControl("SFRM_HALLO")
	if kForm:IsNil() then return end
	local kTTW = GetTT(iTTW)
	kForm:SetStaticTextW(kTTW)
	
	kWnd:SetCustomDataAsInt(iCommand)
end

--///////////////////////////////////////////////////////////////////////	CREATE YES / NO
function CallGuild_CreateYesNo(kString)
	if nil == kString then return end
	local kWnd = CallUI("SFRM_GUILD_MASTER_CREATE_YES_NO")
	if kWnd:IsNil() then return end
	local kForm = kWnd:GetControl("SFRM_HALLO")
	if kForm:IsNil() then return end
	--local kTTW = GetTT(iTTW)
	kForm:SetStaticTextW(kString)
end
--///////////////////////////////////////////////////////////////////////	Common
function CallGuild_Command(iTTW)
	if nil == iTTW then return end
	if 0 >= iTTW then return end
	local kWnd = ActivateUI("SFRM_GUILD_MASTER_COMMON")
	if kWnd:IsNil() then return end
	local kForm = kWnd:GetControl("SFRM_HALLO")
	if kForm:IsNil() then return end
	local kTTW = GetTT(iTTW)
	kForm:SetStaticTextW(kTTW)
end
]]--

function CallGuildMemberPopup(kTopWnd)
	local kGuid = kTopWnd:GetOwnerGuid()
	local kMyGuid = g_pilotMan:GetPlayerPilotGuid()
	local kCursorPos = GetCursorPos()
	local bIsMine = kMyGuid:IsEqual(kGuid)
	local kCmdArray = {}
	if not bIsMine then
		kCmdArray[1] = "WHISPER"
		if not IsMyFriend(kGuid) then
			kCmdArray[2] = "ADD_FRIEND"
		end
		kCmdArray[3] = "INVITE_PARTY"
		if not IamHaveCouple() then
			kCmdArray[4] = "REQ_COUPLE"
		end
		if AmIGuildMaster() then
			kCmdArray[5] = "GUILD_KICK"
		end
		if AmIGuildOwner() then
			kCmdArray[6] = "GUILD_CHANGE_OWNER"
			if not IsGuildMaster(kGuid) then
				kCmdArray[7] = "GUILD_SET_MASTER"
			else
				kCmdArray[8] = "GUILD_UNSET_MASTER"
			end
		end
		if CheckExistHaveHome() then
			kCmdArray[10] = "GUILD_MYHOME_INVITE"	
		end
	else
		if not AmIGuildMaster() then
			kCmdArray[9] = "GUILD_LEAVE"
		end
	end

	CallCommonPopup(kCmdArray, kGuid, nil, kCursorPos)
end


function OnBuildGuildEmblemImg(kSelf)
	if kSelf:IsNil() then return end
	local iBuildIndex = kSelf:GetBuildIndex()
	kSelf:SetUVIndex( iBuildIndex+1 )
	kSelf:Visible( iBuildIndex < GetMaxGuildMarkCount() )
end
function OnBuildCanGuildEmblemBtn(kSelf)
	if kSelf:IsNil() then return end
	local iBuildIndex = kSelf:GetBuildIndex()
	kSelf:SetCustomDataAsInt( iBuildIndex )
	kSelf:Visible( iBuildIndex < GetMaxGuildMarkCount() )
end

function OnCashChangeGuildMark(kSelf)
	if kSelf:IsNil() then return end
	local kTopWnd = kSelf:GetParent()
	local kSelectedWnd = kTopWnd:GetControl("IMG_OVER1")
	local iSelectedNo = kSelectedWnd:GetCustomDataAsInt()
	if 0 <= iSelectedNo and GetMaxGuildMarkCount() > iSelectedNo then
		if CallChangeMarkYesNo( kTopWnd, iSelectedNo ) then
			kTopWnd:Close()
		else
			AddWarnDataTT(400838)
		end
	else
		AddWarnDataTT(400838)
	end
end
