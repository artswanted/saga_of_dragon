#include "stdafx.h"
#include "Global.h"
#include "PgCustomUnit.h"
//#include "lohengrin/dbtables.h"
#include "TableDataManager.h"
//#include "../PgClassDefMgr.h"
#include "PgControlDefMgr.h"

PgCustomUnit::PgCustomUnit()
{
}

PgCustomUnit::~PgCustomUnit()
{
}

HRESULT PgCustomUnit::Create(void const *pkInfo )
{
	Init();

	SCustomUnitInfo const *pkCustomUnitInfo = (SCustomUnitInfo const*)pkInfo;
	if ( SetInfo( pkCustomUnitInfo ) )
	{
		SetAbil(AT_CANNOT_DAMAGE, 0);	//공격받아서는 안된다.
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgCustomUnit::SetInfo(const SCustomUnitInfo* pkInfo)
{
	SetID(pkInfo->kGuid);
	SetPos(pkInfo->ptPos);
	SetAbil(AT_CLASS, pkInfo->kClassKey.iClass);
	SetAbil(AT_LEVEL, pkInfo->kClassKey.nLv);
	SetAbil(AT_C_MAX_HP, 1);

	return true;
}

bool PgCustomUnit::IsCheckZoneTime( DWORD dwElapsed )
{
	if (m_dwElapsedTimeCheckZone < dwElapsed)
	{
		m_dwElapsedTimeCheckZone += 1500;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


void PgCustomUnit::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
{
	CUnit::WriteToPacket(rkPacket,kWriteType);
	rkPacket.Push(GetPos());
}

EWRITETYPE PgCustomUnit::ReadFromPacket(BM::Stream &rkPacket)
{
	EWRITETYPE kWriteType = CUnit::ReadFromPacket(rkPacket);
	POINT3 ptPos;
	rkPacket.Pop(ptPos);
	SetPos(ptPos);	
	return kWriteType;
}

/*
void PgCustomUnit::CopyAbilFromClassDef(const CLASS_DEF_BUILT* pkDef)
{
	if (pkDef == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkDef is NULL"));
		return;
	}

	CLASSDEF_ABIL_CONT::const_iterator itor = pkDef->kAbil.begin();
	while (itor != pkDef->kAbil.end())
	{
		SetAbil(itor->first, itor->second);

		++itor;
	}
	if(0 == GetAbil(AT_MAX_HP))
	{
		SetAbil(AT_MAX_HP, static_cast<int>(pkDef->kDef.sMaxHP) );
	}
	if(0 == GetAbil(AT_HP))
	{
		SetAbil(AT_HP, GetAbil(AT_MAX_HP) );
	}
	
	SetAbil(AT_HP_RECOVERY_INTERVAL, static_cast<int>(pkDef->kDef.sHPRecoveryInterval) );
	SetAbil(AT_HP_RECOVERY, static_cast<int>(pkDef->kDef.sHPRecovery) );
	SetAbil(AT_MAX_MP, static_cast<int>(pkDef->kDef.sMaxMP) );
	SetAbil(AT_MP, static_cast<int>(pkDef->kDef.sMaxMP) );
	SetAbil(AT_MP_RECOVERY_INTERVAL, static_cast<int>(pkDef->kDef.sMPRecoveryInterval) );
	SetAbil(AT_MP_RECOVERY, static_cast<int>(pkDef->kDef.sMPRecovery) );
	SetAbil(AT_MOVESPEED, static_cast<int>(pkDef->kDef.sMoveSpeed) );
	SetAbil(AT_IDLE_TIME, m_kLifeTime * 2);	// AI로직에서 IDLE을 LifeTime동안 유지 하기 위해 세팅
}
*/

