---- lua_tinker���� ���� �޼����� ����� �� ����ϴ� �Լ�
function _ALERT(msg)
	MessageBox(msg, "lua error")
end

-- Checker
function LoadingChecker()
	return false
end

-- Dummy for galaxy map
function UpdateProgressMap(kSelf)
end

-- new UI
function UI_T_Y_ToCenter(wnd)
	if wnd:IsNil() == true then return end
	local text = wnd:GetStaticText()
	if nil==text or text:IsNil() then
		return
	end
	local kTextPos = wnd:GetTextPos()
	local kTxtSize = wnd:GetTextSize()
	kTextPos:SetY( (wnd:GetSize():GetY() - kTxtSize:GetY())/2 )
	wnd:SetTextPos( kTextPos )
end
---- Include 
--
-- System
DoFile("constant.lua")
DoFile("SystemDefinition.lua")

DoFile("world.lua")
DoFile("actor.lua")
DoFile("CSCommon.lua")
-- Actions and Skills
DoFile("action.lua")
DoFile("skill.lua")
DoFile("trigger.lua")
DoFile("ActionEffect.lua")
DoFile("worldaction.lua")
-- Projectile
DoFile("projectile.lua")

-- EventScript
DoFile("EventScript.lua")

-- Network packet handler
DoFile("Net/net_basic.lua")
DoFile("Net/net.lua")
DoFile("Net/god_cmd.lua")

-- UI
DoFile("UI/Common.lua")
DoFile("UI/WorldMap.lua")
DoFile("UI/ui.lua")
DoFile("UI/inv.lua")
DoFile("UI/MiningTimer.lua")
DoFile("UI/loading.lua")
DoFile("UI/logo.lua")
UI_ColorSet( 0 )
DoFile("UI/MainUI.lua")
DoFile("UI/SkillWindow.lua")
DoFile("UI/QuickSlot.lua")
DoFile("UI/Option.lua")
DoFile("UI/Community.lua")
DoFile("UI/Guild.lua")
DoFile("UI/QuestUI.lua")
DoFile("UI/SelectMapUI.lua")
UI_ColorSet(Config_GetValue(HEADKEY_OPTION, SUBKEY_THEME_COLOR_SET))
DoFile("UI/PetitionUI.lua")
DoFile("UI/MapNetwork.lua")
DoFile("UI/CommonTalkMenu.lua")
DoFile("UI/Interface.lua")
-- NPC
DoFile("NPC/npc.lua")

-- Actor
DoFile("Actor/pickup.lua")

DoFile("UI/PvPUI.lua")
DoFile("World/myhome.lua")

--Helper
DoFile("Helper.lua")

-- Checker
function LoadingChecker()
	return true
end
-- init_single������ �̰� ���� �Ѵ�.
SetAddUnitThread(false)


-- ĳ���� �߰��� AddChar�� ���ø� �˴ϴ�. 
-- class, Gender, �׸��� �� ĳ������ ���ڸ� �ֽø� �˴ϴ�.
-- ����� ���ٰ�, ���縦 �߰� �Ͻø鼭 SetPC���ڸ� True�� �ֽø�
-- ���簡 �߰��Ǹ鼭 ���縦 ������ �� �ֽ��ϴ�.
function AddChar(class, gender, setPC)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	x_diff = x_diff + 10;
	local guid = GUID("123")
	guid:Generate()
	local pilot = g_pilotMan:NewPilot(guid, class, gender)
	local spawnLoc = g_world:FindSpawnLoc("char_spawn_1")
	spawnLoc:SetX(spawnLoc:GetX() + x_diff)
	spawnLoc:SetY(spawnLoc:GetY() + y_diff)
	g_world:AddActor(guid, pilot:GetActor(), spawnLoc, 1)
	pilot:GetActor():ClearActionState()
	pilot:SetAbil(AT_MAX_HP, 10000)
	pilot:SetAbil(AT_HP, 10000)

	if setPC == true then
		g_playerInfo.guidPilot = guid
		g_world:SetPlayer(pilot:GetActor())
		g_world:SetCameraMode(1, pilot:GetActor())
		pilot:ReanalyseSkillTree();
		pilot:SetAbil(AT_C_HIT_SUCCESS_VALUE,100100);
		pilot:SetAbil(AT_HP, 10000)
		pilot:SetAbil(AT_C_ATTACK_SPEED,10000)
	end

	local actor = pilot:GetActor()
	actor:LockBidirection(false)
	actor:AddToDefaultItem(0, 20200086)	-- HairColor

	if gender == 1 then
		actor:AddToDefaultItem(1, 20000001)	-- HairStyle
		actor:AddToDefaultItem(2, 20001001)	-- Face
	elseif gender == 2 then
		actor:AddToDefaultItem(1, 20000004)	-- HairStyle
		actor:AddToDefaultItem(2, 20001002)	-- Face
	end

	if class == CT_FIGHTER or class == CT_KNIGHT then
	
		actor:AddToDefaultItem(21, 31000010)	-- ����
		actor:AddToDefaultItem(22, 31000020) -- ����
		actor:AddToDefaultItem(24, 31000030) -- �尩
		actor:AddToDefaultItem(23, 31000040)	-- �Ź�
		actor:AddToDefaultItem(6, 11000280)

	elseif class == CT_MAGICIAN or class == CT_MAGE then
	
		actor:AddToDefaultItem(21, 33000010)	-- Jacket
		actor:AddToDefaultItem(22, 33000020) -- Gloves
		actor:AddToDefaultItem(23, 33000040) -- Pantes
		actor:AddToDefaultItem(24, 33000030)	-- Shoes
		actor:AddToDefaultItem(6, 12000260)

		
	elseif class == CT_ARCHER then
	
		actor:AddToDefaultItem(21, 33200010)	-- Jacket
		actor:AddToDefaultItem(22, 33500010) -- Gloves
		actor:AddToDefaultItem(23, 33300010) -- Pantes
		actor:AddToDefaultItem(24, 33600020)	-- Shoes
		actor:AddToDefaultItem(6, 13000260)
		
	elseif class == CT_THIEF or class == CT_CLOWN then
	
		actor:AddToDefaultItem(21, 34200010)	-- Jacket
		actor:AddToDefaultItem(22, 34500010) -- Gloves
		actor:AddToDefaultItem(23, 34300010) -- Pantes
		actor:AddToDefaultItem(24, 34600020)	-- Shoes
		actor:AddToDefaultItem(6, 14000260)
		
	elseif class == CT_WARRIOR then
	
		actor:AddToDefaultItem(21, 31200010)	-- Jacket
		actor:AddToDefaultItem(22, 31500010) -- Gloves
		actor:AddToDefaultItem(23, 31300010) -- Pantes
		actor:AddToDefaultItem(24, 31600020)	-- Shoes
		actor:AddToDefaultItem(6, 11000260)	--	 Twohand Sword
		
	elseif class == CT_BATTLEMAGE then
	
		actor:AddToDefaultItem(21, 31200010)	-- Jacket
		actor:AddToDefaultItem(22, 31500010) -- Gloves
		actor:AddToDefaultItem(23, 31300010) -- Pantes
		actor:AddToDefaultItem(24, 31600020)	-- Shoes
		actor:AddToDefaultItem(6, 11000260)

	elseif class == CT_RANGER then
	
		actor:AddToDefaultItem(21, 31200010)	-- Jacket
		actor:AddToDefaultItem(22, 31500010) -- Gloves
		actor:AddToDefaultItem(23, 31300010) -- Pantes
		actor:AddToDefaultItem(24, 31600020)	-- Shoes
		actor:AddToDefaultItem(6, 11000260)
	elseif class == CT_ASSASSIN then
	
		actor:AddToDefaultItem(21, 31200010)	-- Jacket
		actor:AddToDefaultItem(22, 31500010) -- Gloves
		actor:AddToDefaultItem(23, 31300010) -- Pantes
		actor:AddToDefaultItem(24, 31600020)	-- Shoes
		actor:AddToDefaultItem(6, 11000260)
	end
	
	pilot:GetActor():EquipAllItem()

	actor:ReserveTransitAction("a_idle");
	g_actor = actor
end

function EquipItem(pos, num)
	if( CheckNil(g_actor == nil) ) then return false end
	g_actor:AddToDefaultItem(pos, num)
	g_actor:EquipAllItem()
end


function AddPuppet()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local guid = GUID("1")
	guid:Generate()
	local pos = GetPlayer():GetTranslate()
	local quat = Quaternion(0,Point3(0,0,0))
	puppet = g_world:AddPuppet(guid, "p_5", pos, quat)
end

-- ���� ���� ����
g_bAddedUIScene = false
g_Mon = nil
g_actor = nil
g_mapNo = 0
g_renderMan = RenderMan()
g_pilotMan = GetPilotMan()
g_particleMan = GetParticleMan()
g_cameraTrn = nil
g_cameraRot = nil
SetSingleMode(true);
g_ChatMgrClient = GetChatMgrClient()
g_FriendMgr = GetFriendMgr()
g_firstMapName = "w_test"
g_lastCharGender = 1
g_lastCharClass = 1
g_Petition = GetPetitionUI()
g_MovieMgr = GetMovieMgr()

Init_LogWnd_FilterSet();

-- �׽�Ʈ�� ���带 �ε��Ѵ�.
g_renderMan:AddScene("s_login")

function AddMonster(monNo, setmyPC, x, y, z)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	--x_diff = x_diff+10;
	local guid = GUID("123")
	guid:Generate()
	--local pos = g_world:FindSpawnLoc("char_spawn_1")
	local pos = GetPlayer():GetPos()
	--pos:SetX(pos:GetX() )
	--pos:SetY(pos:GetY() )
	if x ~= nil and y ~= nil and z ~= nil then
		pos:SetX(x)
		pos:SetY(y)
		pos:SetZ(z)
	end

	local pilot = g_pilotMan:NewPilot(guid, monNo, 0)
	if pilot:IsNil() == false then
		g_world:AddActor(guid, pilot:GetActor(), pos, 2)
		pilot:GetActor():ReserveTransitAction("a_opening")
		pilot:GetActor():ClearActionState()
		if setmyPC == true then
			g_playerInfo.guidPilot = guid
			g_world:SetPlayer(pilot:GetActor())
			g_world:SetCameraMode(1, pilot:GetActor())
		end
	
		pilot:SetAbil(AT_HP, 1000000);
		pilot:SetAbil(AT_UNIT_SIZE,2);
		pilot:SetAbil(AT_DAMAGEACTION_TYPE,0);
		pilot:SetAbil(AT_NOT_SEE_PLAYER, 0)
		pilot:SetAbil(AT_DAMAGEACTION_TYPE, 0)

		g_Mon = pilot:GetActor()
	end
end

g_BreakableObjectGroupGUID = GUID("123")
g_BreakableObjectGroupGUID:Generate()

g_VerticalLoc = 0

function AddRidingObject(ObjNo, setmyPC, x, y, z)

	if ObjNo == nil then
		ObjNo = 5000401
	end

	--x_diff = x_diff+10;
	local guid = GUID("123")
	guid:Generate()
	--local pos = g_world:FindSpawnLoc("char_spawn_1")
	local pos = GetPlayer():GetPos()
	--pos:SetX(pos:GetX() )
	--pos:SetY(pos:GetY() )
	if x ~= nil and y ~= nil and z ~= nil then
		pos:SetX(x)
		pos:SetY(y)
		pos:SetZ(z)
	end

	local pilot = g_pilotMan:NewPilot(guid, ObjNo, 0,"OBJECT",2)
	if pilot:IsNil() == false then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, pilot:GetActor(), pos, 11)
		g_RidingObj = pilot:GetActor();
		g_RidingObj:ReserveTransitAction("a_ridingobject_move");
	end
end
function AddObject(ObjNo, setmyPC, x, y, z)
	--x_diff = x_diff+10;
	local guid = GUID("123")
	guid:Generate()
	--local pos = g_world:FindSpawnLoc("char_spawn_1")
	local pos = GetPlayer():GetPos()
	--pos:SetX(pos:GetX() )
	--pos:SetY(pos:GetY() )
	if x ~= nil and y ~= nil and z ~= nil then
		pos:SetX(x)
		pos:SetY(y)
		pos:SetZ(z)
	end

	local pilot = g_pilotMan:NewPilot(guid, ObjNo, 0,"OBJECT")
	if pilot:IsNil() == false then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, pilot:GetActor(), pos, 11)
		g_Obj = pilot:GetObject()
		g_Obj:SetParentGroupGUID(g_BreakableObjectGroupGUID)
		g_Obj:SetVerticalLocation(g_VerticalLoc);
		
		g_VerticalLoc = g_VerticalLoc+1;
	end
end
function PrefMapMove()
	local kActor = GetPlayer()
	g_lastCharGender = kActor:GetAbil(AT_GENDER)
	g_lastCharClass = kActor:GetAbil(AT_CLASS)

	if g_lastCharGender == 0 then
		g_lastCharGender = 1
	end

	if g_lastCharClass == 0 then
		g_lastCharClass = 1
	end

	StopBgSound()
	g_renderMan:RemoveAllScene("UIScene")
	
	if g_bAddedUIScene == false then
		g_renderMan:AddScene("UIScene")
		g_bAddedUIScene = true
	end	

	g_renderMan:InvalidateSceneContainer()	-- UIScene�� ��� �߰��ؾ���.

	--CloseUI("FRM_MINIMAP")
	CallUI("FRM_LOADING_IMG")
	UI_SetLoadingProgress(20)
end

-- Post MapMove
function PostMapMove()
	if g_world:IsNil() == true then
		g_world = ToWorld(g_renderMan:AddScene(g_firstMapName))
	end
	
	if g_mapNo == 9991001 then
		g_world:SetAttr(GATTR_MYHOME)
	end
	
	g_worldID = g_world:GetID()
	UI_SetLoadingProgress(60)

	AddChar(g_lastCharClass, g_lastCharGender, true)

	if g_mapNo == 9020200 then
		AddMonster(200591, false)
		g_Mon:FreeMove(true)
		g_Mon:SetTranslate(Point3(0, 0, 0))
	end
	
	UI_SetLoadingProgress(100)

	CloseUI("FRM_LOADING_IMG")
		
	-- Game UI
	ActivateUI("Cursor")
	ActivateUI("ChatBar")		
	PlayBgSound(0)
	ActivateUI("FRM_MINIMAP")
	
	if g_bMovingToSameMap == false then
	
		--	�� �̵��� �߻��Ǵ� ���� �ý��� ȣ��
		GetHelpSystem():ActivateByConditionString("MapMove",g_world:GetID());
		--	�� �̵��� �߻��Ǵ� �̺�Ʈ ��ũ��Ʈ ȣ��
		if true == g_world:CheckEventScriptOnEnter() then
			GetEventScriptSystem():ActivateEvent(g_world:GetEventScriptIDOnEnter(),g_world:GetAccumTime(),0.1);
		end
	
	end
end

-- �̱� ��� ���� �� �̵�.
function ReqMapMove(mapNo)
	g_mapNo = mapNo
	PrefMapMove()
	-- ĳ���͵��� �ʿ� �߰��Ѵ�.
 	g_world = ToWorld(g_renderMan:AddSceneByNo(mapNo))
	PostMapMove()
end

-- �̱� ��� ���� �� �̵�(�� �̸�����)
function ReqMapMoveByName(mapName)	
	PrefMapMove()
	-- ĳ���͵��� �ʿ� �߰��Ѵ�.
 	g_world = ToWorld(g_renderMan:AddScene(mapName))
	PostMapMove()
end

-- ResetMap
function Re()
	PrefMapMove()
 	g_world = ToWorld(g_renderMan:AddScene(g_firstMapName))
	PostMapMove()
end

function UI_endLogo()
	CloseUI("LoginBg")
	CloseUI("LoginDlg")
	CloseUI("AuthFailDlg")
	CloseUI("FRM_LOGO_IMG")
	CloseUI("FRM_MINIMAP")
	CallUI("Cursor")
	CallUI("ChatBar")
	CallUI("QuickMenu")
	CallUI("SKILL_MAINTENANCETIME_BAR")
	CallUI("SKILL_CASTTIME_BAR")
	--CallUI("FRM_SHINESTONE_MAIN")

	g_world = ToWorld(g_renderMan:AddScene(g_firstMapName))
	g_playerInfo = {}

	g_ChatMgrClient = GetChatMgrClient()

	-- �߰� ���
	y_diff = -20
	x_diff = 20	

	--guid = GUID("123")
	--guid:Generate()
	--pilot = g_pilotMan:NewPilot(guid, 200023, 0)
	--spawnLoc = g_world:FindSpawnLoc("char_spawn_1")
	--spawnLoc:SetX(spawnLoc:GetX() + x_diff)
	--g_world:AddActor(guid, pilot:GetActor(), spawnLoc, 1)
	--pilot:GetActor():ClearActionState()
	--g_playerInfo.guidPilot = guid
	--g_world:SetPlayer(pilot:GetActor())
	--g_world:SetCameraMode(1, pilot:GetActor())

	-- ����
	--AddChar(1, 1, true)		-- ����
	--AddChar(2, 1, true)	-- ����
	--GetPlayer():GetPilot():SetAbil(AT_MP, 100)
	--AddChar(3, 1, true)	-- �ü�
	--AddChar(4, 1, true)	-- ����

	-- ����
	--AddChar(1, 2, false)	-- ����
	--AddChar(2, 2, false)	-- ����
	--AddChar(3, 2, false)	-- �ü�
	--AddChar(4, 2, false)	-- ����
	


	--ActivateUI("FRM_MINIMAP")
	PostMapMove()
end

function	AddCharRandom(iCount)

	local	kMyActor = GetMyActor();

	while	iCount>0 do

		local	iGender = math.random(1,2);
		local	iClass = math.random(1,4);
		local	kGuid = GUID("123");
		kGuid:Generate();
		
		local	ptSpawnLoc = kMyActor:GetPos();
		
		ptSpawnLoc:SetX(ptSpawnLoc:GetX()+math.random(-100,100));
		local	kPilot = g_pilotMan:NewPilot(kGuid,iClass,iGender);
		
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(kGuid,kPilot:GetActor(),ptSpawnLoc,1);
		kPilot:GetActor():ClearActionState();
		
		local	kActor = kPilot:GetActor();
		
		kActor:AddToDefaultItem(1, GetRandomItemNo(iClass,iGender,1))	-- HairStyle
		kActor:AddToDefaultItem(2, GetRandomItemNo(iClass,iGender,2))	-- HairStyle
		kActor:AddToDefaultItem(3, GetRandomItemNo(iClass,iGender,3))	-- HairStyle
		kActor:AddToDefaultItem(4, GetRandomItemNo(iClass,iGender,4))	-- HairStyle
		kActor:AddToDefaultItem(6, GetRandomItemNo(iClass,iGender,6))	-- HairStyle
		kActor:AddToDefaultItem(20, GetRandomItemNo(iClass,iGender,20))	-- HairStyle
		kActor:AddToDefaultItem(21, GetRandomItemNo(iClass,iGender,21))	-- HairStyle
		kActor:AddToDefaultItem(22, GetRandomItemNo(iClass,iGender,22))	-- HairStyle
		kActor:AddToDefaultItem(23, GetRandomItemNo(iClass,iGender,23))	-- HairStyle
		kActor:AddToDefaultItem(24, GetRandomItemNo(iClass,iGender,24))	-- HairStyle
	
		kPilot:GetActor():EquipAllItem()
		local kAction = kPilot:GetActor():ReserveTransitAction("a_idle");
		kAction:SetParamInt(16,1);
		
		iCount=iCount-1;
	end
	
end

function AddChars(iClassNo,iSex,iCount)

	local	fStart = GetCurrentTimeInSec()
	
	local	i = 0;
	while i<iCount do
	
		AddChar(iClassNo,iSex,false)
		
		i=i+1;
	end
	
	local	fElapsed = GetCurrentTimeInSec() - fStart;
	
	MessageBox("ElapsedTime : "..fElapsed,"TimeCheck");
	
end

function AddMonsters(iClassNo,iCount)

	local	i = 0
	while i< iCount do
		AddMonster(iClassNo)
		i=i+1
	end
end

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
	if kMyActor:IsNil() then
		return
	end
	
	if iEffectID == nil then
		return
	end
		
	local	kStatusEffectMan = GetStatusEffectMan();
	kStatusEffectMan:RemoveStatusEffect(kMyActor:GetPilotGuid(), iEffectID);
	
end
function AddStatusEffectName(kEffectXMLName,iValue)

	local	kMyActor = GetMyActor();
	if kMyActor:IsNil() then
		return
	end
	
	if iValue == nil then
		iValue = 0
	end
	
	if kEffectXMLName == nil then
		return
	end
	
	local	kStatusEffectMan = GetStatusEffectMan();
	kStatusEffectMan:AddStatusEffectToActor(kMyActor:GetPilotGuid(),kEffectXMLName,0, iValue)

end
function RemoveStatusEffectName()

	local	kMyActor = GetMyActor();
	if kMyActor:IsNil() then
		return
	end
		
	local	kStatusEffectMan = GetStatusEffectMan();
	kStatusEffectMan:RemoveStatusEffectFromActor(kMyActor:GetPilotGuid(), 0);
	
end
function NewCombo(kActionID)

    local   kComboAdvisor = GetComboAdvisor();
    kComboAdvisor:OnNewActionEnter(kActionID);

end
function ResetCombo()
    local   kComboAdvisor = GetComboAdvisor();
    kComboAdvisor:ResetComboAdvisor();
end

function PlayActionID(kActionID)

  	local	kMyActor = GetMyActor();
	if kMyActor:IsNil() then
		return
	end  
	
	kMyActor:ReserveTransitAction(kActionID);

end
function MonPlayActionID(kActionID)

  	local	kMyActor = g_Mon;
	if kMyActor == nil or kMyActor:IsNil() then
		return
	end  
	
	kMyActor:LockBidirection(false);
	kMyActor:LookAt(GetMyActor():GetPos(),true,false);
	kMyActor:LockBidirection(true);
		
	kAction = kMyActor:ReserveTransitAction(kActionID);
	kAction:SetActionParam(AP_CASTING)

end
function MonPlayActionNo(kActionNo)

  	local	kMyActor = g_Mon;
	if kMyActor == nil or kMyActor:IsNil() then
		return
	end  
	
	kMyActor:LockBidirection(false);
	kMyActor:LookAt(GetMyActor():GetPos(),true,false);
	kMyActor:LockBidirection(true);
		
	kAction = kMyActor:ReserveTransitActionByActionNo(kActionNo);
	kAction:SetActionParam(AP_CASTING)

end

function Singha()

	local guid = GUID("123")

	guid:Generate()


	local Pos = GetPlayer():GetTranslate()
	local kMovingDir = GetPlayer():GetMovingDir()
	kMovingDir:Multiply(50)
	Pos:Add(kMovingDir)
	
	local pilot = g_pilotMan:NewPilot(guid, 900004, 0)

	if pilot:IsNil() == false then
		local kActor = pilot:GetActor()
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, kActor, Pos, 6)

		kActor:TransitAction("a_Brother is Return_Play")

		kActor:ClearActionState()

	end
end

function AddChat(string,iCount)

	local i =0 
	local fStartTime = GetCurrentTimeInSec();
	while i<iCount do
	
		g_ChatMgrClient:AddLogMessage(WideString(string));
	
		i=i+1
	end
	
	local fElapsedTime = GetCurrentTimeInSec() - fStartTime;
	ODS("[Chatting Test]Total Elaped Time : "..fElapsedTime.."\n");
	

end

function ParticleTest(flag)
	local i = 0

	while i < 2 do
		GetPlayer():AttachParticle(math.random(1,10000), "char_root", "ef_bang")
		i = i + 1
	end
end

function DropboxTest(boxNo, itemNo)
	local guid = GUID("123")

	guid:Generate()


	local Pos = GetPlayer():GetTranslate()
--	local kMovingDir = GetPlayer():GetMovingDir()
--	kMovingDir:Multiply(50)
--	Pos:Add(kMovingDir)

	local pilot = g_pilotMan:NewPilot(guid, boxNo, 0)

	if pilot:IsNil() == false then
		local dropbox = PilotToDropBox(pilot)
		if dropbox:IsNil() == false then
			if itemNo == 0 then
				itemNo = 20101010
			end
			dropbox:SetItemNum(itemNo)
			dropbox:SetGuid(guid)
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			g_world:AddDropBox(guid, dropbox, Pos)

			local Oguid = GUID("123")
			Oguid:Generate()
			dropbox:SetOfferer(Oguid, Pos)
		end
--		local kActor = pilot:GetActor()
--		if kActor:IsNil() == false then
--			g_world:AddActor(guid, kActor, Pos, 3)
--		end

	end	
end

function Net_HomeArrangeFurniture(world)
	home = world:GetHome()
	if home:IsNil() then
		return true
	end

	home:SetArrangeMode(false)
end


cur_furniture = 1
furniturepath = {}
furniturepath[1] = "../XML/Item/5_Furniture/table.xml"
furniturepath[2] = "../XML/Item/5_Furniture/window.xml"
furniturepath[3] = "../XML/Item/5_Furniture/light.xml"
furniturepath[4] = "../XML/Item/5_Furniture/arc_desk.xml"
furniturepath[5] = "../XML/Item/5_Furniture/sofa.xml"
furniturepath[6] = "../XML/Item/5_Furniture/chest.xml"

function Net_HomeToArrangeMode(world)
	local home = world:GetHome()
	if home:IsNil() then
		return true
	end

	furniture = NewFurnitureFromFile(furniturepath[cur_furniture])
	cur_furniture = cur_furniture + 1
	if cur_furniture > 6 then
		cur_furniture = 1
	end

	home:SetArrangingFurniture(furniture)
	home:SetArrangeMode(true)
end

function Net_HomeRotateFurniture(world)
	home = world:GetHome()
	if home:IsNil() then
		return true
	end

	if not home:IsArrangeMode() then
		return true
	end

	furniture = home:GetArrangingFurniture()
	if furniture:IsNil() then
		return true
	end

	furniture:IncRotate(math.pi / 4)
end

function SelfReload()
	g_reloadSelf = true 
	DoFile("ReloadScript_F9.lua")
	g_reloadSelf = false
end

function Test1(iNum)

	local	iResult=0;
	local	i=0;
	while i<iNum do
		iResult = iResult+(i+1);
		i=i+1;
	end

end

--�̱� ��忡�� Ʈ���� �̸��� �������� ���� ����
g_ShowTriggerTitleText = true

function ShowTriggerTitleText()
	return g_ShowTriggerTitleText;
end

function SinglePacketHandle(wType, packet)
	ODS("Packet recv: " .. wType .. "\n")
end

function EnableShortCutKey()
	return true
end