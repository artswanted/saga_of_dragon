#include "StdAfx.h"
#include "TableDataManager.h"
#include "PgQuestInfo.h"
#include "MonsterDefMgr.h"
#include "TableDataManager.h"
 
CMonsterDefMgr::CMonsterDefMgr(void)
{
}

CMonsterDefMgr::~CMonsterDefMgr(void)
{
	Clear();
}

void CMonsterDefMgr::Clear()
{
	T_MY_BASE_TYPE::Clear();
	m_kContDefMonsterBag.clear();
}

void CMonsterDefMgr::swap(CMonsterDefMgr &rkRight)
{
	m_contDef.swap(rkRight.m_contDef);
	m_DefPool.swap(rkRight.m_DefPool);
	m_kContDefMonsterBag.swap(rkRight.m_kContDefMonsterBag);
}

bool CMonsterDefMgr::Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil)
{
	const CONT_DEFSTRINGS* pContDefStrings = NULL;
	g_kTblDataMgr.GetContDef(pContDefStrings);
	if( !pContDefStrings )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" Not Found DefString") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return Build(tblBase,tblAbil,*pContDefStrings);
}

bool CMonsterDefMgr::Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil,const CONT_DEFSTRINGS& rkDefString)
{
	Clear();
	bool bResult = true;

	wchar_t const *pText = NULL;
	CONT_BASE::const_iterator base_itor = tblBase.begin();
	
	while( tblBase.end() != base_itor )
	{
		DEF *pDef = NewDef();

		pDef->No( (*base_itor).second.MonsterNo );
		pDef->NameNo((*base_itor).second.Name);

		CONT_DEFSTRINGS::const_iterator str_itor = rkDefString.find(pDef->NameNo());
		if( rkDefString.end() == str_itor )
		{
			bResult = false;
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" Monster[") << pDef->No() << _T("] DefString[") << pDef->NameNo() << _T("] Not Found") );
			DeleteDef(pDef);
		}
		else
		{
			for(int iIndex = 0; MAX_MONSTER_ABIL_LIST > iIndex ; ++iIndex)
			{//기본 리스트 번호 찾고 
				int const iAbilNo = (*base_itor).second.aAbil[iIndex];
				if( iAbilNo )
				{
					CONT_ABIL::const_iterator abil_itor = tblAbil.find( iAbilNo );

					if( abil_itor != tblAbil.end() )
					{
						for( int j = 0; MAX_MONSTER_ABIL_ARRAY > j ; ++j)
						{
							if(0 != (*abil_itor).second.aType[j])
							{
								switch((*abil_itor).second.aType[j])
								{
								case AT_EXPERIENCE:
								case AT_MONEY:
								case AT_DIE_EXP:
								case AT_CLASSLIMIT:	
									{
	//									assert(NULL);
										pDef->SetAbil64((*abil_itor).second.aType[j], (*abil_itor).second.aValue[j]); 
									}break;
								default:			
									{ 
										pDef->SetAbil((*abil_itor).second.aType[j], (*abil_itor).second.aValue[j]); 
									}break;
								}
							}
						}
					}
					else
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Find MonsterAbil : Mon[") << (*base_itor).second.MonsterNo << _T("], AbilTable[") << iAbilNo << _T("]") );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
						bResult = false;
					}
				}
			}

			auto kRet = m_contDef.insert( std::make_pair(pDef->No(), pDef) );
			if( !kRet.second )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Duplicater DefMonster ID[") << pDef->No() << _T("]") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
				DeleteDef(pDef);
				bResult = false;
			}
		}
		++base_itor;
	}

	if( !bResult )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" Build Failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	return bResult;
}

//bool CMonsterDefMgr::BuildQuest(const ContMonsterQuestDef& rkVec)
//{
//	ContQuestMonsterDef::const_iterator iter = rkVec.begin();
//	while(rkVec.end() != iter)
//	{
//		CMonsterDef* pDef = NULL;
//		const ContQuestMonsterDef::key_type& iClassNo = (*iter).first;
//		CONT_DEF::iterator monster_iter = m_contDef.find(iClassNo);
//		if( m_contDef.end() != monster_iter )
//		{
//			pDef = (*monster_iter).second;
//		}
//		if( pDef )
//		{
//			pDef->AddQuest(rkVec[i].second);
//		}
//		else
//		{
//			VERIFY_INFO_LOG(false, BM::LOG_LV3, _T("[%s]-[%d] Can't find monster[%d], Quest ID[%d]"), __FUNCTIONW__, __LINE__, rkVec[i].first, rkVec[i].second);
//		}
//
//		++iter;
//	}
//	return true;
//}