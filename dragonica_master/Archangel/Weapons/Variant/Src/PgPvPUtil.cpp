#include "stdafx.h"
#include "Variant/PgPlayer.h"
#include "PgPvPUtil.h"

namespace PVPUTIL
{

CONT_DEF_PVP_GROUNDGROUP::const_iterator IsHavePvPType(CONT_DEF_PVP_GROUNDGROUP const *pkDefPvPGroup, int const iType)
{
	if ( pkDefPvPGroup && !(pkDefPvPGroup->empty()) )
	{
		CONT_DEF_PVP_GROUNDGROUP::const_iterator pvp_itr = pkDefPvPGroup->begin();

		while( pkDefPvPGroup->end() != pvp_itr )
		{
			if( iType & pvp_itr->first )
			{
				return pvp_itr;
			}
			++pvp_itr;
		}
	}
	return pkDefPvPGroup->end();
}

int	GetRandomPvPGround(CONT_DEF_PVP_GROUNDGROUP const *pkDefPvPGroup, int const iType, int const iUserCount)
{
	if ( pkDefPvPGroup && !(pkDefPvPGroup->empty()) )
	{
		CONT_DEF_PVP_GROUNDGROUP::const_iterator pvp_itr = IsHavePvPType(pkDefPvPGroup, iType);
		if( pkDefPvPGroup->end() != pvp_itr )
		{
			CONT_DEF_PVP_MAP const &kMap = pvp_itr->second;
			CONT_DEF_PVP_MAP::const_iterator map_itr = kMap.find(iUserCount);
			CONT_DEF_PVP_GROUND const & UserCountGround = map_itr->second;
			CONT_DEF_PVP_GROUND::const_iterator gnd_itr;
			if( S_OK == ::RandomElement(UserCountGround, gnd_itr) )
			{//랜덤 나와랑~
				return gnd_itr->iGroundNo;
			}
		}
	}

	return 0;
}

void CopyPlayerToLobbyUser( PgPlayer const * pkPlayer, SPvPLobbyUser_Base &rkLobbyUserBase )
{
	if ( pkPlayer )
	{
		rkLobbyUserBase.iRoomIndex = pkPlayer->GetAbil(AT_PVP_ROOM_INDEX);
		rkLobbyUserBase.kCharacterGuid = pkPlayer->GetID();
		rkLobbyUserBase.sLevel = static_cast<short>(pkPlayer->GetAbil(AT_LEVEL));
		rkLobbyUserBase.iClass = pkPlayer->GetAbil(AT_CLASS);
		rkLobbyUserBase.wstrName = pkPlayer->Name();
		rkLobbyUserBase.kGuildGuid = pkPlayer->GuildGuid();
		
		SPvPReport const * pkPvPReport = dynamic_cast<SPvPReport const*>( pkPlayer );
		if ( pkPvPReport )
		{
			rkLobbyUserBase.kPvPReport = *pkPvPReport;
		}
	}
}

void WriteToPacket_PlayerToLobbyUser( PgPlayer const *pkPlayer, BM::Stream &rkPacket )
{// SPvPLobbyUser_Base::WriteToPacket과 동일하게 만들어야 함.
	if ( pkPlayer )
	{
		rkPacket.Push( pkPlayer->GetAbil( AT_PVP_ROOM_INDEX ) );
		rkPacket.Push( pkPlayer->GetID() );
		rkPacket.Push( static_cast<short>(pkPlayer->GetAbil(AT_LEVEL)) );
		rkPacket.Push( pkPlayer->GetAbil(AT_CLASS) );
		rkPacket.Push( pkPlayer->Name() );
		rkPacket.Push( pkPlayer->GuildGuid() );
		pkPlayer->SPvPReport::WriteToPacket( rkPacket );
	}
}

bool IsLobbyUser( SPvPLobbyUser_Base const &kLobbyUser )
{
	return 0 > kLobbyUser.iRoomIndex;
}

void WriteToPacket_MapToLobby( PgPlayer * pkPlayer, BM::Stream &rkPacket )
{
	rkPacket.Push( pkPlayer->GetRandomSeed() );
	rkPacket.Push( pkPlayer->FrontDirection() );
	pkPlayer->PgPlayerPlayTime::WriteToPacket_PlayTime(rkPacket);
	rkPacket.Push( pkPlayer->GetAbil64(AT_MONEY) );
	rkPacket.Push( pkPlayer->GetAbil(AT_CP) );
	pkPlayer->GetQInven()->WriteToPacket(rkPacket);
	pkPlayer->GetInven()->WriteToPacket(rkPacket, WT_DEFAULT);
	pkPlayer->SPvPReport::WriteToPacket(rkPacket);

	rkPacket.Push( pkPlayer->MacroWaitAns() );
	rkPacket.Push( pkPlayer->MacroMode() );
	rkPacket.Push( pkPlayer->MacroRecvExp() );
	rkPacket.Push( pkPlayer->MacroModeStartTime() );
	rkPacket.Push( pkPlayer->MacroLastRecvExpTime() );
	rkPacket.Push( pkPlayer->MacroKeyCount() );
	rkPacket.Push( pkPlayer->MacroInputFailCount() );
	rkPacket.Push( pkPlayer->MacroCheckCount() );
	rkPacket.Push( pkPlayer->MacroPopupCount() );
}

void ReadFromPacket_MapToLobby( PgPlayer * pkPlayer, BM::Stream &rkPacket )
{
	DWORD dwRandomSeed = 0;rkPacket.Pop(dwRandomSeed);pkPlayer->SetRandomSeed(dwRandomSeed);
	Direction eDir = DIR_NONE;rkPacket.Pop(eDir);pkPlayer->FrontDirection(eDir);
	pkPlayer->PgPlayerPlayTime::ReadFromPacket_PlayTime(rkPacket);
	__int64 i64Value = 0i64;rkPacket.Pop(i64Value);pkPlayer->SetAbil64(AT_MONEY,i64Value);
	int iCP = 0;rkPacket.Pop(iCP);pkPlayer->SetAbil(AT_CP,iCP);
	pkPlayer->GetQInven()->ReadFromPacket(rkPacket);
	pkPlayer->GetInven()->ReadFromPacket(rkPacket, WT_DEFAULT);
	pkPlayer->SPvPReport::ReadFromPacket(rkPacket);

	bool bMacroWatiAns=false;rkPacket.Pop(bMacroWatiAns);pkPlayer->MacroWaitAns(bMacroWatiAns);
	bool bMacroMode=false;rkPacket.Pop(bMacroMode);pkPlayer->MacroMode(bMacroMode);
	bool bMacroRecvExp=false;rkPacket.Pop(bMacroRecvExp);pkPlayer->MacroRecvExp(bMacroRecvExp);
	__int64 i64MacroModeStartTime=0i64;rkPacket.Pop(i64MacroModeStartTime);pkPlayer->MacroModeStartTime(i64MacroModeStartTime);
	__int64 i64MacroLastRecvExpTime=0i64;rkPacket.Pop(i64MacroLastRecvExpTime);pkPlayer->MacroLastRecvExpTime(i64MacroLastRecvExpTime);
	int iMacroKeyCount=0;rkPacket.Pop(iMacroKeyCount);pkPlayer->MacroKeyCount(iMacroKeyCount);
	short sMacroInputFailCount=0;rkPacket.Pop(sMacroInputFailCount);pkPlayer->MacroInputFailCount(sMacroInputFailCount);
	short sMacroCheckCount=0;rkPacket.Pop(sMacroCheckCount);pkPlayer->MacroCheckCount(sMacroCheckCount);
	short sMacroPopupCount=0;rkPacket.Pop(sMacroPopupCount);pkPlayer->MacroPopupCount(sMacroPopupCount);
}

//----------------------------------------------------------------------------------------
// PgEvent_Add
//----------------------------------------------------------------------------------------
PgUserEvent::PgUserEvent(void)
{}

void PgUserEvent::Clear(void)
{
	m_kContEvent.clear();
}

void PgUserEvent::Swap( PgUserEvent &rhs )
{
	m_kContEvent.swap( rhs.m_kContEvent );
}

void PgUserEvent::Merge( PgUserEvent const &rhs )
{
	CONT_EVENT::const_iterator itr = rhs.m_kContEvent.begin();
	for ( ; itr!=rhs.m_kContEvent.end(); ++itr )
	{
		DoAdd( itr->second.kUserInfo, itr->second.byType );
	}
}

bool PgUserEvent::DoAdd( SPvPLobbyUser_Base const &kUserInfo, BYTE const byEventType )
{
	auto kPair = m_kContEvent.insert( std::make_pair( kUserInfo.kCharacterGuid, SEventInfo( kUserInfo, byEventType ) ) );
	if ( !kPair.second )
	{
		switch( byEventType )
		{
		case E_EVENT_ADD:
			{
				if ( kPair.first->second.byType == E_EVENT_REMOVE )
				{// Remove -> Add 면 그냥 지워 버리면 되!
					m_kContEvent.erase( kPair.first );
					return true;
				}
			}break;
		case E_EVENT_REMOVE:
			{
				if ( kPair.first->second.byType == E_EVENT_ADD )
				{// Add -> Remove 면 그냥 지워버리면 된다.
					m_kContEvent.erase( kPair.first );
					return true;
				}
			}break;
		default:
			{
				if ( kPair.first->second.byType == E_EVENT_ADD )
				{
					// Add 면 그냥 ADD를 하면 된다.
					kPair.first->second.kUserInfo = kUserInfo;
					return true;
				}
			}break;
		}

		kPair.first->second.kUserInfo = kUserInfo;
		kPair.first->second.byType = byEventType;
	}
	return true;
}

bool PgUserEvent::WriteToPacket( BM::Stream &kPacket )const
{
//	size_t const iWRPos = kPacket.WrPos();
//	bool bModifySize = false;
	size_t iSize = m_kContEvent.size();
	kPacket.Push( iSize );

	CONT_EVENT::const_iterator itr = m_kContEvent.begin();
	for ( ; itr != m_kContEvent.end() ; ++itr )
	{
		itr->second.WriteToPacket( kPacket );
	}

// 	if ( bModifySize )
// 	{
// 		kPacket.ModifyData( iWRPos, &iSize, sizeof(size_t) );
// 	}
	return iSize > 0;
}

};// End namespace PVPUTIL