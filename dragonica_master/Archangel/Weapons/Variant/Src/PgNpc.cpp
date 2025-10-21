#include "stdafx.h"
#include "Global.h"
#include "PgStringUtil.h"
#include "PgNpc.h"
#include "TableDataManager.h"

namespace PgNpcUtil
{
	//
	struct SMenuItem
	{
		SMenuItem(unsigned short MenuType, TCHAR const* pkMenuName)
			: sMenuType(MenuType), kMenuName(pkMenuName)
		{
			BM::vstring::ConvToUPR(kMenuName);
		}

		SMenuItem(SMenuItem const& rhs)
			: sMenuType(rhs.sMenuType), kMenuName(rhs.kMenuName)
		{
		}

		SMenuItem operator = (SMenuItem const& rhs)
		{
			sMenuType = rhs.sMenuType;
			kMenuName = rhs.kMenuName;
			return *this;
		}
		unsigned short sMenuType;
		std::wstring kMenuName;
	};

	//
	SMenuItem const pkMenuPreSet[] = {	SMenuItem(NMT_Quest, L"QUEST"), 
										SMenuItem(NMT_Quest, L"TACTICS_QUEST"), 
										SMenuItem(NMT_Quest, L"DAILYQUEST_LV0"), 
										SMenuItem(NMT_Quest, L"DAILYQUEST_LV1"), 
										SMenuItem(NMT_Quest, L"DAILYQUEST_LV2"), 
										SMenuItem(NMT_Quest, L"RANDOM_QUEST"), 
										SMenuItem(NMT_Quest, L"TACTICS_RANDOM_QUEST"), 
										SMenuItem(NMT_Quest, L"WANTED_QUEST"),
										SMenuItem(NMT_Shop, L"SHOP"), 
										SMenuItem(NMT_Shop, L"COIN"), 
										SMenuItem(NMT_Shop, L"JOBSKILL_SHOP"),
										SMenuItem(NMT_GemStore, L"SWORD_CP_SHOP"), 
										SMenuItem(NMT_GemStore, L"MAGICIAN_CP_SHOP"), 
										SMenuItem(NMT_GemStore, L"ARCHER_CP_SHOP"), 
										SMenuItem(NMT_GemStore, L"THIEF_CP_SHOP"), 
										SMenuItem(NMT_Repair, L"REPAIR"), 
										SMenuItem(NMT_InvenSafe, L"INVSAFE"), 
										SMenuItem(NMT_Enchant, L"ENCHANT"), 
										SMenuItem(NMT_SoulCraft, L"RARITY_UPGRADE"), 
										SMenuItem(NMT_SoulCraft, L"CRAFT"), 
										SMenuItem(NMT_Post, L"POST"), 
										SMenuItem(NMT_Fran, L"FRAN"), 
										SMenuItem(NMT_Soul, L"SOUL"), 
										SMenuItem(NMT_Guild, L"GUILD"), 
										SMenuItem(NMT_GuildInventory, L"GUILD_INVENTORY"),
										SMenuItem(NMT_Coupon, L"COUPON"), 
										SMenuItem(NMT_Making, L"LOTTERY"), 
										SMenuItem(NMT_Making, L"SHINE_UP"), 
										SMenuItem(NMT_Making, L"COOKING"), 
										SMenuItem(NMT_Making, L"TRADE_UNSEALINGSCROLL"), 
										SMenuItem(NMT_Making, L"TRADE_SOULSTONE"), 
										SMenuItem(NMT_Making, L"RARITY_BUILD_UP"),
										SMenuItem(NMT_Making, L"RARITY_AMPLIFY"),
										SMenuItem(NMT_ClientOnly, L"WARNING"), 
										SMenuItem(NMT_ClientOnly, L"SWORD_CLASS_HELP"), 
										SMenuItem(NMT_ClientOnly, L"MAGICIAN_CLASS_HELP"), 
										SMenuItem(NMT_ClientOnly, L"ARCHER_CLASS_HELP"), 
										SMenuItem(NMT_ClientOnly, L"THIEF_CLASS_HELP"), 
										SMenuItem(NMT_ClientOnly, L"TALK"),
										SMenuItem(NMT_ClientOnly, L"TALK_ONLY"), // same TALK
										SMenuItem(NMT_EmporiaAdmin, L"EM_ADMINISTRATION"),
										SMenuItem(NMT_GemStore, L"GEMSTORE"),
										SMenuItem(NMT_GemStore, L"GEMSTORE_SHOP1"), 
										SMenuItem(NMT_GemStore, L"GEMSTORE_SHOP2"), 
										SMenuItem(NMT_Socket, L"SOCKET_CREATE"),
										SMenuItem(NMT_Socket, L"SOCKET_RESTORATION"),
										SMenuItem(NMT_Socket, L"SOCKET_RESET"),
										SMenuItem(NMT_Socket, L"SOCKET_MAKE"),
										SMenuItem(NMT_Mission_Event, L"MISSION_EVENT"),
										SMenuItem(NMT_ItemConvert, L"ITEMCONVERT"),
										SMenuItem(NMT_EventTWEffectQuest, L"EVENT_TW_EFFECTQUEST"),
										SMenuItem(NMT_RealtyDealer, L"REALTY_DEALER"),
										SMenuItem(NMT_MixupItem,L"MIXUPITEM"),
										SMenuItem(NMT_SoulStoneTrade, L"SOUL_STONE_TRADE"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_HELP"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_SETTING"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_DECORATION"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_INVITATION"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_TAX"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_SOCKET"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_AUCTION_MENU"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_AUCTION_INFO"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_NPC_MOVE"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_NPC_REMOVE"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_SOUL_GOLD"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_SOUL_COUNT"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_ENCHANT_RATE"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_SIDE_JOB_BOARD"),
										SMenuItem(NMT_ClientOnly, L"MYHOME_INFO"),
										SMenuItem(NMT_ClientOnly, L"TOWN_HELP"),
										SMenuItem(NMT_ClientOnly, L"TOWN_BOARD"),
										SMenuItem(NMT_ClientOnly, L"TOWN_APT_BUY"),
										SMenuItem(NMT_ClientOnly, L"TOWN_APT_SELL"),
										SMenuItem(NMT_ClientOnly, L"TOWN_MOVE"),
										SMenuItem(NMT_ClientOnly, L"MARRY_IF_TALK"),
										SMenuItem(NMT_ClientOnly, L"MARRY_REQSEND"),
										SMenuItem(NMT_ClientOnly, L"MARRY_MONEY"),
										SMenuItem(NMT_ClientOnly, L"MARRY_MGS"),
										SMenuItem(NMT_ClientOnly, L"MARRY_START"),
										SMenuItem(NMT_EventQuestSys, L"EVENT_QUEST_SYS"),
										SMenuItem(NMT_ClientOnly, L"EVENT_MISSION"),
										SMenuItem(NMT_ClientOnly, L"IGGACHA"),
										SMenuItem(NMT_BattleSquare, L"BATTLE_SQUARE"),
										SMenuItem(NMT_InvenSafe, L"SHARE_SAFE_INV"),
										SMenuItem(NMT_TransTower, L"TRANSPORT"),
										SMenuItem(NMT_TransTower, L"SAVEPOS"),
										SMenuItem(NMT_ClientOnly, L"HIDDEN_MSG"),
										SMenuItem(NMT_ClientOnly, L"HIDDEN_OPEN"),
										SMenuItem(NMT_ClientOnly, L"CHRISTMAS_EVENT"),
										SMenuItem(NMT_ClientOnly, L"HELP_TALK"),
										SMenuItem(NMT_ClientOnly, L"GEMHELP"),
										SMenuItem(NMT_ClientOnly, L"MON_CARD2_MIX"),
										SMenuItem(NMT_ClientOnly, L"JAPAN_EVENT"),
										SMenuItem(NMT_ClientOnly, L"ENCHANT_SHIFT"),
										SMenuItem(NMT_ClientOnly, L"SKILL_MASTER"),
                                        SMenuItem(NMT_Shop, L"EMPORIA_SHOP"),
										SMenuItem(NMT_ItemConvert, L"EXCHANGE_PET_EXP"),
										SMenuItem(NMT_PvPLeague, L"PVPLEAGUE_REGIST"),
										SMenuItem(NMT_PvPLeague, L"PVPLEAGUE_MOVE"),
										SMenuItem(NMT_JobSkill, L"JOB_SKILL_LEARN"),
										SMenuItem(NMT_ClientOnly, L"JOB_SKILL3_CREATEITEM"),
										SMenuItem(NMT_ClientOnly, L"JOB_SKILL_SOUL_TRANSFER"),
										SMenuItem(NMT_Expedition, L"EXPEDITION"),
										//SMenuItem(NMT_ClientOnly, L"SHAMAN_CLASS_HELP"),
										SMenuItem(NMT_GemStore, L"SHAMAN_CP_SHOP"),
										//SMenuItem(NMT_ClientOnly, L"DOUBLE_FIGHTER_CLASS_HELP"), 
										SMenuItem(NMT_GemStore, L"DOUBLE_FIGHTER_CP_SHOP"),
										SMenuItem(NMT_Making, L"TRADE_UNSEALINGSCROLL2"), 
										SMenuItem(NMT_SoulCraft, L"CRAFT2"), 
										SMenuItem(NMT_ClientOnly, L"ELEMENTHELP"),
										SMenuItem(NMT_GemStore, L"COLLECT_ANTIQUE"),
										SMenuItem(NMT_GemStore, L"EXCHANGE_SOCKETCARD"),
										SMenuItem(NMT_ElementDungeon, L"INFO_ELEMENT_DUNGEON"),
										SMenuItem(NMT_ElementDungeon, L"INFO_USE_ELEMENT_STONE"),
										SMenuItem(NMT_ElementDungeon, L"ENTER_ELEMENT_DUNGEON"),
										SMenuItem(NMT_ElementDungeon, L"EXCHANGE_CARDBOX"),
										SMenuItem(NMT_ElementDungeon, L"EXCHANGE_SOCKETCARD"),
										SMenuItem(NMT_ElementDungeon, L"ELEMENT_STORE"),
										SMenuItem(NMT_ClientOnly, L"HALLOWEEN_EVENT"), 
										SMenuItem(NMT_GemStore, L"EXCHANGE_GATHER"), 
										SMenuItem(NMT_GemStore, L"EVENTSTORE"),
										SMenuItem(NMT_CommunityEvent, L"COMMUNITY_EVENT"),
										SMenuItem(NMT_PetUpgrade, L"PET_UPGRADE"),
										SMenuItem(NMT_RagnarokTheme, L"EXCHANGE_ELUNIUM"),
										SMenuItem(NMT_RagnarokTheme, L"EXCHANGE_PURESILVER_KEY"),
										SMenuItem(NMT_RagnarokTheme, L"RAGNAROK_BLESS"),
										SMenuItem(NMT_GemStore, L"VALKYRIE_STORE"),
										SMenuItem(NMT_GemStore, L"ITEM_STORE"),
										SMenuItem(NMT_PetUpgrade, L"PET_UPGRADE"),
										SMenuItem(NMT_GemStore, L"ITEM_TREE"),
										SMenuItem(NMT_GemStore, L"EVENT_SHOP"),
										SMenuItem(NMT_RagnarokTheme, L"BUFF"),
										SMenuItem(NMT_GemStore, L"ITEM_SWAP"),
										SMenuItem(NMT_GemStore, L"ITEM_PLUS"),
	};

	//
	struct SScriptMenu
	{
		SScriptMenu(TCHAR const* pkScript, TCHAR const* pkMenuString)
			: kScript(pkScript), kMenuString(pkMenuString)
		{
			BM::vstring::ConvToUPR(kScript);
			BM::vstring::ConvToUPR(kMenuString);

			VEC_WSTRING kTemp;
			PgStringUtil::BreakSep(kMenuString, kTemp, _T("/"));
			VEC_WSTRING::const_iterator iter = kTemp.begin();
			while( kTemp.end() != iter )
			{
				VEC_WSTRING::value_type const& rkString = (*iter);
				SMenuItem const* pkBegin = pkMenuPreSet;
				while( PgArrayUtil::IsInArray(pkBegin, pkMenuPreSet) )
				{
					SMenuItem const& rkMenuItem = (*pkBegin);
					if( rkString == rkMenuItem.kMenuName )
					{
						kContMenu.insert( rkMenuItem.sMenuType ); // Str to MenuID
						break;
					}
					++pkBegin;
				}

				if( !PgArrayUtil::IsInArray(pkBegin, pkMenuPreSet) )
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Can't find MenuItem Name[") << rkString << _T("]"));
					ASSERT_LOG(false, BM::LOG_LV1, __FL__ << _T("Can't find MenuItem Name[") << rkString << _T("]"));
				}
				++iter;
			}
		}
		SScriptMenu(SScriptMenu const& rhs)
			: kScript(rhs.kScript), kMenuString(rhs.kMenuString)
		{
		}

		SScriptMenu operator = (SScriptMenu const& rhs)
		{
			kScript = rhs.kScript;
			kMenuString = rhs.kMenuString;
			return *this;
		}

		std::wstring kScript;
		std::wstring kMenuString;
		ContMenuItem kContMenu;
	};

	//
	SScriptMenu const pkNpcMenuScript[] = {	SScriptMenu(L"NPC_CLICK", L"QUEST/TALK"),
											SScriptMenu(L"NPC_TALK_ONLY", L"QUEST/TALK_ONLY"),
											SScriptMenu(L"NPC_FRAN", L"QUEST/FRAN/TALK"),
											SScriptMenu(L"NPC_GUILD", L"QUEST/DAILYQUEST_LV2/GUILD/TALK/GUILD_INVENTORY"),
											SScriptMenu(L"NPC_GUILD_NEW", L"QUEST/TACTICS_RANDOM_QUEST/GUILD/TALK/GUILD_INVENTORY"),
											SScriptMenu(L"NPC_CLICK_SHOP", L"QUEST/SHOP/TALK"),
											SScriptMenu(L"NPC_ENCHANT", L"QUEST/ENCHANT/TALK"),
											SScriptMenu(L"NPC_RARITYUPGRADE", L"QUEST/RARITY_UPGRADE/TALK"),
											SScriptMenu(L"NPC_ITEMREPAIR", L"QUEST/REPAIR/TALK"),
											SScriptMenu(L"NPC_BLACKSMITH", L"QUEST/REPAIR/ENCHANT/TALK"),
											SScriptMenu(L"NPC_INVSAFE", L"QUEST/INVSAFE/SHARE_SAFE_INV/TALK"),
											SScriptMenu(L"NPC_SHINESTONE", L"QUEST/LOTTERY/SHINE_UP/TALK"),
											SScriptMenu(L"NPC_COOKING", L"QUEST/COOKING/TALK"),
											SScriptMenu(L"NPC_WARNING", L"QUEST/WARNING/TALK"),
											SScriptMenu(L"NPC_COINCHANGER", L"QUEST/COIN/TALK"),
											SScriptMenu(L"NPC_CRAFT_ITEM_CHANGE", L"QUEST/TRADE_UNSEALINGSCROLL/CRAFT/ITEMCONVERT/RARITY_BUILD_UP/MIXUPITEM/TALK"),
											SScriptMenu(L"NPC_CRAFT", L"QUEST/TRADE_UNSEALINGSCROLL/CRAFT/RARITY_BUILD_UP/MIXUPITEM/TALK"),
											SScriptMenu(L"NPC_POSTBOX", L"POST"),
											SScriptMenu(L"NPC_COUPON", L"QUEST/COUPON/TALK/ITEMCONVERT"),
											SScriptMenu(L"NPC_SWORD_LEADER", L"QUEST/SWORD_CLASS_HELP/SWORD_CP_SHOP/MON_CARD2_MIX/TALK"),
											SScriptMenu(L"NPC_MAGICIAN_LEADER", L"QUEST/MAGICIAN_CLASS_HELP/MAGICIAN_CP_SHOP/MON_CARD2_MIX/TALK"),
											SScriptMenu(L"NPC_ARCHER_LEADER", L"QUEST/ARCHER_CLASS_HELP/ARCHER_CP_SHOP/MON_CARD2_MIX/TALK"),
											SScriptMenu(L"NPC_THEIF_LEADER", L"QUEST/THIEF_CLASS_HELP/THIEF_CP_SHOP/MON_CARD2_MIX/TALK"),
											SScriptMenu(L"NPC_DAILYQUEST_LV0", L"QUEST/DAILYQUEST_LV0/TALK"),
											SScriptMenu(L"NPC_DAILYQUEST_LV1", L"QUEST/DAILYQUEST_LV1/TALK"),
											SScriptMenu(L"NPC_DAILYQUEST_LV2", L"QUEST/DAILYQUEST_LV2/TALK"),
											SScriptMenu(L"NPC_SOULTRADER", L"QUEST/TRADE_SOULSTONE/TALK"),
											SScriptMenu(L"NPC_EMPORIAADMINSTRATOR", L"EM_ADMINISTRATION/TALK/SHOP/EMPORIA_SHOP"),
											SScriptMenu(L"NPC_MISSION_EVENT", L"MISSION_EVENT/QUEST"),
											SScriptMenu(L"NPC_GEMSTORE", L"QUEST/GEMHELP/GEMSTORE/GEMSTORE_SHOP1/GEMSTORE_SHOP2/TALK/COLLECT_ANTIQUE"),// /EXCHANGE_SOCKETCARD"),
											SScriptMenu(L"NPC_DEFENCE_GEMSTORE", L"QUEST/GEMHELP/GEMSTORE/GEMSTORE_SHOP1/GEMSTORE_SHOP2/TALK/COLLECT_ANTIQUE"), // /EXCHANGE_SOCKETCARD"),
											SScriptMenu(L"NPC_DEFENCE_GEMSTORE2", L"QUEST/GEMHELP/GEMSTORE/GEMSTORE_SHOP1/GEMSTORE_SHOP2/TALK/COLLECT_ANTIQUE"), // /EXCHANGE_SOCKETCARD"),
											SScriptMenu(L"NPC_EVENT_TW_EFFECTQUEST", L"QUEST/EVENT_TW_EFFECTQUEST/TALK"),
											SScriptMenu(L"NPC_REALTY", L"QUEST/REALTY_DEALER/TOWN_HELP/TOWN_BOARD/TOWN_APT_BUY/TOWN_APT_SELL/TALK"),
											SScriptMenu(L"NPC_MIXUPITEM", L"QUEST/MIXUPITEM/TALK"),
											SScriptMenu(L"NPC_GUILD_CRAFT", L"CRAFT/TALK"),
											SScriptMenu(L"NPC_SOULTRADER2", L"QUEST/SOUL_STONE_TRADE/TALK"),
											SScriptMenu(L"NPC_HOME_MAID", L"MYHOME_HELP/MYHOME_SETTING/MYHOME_DECORATION/MYHOME_INVITATION/MYHOME_TAX/MYHOME_AUCTION_MENU/TALK"),
											SScriptMenu(L"NPC_HOME_MAID_SOUL_GOLD", L"MYHOME_NPC_MOVE/MYHOME_NPC_REMOVE/MYHOME_SOUL_GOLD/TALK"),
											SScriptMenu(L"NPC_HOME_MAID_SOUL_COUNT", L"MYHOME_NPC_MOVE/MYHOME_NPC_REMOVE/MYHOME_SOUL_COUNT/TALK"),
											SScriptMenu(L"NPC_HOME_MAID_SOCKET_GOLD", L"MYHOME_NPC_MOVE/MYHOME_NPC_REMOVE/TALK"),
											SScriptMenu(L"NPC_HOME_MAID_SOCKET_COUNT", L"MYHOME_NPC_MOVE/MYHOME_NPC_REMOVE/TALK"),
											SScriptMenu(L"NPC_HOME_MAID_ENCHANT_RATE", L"MYHOME_NPC_MOVE/MYHOME_NPC_REMOVE/MYHOME_ENCHANT_RATE/TALK"),
											SScriptMenu(L"NPC_HOME_SIDE_JOB_BOARD", L"MYHOME_SIDE_JOB_BOARD/TALK"),
											SScriptMenu(L"NPC_HOME_HOMETOWN_PORTAL", L"TOWN_MOVE/TALK"),
											SScriptMenu(L"NPC_WEDDING_PLANNER", L"QUEST/MARRY_IF_TALK/MARRY_REQSEND/TALK"),
											SScriptMenu(L"NPC_DONATIONBOX", L"MARRY_MONEY"),
											SScriptMenu(L"NPC_WEDDING", L"MARRY_MGS/MARRY_START"),
											SScriptMenu(L"NPC_TW_WOMEN", L"QUEST/EVENT_QUEST_SYS/TALK"),
											SScriptMenu(L"NPC_EVENTQUEST_SYS", L"QUEST/EVENT_QUEST_SYS/TALK"),
											SScriptMenu(L"NPC_GLOG_MISSION", L"QUEST/EVENT_MISSION/TALK"),
											SScriptMenu(L"NPC_IGGACHA", L"IGGACHA/EVENTSTORE/EVENT_SHOP"),
											SScriptMenu(L"NPC_BATTLE_SQUARE", L"QUEST/BATTLE_SQUARE/TALK"),
											SScriptMenu(L"NPC_TRANSTOWER", L"TRANSPORT/SAVEPOS"),
											SScriptMenu(L"NPC_RANDOMQUEST", L"QUEST/RANDOM_QUEST/TALK"),
											SScriptMenu(L"NPC_WANTEDQUEST", L"QUEST/WANTED_QUEST"),
											SScriptMenu(L"NPC_HIDDEN", L"HIDDEN_MSG/TALK"),	//HIDDEN_OPEN/
											SScriptMenu(L"NPC_CHRISTMAS_EVENT", L"QUEST/CHRISTMAS_EVENT/TALK"),
											SScriptMenu(L"NPC_HELP_TALK", L"QUEST/HELP_TALK/TALK"),
											SScriptMenu(L"NPC_JAPAN_EVENT", L"QUEST/JAPAN_EVENT"),
											SScriptMenu(L"NPC_SKILL_MASTER", L"SKILL_MASTER"),
											SScriptMenu(L"NPC_CLICK_SHOP_PET", L"QUEST/PET_UPGRADE/SHOP/TALK"),
											SScriptMenu(L"NPC_PVP_LEAGUE", L"PVPLEAGUE_REGIST/PVPLEAGUE_MOVE/TALK"),
                                            SScriptMenu(L"NPC_EMPORIA_SHOP", L"EMPORIA_SHOP"),
											SScriptMenu(L"NPC_JOBSKILL", L"QUEST/TALK/JOB_SKILL_LEARN/COLLECT_ANTIQUE/JOB_SKILL3_CREATEITEM/JOBSKILL_SHOP/SOCKET_MAKE/JOB_SKILL_SOUL_TRANSFER/ENCHANT_SHIFT/RARITY_AMPLIFY"),
											SScriptMenu(L"NPC_EXPEDITION", L"EXPEDITION"),
											SScriptMenu(L"NPC_SHAMAN_LEADER", L"QUEST/SHAMAN_CP_SHOP/MON_CARD2_MIX/TALK"),
											SScriptMenu(L"NPC_DOUBLE_FIGHTER_LEADER", L"QUEST/DOUBLE_FIGHTER_CP_SHOP/MON_CARD2_MIX/TALK"),
//											SScriptMenu(L"NPC_SHAMAN_LEADER", L"QUEST/SHAMAN_CLASS_HELP/SHAMAN_CP_SHOP/MON_CARD2_MIX/TALK"),
//											SScriptMenu(L"NPC_DOUBLE_FIGHTER_LEADER", L"QUEST/DOUBLE_FIGHTER_CLASS_HELP/DOUBLE_FIGHTER_CP_SHOP/MON_CARD2_MIX/TALK"),
											SScriptMenu(L"NPC_CRAFT2", L"QUEST/TRADE_UNSEALINGSCROLL2/CRAFT2/ITEMCONVERT/RARITY_BUILD_UP/MIXUPITEM/TALK"),
											SScriptMenu(L"NPC_HALLOWEEN_EVENT", L"QUEST/HALLOWEEN_EVENT/TALK"),
											SScriptMenu(L"NPC_ELEMENT_DUNGEON", L"QUEST/INFO_ELEMENT_DUNGEON/INFO_USE_ELEMENT_STONE/ENTER_ELEMENT_DUNGEON/EXCHANGE_CARDBOX/EXCHANGE_SOCKETCARD/ELEMENT_STORE/TALK"),
											SScriptMenu(L"NPC_EXCHANE_GATHER", L"QUEST/TALK/EXCHANGE_GATHER"),
											SScriptMenu(L"NPC_COMMUNITY_EVENT", L"QUEST/TALK/COMMUNITY_EVENT/ITEM_STORE"),
											SScriptMenu(L"NPC_RAGNAROK_KAFRA", L"QUEST/TALK/EXCHANGE_ELUNIUM/VALKYRIE_STORE/EXCHANGE_PURESILVER_KEY"),
											SScriptMenu(L"NPC_RAGNAROK_BLESS", L"QUEST/TALK/RAGNAROK_BLESS"),
											SScriptMenu(L"NPC_RAGNAROK_EX_ITEM", L"QUEST/TALK/ITEM_STORE/REPAIR"),
											SScriptMenu(L"NPC_HALLOWEEN_RACE", L"QUEST/TALK/ITEM_STORE"),
											SScriptMenu(L"NPC_ITEM_STORE", L"QUEST/ITEM_STORE/TALK"),
											SScriptMenu(L"NPC_ITEM_TREE", L"QUEST/TALK/ITEM_TREE/ITEM_SWAP/ITEM_PLUS"),
											SScriptMenu(L"NPC_FOURTH_ANNIVERSARY", L"QUEST/TALK/BUFF")
	};

	//
	bool ConvertScriptToMenu(std::wstring& rkScript, ContMenuItem& rkOut)
	{
		BM::vstring::ConvToUPR(rkScript);

		SScriptMenu const* pkBegin = pkNpcMenuScript;
		while( PgArrayUtil::IsInArray(pkBegin, pkNpcMenuScript) )
		{
			SScriptMenu const& rkScriptMenu = (*pkBegin);
			if( rkScriptMenu.kScript == rkScript )
			{
				rkScript = rkScriptMenu.kMenuString;
				rkOut = rkScriptMenu.kContMenu;
				return true;
			}
			++pkBegin;
		}
		return false;
	}
}


PgNpc::PgNpc(void)
:	m_kNpcType(E_NPC_TYPE_FIXED)
{
}

PgNpc::~PgNpc(void)
{
}

int PgNpc::CallAction(WORD wActCode, SActArg *pActArg)
{
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

HRESULT PgNpc::Create(const void* pkInfo)
{
	if( !pkInfo )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" pkInfo is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
	const SNpcCreateInfo* pkNpcInfo = (const SNpcCreateInfo*) pkInfo;
	if( Create(pkNpcInfo->kGuid, pkNpcInfo->wstrName, pkNpcInfo->wstrActor, pkNpcInfo->wstrScriptName, pkNpcInfo->kPosition, pkNpcInfo->iID, pkNpcInfo->GiveEffectNo, pkNpcInfo->eType, pkNpcInfo->bHideMiniMap) )
	{
		return S_OK;
	}
	return E_FAIL;
}

bool PgNpc::Create(BM::GUID const &rkGuid, std::wstring const &rstrkName, std::wstring const &rstrkActor, std::wstring const &rstrkScript, POINT3 const& rkPosition, int iID, int GiveEffectNo, ENpcType const eType, bool bHideMiniMap)
{
	SetID(rkGuid);
	Name(rstrkName);
	m_kActorName = rstrkActor;
	m_kMenuStr = rstrkScript;
	m_kHideMiniMap = bHideMiniMap;
	m_kGiveEffectNo = GiveEffectNo;
	if( !PgNpcUtil::ConvertScriptToMenu(m_kMenuStr, m_kMenu) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("NPC Script\"") << m_kMenuStr << _T("\" is unknown script type"));
		return false;
	}
	SetAbil(AT_CLASS, iID);//NPC는 자기 번호를 클래스로 가진다.

	SetPos(rkPosition);
	m_kNpcType = eType;
	SetAbil(AT_HP, (E_NPC_TYPE_CREATURE == NpcType() ? 0 : 1) );

	CONT_DEF_EXPEDITION_NPC const * pContExpeditionNpc = NULL;	// 이 NPC가 원정대 NPC인지 검사한다.
	g_kTblDataMgr.GetContDef(pContExpeditionNpc);

	if( pContExpeditionNpc )
	{
		m_kExpeditionNpc = (pContExpeditionNpc->end() != pContExpeditionNpc->find(rkGuid)); // 원정대 NPC이면 참
	}

	return true;
}

void PgNpc::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
{
	CUnit::WriteToPacket(rkPacket, kWriteType);//NPC는 무조건 심플?

	rkPacket.Push(m_kName);
	rkPacket.Push(GetID());
	rkPacket.Push(GetPos());

	rkPacket.Push(m_kActorName);
	rkPacket.Push(m_kMenuStr);
	rkPacket.Push(NpcType());

	rkPacket.Push(m_kCommon);
	rkPacket.Push(GetAbil(AT_C_MAX_HP));
	rkPacket.Push(GetAbil(AT_HP));
	rkPacket.Push(GetAbil(AT_C_MAX_MP));
	rkPacket.Push(GetAbil(AT_MP));
	//	rkPacket.Push(GetAbil(AT_C_MOVESPEED));
}

EWRITETYPE PgNpc::ReadFromPacket(BM::Stream &rkPacket)
{
	EWRITETYPE kWriteType = CUnit::ReadFromPacket(rkPacket);

	POINT3 ptPos;

	rkPacket.Pop(m_kName);
	rkPacket.Pop(m_kGuid);
	rkPacket.Pop(ptPos);

	rkPacket.Pop(m_kActorName);
	rkPacket.Pop(m_kMenuStr);
	rkPacket.Pop(m_kNpcType);

	SetPos(ptPos);

	int iHP = 0, iMaxHP = 0;
	int iMP = 0, iMaxMP = 0;

	rkPacket.Pop(m_kCommon);
	rkPacket.Pop(iMaxHP);
	rkPacket.Pop(iHP);
	rkPacket.Pop(iMaxMP);
	rkPacket.Pop(iMP);

	//////////////////
	SetAbil(AT_C_MAX_HP, iMaxHP);
	SetAbil(AT_HP, iHP);
	SetAbil(AT_C_MAX_MP, iMaxMP);
	SetAbil(AT_MP, iMP);
	return kWriteType;
}

void PgNpc::Invalidate()
{
	SetID(BM::GUID::NullData());

	CUnit::Invalidate();
}

void PgNpc::Init()
{
	CUnit::Init();
	m_kActorName = _T("");
	m_kMenuStr = _T("");
	m_kMenu.clear();
	m_kNpcType = E_NPC_TYPE_FIXED;
}

bool PgNpc::IsCanMenu(ENpcMenuType const eType)
{	
  bool bRet = m_kMenu.end() != m_kMenu.find(eType);
  if( false == bRet )
  {
	  if( (m_kMenu.end() != m_kMenu.find(NMT_Expedition))
		  && ExpeditionNpc() )
	  {
		  return true;
	  }
	  else 
	  {
		  return false;
	  }
  }
  return bRet;
}
