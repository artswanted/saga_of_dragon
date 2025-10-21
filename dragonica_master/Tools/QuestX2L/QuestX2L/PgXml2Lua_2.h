#pragma once

/*
//XML Type Parameter
//SArgument(ID, OBJECTNO, TYPE, VALUE)
const TCHAR* DEPEND_MON_HUNT = L"HUNT";
const TCHAR* DEPEND_MON_DROP = L"DROP";				//"100/30/3/1/100000"
const TCHAR* DEPEND_MON_INST_DROP = L"INST_DROP";		//"100/30/3/1/100000"

//SArgument(ID, OBJECTNO, TYPE, VALUE)
const TCHAR* DEPEND_NPC_ACCEPT_GIVE_ITEM = L"ACCEPT_GIVE_ITEM";

const TCHAR* DEPEND_NPC_CLIENT = L"CLIENT";
const TCHAR* DEPEND_NPC_PAYER = L"PAYER";
const TCHAR* DEPEND_NPC_TARGET = L"TARGET";
const TCHAR* DEPEND_NPC_DELIVERY = L"DELIVERY";
const TCHAR* DEPEND_NPC_INGDIALOG = L"ING_DIALOG";
const TCHAR* DEPEND_NPC_INGBALLOON = L"ING_BALLOON";
*/



//Default
const TCHAR* LUA_INIT			= L"local __TEMP__ = %d\n";//QuestID
const TCHAR* LUA_STARTER			= L"Q[__TEMP__] = {}\n";
const TCHAR* LUA_HEADER			= L"Q[__TEMP__] = {\n";
const TCHAR* LUA_FOOTER			= L"}\n\n";

const TCHAR* LUA_ADD_STATE_STRING	= L"\
	%s = '%s';\n";//Name, Value String
const TCHAR* LUA_ADD_STATE_INT		= L"\
	%s = %d;\n";//Name, Value Integer

//Default Function List
	//Accept
	const TCHAR* LUA_ACCEPT_HEADER = L"\
	OnAssign = function(who)\n";
	const TCHAR* LUA_ACCEPT_FOOTER = L"\
	end;\n\n";

	//Drop
	const TCHAR* LUA_DROP_HEADER = L"\
	OnResign = function(who, kGround)\n";
	const TCHAR* LUA_DROP_FOOTER = L"\
	end;\n\n";

	//Complete
	const TCHAR* LUA_COMPLETE_HEADER = L"\
	OnComplete = function(who, kGround)\n";
	const TCHAR* LUA_COMPLATE_FOOTER = L"\
	end;\n\n";

	const TCHAR* LUA_ONDIALOG_HEADER = L"\
	OnDialog = function(kGround, who, dialog, id1, id2, id3, id4, id5)\n";
	const TCHAR* LUA_ONDIALOG_FOOTER = L"\
	end;\n\n";

	const TCHAR* LUA_ONDIALOG_AFTER_HEADER = L"\
	OnDialog_After = function(kGround, who, iNextDialog)\n";
	const TCHAR* LUA_ONDIALOG_AFTER_FOOTER = L"\
	end;\n\n";

///////////////////////////////////////////////////////////////////////////////////////////
// Monster
const TCHAR* LUA_MONSTER_STARTER	= L"Q[__TEMP__].M = {}\n";
const TCHAR* LUA_MONSTER_HEADER	= L"Q[__TEMP__].M[%d] = {\n";//Monster KID
const TCHAR* LUA_MONSTER_FOOTER	= L"}\n\n";


const TCHAR* LUA_MONSTER_QUEST_ONDIE_START = L"\
	OnDie = function(who, kMonster, kGround)\n";
const TCHAR* LUA_MONSTER_QUEST_ONDIE_END = L"\
	end;\n";//

//TYPE(INCPARAM, "INCPARAM")
const TCHAR* LUA_MONSTER_QUEST_ONDIE_INCMON = L"\
		who:IncQuestParam(__TEMP__, %d, %s)\n";//Object No, Inc Value

TYPE(INCPARAM_IN, "INCPARAM_IN")
const TCHAR* LUA_MONSTER_QUEST_ONDIE_INCMON_IN = L"\
		local iParamNo = %d\n\
		if %s == kGround:GroundNo() then\n\
			who:IncQuestParam(__TEMP__, iParamNo, %s)\n\
		end\n";//Object No, GroundNo, Inc Value

TYPE(INSTDROP, "INSTDROP")
const TCHAR* LUA_MONSTER_QUEST_ONDIE_INSTDROP = L"\
		local iParamNo = %d\n\
		if who:CheckGoingParam(__TEMP__, iParamNo) ~= 0 then\n\
			if RAND_S(%s, 0) > %s then\n\
				local iItemCnt = RAND_S(%s, %s)\n\
				if 0 < iItemCnt then\n\
					kGround:GiveItem(who, %s, iItemCnt)--Give Item\n\
				end\n\
			end\n\
		end\n";//Param No, Max Percent, Cut Line, Item Max, Item Min, ItemNo

TYPE(INSTDROP_IN, "INSTDROP_IN")
const TCHAR* LUA_MONSTER_QUEST_ONDIE_INSTDROP_IN = L"\
		local iParamNo = %d\n\
		if who:CheckGoingParam(__TEMP__, iParamNo) ~= 0 then\n\
			if %s == kGround:GroundNo() then \n\
				if RAND_S(%s, 0) > %s then\n\
					local iItemCnt = RAND_S(%s, %s)\n\
					if 0 < iItemCnt then\n\
						kGround:GiveItem(who, %s, iItemCnt)--Give Item\n\
					end\n\
				end\n\
			end\n\
		end\n";//Param No, GroundNo, Max Percent, Cut Line, Item Max, Item Min, ItemNo


TYPE(COMBOCHECK, "COMBOCHECK")
const TCHAR* LUA_MONSTER_QUEST_ONDIE_COMBOCHECK = L"\
		local iCurCombo = kGround:GetComboCount(who)\n\
		if %s <= iCurCombo then\n\
			who:IncQuestParam(__TEMP__, %d, %s)\n\
		end\n\
		\n";//CheckCombo Count, ObjectNo, Inc Value

///////////////////////////////////////////////////////////////////////////////////////////
// MISSION
const TCHAR* LUA_MISSION_STARTER	= L"Q[__TEMP__].Mission = {}\n";
const TCHAR* LUA_MISSION_HEADER		= L"Q[__TEMP__].Mission[%d] = {\n";//MissionKey
const TCHAR* LUA_MISSION_FOOTER		= L"}\n\n";

TYPE(PERCENT, "PERCENT")
const TCHAR* LUA_MISSION_PERCENT			= L"\
	OnPercent = function(who, iPercent)\n\
		local iParamNo = %d\n\
		if %s <= iPercent then\n\
			if who:CheckGoingParam(__TEMP__, iParamNo) ~= 0 then\n\
				who:IncQuestParam(__TEMP__, iParamNo, 1)\n\
			end\n\
		end\n\
	end;\n";//Param No, TargetPercent

TYPE(RANK, "RANK")
const TCHAR* LUA_MISSION_RANK =	L"Q[__TEMP__].Mission[%d] = {}\n";
const TCHAR* LUA_MISSION_RANK_STATER	= L"Q[__TEMP__].Mission[%d].OnRank = {}\n";
const TCHAR* LUA_MISSION_RANK_HEADER	= L"\
Q[__TEMP__].Mission[%d].OnRank[%d] = function(who, iCurRank)\n";//MissionKey, Mission Level(1~6)
const TCHAR* LUA_MISSION_RANK_BODY		= L"\
	if %s == iCurRank then\n\
		local iParamNo = %d\n\
		if who:CheckGoingParam(__TEMP__, iParamNo) ~= 0 then\n\
			who:IncQuestParam(__TEMP__, iParamNo, 1)\n\
			return\n\
		end\n\
	end\n";//Target Rank(S[0] ~ ...), Param No
const TCHAR* LUA_MISSION_RANK_FOOTER	= L"\
end;\n";//Target Rank(S[0] ~ ...), Param No

///////////////////////////////////////////////////////////////////////////////////////////
// PVP
const TCHAR* LUA_PVP_STARTER	= L"Q[__TEMP__].PVP = {\n";
const TCHAR* LUA_PVP_FOOTER		= L"}\n\n";

TYPE(WIN, "WIN")
const TCHAR* LUA_PVP_WIN			= L"\
	OnWin = function(who, bWin)\n\
		if bWin then\n\
			local iParamNo = %d\n\
			if who:CheckGoingParam(__TEMP__, iParamNo) ~= 0 then\n\
				who:IncQuestParam(__TEMP__, iParamNo, 1)\n\
			end\n\
		end\n\
	end;\n";//Param No

///////////////////////////////////////////////////////////////////////////////////////////
// LOCATION
const TCHAR* LUA_LOCATION_STARTER		= L"Q[__TEMP__].Location = {}\n";
const TCHAR* LUA_LOCATION_HEADER		= L"Q[__TEMP__].Location[%d] = {\n";//GroundNo
const TCHAR* LUA_LOCATION_FOOTER		= L"}\n\n";

TYPE(LOCATION_ENTER, "LOCATION_ENTER")
const TCHAR* LUA_LOCATION_ENTER_HEAD		= L"\
	OnEnter = function(kGround, who, sTriggerNo)\n";
const TCHAR* LUA_LOCATION_ENTER_BODY		= L"\
		if sTriggerNo == %d then\n\
			local iParamNo = %d\n\
			if who:CheckGoingParam(__TEMP__, iParamNo) ~= 0 then\n\
				who:IncQuestParam(__TEMP__, iParamNo, 1)\n\
			end\n\
		end\n";//TriggerNo, Param No
const TCHAR* LUA_LOCATION_ENTER_FOOT		= L"\
	end;\n";

///////////////////////////////////////////////////////////////////////////////////////////
// ITEM
//const TCHAR* LUA_ITEM_TOPARAM_HEADER = L"Q[__TEMP__].ItemToParam = {}\n";
//const TCHAR* LUA_ITEM_TOPARAM		= L"Q[__TEMP__].ItemToParam[%s] = %s\n"; //ItemNo, ParamNo
//
//const TCHAR* LUA_ITEM_STATER = L"Q[__TEMP__].Item = {}\n";
//const TCHAR* LUA_ITEM_HEADER = L"Q[__TEMP__].Item[%s] = {\n";//ParamNo
//const TCHAR* LUA_ITEM_FOOTER = L"}\n\n";

TYPE(CHANGE_COUNT, "CHANGE_COUNT")
//const TCHAR* LUA_ITEM_ONCHANGE_START = L"\
//	OnChange = function(who)\n\
//		local kInven = who:GetInventory()\n\
//		if not kInven:IsNil() then\n\
//			local iCount = 0\n";
//
//const TCHAR* LUA_ITEM_ONCHANGE_BODY = L"\
//			iCount = iCount + kInven:GetItemCount(%s, 1)\n";//ItemNo
//
//const TCHAR* LUA_ITEM_ONCHANGE_END = L"\
//			who:SetQuestParam(__TEMP__, %s, iCount)\n\
//		end\n\
//	end;\n";//ParamNo
TYPE(DURABILITY_CHECK, "DURABILITY_CHECK")
TYPE(PLUS_CHECK, "PLUS_CHECK")

///////////////////////////////////////////////////////////////////////////////////////////
// NPC
const TCHAR* LUA_NPC_STATER = L"Q[__TEMP__].N = {}\n";
const TCHAR* LUA_NPC_HEADER = L"Q[__TEMP__].N[%d] = {\n";//NPC KID No
const TCHAR* LUA_NPC_FOOTER = L"}\n\n";

const TCHAR* LUA_NPC_ONTALK_START = L"\
	OnTalk = function(kGround, who, kNpc)\n";
const TCHAR* LUA_NPC_ONTALK_END = L"\
	end;\n";

TYPE(CLIENT, "CLIENT")
const TCHAR* LUA_NPC_QUEST_CLIENT = L"\
		local iRet = who:CheckBeginQuest(__TEMP__)\n\
		local iNextDialog = %s\n\
		if 0 == iRet then--i'm can begin quest\n\
			--Do nothing\n\
		elseif 8 == iRet then\n\
			iNextDialog = 903--Full ing Quest\n\
		end\n\
		Q[__TEMP__].OnDialog(kGround, who, 0, iNextDialog, 0, 0, 0, 0)\n\
		\n";//Default Dialog ID

TYPE(PAYER, "PAYER")
const TCHAR* LUA_NPC_QUEST_PAYER = L"\
		if who:CheckCompleteQuest(__TEMP__) then--i'm can end quest\n\
			Q[__TEMP__].OnDialog(kGround, who, 0, %s, 0, 0, 0, 0)\n\
			return\n\
		end\n";//Default Dialog ID

TYPE(ING_DLG, "ING_DLG")
const TCHAR* LUA_NPC_QUEST_ING_DLG_START = L"\
		if who:CheckCompleteQuest(__TEMP__) == false then--i'm doing quest now\n";

const TCHAR* LUA_NPC_QUEST_ING_DLG_END = L"\
		end\n";

const TCHAR* LUA_NPC_QUEST_ING_DLG_PARAM_START = L"			if who:CheckGoingParam(__TEMP__, %d) ~= 0 then\n";//ObjectNo
const TCHAR* LUA_NPC_QUEST_ING_DLG_PARAM_END_1 = L"				return\n";
const TCHAR* LUA_NPC_QUEST_ING_DLG_PARAM_END_2 = L"			end\n";

//const TCHAR* LUA_NPC_QUEST_ING_DLG_SINGLE = L"\
//				who:ShowQuestDialog(__TEMP__, %s)\n";//Dialog ID

const TCHAR* LUA_NPC_QUEST_ING_DLG_HEAD = L"				local iNextDialog = 0\n";
const TCHAR* LUA_NPC_QUEST_ING_DLG_SINGLE = L"				iNextDialog = %s\n";//Item Cnt + 1
const TCHAR* LUA_NPC_QUEST_ING_DLG_RND_HEAD = L"			local kItem = {}\n";
const TCHAR* LUA_NPC_QUEST_ING_DLG_RND_ITEM = L"			kItem[%d] = %s\n";//Index(1~n), Dialog ID
const TCHAR* LUA_NPC_QUEST_ING_DLG_RND_RAND = L"			iNextDialog = RAND_S(%d, 1)\n";//Item Cnt + 1
const TCHAR* LUA_NPC_QUEST_ING_DLG_RND = L"\
				Q[__TEMP__].OnDialog(kGround, who, 0, iNextDialog, 0, 0, 0, 0)\n";
				//who:ShowQuestDialog(__TEMP__, iNextDialog)\n";

TYPE(ING_CHAT, "ING_CHAT")
//const TCHAR* LUA_NPC_QUEST_ING_CHAT_SINGLE = L"\
//				who:ShowBalloonDialog(__TEMP__, %s)\n";//Text ID
const TCHAR* LUA_NPC_QUEST_ING_CHAT_RND = L"\
				who:ShowBalloonDialog(__TEMP__, iNextDialog)\n";

//Log
//InfoLog(8, ' Item Cnt is' .. iCnt)\n\

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part 2 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//LUA_ONDIALOG_HEADER
//OnDialog = function(who, dialog, id1, id2, id3, id4, id5)

const TCHAR* LUA_ONDIALOG_HEAD_DEFAULT = L"\
		\n\
		if 0 >= id1 then\n\
			return\n\
		end\n\
		local kInven = who:GetInventory()\n\
		local iNextDialog = 0\n\n";

//보상 이전 부분
const TCHAR* LUA_ONDIALOG_BODY_DEFAULT_1 = L"\
		\n\
		--Inven Check\n\
		if kInven:IsCanQuestTalk(__TEMP__, who) then\n\
			\n\
		else\n\
			id1 = 902--Full inven\n\
		end\n\
		\n\
		if 0 == id1 then\n\
			--Intercepted event previous script\n\
		elseif 100 < id1 and 1000 > id1 then\n\
			iNextDialog = id1\n\
		elseif 10000 <= id1 and 19999 >= id1 then--Accept\n\
			local iRet = who:BeginQuest(__TEMP__)\n\
			if 0 == iRet then --Ret == S_OK\n\
				if 10000 == id1 then\n\
					iNextDialog = 301\n\
				elseif 19999 == id1 then\n\
					iNextDialog = 0\n\
				else\n\
					iNextDialog = id1 - 10000\n\
				end\n\
			else\n\
				--if 8 == iRet then\n\
					iNextDialog = 903--full quest\n\
				--else\n\
				--	iNextDialog = 902--Inven Full\n\
				--end\n\
			end\n\
		elseif 20000 <= id1 and 29999 >= id1 then--Reject\n\
			if 20000 == id1 then\n\
				iNextDialog = 401\n\
			elseif 29999 == id1 then\n\
				iNextDialog = 0\n\
			else\n\
				iNextDialog = id1 - 20000\n\
			end\n\
		elseif 30000 <= id1 and 39999 >= id1 then--Complete\n\
			if 30000 == id1 then\n\
				iNextDialog = 0\n\
			elseif 39999 == id1 then\n\
				iNextDialog = 0\n\
			else\n\
				iNextDialog = id1 - 30000\n\
			end\n\
			if who:CheckCompleteQuest(__TEMP__) then\n\
				\n";
//보상
const TCHAR* LUA_INDIALOG_BODY_REWARD_2 = L"\
				local iMinLevel = %d\n\
				local iFromClass = %d\n\
				local iLevel = who:GetAbil(AT_LEVEL)\n\
				local iClass = who:GetAbil(AT_CLASS)\n\
				if iLevel >= iMinLevel and iClass == iFromClass then\n\
					who:ChangeClass(%d)--Transform class\n\
				else\n\
					InfoLog(1, '[ChangeClass][Error] Player level or class is invalid[L:'..iLevel..'][C:'..iClass..']')\n\
				end\n";//Min, From Class, Target Class
const TCHAR* LUA_INDIALOG_BODY_REWARD_END = L"\
			end\n";
//보상 이후 부분
//보상 이후 이어지는 다이얼로그를 701에서 0(닫기)으로 한다.
const TCHAR* LUA_ONDIALOG_BODY_DEFAULT_2 = L"\
		elseif 40000 <= id1 and 49999 >= id1 then--ETC\n\
			iNextDialog = id1 - 40000\n\
		elseif iNextDialog == 0 and id1 ~= 0 then--Default\n\
			iNextDialog = id1\n\
		end\n\n";

const TCHAR* LUA_ONDIALOG_FOOT_DEFAULT = L"\
		\n\
		if iNextDialog ~= 0 then\n\
			who:ShowQuestDialog(__TEMP__, iNextDialog)\n\
		elseif iNextDialog == 0 then\n\
			return;\n\
		end\n\
		Q[__TEMP__].OnDialog_After(kGround, who, iNextDialog)--after\n";


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// PRE TYPE

// LUA_ONDIALOG_HEAD_DEFAULT> ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ <LUA_ONDIALOG_BODY_DEFAULT
TYPE(CHECKPARAM, "CHECKPARAM");
const TCHAR* ONDIALOG_CHECKPARAM = L"\
		\n\
		if %s == dialog and %s == id1 then\n\
			if 0 ~= who:CheckGoingParam(__TEMP__, %s) then\n\
				iNextDialog = %s\n\
				id1 = 0\n\
			end\n\
		end\n";//Cur Dialog ID, Next Dialog ID, Check Param No, Failed Dialog ID

TYPE(CHECKITEM, "CHECKITEM")//Check only inventory
const TCHAR* ONDIALOG_CHECKITEM_5 = L"\
		if %s == dialog and %s == id1 then\n\
			local iCnt = kInven:GetItemCount(%s, 1)\n\
			if iCnt < %s then\n\
				iNextDialog = %s\n\
				id1 = 0\n\
			end\n\
		end\n";//Cur Dialog ID, Next Dialog ID, Item No, Minimum Item Count, Failed Dialog ID
const TCHAR* ONDIALOG_CHECKITEM_4 = L"\
		if %s == id1 then\n\
			local iCnt = kInven:GetItemCount(%s, 1)\n\
			if iCnt < %s then\n\
				iNextDialog = %s\n\
				id1 = 0\n\
			end\n\
		end\n";//Cur Dialog ID, Item No, Minimum Item Count, Failed Dialog ID

TYPE(CHECKGOLD, "CHECKGOLD")
const TCHAR* ONDIALOG_CHECKGOLD = L"\
		if %s == id1 then \n\
			local bRet = who:CompareMoney(\"%s\", %s, %s, %s)\n\
			if not bRet then\n\
				iNextDialog = %s\n\
				id1 = 0\n\
			end\n\
		end\n";//Cur Dialog DI, Sign, Gold, Silver, Bronze, Failed Dialog ID

// AFTER TYPE

// LUA_ONDIALOG_BODY_DEFAULT> ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ <LUA_ONDIALOG_FOOT_DEFAULT
TYPE(REMOVEITEM, "REMOVEITEM");
const TCHAR* ONDIALOG_EVENT_REMOVEITEM = L"\
		\n\
		if %s == iNextDialog then\n\
			kGround:DeletePlayerItem(who, %s, %s)\n\
		end\n";//Result Next Dialog ID, ItemNo, ItemCount

TYPE(GIVEITEM, "GIVEITEM");
const TCHAR* ONDIALOG_EVENT_GIVEITEM = L"\
		\n\
		if %s == iNextDialog then\n\
			kGround:GiveItem(who, %s, %s)\n\
		end\n";//Result next Dialog ID, ItemNo, ItemCount

TYPE(INCPARAM, "INCPARAM");
const TCHAR* ONDIALOG_EVENT_INCPARAM = L"\
		\n\
		if %s == iNextDialog then\n\
			who:IncQuestParam(__TEMP__, %d, %s)\n\
		end\n";//Result Next Dialog ID, ParamNo, Inc Val

TYPE(COMPLETEQUEST, "COMPLETEQUEST");
const TCHAR* ONDIALOG_EVENT_COMPLETEQUEST = L"\
		\n\
		if %s == iNextDialog then\n\
			who:CompleteQuest(%s)--Complete\n\
		end\n";//Result Next Dialog ID, QuestID

//TYPE(DROPQUEST, "DROPQUEST");
//const TCHAR* ONDIALOG_EVENT_DROPQUEST = L"\
//		\n\
//		if %s == iNextDialog then\n\
//			who:DropQuest(%s)--Drop\n\
//		end\n";//Result Next Dialog ID, QuestID

TYPE(ADDGOLD, "ADDGOLD");
const TCHAR* ONDIALOG_EVENT_ADDGOLD = L"\
		\n\
		if %s == iNextDialog then\n\
			kGround:AddGold(\"%s\", %s, %s, %s, who)\n\
		end\n";//Result Next Dialog ID, Sign, Gold, Silver, Bronze

//end;