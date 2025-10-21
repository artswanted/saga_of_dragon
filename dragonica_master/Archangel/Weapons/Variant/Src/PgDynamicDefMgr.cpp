#include "StdAfx.h"
#include "PgDynamicDefMgr.h"
#include "TableDataManager.h"

PgDynamicDefMgr::PgDynamicDefMgr()
{}

PgDynamicDefMgr::~PgDynamicDefMgr()
{}

void PgDynamicDefMgr::swap(PgDynamicDefMgr& rkRight)
{
	m_contDef.swap(rkRight.m_contDef);
	m_DefPool.swap(rkRight.m_DefPool);
}

bool PgDynamicDefMgr::Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil)
{
	return Build(&tblBase,&tblAbil);
}

bool PgDynamicDefMgr::Build(const CONT_BASE *ptblBase, const CONT_ABIL *ptblAbil)
{
	bool bReturn = true;

	if ( !ptblBase || !ptblAbil )
	{// DefControl은 없어도 된다.
		return true;
	}
	Clear();

	PgAddAbilRateDef *pkDef = NULL;
	CONT_BASE::const_iterator bag_itr;
	for( bag_itr=ptblBase->begin(); bag_itr!=ptblBase->end(); ++bag_itr )
	{
		CONT_BASE::mapped_type element = bag_itr->second;
		if( NULL == NewDef(&pkDef) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Memory Error!!!");
			bReturn = false;
		}

		auto kConPair = m_contDef.insert(std::make_pair(element.BagID,pkDef));
		if ( !kConPair.second )
			continue;

		for( int i=0; i!=MAX_DYNAMICABILRATE_LIST; ++i )
		{
			CONT_ABIL::const_iterator abil_itr = ptblAbil->find(element.aAbil[i]);
			if ( abil_itr != ptblAbil->end() )
			{
				CONT_ABIL::mapped_type abil_element = abil_itr->second;
				for( int j=0; j!=MAX_DYNAMICABILRATE_ARRAY; ++j )
				{
					if ( abil_element.aType[j] )
					{
						pkDef->AddAbil( abil_element.aType[j], abil_element.aValue[j] );
					}
				}
			}
		}
		pkDef = NULL;
	}
	DeleteDef(pkDef);
	return bReturn;
}