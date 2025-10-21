-- System
DoFile("world.lua")

DoFile("CSCommon.lua")

-- Actions and Skills
DoFile("action.lua")
DoFile("skill.lua")
DoFile("worldaction.lua")
DoFile("ActionEffect.lua")
DoFile("trigger.lua")
DoFile("StatusEffect.lua")
-- Projectile
DoFile("projectile.lua")

-- EventScript
DoFile("EventScript.lua")

-- Network packet handler
DoFile("Net/net.lua")
DoFile("Net/login.lua")

-- Packet Handler
DoFile("Net/PvP.lua")	-- PvP 관련 패킷 및 함수
DoFile("Net/pet.lua")		-- 펫 관련 패킷
DoFile("Net/indun.lua")	-- 인던 관련 패킷
DoFile("Net/item.lua")	-- 아이템 관련 패킷
DoFile("Net/god_cmd.lua")	-- 갓 명령들
DoFile("Net/Mission.lua")	-- 미션 명령
DoFile("Net/emporia.lua")	-- 엠포리아 

-- UI
DoFile("UI/Common.lua")
DoFile("UI/Interface.lua")
DoFile("UI/MainUI.lua")
DoFile("UI/SkillWindow.lua")
DoFile("UI/PetSkillWindow.lua")
DoFile("UI/QuickSlot.lua")
DoFile("UI/MyQuestListWindow.lua")
DoFile("UI/PvPUI.lua")
DoFile("UI/duel.lua")
DoFile("UI/Community.lua")
DoFile("UI/Party.lua")
DoFile("UI/Guild.lua")
DoFile("UI/Option.lua")
DoFile("UI/QuestUI.lua")
DoFile("UI/SelectMapUI.lua")
DoFile("UI/PetitionUI.lua")
DoFile("UI/ItemPlusUpgrade.lua")
DoFile("UI/ItemRarityUpgrade.lua")
DoFile("UI/WorldMap.lua")
DoFile("UI/MapNetwork.lua")
DoFile("UI/HelpSystem.lua")
DoFile("UI/Couple.lua")
DoFile("UI/MyHome.lua")
DoFile("UI/Auction.lua")
DoFile("UI/CommonTalkMenu.lua")
DoFile("UI/CashShopUI.lua")
DoFile("UI/Emporia.lua")
DoFile("UI/ItemBind.lua")
DoFile("UI/PetUI.lua")
DoFile("UI/Wedding.lua")
DoFile("UI/BattleSquare.lua")
DoFile("UI/Event.lua")
DoFile("UI/TransTower.lua")
DoFile("UI/HardCoreDungeon.lua")	-- HardCoreDungeon
DoFile("UI/Post.lua")
DoFile("UI/TrueTW.lua")
DoFile("UI/AlramMission.lua")
DoFile("UI/SuperGround.lua")
DoFile("UI/CashItem.lua")
DoFile("UI/JobSkill.lua")
DoFile("UI/CustomUI.lua")
DoFile("UI/Manufacture.lua")
DoFile("UI/RunningEvent.lua")
DoFile("UI/Constellation.lua")
DoFile("UI/MutatorUI.lua")

-- NPC
DoFile("NPC/npc.lua")

-- Actor
DoFile("Actor/pickup.lua")
DoFile("Actor/MouseAction.lua")

-- Trade
DoFile("Net/Trade.lua")

-- Mail
DoFile("Mail/MailTest.lua")

-- UserMarket
DoFile("UserMarket/MarketTest.lua")

-- ItemRarityUpgrade
DoFile("ItemRarityUpgrade/itemrarityupgrade.lua")

-- systeminventory
DoFile("SystemInv/testsysteminv.lua")

-- Actor Mob Base
DoFile("Actor/Mob_Base.lua") --펫 소지하고 처음 게임 로딩시 Actor_Mob_Base 에러가 발생. 스레딩 관련 우선 순위 문제가 있어 미리 선언하기로 함.


DoFile("World/myhome.lua")

DoFile("World/95_Dungeon/010801_DG.lua")

-- Checker
function LoadingChecker()
	return true
end

-- 전역 변수 설정
g_ChatMgrClient = GetChatMgrClient()
g_FriendMgr = GetFriendMgr()
g_Petition = GetPetitionUI()


--chat blocking
--g_ChatMgrClient:LimitInput(0.77)--2. 이번으로 포함하여 iLimitCount+1번을 fLimitInput초 안에 입력하면 금지
--g_ChatMgrClient:LimitPrevInput(0.21)--1. 바로 이전과 비교해서 LimitPrevInput 보다 작으면 금지
--g_ChatMgrClient:SameInput(1.17)--3. 이전 fSameInput초 안에 같은 입력이 iLimitInput개 이상이면 금지
--g_ChatMgrClient:LimitCount(2)--현재를 뺀 검사할 대상의 갯수

function InitBlockSpamChat()
	g_ChatMgrClient:SetMaxInputLog(32)		--입력한 채팅로그를 몇개까지 저장 할 것 인가
	g_ChatMgrClient:SetMaxBlockTime(120) 	-- 도배로 인한 최대 채팅 금지 시간	
	g_ChatMgrClient:SetSpamChkFlag(12)		-- FLAG 값 1:연속 입력, 2:느슨한 연속입력 , 4: 일정 간격 입력, 8: 같은값 연속 입력

	--1:연속 입력 설정
	g_ChatMgrClient:SetBlockSpamChat_ChainInput(1, 2)		  -- T 초 이내 채팅 입력하면 도배 판단(T,fBlockTime)
	--2:느슨한 연속입력 설정
	g_ChatMgrClient:SetBlockSpamChat_LooseChainInput(3, 2, 5) -- T초 이내 X번 채팅 입력 하면 도배 판단(T,X,fBlockTime)
	--4: 일정 간격 입력 설정
	g_ChatMgrClient:SetBlockSpamChat_MacroInput(5, 1, 6)	  -- 일정간격(자동계산) 마다(오차 R초) 같은문장이 X번 입력되면 도배 판단(X, R, fBlockTime)
	--8: 같은값 연속 입력 설정
	g_ChatMgrClient:SetBlockSpamChat_SameInput(10, 4, 30)		  -- T초이내 같은 문장이 연속으로 X번 입력하면 도배(T, X, fBlockTime)
end
InitBlockSpamChat()

--Notice Level color set
--Lua Hex code bug
g_ChatMgrClient:NoticeLevelColor_Clear()
g_ChatMgrClient:NoticeLevelColor_Add(0, 4294965588, 0xFF000000, false)--Level Default (0xFFFFF954, 0xFF000000)
g_ChatMgrClient:NoticeLevelColor_Add(1, 4294912559, 0xFF000000, true )--Level 1 (0xFFFF2A2F, 0xFF000000)
g_ChatMgrClient:NoticeLevelColor_Add(2, 4278321070, 0xFF000000, false)--Level 2 (0xFF01FFAE, 0xFF000000)
g_ChatMgrClient:NoticeLevelColor_Add(3, 4294366582, 0xFF000000, true )--Level 3 PvP (0xFFF6D576, 0xFF000000)
g_ChatMgrClient:NoticeLevelColor_Add(4, 4294945076, 0xFF000000, true )--Level 4 GM (0xFFFFA934, 0xFF000000)
g_ChatMgrClient:NoticeLevelColor_Add(5, 4278255360, 0xFF000000, true )--Level 5 중요정보 갱신
g_ChatMgrClient:NoticeLevelColor_Add(6, 4282841343, 0xFF000000, true )--Level 6 유저 관계된 공지
g_ChatMgrClient:NoticeLevelColor_Add(7, 4294967040, 0xFF000000, true )--Level 7 정보갱신, 유저에게 알림
g_ChatMgrClient:NoticeLevelColor_Add(8, 4286631679, 0xFF000000, false )--Level 8 레벨업 알림

--Init Log Window filter set
Init_LogWnd_FilterSet()
Init_ChatFilter()

function AddStatusEffect(iEffectID,iValue)

	local	kMyActor = GetMyActor();
	if kMyActor:IsNil() then
		return
	end
	
	if iValue == nil then
		iValue = 0
	end
	
	if iEffectID == nil then
		return
	end
	
	local	kStatusEffectMan = GetStatusEffectMan();
	kStatusEffectMan:AddStatusEffect(kMyActor:GetPilotGuid(),kMyActor:GetPilotGuid(),0, iEffectID, iValue)

end

function RemoveStatusEffect(iEffectID)

	local	kMyActor = GetMyActor();
	if kMyActor:IsNil()  then
		return
	end
	
	if iEffectID == nil then
		return
	end
		
	local	kStatusEffectMan = GetStatusEffectMan();
	kStatusEffectMan:RemoveStatusEffect(kMyActor:GetPilotGuid(), iEffectID);
	
end

function InitChargeKey()
	KEY_CHARGE = 1057
	KEY_CHARGING_UKEY = 1057
end
InitChargeKey() --Initialize