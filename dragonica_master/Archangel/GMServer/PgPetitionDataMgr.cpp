#include "StdAfx.h"
#include "Variant/GM_const.h"
#include "Variant/PgDBCache.h"
#include "PgPetitionDataMgr.h"
#include "PgGMUserMgr.h"
#include "PgGmTask.h"
#include "PgDBProcess.h"

PgPetitionDataMgr::PgPetitionDataMgr(void)
:	m_iCount(0)
{
}

PgPetitionDataMgr::~PgPetitionDataMgr(void)
{
}

void PgPetitionDataMgr::SendPetitionDataToUser(BM::GUID const &kReqGuid, unsigned short usRealmNo)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::Stream kPacket(PT_G_O_ANS_GMCOMMAND);
	kPacket.Push(kReqGuid);
	kPacket.Push(EGMC_GET_PETITIONDATA);

	if(usRealmNo > 0)
	{
		CONT_ALL_PETITION_DATA::iterator itr_data = m_kPetitionData.find(usRealmNo);
		if(itr_data == m_kPetitionData.end())
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("ServerNo: ") << usRealmNo << _T(" PetitionData empty!!") );
			return ;
		}

		CONT_PETITION_DATA kReqData;
		kReqData = itr_data->second;
		kPacket.Push(kReqData.size());

		CONT_PETITION_DATA::iterator Itr = kReqData.begin();
		while(Itr != kReqData.end())
		{
			const CONT_PETITION_DATA::value_type& rkElement = (*Itr);
			rkElement.WriteToPacket(kPacket);
			Itr++;
		}
	}
	else	//usRealmNo = 0이면 모든 서버(채널)의 접수된 진정을 보낸다.)
	{
		CONT_ALL_PETITION_DATA::iterator Itr_data = m_kPetitionData.begin();
		if(Itr_data == m_kPetitionData.end())
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("All receipt petition data is nothing!!!") );
			return ;
		}

		int iSize = 0;
		BM::Stream kPackData;

		while(Itr_data != m_kPetitionData.end())
		{
			CONT_PETITION_DATA kReqData;
			kReqData = Itr_data->second;
			CONT_PETITION_DATA::iterator Itr = kReqData.begin();
			iSize += (int)kReqData.size();
			while(Itr != kReqData.end())
			{
				const CONT_PETITION_DATA::value_type& rkElement = (*Itr);
				rkElement.WriteToPacket(kPackData);
				Itr++;
			}
			Itr_data++;
		}
		kPacket.Push(iSize);
		kPacket.Push(kPackData);
	}
	g_kGMUserMgr.SendToLogin(kReqGuid, kPacket);
}

void PgPetitionDataMgr::LoadToPetitionData(unsigned short usRealmNo) const
{
	BM::CAutoMutex kLock(m_kMutex);

//	CEL::DB_QUERY kQuery(DT_GM, DQT_SELECT_PETITION_DATA, _T("EXEC [DBO].[UP_Select_PetitionData] ?, ?"));
//	kQuery.PushParam(iMaxLoadData());
//	kQuery.PushParam(usRealmNo);


	CEL::DB_QUERY kQuery(DT_GM, DQT_SELECT_PETITION_DATA, _T("EXEC [DBO].[UP_Select_PetitionData]"));
	kQuery.PushStrParam(iMaxLoadData());
	kQuery.PushStrParam(usRealmNo);
	g_kCoreCenter.PushQuery(kQuery, true);
}

bool PgPetitionDataMgr::AddPetitionData(unsigned short usRealmNo, CONT_PETITION_DATA const &rkNewData)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_ALL_PETITION_DATA::iterator Itr = m_kPetitionData.find(usRealmNo);
	if(Itr != m_kPetitionData.end())
	{
		//DB에서 데이터를 불러올때는 항상 MAX단위로 전체를 바꾼다.(중간에 처리 완료된 데이터는 메모리 상에서만 변경
		m_kPetitionData.erase(Itr);		
	}

	auto kPair =  m_kPetitionData.insert(std::make_pair(usRealmNo, rkNewData));
	if(!kPair.second)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Petition Data Add Failed!!  RealmNo : ") << usRealmNo );
		return false;
	}
	return true;
}

void PgPetitionDataMgr::ReflashAllPetitionData()
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kPetitionData.clear();
	LoadToPetitionData();
	BroadcastPetitionData(BM::GUID::NullData());
}

void PgPetitionDataMgr::UpdatePetitionData(BM::GUID const &kReqGuid, SPetitionData const &UpdateData)
{
	if(UpdateData.m_PetitionId == BM::GUID::NullData())
	{
		INFO_LOG( BM::LOG_LV0, _T("UpdateData is NULL") );
		return ;
	}

	BM::CAutoMutex kLock(m_kMutex);
	if(m_kPetitionData.empty())
	{
		INFO_LOG( BM::LOG_LV0, _T("All server`s petition data is nothing!!!") );
		return;
	} 

	CONT_ALL_PETITION_DATA::iterator Itr_data = m_kPetitionData.begin();
	while(Itr_data != m_kPetitionData.end())
	{
		CONT_PETITION_DATA* kPetitionData;
		kPetitionData = &(Itr_data->second);

		CONT_PETITION_DATA::iterator Itr = kPetitionData->begin();
		while(Itr != kPetitionData->end())
		{
			if( Itr->m_PetitionId == UpdateData.m_PetitionId)
			{
				//변경 될 수 있는 데이터는 다음과 같다.
				Itr->m_State		= UpdateData.m_State;
				Itr->m_Kind			= UpdateData.m_Kind;
				Itr->m_GmId			= UpdateData.m_GmId;
				Itr->m_Answerkind	= UpdateData.m_Answerkind;
				Itr->m_dtStartTime	= UpdateData.m_dtStartTime;
				Itr->m_dtEndTime	= UpdateData.m_dtEndTime;

//				CEL::DB_QUERY kQuery( DT_GM, DQT_MODIFY_PETITION_DATA, _T("EXEC [dbo].[UP_Modify_Petition_Data] ?, ?, ?, ?, ?, ?, ?"));
//				kQuery.QueryOwner(kReqGuid);
//				kQuery.PushParam(UpdateData.m_PetitionId);
//				kQuery.PushParam(UpdateData.m_GmId);
//				kQuery.PushParam(UpdateData.m_State);
//				kQuery.PushParam(UpdateData.m_Kind);
//				kQuery.PushParam(UpdateData.m_Answerkind);
//				kQuery.PushParam(UpdateData.m_dtStartTime);
//				kQuery.PushParam(UpdateData.m_dtEndTime);


				CEL::DB_QUERY kQuery( DT_GM, DQT_MODIFY_PETITION_DATA, _T("EXEC [dbo].[UP_Modify_Petition_Data]"));
				kQuery.QueryOwner(kReqGuid);
				kQuery.PushStrParam(UpdateData.m_PetitionId);
				kQuery.PushStrParam(UpdateData.m_GmId);
				kQuery.PushStrParam(UpdateData.m_State);
				kQuery.PushStrParam(UpdateData.m_Kind);
				kQuery.PushStrParam(UpdateData.m_Answerkind);
				kQuery.PushStrParam(UpdateData.m_dtStartTime);
				kQuery.PushStrParam(UpdateData.m_dtEndTime);
				g_kCoreCenter.PushQuery(kQuery, true);
			}
			Itr++;
		}
		Itr_data++;
	}
}

void PgPetitionDataMgr::BroadcastPetitionData(const BM::GUID &kPetitionGuid, unsigned short usRealmNo)
{
	BM::CAutoMutex kLock(m_kMutex);

	if(m_kPetitionData.size())
	{
		BM::Stream kPacket(PT_G_O_ANS_GMCOMMAND);
		{
			if(usRealmNo > 0)
			{
				kPacket.Push(EGMC_MODIFY_PETITIONDATA);
				CONT_ALL_PETITION_DATA::iterator Itr = m_kPetitionData.find(usRealmNo);
				if(Itr != m_kPetitionData.end())
				{
					CONT_ALL_PETITION_DATA::mapped_type const &kElement = Itr->second;
					CONT_PETITION_DATA::const_iterator Itr_Data = kElement.begin();
					if(BM::GUID::IsNull(kPetitionGuid))
					{
						kPacket.Push(kElement.size());		
						while(Itr_Data != kElement.end())
						{
							CONT_PETITION_DATA::value_type const &kData = (*Itr_Data);
							kData.WriteToPacket(kPacket);
							++Itr_Data;
						}
					}
					else
					{
						kPacket.Push(1);
						while(Itr_Data != kElement.end())
						{
							CONT_PETITION_DATA::value_type const &kData = (*Itr_Data);
							if(kData.m_PetitionId == kPetitionGuid)
								kData.WriteToPacket(kPacket);
							++Itr_Data;
						}
					}
				}
			}
			else
			{
				kPacket.Push(EGMC_GET_PETITIONDATA);
				CONT_ALL_PETITION_DATA::iterator Itr_data = m_kPetitionData.begin();
				while(Itr_data != m_kPetitionData.end())
				{
					CONT_PETITION_DATA kReqData = Itr_data->second;
					kPacket.Push(kReqData.size());

					CONT_PETITION_DATA::iterator Itr = kReqData.begin();
					while(Itr != kReqData.end())
					{
						const CONT_PETITION_DATA::value_type& rkElement = (*Itr);
						rkElement.WriteToPacket(kPacket);
						Itr++;
					}
					++Itr_data;
				}
			}
		}
		g_kGMUserMgr.BroadCast(kPacket);
	}
}

///////////////////////////////////////////////Petition Process//////////////////////////////////////////////////////
HRESULT PgPetitionDataMgr::ReceiptPetition(BM::Stream * const pkPacket)
{
	unsigned short usRealm = 0;
	unsigned short usChannel = 0;
	BM::GUID kMemberId;
	std::wstring kName;
	std::wstring kTitle;
	std::wstring kNote;

	pkPacket->Pop(usRealm);
	pkPacket->Pop(usChannel);
	pkPacket->Pop(kMemberId);
	pkPacket->Pop(kName);
	pkPacket->Pop(kTitle);
	pkPacket->Pop(kNote);
/*
	CEL::DB_QUERY kQuery(DT_GM, DQT_RECEIPT_PETITION, L"EXEC [dbo].[UP_Receipt_Petition] ?, ?, ?, ?, ?, ?" );

	kQuery.PushParam(kMemberId);
	kQuery.PushParam(kName);
	kQuery.PushParam(usRealm);
	kQuery.PushParam(usChannel);
	kQuery.PushParam(kTitle);
	kQuery.PushParam(kNote);
*/
	CEL::DB_QUERY kQuery(DT_GM, DQT_RECEIPT_PETITION, L"EXEC [dbo].[UP_Receipt_Petition]" );

	kQuery.PushStrParam(kMemberId);
	kQuery.PushStrParam(kName);
	kQuery.PushStrParam(usRealm);
	kQuery.PushStrParam(usChannel);
	kQuery.PushStrParam(kTitle);
	kQuery.PushStrParam(kNote);

	return g_kCoreCenter.PushQuery(kQuery);
}

HRESULT PgPetitionDataMgr::RemainderPetition(unsigned short const usRealmNo, BM::GUID const &MemberGuid)
{
//	INFO_LOG(BM::LOG_LV6, _T("[%s] Select Remainder Petition"), __FUNCTIONW__);
/*
	CEL::DB_QUERY kQuery(DT_GM, DQT_REMAINDER_PETITION, L"EXEC [dbo].[UP_Select_Remainder_Petition] ?, ?" );
	kQuery.PushParam(usRealmNo);
	kQuery.PushParam(MemberGuid);
*/
	CEL::DB_QUERY kQuery(DT_GM, DQT_REMAINDER_PETITION, L"EXEC [dbo].[UP_Select_Remainder_Petition]" );
	kQuery.PushStrParam(usRealmNo);
	kQuery.PushStrParam(MemberGuid);

	return g_kCoreCenter.PushQuery(kQuery, true);
} 

HRESULT PgPetitionDataMgr::Select_PetitionState(BM::Stream * const pkPacket)
{
	unsigned short usRealmNo = 0;
	unsigned short usChannelNo = 0;
	BM::GUID kMemberGuid;
	pkPacket->Pop(usRealmNo);
	pkPacket->Pop(usChannelNo);
	pkPacket->Pop(kMemberGuid);

	INFO_LOG( BM::LOG_LV6, __FL__ << _T("Select PetitionState") );
/*
	CEL::DB_QUERY kQuery(DT_GM, DQT_SELECT_PETITION_STATE, L"EXEC [dbo].[UP_Select_Petition_State] ?, ?" );
	kQuery.PushParam(usRealmNo);
	kQuery.PushParam(kMemberGuid);
*/

	CEL::DB_QUERY kQuery(DT_GM, DQT_SELECT_PETITION_STATE, L"EXEC [dbo].[UP_Select_Petition_State]" );
	kQuery.PushStrParam(usRealmNo);
	kQuery.PushStrParam(kMemberGuid);
	return g_kCoreCenter.PushQuery(kQuery, true);
} 


//////////////////////////////////////////////////////////////////////////////////////////////////////////////