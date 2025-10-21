#include "stdafx.h"
#include "Lohengrin/Dbtables2.h"
#include "Variant/Global.h"
#include "Variant/PgBattleSquare.h"
#include "PgBSGame.h"
#include "PgServerSetMgr.h"
#include "variant/PgStringUtil.h"
#include "Variant/PgDBCache.h"
#include "CSVQuery.h"

namespace BSGameUtil
{
	typedef std::list< BM::vstring > ContErrorMsg;
	ContErrorMsg kErrorMsg;
	CONT_BS_ITEM kDefItemPoint;

	//
	PgVerify::PgVerify()
	{
		g_kTblDataMgr.GetContDef(m_pkDefEffect);
		g_kTblDataMgr.GetContDef(m_pkDefItem);
	}
	PgVerify::~PgVerify()
	{
	}
	bool PgVerify::Check()
	{
		bool bRet = true;

		CONT_BS_ITEM::const_iterator iter = kDefItemPoint.begin();
		while( kDefItemPoint.end() != iter )
		{
			CONT_BS_ITEM::mapped_type const& rkPointItem = (*iter).second;
			if( m_pkDefItem->end() == m_pkDefItem->find(rkPointItem.iItemNo) )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't find effect in TB_DefEffect, TB_BattleSquare_Item[ItemNo:"<<rkPointItem.iItemNo<<L"]" );
				bRet = false;
			}

			bRet = CheckEffect(rkPointItem.iItemNo, rkPointItem.iEffect1) && bRet;
			bRet = CheckEffect(rkPointItem.iItemNo, rkPointItem.iEffect2) && bRet;
			bRet = CheckEffect(rkPointItem.iItemNo, rkPointItem.iEffect3) && bRet;
			++iter;
		}
		kDefItemPoint.swap(CONT_BS_ITEM());
		return bRet && kErrorMsg.empty();
	}
	bool PgVerify::CheckEffect(int const iItemNo, int const iEffectNo)
	{
		if( 0 == iEffectNo )
		{
			return true;
		}


		if( m_pkDefEffect )
		{
			if( m_pkDefEffect->end() != m_pkDefEffect->find(iEffectNo) )
			{
				return true;
			}
		}
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Can't find effect in TB_DefEffect, TB_BattleSquare_Item[ItemNo:"<<iItemNo<<L", EffectNo:"<<iEffectNo<<L"]" );
		return false;
	}

	//
	struct FindGameIDX : public std::binary_function< PgBSContentsGameExt const&, int, bool >
	{
		FindGameIDX(int const iGameIDX)
			: iFindGameIDX(iGameIDX)
		{}
		bool operator() (PgBSContentsGameExt const& lhs)
		{
			return iFindGameIDX == lhs.GameInfo().iGameIDX;
		}
	private:
		int const iFindGameIDX;
	};

	//
	bool IsCanTime(__int64 const iNowTime, __int64 const iStartTime, __int64 const iEndTime)
	{
		if( iStartTime <= iNowTime
		&&	iEndTime >= iNowTime )
		{
			return true;
		}
		return false;
	}
	bool IsInWeek(int const iWeekOfDay, int const iTodayWeekOfDay)
	{
		return 0 != (iWeekOfDay & (1 << iTodayWeekOfDay));
	}

	typedef std::list< BM::vstring > CONT_ERROR_MSG;
	CONT_ERROR_MSG kContErrorMsg;
	void AddErrorMsg(BM::vstring const& rkErrorMsg)
	{
		kContErrorMsg.push_back( rkErrorMsg );
	}
	bool Display()
	{
		CONT_ERROR_MSG::const_iterator iter = kContErrorMsg.begin();
		while( kContErrorMsg.end() != iter )
		{
			CAUTION_LOG(BM::LOG_LV1, (*iter));
			++iter;
		}
		return !kContErrorMsg.empty();
	}

	struct AddError
	{
		AddError()
		{
		}
		void operator()(BM::vstring const& rhs)
		{
			AddErrorMsg(rhs);
		}
	};

	//
	bool Q_DQT_LOAD_BS_GAME(CEL::DB_RESULT &rkResult)
	{
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		CEL::DB_RESULT_COUNT const& rkCountVec = rkResult.vecResultCount;
		CEL::DB_RESULT_COUNT::const_iterator count_iter = rkCountVec.begin();

		CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

		CONT_BS_GAME kBSGame;
		CONT_BS_ITEM kItemMap;

		typedef std::map< int, CONT_BS_WIN_TEAM_REWARD_ITEM > CONT_IDX_2_WIN_TEAM_REWARD_ITEM;
		typedef std::map< int, CONT_BS_PRIVATE_REWARD_ITEM > CONT_IDX_2_PRIVATE_REWARD_ITEM;
		typedef std::map< int, CONT_BS_PRIVATE_LEVEL_REWARD_ITEM > CONT_IDX_2_PRIVATE_LEVEL_REWARD_ITEM;
		typedef std::map< int, CONT_BS_WIN_BONUS_REWARD_ITEM > CONT_IDX_2_WIN_TEAM_REWARD_ITEM;
		CONT_IDX_2_PRIVATE_LEVEL_REWARD_ITEM kContLevelRewardItem;
		CONT_IDX_2_PRIVATE_REWARD_ITEM kContPrivateRewardItem;
		CONT_IDX_2_WIN_TEAM_REWARD_ITEM kContWinTeamReardItem;
		CONT_IDX_2_WIN_TEAM_REWARD_ITEM kContWinBonusRewardItem;
		CONT_IDX_2_WIN_TEAM_REWARD_ITEM kContWinTeamReardItemLose;

		GET_DEF(CItemDefMgr, kItemDefMgr);

		DBCacheUtil::PgNationCodeHelper< CONT_BS_GAME::key_type, CONT_BS_GAME::mapped_type, CONT_BS_GAME, BM::vstring, AddError > kNationCodeUtil( L"Duplicate ItemNo[" DBCACHE_KEY_PRIFIX L"]" );

		{ // Item
			int const iItemCount = (*count_iter);	++count_iter;
			for( int iCur = 0; iItemCount > iCur; ++iCur )
			{
				CONT_BS_ITEM::mapped_type kNewElement;
				result_iter->Pop( kNewElement.iItemNo );		++result_iter;
				result_iter->Pop( kNewElement.iEffect1 );		++result_iter;
				result_iter->Pop( kNewElement.iEffect2 );		++result_iter;
				result_iter->Pop( kNewElement.iEffect3 );		++result_iter;
				result_iter->Pop( kNewElement.iMinimapIcon );	++result_iter;

				if( NULL == kItemDefMgr.GetDef(kNewElement.iItemNo) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Can't find Item NO in TB_DEFITEM, TB_BattleSquare_Item[ItemNo:"<<kNewElement.iItemNo<<L"]";
					continue;
				}

				auto kRet = kItemMap.insert( std::make_pair(kNewElement.iItemNo, kNewElement) );
				if( !kRet.second )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"duplicate Item No in TB_DefBattleSquare_ItemPoint[ItemNo:"<<kNewElement.iItemNo<<L"]";
				}
			}
		}
		{ // Team Reward Item
			int const iRewardCount = (*count_iter);	++count_iter;
			for( int iCur = 0; iRewardCount > iCur; ++iCur )
			{
				int iRewardIdx = 0;
				CONT_BS_WIN_TEAM_REWARD_ITEM::value_type kNewElement;
				result_iter->Pop( iRewardIdx );					++result_iter;
				result_iter->Pop( kNewElement.iMinPoint );		++result_iter;
				result_iter->Pop( kNewElement.iItemNo1 );		++result_iter;
				result_iter->Pop( kNewElement.iCount1 );		++result_iter;
				result_iter->Pop( kNewElement.iItemNo2 );		++result_iter;
				result_iter->Pop( kNewElement.iCount2 );		++result_iter;

				if( 0 != kNewElement.iItemNo1
				&&	NULL == kItemDefMgr.GetDef(kNewElement.iItemNo1) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Can't find Item NO in TB_DEFITEM, TB_BattleSquare_TeamReward[TeamRewardIdx:"<<iRewardIdx<<L", PointMin:"<<kNewElement.iMinPoint<<L", ItemNo1:"<<kNewElement.iItemNo1<<L", Count1:"<<kNewElement.iCount1<<L"]";
					continue;
				}
				if( 0 != kNewElement.iItemNo2
				&&	NULL == kItemDefMgr.GetDef(kNewElement.iItemNo2) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Can't find Item NO in TB_DEFITEM, TB_BattleSquare_TeamReward[TeamRewardIdx:"<<iRewardIdx<<L", PointMin:"<<kNewElement.iMinPoint<<L", ItemNo2:"<<kNewElement.iItemNo2<<L", Count2:"<<kNewElement.iCount2<<L"]";
					continue;
				}
				
				if( !PgBSGame::AddRewardItem(kContWinTeamReardItem[iRewardIdx], kNewElement) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Duplicate [MinPoint], TB_BattleSquare_TeamReward[TeamRewardIdx:"<<iRewardIdx<<L", PointMin:"<<kNewElement.iMinPoint<<L", ItemNo:"<<kNewElement.iItemNo1<<L", Count:"<<kNewElement.iCount1<<L"], ItemNo:"<<kNewElement.iItemNo2<<L", Count:"<<kNewElement.iCount2<<L"]";
				}
			}
		}
		{ // Team Reward Item Lose
			int const iRewardCount = (*count_iter);	++count_iter;
			for( int iCur = 0; iRewardCount > iCur; ++iCur )
			{
				int iRewardIdx = 0;
				CONT_BS_WIN_TEAM_REWARD_ITEM::value_type kNewElement;
				result_iter->Pop( iRewardIdx );					++result_iter;
				result_iter->Pop( kNewElement.iMinPoint );		++result_iter;
				result_iter->Pop( kNewElement.iItemNo1 );		++result_iter;
				result_iter->Pop( kNewElement.iCount1 );		++result_iter;
				result_iter->Pop( kNewElement.iItemNo2 );		++result_iter;
				result_iter->Pop( kNewElement.iCount2 );		++result_iter;

				if( 0 != kNewElement.iItemNo1
				&&	NULL == kItemDefMgr.GetDef(kNewElement.iItemNo1) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Can't find Item NO in TB_DEFITEM, TB_BattleSquare_TeamReward[TeamRewardIdx:"<<iRewardIdx<<L", PointMin:"<<kNewElement.iMinPoint<<L", ItemNo1:"<<kNewElement.iItemNo1<<L", Count1:"<<kNewElement.iCount1<<L"]";
					continue;
				}
				if( 0 != kNewElement.iItemNo2
				&&	NULL == kItemDefMgr.GetDef(kNewElement.iItemNo2) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Can't find Item NO in TB_DEFITEM, TB_BattleSquare_TeamReward[TeamRewardIdx:"<<iRewardIdx<<L", PointMin:"<<kNewElement.iMinPoint<<L", ItemNo2:"<<kNewElement.iItemNo2<<L", Count2:"<<kNewElement.iCount2<<L"]";
					continue;
				}
				
				if( !PgBSGame::AddRewardItem(kContWinTeamReardItemLose[iRewardIdx], kNewElement) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Duplicate [MinPoint], TB_BattleSquare_TeamReward[TeamRewardIdx:"<<iRewardIdx<<L", PointMin:"<<kNewElement.iMinPoint<<L", ItemNo:"<<kNewElement.iItemNo1<<L", Count:"<<kNewElement.iCount1<<L"], ItemNo:"<<kNewElement.iItemNo2<<L", Count:"<<kNewElement.iCount2<<L"]";
				}
			}
		}
		{ // Private Reward Item
			int const iRewardCount = (*count_iter);	++count_iter;
			for( int iCur = 0; iRewardCount > iCur; ++iCur )
			{
				int iRewardIdx = 0;
				CONT_BS_PRIVATE_REWARD_ITEM::value_type kNewElement;
				result_iter->Pop( iRewardIdx );					++result_iter;
				result_iter->Pop( kNewElement.iMinPoint );		++result_iter;
				result_iter->Pop( kNewElement.iItemNo1 );		++result_iter;
				result_iter->Pop( kNewElement.iCount1 );		++result_iter;
				result_iter->Pop( kNewElement.iItemNo2 );		++result_iter;
				result_iter->Pop( kNewElement.iCount2 );		++result_iter;

				if( 0 != kNewElement.iItemNo1
					&&	NULL == kItemDefMgr.GetDef(kNewElement.iItemNo1) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Can't find Item NO in TB_DEFITEM, TB_BattleSquare_PrivateReward[PtReardIdx:"<<iRewardIdx<<L", PointMin:"<<kNewElement.iMinPoint<<L", ItemNo1:"<<kNewElement.iItemNo1<<L", Count1:"<<kNewElement.iCount1<<L"]";
					continue;
				}
				if( 0 != kNewElement.iItemNo2
					&&	NULL == kItemDefMgr.GetDef(kNewElement.iItemNo2) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Can't find Item NO in TB_DEFITEM, TB_BattleSquare_PrivateReward[PtReardIdx:"<<iRewardIdx<<L", PointMin:"<<kNewElement.iMinPoint<<L", ItemNo2:"<<kNewElement.iItemNo2<<L", Count2:"<<kNewElement.iCount2<<L"]";
					continue;
				}
				if( !PgBSGame::AddRewardItem(kContPrivateRewardItem[iRewardIdx], kNewElement) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Duplicate [MinPoint], TB_BattleSquare_PrivateReward[PtReardIdx:"<<iRewardIdx<<L", PointMin:"<<kNewElement.iMinPoint<<L", ItemNo:"<<kNewElement.iItemNo1<<L", Count:"<<kNewElement.iCount1<<L"], ItemNo:"<<kNewElement.iItemNo2<<L", Count:"<<kNewElement.iCount2<<L"]";
				}
			}
		}
		{ // Private Level Reward Item
			int const iRewardCount = (*count_iter);	++count_iter;
			for( int iCur = 0; iRewardCount > iCur; ++iCur )
			{
				int iRewardIdx = 0;
				CONT_BS_PRIVATE_LEVEL_REWARD_ITEM::value_type kNewElement;
				result_iter->Pop( iRewardIdx );					++result_iter;
				result_iter->Pop( kNewElement.iMinPoint );		++result_iter;
				result_iter->Pop( kNewElement.iItemNo1 );		++result_iter;
				result_iter->Pop( kNewElement.iCount1 );		++result_iter;
				result_iter->Pop( kNewElement.iItemNo2 );		++result_iter;
				result_iter->Pop( kNewElement.iCount2 );		++result_iter;

				if( 0 != kNewElement.iItemNo1
					&&	NULL == kItemDefMgr.GetDef(kNewElement.iItemNo1) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Can't find Item NO in TB_DEFITEM, TB_BattleSquare_PrivateLevelReward[LevelRewardIdx:"<<iRewardIdx<<L", PointMin:"<<kNewElement.iMinPoint<<L", ItemNo1:"<<kNewElement.iItemNo1<<L", Count1:"<<kNewElement.iCount1<<L"]";
					continue;
				}
				if( 0 != kNewElement.iItemNo2
					&&	NULL == kItemDefMgr.GetDef(kNewElement.iItemNo2) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Can't find Item NO in TB_DEFITEM, TB_BattleSquare_PrivateLevelReward[LevelRewardIdx:"<<iRewardIdx<<L", PointMin:"<<kNewElement.iMinPoint<<L", ItemNo2:"<<kNewElement.iItemNo2<<L", Count2:"<<kNewElement.iCount2<<L"]";
					continue;
				}
				if( !PgBSGame::AddRewardItem(kContLevelRewardItem[iRewardIdx], kNewElement) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Duplicate [MinPoint], TB_BattleSquare_PrivateLevelReward[LevelRewardIdx:"<<iRewardIdx<<L", LevelMin:"<<kNewElement.iMinPoint<<L", ItemNo:"<<kNewElement.iItemNo1<<L", Count:"<<kNewElement.iCount1<<L"], ItemNo:"<<kNewElement.iItemNo2<<L", Count:"<<kNewElement.iCount2<<L"]";
				}
			}
		}
		{ // Win Bonus Reward Item
			int const iRewardCount = (*count_iter);	++count_iter;
			for( int iCur = 0; iRewardCount > iCur; ++iCur )
			{
				int iRewardIdx = 0;
				CONT_BS_PRIVATE_LEVEL_REWARD_ITEM::value_type kNewElement;
				result_iter->Pop( iRewardIdx );					++result_iter;
				result_iter->Pop( kNewElement.iMinPoint );		++result_iter;
				result_iter->Pop( kNewElement.iItemNo1 );		++result_iter;
				result_iter->Pop( kNewElement.iCount1 );		++result_iter;
				result_iter->Pop( kNewElement.iItemNo2 );		++result_iter;
				result_iter->Pop( kNewElement.iCount2 );		++result_iter;

				if( 0 != kNewElement.iItemNo1
					&&	NULL == kItemDefMgr.GetDef(kNewElement.iItemNo1) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Can't find Item NO in TB_DEFITEM, TB_BattleSquare_WinBonusReward[BnsRewardIdx:"<<iRewardIdx<<L", PointMin:"<<kNewElement.iMinPoint<<L", ItemNo1:"<<kNewElement.iItemNo1<<L", Count1:"<<kNewElement.iCount1<<L"]";
					continue;
				}
				if( 0 != kNewElement.iItemNo2
					&&	NULL == kItemDefMgr.GetDef(kNewElement.iItemNo2) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Can't find Item NO in TB_DEFITEM, TB_BattleSquare_WinBonusReward[BnsRewardIdx:"<<iRewardIdx<<L", PointMin:"<<kNewElement.iMinPoint<<L", ItemNo2:"<<kNewElement.iItemNo2<<L", Count2:"<<kNewElement.iCount2<<L"]";
					continue;
				}
				if( !PgBSGame::AddRewardItem(kContWinBonusRewardItem[iRewardIdx], kNewElement) )
				{
					std::back_inserter(kErrorMsg) = BM::vstring() << L"Duplicate [MinPoint], TB_BattleSquare_WinBonusReward[BnsRewardIdx:"<<iRewardIdx<<L", LevelMin:"<<kNewElement.iMinPoint<<L", ItemNo:"<<kNewElement.iItemNo1<<L", Count:"<<kNewElement.iCount1<<L"], ItemNo:"<<kNewElement.iItemNo2<<L", Count:"<<kNewElement.iCount2<<L"]";
				}
			}
		}
		{ // Game
			CONT_DEF_BS_GEN_GROUND kContBSGenGround;

			int const iGameCount = (*count_iter);	++count_iter;
			for( int iCur = 0; iGameCount > iCur; ++iCur )
			{
				SBSGame kNewElement;

				bool bWeek = false;
				int iWeekCount = 0;
				BM::DBTIMESTAMP_EX kTempTime;
				int iTeamReawrdIdx = 0, iPtRewardIdx = 0, iLevelReawrdIdx = 0, iBonusRewardIdx = 0, iTeamReawrdLoseIdx = 0;
				std::wstring kNationCodeStr;

				result_iter->Pop( kNationCodeStr );						++result_iter;
				result_iter->Pop( kNewElement.iGameIDX );				++result_iter;
				result_iter->Pop( kNewElement.bUse );					++result_iter;
				result_iter->Pop( kNewElement.iChannelNameTextID );		++result_iter;
				result_iter->Pop( kNewElement.iLevelMin );				++result_iter;
				result_iter->Pop( kNewElement.iLevelMax );				++result_iter;
				result_iter->Pop( kNewElement.iMaxUser );				++result_iter;
				result_iter->Pop( bWeek );								++result_iter;	if( bWeek )	{ kNewElement.iWeekOfDay |= (1<<iWeekCount); };	++iWeekCount;//일
				result_iter->Pop( bWeek );								++result_iter;	if( bWeek )	{ kNewElement.iWeekOfDay |= (1<<iWeekCount); };	++iWeekCount;
				result_iter->Pop( bWeek );								++result_iter;	if( bWeek )	{ kNewElement.iWeekOfDay |= (1<<iWeekCount); };	++iWeekCount;
				result_iter->Pop( bWeek );								++result_iter;	if( bWeek )	{ kNewElement.iWeekOfDay |= (1<<iWeekCount); };	++iWeekCount;
				result_iter->Pop( bWeek );								++result_iter;	if( bWeek )	{ kNewElement.iWeekOfDay |= (1<<iWeekCount); };	++iWeekCount;
				result_iter->Pop( bWeek );								++result_iter;	if( bWeek )	{ kNewElement.iWeekOfDay |= (1<<iWeekCount); };	++iWeekCount;
				result_iter->Pop( bWeek );								++result_iter;	if( bWeek )	{ kNewElement.iWeekOfDay |= (1<<iWeekCount); };	++iWeekCount;//토
				result_iter->Pop( kTempTime );							++result_iter;	kNewElement.kStartTime = BM::PgPackedTime(kTempTime);
				result_iter->Pop( kNewElement.iGameSec );				++result_iter;
				result_iter->Pop( kNewElement.iGroundNo );				++result_iter;
				result_iter->Pop( kNewElement.iPreOpenSec );			++result_iter;
				result_iter->Pop( iTeamReawrdIdx );						++result_iter;// TeamReward
				result_iter->Pop( iTeamReawrdLoseIdx );					++result_iter;// TeamReward Lose
				result_iter->Pop( iPtRewardIdx );						++result_iter;// PointReward
				result_iter->Pop( iLevelReawrdIdx );					++result_iter;// LevelReward
				result_iter->Pop( iBonusRewardIdx );					++result_iter;// BnsReward
				result_iter->Pop( kNewElement.iMapBagItemGroundNo );	++result_iter;
				result_iter->Pop( kNewElement.iGenGroupGroundNo );		++result_iter;
				result_iter->Pop( kNewElement.iMonsterBagControlNo );	++result_iter;
								
				if( 0 < kNewElement.iPreOpenSec )
				{
					kNewElement.iPreOpenSec = -kNewElement.iPreOpenSec;
				}				

				//auto kRet = kBSGame.insert( std::make_pair(kNewElement.iGameIDX, PgBSGame(kNewElement)) );
				//if( kRet.second )
				{
					PgBSGame kBSGame(kNewElement);
					CONT_BS_GAME::mapped_type& rkBsGame = kBSGame;	//(*kRet.first).second;
					rkBsGame.SetBSItem( kItemMap );
					{
						CONT_IDX_2_PRIVATE_LEVEL_REWARD_ITEM::const_iterator iter = kContLevelRewardItem.find(iLevelReawrdIdx);
						if( kContLevelRewardItem.end() != iter )
						{
							rkBsGame.AddPrivateLevelRewardItem((*iter).second);
						}
						else
						{
							std::back_inserter(kErrorMsg) = BM::vstring() << L"wrong [f_LvRewardIdx], TB_DefBS_Game[GameIDX:"<<kNewElement.iGameIDX<<L", "<<iLevelReawrdIdx<<L"]";
						}
					}
					{
						CONT_IDX_2_PRIVATE_REWARD_ITEM::const_iterator iter = kContPrivateRewardItem.find(iPtRewardIdx);
						if(kContPrivateRewardItem.end() != iter)
						{
							rkBsGame.AddPrivateRewardItem((*iter).second);
						}
						else
						{
							std::back_inserter(kErrorMsg) = BM::vstring() << L"wrong [f_PtRewardIdx], TB_DefBS_Game[GameIDX:"<<kNewElement.iGameIDX<<L", "<<iPtRewardIdx<<L"]";
						}
					}
					{
						CONT_IDX_2_WIN_TEAM_REWARD_ITEM::const_iterator iter =  kContWinBonusRewardItem.find(iBonusRewardIdx);
						if(kContWinBonusRewardItem.end() != iter)
						{
							rkBsGame.AddWinBonusRewardItem((*iter).second);
						}
						else
						{
							std::back_inserter(kErrorMsg) = BM::vstring() << L"wrong [f_BndRewardIdx], TB_DefBS_Game[GameIDX:"<<kNewElement.iGameIDX<<L", "<<iBonusRewardIdx<<L"]";
						}
					}
					{
						CONT_IDX_2_WIN_TEAM_REWARD_ITEM::const_iterator iter =  kContWinTeamReardItem.find(iTeamReawrdIdx);
						if(kContWinTeamReardItem.end() != iter)
						{
							rkBsGame.AddWinTeamRewardItem((*iter).second);
						}
						else
						{
							std::back_inserter(kErrorMsg) = BM::vstring() << L"wrong [f_TeamRewardIdx], TB_DefBS_Game[GameIDX:"<<kNewElement.iGameIDX<<L", "<<iTeamReawrdIdx<<L"]";
						}
					}
					{
						CONT_IDX_2_WIN_TEAM_REWARD_ITEM::const_iterator iter =  kContWinTeamReardItemLose.find(iTeamReawrdLoseIdx);
						if(kContWinTeamReardItemLose.end() != iter)
						{
							rkBsGame.AddLoseTeamRewardItem((*iter).second);
						}
						else
						{
							std::back_inserter(kErrorMsg) = BM::vstring() << L"wrong [f_TeamRewardLoseIdx], TB_DefBS_Game[GameIDX:"<<kNewElement.iGameIDX<<L", "<<iTeamReawrdLoseIdx<<L"]";
						}
					}

					kNationCodeUtil.Add(kNationCodeStr, kNewElement.iGameIDX, rkBsGame, __FUNCTIONW__, __LINE__);
				}

				{
					static CONT_DEF_BS_GEN_GROUND::mapped_type kNullData;
					auto kPair = kContBSGenGround.insert(std::make_pair(kNewElement.iGroundNo, kNullData));
					kPair.first->second.insert(kNewElement.iGenGroupGroundNo);
				}
			}
			
			g_kTblDataMgr.SetContDef(kContBSGenGround);
		}

		if( kErrorMsg.empty() )
		{
			kDefItemPoint = kItemMap;

			BM::Stream kPacket(PT_N_N_NFY_RELOAD_BS_GAME);
			PU::TWriteTable_AM(kPacket, kNationCodeUtil.GetResult());
			::SendToRealmContents(PMET_BATTLESQUARE, kPacket);
		}
		else
		{
			ContErrorMsg::const_iterator iter = kErrorMsg.begin();
			while( kErrorMsg.end() != iter )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << (*iter));
				++iter;
			}
		}

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}
	bool Q_DQT_BATTLE_SQUARE_COMMON(CEL::DB_RESULT &rkResult)
	{
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
			return false;
		}
		g_kCoreCenter.ClearQueryResult(rkResult);
		return true;
	}

	//
	PgBSStatusMng::PgBSStatusMng()
		: m_kSQLGuid()
	{
	}
	PgBSStatusMng::~PgBSStatusMng()
	{
	}
	void PgBSStatusMng::Init()
	{
		CEL::DB_QUERY kQuery(DT_MEMBER, DQT_INIT_BS_STATUS, _T("EXEC [DBO].[UP_BS_Init_Status]"));
		kQuery.InsertQueryTarget(m_kSQLGuid);
		kQuery.PushStrParam( static_cast< int >(g_kProcessCfg.ServerIdentity().nRealm) );
		kQuery.QueryOwner(m_kSQLGuid);
		g_kCoreCenter.PushQuery(kQuery);
	}
	void PgBSStatusMng::Update(int const iGameIDX, int const iChannelNo, EBS_GAME_STATUS const eStatus)
	{
		CEL::DB_QUERY kQuery(DT_MEMBER, DQT_UPDATE_BS_STATUS, _T("EXEC [DBO].[UP_BS_Update_Status]"));
		kQuery.InsertQueryTarget(m_kSQLGuid);
		kQuery.PushStrParam( iGameIDX );
		kQuery.PushStrParam( static_cast< int >(g_kProcessCfg.ServerIdentity().nRealm) );
		kQuery.PushStrParam( static_cast< int >(iChannelNo) );
		kQuery.PushStrParam( static_cast< int >(eStatus) );
		kQuery.QueryOwner(m_kSQLGuid);
		g_kCoreCenter.PushQuery(kQuery);
	}
	void PgBSStatusMng::Update(SBSGame const& rkBSGame, EBS_GAME_STATUS const eStatus)
	{
		Update(rkBSGame.iGameIDX, -1, eStatus);
	}
};

//
PgBSContentsGameExt::PgBSGameUserControl::PgBSGameUserControl()
	: m_kContReserveUser(), m_kContPlayer()
{
}
PgBSContentsGameExt::PgBSGameUserControl::~PgBSGameUserControl()
{
}
EBS_JOIN_RETURN PgBSContentsGameExt::PgBSGameUserControl::Reserve(SBSGame const& rkGameInfo, EBS_GAME_STATUS const eStatus, BM::GUID const& rkGuid, int const iLevel)
{
	switch( eStatus )
	{
	case BSGS_NOW_GAME:
	case BSGS_NOW_PREOPEN:
		{
			// 통과
		}break;
	default:
		{
			return BSJR_NOTOPEN;
		}break;
	}

	if( rkGameInfo.iLevelMin > iLevel
	||	rkGameInfo.iLevelMax < iLevel )
	{
		return BSJR_LEVEL;
	}

	if( rkGameInfo.iMaxUser <= CurUserCount() )
	{
		return BSJR_MAX;
	}

	if( m_kContReserveUser.end() != m_kContReserveUser.find(rkGuid) )
	{
		return BSJR_DUPLICATE;
	}

	m_kContReserveUser.insert( rkGuid );
	return BSJR_SUCCESS;
}
bool PgBSContentsGameExt::PgBSGameUserControl::Join(BM::GUID const& rkGuid)
{
	if( m_kContReserveUser.end() != m_kContReserveUser.find(rkGuid) )
	{
		m_kContReserveUser.erase( rkGuid );
		m_kContPlayer.insert( rkGuid );
		return true;
	}
	return false;
}
void PgBSContentsGameExt::PgBSGameUserControl::Leave(BM::GUID const& rkGuid)
{
	m_kContReserveUser.erase( rkGuid );
	m_kContPlayer.erase( rkGuid );
}
int PgBSContentsGameExt::PgBSGameUserControl::CurUserCount() const
{
	return static_cast< int >(m_kContPlayer.size() + m_kContReserveUser.size());
}
void PgBSContentsGameExt::PgBSGameUserControl::ClearUser()
{
	m_kContReserveUser.clear();
	m_kContPlayer.clear();
}

//
PgBSContentsGameExt::PgBSContentsGameExt()
	: PgBSContentsGame(), m_kUser()
{
}
PgBSContentsGameExt::PgBSContentsGameExt(PgBSGame const& rhs)
	: PgBSContentsGame(rhs), m_kUser()
{
}
PgBSContentsGameExt::~PgBSContentsGameExt()
{
}
void PgBSContentsGameExt::Status(EBS_GAME_STATUS const& eStatus, BSGameUtil::PgBSStatusMng& rkStatusMng)
{
	if( m_kStatus != eStatus )
	{
		rkStatusMng.Update(GameInfo(), eStatus);
	}
	m_kStatus = eStatus;
}
bool PgBSContentsGameExt::MakeTime(EBS_GAME_STATUS const eStatus, BSGameUtil::PgBSStatusMng& rkStatusMng, bool const bForce)
{
	// 시작 시간, 오늘 날자
	// 종료 시간, 오늘 날자 + 게임 시간
	// 아직 지나지 않았으면 (시작 시간보다 현재가 작다) --> 대기
	// 이미 지나갔으면 (종료 시간보다 현재가 이후이다) --> 날자 + 1일 해서 다시 날자를 제작한다

	__int64 iNowSecTime = 0;
	SYSTEMTIME kNowDateTime;
	::GetLocalTime(&kNowDateTime);
	if( !CGameTime::SystemTime2SecTime(kNowDateTime, iNowSecTime) )
	{
		return false;
	}

	if( false == bForce )
	{
		if( 0 != m_kEndTime
		&&	iNowSecTime < m_kEndTime ) // 아직 종료 되지 않았음
		{
			return true;
		}
	}

	SYSTEMTIME kStartDateTime(kNowDateTime);
	kStartDateTime.wHour = static_cast< WORD >(m_kGameInfo.kStartTime.Hour());
	kStartDateTime.wMinute = static_cast< WORD >(m_kGameInfo.kStartTime.Min());
	kStartDateTime.wSecond = static_cast< WORD >(m_kGameInfo.kStartTime.Sec());
	__int64 iStartSecTime = 0;
	CGameTime::SystemTime2SecTime(kStartDateTime, iStartSecTime);

	SYSTEMTIME kEndDateTime;
	__int64 iEndSecTime = iStartSecTime + m_kGameInfo.iGameSec;
	CGameTime::SecTime2SystemTime(iEndSecTime, kEndDateTime);

	__int64 iPreOpenTime = iStartSecTime + m_kGameInfo.iPreOpenSec;

	if( iEndSecTime < iNowSecTime ) // 이미 지난 이벤트
	{
		__int64 const iOneDay = CGameTime::OneDay / CGameTime::SECOND;
		iStartSecTime += iOneDay;
		iEndSecTime += iOneDay;
		iPreOpenTime += iOneDay;
	}

	m_kStartTime = iStartSecTime;
	m_kEndTime = iEndSecTime;
	m_kPreOpenTime = iPreOpenTime;

	Status(eStatus, rkStatusMng);
	return true;
}


//
PgBSGameMngImpl::PgBSGroundGameMng::PgBSGroundGameMng()
{
}
PgBSGameMngImpl::PgBSGroundGameMng::~PgBSGroundGameMng()
{
}
bool PgBSGameMngImpl::PgBSGroundGameMng::Add(PgBSContentsGame const& rkBSGame)
{
	auto kRet = m_kGndGame.insert( std::make_pair(rkBSGame.GameInfo().iGroundNo, rkBSGame.GameInfo().iGameIDX) );
	return kRet.second;
}
bool PgBSGameMngImpl::PgBSGroundGameMng::Del(PgBSContentsGame const& rkBSGame)
{
	CONT_BS_GND_GAME::iterator find_iter = m_kGndGame.find(rkBSGame.GameInfo().iGroundNo);
	if( m_kGndGame.end() != find_iter )
	{
		m_kGndGame.erase(find_iter);
		return true;
	}
	return false;
}
bool PgBSGameMngImpl::PgBSGroundGameMng::IsEmptyGround(PgBSContentsGame const& rkBSGame) const
{
	return m_kGndGame.end() == m_kGndGame.find(rkBSGame.GameInfo().iGroundNo);
}
bool PgBSGameMngImpl::PgBSGroundGameMng::IsGroundGame(PgBSContentsGame const& rkBSGame) const
{
	CONT_BS_GND_GAME::const_iterator find_iter = m_kGndGame.find(rkBSGame.GameInfo().iGroundNo);
	if( m_kGndGame.end() != find_iter )
	{
		return rkBSGame.GameInfo().iGameIDX == (*find_iter).second;
	}
	return false;
}
int PgBSGameMngImpl::PgBSGroundGameMng::IsGroundGame(int const iGroundNo) const
{
	CONT_BS_GND_GAME::const_iterator find_iter = m_kGndGame.find(iGroundNo);
	if( m_kGndGame.end() != find_iter )
	{
		return (*find_iter).second;
	}
	return 0;
}

//
PgBSGameMngImpl::PgBSUserControl::PgBSUserControl()
{
}
PgBSGameMngImpl::PgBSUserControl::~PgBSUserControl()
{
}
bool PgBSGameMngImpl::PgBSUserControl::Add(BM::GUID const& rkGuid, int const iGameIDX)
{
	auto kRet = m_kContGuidIdx.insert( std::make_pair(rkGuid, iGameIDX) );
	return kRet.second;
}
void PgBSGameMngImpl::PgBSUserControl::Del(BM::GUID const& rkGuid)
{
	m_kContGuidIdx.erase( rkGuid );
}
void PgBSGameMngImpl::PgBSUserControl::DelGame(int const iGameIDX)
{
	CONT_GUID_IDX::iterator iter = m_kContGuidIdx.begin();
	while( m_kContGuidIdx.end() != iter )
	{
		if( iGameIDX == (*iter).second )
		{
			iter = m_kContGuidIdx.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}
int PgBSGameMngImpl::PgBSUserControl::Get(BM::GUID const& rkGuid) const
{
	CONT_GUID_IDX::const_iterator iter = m_kContGuidIdx.find(rkGuid);
	if( m_kContGuidIdx.end() != iter )
	{
		return (*iter).second;
	}
	return -1;
}

//
PgBSGameMngImpl::PgBSGameMngImpl()
	: m_kContBSGame(), m_kStatusMng(), m_kGndGameMng(), m_kUsers(), m_kContLoginedUser()
{
	::GetLocalTime(&m_kDateTime);
}
PgBSGameMngImpl::~PgBSGameMngImpl()
{
}
void PgBSGameMngImpl::BuildTime(bool const bForce)
{
	SYSTEMTIME kNow;
	::GetLocalTime( &kNow );
	__int64 iNowTime = 0;
	CGameTime::SystemTime2SecTime(kNow, iNowTime);
	
	CONT_BS_CONTENTS_GAME_EXT::iterator iter = m_kContBSGame.begin();
	while( m_kContBSGame.end() != iter )
	{
		CONT_BS_CONTENTS_GAME_EXT::value_type& rkBSGame = (*iter);
		switch( rkBSGame.Status() )
		{
		case BSGS_NOW_GAME:
			{
				// 게임중은 건들이지 않는다
			}break;
		case BSGS_NOW_PREOPEN:
			{
				rkBSGame.MakeTime(rkBSGame.Status(), m_kStatusMng, bForce);
				if( iNowTime < rkBSGame.PreOpenTime() )
				{
					rkBSGame.Status(BSGS_WAIT_START, m_kStatusMng); // 시간 조정으로 닫히게 되면
				}
			}break;
		case BSGS_NONE:
			{
				rkBSGame.MakeTime(BSGS_WAIT_START, m_kStatusMng, bForce);
			}break;
		default:
			{
				rkBSGame.MakeTime(rkBSGame.Status(), m_kStatusMng, bForce);
			}break;
		}
		++iter;
	}
}
void PgBSGameMngImpl::NfyAllNotice(PgBSContentsGame const& rkBSGame, BM::Stream& rkOut, EBS_GAME_STATUS const eTempStatus)
{
	rkOut.Push( rkBSGame.GameInfo().iGameIDX );
	rkOut.Push( ((BSGS_NONE == eTempStatus)? rkBSGame.Status(): eTempStatus) );
	rkOut.Push( rkBSGame.GameInfo().iChannelNameTextID );
	rkOut.Push( rkBSGame.GameInfo().iLevelMin );
	rkOut.Push( rkBSGame.GameInfo().iLevelMax );
	rkOut.Push( rkBSGame.LastNoticeTime() );
}
void PgBSGameMngImpl::NfyGndNotice(SYSTEMTIME const& rkNow, __int64 const iNowSecTime, PgBSContentsGame& rkBSGame, bool const bForce)
{
	if( BSGS_NONE == rkBSGame.Status()
	||	!rkBSGame.GameInfo().bUse )
	{
		return;
	}

	__int64 const iOneMinute = 60i64;

	bool bNotice = false;
	EBS_GAME_STATUS eStatus = rkBSGame.Status();
	switch( eStatus )
	{
	case BSGS_WAIT_START:
	case BSGS_NOW_PREOPEN:
		{
			// 시작전 30분 부터 5분 단위
			// 시작전 10분 부터 2분 단위
			// 시작전 5분 부터 1분 단위
			__int64 const iDiffStartSec = iNowSecTime - rkBSGame.StartTime();
			__int64 const iDiffStartMinute = (iDiffStartSec / iOneMinute);
			if( BSGameUtil::IsInWeek(rkBSGame.GameInfo().iWeekOfDay, rkNow.wDayOfWeek)
			&&	rkBSGame.LastNoticeTime() != iDiffStartSec )
			{
				if( -iOneMinute <= iDiffStartSec )
				{
					switch( iDiffStartSec )
					{
					case -60: case -30:
					//case -50: case -40: case -25: case -20: case -15:
					case -10: case -9: case -8: case -7: case -6: case -5: case -4: case -3: case -2: case -1:
						{
							bNotice = true;
						}break;
					}
				}
				else
				{
					if( (rkBSGame.LastNoticeTime()/iOneMinute) != iDiffStartMinute )
					{
						switch( iDiffStartMinute-1 )
						{
						case -30: case -25: case -20: case -15: case -10:
						case -8: case -6: case -5: case -4: case -3: case -2:
							{
								bNotice = true;
							}
						}
					}
				}

				if( bNotice )
				{
					rkBSGame.LastNoticeTime(iDiffStartSec);
				}
			}
			if( !bForce )
			{
				eStatus = BSGS_WAIT_START; // 강제 아닐 땐 시작 대기 공지
			}
		}break;
	case BSGS_NOW_GAME:
		{
			__int64 const iDiffEndSec = rkBSGame.EndTime() - iNowSecTime;
			__int64 const iDiffEndMinute = (iDiffEndSec / iOneMinute);
			// 매 5분 마다
			// 종료 10분 전에는 2분 마다
			// 종료 5분 전에는 1분 마다
			if( rkBSGame.LastNoticeTime() != iDiffEndSec )
			{
				if( 15 >= iDiffEndMinute )
				{
					if( iOneMinute >= iDiffEndSec )
					{
						switch( iDiffEndSec )
						{
						case 60: /*case 30: */
						//case 50: case 40: case 25: case 20: case 15:
						case 10: /* case 9: case 8: case 7: case 6:*/ case 5: case 4: case 3: case 2: case 1:
							{
								bNotice = true;
							}break;
						}
					}
					else
					{
						if( (rkBSGame.LastNoticeTime()/iOneMinute) != iDiffEndMinute )
						{
							switch( iDiffEndMinute+1 )
							{
							/*case 15: case 13: case 11:*/ case 10: /*case 9: case 8: case 7: case 6:*/
							case 5: /*case 4:*/ case 3: /*case 2:*/
								{
									bNotice = true;
								}break;
							}
						}
					}
				}
				else
				{
					if( 5 <= (rkBSGame.LastNoticeTime()/iOneMinute) - iDiffEndMinute )
					{
						bNotice = true;
					}
				}

				if( bNotice )
				{
					rkBSGame.LastNoticeTime(iDiffEndSec);
				}
			}
		}break;
	//case BSGS_REQ_START:
	//case BSGS_REQ_END:
	//	{
	//		bNotice = true;
	//	}break;
	}

	if( bNotice
	||	bForce )
	{
		if( m_kGndGameMng.IsGroundGame(rkBSGame) )
		{
			size_t const iNfyCount = 1; // 항상 1개
			bool const bNotice = true;
			BM::Stream kPacket(PT_N_C_NFY_BS_GND_NOTICE);
			kPacket.Push( rkBSGame.GameInfo().iGroundNo );
			kPacket.Push( iNfyCount );
			NfyAllNotice(rkBSGame, kPacket, eStatus);
			kPacket.Push( bNotice );
			g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, kPacket);
		}
	}
}
void PgBSGameMngImpl::OnTick()
{
	size_t iNfyInfoCount = 0;
	BM::Stream kTempNfyInfoPacket;

	SYSTEMTIME kNow;
	::GetLocalTime( &kNow );
	__int64 iNowTime = 0;
	CGameTime::SystemTime2SecTime(kNow, iNowTime);

	if( kNow.wDay != m_kDateTime.wDay )
	{
		BuildTime();
		m_kDateTime = kNow;
	}

	CONT_BS_CONTENTS_GAME_EXT::iterator iter = m_kContBSGame.begin();
	while( m_kContBSGame.end() != iter )
	{
		CONT_BS_CONTENTS_GAME_EXT::value_type& rkBSGame = (*iter);

		NfyGndNotice(kNow, iNowTime, rkBSGame);

		if( rkBSGame.GameInfo().bUse )
		{
			switch( rkBSGame.Status() )
			{
			case BSGS_NONE:
				{
					rkBSGame.MakeTime(BSGS_WAIT_START, m_kStatusMng);
				}break;
			case BSGS_WAIT_START:
				{
					if( m_kGndGameMng.IsEmptyGround(rkBSGame) )
					{
						if( iNowTime >= rkBSGame.PreOpenTime()
						&&	BSGameUtil::IsInWeek(rkBSGame.GameInfo().iWeekOfDay, kNow.wDayOfWeek) )
						{
							// 시작 가능
							rkBSGame.Status(BSGS_NOW_PREOPEN, m_kStatusMng);
							m_kGndGameMng.Add(rkBSGame);
							NfyBSGameInfo( rkBSGame );

							if( rkBSGame.StartTime() != rkBSGame.PreOpenTime() )
							{
								NfyAllNotice(rkBSGame, kTempNfyInfoPacket);	++iNfyInfoCount;
							}
						}
					}
				}//break; 없다
			case BSGS_NOW_PREOPEN:
				{
					if( m_kGndGameMng.IsEmptyGround(rkBSGame)
					||	m_kGndGameMng.IsGroundGame(rkBSGame) )
					{
						if( BSGameUtil::IsCanTime(iNowTime, rkBSGame.StartTime(), rkBSGame.EndTime())
						&&	BSGameUtil::IsInWeek(rkBSGame.GameInfo().iWeekOfDay, kNow.wDayOfWeek) )
						{
							// 시작 가능
							rkBSGame.Status(BSGS_REQ_START, m_kStatusMng);
							m_kGndGameMng.Add(rkBSGame);
							NfyBSGameInfo( rkBSGame );
						}
					}
				}break;
			case BSGS_REQ_START: // 시작 요청
				{
					NfyAllNotice(rkBSGame, kTempNfyInfoPacket);	++iNfyInfoCount;
					rkBSGame.Status(BSGS_NOW_GAME, m_kStatusMng);
					NfyBSGameInfo( rkBSGame );
				}break;
			case BSGS_NOW_GAME: // 게임 진행중
				{
					if( !BSGameUtil::IsCanTime(iNowTime, rkBSGame.StartTime(), rkBSGame.EndTime()) )
					{
						rkBSGame.Status(BSGS_REQ_END, m_kStatusMng);
					}
				}break;
			case BSGS_REQ_END: // 종료 요청
				{
					NfyAllNotice(rkBSGame, kTempNfyInfoPacket);	++iNfyInfoCount;
					rkBSGame.Status(BSGS_NOW_END, m_kStatusMng);
					NfyBSGameInfo( rkBSGame );
				}break;
			case BSGS_NOW_END: // 게임 종료 되었음
				{
					__int64 const iThreeMinute = 60 * 3;
					if( iThreeMinute <= (iNowTime - rkBSGame.EndTime()) ) // 3분 대기
					{
						rkBSGame.Status(BSGS_NONE, m_kStatusMng);
						m_kGndGameMng.Del(rkBSGame);
						NfyBSGameInfo( rkBSGame );
					}
				}break;
			//
			//case BSGS_REQ_END_ERASE:
			default:
				{
					// 뭥미?
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Wrong Game Status["<< static_cast< int >(rkBSGame.Status()) <<L"]");
				}break;
			}
		}
		++iter;
	}
	if( 0 != kTempNfyInfoPacket.Size() )
	{
		bool const bNotice = true;
		BM::Stream kNfyPacket(PT_N_C_NFY_BS_NOTICE);
		kNfyPacket.Push( iNfyInfoCount );
		kNfyPacket.Push( kTempNfyInfoPacket );
		kNfyPacket.Push( bNotice );
		g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, kNfyPacket);
	}
}
bool PgBSGameMngImpl::ProcessMsg(BM::Stream& rkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	rkPacket.Pop( usType );

	switch( usType )
	{
	case PT_A_N_REQ_INIT_BS_GAME:
		{
			m_kStatusMng.Init();
		}break;
	case PT_A_N_REQ_RELOAD_BS_GAME:
		{
			// reload 기능은 없다
			CEL::DB_QUERY kQuery(DT_DEF, DQT_LOAD_BS_GAME, _T("EXEC [dbo].[UP_LoadDefBattleSquare]"));
			PushCSVQuery(kQuery, true);
		}break;
	case PT_A_N_NFY_BS_SUSPEND:
		{
			SGmOrder kOrder;
			kOrder.ReadFromPacket( &rkPacket );

			CONT_BS_CONTENTS_GAME_EXT::iterator iter = m_kContBSGame.begin();
			while( m_kContBSGame.end() != iter )
			{
				CONT_BS_CONTENTS_GAME_EXT::value_type& rkBSGame = (*iter);
				if( rkBSGame.GameInfo().iGameIDX == kOrder.iValue[0] )
				{
					switch( rkBSGame.Status() )
					{
					case BSGS_NOW_GAME:
						{
							rkBSGame.Status(BSGS_REQ_END, m_kStatusMng); // 중단!!
						}break;
					}
				}
				++iter;
			}
		}break;
	case PT_N_N_NFY_RELOAD_BS_GAME:
		{
			CONT_BS_GAME kBSGame;
			PU::TLoadTable_AM(rkPacket, kBSGame);

			CONT_BS_GAME::const_iterator new_iter = kBSGame.begin();
			while( kBSGame.end() != new_iter )
			{
				m_kContBSGame.push_back( PgBSContentsGameExt((*new_iter).second) );
				++new_iter;
			}
			BuildTime(true);
		}break;
	case PT_A_N_NFY_GAME_STATUS:
		{
			int iGameIDX = 0;
			int iChannelNo = 0;
			EBS_GAME_STATUS eStatus = BSGS_NONE;

			rkPacket.Pop( iGameIDX );
			rkPacket.Pop( iChannelNo );
			rkPacket.Pop( eStatus );

			m_kStatusMng.Update(iGameIDX, iChannelNo, eStatus);
		}break;
	case PT_A_A_REQ_BS_GAME_INFO:
		{
			SGroundKey kGndKey;
			kGndKey.ReadFromPacket( rkPacket );

			int const iGameIDX = m_kGndGameMng.IsGroundGame(kGndKey.GroundNo());
			if( 0 != iGameIDX )
			{
				CONT_BS_CONTENTS_GAME_EXT::const_iterator iter = m_kContBSGame.begin();
				while( m_kContBSGame.end() != iter )
				{
					if( iGameIDX == (*iter).GameInfo().iGameIDX )
					{
						NfyBSGameInfo( (*iter) );
					}
					++iter;
				}
			}
		}break;
		
	case PT_C_T_REQ_BS_CHANNEL_INFO:
		{
			BM::GUID kCharGuid;
			rkPacket.Pop(kCharGuid);

			BM::Stream kChnInfoPacket(PT_T_C_ANS_BS_CHANNEL_INFO);
			kChnInfoPacket.Push(kCharGuid);
			size_t const iWrPos = kChnInfoPacket.WrPos();
			size_t iCount = 0;
			kChnInfoPacket.Push( m_kContBSGame.size() );
			CONT_BS_CONTENTS_GAME_EXT::iterator iter = m_kContBSGame.begin();
			while( m_kContBSGame.end() != iter )
			{
				CONT_BS_CONTENTS_GAME_EXT::value_type const& rkBSGame = (*iter);
				if( rkBSGame.GameInfo().bUse )
				{
					++iCount;
					kChnInfoPacket.Push( rkBSGame.Status() );
					kChnInfoPacket.Push( rkBSGame.CurUserCount() );
					rkBSGame.GameInfo().WriteToPacket( kChnInfoPacket );
				}
				++iter;
			}
			if( 0 != iCount )
			{
				kChnInfoPacket.ModifyData(iWrPos, &iCount, sizeof(iCount));
				g_kRealmUserMgr.Locked_SendToUserGround(kCharGuid, kChnInfoPacket, false, true);
			}
		}break;
	case PT_C_T_REQ_WANT_JOIN_BS_CHANNEL:
		{
			BM::GUID kCharGuid;
			int iGameIDX = 0;
			int iLevel = 0;

			rkPacket.Pop( kCharGuid );
			rkPacket.Pop( iLevel );
			rkPacket.Pop( iGameIDX );

			CONT_BS_CONTENTS_GAME_EXT::iterator find_iter = std::find_if(m_kContBSGame.begin(), m_kContBSGame.end(), BSGameUtil::FindGameIDX(iGameIDX));
			if( m_kContBSGame.end() != find_iter )
			{
				CONT_BS_CONTENTS_GAME_EXT::value_type& rkBSGame = (*find_iter);
				switch( rkBSGame.Status() )
				{
				case BSGS_NOW_PREOPEN:
				case BSGS_NOW_GAME:
					{
						EBS_JOIN_RETURN const eRet = rkBSGame.Reserve(kCharGuid, iLevel);
						switch( eRet )
						{
						case BSJR_SUCCESS:
							{
								rkBSGame.Join(kCharGuid);

								BM::Stream kPacket(PT_T_M_NFY_WANT_JOIN_BS_CHANNEL);
								kPacket.Push( kCharGuid );
								kPacket.Push( rkBSGame.GameInfo().iGroundNo );
								g_kRealmUserMgr.Locked_SendToUserGround(kCharGuid, kPacket, false, true);

								m_kUsers.Add(kCharGuid, rkBSGame.GameInfo().iGameIDX);
							}break;
							/*
						case BSJR_MAX:
							{
								int iNewGameIDX = 0;
								CONT_BS_CONTENTS_GAME_EXT::iterator iter = m_kContBSGame.begin();
								while( m_kContBSGame.end() != iter )
								{
									CONT_BS_CONTENTS_GAME_EXT::value_type& rkNewBSGame = (*iter);
									if( rkNewBSGame.GameInfo().iLevelMin <= iLevel
									&&	rkNewBSGame.GameInfo().iLevelMax >= iLevel
									&&	((BSGS_NOW_PREOPEN == rkNewBSGame.Status()) || (BSGS_NOW_GAME == rkNewBSGame.Status()))
									&&	rkNewBSGame.GameInfo().iMaxUser > rkNewBSGame.CurUserCount() )
									{
										iNewGameIDX = rkNewBSGame.GameInfo().iGameIDX; // 들어갈 수 있는 다른 채널을 찾았다

										BM::Stream kPacket(PT_C_T_REQ_WANT_JOIN_BS_CHANNEL);
										kPacket.Push( kCharGuid );
										kPacket.Push( iLevel );
										kPacket.Push( iNewGameIDX );
										::SendToRealmContents(PMET_BATTLESQUARE, kPacket); // 다른 게임으로 입장 시도
										break;
									}
									++iter;
								}
								if( 0 != iNewGameIDX )
								{
									break; // 에러 패킷을 보내지 않는다
								}
							} // no break;
							*/
						default:
							{
								BM::Stream kPacket(PT_T_C_ANS_WANT_JOIN_BS_CHANNEL);
								kPacket.Push( eRet );
								g_kRealmUserMgr.Locked_SendToUser(kCharGuid, kPacket, false);
							}break;
						}
					}break;
				default:
					{
					}break;
				}
			}
		}break;
	case PT_A_N_NFY_WANT_JOIN_BS_CHANNEL:
		{
			BM::GUID kCharGuid;
			EBS_JOIN_RETURN eRet = BSJR_SUCCESS;

			rkPacket.Pop( kCharGuid );
			rkPacket.Pop( eRet );

			int const iGameIDX = m_kUsers.Get(kCharGuid);
			if( 0 < iGameIDX )
			{
				CONT_BS_CONTENTS_GAME_EXT::iterator find_iter = std::find_if(m_kContBSGame.begin(), m_kContBSGame.end(), BSGameUtil::FindGameIDX(iGameIDX));
				if( m_kContBSGame.end() != find_iter )
				{
					CONT_BS_CONTENTS_GAME_EXT::value_type& rkBSGame = (*find_iter);
					switch( eRet )
					{
					case BSJR_NONE:
						{
							rkBSGame.Leave(kCharGuid);
						}break;
					default:
						{
						}break;
					}
				}
			}
			m_kUsers.Del(kCharGuid);
		}break;

		// from gm
	case PT_GM_N_REQ_SET_GAME_STATUS:
		{
			ProcessGMCommand( rkPacket );
		}break;
		// from sys
	case PT_A_NFY_USER_DISCONNECT:
		{
			SContentsUser kUser;
			kUser.ReadFromPacket( rkPacket );
			int const iGameIDX = m_kUsers.Get(kUser.kCharGuid);
			if( 0 < iGameIDX )
			{
				CONT_BS_CONTENTS_GAME_EXT::iterator find_iter = std::find_if(m_kContBSGame.begin(), m_kContBSGame.end(), BSGameUtil::FindGameIDX(iGameIDX));
				if( m_kContBSGame.end() != find_iter )
				{
					CONT_BS_CONTENTS_GAME_EXT::value_type& rkBSGame = (*find_iter);
					rkBSGame.Leave(kUser.kCharGuid);
				}
			}
			m_kUsers.Del(kUser.kCharGuid);
			m_kContLoginedUser.erase(kUser.kCharGuid);
		}break;
	case PT_T_N_NFY_USER_ENTER_GROUND:
		{
			SContentsUser kUser;
			SAnsMapMove_MT kAMM;
			rkPacket.Pop( kAMM );
			kUser.ReadFromPacket( rkPacket );

			if( m_kContLoginedUser.end() != m_kContLoginedUser.find(kUser.kCharGuid) )
			{
				break; // 처음만 동기화
			}
			m_kContLoginedUser.insert(kUser.kCharGuid);

			BM::Stream kTempPacket;
			size_t iCount = 0;
			CONT_BS_CONTENTS_GAME_EXT::const_iterator iter = m_kContBSGame.begin();
			while( m_kContBSGame.end() != iter )
			{
				CONT_BS_CONTENTS_GAME_EXT::value_type const& rkBSGame = (*iter);
				switch( rkBSGame.Status() )
				{
				case BSGS_NOW_GAME:
				case BSGS_NOW_PREOPEN:
					{
						NfyAllNotice(rkBSGame, kTempPacket);
						++iCount;
					}break;
				default:
					{
					}break;
				}
				++iter;
			}

			if( 0 != iCount )
			{
				bool const bNotice = false;
				BM::Stream kPacket(PT_N_C_NFY_BS_NOTICE);
				kPacket.Push( iCount );
				kPacket.Push( kTempPacket );
				kPacket.Push( bNotice );
				g_kRealmUserMgr.Locked_SendToUser(kUser.kMemGuid, kPacket, true);
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, L"Wrong BattleSquare packet type:"<<usType);
			return false;
		}break;
	}
	return true;
}
void PgBSGameMngImpl::NfyBSGameInfo(PgBSContentsGameExt const& rkBSGame)
{
	BM::Stream kPacket(PT_A_A_NFY_BS_GAME_INFO);
	kPacket.Push( rkBSGame.GameInfo().iGroundNo );
	kPacket.Push( rkBSGame.Status() );
	rkBSGame.WriteToPacket( kPacket );
	kPacket.Push( rkBSGame.StartTime() );
	kPacket.Push( rkBSGame.PreOpenTime() );
	g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, kPacket);
}
void PgBSGameMngImpl::ProcessGMCommand(BM::Stream& rkPacket)
{
	int iGameIDX = 0;
	int iStatus = -1;

	rkPacket.Pop( iGameIDX );
	rkPacket.Pop( iStatus );

	if( BSGS_NOW_END >= iStatus
	&&	BSGS_NONE <= iStatus )
	{
		CONT_BS_CONTENTS_GAME_EXT::iterator find_iter = std::find_if(m_kContBSGame.begin(), m_kContBSGame.end(), BSGameUtil::FindGameIDX(iGameIDX));
		if( m_kContBSGame.end() != find_iter )
		{
			__int64 iNowSecTime = 0;
			SYSTEMTIME kNowDateTime;
			::GetLocalTime(&kNowDateTime);
			CGameTime::SystemTime2SecTime(kNowDateTime, iNowSecTime);

			__int64 const iThreeMinute = 60 * 3;
			bool bSetEnable = true;
			CONT_BS_CONTENTS_GAME_EXT::value_type& rkBSGame = (*find_iter);
			if( rkBSGame.GameInfo().bUse )
			{
				switch( iStatus ) // 셋팅하려는 것이
				{
				case BSGS_NONE:
					{
						switch( rkBSGame.Status() )
						{
						case BSGS_NOW_GAME:
							{
								iStatus = BSGS_REQ_END;
								rkBSGame.EndTime( iNowSecTime );
							}break;
						case BSGS_NOW_END:
							{
								rkBSGame.EndTime( iNowSecTime - iThreeMinute );
							}//no break;
						default:
							{
								bSetEnable = false;
							}break;
						}
					}break;
				case BSGS_NOW_PREOPEN:
					{
						iNowSecTime -= rkBSGame.GameInfo().iPreOpenSec;
					}// no break;
				case BSGS_REQ_START:
					{
						if( BSGS_NONE == rkBSGame.Status()
						||	BSGS_WAIT_START == rkBSGame.Status()
						||	BSGS_NOW_PREOPEN == rkBSGame.Status() )
						{
							rkBSGame.PreOpenTime( iNowSecTime + rkBSGame.GameInfo().iPreOpenSec ); // iPreOpenSec는 -n
							rkBSGame.StartTime( iNowSecTime );
							rkBSGame.EndTime( iNowSecTime + rkBSGame.GameInfo().iGameSec );
							iStatus = BSGS_WAIT_START;
						}
						else
						{
							bSetEnable = false;
						}
					}break;
				case BSGS_REQ_END: { } break; // Ignore
				case BSGS_WAIT_START:
				default:
					{
						bSetEnable = false; // 설정 허용 불가
					}
				}

				if( bSetEnable )
				{
					rkBSGame.Status(static_cast< EBS_GAME_STATUS >(iStatus), m_kStatusMng);
					NfyBSGameInfo(rkBSGame);
				}
			}
		}
	}
}


//
PgBSGameMng::PgBSGameMng()
{
}
PgBSGameMng::~PgBSGameMng()
{
}
bool PgBSGameMng::ProcessMsg(BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->ProcessMsg(rkPacket);
}
void PgBSGameMng::OnTick()
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	Instance()->OnTick();
}