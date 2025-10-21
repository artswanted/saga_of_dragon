#include "StdAfx.h"
#include "BM/LocalMgr.h"
#include "Lohengrin/packetstruct.h"
#include "Lohengrin/LogGroup.h"
#include "Lohengrin/ProcessConfig.h"
#include "item.h"
#include "PgStringUtil.h"
#include "TableDataManager.h"
#include "PgQuestInfo.h"
#include "PgDBCache.h"
#include "PgMyQuestUtil.h"
#include "Variant/pgcontroldefmgr.h"
#include "Variant/ItemDefMgr.h"
#include "PgMissionMutator.h"
#include <csvdb/load.h>
#include "CSVQuery.h"

namespace DBCacheUtil
{
	//
	std::wstring const kDefaultNationCode( L"0" );
	int iForceNationCode = 0;

	//
	typedef std::list< BM::vstring > CONT_ERROR_MSG;
	Loki::Mutex kErrorMutex;
	CONT_ERROR_MSG kContErrorMsg;
	void AddErrorMsg(BM::vstring const& rkErrorMsg)
	{
		BM::CAutoMutex kLock(kErrorMutex);
		kContErrorMsg.push_back( rkErrorMsg );
	}
	bool DisplayErrorMsg()
	{
		BM::CAutoMutex kLock(kErrorMutex);

		CONT_ERROR_MSG kTemp;
		kContErrorMsg.swap(kTemp);

		CONT_ERROR_MSG::const_iterator iter = kTemp.begin();
		while( kTemp.end() != iter )
		{
			CAUTION_LOG(BM::LOG_LV1, (*iter));
			++iter;
		}
		return !kTemp.empty();
	}

	//
	std::wstring const kKeyPrifix( DBCACHE_KEY_PRIFIX );

	bool IsDefaultNation(std::wstring const& rkNationCodeStr)
	{
		return kDefaultNationCode == rkNationCodeStr;
	}

	struct ConvertPair
	{
		template< typename _T_LEFT, typename _T_RIGHT >
		ConvertPair(std::pair< _T_LEFT, _T_RIGHT > const& rhs)
		{
			m_kStr << L"(" << rhs.first << L", " << rhs.second << L")";
		}
		operator std::wstring const& () const
		{
			return m_kStr;
		}
		BM::vstring m_kStr;
	};
	struct ConvertTriple
	{
		template< typename _T_K1, typename _T_K2, typename _T_K3 >
		ConvertTriple(tagTripleKey< _T_K1, _T_K2, _T_K3 > const& rhs)
		{
			m_kStr << L"(K1:" << rhs.kPriKey << L", K2:" << rhs.kSecKey << L", K3:" << rhs.kTrdKey << L")";
		}
		operator std::wstring const& () const
		{
			return m_kStr;
		}
		BM::vstring m_kStr;
	};
	struct ConvertDefenceAddMonsterKey
	{
		ConvertDefenceAddMonsterKey(CONT_DEF_DEFENCE_ADD_MONSTER::key_type const& rhs)
		{
			m_kStr << L"Key(GroupNo:" << rhs.iAddMonster_GroupNo << L", SuccessCount:" << rhs.iSuccess_Count << L")";
		}
		operator std::wstring const& () const
		{
			return m_kStr;
		}
		BM::vstring m_kStr;
	};
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
}

bool PgDBCache::m_bIsForTool = false;

#include "Src/datastruct/DefItemOptionAbil.cpp"
#include "Src/datastruct/DefItemSet.cpp"

PgDBCache::PgDBCache(void)
{
}

PgDBCache::~PgDBCache(void)
{
}

bool PgDBCache::Init()
{
	BM::CAutoMutex kLock(m_kMutex);
	g_kTblDataMgr.Clear(false);
	return true;
}

bool PgDBCache::DisplayErrorMsg()
{
	return DBCacheUtil::DisplayErrorMsg();
}
void PgDBCache::AddErrorMsg(BM::vstring const& rkErrorMsg)
{
	DBCacheUtil::AddErrorMsg(rkErrorMsg);
}

//Load
bool PgDBCache::Q_DQT_DEF_ABIL_TYPE( CEL::DB_RESULT &rkResult )
{//select [AbilNo], [Name] from dbo.TB_DefAbilType
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEF_ABIL_TYPE map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEF_ABIL_TYPE::mapped_type element;

		(*itor).Pop( element.AbilNo );			++itor;
		(*itor).Pop( element.NameNo );			++itor;

		map.insert( std::make_pair(element.AbilNo, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}



/*
bool PgDBCache::Q_DQT_DEFAIPATTERN( CEL::DB_RESULT &rkResult )
{//	select [AINo] from dbo.TB_DefAIPattern
CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

CONT_DEFAIPATTERN map;

while( rkResult.vecArray.end() != itor )
{
CONT_DEFAIPATTERN::mapped_type element;

(*itor).Pop( element.AINo );	++itor;

map.insert( std::make_pair(element.AINo, element) );
}

if( map.size() )
{
g_kCoreCenter.ClearQueryResult(rkResult);
g_kTblDataMgr.SetContDef(map);
return true;
}

VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
return false;
}
*/

bool PgDBCache::Q_DQT_DEFCLASS_NATIONCODE( CEL::DB_RESULT & Result )
{
	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper<CONT_DEFCLASS::key_type, CONT_DEFCLASS::mapped_type, CONT_DEFCLASS> NationCodeUtil(L"Duplicate index[" DBCACHE_KEY_PRIFIX L"]");

	CEL::DB_DATA_ARRAY::const_iterator itor = Result.vecArray.begin();
	int index = 1;
	while( Result.vecArray.end() != itor )
	{
		TBL_DEF_CLASS element;

		(*itor).Pop( NationCodeStr );					++itor;
		(*itor).Pop( element.iClass );					++itor;
		(*itor).Pop( element.sLevel );					++itor;
		(*itor).Pop( element.i64Experience );			++itor;
		(*itor).Pop( element.sMaxHP );					++itor;
		(*itor).Pop( element.sHPRecoveryInterval );		++itor;
		(*itor).Pop( element.sHPRecovery );				++itor;
		(*itor).Pop( element.sMaxMP );					++itor;
		(*itor).Pop( element.sMPRecoveryInterval );		++itor;
		(*itor).Pop( element.sMPRecovery );				++itor;
		(*itor).Pop( element.iStr );					++itor;
		(*itor).Pop( element.iInt );					++itor;
		(*itor).Pop( element.iCon );					++itor;
		(*itor).Pop( element.iDex );					++itor;
		(*itor).Pop( element.sMoveSpeed );				++itor;
		for (int i=0; i<MAX_ITEM_ABIL_LIST; i++)
		{
			(*itor).Pop( element.aiAbil[i] );			++itor;
		}
		(*itor).Pop( element.sBonusStatus );			++itor;
		(*itor).Pop( element.sLimitStr );				++itor;
		(*itor).Pop( element.sLimitInt );				++itor;
		(*itor).Pop( element.sLimitCon );				++itor;
		(*itor).Pop( element.sLimitDex );				++itor;

		NationCodeUtil.Add(NationCodeStr, index++, element, __FUNCTIONW__, __LINE__);
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		g_kCoreCenter.ClearQueryResult(Result);
		g_kTblDataMgr.SetContDef( NationCodeUtil.GetResult() );
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFCLASS_ABIL_NATIONCODE(CEL::DB_RESULT & Result)
{
	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper<CONT_DEFCLASS_ABIL::key_type, CONT_DEFCLASS_ABIL::mapped_type, CONT_DEFCLASS_ABIL> NationCodeUtil(L"Duplicate iAbilNo[" DBCACHE_KEY_PRIFIX L"]");

	CEL::DB_DATA_ARRAY::const_iterator itor = Result.vecArray.begin();
	while( Result.vecArray.end() != itor )
	{
		TBL_DEF_CLASS_ABIL element;

		(*itor).Pop( NationCodeStr );	++itor;
		(*itor).Pop( element.iAbilNo );	++itor;
		for (int i=0; i<MAX_CLASS_ABIL_ARRAY; i++)
		{
			(*itor).Pop( element.iType[i] );++itor;
			(*itor).Pop( element.iValue[i] );++itor;
		}

		NationCodeUtil.Add(NationCodeStr, element.iAbilNo, element, __FUNCTIONW__, __LINE__);
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		g_kCoreCenter.ClearQueryResult(Result);
		g_kTblDataMgr.SetContDef( NationCodeUtil.GetResult() );
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFCLASS_PET( CEL::DB_RESULT &rkResult )
{//select [ClassNo], [Name], [ParentClassNo] from dbo.TB_DefClass
	CONT_DEFCLASS_PET map;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();

	while( rkResult.vecArray.end() != itr )
	{
		CONT_DEFCLASS_PET::mapped_type element;
		itr->Pop( element.iClass );					++itr;
		itr->Pop( element.iClassName );				++itr;
		itr->Pop( element.byPetType );				++itr;
		itr->Pop( element.iLevelIndex );			++itr;
		itr->Pop( element.iSkillIndex );			++itr;
		itr->Pop( element.iItemOptionIndex );		++itr;
		itr->Pop( element.iDefaultHair );			++itr;
		itr->Pop( element.iDefaultFace );			++itr;
		itr->Pop( element.iDefaultBody );			++itr;

		map.insert( std::make_pair( element.iClass, element ) );
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(map);
	return true;
}

bool PgDBCache::Q_DQT_DEFCLASS_PET_LEVEL( CEL::DB_RESULT &rkResult )
{
	CONT_DEFCLASS_PET_LEVEL map;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();

	while( rkResult.vecArray.end() != itr )
	{
		CONT_DEFCLASS_PET_LEVEL::key_type		kKey;
		CONT_DEFCLASS_PET_LEVEL::mapped_type	element;
		itr->Pop( kKey.iClass );					++itr;
		itr->Pop( element.sLevel );					++itr;
		itr->Pop( element.i64Experience );			++itr;
		itr->Pop( element.iTimeExperience );		++itr;
		itr->Pop( element.sMaxMP );					++itr;
		itr->Pop( element.sMPRecoveryInterval );	++itr;
		itr->Pop( element.sMPRecovery );			++itr;

		for ( int i = 0 ; i < MAX_CLASS_ABIL_LIST ; ++i )
		{
			itr->Pop( element.aiAbil[i] );			++itr;
		}

		kKey.nLv = element.sLevel;
		map.insert( std::make_pair( kKey, element ) );
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(map);
	return true;
}

bool PgDBCache::Q_DQT_DEFCLASS_PET_SKILL(CEL::DB_RESULT& rkResult)
{
	CONT_DEFCLASS_PET_SKILL map;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != itr )
	{
		CONT_DEFCLASS_PET_SKILL::key_type					kKey;
		CONT_DEFCLASS_PET_SKILL::mapped_type::value_type	kElement;

		itr->Pop( kKey );							++itr;
		itr->Pop( kElement.nLevel );				++itr;
		for ( size_t i = 0 ; i < MAX_PET_SKILLCOUNT ; ++i )
		{
			itr->Pop( kElement.iSkillNo[i] );		++itr;
		}
		itr->Pop( kElement.iSkillUseTimeBit );		++itr;

		CONT_DEFCLASS_PET_SKILL::iterator map_itr = map.find( kKey );
		if ( map_itr == map.end() )
		{
			auto kPair = map.insert( std::make_pair( kKey, CONT_DEFCLASS_PET_SKILL::mapped_type() ) );
			map_itr = kPair.first;
		}

		map_itr->second.insert( kElement );
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(map);
	return true;
}

bool PgDBCache::Q_DQT_DEFCLASS_PET_ITEMOPTION(CEL::DB_RESULT& rkResult)
{
	CONT_DEFCLASS_PET_ITEMOPTION map;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != itr )
	{
		CONT_DEFCLASS_PET_ITEMOPTION::key_type		kKey;
		CONT_DEFCLASS_PET_ITEMOPTION::mapped_type	element;

		itr->Pop( kKey.iClass );					++itr;
		itr->Pop( kKey.nLv );						++itr;
		for ( size_t i = 0 ; i < MAX_PET_ITEMOPTION_COUNT ; ++i )
		{
			itr->Pop( element.iOptionType[i] );		++itr;
			itr->Pop( element.iOptionLevel[i] );	++itr;
			if ( !element.iOptionType[i] || !element.iOptionLevel[i] )
			{// ���߿� �ϳ��� ������ �Ѵ� ���°�
				element.iOptionType[i] = element.iOptionLevel[i] = 0;
			}
		}

		map.insert( std::make_pair( kKey, element ) );
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(map);
	return true;
}

bool PgDBCache::Q_DQT_DEFCLASS_PET_ABIL(CEL::DB_RESULT& rkResult)
{
	CONT_DEFCLASS_PET_ABIL map;

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != itor )
	{
		TBL_DEF_CLASS_PET_ABIL element;

		(*itor).Pop( element.iAbilNo );	++itor;
		for (int i=0; i<MAX_CLASS_ABIL_ARRAY; i++)
		{
			(*itor).Pop( element.iType[i] );++itor;
			(*itor).Pop( element.iValue[i] );++itor;
		}
		map.insert(std::make_pair(element.iAbilNo, element));
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(map);
	return true;
}

bool PgDBCache::Q_DQT_DEFITEM_RES_CONVERT( CEL::DB_RESULT &rkResult )
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEF_ITEM_RES_CONVERT map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEF_ITEM_RES_CONVERT::mapped_type element;

		(*itor).Pop( element.ItemNo );	++itor;
		(*itor).Pop( element.NationCode );	++itor;
		(*itor).Pop( element.ResNo );	++itor;
		if( g_kLocal.IsServiceRegion(element.NationCode) )
		{
			map.insert( std::make_pair(element.ItemNo, element) );
		}
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFITEMABIL( const char* pkTbPath )
{
	static const char* TB_DEF_ITEM_ABILS[3] =
	{
		"TB_DefItemAbil.csv",
		"TB_DefItemAbil_Balance.csv",
		"TB_DefItemAbil_Pet.csv",
	};

	DBCacheUtil::PgNationCodeHelper< CONT_DEFITEMABIL::key_type, CONT_DEFITEMABIL::mapped_type, CONT_DEFITEMABIL > kNationCodeUtil( L"Duplicate AbilNo[" DBCACHE_KEY_PRIFIX L"]" );
	for (size_t i = 0; i < _countof(TB_DEF_ITEM_ABILS); i++)
	{
		BM::Stream::STREAM_DATA buff;
		CsvParser csv;
		csvdb::load(buff, csv, BM::vstring(pkTbPath) << TB_DEF_ITEM_ABILS[i]);
		while(csv.next_row()/*skip header*/)
		{
			CONT_DEFITEMABIL::mapped_type element;
			const BM::vstring kNationCodeStr = BM::vstring(csv.col(0/*f_NationCodeStr*/));
			element.ItemAbilNo = csv.col_int(1/*ItemAbilNo*/);

			for(size_t i = 0; i < MAX_ITEM_ABIL_ARRAY; i++)
			{
				element.aType[i] = csv.col_int(3 + (i << 1)); // Type0N
				element.aValue[i] = csv.col_int(4 + (i << 1)); // Value0N
			}

			kNationCodeUtil.Add( kNationCodeStr, element.ItemAbilNo, element, __FUNCTIONW__, __LINE__ );
		}
	}

	if( !kNationCodeUtil.IsEmpty() )
	{
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFITEMRARE( CEL::DB_RESULT &rkResult )
{//select [RareNo], [Name] from dbo.TB_DefItemRare
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFITEMRARE map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFITEMRARE::mapped_type element;

		(*itor).Pop( element.RareNo );	++itor;
		(*itor).Pop( element.NameNo );	++itor;

		map.insert( std::make_pair(element.RareNo, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFITEMRAREGROUP( CEL::DB_RESULT &rkResult )
{//select [GroupNo], [Name], [RareNo01], [RareNo02], [RareNo03], [RareNo04], [RareNo05], [RareNo06], [RareNo07], [RareNo08], [RareNo09], [RareNo10] from dbo.TB_DefItemRareGroup
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFITEMRAREGROUP map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFITEMRAREGROUP::mapped_type element;

		(*itor).Pop( element.GroupNo );	++itor;
		(*itor).Pop( element.NameNo );	++itor;

		for(int i = 0; MAX_ITEM_RARE_KIND_ARRAY > i ; i++)
		{
			(*itor).Pop( element.aRareNo[i] );		++itor;
		}

		map.insert( std::make_pair(element.GroupNo, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFMAP( CEL::DB_RESULT &rkResult )
{
	if( CEL::DR_SUCCESS != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot Load DefMap SP=") << rkResult.Command());
		return false;
	}

	DBCacheUtil::PgNationCodeHelper< CONT_DEFMAP::key_type, CONT_DEFMAP::mapped_type, CONT_DEFMAP > NationCodeUtil
	( L"Duplicate DEFMAPNATION[" DBCACHE_KEY_PRIFIX L"]" );
	SET_INT kChkKey;
	SET_INT kContErrorKeyList;
	CONT_DEFMAP map;
	CONT_DEFMAP_ABIL map_abil;
	CONT_DEFMAP_TOMISSION kCont_ToMission;
	CONT_TOWN2GROUND kContTown2Ground;
	
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	CEL::DB_RESULT_COUNT::const_iterator count_itor = rkResult.vecResultCount.begin();

 	size_t const iMapCount = (*count_itor);	++count_itor;

	for (size_t i=0; i<iMapCount; i++)
	{//f_NationCodeStr],[MapNo], [MapKey], [Attribute], [NameNo],[XmlPath], [ZoneCX], [ZoneCY], [ZoneCZ],
	//[Continent], [HometownNo], [Hidden_Index],[Abil01], [Abil02], [Abil03], [Abil04], [Abil05],
	//[Recommend_Level], [Recommend_Party],	[Group_TXT], [LevelLimit_Min], [LevelLimit_Max], [DESCRIPTION_TEXT],[PartyBreakIn]
		std::wstring NationCodeStr, KeyItem, CashKeyItem;
		CONT_DEFMAP::mapped_type element;
		
		(*itor).Pop(NationCodeStr);			++itor;
		(*itor).Pop(element.iMapNo);		++itor;
		(*itor).Pop(element.iKey);			++itor;
		(*itor).Pop(element.iAttr);			++itor;
		(*itor).Pop(element.NameNo);		++itor;
		(*itor).Pop(element.strXMLPath);	++itor;
		(*itor).Pop(element.sZoneCX);		++itor;
		(*itor).Pop(element.sZoneCY);		++itor;
		(*itor).Pop(element.sZoneCZ);		++itor;
		(*itor).Pop(element.sContinent);	++itor;
		(*itor).Pop(element.sHometownNo);	++itor;
		(*itor).Pop(element.sHiddenIndex);	++itor;

		for (int iAbilCount = 0; iAbilCount<MAX_MAP_ABIL_COUNT; iAbilCount++)
		{
			(*itor).Pop(element.iAbil[iAbilCount]);	++itor;
		}

		(*itor).Pop(element.byRecommend_Level);		++itor;
		(*itor).Pop(element.byRecommend_Party);		++itor;
		(*itor).Pop(element.iGroupTextNo);			++itor;
		(*itor).Pop(element.byLevelLimit_Min);		++itor;
		(*itor).Pop(element.byLevelLimit_Max);		++itor;
		(*itor).Pop(element.iDescriptionTextNo);	++itor;
		(*itor).Pop(element.byPartyBreakIn);		++itor;
		(*itor).Pop(KeyItem);						++itor;
		(*itor).Pop(CashKeyItem);					++itor;

		VEC_WSTRING::const_iterator iter;
		VEC_WSTRING ValueVector;

		PgStringUtil::BreakSep(std::wstring(KeyItem), ValueVector, _T("/"));
		iter = ValueVector.begin();
		while( ValueVector.end() != iter )
		{
			int ItemNo = PgStringUtil::SafeAtoi(*iter);
			if( ItemNo > 0 )
			{
				element.ContItemNo.push_back(ItemNo);
			}
			++iter;
		}

		ValueVector.clear();

		PgStringUtil::BreakSep(std::wstring(CashKeyItem), ValueVector, _T("/"));
		iter = ValueVector.begin();
		while( ValueVector.end() != iter )
		{
			int ItemNo = PgStringUtil::SafeAtoi(*iter);
			if( ItemNo > 0 )
			{
				element.ContCashItemNo.push_back(ItemNo);
			}
			++iter;
		}

		NationCodeUtil.Add(NationCodeStr, element.iMapNo, element, __FUNCTIONW__, __LINE__);

	}
	if(!NationCodeUtil.IsEmpty())
	{
		map = NationCodeUtil.GetResult();
	}

	DBCacheUtil::PgNationCodeHelper<CONT_DEFMAP_ABIL::key_type, CONT_DEFMAP_ABIL::mapped_type, CONT_DEFMAP_ABIL> 
		NationCodeUtilAbil( L"Duplicate EventNo[" DBCACHE_KEY_PRIFIX L"]" );

	size_t const iMapAbilCount = (*count_itor);	++count_itor;
	for (size_t j=0; j<iMapAbilCount; j++)
	{
		CONT_DEFMAP_ABIL::mapped_type element;
		std::wstring NationCodeStr;
		(*itor).Pop(NationCodeStr);			++itor;
		(*itor).Pop(element.iAbilNo);		++itor;
		for (int k=0; k<MAX_MAP_ABIL; k++)
		{
			(*itor).Pop(element.iType[k]);	++itor;
			(*itor).Pop(element.iValue[k]);	++itor;
		}
		NationCodeUtilAbil.Add(NationCodeStr, element.iAbilNo, element, __FUNCTIONW__, __LINE__);
	}
	
	if(!NationCodeUtilAbil.IsEmpty())
	{
		map_abil = NationCodeUtilAbil.GetResult();
	}


	size_t const iMissionMapCount = (*count_itor);	++count_itor;
	for (size_t j=0; j<iMissionMapCount; ++j)
	{
		CONT_DEFMAP_TOMISSION::mapped_type element;
		int DungeonNo[5] = {0};

		(*itor).Pop(element.iMapNo);			++itor;
		(*itor).Pop(element.iMissionNo);		++itor;

		for( int index = 0; index < 5; ++index )
		{
			(*itor).Pop(element.iConnectedMapNo[index]);			++itor;
		}
		kCont_ToMission.insert( std::make_pair(element.iMapNo, element) );
	}


	//--------------------------------------------------------------------------------------
	// post_process
	//--------------------------------------------------------------------------------------
	typedef std::vector<int> CONT_INT;
	CONT_INT kVecHiddenIndex;
	
	CONT_DEFMAP::iterator Itor = map.begin();
	CONT_DEFMAP::const_iterator ItorEnd = map.end();
	CONT_DEFMAP_TOMISSION::const_iterator ItorDefMap_ToMission_End = kCont_ToMission.end();
	while(ItorEnd != Itor)
	{
		CONT_DEFMAP::mapped_type& kDefMap = (*Itor).second;

		{
			if(0 < kDefMap.sHometownNo)
			{
				if(!kContTown2Ground.insert(std::make_pair(kDefMap.sHometownNo, kDefMap.iMapNo)).second)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << L"TABLE [TB_DefMap] HometownNo Error !! MapNo[" 
															   << kDefMap.iMapNo << L"]");
				}
			}
		}

		{
			if(kDefMap.iKey)
			{
				auto kContChkKey_Pair = kChkKey.insert(kDefMap.iKey);
				if(!kContChkKey_Pair.second)
				{
					kContErrorKeyList.insert(kDefMap.iKey);
				}
			}
		}

		{
			int iHiddenIndex = static_cast<int>(kDefMap.sHiddenIndex);
			if(0 != iHiddenIndex)
			{
				CONT_MISSION_NO::const_iterator citor = std::find(kVecHiddenIndex.begin(), kVecHiddenIndex.end(), iHiddenIndex);
				if(kVecHiddenIndex.end() == citor)
				{
					std::back_inserter(kVecHiddenIndex) = iHiddenIndex;
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("TABLE [TB_DefMap] HiddenIndex No Unique! Index[") 
															   << iHiddenIndex 
															   << _T("]"));
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
		}

		{
			CONT_DEFMAP_TOMISSION::const_iterator ItorFind = kCont_ToMission.find(kDefMap.iMapNo);
			if(ItorDefMap_ToMission_End != ItorFind)
			{
				CONT_DEFMAP_TOMISSION::mapped_type const& kDefMap_ToMission = (*ItorFind).second;

				if(kDefMap_ToMission.iMissionNo)
				{
					CONT_MISSION_NO& kContMissionNo = kDefMap.kContMissionNo;
					CONT_MISSION_NO::iterator itr = std::find(kContMissionNo.begin(), kContMissionNo.end(), kDefMap_ToMission.iMissionNo);
					if(kContMissionNo.end() == itr)
					{
						kContMissionNo.push_back(kDefMap_ToMission.iMissionNo);
					}
					else
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << L"TABLE [TB_DefMapToMission] No Unique MapNo[" << kDefMap.iMapNo
																   << L"] And MissionNo[" << kDefMap_ToMission.iMissionNo << L"]");
					}
				}

				for(int i = 0; i < 5; i ++)
				{
					if(kDefMap_ToMission.iConnectedMapNo[i])
					{
						CONT_MISSION_NO& kContDungeonNo = kDefMap.ContDungeonNo;
						CONT_MISSION_NO::const_iterator citor = std::find(kContDungeonNo.begin(), kContDungeonNo.end(), kDefMap_ToMission.iConnectedMapNo[i]);
						if(kContDungeonNo.end() == citor)
						{
							kContDungeonNo.push_back(kDefMap_ToMission.iConnectedMapNo[i]);
						}
						else
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << L"TABLE [TB_DefMapToMission] No Unique MapNo["
																	   << kDefMap.iMapNo 
																	   << L"] And DungeonNo[" 
																	   << kDefMap_ToMission.iConnectedMapNo[i]
																	   << L"]");
						}
					}
				}				
			}
		}

		++ Itor;
	}


	if ( kContErrorKeyList.size() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Overlapping CONT_DEFMAP's Map KEY!!! Next List " );

		SET_INT::const_iterator error_itr = kContErrorKeyList.begin();
		for ( ; error_itr != kContErrorKeyList.end() ; ++error_itr )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L" KEY : " << *error_itr );
		}
	}
	else if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		g_kTblDataMgr.SetContDef(kContTown2Ground);
		g_kTblDataMgr.SetContDef(map_abil);
		return true;
	}	

	//VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("[%s]-[%d] row count 0"), __FUNCTIONW__, __LINE__);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


bool PgDBCache::Q_DQT_DEFMAPITEM( CEL::DB_RESULT &rkResult )
{//select [PosID], [MapNo], [ItemNo], [PosX], [PosY], [PosZ] from dbo.TB_DefMapItem
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFMAPITEM map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFMAPITEM::mapped_type element;

		(*itor).Pop( element.PosID );	++itor;
		(*itor).Pop( element.MapNo );	++itor;
		(*itor).Pop( element.ItemNo );	++itor;

		int x=0,y=0,z=0;
		(*itor).Pop( x );	++itor;
		(*itor).Pop( y );	++itor;
		(*itor).Pop( y );	++itor;

		element.ptPos.x = (float)x;
		element.ptPos.y = (float)y;
		element.ptPos.z = (float)z;

		map.insert( std::make_pair(element.PosID, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFMAPMONSTERREGEN( CEL::DB_RESULT &rkResult )
{//SELECT     [index], PosGUID, MapNo, MonBagNo, RegenPeriod, PosX, PosY, PosZ FROM TB_DefMapMonsterRegen
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEF_MAP_REGEN_POINT map;

	int index = 0;
	while(rkResult.vecArray.end() != itor)
	{
		CONT_DEF_MAP_REGEN_POINT::mapped_type element;

		(*itor).Pop( element.iMapNo );			++itor;
		(*itor).Pop( element.cBagControlType );	++itor;
		(*itor).Pop( element.iBagControlNo );	++itor;
		(*itor).Pop( element.kPosGuid );		++itor;
		(*itor).Pop( element.iPointGroup);		++itor;
		(*itor).Pop( element.dwPeriod );		++itor;

		double x=0,y=0,z=0;
		(*itor).Pop( x );	++itor;
		(*itor).Pop( y );	++itor;
		(*itor).Pop( z );	++itor;

		element.pt3Pos.x = (float)x;
		element.pt3Pos.y = (float)y;
		element.pt3Pos.z = (float)z;

		(*itor).Pop(element.iMoveRange); ++itor;
		(*itor).Pop(element.cDirection); ++itor;

		map.insert(std::make_pair(index++,element));
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

static bool Q_DQT_DEFMONSTER( const char *pkTBPath )
{
	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEFMONSTER::key_type, CONT_DEFMONSTER::mapped_type, CONT_DEFMONSTER > NationCodeUtil( L"Duplicate MonsterNo[" DBCACHE_KEY_PRIFIX L"]" );
    /*Code generated by codegen version: 0.0.1*/
    BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefMonster.csv");
    const size_t f_NationCodeStr = 0, f_MonsterNo = 1, f_Name = 2, f_Abil01 = 4;
    while( csv.next_row() /*skip header*/ )
	{
		CONT_DEFMONSTER::mapped_type element;
		NationCodeStr = UNI(csv.col(f_NationCodeStr));
		element.MonsterNo = csv.col_int(f_MonsterNo);
		element.Name = csv.col_int(f_Name);
		for(int i = 0; MAX_MONSTER_ABIL_LIST> i ; i++)
			element.aAbil[i] = csv.col_int(i + f_Abil01);

		NationCodeUtil.Add(NationCodeStr, element.MonsterNo, element, __FUNCTIONW__, __LINE__);
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		g_kTblDataMgr.SetContDef(NationCodeUtil.GetResult());
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

static bool Q_DQT_DEFMONSTERABIL( const char* pkTBPath )
{
	std::wstring kNationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEFMONSTERABIL::key_type, CONT_DEFMONSTERABIL::mapped_type, CONT_DEFMONSTERABIL > kNationCodeUtil( L"duplicate MonsterAbilNo[" DBCACHE_KEY_PRIFIX L"], in [TB_DefMonsterAbil, TB_DefMonsterAbilTunning]" );
	BM::Stream::STREAM_DATA buff;
	CsvParser csv;
	csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefMonsterAbil.csv");
	const size_t f_NationCodeStr = 0, f_MonsterAbilNo = 1, f_Type01 = 3, f_Value01 = 4;
	while( csv.next_row() /*skip header*/ )
	{
		CONT_DEFMONSTERABIL::mapped_type element;
		kNationCodeStr = UNI(csv.col(f_NationCodeStr));
		element.MonsterAbilNo = csv.col_int(f_MonsterAbilNo);
		for(int i = 0; MAX_MONSTER_ABIL_ARRAY > i ; i++)
		{
			element.aType[i] = csv.col_int(f_Type01 + (i << 1)); // Type0N
			element.aValue[i] = csv.col_int(f_Value01 + (i << 1)); // Value0N
		}

		kNationCodeUtil.Add(kNationCodeStr, element.MonsterAbilNo, element, __FUNCTIONW__, __LINE__);
	}

	if( !kNationCodeUtil.IsEmpty() )
	{
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFMONSTERTUNNING( CEL::DB_RESULT &rkResult )
{//select [KeyNo], [Grade], [Level], [Type01], [Value01], [Type02], [Value02], [Type03], [Value03], [Type04], [Value04], [Type05], [Value05], [Type06], [Value06], [Type07], [Value07], [Type08], [Value08], [Type09], [Value09], [Type10], [Value10] from dbo.TB_DefMonsterTunningAbil
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFMONSTERTUNNINGABIL map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFMONSTERTUNNINGABIL::mapped_type element;

		(*itor).Pop( element.kKey.iNo );	++itor;
		(*itor).Pop( element.kKey.iGrade );	++itor;
		(*itor).Pop( element.kKey.iLevel );	++itor;
		for(int i = 0; MAX_MONSTER_ABIL_ARRAY > i ; i++)
		{
			(*itor).Pop( element.iAbil[i] );		++itor;
		}

		map.insert( std::make_pair(element.kKey, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFNPC( CEL::DB_RESULT &rkResult )
{//	select [NPCNo], [Name], [Abil01], [Abil02], [Abil03], [Abil04], [Abil05], [Abil06], [Abil07], [Abil08], [Abil09], [Abil10]  from dbo.TB_DefNPC
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFNPC map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFNPC::mapped_type element;

		(*itor).Pop( element.NPCNo );	++itor;
		(*itor).Pop( element.Name );	++itor;

		for(int i = 0; MAX_NPC_ABIL_LIST> i ; i++)
		{
			(*itor).Pop( element.aAbil[i] );		++itor;
		}

		map.insert( std::make_pair(element.NPCNo, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFNPCABIL( CEL::DB_RESULT &rkResult )
{//select [NPCAbilNo], [Type01], [Value01], [Type02], [Value02], [Type03], [Value03], [Type04], [Value04], [Type05], [Value05], [Type06], [Value06], [Type07], [Value07], [Type08], [Value08], [Type09], [Value09], [Type10], [Value10] from dbo.TB_DefNPCAbil
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFNPCABIL map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFNPCABIL::mapped_type element;

		(*itor).Pop( element.NPCAbilNo );	++itor;
		for(int i = 0; MAX_NPC_ABIL_ARRAY > i ; i++)
		{
			(*itor).Pop( element.aType[i] );		++itor;
			(*itor).Pop( element.aValue[i] );		++itor;
		}

		map.insert( std::make_pair(element.NPCAbilNo, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;

}





bool PgDBCache::Q_DQT_DEFSKILL( CEL::DB_RESULT &rkResult )
{
	/*
	SELECT [SkillNo] ,[Level] ,[NameNo] ,[RscNameNo] ,[ActionName]
	,[Memo] ,[Type] ,[ClassLimit] ,[LevelLimit] ,[WeaponLimit]
	,[StateLimit] ,[ParentSkill] ,[Target] ,[Range] ,[CastTime]
	,[CoolTime] ,[AnimationTime] ,[MP] ,[HP] ,[EffectID]
	,[Abil01] ,[Abil02] ,[Abil03] ,[Abil04] ,[Abil05]
	,[Abil06] ,[Abil07] ,[Abil08] ,[Abil09] ,[Abil10]
	FROM [DR2_Def].[dbo].[TB_DefSkill2]
	*/
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFSKILL map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFSKILL::mapped_type element;
		std::wstring kActionName = _T("");

		(*itor).Pop( element.SkillNo );	++itor;
		(*itor).Pop( element.Lv );	++itor;
		(*itor).Pop( element.NameNo );	++itor;
		(*itor).Pop( element.RscNameNo );	++itor;
		(*itor).Pop( element.iActionCategoryNo );	++itor;
		(*itor).Pop( kActionName ); ++itor;
		ZeroMemory(element.chActionName,sizeof(TCHAR) * 30 );
		wsprintf(element.chActionName,kActionName.c_str());
		(*itor).Pop( element.byType );	++itor;
		(*itor).Pop( element.i64ClassLimit );	++itor;
		(*itor).Pop( element.i64DraClassLimit );	++itor;
		(*itor).Pop( element.sLevelLimit );	++itor;
		(*itor).Pop( element.iWeaponLimit );	++itor;
		(*itor).Pop( element.iStateLimit );	++itor;
		(*itor).Pop( element.iParentSkill );	++itor;
		(*itor).Pop( element.iTarget );	++itor;
		//(*itor).Pop( element.bySelect );	++itor;
		(*itor).Pop( element.sRange );	++itor;
		(*itor).Pop( element.sCastTime );	++itor;
		(*itor).Pop( element.sCoolTime );	++itor;
		(*itor).Pop( element.iAnimationTime); ++itor;
		//(*itor).Pop( element.sHate );	++itor;
		(*itor).Pop( element.sMP );	++itor;
		(*itor).Pop( element.sHP );	++itor;
		//(*itor).Pop( element.iNeedSkill );	++itor;
		//(*itor).Pop( element.iToggleSkill );	++itor;
		//(*itor).Pop( element.sHitorate );	++itor;
		(*itor).Pop( element.iEffectID );	++itor;
		//(*itor).Pop( element.iEventID );	++itor;
		(*itor).Pop( element.iCmdStringNo);	++itor;

		element.i64ClassLimit = HumanAddFiveJobClassLimit(element.i64ClassLimit);
		for(int i = 0; MAX_SKILL_ABIL_LIST> i ; i++)
		{
			(*itor).Pop( element.aAbil[i] );		++itor;
		}

		auto kRet = map.insert( std::make_pair(element.SkillNo, element) );
		if(!kRet.second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Skill ["<<element.SkillNo<<L"] is dupulication");
		}
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFSKILL_NATIONCODE( CEL::DB_RESULT & Result )
{
	CONT_DEFSKILL const * pConstContDef = NULL;
	g_kTblDataMgr.GetContDef(pConstContDef);	// DEFSKILL �����̳ʸ� ����.
	if( NULL == pConstContDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CONT_DEFSKILL is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEFSKILL * pContDef = const_cast<CONT_DEFSKILL *>(pConstContDef);	// �����ڵ尡 �߰��� ���̺��� CONT_DEFSKILL �����̳ʿ� �ֱ� ���� const �Ӽ��� ����.
	if( NULL == pContDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CONT_DEFSKILL is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEFSKILL::key_type, CONT_DEFSKILL::mapped_type, CONT_DEFSKILL > NationCodeUtil( L"Duplicate SkillNo[" DBCACHE_KEY_PRIFIX L"]" );

	CEL::DB_DATA_ARRAY::const_iterator itor = Result.vecArray.begin();
	while( Result.vecArray.end() != itor )
	{
		CONT_DEFSKILL::mapped_type element;
		std::wstring kActionName = _T("");

		(*itor).Pop( NationCodeStr ); ++itor;
		(*itor).Pop( element.SkillNo );	++itor;
		(*itor).Pop( element.Lv );	++itor;
		(*itor).Pop( element.NameNo );	++itor;
		(*itor).Pop( element.RscNameNo );	++itor;
		(*itor).Pop( element.iActionCategoryNo );	++itor;
		(*itor).Pop( kActionName ); ++itor;
		ZeroMemory(element.chActionName,sizeof(TCHAR) * 30 );
		wsprintf(element.chActionName,kActionName.c_str());
		(*itor).Pop( element.byType );	++itor;
		(*itor).Pop( element.i64ClassLimit );	++itor;
		(*itor).Pop( element.i64DraClassLimit );	++itor;
		(*itor).Pop( element.sLevelLimit );	++itor;
		(*itor).Pop( element.iWeaponLimit );	++itor;
		(*itor).Pop( element.iStateLimit );	++itor;
		(*itor).Pop( element.iParentSkill );	++itor;
		(*itor).Pop( element.iTarget );	++itor;
		(*itor).Pop( element.sRange );	++itor;
		(*itor).Pop( element.sCastTime );	++itor;
		(*itor).Pop( element.sCoolTime );	++itor;
		(*itor).Pop( element.iAnimationTime); ++itor;
		(*itor).Pop( element.sMP );	++itor;
		(*itor).Pop( element.sHP );	++itor;
		(*itor).Pop( element.iEffectID );	++itor;
		(*itor).Pop( element.iCmdStringNo);	++itor;

		element.i64ClassLimit = HumanAddFiveJobClassLimit(element.i64ClassLimit);
		for(int i = 0; MAX_SKILL_ABIL_LIST> i ; i++)
		{
			(*itor).Pop( element.aAbil[i] );		++itor;
		}

		NationCodeUtil.Add(NationCodeStr, element.SkillNo, element, __FUNCTIONW__, __LINE__);
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		int bSuccess = true;
		CONT_DEFSKILL ContSkill = NationCodeUtil.GetResult();

		// �ߺ� �˻�.
		CONT_DEFSKILL::iterator itor = ContSkill.begin();
		for( ; itor != ContSkill.end() ; ++itor )
		{
			auto Ret = pContDef->insert( std::make_pair( (*itor).first, (*itor).second ) );
			if( !Ret.second )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Duplicate SkillNo[") << (*itor).first << _T("]"));
				bSuccess = false;
			}
		}

		ASSERT_LOG(bSuccess, BM::LOG_LV4, __FL__<<L"Duplicate SkillNo");

		g_kCoreCenter.ClearQueryResult(Result);
		g_kTblDataMgr.SetContDef(*pContDef);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFSKILLABIL( CEL::DB_RESULT &rkResult )
{//select [SkillAbilID], [Type01], [Value01], [Type02], [Value02], [Type03], [Value03], [Type04], [Value04], [Type05], [Value05], [Type06], [Value06], [Type07], [Value07], [Type08], [Value08], [Type09], [Value09], [Type10], [Value10] from dbo.TB_DefSkillAbil
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFSKILLABIL map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFSKILLABIL::mapped_type element;

		(*itor).Pop( element.SkillAbilNo );	++itor;
		for(int i = 0; MAX_NPC_ABIL_ARRAY > i ; i++)
		{
			(*itor).Pop( element.aType[i] );		++itor;
			(*itor).Pop( element.aValue[i] );		++itor;
		}

		auto kRet = map.insert( std::make_pair(element.SkillAbilNo, element) );
		if(!kRet.second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Skill abil ["<<element.SkillAbilNo<<L"] is duplication");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
		}
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFSKILLABIL_NATIONCODE( CEL::DB_RESULT & Result )
{
	CONT_DEFSKILLABIL const * pConstContDef = NULL;
	g_kTblDataMgr.GetContDef(pConstContDef);	// DEFSKILLABIL �����̳ʸ� ����.
	if( NULL == pConstContDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CONT_DEFEFFECT is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEFSKILLABIL * pContDef = const_cast<CONT_DEFSKILLABIL *>(pConstContDef);	// ���� �ڵ尡 �߰��� ���̺��� CONT_DEFSKILLABIL �����̳ʿ� �ֱ� ���� const �Ӽ��� ����.
	if( NULL == pContDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CONT_DEFEFFECT is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEFSKILLABIL::key_type, CONT_DEFSKILLABIL::mapped_type, CONT_DEFSKILLABIL > NationCodeUtil( L"Duplicate SkillAbilNo[" DBCACHE_KEY_PRIFIX L"]" );

	CEL::DB_DATA_ARRAY::const_iterator itor = Result.vecArray.begin();
	while( Result.vecArray.end() != itor )
	{
		CONT_DEFSKILLABIL::mapped_type element;

		(*itor).Pop( NationCodeStr );		++itor;
		(*itor).Pop( element.SkillAbilNo );	++itor;
		for(int i = 0; MAX_NPC_ABIL_ARRAY > i ; i++)
		{
			(*itor).Pop( element.aType[i] );		++itor;
			(*itor).Pop( element.aValue[i] );		++itor;
		}

		NationCodeUtil.Add(NationCodeStr, element.SkillAbilNo, element, __FUNCTIONW__, __LINE__);
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		int bSuccess = true;
		CONT_DEFSKILLABIL ContSkillAbil = NationCodeUtil.GetResult();

		// �ߺ� �˻�.
		CONT_DEFSKILLABIL::iterator itor = ContSkillAbil.begin();
		for( ; itor != ContSkillAbil.end() ; ++itor )
		{
			auto Ret = pContDef->insert( std::make_pair( (*itor).first, (*itor).second ) );
			if( !Ret.second )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Duplicate SkillAbilNo[") << (*itor).first << _T("]"));
				bSuccess = false;
			}
		}

		ASSERT_LOG(bSuccess, BM::LOG_LV4, __FL__<<L"Duplicate SkillAbilNo");

		g_kCoreCenter.ClearQueryResult(Result);
		g_kTblDataMgr.SetContDef(*pContDef);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFSKILLSET( CEL::DB_RESULT &rkResult )
{//select [SetNo],[Limit],[ConditionType],[ConditionValue],[ResNo] from dbo.TB_DefSkillSet
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFSKILLSET map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFSKILLSET::mapped_type element;

		(*itor).Pop( element.bySetNo );				++itor;
		(*itor).Pop( element.byLimit );				++itor;
		(*itor).Pop( element.byConditionType );		++itor;
		(*itor).Pop( element.byConditionValue );	++itor;
		(*itor).Pop( element.iResNo );				++itor;

		auto kRet = map.insert( std::make_pair(element.bySetNo, element) );
		if(!kRet.second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Skill set ["<<element.bySetNo<<L"] is duplication");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
		}
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFRES( const char* pkTablePath )
{
	static const char* TB_DEF_RES[] =
	{
		"TB_DefRes.csv",
		"TB_DefRes_Mon.csv",
		"TB_DefRes_Item.csv",
	};

	CONT_DEFRES map;
	for(size_t i = 0; i < _countof(TB_DEF_RES); i++)
	{
		BM::Stream::STREAM_DATA buff;
		CsvParser csv;
		csvdb::load(buff, csv, BM::vstring(pkTablePath) << TB_DEF_RES[i]);
		const size_t collumn_ResNo = 0, collumn_IconPath = 2, collumn_XmlPath = 3,
			collumn_U = 4, collumn_V = 5, collumn_UVIndex = 6,
			collumn_AddResNo1 = 7, collumn_AddResNo2 = 8, collumn_AddResNo3 = 9;
		while( csv.next_row()/*skip header*/ )
		{
			CONT_DEFRES::mapped_type element;
			element.ResNo = csv.col_int(collumn_ResNo);

			element.U = csv.col_int(collumn_U);
			element.V = csv.col_int(collumn_V);
			element.UVIndex = csv.col_int(collumn_UVIndex);

			element.strIconPath = UNI(csv.col(collumn_IconPath));
			element.strXmlPath = UNI(csv.col(collumn_XmlPath));

			element.AddResNo[0] = csv.col_int(collumn_AddResNo1);
			element.AddResNo[1] = csv.col_int(collumn_AddResNo2);
			element.AddResNo[2] = csv.col_int(collumn_AddResNo3);

			map.insert( std::make_pair(element.ResNo, element) );
		}
	}

	if( map.size() )
	{
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_LOAD_DEF_CHANNEL_EFFECT( CEL::DB_RESULT &rkResult )
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEF_CHANNEL_EFFECT map;

	while( rkResult.vecArray.end() != itor )
	{
		TBL_CHANNEL_EFFECT element;
		(*itor).Pop( element.iEffectNo );	++itor;
		(*itor).Pop( element.sRealm );		++itor;
		(*itor).Pop( element.sChannel );	++itor;
		(*itor).Pop( element.sMinLv );		++itor;
		(*itor).Pop( element.sMaxLv );		++itor;
		map.push_back( element );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFSTRINGS( CEL::DB_RESULT &rkResult )
{//select [StringNo], [Text], [TextEng] from dbo.TB_DefStrings
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"DO NOT CALL Q_DQT_DEFSTRINGS");
	return false;
}

bool PgDBCache::Q_DQT_DEFSTRINGS_NATIONCODE( CEL::DB_RESULT & Result )
{
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"DO NOT CALL Q_DQT_DEFSTRINGS_NATIONCODE");
	return false;
}

bool PgDBCache::Q_DQT_DEFEFFECT(CEL::DB_RESULT& rkResult )
{
	CONT_DEFEFFECT map;

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != itor )
	{
		TBL_DEF_EFFECT element;

		(*itor).Pop( element.iEffectID );	++itor;
		(*itor).Pop( element.iName );		++itor;
		(*itor).Pop( element.strActionName );++itor;

		(*itor).Pop( element.sType );	++itor;
		(*itor).Pop( element.sInterval );	++itor;
		(*itor).Pop( element.iDurationTime );	++itor;
		(*itor).Pop( element.byToggle ); ++itor;

		for (int i=0; i<MAX_EFFECT_ABIL_LIST; i++)
		{
			(*itor).Pop( element.aiAbil[i] );++itor;
		}
		map.insert(std::make_pair(element.iEffectID, element));
	}

	if(map.size())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

static bool Q_DQT_DEFEFFECT_NATIONCODE( const char* pkTBPath )
{
	CONT_DEFEFFECT const * pConstContDef = NULL;
	g_kTblDataMgr.GetContDef(pConstContDef); // First we need load other Effects
	if( NULL == pConstContDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CONT_DEFEFFECT is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEFEFFECT * pContDef = const_cast<CONT_DEFEFFECT *>(pConstContDef);
	if( NULL == pContDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CONT_DEFEFFECT is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEFEFFECT::key_type, CONT_DEFEFFECT::mapped_type, CONT_DEFEFFECT > NationCodeUtil( L"Duplicate iEffectID[" DBCACHE_KEY_PRIFIX L"]" );

	static const char* Tables[2] =
	{
		"TB_DefEffect_Player.csv",
		"TB_DefEffect_Item.csv"
	};
	const size_t f_NationCodeStr = 0, f_EffectID = 1, f_Name = 2,
        f_ActionName = 3, f_Type = 5, f_Interval = 6, f_DurationTime = 7,
        f_Toggle = 8, f_Abil01 = 9;
	for(size_t i = 0; i < _countof(Tables); i++)
	{
		BM::Stream::STREAM_DATA buff;
		CsvParser csv;
		csvdb::load(buff, csv, BM::vstring(pkTBPath) << Tables[i]);
		while( csv.next_row() /*skip header*/ )
		{
			TBL_DEF_EFFECT element;
			NationCodeStr = UNI(csv.col(f_NationCodeStr));
			element.iEffectID = csv.col_int(f_EffectID);
			element.iName = csv.col_int(f_Name);
			element.strActionName = UNI(csv.col(f_ActionName));
			element.sType = csv.col_int(f_Type);
			element.sInterval = csv.col_int(f_Interval);
			element.iDurationTime = csv.col_int(f_DurationTime);
			element.byToggle = csv.col_int(f_Toggle);

			for (int i=0; i<MAX_EFFECT_ABIL_LIST; i++)
				element.aiAbil[i] = csv.col_int(f_Abil01 + i);

			NationCodeUtil.Add(NationCodeStr, element.iEffectID, element, __FUNCTIONW__, __LINE__);
		}
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		int bSuccess = true;
		CONT_DEFEFFECT ContEffect = NationCodeUtil.GetResult();

		CONT_DEFEFFECT::iterator itor = ContEffect.begin();
		for( ; itor != ContEffect.end() ; ++itor )
		{
			auto Ret = pContDef->insert( std::make_pair( (*itor).first, (*itor).second ) );
			if( !Ret.second )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Duplicate iEffectID[") << (*itor).first << _T("]"));
				bSuccess = false;
			}
		}

		ASSERT_LOG(bSuccess, BM::LOG_LV4, __FL__<<L"Duplicate iEffectID");

		g_kTblDataMgr.SetContDef(*pContDef);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFEFFECTABIL(CEL::DB_RESULT& rkResult )
{
	CONT_DEFEFFECTABIL map;

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != itor )
	{
		TBL_DEF_EFFECTABIL element;

		(*itor).Pop( element.iAbilNo );	++itor;
		for (int i=0; i<MAX_EFFECT_ABIL_ARRAY; i++)
		{
			(*itor).Pop( element.iType[i] );++itor;
			(*itor).Pop( element.iValue[i] );++itor;
		}
		map.insert(std::make_pair(element.iAbilNo, element));
	}

	if(map.size())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;	
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

static bool Q_DQT_DEFEFFECTABIL_NATIONCODE( const char* pkTBPath )
{
	CONT_DEFEFFECTABIL const * pConstContDef = NULL;
	g_kTblDataMgr.GetContDef(pConstContDef);
	if( NULL == pConstContDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CONT_DEFEFFECTABIL is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEFEFFECTABIL * pContDef = const_cast<CONT_DEFEFFECTABIL *>(pConstContDef);
	if( NULL == pContDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CONT_DEFEFFECTABIL is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEFEFFECTABIL::key_type, CONT_DEFEFFECTABIL::mapped_type, CONT_DEFEFFECTABIL > NationCodeUtil( L"Duplicate iAbilNo[" DBCACHE_KEY_PRIFIX L"]" );

	static const char* Tables[3] =
	{
		"TB_DefEffectAbil_Item.csv",
		"TB_DefEffectAbil_Mon.csv",
		"TB_DefEffectAbil_Player.csv",
	};
    const size_t f_NationCodeStr = 0, f_EffectAbilNo = 1, f_Type01 = 3, f_Value01 = 4;
	for(size_t i = 0; i < _countof(Tables); i++)
	{
		BM::Stream::STREAM_DATA buff;
		CsvParser csv;
		csvdb::load(buff, csv, BM::vstring(pkTBPath) << Tables[i]);
		while( csv.next_row() /*skip header*/ )
		{
			TBL_DEF_EFFECTABIL element;
			NationCodeStr = UNI(csv.col(f_NationCodeStr));
			element.iAbilNo = csv.col_int(f_EffectAbilNo);
			for (int i=0; i<MAX_EFFECT_ABIL_ARRAY; i++)
			{
				element.iType[i] = csv.col_int(f_Type01 + (i << 1)); // Type0N
				element.iValue[i] = csv.col_int(f_Value01 + (i << 1)); // Value0N
			}
			NationCodeUtil.Add(NationCodeStr, element.iAbilNo, element, __FUNCTIONW__, __LINE__);
		}
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		int bSuccess = true;
		CONT_DEFEFFECTABIL ContEffectAbil = NationCodeUtil.GetResult();

		CONT_DEFEFFECTABIL::iterator itor = ContEffectAbil.begin();
		for( ; itor != ContEffectAbil.end() ; ++itor )
		{
			auto Ret = pContDef->insert( std::make_pair( (*itor).first, (*itor).second ) );
			if( !Ret.second )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Duplicate iAbilNo[") << (*itor).first << _T("]"));
				bSuccess = false;
			}
		}

		ASSERT_LOG(bSuccess, BM::LOG_LV4, __FL__<<L"Duplicate iAbilNo");

		g_kTblDataMgr.SetContDef(*pContDef);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFUPGRADECLASS(CEL::DB_RESULT& rkResult )
{
	CONT_DEFUPGRADECLASS map;

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != itor )
	{
		TBL_DEF_EFFGRADECLASS element;

		(*itor).Pop( element.iClass ); ++itor;
		(*itor).Pop( element.iParentClass ); ++itor;
		(*itor).Pop( element.byGrade ); ++itor;
		(*itor).Pop( element.sMinLevel ); ++itor;
		(*itor).Pop( element.sMaxLevel ); ++itor;
		(*itor).Pop( element.i64Bitmask ); ++itor;
		(*itor).Pop( element.byKind ); ++itor;

		map.insert(std::make_pair(element.iClass, element));
	}

	if(map.size())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

/*	
[dbo].[UP_LoadDefItemEnchant]

select [TypeNo], [Level], [NameNo], 
[Abil01], [Abil02], [Abil03], [Abil04], [Abil05], 
[Abil06], [Abil07], [Abil08], [Abil09], [Abil10]
from [dbo].[TB_DefItemEnchant]
*/
bool PgDBCache::Q_DQT_DEFITEMENCHANT( CEL::DB_RESULT& rkResult)
{//	select [ItemAbilNo], [Type01], [Value01], [Type02], [Value02], [Type03], [Value03], [Type04], [Value04], [Type05], [Value05], [Type06], [Value06], [Type07], [Value07], [Type08], [Value08], [Type09], [Value09], [Type10], [Value10] from dbo.TB_DefItemAbil
	
	if ( 3 != rkResult.vecResultCount.size() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Error ResultCount<" << rkResult.vecResultCount.size() << L">" );
		return false;
	}

	int const iMaxType = 127;
	bool bRet = true;
	
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	CONT_DEFITEMENCHANT map;

	{// Default
		int const iEnchantCount = (*count_iter);	++count_iter;
		for(int i = 0;i < iEnchantCount;++i)
		{
			CONT_DEFITEMENCHANT::mapped_type element;

			(*itor).Pop( element.Type );				++itor;
			(*itor).Pop( element.Lv );					++itor;
			(*itor).Pop( element.NameNo );				++itor;

			for(int i = 0; MAX_ITEM_ENCHANT_ABIL_LIST > i ; i++)
			{
				(*itor).Pop( element.aAbil[i] );		++itor;
			}

			if ( 0 > element.Type || iMaxType < element.Type )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Type<" << element.Type << L">" );
				bRet = false;
			}

			map.insert( std::make_pair(element.Key(), element) );
		}
	}

	{// Pet Enchant
		int const iEnchantCount = (*count_iter);	++count_iter;
		for(int i = 0;i < iEnchantCount;++i)
		{
			CONT_DEFITEMENCHANT::mapped_type element;

			(*itor).Pop( element.Type );				++itor;
			(*itor).Pop( element.Lv );					++itor;
			(*itor).Pop( element.NameNo );				++itor;

			for(int i = 0; MAX_ITEM_ENCHANT_ABIL_LIST > i ; i++)
			{
				(*itor).Pop( element.aAbil[i] );		++itor;
			}

			if ( 0 > element.Type || iMaxType < element.Type )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Type<" << element.Type << L">" );
				bRet = false;
			}

			element.Type += SItemEnchantKey::ms_iPetBaseType;
			map.insert( std::make_pair(element.Key(), element) );
		}
	}

	CONT_DEFITEMENCHANTABILWEIGHT kWeightMap;

	int const iWeightCount = (*count_iter);		++count_iter;

	for(int i = 0;i < iWeightCount;++i)
	{
		CONT_DEFITEMENCHANTABILWEIGHT::key_type kKey;
		CONT_DEFITEMENCHANTABILWEIGHT::mapped_type kValue;
		(*itor).Pop( kKey );					++itor;
		(*itor).Pop( kValue );					++itor;
		kWeightMap.insert(std::make_pair(kKey,kValue));
	}

	g_kCoreCenter.ClearQueryResult(rkResult);

	if( true == bRet )
	{	
		g_kTblDataMgr.SetContDef(map);
		g_kTblDataMgr.SetContDef(kWeightMap);
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFCHARACTER_BASEWEAR(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFCHARACTER_BASEWEAR map;
	while(rkResult.vecArray.end() != itor)
	{
		CONT_DEFCHARACTER_BASEWEAR::mapped_type kElement;	

		std::wstring wstrIconPath;

		(*itor).Pop(kElement.iWearNo);		++itor;
		(*itor).Pop(kElement.iWearType);	++itor;
		(*itor).Pop(kElement.iSetNo);		++itor;
		(*itor).Pop(kElement.iClassNo);		++itor;
		(*itor).Pop(kElement.strIconPath);			++itor;

		auto ret = map.insert(std::make_pair(kElement.iWearNo, kElement));
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

/*	select 	[EquipPos],	[PlusLevel],	[SuccessRate],	[RareGroupNo],	[NeedItemNo01],	[NeedItemCount01],	[NeedItemNo02],	[NeedItemCount02],	[NeedItemNo03],	[NeedItemCount03]	 from dbo.TB_DefItemPlusUpgrade */
bool PgDBCache::Q_DQT_DEF_ITEM_PLUS_UPGRADE(CEL::DB_RESULT& rkResult)
{
	CEL::DB_RESULT_COUNT::iterator count_itr = rkResult.vecResultCount.begin();
	if ( count_itr != rkResult.vecResultCount.end() )
	{
		CONT_DEF_ITEM_PLUS_UPGRADE map;
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

		/*int iCount = *count_itr;
		for ( int count = 0; count < iCount ; ++count )
		{
			if( rkResult.vecArray.end() != itor )
			{
				CONT_DEF_ITEM_PLUS_UPGRADE::mapped_type kElement;	
				CONT_DEF_ITEM_PLUS_UPGRADE::key_type kKey(false, 0, 0);

				(*itor).Pop(kKey.kSecKey);						++itor;
				(*itor).Pop(kKey.kTrdKey);						++itor;

				(*itor).Pop(kElement.SuccessRate);				++itor;
				(*itor).Pop(kElement.SuccessRateBonus);			++itor;
				(*itor).Pop(kElement.iNeedMoney);				++itor;

				(*itor).Pop(kElement.RareGroupNo);				++itor;
				(*itor).Pop(kElement.RareGroupSuccessRate);		++itor;

				for(int i = 0; MAX_ITEM_PLUS_UPGRADE_NEED_ARRAY > i ; i++)
				{
					(*itor).Pop(kElement.aNeedItemNo[i]);		++itor;
					(*itor).Pop(kElement.aNeedItemCount[i]);	++itor;
				}

				map.insert(std::make_pair(kKey, kElement));
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error!!!" );
			}
		}*/

		//++count_itr;
		if ( count_itr != rkResult.vecResultCount.end() )
		{
			int iCount = *count_itr;
			for ( int count = 0; count < iCount ; ++count )
			{
				if( rkResult.vecArray.end() != itor )
				{
					CONT_DEF_ITEM_PLUS_UPGRADE::mapped_type kElement;	
					CONT_DEF_ITEM_PLUS_UPGRADE::key_type kKey(true, 0, 0);

					(*itor).Pop(kKey.kSecKey);						++itor;
					(*itor).Pop(kKey.kTrdKey);						++itor;

					(*itor).Pop(kElement.SuccessRate);				++itor;
					(*itor).Pop(kElement.SuccessRateBonus);			++itor;
					(*itor).Pop(kElement.iNeedMoney);				++itor;

					(*itor).Pop(kElement.RareGroupNo);				++itor;
					(*itor).Pop(kElement.RareGroupSuccessRate);		++itor;

					for(int i = 0; MAX_ITEM_PLUS_UPGRADE_NEED_ARRAY > i ; i++)
					{
						(*itor).Pop(kElement.aNeedItemNo[i]);		++itor;
						(*itor).Pop(kElement.aNeedItemCount[i]);	++itor;
					}

					map.insert(std::make_pair(kKey, kElement));
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error!!!" );
				}
			}
		}

		if(!map.empty())
		{
			g_kCoreCenter.ClearQueryResult(rkResult);
			g_kTblDataMgr.SetContDef(map);
			return true;
		}	
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

/*	select 	[NationCode], [EquipPos],	[PlusLevel],	[SuccessRate],	[RareGroupNo],	[NeedItemNo01],	[NeedItemCount01],	[NeedItemNo02],	[NeedItemCount02],	[NeedItemNo03],	[NeedItemCount03]	 from dbo.TB_DefItemPlusUpgrade */
bool PgDBCache::Q_DQT_DEF_ITEM_PLUS_UPGRADE_NATIONCODE(CEL::DB_RESULT& rkResult)
{
	CONT_DEF_ITEM_PLUS_UPGRADE const * pConstContDefItemTbl = NULL;
	g_kTblDataMgr.GetContDef(pConstContDefItemTbl);
	if( NULL == pConstContDefItemTbl )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"CONT_DEF_ITEM_PLUS_UPGRADE is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_ITEM_PLUS_UPGRADE * pContDefItemTbl = const_cast<CONT_DEF_ITEM_PLUS_UPGRADE *>(pConstContDefItemTbl);
	if( NULL == pContDefItemTbl )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"CONT_DEF_ITEM_PLUS_UPGRADE is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper<CONT_DEF_ITEM_PLUS_UPGRADE::key_type, CONT_DEF_ITEM_PLUS_UPGRADE::mapped_type, CONT_DEF_ITEM_PLUS_UPGRADE, DBCacheUtil::ConvertTriple, DBCacheUtil::AddError, DBCacheUtil::MapContainerInsert >
		NationCodeUtil(_T("Duplicate Table Key[") DBCACHE_KEY_PRIFIX _T("]"));

	CEL::DB_RESULT_COUNT::iterator count_itr = rkResult.vecResultCount.begin();
	if ( count_itr != rkResult.vecResultCount.end() )
	{
		CONT_DEF_ITEM_PLUS_UPGRADE map;
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

		int iCount = *count_itr;
		for ( int count = 0; count < iCount ; ++count )
		{
			if( rkResult.vecArray.end() != itor )
			{
				CONT_DEF_ITEM_PLUS_UPGRADE::mapped_type kElement;	
				CONT_DEF_ITEM_PLUS_UPGRADE::key_type kKey(false, 0, 0);

				(*itor).Pop(NationCodeStr);						++itor;
				(*itor).Pop(kKey.kSecKey);						++itor;
				(*itor).Pop(kKey.kTrdKey);						++itor;

				(*itor).Pop(kElement.SuccessRate);				++itor;
				(*itor).Pop(kElement.SuccessRateBonus);			++itor;
				(*itor).Pop(kElement.iNeedMoney);				++itor;

				(*itor).Pop(kElement.RareGroupNo);				++itor;
				(*itor).Pop(kElement.RareGroupSuccessRate);		++itor;

				for(int i = 0; MAX_ITEM_PLUS_UPGRADE_NEED_ARRAY > i ; i++)
				{
					(*itor).Pop(kElement.aNeedItemNo[i]);		++itor;
					(*itor).Pop(kElement.aNeedItemCount[i]);	++itor;
				}

				NationCodeUtil.Add(NationCodeStr, kKey, kElement, __FUNCTIONW__, __LINE__);
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error!!!" );
			}
		}

	/*	++count_itr;
		if ( count_itr != rkResult.vecResultCount.end() )
		{
			iCount = *count_itr;
			for ( int count = 0; count < iCount ; ++count )
			{
				if( rkResult.vecArray.end() != itor )
				{
					CONT_DEF_ITEM_PLUS_UPGRADE::mapped_type kElement;	
					CONT_DEF_ITEM_PLUS_UPGRADE::key_type kKey(true, 0, 0);

					(*itor).Pop(kKey.kSecKey);						++itor;
					(*itor).Pop(kKey.kTrdKey);						++itor;

					(*itor).Pop(kElement.SuccessRate);				++itor;
					(*itor).Pop(kElement.SuccessRateBonus);			++itor;
					(*itor).Pop(kElement.iNeedMoney);				++itor;

					(*itor).Pop(kElement.RareGroupNo);				++itor;
					(*itor).Pop(kElement.RareGroupSuccessRate);		++itor;

					for(int i = 0; MAX_ITEM_PLUS_UPGRADE_NEED_ARRAY > i ; i++)
					{
						(*itor).Pop(kElement.aNeedItemNo[i]);		++itor;
						(*itor).Pop(kElement.aNeedItemCount[i]);	++itor;
					}

					map.insert(std::make_pair(kKey, kElement));
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error!!!" );
				}
			}
		}*/

		if( !NationCodeUtil.IsEmpty() )
		{
			int bSuccess = true;
			CONT_DEF_ITEM_PLUS_UPGRADE ContItemPlus = NationCodeUtil.GetResult();

			// �ߺ� �˻�.
			CONT_DEF_ITEM_PLUS_UPGRADE::iterator iter = ContItemPlus.begin();
			for( ; iter != ContItemPlus.end() ; ++iter )
			{
				auto Ret = pContDefItemTbl->insert( std::make_pair( (*iter).first, (*iter).second ) );
				if( !Ret.second )
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Duplicate index[") << (*iter).first.kPriKey << _T("]"));
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Duplicate index[") << (*iter).first.kSecKey << _T("]"));
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Duplicate index[") << (*iter).first.kTrdKey << _T("]"));
					bSuccess = false;
					break;
				}
			}

			ASSERT_LOG(bSuccess, BM::LOG_LV4, __FL__ << _T("Duplicaste Key"));

			g_kCoreCenter.ClearQueryResult(rkResult);
			g_kTblDataMgr.SetContDef(*pContDefItemTbl);
			return true;
		}	
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

/*	select [EquipType], [LevelLimit], [EnchnatGrade], ... from dbo.TB_DefItemEnchantShift */
bool PgDBCache::Q_DQT_DEF_ITEM_ENCHANT_SHIFT(CEL::DB_RESULT& rkResult)
{
	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_ITEM_ENCHANT_SHIFT::key_type, CONT_DEF_ITEM_ENCHANT_SHIFT::mapped_type,
		CONT_DEF_ITEM_ENCHANT_SHIFT, DBCacheUtil::ConvertTriple > NationCodeUtil( L"Duplicate ITEM_ENCHANT_SHIFT[" DBCACHE_KEY_PRIFIX L"]" );

	CEL::DB_RESULT_COUNT::iterator count_itr = rkResult.vecResultCount.begin();
	if ( count_itr != rkResult.vecResultCount.end() )
	{
		//CONT_DEF_ITEM_ENCHANT_SHIFT map;
		CONT_DEF_ITEM_ENCHANT_SHIFT::key_type kKey(0,0,0);
		CONT_DEF_ITEM_ENCHANT_SHIFT::mapped_type kElement;
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

		int iCount = *count_itr;
		for ( int count = 0; count < iCount ; ++count )
		{
			if( rkResult.vecArray.end() != itor )
			{
				(*itor).Pop( NationCodeStr );					++itor;
				(*itor).Pop(kElement.EquipType);				++itor;
				(*itor).Pop(kElement.LevelLimit);				++itor;
				(*itor).Pop(kElement.Grade);					++itor;

				(*itor).Pop(kElement.EnchantItemNo);			++itor;
				(*itor).Pop(kElement.EnchantItemCount);			++itor;
				(*itor).Pop(kElement.EnchantShiftCost);			++itor;
				(*itor).Pop(kElement.EnchantShiftGemNo);		++itor;
				(*itor).Pop(kElement.EnchantShiftGemCount);		++itor;
				(*itor).Pop(kElement.InsuranceItemNo);			++itor;

				for(int i = 0; ESR_NUM > i ; i++)
				{
					(*itor).Pop(kElement.EnchantShiftRate[i]);	++itor;
				}

				kKey.kPriKey = kElement.EquipType;
				kKey.kSecKey = kElement.Grade;
				kKey.kTrdKey = kElement.LevelLimit;

				NationCodeUtil.Add(NationCodeStr, kKey, kElement, __FUNCTIONW__, __LINE__);
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error!!!" );
			}
		}

		if( !NationCodeUtil.IsEmpty() )
		{
			g_kCoreCenter.ClearQueryResult(rkResult);
			g_kTblDataMgr.SetContDef(NationCodeUtil.GetResult());
			return true;
		}	
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

static bool Q_DQT_DEF_SUCCESS_RATE_CONTROL(const char* pkTBPath)
{
	std::wstring kNationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_SUCCESS_RATE_CONTROL::key_type, CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type, CONT_DEF_SUCCESS_RATE_CONTROL > kNationCodeUtil( L"duplicate SuccessRateControlNo[" DBCACHE_KEY_PRIFIX L"], in [TB_DefSuccessRateControl]" );
    /*Code generated by codegen version: 0.0.1*/
    BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefSuccessRateControl.csv");
    const size_t f_NationCodeStr = 0, f_No = 1, f_Rate01 = 3;
    while( csv.next_row() /*skip header*/ )
	{
		CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type kElement;
		kNationCodeStr = UNI(csv.col(f_NationCodeStr));
		int iNo = csv.col_int(f_No);

		for(int i = 0; MAX_SUCCESS_RATE_ARRAY> i ; i++)
		{
			kElement.aRate[i] = csv.col_int(f_Rate01 + i);
			kElement.iTotal += kElement.aRate[i];
		}

		kNationCodeUtil.Add(kNationCodeStr, iNo, kElement, __FUNCTIONW__, __LINE__);
	}

	if(!kNationCodeUtil.IsEmpty())
	{
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFMONSTERBAG(CEL::DB_RESULT &rkResult)
{//	select [BagNo], [Rate] from dbo.TB_DefMonsterBag
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEF_MONSTER_BAG map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEF_MONSTER_BAG::mapped_type element;

		(*itor).Pop( element.iBagNo );		++itor;
		(*itor).Pop( element.iElementNo );	++itor;
		(*itor).Pop( element.iRateNo );		++itor;
		(*itor).Pop( element.iTunningNo_Min );	++itor;
		(*itor).Pop( element.iTunningNo_Max );	++itor;

		map.insert( std::make_pair(element.iBagNo, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}



bool PgDBCache::Q_DQT_DEFITEMBAG( const char* pkTBPath )
{
	BM::Stream::STREAM_DATA buff;
	CsvParser csv;
	csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefItemBag.csv");
	std::wstring kNationCodeStr;
	const size_t f_NationCodeStr = 0, f_BagNo = 1, f_Level = 2, f_ElementsNo = 4,
        f_SuccessRateControlNo = 5, f_CountControlNo = 6, f_DropMoneyControlNo = 7;
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_ITEM_BAG::key_type, CONT_DEF_ITEM_BAG::mapped_type, CONT_DEF_ITEM_BAG, DBCacheUtil::ConvertPair > kNationCodeUtil( L"duplicate ItemBagNo[" DBCACHE_KEY_PRIFIX L"], in [TB_DefItemBag]" );
	while( csv.next_row() /*Skip header*/ )
	{
		CONT_DEF_ITEM_BAG::key_type key;
		CONT_DEF_ITEM_BAG::mapped_type element;
		kNationCodeStr = UNI(csv.col(f_NationCodeStr));
		key.first = csv.col_int(f_BagNo);
		key.second = csv.col_int(f_Level);
		element.iElementsNo = csv.col_int(f_ElementsNo);
		element.iRaseRateNo = csv.col_int(f_SuccessRateControlNo);
		element.iCountControlNo = csv.col_int(f_CountControlNo);
		element.iDropMoneyControlNo = csv.col_int(f_DropMoneyControlNo);
		kNationCodeUtil.Add(kNationCodeStr, key, element, __FUNCTIONW__, __LINE__);
	}

	if( !kNationCodeUtil.IsEmpty() )
	{
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFITEMCONTAINER( CEL::DB_RESULT &rkResult )
{//	Select [ContainerNo], [SuccessRateControlNo], [BagNo01], [BagNo02], [BagNo03], [BagNo04], [BagNo05]
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	std::wstring kNationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_ITEM_CONTAINER::key_type, CONT_DEF_ITEM_CONTAINER::mapped_type, CONT_DEF_ITEM_CONTAINER > kNationCodeUtil( L"duplicate ContainerNo[" DBCACHE_KEY_PRIFIX L"], in [TB_DefItemContainer]" );
	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEF_ITEM_CONTAINER::key_type kKey;
		CONT_DEF_ITEM_CONTAINER::mapped_type kElement;
		itor->Pop( kNationCodeStr );				++itor;
		itor->Pop( kKey );							++itor;
		itor->Pop( kElement.iRewordItem );				++itor;
		itor->Pop( kElement.iSuccessRateControlNo );	++itor;
		for( int i=0; i!=MAX_ITEM_CONTAINER_LIST; ++i )
		{
			itor->Pop(kElement.aiItemBagGrpNo[i]);		++itor;
		}

		kNationCodeUtil.Add(kNationCodeStr, kKey, kElement, __FUNCTIONW__, __LINE__);
	}

	if ( !kNationCodeUtil.IsEmpty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
	}
	return true;
}

static bool Q_DQT_DEF_DROP_MONEY_CONTROL( const char* pkTBPath )
{
	CONT_DEF_DROP_MONEY_CONTROL map;

    /*Code generated by codegen version: 0.0.1*/
    BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefDropMoneyControl.csv");
    const size_t f_No = 0, f_Rate = 2, f_Min = 3, f_Max = 4;
    while( csv.next_row() /*skip header*/ )
	{
		CONT_DEF_DROP_MONEY_CONTROL::mapped_type element;
		element.iNo = csv.col_int(f_No);
		element.iRate = csv.col_int(f_Rate);
		element.iMin = csv.col_int(f_Min);
		element.iMax = csv.col_int(f_Max);

		map.insert( std::make_pair(element.iNo, element) );
	}

	if( map.size() )
	{
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

static bool Q_DQT_DEF_COUNT_CONTROL(const char* pkTBPath)
{
	std::wstring kNationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_COUNT_CONTROL::key_type, CONT_DEF_COUNT_CONTROL::mapped_type, CONT_DEF_COUNT_CONTROL > kNationCodeUtil( L"duplicate CountControlNo[" DBCACHE_KEY_PRIFIX L"], in [TB_DefCountControl]" );
    /*Code generated by codegen version: 0.0.1*/
    BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefCountControl.csv");
    const size_t f_NationCodeStr = 0, f_No = 1, f_Count01 = 3;
    while( csv.next_row() /*skip header*/ )
	{
		CONT_DEF_COUNT_CONTROL::mapped_type kElement;	
		CONT_DEF_COUNT_CONTROL::key_type kKey;
		kNationCodeStr = UNI(csv.col(f_NationCodeStr));
		kKey = csv.col_int(f_No);

		for(int i = 0; MAX_SUCCESS_RATE_ARRAY > i ; i++)
		{
			kElement.aCount[i] = csv.col_int(f_Count01 + i);
			kElement.iTotal += kElement.aCount[i];
		}

		kNationCodeUtil.Add(kNationCodeStr, kKey, kElement, __FUNCTIONW__, __LINE__);
	}

	if(!kNationCodeUtil.IsEmpty())
	{
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_ITEM_BAG_ELEMENTS(const char* pkTBPath)
{
	BM::Stream::STREAM_DATA buff;
	CsvParser csv;
	csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefItemBagElements.csv");
	const size_t f_NationCodeStr = 0, f_BagNo = 1, f_ItemNo01 = 3, f_ItemNoTypeFlag = 13;
	std::wstring kNationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_ITEM_BAG_ELEMENTS::key_type, CONT_DEF_ITEM_BAG_ELEMENTS::mapped_type, CONT_DEF_ITEM_BAG_ELEMENTS > kNationCodeUtil( L"duplicate ItemBagElementsNo[" DBCACHE_KEY_PRIFIX L"], in [TB_DefItemBagElements]" );
	while(csv.next_row() /*skip header*/ )
	{
		CONT_DEF_ITEM_BAG_ELEMENTS::mapped_type kElement;
		CONT_DEF_ITEM_BAG_ELEMENTS::key_type kKey = csv.col_int(f_BagNo);
		kNationCodeStr = UNI(csv.col(f_NationCodeStr));
		for(int i = 0; MAX_SUCCESS_RATE_ARRAY > i ; i++)
			kElement.aElement[i] = csv.col_int(i + f_ItemNo01);
		kElement.nTypeFlag = csv.col_int(f_ItemNoTypeFlag);
		kNationCodeUtil.Add(kNationCodeStr, kKey, kElement, __FUNCTIONW__, __LINE__);
	}

	if(!kNationCodeUtil.IsEmpty())
	{
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_MONSTER_BAG_ELEMENTS(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEF_MONSTER_BAG_ELEMENTS map;

	while(rkResult.vecArray.end() != itor)
	{
		CONT_DEF_MONSTER_BAG_ELEMENTS::mapped_type kElement;	
		CONT_DEF_MONSTER_BAG_ELEMENTS::key_type kKey;

		(*itor).Pop(kKey);		++itor;

		for(int i = 0; MAX_SUCCESS_RATE_ARRAY > i ; i++)
		{
			(*itor).Pop(kElement.aElement[i]);		++itor;
			//			kElement.iTotal += kElement.aElement[i];
		}

		auto ret = map.insert(std::make_pair(kKey, kElement));
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_MONSTER_BAG_CONTROL(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEF_MONSTER_BAG_CONTROL map;

	while(rkResult.vecArray.end() != itor)
	{
		CONT_DEF_MONSTER_BAG_CONTROL::mapped_type kElement;	
		//CONT_DEF_MONSTER_BAG_CONTROL::key_type kKey;

		(*itor).Pop(kElement.iParentBagNo);		++itor;

		for(int i = 0; MAX_MONSTERBAG_ELEMENT > i ; i++)
		{
			(*itor).Pop(kElement.aBagElement[i]);		++itor;
		}

		auto ret = map.insert(std::make_pair(kElement.iParentBagNo, kElement));
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_OBJECT(CEL::DB_RESULT& rkResult)
{
	CONT_DEF_OBJECT map;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != itr)
	{
		CONT_DEF_OBJECT::mapped_type kElement;	

		itr->Pop(kElement.iObjectNo);		++itr;
		//		itr->Pop(kElement.Name);			++itr;
		itr->Pop(kElement.fHeightValue);	++itr;
		for ( int i=0; i!=MAX_OBJECT_ABIL_LIST; ++i )
		{
			itr->Pop(kElement.aAbil[i]);	++itr;
		}

		auto ret = map.insert(std::make_pair(kElement.iObjectNo, kElement));
		if ( !ret.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"ObjectNo["<<kElement.iObjectNo<<L"] Error!!" );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
	}	
	return true;
}

bool PgDBCache::Q_DQT_DEF_OBJECTABIL(CEL::DB_RESULT& rkResult)
{
	CONT_DEF_OBJECTABIL map;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != itr)
	{
		CONT_DEF_OBJECTABIL::mapped_type kElement;	

		itr->Pop(kElement.iObjectAbilNo);	++itr;
		for ( int i=0; i!=MAX_OBJECT_ABIL_ARRAY; ++i )
		{
			itr->Pop(kElement.aType[i]);	++itr;
			itr->Pop(kElement.aValue[i]);	++itr;
		}

		auto ret = map.insert(std::make_pair(kElement.iObjectAbilNo, kElement));
		if ( !ret.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"ObjectAbilNo["<<kElement.iObjectAbilNo<<L"] Error!!");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
	}	
	return true;
}

bool PgDBCache::Q_DQT_DEF_ITEM_BY_LEVEL(CEL::DB_RESULT& rkResult)
{
	CONT_DEF_ITEM_BY_LEVEL map;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != itr)
	{
		int iIndex = 0;
		int iLevel = 0;
		int iItemNo = 0;
		itr->Pop( iIndex );				++itr;
		itr->Pop( iLevel );				++itr;
		itr->Pop( iItemNo );			++itr;

		auto kPair = map.insert( std::make_pair( iIndex, CONT_DEF_ITEM_BY_LEVEL::mapped_type() ) );
		kPair.first->second.insert( TBL_DEF_ITEM_BY_LEVEL_ELEMENT( iLevel, iItemNo ) );
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(map);
	return true;
}

bool PgDBCache::Q_DEF_MISSION_DEFENCE_STAGE(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_MISSION_DEFENCE_STAGE_BAG map;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_MISSION_DEFENCE_STAGE_BAG::key_type		kKey;
		SMISSION_DEFENCE_STAGE							kValue;

		result_iter->Pop( kKey.iMissionNo );			++result_iter;
		result_iter->Pop( kKey.iParty_Number );			++result_iter;
		result_iter->Pop( kKey.iStageNo );				++result_iter;
		result_iter->Pop( kValue.iStage_Time );			++result_iter;
		result_iter->Pop( kValue.iWave_GroupNo );		++result_iter;
		result_iter->Pop( kValue.iAddMonster_GroupNo );	++result_iter;
		result_iter->Pop( kValue.iTimeToExp_Rate );		++result_iter;
		//result_iter->Pop( kValue.iStage_Delay );		++result_iter;
		result_iter->Pop( kValue.iResultNo );			++result_iter;
		result_iter->Pop( kValue.iResultCount );		++result_iter;
		result_iter->Pop( kValue.iDirection_Item );		++result_iter;

		CONT_MISSION_DEFENCE_STAGE_BAG::iterator find_itr = map.find( kKey );
		if( map.end() != find_itr )
		{
			(*find_itr).second.kCont.push_back(kValue);
		}
		else
		{			
			CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type kTypeValue;

			kTypeValue.kCont.push_back(kValue);

			auto kRet = map.insert( std::make_pair(kKey, kTypeValue) );
			if( !kRet.second )
			{
				(*kRet.first).second.kCont.push_back(kValue);
			}
		}		
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DEF_MISSION_DEFENCE_WAVE(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_MISSION_DEFENCE_WAVE_BAG map;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_MISSION_DEFENCE_WAVE_BAG::key_type		kKey;
		SMISSION_DEFENCE_WAVE						kValue;

		result_iter->Pop( kKey.iWave_GroupNo );				++result_iter;
		result_iter->Pop( kKey.iWave_No );					++result_iter;
		result_iter->Pop( kValue.iWave_Delay );				++result_iter;		

		for(int iMonCount = 0; iMonCount<MAX_WAVE_MONSTER_NUM; ++iMonCount)
		{
			result_iter->Pop( kValue.iMonster[iMonCount] );		++result_iter;
		}

		result_iter->Pop( kValue.iAddMoveSpeedPercent );	++result_iter;
		result_iter->Pop( kValue.iTunningNo );				++result_iter;
		result_iter->Pop( kValue.iAddExpPercent );			++result_iter;
		result_iter->Pop( kValue.iAddHPPercent );			++result_iter;
		result_iter->Pop( kValue.iAddDamagePercent );		++result_iter;

		CONT_MISSION_DEFENCE_WAVE_BAG::iterator find_itr = map.find( kKey );
		if( map.end() != find_itr )
		{
			(*find_itr).second.kCont.push_back(kValue);
		}
		else
		{			
			CONT_MISSION_DEFENCE_WAVE_BAG::mapped_type kTypeValue;

			kTypeValue.kCont.push_back(kValue);

			auto kRet = map.insert( std::make_pair(kKey, kTypeValue) );
			if( !kRet.second )
			{
				(*kRet.first).second.kCont.push_back(kValue);
			}
		}		
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DEF_MISSION_DEFENCE7_MISSION(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_MISSION_DEFENCE7_MISSION_BAG map;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_MISSION_DEFENCE7_MISSION_BAG::key_type		kKey;
		SMISSION_DEFENCE7_MISSION						kValue;

		result_iter->Pop( kKey.iMissionNo );			++result_iter;
		result_iter->Pop( kKey.iMissionType );			++result_iter;
		result_iter->Pop( kValue.iMin_Level );			++result_iter;
		result_iter->Pop( kValue.iMax_Level );			++result_iter;
		result_iter->Pop( kValue.iSlot_F1 );			++result_iter;
		result_iter->Pop( kValue.iSlot_F2 );			++result_iter;
		result_iter->Pop( kValue.iSlot_F3 );			++result_iter;
		result_iter->Pop( kValue.iSlot_F4 );			++result_iter;
		result_iter->Pop( kValue.iSlot_F5 );			++result_iter;
		result_iter->Pop( kValue.iSlot_F6 );			++result_iter;
		result_iter->Pop( kValue.iSlot_F7 );			++result_iter;
		result_iter->Pop( kValue.iSlot_F8 );			++result_iter;

		for(int iSkillCount = 0; iSkillCount<MAX_DEFENCE7_MISSION_SKILL; ++iSkillCount)
		{
			result_iter->Pop( kValue.iSkill[iSkillCount] );			++result_iter;
		}

		CONT_MISSION_DEFENCE7_MISSION_BAG::iterator find_itr = map.find( kKey );
		if( map.end() != find_itr )
		{
			(*find_itr).second.kCont.push_back(kValue);
		}
		else
		{			
			CONT_MISSION_DEFENCE7_MISSION_BAG::mapped_type kTypeValue;

			kTypeValue.kCont.push_back(kValue);

			auto kRet = map.insert( std::make_pair(kKey, kTypeValue) );
			if( !kRet.second )
			{
				(*kRet.first).second.kCont.push_back(kValue);
			}
		}		
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DEF_MISSION_DEFENCE7_STAGE(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_MISSION_DEFENCE7_STAGE_BAG map;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_MISSION_DEFENCE7_STAGE_BAG::key_type		kKey;
		SMISSION_DEFENCE7_STAGE							kValue;

		result_iter->Pop( kKey.iMissionNo );			++result_iter;
		result_iter->Pop( kKey.iMissionType );			++result_iter;
		result_iter->Pop( kKey.iParty_Number );			++result_iter;
		result_iter->Pop( kKey.iStageNo );				++result_iter;
		result_iter->Pop( kValue.iResult_ItemNo );		++result_iter;
		result_iter->Pop( kValue.iResult_ItemCount );	++result_iter;
		result_iter->Pop( kValue.iWin_ItemNo );			++result_iter;
		result_iter->Pop( kValue.iWin_ItemCount );		++result_iter;
		result_iter->Pop( kValue.iClear_StategicPoint );++result_iter;
		result_iter->Pop( kValue.iResult_No );			++result_iter;
		result_iter->Pop( kValue.iResult_Count );		++result_iter;
		result_iter->Pop( kValue.iUseCoin_StrategicPoint );		++result_iter;

		CONT_MISSION_DEFENCE7_STAGE_BAG::iterator find_itr = map.find( kKey );
		if( map.end() != find_itr )
		{
			(*find_itr).second.kCont.push_back(kValue);
		}
		else
		{			
			CONT_MISSION_DEFENCE7_STAGE_BAG::mapped_type kTypeValue;

			kTypeValue.kCont.push_back(kValue);

			auto kRet = map.insert( std::make_pair(kKey, kTypeValue) );
			if( !kRet.second )
			{
				(*kRet.first).second.kCont.push_back(kValue);
			}
		}		
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DEF_MISSION_DEFENCE7_WAVE(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_MISSION_DEFENCE7_WAVE_BAG map;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_MISSION_DEFENCE7_WAVE_BAG::key_type	kKey;
		SMISSION_DEFENCE7_WAVE						kValue;

		result_iter->Pop( kKey.iMissionNo );				++result_iter;
		result_iter->Pop( kKey.iMissionType );				++result_iter;
		result_iter->Pop( kKey.iParty_Number );				++result_iter;
		result_iter->Pop( kKey.iStageNo );					++result_iter;
		result_iter->Pop( kKey.iWave_No );					++result_iter;

		result_iter->Pop( kValue.iWave_Delay );				++result_iter;
		result_iter->Pop( kValue.iGive_StrategicPoint );	++result_iter;

		for(int iMonCount = 0; iMonCount<MAX_DEFENCE7_WAVE_MONSTER; ++iMonCount)
		{
			result_iter->Pop( kValue.iMonster[iMonCount] );	++result_iter;
		}

		result_iter->Pop( kValue.iTunningNo );				++result_iter;
		result_iter->Pop( kValue.iAddMoveSpeedPercent );	++result_iter;		
		result_iter->Pop( kValue.iAddExpPercent );			++result_iter;
		result_iter->Pop( kValue.iAddHPPercent );			++result_iter;
		result_iter->Pop( kValue.DamageDecRate );			++result_iter;

		CONT_MISSION_DEFENCE7_WAVE_BAG::iterator find_itr = map.find( kKey );
		if( map.end() != find_itr )
		{
			(*find_itr).second.kCont.push_back(kValue);
		}
		else
		{			
			CONT_MISSION_DEFENCE7_WAVE_BAG::mapped_type kTypeValue;

			kTypeValue.kCont.push_back(kValue);

			auto kRet = map.insert( std::make_pair(kKey, kTypeValue) );
			if( !kRet.second )
			{
				(*kRet.first).second.kCont.push_back(kValue);
			}
		}		
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DEF_MISSION_DEFENCE7_GUARDIAN(CEL::DB_RESULT& rkResult)
{	
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_MISSION_DEFENCE7_GUARDIAN_BAG map;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_MISSION_DEFENCE7_GUARDIAN_BAG::key_type	kKey;
		SMISSION_DEFENCE7_GUARDIAN						kValue;

		result_iter->Pop( kKey );							++result_iter;
		result_iter->Pop( kValue.iMonsterNo );				++result_iter;
		result_iter->Pop( kValue.iGuardian_Abil01 );		++result_iter;
		result_iter->Pop( kValue.iGuardian_Abil02 );		++result_iter;
		result_iter->Pop( kValue.iGuardian_TunningNo );		++result_iter;
		result_iter->Pop( kValue.iNeed_StrategicPoint );	++result_iter;
		result_iter->Pop( kValue.iUpgrade_Type );			++result_iter;
		result_iter->Pop( kValue.iUpgrade_Step );			++result_iter;

		for(int iUpgradeCount = 0; iUpgradeCount<MAX_DEFENCE7_GUARDIAN_UPGRADE; ++iUpgradeCount)
		{
			result_iter->Pop( kValue.iUpgrade[iUpgradeCount] );	++result_iter;
		}

		result_iter->Pop( kValue.Skill01_No );				++result_iter;
		result_iter->Pop( kValue.Skill01_StrategicPoint );	++result_iter;
		result_iter->Pop( kValue.Skill02_No );				++result_iter;
		result_iter->Pop( kValue.Skill02_StrategicPoint );	++result_iter;
		result_iter->Pop( kValue.Sell_StrategicPoint );		++result_iter;
		result_iter->Pop( kValue.GuardianType );			++result_iter;
		result_iter->Pop( kValue.iAutoSell_StrategicPoint );++result_iter;

		CONT_MISSION_DEFENCE7_GUARDIAN_BAG::iterator find_itr = map.find( kKey );
		if( map.end() != find_itr )
		{
			(*find_itr).second.kCont.push_back(kValue);
		}
		else
		{			
			CONT_MISSION_DEFENCE7_GUARDIAN_BAG::mapped_type kTypeValue;

			kTypeValue.kCont.push_back(kValue);

			auto kRet = map.insert( std::make_pair(kKey, kTypeValue) );
			if( !kRet.second )
			{
				(*kRet.first).second.kCont.push_back(kValue);
			}
		}		
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DEF_MISSION_BONUSMAP(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_MISSION_BONUSMAP map;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_MISSION_BONUSMAP::key_type		kKey;
		CONT_MISSION_BONUSMAP::mapped_type	kValue;

		result_iter->Pop( kKey );							++result_iter;
		result_iter->Pop( kValue.iUse );					++result_iter;
		result_iter->Pop( kValue.iTimeLimit );				++result_iter;
		result_iter->Pop( kValue.iChanceRate );				++result_iter;
		result_iter->Pop( kValue.iBonus1stMapRate );		++result_iter;
		result_iter->Pop( kValue.iBonusMap1 );				++result_iter;
		result_iter->Pop( kValue.iBonusMap2 );				++result_iter;
		

		//if( 0 < kValue.iUse )
		{
			auto ret = map.insert(std::make_pair(kKey, kValue));
			if ( !ret.second )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"LevelNo["<< kKey <<L"] Error!!");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}	
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_MISSION_CANDIDATE(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEF_MISSION_CANDIDATE map;
	while(rkResult.vecArray.end() != itor)
	{
		CONT_DEF_MISSION_CANDIDATE::key_type kKey;
		CONT_DEF_MISSION_CANDIDATE::mapped_type kElement;
		itor->Pop(kKey);							++itor;
		itor->Pop(kElement.iType);					++itor;
		itor->Pop(kElement.iUseCount);				++itor;
		itor->Pop(kElement.iCandidate_End);		++itor;
		for(int i=0; i<MAX_MISSION_CANDIDATE ; ++i)
		{
			itor->Pop(kElement.aiCandidate[i]);		++itor;
		}

		for(int i=0; i<(MAX_MISSION_RANK_CLEAR_LV-1); ++i)
		{
			itor->Pop(kElement.aiRankPoint[i]);		++itor;
		}
		kElement.aiRankPoint[MAX_MISSION_RANK_CLEAR_LV-1] = 0;

		itor->Pop(kElement.iMissionTunningNo);		++itor;
		auto ret = map.insert(std::make_pair(kKey,kElement));
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_MISSION_ROOT(CEL::DB_RESULT& rkResult)
{
	//	SELECT [MissionNo], [MissionName], [Stage01], [Stage02], [Stage03], [Stage04], [Stage01_AbilRateBagID], [Stage02_AbilRateBagID], [Stage03_AbilRateBagID], [Stage04_AbilRateBagID], [ResultTableNo] FROM TB_DefMission_StageRoot	CONT_DEF_MISSION_ROOT map;
	CONT_DEF_MISSION_ROOT map;

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();	
	while(rkResult.vecArray.end() != itor)
	{
		CONT_DEF_MISSION_ROOT::mapped_type kElement;

		itor->Pop(kElement.iMissionNo);						++itor;
		itor->Pop(kElement.iKey);							++itor;
		for(int i=0; i!=MAX_MISSION_LEVEL; ++i)
		{
			itor->Pop(kElement.aiLevel[i]);					++itor;
			itor->Pop(kElement.aiLevel_AbilRateBagID[i]);	++itor;
			itor->Pop(kElement.aiMissionResultNo[i]);		++itor;

			itor->Pop(kElement.aiLevel_Min[i]);				++itor;
			itor->Pop(kElement.aiLevel_Max[i]);				++itor;
			itor->Pop(kElement.aiLevel_AvgMin[i]);			++itor;
			itor->Pop(kElement.aiLevel_AvgMax[i]);			++itor;
		}
		itor->Pop(kElement.iDefence);						++itor;
		itor->Pop(kElement.iDefence7);						++itor;
		itor->Pop(kElement.iDefence8);						++itor;

		auto ret = map.insert(std::make_pair(kElement.iMissionNo, kElement));
		if( !ret.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Overlapping Data : Mission No["<<kElement.iMissionNo<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_MISSION_CLASS_REWARD(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_MISSION_CLASS_REWARD_BAG map;

	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iItemCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iItemCount;++i)
	{
		SMISSION_CLASS_REWARD_DBCACHE_KEY			kKey;
		SMISSION_CLASS_REWARD_ITEM					kValue;

		result_iter->Pop( kKey.iMissionNo );	++result_iter;
		result_iter->Pop( kKey.kLevelNo );		++result_iter;
		result_iter->Pop( kKey.kClassNo );		++result_iter;
		result_iter->Pop( kValue.iItemBagNo );	++result_iter;
		result_iter->Pop( kValue.iRate);		++result_iter;

		VEC_WSTRING kLevelVec, kClassVec;
		PgStringUtil::BreakSep(kKey.kLevelNo, kLevelVec, _T("/"));
		PgStringUtil::BreakSep(kKey.kClassNo, kClassVec, _T("/"));

		VEC_WSTRING::const_iterator level_iter = kLevelVec.begin();
		while( level_iter != kLevelVec.end() )
		{
			VEC_WSTRING::const_iterator class_iter = kClassVec.begin();
			while( class_iter != kClassVec.end() )
			{
				int iLevel = PgStringUtil::SafeAtoi(*level_iter);
				int iClass = PgStringUtil::SafeAtoi(*class_iter);
				if( 0 != iClass )
				{
					SMISSION_CLASS_REWARD_KEY kRewardKey(kKey.iMissionNo, iLevel, iClass);
					CONT_MISSION_CLASS_REWARD_BAG::iterator find_itr = map.find( kRewardKey );
					if( map.end() != find_itr )
					{
						(*find_itr).second.kCont.push_back(kValue);
					}
					else
					{
						CONT_MISSION_CLASS_REWARD_BAG::mapped_type kValueType;
						kValueType.kCont.push_back(kValue);

						auto kRet = map.insert( std::make_pair(kRewardKey, kValueType) );
						if( !kRet.second )
						{
							(*kRet.first).second.kCont.push_back(kValue);
						}
					}
				}
				++class_iter;
			}
			++level_iter;
		}
		
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_MISSION_RANK_REWARD(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_MISSION_RANK_REWARD_BAG map;

	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iItemCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iItemCount;++i)
	{
		CONT_MISSION_RANK_REWARD_BAG::key_type		kKey;
		SMISSION_RANK_REWARD_ITEM					kValue;

		result_iter->Pop( kKey.iMissionNo );	++result_iter;
		result_iter->Pop( kKey.iLevelNo );		++result_iter;
		result_iter->Pop( kKey.iRankNo );		++result_iter;
		result_iter->Pop( kValue.iItemNo );		++result_iter;
		result_iter->Pop( kValue.iRate );		++result_iter;
		result_iter->Pop( kValue.sCount );		++result_iter;

		CONT_MISSION_RANK_REWARD_BAG::iterator find_itr = map.find( kKey );
		if( map.end() != find_itr )
		{
			(*find_itr).second.kCont.push_back(kValue);
		}
		else
		{			
			CONT_MISSION_RANK_REWARD_BAG::mapped_type kTypeValue;

			kTypeValue.kCont.push_back(kValue);

			auto kRet = map.insert( std::make_pair(kKey, kTypeValue) );
			if( !kRet.second )
			{
				(*kRet.first).second.kCont.push_back(kValue);
			}
		}
		
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_DEFENCE_ADD_MONSTER(CEL::DB_RESULT& rkResult)
{
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_DEFENCE_ADD_MONSTER::key_type, CONT_DEF_DEFENCE_ADD_MONSTER::mapped_type, CONT_DEF_DEFENCE_ADD_MONSTER, DBCacheUtil::ConvertDefenceAddMonsterKey > kNationCodeUtil( L"duplicate DefenceAddMonster[" DBCACHE_KEY_PRIFIX L"], in [TB_DefDefenceAddMonster]" );

	CONT_DEF_DEFENCE_ADD_MONSTER map;

	CEL::DB_DATA_ARRAY::const_iterator result_itor = rkResult.vecArray.begin();	
	while(rkResult.vecArray.end() != result_itor)
	{
		CONT_DEF_DEFENCE_ADD_MONSTER::key_type kKey;
		CONT_DEF_DEFENCE_ADD_MONSTER::mapped_type kElement;
		std::wstring kNationCodeStr;		

		result_itor->Pop(kNationCodeStr); ++result_itor;
		result_itor->Pop(kKey.iAddMonster_GroupNo); ++result_itor;
		result_itor->Pop(kKey.iSuccess_Count); ++result_itor;
		result_itor->Pop(kElement.iAdd_StageTime); ++result_itor;
		result_itor->Pop(kElement.iMonsterNo); ++result_itor;
		result_itor->Pop(kElement.iEnchant_Probability);	++result_itor;
		result_itor->Pop(kElement.iExpRate);		++result_itor;
		result_itor->Pop(kElement.iItemNo);		++result_itor;
		result_itor->Pop(kElement.iItemCount);		++result_itor;
		result_itor->Pop(kElement.iDropRate);		++result_itor;
		result_itor->Pop(kElement.wstrIconPath);	++result_itor;
		
		kNationCodeUtil.Add(kNationCodeStr, kKey, kElement, __FUNCTIONW__, __LINE__);
	}

	if( !kNationCodeUtil.IsEmpty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

/*
bool PgDBCache::Q_DQT_DEF_REVIVEPOS(CEL::DB_RESULT& rkResult)
{
	SELECT [FromMapNo] ,[ToMapNo] ,[PosX] ,[PosY] ,[PosZ]
	,[PosCX] ,[PosCY]  ,[PosCZ]
	FROM [DR2_Def_Eric].[dbo].[TB_DefRevivePos]

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEF_REVIVEPOS map;

	while(rkResult.vecArray.end() != itor)
	{
		CONT_DEF_REVIVEPOS::mapped_type kElement;	

		(*itor).Pop(kElement.iFromMapNo);	++itor;
		(*itor).Pop(kElement.iToMapNo);		++itor;
		(*itor).Pop(kElement.ptToPos.x);	++itor;
		(*itor).Pop(kElement.ptToPos.y);	++itor;
		(*itor).Pop(kElement.ptToPos.z);	++itor;
		(*itor).Pop(kElement.sizePos.x);	++itor;
		(*itor).Pop(kElement.sizePos.y);	++itor;
		(*itor).Pop(kElement.sizePos.z);	++itor;

		auto ret = map.insert(std::make_pair(kElement.iFromMapNo, kElement));
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
*/



bool PgDBCache::Q_DQT_DEF_ITEM_OPTION(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_TBL_DEF_ITEM_OPTION vec;
	CEL::DB_RESULT_COUNT::const_iterator count_itr = rkResult.vecResultCount.begin();
	if( count_itr != rkResult.vecResultCount.end() )
	{
		vec.reserve( *count_itr );
	}

	std::wstring kNationCodeStr;
	DBCacheUtil::PgNationCodeHelper< TBL_TRIPLE_KEY_INT, CONT_TBL_DEF_ITEM_OPTION::value_type, CONT_TBL_DEF_ITEM_OPTION, DBCacheUtil::ConvertTriple, DBCacheUtil::AddError, DBCacheUtil::VectorContainerInsert > kNationCodeUtil( L"Duplicate Option Key[" DBCACHE_KEY_PRIFIX L"]" );
	while( rkResult.vecArray.end() != itor )
	{
		CONT_TBL_DEF_ITEM_OPTION::value_type element;
		(*itor).Pop( kNationCodeStr );			++itor;
		(*itor).Pop( element.iOptionType );		++itor;
		(*itor).Pop( element.iOptionGroup);		++itor;
		(*itor).Pop( element.iAbleEquipPos );	++itor;
		(*itor).Pop( element.iOutRate );		++itor;
		(*itor).Pop( element.iNameNo );			++itor;
		(*itor).Pop( element.iAbilType );		++itor;

		for(int i = 0; i < MAX_ITEM_OPTION_ABIL_NUM; ++i)
		{
			(*itor).Pop( element.aiValue[i] );	++itor;
		}
		kNationCodeUtil.Add( kNationCodeStr, TBL_TRIPLE_KEY_INT(element.iOptionType, element.iOptionGroup, element.iAbleEquipPos), element, __FUNCTIONW__, __LINE__ );
	}

	if( !kNationCodeUtil.IsEmpty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

#if 0
bool PgDBCache::Q_DQT_DEF_ITEM_RARITY_UPGRADE(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEF_ITEM_RARITY_UPGRADE map;

	int iIDX = 0;
	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEF_ITEM_RARITY_UPGRADE::mapped_type element;

		(*itor).Pop( element.iItemGrade );	++itor;
		(*itor).Pop( element.iNeedMoney );	++itor;

		for(int i=0; i!=MAX_ITEM_RARITY_UPGRADE_NEED_ARRAY; ++i)
		{
			itor->Pop(element.aiNeedItemNo[i]);	++itor;
			itor->Pop(element.anNeedItemCount[i]);	++itor;
		}
		itor->Pop(element.nSuccessRate);	++itor;

		map.insert( std::make_pair(element.iItemGrade, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	return false;
}
#else
bool PgDBCache::Q_DQT_DEF_ITEM_RARITY_UPGRADE(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEF_ITEM_RARITY_UPGRADE map;

	int iIDX = 0;
	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEF_ITEM_RARITY_UPGRADE::mapped_type element;

		(*itor).Pop( element.iItemGrade );			++itor;
		(*itor).Pop( element.iItemOptionLvMin );	++itor;
		(*itor).Pop( element.iItemOptionLvMax );	++itor;
		(*itor).Pop( element.iItemOptionCount );	++itor;
		(*itor).Pop( element.iItemRarityMin );		++itor;
		(*itor).Pop( element.iItemRarityMax );		++itor;

		map.insert( std::make_pair(element.iItemGrade, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
#endif




bool PgDBCache::Q_DQT_DEF_ITEM_BAG_GROUP(const char* pkTBPath)
{
	BM::Stream::STREAM_DATA buff;
	CsvParser csv;
	csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefItemBagGroup.csv");
	const size_t f_NationCodeStr = 0, f_BagGroupNo = 1, f_SuccessRateNo = 3, f_BagNo01 = 4;
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_ITEM_BAG_GROUP::key_type, CONT_DEF_ITEM_BAG_GROUP::mapped_type, CONT_DEF_ITEM_BAG_GROUP > kNationCodeUtil( L"duplicate BagGroupNo[" DBCACHE_KEY_PRIFIX L"], in [TB_DefItemBagGroup]" );
	while(csv.next_row() /*skip header*/)
	{
		CONT_DEF_ITEM_BAG_GROUP::mapped_type element;
		const std::wstring kNationCodeStr = UNI(csv.col(f_NationCodeStr));
		element.iBagGroupNo = csv.col_int(f_BagGroupNo);
		element.iSuccessRateNo = csv.col_int(f_SuccessRateNo);
		for(int i = 0; MAX_ITEM_BAG_ELEMENT > i ; i++)
			element.aiBagNo[i] = csv.col_int(i + f_BagNo01);

		kNationCodeUtil.Add(kNationCodeStr, element.iBagGroupNo, element, __FUNCTIONW__, __LINE__);
	}

	if(!kNationCodeUtil.IsEmpty())
	{
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_PVP_GROUNDMODE( CEL::DB_RESULT& rkResult )
{
	int iIDX = 0;
	CONT_DEF_PVP_GROUNDGROUP kPvPGroundGroup;
	CONT_DEF_PVP_GROUNDMODE map;
	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while ( itr != rkResult.vecArray.end() )
	{
		CONT_DEF_PVP_GROUNDMODE::mapped_type element;
		itr->Pop( element.iGroundNo );			++itr;
		itr->Pop( element.iName );				++itr;
		itr->Pop( element.iType );				++itr;
		itr->Pop( element.wstrPreviewImg );		++itr;
		itr->Pop( element.iUserCount );			++itr;
		map.insert( std::make_pair( element.iGroundNo, element ) );

		// CONT_DEF_PVP_GROUNDGROUP�� mapped_type�� ���� ä���ش�.
		TBL_DEF_PVP_GROUND TempGround;
		TempGround.iGroundNo = element.iGroundNo;
		TempGround.iName = element.iName;
		TempGround.wstrPreviewImg = element.wstrPreviewImg;

		CONT_DEF_PVP_GROUNDGROUP::iterator group_itr = kPvPGroundGroup.find(element.iType);
		if( kPvPGroundGroup.end() != group_itr )
		{ // Ű���� Ÿ���� �����ϸ� �� �ȿ� �߰��ϰ�
			CONT_DEF_PVP_GROUNDGROUP::mapped_type & PvPMap = group_itr->second;
			CONT_DEF_PVP_GROUNDGROUP::mapped_type::iterator map_itr = PvPMap.find(element.iUserCount);
			if( PvPMap.end() != map_itr )
			{ // Ű���� ������������ �����ϸ� ���븸 �߰��ϰ�
				CONT_DEF_PVP_MAP::mapped_type & PvPGround = map_itr->second;
				PvPGround.push_back( TempGround );
			}
			else
			{ // ������ Ű���� �Բ� ���뵵 �����̳ʿ� �ִ´�.
				CONT_DEF_PVP_MAP::mapped_type PvPGround;
				PvPGround.push_back( TempGround );
				PvPMap.insert( std::make_pair( element.iUserCount, PvPGround) );
			}
		}
		else
		{ // Ÿ���� �������� ������ Ÿ�԰� �Բ� ���뵵 �����̳ʿ� �ִ´�.
			CONT_DEF_PVP_MAP::mapped_type kGround;
			kGround.push_back( TempGround );
			CONT_DEF_PVP_GROUNDGROUP::mapped_type kMap;
			kMap.insert( std::make_pair( element.iUserCount, kGround ) );
			kPvPGroundGroup.insert( std::make_pair( element.iType, kMap ) );
		}
	}

	if ( map.empty() )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"TABLE [CONT_TBL_DEF_PVP_GROUNDMODE] is row count 0");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(map);
	g_kTblDataMgr.SetContDef(kPvPGroundGroup);
	return true;
}

bool PgDBCache::Q_DQT_DEFITEMMAKING( CEL::DB_RESULT &rkResult )
{
	//SELECT [Index] ,[No] ,[TitleStringNo] ,[ContentStringNo] ,[Memo] ,[NeedMoney] ,[NeedElementsNo] ,[NeedCountControlNo] ,[SuccessRateControlNo] ,[ResultControlNo]
	//		,[Elements01] ,[CountControl01] ...
	//  FROM [dbo].[TB_DefItemMaking]
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFITEMMAKING map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFITEMMAKING::mapped_type element;

		(*itor).Pop( element.iNo );						++itor;
		(*itor).Pop( element.iTypeNo );					++itor;
		(*itor).Pop( element.iContentStringNo );		++itor;
		(*itor).Pop( element.iNeedMoney );				++itor;
		(*itor).Pop( element.iNeedElementsNo );			++itor;
		(*itor).Pop( element.iNeedCountControlNo );		++itor;
		(*itor).Pop( element.iSuccesRateControlNo );	++itor;
		(*itor).Pop( element.iResultControlNo );		++itor;
		for(int i = 0; MAX_ITEMMAKING_ARRAY > i ; i++)
		{
			(*itor).Pop( element.aiElements[i] );		++itor;
			(*itor).Pop( element.aiCountControl[i] );	++itor;
		}
		(*itor).Pop( element.iRarityControlNo );		++itor;

		map.insert( std::make_pair(element.iNo, element) );
	}

	if( map.size() )
	{
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"TABLE [TB_DefItemMaking] is row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFCOOKING( CEL::DB_RESULT &rkResult )
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFCOOKING map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFCOOKING::mapped_type element;

		(*itor).Pop( element.iNo );						++itor;
		(*itor).Pop( element.iTypeNo );					++itor;
		(*itor).Pop( element.iNeedMoney );				++itor;
		for(int i = 0; i < TBL_DEF_COOKING::MAX_NEEDITEM; ++i)
		{
			(*itor).Pop( element.kNeedItem[i].iItemNo );	++itor;
			(*itor).Pop( element.kNeedItem[i].iCount );		++itor;
		}
		(*itor).Pop( element.kOptionItem.iItemNo );		++itor;
		(*itor).Pop( element.kOptionItem.iCount );		++itor;
		for(int i = 0; i < TBL_DEF_COOKING::MAX_RESULT; ++i)
		{
			(*itor).Pop( element.kResult[i].iItemNo );		++itor;
			(*itor).Pop( element.kResult[i].iCount );		++itor;
			(*itor).Pop( element.kResult[i].iRate );		++itor;
			(*itor).Pop( element.kResult[i].iOptionRate );	++itor;
		}

		map.insert( std::make_pair(element.iNo, element) );
	}

	if( map.size() )
	{
		g_kTblDataMgr.SetContDef(map);
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"TABLE [TB_DefCooking] is row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEFRESULTCONTROL( CEL::DB_RESULT &rkResult )
{
	//SELECT [No] ,[Result01] ,[Result02] ,[Result03] ,[Result04] ,[Result05] ,[Result06] ,[Result07] ,[Result08] ,[Result09] ,[Result10]
	//  FROM [dbo].[TB_DefResultControl]
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	CONT_DEFRESULT_CONTROL map;

	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFRESULT_CONTROL::mapped_type element;

		(*itor).Pop( element.iNo );	++itor;
		for(int i = 0; MAX_RESULT_ARRAY > i ; i++)
		{
			(*itor).Pop( element.aiResult[i] );		++itor;
		}

		map.insert( std::make_pair(element.iNo, element) );
	}

	if( map.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"TABLE [TB_DefResultControl] is row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_QUEST_RESET_SCHEDULE(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY const& rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_QUEST_RESET_SCHEDULE kList;

	while(rkVec.end() != result_iter)
	{
		BM::DBTIMESTAMP_EX kDate;
		CONT_DEF_QUEST_RESET_SCHEDULE::value_type kNew;
		(*result_iter).Pop(kNew.iQuestID);			++result_iter;
		(*result_iter).Pop(kDate);					++result_iter;
		(*result_iter).Pop(kNew.iResetLoopDay);		++result_iter;

		kDate.hour = kDate.minute = kDate.second = 0;
		kDate.fraction = 0;

		kNew.kResetBaseDate = kDate;
		kList.push_back( kNew );
	}

	if( kList.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kList);
		return true;//����
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"TABLE [TB_DefQuest_ResetSchedule] is row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;//���� (?)
}

bool PgDBCache::Q_DQT_DEF_QUEST_MIGRATION(CEL::DB_RESULT& rkResult)
{
	using namespace MyQuestMigrationTool;
	CONT_END_QUEST_MIGRATION_VER kCont;
	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		CEL::DB_DATA_ARRAY const& rkVec = rkResult.vecArray;
		CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

		while(rkVec.end() != result_iter)
		{
			int iMigrationRevision = 0;
			int iQuestID = 0;
			std::wstring kMigrationRule;
			int iTargetValue = 0;

			(*result_iter).Pop( iMigrationRevision );	++result_iter;
			(*result_iter).Pop( iQuestID );				++result_iter;
			(*result_iter).Pop( kMigrationRule );		++result_iter;
			(*result_iter).Pop( iTargetValue );			++result_iter;

			CONT_END_QUEST_MIGRATION_VER::iterator find_iter = kCont.find(iMigrationRevision);
			if( kCont.end() == find_iter )
			{
				auto kRet = kCont.insert( std::make_pair(iMigrationRevision, CONT_END_QUEST_MIGRATION_VER::mapped_type(iMigrationRevision)) );
				if( kRet.second )
				{
					find_iter = kRet.first;
				}
				else
				{
					AddErrorMsg( BM::vstring() << __FL__ << L"Can't make the memmory" );
				}
			}

			if( kCont.end() != find_iter )
			{
				if( !(*find_iter).second.Add(iQuestID, kMigrationRule, iTargetValue) )
				{
					AddErrorMsg( BM::vstring() << __FL__ << L"Wrong migration info[Revision="<<iMigrationRevision<<L", QuestID="<<iQuestID<<L", Rule="<<kMigrationRule<<L", TargetValue="<<iTargetValue<<L"]" );
				}
			}
		}
	}

	if( !kCont.empty() )
	{
		SetMigrationVer(kCont);
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	return true; // �׻� ����
}

bool PgDBCache::Q_DQT_DEF_QUEST_RANDOM_EXP(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY const& rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_QUEST_RANDOM_EXP kMap;

	while(rkVec.end() != result_iter)
	{
		BM::DBTIMESTAMP_EX kDate;
		CONT_DEF_QUEST_RANDOM_EXP::mapped_type kNew;

		(*result_iter).Pop(kNew.iLevel);			++result_iter;

		for( int iCur = 0; MAX_QUEST_RANDOM_EXP_COUNT > iCur; ++iCur )
		{
			(*result_iter).Pop(kNew.aiExp[iCur]);	++result_iter;
		}

		auto kRet = kMap.insert( std::make_pair(kNew.iLevel, kNew) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplicate [TB_DefQuest_RandomExp] Level["<<kNew.iLevel<<L"]");
		}
	}

	if( kMap.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kMap);
		return true;//����
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"TABLE [TB_DefQuest_ResetSchedule] is row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;//���� (?)
}

bool PgDBCache::Q_DQT_DEF_QUEST_RANDOM_TACTICS_EXP(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY const& rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_QUEST_RANDOM_TACTICS_EXP kMap;

	while(rkVec.end() != result_iter)
	{
		BM::DBTIMESTAMP_EX kDate;
		CONT_DEF_QUEST_RANDOM_TACTICS_EXP::mapped_type kNew;

		(*result_iter).Pop(kNew.iLevel);					++result_iter;
		for( int iCur = 0; MAX_QUEST_RANDOM_EXP_COUNT > iCur; ++iCur )
		{
			(*result_iter).Pop(kNew.aiExp[iCur]);			++result_iter;
		}
		for( int iCur = 0; MAX_QUEST_RANDOM_EXP_COUNT > iCur; ++iCur )
		{
			(*result_iter).Pop(kNew.aiGuildExp[iCur]);		++result_iter;
		}

		auto kRet = kMap.insert( std::make_pair(kNew.iLevel, kNew) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplicate [TB_DefQuest_RandomExp] Level["<<kNew.iLevel<<L"]");
		}
	}

	if( kMap.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kMap);
		return true;//����
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"TABLE [TB_DefQuest_ResetSchedule] is row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;//���� (?)
}

bool PgDBCache::Q_DQT_DEF_QUEST_REWARD(const char* pkTBPath)
{
	using namespace PgQuestInfoVerifyUtil;

	CONT_DEF_QUEST_REWARD kMap;
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_QUEST_REWARD::key_type, CONT_DEF_QUEST_REWARD::mapped_type, CONT_DEF_QUEST_REWARD > kNationCodeUtil( L" ", false );

	CONT_DEF_QUEST_WANTED kContQuestWanted;

	BM::Stream::STREAM_DATA buff;
	CsvParser csv;
	csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefQuestReward.csv");

	const size_t    f_NationCodeStr = 0,
        f_QuestID = 1, f_XmlPath = 3, f_GroupNo = 4, f_QuestType = 5,
        f_RewardExp = 6, f_RewardGold = 7, f_iRewardPerLevel_Exp = 8,
        f_iRewardPerLevel_Money = 9, f_LevelMin = 10, f_LevelMax = 11,
        f_LimitLevelMax = 12, f_TacticsLevel = 13, f_TacticsExp = 14,
        f_GuildExp = 15, f_QuestDifficult = 16, f_OrderIndex = 17,
        f_IsCanRemoteComplete = 18, f_IsCanShare = 19, f_TitleText = 20,
        f_GroupName = 21, f_Class = 22, f_PreQuestAnd = 23, f_PreQuestOr = 24,
        f_NotQuest = 25, f_MinParty = 26, f_MaxParty = 27, f_DraClassLimit = 28,
        f_Wanted_ClearCount = 29, f_Wanted_ItemNo = 30, f_Wanted_ItemCount = 31,
        f_Wanted_CoolTime = 32, f_Dra_Wanted_ItemNo = 33, f_Dra_Wanted_ItemCount = 34,
        f_IsCanRemoteAccept = 35;

	while(csv.next_row() /*Skip header*/)
	{
		CONT_DEF_QUEST_REWARD::mapped_type kNew;
		CONT_DEF_QUEST_WANTED::mapped_type kQuestWanted;

		kNew.kNationCodeStr		= UNI(csv.col(f_NationCodeStr));
		kNew.iQuestID			= csv.col_int(f_QuestID);
		kNew.kXmlPath			= UNI(csv.col(f_XmlPath));
		kNew.iGroupNo 			= csv.col_int(f_GroupNo);
		kNew.iDBQuestType 		= csv.col_int(f_QuestType);
		kNew.iExp 				= csv.col_int(f_RewardExp);
		kNew.iGold 				= csv.col_int(f_RewardGold);
		kNew.iRewardPerLevel_Exp = csv.col_int(f_iRewardPerLevel_Exp);
		kNew.iRewardPerLevel_Money = csv.col_int(f_iRewardPerLevel_Money);
		kNew.iLevelMin 			= csv.col_int(f_LevelMin);
		kNew.iLevelMax 			= csv.col_int(f_LevelMax);
		kNew.iLimitMaxLevel 	= csv.col_int(f_LimitLevelMax);
		kNew.iTacticsLevel 		= csv.col_int(f_TacticsLevel);
		kNew.iTacticsExp 		= csv.col_int(f_TacticsExp);
		kNew.iGuildExp 			= csv.col_int(f_GuildExp);
		kNew.iDifficult 		= csv.col_int(f_QuestDifficult);
		kNew.iOrderIndex 		= csv.col_int(f_OrderIndex);
		kNew.byIsCanRemoteComplete	= csv.col_int(f_IsCanRemoteComplete);
		kNew.bIsCanShare		= csv.col_int(f_IsCanShare);
		kNew.iTitleText			= csv.col_int(f_TitleText);
		kNew.iGroupName			= csv.col_int(f_GroupName);
		kNew.i64ClassFlag		= csv.col_int64(f_Class);
		kNew.i64DraClassLimit	= HumanAddFiveJobClassLimit(csv.col_int64(f_DraClassLimit));
		kNew.kPreQuestAnd		= UNI(csv.col(f_PreQuestAnd));
		kNew.kPreQuestOr		= UNI(csv.col(f_PreQuestOr));
		kNew.kNotQuest			= UNI(csv.col(f_NotQuest));
		kNew.iMinParty			= csv.col_int(f_MinParty);
		kNew.iMaxParty			= csv.col_int(f_MaxParty);

		kNew.iWantedClearCount	= csv.col_int(f_Wanted_ClearCount);
		kNew.iWantedItemNo		= csv.col_int(f_Wanted_ItemNo);
		kNew.iWantedItemCount	= csv.col_int(f_Wanted_ItemCount);
		kNew.iWantedCoolTime	= csv.col_int(f_Wanted_CoolTime);
		kNew.iDra_WantedItemNo	= csv.col_int(f_Dra_Wanted_ItemNo);
		kNew.iDra_WantedItemCount	= csv.col_int(f_Dra_Wanted_ItemCount);
		kNew.bIsCanRemoteAccept	= csv.col_int(f_IsCanRemoteAccept);
		switch( kNew.iDBQuestType )
		{
		case QT_Normal:
		case QT_Event_Normal:
		case QT_Scenario:
		case QT_ClassChange:
		case QT_Loop:
		case QT_Event_Loop:
		case QT_Scroll:
		case QT_Day:
		case QT_MissionQuest:
		case QT_GuildTactics:
		case QT_Deleted:
		case QT_Dummy:
		case QT_Normal_Day:
		case QT_Event_Normal_Day:
		case QT_Couple:
		case QT_SweetHeart:
		case QT_Random:
		case QT_RandomTactics:
		case QT_Soul:
		case QT_Week:
		case QT_BattlePass:
			{
			}break;
		case QT_Wanted:
			{
				kQuestWanted.iQuestID = kNew.iQuestID;
				kQuestWanted.iClearCount = kNew.iWantedClearCount;
				kQuestWanted.iItemNo = kNew.iWantedItemNo;
				kQuestWanted.iItemCount = kNew.iWantedItemCount;
				kQuestWanted.iCoolTime = kNew.iWantedCoolTime;
				kQuestWanted.iDra_ItemNo = kNew.iDra_WantedItemNo;
				kQuestWanted.iDra_ItemCount = kNew.iDra_WantedItemCount;
			}break;
		case QT_None:
		default:
			{
				BM::vstring const kErrorMsg = BM::vstring(L"Invalid QuestType[")<<kNew.iDBQuestType<<L"] QuestID: "<<kNew.iQuestID;
				AddError( SQuestInfoError(ET_DataError, std::string("DB"), kNew.iQuestID, __FUNCTIONW__, __LINE__, kErrorMsg) );
			}break;
		}
		
		DBCacheUtil::EAddResult eResult = kNationCodeUtil.Add( kNew.kNationCodeStr, kNew.iQuestID, kNew, __FUNCTIONW__, __LINE__ );
		if( DBCacheUtil::E_ADD_FAIL_DUPLICATE == eResult )
		{
			BM::vstring const kErrorMsg = BM::vstring(L"Duplicate QuestID[")<<kNew.iQuestID<<L"] Reward Info";
			AddError( SQuestInfoError(ET_DataError, std::string("DB"), kNew.iQuestID, __FUNCTIONW__, __LINE__, kErrorMsg) );
		}
		else if( DBCacheUtil::E_ADD_FAIL_NATION == eResult )
		{// �ش��� ���� ����Ʈ�� �ε� �Ѵ�.
			continue;
		}

		// ���� ���� ����Ʈ
		kContQuestWanted.insert( std::make_pair( kQuestWanted.iQuestID, kQuestWanted ) );

		if( false == m_bIsForTool
		&&	(QT_Random == kNew.iDBQuestType
		||	QT_RandomTactics == kNew.iDBQuestType 
		||  QT_Wanted == kNew.iDBQuestType) ) // ���� ����Ʈ, �����������Ʈ�� DB���� Min/Max ���� ������� �ʴ´�
		{
			int const iMinLevel = 1;
			int const iMaxLevel = 999;
			kNew.iLevelMin = iMinLevel;
			kNew.iLevelMax = iMaxLevel;
			kNew.iLimitMaxLevel = iMaxLevel;
		}

		if( kNew.kXmlPath.empty() )
		{
			BM::vstring const kErrorMsg = BM::vstring(L"QuestXML Path is Empty QuestID[") << kNew.iQuestID << L"] NationCodeStr[" << kNew.kNationCodeStr << L"]";
			AddError( SQuestInfoError(ET_DataError, std::string("DB"), kNew.iQuestID, __FUNCTIONW__, __LINE__, kErrorMsg) );
			continue;
		}
	}

	if( kContQuestWanted.size() )
		g_kTblDataMgr.SetContDef(kContQuestWanted);

	if( !kNationCodeUtil.IsEmpty() )
	{
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		return true;
	}

	BM::vstring const kErrorMsg = BM::vstring()<<L"TABLE [TB_DefQuestReward] is row count 0";
	AddError( SQuestInfoError(ET_DataError, std::string("DB"), 0, __FUNCTIONW__, __LINE__, kErrorMsg) );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_SPEND_MONEY(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY const& rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_itor = rkVec.begin();

	CONT_DEF_SPEND_MONEY kMap;
	while(rkVec.end() != result_itor)
	{
		CONT_DEF_SPEND_MONEY::mapped_type kElement;
		result_itor->Pop(kElement.iID);				++result_itor;
		result_itor->Pop(kElement.iType);			++result_itor;
		result_itor->Pop(kElement.iLevelMin);		++result_itor;
		result_itor->Pop(kElement.iLevelMax);		++result_itor;
		result_itor->Pop(kElement.iSpendMoneyValue);	++result_itor;
		if( kElement.iSpendMoneyValue <= 0 )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"[TB_DefSpendMoney] ID["<<kElement.iID<<L"] Value["<<kElement.iSpendMoneyValue<<L"] Error!!");
		}

		auto kPair = kMap.insert(std::make_pair(kElement.iID,kElement));
		if( !kPair.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"[TB_DefSpendMoney] ID["<<kElement.iID<<L"] Overlapping");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
		}
	}

	if ( kMap.size() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kMap);
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_GUILD_LEVEL(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed[Result: "<<rkResult.eRet<<L"] DB Load");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_GUILD_LEVEL kCont;
	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();
	while(rkVec.end() != result_iter)
	{
		typedef std::vector< int > CONT_DAY;
		CONT_DAY kTemp;

		CONT_DEF_GUILD_LEVEL::mapped_type kNewElement;

		result_iter->Pop(kNewElement.sGuildLv);					++result_iter;
		result_iter->Pop(kNewElement.iMaxMemberCount);			++result_iter;
		result_iter->Pop(kNewElement.iSkillPoint);				++result_iter;
		result_iter->Pop(kNewElement.iExp);						++result_iter;
		result_iter->Pop(kNewElement.iGold);					++result_iter;
		result_iter->Pop(kNewElement.iItemNo[0]);				++result_iter;
		result_iter->Pop(kNewElement.iCount[0]);				++result_iter;
		result_iter->Pop(kNewElement.iItemNo[1]);				++result_iter;
		result_iter->Pop(kNewElement.iCount[1]);				++result_iter;
		result_iter->Pop(kNewElement.iItemNo[2]);				++result_iter;
		result_iter->Pop(kNewElement.iCount[2]);				++result_iter;
		result_iter->Pop(kNewElement.iItemNo[3]);				++result_iter;
		result_iter->Pop(kNewElement.iCount[3]);				++result_iter;
		result_iter->Pop(kNewElement.iItemNo[4]);				++result_iter;
		result_iter->Pop(kNewElement.iCount[4]);				++result_iter;
		result_iter->Pop(kNewElement.iIntroText);				++result_iter;
		result_iter->Pop(kNewElement.iResultText);				++result_iter;
		result_iter->Pop(kNewElement.iAutoChangeOwner1);		++result_iter;
		result_iter->Pop(kNewElement.iAutoChangeOwner2);		++result_iter;
		result_iter->Pop(kNewElement.iAutoChangeOwnerRun);		++result_iter;

		{
			kTemp.push_back( kNewElement.iAutoChangeOwner1 );
			kTemp.push_back( kNewElement.iAutoChangeOwner2 );
			kTemp.push_back( kNewElement.iAutoChangeOwnerRun );
			std::sort(kTemp.begin(), kTemp.end());
			if( kTemp[0] != kNewElement.iAutoChangeOwner1 // ���ڸ� �߸� ���� �ߴ°�
			||	kTemp[1] != kNewElement.iAutoChangeOwner2
			||	kTemp[2] != kNewElement.iAutoChangeOwnerRun
			||	kTemp[0] == kTemp[1] // ���� ���ڸ� ���� ������ �ʾҴ°�
			||	kTemp[0] == kTemp[2]
			||	kTemp[1] == kTemp[2] )
			{
				AddErrorMsg( BM::vstring()<<__FL__<<L"wrong guild auto change owner day[1st="<<kNewElement.iAutoChangeOwner1<<L", 2nd="<<kNewElement.iAutoChangeOwner2<<L", run="<<kNewElement.iAutoChangeOwnerRun<<L"] from [TB_DefGuildLevel], right day[1st < 2nd < run]" );
				kNewElement.iAutoChangeOwner1 = kTemp[0];
				kNewElement.iAutoChangeOwner2 = kTemp[1];
				kNewElement.iAutoChangeOwnerRun = kTemp[2];
			}
		}

		if( kCont.end() != kCont.find(kNewElement.sGuildLv) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplicate Guild Level["<<kNewElement.sGuildLv<<L"] from [TB_DefGuildLevel]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		std::inserter(kCont, kCont.end()) = std::make_pair(kNewElement.sGuildLv, kNewElement);
	}

	if( kCont.empty() )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);

	return true;
}

bool PgDBCache::Q_DQT_DEF_GUILD_SKILL(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed[Result: "<<rkResult.eRet<<L"] DB Load");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_GUILD_SKILL kCont;
	const CEL::DB_DATA_ARRAY &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();
	while(rkVec.end() != result_iter)
	{
		CONT_DEF_GUILD_SKILL::mapped_type kNewElement;

		result_iter->Pop(kNewElement.sGuildLv);			++result_iter;
		result_iter->Pop(kNewElement.iSkillNo);			++result_iter;
		result_iter->Pop(kNewElement.iSkillPoint);		++result_iter;
		result_iter->Pop(kNewElement.iGold);			++result_iter;
		result_iter->Pop(kNewElement.iGuildExp);		++result_iter;
		result_iter->Pop(kNewElement.iItemNo[0]);		++result_iter;
		result_iter->Pop(kNewElement.iCount[0]);		++result_iter;
		result_iter->Pop(kNewElement.iItemNo[1]);		++result_iter;
		result_iter->Pop(kNewElement.iCount[1]);		++result_iter;


		auto kRet = kCont.insert( std::make_pair(kNewElement.iSkillNo, kNewElement) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplicate Guild SkillNo["<<kNewElement.iSkillNo<<L"] from [TB_DefGuildSkill]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if( kCont.empty() )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);

	return true;
}
/*
bool PgDBCache::Q_DQT_DEF_PROPERTY(CEL::DB_RESULT &rkResult)
{
if( CEL::DR_SUCCESS != rkResult.eRet )
{
VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[%s]-[%d] Failed[Result: %d] DB Load"), __FUNCTIONW__, __LINE__, rkResult.eRet);
return false;
}

CONT_DEF_PROPERTY kCont;
const CEL::DB_DATA_ARRAY &rkVec = rkResult.vecArray;
CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();
while(rkVec.end() != result_iter)
{
CONT_DEF_PROPERTY::mapped_type kNewElement;

result_iter->Pop(kNewElement.kKey.iOffense);	++result_iter;
result_iter->Pop(kNewElement.kKey.iDefense);	++result_iter;
result_iter->Pop(kNewElement.iRate);			++result_iter;

auto kRet = kCont.insert( std::make_pair(kNewElement.kKey, kNewElement) );
if( !kRet.second )
{
VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[%s]-[%d] Duplicate Property[%d,%d] from [TB_DefProperty]"), __FUNCTIONW__, __LINE__, kNewElement.kKey.iOffense,kNewElement.kKey.iDefense);
return false;
}
}

if( kCont.empty() )
{
INFO_LOG(BM::LOG_LV1, _T("[%s]-[%d] failed [TB_DefProperty] row count is 0"), __FUNCTIONW__, __LINE__);
return false;
}

g_kCoreCenter.ClearQueryResult(rkResult);
g_kTblDataMgr.SetContDef(kCont);

return true;
}
*/
bool PgDBCache::Q_DQT_DEF_PROPERTY(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed[Result: "<<rkResult.eRet<<L"] DB Load");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_PROPERTY kCont;
	const CEL::DB_DATA_ARRAY &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	int iRate = 0;

	while(rkVec.end() != result_iter)
	{
		CONT_DEF_PROPERTY::mapped_type kNewElement;

		result_iter->Pop(kNewElement.kKey.iProperty);		++result_iter;
		result_iter->Pop(kNewElement.kKey.iPropertyLevel);	++result_iter;
		for(int i = 0;i < PROPERTY_TABLE_LIMIT;i++)
		{
			result_iter->Pop(iRate);						++result_iter;
			kNewElement.kContRate.push_back(iRate);
		}

		auto kRet = kCont.insert( std::make_pair(kNewElement.kKey, kNewElement) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplicate Property["<<kNewElement.kKey.iProperty<<L","<<kNewElement.kKey.iPropertyLevel<<L"] from [TB_DefProperty]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if( kCont.empty() )
	{
		INFO_LOG(BM::LOG_LV1, __FL__<<L"failed [TB_DefProperty] row count is 0");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);

	return true;
}




bool PgDBCache::Q_DQT_DEF_FIVE_ELEMENT_INFO(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet 
		&&	CEL::DR_NO_RESULT != rkResult.eRet)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed[Result: "<<rkResult.eRet<<L"] DB Load");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_FIVE_ELEMENT_INFO kCont;
	const CEL::DB_DATA_ARRAY &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();
	while(rkVec.end() != result_iter)
	{
		CONT_FIVE_ELEMENT_INFO::mapped_type kValue;

		result_iter->Pop(kValue.iAttrType);	++result_iter;
		result_iter->Pop(kValue.iNameNo);	++result_iter;
		result_iter->Pop(kValue.iCrystalStoneNo);	++result_iter;

		kCont.insert(std::make_pair(kValue.iAttrType, kValue));
	}

	if( kCont.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);

	return true;
}


bool PgDBCache::Q_DQT_DEF_ITEM_RARITY_UPGRADE_COST_RATE(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed[Result: "<<rkResult.eRet<<L"] DB Load");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_ITEM_RARITY_UPGRADE_COST_RATE kCont;
	const CEL::DB_DATA_ARRAY &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();
	while(rkVec.end() != result_iter)
	{
		CONT_ITEM_RARITY_UPGRADE_COST_RATE::mapped_type kValue;

		result_iter->Pop(kValue.iEquipPos);	++result_iter;
		result_iter->Pop(kValue.iCostRate);	++result_iter;
		kCont.insert(std::make_pair(kValue.iEquipPos, kValue));
	}

	if( kCont.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);

	return true;
}

bool PgDBCache::Q_DQT_DEF_TACTICS_LEVEL(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed[Can't load TB_DefTacticsLevel table] Result: "<<rkResult.eRet);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_TACTICS_LEVEL kCont;
	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_TACTICS_LEVEL::mapped_type kNewElement;
	while( rkVec.end() != result_iter )
	{
		result_iter->Pop( kNewElement.sTacticsLevel );		++result_iter;
		result_iter->Pop( kNewElement.iTacticsExp );		++result_iter;
		result_iter->Pop( kNewElement.iTacticsLevelTextID);	++result_iter;

		auto kRet = kCont.insert( std::make_pair(kNewElement.sTacticsLevel, kNewElement) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't insert new element: "<<kNewElement.sTacticsLevel);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if( kCont.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);

	return true;
}





bool PgDBCache::Q_DQT_DEF_EMPORIA( CEL::DB_RESULT& rkResult )
{
	if(		CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Can't load [TB_DefGuildEmporia] table Result: "<<rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	if ( itr != rkResult.vecArray.end() )
	{
		int iCount = *(rkResult.vecResultCount.begin());

		CONT_DEF_EMPORIA map;

		while ( itr != rkResult.vecArray.end() )
		{
			if ( iCount > 0 )
			{
				CONT_DEF_EMPORIA::mapped_type kElement;

				itr->Pop( kElement.guidEmporiaID );				++itr;
				itr->Pop( kElement.iBaseMapNo );				++itr;

				for ( int i=0; i<MAX_EMPORIA_GRADE; ++i )
				{
					itr->Pop( kElement.iEmporiaGroundNo[i] );	++itr;
					itr->Pop( kElement.iBattleGroundNo[i] );	++itr;
				}

				itr->Pop( kElement.iChallengeBattleGroundNo );	++itr;
				itr->Pop( kElement.iLimitCost );	++itr;

				//Reward
				itr->Pop( kElement.kReward.iFromNo );			++itr;
				itr->Pop( kElement.kReward.iTitleNo );			++itr;
				itr->Pop( kElement.kReward.iBodyNo );			++itr;
				for(int i=0; i<TBL_DEF_EMPORIA_REWARD::ECG_END; ++i)
				{
					itr->Pop( kElement.kReward.kChallenge[i].iItem );				++itr;
					itr->Pop( kElement.kReward.kChallenge[i].iMoney );				++itr;
					itr->Pop( kElement.kReward.kChallenge[i].iGradeTextNo );		++itr;
				}
				for(int i=0; i<MAX_EMPORIA_GRADE; ++i)
				{
					itr->Pop( kElement.kReward.kGrade[i].iItem );			++itr;
					itr->Pop( kElement.kReward.kGrade[i].iMoney );			++itr;
					itr->Pop( kElement.kReward.kGrade[i].iGradeTextNo );	++itr;
				}
				itr->Pop( kElement.kReward.sLimitDay );			++itr;

				auto kPair = map.insert( std::make_pair( kElement.guidEmporiaID, kElement ) );
				if ( !kPair.second )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Overlapping EmporiaID<") << kElement.guidEmporiaID << _T(">") );
				}

				--iCount;
			}
			else
			{
				BM::GUID kEmporiaID;
				BYTE byGrade = 0;
				CONT_DEF_EMPORIA_FUNCTION::mapped_type kElement;

				itr->Pop( kEmporiaID );					++itr;
				itr->Pop( byGrade );					++itr;
				itr->Pop( kElement.nFuncNo );			++itr;
				itr->Pop( kElement.nPrice_ForWeek );	++itr;
				itr->Pop( kElement.iValue );		++itr;

				int const iGrade = static_cast<int>(byGrade-1);
				if ( iGrade < MAX_EMPORIA_GRADE )
				{
					CONT_DEF_EMPORIA::iterator itr = map.find( kEmporiaID );
					if ( itr != map.end() )
					{
						auto kPair = itr->second.m_kContDefFunc[iGrade].insert( std::make_pair( kElement.nFuncNo, kElement ) );
					}
					else
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found EmporiaID<" << kEmporiaID << L">" );
					}
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Emporia Grade<" << iGrade << L"> Error ID<" << kEmporiaID << L">" );
				}
			}
		}

		g_kCoreCenter.ClearQueryResult(rkResult);
		if ( !map.empty() )
		{
			g_kTblDataMgr.SetContDef(map);
		}

		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_MONSTER_KILL_COUNT_REWARD( CEL::DB_RESULT& rkResult )
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Can't load [TB_DefGuildEmporia] table Result: "<<rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_MONSTER_KILL_COUNT_REWARD kCont;
	CONT_DEF_MONSTER_KILL_COUNT_REWARD::iterator iter;
	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_MONSTER_KILL_COUNT_REWARD::mapped_type kNewElement;
	int iKillCount = 0;
	while( rkVec.end() != result_iter )
	{
		result_iter->Pop( iKillCount );				++result_iter;
		result_iter->Pop( kNewElement.iMinLevel );	++result_iter;
		result_iter->Pop( kNewElement.iMaxLevel );	++result_iter;
		result_iter->Pop( kNewElement.iItemNo );	++result_iter;
		result_iter->Pop( kNewElement.iCount );		++result_iter;

		iter = kCont.insert( std::make_pair(iKillCount, kNewElement) );
		//if( !kRet.second )
		//{
		//	VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[%s]-[%d] Can't insert new monster kill count: %d"), __FUNCTIONW__, __LINE__, iKillCount);
		//	return false;
		//}
	}

	if( kCont.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);

	return true;
}

void Q_DQT_DEF_ACHIEVEMENTS(CONT_DEF_ACHIEVEMENTS_SAVEIDX& kContAchievements_SaveIdx)
{
	CONT_DEF_CONT_ACHIEVEMENTS kCont;
	CONT_DEF_ACHIEVEMENTS kContAchievements;
	CONT_DEF_ITEM2ACHIEVEMENT kContItem2Achievement;

	CONT_DEF_ACHIEVEMENTS_SAVEIDX::iterator iter = kContAchievements_SaveIdx.begin();
	while( kContAchievements_SaveIdx.end() != iter )
	{
		CONT_DEF_ACHIEVEMENTS_SAVEIDX::mapped_type const& kNewElement = (*iter).second;
		auto kRet = kCont[kNewElement.iType].insert(std::make_pair(kNewElement.iValue, kNewElement));
		if( kRet.second )
		{
			kContAchievements.insert(std::make_pair(kNewElement.iIdx,kNewElement));

			CONT_DEF_ITEM2ACHIEVEMENT::mapped_type kItem;
			kItem.iItemNo = kNewElement.iItemNo;
			kItem.iSaveIdx = kNewElement.iSaveIdx;

			kContItem2Achievement.insert(std::make_pair(kItem.iItemNo,kItem));
		}
		else
		{
			DBCacheUtil::AddErrorMsg( BM::vstring() << __FL__<<L"Already exists achievements: IDX"<<kNewElement.iIdx );
		}
		++iter;
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kTblDataMgr.SetContDef(kContAchievements);
	g_kTblDataMgr.SetContDef(kContAchievements_SaveIdx);
	g_kTblDataMgr.SetContDef(kContItem2Achievement);
}

bool PgDBCache::Q_DQT_DEF_ACHIEVEMENTS(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Can't load [TB_DefAchievements] table Result: "<<rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	DBCacheUtil::PgNationCodeHelper< CONT_DEF_ACHIEVEMENTS_SAVEIDX::key_type, CONT_DEF_ACHIEVEMENTS_SAVEIDX::mapped_type, CONT_DEF_ACHIEVEMENTS_SAVEIDX > kNationCodeUtil(L"duplicate Achievement SaveIdx[" DBCACHE_KEY_PRIFIX L"]" );
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_ACHIEVEMENTS::key_type, CONT_DEF_ACHIEVEMENTS::mapped_type, CONT_DEF_ACHIEVEMENTS > kNationCodeUtil2(L"duplicate Achievement Idx[" DBCACHE_KEY_PRIFIX L"]" );
	while( rkVec.end() != result_iter )
	{
		std::wstring kNationCodeStr;
		CONT_DEF_ACHIEVEMENTS_SAVEIDX::mapped_type kNewElement;

		result_iter->Pop( kNewElement.iIdx );			++result_iter;
		result_iter->Pop( kNationCodeStr );				++result_iter;
		result_iter->Pop( kNewElement.iCategory );		++result_iter;
		result_iter->Pop( kNewElement.iTitleNo );		++result_iter;
		result_iter->Pop( kNewElement.iDiscriptionNo );	++result_iter;
		result_iter->Pop( kNewElement.iType );			++result_iter;
		result_iter->Pop( kNewElement.iValue );			++result_iter;
		result_iter->Pop( kNewElement.iItemNo );		++result_iter;
		result_iter->Pop( kNewElement.iSaveIdx );		++result_iter;
		result_iter->Pop( kNewElement.iRankPoint );		++result_iter;
		result_iter->Pop( kNewElement.iLevel );			++result_iter;

		DWORD dwValue = 0;
		BYTE bValue = 0;

		kNewElement.kStartTime.Clear();
		result_iter->Pop( dwValue );		++result_iter;	kNewElement.kStartTime.Year(std::max(static_cast<int>(dwValue - BM::PgPackedTime::BASE_YEAR),0));
		result_iter->Pop( bValue );			++result_iter;	kNewElement.kStartTime.Month(bValue);
		result_iter->Pop( bValue );			++result_iter;	kNewElement.kStartTime.Day(bValue);
		result_iter->Pop( bValue );			++result_iter;	kNewElement.kStartTime.Hour(bValue);
		result_iter->Pop( bValue );			++result_iter;	kNewElement.kStartTime.Min(bValue);

		kNewElement.kEndTime.Clear();
		result_iter->Pop( dwValue );		++result_iter;	kNewElement.kEndTime.Year(std::max(static_cast<int>(dwValue - BM::PgPackedTime::BASE_YEAR),0));
		result_iter->Pop( bValue );			++result_iter;	kNewElement.kEndTime.Month(bValue);
		result_iter->Pop( bValue );			++result_iter;	kNewElement.kEndTime.Day(bValue);
		result_iter->Pop( bValue );			++result_iter;	kNewElement.kEndTime.Hour(bValue);
		result_iter->Pop( bValue );			++result_iter;	kNewElement.kEndTime.Min(bValue);

		result_iter->Pop( kNewElement.iUseTime );		++result_iter;

		BYTE bIsUse = false;
		result_iter->Pop( bIsUse );			++result_iter;

		result_iter->Pop( kNewElement.iGroupNo );			++result_iter;
		result_iter->Pop( kNewElement.i64CustomValue );		++result_iter;

		switch( kNewElement.iCategory )
		{
			case AC_TOTAL:
			case AC_CATEGORY_01:
			case AC_CATEGORY_02:
			case AC_CATEGORY_03:
			case AC_CATEGORY_04:
			case AC_CATEGORY_05:
			case AC_CATEGORY_06:
				{
				}break;
			default:
				{
					AddErrorMsg( BM::vstring() << __FL__ << L"Wrong Achievement Category[" << kNewElement.iCategory << L"] IDX[" << kNewElement.iIdx << L"]" );
				}break;
		}

		if( !bIsUse )
		{
			//INFO_LOG(BM::LOG_LV6, __FL__<<L"Use Not Achievement SaveIdx["<<kNewElement.iSaveIdx<<L"]");
			continue;
		}

		kNationCodeUtil.Add( kNationCodeStr, kNewElement.iSaveIdx, kNewElement, __FUNCTIONW__, __LINE__);
		kNationCodeUtil2.Add( kNationCodeStr, kNewElement.iIdx, kNewElement, __FUNCTIONW__, __LINE__);
	}
	::Q_DQT_DEF_ACHIEVEMENTS( kNationCodeUtil.GetResult() ); // �Ѱ���

	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}



bool PgDBCache::Q_DQT_DEF_FILTER_UNICODE(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet) // �̰� ��� ������ �ȵ�
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"No FIlter Unicode Range");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CONT_DEF_FILTER_UNICODE kList;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != kIter)
	{
		CONT_DEF_FILTER_UNICODE::value_type kNewFilter;

		std::wstring kTemp;
		kIter->Pop(kNewFilter.iFuncCode);		++kIter;
		kIter->Pop(kNewFilter.cStart);			++kIter;
		kIter->Pop(kNewFilter.cEnd);			++kIter;
		kIter->Pop(kNewFilter.bFilterType);		++kIter;

		kList.push_back( kNewFilter );
	}

	if( !kList.empty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kList);
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" [TB_DefFIlterUnicode] table is row 0"));
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_MONSTERCARD(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet) // �̰� ��� ������ �ȵ�
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CONT_MONSTERCARD kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != kIter)
	{
		CONT_MONSTERCARD::key_type kKey;
		CONT_MONSTERCARD::mapped_type kValue;

		kIter->Pop(kKey.iOrderIndex);			++kIter;
		kIter->Pop(kKey.iCardNo);				++kIter;
		kIter->Pop(kValue);						++kIter;

		if(false == kCont.insert(std::make_pair(kKey, kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" [Q_DQT_DEF_MONSTERCARD] Table Data Error !! kKey[") << kKey.iOrderIndex << _T(", ") << kKey.iCardNo << _T("]"));
			return false;
		}
	}

	if( !kCont.empty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kCont);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" [TB_DefMonsterCard] table is row 0"));
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_MARRYTEXT(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet) // �̰� ��� ������ �ȵ�
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CONT_MARRYTEXT kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != kIter)
	{
		CONT_MARRYTEXT::key_type kKey;
		CONT_MARRYTEXT::mapped_type kValue;

		kIter->Pop(kKey);			++kIter;
		kIter->Pop(kValue);			++kIter;

		if(false == kCont.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" [Q_DQT_DEF_MARRYTEXT] Table Data Error !! kKey[") << kKey << _T("]"));
			return false;
		}
	}

	if( !kCont.empty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kCont);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" [TB_DefMarryText] table is row 0"));
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_HIDDENREWORDITEM(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet) // �̰� ��� ������ �ȵ�
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CONT_HIDDENREWORDITEM kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != kIter)
	{
		CONT_HIDDENREWORDITEM::key_type kKey;
		CONT_HIDDENREWORDITEM::mapped_type kValue;

		kIter->Pop(kKey);						++kIter;
		kIter->Pop(kValue.iMinLevel);			++kIter;
		kIter->Pop(kValue.iMaxLevel);			++kIter;
		kIter->Pop(kValue.iItemNo);				++kIter;

		CONT_HIDDENREWORDITEM::iterator iter;
		iter = kCont.insert(std::make_pair(kKey, kValue));
		/*if( !kPair.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" [Q_DQT_DEF_HIDDENREWORDITEM] Table Data Error !! kKey[") << kKey << _T("]"));
			return false;
		}*/
	}

	if( !kCont.empty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kCont);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" [TB_DefHiddenRewordItem] table is row 0"));
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_HIDDENREWORDBAG(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet) // �̰� ��� ������ �ȵ�
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CONT_HIDDENREWORDBAG kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != kIter)
	{
		CONT_HIDDENREWORDBAG::key_type kKey;
		CONT_HIDDENREWORDBAG::mapped_type kValue;

		kIter->Pop(kKey);						++kIter;
		kIter->Pop(kValue.iContainerNo);		++kIter;
		kIter->Pop(kValue.iRarityControl);		++kIter;
		kIter->Pop(kValue.iPlusUpControl);		++kIter;

		kCont.insert(std::make_pair(kKey, kValue));
	}

	if( !kCont.empty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kCont);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" [TB_DefHiddenRewordBag] table is row 0"));
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_EMOTION(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet) // �̰� ��� ������ �ȵ�
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CONT_EMOTION kCont;

	CONT_EMOTION_GROUP kContGroup;

	CEL::DB_RESULT_COUNT::const_iterator countiter = rkResult.vecResultCount.begin();
	if(countiter == rkResult.vecResultCount.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	int const iEmotionGroupCount = (*countiter); ++countiter;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

	for(int i = 0;i < iEmotionGroupCount;++i)
	{
		CONT_EMOTION_GROUP::key_type kKey;
		CONT_EMOTION_GROUP::mapped_type kValue;

		kIter->Pop(kKey.bType);					++kIter;
		kIter->Pop(kKey.iGroupNo);				++kIter;
		kIter->Pop(kValue);						++kIter;

		if(false == kContGroup.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" [TB_DefEmotionGroup] Table Data Error !! kKey[GroupNo:") << kKey.iGroupNo << _T(",Type:") << kKey.bType << _T("]"));
			return false;
		}
	}

	int const iEmotioncount = (*countiter);	++countiter;
	for(int i = 0;i < iEmotioncount;++i)
	{
		CONT_EMOTION::key_type kKey;
		CONT_EMOTION::mapped_type kValue;

		kIter->Pop(kKey.iEMotionNo);			++kIter;
		kIter->Pop(kKey.bType);					++kIter;
		kIter->Pop(kValue);						++kIter;

		if(false == kCont.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" [TB_DefEmotion] Table Data Error !! kKey[EmotionNo:") << kKey.iEMotionNo << _T(",Type:") << kKey.bType << _T("]"));
			return false;
		}
	}

	if(!kContGroup.empty() && !kCont.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kContGroup);
		g_kTblDataMgr.SetContDef(kCont);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" [TB_DefEmotionGroup] table is row 0"));
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_PET_HATCH(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	int i = 0;
	int j = 0;
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_PET_HATCH::key_type, CONT_DEF_PET_HATCH::mapped_type, CONT_DEF_PET_HATCH > kNationCodeUtil( L"duplicate HatchID[" DBCACHE_KEY_PRIFIX L"], in [TB_DefPat_Hatch]" );
	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while(itr != rkResult.vecArray.end())
	{
		std::wstring kNationCodeStr;
		CONT_DEF_PET_HATCH::mapped_type	kElement;

		itr->Pop( kNationCodeStr );								++itr;
		itr->Pop( kElement.iID );								++itr;

		for ( i = 0 ; i < PET_HATCH_MAX_CLASS ; ++i )
		{
			itr->Pop( kElement.iClass[i] );						++itr;
			itr->Pop( kElement.sLevel[i] );						++itr;
			itr->Pop( kElement.iClassRate[i] );					++itr;
			for ( j = 0 ; j < PET_HATCH_MAX_BONUSSTATUS ; ++j )
			{
				itr->Pop( kElement.iBonusStatusRateID[i][j] );	++itr;
			}
		}

		for ( i = 0; i < PET_HATCH_MAX_PERIOD ; ++i )
		{
			itr->Pop( kElement.sPeriod[i] );					++itr;
			itr->Pop( kElement.iPeriodRate[i] );				++itr;
		}

		kNationCodeUtil.Add(kNationCodeStr, kElement.iID, kElement, __FUNCTIONW__, __LINE__);
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
	return true;
}



bool PgDBCache::Q_DQT_DEF_LOAD_RECOMMENDATIONITEM(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("load failed ErrorCode = ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_RECOMMENDATION_ITEM kCont;

	CEL::DB_RESULT_COUNT::const_iterator return_iter = rkResult.vecResultCount.begin();
	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();

	if((itr!=rkResult.vecArray.end()) && (return_iter != rkResult.vecResultCount.end()))
	{
		while(itr != rkResult.vecArray.end())
		{
			CONT_DEF_RECOMMENDATION_ITEM::mapped_type kElement;

			(*itr).Pop(kElement.iIDX);			++itr;
			(*itr).Pop(kElement.iClass);		++itr;
			(*itr).Pop(kElement.iLvMin);		++itr;
			(*itr).Pop(kElement.iLvMax);		++itr;

			for(int i = 0;i < CONT_DEF_RECOMMENDATION_ITEM::mapped_type::E_MAX_RECOMMENDATION_ITEM;i++)
			{
				int iItemNo = 0;
				(*itr).Pop(iItemNo);			++itr;
				if(iItemNo)
				{
					kElement.kContItem.push_back(iItemNo);
				}
			}

			auto kPair = kCont.insert(std::make_pair(kElement.iIDX,kElement));
			if( !kPair.second )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Data Error !! [") << kElement.iIDX << _T("]"));
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed Data"));
			}
		}

		g_kTblDataMgr.SetContDef(kCont);
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


bool PgDBCache::Q_DQT_DEF_RARE_MONSTER_SPEECH(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("load failed ErrorCode = ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_RARE_MONSTER_SPEECH::key_type kTalkNo = 0;
	CONT_DEF_RARE_MONSTER_SPEECH kCont;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();

	while(itr != rkResult.vecArray.end())
	{
		(*itr).Pop(kTalkNo);					++itr;
		for(int i = 0;i < CONT_DEF_RARE_MONSTER_SPEECH::mapped_type::MAX_SPEECH_NUM;++i)
		{
			int iSpeechNo = 0;
			(*itr).Pop(iSpeechNo);		++itr;
			if(iSpeechNo)
			{
				kCont[kTalkNo].push_back(iSpeechNo);
			}
		}
	}

	g_kTblDataMgr.SetContDef(kCont);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_CARD_LOCAL(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_CARD_LOCAL::mapped_type kData;
	CONT_CARD_LOCAL kTable;
	while( rkVec.end() != result_iter )
	{
		result_iter->Pop( kData.iLocal );		++result_iter;
		result_iter->Pop( kData.kText);			++result_iter;
		if(false == kTable.insert(std::make_pair(kData.iLocal,kData)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Card Local Key Error!!") << kData.iLocal << kData.kText);
			return false;
		}
	}

	g_kTblDataMgr.SetContDef(kTable);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_HOMETOWNTOMAPCOST(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_HOMETOWNTOMAPCOST kTable;

	while( rkVec.end() != result_iter )
	{
		CONT_HOMETOWNTOMAPCOST::key_type kKey;
		CONT_HOMETOWNTOMAPCOST::mapped_type kValue;
		result_iter->Pop( kKey );			++result_iter;
		result_iter->Pop( kValue.iCost );	++result_iter;

		if(false == kTable.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ 
				<< _T("HOMETOWN MAP MOVE LEVEL LIMIT ERROR !!") 
				<< _T(" LEVEL LIMIT :") << kKey);
			return false;
		}
	}

	g_kTblDataMgr.SetContDef(kTable);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}


bool PgDBCache::Q_DQT_LOAD_DEF_MYHOME_TEX(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_MYHOME_TEX kTable;

	while( rkVec.end() != result_iter )
	{
		CONT_MYHOME_TEX::key_type kKey;
		CONT_MYHOME_TEX::mapped_type kValue;
		result_iter->Pop( kKey );			++result_iter;
		result_iter->Pop( kValue.iTex );	++result_iter;

		if(false == kTable.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ 
				<< _T("MYHOME AUCTION LEVEL LIMIT ERROR !!") 
				<< _T(" LEVEL LIMIT :") << kKey);
			return false;
		}
	}

	g_kTblDataMgr.SetContDef(kTable);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_SKILLIDX_TO_SKILLNO(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_IDX2SKILLNO kTable;

	while( rkVec.end() != result_iter )
	{
		CONT_IDX2SKILLNO::key_type kKey;
		CONT_IDX2SKILLNO::mapped_type kValue;
		result_iter->Pop( kKey );			++result_iter;
		result_iter->Pop( kValue );			++result_iter;

		if(false == kTable.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("SKILL IDX ERROR !!") << _T("Key:") << kKey);
			return false;
		}
	}

	g_kTblDataMgr.SetContDef(kTable);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_DEF_LOAD_CARDABIL(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T(" Table is Empty!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_CARD_ABIL::key_type kKey;
	CONT_CARD_ABIL::mapped_type kValue;
	CONT_CARD_ABIL kCont;

	while( rkVec.end() != result_iter )
	{
		result_iter->Pop( kKey.kConstellation );		++result_iter;
		result_iter->Pop( kKey.kBlood );				++result_iter;
		result_iter->Pop( kKey.kHobby );				++result_iter;
		result_iter->Pop( kKey.kStyle );				++result_iter;
		result_iter->Pop( kValue.wAbilType );			++result_iter;
		result_iter->Pop( kValue.iValue);				++result_iter;

		if(false == kCont.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T(" Card Abil Key Error !! Key : ") 
				<< kKey.kConstellation << _T(" : ") << kKey.kBlood << _T(" : ") << kKey.kHobby << _T(" : ") << kKey.kStyle);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);

	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_CARD_KEY_STRING(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_CARD_KEY_STRING::key_type kKey;
	CONT_DEF_CARD_KEY_STRING::mapped_type kData;
	CONT_DEF_CARD_KEY_STRING kTable;
	while( rkVec.end() != result_iter )
	{
		result_iter->Pop( kKey.bKeyType );		++result_iter;
		result_iter->Pop( kKey.bKeyValue);		++result_iter;
		result_iter->Pop( kData.iStringNo);		++result_iter;
		result_iter->Pop( kData.bCardKey);		++result_iter;

		if(false == kTable.insert(std::make_pair(kKey,kData)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Card Local Key Error!!") << _T("KeyType:") << kKey.bKeyType << _T("KeyValue:") << kKey.bKeyValue << _T("StringNo") << kData.iStringNo);
			return false;
		}
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kTable);
	return true;
}

bool PgDBCache::Q_DQT_DEF_CONVERTITEM(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return false;
	}

	CONT_DEF_CONVERTITEM kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

	while(kIter != rkResult.vecArray.end())
	{
		CONT_DEF_CONVERTITEM::mapped_type			kValue;

		kIter->Pop(kValue.iSourceItemNo);			++kIter;
		kIter->Pop(kValue.sSourceItemNum);			++kIter;
		kIter->Pop(kValue.iTargetItemNo);			++kIter;
		kIter->Pop(kValue.sTargetItemNum);			++kIter;

		kCont.insert(std::make_pair(kValue.iSourceItemNo,kValue));
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);
	return true;
}

bool PgDBCache::Q_DQT_DEF_GROUND_RARE_MONSTER(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("load failed ErrorCode = ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_GROUND_RARE_MONSTER::key_type kGroundNo = 0;
	CONT_DEF_GROUND_RARE_MONSTER kCont;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();

	while(itr != rkResult.vecArray.end())
	{
		CONT_DEF_GROUND_RARE_MONSTER::mapped_type::mapped_type kElement;

		(*itr).Pop(kGroundNo);					++itr;
		(*itr).Pop(kElement.iMonNo);			++itr;
		(*itr).Pop(kElement.iGenGroupNo);		++itr;
		(*itr).Pop(kElement.iDelayGenTime);		++itr;
		(*itr).Pop(kElement.iRegenTime);		++itr;

		auto kPair = kCont[kGroundNo].insert(std::make_pair(kElement.iMonNo,kElement));
		if( !kPair.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Data Error !! GroundNo [") << kGroundNo << _T("] MonsterNo [") << kElement.iMonNo << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed Data"));
		}
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);
	return true;
}

bool PgDBCache::Q_DQT_DEF_TRANSTOWER(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("load failed ErrorCode = ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_TRANSTOWER	kCont;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while(itr != rkResult.vecArray.end())
	{
		BM::GUID					kGuid;
		TBL_DEF_TRANSTOWER_TARGET	kElement;

		itr->Pop( kGuid );						++itr;
		itr->Pop( kElement.nSort );				++itr;
		itr->Pop( kElement.iGroundNo );			++itr;
		itr->Pop( kElement.nTargetSpawn );		++itr;
		itr->Pop( kElement.i64Price );			++itr;
		itr->Pop( kElement.iMemo );				++itr;
		
		auto kPair = kCont.insert( std::make_pair( kGuid, CONT_DEF_TRANSTOWER_TARGET() ) );
		auto kPair2 = kPair.first->second.insert( kElement );
		if ( !kPair2.second )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Data Error !! GUID<" << kGuid << L"> GroundNo<" << kElement.iGroundNo << L"> TargetSpawn<" << kElement.nTargetSpawn << L"> is Overlaping Data!!");
		}
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);
	return true;
}

bool PgDBCache::Q_DQT_DEF_PARTY_INFO(CEL::DB_RESULT& rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("load failed ErrorCode = ") << rkResult.eRet );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_PARTY_INFO	kCont;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while(itr != rkResult.vecArray.end())
	{
		CONT_DEF_PARTY_INFO::key_type			iContinent;
		TBL_DEF_PARTY_INFO_TARGET				kElement;
		int	iUse = 0;

		itr->Pop( kElement.iAttribute );		++itr;
		itr->Pop( iContinent );					++itr;
		itr->Pop( kElement.iSort );				++itr;
		itr->Pop( kElement.iArea_NameNo );		++itr;
		itr->Pop( iUse );						++itr;
		itr->Pop( kElement.iGroundNo );			++itr;
		
		if( 0 < iUse )
		{
			auto kPair = kCont.insert( std::make_pair( iContinent, CONT_DEF_PARTY_INFO_TARGET() ) );
			auto kPair2 = kPair.first->second.insert( kElement );
			if ( !kPair2.second )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Data Error !! GUID<" << iContinent << L"> Attribute<" << kElement.iAttribute << L"> Sort<" << kElement.iSort << L"> Area<" << kElement.iArea_NameNo << L"> is Overlaping Data!!");
			}
		}
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);
	return true;
}

bool PgDBCache::Q_DQT_DEF_MAP_ITEM_BAG(CEL::DB_RESULT& rkResult)
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	DBCacheUtil::PgNationCodeHelper< CONT_DEF_MAP_ITEM_BAG::key_type, CONT_DEF_MAP_ITEM_BAG::mapped_type, CONT_DEF_MAP_ITEM_BAG > kNationCodeUtil( L"duplicate GroundNo[" DBCACHE_KEY_PRIFIX L"] in [TB_DefMapItemBag]" );
	while(rkResult.vecArray.end() != itor)
	{
		std::wstring kNationCodeStr;
		CONT_DEF_MAP_ITEM_BAG::mapped_type element;

		(*itor).Pop( kNationCodeStr );			++itor;
		(*itor).Pop( element.iMapNo );			++itor;
		(*itor).Pop( element.iSuccessRateNo );	++itor;

		for(int i = 0; MAP_ITEM_BAG_GRP > i ; i++)
		{
			(*itor).Pop( element.aiBagGrpNo[i] );	++itor;
		}

		kNationCodeUtil.Add(kNationCodeStr, element.iMapNo, element, __FUNCTIONW__, __LINE__);
	}

	if(!kNationCodeUtil.IsEmpty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_MISSION_RESULT(CEL::DB_RESULT& rkResult)
{
	//	select [ResultNo], [RankSS], [RankS], [RankA], [RankB], [RankC], [RankD], [RankE] FROM TB_DefMission_Result
	CONT_DEF_MISSION_RESULT map;

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != itor)
	{
		CONT_DEF_MISSION_RESULT::mapped_type kElement;	
		//CONT_DEF_MISSION_RESULT::key_type kKey;

		(*itor).Pop(kElement.iResultNo);			++itor;
		for(int i = 0; MAX_MISSION_RANK_CLEAR_LV > i ; i++)
		{
			(*itor).Pop(kElement.aiResultContainer[i]);		++itor;
		}
		(*itor).Pop(kElement.iRouletteBagGrpNo);	++itor;

		for(int i = 0; MAX_MISSION_RANK_CLEAR_LV > i ; i++)
		{
			(*itor).Pop(kElement.aiResultCountMin[i]);		++itor;
			(*itor).Pop(kElement.aiResultCountMax[i]);		++itor;
		}

		auto ret = map.insert(std::make_pair(kElement.iResultNo, kElement));
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_DEF_OBJECT_BAG_ELEMENTS(CEL::DB_RESULT& rkResult)
{
	CONT_DEF_OBJECT_BAG_ELEMENTS map;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != itr)
	{
		CONT_DEF_OBJECT_BAG_ELEMENTS::mapped_type kElement;	

		itr->Pop(kElement.iElementNo);			++itr;
		itr->Pop(kElement.iObjectNo);			++itr;
		itr->Pop(kElement.pt3RelativePos.x);	++itr;
		itr->Pop(kElement.pt3RelativePos.y);	++itr;
		itr->Pop(kElement.pt3RelativePos.z);	++itr;

		auto ret = map.insert(std::make_pair(kElement.iElementNo, kElement));
		if ( !ret.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"ElementNoo["<<kElement.iElementNo<<L"] Error!!");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
	}	
	return true;
}

bool PgDBCache::Q_DQT_DEF_OBJECT_BAG(CEL::DB_RESULT& rkResult)
{
	CONT_DEF_OBJECT_BAG map;

	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	while(rkResult.vecArray.end() != itr)
	{
		CONT_DEF_OBJECT_BAG::mapped_type kElement;	

		itr->Pop(kElement.iBagNo);				++itr;
		for ( int i=0; i!=MAX_OBJECT_ELEMENT_NO; ++i )
		{
			itr->Pop(kElement.iElementNo[i]);	++itr;
		}

		auto ret = map.insert(std::make_pair(kElement.iBagNo, kElement));
		if ( !ret.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"BagNo["<<kElement.iBagNo<<L"] Error!!");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
	}	
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_CHARCARDEFFECT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_DEFCHARCARDEFFECT kCont;

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		CONT_DEFCHARCARDEFFECT::key_type	kKey;

		(*iter).Pop(kKey);					++iter;

		int iValue = 0;

		(*iter).Pop(iValue);				++iter;

		kCont[kKey].push_back(iValue);
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);

	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_SIDEJOBRATE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_DEFSIDEJOBRATE kCont;

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		CONT_DEFSIDEJOBRATE::key_type	kKey;
		CONT_DEFSIDEJOBRATE::mapped_type kValue;

		(*iter).Pop(kKey);					++iter;
		(*iter).Pop(kValue.iCostRate);		++iter;
		(*iter).Pop(kValue.iSoulRate);		++iter;
		(*iter).Pop(kValue.iSuccessRate);	++iter;
		(*iter).Pop(kValue.iPayRate);		++iter;

		if(false == kCont.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"SIDE JOB KEY ["<<kKey<<L"] ERROR!!");
			return false;
		}
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);

	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_EVENTITEMSET(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_DEF_EVENT_ITEM_SET kCont;

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		CONT_DEF_EVENT_ITEM_SET::value_type kNewElement;
		(*iter).Pop(kNewElement.iEventItemSetID);				++iter;
		(*iter).Pop(kNewElement.bIsUse);						++iter;
		(*iter).Pop(kNewElement.bIsAbsoluteCount);				++iter;
		(*iter).Pop(kNewElement.iEquipItemNo);					++iter;
		(*iter).Pop(kNewElement.iItemCount);					++iter;
		for( int iCur = 0; MAX_EVENT_ITEM_SET_REWARD > iCur; ++iCur )
		{
			(*iter).Pop(kNewElement.aiRewardEffectID[iCur]);	++iter;
		}

		kCont.push_back( kNewElement );
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_REDICEOPTIONCOST(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_DEFREDICEOPTIONCOST kCont;

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		CONT_DEFREDICEOPTIONCOST::key_type kKey;
		CONT_DEFREDICEOPTIONCOST::mapped_type kValue;
		(*iter).Pop(kKey);							++iter;
		(*iter).Pop(kValue);						++iter;
		if(false == kCont.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L" KEY ["<<kKey<<L"] ERROR!!");
			return false;
		}
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_MYHOMESIDEJOBTIME(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_DEFMYHOMESIDEJOBTIME kCont;

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		CONT_DEFMYHOMESIDEJOBTIME::key_type kKey;
		CONT_DEFMYHOMESIDEJOBTIME::mapped_type kValue;
		(*iter).Pop(kKey);							++iter;
		(*iter).Pop(kValue);						++iter;
		if(false == kCont.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L" KEY ["<<kKey<<L"] ERROR!!");
			return false;
		}
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_MONSTER_ENCHANT_GRADE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	std::wstring kTemp;
	CONT_DEF_MONSTER_ENCHANT_GRADE kCont;
	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		CONT_DEF_MONSTER_ENCHANT_GRADE::key_type kKey;
		CONT_DEF_MONSTER_ENCHANT_GRADE::mapped_type kValue;
		(*iter).Pop(kKey);							++iter;
		(*iter).Pop(kValue.iEnchantLevel);			++iter;
		(*iter).Pop(kValue.iPrefixNameNo);			++iter;
		for( int iCur = 0; MAX_MONSTER_ENCHANT_EFFECT_COUNT > iCur; ++iCur )
		{
			(*iter).Pop(kTemp);						++iter;		kValue.akEffectName[iCur] = MB(kTemp);
			(*iter).Pop(kTemp);						++iter;		kValue.akEffectRoot[iCur] = MB(kTemp);
		}
		for( int iCur = 0; MAX_MONSTER_ENCHANT_ADD_ABIL_COUNT > iCur; ++iCur )
		{
			(*iter).Pop(kValue.aiAddAbilNo[iCur]);	++iter;
		}
		for( int iCur = 0; MAX_MONSTER_ENCHANT_SET_ABIL_COUNT > iCur; ++iCur )
		{
			(*iter).Pop(kValue.aiSetAbilNo[iCur]);	++iter;
		}
		if(false == kCont.insert(std::make_pair(kKey,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L" KEY ["<<kKey<<L"] ERROR!!");
			return false;
		}
	}

	{
		CONT_DEFMONSTERABIL const* pkDefMonAbil = NULL;
		g_kTblDataMgr.GetContDef(pkDefMonAbil);
		CONT_DEF_MONSTER_ENCHANT_GRADE::const_iterator grade_iter = kCont.begin();
		while( kCont.end() != grade_iter )
		{
			CONT_DEF_MONSTER_ENCHANT_GRADE::key_type const& rkKey = (*grade_iter).first;
			CONT_DEF_MONSTER_ENCHANT_GRADE::mapped_type const& rkVal = (*grade_iter).second;

			for( int iCur = 0; MAX_MONSTER_ENCHANT_ADD_ABIL_COUNT > iCur; ++iCur )
			{
				int const iMonAbilNo = rkVal.aiAddAbilNo[iCur];
				if( 0 < iMonAbilNo
				&&	pkDefMonAbil->end() == pkDefMonAbil->find(iMonAbilNo) )
				{
					AddErrorMsg(BM::vstring()<<__FL__<<L"Wrong MonsterEnchant[AddAbil"<<(iCur+1)<<L":"<<iMonAbilNo<<L", EnchantGradeNo: "<<rkKey<<L"], Can't find in [TB_DefMonsterAbil] table" );
				}
			}
			for( int iCur = 0; MAX_MONSTER_ENCHANT_SET_ABIL_COUNT > iCur; ++iCur )
			{
				int const iMonAbilNo = rkVal.aiSetAbilNo[iCur];
				if( 0 < iMonAbilNo
				&&	pkDefMonAbil->end() == pkDefMonAbil->find(iMonAbilNo) )
				{
					AddErrorMsg(BM::vstring()<<__FL__<<L"Wrong MonsterEnchant[SetAbil"<<(iCur+1)<<L":"<<iMonAbilNo<<L", EnchantGradeNo: "<<rkKey<<L"], Can't find in [TB_DefMonsterAbil] table" );
				}
			}
			++grade_iter;
		}
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_BASICOPTIONAMP(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_DEFBASICOPTIONAMP kCont;
	CONT_DEFBASICOPTIONAMP::key_type		kKey;
	CONT_DEFBASICOPTIONAMP::mapped_type		kValue;

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		(*iter).Pop(kValue.iEquipPos);			++iter;
		(*iter).Pop(kValue.iLevelLimit);		++iter;
		(*iter).Pop(kValue.iAmpLv);				++iter;
		(*iter).Pop(kValue.iAmpMaxLv);			++iter;
		(*iter).Pop(kValue.iAmpRate);			++iter;
		(*iter).Pop(kValue.iSuccessRate);		++iter;
		(*iter).Pop(kValue.iNeedCost);			++iter;
		(*iter).Pop(kValue.iNeedSoulCount);		++iter;
		(*iter).Pop(kValue.iAmpItemNo);			++iter;
		(*iter).Pop(kValue.iAmpItemCount);		++iter;
		(*iter).Pop(kValue.iInsuranceItemNo);	++iter;

		kKey.kPriKey = kValue.iEquipPos;
		kKey.kSecKey = kValue.iLevelLimit;
		kKey.kTrdKey = kValue.iAmpLv;

		if(false == kCont.insert(std::make_pair(kKey, kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L" KEY ["<< kKey.kPriKey <<L","<< kKey.kSecKey <<L","<< kKey.kTrdKey <<L"] ERROR!!");
			return false;
		}
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_ITEM_AMP_SPECIFIC( CEL::DB_RESULT &rkResult )
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();

	std::wstring kNationCodeStr;
	CONT_DEFITEM_AMP_SPECIFIC DefaultMap;
	CONT_DEFITEM_AMP_SPECIFIC map;
	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEFITEM_AMP_SPECIFIC::mapped_type element;
		int iEquipType = 0;
		WORD wAbilType = 0;
		int iRate = 0;
		(*itor).Pop( kNationCodeStr );	++itor;
		(*itor).Pop( iEquipType );		++itor;
		(*itor).Pop( wAbilType );		++itor;
		(*itor).Pop( iRate );			++itor;

		if( DBCacheUtil::IsDefaultNation(kNationCodeStr) )
		{
			SDefItemAmplify_Specific empty(iEquipType);
			auto kRet = DefaultMap.insert(std::make_pair(iEquipType, empty));
			kRet.first->second.kContSpecific.insert(std::make_pair(wAbilType, iRate));
		}
		else
		{
			if( DBCacheUtil::IsCanNation< CONT_DEFITEM_AMP_SPECIFIC::key_type, BM::vstring, DBCacheUtil::AddError >(kNationCodeStr, iEquipType, __FUNCTIONW__, __LINE__) )
			{
				SDefItemAmplify_Specific empty(iEquipType);
				auto kRet = map.insert(std::make_pair(iEquipType, empty));
				kRet.first->second.kContSpecific.insert(std::make_pair(wAbilType, iRate));
			}
		}
	}

	if( map.size() || DefaultMap.size() )
	{
		map.insert(DefaultMap.begin(), DefaultMap.end());

		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_LOAD_DEF_MYHOMEBUILDINGS(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_DEFMYHOMEBUILDINGS kContHB;
	CONT_DEFGROUNDBUILDINGS kContGB;

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		CONT_DEFMYHOMEBUILDINGS::mapped_type	kValue;
		(*iter).Pop(kValue.iBuildingNo);		++iter;
		(*iter).Pop(kValue.iGroundNo);			++iter;
		(*iter).Pop(kValue.iGrade);				++iter;
		(*iter).Pop(kValue.iMaxRoom);			++iter;

		if(false == kContHB.insert(std::make_pair(kValue.iBuildingNo,kValue)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L" KEY ["<<kValue.iBuildingNo<<L"] ERROR!!");
			return false;
		}

		kContGB[kValue.iGroundNo].kCont.push_back(kValue);
	}

	g_kTblDataMgr.SetContDef(kContHB);
	g_kTblDataMgr.SetContDef(kContGB);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_ALRAM_MISSION(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << L" DB Query failed ErrorCode=" << rkResult.eRet << L", Query=" << rkResult.Command());
		return false;
	}

	if ( 2 != rkResult.vecResultCount.size() )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << L" DB Query Result Count= " << rkResult.vecResultCount.size() << L" Error, Query=" << rkResult.Command() );
		return false;
	}

	CEL::DB_RESULT_COUNT::const_iterator count_itr = rkResult.vecResultCount.begin();
	int iCount = *count_itr;

	CONT_DEF_ALRAM_MISSION temp_map;

	CEL::DB_DATA_ARRAY::iterator itr = rkResult.vecArray.begin();
	while(itr != rkResult.vecArray.end())
	{
		if ( 0 < iCount )
		{
			TBL_DEF_ALRAM_MISSION kElement;
			itr->Pop( kElement.iAlramID );			++itr;
			itr->Pop( kElement.iType );				++itr;
			itr->Pop( kElement.iTitleNo );			++itr;
			itr->Pop( kElement.iAlramID_Next );		++itr;
			itr->Pop( kElement.nIsDraClass );		++itr;
			itr->Pop( kElement.i64ClassLimit );		++itr;
			itr->Pop( kElement.iTime );				++itr;
			itr->Pop( kElement.nPoint );			++itr;
			itr->Pop( kElement.iExp );				++itr;
			itr->Pop( kElement.iEffect );			++itr;

			kElement.i64ClassLimit = HumanAddFiveJobClassLimit(kElement.i64ClassLimit);
			for ( int i=0 ; i < MAX_ALRAM_MISSION_ITEMBAG; ++i )
			{
				itr->Pop( kElement.iItemBag[i] );	++itr;
			}

			temp_map.insert( std::make_pair( kElement.iAlramID, kElement ) );

			--iCount;
		}
		else
		{
			int iAlramID = 0;
			TBL_DEF_ALRAM_MISSION_ACTION kElement;
			itr->Pop( iAlramID );					++itr;
			itr->Pop( kElement.iDiscriptionNo );	++itr;
			itr->Pop( kElement.iActionType );		++itr;
			itr->Pop( kElement.iObjectType );		++itr;
			itr->Pop( kElement.i64ObjectValue );	++itr;
			itr->Pop( kElement.iCount );			++itr;
			itr->Pop( kElement.byFailType );		++itr;

			CONT_DEF_ALRAM_MISSION::iterator map_itr = temp_map.find( iAlramID );
			if ( map_itr != temp_map.end() )
			{
				map_itr->second.kActionList.push_back( kElement );
			}
		}
	}

	// def�� �޸� ���� ���� �ϱ� ����
	CONT_DEF_ALRAM_MISSION map;
	map = temp_map;// ��������ڿ��� reserve�� �Ѵ�.

	g_kTblDataMgr.SetContDef(map);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_DEATHPENALTY(CEL::DB_RESULT& rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}
	
	CONT_DEFDEATHPENALTY kCont;
	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		VEC_DEFDEATHPENALTY::value_type kValue;
		(*iter).Pop(kValue.iGroundAttr);++iter;
		(*iter).Pop(kValue.iGroundNo);++iter;
		(*iter).Pop(kValue.sLevelMin);++iter;
		(*iter).Pop(kValue.sLevelMax);++iter;
		(*iter).Pop(kValue.bySuccessionalDeath);++iter;
		for (int i=0; i<VEC_DEFDEATHPENALTY::value_type::MAX_DEATH_PENALTY_EFFECTSIZE; i++)
		{
			(*iter).Pop(kValue.iEffect[i]);++iter;
		}
		
		CONT_DEFDEATHPENALTY::iterator itor_death = kCont.find(kValue.iGroundNo);
		if (itor_death == kCont.end())
		{
			auto ibRet = kCont.insert(std::make_pair(kValue.iGroundNo, VEC_DEFDEATHPENALTY()));
			itor_death = ibRet.first;
		}

		VEC_DEFDEATHPENALTY& kInfoVec = (*itor_death).second;
		kInfoVec.push_back(kValue);

		++itor_death;
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_SKILLEXTENDITEM(CEL::DB_RESULT& rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}
	
	CONT_DEFSKILLEXTENDITEM kCont;
	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		CONT_DEFSKILLEXTENDITEM::key_type kKey;
		CONT_DEFSKILLEXTENDITEM::mapped_type kValue;

		(*iter).Pop(kKey);				++iter;
		for(int i = 0;i < MAX_SKILLEXTENDITEM_NUM;++i)
		{
			int iSkillNo = 0;
			(*iter).Pop(iSkillNo);++iter;
			if(0 < iSkillNo)
			{
				kCont[kKey].kCont.insert(iSkillNo);
			}
		}
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_NPC_TALK_MAP_MOVE(CEL::DB_RESULT& rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_DEFMAP const* pkDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkDefMap);
	
	CONT_TBL_DEF_NPC_TALK_MAP_MOVE kCont;
	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		CONT_TBL_DEF_NPC_TALK_MAP_MOVE::key_type kKey;
		CONT_TBL_DEF_NPC_TALK_MAP_MOVE::mapped_type::value_type kValue;

		(*iter).Pop(kKey);						++iter;
		(*iter).Pop(kValue.kNpcGuid);			++iter;
		(*iter).Pop(kValue.iTargetGroundNo);	++iter;
		(*iter).Pop(kValue.nTargetPortalNo);	++iter;

		bool const bFindOrg = pkDefMap->end() != pkDefMap->find(kKey);
		if( !bFindOrg )
		{
			AddErrorMsg( BM::vstring() << __FL__ << L"Error GroundNo, [TB_DefNpcTalkMapMove] table in [f_GroundNo]["<<kKey<<L"]" );
		}
		
		CONT_DEFMAP::const_iterator map_iter = pkDefMap->find(kValue.iTargetGroundNo);
		if( bFindOrg
		&&	pkDefMap->end() != map_iter )
		{
			switch( (*map_iter).second.iAttr )
			{
			case GATTR_DEFAULT:
			case GATTR_VILLAGE:
			case GATTR_HOMETOWN:
			case GATTR_STATIC_DUNGEON:
				{
					auto kRet = kCont.insert( std::make_pair(kKey, CONT_TBL_DEF_NPC_TALK_MAP_MOVE::mapped_type()) );
					(*kRet.first).second.push_back( kValue );
				}break;
			default:
				{
					AddErrorMsg( BM::vstring() << __FL__ << L"Wrong Attribute [Attribute]["<<(*map_iter).second.iAttr<<L"], [TB_DefNpcTalkMapMove] table in [f_GroundNo]["<<kKey<<L"] [f_TargetGroundNo]["<<kValue.iTargetGroundNo<<L"]"  );
				}break;
			}
		}
		else
		{
			if( pkDefMap->end() == map_iter )
			{
				AddErrorMsg( BM::vstring() << __FL__ << L"Error [f_TargetGroundNo]["<<kValue.iTargetGroundNo<<L"], [TB_DefNpcTalkMapMove] table in [f_GroundNo]["<<kKey<<L"]" );
			}
		}
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_SPECIFIC_REWARD(CEL::DB_RESULT& rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	std::wstring NationCodeStr;

	CONT_DEF_SPECIFIC_REWARD DefaultCont;
	CONT_DEF_SPECIFIC_REWARD kCont;
	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while( iter != rkResult.vecArray.end() )
	{	
		CONT_DEF_SPECIFIC_REWARD::mapped_type kValue;
		
		(*iter).Pop(NationCodeStr);			++iter;
		(*iter).Pop(kValue.iType);			++iter;
		(*iter).Pop(kValue.iSubType);		++iter;
		(*iter).Pop(kValue.iRewardItemNo);	++iter;
		(*iter).Pop(kValue.i64RewardCount);	++iter;

		if( DBCacheUtil::IsDefaultNation( NationCodeStr ) )
		{
			DefaultCont.insert( std::make_pair( POINT2(kValue.iType, kValue.iSubType), kValue ) );
		}
		else
		{
			if( DBCacheUtil::IsCanNation< int, BM::vstring, DBCacheUtil::AddError >(NationCodeStr, kValue.iType, __FUNCTIONW__, __LINE__) )
			{
				kCont.insert( std::make_pair( POINT2(kValue.iType, kValue.iSubType), kValue ) );
			}
		}
	}

	if( kCont.size() || DefaultCont.size() )
	{
		kCont.insert( DefaultCont.begin(), DefaultCont.end() );
	}

	g_kTblDataMgr.SetContDef(kCont);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_SPECIFIC_REWARD_EVENT(CEL::DB_RESULT & Result)
{
	if(CEL::DR_SUCCESS != Result.eRet && CEL::DR_NO_RESULT != Result.eRet )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << Result.eRet << _T(", Query=") << Result.Command());
		return false;
	}

	CONT_DEF_SPECIFIC_REWARD_EVENT Cont;
	CEL::DB_DATA_ARRAY::iterator iter = Result.vecArray.begin();
	while( iter != Result.vecArray.end() )
	{
		CONT_DEF_SPECIFIC_REWARD_EVENT::mapped_type Value;

		(*iter).Pop(Value.EventNo);				++iter;
		(*iter).Pop(Value.StartTime);			++iter;
		(*iter).Pop(Value.EndTime);				++iter;
		(*iter).Pop(Value.LevelLimit);			++iter;
		(*iter).Pop(Value.DraLevelLimit);		++iter;
		(*iter).Pop(Value.RewardCount);			++iter;
		(*iter).Pop(Value.RewardPeriod);		++iter;
		(*iter).Pop(Value.RewardItemGroupNo);	++iter;

		Cont.insert( std::make_pair( Value.EventNo, Value ) );
	}

	g_kTblDataMgr.SetContDef(Cont);
	g_kCoreCenter.ClearQueryResult(Result);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_MISSION_MUTATOR(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_DEF_MISSION_MUTATOR::mapped_type kNewElement;
	CONT_DEF_MISSION_MUTATOR kMissionMutator;
	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();

	while(iter != rkResult.vecArray.end())
	{
		iter->Pop(kNewElement.iMutatorNo);				++iter;
		iter->Pop(kNewElement.iMutatorNameNo);			++iter;
		iter->Pop(kNewElement.iDescription);			++iter;
		iter->Pop(kNewElement.iAddedExpRate);			++iter;
		iter->Pop(kNewElement.iAddedMoneyRate);			++iter;
		iter->Pop(kNewElement.iMutatorType);			++iter;
		iter->Pop(kNewElement.iMutatorUnitType);		++iter;
		for(int i = 0; i < MAX_MUTATORABIL; i++)
		{
			iter->Pop(kNewElement.iAbil[i]);			++iter;
		}

		switch(kNewElement.iMutatorType)
		{
		case MISSION_MUTATOR_TYPE_NONE:
		case MISSION_MUTATOR_TYPE_POSITIVE:
		case MISSION_MUTATOR_TYPE_NEGATIVE:
			{
			}break;
		default:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << L"Unknow Mutator Type[" << kNewElement.iMutatorType << "] MutatorNo[" << kNewElement.iMutatorNo << "]");
			}break;
		}

		switch(kNewElement.iMutatorUnitType)
		{
		case MISSION_MUTATOR_UNIT_NONE:
		case MISSION_MUTATOR_UNIT_MONSTER:
		case MISSION_MUTATOR_UNIT_PLAYER:
		case MISSION_MUTATOR_UNIT_OBJ:
		case MISSION_MUTATOR_UNIT_PET:
		case MISSION_MUTATOR_UNIT_SUMMONER:
			{
			}break;
		default:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << L"Unknow Mutator Unit Type[" << kNewElement.iMutatorUnitType << "] MutatorNo[" << kNewElement.iMutatorNo <<"]");
			}break;
		}

		bool bRet = kMissionMutator.insert(std::make_pair(kNewElement.iMutatorNo, kNewElement)).second;
		ASSERT_LOG(bRet, BM::LOG_LV4, __FL__ << L"Duplicate MutatorNo[" <<  kNewElement.iMutatorNo << "] in [TB_DefMission_Mutator]");
	}

	g_kTblDataMgr.SetContDef(kMissionMutator);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_MISSION_MUTATOR_ABIL(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_DEF_MISSION_MUTATOR_ABIL::mapped_type kNewElement;
	CONT_DEF_MISSION_MUTATOR_ABIL kMutatorAbil;

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while(iter != rkResult.vecArray.end())
	{
		iter->Pop(kNewElement.iAbilNo);		++iter;
		for(int i = 0; i < MAX_MUTATUR_ABIL_TYPE; i++)
		{
			iter->Pop(kNewElement.iType[i]);	++iter;
			iter->Pop(kNewElement.iValue[i]);	++iter;
		}
		bool bRet = kMutatorAbil.insert(std::make_pair(kNewElement.iAbilNo, kNewElement)).second;
		ASSERT_LOG(bRet, BM::LOG_LV4, __FL__ << L"Duplicate MutatorNo[" <<  kNewElement.iAbilNo << "] in [TB_DefMission_Mutator]");
	}

	g_kTblDataMgr.SetContDef(kMutatorAbil);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_JOBSKILL_PROBABILITY(CEL::DB_RESULT& rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_PROBABILITY::value_type kNewProbability;
	CONT_DEF_JOBSKILL_PROBABILITY_BAG::mapped_type kNewElement;

	CONT_DEF_JOBSKILL_PROBABILITY_BAG kContMap;

	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_JOBSKILL_PROBABILITY_BAG::key_type, CONT_DEF_JOBSKILL_PROBABILITY_BAG::mapped_type, CONT_DEF_JOBSKILL_PROBABILITY_BAG > NationCodeUtil( L"Duplicate iNo[" DBCACHE_KEY_PRIFIX L"]" );

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != iter )
	{
		int iType = 0;
		(*iter).Pop(NationCodeStr);						++iter;
		(*iter).Pop(kNewElement.iNo);					++iter;
		(*iter).Pop(iType);								++iter;
		(*iter).Pop(kNewElement.iTotalProbability);		++iter;

		kNewElement.kContProbability.clear();
		int const iMaxCount = 10;
		for( int iCur = 0; iMaxCount > iCur; ++iCur )
		{
			(*iter).Pop(kNewProbability.iResultNo);		++iter;
			(*iter).Pop(kNewProbability.iCount);		++iter;
			(*iter).Pop(kNewProbability.iProbability);	++iter;
			kNewElement.Add( kNewProbability );
		}
		(*iter).Pop(kNewElement.i01NeedSaveIdx);		++iter;

		kNewElement.eType = static_cast< EProbablityType >(iType);
		
		NationCodeUtil.Add(NationCodeStr, kNewElement.iNo, kNewElement, __FUNCTIONW__, __LINE__);
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(NationCodeUtil.GetResult());
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_LOAD_DEF_JOBSKILL_ITEMUPGRADE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}
	
	CONT_DEF_JOBSKILL_ITEM_UPGRADE kContMap;
	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != iter )
	{
		CONT_DEF_JOBSKILL_ITEM_UPGRADE::mapped_type kNewElement;
		(*iter).Pop(kNewElement.iItemNo);					++iter;
		(*iter).Pop(kNewElement.iGrade);					++iter;
		(*iter).Pop(kNewElement.iResourceGroupNo);			++iter;
		(*iter).Pop(kNewElement.iUpgradeCount);				++iter;
		(*iter).Pop(kNewElement.iUpgradeTime);				++iter;
		(*iter).Pop(kNewElement.iBasicExpertness);			++iter;
		(*iter).Pop(kNewElement.iGatherType);				++iter;
		(*iter).Pop(kNewElement.iMachine_UseDuration);		++iter;
		(*iter).Pop(kNewElement.iResultProbabilityNo);		++iter;
		(*iter).Pop(kNewElement.iResourceProbabilityUp);	++iter;
		(*iter).Pop(kNewElement.iErrorStateTimeAbsolute);	++iter;
		(*iter).Pop(kNewElement.iErrorStateMaxProbability);	++iter;
		(*iter).Pop(kNewElement.iErrorStateProbability);	++iter;
		(*iter).Pop(kNewElement.iNeedSaveIdx);				++iter;
		(*iter).Pop(kNewElement.iResourceType);				++iter;
		(*iter).Pop(kNewElement.iExpertnessUpVolume);		++iter;
		
		auto kRet = kContMap.insert( std::make_pair(kNewElement.iItemNo, kNewElement) );
		if( !kRet.second )
		{
			DBCacheUtil::AddErrorMsg( BM::vstring() << __FL__ << L"Duplicate ItemNo["<<kNewElement.iItemNo<<"] in [TB_DefJobSkill_ItemUpgrade]");
		}
	}

	//����üũ
	if(!kContMap.empty())
	{
		CONT_DEF_JOBSKILL_PROBABILITY_BAG const* pkDefJSProb = NULL;
		g_kTblDataMgr.GetContDef(pkDefJSProb);
		if( pkDefJSProb )
		{
			for(CONT_DEF_JOBSKILL_ITEM_UPGRADE::const_iterator c_it=kContMap.begin(); c_it!=kContMap.end(); ++c_it)
			{
				if( (*c_it).second.iResultProbabilityNo )
				{
					CONT_DEF_JOBSKILL_PROBABILITY_BAG::const_iterator prob_it = pkDefJSProb->find( (*c_it).second.iResultProbabilityNo );
					if(prob_it==pkDefJSProb->end())
					{
						AddErrorMsg( BM::vstring() << __FL__ << L"Can't find ItemNo<" << (*c_it).second.iItemNo << L"> ProbabilityNo<" << (*c_it).second.iResultProbabilityNo << L">" );
					}
				}
			}
		}
	}

	g_kTblDataMgr.SetContDef(kContMap);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_JOBSKILL_SAVEIDX(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}
	
	CONT_DEF_JOBSKILL_SAVEIDX kContMap;
	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != iter )
	{
		CONT_DEF_JOBSKILL_SAVEIDX::mapped_type kNewElement;
		(*iter).Pop(kNewElement.iSaveIdx);						++iter;
		(*iter).Pop(kNewElement.iRecipeItemNo);					++iter;
		(*iter).Pop(kNewElement.iResourceGroupNo);				++iter;
		(*iter).Pop(kNewElement.iNeedSkillNo01);				++iter;
		(*iter).Pop(kNewElement.iNeedSkillExpertness01);		++iter;
		(*iter).Pop(kNewElement.iNeedSkillNo02);				++iter;
		(*iter).Pop(kNewElement.iNeedSkillExpertness02);		++iter;
		(*iter).Pop(kNewElement.iNeedSkillNo03);				++iter;
		(*iter).Pop(kNewElement.iNeedSkillExpertness03);		++iter;
		(*iter).Pop(kNewElement.iRecipePrice);					++iter;
		(*iter).Pop(kNewElement.iBookItemNo);					++iter;
		(*iter).Pop(kNewElement.iAlreadyLearn);					++iter;
		(*iter).Pop(kNewElement.iShowUser);						++iter;

		auto kRet = kContMap.insert( std::make_pair(kNewElement.iSaveIdx, kNewElement) );
		if( !kRet.second )
		{
			DBCacheUtil::AddErrorMsg( BM::vstring() << __FL__ << L"Duplicate SaveIdx["<< kNewElement.iSaveIdx <<"] in [TB_DefJobSkill_SaveIdx]");
		}
	}

	g_kTblDataMgr.SetContDef(kContMap);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_LOAD_DEF_JOBSKILL_LOCATIONITEM(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_JOBSKILL_LOCATIONITEM map;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_DEF_JOBSKILL_LOCATIONITEM::key_type		kKey;
		CONT_DEF_JOBSKILL_LOCATIONITEM::mapped_type		kValue;

		result_iter->Pop( kKey );							++result_iter;
		result_iter->Pop( kValue.iGatherType );				++result_iter;
		result_iter->Pop( kValue.iSpot_Probability );		++result_iter;
		result_iter->Pop( kValue.iSpot_TotalProbability );	++result_iter;
		result_iter->Pop( kValue.iSpot_Cycle_Min );			++result_iter;
		result_iter->Pop( kValue.iSpot_Cycle_Max );			++result_iter;
		result_iter->Pop( kValue.iSpot_Dration_Min );		++result_iter;
		result_iter->Pop( kValue.iSpot_Dration_Max );		++result_iter;
		for(int iCount = 0; iCount < COUNT_JOBSKILL_RESULT_PROBABILITY_NO; iCount++)
		{
			result_iter->Pop( kValue.iResultProbability_No[iCount] );	++result_iter;
		}
		result_iter->Pop( kValue.iBase_Expertness );	++result_iter;		
		result_iter->Pop( kValue.i01Need_SaveIdx );		++result_iter;
		result_iter->Pop( kValue.i02Need_SaveIdx );		++result_iter;
		result_iter->Pop( kValue.i03Need_SaveIdx );		++result_iter;
		result_iter->Pop( kValue.i01Need_Skill_No );		++result_iter;
		result_iter->Pop( kValue.i01Need_Expertness );		++result_iter;
		result_iter->Pop( kValue.i02Need_Skill_No );		++result_iter;
		result_iter->Pop( kValue.i02Need_Expertness );		++result_iter;
		result_iter->Pop( kValue.i03Need_Skill_No );		++result_iter;
		result_iter->Pop( kValue.i03Need_Expertness );		++result_iter;
		result_iter->Pop( kValue.iProbability_UpRate );	++result_iter;

		auto kRet = map.insert( std::make_pair(kKey, kValue) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG( false , BM::LOG_LV0, __FL__ << _T("Data Error!!!") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed DefJobSkill_LocationItem"));
			return false;
		}
	}

	//����üũ
	if(!map.empty())
	{
		CONT_DEF_JOBSKILL_PROBABILITY_BAG const* pkDefJSProb = NULL;
		g_kTblDataMgr.GetContDef(pkDefJSProb);
		if( pkDefJSProb )
		{
			for(CONT_DEF_JOBSKILL_LOCATIONITEM::const_iterator c_it=map.begin(); c_it!=map.end(); ++c_it)
			{
				for(int i=0; i<COUNT_JOBSKILL_RESULT_PROBABILITY_NO; ++i)
				{
					if( (*c_it).second.iResultProbability_No[i] )
					{
						CONT_DEF_JOBSKILL_PROBABILITY_BAG::const_iterator prob_it = pkDefJSProb->find( (*c_it).second.iResultProbability_No[i] );
						if(prob_it==pkDefJSProb->end())
						{
							AddErrorMsg( BM::vstring() << __FL__ << L"Can't find JobGrade<" << (*c_it).first << L"> ProbabilityNo[" << i << L"]<" << (*c_it).second.iResultProbability_No[i] << L">" );
						}
					}
				}
			}
		}
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_LOAD_DEF_JOBSKILL_SKILL(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_JOBSKILL_SKILL map;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_DEF_JOBSKILL_SKILL::key_type			kKey;
		CONT_DEF_JOBSKILL_SKILL::mapped_type		kValue;
		int iTempValue = 0;

		result_iter->Pop( kKey );										++result_iter;
		result_iter->Pop( kValue.iGatherType );							++result_iter;
		result_iter->Pop( kValue.eJobSkill_Type );						++result_iter;
		result_iter->Pop( kValue.i01NeedParent_JobSkill_No );			++result_iter;
		result_iter->Pop( kValue.i01NeedParent_JobSkill_Expertness );	++result_iter;
		result_iter->Pop( kValue.i02NeedParent_JobSkill_No );			++result_iter;
		result_iter->Pop( kValue.i02NeedParent_JobSkill_Expertness );	++result_iter;
		result_iter->Pop( kValue.i03NeedParent_JobSkill_No );			++result_iter;
		result_iter->Pop( kValue.i03NeedParent_JobSkill_Expertness );	++result_iter;

		auto kRet = map.insert( std::make_pair(kKey, kValue) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG( false , BM::LOG_LV0, __FL__ << _T("Data Error!!!") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed DefJobSkill_Skill"));
			return false;
		}
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_LOAD_DEF_JOBSKILL_SKILLEXPERTNESS(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_JOBSKILL_SKILLEXPERTNESS kContList;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type		kValue;

		result_iter->Pop( kValue.iSkillNo );							++result_iter;
		result_iter->Pop( kValue.iSkill_Expertness_Min );				++result_iter;
		result_iter->Pop( kValue.iSkill_Expertness_Max );				++result_iter;
		result_iter->Pop( kValue.iExpertness_Gain_Min );				++result_iter;
		result_iter->Pop( kValue.iExpertness_Gain_Max );				++result_iter;
		result_iter->Pop( kValue.iExpertness_Gain_Probability );		++result_iter;
		result_iter->Pop( kValue.iExpertness_Gain_TotalProbability );	++result_iter;
		result_iter->Pop( kValue.iUse_Duration );						++result_iter;
		result_iter->Pop( kValue.iUse_Duration_Probability );			++result_iter;
		result_iter->Pop( kValue.iUse_Duration_TotalProbability );		++result_iter;
		result_iter->Pop( kValue.iBasic_Turn_Time );					++result_iter;
		result_iter->Pop( kValue.iMax_Exhaustion );						++result_iter;
		result_iter->Pop( kValue.iUse_Exhaustion );						++result_iter;
		result_iter->Pop( kValue.iMax_Machine );						++result_iter;
		result_iter->Pop( kValue.iNeed_CharLevel );						++result_iter;
		result_iter->Pop( kValue.iSkill_Probability );					++result_iter;

		if( kContList.end() != std::find(kContList.begin(), kContList.end(), kValue) )
		{
			DBCacheUtil::AddErrorMsg( BM::vstring() << __FL__ << _T("Warning!! Insert Failed DefJobSkill_SkillExpertness[SkillNo:"<<kValue.iSkillNo<<L", Min:"<<kValue.iSkill_Expertness_Min<<L",Max:"<<kValue.iExpertness_Gain_Max<<L"]") );
			return false;
		}
		else
		{
			kContList.push_back( kValue );
			kContList.sort();
		}
	}

	if(!kContList.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kContList);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_LOAD_DEF_JOBSKILL_TOOL(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_JOBSKILL_TOOL map;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_DEF_JOBSKILL_TOOL::key_type			kKey;
		CONT_DEF_JOBSKILL_TOOL::mapped_type		kValue;

		result_iter->Pop( kKey );										++result_iter;
		result_iter->Pop( kValue.iToolType );							++result_iter;
		result_iter->Pop( kValue.iGatherType );							++result_iter;
		result_iter->Pop( kValue.i01Need_Skill_No );					++result_iter;
		result_iter->Pop( kValue.i01Need_Skill_Expertness );			++result_iter;
		result_iter->Pop( kValue.i02Need_Skill_No );					++result_iter;
		result_iter->Pop( kValue.i02Need_Skill_Expertness );			++result_iter;
		result_iter->Pop( kValue.i03Need_Skill_No );					++result_iter;
		result_iter->Pop( kValue.i03Need_Skill_Expertness );			++result_iter;
		result_iter->Pop( kValue.iGetCount );							++result_iter;
		result_iter->Pop( kValue.iGetCountMax );						++result_iter;
		result_iter->Pop( kValue.iOption_TurnTime );					++result_iter;
		result_iter->Pop( kValue.iOption_CoolTime );					++result_iter;
		result_iter->Pop( kValue.iOption_User_Exhaustion );				++result_iter;
		result_iter->Pop( kValue.iResult_No_Min );						++result_iter;
		result_iter->Pop( kValue.iResult_No_Max );						++result_iter;

		auto kRet = map.insert( std::make_pair( kKey, kValue ) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG( false , BM::LOG_LV0, __FL__ << _T("Data Error!!!") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed DefJobSkill_Tool"));
			return false;
		}
	}

 	if(!map.empty())
 	{
 		g_kCoreCenter.ClearQueryResult(rkResult);
 		g_kTblDataMgr.SetContDef(map);
 		return true;
 	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_LOAD_DEF_JOBSKILL_SHOP(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
	&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_JOBSKILL_SHOP kContMap;

	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_JOBSKILL_SHOP::key_type, CONT_DEF_JOBSKILL_SHOP::mapped_type, CONT_DEF_JOBSKILL_SHOP > NationCodeUtil( L"Duplicate iIDX[" DBCACHE_KEY_PRIFIX L"]" );

	while( rkVec.end() != result_iter )
	{
		CONT_DEF_JOBSKILL_SHOP::mapped_type kNewElement;
		result_iter->Pop( NationCodeStr );				++result_iter;
		result_iter->Pop( kNewElement.iIDX );			++result_iter;
		result_iter->Pop( kNewElement.iCategory );		++result_iter;
		result_iter->Pop( kNewElement.iCP );			++result_iter;
		result_iter->Pop( kNewElement.iItemNo );		++result_iter;
		result_iter->Pop( kNewElement.iPrice );			++result_iter;
		result_iter->Pop( kNewElement.kShopGuid );		++result_iter;
		result_iter->Pop( kNewElement.bTimeType );		++result_iter;
		result_iter->Pop( kNewElement.iUseTime );		++result_iter;

		if( 0 < kNewElement.iIDX )
		{
			NationCodeUtil.Add(NationCodeStr, kNewElement.iIDX, kNewElement, __FUNCTIONW__, __LINE__);
		}
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(NationCodeUtil.GetResult());
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_LOAD_DEF_JOBSKILL_RECIPE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}
	
	CONT_DEF_JOBSKILL_RECIPE kContMap;

	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEF_JOBSKILL_RECIPE::key_type, CONT_DEF_JOBSKILL_RECIPE::mapped_type, CONT_DEF_JOBSKILL_RECIPE > NationCodeUtil( L"Duplicate Recipe_ItemNo[" DBCACHE_KEY_PRIFIX L"]" );

	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != iter )
	{
		CONT_DEF_JOBSKILL_RECIPE::mapped_type kNewElement;
		(*iter).Pop(NationCodeStr);							++iter;
		(*iter).Pop(kNewElement.iItemNo);					++iter;
		(*iter).Pop(kNewElement.iOrderID);					++iter;
		(*iter).Pop(kNewElement.iNeedSkillNo);				++iter;
		(*iter).Pop(kNewElement.iNeedSkillExpertness);		++iter;
		for(int i=0; i<MAX_JS3_RECIPE_RES; ++i)
		{
			(*iter).Pop(kNewElement.kResource[i].iGroupNo);		++iter;
			(*iter).Pop(kNewElement.kResource[i].iGrade);		++iter;
			(*iter).Pop(kNewElement.kResource[i].iCount);		++iter;
		}
		(*iter).Pop(kNewElement.iNeedMoney);				++iter;
		(*iter).Pop(kNewElement.kProbability.iNo);			++iter;
		(*iter).Pop(kNewElement.kProbability.iRate);		++iter;
		(*iter).Pop(kNewElement.kProbability.iRateItem);	++iter;
		(*iter).Pop(kNewElement.iNeedProductPoint);			++iter;
		(*iter).Pop(kNewElement.iExpertnessGain);			++iter;

		NationCodeUtil.Add(NationCodeStr, kNewElement.iItemNo, kNewElement, __FUNCTIONW__, __LINE__);
	}

	//����üũ
	if( !NationCodeUtil.IsEmpty() )
	{
		CONT_DEF_JOBSKILL_PROBABILITY_BAG const* pkDefJSProb = NULL;
		g_kTblDataMgr.GetContDef(pkDefJSProb);
		if( pkDefJSProb )
		{
			CONT_DEF_JOBSKILL_RECIPE const& rkDefRecipe = NationCodeUtil.GetResult();
			for(CONT_DEF_JOBSKILL_RECIPE::const_iterator c_it=rkDefRecipe.begin(); c_it!=rkDefRecipe.end(); ++c_it)
			{
				if( (*c_it).second.kProbability.iNo )
				{
					CONT_DEF_JOBSKILL_PROBABILITY_BAG::const_iterator prob_it = pkDefJSProb->find( (*c_it).second.kProbability.iNo );
					if(prob_it==pkDefJSProb->end())
					{
						AddErrorMsg( BM::vstring() << __FL__ << L"Can't find ItemNo<" << (*c_it).second.iItemNo << L"> ProbabilityNo<" << (*c_it).second.kProbability.iNo << L">" );
					}
				}
			}
		}
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(NationCodeUtil.GetResult());
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_LOAD_DEF_SOCKET_ITEM(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_SOCKET_ITEM map;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_DEF_SOCKET_ITEM::key_type			kKey;
		CONT_DEF_SOCKET_ITEM::mapped_type		kValue;

		result_iter->Pop( kKey.iEquipPos );					++result_iter;
		result_iter->Pop( kKey.iLevelLimit );				++result_iter;
		result_iter->Pop( kKey.iSocket_Order );				++result_iter;
		result_iter->Pop( kValue.iSuccessRate );			++result_iter;
		result_iter->Pop( kValue.iSoulItemCount );			++result_iter;
		result_iter->Pop( kValue.iNeedMoney );				++result_iter;
		result_iter->Pop( kValue.iSocketItemNo );			++result_iter;
		result_iter->Pop( kValue.iRestorationItemNo );		++result_iter;
		result_iter->Pop( kValue.iResetItemNo );			++result_iter;		

		auto kRet = map.insert( std::make_pair(kKey, kValue) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG( false , BM::LOG_LV0, __FL__ << _T("Data Error!!!") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Insert Failed DefSocketItem"));
			return false;
		}
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_LOAD_DEF_JOBSKILL_ITEMSOULEXTRACT(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_JOBSKILL_SOUL_EXTRACT kContList;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_DEF_JOBSKILL_SOUL_EXTRACT::value_type		kValue;

		result_iter->Pop( kValue.iEquipPos );							++result_iter;
		result_iter->Pop( kValue.iLevelLimit );							++result_iter;
		result_iter->Pop( kValue.iSuccessRate );						++result_iter;
		result_iter->Pop( kValue.iExtractItemNo );						++result_iter;
		result_iter->Pop( kValue.iResultItemNo );						++result_iter;

		if( kContList.end() != std::find(kContList.begin(), kContList.end(), kValue) )
		{
			DBCacheUtil::AddErrorMsg( BM::vstring() << __FL__ << _T("Warning!! Insert Failed DefItemSoulExtract") );
			return false;
		}
		else
		{
			kContList.push_back( kValue );
			kContList.sort();
		}
	}

	if(!kContList.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kContList);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
bool PgDBCache::Q_DQT_LOAD_DEF_JOBSKILL_ITEMSOULTRANSITION(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();

	CONT_DEF_JOBSKILL_SOUL_TRANSITION kContList;
	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

	if(count_iter == rkResult.vecResultCount.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	std::wstring NationCodeStr;
	DBCacheUtil::PgNationCodeHelper< TBL_TRIPLE_KEY_INT, CONT_DEF_JOBSKILL_SOUL_TRANSITION::value_type, CONT_DEF_JOBSKILL_SOUL_TRANSITION, DBCacheUtil::ConvertTriple, DBCacheUtil::AddError, DBCacheUtil::VectorContainerInsert > NationCodeUtil( L"Duplicate ITEMSOULTRANSITION Key[" DBCACHE_KEY_PRIFIX L"]" );

	int const iCount = (*count_iter); ++count_iter;

	for(int i = 0;i < iCount;++i)
	{
		CONT_DEF_JOBSKILL_SOUL_TRANSITION::value_type		kValue;

		result_iter->Pop( NationCodeStr );						++result_iter;
		result_iter->Pop( kValue.iEquipType );					++result_iter;
		result_iter->Pop( kValue.iLevelLimit );					++result_iter;
		result_iter->Pop( kValue.iRairity_Grade );				++result_iter;
		result_iter->Pop( kValue.iSuccessRate );				++result_iter;
		result_iter->Pop( kValue.iSoulItemCount );				++result_iter;
		result_iter->Pop( kValue.iNeedMoney );					++result_iter;
		result_iter->Pop( kValue.iInsuranceitemNo );			++result_iter;
		result_iter->Pop( kValue.iProbabilityUpItemNo );		++result_iter;
		result_iter->Pop( kValue.iProbabilityUpRate );			++result_iter;
		result_iter->Pop( kValue.iProbabilityUpItemCount );		++result_iter;

		NationCodeUtil.Add(NationCodeStr, TBL_TRIPLE_KEY_INT(kValue.iEquipType, kValue.iLevelLimit, 
			kValue.iRairity_Grade), kValue, __FUNCTIONW__, __LINE__);

		/*if( kContList.end() != std::find(kContList.begin(), kContList.end(), kValue) )
		{
			DBCacheUtil::AddErrorMsg( BM::vstring() << __FL__ << _T("Warning!! Insert Failed DefItemSoulTransition") );
			return false;
		}
		else
		{
			kContList.push_back( kValue );
			kContList.sort();
		}*/
	}

	if( !NationCodeUtil.IsEmpty() )
	{
		int bSuccess = true;
		CONT_DEF_JOBSKILL_SOUL_TRANSITION cont = NationCodeUtil.GetResult();

		cont.sort();

		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(cont);
		return true;
	}	

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_LOAD_DEF_EXPEDITION_NPC(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	CONT_PROBABILITY::value_type kNewProbability;
	CONT_DEF_EXPEDITION_NPC::mapped_type kNewElement;

	CONT_DEF_EXPEDITION_NPC kContMap;
	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != iter )
	{
		(*iter).Pop(kNewElement.kNpcGuid);					++iter;
		(*iter).Pop(kNewElement.iMapNo);					++iter;
		(*iter).Pop(kNewElement.iMinPlayerCnt);				++iter;
		(*iter).Pop(kNewElement.iMaxPlayerCnt);				++iter;
		(*iter).Pop(kNewElement.iNeedItemNo);				++iter;
		(*iter).Pop(kNewElement.iNeedItemCnt);				++iter;
		(*iter).Pop(kNewElement.iDungeonStringNo);			++iter;
		(*iter).Pop(kNewElement.kStrImgPath);				++iter;
		(*iter).Pop(kNewElement.iDungeonInfoStringNo);		++iter;
		
		auto kRet = kContMap.insert( std::make_pair(kNewElement.kNpcGuid, kNewElement) );
		if( !kRet.second )
		{
			DBCacheUtil::AddErrorMsg( BM::vstring() << __FL__ << L"Duplicate Expedition_NPC_Guid["<<kNewElement.kNpcGuid.str().c_str()<<"] in [TB_DefExpeditionNpc]");
		}
	}

	g_kTblDataMgr.SetContDef(kContMap);
	g_kCoreCenter.ClearQueryResult(rkResult);
	return true;
}

bool PgDBCache::Q_DQT_SELECT_TOP1_EMPORIA_FOR_CHECK(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	BM::GUID guid;
	CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != iter )
	{
		(*iter).Pop(guid);					++iter;
	}

	INFO_LOG(BM::LOG_LV5, __FL__ << _T(" guid=") << guid.str());
	
	return true;
}


static bool Q_DQT_PREMIUM_SERVICE( const char* pkTBPath )
{
	CONT_DEF_PREMIUM_SERVICE map;

    /*Code generated by codegen version: 0.0.1*/
    BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefPremiumService.csv");
    const size_t f_ServiceNo = 0, f_Title = 1, f_ServiceType = 2, f_Grade = 3, f_UseDate = 5, f_Article01 = 6;
    while( csv.next_row() /*skip header*/ )
	{
		CONT_DEF_PREMIUM_SERVICE::mapped_type element;
		element.iServiceNo = csv.col_int(f_ServiceNo);
		element.kTitle = UNI(csv.col(f_Title));
		element.byServiceType = csv.col_int(f_ServiceType);
		element.byGrade = csv.col_int(f_Grade);
		element.wUseDate = csv.col_int(f_UseDate);

		for (int i=0; i<MAX_PREMIUM_ARTICLE; i++)
			element.iArticle[i] = csv.col_int(f_Article01 + i);

		map.insert(std::make_pair(element.iServiceNo, element));
	}

	if(!map.empty())
	{
		g_kTblDataMgr.SetContDef(map);
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

static bool Q_DQT_PREMIUM_ARTICLE( const char* pkTBPath )
{
	CONT_DEF_PREMIUM_ARTICLE map;
    /*Code generated by codegen version: 0.0.1*/
    BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    csvdb::load(buff, csv, BM::vstring(pkTBPath) << "TB_DefPremiumArticle.csv");
    const size_t f_ArticleNo = 0, f_Type = 1, f_Lv = 2, f_Abil01 = 4, f_Value01 = 5;
    while( csv.next_row() /*skip header*/ )
	{
		CONT_DEF_PREMIUM_ARTICLE::mapped_type element;
		element.iServiceTypeNo = csv.col_int(f_ArticleNo);
		element.wType = csv.col_int(f_Type);
		element.wLv = csv.col_int(f_Lv);
		for (int i=0; i<MAX_PREMIUM_ARTICLE_ABIL; i++)
		{
			element.iAbil[i] = csv.col_int(f_Abil01 + (i << 1)); // Type0N
			element.iValue[i] = csv.col_int(f_Value01 + (i << 1)); // Value0N
		}

		map.insert(std::make_pair(element.iServiceTypeNo, element));
	}

	if(!map.empty())
	{
		g_kTblDataMgr.SetContDef(map);
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_UPDATE_PREMIUM_SERVICE( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}
	
	return true;
}

bool PgDBCache::Q_DQT_SELECT_CASHITEM_FOR_SOUL_LEVEL_DEC(CEL::DB_RESULT &rkResult)
{
	PgBase_Item kitem;
	SEnchantInfo keinfo;
	BM::GUID kGuid;
	int itemno;
	__int64 e1;
	__int64 e2;
	__int64 e3;
	__int64 e4;
	CONT_ITEM_CREATE_ORDER kcont;
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	int idx = 0;

	SYSTEMTIME t;
	::GetLocalTime(&t);

	char szFileName[MAX_PATH] = {0,};
	::sprintf_s(szFileName, MAX_PATH, "../../LogFiles/CautionLog/R%dC%d_CONTENTS%04d_SOUL_LEVEL_DEC_%04d%02d%02d_%02d%02d%02d.txt", g_kProcessCfg.RealmNo(), g_kProcessCfg.ChannelNo(), g_kProcessCfg.ServerNo(),
		t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond );

	std::ofstream outfile (szFileName);

	while( rkResult.vecArray.end() != itor )
	{
		kitem.Clear();
		keinfo.Clear();
		(*itor).Pop(kGuid); ++itor;
		(*itor).Pop(itemno); ++itor;
		(*itor).Pop(e1); ++itor;
		(*itor).Pop(e2); ++itor;
		(*itor).Pop(e3); ++itor;
		(*itor).Pop(e4); ++itor;

		keinfo.Field_1(e1);
		keinfo.Field_2(e2);
		keinfo.Field_3(e3);
		keinfo.Field_4(e4);

		CItemDef const* pkDefItem = kItemDefMgr.GetDef(itemno);
		if (pkDefItem)
		{
			if((EQUIP_LIMIT_WEAPON & pkDefItem->GetAbil(AT_EQUIP_LIMIT)) &&
				(true==pkDefItem->IsType(ITEM_TYPE_AIDS)))
			{
				kitem.Guid(kGuid);
				kitem.ItemNo(itemno);

				bool bchange = false;

				if(2 < keinfo.BasicLv1())
				{
					keinfo.BasicLv1(2i64);
					bchange = true;
				}
				if(2 < keinfo.BasicLv2())
				{
					keinfo.BasicLv2(2i64);
					bchange = true;
				}
				if(2 < keinfo.BasicLv3())
				{
					keinfo.BasicLv3(2i64);
					bchange = true;
				}
				if(2 < keinfo.BasicLv4())
				{
					keinfo.BasicLv4(2i64);
					bchange = true;
				}

				if(true==bchange)
				{
					BM::vstring vqry(L"Update dr2_user.dbo.TB_UserItem_Enchant set Enchant_01 = ");
					vqry+=keinfo.Field_1();
					vqry+=L", Enchant_02 = ";
					vqry+=keinfo.Field_2();
					vqry+=" where itemguid = '";
					vqry+=kitem.Guid().str();
					vqry+=L"'";

					INFO_LOG(BM::LOG_LV4, __FL__<<idx<<L" "<<vqry);

					outfile<<(std::string const)vqry<<std::endl;

					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_NO_OP, (std::wstring const&)vqry);
					g_kCoreCenter.PushQuery( kQuery );
					++idx;
				}
			}
		}
	}
	INFO_LOG(BM::LOG_LV3, __FL__<<L"Totoal Count : "<<idx);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_EVENTGROUP(CEL::DB_RESULT & Result)
{
	if( CEL::DR_SUCCESS != Result.eRet && CEL::DR_NO_RESULT != Result.eRet )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << Result.eRet << _T(", Query=") << Result.Command());
	}

	CONT_DEF_EVENT_GROUP ContEventGroup;
	SEventGroup NewElement;
	BM::DBTIMESTAMP_EX TimeStamp;

	CEL::DB_DATA_ARRAY::iterator iter = Result.vecArray.begin();
	while( Result.vecArray.end() != iter )
	{
		(*iter).Pop(NewElement.EventGroupNo);						++iter;
		(*iter).Pop(NewElement.MonsterGroupNo);						++iter;
		(*iter).Pop(TimeStamp);	NewElement.StartTime = TimeStamp;	++iter;
		(*iter).Pop(TimeStamp); NewElement.EndTime = TimeStamp;		++iter;
		(*iter).Pop(NewElement.RegenPeriod);						++iter;
		(*iter).Pop(NewElement.DelayGenTime);						++iter;
		(*iter).Pop(NewElement.Message);							++iter;

		ContEventGroup.insert( std::make_pair( NewElement.EventGroupNo, NewElement) );
	}

	g_kCoreCenter.ClearQueryResult(Result);
	g_kTblDataMgr.SetContDef(ContEventGroup);

	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_EVENTMONSTERGROUP(CEL::DB_RESULT & Result)
{
	if( CEL::DR_SUCCESS != Result.eRet && CEL::DR_NO_RESULT != Result.eRet )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << Result.eRet << _T(", Query=") << Result.Command());
	}

	CONT_DEF_EVENT_MONSTER_GROUP ContMonsterGroup;
	CONT_EVENTMONSTERGROUP ContMonsterInfo;
	SEventMonsterGroup NewElement;

	int prevKey = 0;
	double pos = 0.0;
	SEvnetMonsterInfo NewData;
	
	CEL::DB_DATA_ARRAY::iterator iter = Result.vecArray.begin();
	while( Result.vecArray.end() != iter )
	{
		(*iter).Pop(NewElement.MonsterGroupNo);										++iter;
		(*iter).Pop(NewElement.MapNo);												++iter;
		(*iter).Pop(NewData.MonsterNo);												++iter;
		(*iter).Pop(NewData.GenDelay);												++iter;
		(*iter).Pop(NewData.Order);													++iter;
		(*iter).Pop(NewData.MonsterLevel);											++iter;
		(*iter).Pop(NewData.DifficultyHP);											++iter;
		(*iter).Pop(NewData.DifficultyDamage);										++iter;
		(*iter).Pop(NewData.DifficultyDefence);										++iter;
		(*iter).Pop(NewData.DifficultyExp);											++iter;
		(*iter).Pop(NewData.DifficultyAttackSpeed);									++iter;
		(*iter).Pop(NewData.DifficultyEvade);										++iter;
		(*iter).Pop(NewData.DifficultyHitrate);										++iter;
		(*iter).Pop(pos);	NewData.GenPos.x = static_cast<float>(pos);				++iter;
		(*iter).Pop(pos);	NewData.GenPos.y = static_cast<float>(pos);				++iter;
		(*iter).Pop(pos);	NewData.GenPos.z = static_cast<float>(pos);				++iter;
		(*iter).Pop(NewData.RewardItemGroup);										++iter;
		(*iter).Pop(NewData.RewardItemCount);										++iter;

		if( prevKey != NewElement.MonsterGroupNo )
		{
			ContMonsterInfo.clear();
			prevKey = NewElement.MonsterGroupNo;
		}

		auto InfoRet = ContMonsterInfo.insert( std::make_pair( NewElement.MapNo, NewElement ) );
		InfoRet.first->second.MonsterInfo.push_back(NewData);

		auto GroupRet = ContMonsterGroup.insert( std::make_pair(NewElement.MonsterGroupNo, ContMonsterInfo) );
		if( !GroupRet.second )
		{
			CONT_EVENTMONSTERGROUP::iterator find_iter = GroupRet.first->second.find( NewElement.MapNo );
			if( find_iter != GroupRet.first->second.end() )
			{
				find_iter->second.MonsterInfo.push_back(NewData);
			}
			else
			{
				auto insterRet = GroupRet.first->second.insert( std::make_pair( NewElement.MapNo, NewElement ) );
				insterRet.first->second.MonsterInfo.push_back(NewData);
			}
		}
	}
	
	g_kCoreCenter.ClearQueryResult(Result);
	g_kTblDataMgr.SetContDef(ContMonsterGroup);
	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_EVENTREWARDITEMGROUP(CEL::DB_RESULT & Result)
{
	if( CEL::DR_SUCCESS != Result.eRet && CEL::DR_NO_RESULT != Result.eRet )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << Result.eRet << _T(", Query=") << Result.Command());
	}

	CONT_DEF_EVENT_REWARD_ITEM_GROUP ContRewardItemGroup;
	SEventRewardItemGroup NewElement;

	CEL::DB_DATA_ARRAY::iterator iter = Result.vecArray.begin();
	while( Result.vecArray.end() != iter )
	{
		(*iter).Pop(NewElement.RewardItemGroup);				++iter;
		(*iter).Pop(NewElement.ItemNo);							++iter;
		(*iter).Pop(NewElement.Rate);							++iter;
		(*iter).Pop(NewElement.Count);							++iter;

		VEC_EVENTREWARDITEMGROUP Vec_RewardItemGroup;
		auto GroupRet = ContRewardItemGroup.insert( std::make_pair( NewElement.RewardItemGroup, Vec_RewardItemGroup) );
		GroupRet.first->second.push_back( NewElement );
	}

	g_kCoreCenter.ClearQueryResult(Result);
	g_kTblDataMgr.SetContDef(ContRewardItemGroup);

	return true;
}

bool PgDBCache::Q_DQT_LOAD_DEF_EVENTSCHEDULE( CEL::DB_RESULT & Result )
{
	if( CEL::DR_SUCCESS != Result.eRet && CEL::DR_NO_RESULT != Result.eRet )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << Result.eRet << _T(", Query=") << Result.Command());
	}
	
	CONT_DEF_EVENT_SCHEDULE EventSchedule;
	std::wstring temp(_T(""));

	bool bSuccess = true;
	CEL::DB_DATA_ARRAY::iterator iter = Result.vecArray.begin();
	while( Result.vecArray.end() != iter )
	{
		SEventScheduleData NewElement;
		(*iter).Pop(NewElement.MenuText);					++iter;
		(*iter).Pop(NewElement.EventType);					++iter;
		(*iter).Pop(NewElement.EventNo);					++iter;
		(*iter).Pop(NewElement.EventGround);				++iter;
		(*iter).Pop(NewElement.LevelMin);					++iter;
		(*iter).Pop(NewElement.LevelMax);					++iter;
		(*iter).Pop(NewElement.ClassLimitHuman);			++iter;
		(*iter).Pop(NewElement.ClassLimitDragon);			++iter;
		(*iter).Pop(NewElement.PartyMemeberMin);			++iter;
		(*iter).Pop(NewElement.UserCountMax);				++iter;
		(*iter).Pop(NewElement.PKOption);					++iter;
		(*iter).Pop(NewElement.NeedQuest);					++iter;
		(*iter).Pop(NewElement.NeedQuestState);				++iter;
		(*iter).Pop(NewElement.NeedItemNo);					++iter;
		(*iter).Pop(NewElement.NeedItemCount);				++iter;
		(*iter).Pop(NewElement.UnUsableConsumeItem);		++iter;
		(*iter).Pop(NewElement.UnUsableReviveItem);			++iter;
		(*iter).Pop(NewElement.StartDate);					++iter;
		(*iter).Pop(NewElement.StartTime);					++iter;
		(*iter).Pop(NewElement.EndDate);					++iter;
		(*iter).Pop(NewElement.EventPeriod);				++iter;
		(*iter).Pop(NewElement.EventCount);					++iter;
		(*iter).Pop(temp);									++iter;
		PgStringUtil::BreakSep(temp, NewElement.DayofWeek, _T("/"));

		NewElement.ClassLimitHuman = HumanAddFiveJobClassLimit(NewElement.ClassLimitHuman);
		if( 0 == ::_tcscmp( NewElement.EventType.c_str(), _T("BossBattle")) )
		{

			if( 1 == NewElement.EventCount )
			{
				CONT_DEF_EVENT_BOSSBATTLE const* pEventBossBattle = NULL;
				g_kTblDataMgr.GetContDef(pEventBossBattle);
				if( pEventBossBattle )
				{
					CONT_DEF_EVENT_BOSSBATTLE::const_iterator boss_iter = pEventBossBattle->find(NewElement.EventNo);
					if( pEventBossBattle->end() != boss_iter )
					{
						NewElement.EventPeriod = boss_iter->second.LimitTime + 600; // ��
					}
					else
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Failed find BossBattle Event."));
						continue;
					}
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Failed find BossBattle EventTable."));
					continue;
				}
			}
			else
			{
				if( 0 == NewElement.EventPeriod )
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("When the EventCount is greater than 1, the EventPeriod can not be 0."));
					continue;
				}
			}
		}
		else if( 0 == ::_tcscmp( NewElement.EventType.c_str(), _T("Race")) )
		{

			if( 1 == NewElement.EventCount )
			{
				CONT_DEF_EVENT_RACE const* pEventRace = NULL;
				g_kTblDataMgr.GetContDef(pEventRace);
				if( pEventRace )
				{
					CONT_DEF_EVENT_RACE::const_iterator race_iter = pEventRace->find(NewElement.EventNo);
					if( pEventRace->end() != race_iter )
					{
						NewElement.EventPeriod = race_iter->second.LimitTime + 10; // ��
					}
					else
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Failed find Race Event."));
						continue;
					}
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Failed find Race EventTable."));
					continue;
				}
			}
			else
			{
				if( 0 == NewElement.EventPeriod )
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("When the EventCount is greater than 1, the EventPeriod can not be 0."));
					continue;
				}
			}
		}

		auto Ret = EventSchedule.insert( std::make_pair( NewElement.EventNo, NewElement) );
		if( !Ret.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Duplicate EventNo[") << NewElement.EventNo << _T("]"));
			bSuccess = false;
		}

		ASSERT_LOG(bSuccess, BM::LOG_LV4, __FL__<<L"Duplicate EventNo");
	}

	if( !EventSchedule.empty() )
	{
		g_kCoreCenter.ClearQueryResult(Result);
		g_kTblDataMgr.SetContDef(EventSchedule);
		return true;
	}
	
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_LOAD_DEF_EVENT_BOSSBATTLE( CEL::DB_RESULT & Result )
{
	if( CEL::DR_SUCCESS != Result.eRet && CEL::DR_NO_RESULT != Result.eRet )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << Result.eRet << _T(", Query=") << Result.Command());
	}

	CONT_DEF_EVENT_BOSSBATTLE BossBattle;
	SEventBossBattleData NewElement;

	bool bSuccess = true;
	CEL::DB_DATA_ARRAY::iterator iter = Result.vecArray.begin();
	while( Result.vecArray.end() != iter )
	{
		(*iter).Pop(NewElement.BossBattleEventNo);				++iter;
		(*iter).Pop(NewElement.StartEffect);					++iter;
		(*iter).Pop(NewElement.LimitTime);						++iter;
		(*iter).Pop(NewElement.MonsterRegenType);				++iter;
		(*iter).Pop(NewElement.MonsterGroupNo);					++iter;
		(*iter).Pop(NewElement.HarvestLumberEventItemNo);		++iter;
		(*iter).Pop(NewElement.HarvestGemEventItemNo);			++iter;
		(*iter).Pop(NewElement.HarvestHerbEventItemNo);			++iter;
		(*iter).Pop(NewElement.HarvestFishEventItemNo);			++iter;
		(*iter).Pop(NewElement.HarvestExpMin);					++iter;

		auto Ret = BossBattle.insert( std::make_pair(NewElement.BossBattleEventNo, NewElement) );
		if( !Ret.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Duplicate BossBattleEventNo[") << NewElement.BossBattleEventNo << _T("]"));
			bSuccess = false;
		}

		ASSERT_LOG(bSuccess, BM::LOG_LV4, __FL__<<L"Duplicate BossBattleEventNo");
	}

	if( !BossBattle.empty() )
	{
		g_kCoreCenter.ClearQueryResult(Result);
		g_kTblDataMgr.SetContDef(BossBattle);
		return true;
	}
	
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_LOAD_DEF_EVENT_RACE( CEL::DB_RESULT & Result )
{
	if( CEL::DR_SUCCESS != Result.eRet && CEL::DR_NO_RESULT != Result.eRet )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << Result.eRet << _T(", Query=") << Result.Command());
	}

	CONT_DEF_EVENT_RACE kContRaceEvent;
	SEventRaceData NewElement;

	bool bSuccess = true;
	CEL::DB_DATA_ARRAY::iterator iter = Result.vecArray.begin();
	while( Result.vecArray.end() != iter )
	{
		(*iter).Pop(NewElement.EventNo);						++iter;
		(*iter).Pop(NewElement.StartEffect);					++iter;
		(*iter).Pop(NewElement.LimitTime);						++iter;
		(*iter).Pop(NewElement.MoveSpeed);						++iter;
		BYTE byUseSkill = 0;
		(*iter).Pop(byUseSkill);								++iter;
		NewElement.UseSkill = byUseSkill != 0;
		(*iter).Pop(NewElement.RaceType);						++iter;
		(*iter).Pop(NewElement.TrackAround);					++iter;
		(*iter).Pop(NewElement.MpMax);							++iter;
		for(int i = 0; i < 5; i++)
		{
			(*iter).Pop(NewElement.RewardItemGroup[i]);			++iter;
			for(int j = 0; j < 4; j++)
			{
				(*iter).Pop(NewElement.RewardItemBagGroup[i][j]);		++iter;
			}
		}
/*
		for(int i = 0; i < 5; i++)
		{
			(*iter).Pop(NewElement.RewardItemGroup[i]);			++iter;
			(*iter).Pop(NewElement.RewardItemBagGroup[i]);		++iter;
		}
*/
		auto Ret = kContRaceEvent.insert( std::make_pair(NewElement.EventNo, NewElement) );
		if( !Ret.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Duplicate RaceEventNo[") << NewElement.EventNo << _T("]"));
			bSuccess = false;
		}

		ASSERT_LOG(bSuccess, BM::LOG_LV4, __FL__<<L"Duplicate RaceEventNo");
	}

	if( !kContRaceEvent.empty() )
	{
		g_kCoreCenter.ClearQueryResult(Result);
		g_kTblDataMgr.SetContDef(kContRaceEvent);
		return true;
	}
	
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_LOAD_QUEST_LEVEL_REWARD( CEL::DB_RESULT &rkResult )
{
	CONT_DEF_QUEST_LEVEL_REWARD map;

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEF_QUEST_LEVEL_REWARD::mapped_type element;

		(*itor).Pop( element.iLevel );		++itor;
		(*itor).Pop( element.iExp );		++itor;
		(*itor).Pop( element.iMoney );		++itor;

		map.insert(std::make_pair(element.iLevel, element));
	}

	if(!map.empty())
	{
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(map);
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


bool PgDBCache::Q_DQT_LOAD_JOBSKILL_EVENT_LOCATION(CEL::DB_RESULT &rkResult)
{
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	BM::DBTIMESTAMP_EX TimeStamp;
	std::wstring kNationCodeStr;
	DBCacheUtil::PgNationCodeHelper< TBL_KEY_INT, CONT_DEF_JOBSKILL_EVENT_LOCATION::value_type, CONT_DEF_JOBSKILL_EVENT_LOCATION, BM::vstring, DBCacheUtil::AddError, DBCacheUtil::VectorContainerInsert >
		kNationCodeUtil( L"Duplicate JobGrade[" DBCACHE_KEY_PRIFIX L"]" );
	int iIndex = 0;
	while( rkResult.vecArray.end() != itor )
	{
		CONT_DEF_JOBSKILL_EVENT_LOCATION::value_type element;
		(*itor).Pop( kNationCodeStr );								++itor;
		(*itor).Pop(TimeStamp);	element.kStartDate	= TimeStamp;	++itor;
		(*itor).Pop(TimeStamp); element.kEndDate	= TimeStamp;	++itor;
		(*itor).Pop( element.iJobGrade );							++itor;
		(*itor).Pop( element.iRewardItemGroup );					++itor;
		(*itor).Pop( element.iRate );								++itor;

		kNationCodeUtil.Add(kNationCodeStr, iIndex, element, __FUNCTIONW__, __LINE__);
		++iIndex;
	}
	if( !kNationCodeUtil.IsEmpty() )
	{
		CONT_DEF_JOBSKILL_EVENT_LOCATION kContItem = kNationCodeUtil.GetResult();
		g_kCoreCenter.ClearQueryResult(rkResult);
		g_kTblDataMgr.SetContDef(kContItem);
		return true;
	}	
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDBCache::Q_DQT_LOAD_DEF_CHARACTER_CREATE_SET(CEL::DB_RESULT &rkResult)
{
	CONT_DEF_CHARACTER_CREATE_SET kMap;
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while (rkResult.vecArray.end() != itor)
	{
		TBL_DEF_CHARACTER_CREATE_SET	kElement;

		(*itor).Pop(kElement.iSetNo);				++itor;
		(*itor).Pop(kElement.byClassNo);			++itor;
		(*itor).Pop(kElement.byGender);			++itor;
		(*itor).Pop(kElement.byInvType);			++itor;
		(*itor).Pop(kElement.byInvPos);			++itor;
		(*itor).Pop(kElement.iItemNo);			++itor;

		const bool bRet = kMap.insert(kElement).second;
		if (!bRet)
		{
			//TODO: fix that
			//VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Duplicate CharacterCreateSet[") << kElement.iSetNo << _T("]"));
			//ASSERT_LOG(0, BM::LOG_LV4, __FL__ << L"Duplicate CharacterCreateSet");
			//return false;
		}
	}

	if (kMap.empty())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"row count 0");
	}
	g_kTblDataMgr.SetContDef(kMap);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return true"));
	return true;
}

static HRESULT CALLBACK Q_DQT_LOAD_DEF_BATTLE_PASS_QUEST(CEL::DB_RESULT &rkResult)
{
	CONT_DEF_BATTLE_PASS_QUEST kMap;
	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while (rkResult.vecArray.end() != itor)
	{
		CONT_DEF_BATTLE_PASS_QUEST::mapped_type	kElement;

		(*itor).Pop(kElement.iBattlePassIdx);		++itor;
		(*itor).Pop(kElement.iGradeNo);				++itor;
		(*itor).Pop(kElement.iBattlePassDay);		++itor;
		(*itor).Pop(kElement.iBattlePassQuest);		++itor;

		CONT_DEF_BATTLE_PASS_QUEST::key_type kKey(kElement.iBattlePassIdx, kElement.iGradeNo, kElement.iBattlePassDay);
		if (!kMap.insert(std::make_pair(kKey, kElement)).second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Duplicate Battle pass quest[ Idx: ") << kElement.iBattlePassIdx << " Grade:" << kElement.iGradeNo <<  " DAY: " << kElement.iBattlePassDay << _T("]"));
			return E_FAIL;
		}
	}
	g_kTblDataMgr.SetContDef(kMap);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return true"));
	return S_OK;
}

#define DT_DEF_PATH "./Table/DR2_Def/"
#define DT_LOCAL_PATH "./Table/DR2_Local/"

#define QUERY(DBType, QueryType, Query) \
	{ \
		CEL::DB_QUERY kQuery( DBType, -1, _T(Query)); \
		kQuery.QueryResultCallback(QueryType); \
		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;} \
	}

bool PgDBCache::TableDataQuery( bool bReload )
{// ���ڵ� �������ϰ� ������ ���ÿ�.

	//CTableDataManager kTempTDM;
	//g_kTblDataMgr.swap(kTempTDM, bReload);	//���� ������ ���.
	//g_kTblDataMgr.Clear(bReload);//���̺� ������

	std::wstring kStrQuery;
	bool bIsImmidiate = true;
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_CHANNEL_EFFECT, _T("EXEC [dbo].[up_LoadChannelEffect]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_MYHOME_TEX, _T("EXEC [dbo].[up_LoadDefMyHomeTex]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_DEF_FILTER_UNICODE, _T("EXEC [dbo].[UP_LoadDefFilterUnicode]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_CARD_LOCAL, _T("EXEC [dbo].[up_LoadCardLocal]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_SPECIFIC_REWARD, _T("EXEC [dbo].[up_LoadDefSpecificReward]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_SPECIFIC_REWARD_EVENT, _T("EXEC [dbo].[up_LoadDefSpecificRewardEvent]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ABIL_TYPE, _T("EXEC [dbo].[UP_LoadDefAbilType]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	//{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFCLASS, _T("EXEC [dbo].[UP_LoadDefClass]"));								if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFCLASS_NATIONCODE, _T("EXEC [dbo].[up_LoadDefClass_NationCode]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	//{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFCLASS_ABIL, _T("EXEC [dbo].[UP_LoadDefClassAbil]")); 						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFCLASS_ABIL_NATIONCODE, _T("EXEC [dbo].[up_LoadDefClassAbil_NationCode]")); if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFCLASS_PET, _T("EXEC [dbo].[UP_LoadDefClassPet]"));							if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFCLASS_PET_LEVEL, _T("EXEC [dbo].[UP_LoadDefClassPetLevel]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFCLASS_PET_SKILL, _T("EXEC [dbo].[UP_LoadDefClassPetSkills]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFCLASS_PET_ITEMOPTION, _T("EXEC [dbo].[up_LoadDefClassPetItemOption]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFCLASS_PET_ABIL, _T("EXEC [dbo].[UP_LoadDefClassPetAbil]")); 				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFITEM_RES_CONVERT, _T("EXEC [dbo].[UP_LoadDefResConvert]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
		if (!Q_DQT_DEFITEMABIL( DT_DEF_PATH ))	goto __RECORVER;
		if (!Q_DQT_DEFITEMBAG( DT_DEF_PATH ))	goto __RECORVER;
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFITEMCONTAINER, _T("EXEC [dbo].[UP_LoadDefItemContainer2]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
		if (!Q_DQT_DEF_DROP_MONEY_CONTROL( DT_DEF_PATH ))	goto __RECORVER;
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFITEMRARE, _T("EXEC [dbo].[UP_LoadDefItemRare]"));							if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFITEMRAREGROUP, _T("EXEC [dbo].[UP_LoadDefItemRareGroup]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFMAP, _T("EXEC [dbo].[UP_LoadDefMap]"));									if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFMAPITEM, _T("EXEC [dbo].[UP_LoadDefMapItem]"));							if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFMAPMONSTERREGEN, _T("EXEC [dbo].[UP_LoadDefMapRegenPoint2]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
		if (!Q_DQT_DEFMONSTER( DT_DEF_PATH ))  goto __RECORVER;
		if (!Q_DQT_DEFMONSTERABIL( DT_DEF_PATH )) goto __RECORVER;
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFMONSTERTUNNING, _T("EXEC [dbo].[UP_LoadDefMonsterTunning]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFNPC, _T("EXEC [dbo].[UP_LoadDefNPC]"));									if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFNPCABIL, _T("EXEC [dbo].[UP_LoadDefNPCAbil]"));							if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFSKILL, _T("EXEC [dbo].[UP_LoadDefSkill]"));								if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFSKILL_NATIONCODE, _T("EXEC [dbo].[up_LoadDefSkill_NationCode]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFSKILLABIL, _T("EXEC [dbo].[UP_LoadDefSkillAbil2]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFSKILLABIL_NATIONCODE, _T("EXEC [dbo].[up_LoadDefSkillAbil_NationCode]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFSKILLSET, _T("EXEC [dbo].[UP_LoadDefSkillSet]"));							if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFEFFECT, _T("EXEC [dbo].[UP_LoadDefEffect2]")); 							if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
		if (!Q_DQT_DEFEFFECT_NATIONCODE( DT_DEF_PATH )) goto __RECORVER;
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFEFFECTABIL, _T("EXEC [dbo].[UP_LoadDefEffectAbil2]")); 					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
		if (!Q_DQT_DEFEFFECTABIL_NATIONCODE( DT_DEF_PATH )) goto __RECORVER;
		if (!Q_DQT_DEFRES( DT_DEF_PATH )) goto __RECORVER;
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFUPGRADECLASS, _T("EXEC [dbo].[UP_LoadDefUpgradeClass]")); 					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFITEMENCHANT, _T("EXEC [dbo].[UP_LoadDefItemEnchant]"));					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFCHARACTER_BASEWEAR, _T("EXEC [dbo].[UP_LoadDefCharacterBaseWear]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ITEM_PLUS_UPGRADE, _T("EXEC [dbo].[UP_LoadDefItemPlusUpgrade]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ITEM_PLUS_UPGRADE_NATIONCODE, _T("EXEC [dbo].[UP_LoadDefItemPlusUpgrade_NationCode]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ITEM_ENCHANT_SHIFT, _T("EXEC [dbo].[UP_LoadDefItemEnchantShift]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
		if (!Q_DQT_DEF_SUCCESS_RATE_CONTROL( DT_DEF_PATH ))	goto __RECORVER;
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFMONSTERBAG, _T("EXEC [dbo].[UP_LoadDefMonsterBag]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
		if (!Q_DQT_DEF_COUNT_CONTROL( DT_DEF_PATH )) goto __RECORVER;
		if (!Q_DQT_DEF_ITEM_BAG_ELEMENTS( DT_DEF_PATH )) goto __RECORVER;
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MONSTER_BAG_ELEMENTS, _T("EXEC [dbo].[UP_LoadDefMonsterBagElements]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_OBJECT, _T("EXEC [dbo].[UP_LoadDefObject]"));								if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_OBJECTABIL, _T("EXEC [dbo].[UP_LoadDefObjectAbil]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ITEM_BY_LEVEL, _T("EXEC [dbo].[up_LoadDefItemByLevel]"));					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_PVP_GROUNDMODE, _T("EXEC [dbo].[UP_LoadDefPvPGroundMode]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFITEMMAKING, _T("EXEC [dbo].[UP_LoadDefItemMaking]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFCOOKING, _T("EXEC [dbo].[UP_LoadDefCooking]"));							if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEFRESULTCONTROL, _T("EXEC [dbo].[UP_LoadDefResultControl]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
		if (!Q_DQT_DEF_QUEST_REWARD( DT_DEF_PATH )) goto __RECORVER;
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_QUEST_MIGRATION, _T("EXEC [dbo].[UP_LoadDefQuest_Migration]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_QUEST_RANDOM_EXP, _T("EXEC [dbo].[UP_LoadDefQuestRandomExp]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_QUEST_RANDOM_TACTICS_EXP, _T("EXEC [dbo].[UP_LoadDefQuestRandomTacticsExp]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_QUEST_RESET_SCHEDULE, _T("EXEC [dbo].[UP_LoadDefQuestResetSchedule]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}		
		if (!Q_DQT_DEF_ITEM_SET( DT_DEF_PATH )) goto __RECORVER;
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_SPEND_MONEY, _T("EXEC [dbo].[UP_LoadDefSpendMoney]"));					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_GUILD_LEVEL, L"EXEC [dbo].[UP_LoadDefGuildLevel]");						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_GUILD_SKILL, L"EXEC [dbo].[UP_LoadDefGuildSkill]");						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ITEM_OPTION, _T("EXEC [dbo].[UP_LoadDefItemOpt]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
		if (!Q_DQT_DEF_ITEM_OPTION_ABIL( DT_DEF_PATH )) goto __RECORVER;
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ITEM_RARITY_UPGRADE, _T("EXEC [dbo].[UP_LoadDefItemRarityUpgrade2]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_PROPERTY, _T("EXEC [dbo].[UP_LoadDefProperty2]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
		if (!Q_DQT_DEF_ITEM_BAG_GROUP( DT_DEF_PATH )) goto __RECORVER;
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_FIVE_ELEMENT_INFO, _T("EXEC [dbo].[UP_LoadDefFiveElementInfo]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}		
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_TACTICS_LEVEL, _T("EXEC [dbo].[UP_LoadDefTacticsLevel]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ACHIEVEMENTS, _T("EXEC [dbo].[UP_LOADDEFACHIEVEMENTS]"));					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_LOAD_RECOMMENDATIONITEM, _T("EXEC [dbo].[up_LoadDefRecommendationItem]"));if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_RARE_MONSTER_SPEECH, _T("EXEC [dbo].[UP_LoadDefRareMonsterSpeech]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}		
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MONSTERCARD, _T("EXEC [dbo].[up_LoadDefMonsterCard]"));					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MARRYTEXT, _T("EXEC [dbo].[up_LoadDefMarryText]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_HIDDENREWORDITEM, _T("EXEC [dbo].[up_LoadDefHiddenRewordItem]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_HIDDENREWORDBAG, _T("EXEC [dbo].[up_LoadDefHiddenRewordBag]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_EMOTION, _T("EXEC [dbo].[UP_LoadDefEmotionGroup]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_CONVERTITEM, _T("EXEC [dbo].[up_LoadDefConvertItem]"));					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_PET_HATCH, _T("EXEC [dbo].[up_LoadDefPet_Hatch]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_HOMETOWNTOMAPCOST, _T("EXEC [dbo].[up_LoadDefHometownToMapCost]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_SKILLIDX_TO_SKILLNO, _T("EXEC [dbo].[up_LoadDefSkillIdxToSkillNo]"));if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_LOAD_CARDABIL, _T("EXEC [dbo].[up_LoadDefCardAbil]"));					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_CARD_KEY_STRING, _T("EXEC [dbo].[up_LoadDefCardAbilKey]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_GROUND_RARE_MONSTER, _T("EXEC [dbo].[UP_LoadDefRareMonsterRegen]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MONSTER_BAG_CONTROL, _T("EXEC [dbo].[UP_LoadDefMapMonsterBagControl2]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MISSION_ROOT, _T("EXEC [dbo].[UP_LoadDefMissionLevels]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MISSION_CANDIDATE, _T("EXEC [dbo].[UP_LoadDefMissionLevelCandidate]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MISSION_DEFENCE_STAGE, _T("EXEC [dbo].[up_LoadDefDefenceStage]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MISSION_DEFENCE_WAVE, _T("EXEC [dbo].[up_LoadDefDefenceWave]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}		
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MISSION_DEFENCE7_MISSION, _T("EXEC [dbo].[up_LoadDefDefence7_Mission]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}		
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MISSION_DEFENCE7_STAGE, _T("EXEC [dbo].[up_LoadDefDefence7_Stage]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}		
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MISSION_DEFENCE7_WAVE, _T("EXEC [dbo].[up_LoadDefDefence7_Wave]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}		
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MISSION_DEFENCE7_GUARDIAN, _T("EXEC [dbo].[up_LoadDefDefence7_Guardian]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}		
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MISSION_BONUSMAP, _T("EXEC [dbo].[up_LoadDefMissionBonusMap]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}		
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_EMPORIA, _T("EXEC [dbo].[UP_LoadDefEmporiaPack]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_ITEM_RARITY_UPGRADE_COST_RATE, _T("EXEC [dbo].[up_LoadDefItemRarityUpgradeCostRate]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MONSTER_KILL_COUNT_REWARD, _T("EXEC [dbo].[UP_LoadDefMonsterKillCountReward]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_TRANSTOWER, _T("EXEC [dbo].[UP_LoadDefTransTower]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_PARTY_INFO, _T("EXEC [dbo].[up_LoadDefParty_Info]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MAP_ITEM_BAG, _T("EXEC [dbo].[UP_LoadDefMapItemBag]"));					if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_OBJECT_BAG, _T("EXEC [dbo].[UP_LoadDefObjectBag]"));						if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_OBJECT_BAG_ELEMENTS, _T("EXEC [dbo].[UP_LoadDefObjectElement]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MISSION_RESULT, _T("EXEC [dbo].[UP_LoadDefMissionResult2]"));				if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MISSION_CLASS_REWARD, _T("EXEC [dbo].[up_LoadDefMission_ClassReward]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_MISSION_RANK_REWARD, _T("EXEC [dbo].[up_LoadDefMission_RankReward]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_CHARCARDEFFECT, _T("EXEC [dbo].[up_LoadDefCharCardEffect]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_SIDEJOBRATE, _T("EXEC [dbo].[up_LoadDefSideJobRate]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}	
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_EVENTITEMSET, _T("EXEC [dbo].[UP_LoadDefEventItemSet]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_REDICEOPTIONCOST, _T("EXEC [dbo].[up_LoadDefRediceCost]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_MYHOMESIDEJOBTIME, _T("EXEC [dbo].[up_LoadDefMyHomeSidejobTime]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_MONSTER_ENCHANT_GRADE, _T("EXEC [dbo].[UP_LoadDefMonsterEnchantGrade]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_MYHOMEBUILDINGS, _T("EXEC [dbo].[up_LoadDefMyhomeBuildings]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_BASICOPTIONAMP, _T("EXEC [dbo].[up_LoadDefBasicOptionAmp]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_ITEM_AMP_SPECIFIC, _T("EXEC [dbo].[up_LoadDefItemAmplify_Specific]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_ALRAM_MISSION, _T("EXEC [dbo].[up_LoadDefAlramMission]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_DEATHPENALTY, _T("EXEC [dbo].[up_LoadDefDeathPenalty]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_DEF_DEFENCE_ADD_MONSTER, _T("EXEC [dbo].[up_LoadDefDefenceAddMonster]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_SKILLEXTENDITEM, _T("EXEC [dbo].[up_LoadDefSkillExtendItem]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_NPC_TALK_MAP_MOVE, _T("EXEC [dbo].[UP_LoadDefNpcTalkMapMove]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_JOBSKILL_PROBABILITY, _T("EXEC [dbo].[UP_LoadDefJobSkill_Probability]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_JOBSKILL_ITEMUPGRADE, _T("EXEC [dbo].[UP_LoadDefJobSkill_ItemUpgrade]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_JOBSKILL_SAVEIDX, _T("EXEC [dbo].[UP_LoadDefJobSkill_SaveIdx]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_JOBSKILL_LOCATIONITEM, _T("EXEC [dbo].[up_LoadDefJobSkill_LocationItem]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_JOBSKILL_SKILL, _T("EXEC [dbo].[up_LoadDefJobSkill_Skill]"));		if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_JOBSKILL_SKILLEXPERTNESS, _T("EXEC [dbo].[up_LoadDefJobSkill_SkillExpertness]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_JOBSKILL_TOOL, _T("EXEC [dbo].[up_LoadDefJobSkill_Tool]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_JOBSKILL_RECIPE, _T("EXEC [dbo].[UP_LoadDefJobSkill_Recipe]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_SOCKET_ITEM, _T("EXEC [dbo].[up_LoadDefItemSocket]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_JOBSKILL_ITEMSOULEXTRACT, _T("EXEC [dbo].[up_LoadDefItemSoulExtract]"));			if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_JOBSKILL_ITEMSOULTRANSITION, _T("EXEC [dbo].[up_LoadDefItemSoulTransition]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_EXPEDITION_NPC, _T("EXEC [dbo].[up_LoadDefExpeditionNpc]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
		if (!Q_DQT_PREMIUM_SERVICE( DT_LOCAL_PATH )) goto __RECORVER;
		if (!Q_DQT_PREMIUM_ARTICLE( DT_LOCAL_PATH )) goto __RECORVER;

	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_EVENTGROUP, _T("EXEC [dbo].[up_LoadDefEventGroup]")); if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ) { kStrQuery = kQuery.Command(); goto __RECORVER;} }
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_EVENTMONSTERGROUP, _T("EXEC [dbo].[up_LoadDefEventMonsterGroup]")); if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ) { kStrQuery = kQuery.Command(); goto __RECORVER;} }
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_EVENTREWARDITEMGROUP, _T("EXEC [dbo].[up_LoadDefEventRewardItemGroup]")); if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ) { kStrQuery = kQuery.Command(); goto __RECORVER;} }
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_EVENT_BOSSBATTLE, _T("EXEC [dbo].[up_LoadDefEvent_BossBattle]")); if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ) { kStrQuery = kQuery.Command(); goto __RECORVER;} }
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_EVENT_RACE, _T("EXEC [dbo].[up_LoadDefEvent_Race]")); if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ) { kStrQuery = kQuery.Command(); goto __RECORVER;} }
	{	CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_EVENTSCHEDULE, _T("EXEC [dbo].[up_LoadDefEventSchedule]")); if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ) { kStrQuery = kQuery.Command(); goto __RECORVER;} }
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_QUEST_LEVEL_REWARD, _T("EXEC [dbo].[up_LoadDefQuest_LevelReward]"));	if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_CHARACTER_CREATE_SET, _T("EXEC [dbo].[up_LoadDefCharacterCreateSet]"));	if (S_OK != PushCSVQuery(kQuery, bIsImmidiate)) { kStrQuery = kQuery.Command(); goto __RECORVER; }}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_MISSION_MUTATOR, _T("EXEC [dbo].[up_LoadDefMissionMutator]")); if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	{	CEL::DB_QUERY kQuery( DT_DEF, DQT_LOAD_DEF_MISSION_MUTATOR_ABIL, _T("EXEC [dbo].[up_LoadDefMissionMutator_Abil]")); if( S_OK != PushCSVQuery(kQuery, bIsImmidiate) ){kStrQuery = kQuery.Command(); goto __RECORVER;}}
	QUERY(DT_DEF, Q_DQT_LOAD_DEF_BATTLE_PASS_QUEST, "EXEC [dbo].[up_LoadDefBattlePass_Quest]")
	return true;
__RECORVER:
	{
		//g_kTblDataMgr.Clear();
		//g_kTblDataMgr.swap(kTempTDM, bReload);	//����� ������ �ǵ�����.

		INFO_LOG( BM::LOG_LV1, _T("Query Fail Data Recovery... Cause = ") << kStrQuery );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
}
#undef DT_LOCAL_PATH
#undef DT_DEF_PATH
#undef QUERY

bool PgDBCache::OnDBExcute(CEL::DB_RESULT &rkResult)
{
	switch( rkResult.QueryType() )
	{
	case DQT_LOAD_DEF_CHANNEL_EFFECT:			{ Q_DQT_LOAD_DEF_CHANNEL_EFFECT( rkResult ); }break;
	//case DQT_DEFCLASS:							{ Q_DQT_DEFCLASS(rkResult); }break;
	case DQT_DEFCLASS_NATIONCODE:				{ Q_DQT_DEFCLASS_NATIONCODE(rkResult); }break;
	//case DQT_DEFCLASS_ABIL:						{ Q_DQT_DEFCLASS_ABIL(rkResult); }break;
	case DQT_DEFCLASS_ABIL_NATIONCODE:			{ Q_DQT_DEFCLASS_ABIL_NATIONCODE(rkResult); }break;
	case DQT_DEFCLASS_PET:						{ Q_DQT_DEFCLASS_PET(rkResult); }break;
	case DQT_DEFCLASS_PET_LEVEL:				{ Q_DQT_DEFCLASS_PET_LEVEL(rkResult); }break;
	case DQT_DEFCLASS_PET_SKILL:				{ Q_DQT_DEFCLASS_PET_SKILL(rkResult); }break;
	case DQT_DEFCLASS_PET_ITEMOPTION:			{ Q_DQT_DEFCLASS_PET_ITEMOPTION(rkResult); }break;
	case DQT_DEFCLASS_PET_ABIL:					{ Q_DQT_DEFCLASS_PET_ABIL(rkResult); }break;		
	case DQT_DEF_ABIL_TYPE:						{ Q_DQT_DEF_ABIL_TYPE(rkResult); }break;	
	case DQT_DEFITEMCONTAINER:					{ Q_DQT_DEFITEMCONTAINER(rkResult);	}break;
	case DQT_DEFITEM_RES_CONVERT:				{ Q_DQT_DEFITEM_RES_CONVERT(rkResult); }break;
	case DQT_DEFITEMRARE:						{ Q_DQT_DEFITEMRARE(rkResult); }break;
	case DQT_DEFITEMRAREGROUP:					{ Q_DQT_DEFITEMRAREGROUP(rkResult); }break;
	case DQT_DEFMAP:							{ Q_DQT_DEFMAP(rkResult); }break;
	case DQT_DEFMAPITEM:						{ Q_DQT_DEFMAPITEM(rkResult); }break;
	case DQT_DEFMAPMONSTERREGEN:				{ Q_DQT_DEFMAPMONSTERREGEN(rkResult ); }break;
	case DQT_DEFMONSTERTUNNING:					{ Q_DQT_DEFMONSTERTUNNING(rkResult); }break;
	case DQT_DEFNPC:							{ Q_DQT_DEFNPC(rkResult); }break;
	case DQT_DEFNPCABIL:						{ Q_DQT_DEFNPCABIL(rkResult); }break;	
	case DQT_DEFSKILL:							{ Q_DQT_DEFSKILL(rkResult); }break;
	case DQT_DEFSKILL_NATIONCODE:				{ Q_DQT_DEFSKILL_NATIONCODE(rkResult); }break;
	case DQT_DEFSKILLABIL:						{ Q_DQT_DEFSKILLABIL(rkResult); }break;
	case DQT_DEFSKILLABIL_NATIONCODE:			{ Q_DQT_DEFSKILLABIL_NATIONCODE(rkResult); }break;
	case DQT_DEFSKILLSET:						{ Q_DQT_DEFSKILLSET(rkResult); }break;
	case DQT_DEFITEMMAKING:						{ Q_DQT_DEFITEMMAKING(rkResult); }break;
	case DQT_DEFCOOKING:						{ Q_DQT_DEFCOOKING(rkResult); }break;
	case DQT_DEFRESULTCONTROL:					{ Q_DQT_DEFRESULTCONTROL(rkResult); }break;
	case DQT_DEFEFFECT:							{ Q_DQT_DEFEFFECT(rkResult); }break;
	case DQT_DEFEFFECTABIL:						{ Q_DQT_DEFEFFECTABIL(rkResult); }break;
	case DQT_DEFUPGRADECLASS:					{ Q_DQT_DEFUPGRADECLASS(rkResult); }break;
	case DQT_DEFCHARACTER_BASEWEAR:				{ Q_DQT_DEFCHARACTER_BASEWEAR(rkResult); }break;
	case DQT_DEFITEMENCHANT:					{ Q_DQT_DEFITEMENCHANT(rkResult); }break;
	case DQT_DEF_ITEM_PLUS_UPGRADE:				{ Q_DQT_DEF_ITEM_PLUS_UPGRADE(rkResult); }break;
	case DQT_DEF_ITEM_PLUS_UPGRADE_NATIONCODE:	{ Q_DQT_DEF_ITEM_PLUS_UPGRADE_NATIONCODE(rkResult); }break;
	case DQT_DEF_ITEM_ENCHANT_SHIFT:			{ Q_DQT_DEF_ITEM_ENCHANT_SHIFT(rkResult); }break;
	case DQT_DEFMONSTERBAG:						{ Q_DQT_DEFMONSTERBAG(rkResult); }break;	
	case DQT_DEF_MONSTER_BAG_ELEMENTS:			{ Q_DQT_DEF_MONSTER_BAG_ELEMENTS(rkResult); }break;
	case DQT_DEF_MONSTER_BAG_CONTROL:			{ Q_DQT_DEF_MONSTER_BAG_CONTROL(rkResult); }break;
	case DQT_DEF_OBJECT:						{ Q_DQT_DEF_OBJECT(rkResult);}break;
	case DQT_DEF_OBJECTABIL:					{ Q_DQT_DEF_OBJECTABIL(rkResult);}break;
	case DQT_DEF_ITEM_BY_LEVEL:					{ Q_DQT_DEF_ITEM_BY_LEVEL(rkResult);}break;
	case DQT_DEF_QUEST_RANDOM_EXP:				{ Q_DQT_DEF_QUEST_RANDOM_EXP(rkResult); }break;
	case DQT_DEF_QUEST_RANDOM_TACTICS_EXP:		{ Q_DQT_DEF_QUEST_RANDOM_TACTICS_EXP(rkResult); }break;
	case DQT_DEF_QUEST_RESET_SCHEDULE:			{ Q_DQT_DEF_QUEST_RESET_SCHEDULE(rkResult); }break;	
	case DQT_DEF_QUEST_MIGRATION:				{ Q_DQT_DEF_QUEST_MIGRATION(rkResult); }break;	
	case DQT_DEF_MISSION_CANDIDATE:				{Q_DQT_DEF_MISSION_CANDIDATE(rkResult);}break;
	case DQT_DEF_MISSION_ROOT:					{Q_DQT_DEF_MISSION_ROOT(rkResult);}break;	
	case DQT_DEF_DEFENCE_ADD_MONSTER:			{Q_DQT_DEF_DEFENCE_ADD_MONSTER(rkResult);}break;	
	case DQT_DEF_MISSION_DEFENCE_STAGE:			{Q_DEF_MISSION_DEFENCE_STAGE(rkResult);}break;	
	case DQT_DEF_MISSION_DEFENCE_WAVE:			{Q_DEF_MISSION_DEFENCE_WAVE(rkResult);}break;	
	case DQT_DEF_MISSION_DEFENCE7_MISSION:		{Q_DEF_MISSION_DEFENCE7_MISSION(rkResult);}break;	
	case DQT_DEF_MISSION_DEFENCE7_STAGE:		{Q_DEF_MISSION_DEFENCE7_STAGE(rkResult);}break;	
	case DQT_DEF_MISSION_DEFENCE7_WAVE:			{Q_DEF_MISSION_DEFENCE7_WAVE(rkResult);}break;	
	case DQT_DEF_MISSION_DEFENCE7_GUARDIAN:		{Q_DEF_MISSION_DEFENCE7_GUARDIAN(rkResult);}break;	
	case DQT_DEF_MISSION_BONUSMAP:				{Q_DEF_MISSION_BONUSMAP(rkResult);}break;			
	case DQT_DEF_ITEM_OPTION:					{Q_DQT_DEF_ITEM_OPTION(rkResult);}break;
	case DQT_DEF_ITEM_RARITY_UPGRADE:			{Q_DQT_DEF_ITEM_RARITY_UPGRADE(rkResult);}break;
	case DQT_DEF_PVP_GROUNDMODE:				{Q_DQT_DEF_PVP_GROUNDMODE(rkResult);	}break;
	case DQT_DEF_FILTER_UNICODE:				{Q_DQT_DEF_FILTER_UNICODE(rkResult);}break;
	case DQT_DEF_SPEND_MONEY:					{Q_DQT_DEF_SPEND_MONEY(rkResult);}break;
	case DQT_DEF_GUILD_LEVEL:					{Q_DQT_DEF_GUILD_LEVEL(rkResult);}break;
	case DQT_DEF_GUILD_SKILL:					{Q_DQT_DEF_GUILD_SKILL(rkResult);}break;
	case DQT_DEF_EMPORIA:						{Q_DQT_DEF_EMPORIA(rkResult);}break;
	case DQT_DEF_PROPERTY:						{Q_DQT_DEF_PROPERTY(rkResult);}break;	
	case DQT_DEF_FIVE_ELEMENT_INFO:				{Q_DQT_DEF_FIVE_ELEMENT_INFO(rkResult);}break;	
	case DQT_DEF_ITEM_RARITY_UPGRADE_COST_RATE:	{Q_DQT_DEF_ITEM_RARITY_UPGRADE_COST_RATE(rkResult);}break;
	case DQT_DEF_TACTICS_LEVEL:					{Q_DQT_DEF_TACTICS_LEVEL(rkResult);}break;	
	case DQT_DEF_MONSTER_KILL_COUNT_REWARD:		{Q_DQT_DEF_MONSTER_KILL_COUNT_REWARD(rkResult);}break;
	case DQT_DEF_ACHIEVEMENTS:					{Q_DQT_DEF_ACHIEVEMENTS(rkResult);}break;	
	case DQT_DEF_MONSTERCARD:					{Q_DQT_DEF_MONSTERCARD(rkResult);}break;
	case DQT_DEF_MARRYTEXT:						{Q_DQT_DEF_MARRYTEXT(rkResult);}break;
	case DQT_DEF_HIDDENREWORDITEM:				{Q_DQT_DEF_HIDDENREWORDITEM(rkResult);}break;
	case DQT_DEF_HIDDENREWORDBAG:				{Q_DQT_DEF_HIDDENREWORDBAG(rkResult);}break;
	case DQT_DEF_EMOTION:						{Q_DQT_DEF_EMOTION(rkResult);}break;	
	case DQT_DEF_CONVERTITEM:					{Q_DQT_DEF_CONVERTITEM(rkResult);}break;
	case DQT_DEF_PET_HATCH:						{Q_DQT_DEF_PET_HATCH(rkResult);}break;	
	case DQT_DEF_LOAD_RECOMMENDATIONITEM:		{Q_DQT_DEF_LOAD_RECOMMENDATIONITEM(rkResult);}break;
	case DQT_DEF_RARE_MONSTER_SPEECH:			{Q_DQT_DEF_RARE_MONSTER_SPEECH(rkResult);}break;
	case DQT_LOAD_CARD_LOCAL:					{Q_DQT_LOAD_CARD_LOCAL(rkResult);}break;
	case DQT_LOAD_DEF_HOMETOWNTOMAPCOST:		{Q_DQT_LOAD_DEF_HOMETOWNTOMAPCOST(rkResult);} break;
	case DQT_LOAD_DEF_MYHOME_TEX:				{Q_DQT_LOAD_DEF_MYHOME_TEX(rkResult);} break;
	case DQT_LOAD_DEF_SKILLIDX_TO_SKILLNO:		{Q_DQT_LOAD_DEF_SKILLIDX_TO_SKILLNO(rkResult);}break;
	case DQT_DEF_LOAD_CARDABIL:					{Q_DQT_DEF_LOAD_CARDABIL(rkResult);}break;
	case DQT_LOAD_DEF_CARD_KEY_STRING:			{Q_DQT_LOAD_DEF_CARD_KEY_STRING(rkResult);}break;
	case DQT_DEF_GROUND_RARE_MONSTER:			{Q_DQT_DEF_GROUND_RARE_MONSTER(rkResult);}break;
	case DQT_DEF_TRANSTOWER:					{Q_DQT_DEF_TRANSTOWER(rkResult);}break;
	case DQT_DEF_PARTY_INFO:					{Q_DQT_DEF_PARTY_INFO(rkResult);}break;
	case DQT_DEF_MAP_ITEM_BAG:					{Q_DQT_DEF_MAP_ITEM_BAG(rkResult);}break;
	case DQT_DEF_OBJECT_BAG:					{Q_DQT_DEF_OBJECT_BAG(rkResult);}break;
	case DQT_DEF_OBJECT_BAG_ELEMENTS:			{Q_DQT_DEF_OBJECT_BAG_ELEMENTS(rkResult);}break;
	case DQT_DEF_MISSION_RESULT:				{Q_DQT_DEF_MISSION_RESULT(rkResult);}break;
	case DQT_DEF_MISSION_CLASS_REWARD:			{Q_DQT_DEF_MISSION_CLASS_REWARD(rkResult);}break;
	case DQT_DEF_MISSION_RANK_REWARD:			{Q_DQT_DEF_MISSION_RANK_REWARD(rkResult);}break;
	case DQT_LOAD_DEF_CHARCARDEFFECT:			{Q_DQT_LOAD_DEF_CHARCARDEFFECT(rkResult);}break;
	case DQT_LOAD_DEF_SIDEJOBRATE:				{Q_DQT_LOAD_DEF_SIDEJOBRATE(rkResult);}break;
	case DQT_LOAD_DEF_EVENTITEMSET:				{Q_DQT_LOAD_DEF_EVENTITEMSET(rkResult);}break;
	case DQT_LOAD_DEF_REDICEOPTIONCOST:			{Q_DQT_LOAD_DEF_REDICEOPTIONCOST(rkResult);}break;
	case DQT_LOAD_DEF_MYHOMESIDEJOBTIME:		{Q_DQT_LOAD_DEF_MYHOMESIDEJOBTIME(rkResult);}break;
	case DQT_LOAD_DEF_MYHOMEBUILDINGS:			{Q_DQT_LOAD_DEF_MYHOMEBUILDINGS(rkResult);}break;
	case DQT_LOAD_DEF_MONSTER_ENCHANT_GRADE:	{Q_DQT_LOAD_DEF_MONSTER_ENCHANT_GRADE(rkResult);}break;
	case DQT_LOAD_DEF_BASICOPTIONAMP:			{Q_DQT_LOAD_DEF_BASICOPTIONAMP(rkResult);}break;
	case DQT_LOAD_DEF_ITEM_AMP_SPECIFIC:		{Q_DQT_LOAD_DEF_ITEM_AMP_SPECIFIC(rkResult);}break;		
	case DQT_LOAD_DEF_ALRAM_MISSION:			{Q_DQT_LOAD_DEF_ALRAM_MISSION(rkResult);}break;
	case DQT_LOAD_DEF_DEATHPENALTY:				{Q_DQT_LOAD_DEF_DEATHPENALTY(rkResult);}break;
	case DQT_LOAD_DEF_SKILLEXTENDITEM:			{Q_DQT_LOAD_DEF_SKILLEXTENDITEM(rkResult);}break;
	case DQT_LOAD_DEF_NPC_TALK_MAP_MOVE:		{Q_DQT_LOAD_DEF_NPC_TALK_MAP_MOVE(rkResult);}break;
	case DQT_LOAD_DEF_SPECIFIC_REWARD:			{Q_DQT_LOAD_DEF_SPECIFIC_REWARD(rkResult);}break;
	case DQT_LOAD_DEF_SPECIFIC_REWARD_EVENT:	{Q_DQT_LOAD_DEF_SPECIFIC_REWARD_EVENT(rkResult);}break;
	case DQT_LOAD_DEF_JOBSKILL_PROBABILITY:		{Q_DQT_LOAD_DEF_JOBSKILL_PROBABILITY(rkResult);}break;
	case DQT_LOAD_DEF_JOBSKILL_LOCATIONITEM:	{Q_LOAD_DEF_JOBSKILL_LOCATIONITEM(rkResult);}break;	
	case DQT_LOAD_DEF_JOBSKILL_SKILL:			{Q_LOAD_DEF_JOBSKILL_SKILL(rkResult);}break;		
	case DQT_LOAD_DEF_JOBSKILL_SKILLEXPERTNESS:	{Q_LOAD_DEF_JOBSKILL_SKILLEXPERTNESS(rkResult);}break;	
	case DQT_LOAD_DEF_JOBSKILL_TOOL:			{Q_LOAD_DEF_JOBSKILL_TOOL(rkResult);}break;
	case DQT_LOAD_DEF_JOBSKILL_ITEMUPGRADE:		{Q_DQT_LOAD_DEF_JOBSKILL_ITEMUPGRADE(rkResult);}break;
	case DQT_LOAD_DEF_JOBSKILL_SAVEIDX:			{Q_DQT_LOAD_DEF_JOBSKILL_SAVEIDX(rkResult);}break;	
	case DQT_LOAD_DEF_JOBSKILL_SHOP:			{Q_DQT_LOAD_DEF_JOBSKILL_SHOP(rkResult);}break;
	case DQT_LOAD_DEF_JOBSKILL_RECIPE:			{Q_DQT_LOAD_DEF_JOBSKILL_RECIPE(rkResult);}break;	
	case DQT_LOAD_DEF_SOCKET_ITEM:				{Q_LOAD_DEF_SOCKET_ITEM(rkResult);}break;
	case DQT_LOAD_DEF_JOBSKILL_ITEMSOULEXTRACT:			{Q_DQT_LOAD_DEF_JOBSKILL_ITEMSOULEXTRACT(rkResult);}break;
	case DQT_LOAD_DEF_JOBSKILL_ITEMSOULTRANSITION:		{Q_DQT_LOAD_DEF_JOBSKILL_ITEMSOULTRANSITION(rkResult);}break;	
	case DQT_LOAD_DEF_EXPEDITION_NPC:		{Q_DQT_LOAD_DEF_EXPEDITION_NPC(rkResult);}break;	
	case DQT_SELECT_TOP1_EMPORIA_FOR_CHECK:				{Q_DQT_SELECT_TOP1_EMPORIA_FOR_CHECK(rkResult);}break;
	case DQT_UPDATE_PREMIUM_SERVICE:			{Q_DQT_UPDATE_PREMIUM_SERVICE(rkResult);}break;
	case DQT_SELECT_CASHITEM_FOR_SOUL_LEVEL_DEC:		{Q_DQT_SELECT_CASHITEM_FOR_SOUL_LEVEL_DEC(rkResult);}break;

	case DQT_LOAD_DEF_EVENTGROUP:				{Q_DQT_LOAD_DEF_EVENTGROUP(rkResult);}break;
	case DQT_LOAD_DEF_EVENTMONSTERGROUP:		{Q_DQT_LOAD_DEF_EVENTMONSTERGROUP(rkResult);}break;
	case DQT_LOAD_DEF_EVENTREWARDITEMGROUP:		{Q_DQT_LOAD_DEF_EVENTREWARDITEMGROUP(rkResult);}break;
	case DQT_LOAD_DEF_EVENT_BOSSBATTLE:			{Q_DQT_LOAD_DEF_EVENT_BOSSBATTLE(rkResult);}break;
	case DQT_LOAD_DEF_EVENT_RACE:				{Q_DQT_LOAD_DEF_EVENT_RACE(rkResult);}break;
	case DQT_LOAD_DEF_EVENTSCHEDULE:			{Q_DQT_LOAD_DEF_EVENTSCHEDULE(rkResult);}break;
	case DQT_LOAD_QUEST_LEVEL_REWARD:			{Q_DQT_LOAD_QUEST_LEVEL_REWARD(rkResult);}break;
	case DQT_LOAD_JOBSKILL_EVENT_LOCATION:			{Q_DQT_LOAD_JOBSKILL_EVENT_LOCATION(rkResult);}break;
	case DQT_LOAD_DEF_CHARACTER_CREATE_SET:			{Q_DQT_LOAD_DEF_CHARACTER_CREATE_SET(rkResult); } break;
	case DQT_LOAD_DEF_MISSION_MUTATOR:			{Q_DQT_LOAD_DEF_MISSION_MUTATOR(rkResult);}break;
	case DQT_LOAD_DEF_MISSION_MUTATOR_ABIL:		{Q_DQT_LOAD_DEF_MISSION_MUTATOR_ABIL(rkResult);}break;
	default:
		{
			return false;
		}
	}

	return true;
}