#include "stdafx.h"
#include <math.h>

#include "PgStoneMgr.h"

#include "PgAction.h"
#include "PgActionAchievement.h"

//스톤 매니저에서
// 그라운드에서 먹을 수 있는 샤인스톤 개수를 정함.
// 그걸로 빌드를 함.
// 먹을때 마다 업데이트를 돌려서 아이템 갯수 갱신.

// PgPlayerStone
PgStoneControl::PgStoneControl()
{
	int i = 0;
	while(MAX_STONE_TICKET > i)
	{
		m_aiRemainTicketCount[i] = 0;
		++i;
	}
	m_dwTickTime= BM::GetTime32();
}

PgStoneControl::~PgStoneControl()
{
}

void PgStoneControl::Init(TBL_DEF_MAP_STONE_CONTROL const &kTbl)
{
	m_kTblData = kTbl;
}

void PgStoneControl::OnTick()
{
	if(BM::TimeCheck(m_dwTickTime, m_kTblData.iPeriod))//iPeriod는 여러개로 만들 수가 없다.
	{//After Period
		int i = 0; 
		while(MAX_STONE_TICKET > i)
		{
			m_aiRemainTicketCount[i] = 	m_kTblData.aStoneCtrl[i].iTicketCount;
			++i;
		}
	}

	m_kContCustomer.clear();
}

bool PgStoneControl::AddData(SStoneRegenPoint const &kStoneRegenPoint)
{//From XML.
	m_kStoneRegenPoint.push_back(kStoneRegenPoint);
	return true;
}

bool PgStoneControl::TryCatchStoneTicket(CUnit *pkPlayer)
{
	if( !pkPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_CUSTOMER::const_iterator customer_itor = m_kContCustomer.find(pkPlayer->GetID());
	if(customer_itor != m_kContCustomer.end())
	{// Already Customer
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

//티켓 목록에서 뽑는다.
	int i = 0;
	int iTotalTickets = 0;
	while(MAX_STONE_TICKET >i)
	{
		iTotalTickets += m_aiRemainTicketCount[i];
		++i;
	}

	int const iRandResult = BM::Rand_Range(iTotalTickets);
	iTotalTickets = 0;
	
	i= 0;
	bool bIsSuccess = false;
	while(MAX_STONE_TICKET >i)
	{
		iTotalTickets += m_aiRemainTicketCount[i];
		if(iTotalTickets < iRandResult)
		{
			bIsSuccess = true;
			break;
		}
		++i;
	}

	if(bIsSuccess)
	{
		if(0 < m_aiRemainTicketCount[i])
		{
			--m_aiRemainTicketCount[i];

			CONT_STONE_REGEN_POINT kTempPoint = m_kStoneRegenPoint;
			std::random_shuffle(kTempPoint.begin(), kTempPoint.end(), RandomLib::RandomCanonical<RandomLib::SRandomGenerator64>());//섞은다음
			kTempPoint.resize(m_kTblData.aStoneCtrl[i].iStoneCount);

			m_kContCustomer.insert(pkPlayer->GetID());//고객명단으로 등록.

			auto ret = m_kContReservedStone.insert(std::make_pair(pkPlayer->GetID(), CONT_RESERVED_STONE_LIST::mapped_type()));
			
			(*ret.first).second.clear();//기존꺼 있다면 지워.
			CONT_STONE_REGEN_POINT::const_iterator point_itor = kTempPoint.begin();
			while(point_itor != kTempPoint.end())
			{
				CONT_RESERVED_STONE_LIST::mapped_type &rkElement = (*ret.first).second;

				rkElement.insert(std::make_pair(BM::GUID::Create(), point_itor->kPos));
				++point_itor;
			}
		}
	}

	return true;
}

bool PgStoneControl::WriteToPacket(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)const
{
	rkPacket.Push(CONT_RESERVED_STONE_LIST::mapped_type());
/*
	CONT_RESERVED_STONE_LIST::const_iterator my_stone_itor = m_kContReservedStone.find(rkCharGuid);
 
	if(my_stone_itor != m_kContReservedStone.end())
	{
		rkPacket.Push((*my_stone_itor).second);
	}
	else
	{
		rkPacket.Push(CONT_RESERVED_STONE_LIST::mapped_type());
	}
*/	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgStoneControl::PlayerStoneUpdate(CUnit *pkUnit, SGroundKey const & rkGndKey, BM::GUID const &rkStoneGuid)
{//샤인스톤 먹는부분.
	CONT_RESERVED_STONE_LIST::iterator resv_stone_itor = m_kContReservedStone.find(pkUnit->GetID());

	bool bIsSuccess = false;
	int iItemNo = 0;
	if(resv_stone_itor != m_kContReservedStone.end())//Find at Tickets
	{
		CONT_RESERVED_STONE_LIST::mapped_type::iterator my_stone_itor = (*resv_stone_itor).second.find(rkStoneGuid);
		
		if(my_stone_itor != (*resv_stone_itor).second.end())
		{
			(*resv_stone_itor).second.erase(rkStoneGuid);
			bIsSuccess = true;
			//float const fStoneLimitDist = pow(85.f, 2);// 1.f == 1/50 캐릭터 길이 (1.f == 1 inch)
			//-> 거리 체크를 해야되는데 고속이동시 먹으면 문제가 될 수 있어서, 거리체크를 못함.
			//-> 해킹 방어를 위해서는 체크를 해야함.
		}
	}

	if(bIsSuccess)
	{
		int const iFiveElement = pkUnit->GetAbil(AT_FIVE_ELEMENT_TYPE_AT_BODY);

		CONT_FIVE_ELEMENT_INFO const * pContDefFiveElementInfo = NULL;
		g_kTblDataMgr.GetContDef(pContDefFiveElementInfo);
		
		CONT_FIVE_ELEMENT_INFO::const_iterator five_itor;
		RandomElement(*pContDefFiveElementInfo, five_itor);
//		CONT_FIVE_ELEMENT_INFO::const_iterator five_itor = pContDefFiveElementInfo->find(iFiveElement);
		if(five_itor != pContDefFiveElementInfo->end())
		{
			iItemNo = (*five_itor).second.iCrystalStoneNo;
		}


// 		PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
// 		if ( pkPlayer )
// 		{
// 			CONT_PLAYER_MODIFY_ORDER kOrder;
// 			SPMOD_AddRankPoint kAddRank(E_RANKPOINT_CRYSTALSTONE,1);// 랭킹 올려.
// 			kOrder.push_back(SPMO(IMET_ADD_RANK_POINT, pkUnit->GetID(), kAddRank));
// 
// 			PgAction_ReqModifyItem kItemModifyAction(CIE_Rank_Point, rkGndKey, kOrder);
// 			kItemModifyAction.DoAction(pkUnit, NULL);
// 		}
	}

	if(iItemNo)
	{
		CONT_PLAYER_MODIFY_ORDER kOrder;

		tagPlayerModifyOrderData_Add_Any kAddData(iItemNo,1);
		SPMO kIMO(IMET_ADD_ANY, pkUnit->GetID(), kAddData);
		kOrder.push_back(kIMO);

		PgAction_ReqModifyItem kAddAction(CIE_ShineStone, rkGndKey, kOrder);
		kAddAction.DoAction(pkUnit, NULL);

		PgAddAchievementValue kMA(AT_ACHIEVEMENT_CONQUEROR,1,rkGndKey);
		kMA.DoAction(pkUnit, NULL);
	}

	//먹든 못먹든 사라져야 하므로. 아래 리턴 처리 없음.
	BM::Stream kPacket(PT_M_C_NFY_SHINESTONE_MSG);//사라져라

	kPacket.Push(rkStoneGuid);
	kPacket.Push(iItemNo);
	pkUnit->Send(kPacket);

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgStoneControl::RemoveTicket(CUnit *pkPlayer)
{
	m_kContCustomer.erase(pkPlayer->GetID());
	m_kContReservedStone.erase(pkPlayer->GetID());
	return true;
}