#ifndef WEAPON_VARIANT_WAR_PGPVPUTIL_H
#define WEAPON_VARIANT_WAR_PGPVPUTIL_H

#include "Lohengrin/PacketStruct2.h"

class PgPlayer;

namespace PVPUTIL
{
extern CONT_DEF_PVP_GROUNDGROUP::const_iterator IsHavePvPType(CONT_DEF_PVP_GROUNDGROUP const *pkDefPvPGroup, int const iType);
extern int	GetRandomPvPGround(CONT_DEF_PVP_GROUNDGROUP const *pkDefPvPGroup, int const iType, int const iUserCount);
extern void CopyPlayerToLobbyUser( PgPlayer const * pkPlayer, SPvPLobbyUser_Base &rkLobbyUserBase );
extern void WriteToPacket_PlayerToLobbyUser( PgPlayer const *pkPlayer, BM::Stream &rkPacket );
extern bool IsLobbyUser( SPvPLobbyUser_Base const &kLobbyUser );

extern void WriteToPacket_MapToLobby( PgPlayer * pkPlayer, BM::Stream &rkPacket );
extern void ReadFromPacket_MapToLobby( PgPlayer * pkPlayer, BM::Stream &rkPacket );

typedef enum
{
	E_EVENT_NONE			= 0x00,
	E_EVENT_ADD				= 0x01,
	E_EVENT_REMOVE			= 0x02,
	E_EVENT_JOIN_ROOM		= 0x04,
	E_EVENT_EXIT_ROOM		= 0x08,
}EEventType;

typedef std::map< BM::GUID, SPvPLobbyUser_Base >		CONT_USERLIST_CLIENT;

class PgUserEvent
{
	typedef struct tagEventInfo
	{
		tagEventInfo( SPvPLobbyUser_Base const &_kUserInfo, BYTE const _byType )
			:	kUserInfo(_kUserInfo)
			,	byType(_byType)
		{}

		void WriteToPacket( BM::Stream &kPacket )const
		{
			kPacket.Push( byType );
			switch ( byType )
			{
			case E_EVENT_ADD:
			case E_EVENT_JOIN_ROOM:// 다 Add로 주어야 한다.(게임하고 있는 놈은 못받았을 수도 있다.)
			case E_EVENT_EXIT_ROOM:
				{
					kUserInfo.WriteToPacket( kPacket );
				}break;
			case E_EVENT_REMOVE:
				{
					kPacket.Push( kUserInfo.kCharacterGuid );
				}break;
			}
		}

		BYTE				byType;
		SPvPLobbyUser_Base	kUserInfo;
	}SEventInfo;

	typedef std::map< BM::GUID, SEventInfo >						CONT_EVENT;		// second EEventType
public:
	PgUserEvent(void);
	~PgUserEvent(void){}

	bool DoAdd( SPvPLobbyUser_Base const &kUserInfo, BYTE const byEventType );
	bool WriteToPacket( BM::Stream &kPacket )const;

	size_t Size()const{return m_kContEvent.size();}
	void Clear(void);
	void Swap( PgUserEvent &rhs );
	void Merge( PgUserEvent const &rhs );

private:
	CONT_EVENT	m_kContEvent;
};

};// End namespace PVPUTIL

#endif // WEAPON_VARIANT_WAR_PGPVPUTIL_H