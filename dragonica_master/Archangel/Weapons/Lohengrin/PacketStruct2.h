#ifndef WEAPON_LOHENGRIN_PACKET_PACKETSTRUCT2_H
#define WEAPON_LOHENGRIN_PACKET_PACKETSTRUCT2_H

#include "dbtables.h"
#include "../Weapons/Variant/PgConstellation.h"

#pragma pack(1)

// --------------- Notice ------------------
typedef enum
{
	NOTICE_ALL,
	NOTICE_GUILD,
}E_NOTICE_TYPE;

typedef enum 
{
	E_NoticeMsg_None = 0,
	E_NoticeMsg_SendPacket,
	E_NoticeMsg_Add,
	E_NoticeMsg_UserLogin,
	E_NoticeMsg_GuildChatMsg,
	E_NoticeMsg_TradeChatMsg,
} ENotic_MsgType;

typedef enum
{
	E_CouponEvnet_None = 0,
	E_CouponEvent_TakeCoupon,
	E_CouponEvent_UpdateFailed,
	E_CouponEvent_ReadFromPacket,
} ECouponEvent_MsgType;

typedef enum
{
	E_TRANSTOWER_SAVEPOS,// 포지션 저장
	E_TRANSTOWER_MAPMOVE,// 맵이동
	E_TRANSTOWER_OPENMAP,// 맵오픈
} ETransTower_FuncType;;

// --------------------PvP------------------

enum ePvPMode
{
	PVP_MODE_TEAM				= 0,
	PVP_MODE_PERSONAL			= 1,
	PVP_MODE_MAX				= 2,
};
typedef BYTE EPVPMODE;

enum ePvPType
{
	PVP_TYPE_NONE				= 0x0000,
	PVP_TYPE_DM					= 0x0001,	// (1)		사투전
	PVP_TYPE_KTH				= 0x0002,	// (2)		점령전
	PVP_TYPE_ANNIHILATION		= 0x0004,	// (4)		섬멸전
	PVP_TYPE_DESTROY			= 0x0008,	// (8)		유물파괴전
	PVP_TYPE_WINNERS			= 0x0010,	// (16)		승자전
	PVP_TYPE_WINNERS_TEAM		= 0x0020,	// (32)		팀승자전
	PVP_TYPE_ACE		        = 0x0040,	// (64)		대장전
	PVP_TYPE_LOVE				= 0x0080,	// (128)	러브러브 대작전

	PVP_TYPE_ALL				= 0x00FF,	// (255)	
	// PvP는 0x00FF를 넘지 말자 

	WAR_TYPE_DESTROYCORE		= 0x0100,	// (256)	코어파괴전
	WAR_TYPE_PROTECTDRAGON		= 0x0200,	// (512)	드래곤 방어전
	WAR_TYPE_ALL				= 0xFF00,	// (65280)
};
typedef WORD EPVPTYPE;

typedef enum
{
	GAME_DRAW			= 0,
	GAME_WIN			= 1,
	GAME_LOSE			= 2,
	GAME_DRAW_EXERCISE	= 3,
	GAME_WIN_EXERCISE	= 4,
	GAME_LOSE_EXERCISE	= 5,
	////////////////////////////

	GAME_POINT		= 10, //Win타입 아님. 세이브 때문에 선언
	GAME_KILL		= 11,
	GAME_DEATH		= 12,
}EWin;

typedef enum : int
{
	TEAM_NONE		= 0,
	TEAM_RED		= 1,
	TEAM_BLUE		= 2,
	TEAM_MAX		= 3,
	TEAM_ATTACKER	= TEAM_RED,
	TEAM_DEFENCER	= TEAM_BLUE,

	TEAM_PERSONAL_CHK	= 100,
	TEAM_PERSONAL_BEGIN	= 101,
	TEAM_PERSONAL_END	= 255,// 현재 255를 넘으면 안된다.
}ETeam;

typedef enum : BYTE
{
	PS_EMPTY					= 0x00,
	PS_READY					= 0x01,
	PS_PLAYING					= 0x02,
	PS_MASTER					= 0x04,
	PS_OBSERVER					= 0x08,

	PS_DEAD						= 0x10,

	PS_POSSIBLESTART_AND_FLAG	= PS_READY|PS_MASTER,
	PS_STARTGAME_AND_FLAG		= PS_PLAYING,
	PS_NOTCHANGE_TEAM_FALG		= PS_READY|PS_PLAYING,
	PS_RELOADROOM_AND_FLAG		= 0xFC,//~(PS_READY|PS_PLAYING),
}EPlayerStatus;

typedef enum : int
{
	PVP_JOIN_SUCCEED			= 0x00,
	PVP_JOIN_PASSWORD_EMPTY,
	PVP_JOIN_PLAYING			= 200102,//게임중이라 들어올 수 없음
	PVP_JOIN_ERROR				= 200103,
	PVP_JOIN_FULL_SLOT			= 200104,
	PVP_JOIN_PASSWORD_FAILED	= 200105,
	PVP_JOIN_LEVELLIMIT			= 200106,
	PVP_JOIN_BALANCECLASS		= 200108,// ClassBallnce가 맞지 않는다.
	PVP_JOIN_MIN_TEAM			= 200156,// 인원이 부족합니다.
	PVP_JOIN_MINIMUM_TEAM		= 200157,// 팀원이 최소 2:2부터 시작이 가능합니다.
}EPvPRoomJoinRet;

size_t const PVP_ROOM_NAME_MAX_LENS = 100;
size_t const PVP_ROOM_PASSWORD_LENS = 4;
size_t const PVP_INVITE_MAX_USER_COUNT = 10;// PvP에서 최대 초대 허용 숫자
int const PVP_CP_DM_WIN_SCORE = 1000;
int const PVP_CP_DM_LOSE_SCORE = 200;
int const PVP_CP_ANNIHILATION_WIN_SCORE = 1500;
int const PVP_CP_ANNIHILATION_LOSE_SCORE = 300;
int const PVP_LOSE_SCORE = 200;

typedef struct tagPvPResult
{
	tagPvPResult()
		:	usKillCount(0),	usDieCount(0),	iPoint(0),	iCapturePoint(0), fCaptureAccm(0.0f)
	{}

	tagPvPResult( tagPvPResult const &rhs )
		:	usKillCount(rhs.usKillCount)
		,	usDieCount(rhs.usDieCount)
		,	iPoint(rhs.iPoint)
		,	iCapturePoint(rhs.iCapturePoint)
		,	fCaptureAccm(rhs.fCaptureAccm)
	{}

	void Clear()
	{
		usKillCount = 0;
		usDieCount = 0;
		iPoint = 0;
		iCapturePoint = 0;
		fCaptureAccm = 0.0f;
	}

	void Dead( int const iMinusPoint )
	{	
		++usDieCount;
		AddPoint(-iMinusPoint);
	}

	void Kill( int const iAddPoint )
	{
		++usKillCount;
		AddPoint(iAddPoint);
	}

	void SetPoint( int const iSetPoint )
	{
		if( 0 <= iSetPoint )
		{
			iPoint = iSetPoint;
		}
	}

	void AddPoint( int const iAddPoint )
	{
		if ( iAddPoint )
		{
			iPoint += iAddPoint;
			if ( 0 > iPoint )
			{
				iPoint = ( 0 < iAddPoint ) ? INT_MAX : 0;
			}
		}
	}

	bool operator<( tagPvPResult const &rhs ) const
	{
		if ( iPoint != rhs.iPoint )
		{
			return iPoint < rhs.iPoint;
		}

		if ( usKillCount != rhs.usKillCount )
		{
			return usKillCount < rhs.usKillCount;
		}

		if( iCapturePoint != rhs.iCapturePoint )
		{
			return iCapturePoint < rhs.iCapturePoint;
		}

		return usDieCount > rhs.usDieCount;
	}

	bool operator>( tagPvPResult const &rhs ) const
	{
		if ( iPoint != rhs.iPoint )
		{
			return iPoint > rhs.iPoint;
		}

		if ( usKillCount != rhs.usKillCount )
		{
			return usKillCount > rhs.usKillCount;
		}

		if( iCapturePoint != rhs.iCapturePoint )
		{
			return iCapturePoint > rhs.iCapturePoint;
		}

		return usDieCount < rhs.usDieCount;
	}

	bool operator==( tagPvPResult const &rhs ) const
	{
		return ( iPoint == rhs.iPoint ) && ( usKillCount == rhs.usKillCount ) 
			&& ( usDieCount == rhs.usDieCount ) && ( iCapturePoint == rhs.iCapturePoint );
	}


	size_t min_size()const
	{
		return 
			sizeof(usKillCount)+
			sizeof(usDieCount)+
			sizeof(iPoint)+
			sizeof(iCapturePoint);
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(usKillCount);
		kPacket.Push(usDieCount);
		kPacket.Push(iPoint);
		kPacket.Push(iCapturePoint);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(usKillCount);
		kPacket.Pop(usDieCount);
		kPacket.Pop(iPoint);
		kPacket.Pop(iCapturePoint);
	}

	WORD	usKillCount;	// 죽인 수
	WORD	usDieCount;		// 죽은 수
	int		iPoint;			// 획득 점수
	int		iCapturePoint;	// 진지를 점령할 때 얻는 점수.
	float	fCaptureAccm;	// 진지 점령을 할 때 부동소수점 단위 계산용.(패킷에 포함 안함.)
}SPvPResult;

typedef struct tagPvPReport	
{
	tagPvPReport()
	{
		Clear();
	}

	virtual ~tagPvPReport()
	{
		Clear();
	}

	tagPvPReport& operator=( tagPvPReport const &rhs )
	{
		::memcpy( m_iRecords, rhs.m_iRecords, sizeof(m_iRecords) );
		m_iKill = rhs.m_iKill;
		m_iDeath = rhs.m_iDeath;
		m_iPoint = rhs.m_iPoint;
		return *this;
	}

	void Clear()
	{	
		::memset( m_iRecords, 0, sizeof(m_iRecords) );
		m_iKill=0;
		m_iDeath=0;
		m_iPoint=0;
	}

	void Refresh()
	{
		for ( int i=0; i<6; ++i )
		{
			if ( m_iRecords[i] < 0 )
			{
				m_iRecords[i] = INT_MAX;
			}
		}

		if ( m_iKill < 0 )	m_iKill = _I64_MAX;
		if ( m_iDeath < 0 )	m_iDeath = _I64_MAX;
		if ( m_iPoint < 0 )	m_iPoint = INT_MAX;
	}

	void Add( SPvPResult const &kResult )
	{
		m_iKill += (__int64)kResult.usKillCount;
		m_iDeath += (__int64)kResult.usDieCount;
		Refresh();
	}

	void Mearge( tagPvPReport const &rhs )
	{
		for ( int i=0; i<6; ++i )
		{
			m_iRecords[i] += rhs.m_iRecords[i];
		}
		m_iKill += rhs.m_iKill;
		m_iDeath += rhs.m_iDeath;
		m_iPoint += rhs.m_iPoint;
		Refresh();
	}

	double GetWinAvg( bool const bIsExercise )const
	{
		unsigned int const uiWin = static_cast<unsigned int>(GetRecord(GAME_WIN, bIsExercise));
		unsigned int const uiTotal = uiWin + static_cast<unsigned int>(GetRecord(GAME_LOSE, bIsExercise));
		if ( uiWin )
		{
			return static_cast<double>(uiTotal / uiWin);
		}
		return 0.0;
	}

	int GetRecord( EWin const kType, bool const bIsExercise )const
	{
		if ( GAME_LOSE < kType )
		{
			return 0;
		}

		if ( true == bIsExercise )
		{
			return m_iRecords[kType+GAME_DRAW_EXERCISE];
		}
		return m_iRecords[kType];
	}

	void SetPvPRecord( WORD const wType, __int64 const i64Value )
	{
		switch(wType)
		{
		case GAME_DRAW:
		case GAME_WIN:
		case GAME_LOSE:
		case GAME_DRAW_EXERCISE:
		case GAME_WIN_EXERCISE:
		case GAME_LOSE_EXERCISE:
			{
				m_iRecords[wType] = static_cast<int>(i64Value);
			}break;
		case GAME_POINT:
			{
				m_iPoint = static_cast<int>(i64Value);
			}break;
		case GAME_KILL:
			{
				m_iKill = i64Value;
			}break;
		case GAME_DEATH:
			{
				m_iDeath = i64Value;
			}break;
		}
	}

	DEFAULT_TBL_PACKET_FUNC();

	int			m_iRecords[6];
	__int64		m_iKill, m_iDeath;
	int			m_iPoint;
}SPvPReport;

struct SDoc_PvPReport
{
	SDoc_PvPReport(SPvPReport &kPvpReport)
		:m_kPvpReport(kPvpReport)
	{
	}
	SPvPReport &m_kPvpReport;
};

typedef struct tagPvPTeamSlot
{
	tagPvPTeamSlot( BYTE _kTeam=TEAM_NONE, BYTE _kSlot=0)
		:	kTeam(_kTeam),	kSlot(_kSlot)
	{}

	bool operator<( tagPvPTeamSlot const &rhs )const
	{
		if ( kSlot == rhs.kSlot )
		{
			return kTeam < rhs.kTeam;
		}
		return kSlot < rhs.kSlot;
	}

	bool operator>( tagPvPTeamSlot const &rhs )const
	{
		if ( kSlot == rhs.kSlot )
		{
			return kTeam > rhs.kTeam;
		}
		return kSlot > rhs.kSlot;
	}

	bool operator==( tagPvPTeamSlot const &rhs )const
	{
		return (kTeam == rhs.kTeam) && kSlot == rhs.kSlot; 
	}

	void Clear()
	{
		kTeam = TEAM_NONE;
		kSlot = 0;
	}

	void SetTeam( int const _iTeam ){kTeam = (BYTE)_iTeam;}
	int GetTeam()const{return (int)kTeam;}
	void SetSlot( BYTE const _kSlot ){kSlot=_kSlot;}
	int GetSlot()const{return (int)kSlot;}

	bool IsCorrect( bool const bExercise )const
	{
		if ( true == bExercise )
		{
			return 10 > kSlot;
		}
		return 4 > kSlot;
	}

	void SetEnemySlot()
	{
		if ( TEAM_RED == kTeam )
		{
			kTeam = TEAM_BLUE;
		}
		else
		{
			kTeam = TEAM_RED;
		}
	}

	BYTE			kTeam;
	BYTE			kSlot;
	DEFAULT_TBL_PACKET_FUNC();
}SPvPTeamSlot;

typedef struct tagPvPLobbyUser_Base
{
	tagPvPLobbyUser_Base()
		:	iRoomIndex(-1)
		,	sLevel(0)
		,	iClass(0)
	{
	}

	tagPvPLobbyUser_Base( BM::GUID const &kGuid )
		:	iRoomIndex(-1)
		,	sLevel(0)
		,	iClass(0)
		,	kCharacterGuid(kGuid)
	{}

	tagPvPLobbyUser_Base( tagPvPLobbyUser_Base const &rhs )
		:	iRoomIndex(rhs.iRoomIndex)
		,	sLevel(rhs.sLevel)
		,	iClass(rhs.iClass)
		,	wstrName(rhs.wstrName)
		,	kGuildGuid(rhs.kGuildGuid)
		,	kCharacterGuid(rhs.kCharacterGuid)
		,	kPvPReport(rhs.kPvPReport)
	{
	}

	void Clear()
	{
		iRoomIndex = -1;
		sLevel = 0;
		iClass = 0;
		wstrName.clear();
		kCharacterGuid.Clear();
		kPvPReport.Clear();
		kGuildGuid.Clear();
	}

	size_t min_size()const
	{
		return 
			sizeof(iRoomIndex)+
			sizeof(kCharacterGuid)+
			sizeof(sLevel)+
			sizeof(iClass)+
			sizeof(size_t)+//wstrName);
			sizeof(kGuildGuid)+
			kPvPReport.min_size();
	}

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kPacket.Push(iRoomIndex);
		kPacket.Push(kCharacterGuid);
		kPacket.Push(sLevel);
		kPacket.Push(iClass);
		kPacket.Push(wstrName);
		kPacket.Push(kGuildGuid);
		kPvPReport.WriteToPacket(kPacket);
	}

	void ReadFromPacket( BM::Stream &kPacket )
	{
		kPacket.Pop(iRoomIndex);
		kPacket.Pop(kCharacterGuid);
		kPacket.Pop(sLevel);
		kPacket.Pop(iClass);
		kPacket.Pop(wstrName);
		kPacket.Pop(kGuildGuid);
		kPvPReport.ReadFromPacket(kPacket);
	}

	int				iRoomIndex;
	int				iClass;
	short			sLevel;
	std::wstring	wstrName;
	BM::GUID		kCharacterGuid;
	BM::GUID		kGuildGuid;
	SPvPReport		kPvPReport;
}SPvPLobbyUser_Base;// Client에서 사용

// typedef struct tagPvPLobbyUser
// :	public tagPvPLobbyUser_Simple
// {
// 	typedef enum
// 	{
// 		E_WR_ALL,
// 		E_WR_SIMPLE,
// 		E_WR_UPDATE,
// 	}EWriteType;
// 
// 	tagPvPLobbyUser()
// 		:	iMaxHP(0)
// 		,	iMaxMP(0)
// 		,	iMP(0)
// 		,	iHP(0)
// 		,	byGMLevel(0)
// 		,	dwLastCommandTime(0)
// 	{
// 	}
// 
// 	tagPvPLobbyUser( tagPvPLobbyUser const &rhs )
// 		:	tagPvPLobbyUser_Simple(rhs)
// 		,	iMaxHP(rhs.iMaxHP)
// 		,	iMaxMP(rhs.iMaxMP)
// 		,	iHP(rhs.iHP)
// 		,	iMP(rhs.iMP)
// 		,	kGuildGuid(rhs.kGuildGuid)
// 		,	kCoupleGuid(rhs.kCoupleGuid)
// 		,	byGMLevel(rhs.byGMLevel)
// 		,	dwLastCommandTime(rhs.dwLastCommandTime)
// 	{
// 	}
// 
// 	void Clear()
// 	{
// 		tagPvPLobbyUser_Simple::Clear();
// 		iMaxHP = 0;
// 		iMaxMP = 0;
// 		iMP = 0;
// 		iHP = 0;
// 		kGuildGuid.Clear();
// 		kCoupleGuid.Clear();
// 		byGMLevel = 0;
// 		dwLastCommandTime = 0;
// 	}
// 
// 	size_t min_size( BYTE const byType)const
// 	{
// 		size_t ret_size = 0;
// 		switch( byType )
// 		{
// 		case E_WR_ALL:
// 			{
// 				ret_size += sizeof(iMaxHP);
// 				ret_size += sizeof(iMaxMP);
// 				ret_size += sizeof(iHP);
// 				ret_size += sizeof(iMP);
// 				ret_size += sizeof(kGuildGuid);
// 				ret_size += sizeof(kCoupleGuid);
// 				ret_size += sizeof(byGMLevel);
// 			}// no break
// 		case E_WR_SIMPLE:
// 			{
// 				ret_size += tagPvPLobbyUser_Simple::min_size();
// 			}break;
// 		case E_WR_UPDATE:
// 			{
// 				ret_size += kPvPReport.min_size();
// 			}break;
// 		}
// 		return ret_size;
// 	}
// 
// 	void WriteToPacket( BM::Stream &kPacket, BYTE const byType )const
// 	{
// 		switch( byType )
// 		{
// 		case E_WR_ALL:
// 			{
// 				kPacket.Push(iMaxHP);
// 				kPacket.Push(iMaxMP);
// 				kPacket.Push(iHP);
// 				kPacket.Push(iMP);
// 				kPacket.Push(kGuildGuid);
// 				kPacket.Push(kCoupleGuid);
// 				kPacket.Push(byGMLevel);
// 			}// no break
// 		case E_WR_SIMPLE:
// 			{
// 				tagPvPLobbyUser_Simple::WriteToPacket( kPacket );
// 			}break;
// 		case E_WR_UPDATE:
// 			{
// 				kPacket.Push( kPvPReport );
// 			}break;
// 		}
// 	}
// 
// 	void ReadFromPacket( BM::Stream &kPacket, BYTE const byType )
// 	{
// 		switch( byType )
// 		{
// 		case E_WR_ALL:
// 			{
// 				kPacket.Pop(iMaxHP);
// 				kPacket.Pop(iMaxMP);
// 				kPacket.Pop(iHP);
// 				kPacket.Pop(iMP);
// 				kPacket.Pop(kGuildGuid);
// 				kPacket.Pop(kCoupleGuid);
// 				kPacket.Pop(byGMLevel);
// 			}// no break
// 		case E_WR_SIMPLE:
// 			{
// 				tagPvPLobbyUser_Simple::ReadFromPacket( kPacket );;
// 			}break;
// 		case E_WR_UPDATE:
// 			{
// 				kPacket.Pop( kPvPReport );
// 			}break;
// 		}
// 	}
// 
// 	
// 
// 	DWORD			dwLastCommandTime;//마지막 커맨드 명령시간(서버에 부하를 없애기 위해 : 초대기능같은경우 계속 할 수도 있다.)
// }SPvPLobbyUser;

typedef struct tagPvPChannelInfo
{
	tagPvPChannelInfo()
		:	nChannelNo(0)
		,	iMinLevel(0)
		,	iMaxLevel(0)
		,	iNowPlayerCount(0)
		,	iMaxPlayerCount(0)
	{}

	short	nChannelNo;
	int		iMinLevel : 16;
	int		iMaxLevel : 16;
	int		iNowPlayerCount : 16;
	int		iMaxPlayerCount : 16;
}SPvPChannelInfo;

typedef struct tagCashShopUser
{
	tagCashShopUser()
	{}

	BM::GUID		kMemberGuid;
	SERVER_IDENTITY	kSwitchServer;

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kPacket.Push( kMemberGuid );
		kSwitchServer.WriteToPacket( kPacket );
	}

	void ReadFromPacket( BM::Stream &kPacket )
	{
		kPacket.Pop( kMemberGuid );
		kSwitchServer.ReadFromPacket( kPacket );
	}
}SCashShopUser;

typedef struct tagPvPGameUser
:	public tagPvPLobbyUser_Base
{
	tagPvPGameUser()
	{
		Clear();
	}

	void Clear()
	{
		SPvPLobbyUser_Base::Clear();
		iMaxHP = 0;
		iMaxMP = 0;
		iHP = 0;
		iMP = 0;
		kGuildGuid = BM::GUID::NullData();
		kCoupleGuid = BM::GUID::NullData();
		byGMLevel = 0;
		kTeamSlot.Clear();
//		iHandyCap = 0;
		byStatus = PS_EMPTY;
		byEntryNo = 0;

		kResult.Clear();

		wstrUI.clear();
	}

	int				iMaxHP;
	int				iMaxMP;
	int				iHP;
	int				iMP;
	BM::GUID		kCoupleGuid;
	BYTE			byGMLevel;

	SPvPTeamSlot	kTeamSlot;
//	int				iHandyCap;// 핸디캡 값
	BYTE			byStatus;
	BYTE			byEntryNo;

	//
	SPvPResult		kResult;
	
	// No Packet
	std::wstring	wstrUI;

	size_t min_size()const
	{
		return
			SPvPLobbyUser_Base::min_size()+
			sizeof(iMaxHP)+
			sizeof(iMaxMP)+
			sizeof(iHP)+
			sizeof(iMP)+
			sizeof(kCoupleGuid)+
			sizeof(byGMLevel)+
			kTeamSlot.min_size()+
//			sizeof(iHandyCap)+
			sizeof(byStatus)+
			sizeof(byEntryNo);
	}

	void WriteToPacket( BM::Stream &kPacket )const
	{
		SPvPLobbyUser_Base::WriteToPacket( kPacket );
		kPacket.Push( iMaxHP );
		kPacket.Push( iMaxMP );
		kPacket.Push( iHP );
		kPacket.Push( iMP );
		kPacket.Push( kCoupleGuid );
		kPacket.Push( byGMLevel );
		kTeamSlot.WriteToPacket(kPacket);
//		kPacket.Push(iHandyCap);
		kPacket.Push(byStatus);
		kPacket.Push(byEntryNo);
	}

	bool ReadFromPacket( BM::Stream &kPacket )
	{
		SPvPLobbyUser_Base::ReadFromPacket( kPacket );
		kPacket.Pop( iMaxHP );
		kPacket.Pop( iMaxMP );
		kPacket.Pop( iHP );
		kPacket.Pop( iMP );
		kPacket.Pop( kCoupleGuid );
		kPacket.Pop( byGMLevel );
		kTeamSlot.ReadFromPacket(kPacket);
//		kPacket.Pop(iHandyCap);
		kPacket.Pop(byStatus);
		return kPacket.Pop(byEntryNo);
	}

}SPvPGameUser;

typedef struct tagPvPTeamEntry
{
	tagPvPTeamEntry( BYTE _kTeam=TEAM_NONE, BYTE _kEntryNo=0)
		:	kTeam(_kTeam),	kEntryNo(_kEntryNo)
	{}

	bool operator<( tagPvPTeamEntry const &rhs )const
	{
		if ( kEntryNo == rhs.kEntryNo )
		{
			return kTeam < rhs.kTeam;
		}
		return kEntryNo < rhs.kEntryNo;
	}

	bool operator>( tagPvPTeamEntry const &rhs )const
	{
		if ( kEntryNo == rhs.kEntryNo )
		{
			return kTeam > rhs.kTeam;
		}
		return kEntryNo > rhs.kEntryNo;
	}

	bool operator==( tagPvPTeamEntry const &rhs )const
	{
		return (kTeam == rhs.kTeam) && kEntryNo == rhs.kEntryNo; 
	}

	void Clear()
	{
		kTeam = TEAM_NONE;
		kEntryNo = 0;
	}

	void SetTeam( int const _iTeam ){kTeam = (BYTE)_iTeam;}
	int GetTeam()const{return (int)kTeam;}
	void SetEntry( BYTE const _kEntryNo ){kEntryNo=_kEntryNo;}
	int GetEntry()const{return (int)kEntryNo;}

	BYTE			kTeam;
	BYTE			kEntryNo;
	DEFAULT_TBL_PACKET_FUNC();
}SPvPTeamEntry;

typedef std::map< BM::GUID, SPvPGameUser>	CONT_PVP_GAME_USER;
typedef std::set< SPvPTeamSlot >				CONT_PVP_SLOT;
typedef std::map< TBL_KEY_INT, SPvPResult >		CONT_PVP_TEAM_RESULT;
typedef std::set< SPvPTeamEntry >			CONT_PVP_ENTRY;
typedef std::map< std::string, BM::GUID >	CONT_PVP_TRIGGER_ENTITY;
typedef std::map< BM::GUID, SPvPResult >	CONT_SEND_PVP_POINT;

// PvP Reward
typedef struct tagPvPReward
{
	tagPvPReward()
	{
		Clear();
	}

	tagPvPReward( BM::GUID const &_kCharGuid, SPvPResult const &_kResult, BYTE const _kWinLose=GAME_DRAW )
		:	kCharGuid(_kCharGuid)
		,	kResult(_kResult)
		,	kWinLose(_kWinLose)
		,	kRank(0)
		,	kSpecialPoint(0)
		,	kRankPoint(0)
		,	kWinPoint(0)
		,	iCP(0)
	{
	}

	bool operator<( tagPvPReward const &rhs )const{return kResult < rhs.kResult;}
	bool operator>( tagPvPReward const &rhs )const{return kResult > rhs.kResult;}
	bool operator==( tagPvPReward const &rhs )const{return kResult == rhs.kResult;}

	void Clear()
	{
		kCharGuid.Clear();
		kResult.Clear();
		kRank = 0;
		kWinLose = GAME_DRAW;
		kSpecialPoint = 0;
		kRankPoint = 0;
		kWinPoint = 0;
		iCP = 0;
		kContItem.clear();
	}

	size_t min_size()const
	{
		return 
			sizeof(kCharGuid)+
			kResult.min_size()+
			sizeof(kRank)+
			sizeof(kWinLose)+
			sizeof(kSpecialPoint)+
			sizeof(kRankPoint)+
			sizeof(kWinPoint)+
			sizeof(iCP)+
			sizeof(size_t);//kContItem);
	}

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kPacket.Push(kCharGuid);
		kResult.WriteToPacket(kPacket);
		kPacket.Push(kRank);
		kPacket.Push(kWinLose);
		kPacket.Push(kSpecialPoint);
		kPacket.Push(kRankPoint);
		kPacket.Push(kWinPoint);
		kPacket.Push(iCP);
		kPacket.Push(kContItem);
	}

	void ReadFromPacket( BM::Stream &kPacket )
	{
		kPacket.Pop(kCharGuid);
		kResult.ReadFromPacket(kPacket);
		kPacket.Pop(kRank);
		kPacket.Pop(kWinLose);
		kPacket.Pop(kSpecialPoint);
		kPacket.Pop(kRankPoint);
		kPacket.Pop(kWinPoint);
		kPacket.Pop(iCP);
		kContItem.clear();
		kPacket.Pop(kContItem);
	}

	int GetTotalPoint()const{return kResult.iPoint + static_cast<int>(kRankPoint) + static_cast<int>(kWinPoint) + static_cast<int>(kSpecialPoint); }

	BM::GUID					kCharGuid;	
	SPvPResult					kResult;
	BYTE						kRank;
	BYTE						kWinLose;
	WORD						kSpecialPoint;
	WORD						kRankPoint;
	WORD						kWinPoint;
	int							iCP;
	REWARD_ITEM_LIST			kContItem;
} SPvPReward;
typedef std::vector<SPvPReward>						CONT_PVP_GAME_REWARD;

// PvP Room---->
typedef enum : BYTE
{
	ROOM_STATUS_CLOSE			= 0x00,		// 생성되지 않은 방
	ROOM_STATUS_LOBBY			= 0x01,		// 대기실 방
	ROOM_STATUS_READY			= 0x02,
	ROOM_STATUS_PLAY			= 0x04,
	ROOM_STATUS_RESULT			= 0x08,
	ROOM_STATUS_READY_EXITUSER	= 0x10,		// 게임 레디중에 유저가 나갔음
} EPvPRoomStatus;

typedef struct tagPvPRoomBasicInfo
{
	tagPvPRoomBasicInfo( BYTE const kStatus=ROOM_STATUS_LOBBY )
		:	m_kStatus(kStatus)
		,	m_ucNowUser(0)
		,	m_ucMaxUser(0)
		,	m_iGameTime(0)
		,	m_iGamePoint(0)
		,	m_kMode(PVP_MODE_TEAM)
		,	m_kType(PVP_TYPE_NONE)
		,	m_iGndNo(0)
		,	m_bPwd(false)
		,	m_sLevelLimit_Min(1)
		,	m_sLevelLimit_Max(100)
		,	m_iRoomIndex(0)
	{}

	tagPvPRoomBasicInfo( tagPvPRoomBasicInfo const &rhs )
	{
		*this = rhs;
	}

	tagPvPRoomBasicInfo& operator=( tagPvPRoomBasicInfo const &rhs )
	{
		m_kStatus = rhs.m_kStatus;
		m_wstrName = rhs.m_wstrName;
		m_ucNowUser = rhs.m_ucNowUser;
		m_ucMaxUser = rhs.m_ucMaxUser;
		m_iGameTime = rhs.m_iGameTime;
		m_iGamePoint = rhs.m_iGamePoint;
		m_kMode = rhs.m_kMode;
		m_kType = rhs.m_kType;
		m_iGndNo = rhs.m_iGndNo;
		m_bPwd = rhs.m_bPwd;
		m_sLevelLimit_Min = rhs.m_sLevelLimit_Min;
		m_sLevelLimit_Max = rhs.m_sLevelLimit_Max;
		m_iRoomIndex = rhs.m_iRoomIndex;
		m_kBattleGuid = rhs.m_kBattleGuid;
		return *this;
	}

	void Clear()
	{
		m_kStatus = ROOM_STATUS_CLOSE;
		m_wstrName.clear();
		m_ucNowUser = 0;
		m_ucMaxUser	= 0;
		m_iGameTime = 0;
		m_iGamePoint = 0;
		m_kMode = PVP_MODE_TEAM;
		m_kType = PVP_TYPE_NONE;
		m_iGndNo = 0;
		m_bPwd = false;
		m_sLevelLimit_Min = 1;
		m_sLevelLimit_Max = 100;
		m_iRoomIndex = 0;
		m_kBattleGuid.Clear();
	}

	void SetRoomInfo( tagPvPRoomBasicInfo const &kInfo )
	{// 
//		m_kStatus = kInfo.m_kStatus;
		m_wstrName = kInfo.m_wstrName;
//		m_ucNowUser = kInfo.m_ucNowUser;
		m_ucMaxUser	= kInfo.m_ucMaxUser;
		m_iGameTime = kInfo.m_iGameTime;
		m_iGamePoint = kInfo.m_iGamePoint;
		m_kMode = kInfo.m_kMode;
		m_kType = kInfo.m_kType;
		m_iGndNo = kInfo.m_iGndNo;
//		m_bPwd = kInfo.m_bPwd;
//		m_iRoomIndex = kInfo.m_iRoomIndex;
		m_sLevelLimit_Min = kInfo.m_sLevelLimit_Min;
		m_sLevelLimit_Max = kInfo.m_sLevelLimit_Max;
		m_kBattleGuid = kInfo.m_kBattleGuid;
	}

	size_t min_size()const
	{
		return sizeof(m_kStatus);
	}

	bool IsHaveLimitLevel()const
	{ // min, max 둘다 0일 경우 제한이 없는 것으로 한다.
		return (m_sLevelLimit_Min + m_sLevelLimit_Max) ? true : false;
	}

	bool IsHavePvPType(int const Type)
	{ // PVP 타입을 검사한다.
		return (m_kType & Type) ? true : false;
	}

	void WriteToPacket( BM::Stream& rkPacket )const
	{
		rkPacket.Push(m_kStatus);
		if ( m_kStatus != ROOM_STATUS_CLOSE )
		{
			rkPacket.Push(m_wstrName);
			rkPacket.Push(m_ucNowUser);
			rkPacket.Push(m_ucMaxUser);
			rkPacket.Push(m_iGameTime);
			rkPacket.Push(m_iGamePoint);
			rkPacket.Push(m_kMode);
			rkPacket.Push(m_kType);
			rkPacket.Push(m_iGndNo);
			rkPacket.Push(m_bPwd);
			rkPacket.Push(m_sLevelLimit_Min);
			rkPacket.Push(m_sLevelLimit_Max);
			rkPacket.Push(m_iRoomIndex);
			rkPacket.Push(m_kBattleGuid);
		}
	}

	bool ReadFromPacket( BM::Stream& rkPacket )
	{
		if ( true == rkPacket.Pop(m_kStatus) )
		{
			if ( m_kStatus != ROOM_STATUS_CLOSE )
			{
				if ( true == rkPacket.Pop( m_wstrName, PVP_ROOM_NAME_MAX_LENS ) )
				{
					rkPacket.Pop(m_ucNowUser);
					rkPacket.Pop(m_ucMaxUser);
					rkPacket.Pop(m_iGameTime);
					rkPacket.Pop(m_iGamePoint);
					rkPacket.Pop(m_kMode);
					rkPacket.Pop(m_kType);
					rkPacket.Pop(m_iGndNo);
					rkPacket.Pop(m_bPwd);
					rkPacket.Pop(m_sLevelLimit_Min);
					rkPacket.Pop(m_sLevelLimit_Max);
					rkPacket.Pop(m_iRoomIndex);
					return rkPacket.Pop(m_kBattleGuid);
				}	
			}
			Clear();
		}
		return false;
	}

	bool IsColsed()const{return m_kStatus == ROOM_STATUS_CLOSE;}

	BYTE			m_kStatus;		// 방의 상태
	std::wstring	m_wstrName;		// 방의 이름
	BYTE			m_ucNowUser;	// 현재 유저 숫자
	BYTE			m_ucMaxUser;	// 최대 유저 숫자
	int				m_iGameTime;	// 게임 시간
	int				m_iGamePoint;	// 게임 점수
	EPVPMODE		m_kMode;		// 게임 모드
	EPVPTYPE		m_kType;		// 게임 타입
	int				m_iGndNo;		// 맵번호
	bool			m_bPwd;			// 패스워드가 있느냐?
	short			m_sLevelLimit_Min;// 최소 입장레벨
	short			m_sLevelLimit_Max;// 최대 입장레벨
	int				m_iRoomIndex;	// 방의 고유 인덱스 번호
	BM::GUID    m_kBattleGuid;
}SPvPRoomBasicInfo;
typedef std::map< unsigned int, SPvPRoomBasicInfo, std::greater<unsigned int> >	CONT_PVPROOM_LIST;
typedef std::vector< SPvPRoomBasicInfo > CONT_ROOM_INFO;

typedef enum ePvPExtOption
{
	E_PVP_OPT_NONE				= 0x00,
	E_PVP_OPT_USEHANDYCAP		= 0x01,
	E_PVP_OPT_USEITEM			= 0x02,
	E_PVP_OPT_DISABLEDASHJUMP	= 0x04,
	E_PVP_OPT_USEBATTLELEVEL	= 0x08,
}EPvPExtOption;

typedef struct tagPvPRoomExtInfo
{
	tagPvPRoomExtInfo()
		:	ucRound(1)
		,	kOption(E_PVP_OPT_NONE)
	{}

	void Clear()
	{
		strPassWord.clear();
		ucRound = 1;
		kOption = E_PVP_OPT_NONE;
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(strPassWord);
		kPacket.Push(ucRound);
		kPacket.Push(kOption);
	}

	bool ReadFromPacket(BM::Stream &kPacket)
	{
		if ( true == kPacket.Pop( strPassWord, PVP_ROOM_PASSWORD_LENS ) )
		{
			kPacket.Pop(ucRound);
			return kPacket.Pop(kOption);
		}
		return false;
	}

	bool IsUseHandyCap()const{return E_PVP_OPT_USEHANDYCAP & kOption;}
	bool IsUseItem()const{return E_PVP_OPT_USEITEM & kOption;}
	bool IsDisableDashJump()const{return E_PVP_OPT_DISABLEDASHJUMP & kOption;}
	bool IsUseBattleLevel()const{return E_PVP_OPT_USEBATTLELEVEL & kOption;}

	std::string strPassWord;
	BYTE		ucRound;
	BYTE		kOption;
}SPvPRoomExtInfo;

typedef struct tagPvPRoomPage
{
	tagPvPRoomPage( EPVPTYPE const _kType=PVP_TYPE_ALL, BYTE const _iPage=0 )
		:	kType(_kType)
		,	iPage(_iPage)
	{
	}

	void Clear()
	{
		kType = PVP_TYPE_ALL;
		iPage = 0;
	}

	int GetIndex()const
	{
		return ( kType << 16 ) + iPage;
	}

	static int GetMinIndex()
	{
		return 0x10000;
	}

	bool operator<( tagPvPRoomPage const &rhs )const
	{
		if ( kType == rhs.kType )
		{
			return iPage > rhs.iPage;// 꺼꾸로
		}
		return kType > rhs.kType;//꺼꾸로
	}

	EPVPTYPE	kType;
	BYTE		iPage;
}SPvPRoomPage;
// ====================PvP==================

// =============== Emporia Battle ===============
typedef enum
{
	E_BATTLE_RESERVE_REQ,
	E_BATTLE_RESERVE_SUCCEEDED,
	E_BATTLE_RESERVE_FAILED,
	E_BATTLE_THROW_REQ,
	E_BATTLE_THROW_SUCCEEDED,
	E_BATTLE_THROW_FAILED,
	E_BATTLE_REFUNDEXP,			// 경험치 환불
}EReserveType;

typedef struct tagGuildEmporiaState
{
	tagGuildEmporiaState()
		:	byStatus(0)
		,	byHaveGuildEmblem(0)
		,	byAttackGuildEmblem(0)
		,	iReserveGuildCount(0)
		,	i64MinReserveExp(0i64)
		,	i64NextBattleTime(0i64)
		,	i64ReserveEndTime(0i64)
	{}

	BYTE			byStatus;
	SGroundKey		kEmporiaGndKey;

	std::wstring	wstrHaveGuildName;
	BYTE			byHaveGuildEmblem;

	std::wstring	wstrAttackGuildName;
	BYTE			byAttackGuildEmblem;

	size_t			iReserveGuildCount;
	__int64			i64MinReserveExp;

	__int64			i64NextBattleTime;
	__int64			i64ReserveEndTime;

	BM::GUID GetEmporiaID()const{return kEmporiaGndKey.Guid();}
	int GetEmporiaNo()const{return kEmporiaGndKey.GroundNo();}

	size_t min_size()const
	{
		return 
			sizeof(byStatus)+
			kEmporiaGndKey.min_size()+
			sizeof(size_t)+//wstrHaveGuildName );
			sizeof(byHaveGuildEmblem)+
			sizeof(size_t)+//wstrAttackGuildName );
			sizeof(byAttackGuildEmblem)+
			sizeof(iReserveGuildCount)+
			sizeof(i64MinReserveExp)+
			sizeof(i64NextBattleTime)+
			sizeof(i64ReserveEndTime);
	}

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kPacket.Push( byStatus );
		kEmporiaGndKey.WriteToPacket( kPacket );
		kPacket.Push( wstrHaveGuildName );
		kPacket.Push( byHaveGuildEmblem );
		kPacket.Push( wstrAttackGuildName );
		kPacket.Push( byAttackGuildEmblem );
		kPacket.Push( iReserveGuildCount );
		kPacket.Push( i64MinReserveExp );
		kPacket.Push( i64NextBattleTime );
		kPacket.Push( i64ReserveEndTime );
	}

	void ReadFromPacket( BM::Stream &kPacket )
	{
		kPacket.Pop( byStatus );
		kEmporiaGndKey.ReadFromPacket( kPacket );
		kPacket.Pop( wstrHaveGuildName );
		kPacket.Pop( byHaveGuildEmblem );
		kPacket.Pop( wstrAttackGuildName );
		kPacket.Pop( byAttackGuildEmblem );
		kPacket.Pop( iReserveGuildCount );
		kPacket.Pop( i64MinReserveExp );
		kPacket.Pop( i64NextBattleTime );
		kPacket.Pop( i64ReserveEndTime );
	}

}SGuildEmporiaState;
typedef std::list< SGuildEmporiaState >		CONT_EMPORIA_STATE_LIST;

typedef struct tagGuildBattleInfo
{
	tagGuildBattleInfo()
	{
		Clear();
	}

	void Clear()
	{
		nCharacterPoint = 0;
		nCorePoint = 0;
		kGuildGuid.Clear();
	}

	void Set( BM::GUID const &_kGuildGuid, std::wstring const &_wstrName )
	{
		kGuildGuid = _kGuildGuid;
		wstrName = _wstrName;
		nCharacterPoint = 0;
		nCorePoint = 0;
	}

	size_t GetPoint(void)const{return (size_t)nCharacterPoint + (size_t)nCorePoint;}
	void SetPoint( unsigned short const nPoint, bool const bCharacterPoint )
	{
		if ( bCharacterPoint )
		{
			nCharacterPoint = nPoint;
		}
		else
		{
			nCorePoint = nPoint;
		}		
	}

	void AddPoint( unsigned short const nPoint, bool const bCharacterPoint )
	{
		if ( bCharacterPoint )
		{
			unsigned short nTemp = nCharacterPoint + nPoint;
			if ( nTemp > nCharacterPoint )
			{
				nCharacterPoint = nTemp;
			}
			else
			{
				nCharacterPoint = USHRT_MAX;
			}
		}
		else
		{
			unsigned short nTemp = nCorePoint + nPoint;
			if ( nTemp > nCorePoint )
			{
				nCorePoint = nTemp;
			}
			else
			{
				nCorePoint = USHRT_MAX;
			}
		}
	}

	BM::GUID		kGuildGuid;
	std::wstring	wstrName;
	unsigned short	nCharacterPoint;
	unsigned short	nCorePoint;

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kPacket.Push( kGuildGuid );
		kPacket.Push( wstrName );
		kPacket.Push( nCharacterPoint );
		kPacket.Push( nCorePoint );
	}

	void ReadFromPacket( BM::Stream &kPacket )
	{
		kPacket.Pop( kGuildGuid );
		kPacket.Pop( wstrName );
		kPacket.Pop( nCharacterPoint );
		kPacket.Pop( nCorePoint );
	}

}SGuildBattleInfo;

typedef struct tagEmporiaBattleObjectInfo
{
	tagEmporiaBattleObjectInfo( WORD const _kUnitType=0 )
		:	kUnitType(_kUnitType)
		,	byGrade(EOGRADE_SUBCORE)
		,	iHP(1)
		,	iTeam(TEAM_NONE)
		,	iBattleAreaIndex(0)
	{}

	bool IsDead()const{return iHP<=0;}

	WORD	kUnitType;
	BYTE	byGrade;
	int		iHP;
	int		iTeam;
	int		iBattleAreaIndex;

	DEFAULT_TBL_PACKET_FUNC();
}SEmporiaBattleObjectInfo;
typedef std::map< BM::GUID, SEmporiaBattleObjectInfo >	CONT_EMBATTLE_OBJECT;

typedef struct tagGuildBattleUser
{
	tagGuildBattleUser()
		:	iTeam(TEAM_NONE)
		,	iClass(0)
	{}

	BM::GUID		kCharGuid;
	std::wstring	wstrName;
	int				iClass;
	int				iTeam;
	bool			bMercenary;
	SPvPResult		kResult;

	bool operator<( tagGuildBattleUser const &rhs )const{return kResult < rhs.kResult;}
	bool operator>( tagGuildBattleUser const &rhs )const{return kResult > rhs.kResult;}
	bool operator==( tagGuildBattleUser const &rhs )const{return kResult == rhs.kResult;}

	size_t min_size()const
	{
		return 
			sizeof(kCharGuid)+
			sizeof(size_t)+//wstrName)+
			(sizeof(int) * 2)+
			sizeof(bMercenary)+
			kResult.min_size();
	}

	size_t max_size()const
	{
		return	min_size()
			+	(sizeof(wchar_t) * MAX_CHARACTERNAME_LEN);
	}

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kPacket.Push(kCharGuid);
		kPacket.Push(wstrName);
		kPacket.Push(iClass);
		kPacket.Push(iTeam);
		kPacket.Push(bMercenary);
		kResult.WriteToPacket( kPacket );
	}

	void ReadFromPacket( BM::Stream &kPacket )
	{
		kPacket.Pop(kCharGuid);
		kPacket.Pop(wstrName);
		kPacket.Pop(iClass);
		kPacket.Pop(iTeam);
		kPacket.Pop(bMercenary);
		kResult.ReadFromPacket( kPacket );
	}
}SGuildBattleUser;
typedef std::map< BM::GUID, SGuildBattleUser>	CONT_EMPORIA_BATTLE_USER;
typedef std::vector< SGuildBattleUser >			CONT_EMPORIA_BATTLE_RESULT;

typedef struct tagGuildBattleResultUser
{
	tagGuildBattleResultUser()
		: bMercenary(false)
	{}

	BM::GUID kCharGuid;
	bool bMercenary;

	DEFAULT_TBL_PACKET_FUNC();
}SGuildBattleResultUser;
typedef std::vector< SGuildBattleResultUser >	CONT_EMPORIA_BATTLE_RESULT_USER;
typedef CONT_EMPORIA_BATTLE_RESULT_USER			CONT_EM_RESULT_USER;

typedef struct tagReqUnKeepEmporia
{
	tagReqUnKeepEmporia( short _nFuncNo=0, BYTE _nWeek=0 )
		:	nFuncNo(_nFuncNo)
		,	nWeek(_nWeek)
	{}

	tagReqUnKeepEmporia( tagReqUnKeepEmporia const &rhs )
		:	nFuncNo(rhs.nFuncNo)
		,	nWeek(rhs.nWeek)
	{}

	short	nFuncNo;
	BYTE	nWeek;

	DEFAULT_TBL_PACKET_FUNC();
}SReqUnKeepEmporia;

typedef enum
{
	E_BATTLEAREA_TYPE_NONE		= 0,
	E_BATTLEAREA_TYPE_ATK		= 1,
	E_BATTLEAREA_TYPE_BASE		= 2,
	E_BATTLEAREA_TYPE_FAT		= 3,
	E_BATTLEAREA_TYPE_MAGIC		= 4,
	E_BATTLEAREA_TYPE_AGILITY	= 5,
	E_BATTLEAREA_TYPE_BOSS		= 6,
}E_BATTLEAREA_TYPE;

typedef enum ePvPLeagueEvent
{
	PVPLE_NONE = 0,
	PVPLE_JOIN_START,
	PVPLE_JOIN_END,
	PVPLE_NFY_GMAE_READY,
	PVPLE_GAME_READY,
	PVPLE_GAME_START,
	PVPLE_GAME_END,
}EPvPLeagueEvent;

typedef enum ePvPLeagueState
{
	PVPLS_CLOSE = 0,
	PVPLS_WAIT,
	PVPLS_JOIN,
	PVPLS_READY,
	PVPLS_GAME,
	PVPLS_END,
}EPvPLeagueState;

typedef enum ePvPLeagueBattle
{
	PVPLB_CLOSE = 0,
	PVPLB_WAIT,
	PVPLB_READY,
	PVPLB_PLAY,
	PVPLB_END,
}EPvPLeagueBattle;

typedef enum ePvPLeagueMsg
{
	PVPLM_READY = 0,//x분 후 pvp리그 x강전이 시작된다는 메시지
	PVPLM_INVITE,//시작되었으니 입장하라는 메시지
	PVPLM_AUTOWIN,//부전승 안내 메시지
	PVPLM_CREATE_TOURNAMENT,//리그 신청 완료되어 리그가 시작되었음을 알림
	PVPLM_GAME_END,// x강이 종료 됨.
}EPvPLeagueMsg;

typedef struct tagPvPLeague_Event
{
	tagPvPLeague_Event()
		:	i64Time(0i64)
		,	iLevel(0)
	{
		eEvent =PVPLE_NONE;
	}

	__int64 i64Time;
	ePvPLeagueEvent eEvent;
	int iLevel;
}SPvPLeague_Event;

typedef struct tagPvPLeague
{
	tagPvPLeague(void)
		:	iLeagueState(0)
		,	iSession(0)
	{
		Clear();
	}

	BM::GUID kTournamentGuid;
	int iLeagueState;
	int iSession;
	BM::DBTIMESTAMP_EX kRegistDate;

	void Clear()
	{
		kRegistDate.Clear();
		kTournamentGuid.Clear();
	}

	bool operator<(tagPvPLeague const& rhs)const
	{
		return kRegistDate < rhs.kRegistDate;
	}
	bool operator>(tagPvPLeague const& rhs)const
	{
		return kRegistDate > rhs.kRegistDate;
	}
	bool operator==(tagPvPLeague const& rhs)const
	{
		return (kTournamentGuid==rhs.kTournamentGuid) && (kRegistDate==rhs.kRegistDate) && (iLeagueState==rhs.iLeagueState);
	}
	size_t min_size()const
	{
		return 
			sizeof(kTournamentGuid)+
			sizeof(iLeagueState)+
			sizeof(iSession)+
			sizeof(kRegistDate);
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(kTournamentGuid);
		kPacket.Push(iLeagueState);
		kPacket.Push(iSession);
		kPacket.Push(kRegistDate);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(kTournamentGuid);
		kPacket.Pop(iLeagueState);
		kPacket.Pop(iSession);
		kPacket.Pop(kRegistDate);
	}
}TBL_DEF_PVPLEAGUE;

typedef std::map< BM::GUID, TBL_DEF_PVPLEAGUE > CONT_DEF_PVPLEAGUE;

typedef struct tagPvPLeague_User
{
	BM::GUID kGuid;
	std::wstring wstrName;

	bool operator<(tagPvPLeague_User const& rhs)const
	{
		return kGuid < rhs.kGuid;
	}
	bool operator>(tagPvPLeague_User const& rhs)const
	{
		return kGuid > rhs.kGuid;
	}
	bool operator==(tagPvPLeague_User const& rhs)const
	{
		return (kGuid==rhs.kGuid) && (wstrName==rhs.wstrName);
	}
	size_t min_size()const
	{
		return 
			sizeof(kGuid)+
			sizeof(size_t);
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(kGuid);
		kPacket.Push(wstrName);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(kGuid);
		kPacket.Pop(wstrName);
	}
}SPvPLeague_User;
typedef std::vector<SPvPLeague_User> CONT_LEAGUE_USER;

typedef struct tagPvPLeague_Team
{
	tagPvPLeague_Team(void)
		:	iTournamentIndex(0)
		,	iLeagueLevel(0)
		,	iPoint(0)
	{
		Clear();
	}

	void Clear()
	{
		wstrTeamName.clear();
		kContUserGuid.clear();
		kRegistDate.Clear();
	}

	std::wstring wstrTeamName;
	CONT_LEAGUE_USER kContUserGuid;
	BM::DBTIMESTAMP_EX kRegistDate;
	int iTournamentIndex;
	int iLeagueLevel;
	int iPoint;

	bool operator<(tagPvPLeague_Team const& rhs)const
	{
		return kRegistDate < rhs.kRegistDate;
	}
	bool operator>(tagPvPLeague_Team const& rhs)const
	{
		return kRegistDate > rhs.kRegistDate;
	}
	bool operator==(tagPvPLeague_Team const& rhs)const
	{
		return (kRegistDate==rhs.kRegistDate) && (wstrTeamName==rhs.wstrTeamName);
	}
	size_t min_size()const
	{
		return 
			sizeof(wstrTeamName)+
			sizeof(size_t)+ //VEC_GUID kContUserGuid
			sizeof(kRegistDate)+
			sizeof(iTournamentIndex)+
			sizeof(iLeagueLevel)+
			sizeof(iPoint);
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(wstrTeamName);
		PU::TWriteArray_M(kPacket, kContUserGuid);
		kPacket.Push(kRegistDate);
		kPacket.Push(iTournamentIndex);
		kPacket.Push(iLeagueLevel);
		kPacket.Push(iPoint);
	}

	bool ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(wstrTeamName);
		PU::TLoadArray_M(kPacket, kContUserGuid);
		kPacket.Pop(kRegistDate);
		kPacket.Pop(iTournamentIndex);
		kPacket.Pop(iLeagueLevel);
		return kPacket.Pop(iPoint);
	}
}TBL_DEF_PVPLEAGUE_TEAM;

typedef std::map< BM::GUID, TBL_DEF_PVPLEAGUE_TEAM > CONT_DEF_PVPLEAGUE_TEAM;
typedef std::map< BM::GUID, BM::GUID > CONT_PVPLEAGUE_CHAR_TO_TEAM;
typedef std::map< std::wstring, BM::GUID > CONT_PVPLEAGUE_NAME_TO_TEAM;

typedef struct tagPvPLeague_Battle
{
	tagPvPLeague_Battle(void)
		:	iTournamentIndex(0)
		,	iGroupIndex(0)
		,	iBattleState(0)
		,	iRoomIndex(0)
	{
		Clear();
	}

	void Clear()
	{
		kTournamentGuid.Clear();
		kTeamGuid1.Clear();
		kTeamGuid2.Clear();
		kWinTeamGuid.Clear();
	}

	BM::GUID kTournamentGuid;
	int iTournamentIndex;
	BM::GUID kTeamGuid1;
	BM::GUID kTeamGuid2;
	BM::GUID kWinTeamGuid;
	int iGroupIndex;
	
	int iBattleState;//db에 기록 안함
	int iRoomIndex;//db에 기록 안함

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_PVPLEAGUE_BATTLE;

typedef std::map< BM::GUID, TBL_DEF_PVPLEAGUE_BATTLE > CONT_DEF_PVPLEAGUE_BATTLE;
typedef std::map< BM::GUID, BM::GUID > CONT_PVPLEAGUE_TEAM_TO_BATTLE;
typedef std::map< int, BM::GUID > CONT_PVPLEAGUE_INDEX_TO_BATTLE;

typedef struct tagPvPLeague_Tournament
{
	tagPvPLeague_Tournament(void)
		:	iLeagueLevel(0)
		,	iGameType(0)
		,	iGameMode(0)
		,	iGameTime(0)
	{
		Clear();
	}

	void Clear()
	{
		kBeginTime.Clear();
	}

	int iLeagueLevel;
	int iGameType;
	int iGameMode;
	int iGameTime;
	BM::DBTIMESTAMP_EX kBeginTime;

	bool operator<(tagPvPLeague_Tournament const& rhs)const
	{
		return iLeagueLevel < rhs.iLeagueLevel;
	}

	bool operator>(tagPvPLeague_Tournament const& rhs)const
	{
		return iLeagueLevel > rhs.iLeagueLevel;
	}

	bool operator==(tagPvPLeague_Tournament const& rhs)const
	{
		return (iLeagueLevel==rhs.iLeagueLevel) && (kBeginTime==rhs.kBeginTime);
	}

	size_t min_size()const
	{
		return 
			sizeof(iLeagueLevel)+
			sizeof(iGameType)+ //VEC_GUID kContUserGuid
			sizeof(iGameMode)+
			sizeof(iGameTime)+
			sizeof(kBeginTime);
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(iLeagueLevel);
		kPacket.Push(iGameType);
		kPacket.Push(iGameMode);
		kPacket.Push(iGameTime);
		kPacket.Push(kBeginTime);
	}

	bool ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(iLeagueLevel);
		kPacket.Pop(iGameType);
		kPacket.Pop(iGameMode);
		kPacket.Pop(iGameTime);
		return kPacket.Pop(kBeginTime); 
	}
}TBL_DEF_PVPLEAGUE_TOURNAMENT;

typedef std::map< BM::GUID, TBL_DEF_PVPLEAGUE_TOURNAMENT > CONT_DEF_PVPLEAGUE_TOURNAMENT;


typedef struct tagGroundMakeOrder
{
	tagGroundMakeOrder()
	{
		Clear();
	}

	explicit tagGroundMakeOrder( SGroundKey const &kGndKey )
		:	kKey(kGndKey)
		,	byWeigth(0)
		,	byBalance(0)
		,	iOwnerLv(0)
		,	bIndunPartyDie(false)
	{}

	void Clear()
	{
		kKey.Clear();
		byWeigth = 0;
		byBalance = 0;
		iOwnerLv = 0;
		bIndunPartyDie = false;
	}

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kKey.WriteToPacket( kPacket );
		kPacket.Push( byWeigth );
		kPacket.Push( byBalance );
		kPacket.Push( iOwnerLv );
		kPacket.Push( bIndunPartyDie );
		constellationMission.WriteToPacket(kPacket);
	}

	void ReadFromPacket( BM::Stream &kPacket )
	{
		kKey.ReadFromPacket( kPacket );
		kPacket.Pop( byWeigth );
		kPacket.Pop( byBalance );
		kPacket.Pop( iOwnerLv );
		kPacket.Pop( bIndunPartyDie );
		constellationMission.ReadFromPacket(kPacket);
	}

	SGroundKey kKey;
	BYTE byWeigth;// Ground 가중치
	BYTE byBalance;
	int iOwnerLv;
	bool bIndunPartyDie;
	Constellation::SConstellationMission constellationMission;
}SGroundMakeOrder;

#pragma pack()

#endif // WEAPON_LOHENGRIN_PACKET_PACKETSTRUCT2_H