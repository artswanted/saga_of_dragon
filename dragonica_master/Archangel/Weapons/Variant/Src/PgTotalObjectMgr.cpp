#include "stdafx.h"
#include "PgTotalObjectMgr.h"

PgTotalObjectMgr::PgTotalObjectMgr()
	:	m_kPlayerPool(100, 100)
	,	m_kMonsterPool(50, 50)
	,	m_kNpcPool(50, 50)
	,	m_kBossPool(1, 1)
	,	m_kGndItemBoxPool(100, 100)
	,	m_kEntityPool(30, 30)
	,	m_kObjectUnitPool(20,20)
	,	m_kPetPool(30, 30)
	,	m_kSummonedPool(100, 100)
	,	m_kSubPlayerPool(100, 100)
	,	m_kCustomUnitPool(100, 20)
{
}

CUnit* PgTotalObjectMgr::CreateUnit(EUnitType const eType, BM::GUID const &rkGuid, bool* pbFind )
{//락을 잡지 마라! 절대로.
	//INFO_LOG(BM::LOG_LV8, _T("[%s] UnitType[%d], Guid[%s]"), __FUNCTIONW__, eType, rkGuid.str().c_str());
	CUnit *pkUnit = NULL;
	if ( pbFind )
	{
		BM::CAutoMutex kLock(m_kMutex);
		Cont_Unit::iterator unit_itr = m_kTotalUnit.find(rkGuid);
		if ( unit_itr != m_kTotalUnit.end() )
		{
			*pbFind = true;
			return unit_itr->second;
		}
		*pbFind = false;
	}

	switch(eType)
	{
	case UT_PLAYER:		{	pkUnit = m_kPlayerPool.New();		}break;
	case UT_MONSTER:	{	pkUnit = m_kMonsterPool.New();		}break;
	case UT_NPC:		{	pkUnit = m_kNpcPool.New();			}break;
	case UT_BOSSMONSTER:{	pkUnit = m_kBossPool.New();			}break;
	case UT_GROUNDBOX:	{	pkUnit = m_kGndItemBoxPool.New();	}break;
	case UT_ENTITY:		{	pkUnit = m_kEntityPool.New();		}break;
	case UT_OBJECT:		{	pkUnit = m_kObjectUnitPool.New();	}break;
	case UT_PET:		{	pkUnit = m_kPetPool.New();			}break;
	case UT_MYHOME:		{	pkUnit = m_kMyHomePool.New();		}break;
	case UT_SUMMONED:	{	pkUnit = m_kSummonedPool.New();		}break;
	case UT_SUB_PLAYER: {	pkUnit = m_kSubPlayerPool.New();	}break;
	case UT_CUSTOM_UNIT:{	pkUnit = m_kCustomUnitPool.New();	}break;
	}

	if(pkUnit)
	{
		pkUnit->ClearObserver();//옵저버 제거
		pkUnit->Init();
		pkUnit->SetID(rkGuid);
		return pkUnit;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Create Unit Failed") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

bool PgTotalObjectMgr::RegistUnit(CUnit* pkUnit)
{
	BM::CAutoMutex kLock(m_kMutex);

	auto itor = m_kTotalUnit.insert( std::make_pair(pkUnit->GetID(), pkUnit));

	if(itor.second)
	{
		return true;	
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Can't Regist Unit (") << pkUnit->GetID().str().c_str() << _T(")") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
#ifndef _MDo_

	CUnit* pkAlreadyExistUnit = itor.first->second;
	if (pkAlreadyExistUnit)
	{
		INFO_LOG( BM::LOG_LV1, __FL__ << _T(" ") << pkAlreadyExistUnit->GetID().str().c_str() << _T(" unit alread exist(") << pkAlreadyExistUnit->UnitType()
			<< _T(") - try to add (") << pkUnit->UnitType() << _T(")") );
	}
#endif
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgTotalObjectMgr::UnRegistUnit(CUnit* pkUnit)
{
	BM::CAutoMutex kLock(m_kMutex);

	m_kTotalUnit.erase( pkUnit->GetID() );
	return true;
}

bool PgTotalObjectMgr::ReleaseFlush()
{
	BM::CAutoMutex kLock(m_kReleaseMutex);

	RELEASE_WAIT_CONT::iterator wait_itor = m_kRelWait.begin();

	while(wait_itor != m_kRelWait.end())//유닛 Null 은 Release 단계 에서 막는다.
	{
		CUnit *pkUnit = (*wait_itor);

		//INFO_LOG(BM::LOG_LV8, _T("[%s] UnitType[%d], Guid[%s]"), __FUNCTIONW__, pkUnit->UnitType(), pkUnit->GetID().str().c_str());
		pkUnit->Init();
		//pkUnit->ClearObserver();//옵저버 제거
//	그냥 버려. 꺼내쓸때 닦아써.
//	pkUnit->Invalidate();//클리어링		switch(pkUnit->UnitType())
		switch(pkUnit->UnitType())
		{
		case UT_MYHOME:{PgMyHome *pkTemp = dynamic_cast<PgMyHome*>(pkUnit); m_kMyHomePool.Delete(pkTemp);}break;
		case UT_PLAYER:{PgPlayer *pkTemp = dynamic_cast<PgPlayer*>(pkUnit); m_kPlayerPool.Delete(pkTemp);}break;
		case UT_ENTITY:{ PgEntity* pkTemp = dynamic_cast<PgEntity*>(pkUnit); m_kEntityPool.Delete(pkTemp); }break;
		case UT_NPC:		{ PgNpc *pkTemp = dynamic_cast<PgNpc*>(pkUnit); m_kNpcPool.Delete(pkTemp); }break;
		case UT_MONSTER:	{ PgMonster *pkTemp = dynamic_cast<PgMonster*>(pkUnit); m_kMonsterPool.Delete(pkTemp); }break;
		case UT_BOSSMONSTER:{ PgBoss *pkTemp = dynamic_cast<PgBoss*>(pkUnit); m_kBossPool.Delete(pkTemp); }break;
		case UT_GROUNDBOX:	{ PgGroundItemBox* pkTemp = dynamic_cast<PgGroundItemBox*>(pkUnit); m_kGndItemBoxPool.Delete(pkTemp);}break;
		case UT_OBJECT:		{ PgObjectUnit *pkTemp = dynamic_cast<PgObjectUnit*>(pkUnit);	m_kObjectUnitPool.Delete(pkTemp);}break;
		case UT_PET:		{ PgPet *pkTemp = dynamic_cast<PgPet*>(pkUnit);	m_kPetPool.Delete(pkTemp);}break;
		case UT_SUMMONED:		{ PgSummoned *pkTemp = dynamic_cast<PgSummoned*>(pkUnit);	m_kSummonedPool.Delete(pkTemp);}break;
		case UT_SUB_PLAYER: { PgSubPlayer *pkTemp = dynamic_cast<PgSubPlayer*>(pkUnit);	 m_kSubPlayerPool.Delete(pkTemp);}break;
		case UT_CUSTOM_UNIT:{ PgCustomUnit *pkTemp = dynamic_cast<PgCustomUnit*>(pkUnit);	 m_kCustomUnitPool.Delete(pkTemp);}break;
		default:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Unknown Unit Type") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			}break;
		}
		++wait_itor;
	}
	m_kRelWait.clear();
	return true;
}

bool PgTotalObjectMgr::ReleaseUnit(CUnit* pkUnit)
{
	if(!pkUnit)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Can't Release Unit") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// 아래 Lock 걸기 전에 호출해 주어야 한다.
	// Lock 잡아주고 호출하면 DeadLock 발생할 수 있다.
	//	1. PgTotalObjectMgr::Lock
	//		CSubject::Lock
	//	2. Packet from client
	//		CUnit::CSubject::Lock
	//			CUnit::VNotify(..)
	//				Packet processing
	//					PgTotalObjectMgr::ReleaseUnit
	//						PgTotalObjectMgr::Lock
	//pkUnit->ClearObserver();

	BM::CAutoMutex kLock(m_kReleaseMutex);

	//INFO_LOG(BM::LOG_LV8, _T("[%s] UnitType[%d], Guid[%s]"), __FUNCTIONW__, pkUnit->UnitType(), pkUnit->GetID().str().c_str());

	// 여기서 전체목록에서는 절대 삭제하면 안됨
	// 전체목록 Regist를 필요에 의해서만 했기때문에 UnRegist는 따로 처리해야 한다!!!!
	//	m_kTotalUnit.erase( pkUnit->GetID() );
	m_kRelWait.insert(pkUnit);

	pkUnit = NULL;

	return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

UNIT_PTR_ARRAY::UNIT_PTR_ARRAY()
:	m_kWriteType(WT_NONE)
{
}

UNIT_PTR_ARRAY::UNIT_PTR_ARRAY( CUnit *pkUnit, bool bAutoRemove, bool bFront, bool bRef )
:	m_kWriteType(WT_NONE)
{
	Add( pkUnit, bAutoRemove, bFront, bRef );
}

UNIT_PTR_ARRAY::~UNIT_PTR_ARRAY()
{
	clear();
}

void UNIT_PTR_ARRAY::clear()
{
	UNIT_PTR_ARRAY::iterator unit_itor = begin();
	while(unit_itor != end())
	{
		if (unit_itor->bAutoRemove)
		{
			g_kTotalObjMgr.ReleaseUnit((*unit_itor).pkUnit);
		}
		++unit_itor;
	}
	std::list<UNIT_PTR_HELPER>::clear();
	m_kWriteType = WT_NONE;
}

void UNIT_PTR_ARRAY::swap( UNIT_PTR_ARRAY& rhs )
{
	std::list<UNIT_PTR_HELPER>::swap( std::list<UNIT_PTR_HELPER>(rhs) );
	std::swap( m_kWriteType, rhs.m_kWriteType );
}

bool UNIT_PTR_ARRAY::swap( UNIT_PTR_ARRAY &rhs, UNIT_PTR_ARRAY::iterator &unit_itr )
{
	if ( unit_itr != rhs.end() )
	{
		push_back(*unit_itr);
		unit_itr = rhs.OnlyErase(unit_itr);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void UNIT_PTR_ARRAY::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
{
	size_t const iWrPos = rkPacket.WrPos();
	size_t unit_count = size();
	size_t iSendCount = 0;
	rkPacket.Push(unit_count);

	UNIT_PTR_ARRAY::const_iterator unit_itr = begin();
	for( ; unit_itr != end() ; ++unit_itr )
	{
		unit_itr->pkUnit->WriteToPacket(rkPacket, kWriteType);//현재 내 상태도 뒤에 붙임.
		++iSendCount;
	}

	if (unit_count != iSendCount)
	{
		rkPacket.ModifyData(iWrPos, &iSendCount, sizeof(size_t));
	}
}

EWRITETYPE UNIT_PTR_ARRAY::ReadFromPacket( BM::Stream &rkPacket, bool bTotalObjFind )
{
	clear();//초기화 하고 시작하자

	size_t unit_count = 0;
	rkPacket.Pop(unit_count);

	bool bAutoRemove = true;

	//while(unit_count--)
	for (size_t i=0; i!=unit_count; i++)
	{//유닛으로 들어오는데. 
		size_t const rd_pos = rkPacket.RdPos();
		int lUnitType = 0;
		BM::GUID kGuid;
		rkPacket.Pop(lUnitType);	//-
		rkPacket.Pop(kGuid);	//-
		rkPacket.RdPos(rd_pos);

		CUnit *pkUnit = NULL;
		
		if (bTotalObjFind)
		{
			pkUnit = g_kTotalObjMgr.CreateUnit((EUnitType)lUnitType, kGuid, &bAutoRemove );
			bAutoRemove = !bAutoRemove;
		}
		else
		{
			pkUnit = g_kTotalObjMgr.CreateUnit((EUnitType)lUnitType, kGuid);
		}

		if( pkUnit )
		{
			m_kWriteType = pkUnit->ReadFromPacket(rkPacket);
			Add(pkUnit, bAutoRemove);
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" pkUnit is Null") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkUnit is NULL"));
			break;// 이거 ReadFrom못하면 다깨진다. 그냥 break하자..
		}
	}
	return m_kWriteType;
}

void UNIT_PTR_ARRAY::Add(CUnit* pkUnit, bool const bAutoRemove, bool const bFront, bool const bRef)
{
	if( !pkUnit )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" pkUnit is Null") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkUnit is NULL"));
		return;
	}

	UNIT_PTR_HELPER kInfo( pkUnit, bAutoRemove, bRef );
	if ( bFront )
	{
		push_front(kInfo);
	}
	else
	{
		push_back(kInfo);
	}
}

void UNIT_PTR_ARRAY::AddToRestore(CUnit* pkUnit, bool const bRestore, bool const bAutoRemove, bool const bFront, bool const bRef)
{
	if( !pkUnit )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" pkUnit is Null") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkUnit is NULL"));
		return;
	}

	UNIT_PTR_HELPER kInfo( pkUnit, bAutoRemove, bRef );
	kInfo.bRestore = bRestore;

	if ( bFront )
	{
		push_front(kInfo);
	}
	else
	{
		push_back(kInfo);
	}
}

void UNIT_PTR_ARRAY::AddToUniqe(CUnit* pkUnit, bool const bAutoRemove, bool const bFront, bool const bRef)
{
	UNIT_PTR_ARRAY::const_iterator unit_itr = std::find(begin(),end(),pkUnit);
	if(unit_itr != end())
	{
		return;
	}
	Add(pkUnit, bAutoRemove, bFront, bRef);
}

UNIT_PTR_ARRAY::iterator UNIT_PTR_ARRAY::Remove( CUnit *pkUnit )
{
	UNIT_PTR_ARRAY::iterator unit_itr = std::find(begin(),end(),pkUnit);
	return erase(unit_itr);
}

UNIT_PTR_ARRAY::iterator UNIT_PTR_ARRAY::erase(UNIT_PTR_ARRAY::iterator unit_itr)
{
	if(unit_itr==end())
	{
		return end();
	}

	if(unit_itr->bAutoRemove)
	{
		g_kTotalObjMgr.ReleaseUnit(unit_itr->pkUnit);
	}

	return std::list<UNIT_PTR_HELPER>::erase(unit_itr);
}

UNIT_PTR_ARRAY::iterator UNIT_PTR_ARRAY::OnlyErase(UNIT_PTR_ARRAY::iterator unit_itr)
{
	return std::list<UNIT_PTR_HELPER>::erase(unit_itr);
}
