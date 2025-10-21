#include "StdAfx.h"
#include "TableDataManager.h"
#include "PgObjcetUnitDefMgr.h"

PgObjectUnitDefMgr::PgObjectUnitDefMgr(void)
{
}

PgObjectUnitDefMgr::~PgObjectUnitDefMgr(void)
{
	Clear();
}

void PgObjectUnitDefMgr::Clear()
{
	T_MY_BASE_TYPE::Clear();
}

void PgObjectUnitDefMgr::swap(PgObjectUnitDefMgr &rkRight)
{
	m_contDef.swap(rkRight.m_contDef);
	m_DefPool.swap(rkRight.m_DefPool);
}

bool PgObjectUnitDefMgr::Build(CONT_BASE const  &tblBase, CONT_ABIL const &tblAbil)
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

bool PgObjectUnitDefMgr::Build(CONT_BASE const &tblBase, CONT_ABIL const &tblAbil,CONT_DEFSTRINGS const&rkDefString)
{
	Clear();
	bool bReturn = true;

	wchar_t const *pText = NULL;
	CONT_BASE::const_iterator base_itor = tblBase.begin();

	while( tblBase.end() != base_itor )
	{
		DEF *pDef = NewDef();

		pDef->No( base_itor->second.iObjectNo );
//		pDef->NameNo( base_itor->second.Name );
		pDef->Height( base_itor->second.fHeightValue );

// 		CONT_DEFSTRINGS::const_iterator str_itor = rkDefString.find( pDef->NameNo() );
// 		if ( str_itor==rkDefString.end() )
// 		{
// 			bResult = false;
// 			VERIFY_INFO_LOG(false, BM::LOG_LV4,_T("[%s] Object[%d] DefString[%d] Not Found"),__FUNCTIONW__, pDef->No(), pDef->NameNo());
// 			m_DefPool.Delete(pDef);
// 			++base_itor;
// 			continue;
// 		}

		
		for(int iIndex = 0; MAX_OBJECT_ABIL_LIST > iIndex ; ++iIndex)
		{//기본 리스트 번호 찾고 
			int const iAbilNo = (*base_itor).second.aAbil[iIndex];
			if( iAbilNo )
			{
				CONT_ABIL::const_iterator abil_itor = tblAbil.find( iAbilNo );

				if( abil_itor != tblAbil.end() )
				{
					for( int j = 0; MAX_OBJECT_ABIL_ARRAY > j ; ++j)
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
					VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Find ObjectAbil : Object[") << (*base_itor).second.iObjectNo << _T("], AbilTable[") << iAbilNo << _T("]") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Abil"));
					bReturn = false;
				}
			}
		}

		auto kRet = m_contDef.insert( std::make_pair(pDef->No(), pDef) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Duplicater ObjectUnitNo[") << pDef->No() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
			bReturn = false;
		}
		++base_itor;
	}

	if( !bReturn )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" Build Failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		bReturn = false;
	}
	return bReturn;
}
