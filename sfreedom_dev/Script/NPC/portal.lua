--[[
function NPC_Portal(npc)
	npc = ToActor(npc)
	npc:Talk(100004)
	CallUI("Select_Portal")
end

function Item_Give(npc)
	NPC_Click(npc)
	
	npc = ToActor(npc)
	npc:Talk(100003)
	CallUI("Select_Item")
end

function sb_crystal_portal(npc)
	Net_ReqMapMove(9028100, 1)
end
]]

function CanNpcClickTest()
	if GetEventScriptSystem():IsNowActivate() then return false end
	return true
end

--------------------------------------------------------------------------
function BreakSep(kInput, kSep)
	local kRet = {}
	local iFind = string.find(kInput, "/")
	local iCount = 1
	while nil ~= iFind do
		if nil == iFind then break end
		kRet[iCount] = string.sub(kInput, 1, iFind-1)
		kInput = string.sub(kInput, iFind+1)
		iFind = string.find(kInput, "/")
		iCount = iCount + 1
	end
	if 0 ~= string.len(kInput) then
		kRet[iCount] = kInput
	end
	return kRet
end

--------------------------------------------------------------------------

function NPC_Common(npc)
	if nil == npc then return true end
	local kNpcActor = ToActor(npc);
	if kNpcActor:IsNil() == true then
		return true
	end
	
	if true == IsIamGroundOwner() then
		local kCmdStr = kNpcActor:GetNpcMenuStr()
		if kCmdStr:IsNil() then return true end
		local kCmdArray = BreakSep(kCmdStr:GetStr(), "/")
		if not CanNpcClickTest() then return true end
		
		if false == NPC_CLICK_NEW(kNpcActor, kCmdArray) then return false end
		NPC_SAY(kNpcActor)
		return true
	else
		AddWarnDataTT(70097)
		return false
	end	
end

--[[
--Click Normal NPC
function NPC_Click(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc);
	if kNpcActor:IsNil() == true then
		return false
	end
	
    if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "TALK"}) then return false end
	NPC_SAY(kNpcActor)
	return true
end

--Click Fran NPC
function NPC_FRAN(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc);
	if kNpcActor:IsNil() == true then
		return false
	end
	
	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "FRAN", "TALK"}) then return false end	
	NPC_SAY(kNpcActor)
	return true
end

--Click Guild Manager NPC
function NPC_Guild(kNpc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(kNpc);
	if kNpcActor:IsNil() == true then
		return false
	end
	
	if false == NPC_CLICK_NEW(kNpcActor, {"DAILYQUEST_LV2", "GUILD", "TALK"}) then return false end
	NPC_SAY(kNpcActor)
	return true
end

--Click Shop NPC
function NPC_Click_Shop(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc);
	if kNpcActor:IsNil() == true then
		return false
	end
	
	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "SHOP", "TALK"}) then return false end
	NPC_SAY(kNpcActor)
	return true
end

--Click Enchant NPC
function NPC_Enchant(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end

	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "ENCHANT", "TALK"}) then return false end
	NPC_SAY(kNpcActor)
	return true
end

function NPC_RarityUpgrade(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end

	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "RARITY_UPGRADE", "TALK"}) then return false end
	NPC_SAY(kNpcActor)
	return true
end

--Click Repair NPC
function NPC_ItemRepair(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end
	
	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "REPAIR", "TALK"}) then return false end
	NPC_SAY(kNpcActor)
	return true
end

--Click Repair / Enchant NPC
function NPC_BlackSmith(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end

	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "REPAIR", "ENCHANT", "TALK"}) then return false end
	NPC_SAY(kNpcActor)
	return true
end

--Click Bank NPC
function NPC_InvSafe(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end

	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "INVSAFE", "TALK"}) then return false end
	NPC_SAY(kNpcActor)
	return true
end

--Click ShineStone NPC
function NPC_ShineStone(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end

	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "LOTTERY", "SHINE_UP", "TALK"}) then return false end
	NPC_SAY(kNpcActor)
	return true
end

--Click Cooking NPC
function NPC_Cooking(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end

	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "COOKING", "TALK"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end

--Click Warning Board
function NPC_Warning(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end

	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "WARNING", "TALK"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end

function NPC_CoinChanger(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end

	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "COIN", "TALK"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end

function NPC_CRAFT(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end

	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "TRADE_UNSEALINGSCROLL", "CRAFT", "TALK"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end

function NPC_PostBox(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc);
	if kNpcActor:IsNil() == true then
		return false
	end
	
	if false == NPC_CLICK_NEW(kNpcActor, {"POST"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end

function NPC_Coupon(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc);
	if kNpcActor:IsNil() == true then
		return false
	end

	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "COUPON", "TALK"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end

-- Class Leader
function NPC_SWORD_LEADER(npc) -- sword
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end
	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "SWORD_CLASS_HELP", "SWORD_CP_SHOP", "TALK"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end
function NPC_MAGICIAN_LEADER(npc) -- magician
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end
	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "MAGICIAN_CLASS_HELP", "MAGICIAN_CP_SHOP", "TALK"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end
function NPC_ARCHER_LEADER(npc) -- archer
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end
	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "ARCHER_CLASS_HELP", "ARCHER_CP_SHOP", "TALK"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end
function NPC_THEIF_LEADER(npc) -- theif
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end
	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "THIEF_CLASS_HELP", "THIEF_CP_SHOP", "TALK"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end


-- daily quest NPC
function NPC_DAILYQUEST_LV0(npc) -- daily quest lv 0
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end
	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "DAILYQUEST_LV0", "TALK"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end
function NPC_DAILYQUEST_LV1(npc) -- daily quest lv 1
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end
	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "DAILYQUEST_LV1", "TALK"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end
function NPC_DAILYQUEST_LV2(npc) -- daily quest lv 2
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end
	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "DAILYQUEST_LV2", "TALK"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end

function NPC_SOULTRADER(npc) -- Soul Trader
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc)
	if kNpcActor:IsNil() == true then
		return false
	end
	if false == NPC_CLICK_NEW(kNpcActor, {"QUEST", "TRADE_SOULSTONE", "TALK"})  then return false end
	NPC_SAY(kNpcActor)
	return true
end


-- Emporia administrator
function NPC_EmporiaAdminStrator(npc)
	if not CanNpcClickTest() then return true end
	local kNpcActor = ToActor(npc);
	if nil == kNpcActor or kNpcActor:IsNil() then
		return false
	end
	
	if false == NPC_CLICK_NEW( kNpcActor, {"EM_ADMINISTRATION", "TALK"}) then return false end
	NPC_SAY(kNpcActor)
	return true
end
]]--
--##############################################

--Call Talk Interface
function NPC_SAY(kNpcActor)
	if nil == kNpcActor or kNpcActor:IsNil() then
		return
	end
	if kNpcActor:OnClickSay() then
		--
	else
		local kAction = kNpcActor:GetAction()
		if kAction:IsNil() then 
			kNpcActor:ReserveTransitAction("a_talk")
		else
			if kAction:GetID() ~= "a_talk" then
				kNpcActor:ReserveTransitAction("a_talk")
			end
		end
	end
end

--Call Quest Trigger
function NPC_TRIGGER(kNpcActor)
	local sObjType = 1
	local kGuid = kNpcActor:GetPilotGuid()

	NET_C_M_REQ_TRIGGER(sObjType, kGuid, TRIGGER_ACTION_CLICKED)
end

--Call Shop Interface(base function)
function NPC_Shop(kNpcActorGuid)
	packet = NewPacket(14201)
	packet:PushGuid(kNpcActorGuid)
	packet:PushByte(0)
	Net_Send(packet)
	DeletePacket(packet)
end

function NPC_EmporiaShop(kNpcActorGuid)
	packet = NewPacket(14201)--[[PT_C_M_REQ_STORE_ITEM_LIST]]
	packet:PushGuid(kNpcActorGuid)
	packet:PushByte(3)--[[STORE_TYPE_EMPORIA]]
	Net_Send(packet)
	DeletePacket(packet)
end

function NPC_JobSkillShop(kNpcActorGuid)
	packet = NewPacket(14201)--[[PT_C_M_REQ_STORE_ITEM_LIST]]
	packet:PushGuid(kNpcActorGuid)
	packet:PushByte(4)--[[STORE_TYPE_JOBKSILL]]
	Net_Send(packet)
	DeletePacket(packet)
end

-- ####################### New NPC Click Script #######################
function NPC_CLICK_NEW(kActor, Arg)
	if nil == kActor or kActor:IsNil() then return end

	-- 안전거품 NPC 사용 금지
	if g_pilotMan:IsLockMyInput() == true then
		return
	end	
	
	local MyActor = GetMyActor()
	if MyActor:IsNil() then return end
	if false == CheckActorDist(kActor, MyActor, 90) then return false end
	
	local kGuid = kActor:GetPilotGuid()
	local kArg, iCount = CheckPopupCommand(kActor, Arg)--Check
	if 1 < iCount then
		CallCommonPopup(kArg, kGuid, kActor)
	elseif 1 == iCount then
		for kKey, kVal in pairs(kArg) do
			ProcessCommonPopupMessageCmdName(kVal, kGuid)
		end
	end

	return true
end

function CheckActorDist(kFrom, kTo, fDist)	--두 액터 사이의 길이를 재자
	if nil == kFrom or kFrom:IsNil() then return false end
	if nil == kTo or kTo:IsNil() then return false end
	if fDist==0 then return true end
	if fDist<0 then
		fDist = math.abs(fDist)
	end

	local kFromPos = kFrom:GetPos()
	local kToPos = kTo:GetPos()

	local fNowDist = kFromPos:Distance(kToPos)
	fNowDist = math.abs(fNowDist)
	ODS("측정거리 : " .. fDist .. " 현재거리 : " .. fNowDist .. "\n") 

	return fDist >= fNowDist
end
