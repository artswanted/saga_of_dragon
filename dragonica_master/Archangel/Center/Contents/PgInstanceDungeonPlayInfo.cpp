#include "stdafx.h"
#include "Variant/Item.h"
#include "PgInstanceDungeonPlayInfo.h"
#include "PgActionT2M.h"

PgInstanceDungeonPlayInfo::PgInstanceDungeonPlayInfo()
:	m_iMaxItemCount(0)
,	m_bRecvResultItem(false)
,	m_bResultStart(false)
{
	m_kSelectBox.reserve( ms_iRESULTSELECT_MAXCOUNT );

	for( size_t i=0; i!=ms_iRESULTSELECT_MAXCOUNT; ++i )
	{
		m_kSelectBox.push_back( BM::GUID::NullData() );
	}
}

PgInstanceDungeonPlayInfo::~PgInstanceDungeonPlayInfo()
{}

void PgInstanceDungeonPlayInfo::Init( SGroundKey const rkKey )
{
	Clear();
	m_kKey = rkKey;
}

void PgInstanceDungeonPlayInfo::Clear()
{
	BM::CAutoMutex kLock( m_kMutex );
	m_dwTime = 0;
	m_bRecvResultItem = false;
	m_bResultStart = false;
	m_kKey.Clear();
	m_kPlayerInfo.clear();
	m_iMaxItemCount = 0;

	CONT_SELECT::iterator select_itr;
	for ( select_itr=m_kSelectBox.begin(); select_itr!=m_kSelectBox.end(); ++select_itr )
	{
		(*select_itr) = BM::GUID::NullData();
	}
}

bool PgInstanceDungeonPlayInfo::ResultStart( VEC_RESULTINDUN const& rkResultList )
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( m_bResultStart )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	//CURR_USER_LOG(BM::LOG_LV7, _T("[INSTANCEDUNGEON::RESULT_START] GroundKey[%d-%s]"), m_kKey.GroundNo(), m_kKey.Guid().str().c_str() );
	BM::Stream kCPacket( PT_N_C_NFY_BOSS_RESULT, m_bRecvResultItem );
	if ( true == m_bRecvResultItem )
	{
		kCPacket.Push( m_kPlayerInfo.size() );

		VEC_RESULTINDUN::const_iterator rst_itr;
		CONT_ITEM_CREATE_ORDER::iterator item_itr;
		CONT_PLAYER::iterator user_itr;
		for ( rst_itr=rkResultList.begin(); rst_itr!=rkResultList.end(); ++rst_itr )
		{
			user_itr=m_kPlayerInfo.find( rst_itr->kCharGuid );
			if ( user_itr==m_kPlayerInfo.end() )
			{
				continue;
			}

			SINDUNPLAYINFO& rkInfo = user_itr->second;
			rkInfo.iNowExp = rst_itr->iNowExp - rkInfo.iNowExp; // iNowExp 값이 여러번 사망시 음수가 될 수 있음
			int iAddExp = std::max(static_cast< int >(rkInfo.iNowExp), 0); // 최소 0 (표시 항상 양수가 되도록, 음수일 경우 0으로 표시)
			int iTemp = 0;
			kCPacket.Push( rkInfo.kMemberGuid );					// GUID
			kCPacket.Push( user_itr->first );						// Character GUID
			kCPacket.Push( iTemp );									// 예비
			kCPacket.Push( iTemp );									// 예비
			kCPacket.Push( iTemp+1 );								// 보스
			kCPacket.Push( iAddExp );								// 경험치
			kCPacket.Push( iTemp );									// 보너스 경험치
			kCPacket.Push( rst_itr->iLevel );						// 레벨
			kCPacket.Push( rkInfo.wstrName );						// 이름
			int iItemCount = m_iMaxItemCount;
			kCPacket.Push( m_iMaxItemCount );						// 결과 아이템 갯수

			PU::TWriteArray_M(kCPacket, rkInfo.kResultItemList);
			kCPacket.Push( rkInfo.fParticipationRate );				// 참여율
	//		kCPacket.Push( rkInfo.kResultItemList );


	// 		for( item_itr=rkInfo.kResultItemList.begin(); item_itr!=rkInfo.kResultItemList.end(); ++item_itr )
	// 		{
	// 			kCPacket.Push( (int)(item_itr->ItemNo()) );		// 아이템 번호(***나중에 DWORD로???***)
	// 			--iItemCount;
	// 		}
	// 
	// 		while( iItemCount-- )	{	kCPacket.Push( iTemp );	}	// 빈 아이템
		}

		m_dwTime = BM::GetTime32() + ms_dwRESULTSELECT_WAIT_TIME;
	}

	BroadCast( kCPacket );	
	m_bResultStart = true;
	return m_bRecvResultItem;
}

bool PgInstanceDungeonPlayInfo::SetResultItem( const SNfyResultItemList& rkResultItem )
{
	BM::CAutoMutex kLock( m_kMutex );
	m_iMaxItemCount = rkResultItem.iMaxItemCount;

	const VEC_RESULTITEM& rkContList = rkResultItem.kResultItemList;
	
	VEC_RESULTITEM::const_iterator item_itr = rkContList.begin();
	while(item_itr!=rkContList.end() )
	{
		CONT_PLAYER::iterator user_itr = m_kPlayerInfo.find( item_itr->kCharGuid );
		
		if(user_itr == m_kPlayerInfo.end())
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("This User No Dungeon User, Character GUID=") << item_itr->kCharGuid << _T(" / Dungeon=") << C2L(m_kKey) );
			++item_itr;
			continue;
		}

		user_itr->second.kResultItemList = item_itr->kItemList;
		user_itr->second.fParticipationRate = item_itr->fParticipationRate;
		++item_itr;
	}

	m_bRecvResultItem = true;
	return true;
}

bool PgInstanceDungeonPlayInfo::SetSelectBox( size_t iSelect, BM::GUID const &kCharGuid )
{
	BM::CAutoMutex kLock( m_kMutex );
	if ( iSelect >= ms_iRESULTSELECT_MAXCOUNT )
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("This Select BoxNo[") << iSelect << _T("] is Bad, Character GUID[") << kCharGuid << _T("] Dungeon=") << C2L(m_kKey) );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_PLAYER::iterator user_itr = m_kPlayerInfo.find( kCharGuid );
	if ( user_itr==m_kPlayerInfo.end() )
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("This User No Dungeon User, Character GUID[") << kCharGuid << _T("], Dungeon=") << C2L(m_kKey));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	else
	{
		if ( user_itr->second.uiSelectBoxNum != SINDUNPLAYINFO::ms_unSelectBoxNum )
		{
			// 이미 선택한 놈이 또 선택하려고 하면 어쩌냐
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("This User Select New BoxNo(") << iSelect << _T("), Character GUID[") << kCharGuid << _T("] / Dungeon=") << C2L(m_kKey));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if ( m_kSelectBox[iSelect] != BM::GUID::NullData() )
	{
		// 이미 선택되어 있는 박스이다.
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("This Box[") << iSelect << _T("] Selected->Character GUID[") << m_kSelectBox[iSelect] << _T("] / Dungeon[") << C2L(m_kKey) << _T("]")  );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	//INFO_LOG( BM::LOG_LV0, _T("[%s] Select Box[%d] / Dungeon[%d-%s]"), __FUNCTIONW__, iSelect, m_kKey.GroundNo(), m_kKey.Guid().str().c_str() );
	user_itr->second.uiSelectBoxNum = iSelect;
	m_kSelectBox[iSelect] = kCharGuid;
	BM::Stream kCPacket(PT_M_C_NFY_SELECTED_BOX, (int)iSelect );
	kCPacket.Push( kCharGuid );
	BroadCast( kCPacket );
	return true;
}

void PgInstanceDungeonPlayInfo::ResultEnd()
{
	BM::CAutoMutex kLock( m_kMutex );
	if ( !m_bResultStart )
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Not Result Start!! Ground[") << C2L(m_kKey) << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! ResultStart is NULL"));
		return;
	}

	CONT_PLAYER::iterator user_itr;
	size_t iCash = 0;
	for ( user_itr=m_kPlayerInfo.begin(); user_itr!=m_kPlayerInfo.end(); ++user_itr )
	{
		// 아이템을 준다
		CONT_ITEM_CREATE_ORDER const& rkResultItemList = user_itr->second.kResultItemList;
		if( !rkResultItemList.empty() )
		{
			// 결과 아이템 생성 요청(맵서버로)
			PgActionT2M_CreateItem kAction(CIE_Mission, rkResultItemList);
			kAction.DoAction(user_itr->first, m_kKey);
			//CURR_USER_LOG(BM::LOG_LV7, _T("[INSTANCEDUNGEON::CREATE_RESULT_ITEM] ItemNo[%d] UserCHARGUID[%s] GroundKey[%d-%s]"), item_itr->ItemNo(), user_itr->first.str().c_str(), m_kKey.GroundNo(), m_kKey.Guid().str().c_str() );
		}

		if ( user_itr->second.uiSelectBoxNum != SINDUNPLAYINFO::ms_unSelectBoxNum )
		{
			continue;
		}

		for( size_t i=iCash; i!=ms_iRESULTSELECT_MAXCOUNT; ++i )
		{
			if ( m_kSelectBox[i] == BM::GUID::NullData() )
			{
				user_itr->second.uiSelectBoxNum = i;
				m_kSelectBox[i] = user_itr->first;
				BM::Stream kCPacket(PT_M_C_NFY_SELECTED_BOX, (int)i );
				kCPacket.Push( user_itr->first );
				BroadCast( kCPacket );
				iCash = i;
				break;
			}
		}
	}

	BM::Stream kPacket( PT_M_C_NFY_OPEN_BOX );
	BroadCast( kPacket );
	m_bResultStart = false;
	//CURR_USER_LOG(BM::LOG_LV7, _T("[INSTANCEDUNGEON::RESULT_END] GroundKey[%d-%s]"), m_kKey.GroundNo(), m_kKey.Guid().str().c_str() );
}

void PgInstanceDungeonPlayInfo::BroadCast( BM::Stream const& rkPacket )
{
	BM::CAutoMutex kLock( m_kMutex );
	CONT_PLAYER::iterator user_itr;
	for ( user_itr=m_kPlayerInfo.begin(); user_itr!=m_kPlayerInfo.end(); ++user_itr )
	{
		g_kServerSetMgr.Locked_SendToUser( user_itr->second.kMemberGuid, rkPacket );
	}
}


bool PgInstanceDungeonPlayInfo::Join( const SNfyJoinIndun& kJoinInfo )
{
	BM::CAutoMutex kLock( m_kMutex );
	std::wstring kCharName;
	if( S_OK == GetCharacterNameByGuid(kJoinInfo.kCharGuid, false, kCharName) )
	{
		m_kPlayerInfo.insert( std::make_pair( kJoinInfo.kCharGuid, SINDUNPLAYINFO(kJoinInfo.kMemberGuid, kJoinInfo.iNowExp, kCharName.c_str()) ) );
		CURR_USER_LOG(BM::LOG_LV7, L"[INSTANCEDUNGEON::JOINPLAYER] CHARGUID["<<kJoinInfo.kCharGuid<<L"], "<<C2L(m_kKey));
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgInstanceDungeonPlayInfo::Leave( BM::GUID const &kCharGuid )
{
	BM::CAutoMutex kLock( m_kMutex );
	CURR_USER_LOG(BM::LOG_LV7, L"[INSTANCEDUNGEON::LEAVEPLAYER] CHARGUID["<<kCharGuid<<L"], "<<C2L(m_kKey));
	m_kPlayerInfo.erase( kCharGuid );
}

//////////////////////////////////////////////////////////////////////////////////////////////
// 원정대

PgExpeditionIndunPlayInfo::PgExpeditionIndunPlayInfo()
:	m_MaxItemCount(0)
,	m_MaxSpecialItemCount(0)
,	m_RecvResultItem(false)
,	m_ResultStart(false)
,	m_Tender(false)
,	m_State(ERS_NONE)
,	m_BestTeamNo(0)
,	m_BestTeamDP(0)
,	m_BestPlayerDP(0)
,	m_BestSupporterHP(0)
,	m_BestPlayer(BM::GUID::NullData())
,	m_BestSupporter(BM::GUID::NullData())
{
}

PgExpeditionIndunPlayInfo::~PgExpeditionIndunPlayInfo()
{
}

void PgExpeditionIndunPlayInfo::Init(SGroundKey const Key)
{
	Clear();
	m_Key = Key;
}

void PgExpeditionIndunPlayInfo::Clear()
{
	BM::CAutoMutex Lock(m_Mutex);

	m_Time = 0;
	m_RecvResultItem = false;
	m_ResultStart = false;
	m_Tender = false;
	m_Key.Clear();
	m_PlayerInfo.clear();
	m_MaxItemCount = 0;
	m_MaxSpecialItemCount= 0;
	m_State = ERS_NONE;

	m_BestTeamNo = 0;
	m_BestTeamDP = 0;
	m_BestPlayerDP = 0;
	m_BestSupporterHP = 0;
	m_BestPlayer = BM::GUID::NullData();
	m_BestSupporter = BM::GUID::NullData();

	m_SpecialItem.clear();
	m_TenderCharList.clear();
	m_TenderGiveUpList.clear();
}

bool PgExpeditionIndunPlayInfo::Join(SNfyJoinIndun const & JoinInfo)
{
	BM::CAutoMutex Lock(m_Mutex);

	std::wstring CharName;
	if( S_OK == GetCharacterNameByGuid(JoinInfo.kCharGuid, false, CharName) )
	{
		m_PlayerInfo.insert(std::make_pair(JoinInfo.kCharGuid, SEXPEDITIONINDUNPLAYINFO(JoinInfo.kMemberGuid, JoinInfo.kCharGuid, JoinInfo.iNowExp, CharName.c_str(), DefaultTenderCount ) ) );
		CURR_USER_LOG(BM::LOG_LV7, L"[INSTANCEDUNGEON::JOINPLAYER] CHARGUID["<< JoinInfo.kCharGuid << L"], " << C2L(m_Key));
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgExpeditionIndunPlayInfo::Leave(BM::GUID const & CharGuid)
{
	BM::CAutoMutex Lock(m_Mutex);

	CURR_USER_LOG(BM::LOG_LV7, L"[INSTANCEDUNGEON::LEAVEPLAYER] CHARGUID[" << CharGuid << L"], " << C2L(m_Key));

	// 던전에서 나가면 입찰자 리스트에서 제거.
	VEC_GUID::iterator tender_iter = std::find(m_TenderCharList.begin(), m_TenderCharList.end(), CharGuid);
	if( tender_iter != m_TenderCharList.end() )
	{
		m_TenderCharList.erase(tender_iter);
	}

	// 던전에서 나가면 포기 리스트에서 제거.
	VEC_GUID::iterator giveup_iter = std::find(m_TenderGiveUpList.begin(), m_TenderGiveUpList.end(), CharGuid);
	if( giveup_iter != m_TenderGiveUpList.end() )
	{
		m_TenderGiveUpList.erase(giveup_iter);
	}

	// 던전에서 나가면 플레이어 정보 리스트에서 제거.
	CONT_EXPEDITION_PLAYER::iterator info_iter = m_PlayerInfo.find(CharGuid);
	if( info_iter != m_PlayerInfo.end() )
	{
		m_PlayerInfo.erase(info_iter);
	}
}

// 입찰.
bool PgExpeditionIndunPlayInfo::Tender(BM::GUID const & CharGuid)
{
	BM::CAutoMutex Lock(m_Mutex);

	if( m_ResultStart && m_Tender )
	{
		VEC_GUID::iterator iter = std::find(m_TenderCharList.begin(), m_TenderCharList.end(), CharGuid);
		if( iter == m_TenderCharList.end() )
		{
			CONT_EXPEDITION_PLAYER::iterator user_iter = m_PlayerInfo.find(CharGuid);
			if( user_iter != m_PlayerInfo.end() )
			{
				if( 0 < user_iter->second.TenderableCount )	// 입찰기회가 있을때만 가능.
				{
					m_TenderCharList.push_back(CharGuid);
					user_iter->second.TenderableCount -= 1;	// 입찰 하나 빼줌.
				}
				// 모든 유저가 입찰 또는 입찰 포기를 했다면 기다릴 필요없이 바로 넘어감.
				int Count = m_TenderCharList.size() + m_TenderGiveUpList.size();
				if( Count == m_PlayerInfo.size() )
				{
					SetState(ERS_TENDEREND);
				}
				return true;
			}
		}
	}

	return false;
}

// 입찰 포기.
bool PgExpeditionIndunPlayInfo::GiveupTender(BM::GUID const & CharGuid)
{
	BM::CAutoMutex Lock(m_Mutex);

	if( m_ResultStart && m_Tender )
	{
		VEC_GUID::iterator iter = std::find(m_TenderGiveUpList.begin(), m_TenderGiveUpList.end(), CharGuid);
		if( iter == m_TenderGiveUpList.end() )
		{
			m_TenderGiveUpList.push_back(CharGuid);
			// 모든 유저가 입찰 또는 입찰 포기를 했다면 기다릴 필요없이 바로 넘어감.
			int Count = m_TenderCharList.size() + m_TenderGiveUpList.size();
			if( Count == m_PlayerInfo.size() )
			{
				SetState(ERS_TENDEREND);
			}
			return true;
		}
	}

	return false;
}

void PgExpeditionIndunPlayInfo::BroadCast(BM::Stream const & Packet)
{
	BM::CAutoMutex Lock(m_Mutex);

	CONT_EXPEDITION_PLAYER::iterator user_iter;
	for( user_iter = m_PlayerInfo.begin() ; user_iter != m_PlayerInfo.end() ; ++user_iter )
	{
		g_kServerSetMgr.Locked_SendToUser(user_iter->second.MemberGuid, Packet);
	}
}

bool PgExpeditionIndunPlayInfo::ResultStart(VEC_RESULTINDUN const & ResultList)
{
	BM::CAutoMutex Lock(m_Mutex);

	if( m_ResultStart )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::Stream Packet(PT_N_C_NFY_EXPEDITION_BOSS_RESULT, m_RecvResultItem);
	if( true == m_RecvResultItem )
	{
		Packet.Push(m_BestTeamNo);
		Packet.Push(m_BestTeamDP);
		
		Packet.Push(m_BestPlayer);
		Packet.Push(m_BestPlayerDP);
		
		Packet.Push(m_BestSupporter);
		Packet.Push(m_BestSupporterHP);

		Packet.Push(ResultList.size());

		CONT_EXPEDITION_PLAYER::iterator user_iter;
		VEC_RESULTINDUN::const_iterator res_iter = ResultList.begin();
		for( res_iter ; res_iter != ResultList.end() ; ++res_iter )
		{
			user_iter = m_PlayerInfo.find(res_iter->kCharGuid);
			if( user_iter == m_PlayerInfo.end() )
			{
				continue;
			}

			SEXPEDITIONINDUNPLAYINFO & Info = user_iter->second;
			Packet.Push(Info.CharGuid);
			Packet.Push(m_MaxItemCount);	// 결과 아이템 갯수.
			PU::TWriteArray_M(Packet, Info.ResultItemList);
		}
	}

	Packet.Push(m_SpecialItem.size());		// 특별 보상 아이템.
	PU::TWriteArray_M(Packet, m_SpecialItem);

	m_ResultStart = true;

	BroadCast(Packet);

	SetState(ERS_1ST);

	// 기본 보상은 즉시 지급 해준다(결산 중에 나가더라도 기본 보상은 받을 수 있도록).
	CONT_EXPEDITION_PLAYER::iterator user_iter;
	for( user_iter = m_PlayerInfo.begin() ; user_iter != m_PlayerInfo.end() ; ++user_iter )
	{
		//아이템을 준다.
		CONT_ITEM_CREATE_ORDER const & ResultItemList = user_iter->second.ResultItemList;
		if( !ResultItemList.empty() )
		{
			// 결과 아이템 생성 요청( 맵 서버로 )
			PgActionT2M_CreateItem Action(CIE_Mission, ResultItemList);
			Action.DoAction(user_iter->first, m_Key);
		}
	}

	return m_RecvResultItem;
}

void PgExpeditionIndunPlayInfo::ResultEnd()
{
	BM::CAutoMutex Lock(m_Mutex);

	if( false == m_ResultStart )
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Not Result Start!! Ground[") << C2L(m_Key) << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! ResultStart is NULL"));
		return;
	}

	BM::Stream NfyPacket(PT_N_M_NFY_EXPEDITION_RESULT_END);		// 결산 종료 맵으로 통보.
	g_kServerSetMgr.Locked_SendToGround(m_Key, NfyPacket);

	m_ResultStart = false;
}

bool PgExpeditionIndunPlayInfo::SetExpeditionResultItem(BM::Stream & Packet)
{
	BM::CAutoMutex Lock(m_Mutex);

	Packet.Pop(m_BestTeamNo);		// 베스트 팀 번호.
	Packet.Pop(m_BestTeamDP);		// 베스트 팀 DP.

	Packet.Pop(m_BestPlayer);		// 베스트 플레이어 guid.
	Packet.Pop(m_BestPlayerDP);		// 베스트 플레이어 DP.

	///////////////////////////////////////////////////////////////
	// 베스트 플레이어에게 입찰 기회 더 줌.
	CONT_EXPEDITION_PLAYER::iterator user_iter = m_PlayerInfo.find(m_BestPlayer);
	if( user_iter != m_PlayerInfo.end() )
	{
		user_iter->second.TenderableCount += 1;
	}

	///////////////////////////////////////////////////////////////

	Packet.Pop(m_BestSupporter);	// 베스트 서포터 guid.
	Packet.Pop(m_BestSupporterHP);	// 베스트 서포터 HP.

	SNfyResultItemList ResultItem;	// 일반 보상아이템.
	ResultItem.ReadFromPacket(Packet);

	m_MaxItemCount = ResultItem.iMaxItemCount;

	const VEC_RESULTITEM & ContList = ResultItem.kResultItemList;
	VEC_RESULTITEM::const_iterator item_iter = ContList.begin();
	while( item_iter != ContList.end() )
	{
		CONT_EXPEDITION_PLAYER::iterator user_iter = m_PlayerInfo.find(item_iter->kCharGuid);
		if( user_iter == m_PlayerInfo.end() )
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("This User No Dungeon User, Character GUID=") << item_iter->kCharGuid << _T(" / Dungeon=") << C2L(m_Key) );
			++item_iter;
			continue;
		}

		user_iter->second.ResultItemList = item_iter->kItemList;
		++item_iter;
	}

	// 특별 보상아이템.
	Packet.Pop(m_MaxSpecialItemCount);
	PU::TLoadArray_M(Packet, m_SpecialItem);
	
	m_RecvResultItem = true;
	return true;
}

void PgExpeditionIndunPlayInfo::OnTick(DWORD CurTime)
{
	BM::CAutoMutex Lock(m_Mutex);

	switch(m_State)
	{
	case ERS_1ST:
		{
			if( m_Time < CurTime )
			{
				SetState(ERS_WAIT);
			}
		}break;
	case ERS_WAIT:
		{
			if( m_Time < CurTime )
			{
				SetState(ERS_START);
			}
		}break;
	case ERS_START:
		{
			if( m_Time < CurTime )
			{
				SetState(ERS_TENDER);
			}
		}break;
	case ERS_TENDER:
		{
			if( m_Time < CurTime )
			{
				SetState(ERS_TENDEREND);
			}
		}break;
	case ERS_TENDEREND:
		{
			if( m_Time < CurTime )
			{
				SetState(ERS_RESULTEND);
			}
		}break;
	case ERS_RESULTEND:
		{
			if( m_Time < CurTime )
			{
				SetState(ERS_START);
			}
		}break;
	}
}

void PgExpeditionIndunPlayInfo::SetState(ERESULTSTATE State)
{
	BM::CAutoMutex Lock(m_Mutex);

	m_State = State;

	switch(m_State)
	{
	case ERS_1ST:
		{
			m_Time = BM::GetTime32() + EXPEDITION_BOSSDOWN;
		}break;
	case ERS_WAIT:
		{
			m_Time = BM::GetTime32() + EXPEDITION_START;
		}break;
	case ERS_START:
		{
		}break;
	case ERS_TENDER:
		{
			BM::Stream Packet(PT_N_C_NFY_START_TENDER);
			Packet.Push(m_SpecialItem.size()); // 특별 보상 남은 개수를 보내줌.
			BroadCast(Packet);		// 입찰 시작 알림.
			m_Tender = true;

			m_Time = BM::GetTime32() + EXPEDITION_TENDER;
		}break;
	case ERS_TENDEREND:
		{
			m_Tender = false;
			m_Time = BM::GetTime32() + EXPEDITION_TENDER_END;

			BM::GUID Winner = BM::GUID::NullData(); // 당첨자 CHAR_GUID.
			BM::Stream Packet(PT_N_C_NFY_ACCEPTANCE_USER);

			if( !m_TenderCharList.empty() )
			{	// 입찰자가 한명이라도 있다.
				int Idx = BM::Rand_Range(m_TenderCharList.size() - 1);
				
				int Count = 0;	
				VEC_GUID::const_iterator iter = m_TenderCharList.begin();
				for( ; iter != m_TenderCharList.end() ; ++iter)
				{
					if( Count == Idx )
					{
						Winner = (*iter);
						Packet.Push( Winner );
						break;
					}
					++Count;
				}
			}
			else
			{	// 입찰자가 없으면 아무나 주면 됨.
				int Idx = BM::Rand_Range(m_PlayerInfo.size() - 1);
				
				int Count = 0;
				CONT_EXPEDITION_PLAYER::const_iterator iter = m_PlayerInfo.begin();
				for( ; iter != m_PlayerInfo.end() ; ++iter)
				{
					if( Count == Idx )
					{
						Winner = iter->second.CharGuid;
						Packet.Push( Winner );
						break;
					}
					++Count;
				}
			}

			BroadCast(Packet); // 낙찰자 알림.

			if( !m_SpecialItem.empty() )
			{
				CONT_SPECIALITEM::iterator acc_iter = m_SpecialItem.begin();				// 당첨된 아이템은 리스트의 맨 첫번재.

				// 특별 보상 즉시 지급( 하나만 먹고 탈퇴 할 수도 있기 때문에. 바로 안주면 당첨 되고서도 받지 못함).
				{
					// 지급할 아이템.
					CONT_ITEM_CREATE_ORDER CreateItem;
					CreateItem.push_back( acc_iter->ResultItem );

					BM::Stream Packet(PT_N_M_REQ_CREATE_ITEM, Winner);	// Winner  :  당첨자 CHAR_GUID.
					Packet.Push(CIE_Mission);
					PU::TWriteArray_M(Packet, CreateItem);
					g_kServerSetMgr.Locked_SendToGround( m_Key, Packet );
				}

				m_SpecialItem.erase(acc_iter);	// 지급한 아이템은 삭제.
			}

			m_TenderCharList.clear();		// 입찰, 입찰 포기 리스트 비움.
			m_TenderGiveUpList.clear();
		}break;
	case ERS_RESULTEND:
		{
			if( !m_SpecialItem.empty() )		// 아직 줄게 있다면 다시 시작.
			{
				SetState(ERS_TENDER);
			}
			else
			{
				SetState(ERS_END);
			}
		}break;
	case ERS_END:
		{
			ResultEnd();
		}break;
	}
}