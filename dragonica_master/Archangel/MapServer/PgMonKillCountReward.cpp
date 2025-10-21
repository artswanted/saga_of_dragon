#include "stdafx.h"
#include "lohengrin/dbtables2.h"
#include "Lohengrin/VariableContainer.h"
#include "constant.h"
#include "PgMonKillCountReward.h"

PgMonKillCountReward::PgMonKillCountReward()
{
	m_kMonsterKillSystem = false;
	m_kMonsterKillResetMax = 0;
	m_kMonsterKillPerMsg = 0;
	m_kCanCountLvMin = 0;
	m_kCanCountLvMax = 0;
}

PgMonKillCountReward::~PgMonKillCountReward()
{
}

bool PgMonKillCountReward::Init()
{
	bool bReturn = true;
	m_kReward.clear();

	int iValue = 0;
	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Hunting, EVar_MonsterKill_SystemEnable, iValue) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find 'MonsterKill_SystemEnable'");
		bReturn = false;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
	}
	m_kMonsterKillSystem = (iValue > 0) ? true : false;

	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Hunting, EVar_MonsterKill_ResetCount, iValue) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find 'EVar_MonsterKill_ResetCount'");
		bReturn = false;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
	}
	m_kMonsterKillResetMax = iValue;

	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Hunting, EVar_MonsterKill_PerMsg, iValue) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find 'EVar_MonsterKill_PerMsg'");
		bReturn = false;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
	}
	m_kMonsterKillPerMsg = iValue;

	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Hunting, EVar_MonsterKill_CanMinDiffLevel, iValue) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find 'EVar_MonsterKill_CanMinDiffLevel'");
		bReturn = false;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
	}
	m_kCanCountLvMin = iValue;

	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Hunting, EVar_MonsterKill_CanMaxDiffLevel, iValue) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find 'EVar_MonsterKill_CanMaxDiffLevel'");
		bReturn = false;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
	}
	m_kCanCountLvMax = iValue;

	CONT_DEF_MONSTER_KILL_COUNT_REWARD const *pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if( !pkDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Load Def");
		bReturn = false;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
	}



	// 테이터 검증 루틴
	// 레벨업 가능한 최데 레벨 / 몬스터 킬 카운트 보상이 주어지는 숫자를 구해
	// 1:1로 대입해 본다.
	typedef std::set< int > ContKillCount;
	ContKillCount kSet;

	//m_kReward = *pkDef;
	//ContKillCountReward
	CONT_DEF_MONSTER_KILL_COUNT_REWARD::const_iterator loop_iter = pkDef->begin();
	while( pkDef->end() != loop_iter )
	{
		int const iKillCount = (*loop_iter).first;

		kSet.insert(iKillCount);

		auto kRet = m_kReward.insert( std::make_pair(iKillCount, ContKillCountReward::mapped_type()) );
		ContKillCountReward::iterator insert_iter = kRet.first;
		if( m_kReward.end() != insert_iter )
		{
			ContKillCountReward::mapped_type &rkVec = (*insert_iter).second;
			std::back_inserter(rkVec) = (*loop_iter).second;
		}
		++loop_iter;
	}

	int iMaxPlayerLevel = 0; //
	CONT_DEFCLASS const* pkClassDef = NULL;
	g_kTblDataMgr.GetContDef(pkClassDef);
	if( !pkClassDef )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	CONT_DEFCLASS::const_iterator class_iter = pkClassDef->begin();
	while( pkClassDef->end() != class_iter )
	{
		CONT_DEFCLASS::mapped_type const& rkClassDef = (*class_iter).second;
		if( iMaxPlayerLevel < rkClassDef.sLevel )
		{
			iMaxPlayerLevel = rkClassDef.sLevel;
		}
		++class_iter;
	}


	if( !iMaxPlayerLevel )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Can't find, Player can max level"));
		bReturn = false;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false"));
	}

	SMonKillRewardItem kTempItem;
	int const iStartPlayerLevel = 1;
	for( int iCur = iStartPlayerLevel; iMaxPlayerLevel>=iCur; ++iCur )
	{
		ContKillCount::const_iterator kill_iter = kSet.begin();
		while( kSet.end() != kill_iter )
		{
			if( !Check((*kill_iter), iCur, kTempItem) )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Can't Get monster-kill-count-reward player[Level: ") << iCur << _T("]"));
				bReturn = false;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false"));
			}

			++kill_iter;
		}
	}

	return bReturn;
}

bool PgMonKillCountReward::Check(int const iCount, int const iLevel, SMonKillRewardItem &rkOut) const
{
	ContKillCountReward::const_iterator find_iter = m_kReward.find(iCount);
	if( m_kReward.end() != find_iter )
	{
		ContKillCountReward::mapped_type const &rkVec = (*find_iter).second;
		ContReward::const_iterator loop_iter = rkVec.begin();
		while( rkVec.end() != loop_iter )
		{
			if( (*loop_iter).CheckLevel(iLevel) )
			{
				rkOut = (*loop_iter);
				return true;
			}
			++loop_iter;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}