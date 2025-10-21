#include "stdafx.h"
#include "NiCollision.h"
#include "PgSmallArea.h"
#include "Global.h"

PgSmallArea::PgSmallArea()
{
}

bool PgSmallArea::Init(POINT3 const &pt3MinPos, POINT3 const &pt3Size)
{
	MinPos(pt3MinPos);
	MaxPos(pt3MinPos+pt3Size);
	Size(pt3Size);
	m_kUnitCont.clear();
	m_kAdjacentArea.clear();
	m_kAdjacentArea.resize(SaDir_Max);
	return true;
}

HRESULT PgSmallArea::AddUnit(CUnit* pkUnit)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s] Area[%d], Unit[%s]"), __FUNCTIONW__, Index(), pkUnit->GetID().str().c_str());
	auto ret = m_kUnitCont.insert(std::make_pair(pkUnit->GetID(), pkUnit));
	
	pkUnit->LastAreaIndex(Index());
	if ( ret.second )
	{
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FALSE"));
	return S_FALSE;
	// 	{
// 
// /*		BM::GUID kNewGuid = pkUnit->GetID();
// 
// 		BM::Stream kBrPacket(PT_M_C_NFY_ADD_CHARACTER);
// 
// 		UNIT_PTR_ARRAY kUnitArray;
// 		kUnitArray.Add(pkUnit);
// 
// 		kUnitArray.WriteToPacket(kBrPacket, true, false);
// 
// 		Broadcast(&kBrPacket, &CCheckUnit_IgnoreOne(pkUnit->GetID()) );*/
// 
// 		//if(bIsSendAreaData)
// 		//{
// 		//	SendAreaData(pkUnit);
// 		//}
// 		return true;
// 	}
// 	// Client에 위치이동 패킷을 보내게 되면 이쪽으로 들어 올 수 있다.
// 	// 즉,
// 	//	1. 서버가 Player의 위치를 이동시키면서 AddUnit(..)을 호출한다.
// 	//	2. 클라이언트에 위치 이동 패킷을 보낸다.
// 	//	3. 클라이언트에 위치 이동 패킷이 도착하기 전에 클라이언트는 PT_C_M_REQ_ACTION2 을 보낸다.
// 	//	4. 서버는 Player를 또 다시 위치이동시키면서, 이곳에 들어오게 된다.
// 	//assert(NULL); //있는데 또 들어와??
// 	return false;
}

void PgSmallArea::RemoveUnit(BM::GUID const &rkGuid/*, bool bSend*/)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s] Area[%d], Unit[%s]"), __FUNCTIONW__, Index(), rkGuid.str().c_str());
	if (m_kUnitCont.erase(rkGuid) <= 0)
	{
		INFO_LOG(BM::LOG_LV5, __FL__<<L"Delete Failure Guid["<<rkGuid<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Delete Failed!"));
	}
	//VERIFY_INFO_LOG(m_kUnitCont.erase(rkGuid) > 0, BM::LOG_LV4, _T("[%s] Delete Failure Guid[%s]"), __FUNCTIONW__, rkGuid.str().c_str());
}

CUnit* PgSmallArea::GetUnit(BM::GUID const &rkGuid)
{
	UnitCont::iterator itor = m_kUnitCont.find(rkGuid);
	if(itor != m_kUnitCont.end())
	{
		return (*itor).second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

void PgSmallArea::Broadcast(BM::Stream *pkPacket,  BM::GUID const& rkIgnoreGuid, BYTE const bySyncTypeCheck, DWORD const dwSendFlag ) const
{
	BroadcastUtil::CONT_BROAD_LIST kContTarget;
	UnitCont::const_iterator unit_itr = m_kUnitCont.begin();
	for( ; unit_itr != m_kUnitCont.end() ; ++unit_itr )
	{
		CUnit *pkUnit = unit_itr->second;
		if( pkUnit->IsUnitType(UT_PLAYER) )
		{
			PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if(	pkPlayer )
			{
				if( rkIgnoreGuid != pkPlayer->GetID())
				{
					if( (bySyncTypeCheck & pkPlayer->GetSyncType()) == bySyncTypeCheck )
					{
						if(	(true == pkPlayer->IsReadyNetwork())
						||	(E_SENDTYPE_SEND_BYFORCE & dwSendFlag) )
						{
							BroadcastUtil::AddSwitchInfo(kContTarget, pkPlayer); // 브로드캐스트 목록에 넣음
						}
						else if( E_SENDTYPE_MUSTSEND & dwSendFlag )
						{
							pkUnit->Send( *pkPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );
						}
					}
				}
				else
				{
					bool const bIgnoreSelf = true;
					BroadcastUtil::AddSwitchInfo(kContTarget, pkPlayer, bIgnoreSelf);
				}
			}
		}
	}

	BroadcastUtil::Broadcast(kContTarget, *pkPacket);
}

void PgSmallArea::Distance(POINT3 const ptSource, POINT3& rptDistance) const
{
	POINT3 ptShort;
	if (ptSource.x >= MinPos().x && ptSource.x <= MaxPos().x)
	{
		ptShort.x = ptSource.x;
	}
	else
	{
		ptShort.x = (ptSource.x > MaxPos().x) ? MaxPos().x : MinPos().x;
	}

	if (ptSource.y >= MinPos().y && ptSource.y <= MaxPos().y)
	{
		ptShort.y = ptSource.y;
	}
	else
	{
		ptShort.y = (ptSource.y > MaxPos().y) ? MaxPos().y : MinPos().y;
	}

	if (ptSource.z >= MinPos().z && ptSource.z <= MaxPos().z)
	{
		ptShort.z = ptSource.z;
	}
	else
	{
		ptShort.z = (ptSource.z > MaxPos().z) ? MaxPos().z : MinPos().z;
	}

	rptDistance = ptShort - ptSource;
}

void PgSmallArea::GetUnitList( UNIT_PTR_ARRAY& rkArray, CUnit const *pkIgnore, EUnitType eType, BYTE const bySyncTypeCheck )
{
	BM::GUID kIgNoreGuid = ( pkIgnore ? pkIgnore->GetID() : BM::GUID::NullData() );
	GetUnitList( rkArray, kIgNoreGuid, eType, bySyncTypeCheck );
}

void PgSmallArea::GetUnitList( UNIT_PTR_ARRAY& rkArray, BM::GUID const &kIgNoreGuid, EUnitType eType, BYTE const bySyncTypeCheck )
{
	UnitCont::iterator unit_itor = m_kUnitCont.begin();
	while( unit_itor != m_kUnitCont.end() )
	{
		CUnit* pkUnit = (*unit_itor).second;
		if ( (pkUnit->GetSyncType() & bySyncTypeCheck) == bySyncTypeCheck )
		{
			if ( eType == UT_NONETYPE || pkUnit->IsUnitType(eType) )
			{
				//VERIFY_INFO_LOG_RUN(++unit_itor;continue;, VPARAM(pkUnit->GetID() != BM::GUID::NullData(), BM::LOG_LV5, _T("[%s] Invaild Unit found"), __FUNCTIONW__));
				if (pkUnit->GetID() == BM::GUID::NullData())
				{
					INFO_LOG(BM::LOG_LV5, __FL__<<L"Invaild Unit found");
					m_kUnitCont.erase(unit_itor++);
					continue;
				}

				if( kIgNoreGuid != pkUnit->Caller() )
				{
					rkArray.Add(pkUnit);
				}
			}
		}
		++unit_itor;
	}
}


void PgSmallArea::Distance(PgSmallArea const* pkArea, POINT3& rptDistance) const
{
	POINT3 ptCenter;
	pkArea->GetCenter(ptCenter);
	POINT3 ptCenter2;
	GetCenter(ptCenter2);
	
	rptDistance = ptCenter - ptCenter2;
}

void PgSmallArea::GetCenter(POINT3& rkCenter) const
{
	rkCenter = MinPos() + (MaxPos() - MinPos()) / 2.f;
#ifdef AI_DEBUG
//	INFO_LOG(BM::LOG_LV8, _T("[%s] Center[%04d, %04d, %04d], EndPos[%04d, %04d, %04d], StartPos[%04d, %04d, %04d]"), __FUNCTIONW__,
//		(int)rkCenter.x, (int)rkCenter.y, (int)rkCenter.z, (int)EndPos().x, (int)EndPos().y, (int)EndPos().z, (int)StartPos().x, (int)StartPos().y, (int)StartPos().z);
#endif
}


bool PgSmallArea::IsAdjacentArea(PgSmallArea const* pkArea) const
{
	POINT3 ptDistance;
	Distance(pkArea, ptDistance);
#ifdef AI_DEBUG
//	INFO_LOG(BM::LOG_LV8, _T("[%s] Distance [%04d, %04d, %04d]"), __FUNCTIONW__, (int)ptDistance.x, (int)ptDistance.y, (int)ptDistance.z);
#endif
	if ((abs(int(ptDistance.x)) > Size().x)
		|| (abs(int(ptDistance.y)) > Size().y)
		|| (abs(int(ptDistance.z)) > Size().z))
	{
#ifdef AI_DEBUG
//		INFO_LOG(BM::LOG_LV8, _T("\tRETURN false"));
#endif
		return false;
	}
#ifdef AI_DEBUG
//	INFO_LOG(BM::LOG_LV8, _T("\tRETURN true"));
#endif
	return true;
}

bool PgSmallArea::IsAdjacentArea(POINT3 const ptPos) const
{
	POINT3 ptDistance;
	Distance(ptPos, ptDistance);
	if ((abs(ptDistance.x) > Size().x)
		|| (abs(ptDistance.y) > Size().y)
		|| (abs(ptDistance.z) > Size().z))
	{
		return false;
	}
	return true;
}

void PgSmallArea::DisplayState()
{
	EUnitType eType = UT_NONETYPE;

	UnitCont::iterator unit_itor = m_kUnitCont.begin();

	while(unit_itor != m_kUnitCont.end())
	{
		CUnit* pkUnit = (*unit_itor).second;
		if(pkUnit->GetID() == BM::GUID::NullData())
		{
			INFO_LOG(BM::LOG_LV0, __FL__<<L"Unit["<<pkUnit->GetID()<<L"]");
		}
		++unit_itor;
	}
}

void PgSmallArea::Clear()
{
	// 아무것도 할 필요 없다...
	// 생성되면서 Init(...) 함수에서 초기화 시켜준다.
}

void PgSmallArea::SetAdjacentArea(ESmallArea_Direction eDir, PgSmallArea *pkSmallArea)
{
	m_kAdjacentArea[eDir] = pkSmallArea;
}

PgSmallArea* PgSmallArea::GetAdjacentArea(ESmallArea_Direction eDir) const
{
	return m_kAdjacentArea[eDir];
}

void PgSmallArea::Broadcast_Adjacent(BM::Stream *pkPacket, BM::GUID const& rkIgnoreGuid, BYTE const bySyncTypeCheck, DWORD const dwSendFlag ) const
{
	for (ESmallArea_Direction eDir = SaDir_Begin; eDir < SaDir_Max; eDir = ESmallArea_Direction(eDir+1))
	{
		if (m_kAdjacentArea[eDir] != NULL)
		{
			m_kAdjacentArea[eDir]->Broadcast(pkPacket, rkIgnoreGuid, bySyncTypeCheck, dwSendFlag);
		}
	}
}
