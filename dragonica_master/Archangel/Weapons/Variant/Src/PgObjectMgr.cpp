#include "stdafx.h"
#include "PgObjectMgr.h"

PgObjectMgr::PgObjectMgr()
{
}

PgObjectMgr::~PgObjectMgr()
{
	InitObjectMgr();
}

bool PgObjectMgr::GetUnitContainer(EUnitType const eType, CONT_OBJECT_MGR_UNIT *&pkOutWorkingCont)
{
	pkOutWorkingCont = NULL;
	switch(eType)
	{
	case UT_PLAYER:
		{	pkOutWorkingCont = &m_kPlayerCont;	}break;
//	case UT_NPC:
//		{	pkOutWorkingCont = &m_kNpcCont;	}break;
	case UT_PET:
		{	pkOutWorkingCont = &m_kPetCont;	}break;
	case UT_GROUNDBOX:
		{	pkOutWorkingCont = &m_kGndItemBoxCont;	}break;
	case UT_BOSSMONSTER:
	case UT_MONSTER:
		{	pkOutWorkingCont = &m_kMonsterCont;	}break;
	case UT_ENTITY:
		{	pkOutWorkingCont = &m_kEntityCont;	}break;
	case UT_OBJECT:
		{	pkOutWorkingCont = &m_kObjectUnitCont; }break;
	case UT_MYHOME:
		{	pkOutWorkingCont = &m_kMyHomeCont;	}break;
	case UT_SUMMONED:
		{	pkOutWorkingCont = &m_kSummonedCont;}break;
	case UT_SUB_PLAYER:
		{	pkOutWorkingCont = &m_kSubPlayerCont;	}break;
	case UT_CUSTOM_UNIT:
		{	pkOutWorkingCont = &m_kCustomUnitCont; }break;
	default: 
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" Unknown UnitType[") << eType << _T("]") );
			pkOutWorkingCont = NULL;
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}
	return true;
}

bool PgObjectMgr::GetUnitContainer(EUnitType const eType, CONT_OBJECT_MGR_UNIT const *&pkOutWorkingCont)const
{
	pkOutWorkingCont = NULL;
	switch(eType)
	{
	case UT_PLAYER:
		{	pkOutWorkingCont = &m_kPlayerCont;	}break;
//	case UT_NPC:
//		{	pkOutWorkingCont = &m_kNpcCont;	}break;
	case UT_PET:
		{	pkOutWorkingCont = &m_kPetCont;	}break;
	case UT_GROUNDBOX:
		{	pkOutWorkingCont = &m_kGndItemBoxCont;	}break;
	case UT_BOSSMONSTER:
	case UT_MONSTER:
		{	pkOutWorkingCont = &m_kMonsterCont;	}break;
	case UT_ENTITY:
		{	pkOutWorkingCont = &m_kEntityCont;	}break;
	case UT_OBJECT:
		{	pkOutWorkingCont = &m_kObjectUnitCont; }break;
	case UT_MYHOME:
		{	pkOutWorkingCont = &m_kMyHomeCont;	}break;
	case UT_SUMMONED:
		{	pkOutWorkingCont = &m_kSummonedCont;}break;
	case UT_SUB_PLAYER:
		{	pkOutWorkingCont = &m_kSubPlayerCont;	}break;
	case UT_CUSTOM_UNIT:
		{	pkOutWorkingCont = &m_kCustomUnitCont; }break;
	default: 
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" Unknown UnitType[") << eType << _T("]") );
			pkOutWorkingCont = NULL;
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}
	return true;
}

void PgObjectMgr::InitObjectMgr()
{
	// UT_NPC : 삭제하면 안됨..GroundResource에서 항상 존재하고 있음..
	// UT_FURNITURE : 미구현.. 구현되면 추가해야 함.
	EUnitType eUnitType[] = { UT_PLAYER, /*UT_NPC,*/ UT_PET, UT_GROUNDBOX, UT_MONSTER, UT_ENTITY, UT_OBJECT, UT_SUMMONED, UT_SUB_PLAYER,/*UT_FURNITURE,*/ UT_CUSTOM_UNIT, UT_NONETYPE, UT_MYHOME };

	int iIndex = 0;
	while (eUnitType[iIndex] != UT_NONETYPE)
	{
		CONT_OBJECT_MGR_UNIT *pkWorkingCont = NULL;
		if( GetUnitContainer( eUnitType[iIndex], pkWorkingCont) )
		{
			CONT_OBJECT_MGR_UNIT::iterator itr = pkWorkingCont->begin();
			for ( ; itr!=pkWorkingCont->end() ; ++itr )
			{
				g_kTotalObjMgr.ReleaseUnit( itr->second );
			}
			pkWorkingCont->clear();
		}
		++iIndex;
	}
}

bool PgObjectMgr::RegistUnit( EUnitType const eType, BM::GUID const &kAddID, CUnit* pkUnit )
{
	CONT_OBJECT_MGR_UNIT *pkWorkingCont = NULL;
	if( GetUnitContainer(eType, pkWorkingCont) )
	{
		auto ret = pkWorkingCont->insert( std::make_pair( kAddID, pkUnit ) );
		if(!ret.second)
		{
			UnRegistUnit( pkUnit );

			VERIFY_INFO_LOG( false, BM::LOG_LV2, __FL__<<L"Can't Insert UnitType["<<eType<<L"] Already Using Guid["<<pkUnit->GetID()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		return true;
	}

	INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Unknown unit type. [") << eType << _T("]") );
	ASSERT_LOG(false, BM::LOG_LV2, _T("PgObjectMgr::AddUnit Unknown unit type"));
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgObjectMgr::RegistUnit( CUnit* pkUnit )
{
	if(!pkUnit)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" unit in null") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return RegistUnit( pkUnit->UnitType(), pkUnit->GetID(), pkUnit );
}


CUnit* PgObjectMgr::GetUnit(EUnitType const eUnitType, BM::GUID const &rkGuid)
{
	CONT_OBJECT_MGR_UNIT *pkWorkingCont = NULL;

	if(GetUnitContainer(eUnitType, pkWorkingCont))
	{
		CONT_OBJECT_MGR_UNIT::iterator itor = pkWorkingCont->find(rkGuid);

		if(itor != pkWorkingCont->end())
		{
			return (*itor).second;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

CUnit* PgObjectMgr::GetUnit(EUnitType const eUnitType, BM::GUID const &rkGuid) const
{
	CONT_OBJECT_MGR_UNIT const* pkWorkingCont = NULL;

	if(GetUnitContainer(eUnitType, pkWorkingCont))
	{
		CONT_OBJECT_MGR_UNIT::const_iterator itor = pkWorkingCont->find(rkGuid);

		if(itor != pkWorkingCont->end())
		{
			return (*itor).second;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}


CUnit* PgObjectMgr::GetUnit(BM::GUID const &rkGuid)
{//xxx 유닛 타입 늘어나면 여기 고쳐줘야됩니다.
	CUnit* pkUnit = NULL;

	if(!pkUnit){	pkUnit = GetUnit(UT_PLAYER, rkGuid);	}
	if(!pkUnit){	pkUnit = GetUnit(UT_MONSTER, rkGuid);	}
	if(!pkUnit){	pkUnit = GetUnit(UT_GROUNDBOX, rkGuid);	}
	if(!pkUnit){	pkUnit = GetUnit(UT_PET, rkGuid);		}
//	if(!pkUnit){	pkUnit = GetUnit(UT_NPC, rkGuid);		}
	if(!pkUnit){	pkUnit = GetUnit(UT_ENTITY,	rkGuid);	}
	if(!pkUnit){	pkUnit = GetUnit(UT_OBJECT, rkGuid);	}
//	if(!pkUnit){	pkUnit = GetUnit(UT_DEADBODY, rkGuid;)	}//DeadBody는 여기에서 찾지 않게 하자.(중요함)
	if(!pkUnit){	pkUnit = GetUnit(UT_SUMMONED, rkGuid);	}	//UT_MYHOME보다는 적중확률 높음
	if(!pkUnit){	pkUnit = GetUnit(UT_MYHOME, rkGuid);	}
	if(!pkUnit){	pkUnit = GetUnit(UT_SUB_PLAYER, rkGuid);}
	if(!pkUnit){	pkUnit = GetUnit(UT_CUSTOM_UNIT, rkGuid);}
	return pkUnit;
}

CUnit* PgObjectMgr::GetUnit(BM::GUID const &rkGuid) const
{//xxx 유닛 타입 늘어나면 여기 고쳐줘야됩니다.
	CUnit* pkUnit = NULL;

	if(!pkUnit){	pkUnit = GetUnit(UT_PLAYER, rkGuid);	}
	if(!pkUnit){	pkUnit = GetUnit(UT_MONSTER, rkGuid);	}
	if(!pkUnit){	pkUnit = GetUnit(UT_GROUNDBOX, rkGuid);	}
	if(!pkUnit){	pkUnit = GetUnit(UT_PET, rkGuid);		}
//	if(!pkUnit){	pkUnit = GetUnit(UT_NPC, rkGuid);		}
	if(!pkUnit){	pkUnit = GetUnit(UT_ENTITY,	rkGuid);	}
	if(!pkUnit){	pkUnit = GetUnit(UT_OBJECT, rkGuid);	}
//	if(!pkUnit){	pkUnit = GetUnit(UT_DEADBODY, rkGuid;)	}//DeadBody는 여기에서 찾지 않게 하자.(중요함)
	if(!pkUnit){	pkUnit = GetUnit(UT_SUMMONED, rkGuid);	}	//UT_MYHOME보다는 적중확률 높음
	if(!pkUnit){	pkUnit = GetUnit(UT_MYHOME, rkGuid);	}
	if(!pkUnit){	pkUnit = GetUnit(UT_SUB_PLAYER, rkGuid);}
	if(!pkUnit){	pkUnit = GetUnit(UT_CUSTOM_UNIT, rkGuid);}
	return pkUnit;
}

bool PgObjectMgr::UnRegistUnit( eUnitType const eType, BM::GUID const &kGuid )
{
	CONT_OBJECT_MGR_UNIT *pkWorkingCont = NULL;
	if( GetUnitContainer( eType, pkWorkingCont) )
	{
		if( !pkWorkingCont->erase(kGuid) )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Can't Erase Unit Type[") << eType << _T("][") << kGuid.str().c_str() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgObjectMgr::UnRegistUnit(CUnit* pkUnit)
{
	if(!pkUnit)
	{
		assert(NULL && "PgObjectMgr::UnRegistUnit pkUnit == NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return UnRegistUnit( pkUnit->UnitType(), pkUnit->GetID() );
}

bool PgObjectMgr::UnRegistAllUnit(EUnitType const eType)
{
	CONT_OBJECT_MGR_UNIT *pkWorkingCont = NULL;
	if( GetUnitContainer(eType, pkWorkingCont) )
	{
		pkWorkingCont->clear();
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

size_t PgObjectMgr::GetUnitCount(EUnitType const eUnitType)const
{
	CONT_OBJECT_MGR_UNIT const *pkWorkingCont = NULL;
	if(GetUnitContainer(eUnitType, pkWorkingCont))
	{
		return pkWorkingCont->size();
	}

	INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Unknown unit type. [") << eUnitType << _T("]Type???") );
	assert(NULL && "PgObjectMgr::GetUnitCount Unknown unit type");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

void PgObjectMgr::GetFirstUnit(EUnitType const eType, CONT_OBJECT_MGR_UNIT::iterator& rkItor)
{
	CONT_OBJECT_MGR_UNIT *pkWorkingCont = NULL;
	if(GetUnitContainer(eType, pkWorkingCont))
	{
		rkItor = pkWorkingCont->begin();
	}
}

void PgObjectMgr::GetFirstUnit(EUnitType const eType, CONT_OBJECT_MGR_UNIT::const_iterator& rkItor) const
{
	CONT_OBJECT_MGR_UNIT const* pkWorkingCont = NULL;
	if(GetUnitContainer(eType, pkWorkingCont))
	{
		rkItor = pkWorkingCont->begin();
	}
}

CUnit* PgObjectMgr::GetNextUnit(EUnitType const eType, CONT_OBJECT_MGR_UNIT::iterator& rkItor)
{
	CUnit* pkUnit = NULL;

	CONT_OBJECT_MGR_UNIT *pkWorkingCont = NULL;
	if( !GetUnitContainer(eType, pkWorkingCont))
	{
		INFO_LOG(BM::LOG_LV7, __FL__<<L"Cannot GetUnitContainer UnitType["<<eType<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	while(rkItor != pkWorkingCont->end() 
	&& pkUnit == NULL)
	{
		if (rkItor->second->IsUnitType(eType))
		{
			pkUnit = rkItor->second;
		}
		++rkItor;
	}
	return pkUnit;
}

CUnit* PgObjectMgr::GetNextUnit(EUnitType const eType, CONT_OBJECT_MGR_UNIT::const_iterator& rkItor) const
{
	CONT_OBJECT_MGR_UNIT const* pkWorkingCont = NULL;
	if( !GetUnitContainer(eType, pkWorkingCont))
	{
		INFO_LOG(BM::LOG_LV7, __FL__<<L"Cannot GetUnitContainer UnitType["<<eType<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	while( rkItor != pkWorkingCont->end() )
	{
		if ( rkItor->second->IsUnitType(eType) )
		{
			return rkItor->second;
		}
		++rkItor;
	}

	return NULL;
}
