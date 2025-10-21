#include "stdafx.h"
#include "Variant/PgEventView.h"
#include "PgServerSetMgr.h"
#include "PgNotice.h"
#include "Lohengrin/VariableContainer.h"

//---------------------------------------------------------------------
// PgNotice_All
//---------------------------------------------------------------------
PgNotice_All::PgNotice_All()
:	m_i64ReqTime(0i64)
,	m_bSent(false)
,	m_i64ValidTime(0i64)
{}

PgNotice_All::~PgNotice_All()
{}

void PgNotice_All::SetPacket( BM::Stream const &kPacket )
{
	m_kPacket.Clear();
	m_kPacket.Push( kPacket );
	m_kPacket.PosAdjust();
}

void PgNotice_All::WriteToPacket( BM::Stream &kPacket )const
{
	kPacket.Push( m_kPacket );
}

bool PgNotice_All::Send( BM::Stream const &kPacket )const
{
	return ::SendToServerType( CEL::ST_CENTER, kPacket );
}

//---------------------------------------------------------------------
// PgNotice_Guild
//---------------------------------------------------------------------
PgNotice_Guild::PgNotice_Guild()
{
}

PgNotice_Guild::~PgNotice_Guild()
{
}

void PgNotice_Guild::WriteToPacket( BM::Stream &kPacket )const
{
	kPacket.Push( m_kGuildGuid );
	PgNotice_All::WriteToPacket( kPacket );
}

bool PgNotice_Guild::Send( BM::Stream const &kPacket )const
{
	return ::SendToGuildMgr( kPacket );
}

//---------------------------------------------------------------------
// PgNotice_Guild
//---------------------------------------------------------------------
void PgNotice_GuildChatMsg::Send(BM::GUID const& kMemberGuid)const
{
	for(CONT_MSG::const_iterator c_it=m_kContMsg.begin(); c_it!=m_kContMsg.end(); ++c_it)
	{
		g_kRealmUserMgr.Locked_SendToUser( kMemberGuid, (*c_it), true );
	}
}

void PgNotice_GuildChatMsg::Add(BM::Stream & rkPacket)
{
	int iLine = 0;
	if(S_OK != g_kVariableContainer.Get(EVar_Kind_Guild, EVar_Send_LoginUser_ChatLine, iLine))
	{
		int const DEFAULT_GUILD_LOGINUSER_CHAT_LINE = 30;
		iLine = DEFAULT_GUILD_LOGINUSER_CHAT_LINE;
	}

	if(iLine <= 0)
	{
		return;
	}

	m_kContMsg.push_back( rkPacket );
	if( m_kContMsg.size() > iLine )
	{
		m_kContMsg.pop_front();
	}
}

// ---------------------------------------------------
// PgNoticeSystem
// ---------------------------------------------------
PgNoticeSystem::PgNoticeSystem()
{}

PgNoticeSystem::~PgNoticeSystem()
{
	CONT_NOTICE::iterator itr = m_kContNotice.begin();
	for ( ; itr != m_kContNotice.end() ; ++itr )
	{
		SAFE_DELETE( *itr );
	}
}

void PgNoticeSystem::Add( PgNotice_All * pkNotice )
{
	m_kContNotice.push_back( pkNotice );
}

void PgNoticeSystem::OnTick()
{
	if ( !m_kLoginUserPacket.IsEmpty() )
	{
		SET_GUID::const_iterator itr = m_kLoginPacketWaiter.begin();
		for ( ; itr != m_kLoginPacketWaiter.end() ; ++itr )
		{
			g_kRealmUserMgr.Locked_SendToUser( *itr, m_kLoginUserPacket, true );
		}
		m_kLoginPacketWaiter.clear();
	}

	bool bRefreshLoginUserPacket = false;
	__int64 const iNow64Time = g_kEventView.GetLocalSecTime(CGameTime::DEFAULT);

	CONT_NOTICE::iterator notice_itr = m_kContNotice.begin();
	while ( notice_itr != m_kContNotice.end() )
	{
		PgNotice_All *pkNotice = *notice_itr;
		if ( true == pkNotice->IsSent() )
		{
			if ( iNow64Time >= pkNotice->GetValidTime() )
			{
				if ( NOTICE_ALL == pkNotice->GetType() )
				{
					bRefreshLoginUserPacket = true;
				}

				SAFE_DELETE(pkNotice);
				notice_itr = m_kContNotice.erase( notice_itr );
				continue;//
			}
		}
		else
		{
			if ( iNow64Time >= pkNotice->GetRequestTime() )
			{
				BM::Stream kPacket( PT_N_C_NFY_NOTICE_PACKET, static_cast<size_t>(1) );
				pkNotice->WriteToPacket( kPacket );
				pkNotice->Send( kPacket );

				if ( iNow64Time >= pkNotice->GetValidTime() )
				{
					SAFE_DELETE(pkNotice);
					notice_itr = m_kContNotice.erase( notice_itr );
					continue;//
				}

				pkNotice->SetSent();
			}
		}

		++notice_itr;
	}

	if ( true == bRefreshLoginUserPacket )
	{
		m_kLoginUserPacket.Reset( PT_N_C_NFY_NOTICE_PACKET );

		size_t iSize = 0;
		size_t const iWRPos = m_kLoginUserPacket.WrPos();
		m_kLoginUserPacket.Push( iSize );

		notice_itr = m_kContNotice.begin();
		for ( ; notice_itr != m_kContNotice.end() ; ++notice_itr )
		{
			PgNotice_All *pkNotice = *notice_itr;
			if ( true == pkNotice->IsSent() )
			{
				if ( NOTICE_ALL == pkNotice->GetType() )
				{
					++iSize;
					pkNotice->WriteToPacket( m_kLoginUserPacket );
				}
			}
		}

		if ( 0 == iSize )
		{
			m_kLoginUserPacket.Clear();
		}
	}
}

void PgNoticeSystem::OnLogin( BM::GUID const &kMemberGuid )
{
	if ( !m_kLoginUserPacket.IsEmpty() )
	{
		m_kLoginPacketWaiter.insert( kMemberGuid );// 조금 늦게 처리 되도록..
	}
}

void PgNoticeSystem::SendGuildChatMsg( BM::GUID const& kMemberGuid, BM::GUID const& kGuildGuid )const
{
	CONT_GUILD_CHAT_MSG::const_iterator c_it = m_kContGuildChatMsg.find(kGuildGuid);
	if(c_it != m_kContGuildChatMsg.end())
	{
		(*c_it).second.Send(kMemberGuid);
	}
}

void PgNoticeSystem::AddGuildChatMsg( BM::GUID const& kGuildGuid, BM::Stream & rkPacket )
{
	m_kContGuildChatMsg[kGuildGuid].Add( rkPacket );
}

void PgNoticeSystem::SendTradeChatMsg( BM::GUID const& kMemberGuid )
{
	BM::CAutoMutex kLock(m_kContTradeChatMutex);
	m_kContTradeChatMsg.Send(kMemberGuid);
}

void PgNoticeSystem::AddTradeChatMsg( BM::Stream & rkPacket )
{
	BM::CAutoMutex kLock(m_kContTradeChatMutex);
	m_kContTradeChatMsg.Add(rkPacket);
}

bool PgNoticeSystem::Save(const char* filename)
{
	BM::CAutoMutex kLock(m_kContTradeChatMutex);
	if (!filename)
		return false;

	BM::Stream kStream;
	kStream.Push(m_kContTradeChatMsg.m_kContMsg);
	return BM::MemToFile(UNI(filename), kStream.Data());
}

bool PgNoticeSystem::Load(const char* filename)
{
	BM::CAutoMutex kLock(m_kContTradeChatMutex);
	if (!filename)
		return false;

	BM::Stream kStream;
	if (!BM::FileToMem(UNI(filename), kStream.Data()))
		return false;

	kStream.PosAdjust();
	return kStream.Pop(m_kContTradeChatMsg.m_kContMsg);
}

PgNoticeSystem_Wrapper::PgNoticeSystem_Wrapper()
{

}

PgNoticeSystem_Wrapper::~PgNoticeSystem_Wrapper()
{

}

void PgNoticeSystem_Wrapper::Add( PgNotice_All * pkNotice )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_/*, true*/ );
	Instance()->Add( pkNotice );
}

void PgNoticeSystem_Wrapper::OnTick()
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_/*, true*/ );
	Instance()->OnTick();
}

void PgNoticeSystem_Wrapper::OnLogin( BM::GUID const &kMemberGuid )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_/*, true*/ );
	Instance()->OnLogin( kMemberGuid );
}

void PgNoticeSystem_Wrapper::SendGuildChatMsg( BM::GUID const& kMemberGuid, BM::GUID const& kGuildGuid )const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_/*, true*/ );
	Instance()->SendGuildChatMsg( kMemberGuid, kGuildGuid );
}

void PgNoticeSystem_Wrapper::AddGuildChatMsg( BM::GUID const& kGuildGuid, BM::Stream & rkPacket )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_/*, true*/ );
	Instance()->AddGuildChatMsg( kGuildGuid, rkPacket );
}

void PgNoticeSystem_Wrapper::ProcessPacket( BM::Stream * pkPacket )
{
	ENotic_MsgType eOrder = E_NoticeMsg_None;
	pkPacket->Pop(eOrder);
	switch (eOrder)
	{
	case E_NoticeMsg_SendPacket:
		{
			E_NOTICE_TYPE eNoticeType;
			BM::GUID kGuid;
			pkPacket->Pop(eNoticeType);
			pkPacket->Pop(kGuid);

			switch ( eNoticeType )
			{
			case NOTICE_ALL:
				{
					PgNotice_All kAction;
					kAction.SetPacket( *pkPacket );
					
					BM::Stream kNoticePacket( PT_N_C_NFY_NOTICE_PACKET, static_cast<size_t>(1) );
					kAction.WriteToPacket( kNoticePacket );
					kAction.Send( kNoticePacket );
				}break;
			case NOTICE_GUILD:
				{
					PgNotice_Guild kAction;
					kAction.SetPacket( *pkPacket );
					kAction.SetGuildGuid( kGuid );

					BM::Stream kNoticePacket( PT_N_C_NFY_NOTICE_PACKET, static_cast<size_t>(1) );
					kAction.WriteToPacket( kNoticePacket );
					kAction.Send( kNoticePacket );
				}break;
			}
		}break;
	case E_NoticeMsg_Add:
		{
			__int64 i64NoticeTime = 0i64;
			__int64 i64ValidTinme = 0i64;
			E_NOTICE_TYPE	eNoticeType;
			pkPacket->Pop(i64NoticeTime);
			pkPacket->Pop(i64ValidTinme);
			pkPacket->Pop(eNoticeType);

			PgNotice_All * pkAction = NULL;
			switch ( eNoticeType )
			{
			case NOTICE_ALL:
				{
					pkAction = new_tr PgNotice_All;
				}break;
			case NOTICE_GUILD:
				{
					BM::GUID kGuid;
					pkPacket->Pop(kGuid);

					PgNotice_Guild * pkGuildAction = new_tr PgNotice_Guild;
					if ( pkGuildAction )
					{
						pkGuildAction->SetGuildGuid( kGuid );
						pkAction = dynamic_cast<PgNotice_All*>(pkGuildAction);
					}
				}break;
			}

			if ( pkAction )
			{
				pkAction->SetRequestTime( i64NoticeTime );
				pkAction->SetValidTime( i64ValidTinme );
				pkAction->SetPacket( *pkPacket );

				if ( 0i64 == i64NoticeTime )
				{
					BM::Stream kNoticePacket( PT_N_C_NFY_NOTICE_PACKET, static_cast<size_t>(1) );
					pkAction->WriteToPacket( kNoticePacket );
					pkAction->Send( kNoticePacket );

					pkAction->SetSent();
				}

				Add( pkAction );
			}
		}break;
	case E_NoticeMsg_UserLogin:
		{
			BM::GUID kMemberGuid;
			BM::GUID kGuidGuid;
			pkPacket->Pop( kMemberGuid );
			pkPacket->Pop( kGuidGuid );
			OnLogin( kMemberGuid );
			if(kGuidGuid.IsNotNull())
			{
				SendGuildChatMsg( kMemberGuid, kGuidGuid );
			}
			Instance()->SendTradeChatMsg(kMemberGuid);
		}break;
	case E_NoticeMsg_GuildChatMsg:
		{
			BM::GUID kGuildGuid;
			BM::Stream kPacket;
			pkPacket->Pop(kGuildGuid);
			pkPacket->Pop(kPacket.Data());
			kPacket.PosAdjust();

			if(kGuildGuid.IsNotNull())
			{
				AddGuildChatMsg( kGuildGuid, kPacket );
			}
		}break;
	case E_NoticeMsg_TradeChatMsg:
		{
			BM::Stream kPacket;
			kPacket.Push(*pkPacket);
			Instance()->AddTradeChatMsg(kPacket);
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown packet = ") << eOrder);
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unknown packet = ") << eOrder);
		}break;
	}
}
