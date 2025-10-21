#include "stdafx.h"
#include "BM/PgTask.h"
#include "PgPartyItemRule.h"
#include "PgExpeditionItemRule.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgExpedition.h"
#include "Variant/PgExpeditionMgr.h"
#include "PgLocalPartyMgr.h"

PgLocalPartyMgrImpl::PgLocalPartyMgrImpl()
	: PgPartyMgr::T_MY_BASE_MGR_TYPE(), m_kItemRulePool(100, 100), m_kPartyItemRule()
{
}

PgLocalPartyMgrImpl::~PgLocalPartyMgrImpl()
{
	ContPartyItemRule::iterator itr = m_kPartyItemRule.begin();
	for ( ; itr!=m_kPartyItemRule.end() ; ++itr )
	{
		m_kItemRulePool.Delete( itr->second );
	}
	m_kPartyItemRule.clear();
}

void PgLocalPartyMgrImpl::Clear()
{
	T_MY_BASE_MGR_TYPE::Clear();
	ContPartyItemRule::iterator itr = m_kPartyItemRule.begin();
	for ( ; itr!=m_kPartyItemRule.end() ; ++itr )
	{
		m_kItemRulePool.Delete( itr->second );
	}
	m_kPartyItemRule.clear();
}

bool PgLocalPartyMgrImpl::WriteToPacketPartyName(BM::GUID const& rkPartyGuid, BM::Stream& rkPacket)
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return WriteToPacketPartyName(pkParty, rkPacket);
}

typedef struct tagFunctionFindGroundPartyForEach : public PgLocalPartyMgrImpl::SFunctionForEach
{
	tagFunctionFindGroundPartyForEach(BM::Stream& rkPacket)
		: m_rkPacket(rkPacket)
	{
	}

	virtual void operator() (PgLocalPartyMgrImpl::PartyHash const& rkContParty)
	{
		size_t const iRetWrPos = m_rkPacket.WrPos();
		m_rkPacket.Push( static_cast< BYTE >(PRC_Fail_NoMaster) ); // 없는걸 기본으로
		size_t const iCountWrPos = m_rkPacket.WrPos();
		int iCount = 0;
		m_rkPacket.Push( iCount );

		PgLocalPartyMgrImpl::PartyHash::const_iterator iter = rkContParty.begin();
		while( rkContParty.end() != iter )
		{
			PgLocalPartyMgrImpl::PartyHash::mapped_type const pkParty = iter->second;
			if( pkParty )
			{
				if( pkParty->Option().GetOptionPublicTitle() == POT_Public
				&&	pkParty->MaxMemberCount() > pkParty->MemberCount() )
				{
					m_rkPacket.Push( pkParty->PartyGuid() );
					m_rkPacket.Push( pkParty->PartyName() );
					m_rkPacket.Push( static_cast< BYTE >(pkParty->MemberCount()) );
					m_rkPacket.Push( static_cast< BYTE >(pkParty->MaxMemberCount()) );
					m_rkPacket.Push( pkParty->MasterCharGuid() );
					m_rkPacket.Push( pkParty->Option().iPartyOption );
					m_rkPacket.Push( pkParty->Option().iPartyLevel );
					m_rkPacket.Push( pkParty->Option().PartySubName() );
					m_rkPacket.Push( pkParty->Option().iPartyAttribute );
					m_rkPacket.Push( pkParty->Option().iPartyContinent );
					m_rkPacket.Push( pkParty->Option().iPartyArea_NameNo );

					++iCount;
					if( PV_MAX_LIST_CNT <= iCount )
					{
						break;
					}
				}
			}

			++iter;
		}

		if( 0 < iCount )
		{
			BYTE const ucRet = PRC_Success;
			m_rkPacket.ModifyData(iRetWrPos, &ucRet, sizeof(ucRet));
			m_rkPacket.ModifyData(iCountWrPos, &iCount, sizeof(iCount));
		}
	}
private:
	BM::Stream& m_rkPacket;
} SFunctionFindGroundPartyForEach;

typedef struct tagFunctionFindGroundPartyForEach_PartyGuid : public PgLocalPartyMgrImpl::SFunctionForEach
{
	tagFunctionFindGroundPartyForEach_PartyGuid(VEC_GUID & kCont)
		: m_rkCont(kCont)
	{
	}

	virtual void operator() (PgLocalPartyMgrImpl::PartyHash const& rkContParty)
	{
		PgLocalPartyMgrImpl::PartyHash::const_iterator iter = rkContParty.begin();
		while( rkContParty.end() != iter )
		{
			PgLocalPartyMgrImpl::PartyHash::mapped_type const pkParty = iter->second;
			if( pkParty )
			{
				m_rkCont.push_back(pkParty->PartyGuid());
			}
			++iter;
		}
	}
private:
	VEC_GUID & m_rkCont;
} SFunctionFindGroundPartyForEach_PartyGuid;

void PgLocalPartyMgrImpl::WriteToPacketPartyList(BM::Stream& rkPacket){
	ForEach( SFunctionFindGroundPartyForEach(rkPacket) );
}

bool PgLocalPartyMgrImpl::SyncFromContents(BM::GUID const& rkPartyGuid, BM::Stream& rkPacket)
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);

	if( !pkParty )
	{
		SPartyOption kPartyOption;
		kPartyOption.Clear();
		HRESULT const hResult = NewParty(rkPartyGuid, pkParty, kPartyOption);//널 파티 생성
		if( PRC_Success != hResult )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false; // Update 없음
		}
	}

	size_t const iPrevMemberCount = pkParty->MemberCount();
	
	pkParty->ReadFromPacket(rkPacket);//Sync

	size_t const iMemberCount = pkParty->MemberCount();
	if( !iMemberCount && !pkParty->AllowEmpty() )//맴버가 없으면
	{
		Delete(rkPartyGuid);//파티 삭제
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false; // Update 없음
	}

	SyncPartyItemRule(pkParty);//Item Rule Sync

	if( pkParty->AllowEmpty() )
	{
		return true;
	}

	if( iPrevMemberCount == iMemberCount )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false; // Update 없음
	}

	return true; // Update 있음
}

bool PgLocalPartyMgrImpl::SyncFromContentsExpedition(BM::GUID const& rkExpeditionGuid, BM::Stream& rkPacket)
{
	PgLocalExpedition* pkExpedition = GetExpedition(rkExpeditionGuid);

	int iCount = 0;
	VEC_GUID PartyGuidList;
	rkPacket.Pop(iCount);
	for(int i = 0 ; i < iCount ; ++i)
	{
		BM::GUID PartyGuid;
		rkPacket.Pop(PartyGuid);
		PartyGuidList.push_back(PartyGuid);
	}

	if( !pkExpedition )
	{
		SExpeditionOption kOption;
		kOption.Clear();
		HRESULT const hResult = NewExpedition(rkExpeditionGuid, pkExpedition, kOption);//빈 원정대 생성
		if( PRC_Success != hResult )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false; // Update 없음
		}

		if( NULL != pkExpedition )
		{
			pkExpedition->SetPartyHash( &m_kContPartyHash );
			// 로컬파티가 삭제되는 경우가 있음( 원정대 탈퇴할때 비어있는 파티는 로컬에서 자동으로 삭제됨. )
			SPartyOption PartyOption;
			for(int iMaxPartyCount = 0 ; iMaxPartyCount < iCount ; ++iMaxPartyCount)
			{
				PartyOption.Default();
				PgLocalParty* pLocalParty = GetParty(PartyGuidList[iMaxPartyCount]);
				if( NULL == pLocalParty )
				{
					// 파티가 없음. 파티가 없으면 만들어서 넣음.
					HRESULT hRet = NewParty(PartyGuidList[iMaxPartyCount], pLocalParty, PartyOption);
					if( PRC_Success != hRet )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
						return false; // Update 없음
					}
				}
				if( !pkExpedition->AddParty(pLocalParty) )
				{
					// 원정대에 파티를 넣지 못함.
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					DeleteExpedition(rkExpeditionGuid);
					return false; // Update 없음
				}

				pLocalParty->ExpeditionGuid(rkExpeditionGuid);	// 파티에 원정대 guid 정보 넣음.
			}
		}
	}

	size_t const iPrevPartyCount = pkExpedition->GetExpeditionMemberCount();

	// 패킷으로부터 원정대 정보를 채운다.
	pkExpedition->ReadFromPacket(rkPacket);//Sync

	size_t const iPartyCount = pkExpedition->GetExpeditionMemberCount();
	if( !iPartyCount )
	{
		DeleteExpedition(rkExpeditionGuid);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false; // Update 없음
	}

	SyncExpeditionItemRule(pkExpedition);

	if( iPrevPartyCount == iPartyCount )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false; // Update 없음
	}

	return true; // Update 있음
}

bool PgLocalPartyMgrImpl::Delete(BM::GUID const& rkPartyGuid)
{
	//Local에서만 삭제된다.
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;//이서버에 없다
	}

	DelItemRule(rkPartyGuid);//룰 삭제
	PgPartyMgr::Delete(rkPartyGuid);
	return true;
}

bool PgLocalPartyMgrImpl::Leave(SGroundKey const& rkGndKey, BM::Stream& rkPacket)
{
	BM::GUID kPartyGuid;
	BM::GUID kLeaverGuid;
	bool bChangeMaster = false;
	BM::GUID kNewMasterGuid;

	rkPacket.Pop(kPartyGuid);
	rkPacket.Pop(kLeaverGuid);
	rkPacket.Pop(bChangeMaster);
	if( bChangeMaster )
	{
		rkPacket.Pop(kNewMasterGuid);
	}

	PgLocalParty* pkParty = GetParty(kPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPartyItemRule* pkItemRule = NULL;
	bool const bFindRule = GetItemRule(kPartyGuid, pkItemRule);
	if( bFindRule )
	{
		pkItemRule->Del(rkGndKey, kLeaverGuid);
	}
	assert(bFindRule);

	SPartyUserInfo kLeaverInfo;//삭제 유저
	bool const bFindUser = pkParty->GetMemberInfo(kLeaverGuid, kLeaverInfo);
	if( !bFindUser )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	//
	HRESULT const hResult = pkParty->Del(kLeaverGuid, kNewMasterGuid);

	if( true == pkParty->Expedition() )	// 원정대에 속한 파티면 삭제하지 않음.
	{
		return true;
	}

	//
	if( PRC_Success_Destroy == hResult ) // 파티가 삭제되면 모든 파티원들의 Guid를 초기화 한다
	{
		Delete(kPartyGuid);
	}
	return true;
}

bool PgLocalPartyMgrImpl::ChangeMaster(BM::GUID const& rkPartyGuid, BM::GUID const& rkNewMasterGuid)
{
	assert(GUID_NULL != rkPartyGuid);

	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	//BM::GUID rkOldPartyMasterGuid;
	//GetPartyMasterGuid(rkPartyGuid, rkOldPartyMasterGuid);

	bool const bChanged = pkParty->ChangeMaster(rkNewMasterGuid);
	if( !bChanged )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

bool PgLocalPartyMgrImpl::Rename(BM::GUID const& rkPartyGuid, std::wstring const& rkNewPartyName,  SPartyOption const& rkNewPartyOption)
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( rkNewPartyName == pkParty->PartyName() )
	{
		if( rkPartyGuid != pkParty->PartyGuid()  )
			return true;
	}

	pkParty->PartyName(rkNewPartyName);

	pkParty->Option(rkNewPartyOption);
	return true;
}

bool PgLocalPartyMgrImpl::WriteToPacketPartyName(PgLocalParty* pkParty, BM::Stream& rkPacket)
{
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	rkPacket.Clear();

	rkPacket.Push((BM::Stream::DEF_STREAM_TYPE)PT_M_C_ANS_PARTY_NAME);
	rkPacket.Push(pkParty->PartyGuid());
	rkPacket.Push(pkParty->PartyName());
	rkPacket.Push((BYTE)pkParty->MemberCount());
	rkPacket.Push((BYTE)pkParty->MaxMemberCount());
	rkPacket.Push(pkParty->MasterCharGuid());
	pkParty->Option().WriteToPacket(rkPacket);
	return true;
}

bool PgLocalPartyMgrImpl::MapMoved(BM::GUID const& rkPartyGuid, SGroundKey const& rkCurGndKey, SGroundKey const& rkTrgGndKey, VEC_GUID const& kVec)
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPartyItemRule* pkItemRule = NULL;
	bool const bFindItemRule = GetItemRule(rkPartyGuid, pkItemRule);

	SChnGroundKey const kTrgChnGndKey( rkTrgGndKey, g_kProcessCfg.ChannelNo() );

	VEC_GUID::const_iterator guid_iter = kVec.begin();
	while(kVec.end() != guid_iter)
	{
		SPartyUserInfo* pkMember = NULL;
		VEC_GUID::value_type const& rkCharGuid = (*guid_iter);
		bool const bFindMember = pkParty->Get(rkCharGuid, pkMember);
		if( bFindMember )
		{
			SGroundKey const kOldGndKey = static_cast<SGroundKey>(pkMember->kChnGndKey);
			bool const bMovedGnd = pkParty->MovedGnd( rkCharGuid, kTrgChnGndKey );
			if( bMovedGnd
			&&	bFindItemRule )
			{
				pkItemRule->MapMove(kOldGndKey, rkTrgGndKey, rkCharGuid);
			}
		}
		++guid_iter;
	}

	bool const bDeleteParty = IsRemainMemberInServer( pkParty, rkCurGndKey );//관리하는 파티유저가 없으면 파티정보 삭제
	if( bDeleteParty )
	{
		Delete(rkPartyGuid);//

		CleanUpPartyList(pkParty);
	}
	return true;
}

bool PgLocalPartyMgrImpl::IsRemainMemberInServer( PgLocalParty const *pkParty, SGroundKey const& rkGndKey )const
{
	SChnGroundKey const kChnGndKey( rkGndKey, g_kProcessCfg.ChannelNo() );

	VEC_GUID kVec;
	pkParty->GetMemberCharGuidList(kVec);

	VEC_GUID::iterator member_iter = kVec.begin();
	while(kVec.end() != member_iter)
	{
		SPartyUserInfo kMemberInfo;
		bool const bFindMember = pkParty->GetMemberInfo((*member_iter), kMemberInfo);
		if( bFindMember )//맴버를 찾았으면
		{
			if( kChnGndKey == kMemberInfo.kChnGndKey )//이 유저의 Gnd를 서버가 관리하면
			{
				return false;//삭제 하지 마라
			}
		}
		++member_iter;
	}

	return true;//삭제 해라
}

bool PgLocalPartyMgrImpl::UpdateProperty(BM::GUID const& rkPartyGuid, BM::Stream& rkPacket)
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::GUID kCharGuid;
	BYTE cPropertyType = 0;

	rkPacket.Pop(kCharGuid);
	rkPacket.Pop(cPropertyType);
	
	BYTE const cChangeAbil = pkParty->ChangeAbility(kCharGuid, cPropertyType, rkPacket);
	if( 0 != cChangeAbil )
	{
		BM::Stream kPacket(PT_M_N_NFY_PARTY_USER_PROPERTY);
		kPacket.Push( kCharGuid );
		if( pkParty->WriteToChangedAbil(kCharGuid, cChangeAbil, kPacket) )
		{
			// 공용맵에서는  어쩌냐.
			SendToGlobalPartyMgr(kPacket);
		}
	}

	return true;
}

bool PgLocalPartyMgrImpl::IsMaster(BM::GUID const& rkPartyGuid, BM::GUID const& rkCharGuid)const
{
	if( rkPartyGuid == BM::GUID::NullData() )
	{
		return false;
	}

	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		return false;
	}
	return pkParty->IsMaster(rkCharGuid);
}

bool PgLocalPartyMgrImpl::GetPartyMasterGuid(BM::GUID const& rkPartyGuid, BM::GUID& rkOutGuid)const
{
	if( rkPartyGuid == BM::GUID::NullData() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	rkOutGuid = pkParty->MasterCharGuid();
	return true;
}


bool PgLocalPartyMgrImpl::ProcessMsg(unsigned short const usType, SGroundKey const& rkGndKey, BM::Stream* pkMsg)
{
	if( !pkMsg )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	switch( usType )
	{
	case PT_A_T_UPDATE_PARTY_PROPERTY:
		{
			BM::GUID kPartyGuid;
			pkMsg->Pop(kPartyGuid);

			UpdateProperty(kPartyGuid, *pkMsg);
		}break;
	case PT_N_M_NFY_PARTY_USER_MAP_MOVE://맵 이동 하면
		{
			BM::GUID kPartyGuid;
			BM::GUID kCharGuid;
			SGroundKey kTrgGndKey;

			pkMsg->Pop(kPartyGuid);
			pkMsg->Pop(kTrgGndKey);
			pkMsg->Pop(kCharGuid);

			VEC_GUID kVec;
			kVec.push_back(kCharGuid);
			MapMoved(kPartyGuid, rkGndKey, kTrgGndKey, kVec);
		}break;
	case PT_N_M_NFY_LEAVE_PARTY_USER:
		{
			Leave(rkGndKey, *pkMsg);
		}break;
	case PT_N_M_NFY_PARTY_CHANGE_MASTER:
		{
			BM::GUID kPartyGuid;
			BM::GUID kNewMasterGuid;

			pkMsg->Pop(kPartyGuid);
			pkMsg->Pop(kNewMasterGuid);

			ChangeMaster(kPartyGuid, kNewMasterGuid);
		}break;
	case PT_N_M_NFY_DELETE_PARTY:
		{
			BM::GUID kPartyGuid;

			pkMsg->Pop(kPartyGuid);

			Delete(kPartyGuid);
		}break;
	case PT_N_M_NFY_PARTY_RENAME:
		{
			BM::GUID kPartyGuid;
			std::wstring kNewPartName;
			SPartyOption kNewOption;

			pkMsg->Pop(kPartyGuid);
			pkMsg->Pop(kNewPartName);
			kNewOption.ReadFromPacket(*pkMsg);

			Rename(kPartyGuid, kNewPartName, kNewOption);
		}break;
	case PT_N_M_NFY_DELETE_EXPEDITION:
		{
			BM::GUID ExpeditionGuid;

			pkMsg->Pop(ExpeditionGuid);

			PgLocalExpedition* pExpedition = GetExpedition(ExpeditionGuid);
			if( pExpedition )
			{
				CleanUpExpedition(pExpedition);
			}
		}break;
	case PT_N_M_NFY_CHANGEMASTER_EXPEDITION:
		{
			BM::GUID ExpeditionGuid, NewMaster;

			pkMsg->Pop(ExpeditionGuid);
			pkMsg->Pop(NewMaster);

			ChangeExpeditionMaster(ExpeditionGuid, NewMaster);
		}break;
	case PT_N_M_NFY_RENAME_EXPEDITION:
		{
			BM::GUID ExpeditionGuid;
			std::wstring NewName;
			SExpeditionOption NewOption;

			pkMsg->Pop(ExpeditionGuid);
			pkMsg->Pop(NewName);
			NewOption.ReadFromPacket(*pkMsg);
			
			RenameExpedition(ExpeditionGuid, NewName, NewOption);
		}break;
	case PT_N_M_NFY_LEAVE_EXPEDITION:
		{
			LeaveExpedition(rkGndKey, *pkMsg);
		}break;
	case PT_N_M_NFY_DISPERSE_EXPEDITION:
		{
			// 할거 없음.
		}break;
	case PT_N_M_NFY_EXPEDITION_USER_MAP_MOVE:
		{
			BM::GUID ExpeditionGuid;
			BM::GUID CharGuid;
			SGroundKey TrgGndKey;

			pkMsg->Pop(ExpeditionGuid);
			pkMsg->Pop(CharGuid);
			pkMsg->Pop(TrgGndKey);

			VEC_GUID Vec_List;
			Vec_List.push_back(CharGuid);

			ExpeditionMapMoved(ExpeditionGuid, rkGndKey, TrgGndKey, Vec_List);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"invalid packet type ["<<usType<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	return true;
}

bool PgLocalPartyMgrImpl::GetPartyMemberGround(BM::GUID const& rkPartyGuid, SGroundKey const& rkGndKey, VEC_GUID& rkOutVec, BM::GUID const& rkIgnore)const
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SChnGroundKey kChnGndKey( rkGndKey, g_kProcessCfg.ChannelNo() );
	return pkParty->GetLocalMemberList( kChnGndKey, rkOutVec, rkIgnore);
}

size_t PgLocalPartyMgrImpl::GetMemberCount(BM::GUID const& rkPartyGuid)const
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1"));
		return 1;//파티가 없으면 파티원은 1명(나자신)
	}
	return pkParty->MemberCount();
}

size_t PgLocalPartyMgrImpl::GetMaxMemberCount(BM::GUID const& rkPartyGuid)const
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1"));
		return 1;//파티가 없으면 파티원은 1명(나자신)
	}
	return pkParty->MaxMemberCount();
}

std::wstring const& PgLocalPartyMgrImpl::GetPartyName(BM::GUID const& rkPartyGuid)const
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1"));
		return L"";//파티가 없으면 파티원은 1명(나자신)
	}
	return pkParty->PartyName();
}

std::wstring const& PgLocalPartyMgrImpl::GetPartySubName(BM::GUID const& rkPartyGuid)const
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1"));
		return L"";//파티가 없으면 파티원은 1명(나자신)
	}
	return pkParty->Option().GetOptionPartySubName();
}

bool PgLocalPartyMgrImpl::GetTotalLevel(BM::GUID const& rkPartyGuid, SGroundKey const& rkGndKey, unsigned int& iOutTotalLevel)
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;//파티가 없으니 총 레벨은 -_-;
	}

	SChnGroundKey kChnGndKey( rkGndKey, g_kProcessCfg.ChannelNo() );
	return pkParty->GetGndTotalLevel(kChnGndKey, iOutTotalLevel);
}

bool PgLocalPartyMgrImpl::GetPartyOption(BM::GUID const& rkPartyGuid, SPartyOption& rkOut)
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;//실패
	}

	rkOut = pkParty->Option();
	return true;//성공
}

bool PgLocalPartyMgrImpl::GetPartyShareItem_NextOwner(BM::GUID const& rkPartyGuid, SGroundKey const& rkGndKey, BM::GUID& rkNextOwner)
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;//실패
	}

	PgPartyItemRule* pkItemRule = NULL;
	bool bFindRule = GetItemRule(rkPartyGuid, pkItemRule);
	if( !bFindRule )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool const bFindNext = pkItemRule->GetNext(rkGndKey, rkNextOwner);
	return bFindNext;
}

bool PgLocalPartyMgrImpl::GetPartyMaxLevel(BM::GUID const& rkPartyGuid, int& iLevel) const
{
	iLevel = 0;
	if (rkPartyGuid == BM::GUID::NullData())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if (!pkParty)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	VEC_GUID kPartyMembers;
	pkParty->GetMemberCharGuidList(kPartyMembers);
	unsigned short iLv = 0;
	for (VEC_GUID::const_iterator it = kPartyMembers.begin(); it != kPartyMembers.end(); ++it)
	{
		BM::GUID kGuid = *it;
		SPartyUserInfo kPartyUserInfo;
		if (pkParty->GetMemberInfo(kGuid, kPartyUserInfo))
		{
			iLv = std::max(iLv, kPartyUserInfo.sLevel);
		}
	}

	iLevel = static_cast<int>(iLv);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return true"));
	return true;
}

bool PgLocalPartyMgrImpl::AddItemRule(BM::GUID const& rkPartyGuid, PgPartyItemRule*& pkOut)//파티 아이템 습득 룰을 만든다
{
	PgPartyItemRule* pkNewRule = m_kItemRulePool.New();
	if( !pkNewRule )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	auto kRet = m_kPartyItemRule.insert( std::make_pair(rkPartyGuid, pkNewRule) );
	if( !kRet.second )
	{
		m_kItemRulePool.Delete(pkNewRule);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	pkOut = pkNewRule;
	return true;
}

bool PgLocalPartyMgrImpl::GetItemRule(BM::GUID const& rkPartyGuid, PgPartyItemRule*& pkOut)
{
	ContPartyItemRule::iterator rule_iter = m_kPartyItemRule.find(rkPartyGuid);
	if( m_kPartyItemRule.end() != rule_iter)
	{
		pkOut = (*rule_iter).second;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return true;
	}
	return false;
}

bool PgLocalPartyMgrImpl::DelItemRule(BM::GUID const& rkPartyGuid)
{
	PgPartyItemRule* pkItemRule = NULL;
	bool const bFindRule = GetItemRule(rkPartyGuid, pkItemRule);
	if( bFindRule )
	{
		pkItemRule->Clear();

		m_kPartyItemRule.erase(rkPartyGuid);
		m_kItemRulePool.Delete(pkItemRule);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgLocalPartyMgrImpl::SyncPartyItemRule(PgLocalParty* pkParty)
{
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::GUID const& rkPartyGuid = pkParty->PartyGuid();

	//Item룰 생성
	PgPartyItemRule* pkItemRule = NULL;
	bool const bFindRuleRet = GetItemRule(rkPartyGuid, pkItemRule);
	if( !bFindRuleRet )
	{
		bool const bNewRuleRet = AddItemRule(rkPartyGuid, pkItemRule);
		if( !bNewRuleRet )
		{
			INFO_LOG(BM::LOG_LV0, __FL__<<L"Can't make party item rule");	
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	VEC_GUID kVec;
	pkParty->GetMemberCharGuidList(kVec);

	ContPartyMemberGndKey kSet;
	VEC_GUID::const_iterator guid_iter = kVec.begin();
	while(kVec.end() != guid_iter)
	{
		VEC_GUID::value_type const& rkCharGuid = (*guid_iter);
		SPartyUserInfo* pkMember = NULL;
		bool const bFindMember = pkParty->Get(rkCharGuid, pkMember);
		if( bFindMember )
		{
			if ( g_kProcessCfg.ChannelNo() == pkMember->kChnGndKey.Channel() )
			{
				SGroundKey const kGndKey = static_cast<SGroundKey>(pkMember->kChnGndKey);
				pkItemRule->Add( kGndKey, pkMember->kCharGuid);//무조건 추가

				SPartyMemberGndKey kMemberGndKey( kGndKey, pkMember->kCharGuid );
				auto kRet = kSet.insert( kMemberGndKey );
				if( !kRet.second )
				{
					//
				}
			}
		}
		++guid_iter;
	}
	//이후에 없는것 삭제

	pkItemRule->Sync(kSet);

	return true;
}

bool PgLocalPartyMgrImpl::GetPartyMember(BM::GUID const& rkPartyGuid, VEC_GUID& rkOut)const
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pkParty->GetMemberCharGuidList(rkOut);
	return true;
}

void PgLocalPartyMgrImpl::GetContPartyGuid(VEC_GUID & rkCont)const
{
	ForEach( SFunctionFindGroundPartyForEach_PartyGuid(rkCont) );
}

int PgLocalPartyMgrImpl::GetPartyMemberFriend(BM::GUID const& rkPartyGuid, BM::GUID const& rkCharGuid)
{
	PgLocalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	return pkParty->GetLocalMemberFriend(rkCharGuid);
}

////////////////////////////////////////////////////////////////////////////////////////
// 원정대

bool PgLocalPartyMgrImpl::GetExpeditionMemberGround(BM::GUID const& ExpeditionGuid, SGroundKey const & GndKey, VEC_GUID & OutVec, BM::GUID const & Ignore) const
{
	PgLocalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SChnGroundKey kChnGndKey( GndKey, g_kProcessCfg.ChannelNo() );
	return pExpedition->GetLocalMemberList( kChnGndKey, OutVec, Ignore);
}

size_t PgLocalPartyMgrImpl::GetExpeditionMemberCount(BM::GUID const & ExpeditionGuid) const
{
	PgLocalExpedition* pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1"));
		return 1;//원정대가 없으면 멤버는 1명(나자신)
	}
	return pExpedition->GetExpeditionMemberCount();
}

bool PgLocalPartyMgrImpl::GetExpeditionMasterGuid(BM::GUID const & ExpeditionGuid, BM::GUID & OutGuid) const
{
	if( ExpeditionGuid == BM::GUID::NullData() )
	{
		return false;
	}

	PgLocalExpedition* pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	OutGuid = pExpedition->MasterCharGuid();
	return true;
}

bool PgLocalPartyMgrImpl::WriteToPacketExpeditionName(BM::GUID const & ExpeditionGuid, BM::Stream & Packet)
{
	PgLocalExpedition* pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return WriteToPacketExpeditionName(pExpedition, Packet);
}

bool PgLocalPartyMgrImpl::WriteToPacketExpeditionName(PgLocalExpedition* pExpedition, BM::Stream & Packet)
{
	if( !pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	Packet.Clear();

	Packet.Push((BM::Stream::DEF_STREAM_TYPE)PT_N_C_ANS_INFO_EXPEDITION);
	Packet.Push(pExpedition->ExpeditionGuid());
	Packet.Push(pExpedition->ExpeditionName());
	Packet.Push((BYTE)pExpedition->GetExpeditionMemberCount());
	Packet.Push((BYTE)pExpedition->GetMaxMemberCount());
	Packet.Push(pExpedition->MasterCharGuid());
	pExpedition->Option().WriteToPacket(Packet);

	return true;
}

PgLocalParty* PgLocalPartyMgrImpl::GetLocalParty(BM::GUID const & PartyGuid)
{
	return GetParty(PartyGuid);
}

PgLocalExpedition* PgLocalPartyMgrImpl::GetLocalExpedition(BM::GUID const & ExpeditionGuid) const
{
	return GetExpedition(ExpeditionGuid);
}

bool PgLocalPartyMgrImpl::ChangeExpeditionMaster(BM::GUID const & ExpeditionGuid, BM::GUID const & NewMasterGuid)
{
	PgLocalExpedition* pExpedition = GetExpedition(ExpeditionGuid);
	if( !pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool const bChanged = pExpedition->ChangeMaster(NewMasterGuid);
	if( !bChanged )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

bool PgLocalPartyMgrImpl::RenameExpedition(BM::GUID const & ExpeditionGuid, std::wstring const & NewName, SExpeditionOption const & NewOption)
{
	PgLocalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( NewName == pExpedition->ExpeditionName() )
	{
		if( ExpeditionGuid != pExpedition->ExpeditionGuid() )
		{
			return true;
		}
	}

	pExpedition->ExpeditionName(NewName);
	
	pExpedition->Option(NewOption);

	return true;
}

bool PgLocalPartyMgrImpl::LeaveExpedition(SGroundKey const & GndKey, BM::Stream & Packet)
{
	BM::GUID ExpeditionGuid, LeaverGuid;

	Packet.Pop(ExpeditionGuid);
	Packet.Pop(LeaverGuid);

	PgLocalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgExpeditionItemRule * pItemRule = NULL;
	bool const FindRule = GetExpeditionItemRule(ExpeditionGuid, pItemRule);
	if( FindRule )
	{
		pItemRule->Del(GndKey, LeaverGuid);
	}
	assert(FindRule);

	SPartyUserInfo LeaverInfo;
	bool const bFindUser = pExpedition->GetMemberInfo(LeaverGuid, LeaverInfo);
	if( !bFindUser )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pExpedition->DelExpeditionMemberInfo(LeaverGuid);

	return true;
}

bool PgLocalPartyMgrImpl::IsExpeditionMaster(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid)
{
	PgLocalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( false == pExpedition->IsMaster(CharGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

bool PgLocalPartyMgrImpl::ExpeditionMapMoved(BM::GUID const & ExpeditionGuid, SGroundKey const & CurGndKey, SGroundKey const & TrgGndKey, VEC_GUID const & Vec)
{
	PgLocalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgExpeditionItemRule * pItemRule = NULL;
	bool const FindItemRule = GetExpeditionItemRule(ExpeditionGuid, pItemRule);

	SChnGroundKey const TrgChnGndKey(TrgGndKey, g_kProcessCfg.ChannelNo());

	VEC_GUID::const_iterator guid_iter = Vec.begin();
	while( guid_iter != Vec.end() )
	{
		SPartyUserInfo Member;
		VEC_GUID::value_type const & CharGuid = (*guid_iter);
		bool const FindMember = pExpedition->GetMemberInfo(CharGuid, Member);
		if( FindMember )
		{
			SGroundKey const OldGndKey = static_cast<SGroundKey>(Member.kChnGndKey);
			if( FindItemRule )
			{
				pItemRule->MapMove(OldGndKey, TrgGndKey, CharGuid);
			}
		}
		++guid_iter;
	}

	bool const bDeleteExpedition = IsRemainExpeditionMemberInServer( pExpedition, CurGndKey ); //관리하는 원정대유저가 없으면 파티정보 삭제
	if( bDeleteExpedition )
	{
		VEC_GUID PartyList;
		pExpedition->GetPartyList(PartyList);

		VEC_GUID::iterator iter = PartyList.begin();
		for( iter ; iter != PartyList.end() ; ++iter)
		{
			PgLocalParty * pParty = GetParty( *iter );
			if( NULL != pParty )
			{
				Delete(pParty->PartyGuid());
				CleanUpPartyList(pParty);
			}
		}

		DeleteExpedition(ExpeditionGuid);
	}
	return true;
}

bool PgLocalPartyMgrImpl::IsRemainExpeditionMemberInServer( PgLocalExpedition const * pExpedition, SGroundKey const & GndKey ) const
{
	SChnGroundKey const ChnGndKey( GndKey, g_kProcessCfg.ChannelNo() );

	VEC_GUID Vec;
	pExpedition->GetMemberCharGuidList(Vec);

	VEC_GUID::iterator member_iter = Vec.begin();
	while(Vec.end() != member_iter)
	{
		SPartyUserInfo MemberInfo;
		bool const bFindMember = pExpedition->GetMemberInfo((*member_iter), MemberInfo);
		if( bFindMember )//맴버를 찾았으면
		{
			if( ChnGndKey == MemberInfo.kChnGndKey )//이 유저의 Gnd를 서버가 관리하면
			{
				return false;//삭제 하지 마라
			}
		}
		++member_iter;
	}

	return true;//삭제 해라
}

bool PgLocalPartyMgrImpl::CleanUpPartyList(PgLocalParty * pParty)
{
	if( NULL == pParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::GUID const & ExpeditionGuid = pParty->ExpeditionGuid();

	PgLocalExpedition* pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pExpedition->CleanUpPartyList(pParty);
	return true;
}

void PgLocalPartyMgrImpl::CleanUpExpedition(PgLocalExpedition * pExpedition)
{
	VEC_GUID PartyList;
	pExpedition->GetPartyList(PartyList);

	VEC_GUID::iterator itr = PartyList.begin();
	while( itr != PartyList.end() )
	{
		PgLocalParty * pParty = GetParty( *itr );
		if( NULL != pParty )
		{
			if( 0 == pParty->MemberCount() )
			{
				Delete( pParty->PartyGuid() );
			}
		}
		++itr;
	}

	DelExpeditionItemRule(pExpedition->ExpeditionGuid());

	DeleteExpedition(pExpedition->ExpeditionGuid());
}

HRESULT PgLocalPartyMgrImpl::IsStartable(BM::GUID const & ExpeditionGuid)
{
	PgLocalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}
	return pExpedition->IsStartable();
}

bool PgLocalPartyMgrImpl::GetExpeditionMember(BM::GUID const & ExpeditionGuid, VEC_GUID & Out)
{
	PgLocalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( !pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pExpedition->GetMemberCharGuidList(Out);
	return true;
}

BM::GUID const & PgLocalPartyMgrImpl::GetExpeditionNpcGuid(BM::GUID const & ExpeditionGuid)
{
	PgLocalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( !pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return BM::GUID::NullData()"));
		return BM::GUID::NullData();
	}

	return pExpedition->ExpeditionNpc();
}

int PgLocalPartyMgrImpl::GetMemberTeamNo(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid)
{
	PgLocalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( !pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return pExpedition->GetMemberTeamNo(CharGuid);
}

int PgLocalPartyMgrImpl::GetMaxTeamCount(BM::GUID const & ExpeditionGuid)
{
	PgLocalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( !pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return pExpedition->MaxTeamCount();
}

bool PgLocalPartyMgrImpl::AddExpeditionItemRule(BM::GUID const & ExpeditionGuid, PgExpeditionItemRule *& pOut)	// 원정대 아아템 습득 룰을 만든다.
{
	PgExpeditionItemRule * pNewRule = m_ExpeditionItemRulePool.New();
	if( !pNewRule )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	auto Ret = m_ExpeditionItemRule.insert( std::make_pair(ExpeditionGuid, pNewRule) );
	if( !Ret.second )
	{
		m_ExpeditionItemRulePool.Delete(pNewRule);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pOut = pNewRule;
	return true;
}

bool PgLocalPartyMgrImpl::GetExpeditionItemRule(BM::GUID const & ExpeditionGuid, PgExpeditionItemRule *& pOut)
{
	ContExpeditionItemRule::iterator rule_iter = m_ExpeditionItemRule.find(ExpeditionGuid);
	if( rule_iter != m_ExpeditionItemRule.end() )
	{
		pOut = (*rule_iter).second;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgLocalPartyMgrImpl::DelExpeditionItemRule(BM::GUID const & ExpeditionGuid)
{
	PgExpeditionItemRule * pItemRule = NULL;
	bool const FindRule = GetExpeditionItemRule(ExpeditionGuid, pItemRule);
	if( FindRule )
	{
		pItemRule->Clear();

		m_ExpeditionItemRule.erase(ExpeditionGuid);
		m_ExpeditionItemRulePool.Delete(pItemRule);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgLocalPartyMgrImpl::SyncExpeditionItemRule(PgLocalExpedition * pExpedition)
{
	if( !pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::GUID const & ExpeditionGuid = pExpedition->ExpeditionGuid();

	// Item룰 생성.
	PgExpeditionItemRule * pItemRule = NULL;
	bool const FindRuleRet = GetExpeditionItemRule(ExpeditionGuid, pItemRule);
	if( !FindRuleRet )
	{
		bool const NewRuleRet = AddExpeditionItemRule(ExpeditionGuid, pItemRule);
		if( !NewRuleRet )
		{
			INFO_LOG(BM::LOG_LV0, __FL__ << L"Cant't make expedition item rule");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << L"Return false");
			return false;
		}
	}

	VEC_GUID Vec;
	pExpedition->GetMemberCharGuidList(Vec);

	ContExpeditionMemberGndKey Set;
	VEC_GUID::const_iterator guid_iter = Vec.begin();
	while( guid_iter != Vec.end() )
	{
		VEC_GUID::value_type const & CharGuid = (*guid_iter);
		SPartyUserInfo Member;
		bool const FindMember = pExpedition->GetMemberInfo(CharGuid, Member);
		if( FindMember )
		{
			if( g_kProcessCfg.ChannelNo() == Member.kChnGndKey.Channel() )
			{
				SGroundKey const GndKey = static_cast<SGroundKey>(Member.kChnGndKey);
				pItemRule->Add(GndKey, Member.kCharGuid); // 무조건 추가.

				SExpeditionMemberGndKey MemberGndKey(GndKey, Member.kCharGuid);
				auto Ret = Set.insert(MemberGndKey);
				if( !Ret.second )
				{
					//
				}
			}
		}
		++guid_iter;
	}

	pItemRule->Sync(Set);

	return true;
}

bool PgLocalPartyMgrImpl::GetExpeditionShareItem_NextOwner(BM::GUID const ExpeditionGuid, SGroundKey const & GndKey, BM::GUID & NextOwner)
{
	PgLocalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( !pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << L"Return false");
		return false;
	}

	PgExpeditionItemRule * pItemRule = NULL;
	bool FindRule = GetExpeditionItemRule(ExpeditionGuid, pItemRule);
	if( !FindRule )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << L"Return false");
		return false;
	}

	bool const FindNext = pItemRule->GetNext(GndKey, NextOwner);
	return FindNext;
}

bool PgLocalPartyMgrImpl::GetExpeditionOption(BM::GUID const & ExpeditionGuid, SExpeditionOption & Out)
{
	PgLocalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( !pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << L"Return false");
		return false;
	}

	Out = pExpedition->Option();
	return true;
}

//
//
PgLocalPartyMgr::PgLocalPartyMgr()
{
}
PgLocalPartyMgr::~PgLocalPartyMgr()
{
}

void PgLocalPartyMgr::Clear()
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	Instance()->Clear();
}
bool PgLocalPartyMgr::GetPartyMemberGround(BM::GUID const& rkPartyGuid, SGroundKey const& rkGndKey, VEC_GUID& rkOutVec, BM::GUID const& rkIgnore)const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetPartyMemberGround(rkPartyGuid, rkGndKey, rkOutVec, rkIgnore);
}
size_t PgLocalPartyMgr::GetMemberCount(BM::GUID const& rkPartyGuid)const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetMemberCount(rkPartyGuid);
}
size_t PgLocalPartyMgr::GetMaxMemberCount(BM::GUID const& rkPartyGuid)const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetMaxMemberCount(rkPartyGuid);
}
std::wstring const& PgLocalPartyMgr::GetPartyName(BM::GUID const& rkPartyGuid)const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetPartyName(rkPartyGuid);
}
std::wstring const& PgLocalPartyMgr::GetPartySubName(BM::GUID const& rkPartyGuid)const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetPartySubName(rkPartyGuid);
}
bool PgLocalPartyMgr::GetTotalLevel(BM::GUID const& rkPartyGuid, SGroundKey const& rkGndKey, unsigned int& iOutTotalLevel)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetTotalLevel(rkPartyGuid, rkGndKey, iOutTotalLevel);
}
bool PgLocalPartyMgr::GetPartyMember(BM::GUID const& rkPartyGuid, VEC_GUID& rkOut)const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetPartyMember(rkPartyGuid, rkOut);
}
void PgLocalPartyMgr::GetContPartyGuid(VEC_GUID & rkCont)const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetContPartyGuid(rkCont);
}
bool PgLocalPartyMgr::IsMaster(BM::GUID const& rkPartyGuid, BM::GUID const& rkCharGuid)const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->IsMaster(rkPartyGuid, rkCharGuid);
}
bool PgLocalPartyMgr::GetPartyMasterGuid(BM::GUID const& rkPartyGuid, BM::GUID& rkOutGuid)const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetPartyMasterGuid(rkPartyGuid, rkOutGuid);
}
bool PgLocalPartyMgr::GetPartyMaxLevel(BM::GUID const& rkPartyGuid, int &rkOutGuid) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetPartyMaxLevel(rkPartyGuid, rkOutGuid);
}
bool PgLocalPartyMgr::GetPartyOption(BM::GUID const& rkPartyGuid, SPartyOption& rkOut)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetPartyOption(rkPartyGuid, rkOut);
}
bool PgLocalPartyMgr::GetPartyShareItem_NextOwner(BM::GUID const& rkPartyGuid, SGroundKey const& rkGndKey, BM::GUID& rkNextOwner)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetPartyShareItem_NextOwner(rkPartyGuid, rkGndKey, rkNextOwner);
}
bool PgLocalPartyMgr::WriteToPacketPartyName(BM::GUID const& rkPartyGuid, BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->WriteToPacketPartyName(rkPartyGuid, rkPacket);
}
void PgLocalPartyMgr::WriteToPacketPartyList(BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	Instance()->WriteToPacketPartyList(rkPacket);
}
bool PgLocalPartyMgr::ProcessMsg(unsigned short const usType, SGroundKey const& rkGndKey, BM::Stream* pkMsg)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->ProcessMsg(usType, rkGndKey, pkMsg);
}
bool PgLocalPartyMgr::SyncFromContents(BM::GUID const& rkPartyGuid, BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->SyncFromContents(rkPartyGuid, rkPacket);
}
int PgLocalPartyMgr::GetPartyMemberFriend(BM::GUID const& rkPartyGuid, BM::GUID const& rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->GetPartyMemberFriend(rkPartyGuid, rkCharGuid);
}

bool PgLocalPartyMgr::SyncFromContentsExpedition(BM::GUID const& rkPartyGuid, BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->SyncFromContentsExpedition(rkPartyGuid, rkPacket);
}

bool PgLocalPartyMgr::GetExpeditionMemberGround(BM::GUID const& ExpeditionGuid, SGroundKey const & GndKey, VEC_GUID & OutVec, BM::GUID const & Ignore) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->GetExpeditionMemberGround(ExpeditionGuid, GndKey, OutVec, Ignore);
}

bool PgLocalPartyMgr::GetExpeditionMasterGuid(BM::GUID const & Expedition, BM::GUID & OutGuid) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->GetExpeditionMasterGuid(Expedition, OutGuid);
}

size_t PgLocalPartyMgr::GetExpeditionMemberCount(BM::GUID const & ExpeditionGuid) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->GetExpeditionMemberCount(ExpeditionGuid);
}

bool PgLocalPartyMgr::WriteToPacketExpeditionName(BM::GUID const & ExpeditionGuid, BM::Stream & Packet) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->WriteToPacketExpeditionName(ExpeditionGuid, Packet);
}

PgLocalParty* PgLocalPartyMgr::GetLocalParty(BM::GUID const & PartyGuid) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);

	return Instance()->GetLocalParty(PartyGuid);
}

PgLocalExpedition* PgLocalPartyMgr::GetLocalExpedition(BM::GUID const & ExpeditionGuid) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);

	return Instance()->GetLocalExpedition(ExpeditionGuid);
}

bool PgLocalPartyMgr::IsExpeditionMaster(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);

	return Instance()->IsExpeditionMaster(ExpeditionGuid, CharGuid);
}

HRESULT PgLocalPartyMgr::IsStartable(BM::GUID const & ExpeditionGuid)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);

	return Instance()->IsStartable(ExpeditionGuid);
}

bool PgLocalPartyMgr::GetExpeditionMember(BM::GUID const & ExpeditionGuid, VEC_GUID & out)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);

	return Instance()->GetExpeditionMember(ExpeditionGuid, out);
}

BM::GUID const & PgLocalPartyMgr::GetExpeditionNpcGuid(BM::GUID const & ExpeditionGuid)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);

	return Instance()->GetExpeditionNpcGuid(ExpeditionGuid);
}

int PgLocalPartyMgr::GetMemberTeamNo(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);

	return Instance()->GetMemberTeamNo(ExpeditionGuid, CharGuid);
}

int PgLocalPartyMgr::GetMaxTeamCount(BM::GUID const & ExpeditionGuid)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);

	return Instance()->GetMaxTeamCount(ExpeditionGuid);
}

bool PgLocalPartyMgr::GetExpeditionShareItem_NextOwner(BM::GUID const ExpeditionGuid, SGroundKey const & GndKey, BM::GUID & NextOwner)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);

	return Instance()->GetExpeditionShareItem_NextOwner(ExpeditionGuid, GndKey, NextOwner);
}

bool PgLocalPartyMgr::GetExpeditionOption(BM::GUID const & ExpeditionGuid, SExpeditionOption & Out)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);

	return Instance()->GetExpeditionOption(ExpeditionGuid, Out);
}