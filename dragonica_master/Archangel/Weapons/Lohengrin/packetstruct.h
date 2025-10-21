#ifndef WEAPON_LOHENGRIN_PACKET_PACKETSTRUCT_H
#define WEAPON_LOHENGRIN_PACKET_PACKETSTRUCT_H

#define WIN32_LEAN_AND_MEAN

#include <string>
#include "windows.h"
#include "winsock2.h"

#include "BM/vstring.h"
#include "BM/GUID.h"
#include "BM/Point.h"
#include "CEL/Common.h"

#include "CEL/CEL.h"
#include "CEL/CoreCenter.h"
#include "CEL/Query.h"
#include "LogGroup.h"
#include "packettype.h"
#include "Variant/StatTrackInfo.h"

typedef T_PT2<BYTE> SItemPos;

//For Switch
#ifdef _DEBUG	
DWORD const RESERVEMEMBER_TIMEOUT = 300000;
#else
DWORD const RESERVEMEMBER_TIMEOUT = 60000;
#endif

//+---------------------------------------------+
//| 전역 typedef								|
//+---------------------------------------------+
typedef std::vector<BM::GUID>	VEC_GUID;
typedef std::set<BM::GUID>		SET_GUID;
typedef std::list<BM::GUID>		LIST_GUID;
typedef std::vector<int>		VEC_INT;
typedef std::set<int>			SET_INT;
typedef std::vector<SItemPos>	VEC_ITEMPOS;

#define DEFAULT_TBL_PACKET_FUNC() \
	void WriteToPacket(BM::Stream &kPacket)const \
	{\
		kPacket.Push(*this);\
	}\
	bool ReadFromPacket(BM::Stream &kPacket)\
	{\
		return kPacket.Pop(*this);\
	}\
	size_t min_size()const\
	{\
		return sizeof(*this);\
	}

template< typename T >
struct tagTBL_KEY
{
	tagTBL_KEY()
	{
	}

	tagTBL_KEY(T const kInitvalue)
		:kKey(kInitvalue)
	{
	}

	operator T ()const
	{
		return kKey;
	}

	bool operator<(tagTBL_KEY<T> const &rhs )const
	{
		return (kKey < rhs.kKey);
	}

	T kKey;
	DEFAULT_TBL_PACKET_FUNC();
};

template < typename T1, typename T2 >
struct tagPairKey
{
	tagPairKey()
	{
	}

	tagPairKey(T1 const &kPri, T2 const &kSec )
		:kPriKey(kPri), kSecKey(kSec)
	{
	}

	bool operator < ( tagPairKey< T1, T2 > const &rhs )const
	{
		if( kPriKey < rhs.kPriKey )	{return true;}
		if( kPriKey > rhs.kPriKey )	{return false;}

		if( kSecKey < rhs.kSecKey )	{return true;}
		if( kSecKey > rhs.kSecKey )	{return false;}
		
		return false;
	}

	operator std::wstring () const
	{
		BM::vstring kStr;
		kStr << L"(PriKey:" << kPriKey << L", SecKey:" << kSecKey << L")";
		return kStr;
	}

	T1 kPriKey;
	T2 kSecKey;
	DEFAULT_TBL_PACKET_FUNC();
};

template < typename T1, typename T2, typename T3 >
struct tagTripleKey
{
	tagTripleKey()
	{
	}

	tagTripleKey(T1 const &kPri, T2 const &kSec, T3 const &kTrd )
		:kPriKey(kPri), kSecKey(kSec), kTrdKey(kTrd)
	{
	}

	bool operator < ( tagTripleKey< T1, T2, T3 > const &rhs )const
	{
		if( kPriKey < rhs.kPriKey )	{return true;}
		if( kPriKey > rhs.kPriKey )	{return false;}

		if( kSecKey < rhs.kSecKey )	{return true;}
		if( kSecKey > rhs.kSecKey )	{return false;}
		
		if( kTrdKey < rhs.kTrdKey )	{return true;}
		if( kTrdKey > rhs.kTrdKey )	{return false;}
		
		return false;
	}

	T1 kPriKey;
	T2 kSecKey;
	T3 kTrdKey;
	DEFAULT_TBL_PACKET_FUNC();
};

template < typename T1, typename T2, typename T3 , typename T4 >
struct tagQuadKey
{
	tagQuadKey()
	{
	}

	tagQuadKey(T1 const &kPri, T2 const &kSec, T3 const &kTrd, T4 const &kQud )
		:kPriKey(kPri), kSecKey(kSec), kTrdKey(kTrd), kQudKey(kQud)
	{
	}

	bool operator < ( tagQuadKey< T1, T2, T3, T4 > const &rhs )const
	{
		if( kPriKey < rhs.kPriKey )	{return true;}
		if( kPriKey > rhs.kPriKey )	{return false;}

		if( kSecKey < rhs.kSecKey )	{return true;}
		if( kSecKey > rhs.kSecKey )	{return false;}
		
		if( kTrdKey < rhs.kTrdKey )	{return true;}
		if( kTrdKey > rhs.kTrdKey )	{return false;}
		
		if( kQudKey < rhs.kQudKey )	{return true;}
		if( kQudKey > rhs.kQudKey )	{return false;}

		return false;
	}

	T1 kPriKey;
	T2 kSecKey;
	T3 kTrdKey;
	T4 kQudKey;
	DEFAULT_TBL_PACKET_FUNC();
};

// Class to Log
template< typename _T >
inline BM::vstring C2L(_T const& rkClass)
{
	return BM::vstring(rkClass.ToString());
}
template< typename _T >
inline BM::vstring C2L(_T const* const pkPointer)
{
	if( !pkPointer )
	{
		return BM::vstring(typeid(_T).name()) << L"* is NULL pointer";
	}
	return BM::vstring(pkPointer->ToString());
}

#pragma pack (1)

typedef struct tagServerIdentity
{//사이트를 알 필요가 없다. 서버군은 렐름으로 완전히 분리 되어 있기 때문.
	tagServerIdentity()
		:	nRealm(0)
		,	nChannel(0)
		,	nServerNo(0)
		,	nServerType(0)
	{
	}

	void Clear()
	{
		*this = tagServerIdentity();
	}

	bool Empty()const
	{
		return 0 == nServerType;
	}

	void Set(tagServerIdentity const &rhs)
	{
		nRealm = rhs.nRealm;
		nChannel = rhs.nChannel;
		nServerNo = rhs.nServerNo;
		nServerType = rhs.nServerType;
	}

	bool operator < (tagServerIdentity const &rhs) const
	{
		if( nRealm < rhs.nRealm )	{return true;}
		if( nRealm > rhs.nRealm )	{return false;}

		if( nChannel < rhs.nChannel )	{return true;}
		if( nChannel > rhs.nChannel )	{return false;}

		if( nServerNo < rhs.nServerNo )	{return true;}
		if( nServerNo > rhs.nServerNo )	{return false;}

		if( nServerType < rhs.nServerType )	{return true;}
		if( nServerType > rhs.nServerType )	{return false;}

		return false;
	}

	bool operator == (tagServerIdentity const &rhs) const
	{
		return		(nRealm == rhs.nRealm)
				&&	(nChannel == rhs.nChannel)
				&&	(nServerNo == rhs.nServerNo)
				&&	(nServerType == rhs.nServerType);
	}

	bool operator != (tagServerIdentity const &rhs) const
	{
		return !(*this == rhs);
	}
/*
	operator size_t ()const
	{
		return nRealm^nChannel^nServerNo^nServerType;
	}
*/
	DEFAULT_TBL_PACKET_FUNC();

	BM::vstring ToString() const
	{
		return BM::vstring(L" SERVER.ID[R:")<<nRealm<<L",C:"<<nChannel<<L",S:"<<nServerNo<<L",T:"<<nServerType<<L"]";
	}

	short nRealm;//렐름번호
	short nChannel;//하나의 서버군. 채널마다 쎈터가 한개
	short nServerNo;//해당 서버의 번호 (맵서버의 경우 맵번호가됨)
	short nServerType;//E_SESSION_TYPE
	
}SERVER_IDENTITY;

typedef struct tagServerIdentitySite
:	public tagServerIdentity
{
	tagServerIdentitySite()
		:	nSiteNo(0)
	{}

	explicit tagServerIdentitySite( tagServerIdentity const &rhs, short const _nSiteNo )
		:	tagServerIdentity(rhs)
		,	nSiteNo(_nSiteNo)
	{
	}

	bool operator < (tagServerIdentitySite const &rhs) const
	{
		if ( nSiteNo == rhs.nSiteNo )
		{
			return tagServerIdentity::operator < ( rhs );
		}
		return nSiteNo < rhs.nSiteNo;
	}

	bool operator == (tagServerIdentitySite const &rhs) const
	{
		if ( nSiteNo == rhs.nSiteNo )
		{
			return tagServerIdentity::operator == ( rhs );
		}
		return false;
	}

	bool operator != (tagServerIdentitySite const &rhs) const
	{
		return !(*this == rhs);
	}

	DEFAULT_TBL_PACKET_FUNC();

	BM::vstring ToString() const
	{
		return BM::vstring(L" SERVER.ID[SITE:")<<nSiteNo<<L"R:"<<nRealm<<L",C:"<<nChannel<<L",S:"<<nServerNo<<L",T:"<<nServerType<<L"]";
	}

	short	nSiteNo;
}SERVER_IDENTITY_SITE;

typedef std::vector< SERVER_IDENTITY >		ContServerID;
typedef std::vector< SERVER_IDENTITY_SITE > ContServerSiteID;

typedef struct tagNfyReservedMemberLogin
{
	tagNfyReservedMemberLogin()
	{}

	SERVER_IDENTITY kSwitchServer;
	BM::GUID guidMember;
	BM::GUID guidSwitchKey;
}NFY_RESERVED_MEMBER_LOGIN;				

int const MAX_CHANNEL_NAME = 21;
typedef struct tagTBL_ServerList
	: public SERVER_IDENTITY
{
	tagTBL_ServerList()
	{
		pkSession = NULL;
		nPlayerCount = 0;
	}

	tagTBL_ServerList(SERVER_IDENTITY const &kSI)
	{
		SERVER_IDENTITY::operator = (kSI);
		
		pkSession = NULL;
		nPlayerCount = 0;
	}	

	bool operator < (tagTBL_ServerList const &rhs)const
	{//사실 아래 애들은 필요없지;
		if( *(SERVER_IDENTITY const*)this < *(SERVER_IDENTITY const*)&rhs)
		{
			return true;
		}
		return false;
	}

	std::wstring strMachinIP;//Machine IP
	std::wstring strName;//SA Identify Name

	CEL::ADDR_INFO addrServerBind;//Server Bind
	CEL::ADDR_INFO addrServerNat;//
	CEL::ADDR_INFO addrUserBind;//User Bind
	CEL::ADDR_INFO addrUserNat;//

	//아래부터 디비 데이터 아님.
	//디비 데이터 아닌것은 Set 이나 기타 카피때 셋팅 안한다.
//	CEL::SESSION_KEY kSessionKey;//해당 접속된 서버의 세션키
	CEL::CSession_Base * pkSession;//참조값임. 절대 DELETE 하면 안됨.
	size_t nPlayerCount;

	void Set(const tagTBL_ServerList& rhs)
	{
		SERVER_IDENTITY::Set(rhs);

		strMachinIP = rhs.strMachinIP;
		strName = rhs.strName;

		addrServerBind = rhs.addrServerBind;
		addrServerNat = rhs.addrServerNat;
		addrUserBind = rhs.addrUserBind;
		addrUserNat = rhs.addrUserNat;

		//kSessionKey	->동적 데이터 이므로 Set 에서 뭔가 하거나 하지 않는다.
		Build();
	}

	void Clear()
	{
		SERVER_IDENTITY::Clear();

		strMachinIP.clear();
		strName.clear();

		addrServerBind.Clear();
		addrServerNat.Clear();
		addrUserBind.Clear();
		addrUserNat.Clear();
		
//		kSessionKey.Clear();
		pkSession = NULL;
		nPlayerCount= 0;
	}

	size_t min_size()const
	{
		return 
			SERVER_IDENTITY::min_size()+
			sizeof(size_t)+//strMachinIP
			sizeof(size_t)+//strName
			addrServerBind.min_size()+
			addrServerNat.min_size()+
			addrUserBind.min_size()+
			addrUserNat.min_size();
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{	//패킷에 쓰기전에 Build() 를 꼭 했어야 한다.
		SERVER_IDENTITY::WriteToPacket(kPacket);

		kPacket.Push(strMachinIP);
		kPacket.Push(strName);

		addrServerBind.WriteToPacket(kPacket);
		addrServerNat.WriteToPacket(kPacket);
		addrUserBind.WriteToPacket(kPacket);
		addrUserNat.WriteToPacket(kPacket);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		SERVER_IDENTITY::ReadFromPacket(kPacket);

		kPacket.Pop(strMachinIP);
		kPacket.Pop(strName);

		addrServerBind.ReadFromPacket(kPacket);
		addrServerNat.ReadFromPacket(kPacket);
		addrUserBind.ReadFromPacket(kPacket);
		addrUserNat.ReadFromPacket(kPacket);
		Build();
	}

	void Build()
	{
		if( !addrServerNat.ip.s_addr )		{ addrServerNat.ip = addrServerBind.ip;	}
		if( !addrServerNat.wPort )			{ addrServerNat.wPort = addrServerBind.wPort;	}
		if( !addrUserNat.ip.s_addr )		{ addrUserNat.ip = addrUserBind.ip;	}
		if( !addrUserNat.wPort )			{ addrUserNat.wPort = addrUserBind.wPort;	}
	}
}TBL_SERVERLIST;

typedef  std::map< SERVER_IDENTITY, TBL_SERVERLIST > CONT_SERVER_HASH;//Key ServerNo 

// 머신 컨트롤러에서 쓰는 게임 서버 상태.
typedef struct tagPROCESS_INFORMATION_EX
	: public PROCESS_INFORMATION
{
	void Clear()
	{	//CloseHandle 하면서 프로세스 닫힘.
		if( hProcess != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hProcess);
			hProcess = INVALID_HANDLE_VALUE;
			hThread = INVALID_HANDLE_VALUE;
		}

		if( hThread != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hThread);
			hThread = INVALID_HANDLE_VALUE;
		}
		dwProcessId = 0;
		dwThreadId = 0;
	}

	bool IsCorrect()const
	{
		if(hProcess != INVALID_HANDLE_VALUE)
		{
			return true;
		}
		return false;
	}

	tagPROCESS_INFORMATION_EX()
	{
		hProcess = INVALID_HANDLE_VALUE;
		hThread = INVALID_HANDLE_VALUE;
		dwProcessId = 0;
		dwThreadId = 0;

		Clear();
	}

	tagPROCESS_INFORMATION_EX(PROCESS_INFORMATION const &rkIn)
	{
		*this = rkIn;
	}

	void operator = (PROCESS_INFORMATION const &rkIn)
	{
		*(dynamic_cast<PROCESS_INFORMATION*>(this)) = rkIn;
	}

	bool operator == ( PROCESS_INFORMATION const &rhs )const
	{
		return	(hProcess == rhs.hProcess)
			&&	(hThread == rhs.hThread)
			&&	(dwProcessId == rhs.dwProcessId)
			&&	(dwThreadId == rhs.dwThreadId);
	}

	bool operator != ( PROCESS_INFORMATION const &rhs )const
	{
		return !(*this == rhs);
	}

	size_t min_size() const
	{
		return sizeof(PROCESS_INFORMATION);
	}

	void WriteToPacket(BM::Stream& rkPacket) const
	{
		rkPacket.Push(this, sizeof(PROCESS_INFORMATION));
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.PopMemory(this, sizeof(PROCESS_INFORMATION));
	}
}PROCESS_INFORMATION_EX;

typedef struct tagSingleServerStatus
{
	TBL_SERVERLIST kTbl;
	PROCESS_INFORMATION_EX kProcessInfo;
	
	bool bReadyToService;//게임서버가 정상적으로 떠서 ReadyToService 패킷이 왔는가?
	bool bIsException;//게임 프로세스가 Eception이 났는가?
	bool bTickWarning;//일정 시간 이상 HeartBeat가 없었는지
	DWORD dwLastUpdateTime;

	size_t min_size()const
	{
		return 
			kTbl.min_size()+
			kProcessInfo.min_size()+
			sizeof(bReadyToService)+
			sizeof(bIsException)+
			sizeof(bTickWarning);
	}

	tagSingleServerStatus()
	{
		Clear();
	}

	void Set(tagSingleServerStatus const &rhs)
	{
		kTbl = rhs.kTbl;
		kProcessInfo = rhs.kProcessInfo;
		bReadyToService = rhs.bReadyToService;
		bIsException = false;
		bTickWarning = false;
	}

	void Clear()
	{
		kProcessInfo.Clear();
		bReadyToService = false;
		bIsException = false;
		bTickWarning = false;
		dwLastUpdateTime = BM::GetTime32();//Now
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kTbl.WriteToPacket(kPacket);
		kProcessInfo.WriteToPacket(kPacket);
		kPacket.Push(bReadyToService);
		kPacket.Push(bIsException);
		kPacket.Push(bTickWarning);
	}
	
	void ReadFromPacket(BM::Stream &kPacket)
	{
		kTbl.ReadFromPacket(kPacket);
		kProcessInfo.ReadFromPacket(kPacket);
		kPacket.Pop(bReadyToService);
		kPacket.Pop(bIsException);
		kPacket.Pop(bTickWarning);
	}
}SSingleServerStatus;

typedef struct tagExSingleServerStatus
:public tagSingleServerStatus
{
	unsigned int uiMaxUsers;
	unsigned int uiConnectionUsers;

	tagExSingleServerStatus()
	{
		Clear();
	}

	void Clear()
	{
		tagSingleServerStatus::Clear();
		uiMaxUsers = 0;
		uiConnectionUsers = 0;
	}

	size_t min_size()const
	{
		return 
			tagSingleServerStatus::min_size()+
			sizeof(uiMaxUsers)+
			sizeof(uiConnectionUsers);
	}

	void Set(tagExSingleServerStatus const &rhs)
	{
		tagSingleServerStatus::Set(rhs);
		uiMaxUsers = rhs.uiMaxUsers;
		uiConnectionUsers = rhs.uiConnectionUsers;
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		tagSingleServerStatus::WriteToPacket(kPacket);
		kPacket.Push(uiMaxUsers);
		kPacket.Push(uiConnectionUsers);
	}
	
	void ReadFromPacket(BM::Stream &kPacket)
	{
		tagSingleServerStatus::ReadFromPacket(kPacket);
		kPacket.Pop(uiMaxUsers);
		kPacket.Pop(uiConnectionUsers);
	}
} SExSingleServerStatus;

typedef struct tagTBL_MapConfig
{
	tagTBL_MapConfig()
		:	nRealmNo(0)
		,	nChannelNo(0)
		,	nServerNo(0)
		,	iGroundNo(0)
	{}

	tagTBL_MapConfig(const tagTBL_MapConfig& rhs)
		:	nRealmNo(rhs.nRealmNo)
		,	nChannelNo(rhs.nChannelNo)
		,	nServerNo(rhs.nServerNo)
		,	iGroundNo(rhs.iGroundNo)
	{}

	bool operator < (tagTBL_MapConfig const &rhs)const
	{
		if( nRealmNo < rhs.nRealmNo )	{return true;}
		if( nRealmNo > rhs.nRealmNo )	{return false;}

		if( nChannelNo < rhs.nChannelNo )	{return true;}
		if( nChannelNo > rhs.nChannelNo )	{return false;}
	
		if( nServerNo < rhs.nServerNo )	{return true;}
		if( nServerNo > rhs.nServerNo )	{return false;}

		if( iGroundNo < rhs.iGroundNo )	{return true;}
		if( iGroundNo > rhs.iGroundNo )	{return false;}

		return false;
	}

	bool operator > (tagTBL_MapConfig const &rhs)const
	{
		return !((*this) < rhs);
	}

	short nRealmNo;
	short nChannelNo;
	short nServerNo;
	int iGroundNo;	// 미션일 경우에는 (0 != (iAttr & GATTR_MISSION)) MissionNo로 저장되어 있다.
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_MAP_CONFIG;

typedef enum eGroundKind
{
	GKIND_DEFAULT			= 0x00000000,
	GKIND_INDUN				= 0x00000001,
	GKIND_WAR				= 0x00000002,
	GKIND_MISSION			= 0x00000004,
	GKIND_EMPORIA			= 0x00000040,
	GKIND_HOME				= 0x00000080,
	GKIND_BS				= 0x00002000,
	GKIND_HARDCORE			= 0x00004000,
	GKIND_SUPER				= 0x00008000,
	GKIND_EXPEDITION		= 0x00010000,
	GKIND_EXPEDITION_LOBBY	= 0x00020000,
	GKIND_ELEMENT			= 0x00040000,
	GKIND_EVENT				= 0x00080000,
	GKIND_RACE				= 0x00100000,
	GKIND_CONSTELLATION		= 0x00200000,
	GKIND_CART_MISSION		= 0x00400000,
} EGroundKind;

typedef enum eGroundAttribute
{
	GATTR_DEFAULT				= GKIND_DEFAULT,									//(0)Static Ground
	GATTR_INSTANCE				= GKIND_INDUN,										//(1)Instance Ground

	// PvP
	GATTR_FLAG_PVP				= GKIND_WAR,										//(2)
	GATTR_PVP					= GATTR_INSTANCE|GATTR_FLAG_PVP,					//(3)PvP

	// Mission
	GATTR_FLAG_MISSION			= GKIND_MISSION,									//(4)
	GATTR_MISSION				= GATTR_INSTANCE|GATTR_FLAG_MISSION,				//(5)Mission

	// Boss
	GATTR_FLAG_BOSS				= 0x00000008,										//(8)
	GATTR_BOSS					= GATTR_INSTANCE|GATTR_FLAG_BOSS,					//(9)Boss

	// Village
	GATTR_FLAG_VILLAGE			= 0x00000010,										//(16)
	GATTR_VILLAGE				= GATTR_FLAG_VILLAGE,								//(16)Village

	GATTR_FLAG_EMPORIABATTLE	= 0x00000020,										//(32)
	GATTR_EMPORIABATTLE			= GATTR_PVP|GATTR_FLAG_EMPORIABATTLE,				//(35)Siege War

	GATTR_FLAG_EMPORIA			= GKIND_EMPORIA,									//(64)
	GATTR_EMPORIA				= GATTR_INSTANCE|GATTR_FLAG_EMPORIA,				//(65)Emporia

	GATTR_FLAG_MYHOME			= GKIND_HOME,										//(128)
	GATTR_MYHOME				= GATTR_FLAG_MYHOME|GATTR_INSTANCE,					//(129)My Home

	GATTR_FLAG_HIDDEN_F			= 0x00000100,										//(256)
	GATTR_HIDDEN_F				= GATTR_FLAG_HIDDEN_F,								//(256)Hidden Field

	GATTR_FLAG_CHAOS_F			= 0x00000200,										//(512)
	GATTR_CHAOS_F				= GATTR_FLAG_CHAOS_F,								//(512)Chaos Field
	GATTR_CHAOS_MISSION			= GATTR_MISSION | GATTR_FLAG_CHAOS_F,				//(517)Chaos Mission

	GATTR_FLAG_STATIC_DUNGEON	= 0x00000400,										//(1024)
	GATTR_STATIC_DUNGEON		= GATTR_DEFAULT | GATTR_FLAG_STATIC_DUNGEON,		//(1024)Static Dungeon

	GATTR_FLAG_HOMETOWN			= 0x00000800,										//(2048)
	GATTR_HOMETOWN				= GATTR_FLAG_HOMETOWN,								//(2048)public village

	GATTR_FLAG_MARRY			= 0x00001000,										//(4096)
	GATTR_MARRY					= GATTR_FLAG_MARRY,

	GATTR_FLAG_BATTLESQUARE		= 0x00002000,
	GATTR_BATTLESQUARE			= GATTR_FLAG_BATTLESQUARE,							//(8192)

	GATTR_FLAG_HARDCORE_DUNGEON	= GKIND_HARDCORE,									//(16384)
	GATTR_HARDCORE_DUNGEON		= GKIND_INDUN | GATTR_FLAG_HARDCORE_DUNGEON,		//(16385)
	GATTR_HARDCORE_DUNGEON_BOSS	= GATTR_FLAG_BOSS | GATTR_HARDCORE_DUNGEON,			//(16393) 

	GATTR_FLAG_SUPER			= GKIND_SUPER,										//(32768)
	GATTR_SUPER_GROUND			= GKIND_INDUN| GKIND_SUPER,							//(32769)
	GATTR_SUPER_GROUND_BOSS		= GATTR_SUPER_GROUND| GATTR_FLAG_BOSS,				//(32777)

	GATTR_ELEMENT_GROUND		= GKIND_INDUN| GKIND_SUPER | GKIND_ELEMENT,			//(294913)
	GATTR_ELEMENT_GROUND_BOSS	= GATTR_ELEMENT_GROUND| GATTR_FLAG_BOSS,			//(294921)

	GATTR_FLAG_EXPEDITION		= GKIND_EXPEDITION,									//(65536)
	GATTR_EXPEDITION_GROUND		= GKIND_INDUN | GKIND_EXPEDITION,					//(65537)
	GATTR_EXPEDITION_GROUND_BOSS= GATTR_EXPEDITION_GROUND | GATTR_FLAG_BOSS,		//(65545)
	
	GATTR_EXPEDITION_LOBBY		= GKIND_EXPEDITION_LOBBY,							//(131072)

	GATTR_EVENT_GROUND			= GKIND_EVENT,										//(524288)

	GATTR_RACE_GROUND			= GKIND_RACE | GKIND_EVENT,							//(1572864)
	
	GATTR_FLAG_CONSTELLATION	= GKIND_CONSTELLATION,								//(2097152)
	GATTR_CONSTELLATION_GROUND	= GATTR_INSTANCE | GATTR_FLAG_CONSTELLATION,		//(2097153)
	GATTR_CONSTELLATION_BOSS	= GATTR_CONSTELLATION_GROUND | GATTR_FLAG_BOSS,		//(2097161)

	GATTR_FLAG_CART_MISSION		= GKIND_CART_MISSION,								//(4194304)
	GATTR_CART_MISSION_GROUND	= GATTR_INSTANCE | GATTR_FLAG_CART_MISSION,			//(4194305)

	GATTR_FLAG_PUBLIC_CHANNEL	= GATTR_FLAG_PVP|GATTR_FLAG_EMPORIABATTLE|GATTR_FLAG_EMPORIA|GATTR_FLAG_MYHOME|GATTR_HOMETOWN|GATTR_FLAG_BATTLESQUARE|GATTR_FLAG_HARDCORE_DUNGEON,	// Only Public Channel
	GATTR_FLAG_NOPARTY			= GATTR_FLAG_PVP|GATTR_FLAG_EMPORIABATTLE|GATTR_FLAG_EMPORIA|GATTR_FLAG_MYHOME|GATTR_HOMETOWN|GATTR_FLAG_BATTLESQUARE,	// No Party Check
	GATTR_FLAG_NOPVP			= GATTR_INSTANCE|GATTR_FLAG_HOMETOWN|GATTR_FLAG_MARRY|GATTR_FLAG_BATTLESQUARE|GATTR_FLAG_HARDCORE_DUNGEON|GATTR_FLAG_SUPER|GATTR_FLAG_STATIC_DUNGEON,
	GATTR_FLAG_PVP_ABLE			= GATTR_FLAG_PVP|GATTR_FLAG_EMPORIABATTLE|GATTR_FLAG_BATTLESQUARE,
	GATTR_FLAG_HAVEPARTY		= GATTR_INSTANCE|GATTR_FLAG_MISSION|GATTR_FLAG_BOSS|GATTR_FLAG_SUPER,// Need Party or Single
	GATTR_FLAG_RESULT			= GATTR_FLAG_MISSION|GATTR_FLAG_BOSS,				// Result Check Flag
	GATTR_FLAG_NOEXCHANGEITEM	= GATTR_FLAG_PUBLIC_CHANNEL,						// Item Exchange
	GATTR_FLAG_AUTOHEAL_ENALBE	= GATTR_VILLAGE | GATTR_EMPORIA | GATTR_MYHOME | GATTR_HOMETOWN | GATTR_BATTLESQUARE,
	GATTR_FLAG_NOSAVE_RECENT	= GATTR_INSTANCE | GATTR_FLAG_STATIC_DUNGEON | GATTR_FLAG_MYHOME | GATTR_FLAG_HOMETOWN| GATTR_BATTLESQUARE | GATTR_HIDDEN_F | GATTR_FLAG_SUPER | GATTR_FLAG_EXPEDITION, // Static Dungeon
	GATTR_FLAG_CANT_WARP		= GATTR_INSTANCE |GATTR_FLAG_STATIC_DUNGEON |GATTR_FLAG_PVP |GATTR_FLAG_MISSION |GATTR_FLAG_HIDDEN_F |GATTR_FLAG_CHAOS_F |GATTR_FLAG_MYHOME | GATTR_FLAG_HOMETOWN| GATTR_BATTLESQUARE| GATTR_FLAG_SUPER | GATTR_EXPEDITION_LOBBY,
	GATTR_FLAG_CANT_PARTYWARP	= GATTR_INSTANCE | GATTR_FLAG_PVP | GATTR_FLAG_MISSION | GATTR_FLAG_MYHOME | GATTR_FLAG_HOMETOWN| GATTR_BATTLESQUARE| GATTR_FLAG_SUPER,
	GATTR_FLAG_CANT_HOMEWARP	= GATTR_INSTANCE |GATTR_FLAG_STATIC_DUNGEON |GATTR_FLAG_PVP |GATTR_FLAG_MISSION |GATTR_FLAG_HIDDEN_F |GATTR_FLAG_CHAOS_F |GATTR_FLAG_MYHOME| GATTR_BATTLESQUARE| GATTR_FLAG_SUPER,
	GATTR_FLAG_NOADDITEMDROPRATE= GATTR_FLAG_HIDDEN_F | GATTR_FLAG_CHAOS_F | GATTR_FLAG_STATIC_DUNGEON,
	GATTR_FLAG_NO_KILLCOUNT_UI	= GATTR_INSTANCE| GATTR_BATTLESQUARE| GATTR_FLAG_VILLAGE | GATTR_EVENT_GROUND,
	GATTR_FLAG_NO_INIT_KILLCOUNT= GATTR_FLAG_PVP| GATTR_FLAG_VILLAGE| GATTR_FLAG_EMPORIABATTLE| GATTR_FLAG_EMPORIA| GATTR_FLAG_MYHOME| GATTR_FLAG_HOMETOWN| GATTR_FLAG_MARRY| GATTR_FLAG_BATTLESQUARE,
	GATTR_FLAG_RUNNING_PARTY	= GATTR_MISSION | GATTR_FLAG_STATIC_DUNGEON | GATTR_HIDDEN_F | GATTR_SUPER_GROUND_BOSS | GATTR_HARDCORE_DUNGEON_BOSS | GATTR_ELEMENT_GROUND_BOSS,
	GATTR_FLAG_WATING_PARTY		= GATTR_VILLAGE | GKIND_DEFAULT,
	GATTR_FLAG_HIDE_PARTY_LIST	= GKIND_SUPER | GATTR_FLAG_CART_MISSION | GKIND_EXPEDITION | GKIND_ELEMENT | GKIND_EVENT, // 파티 찾기 리스트에서 감춤
	GATTR_FLAG_ENABLE_KICK		= GKIND_INDUN | GATTR_FLAG_STATIC_DUNGEON | GKIND_EVENT, // 이 속성을 가진 맵에서는 파티장과 같은 맵에 있는 유저는 강퇴할 수 없고 다른 맵에 있는 유저는 강퇴할 수 있다.
	GATTR_ALL					= 0xFFFFFF,
}EGndAttr;
typedef INT T_GNDATTR;

enum EDynamicGroundAttr // 동적인 그라운드 속성
{
	DGATTR_NONE					= 0x00000000,
	DGATTR_FLAG_FREEPVP			= 0x00000001,			// 자유 PVP
	DGATTR_FLAG_ALL				= 0xFFFFFFFF,
};

typedef enum eContinentType
{
	CONTINENT_OF_WIND			= 1,// 바람의 대륙
	CONTINENT_OF_CHAOS			= 2,// 혼돈의 대륙
}E_CONTINENT_TYPE; 

typedef struct tagTBL_MapBuildData
:	public tagTBL_MapConfig
{
	tagTBL_MapBuildData(tagTBL_MapConfig const& kMapCfg,T_GNDATTR const _kGndAttr, int const _iMapKey)
		:	tagTBL_MapConfig(kMapCfg)
		,	kGndAttr(_kGndAttr)
		,	iMapKey(_iMapKey)
	{
	}

	T_GNDATTR	kGndAttr;
	int			iMapKey;

	void WriteToPacket(BM::Stream& kPacket)const 
	{
		tagTBL_MapConfig::WriteToPacket(kPacket);
		kPacket.Push(kGndAttr);
		kPacket.Push(iMapKey);
	}

	void ReadFromPacket(BM::Stream& kPacket)
	{
		tagTBL_MapConfig::ReadFromPacket(kPacket);
		kPacket.Pop(kGndAttr);
		kPacket.Pop(iMapKey);
	}

}TBL_MAP_BUILD_DATA;

typedef std::set<TBL_MAP_CONFIG>			CONT_MAP_CONFIG;
typedef std::set<TBL_MAP_BUILD_DATA>		CONT_MAP_BUILD_DATA;

//typedef  std::map< TBL_MAP_SERVER_CONFIG, SGroundAttribute > CONT_MAP_SERVER_CONFIG;// first key : ground info, second : ground Attribute (refrence : EServerSubType)

typedef enum eSwitchReservePlayerResult
{
	SRPR_NONE				= 0,
	SRPR_SUCEESS			= 1,
	SRPR_FAILED				= 2,
	SRPR_FAILED_IN_CENTER	= 3,//쎈터에서 할당 실패
	SRPR_FAILED_IN_SWITCH	= 4,//스위치 서버 할당 실패
}ESwitchReservePlayerResult;

typedef enum eDefExtVar
{
	EXT_VAR_NONE			= 0x0000,
	EXT_VAR_MOBILELOCK		= 0x0001,//모바일 인증 사용
	EXT_VAR_MOBILELOCK_REQ	= 0x0100,//모바일 인증 요청중
}E_DEF_EXT_VAR;

typedef enum eNoticeLevel
{
	EL_Normal		= 0,
	EL_Warning		= 1,	// 오류 메세지 
	EL_Level2		= 2,
	EL_PvPMsg		= 3,
	EL_GMMsg		= 4,	// GM 메세지

	EL_Notice1		= 5,	// 중요 정보 갱신사항
	EL_Notice2		= 6,	// 유저와 관계된 공지
	EL_Notice3		= 7,	// 정보갱신, 유저에게 알림사항
	EL_Notice4		= 8,	// 정보갱신, 능력치 알림
}ENoticeLevel;

typedef enum eConnectionArea
{
	CONN_AREA_BLOCK		= 0x00,
	CONN_AREA_UNKNOWN	= 0x00,
	CONN_AREA_PCROOM	= 0x01,
	CONN_AREA_GM		= 0xFF,// GM은 BLOCK을 제외한 타입을 보유하고 있어야 한다.
}EConnectionArea;

typedef struct tagConnectionArea
{
	tagConnectionArea()
		:	nArea(CONN_AREA_UNKNOWN)
		,	nCustomType(0)
	{}

	tagConnectionArea& operator=( tagConnectionArea const &rhs )
	{
		nArea = rhs.nArea;
		nCustomType = rhs.nCustomType;
		return *this;
	}

	void Clear()
	{
		nArea = CONN_AREA_UNKNOWN;
		nCustomType = 0;
	}

	BM::vstring ToString() const
	{
		return BM::vstring(L"SConnectionArea[A:")<<nArea<<L",CT:"<<nCustomType<<L"]";
	}

	short	nArea;// EConnectionArea 사용
	short	nCustomType;

	DEFAULT_TBL_PACKET_FUNC();
}SConnectionArea;

int const MAX_NAME_LEN = 21;
int const MAX_PW_ENCRYPT_SIZE = 60;
int const MAX_PATCH_VERSION = 15;

typedef struct tagClientTryLogin
{
	tagClientTryLogin();
	tagClientTryLogin(tagClientTryLogin const &rhs);

	CLASS_DECLARATION_S(std::wstring, ID)
	CLASS_DECLARATION_S(std::wstring, PW)
	SConnectionArea m_kConnArea;
	TCHAR szVersion1[MAX_PATCH_VERSION];
	TCHAR szVersion2[MAX_PATCH_VERSION];
	CLASS_DECLARATION_S(short, RealmNo);
	CLASS_DECLARATION_S(short, ChannelNo);
	CLASS_DECLARATION_S(__int64,PatchVersion);

	bool ReadFromPacket(BM::Stream& kPacket);
	void WriteToPacket(BM::Stream& kPacket) const;
	void MakeCorrect();//잘못될 수 있는 데이터를 가공.	
	void Clear();
	bool SetID(std::wstring const &wstrInID);
	bool SetPW(std::wstring const &wstrInPW);
	bool SetVersion( TCHAR const* pszVersion1, TCHAR const* pszVersion2 );
	BM::vstring ToString() const;
	size_t min_size() const;
	static int GetIDMaxLength();
	static int GetPWMaxLength();
}SClientTryLogin;

typedef struct tagServerTryLogin
	: public SClientTryLogin
{
	tagServerTryLogin()
		:	ulExpireTime(0)
		,	iUID(0)
		,	bNewLogin(true)
	{}

	tagServerTryLogin( tagClientTryLogin const &rhs )
		:	tagClientTryLogin(rhs)
		,	ulExpireTime(0)
		,	iUID(0)
		,	bNewLogin(true)
	{}

	tagServerTryLogin( tagServerTryLogin const &rhs )
		:	tagClientTryLogin(rhs)
		,	m_kSessionKey(rhs.m_kSessionKey)
		,	addrRemote(rhs.addrRemote)
		,	kLoginServer(rhs.kLoginServer)
		,	ulExpireTime(rhs.ulExpireTime)
		,	iUID(rhs.iUID)
		,	bNewLogin(rhs.bNewLogin)
		,	iEventRewardKey(rhs.iEventRewardKey)
	{
	}

	void Clear()
	{
		__super::Clear();
		m_kSessionKey.Clear();
		addrRemote.Clear();
		kLoginServer.Clear();
		ulExpireTime = 0;
		iUID = 0;
		bNewLogin = true;
		iEventRewardKey = 0;
	}

	BM::vstring ToString() const
	{
		return BM::vstring(L"SServerTryLogin[")<<__super::ToString()
			<<L", SessionKey:"<<C2L(m_kSessionKey)<<L",AddrRemote:"<<C2L(addrRemote)<<L",L-Server:"<<C2L(kLoginServer)<<L",ExpireTime:"<<ulExpireTime<<L",NewLogin:"<<bNewLogin<<L", UID:"<<iUID<<L"]";
	}

	bool ReadFromPacket(BM::Stream& kPacket)
	{
		__super::ReadFromPacket(kPacket);

		kPacket.Pop(m_kSessionKey);
		addrRemote.ReadFromPacket(kPacket);
		kLoginServer.ReadFromPacket(kPacket);
		kPacket.Pop(ulExpireTime);
		kPacket.Pop(bNewLogin);
		kPacket.Pop(iUID);
		return kPacket.Pop(iEventRewardKey);
	}

	void WriteToPacket(BM::Stream& kPacket) const
	{
		__super::WriteToPacket(kPacket);

		kPacket.Push(m_kSessionKey);
		addrRemote.WriteToPacket(kPacket);
		kLoginServer.WriteToPacket(kPacket);
		kPacket.Push(ulExpireTime);
		kPacket.Push(bNewLogin);
		kPacket.Push(iUID);
		kPacket.Push(iEventRewardKey);
	}

	size_t min_size() const
	{
		return __super::min_size()
			+ sizeof(CEL::SESSION_KEY)
			+ sizeof(CEL::ADDR_INFO)
			+ sizeof(SERVER_IDENTITY)
			+ sizeof(unsigned long)
			+ sizeof(bool)
			+ sizeof(int) * 2;
	}


	CLASS_DECLARATION_S(CEL::SESSION_KEY, SessionKey);
	CEL::ADDR_INFO addrRemote;//유저의 IP
	SERVER_IDENTITY kLoginServer; 
	unsigned long ulExpireTime;
	bool bNewLogin;

	int iUID;		// UID(NC)

	int iEventRewardKey;

}SServerTryLogin;

typedef enum eTryLoginResult//로그인->TryLogin 스위치->TryAccess //두군데 모두 사용
{
	E_TLR_NONE		= 0,
	E_TLR_SUCCESS	= 1,
	E_TLR_FAILED	= 2,
	E_TLR_WAIT		= 3,
	E_TLR_SUCCESS_AUTH	= 4,
	E_TLR_TIMEOVER	= 5,

	// information error
	// 아래 에러코드의 번호는 Client의 TextTable.xml의 에러메시지와 연관있으니, 번호를 함부로 바꾸면 안됩니다.
	E_TLR_ALREADY_TRY				= 10,//같은세션에서 연속해서 눌러서 그렇게된것.
	E_TLR_WRONG_ID					= 11,
	E_TLR_WRONG_PASS				= 12,
	E_TLR_TOO_MANY_USER				= 13,
	E_TLR_WRONG_VERSION				= 23,
	E_TRL_WRONG_CHANNEL				= 32,//잘못된 채널 시도
	E_TLR_TOO_MANY_WATING			= 33,//대기자가 너무 많아
	E_TLR_NO_SWITCH					= 34,//스위치서버가 없어
	E_TLR_INCORRECT_LOGIN_SERVER	= 35,
	E_TLR_NOR_RESERVED_USER			= 36,
	E_TLR_BLOCKED_USER				= 37,
	E_TLR_CHANNEL_NOT_SERVICE_NOW	= 38,
	E_TLR_CHANNEL_INFO_INCORRECT	= 39,//채널 정보가 올바르지 않음
	E_TLR_ALREADY_WAIT_FOR_SWITCH	= 40,//이미 스위치를 기다리는 유저임.
	E_TLR_ALREADY_LOGINED			= 41,//이미 누가 게임중.
	E_TLR_WRONG_PATCH_VERSION		= 42,
	E_TLR_CHANNEL_FULLUSER			= 43,//더이상 해당 채널에 들어 갈 수 없다.
	E_TLR_BLOCKED_IP				= 44,//블록된 IP
	E_TLR_NOT_GMIP					= 45,//GMIP가 아니다.
	E_TLR_BLOCKED_SERVER_LOGIN		= 46,// 로그인이 블록되어 있는 상태
	E_TLR_OTHERLOGIN				= 47,
	E_TLR_EXTVAR_ERROR				= 48,

	// system error
	E_TLR_LOGIN_ERROR				= 51,
	E_TRL_CENTER_ERROR				= 52, 
	E_TLR_DB_ERROR					= 53,
	E_TLR_SWITCH_ERROR				= 54, 
	E_TLR_MEMORY_ERROR		= 55,
	E_TLR_PARSING_ERROR		= 56,			// GALA : HTTP Parsing error
	E_TLR_EXTERNAL_SERVER_ERROR		= 57,	// 외부연동서버 오류
	E_TLR_EXTERNAL_SERVER_DOWN		= 58,	// 외부연동서버 shutdown
	E_TLR_INCORRECT_PERMISSION		= 59,


	E_TLR_AUTHCHECK_ERROR_INVALID_ARGUMENT	= 60,					//	Argument is not valid
	E_TLR_AUTHCHECK_ERROR_INVALID_PASSPORT	= 61,					//	NexonPassport is not valid
	E_TLR_AUTHCHECK_ERROR_TIMEOUT			= 62,						//	[obsolete] same as AUTHCHECK_ERROR_EXPIRED
//	AUTHCHECK_ERROR_EXPIRED:	//	NexonPassport is valid, but arleady expired
	E_TLR_AUTHCHECK_ERROR_AUTH_SERVER_SHUTDOWN	 = 63,				//	Authentication server is shutdown
	E_TLR_AUTHCHECK_ERROR_UNSUPPORTED_LOCALE	 = 64,					//	Unsupported Locale ID
	E_TLR_AUTHCHECK_ERROR_SET_LOCALE_NEEDED		 = 65,					//	call AuthCheck_SetLocale first
	E_TLR_AUTHCHECK_ERROR_NO_SERVER_INFORMATION	 = 66,				//	no server information in config file
	E_TLR_AUTHCHECK_ERROR_USER_IP_MISMATCHED	 = 67,					//	client ip address is mismatched with sso servers data
	E_TLR_AUTHCHECK_ERROR_NO_DECRYPTION_KEY		 = 68,				//	There is no passport decryption key. Check AuthCheck_LoadKey function or key file.
	
	E_TLR_NOT_CONFIRM_EMAIL	= 70,	// 이메일 인증을 하지 않음
	E_TLR_PAYPAL_BLOCK		= 71,	// PAYPAL Block( 그라비티 )
	E_TLR_RESTRICT_AGE		= 72,	// 게임에 허용되지 않는 나이
	E_TLR_NOT_AVALABLE		= 73,	// 탈퇴한 유저


	// 다른건 89까지만 사용 가능
	// GF Mobile Lock
	E_TLR_MOBILE_LOCK					= 90,	// 모바일 인증 진행중
	E_TLR_MOBILE_LOCK_SUCCESS			= 91,	// 모바일 인증 성공
	E_TLR_MOBILE_LOCK_FAILED			= 92,
	E_TLR_MOBILE_LOCK_FAILED_TIMEOUT	= 93,	// 모바일 개통시간 경과
	E_TLR_MOBILE_LOCK_FAILED_OVERLAP1	= 94,
	E_TLR_MOBILE_LOCK_FAILED_OVERLAP2	= 95,
	E_TLR_MOBILE_LOCK_FAILED_WRONGDATA	= 96,	// 유저 데이터가 이상하다?

	//NC Error
	E_TLR_NC_ERROR_BASE		= 100,//100 이후로 추가 마세요.
	
}ETryLoginResult;

typedef enum eSelectCharacterResult
{
	E_SCR_SUCCESS			= 0,
	E_SCR_SYSTEMCHECK		= 1,
	E_SCR_RECENEMAP_ERROR	= 2,
	E_SCR_LASTVILLAGE_ERROR	= 3,
	E_SCR_Q_RANDOMMAP_MOVE	= 4,
	E_SCR_ErrorState		= 5,
	E_SCR_PLAYTIMEOVER		= 6,
	E_SCR_CHARACTERCOUNT_ERROR		= 7,
}ESelectCharacterRet;

int const MAX_CHARACTERNAME_LEN		= (20);
int const MAX_PARTYNAME_LEN			= MAX_CHARACTERNAME_LEN;
int const MAX_DB_EFFECTS_SIZE		= (500);
int const MAX_DB_QUICKSLOT_SIZE		= (281);	// 5(스트럭쳐사이즈) *  8(필드) * 7(로우) + 1(선택페이지 번호 char)
int const MAX_DB_INGQUEST_SIZE		= (350);	// 진행 퀘스트 정보 저장 공간 크기
int const MAX_DB_ENDQUEST_SIZE		= (130);	// 한 세트의 End퀘스트 저장 공간 크기 (한세트당 1040개)
int const MAX_DB_ENDQUEST_EXT_SIZE	= (260);	// 확장 End퀘스트 저장공간 크기 (2080개)
int const MAX_DB_ENDQUEST_EXT2_SIZE	= (480);	// 확장 End퀘스트2 저장공간 크기 (3840개)
int const MAX_DB_ENDQUEST_ALL_SIZE	= (MAX_DB_ENDQUEST_SIZE * 2) + MAX_DB_ENDQUEST_EXT_SIZE + MAX_DB_ENDQUEST_EXT2_SIZE; // 전체 End퀘스트 저장공간 크기
int const MAX_DB_SKILL_SIZE			= (450);
int const MAX_DB_GUILD_SKILL_SIZE	= (200);
int const MAX_DB_INVEXTEND_SIZE		= (40);		// 인밴토로 확장/축소 정보 저장크기
int const MAX_DB_EFFECT_SIZE		= (200);
int const MAX_DB_EMOTICON_SIZE		= (20);		// 이모티콘 사이즈(8*20) 
int const MAX_DB_SKILL_EXTEND_SIZE	= (560);	// 스킬 배우기 확장 저장 공간 스킬번호 + 확장 단계 현재 저장 가능한 스킬 최대 갯수는 112개에 맞춘것임
int const MAX_DB_GUILD_INV_AUTHORITY = (4);		// 길드인벤 넣기/빼기 권한 넣기(2)/빼기(2) : 넣기/빼기별로 길드멤버의 지정된 등급 이상만 사용 가능
int const MAX_DB_JOBKSILL_SAVEIDX_SIZE = (256);	// 채집(직업스킬) SaveIdx 필드 사이즈
int const MAX_JOBSKILL_HISTORYITEM	= (10);
int const MAX_DB_JOBKSILL_HISTORYITEM_SIZE = (MAX_JOBSKILL_HISTORYITEM*sizeof(DWORD));	// 채집(직업스킬) 최근에 제작한 아이템 필드 사이즈
int const MAX_DB_ITEM_GROUP_COOLTIMEP_SIZE = (256); // 아이템 그룹 쿨타임 정보

//
template< typename _ArrayType >
class PgArray
{
public:
	template< size_t _ArraySize >
	inline explicit PgArray(_ArrayType (&abyArray)[_ArraySize])
		:m_pArray(abyArray), m_iArraySize(sizeof(abyArray))
	{
	}
	inline PgArray(PgArray const& rhs)
		:m_pArray(rhs.m_pArray), m_iArraySize(rhs.m_iArraySize)
	{
	}
	~PgArray() {}

	operator _ArrayType* () { return m_pArray; }
	operator _ArrayType const* ()const { return m_pArray; }

	template< typename _PosType >
	_ArrayType& operator[](_PosType iPos) { return m_pArray[iPos]; }
	template< typename _PosType >
	_ArrayType const& operator[](_PosType iPos)const { return m_pArray[iPos]; }

	size_t Size()const { return m_iArraySize; }
	void Clear() { ::ZeroMemory(m_pArray, m_iArraySize); }

	bool Empty()const { return NULL == m_pArray; }

private:
	_ArrayType *m_pArray;
	size_t const m_iArraySize;
};


//
template< typename _ArrayType, size_t _ArraySize >
class PgEternalArray
{
public:
	inline explicit PgEternalArray(_ArrayType (&abyArray)[_ArraySize])
		:m_pArray(abyArray)
	{
	}
	inline PgEternalArray(PgEternalArray const& rhs)
		:m_pArray(rhs.m_pArray)
	{
	}
	~PgEternalArray() {}

	operator _ArrayType* () { return m_pArray; }
	operator _ArrayType const* ()const { return m_pArray; }

	template< typename _PosType >
	_ArrayType& operator[](_PosType iPos) { return m_pArray[iPos]; }
	template< typename _PosType >
	_ArrayType const& operator[](_PosType iPos)const { return m_pArray[iPos]; }

	size_t Size()const { return _eSize; }
	void Clear() { ::ZeroMemory(m_pArray, Size()); }

	bool Empty()const { return NULL == m_pArray; }

private:
	enum
	{
		_eSize = sizeof(_ArrayType)*_ArraySize,
	};
	_ArrayType *m_pArray;
};

//
typedef PgEternalArray< BYTE, MAX_DB_INGQUEST_SIZE > PgQuestIngBuffer;
typedef PgEternalArray< BYTE, MAX_DB_ENDQUEST_SIZE > PgQuestEndBuffer;
typedef PgEternalArray< BYTE, MAX_DB_ENDQUEST_EXT_SIZE > PgQuestEndExtBuffer;
typedef PgEternalArray< BYTE, MAX_DB_ENDQUEST_EXT2_SIZE > PgQuestEndExt2Buffer;
typedef PgEternalArray< BYTE const, MAX_DB_INGQUEST_SIZE > PgQuestIngBufferR;
typedef PgEternalArray< BYTE const, MAX_DB_ENDQUEST_SIZE > PgQuestEndBufferR;
typedef PgEternalArray< BYTE const, MAX_DB_ENDQUEST_EXT_SIZE > PgQuestEndExtBufferR;
typedef PgEternalArray< BYTE const, MAX_DB_ENDQUEST_EXT2_SIZE > PgQuestEndExt2BufferR;

typedef struct tagGroundKey
{
	tagGroundKey()
	{
		Clear();
	}

	tagGroundKey(int const iGroundNo, BM::GUID const &kGuid = BM::GUID::NullData())
	{
		Set(iGroundNo, kGuid);
	}

	void Set(int const iGroundNo, BM::GUID const &kGuid = BM::GUID::NullData())
	{
		GroundNo(iGroundNo);
		Guid(kGuid);
	}

	void Clear()
	{
		Set(0, BM::GUID::NullData());
	}

	bool operator < (tagGroundKey const &rhs) const
	{	
		if(GroundNo() < rhs.GroundNo()){ return true; }
		if(rhs.GroundNo() < GroundNo() ){ return false; }

		if(Guid() < rhs.Guid()){ return true; }
		if(rhs.Guid() < Guid()){ return false; }
		return false;
	}

	bool operator > (tagGroundKey const &rhs)const
	{
		return rhs < *this;
	}

	bool operator == (tagGroundKey const &rhs)const
	{
		if(	(GroundNo() == rhs.GroundNo()) 
		&&	(Guid() == rhs.Guid()))
		{
			return true;
		}
		return false;
	}

	bool operator != (tagGroundKey const &rhs) const
	{
		return !(*this == rhs);
	}

	bool IsField()const
	{
		if(GroundNo())
		{
			if(Guid() == BM::GUID::NullData())
			{
				return true;
			}
		}
		return false;
	}

	bool IsEmpty()const
	{
		return GroundNo() == 0;
	}

	CLASS_DECLARATION_S(int, GroundNo);
	CLASS_DECLARATION_S(BM::GUID, Guid);

	size_t min_size()const
	{
		return 
			sizeof(m_kGroundNo)+
			sizeof(m_kGuid);
	}

	BM::vstring ToString()const
	{
		return BM::vstring(L" GroundKey[No:")<<GroundNo()<<L",G:"<<Guid()<<L"]";
	}

	bool ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(m_kGroundNo);
		return rkPacket.Pop(m_kGuid);
	}

	void WriteToPacket(BM::Stream& rkPacket)const
	{
		rkPacket.Push(GroundNo());
		rkPacket.Push(Guid());
	}
}SGroundKey;

typedef struct tagChnGroundKey
:	public tagGroundKey
{
	tagChnGroundKey()
		:	nChannelNo(0)
	{

	}

	explicit tagChnGroundKey( tagGroundKey const &kGndKey, short const _nChannelNo )
		:	tagGroundKey(kGndKey)
		,	nChannelNo(_nChannelNo)
	{
	}

	void Clear()
	{
		tagGroundKey::Clear();
		nChannelNo = 0;
	}

	tagChnGroundKey& operator = (tagGroundKey const &rhs)
	{	
		GroundNo( rhs.GroundNo() );
		Guid( rhs.Guid() );
		return *this;
	}

	bool operator < (tagChnGroundKey const &rhs)const
	{	
		if ( nChannelNo == rhs.nChannelNo )
		{
			return tagGroundKey::operator < ( static_cast<tagGroundKey>(rhs) );
		}
		return nChannelNo < rhs.nChannelNo;
	}

	bool operator > (tagChnGroundKey const &rhs)const
	{
		return rhs < *this;
	}

	bool operator == (tagChnGroundKey const &rhs)const
	{
		if ( nChannelNo == rhs.nChannelNo )
		{
			return tagGroundKey::operator == ( static_cast<tagGroundKey>(rhs) );
		}
		return false;
	}

	bool operator != (tagChnGroundKey const &rhs)const
	{
		return !(*this == rhs);
	}

	size_t min_size()const
	{
		return 
				tagGroundKey::min_size()
			+	sizeof(nChannelNo);
	}

	BM::vstring ToString()const
	{
		return BM::vstring(L"ChnGroundKey[Chn:")<<Channel()<< L",No:"<<GroundNo()<<L",G:"<<Guid()<<L"]";
	}

	bool ReadFromPacket(BM::Stream& rkPacket)
	{
		if ( true == tagGroundKey::ReadFromPacket(rkPacket) )
		{
			return rkPacket.Pop( nChannelNo );
		}
		return false;
	}

	void WriteToPacket(BM::Stream& rkPacket)const
	{
		tagGroundKey::WriteToPacket(rkPacket);
		rkPacket.Push( nChannelNo );
	}

	CLASS_DECLARATION(short, nChannelNo, Channel);
}SChnGroundKey;

typedef std::list< SGroundKey > GND_KEYLIST;

typedef struct tagGroundOwnerInfo
{
	typedef enum
	{
		OWNER_NONE,
		OWNER_CHARACTER,//개인(마이홈에서 사용하면 될듯)
		OWNER_PARTY,// 파티원들이 오너이다
		OWNER_GUILD,// 길드
	}E_OWNERGUID_TYPE;

	tagGroundOwnerInfo()
		:	byType(OWNER_NONE)
	{}

	explicit tagGroundOwnerInfo( BYTE const _byType, BM::GUID const &_kID )
		:	byType(_byType)
		,	kID(_kID)
	{}

	bool operator < ( tagGroundOwnerInfo const &rhs )const
	{
		if ( byType == rhs.byType )
		{
			return kID < rhs.kID;
		}
		return byType < rhs.byType;
	}

	bool operator > ( tagGroundOwnerInfo const &rhs )const{return rhs < *this;}
	bool operator == ( tagGroundOwnerInfo const &rhs )const{return (kID == rhs.kID) && (byType == rhs.byType);}
	bool operator != ( tagGroundOwnerInfo const &rhs )const{return !((*this) == rhs);}

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kPacket.Push( byType );
		if ( OWNER_NONE != byType )
		{
			kPacket.Push( kID );
		}
	}

	bool ReadFromPacket( BM::Stream &kPacket )
	{
		if ( true == kPacket.Pop( byType ) )
		{
			if ( OWNER_NONE != byType )
			{
				return kPacket.Pop( kID );
			}
				
			kID = BM::GUID::NullData();
			return true;
		}
		return false;
	}

	size_t min_size(void)const
	{
		return	sizeof(byType)
			+	sizeof(kID);
	}

	size_t max_size(void)const{return min_size();}

	BYTE		byType;//E_OWNERGUID_TYPE
	BM::GUID	kID;

}SGroundOwnerInfo;

typedef struct tagRecentInfo
{
	tagRecentInfo(int const iMap=0, POINT3 const &ptPos = POINT3(0.0f, 0.0f, 0.0f))
		:	iMapNo(iMap)
		,	ptPos3(ptPos)
	{}

	bool UpdateGndInfo( SGroundKey const &rkKey )
	{
		if(	!rkKey.GroundNo() )
//		||	rkKey.GroundNo() == iMapNo )//성공임.
		{
			return false;
		}
		iMapNo = rkKey.GroundNo();
		return true;
	}

	void Clear(){*this = tagRecentInfo( 0 );}
	bool IsEmpty()const{return 0 == iMapNo;}

	int				iMapNo;
	POINT3			ptPos3;

	DEFAULT_TBL_PACKET_FUNC();

}SRecentInfo;

typedef struct tagClassKey
{
	int iClass;
	short nLv;

	tagClassKey()
	{
		Set(0,0);
	}

	tagClassKey(int const iInClass, short const nInLv)
	{
		Set(iInClass, nInLv);
	}

	void Set(int const iInClass, short const nInLv)
	{
		iClass = iInClass;
		nLv = nInLv;
	}

	bool operator < ( tagClassKey const &rhs)const
	{
		if( iClass < rhs.iClass )	{return true;}
		if( iClass > rhs.iClass )	{return false;}

		if( nLv < rhs.nLv )	{return true;}
		if( nLv > rhs.nLv )	{return false;}

		return false;
	}

	bool operator == ( tagClassKey const &rhs )const
	{
		return (iClass == rhs.iClass) && (nLv == rhs.nLv); 
	}

	bool operator != (const tagClassKey &rhs)const
	{
		return !( this->operator == ( rhs ) );
	}

}SClassKey;

typedef struct tagSPlayerBinaryData
{//가공되어 쓰이는 데이터
	tagSPlayerBinaryData()
	{
		::memset(abySkills, 0, sizeof(abySkills));
		::memset(abySkillExtends, 0, sizeof(abySkillExtends));
		
		::memset(abyQuickSlot, 0, sizeof(abyQuickSlot));
		::memset(abyEffect, 0, sizeof(abyEffect));
		ClearQuest();
		::memset(abyEmoticon, 0, sizeof(abyEmoticon ));
		::memset(abyEmotion, 0, sizeof(abyEmotion));
		::memset(abyBalloon, 0, sizeof(abyBalloon));
		::memset(dwJobSkillHistoryItem, 0, sizeof(dwJobSkillHistoryItem));
	}

	void ClearQuest()
	{
		::memset(abyIngQuest, 0, sizeof(abyIngQuest));
		::memset(abyEndQuest, 0, sizeof(abyEndQuest));
		::memset(abyEndQuest2, 0, sizeof(abyEndQuest));
		::memset(abyEndQuestExt, 0, sizeof(abyEndQuestExt));
		::memset(abyEndQuestExt2, 0, sizeof(abyEndQuestExt2));
	}

	BYTE abyQuickSlot[MAX_DB_QUICKSLOT_SIZE];// (SItemPos 단위로 입력됨) 
	BYTE abyIngQuest[MAX_DB_INGQUEST_SIZE];
	BYTE abyEndQuest[MAX_DB_ENDQUEST_SIZE];
	BYTE abyEndQuest2[MAX_DB_ENDQUEST_SIZE];
	BYTE abyEndQuestExt[MAX_DB_ENDQUEST_EXT_SIZE];
	BYTE abyEndQuestExt2[MAX_DB_ENDQUEST_EXT2_SIZE];
	BYTE abySkills[MAX_DB_SKILL_SIZE];
	BYTE abyEffect[MAX_DB_EFFECT_SIZE];
	BYTE abyEmoticon[MAX_DB_EMOTICON_SIZE];	// 이모티콘 그룹
	BYTE abyEmotion[MAX_DB_EMOTICON_SIZE];	// 이모션 그룹
	BYTE abyBalloon[MAX_DB_EMOTICON_SIZE];	// 풍선 이모티콘 그룹
	BYTE abySkillExtends[MAX_DB_SKILL_EXTEND_SIZE];
	DWORD dwJobSkillHistoryItem[MAX_JOBSKILL_HISTORYITEM];
}SPlayerBinaryData;

typedef struct tagSPlayerMapSaveData
{
	tagSPlayerMapSaveData()
	{
		Init();
	}

	void Init()
	{
		guidCharacter = BM::GUID::NullData();
		iClass	= 0;
		wLv	= 0;
		i64Exp = 0;
		i64Money = 0;
		iHP	= 0;
		iMP	= 0;
		iCP = 0;
		sSP = 0;
		kGuildGuid.Clear();
		kCoupleGuid.Clear();
		kCoupleColorGuid.Clear();
		kCoupleStatus = 0;
		kRecentInfo.Clear();
		kLastVillage.Clear();
		
		i64Fran = 0;
		sTacticsLevel = 0;
		i64TacticsExp = 0;
		kRefreshDate.Clear();
		iRecommendPoint = 0;
		Strategy_fatigability = 0;
	}
	BM::GUID guidCharacter;
	int iClass;//OK
	WORD wLv;//OK
	__int64 i64Exp;//OK
	__int64 i64Money;//INV
	int iHP;//동기 안함
	int iMP;//동기 안함
	int iCP;//INV
	WORD sSP;//OK ?
	BM::GUID kGuildGuid;
	BM::GUID kCoupleGuid;
	BM::GUID kCoupleColorGuid;
	BYTE kCoupleStatus;
	SRecentInfo kRecentInfo;
	SRecentInfo kLastVillage;
	__int64 i64Fran;//OK
	unsigned short sTacticsLevel;
	__int64 i64TacticsExp;

	BM::PgPackedTime	kRefreshDate;
	int					iRecommendPoint;
	short				Strategy_fatigability;	// 전략피로도.
}SPlayerMapSaveData;

int const MAX_CHARGE_COPY_MACHINE = 11; // 포인트 복제기 최대 누적 횟수

typedef struct tagSPlayerStrategySkillData
{//가공되어 쓰이는 데이터
	tagSPlayerStrategySkillData()
		: sSP(0)
	{
		::memset(abySkills, 0, sizeof(abySkills));
		::memset(abySkillExtends, 0, sizeof(abySkillExtends));
		::memset(abyQuickSlot, 0, sizeof(abyQuickSlot));
	}

	WORD sSP;
	BYTE abySkills[MAX_DB_SKILL_SIZE];
	BYTE abySkillExtends[MAX_DB_SKILL_EXTEND_SIZE];
	BYTE abyQuickSlot[MAX_DB_QUICKSLOT_SIZE];// (SItemPos 단위로 입력됨) 
}SPlayerStrategySkillData;

typedef struct tagPlayerDateContents
{
	tagPlayerDateContents()
	{
		Clear();
	}
	tagPlayerDateContents(tagPlayerDateContents const& rhs)
		: kGuildLeaveDate(rhs.kGuildLeaveDate)
	{
	}
	~tagPlayerDateContents()
	{
	}

	void Clear()
	{
		kGuildLeaveDate.Clear();
	}

	BM::DBTIMESTAMP_EX kGuildLeaveDate;
} SPlayerDateContents;

typedef struct tagSPlayerDBData
	: public SPlayerMapSaveData
{
	tagSPlayerDBData()
	{
		Init();	
	}
	void Init()
	{
//		guidMember = BM::GUID::NullData();
		byState = 0;
		::memset(szName, 0, sizeof(szName));
		byGender = 0;

		iRace = 0;
		iHairColor = 0;
		iHairStyle = 0;
		iFace = 0;
		iJacket = 0;
		
		iPants = 0;
		iShoes = 0;
		iGloves = 0;
		pt3Pos.Clear();
		SPlayerMapSaveData::Init();
		
		ZeroMemory(cGuildSkill, MAX_DB_GUILD_SKILL_SIZE);
		byFiveElementBody = 0;
		i64TotalPlayTimeSec_Character = 0;
		sBonusStatus = sAddedStr = sAddedInt = sAddedCon = sAddedDex = 0;

		StrategySkillOpen = StrategySkillTabNo = 0;

		kDateContents.Clear();

		byCharactorSlot = 0;
	}
//	BM::GUID guidMember;
	BYTE byState;	// Delete or NeedRename Flag.
	TCHAR szName[MAX_CHARACTERNAME_LEN + 1];
	BYTE byGender;
	
	int iRace;
	int	iHairColor;	// 캐릭터생성시 머리색깔
	int iHairStyle;	// 캐릭터생성시 머리스타일
	int iFace;		// 캐릭터생성시 얼굴모양
	int iJacket;	// 캐릭터생성시 상의
	
	int iPants;		// 캐릭터생성시 하의
	int iShoes;		// 캐릭터생성시 신발
	int iGloves;	// 캐릭터생성시 장갑
	BYTE byFiveElementBody;//
	POINT3		pt3Pos;
	unsigned short sBonusStatus;
	unsigned short sAddedStr;
	unsigned short sAddedInt;
	unsigned short sAddedDex;
	unsigned short sAddedCon;
	BYTE StrategySkillOpen;
	BYTE StrategySkillTabNo;
	
	BM::PgPackedTime kBirthDate;
	BYTE cGuildSkill[MAX_DB_GUILD_SKILL_SIZE];
	__int64 i64TotalPlayTimeSec_Character;

	SPlayerDateContents kDateContents;

	BYTE  byCharactorSlot;
}SPlayerDBData;

typedef struct tagPlayerBasicInfo
{
	tagPlayerBasicInfo()
	{
		iMaxHP= 0;		// Basic Max HP (DB value)
		iMaxMP= 0;		// Basic Max MP
		iHPRecoveryInterval= 0;	// Basic HP Recovery Interval
		iMPRecoveryInterval= 0;	// Basic MP Recovery Interval
		sHPRecovery= 0;
		sMPRecovery= 0;

		sInitStr= 0;			// Basic Strength
		sInitInt= 0;			// Basic Intelligence
		sInitCon= 0;			// Basic Constitution
		sInitDex= 0;			// Basic Dexterity
		
		sMoveSpeed= 0;		// Basic Move speed
		sPhysicsDefence= 0;	// Basic Physics Defence
		sMagicDefence= 0;	// Basic Magic Defence
		sAttackSpeed= 0;		// Basic Attack speed
		sBlockRate= 0;		// Basic Block-attack rate
		
		sDodgeRate= 0;		// Basic Dodge-attack rate (회피률)
		sCriticalRate= 0;	// AT_CRITICAL_SUCCESS_VALUE : Basic Critical Rate
		sCriticalPower= 0;	// AT_CRITICAL_POWER : Basic Critical Power
		sHitRate= 0;			// AT_HIT_SUCCESS_VALUE
		iAIType= 0;	// AT_AI_TYPE : AI Type
	}

	int iMaxHP;		// Basic Max HP (DB value)
	int iMaxMP;		// Basic Max MP
	int	iHPRecoveryInterval;	// Basic HP Recovery Interval
	int	iMPRecoveryInterval;	// Basic MP Recovery Interval
	unsigned short  sHPRecovery;
	unsigned short sMPRecovery;

	unsigned short  sInitStr;			// Basic Strength
	unsigned short  sInitInt;			// Basic Intelligence
	unsigned short  sInitCon;			// Basic Constitution
	unsigned short  sInitDex;			// Basic Dexterity
	
	unsigned short  sMoveSpeed;		// Basic Move speed
	unsigned short  sPhysicsDefence;	// Basic Physics Defence
	unsigned short  sMagicDefence;	// Basic Magic Defence
	unsigned short  sAttackSpeed;		// Basic Attack speed
	unsigned short  sBlockRate;		// Basic Block-attack rate
	
	unsigned short  sDodgeRate;		// Basic Dodge-attack rate (회피률)
	unsigned short  sCriticalRate;	// AT_CRITICAL_SUCCESS_VALUE : Basic Critical Rate
	unsigned short  sCriticalPower;	// AT_CRITICAL_POWER : Basic Critical Power
	unsigned short  sHitRate;			// AT_HIT_SUCCESS_VALUE
	int iAIType;	// AT_AI_TYPE : AI Type
}SPlayerBasicInfo;

typedef struct tagSPlayerInfo
	: public SPlayerDBData
{
	
} SPlayerInfo;

typedef struct tagEntityInfo
{
	tagEntityInfo( BM::GUID const &_kGuid, SClassKey const &_kClassKey )
		:	kGuid( _kGuid )
		,	kClassKey(_kClassKey)
		,	kCaller( BM::GUID::NullData() )
		,	kParty(BM::GUID::NullData() )
		,	bSyncUnit(false)
		,	bEternalLife(false)
        ,   iTunningNo(0)
        ,   iTunningLevel(0)
	{}
	BM::GUID kGuid;
	BM::GUID kCaller;
	BM::GUID kParty;
	SClassKey kClassKey;
	SClassKey kCallerClassKey;
	bool bSyncUnit;
	bool bEternalLife;
    int iTunningNo;
    int iTunningLevel;
	std::map<WORD, int> kAbil;

	void SetAbil(WORD wAbil, int iValue)
	{
		kAbil.insert(std::make_pair(wAbil, iValue));
	}
} SEntityInfo;

typedef struct tagCustomUnitInfo
{
	tagCustomUnitInfo(BM::GUID const &_kGuid, SClassKey const &_kClassKey, POINT3 const &_ptPos)
		: kGuid(_kGuid), kClassKey(_kClassKey), ptPos(_ptPos)
	{}

	BM::GUID kGuid;
	SClassKey kClassKey;
	POINT3 ptPos;
}SCustomUnitInfo;

size_t const DEFAULT_CHARACTER_COUNT	= 4;
size_t const DEFAULT_EXTEND_CHARACTER_COUNT = 4;
size_t const MEMBER_BLOCK_MARK_MIN		= 100;

//S -> C
typedef enum eSendOptionFlag
{
	SOF_NONE		= 0,
	SOF_LEVELRANK	= 0x01,
} ESendOptionFlag;

typedef enum eCheckOverlapMsgType
{
	COLT_NONE								= 0,
	COLT_ERROR_JS3_CREATE_EXPERTNESS_MAX	= 0x0001,
} ECheckOverlapMsgType;

typedef struct tagSwitchReserveResult//스위치 할당 되었다고 쎈터가 로그인으로 보냄
	:	public tagServerTryLogin
{	
	tagSwitchReserveResult()
	{
		eRet = SRPR_NONE;
	}
	tagSwitchReserveResult( tagServerTryLogin const &rhs )
		: tagServerTryLogin(rhs)
	{
	}

	bool ReadFromPacket(BM::Stream& kPacket)
	{
		__super::ReadFromPacket(kPacket);
		kPacket.Pop(eRet);
		addrSwitch.ReadFromPacket(kPacket);
		return kPacket.Pop(guidSwitchKey);
	}

	void WriteToPacket(BM::Stream& kPacket)
	{
		__super::WriteToPacket(kPacket);
		kPacket.Push(eRet);
		addrSwitch.WriteToPacket(kPacket);
		kPacket.Push(guidSwitchKey);
	}

	ESwitchReservePlayerResult	eRet;
	CEL::ADDR_INFO	addrSwitch;		//스위치 주소
	BM::GUID	guidSwitchKey;	//스위치에 잡혀있는 유저의 고유키
}SSwitchReserveResult;
					 
typedef struct tagTryAccessSwitch// SSwitchReserveResult을 받아서 대응하는 패킷
	:	public SClientTryLogin
{
	tagTryAccessSwitch()
	{
	}

	bool ReadFromPacket(BM::Stream& kPacket)
	{
		__super::ReadFromPacket(kPacket);
		return kPacket.Pop(guidSwitchKey);
	}

	void WriteToPacket(BM::Stream& kPacket)
	{
		__super::WriteToPacket(kPacket);
		kPacket.Push(guidSwitchKey);
	}

	BM::GUID	guidSwitchKey;	//스위치에 잡혀있는 유저의 고유키
}STryAccessSwitch;

typedef enum eMapMoveEventType // 왜 맵이동을 하게 되었는가
{// !!맵 이동 사유 로그에 쓰이므로 중간에 값 삽입 하지 마세요!!
	MMET_None		= 0,
	MMET_Normal		= 1,
	MMET_PartyWarp,
	MMET_CoupleWarp,
	MMET_GM,
	MMET_GM_ReCall,
	MMET_ClientEnd,

//	----------------------
//	아래 것들은 서버에서 만든다.
	MMET_Login,
	MMET_Mission,
	MMET_MissionDefence8,
	MMET_SuperGround,

	MMET_Failed,
	MMET_TimeOut,
	MMET_PublicChannelFailed,

	// PvP 관련
	MMET_PvP,
	MMET_BackToPvP,// PvP에 입장해 있는 상태이고 Lobby로 돌려보낸다.
	MMET_KickToPvP,

	MMET_GoToPublicGround,	// Public Channel Ground로 이동한다.
	MMET_GoTopublicGroundParty,
	MMET_BackToChannel,		// Public Channel에서 Channel로 돌아간다.

	MMET_GoToPublic_PartyOrder,
	MMET_BATTLESQUARE,		// Public Channel Ground로 이동

//	실패 관련
	MMET_Failed_Access,	// 접근 권한이 없습니다.
	MMET_Failed_Full,	// 유저가 모두 찼습니다.
	MMET_Failed_Creating,//그라운드 생성중
	MMET_Failed_JoinTime,//접근시간이 아님
	MMET_Failed_Login,	//로그인 실패

	MMET_RidingPet_Transport = 50, //라이딩펫 스킬로 전송타워 이동시 (단지 별도의 로딩화면 표시를 알리기 위해 사용)
} EMapMoveEventType;

typedef struct tagReqMapMove_CM
{
	tagReqMapMove_CM(EMapMoveEventType const eType = MMET_None)
		:	nPortalNo(0)
		,	cType((BYTE)eType)
	{}

	BYTE		cType;
	short		nPortalNo;
	POINT3		pt3TargetPos;
	SGroundKey	kGndKey;

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(cType);
		kGndKey.ReadFromPacket(rkPacket);
		rkPacket.Pop(nPortalNo);
		rkPacket.Pop(pt3TargetPos);
	}

	void WriteToPacket(BM::Stream &rkPacket)const
	{
		rkPacket.Push(cType);
		kGndKey.WriteToPacket(rkPacket);
		rkPacket.Push(nPortalNo);
		rkPacket.Push(pt3TargetPos);
	}
}SReqMapMove_CM;

typedef struct tagReqMapMove_MT
{
	tagReqMapMove_MT(EMapMoveEventType const eType = MMET_None)
	{
		Clear();
		cType = (BYTE)eType;
	}

	tagReqMapMove_MT( tagReqMapMove_MT const &kRMM )
	{
		cType = kRMM.cType;
		kCasterSI = kRMM.kCasterSI;
		kTargetSI = kRMM.kTargetSI;
		
		kCasterKey = kRMM.kCasterKey;
		pt3CasterPos = kRMM.pt3CasterPos;
		
		nTargetPortal = kRMM.nTargetPortal;
		pt3TargetPos = kRMM.pt3TargetPos;
		kTargetKey = kRMM.kTargetKey;
		kMapMoveKey = kRMM.kMapMoveKey;
		iCustomValue = kRMM.iCustomValue;
		bIsBossRoom = kRMM.bIsBossRoom;
		bIndunPartyDie = kRMM.bIndunPartyDie;
		iOwnerLv = kRMM.iOwnerLv;
	}

	tagReqMapMove_MT( SReqMapMove_CM const &kRMMC )
	{
		Clear();
		Set(kRMMC);
	}

	tagReqMapMove_MT( EMapMoveEventType const eType, short const nPortalNo, SGroundKey const &kTarget )
	{
		Clear();
		Set(eType, nPortalNo, kTarget);
	}

	tagReqMapMove_MT( EMapMoveEventType const eType, POINT3 const &pt3Pos, SGroundKey const &kTarget )
	{
		Clear();
		Set(eType, pt3Pos, kTarget);
	}

	void Set( EMapMoveEventType const eType, short const nPortalNo, SGroundKey const &kTarget )
	{
		cType = (BYTE)eType;
		nTargetPortal = nPortalNo;
		kTargetKey = kTarget;
		pt3TargetPos.Clear();
	}

	void Set( EMapMoveEventType const eType, POINT3 const &pt3Pos, SGroundKey const &kTarget )
	{
		cType = (BYTE)eType;
		nTargetPortal = 0;
		pt3TargetPos = pt3Pos;
		kTargetKey = kTarget;
	}

	void Set( SGroundKey const &rkCaster, SGroundKey const &rkTarget, POINT3 const &rkPos )
	{
		kCasterKey = rkCaster;
		kTargetKey = rkTarget;
		pt3TargetPos = rkPos;
	}

	void Set( SReqMapMove_CM const &kRMMC )
	{
		cType = kRMMC.cType;
		kTargetKey = kRMMC.kGndKey;
		nTargetPortal = kRMMC.nPortalNo;
		pt3TargetPos = kRMMC.pt3TargetPos;
	}

	void Clear()
	{
		cType = MMET_None;
		kCasterSI.Clear();//Unit이 물려있는 서버no ->  출발지 서버
		kTargetSI.Clear();//Unit이 물려있는 서버no -> 도착지 서버
		nTargetPortal = 0;//요구하는 포탈 번호
		kTargetKey.Clear();
		kCasterKey.Clear();
		pt3CasterPos.Clear();
		pt3TargetPos.Clear();//내가 갈 자리
		kMapMoveKey.Generate();
		iCustomValue = 0;
		bIsBossRoom = false;
		bIndunPartyDie = false;
		iOwnerLv = 0;
	}

	bool SetBackHome( BYTE _cType=MMET_Failed )
	{
		if ( kTargetKey != kCasterKey )
		{
			cType = _cType;
			kTargetSI = kCasterSI;
			kTargetKey = kCasterKey;
			pt3TargetPos = pt3CasterPos;
			nTargetPortal = 0;
			bIsBossRoom = false;
			return true;
		}
		return false;
	}

	BYTE			cType;	// 왜 이동하게 되었는가
	
	SERVER_IDENTITY kCasterSI;
	SERVER_IDENTITY kTargetSI;

	SGroundKey		kCasterKey;
	POINT3			pt3CasterPos;

	short			nTargetPortal;//요구하는 포탈 번호 (0 이라면 pt3TargetPos 값으로 이동시켜야 한다)
	POINT3			pt3TargetPos;//내가 갈 자리
	SGroundKey		kTargetKey;
	BM::GUID		kMapMoveKey;	// MapMove key
	int				iCustomValue;
	bool			bIsBossRoom;
	bool			bIndunPartyDie;
	int				iOwnerLv;
}SReqMapMove_MT;

typedef enum eMapMoveResult
{
	MMR_NONE				= 0,// 처음 로그인 했을 경우에
	MMR_SUCCESS				= 1,// 성공
	MMR_FAILED				= 2,// 실패
	MMR_FAILED_AGAIN		= 3,// 출발맵에서도 실패
	MMR_PVP_LOBBY			= 4,
	MMR_CASHSHOP			= 5,
}EMapMoveResult;

typedef struct tagAnsMapMove_MT
{
	tagAnsMapMove_MT(EMapMoveEventType const eType = MMET_None)
		:eRet(MMR_NONE)
	{
		cType = (BYTE)eType;
		iPortalID = 0;
	}

	tagAnsMapMove_MT(SERVER_IDENTITY const &kInSI, EMapMoveEventType const eType, BYTE _eRet=MMR_NONE )
		:	kSI(kInSI)	
		,	eRet(_eRet)
		,	kAttr(GATTR_DEFAULT)
	{
		cType = (BYTE)eType;
		iPortalID = 0;
	}

	int iPortalID;
	BYTE cType;
	SERVER_IDENTITY kSI;
	POINT3 pt3Pos;
	SGroundKey kGroundKey;
	BYTE eRet;	//EMapMoveResult
	T_GNDATTR kAttr;	//Ground Attr
}SAnsMapMove_MT;

//-> Quest
typedef std::vector< int > ContQuestID;
#define QUEST_PARAMNUM		5
typedef struct tagUserQuestState
{
	int iQuestID;
	BYTE byQuestState;	// QUEST_STATE_XXXX value
	BYTE byParam[QUEST_PARAMNUM];
	BM::PgPackedTime kTime;	//Remain Time (Time Attack)

	tagUserQuestState(int const iID, BYTE const cQuestState)
		: iQuestID(iID), byQuestState(cQuestState), kTime(BM::PgPackedTime::LocalTime())
	{
		memset(byParam, 0, sizeof(byParam));
	}

	tagUserQuestState()
		: iQuestID(0), byQuestState(0)
	{
		memset(byParam, 0, sizeof(byParam));
	}

	tagUserQuestState(const tagUserQuestState &rhs)
		:iQuestID(rhs.iQuestID), byQuestState(rhs.byQuestState), kTime(rhs.kTime)
	{
		size_t const iParamSize = sizeof(BYTE)*QUEST_PARAMNUM;
		memcpy_s(byParam, iParamSize, rhs.byParam, iParamSize);//
	}

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(iQuestID);
		rkPacket.Push(byQuestState);
		rkPacket.Push(byParam);
		rkPacket.Push(kTime);
	}

	void ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop(iQuestID);
		rkPacket.Pop(byQuestState);
		rkPacket.Pop(byParam);
		rkPacket.Pop(kTime);
	}

	size_t WriteToBuff(BYTE* pDstBuff, size_t const iDstSize) const
	{
		size_t const iSize = sizeof(SUserQuestState);
		memcpy_s(pDstBuff, iDstSize, this, iSize);//
		return iSize;
	}

	size_t ReadFromBuff(BYTE const* pSrcBuff)
	{
		size_t const iSize = sizeof(SUserQuestState);
		memcpy_s(this, iSize, pSrcBuff, iSize);
		return iSize;
	}
} SUserQuestState;

typedef struct tagQuestSimple
{
	int iQuestID;
	BYTE cState;	// QS_XXX
	int iQuestMaxLevel;

	tagQuestSimple(int const QuestID, BYTE const State, int const iMaxLevel)
		:iQuestID(QuestID), cState(State), iQuestMaxLevel(iMaxLevel)
	{
	}

	tagQuestSimple(const tagQuestSimple &rkQuestSimple)
	{
		iQuestID = rkQuestSimple.iQuestID;
		cState = rkQuestSimple.cState;
		iQuestMaxLevel = rkQuestSimple.iQuestMaxLevel;
	}

	tagQuestSimple()
		:iQuestID(0), cState(0), iQuestMaxLevel(0)
	{
	}
} SQuestSimple;
typedef std::vector< SQuestSimple > ContSimpleQuest;

//
typedef enum eQuestShowDialogType
{
	QSDT_None = 0,
	QSDT_BalloonTip = 1,
	QSDT_NormalDialog,
	QSDT_BeginDialog,
	QSDT_CompleteDialog,
	QSDT_SelectDialog,

	//기타
	QSDT_FailedDialog,
	QSDT_ErrorDialog = 255,
} EQuestShowDialogType;

typedef enum eKindCharacterBaseWear
{
	KCBW_NONE			= -1,
	KCBW_HAIRSTYLE		= 1,
	KCBW_HAIRCOLOR		= 0,
	KCBW_FACE			= 2,
	KCBW_JACKET			= 21,
	KCBW_PANTS			= 22,
	KCBW_SHOES			= 23,
	KCBW_GLOVES			= 24,
}EKindCharacterBaseWear;

size_t const MAX_CHARACTER_NAME_LEN =10;//캐릭터 이름 최대 길이
typedef enum eCreateCharacterResult
{
	CCR_NONE			= 0,  //
	CCR_SUCCESS			= 1,  // 성공
	CCR_WRONG_WEAR_ITEM = 2,  // 실패 - 잘못된 아이템을 착용하려 했음
	CCR_OVERLAP_NAME	= 100,// 실패 - 중복되는 캐릭터이름						 (주의 : SP 에서도 사용함)
	CCR_FILTER_NAME		= 101,// 실패 - 욕설이 있음.
	CCR_UNKNOWN_CLASS	= 102,// 실패 - 알수 없는 클래스번호
	CCR_CHECK_DUPLICATE	= 103,// Failure - First, you must Check Duplicate
	CCR_FILTER_CODE		= 104,// 실패 - 사용할 수 없는 문자.
	CCR_MAX_LENGTH		= 105,// 실패 - 사용할 수 없는 문자.(최대글자)
	CCR_DISABLED_FUNCTION = 106,	// 실패 - 캐릭터 생성 기능 Disabled
	CCR_MIN_LENGTH		= 107,		// 실패 - 
	CCR_CANT_CREATE_DRAKAN = 108, // can't create drakan charater level or item condition

	CCR_MAX_LIMINT		= 200,// 실패 - 캐릭터 갯수 제한						 (주의 : SP 에서도 사용함)
	CCR_UNKNOWN_MEMBER	= 250,// 실패 - 알수 없는 멤버GUID						 (주의 : SP 에서도 사용함)
	CCR_DB_ERR_1		= 300,// 실패 - DB오류(TB_UserCharacter insert 실패)	 (주의 : SP 에서도 사용함)
	CCR_NOT_JUMPINGEVENT		= 400,// 실패 - 진행중인 점핑 이벤트가 존재하지 않음
	CCR_MAX_JUMPINGEVENT_CHAR	= 401,// 실패 - 더이상 점핑 이벤트 캐릭터를 만들수 없음
	CCR_SET_NOT_FOUND			= 402, // Sets
}ECreateCharacterResult;

typedef enum eGenderWearLimit
{
	GWL_MALE		= 1,
	GWL_FEMALE		= 2,
	GWL_UNISEX		= 3,
	GWL_PET_MALE	= 4,// 사람이 아닌 펫이다.
	GWL_PET_FEMALE	= 8,// 예약(지금은 암수구분이 없긴함)
	GWL_PET_UNISEX	= GWL_PET_MALE|GWL_PET_FEMALE,
}EGenderWearLimit;

typedef struct tagHOMEADDR
{
	tagHOMEADDR():m_kStreetNo(0),m_kHouseNo(0){}
	tagHOMEADDR(short const __streetno,int const __houseno):m_kStreetNo(__streetno),m_kHouseNo(__houseno){}
	CLASS_DECLARATION_S(short,StreetNo);
	CLASS_DECLARATION_S(int,HouseNo);

	bool operator < (tagHOMEADDR const & rhs) const
	{
		if(StreetNo() < rhs.StreetNo())
		{
			return true;
		}
		else if(StreetNo() == rhs.StreetNo())
		{
			if(HouseNo() < rhs.HouseNo())
			{
				return true;
			}
		}
		return false;
	}

	bool operator == (tagHOMEADDR const & rhs) const
	{
		return StreetNo() == rhs.StreetNo() && HouseNo() == rhs.HouseNo();
	}

	bool operator != (tagHOMEADDR const & rhs) const
	{
		return !((*this) == rhs);
	}

	bool IsNull() const
	{
		return 0 == (StreetNo() + HouseNo());
	}

	void Clear()
	{
		StreetNo(0);
		HouseNo(0);
	}

	DEFAULT_TBL_PACKET_FUNC();
}SHOMEADDR;

typedef struct tagActionInfo
{
	tagActionInfo()
		:bIsDown(false),
		byDirection(0),
		dwActionTerm(0),
		iActionID(0),
		iActionInstanceID(0),
		byActionParam(0),
		dwTimeStamp(0),
		iUnitState(0)

	{
	}

	BM::GUID guidPilot;
	bool bIsDown;
	BYTE byDirection;
	POINT3BY ptDirection;	// 서버에서 Player의 위치를 예상해 내기 위한 방향vector
	POINT3 ptPos;
	POINT2BY ptPathNormal;
	DWORD dwActionTerm;
	int iActionID;
	int iActionInstanceID;
	BYTE byActionParam;
	DWORD dwTimeStamp;
	int iUnitState;

	void	SendToServer(BM::Stream &kPacket)
	{
		kPacket.Push(*this);
	}
	void	ReceiveFromServer(BM::Stream &kPacket)
	{
		kPacket.Pop(guidPilot);
		kPacket.Pop( bIsDown);
		kPacket.Pop( byDirection);
		kPacket.Pop( ptPos);
		kPacket.Pop( dwActionTerm);
		kPacket.Pop( iActionID);
		kPacket.Pop( iActionInstanceID);
		kPacket.Pop( byActionParam);
		kPacket.Pop( dwTimeStamp);
		kPacket.Pop( iUnitState);
	}
	void	SendToClient(BM::Stream &kPacket)
	{
		kPacket.Push(guidPilot);
		kPacket.Push( bIsDown);
		kPacket.Push( byDirection);
		kPacket.Push( ptPos);
		kPacket.Push( dwActionTerm);
		kPacket.Push( iActionID);
		kPacket.Push( iActionInstanceID);
		kPacket.Push( byActionParam);
		kPacket.Push( dwTimeStamp);
		kPacket.Push( iUnitState);
	}
	void	ReceiveFromClient(BM::Stream &kPacket)
	{
		kPacket.Pop(*this);
	}
}SActionInfo;

// Action Result Code
typedef enum : short int
{
	EActionR_Success = 0,		// Success : No action needed
	EActionR_Success_Fire = 1,	// Success : FireAction needed
	EActionR_Success_Toggle = 2,	// Success : Toggle skill Fire needed
	EActionR_Success_Max = 10,		// Success : MAX Success code
	EActionR_Err_NoSkillDef = 101,	// Error : Cannot find SkillDef
	EActionR_Err_NotLearned = 102,	// Error : Not Learned skill
	EActionR_Err_WeaponLimit = 103,	// Error : Weapon limit error
	EActionR_Err_StateLimit = 104,	// Error : Player State limit error
	EActionR_Err_CastingErr = 105,	// Error : Casting failure
	EActionR_Err_ChargingTime = 106, // Error : Charging Time error
	EActionR_Err_MCastAttr = 107,	// Error : SAT_MAINTENANCE_CAST 속성 error
	EActionR_Err_ModifyTarget = 108,	// Error : Fire 이후에 ESS_TARGETLISTMODIFY 와야 함.
	EActionR_Err_ToggleOnOff = 109,	// Error : Toggle skill On/Off state error
	EActionR_Err_NotToggle = 110,	// Error : Toggle Skill 이 아닌데 toggle상태 요청
	EActionR_Err_CastType = 111,	// Error : CastType 이 일치 하지 않음
	EActionR_Err_CoolTime = 112,	// Error : CoolTime error
	EActionR_Err_SkillStatus = 113,	// Error : SkillStatus (EActionResultCode) is unknown
	EActionR_Err_ClassLimit = 114,	// Error : Class Limit
	EActionR_Err_LimitMaxTarget = 115, // Error : 유저가 잡은 타겟팅 수가 Skill MaxTarget 제한을 넘어 가는 경우
	EActionR_Err_LimitMaxRange = 116, // Error : 스킬의 최대 사정 거리를 넘어가는 경우
	EActionR_Err_NeedHP = 201,	// Error : need hp error
	EActionR_Err_NeedMP = 202,	// Error : need mp error
	EActionR_Err_NeedMoney = 203, // Error : need Money error
	EActionR_Err_NeedCash = 204, // Error : need Cash error
	EActionR_Err_Unknown = 204, // Error : Hacking으로 의심 되는경우 
	EActionR_Err_NeedAwake = 202,	// Error : need 각성치 error
	EActionR_Err_CantUseThisGround = 301,	// Error 현재 맵에서 사용할수 없는 액션임
	EActionR_Err_NeedSummonSupply = 302,	// Error : need 소환 용량
	EActionR_Err_Already_UniqueSummon = 303,	// Error : 하나만 소환가능한건데 이미 소환되어 있음

} EActionResultCode;

typedef struct tagDefaultMonsterInfo
{
	static const BYTE REGEN_DIRECTION_NOSET = 0xFF;
	tagDefaultMonsterInfo()
		: iTunningNo(0)
		, iTunningGrade(0)
		, iTunningUserLv(0)
	{
		iMonNo = 0;
		
		iState = 0;
		bDir = 0;
		sLevel = 0;
		iMaxHP = 0;
		iHP = 0;
		iMaxMP = 0;
		iMP = 0;
//		iMaxDP = 0;
//		iDP = 0;
		cDirection = REGEN_DIRECTION_NOSET;
	}

	int iMonNo;
	BM::GUID kGuid;
	POINT3 ptPos;
	int iState;
	bool bDir;
	short sLevel;
	int iMaxHP;
	int iHP;
	int iMaxMP;
	int iMP;
//	int iMaxDP;
//	int iDP;
	BYTE cDirection;
	int iTunningNo;
	int iTunningGrade;
	int iTunningUserLv;
	int iEnchantGradeNo;
}DEFAULT_MONSTER_INFO;

struct DefaultSummonedInfo_
{
	DefaultSummonedInfo_() : iClassNo(0), iHP(0), iMaxHP(0), sMoveSpeed(0)
	{
	}
	BM::GUID kGuid;
	int iClassNo;
	POINT3 ptPos;
	int iHP;
	int iMaxHP;
	unsigned short sMoveSpeed;

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(kGuid);
		kPacket.Push(iClassNo);
		kPacket.Push(ptPos);
		kPacket.Push(iHP);
		kPacket.Push(iMaxHP);
		kPacket.Push(sMoveSpeed);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(kGuid);
		kPacket.Pop(iClassNo);
		kPacket.Pop(ptPos);
		kPacket.Pop(iHP);
		kPacket.Pop(iMaxHP);
		kPacket.Pop(sMoveSpeed);
	}
};

typedef struct tagGround
{
	tagGround()
	{
		Clear();
	}
	tagGround(const tagGround& rhs)
	{
		kSI = rhs.kSI;
		kAttr = rhs.kAttr;
		kKey = rhs.kKey;
	}

	tagGround(SERVER_IDENTITY const &kInSI, SGroundKey const &kInKey, T_GNDATTR const Attr=GATTR_DEFAULT )
	{
		kSI = kInSI;
		kKey = kInKey;
		kAttr = Attr;
	}

	void Clear()
	{
		kAttr = GATTR_DEFAULT;
		kKey.Clear();
		kSI.Clear();
	}

	SERVER_IDENTITY kSI;

	SGroundKey	kKey;
	T_GNDATTR	kAttr;
}SGround;

typedef std::map< SGroundKey, SGround > CONT_GROUND;// first : ground No. -> .... LogicalChannel.. 
typedef std::vector<SGround>	GroundArr;
typedef std::vector<int>		GroundNoArr;

typedef struct tagContentsUser// 콘텐츠서버에서 관리하는 '로그인한 전체유저'의 데이터 - 센터서버와 동기화 됨
{	
	tagContentsUser()
	{
		Clear();
	}

	void Clear()
	{
		kName.clear();
		kMemGuid.Clear();
		iUID = 0;
		kCharGuid.Clear();
		kGndKey.Clear();
		sLevel = 0;
		cGender = 0;
		iClass = 0;
		kGuildGuid.Clear();
		kCoupleGuid.Clear();
		sChannel = 0;
		kAccountID.clear();
		kGuildLeaveDate.Clear();
		kHomeAddr.Clear();
		iBaseClass = 0;
	}

	std::wstring const& Name() const
	{
		return kName;
	}
	
	std::wstring kName;
	BM::GUID kMemGuid;
	int iUID;		// NCSoft Member UID
	BM::GUID kCharGuid;
	unsigned short sLevel;
	unsigned char cGender;
	int iClass;
	int iBaseClass;
	BM::GUID kGuildGuid;
	BM::GUID kCoupleGuid;

	SGroundKey	kGndKey;	//현재 위치한 그라운드;
	short sChannel;
	std::wstring kAccountID;

	BM::DBTIMESTAMP_EX kGuildLeaveDate;	// 길드 탈퇴한 시간

	SHOMEADDR	kHomeAddr;

	size_t min_size()const
	{
		return 
			sizeof(size_t)+// kName );
			sizeof( kCharGuid )+
			sizeof( kMemGuid )+
			sizeof( iUID )+
			sizeof( kGndKey )+
			sizeof( sLevel )+
			sizeof( iClass )+
			sizeof( cGender )+
			sizeof( kGuildGuid )+
			sizeof( kCoupleGuid )+
			sizeof( sChannel )+
			sizeof( size_t)+//kAccountID );
			sizeof( kHomeAddr )+
			sizeof( iBaseClass );
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop( kName );
		rkPacket.Pop( kCharGuid );
		rkPacket.Pop( kMemGuid );
		rkPacket.Pop( iUID );
		rkPacket.Pop( kGndKey );
		rkPacket.Pop( sLevel );
		rkPacket.Pop( iClass );
		rkPacket.Pop( cGender );
		rkPacket.Pop( kGuildGuid );
		rkPacket.Pop( kCoupleGuid );
		rkPacket.Pop( sChannel );
		rkPacket.Pop( kAccountID );
		rkPacket.Pop( kGuildLeaveDate );
		rkPacket.Pop( kHomeAddr );
		rkPacket.Pop( iBaseClass);
	}

	void WriteToPacket(BM::Stream& rkPacket) const
	{
		rkPacket.Push( kName );
		rkPacket.Push( kCharGuid );
		rkPacket.Push( kMemGuid );
		rkPacket.Push( iUID );
		rkPacket.Push( kGndKey );
		rkPacket.Push( sLevel );
		rkPacket.Push( iClass );
		rkPacket.Push( cGender );
		rkPacket.Push( kGuildGuid );
		rkPacket.Push( kCoupleGuid );
		rkPacket.Push( sChannel );
		rkPacket.Push( kAccountID );
		rkPacket.Push( kGuildLeaveDate );
		rkPacket.Push( kHomeAddr );
		rkPacket.Push( iBaseClass );
	}

	bool Empty()const
	{
		return kName.empty() || BM::GUID::IsNull(kCharGuid) || BM::GUID::IsNull(kMemGuid);
	}
}SContentsUser;
typedef std::vector< SContentsUser > ContContentsUser;

typedef struct tagUserInfoExt
{
	tagUserInfoExt() : iPremiumNo(0)
	{
		kAddress.Clear();
	}

	tagUserInfoExt const& operator=(tagUserInfoExt const& rhs)
	{
		kAddress = rhs.kAddress;
		iPremiumNo = rhs.iPremiumNo;
		return (*this);
	}

	CEL::ADDR_INFO kAddress;
	int iPremiumNo;
} SUserInfoExt;

typedef enum eKeyInfoType
{
	KIT_CharGuid = 1,
	KIT_MembGuid = 2,
	KIT_CharName = 3,
} EKeyInfoType;

typedef enum eGroundActionNo//PT_U_G_RUN_ACTION
{
	GAN_NONE = 0,
	GAN_Refresh_All_Quest = 1,
	GAN_Resurrect_Spawn_Num,//Spawn Num
	GAN_SetAbiles,
	GAN_SumitLog,
	GAN_AddEffect, // Ground 포인터를 알수 없는 곳에서 Unit에 대고 VNotify로 AddEffect 한다.
	GAN_DelEffect, // Ground 포인터를 알수 없는 곳에서 Unit에 대고 VNotify로 DeleteEffect 한다.
	GAN_SendQuestDialog,
	GAN_MapMove,
	GAN_PushVolatileInven_Item,
	GAN_SuperGroundFeather,
	GAN_SuperGroundMoveFloor,
	GAN_PushMissionDefenceInven_Item,
	GAN_BroadCastSmallArea,
	GAN_SaveCombo,
	GAN_SumitOrder,
} EGroundActionNo;

typedef enum ePartyMemberChangedAbility
{
	PMCA_MAX_PERCENT	= 10000,// 1/n == 1/10000 == 100.00
	PMCA_SameArea		= 0, //서로의 Unit이 보인다. 클라이언트에서 직접 Pilot으로 부터 AT_HP/AT_C_MAX_HP를 쓴다 (클라)
	PMCA_SameTest		= 10,
	PMCA_SameGround		= 100, //서로의 Unit이 없다. 1/100 단위로 변경시 업데이트 한다 (맵)
	PMCA_SameServerNo	= 400, //서로 다른 GroundNo 지만 같은 맵서버 있다. 1/25 단위로 한다 (맵)
	PMCA_SameChannel	= 2000, //같은 물리채널이다. 1/5 단위로 업데이트 한다 (컨텐츠)
	PMCA_SameParty		= 100, //파티원과의 거리가 100일때 업데이트 한다.(맵)
}EPartyMemberChangedProperty;
typedef enum ePartyMemberChangedAbilityType
{
	PMCAT_None	= 0,
	PMCAT_Class	= 0x01,
	PMCAT_Level	= 0x02,
	PMCAT_HP	= 0x04,
	PMCAT_MP	= 0x08,
	PMCAT_POS	= 0x16,
	PMCAT_MAX	= 0xFF,
}EPartyMemberChangedAbilityType;


typedef struct tagAbilInfo
{
	tagAbilInfo()
	{
		Set(0,0);
	}

	tagAbilInfo(WORD const InType, int const iInValue)
	{
		Set(InType,iInValue);
	}

	void Set(WORD const InType, int const iInValue)
	{
		wType = InType;
		iValue = iInValue;
	}

	WORD wType;
	int iValue;
}SAbilInfo;
typedef std::vector<SAbilInfo>		VEC_ABILINFO;

typedef struct tagAbilInfo64
{
	tagAbilInfo64()
	{
		Set(0,0);
	}

	tagAbilInfo64(WORD const InType, __int64 const iInValue)
	{
		Set(InType,iInValue);
	}

	void Set(WORD const InType, __int64 const iInValue)
	{
		wType = InType;
		iValue = iInValue;
	}

	WORD wType;
	__int64 iValue;
}SAbilInfo64;

int const RWD_ITEM_CNT = 4;
typedef std::vector<int> RESULT_ITEM_BAG_LIST_CONT;

typedef struct tagItemInfo
{
	tagItemInfo( int const _iItemNo=0, int const _iCount=1)
		:	iItemNo(_iItemNo)
		,	iCount(_iCount)
	{}

	int iItemNo;
	int iCount;//내구도 아이템인 경우는 내구도

	DEFAULT_TBL_PACKET_FUNC();
}SSimpleItemInfo;
typedef std::vector< SSimpleItemInfo > REWARD_ITEM_LIST;

typedef struct tagResultBonus
{
	tagResultBonus( BM::GUID const &rkGuid=BM::GUID::NullData(), __int64 const iExp=0 )
		:	kCharGuid(rkGuid)
		,	i64BonusExp(iExp)
	{
	}

	void WriteToPacket( BM::Stream& rkPacket ) const
	{
		rkPacket.Push( kCharGuid );
		rkPacket.Push( i64BonusExp );
		rkPacket.Push( kItemBag );
	}
	void ReadFromPacket( BM::Stream& rkPacket )
	{
		kItemBag.clear();
		rkPacket.Pop( kCharGuid );
		rkPacket.Pop( i64BonusExp );
		rkPacket.Pop( kItemBag );
	}
	void push_item( int const iItemNo )
	{
		if ( 0 != iItemNo )
		{
			kItemBag.push_back( iItemNo );
		}
	}
	size_t size_item()	{	return kItemBag.size();	}
	bool empty_item()	{	return kItemBag.empty();}
	BM::GUID					kCharGuid;
	__int64						i64BonusExp;
	RESULT_ITEM_BAG_LIST_CONT	kItemBag;
}SResultBonus;
typedef std::vector<SResultBonus>		ConResultBonus;

typedef struct tagConResultBonus
{
	tagConResultBonus()
	{
	}

	void WriteToPacket( BM::Stream& rkPacket ) const
	{
		rkPacket.Push( kConBonus.size() );
		ConResultBonus::const_iterator itr = kConBonus.begin();
		while(kConBonus.end() != itr)
		{
			itr->WriteToPacket( rkPacket );
			++itr;
		}
	}
	void ReadFromPacket( BM::Stream& rkPacket )
	{
		kConBonus.clear();
		size_t iSize;
		rkPacket.Pop( iSize );
		SResultBonus kTemp;
		while ( iSize-- )
		{		
			kTemp.ReadFromPacket( rkPacket );
			kConBonus.push_back( kTemp );
		}
	}
	void Push( SResultBonus const& rkBonus )
	{
		kConBonus.push_back( rkBonus );
	}
	SResultBonus& at(size_t const index)
	{
		return kConBonus.at(index);
	}
	void Clear()
	{
		kConBonus.clear();
	}
	size_t size()
	{
		return kConBonus.size();
	}
	ConResultBonus kConBonus;
}SConResultBonus;

typedef enum ePartyCommandType
{
	PCT_NONE = 0,
	PCT_REQJOIN_CHARGUID = 1,	
	PCT_REQJOIN_CHARNAME,
	PCT_KICK_CHARGUID,
	PCT_KICK_CHARNAME,
	PCT_REQJOIN_MEMBERGUID,
} EPartyCommandType;

typedef enum eMCtrlCommand
{
	MCC_None				= 0,
	MCC_Shutdown_Service	= 1,	//서비스를 종료 해라 (MCtrl -> Game Server)
	MCC_Start_Service,				//서비스를 시작 했다 (Game Server -> MCtrl)
	//MCC_Start_MCtrl_Service,		//MCtrl서비스(Heart beat)를 시작 해라 (MCtrl -> Game Server)
	MCC_Heart_Beat,					//나 살아 있다. (Game Server -> MCtrl)
	MCC_CHANGE_MAXUSER,
	MCC_GetProcessID,
} EMCtrlCommand;


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// Chat / Messenger

typedef enum eWhisperChatReturn//귓말 리턴
{
	WCR_None	= 0,
	WCR_Success	= 1,//성공
	WCR_NotFoundName,//캐릭터 이름이 없다(캐릭터를 찾을수없다)
	WCR_NotFoundGuid,//캐릭터 Guid가 없다(로그인을 안했다)
} EChatReturnValue;
typedef enum eFriendErrorCode//메신저 리턴 코드
{
	FEC_None	= 0,
	FEC_Success	= 1,//성공
	FEC_Already_Added,//이미 추가
	FEC_NotFound,
	FEC_Failed,
	FEC_Wait,
	FEC_Failed_Limit,
	FEC_Failed_Duplicate,
	FEC_Failed_Me,
	FEC_Accept,
} EFriendErrorCode;

typedef enum eFriendCommandType
{
	FCT_NONE		= 0,
	FCT_ADD_BYGUID	= 1,
	FCT_ADD_BYNAME,
	FCT_FULL_DATA,
	FCT_MODIFY_CHATSTATUS,//통틀어
	FCT_ADD_ACCEPT,//추가허용(특수)
	FCT_ADD_REJECT,//추가거부(특수)
	FCT_ADD_GROUP,//그룹생성
	//FCT_Modify_Group,//그룹수정
	//FCT_Modify_MyStatus,//내정보 수정
	//FCT_Modify_Block,//블럭정보 수정
	FCT_DELETE,
	FCT_LINKSTATUS,
	FCT_GROUP,//그룹수정
	FCT_MAPMOVE,//맵이동
	//FCT_Notify,//상태정보 갱신(온 오프라인/자리비움 등등)
	//FCT_Place,//위치정보 갱신(친구의 맵 위치는? 채널은?)
	FCT_MODIFY,
	FCT_NOT_DELETE,
} EFriendCommandType;

//클라이언트 Log 에서는 1<<ChatType 만큼의 비트플래그로
//서버와 통신간에는 실수를 BYTE로 변환해서 사용한다.
typedef enum eChatType
	: BYTE//채팅타입
{
	CT_ERROR	= 0,//에러 이벤트 타입
	CT_NONE	= 1,
	CT_NORMAL = 2,//채팅모드 가장 기본값 (>>요기서 부터 사용자가 쓸수 있는 모드)
	CT_PARTY = 3,
	CT_WHISPER_BYNAME = 4,
	CT_FRIEND = 5,
	CT_GUILD = 6,
	CT_MANTOMAN = 7,
	CT_TEAM = 8,//PVP에서 TEAM CHATING//(<<요기까지 사용자가 사용 가능한 채팅 모드)
	CT_USERSELECT_MAX = 9,//사용자가 선택할 수 있는 채팅모드
	CT_WHISPER_BYGUID = 10,
	CT_EVENT = 11,//친구, 파티초대, 길드 등에 이벤트 메시지
	CT_EVENT_SYSTEM = 12,//서버/클라이언트 시스템 이벤트 메시지(서버가 종료되었습니다, 연결이 종료 되었습니다.)
	CT_EVENT_GAME = 13,//게임 메시지(공격, 스킬, ..)
	CT_NOTICE = 14,//공지
	CT_BATTLE = 15,//전투
	CT_ITEM = 16,//아이템
	CT_COMMAND = 17,//명령어
	CT_MEGAPHONE_CH = 18,//확성기 - 채널
	CT_MEGAPHONE_SV = 19,//확성기 - 서버
	CT_RAREMONSTERGEN = 20,//희귀 몬스터 생성
	CT_RAREMONSTERDEAD = 21,//희귀 몬스터 죽음

	CT_OXQUIZEVENT = 22,	// OX 퀴즈 이벤트 메시지
	
	CT_ERROR_NOTICE  = 23,
	CT_NORMAL_NOTICE = 24,
	CT_NOTICE1		 = 25,
	CT_NOTICE2		 = 26,
	CT_NOTICE3		 = 27,	

	CT_LOVE_SMS = 28,

	CT_CASHITEM_GIFT = 29, //캐쉬아이템 선물보내기 이벤트 메시지
	CT_GOLD = 30,//골드,캐시
	CT_EXP = 31,//경험치
	CT_EMOTION = 32,//감정표현

	CT_MYHOME_PUBLIC = 33,	// 마이홈 일반 체팅
	CT_MYHOME_PRIVATE = 34,	// 마이홈 귓속말

	CT_TRADE = 39,		// 거래. 거래탭 필터링 채팅 모드.

	CT_DUEL = 40,	//결투신청. 이 모드는 채팅모드가 아닌, "/duel 홍길동" 식의 명령어로만 사용된다.
	
	CT_GUILD_LOINGUSER = 41,	//로그인한 유저에게 전송되는 길드 채팅메시지

	CT_MAX,// eChatType은 < CT_MAX 야 옳다
} EChatType;

typedef enum eNoticeType
{
	NOTICE_ERROR			= 0,
	NOTICE_EM_CHALLENGE_RESULT = 71017,//엠포리아 도전 결과
	NOTICE_EMPORIABATTLE_READY = 71018,
	NOTICE_EMPORIABATTLE_START = 71019,//엠포리아 전쟁 시작.
	NOTICE_EMPORIABATTLE_RESULT = 71022,//엠포리아 전쟁 결과
	NOTICE_EM_AD_MERCENARY		= 71024,//엠포리아 용병 모집

	NOTICE_PLUSE_UPGRADE_ITEM	= 80001,// 아이템 업그레이드 관련 정보
	NOTICE_REGEN_EVENT_MONSTER  = 80101,// 이벤트 몬스터 리젠.
}ENoticeType;

typedef enum eFriendChatStatus	//친구 대화 설정
{	//1바이트 상수					(채팅 보다 연결 상태가 우선된다)
	// 0x0? <- 상태 비트			(채팅 관련)
	FCS_BLOCK_NONE		= 0x00,//차단 없음
	FCS_BLOCK_NO_RECV	= 0x01,//듣지 않겠다	(받지 않는다)		클라처리
	FCS_BLOCK_NO_SEND	= 0x02,//말하지 않겠다	(보내지 않는다)
	FCS_BLOCK_FILTER	= 0x0F,
	// 0x?0 <- 친구추가 관련 비트	(연결 상태 관련)
	FCS_ADD_DELETED		= 0x00,//온/오프, 말하기, 듣기 모두 안한다		(모두 차단, 항상 오프라인)
	FCS_ADD_WAIT_REQ	= 0x10,//등록 요구 후 대기(신청자)				(모두 차단, 항상 오프라인)
	FCS_ADD_WAIT_ANS	= 0x20,//등록 허가 대기(피신청자)				(모두 차단, 항상 오프라인)
	FCS_ADD_ALLOW		= 0x40,//피신청자가 등록을 허가 하였다(양측)	(모두 허용)
	//__________________________________= 0x80,//사용안한다(SIGN, UNSIGNED) 에러 발생 우려(DB에서)
	FCS_ADD_FILTER		= 0xF0,
}EChatFriendStatus;

typedef enum eFriendLinkStatus		//친구 연결 상태
{
	FLS_ONLINE			= 1,//친구 온라인 // 아무상태아님.
	FLS_CHATTER			= 2,//수다중
	FLS_LEVELUP			= 3,//열렙중
	FLS_AFK				= 4,//친구 AWAY FROM KEYBOARD (자리비움)
	FLS_DINNER			= 5,//식사중
	FLS_SLEEPING		= 6,//수면중
	FLS_OFFLINE			= 7,//친구오프라인

}EFriendLinkStatus;

typedef struct tagDBFriendItem//메신져 친구 아이템(Contents -> Client)
{
	tagDBFriendItem()
	{
		Clear();
	}

	tagDBFriendItem( tagDBFriendItem const &rhs )
	{
		*this = rhs;
	}

	tagDBFriendItem &operator=( tagDBFriendItem const &rhs )
	{
		m_kCharGuid = rhs.m_kCharGuid;
		m_kChatStatus = rhs.m_kChatStatus;
		m_kGroupName = rhs.m_kGroupName;
		m_kExtendName = rhs.m_kExtendName;
		m_kHomeAddr = rhs.m_kHomeAddr;
		return *this;
	}

	void Clear()
	{
		m_kCharGuid.Clear();
		m_kChatStatus = 0;
		m_kGroupName.clear();
		m_kExtendName.clear();
	}

	CLASS_DECLARATION_S(BM::GUID,	CharGuid);//MemberGuid
	CLASS_DECLARATION_S(BYTE,		ChatStatus);
	CLASS_DECLARATION_S(std::wstring, GroupName);
	CLASS_DECLARATION_S(std::wstring, ExtendName);
	CLASS_DECLARATION_S(SHOMEADDR, HomeAddr);


	size_t min_size()const
	{
		return 
			sizeof(size_t)+//m_kGroupName)+
			sizeof(m_kCharGuid)+
			sizeof(m_kChatStatus)+
			sizeof(size_t)+//m_kExtendName);
			sizeof(SHOMEADDR);
	}

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(GroupName());
		rkPacket.Push(CharGuid());
		rkPacket.Push(ChatStatus());
		rkPacket.Push(ExtendName());
		rkPacket.Push(HomeAddr());
	}
	void ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop(m_kGroupName);
		rkPacket.Pop(m_kCharGuid);
		rkPacket.Pop(m_kChatStatus);
		rkPacket.Pop(m_kExtendName);
		rkPacket.Pop(m_kHomeAddr);

		if(m_kGroupName.size() >19)
		{
			m_kGroupName.resize(19);
		}

		if(m_kExtendName.size() >19)
		{
			m_kExtendName.resize(19);
		}
	}
}SDBFriendItem;

typedef struct tagFriendItem 
	: public SDBFriendItem
{
	CLASS_DECLARATION_S(BYTE, LinkStatus);// EFriendLinkStatus 사용
	CLASS_DECLARATION_S(SGroundKey, GroundKey);
	CLASS_DECLARATION_S(int, Class);
	CLASS_DECLARATION_S(int, Level);
	CLASS_DECLARATION_S(short, Channel);

	tagFriendItem()
	{
		Clear();
	}

	tagFriendItem( tagFriendItem const &rhs )
	{
		*this = rhs;
	}

	tagFriendItem& operator=( tagFriendItem const &rhs )
	{
		*((SDBFriendItem*)this) = *((SDBFriendItem*)&rhs);
		m_kLinkStatus = rhs.LinkStatus();
		m_kGroundKey = rhs.GroundKey();
		m_kClass = rhs.Class();
		m_kLevel = rhs.Level();
		m_kChannel = rhs.Channel();
		return *this;
	}

	void Clear()
	{
		SDBFriendItem::Clear();
		LinkStatus(FLS_OFFLINE);
		m_kGroundKey.Clear();
		m_kClass = 0;
		m_kLevel = 0;
		m_kChannel = 0;
	}

	void Set(SContentsUser const &rkUser, BYTE cChatStatus)
	{
		Clear();
		//kGroupName;
		CharGuid(rkUser.kCharGuid);
		ChatStatus(cChatStatus);
		ExtendName(rkUser.Name());
		GroundKey(rkUser.kGndKey);
		Class(rkUser.iClass);
		Level(rkUser.sLevel);
		Channel(rkUser.sChannel);
	}
	
	size_t min_size()const
	{
		return 
			SDBFriendItem::min_size()+
			sizeof(m_kLinkStatus)+
			m_kGroundKey.min_size()+
			sizeof(m_kClass)+
			sizeof(m_kLevel)+
			sizeof(m_kChannel);
	}
	
	void WriteToPacket(BM::Stream &rkPacket) const
	{
		SDBFriendItem::WriteToPacket(rkPacket);
		rkPacket.Push(LinkStatus());
		m_kGroundKey.WriteToPacket( rkPacket );
		rkPacket.Push(Class());
		rkPacket.Push(Level());
		rkPacket.Push(Channel());
	}
	void WriteToDBPacket(BM::Stream &rkPacket) const
	{
		SDBFriendItem::WriteToPacket(rkPacket);
	}
	//
	void ReadFromPacket(BM::Stream &rkPacket)
	{
		SDBFriendItem::ReadFromPacket(rkPacket);
		rkPacket.Pop(m_kLinkStatus);
		m_kGroundKey.ReadFromPacket( rkPacket );
		rkPacket.Pop(m_kClass);
		rkPacket.Pop(m_kLevel);
		rkPacket.Pop(m_kChannel);
	}
	void ReadFromDBPacket(BM::Stream &rkPacket)
	{
		SDBFriendItem::ReadFromPacket(rkPacket);
	}
	//
	bool operator == (const tagFriendItem &rhs) const
	{
		if(	GroupName() == rhs.GroupName() 
		&&	CharGuid() == rhs.CharGuid() )
		{
			return true;
		}
		return false;
	}
}SFriendItem;

//////////////////////////////////////////////////////
//		Shine Stone
//>>

typedef struct tagStoneRegenPoint//XML 기준의 데이터
{
	int iID;
	POINT3 kPos;

	tagStoneRegenPoint()
	{
		Clear();
	}

	tagStoneRegenPoint(int iId, POINT3 const &rkPos)
	{
		Clear();
		iID = iId;
		kPos = rkPos;
	}
	void Clear()
	{
		iID = 0;
		kPos.Clear();
	}
}SStoneRegenPoint;
//<<Shine Stone

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//		Guild
int const MAX_GUILDNAME_LEN		= 10;
int const MAX_GUILDGRADENAME	= 5;
int const iNoticeMaxCharacter = 100;
//
typedef enum eGuildMemberGrade
{
	GMG_Owner = 0,		//Super AdmiAnistrator
	GMG_Master = 1,		//Administrator
	GMG_GroupMaster = 2,//Group Mananger
	GMG_Membmer = 0xff,	//User
}EGuildMemberGrade;

typedef enum eGuildEnumeration
{
	GE_Name_Max = 10,
//	GE_TaxRate_Min = 0,
//	GE_TaxRate_Max = 50,
} EGuildEnumeration;

typedef enum eApplicationState : BYTE
{
	AS_NONE = 0,
	AS_ACCEPT = 1, // 길드가입 허용
	AS_REJECT = 2, // 길드가입 허용 안함	
	AS_COMPLETE = 3,
}EApplicationState;

typedef enum eGuildCommand : BYTE
{
	GC_None = 0,//숫자 변경하지 마세요
	GC_Create = 1,
	GC_List = 2,
	GC_Info = 3,
	GC_ReqJoin = 4,
	GC_ReqJoin_Name = 5,
	GC_Join = 6,
	GC_Leave = 7,
	GC_M_AuthOwner = 8,		//소유자 인증
	GC_M_Emblem = 9,		//엠블렘 등록
	//GC_M_TaxRate = 10,		//경험치 세율 조정
	GC_M_Grade = 11,		//계급명 수정
	GC_M_MemberGrade = 12,	//계급 수정
	GC_M_Kick = 13,			//추방
	GC_M_Destroy = 14,		//해체
	GC_M_Notice = 15,		//공지사항
	GC_DB_Basic = 16,		//Center -> Contents용
	//GC_DB_Extern = 17,	// 사용하지 않음
	//GC_DB_Member = 18,
	//GC_DB_Grade = 19,
	GC_S_SaveBasic = 20,	//기본정보 (레벨, 경험치) 저장
	GC_PreCreate = 21,		//만들 수 있는 레벨이나, 돈이 되는지 확인하는 절차
	GC_OtherInfo = 22,		//다른 길드의 간략화된 정보를 요청(Guid,Name,엠블렘)
	GC_Login = 23,
	GC_Logout = 24,
	GC_M_Rename = 25,			//길드 이름 변경 (캐쉬)
	GC_GM_Rename = 26,			//길드 이름 변경 (GM Command)
	GC_Moved = 27,
	GC_ReserveBattleForEmporia = 28,	// 엠포리아 쟁탈전 예약
	GC_EmporiaInfo = 29,
	GC_War_GuildMemberList = 30,// 전쟁에 사용할 길드원 리스트를 달라
	GC_M_LvUp = 31,				//길드 레벨업
	GC_M_AddSkill = 32,			//스킬 배우기
	GC_M_ChangeOwner = 33,		//길드 마스터 변경
	GC_PreCreate_Test = 34,		//길드 생성 이전에 조건체크
	GC_AddExp = 35,				// 용병 퀘스트 보상에 의한 경험치 증가
	GC_UpKeepEmporiaExp = 36,		// 엠포리아 유지비로 경험치 사용
	GC_M_ChangeMark1 = 37,		//길드 마크 변경 (캐쉬탬)
	GC_M_AuthMaster = 38,		//마스터 인증
	GC_GM_ChangeOwner = 39,		// 길드 마스터 변경, GM
	GC_GM_ChangeOwnerKick = 40,	// 길드 마스터 변경 하고 킥, System
	GC_SetMercenary = 41,// 용병정보 셋팅

	GC_SetGuildEntranceOpen = 42,// 길드가입 설정
	GC_GuildEntranceApplicant_List = 43, // 길드가입 신청자에 대한 리스트 얻기
	GC_GuildEntranceApplicant_Accept = 44, // 길드가입 수락
	GC_GuildEntranceApplicant_Reject = 45, // 길드가입 거절
	GC_GuildEntranceApplicant_State = 46, // 길드가입 신청 상태 변화

	GC_M_InventoryCreate = 47,		// 길드창고 생성
	GC_M_InventorySupervision = 48, // 길드창고 권한 설정
	GC_InventoryOpen = 49,	// 길드금고를 열었으니 정보를 주자.
	GC_InventoryClose = 50,	// 길드금고를 닫았다.
	GC_InventoryUpdate = 51, // 길드금고의 아이템 변경
	GC_InventoryLog = 52, // 길드금고의 로그를 달라.

    GC_EmporiaThrow = 53, //엠포리아 도전 포기
	GC_InventoryMoney = 54, // 길드금고의 길드골드정보

	GC_UpdateGuildList = 55, //길드원 정보 갱신

	GC_M_SetLv		= 56,
	GC_GM_SetLv		= 57, //길드 레벨 설정
	GC_M_SetExp		= 58,
	GC_GM_SetExp	= 59, //길드 경험치 설정
	GC_GM_SetMoney	= 60, //길드 금고 돈 설정
}EGuildCommand;

typedef enum eGuildCommandRet
{//숫자 변경하지 마세요
	GCR_None = 0,//Ans 패킷에선 명령을 실행
	GCR_Success = 1,
	GCR_Failed = 2,
	GCR_Notify = 3,
	GCR_NotEmpty = 4,		//삭제시 길드원이 한명이라도 있다		--여기서 부턴 특수 Return Code
	GCR_Reject = 5,			//상대편이 길드 초대를 거부 하였다
	GCR_Wait = 6,			//길드 목록이 없다 기다려라
	GCR_Date_Limit = 7,		//수정한지 얼마 안지났다
	GCR_NotAuth = 8,		//길마가 아니다
	GCR_Error = 9,
	GCR_DoNotMe = 10,
	GCR_Same = 11,			//이전값과 동일하다.
	GCR_Max = 12,			//최대치에 도달했다
	GCR_Duplicate = 13,
	GCR_Level = 14,			//레벨이 안된다
	GCR_Money = 15,			//돈이 부족하다
	GCR_Member = 16,		//이미 길드에 가입해 있다.
	GCR_BadName = 17,		//욕설 필터에 걸렸다
	GCR_Moved = 18,			//이동
	GCR_NotEnoughExp = 21,	//경험치 부족
	GCR_Pre = 22,			//이전 정보
	GCR_Point = 23,			//포인트 부족(스킬 포인트)
	GCR_HaveEmporia = 24,	//엠포리아 소유.
	GCR_CantMe = 25,		//내가 초대/탈퇴/추방 등 명령어를 사용할 수 없는 상태
	GCR_CantHim = 26,		//상대방이 초대/탈퇴/추방 명령어를 사용 받을 수 없는 상태
	GCR_JoinWait = 27,		//길드가입 대기 상태입니다.
	GCR_System = 28,
	GCR_NotFoundUser = 29,	// 대상을 찾을 수 없다.
	GCR_NotInvAuth = 30,	// 길드금고 사용 권한이 없다.
	GCR_NotInvMove = 31,	// 이동할 수 없는 위치
	GCR_LimitCost = 32,     //엠포리아 참가비용 부족
	GCR_Limit = 255,		//시스템 최대치(또는 혹은 제한)
}EGuildCommandRet;

typedef enum
{
	EAR_SUEECSS						= 0,
	EAR_DBERROR						= 1,
	EAR_ONLY_MASTER					= 2,
	EAR_NOT_ENOUGH_GUILDEXP			= 3,
	EAR_CRITICAL_ERROR				= 4,
}EEmporiaAdminstrationRet;

typedef enum
{
	ERNT_NONE			= 0,
	ERNT_USEROUT		= 1,
	ERNT_TIMEOVER		= 2,
	ERNT_DESTORYCORE	= 3,
	ERNT_KILLDRAGON		= 4,
	ERNT_HAVEPOINT		= 5,
}EEmporiaResultNoticeType;

typedef enum
{
	ESLT_NONE			= 0,
	ESLT_MERCENARY		= 1,
}EEmporiaStatusListType;

typedef enum 
{
	MAX_NONE_INVEN_IDX = 0,
	MAX_CHAR_EQUIP_IDX = 16,

	MAX_FIT_IDX = 32,
	MAX_HOME_IDX = 108,
	
	MAX_POST_IDX = 100,
	MAX_USER_MARKET_IDX = 50,

	MAX_SYSTEM_INV_IDX = 250,

	MAX_TIME_SAFE_IDX = 36,
	MAX_TIME_SAFE_IDX_EXTEND = 120,

	MAX_EQUIP_IDX = 36,
	MAX_EQUIP_IDX_EXTEND = 144,

	MAX_CONSUME_IDX = 36,
	MAX_CONSUME_IDX_EXTEND = 144,

	MAX_ETC_IDX = 36,
	MAX_ETC_IDX_EXTEND = 144,

	MAX_CASH_IDX = 36,
	MAX_CASH_IDX_EXTEND = 144,

	MAX_SAFE_IDX = 36,
	MAX_SAFE_IDX_EXTEND = 120,

	MAX_CASH_SAFE_IDX = 36,
	MAX_CASH_SAFE_IDX_EXTEND = 72,

	MAX_PET_IDX = 18,
	MAX_PET_IDX_EXTEND = MAX_PET_IDX * 2,

	MAX_SHARE_TIME_SAFE_IDX = 12,
	MAX_SHARE_TIME_SAFE_EXTEND = 72,

	BASE_GUILD_IDX			= 24, // 기본 24칸
	MAX_GUILD_IDX_EXTEND	= 48, // 최대 48칸

	MAX_SAFE_ADD_IDX = 24,

}EMaxInvIDX;//수량


//
BYTE const cMaxGuildEmblem = 105;
typedef struct tagGuildBasicInfo
{
	tagGuildBasicInfo()
	{
		Clear();
	}

	void Clear()
	{
		kGuildGuid.Clear();
		kMasterGuid.Clear();
		kGuildName.clear();
		sLevel = 1;
		sSkillPoint = 0;
		iExperience = 0;
		cEmblem = 0;
		kCreateDate.Clear();
		//sTaxRate = 0;
		ZeroMemory(cGuildSkill, MAX_DB_GUILD_SKILL_SIZE);
		i64Money = 0i64;

		for( int i = 0, j = 1; i < MAX_DB_INVEXTEND_SIZE; i+=2, ++j )
		{
			abyInvExtern[i] = j;
			abyInvExternIdx[i] = j;

			abyInvExtern[i+1] = MAX_EQUIP_IDX;
			abyInvExternIdx[i+1] = 0;
		}

		//ZeroMemory(abyInvExtern, MAX_DB_INVEXTEND_SIZE);
		//ZeroMemory(abyInvExternIdx, MAX_DB_INVEXTEND_SIZE);
		
		ZeroMemory(abyInvAuthority, MAX_DB_GUILD_INV_AUTHORITY);
	}

	BM::GUID kGuildGuid;
	BM::GUID kMasterGuid;
	std::wstring kGuildName;
	unsigned short sLevel;
	unsigned short sSkillPoint;
	__int64 iExperience;
	BYTE cEmblem;
	BM::PgPackedTime kCreateDate;
	//unsigned short sTaxRate;
	BYTE cGuildSkill[MAX_DB_GUILD_SKILL_SIZE];
	__int64 i64Money;
	BYTE abyInvExtern[MAX_DB_INVEXTEND_SIZE];
	BYTE abyInvExternIdx[MAX_DB_INVEXTEND_SIZE];
	BYTE abyInvAuthority[MAX_DB_GUILD_INV_AUTHORITY];

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(kGuildGuid);
		rkPacket.Push(kMasterGuid);
		rkPacket.Push(kGuildName);
		rkPacket.Push(sSkillPoint);
		rkPacket.Push(sLevel);
		rkPacket.Push(iExperience);
		rkPacket.Push(cEmblem);
		rkPacket.Push(kCreateDate);
		//rkPacket.Push(sTaxRate);
		rkPacket.Push(cGuildSkill);
		rkPacket.Push(i64Money);
		rkPacket.Push(abyInvExtern);
		rkPacket.Push(abyInvExternIdx);
		rkPacket.Push(abyInvAuthority);
	}
	void ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop(kGuildGuid);
		rkPacket.Pop(kMasterGuid);
		rkPacket.Pop(kGuildName);
		rkPacket.Pop(sSkillPoint);
		rkPacket.Pop(sLevel);
		rkPacket.Pop(iExperience);
		rkPacket.Pop(cEmblem);
		rkPacket.Pop(kCreateDate);
		//rkPacket.Pop(sTaxRate);
		rkPacket.Pop(cGuildSkill);
		rkPacket.Pop(i64Money);
		rkPacket.Pop(abyInvExtern);
		rkPacket.Pop(abyInvExternIdx);
		rkPacket.Pop(abyInvAuthority);
	}
} SGuildBasicInfo;

//
typedef struct tagGuildMemberGradeInfo
{	
	tagGuildMemberGradeInfo()
	{
		Clear();
	}

	void Clear()
	{
		kGuildGuid.Clear();
		cGradeGroupLevel = 0;
		kGradeName[0].clear();
		kGradeName[1].clear();
		kGradeName[2].clear();
		kGradeName[3].clear();
		kGradeName[4].clear();
		kLastModifyDate.Clear();
	}

	BM::GUID kGuildGuid;
	BYTE cGradeGroupLevel;
	std::wstring kGradeName[MAX_GUILDGRADENAME];
	BM::PgPackedTime kLastModifyDate;

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(kGuildGuid);
		rkPacket.Push(cGradeGroupLevel);
		rkPacket.Push(kGradeName[0]);
		rkPacket.Push(kGradeName[1]);
		rkPacket.Push(kGradeName[2]);
		rkPacket.Push(kGradeName[3]);
		rkPacket.Push(kGradeName[4]);
		rkPacket.Push(kLastModifyDate);
	}
	void ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop(kGuildGuid);
		rkPacket.Pop(cGradeGroupLevel);
		rkPacket.Pop(kGradeName[0]);
		rkPacket.Pop(kGradeName[1]);
		rkPacket.Pop(kGradeName[2]);
		rkPacket.Pop(kGradeName[3]);
		rkPacket.Pop(kGradeName[4]);
		rkPacket.Pop(kLastModifyDate);
	}
	
	bool Get(int iLevel, std::wstring& rkOut)const
	{
		int const iCur = iLevel - (cGradeGroupLevel * MAX_GUILDGRADENAME);
		if(0 > iCur || MAX_GUILDGRADENAME <= iCur) {return false;};

		rkOut = kGradeName[iCur];
		return true;
	}
} SGuildMemberGradeInfo;
typedef std::vector< SGuildMemberGradeInfo > ContGuildGrade;

//
typedef struct tagGuildMemberInfo
{
	tagGuildMemberInfo()
	{
		Clear();
	}
	
	tagGuildMemberInfo(std::wstring const &rkCharName, BM::GUID const &rkCharGuid, BYTE const cNewGrade)
		:kCharName(rkCharName), kCharGuid(rkCharGuid), cGrade(cNewGrade)
	{
		sLevel = 1;
		cClass = 1;
		kGndKey.Clear();
		sChannel = 0;
	}

	void Clear()
	{
		kCharName.clear();
		kCharGuid.Clear();
		cGrade = GMG_Membmer;
		sLevel = 1;
		cClass = 1;
		kGndKey.Clear();
		sChannel = 0;
	}

	std::wstring kCharName;
	BM::GUID kCharGuid;
	BYTE cGrade;
	unsigned short sLevel;
	BYTE cClass;

	SGroundKey kGndKey;
	short sChannel;
	SHOMEADDR	kHomeAddr;

	void WriteToPacket(BM::Stream &rkPacket)const
	{
		rkPacket.Push(kCharGuid);
		rkPacket.Push(kCharName);
		rkPacket.Push(cGrade);
		rkPacket.Push(sLevel);
		rkPacket.Push(cClass);
		rkPacket.Push(kGndKey);
		rkPacket.Push(sChannel);
		rkPacket.Push(kHomeAddr);
	}
	void ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop(kCharGuid);
		rkPacket.Pop(kCharName);
		rkPacket.Pop(cGrade);
		rkPacket.Pop(sLevel);
		rkPacket.Pop(cClass);
		rkPacket.Pop(kGndKey);
		rkPacket.Pop(sChannel);
		rkPacket.Pop(kHomeAddr);
	}
} SGuildMemberInfo;

// 길드가입 설정
typedef struct tagSetGuildEntrancedOpen
{	
	bool bIsGuildEntrance;			// 가입 신청을 받을 것인가?
	short sGuildEntranceLevel;		// 길드에 가입할 수 있는 레벨 설정
	__int64 i64GuildEntranceClass;	// 클래스 제한
	__int64 i64GuildEntranceFee;	// 길드 가입 비용
	std::wstring wstrGuildPR;

	tagSetGuildEntrancedOpen()
	{	
		bIsGuildEntrance = false;
		sGuildEntranceLevel = 0;
		i64GuildEntranceClass = 0;
		i64GuildEntranceFee = 0;
		wstrGuildPR.clear();
	}

	bool operator ==(const tagSetGuildEntrancedOpen& rhs)const
	{
		return	(bIsGuildEntrance == rhs.bIsGuildEntrance)
		&&		(sGuildEntranceLevel == rhs.sGuildEntranceLevel)
		&&		(i64GuildEntranceClass == rhs.i64GuildEntranceClass)
		&&		(i64GuildEntranceFee == rhs.i64GuildEntranceFee)
		&&		(wstrGuildPR == rhs.wstrGuildPR);
	}

	bool operator !=(const tagSetGuildEntrancedOpen& rhs)const
	{
		 return !(*this == rhs);
	}

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(bIsGuildEntrance);
		rkPacket.Push(sGuildEntranceLevel);
		rkPacket.Push(i64GuildEntranceClass);
		rkPacket.Push(i64GuildEntranceFee);
		rkPacket.Push(wstrGuildPR);
	}

	bool ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop(bIsGuildEntrance);
		rkPacket.Pop(sGuildEntranceLevel);
		rkPacket.Pop(i64GuildEntranceClass);
		rkPacket.Pop(i64GuildEntranceFee);		
		return rkPacket.Pop(wstrGuildPR);
	}
} SSetGuildEntrancedOpen;

// 길드금고 전용 로그
typedef struct tagGuild_Inventory_Log
{
	tagGuild_Inventory_Log()
	{
	}

	explicit tagGuild_Inventory_Log( const BM::GUID &rkGuildGuid, const BM::GUID &rkItemGuid, const BM::DBTIMESTAMP_EX &rkDateTime, const std::wstring &rkCharName, const BYTE byType, 
		const int iItemNo, const __int64 i64Count )
		:m_kGuildGuid(rkGuildGuid), m_kItemGuid(rkItemGuid), m_kDateTime(rkDateTime), m_kCharName(rkCharName), m_kType(byType), m_kItemNo(iItemNo), m_kCount(i64Count)
	{
	}

	tagGuild_Inventory_Log& operator=(const tagGuild_Inventory_Log& rhs)
	{
		m_kGuildGuid = rhs.m_kGuildGuid;
		m_kItemGuid = rhs.m_kItemGuid;
		m_kDateTime = rhs.m_kDateTime;
		m_kCharName = rhs.m_kCharName;
		m_kType = rhs.m_kType;
		m_kItemNo = rhs.m_kItemNo;
		m_kCount = rhs.m_kCount;

		return *this;
	}

	BM::GUID m_kGuildGuid;
	BM::GUID m_kItemGuid;
	BM::DBTIMESTAMP_EX m_kDateTime;
	std::wstring m_kCharName;
	BYTE m_kType;
	int m_kItemNo;
	__int64 m_kCount;

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(m_kGuildGuid);
		rkPacket.Push(m_kItemGuid);
		rkPacket.Push(m_kDateTime);
		rkPacket.Push(m_kCharName);
		rkPacket.Push(m_kType);
		rkPacket.Push(m_kItemNo);		
		rkPacket.Push(m_kCount);		
	}

	void ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop(m_kGuildGuid);
		rkPacket.Pop(m_kItemGuid);
		rkPacket.Pop(m_kDateTime);
		rkPacket.Pop(m_kCharName);
		rkPacket.Pop(m_kType);
		rkPacket.Pop(m_kItemNo);
		rkPacket.Pop(m_kCount);
	}
	
}SGuild_Inventory_Log;

typedef std::deque<SGuild_Inventory_Log> CONT_GUILD_INVENTORY_LOG;
const int GUILD_LOG_PAGE_PER_COUNT = 10;


// 길드가입 신청자
typedef struct tagGuildEntranceApplicant
{
	BM::GUID kGuildGuid;
	BM::GUID kCharGuid;
	std::wstring wstrName;
	short sLevel;
	BYTE byClass;
	std::wstring wstrMessage;
	DBTIMESTAMP kDateTime;
	BYTE byState;
	__int64 i64GuildEntranceFee; // 가입신청 했을 때의 금액이 최종금액이 되어야 한다.

	tagGuildEntranceApplicant operator=(tagGuildEntranceApplicant const& rhs)
	{
		kGuildGuid = rhs.kGuildGuid;
		kCharGuid = rhs.kCharGuid;
		wstrName = rhs.wstrName;
		sLevel = rhs.sLevel;
		byClass = rhs.byClass;
		wstrMessage = rhs.wstrMessage;		
		kDateTime = rhs.kDateTime;
		byState = rhs.byState;
		i64GuildEntranceFee = rhs.i64GuildEntranceFee;
	}

	tagGuildEntranceApplicant()
	{
		kGuildGuid.Clear();
		kCharGuid.Clear();
		wstrName.clear();
		sLevel = 0;
		byClass = 0;
		wstrMessage.clear();		
		ZeroMemory(&kDateTime, sizeof(kDateTime));
		byState = 0;
		i64GuildEntranceFee = 0i64;		
	}

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(kGuildGuid);
		rkPacket.Push(kCharGuid);
		rkPacket.Push(wstrName);
		rkPacket.Push(sLevel);
		rkPacket.Push(byClass);
		rkPacket.Push(wstrMessage);		
		rkPacket.Push(kDateTime);
		rkPacket.Push(byState);
		rkPacket.Push(i64GuildEntranceFee);
	}

	bool ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop(kGuildGuid);
		rkPacket.Pop(kCharGuid);
		rkPacket.Pop(wstrName);
		rkPacket.Pop(sLevel);
		rkPacket.Pop(byClass);
		rkPacket.Pop(wstrMessage);		
		rkPacket.Pop(kDateTime);
		rkPacket.Pop(byState);
		return rkPacket.Pop(i64GuildEntranceFee);
	}
} SGuildEntranceApplicant;

// 길드가입 신청을 허용한 길드 리스트
typedef struct tagEntranceOpenGuild
{
	int iRank;
	BM::GUID kGuildGuid;
	short sGuildEntranceLevel;
	__int64 i64GuildEntranceClass;
	__int64 i64GuildEntranceFee;
	std::wstring wstrGuildPR;
	BYTE byEmblem;
	std::wstring wstrGuildName;
	short sGuildLevel;
	std::wstring wstrMasterName;
	int iGuildMemberCount;

	tagEntranceOpenGuild()
	{
		Init();
	}

	void Init()
	{
		iRank = 0;
		kGuildGuid.Clear();
		sGuildEntranceLevel = 0;
		i64GuildEntranceClass = 0i64;
		i64GuildEntranceFee = 0i64;
		wstrGuildPR.clear();
		byEmblem = 0;
		wstrGuildName.clear();
		sGuildLevel = 0;
		wstrMasterName.clear();
		iGuildMemberCount = 0;
	}

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(iRank);
		rkPacket.Push(kGuildGuid);
		rkPacket.Push(sGuildEntranceLevel);
		rkPacket.Push(i64GuildEntranceClass);
		rkPacket.Push(i64GuildEntranceFee);
		rkPacket.Push(wstrGuildPR);
		rkPacket.Push(byEmblem);
		rkPacket.Push(wstrGuildName);
		rkPacket.Push(sGuildLevel);
		rkPacket.Push(wstrMasterName);
		rkPacket.Push(iGuildMemberCount);
	}

	void ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop(iRank);
		rkPacket.Pop(kGuildGuid);
		rkPacket.Pop(sGuildEntranceLevel);
		rkPacket.Pop(i64GuildEntranceClass);
		rkPacket.Pop(i64GuildEntranceFee);
		rkPacket.Pop(wstrGuildPR);
		rkPacket.Pop(byEmblem);
		rkPacket.Pop(wstrGuildName);
		rkPacket.Pop(sGuildLevel);
		rkPacket.Pop(wstrMasterName);
		rkPacket.Pop(iGuildMemberCount);
	}
} SEntranceOpenGuild;

// 길드금고는 아이템만 저장한다. 골드는 TB_Guild_Basic_Info에 컬럼이 있다.
typedef struct tagGuildInventory
{	
	BM::GUID kItemGuid;
	int iItemNo;
	BYTE byInvType;
	BYTE byItemPos;
	short sItemCount;
	BYTE byItemState;

	tagGuildInventory()
	{
		Init();
	}

	void Init()
	{
		kItemGuid.Clear();
		iItemNo = 0;
		byInvType = 0;
		byItemPos = 0;
		sItemCount = 0;
		byItemState = 0;
	}

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(kItemGuid);
		rkPacket.Push(iItemNo);
		rkPacket.Push(byInvType);
		rkPacket.Push(byItemPos);
		rkPacket.Push(sItemCount);
		rkPacket.Push(byItemState);
	}

	void ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop(kItemGuid);
		rkPacket.Pop(iItemNo);
		rkPacket.Pop(byInvType);
		rkPacket.Pop(byItemPos);
		rkPacket.Pop(sItemCount);
		rkPacket.Pop(byItemState);
	}
} SGuildInventory;

//
typedef SET_GUID ContGuidSet;

//
typedef struct tagGuildOtherInfo
{
	tagGuildOtherInfo()
		: kGuid(), kName(), cEmblem(0), byEmporiaGrade(0)
	{
	}

	tagGuildOtherInfo(tagGuildOtherInfo const& rhs)
		: kGuid(rhs.kGuid), kName(rhs.kName), cEmblem(rhs.cEmblem), byEmporiaGrade(rhs.byEmporiaGrade)
	{
	}

	tagGuildOtherInfo( SGuildBasicInfo const &rkBasicInfo, BYTE _byEmporiaGrade )
		: byEmporiaGrade(_byEmporiaGrade)
	{
		Set(rkBasicInfo.kGuildGuid, rkBasicInfo.kGuildName, rkBasicInfo.cEmblem); 
	}

	void Set(BM::GUID const &rkGuid, std::wstring const &rkName, BYTE const Emblem)
	{
		kGuid = rkGuid;
		kName = rkName;
		cEmblem = Emblem;
	}

	void Set(tagGuildOtherInfo const& rhs)
	{
		Set(rhs.kGuid, rhs.kName, rhs.cEmblem);
		byEmporiaGrade = rhs.byEmporiaGrade;
	}

	void WriteToPacket(BM::Stream& rkPacket)const
	{
		rkPacket.Push(kGuid);
		rkPacket.Push(kName);
		rkPacket.Push(cEmblem);
		rkPacket.Push(byEmporiaGrade);
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(kGuid);
		rkPacket.Pop(kName);
		rkPacket.Pop(cEmblem);
		rkPacket.Pop(byEmporiaGrade);		
	}

	BM::GUID kGuid;
	std::wstring kName;
	BYTE cEmblem;
	BYTE byEmporiaGrade;
} SGuildOtherInfo;

typedef struct tagQuestTriggerInfo
{
	unsigned int iGroundNo;
	int iTriggerNo;

	tagQuestTriggerInfo()
		:iGroundNo(0), iTriggerNo(0)
	{
	}
	tagQuestTriggerInfo(unsigned int const iGndNo, int const iTrgNo)
		:iGroundNo(iGndNo), iTriggerNo(iTrgNo)
	{
	}
	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(iGroundNo);
		rkPacket.Pop(iTriggerNo);
	}
	void WriteToPacket(BM::Stream& rkPacket)const
	{
		rkPacket.Push(iGroundNo);
		rkPacket.Push(iTriggerNo);
	}
	operator size_t () const
	{
		return (size_t)(iGroundNo^iTriggerNo);
	}
} SQuestTriggerInfo;

//-> Party
typedef struct tagPartyUserInfo
{
	BM::GUID kMemberGuid;
	BM::GUID kCharGuid;
	unsigned short sLevel;
	unsigned short sHP; // 1/n
	unsigned short sMP; // 1/n	
	int iClass;
	SChnGroundKey kChnGndKey;
	std::wstring kName;
	int iFriend;
	int iUID;
	POINT3 ptPos;
	SHOMEADDR kHomeAddr;

	tagPartyUserInfo()
	{
		Clear();
	}

	tagPartyUserInfo(SContentsUser const& rkUserInfo)
	{
		Clear();
		kName = rkUserInfo.Name();
		kCharGuid = rkUserInfo.kCharGuid;
		kChnGndKey = rkUserInfo.kGndKey;
		sLevel = rkUserInfo.sLevel;
		iClass = rkUserInfo.iClass;		
		kMemberGuid = rkUserInfo.kMemGuid;
		iUID = rkUserInfo.iUID;
		kHomeAddr = rkUserInfo.kHomeAddr;
	}

	void Set( tagPartyUserInfo const &rhs)
	{
		kCharGuid = rhs.kCharGuid;
		sLevel = rhs.sLevel;
		sHP = rhs.sHP;
		sMP = rhs.sMP;
		iClass = rhs.iClass;
		kChnGndKey = rhs.kChnGndKey;
		kName = rhs.kName;
		iFriend = rhs.iFriend;
		kMemberGuid = rhs.kMemberGuid;
		iUID = rhs.iUID;
		ptPos = rhs.ptPos;
		kHomeAddr = rhs.kHomeAddr;
	}

	void Clear()
	{
		kCharGuid.Clear();
		sHP = 0;//PMCA_MAX_PERCENT;
		sMP = 0;//PMCA_MAX_PERCENT;
		sLevel = 1;
		iClass = 1;
		kChnGndKey.Clear();
		kName.clear();
		iFriend = 0;
		kMemberGuid.Clear();
		iUID = 0;
		ptPos.Clear();
		kHomeAddr.Clear();
	}

	int const GroundNo()const
	{
		return kChnGndKey.GroundNo();
	}
	
	size_t min_size()const
	{
		return 
			sizeof(kCharGuid)+
			sizeof(sLevel)+
			sizeof(sHP)+
			sizeof(sMP)+
			sizeof(iClass)+
			sizeof(kChnGndKey)+
			sizeof(size_t)+//kName)+
			sizeof(iFriend)+
			sizeof(kMemberGuid)+
			sizeof(iUID)+
			sizeof(ptPos)+
			sizeof(kHomeAddr);
	}

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(kCharGuid);
		rkPacket.Push(sLevel);
		rkPacket.Push(sHP);
		rkPacket.Push(sMP);
		rkPacket.Push(iClass);
		kChnGndKey.WriteToPacket( rkPacket );
		rkPacket.Push(kName);
		rkPacket.Push(iFriend);
		rkPacket.Push(kMemberGuid);
		rkPacket.Push(iUID);
		rkPacket.Push(ptPos);
		rkPacket.Push(kHomeAddr);
	}

	void ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop(kCharGuid);
		rkPacket.Pop(sLevel);
		rkPacket.Pop(sHP);
		rkPacket.Pop(sMP);
		rkPacket.Pop(iClass);
		kChnGndKey.ReadFromPacket( rkPacket );
		rkPacket.Pop(kName);
		rkPacket.Pop(iFriend);
		rkPacket.Pop(kMemberGuid);
		rkPacket.Pop(iUID);
		rkPacket.Pop(ptPos);
		rkPacket.Pop(kHomeAddr);
	}
}SPartyUserInfo;

// 원정대 멤버 정보.
typedef struct tagExpeditionUserInfo
{
	BM::GUID kCharGuid;
	std::wstring CharName;			// 원정대원 캐릭터 이름.
	unsigned short sLevel;			// 원정대원 레벨.
	unsigned short sHP;				// 1/n
	unsigned short sMP;				// 1/n
	int iClass;						// 원정대원 직업.
	bool bAlive;					// 원정대원 생존 여부.
	unsigned short sTeam;			// 팀 번호(파티 번호).

	tagExpeditionUserInfo()
	{
		Clear();
	}

	tagExpeditionUserInfo(SContentsUser const & rkUserInfo, unsigned short Team = 0, bool Alive = true)
	{
		kCharGuid		= rkUserInfo.kCharGuid;
		CharName		= rkUserInfo.kName;
		sLevel			= rkUserInfo.sLevel;
		iClass			= rkUserInfo.iClass;
		sHP				= 0;
		sMP				= 0;
		sTeam			= Team;
		bAlive			= Alive;
	}

	tagExpeditionUserInfo(tagPartyUserInfo const & rkUserInfo, unsigned short Team = 0, bool Alive = true)
	{
		kCharGuid		= rkUserInfo.kCharGuid;
		CharName		= rkUserInfo.kName;
		sLevel			= rkUserInfo.sLevel;
		iClass			= rkUserInfo.iClass;
		sHP				= rkUserInfo.sHP;
		sMP				= rkUserInfo.sMP;
		sTeam			= Team;
		bAlive			= Alive;
	}

	tagExpeditionUserInfo(tagExpeditionUserInfo const & rhs)
	{
		kCharGuid = rhs.kCharGuid;
		CharName = rhs.CharName;
		sLevel = rhs.sLevel;
		sHP = rhs.sHP;
		sMP = rhs.sMP;
		iClass = rhs.iClass;
		bAlive = rhs.bAlive;
		sTeam = rhs.sTeam;
	}

	void Set(tagExpeditionUserInfo const & rhs)
	{
		kCharGuid = rhs.kCharGuid;
		CharName = rhs.CharName;
		sLevel = rhs.sLevel;
		sHP = rhs.sHP;
		sMP = rhs.sMP;
		iClass = rhs.iClass;
		bAlive = rhs.bAlive;
		sTeam = rhs.sTeam;
	}

	void Clear()
	{
		kCharGuid.Clear();
		CharName.clear();
		sLevel = 0;
		sHP = 0;	//PMCA_MAX_PERCENT;
		sMP = 0;	//PMCA_MAX_PERCENT;
		iClass = 0;
		bAlive = true;
		sTeam = 0;
	}

	void WriteToPacket(BM::Stream & rkPacket) const
	{
		rkPacket.Push(kCharGuid);
		rkPacket.Push(CharName);
		rkPacket.Push(sLevel);
		rkPacket.Push(iClass);
		rkPacket.Push(bAlive);
		rkPacket.Push(sTeam);
		rkPacket.Push(sHP);
		rkPacket.Push(sMP);
	}

	void ReadFromPacket(BM::Stream & rkPacket)
	{
		rkPacket.Pop(kCharGuid);
		rkPacket.Pop(CharName);
		rkPacket.Pop(sLevel);
		rkPacket.Pop(iClass);
		rkPacket.Pop(bAlive);
		rkPacket.Pop(sTeam);
		rkPacket.Pop(sHP);
		rkPacket.Pop(sMP);
	}
} SExpeditionUserInfo;

// 원정대 초대 유저 리스트 정보
typedef struct tagExpeditionInviteUserInfo
{
	BM::GUID CharGuid;
	std::wstring CharName;
	int iClass;
	unsigned short sLevel;
	bool bHaveKeyItem;

	tagExpeditionInviteUserInfo()
	{
		Clear();
	}

	tagExpeditionInviteUserInfo(BM::GUID const & Guid, std::wstring const & Name, int Class, unsigned short Level, bool KeyItem)
	{
		CharGuid	= Guid;
		CharName	= Name;
		iClass		= Class;
		sLevel		= Level;
		bHaveKeyItem = KeyItem;
	}

	void Clear()
	{
		CharGuid.Clear();
		CharName.clear();
		iClass = 0;
		sLevel = 0;
		bHaveKeyItem = false;
	}

	void WriteToPacket(BM::Stream & Packet) const
	{
		Packet.Push(CharGuid);
		Packet.Push(CharName);
		Packet.Push(iClass);
		Packet.Push(sLevel);
		Packet.Push(bHaveKeyItem);
	}

	void ReadFromPacket(BM::Stream & Packet)
	{
		Packet.Pop(CharGuid);
		Packet.Pop(CharName);
		Packet.Pop(iClass);
		Packet.Pop(sLevel);
		Packet.Pop(bHaveKeyItem);
	}

} SExpeditionInviteUserInfo;

int const DEF_PV_MAX_MEMBER_CNT = 4;
typedef struct tagPartyListInfo
{
	BM::GUID		kPartyGuid;
	std::wstring	kPartyName;
	BYTE			cCurMember;
	BYTE			cMaxMember;
	int				iPartyOption;
	int				iPartyLevel;
	std::wstring	kPartySubName;
	int				iPartyAttribute;
	int				iPartyContinent;
	int				iPartyArea_NameNo;
	BM::GUID		kMasterGuid;
	std::wstring	kMasterName;
	int				iMasterClass;
	unsigned short	iMasterLevel;
	int				iMasterMapNo;
	short			ChannelNo;			// 채널 정보는 ReadFromPacket에서 제외함. 별개로 쏘고 받음.
	BYTE			cStatus;
	SPartyUserInfo	kMemberInfo[DEF_PV_MAX_MEMBER_CNT-1];

	tagPartyListInfo()
	{
		Clear();
	}

	void Set(tagPartyListInfo const & rhs)
	{
		kPartyGuid		=	rhs.kPartyGuid;
		kPartyName		=	rhs.kPartyName;
		cCurMember		=	rhs.cCurMember;	
		cMaxMember		=	rhs.cMaxMember;	
		iPartyOption	=	rhs.iPartyOption;
		iPartyLevel		=	rhs.iPartyLevel;
		kPartySubName	=	rhs.kPartySubName;
		iPartyAttribute	=	rhs.iPartyAttribute;
		iPartyContinent	=	rhs.iPartyContinent;
		iPartyArea_NameNo	=	rhs.iPartyArea_NameNo;
		kMasterGuid		=	rhs.kMasterGuid;
		kMasterName		=	rhs.kMasterName;
		iMasterClass	=	rhs.iMasterClass;
		iMasterLevel	=	rhs.iMasterLevel;
		iMasterMapNo	=	rhs.iMasterMapNo;
		ChannelNo		=	rhs.ChannelNo;
		cStatus			=	rhs.cStatus;
		
		for(int i=0;i<(DEF_PV_MAX_MEMBER_CNT-1); ++i)
		{
			kMemberInfo[i].Set(rhs.kMemberInfo[i]);
		}
	}

	void Clear()
	{
		kPartyGuid.Clear();
		kPartyName.clear();
		cCurMember		=	0;
		cMaxMember		=	0;
		iPartyOption	=	0;
		iPartyLevel		=	0;
		kPartySubName.clear();
		iPartyAttribute	=	0;
		iPartyContinent	=	0;
		iPartyArea_NameNo	=	0;
		kMasterGuid.Clear();
		kMasterName.clear();
		iMasterClass	=	0;
		iMasterLevel	=	0;
		iMasterMapNo	=	0;
		ChannelNo		=	0;
		cStatus			=	0;
		for(int i=0;i<(DEF_PV_MAX_MEMBER_CNT-1); ++i)
		{
			kMemberInfo[i].Clear();
		}
	}

	void ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop(kPartyGuid);
		rkPacket.Pop(kPartyName);
		rkPacket.Pop(cCurMember);
		rkPacket.Pop(cMaxMember);
		rkPacket.Pop(iPartyOption);
		rkPacket.Pop(iPartyLevel);
		rkPacket.Pop(kPartySubName);
		rkPacket.Pop(iPartyAttribute);
		rkPacket.Pop(iPartyContinent);
		rkPacket.Pop(iPartyArea_NameNo);
		rkPacket.Pop(cStatus);
		rkPacket.Pop(kMasterGuid);
		rkPacket.Pop(kMasterName);
		rkPacket.Pop(iMasterClass);
		rkPacket.Pop(iMasterLevel);
		rkPacket.Pop(iMasterMapNo);

		for(int i=0;i<(DEF_PV_MAX_MEMBER_CNT-1); ++i)
		{
			kMemberInfo[i].ReadFromPacket(rkPacket);
		}
	}
}SPartyListInfo;

typedef struct tagFindPartyUserListInfo
{
	int iClass;
	int iOriginalClass;
	unsigned short sLevel;
	BM::GUID kCharGuid;
	SGroundKey kGndKey;
	std::wstring kName;
	std::wstring kContents;

	tagFindPartyUserListInfo()
	{
		Clear();
	}

	tagFindPartyUserListInfo(SContentsUser const& rkUserInfo)
	{
		Clear();
		iClass = rkUserInfo.iBaseClass;
		iOriginalClass = rkUserInfo.iClass;
		sLevel = rkUserInfo.sLevel;
		kCharGuid = rkUserInfo.kCharGuid;
		kGndKey = rkUserInfo.kGndKey;
		kName = rkUserInfo.Name();
	}

	void Set(tagFindPartyUserListInfo const& rhs)
	{
		Clear();
		iClass = rhs.iClass;
		iOriginalClass = rhs.iOriginalClass;
		sLevel = rhs.sLevel;
		kCharGuid = rhs.kCharGuid;
		kGndKey = rhs.kGndKey;
		kName = rhs.kName;
		kContents = rhs.kContents;
	}

	void Clear()
	{
		iClass = 0;
		iOriginalClass = 0;
		sLevel = 0;
		kCharGuid.Clear();		
		kGndKey.Clear();
		kName.clear();
		kContents.clear();
	}

	void WriteToPacket(BM::Stream& rkPacket) const
	{
		rkPacket.Push(kCharGuid);
		rkPacket.Push(iClass);
		rkPacket.Push(iOriginalClass);
		rkPacket.Push(sLevel);
		rkPacket.Push(kGndKey);
		rkPacket.Push(kName);
		rkPacket.Push(kContents);
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(kCharGuid);
		rkPacket.Pop(iClass);
		rkPacket.Pop(iOriginalClass);
		rkPacket.Pop(sLevel);
		rkPacket.Pop(kGndKey);
		rkPacket.Pop(kName);
		rkPacket.Pop(kContents);
	}

}SFindPartyUserListInfo;

typedef struct tagOtherChannelPartyJoinInfo
{
	BM::GUID PartyMasterGuid;
	SContentsUser UserInfo;
	bool bAccept;
	short ChannelNo;

	tagOtherChannelPartyJoinInfo()
	{
		Clear();
	}

	tagOtherChannelPartyJoinInfo(BM::GUID const & _PartyGuid, BM::GUID const & _PartyMatserGuid,
		SContentsUser const & _UserInfo, bool const & _bAccept, short const & _ChannelNo)
	{
		Clear();

		PartyMasterGuid = _PartyMatserGuid;
		UserInfo = _UserInfo;
		bAccept = _bAccept;
		ChannelNo = _ChannelNo;
	}

	void operator = (tagOtherChannelPartyJoinInfo & rhs)
	{
		Clear();

		PartyMasterGuid = rhs.PartyMasterGuid;
		UserInfo = rhs.UserInfo;
		bAccept = rhs.bAccept;
		ChannelNo = rhs.ChannelNo;
	}

	void Clear()
	{
		PartyMasterGuid.Clear();
		UserInfo.Clear();
		bAccept = false;
		ChannelNo = 0;
	}
}SOtherChannelPartyJoinInfo;

typedef struct tagReqItemRepair
{
	tagReqItemRepair()
	{
	}

	tagReqItemRepair(SItemPos const &kItemPos)
	{
		TargetPos(kItemPos);
	}

	CLASS_DECLARATION_S(SItemPos, TargetPos);
}SReqItemRepair;
typedef std::list< SReqItemRepair > CONT_REQ_ITEM_REPAIR;

typedef struct tagReqItemMaking
{
	tagReqItemMaking()
	{
		Clear();
	}
	tagReqItemMaking(int const iNo, int const iMakingCount_)
	{
		Clear();
		iMakingNo = iNo;
		iMakingCount = iMakingCount_;
	}
	
	void Clear()
	{
		iMakingType = 0;
		iMakingNo = 0;
		iMakingCount = 0;
		bUseOptionItem = false;
	}

	void WriteToPacket(BM::Stream &rkPacket) const
	{
		rkPacket.Push(iMakingType);
		rkPacket.Push(iMakingNo);
		rkPacket.Push(iMakingCount);
		rkPacket.Push(bUseOptionItem);
	}
	
	bool ReadFromPacket(BM::Stream &rkPacket)
	{
		return ( true == rkPacket.Pop(iMakingType) && 
				 true == rkPacket.Pop(iMakingNo) &&
				 true == rkPacket.Pop(iMakingCount) &&
				 true == rkPacket.Pop(bUseOptionItem));
	}

	size_t min_size() const
	{
		return sizeof(int) + sizeof(int) + sizeof(int) + sizeof(bool);
	}

	int iMakingType;
	int iMakingNo;
	int iMakingCount;
	bool bUseOptionItem;
	// 절대 아이템 갯수, 이 플래그가 켜져 있으면. 아이템 감소시. 필요 갯수만큼 감소가 아니라. 패킷에 넣어준 갯수 만큼 감소한다.
}SReqItemMaking;

typedef std::vector< SReqItemMaking > CONT_REQ_ITEM_MAKING;
typedef std::map<int,int> CONT_MAKING_ITEM_COUNT;

typedef struct tagReqSoulStoneTrade
{
	tagReqSoulStoneTrade()
	{
		Clear();
	}
	tagReqSoulStoneTrade(int const ItemNo, int const MaterialNo, int const MaterialNeedsCount, int TradeCount)
		:	iItemNo(ItemNo), iMaterialNo(MaterialNo), iMaterialNeedsCount(MaterialNeedsCount), iTradeCount(TradeCount)
	{
	}

	void Clear()
	{
		iItemNo					= 0;
		iMaterialNo				= 0;
		iMaterialNeedsCount		= 0;
		iTradeCount				= 0;
	}

	void WriteToPacket(BM::Stream & rkPacket) const
	{
		rkPacket.Push(iItemNo);
		rkPacket.Push(iMaterialNo);
		rkPacket.Push(iMaterialNeedsCount);
		rkPacket.Push(iTradeCount);
	}

	bool ReadFromPacket(BM::Stream & rkPacket)
	{
		return (rkPacket.Pop(iItemNo)) && (rkPacket.Pop(iMaterialNo)) && (rkPacket.Pop(iMaterialNeedsCount)) && (rkPacket.Pop(iTradeCount));
	}

	size_t min_size() const
	{
		return sizeof(iItemNo) + sizeof(iMaterialNo) + sizeof(iMaterialNeedsCount) + sizeof(iTradeCount);
	}

	int iItemNo;
	int iMaterialNo;
	int iMaterialNeedsCount;
	int iTradeCount;
}SReqSoulStoneTrade;


typedef enum eAddExpCause
{
	AEC_None = 0,
	AEC_KillMonster		= 0x01,
	AEC_ShareParty		= 0x02,
	AEC_QuestReward		= 0x04,
	AEC_MissionBonus	= 0x08,
	AEC_MissionQuest	= 0x10,
	AEC_PetExp			= 0x20,
	AEC_AlramMission	= 0x40,

	AEC_CheckShareParty	= AEC_KillMonster|AEC_ShareParty,
	AEC_Max = 0xFF
}EAddExpCause;

typedef enum eFriendMgr
{
	FM_InitPool_FriendList	= 5000,			//목표동접 1만에 반
	FM_InitPool_FriendItem	= 1000,		//친구목록은 40개씩
	FM_MAXFRIEND			= 30,			//최대 친구 등록 수는 40 (가장 기초)
	FM_MaxFriend_Max		= 200,			//아이템으로 최대 늘어 날 수 있는 친구 목록수
	FM_MaxGroup				= 9,			//빈 그룹 포함 최대 9개
} EFriendMgr;

typedef enum eTradeResult
{
	TR_NONE = 0,
	TR_REQUEST,			//거래할래?
	TR_ACCEPT,			//거래 수락
	TR_REJECT,			//거래 거부
	TR_SUCCESS,			//거래 성공(뭐 또 다른 성공 아무꺼나)
	TR_NO_INVEN,		//인벤 공간 부족
	TR_FAIL,			//거래 실패
	TR_ON_TRADEING,		//거래중
	TR_TRADED_IN_3SEC,	//아직 3초 안지났음
	TR_QUEST_ITEM,		//퀘스트 아이템은 거래 불가
	TR_NOT_FIND_ITEM,	//해당 아이템을 인벤에서 찾을 수 없음. 아마도 사기?
	TR_CASTER_IS_DEAD,	//죽은 놈이 거래 신청
	TR_TARGET_IS_DEAD,	//죽은 놈에게 거래 신청
}ETRADERESULT;

typedef enum eMissionTypeKind
{
	MT_MISSION			= 0,
	MT_EVENT_MISSION	= 1,
	MT_EVENT_HIDDEN		= 2,
	MT_EVENT_MISSION_NPC	= 3,
	MT_MISSION_EASY		= 4,
	MT_EVENT_MISSION_NOT_HAVE_ARCADE = 5,
	MT_MISSION_UNUSED_GADACOIN = 6,
}EMissionTypeKind;

typedef struct tagCreateEntity
{
	tagCreateEntity( BM::GUID const &_kGuid=BM::GUID::NullData(), SClassKey const& _kClassKey=SClassKey() )
		:	kGuid(_kGuid)
		,	kClassKey(_kClassKey)
		,	bUniqueClass(false)
		,	bEternalLife(false)
        ,   iTunningNo(0)
        ,   iTunningLevel(0)
        ,   iLifeTime(0)
	{
	}
	BM::GUID kGuid;
	SClassKey kClassKey;
	POINT3 ptPos;
	bool bUniqueClass;
	bool bEternalLife;
    int iTunningNo;
    int iTunningLevel;
    int iLifeTime;
} SCreateEntity;

typedef struct tagCreateSummoned : public SCreateEntity
{
	tagCreateSummoned()
		: sOverlapClass(0)
		, sNeedSupply(0)
		, bNPC(false)
	{
	}

	short sOverlapClass;
    short sNeedSupply;
	bool bNPC;
} SCreateSummoned;

typedef enum eUserCharacterDeleteResult
{
	UCDR_None = 0,
	UCDR_Success = 1,
	UCDR_Failed = 2,				//찾을수 없어서 실패
	UCDR_Failed_GuildMaster = 3,	//길드 오너라 실패
} EUserCharacterDeleteResult;

typedef enum eSendSkillPacketType : BYTE
{
	SSPT_SKILL_PACKET_NONE			= 0,
	SSPT_SKILL_PACKET_POSITION		= 1, //POINT3
	SSPT_SKILL_PACKET_GUID			= 2, //BM::GUID
}ESendSkillPacketType;

typedef struct tagDBItemData
{
	tagDBItemData()
	{
		InvType = 0;
		InvPos = 0;	

		ItemNo = 0;
		Count = 0;
		Enchant_01 = 0;
		Enchant_02 = 0;
		Enchant_03 = 0;
		Enchant_04 = 0;
		State = 0;
		memset(StatTrackType, 0, sizeof(int) * MAX_STAT_TRACK_ABIL);
		memset(StatTrackValue, 0, sizeof(int) * MAX_STAT_TRACK_ABIL);
	}
//	BYTE DBIdx;
	BM::GUID ItemGuid;
	BM::GUID OwnerGuid;
	BYTE InvType;
	BYTE InvPos;	

	int ItemNo;
	WORD Count;
	__int64 Enchant_01;
	__int64 Enchant_02;
	__int64 Enchant_03;
	__int64 Enchant_04;
	BYTE State;

	BM::DBTIMESTAMP_EX CreateDate;

	// by reOiL Stat Track
	int StatTrackType[MAX_STAT_TRACK_ABIL];
	int StatTrackValue[MAX_STAT_TRACK_ABIL];
}SDBItemData;

typedef std::list< SDBItemData > CONT_DB_ITEM_LIST;

typedef struct tagMissionKey
{
	tagMissionKey()
	{
		Clear();
	}

	tagMissionKey(int const _iKey, int const _iLevel)
		:	iKey(_iKey)
		,	iLevel(_iLevel)
	{}

	void Clear()
	{
		iKey = iLevel = -1;
	}

	bool operator<(const tagMissionKey& rhs)const
	{
		if ( iKey == rhs.iKey )
		{
			return iLevel < rhs.iLevel;
		}
		return iKey < rhs.iKey;
	}

	bool operator>(const tagMissionKey& rhs)const
	{
		if ( iKey == rhs.iKey )
		{
			return iLevel < rhs.iLevel;
		}
		return iKey > rhs.iKey;
	}

	bool operator==(const tagMissionKey& rhs)const
	{
		return iKey == rhs.iKey && iLevel == rhs.iLevel;
	}

	bool operator!=(const tagMissionKey& rhs)const
	{
		return iKey != rhs.iKey || iLevel != rhs.iLevel;
	}

	bool Empty()
	{
		return (iKey < 0 && iLevel < 0) ? true : false;
	}

	std::wstring str() const
	{
		TCHAR chMsg[64];
		_stprintf_s(chMsg, 64, _T("K:%d,L:%d"), iKey, iLevel);
		return chMsg;
	}

	int iKey;
	int iLevel;
	
	DEFAULT_TBL_PACKET_FUNC();
}SMissionKey;

typedef struct tagDefenceAddMonsterKey
{
	tagDefenceAddMonsterKey()
	{
		Clear();
	}

	tagDefenceAddMonsterKey(int const _AddMonster_GroupNo, int const _Success_Count)
		:	iAddMonster_GroupNo(_AddMonster_GroupNo)
		,	iSuccess_Count(_Success_Count)
	{}

	void Clear()
	{
		iAddMonster_GroupNo = iSuccess_Count = -1;
	}

	bool operator<(const tagDefenceAddMonsterKey& rhs)const
	{
		if ( iAddMonster_GroupNo == rhs.iAddMonster_GroupNo )
		{
			return iSuccess_Count < rhs.iSuccess_Count;
		}
		return iAddMonster_GroupNo < rhs.iAddMonster_GroupNo;
	}

	bool operator>(const tagDefenceAddMonsterKey& rhs)const
	{
		if ( iAddMonster_GroupNo == rhs.iAddMonster_GroupNo )
		{
			return iSuccess_Count < rhs.iSuccess_Count;
		}
		return iAddMonster_GroupNo > rhs.iAddMonster_GroupNo;
	}

	bool operator==(const tagDefenceAddMonsterKey& rhs)const
	{
		return iAddMonster_GroupNo == rhs.iAddMonster_GroupNo && iSuccess_Count == rhs.iSuccess_Count;
	}

	bool operator!=(const tagDefenceAddMonsterKey& rhs)const
	{
		return iAddMonster_GroupNo != rhs.iAddMonster_GroupNo || iSuccess_Count != rhs.iSuccess_Count;
	}

	bool Empty()
	{
		return (iAddMonster_GroupNo < 0 && iSuccess_Count < 0) ? true : false;
	}

	int iAddMonster_GroupNo;
	int iSuccess_Count;
	
	DEFAULT_TBL_PACKET_FUNC();
}SDefenceAddMonsterKey;

typedef enum ESMSType
{
	SMS_NONE = 0,
	
	SMS_MIN = 1,

	SMS_IN_GROUND = 1,//해당 그라운드에
	SMS_IN_CHANNEL = 2,//전체 채널에.
	SMS_IN_REALM = 3,//전체 랠름

	SMS_MAX,
}eSMSType;

typedef enum ECARDType
{
	CARD_NONE =	0,
	CARD_POLORING	= 1,
	CARD_LOVE		= 2,
	CARD_FIRE		= 3,
	MARRY_TALK	= 4,
}eCardType;

typedef struct tagSMSHeader
{
	tagSMSHeader()
	{
		eType = SMS_NONE;
		eCType = CARD_NONE;
		iUseItemNo = 0;
	}

	eSMSType eType;
	eCardType eCType;
	std::wstring kSenderName;
	std::wstring kMsg;
	std::wstring kRecverName;
	int iUseItemNo;	

	bool IsCorrect()const
	{
		if( SMS_MIN > eType 
		||	SMS_MAX	< eType)
		{
			return false;
		}
		if(!kMsg.size())
		{
			return false;
		}
		return true;
	}

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		kPacket.Push(eType);
		kPacket.Push(eCType);
		kPacket.Push(kSenderName);
		kPacket.Push(kMsg);
		kPacket.Push(kRecverName);
		kPacket.Push(iUseItemNo);
	}
	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(eType);
		kPacket.Pop(eCType);
		kPacket.Pop(kSenderName);
		kPacket.Pop(kMsg);
		kPacket.Pop(kRecverName);
		kPacket.Pop(iUseItemNo);
	}
}SMS_HEADER;

typedef enum EPostType
{
	POST_NONE = 0,
	POST_NEW,
	POST_CONFIRM,
	POST_RETURN,
	POST_DELETE	
}ePostType;

typedef struct TagPostPack
{
	TagPostPack()
	{
		Clear();
	}

	void Clear()
	{
		kPostGuid.Generate();
		wstrSender.clear();
		wstrReceiver.clear();
		wstrTitle.clear();
		wstrNote.clear();
		eType = POST_NEW;
		biMoney = 0;
		kSendTime.Clear();
		kReceiveTime.Clear();
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(kPostGuid);
		kPacket.Push(wstrSender);
		kPacket.Push(wstrReceiver);
		kPacket.Push(wstrTitle);
		kPacket.Push(wstrNote);
		kPacket.Push(eType);
		kPacket.Push(biMoney);
		kPacket.Push(kSendTime);
		kPacket.Push(kReceiveTime);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(kPostGuid);
		kPacket.Pop(wstrSender);
		kPacket.Pop(wstrReceiver);
		kPacket.Pop(wstrTitle);
		kPacket.Pop(wstrNote);
		kPacket.Pop(eType);
		kPacket.Pop(biMoney);
		kPacket.Pop(kSendTime);
		kPacket.Pop(kReceiveTime);
	}

	BM::GUID			kPostGuid;
	std::wstring		wstrSender;
	std::wstring		wstrReceiver;
	std::wstring		wstrTitle;
	std::wstring		wstrNote;
	ePostType			eType;
	__int64				biMoney;
	BM::DBTIMESTAMP_EX	kSendTime;
	BM::DBTIMESTAMP_EX	kReceiveTime;
}SPost;

typedef struct tagTerminalAuthInfo//관제툴 인증 정보
{
	tagTerminalAuthInfo()
	{
		cGrant =0;
	}

	std::wstring kID;
	std::wstring kPW;
	std::wstring kAuthIP;
	BYTE cGrant;

	bool IsEqual(std::wstring const &rkID, std::wstring const &rkPW, std::wstring const &rkIP) const
	{
		return (rkID == kID) && (rkPW == kPW) && (rkIP == kAuthIP);
	}

	void WriteToPacket(BM::Stream& rkPacket) const
	{
		rkPacket.Push(kID);
		rkPacket.Push(kPW);
		rkPacket.Push(kAuthIP);
		rkPacket.Push(cGrant);
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(kID);
		rkPacket.Pop(kPW);
		rkPacket.Pop(kAuthIP);
		rkPacket.Pop(cGrant);
	}
} STerminalAuthInfo;
typedef std::map< std::wstring, STerminalAuthInfo > ContTermianlAuthInfo;

typedef enum
{
	EMGRADE_NONE		= 0,

	// Monster
	EMGRADE_NORMAL		= 1,
	EMGRADE_UPGRADED	= 2,
	EMGRADE_ELITE		= 3,
	EMGRADE_BOSS		= 4,
	EMGRADE_MONSTERMAX	= 4,

	// 코어
	EOGRADE_SUBCORE		= 5,// 서브코어(엠포리아 쟁탈전)
	EOGRADE_MAINCORE	= 6,// 메인코어(엠포리아 쟁탈전)

	// 미션
	EMGRADE_MISSION_ELITE_OBJECT = 7, // 미션 정예방(마지막 스테이지) 오브젝트

} EUnitGrade;	// GetAbil(AT_GRADE)

typedef enum ePartyCommand
{
	PC_None = 0,
	PC_Summon_Member = 1,	//파티원들을 소환 주채로 소환 (소환장소)
} EPartyCommand;

typedef enum : unsigned short
{
	// 순서 중요~~
	// value 가 진행 순서에 맞게 되도록 해야 한다.
	ELogin_None = 0,
	ELogin_ReqAuth = 1,
	ELogin_ConnectedLogin = 2,
	ELogin_SendReqImm = 3,
} ELoginState;

typedef struct tagAuthInfo
	:	public SServerTryLogin
{
	static int const iNCgusIDLength = 16;
	static int const iGravityAIdLength = 10;

	tagAuthInfo()
		:	eLoginState(ELogin_None)
		,	ucGender(0)
		,	i64TotalConnSec(0)
		,	iAccDisConnSec(0)
		,	iAccConnSec(0)
		,	byBlock(0)
		,	byGMLevel(0)
		,	bIsPCCafe(false)
		,	byPCCafeGrade(0)
		,	nExtVar(0)
	{
		::memset( gusID, 0, sizeof(gusID) );		
	}

	tagAuthInfo( SServerTryLogin const& rSTL )
		:	SServerTryLogin(rSTL)
		,	eLoginState(ELogin_None)
		,	ucGender(0)
		,	i64TotalConnSec(0)
		,	iAccDisConnSec(0)
		,	iAccConnSec(0)
		,	byBlock(0)
		,	byGMLevel(0)
		,	bIsPCCafe(false)
		,	byPCCafeGrade(0)
		,	nExtVar(0)
	{
		::memset( gusID, 0, sizeof(gusID) );
	}

	BM::vstring ToString() const
	{
		return BM::vstring(L"SAuthInfo[")<<__super::ToString()
			<<L",MemberGuid:"<<kMemberGuid<<L",Gender:"<<static_cast<int>(ucGender)<<L",TotalConn:"<<i64TotalConnSec
			<<L",AccDisConnSec:"<<iAccDisConnSec<<L",AccConnSec:"<<iAccConnSec<<L",Birth:"<<dtUserBirth<<L",LastLogout:"<<dtLastLogout<<L",LastLogin:"<<dtLastLogin
			<<L",NowDB:"<<dtNowDbTime<<L",Block:"<<byBlock<<L",GM-lv:"<<byGMLevel<<L",gusID:"<<CHAR2HEX(gusID, iNCgusIDLength)<<L",nExtVar:"<<nExtVar<<L",CreateCharacterEventRewardKey" << iEventRewardKey << L"]";
	}

	bool ReadFromPacket(BM::Stream& kPacket)
	{
		__super::ReadFromPacket(kPacket);
		kPacket.Pop(guidQuery);
		kPacket.Pop(eLoginState);
		kPacket.Pop(kMemberGuid);
		kPacket.Pop(ucGender);
		kPacket.Pop(i64TotalConnSec);
		kPacket.Pop(iAccDisConnSec);
		kPacket.Pop(iAccConnSec);
		kPacket.Pop(dtUserBirth);
		kPacket.Pop(dtLastLogout);
		kPacket.Pop(dtLastLogin);
		kPacket.Pop(dtNowDbTime);
		kPacket.Pop(byBlock);
		kPacket.Pop(byGMLevel);
		kPacket.Pop(bIsPCCafe);
		kPacket.Pop(byPCCafeGrade);
		kPacket.PopMemory(gusID, sizeof(gusID));
		return kPacket.Pop(nExtVar);
	}

	void WriteToPacket(BM::Stream& kPacket) const
	{
		__super::WriteToPacket(kPacket);
		kPacket.Push(guidQuery);
		kPacket.Push(eLoginState);
		kPacket.Push(kMemberGuid);
		kPacket.Push(ucGender);
		kPacket.Push(i64TotalConnSec);
		kPacket.Push(iAccDisConnSec);
		kPacket.Push(iAccConnSec);
		kPacket.Push(dtUserBirth);
		kPacket.Push(dtLastLogout);
		kPacket.Push(dtLastLogin);
		kPacket.Push(dtNowDbTime);
		kPacket.Push(byBlock);
		kPacket.Push(byGMLevel);
		kPacket.Push(bIsPCCafe);
		kPacket.Push(byPCCafeGrade);
		kPacket.Push(gusID, sizeof(gusID));
		kPacket.Push(nExtVar);
	}

	BM::GUID guidQuery;//쿼리의 guid
	ELoginState eLoginState;

	// Additional Login information (ID/PW 인증 이후에 값이 들어가게 된다.)
	// Member DB에 있는 값들
	BM::GUID kMemberGuid;
	unsigned char ucGender;				// 성별
	__int64 i64TotalConnSec;
	int iAccDisConnSec;//누적된 비접속 시간
	int iAccConnSec;//누적된 비접속 시간
	BM::DBTIMESTAMP_EX dtUserBirth;	// 생년월일
	BM::DBTIMESTAMP_EX dtLastLogout;
	BM::DBTIMESTAMP_EX dtLastLogin;
	BM::DBTIMESTAMP_EX dtNowDbTime;
	BYTE byBlock;
	BYTE byGMLevel;
	bool bIsPCCafe; // 가맹PC방 접속?
	BYTE byPCCafeGrade;

	// NC용
	BYTE gusID[iNCgusIDLength];

	// GF용
	short nExtVar;
}SAuthInfo;

typedef struct tagReqSwitchReserveMember//서버간에 주고받음
:	public SServerTryLogin//다른건 더 상속 받으면 곤란.
{
	tagReqSwitchReserveMember()
		:	byGMLevel(0)
		,	ucGender(0)
		,	i64TotalConnSec(0)
		,	iAccConnSec(0)
		,	iAccDisConnSec(0)
		,	nExtVar(0)
	{
	}

	tagReqSwitchReserveMember( SAuthInfo const &rhs )
		: SServerTryLogin((SServerTryLogin)rhs)
		, nExtVar(rhs.nExtVar)
	{
		guidMember = rhs.kMemberGuid;			//유저의 MemberDB Key
		ucGender = rhs.ucGender;
		i64TotalConnSec = rhs.i64TotalConnSec;
		iAccDisConnSec = rhs.iAccDisConnSec;//누적된 비접속 시간
		iAccConnSec = rhs.iAccConnSec;//누적된 비접속 시간
//		dtLastLogin = rhs.dtLastLogin;
		dtUserBirth = rhs.dtUserBirth;
		dtLastLogout = rhs.dtLastLogout;
		dtNowDbTime = rhs.dtNowDbTime;
		byBlock = rhs.byBlock;
		byGMLevel = rhs.byGMLevel;
		bIsPCCafe = rhs.bIsPCCafe;
		byPCCafeGrade = rhs.byPCCafeGrade;
		::memcpy( gusID, rhs.gusID, sizeof(gusID) );
		dtLastLogin = rhs.dtNowDbTime;//로그인 시간 갱신!
		usAge = 0;
		iUID = rhs.iUID;
	}

	void CopyTo( SAuthInfo &kAuthInfo )const
	{
		kAuthInfo.SServerTryLogin::operator = ( (SServerTryLogin)*this );
		kAuthInfo.kMemberGuid = guidMember;
		kAuthInfo.ucGender = ucGender;
		kAuthInfo.i64TotalConnSec = i64TotalConnSec;
		kAuthInfo.iAccDisConnSec = iAccDisConnSec;//누적된 비접속 시간
		kAuthInfo.iAccConnSec = iAccConnSec;//누적된 비접속 시간
		kAuthInfo.dtUserBirth = dtUserBirth;
		kAuthInfo.dtLastLogout = dtLastLogout;
		kAuthInfo.dtNowDbTime = dtNowDbTime;
		kAuthInfo.byBlock = byBlock;
		kAuthInfo.byGMLevel = byGMLevel;
		kAuthInfo.bIsPCCafe = bIsPCCafe;
		kAuthInfo.byPCCafeGrade = byPCCafeGrade;
		::memcpy( kAuthInfo.gusID, gusID, sizeof(gusID) );
		kAuthInfo.dtNowDbTime = dtLastLogin;//로그인 시간 갱신!
		kAuthInfo.nExtVar = nExtVar;
		kAuthInfo.iUID = iUID;
	}

	void Clear()
	{
		__super::Clear();

		guidMember.Clear();
		guidOrderKey.Clear();
		ucGender = 0;
		usAge = 0;

		i64TotalConnSec = 0;
		iAccDisConnSec = 0;
		iAccConnSec = 0;

		dtUserBirth.Clear();
		dtLastLogin.Clear();
		dtLastLogout.Clear();
		dtNowDbTime.Clear();

		byBlock = byGMLevel = 0;
		bIsPCCafe = false;
		byPCCafeGrade = 0;
		kSwitchServer.Clear();

		gusID[0] = 0;
		nExtVar = 0;
	}

	bool ReadFromPacket(BM::Stream& kPacket)
	{
		__super::ReadFromPacket(kPacket);
		kPacket.Pop(guidMember);
		kPacket.Pop(guidOrderKey);
		kPacket.Pop(ucGender);
		kPacket.Pop(usAge);
		kPacket.Pop(i64TotalConnSec);
		kPacket.Pop(iAccDisConnSec);
		kPacket.Pop(iAccConnSec);
		kPacket.Pop(dtUserBirth);
		kPacket.Pop(dtLastLogin);
		kPacket.Pop(dtLastLogout);
		kPacket.Pop(dtNowDbTime);
		kPacket.Pop(byBlock);
		kPacket.Pop(byGMLevel);
		kPacket.Pop(bIsPCCafe);
		kPacket.Pop(byPCCafeGrade);
		kSwitchServer.ReadFromPacket(kPacket);
		return kPacket.PopMemory(gusID, sizeof(gusID));
	}

	void WriteToPacket(BM::Stream& kPacket) const
	{
		__super::WriteToPacket(kPacket);
		kPacket.Push(guidMember);
		kPacket.Push(guidOrderKey);
		kPacket.Push(ucGender);
		kPacket.Push(usAge);
		kPacket.Push(i64TotalConnSec);
		kPacket.Push(iAccDisConnSec);
		kPacket.Push(iAccConnSec);
		kPacket.Push(dtUserBirth);
		kPacket.Push(dtLastLogin);
		kPacket.Push(dtLastLogout);
		kPacket.Push(dtNowDbTime);
		kPacket.Push(byBlock);
		kPacket.Push(byGMLevel);
		kPacket.Push(bIsPCCafe);
		kPacket.Push(byPCCafeGrade);
		kSwitchServer.WriteToPacket(kPacket);
		kPacket.Push(gusID,sizeof(gusID));
	}

	size_t min_size() const
	{
		return __super::min_size() 
			+ sizeof(BM::GUID) * 2
			+ sizeof(unsigned char)
			+ sizeof(unsigned short)
			+ sizeof(__int64)
			+ sizeof(int) * 2
			+ sizeof(BM::DBTIMESTAMP_EX) * 4
			+ sizeof(BYTE) * 2
			+ sizeof(SERVER_IDENTITY)
			+ sizeof(gusID)
			+ sizeof(short)
			+ sizeof(bool);
	}

	BM::GUID guidMember;		//유저의 MemberDB Key
	BM::GUID guidOrderKey;		//스위치에 전달한 해당 유저의 인증키
	unsigned char	ucGender;		//최종적으로 m_kContLoginedUserKey 에 전달된다
	unsigned short	usAge;

	__int64 i64TotalConnSec;
	int iAccDisConnSec;	//누적된 비접속 시간		//접속이 이루어지면 갱신됨
	int iAccConnSec;		//누적된 접속 시간
	BM::DBTIMESTAMP_EX dtUserBirth;
	BM::DBTIMESTAMP_EX dtLastLogin;					//접속이 이루어지면 갱신됨
	BM::DBTIMESTAMP_EX dtLastLogout;
	BM::DBTIMESTAMP_EX dtNowDbTime;
	BYTE byBlock;		// Blocking 당했는가 (MEMBER_BLOCK_MARK_MIN 이상이면 Block 된것)
	BYTE byGMLevel;		// GMLevel
	bool bIsPCCafe;
	BYTE byPCCafeGrade;
	SERVER_IDENTITY kSwitchServer;

	BYTE gusID[16];//NC만 쓰는값.

	short nExtVar;//GF
}SReqSwitchReserveMember;

typedef struct tagSwitchPlayerData//스위치에 할당된 유저 정보
:	public tagReqSwitchReserveMember//다른건 더 상속 받으면 곤란.
{
	tagSwitchPlayerData()
	{
		Clear();
	}

	tagSwitchPlayerData( tagReqSwitchReserveMember const &rhs )
		: tagReqSwitchReserveMember(rhs)
	{
		dwTimeLimit = 0;
	}

	void Clear()
	{
		tagReqSwitchReserveMember::Clear();
		dwTimeLimit = 0;
	}

	bool ReadFromPacket(BM::Stream& kPacket)
	{
		__super::ReadFromPacket(kPacket);
		return kPacket.Pop(dwTimeLimit);
	}
	void WriteToPacket(BM::Stream& kPacket) const
	{
		__super::WriteToPacket(kPacket);
		return kPacket.Push(dwTimeLimit);
	}

	size_t min_size() const
	{
		return __super::min_size() + sizeof(DWORD);
	}

	DWORD			dwTimeLimit;		//센터에서 받은 오더의 유효 시간.
}SSwitchPlayerData;
typedef std::vector<SSwitchPlayerData> VEC_SwitchPlayerData;

typedef enum eMissionQuestCommand
{
	MQC_None			= 0,
	MQC_Nfy_CardItem	= 1,
	MQC_Req_SelectCard,
	MQC_Ans_SelectCard,
	MQC_Ans_StartQuest,
	MQC_Ans_QuestResult,
} EMissionQuestCommand;

typedef enum eMissionQuestCommandResult
{
	MQCR_Failed			= 0,
	MQCR_Success		= 1,
	MQCR_NoQuest,
} EMissionQuestCommandResult;

int const MAX_MAIL_TITLE_LEN = 25;
int const MAX_MAIL_TEXT_LEN = 200;
int const MAX_MAIL_COUNT = 100;

typedef enum ePostMailSendResult : BYTE
{
	PMSR_SUCCESS			= 0,
	PMSR_PAYMENT_NEED_ITEM	= 1,	// 요금 청구 메일은 아이템이 꼭 있어야 한다.
	PMSR_NOT_ENOUGH_MONEY	= 2,	// 잔액이 부족하다.
	PMSR_NOT_FOUND_ANNEX	= 3,	// 첨부 물품을 찾을수 없다.
	PMSR_SENDER_MAIL_FULL	= 4,	// 메일의 전송 가능 용량이 모두 찼다.
	PMSR_NET_ERROR			= 5,	// 네트워크 전송 에러(이런 경우 발생하나?)
	PMSR_DB_ERROR			= 6,	// 아이템 서버에서 에러 발생시 모두 디비 에러로 간주한다.
	PMSR_TITLE_TOO_LONG		= 7,	// 메일 타이틀이 너무 길다.
	PMSR_TEXT_TOO_LONG		= 8,	// 메일 내용이 너무 길다.
	PMSR_NOT_FOUND_RECVER	= 9,	// 수신자가 없다.
	PMSR_RECVER_MAIL_FULL	= 10,	// 메일의 전송 가능 용량이 모두 찼다.
	PMSR_CANT_SEND_TO_SELF	= 11,	// 자기 자신한테는 보낼수 없다.
	PMSR_BADWORD_SUBJECT	= 12,	// 올바르지 못한 제목
	PMSR_BADWORD_CONTENTS	= 13,	// 올바르지 못한 내용
	PMSR_CANT_SEND_ITEM		= 19,	// 메일로 전송 할 수 없은 아이템이다.
	PMSR_INVENTORY_FULL		= 24,	// 인벤토리가 가득 찼습니다.
}EPostMailSendResult;

typedef enum ePostMailModifyType : BYTE
{
	PMMT_NOT_READ	= 0x00,
	PMMT_READ		= 0x01,
	PMMT_DELETE		= 0x02,
	PMMT_RETURN		= 0x04,
	PMMT_GET_ANNEX	= 0x08,
	PMMT_PAYMENT	= 0x10,
}EPostMailModifyType;

typedef enum ePostMailRecvResult : BYTE		// PT_C_M_POST_REQ_MAIL_RECV 반환값
{
	PMRR_SUCCESS = 0,
	PMRR_DB_ERROR,						// 디비 쿼리 에러??
	PMRR_ALREADY_RECVED,				// 이미 받았다?
}EPostMailRecvResult;

typedef enum ePostMailModifyResult : BYTE
{
	PMMR_SUCCESS		= 0,	// 변경 성공
	PMMR_ALREADY_READ	= 1,	// 이미 읽은 메일
	PMMR_HAVE_ANNEX		= 2,	// 첨부가 있다.
	PMMR_NOT_HAVE_ANNEX	= 3,	// 텍스트 메일이다.(반송을 할 수 없음)
	PMMR_IS_RETURNED	= 4,	// 이미 반송된 메일이다.(이경우 반송 되어온 녀석을 다시 반송 할 수도 없다는 의미임)
	PMMR_NEED_PAYMENT	= 5,	// 요금을 납부 해야 한다.
	PMMR_DB_ERROR		= 6,	// 디비 에러
	PMMR_NOT_READ		= 7,	// 아직 읽지 않았다.
	PMMR_IS_NOT_PAYMENT = 8,	// 요금 청구 메일이 아니다.
	PMMR_NOT_FOUND_MAIL	= 9,	// 없는 메일이다.
	PMMR_CANT_RETURN	= 10,	// 반송 불가능한 메일이다.
}EPostMailModifyResult;

// Couple

typedef enum eMarryState
{
	EM_NONE				= 0,
	EM_MARRY,
	EM_TALK,
	EM_ACTION,
	EM_MONSTER,
	EM_ACTION_END,
	EM_MARRY_ACTION,
	EM_NPCTALK,

	EM_NOTICE,

	EM_MAX,
} EMarryState;

typedef enum eCoupleState
{
	CoupleS_None		= 0x00,
	CoupleS_Normal		= 0x01,
	CoupleS_Breaked		= 0x02,	
	CoupleS_SweetHeart	= 0x04,	
} ECoupleState;

typedef enum eCoupleSaveInfo
{
	CoupleSI_None		= 0x00,
	CoupleSI_StartTime	= 0x01,
	CoupleSI_LimitTime	= 0x02,
} ECoupleSaveInfo;

typedef struct tagCouple
{
	tagCouple()
	{
		Clear();
		m_kCouplePanaltyLimitDate.Clear();
	}
	
	void Clear()
	{
		m_kCoupleGuid.Clear();
		m_kCharName.clear();
		m_kStartDate.Clear();
		m_kDDay = 0;
		m_kLimitDate.Clear();		
		m_kAccepted = false;
		m_kBreaked = false;
		LinkStatus(FLS_OFFLINE);
		m_kGroundNo = 0;
		m_kQuest = 0;
		m_kQuestID = 0;
		m_kSweetHeartQuestTime = 0;
		//m_kCouplePanaltyLimitDate.Clear();
		m_kSweetHeartQuestTimeGap = 0;
		m_kSweetHeartQuestUpdate = false;
		m_kcStatus = 0;
		m_kLearnSkill = 0;
		m_kColorGuid.Clear();
	}

	void ReadFromPacket(BM::Stream &rkPacket)
	{
		rkPacket.Pop( m_kCoupleGuid );
		rkPacket.Pop( m_kCharName );
		rkPacket.Pop( m_kStartDate );
		rkPacket.Pop( m_kDDay );
		rkPacket.Pop( m_kLimitDate );		
		rkPacket.Pop( m_kAccepted );
		rkPacket.Pop( m_kBreaked );
		rkPacket.Pop( m_kQuest );
		rkPacket.Pop( m_kQuestID );
		rkPacket.Pop( m_kSweetHeartQuestTime );
		rkPacket.Pop( m_kCouplePanaltyLimitDate );
		rkPacket.Pop( m_kSweetHeartQuestTimeGap );
		rkPacket.Pop( m_kSweetHeartQuestUpdate );
		rkPacket.Pop( m_kcStatus );
		rkPacket.Pop( m_kLearnSkill );
		rkPacket.Pop( m_kColorGuid );
		rkPacket.Pop( m_kHomeAddr );
	}

	void WriteToPacket(BM::Stream &rkPacket)
	{
		rkPacket.Push( m_kCoupleGuid );
		rkPacket.Push( m_kCharName );
		rkPacket.Push( m_kStartDate );
		rkPacket.Push( m_kDDay );
		rkPacket.Push( m_kLimitDate );		
		rkPacket.Push( m_kAccepted );
		rkPacket.Push( m_kBreaked );
		rkPacket.Push( m_kQuest );
		rkPacket.Push( m_kQuestID );
		rkPacket.Push( m_kSweetHeartQuestTime );
		rkPacket.Push( m_kCouplePanaltyLimitDate );
		rkPacket.Push( m_kSweetHeartQuestTimeGap );
		rkPacket.Push( m_kSweetHeartQuestUpdate );
		rkPacket.Push( m_kcStatus );
		rkPacket.Push( m_kLearnSkill );
		rkPacket.Push( m_kColorGuid );
		rkPacket.Push( m_kHomeAddr );
	}

	void SetPanaltyLimitDate(BM::PgPackedTime &kCouplePanaltyLimitDate)
	{
		m_kCouplePanaltyLimitDate = kCouplePanaltyLimitDate;
	}

	void ReadFromDBResult(CEL::DB_DATA_ARRAY::const_iterator &result_iter)
	{
		int iQuest = 0;
		__int64 iSweetHeartQuestTime = 0;
		DBTIMESTAMP kCouplePanaltyLimitDate;
		ZeroMemory(&kCouplePanaltyLimitDate, sizeof(DBTIMESTAMP));

		int iLearnSkill = 0;
		BYTE cStatus = 0;
		DBTIMESTAMP kStartDate, kLimitDate;
		ZeroMemory(&kStartDate, sizeof(DBTIMESTAMP));
		ZeroMemory(&kLimitDate, sizeof(DBTIMESTAMP));
		result_iter->Pop( m_kCoupleGuid );	++result_iter;
		result_iter->Pop( m_kCharName );	++result_iter;
		result_iter->Pop( kStartDate );		++result_iter;
		result_iter->Pop( cStatus );		++result_iter;
		result_iter->Pop( m_kDDay );		++result_iter;
		result_iter->Pop( kLimitDate );		++result_iter;
		result_iter->Pop( iQuest );			++result_iter;
		result_iter->Pop( iSweetHeartQuestTime );		++result_iter;
		result_iter->Pop( iLearnSkill );				++result_iter;
		result_iter->Pop( kCouplePanaltyLimitDate );	++result_iter;
		result_iter->Pop( m_kColorGuid );				++result_iter;

		short sStreetNo;
		int iHouseNo;

		result_iter->Pop( sStreetNo );		++result_iter;
		result_iter->Pop( iHouseNo );		++result_iter;

		HomeAddr(SHOMEADDR(sStreetNo, iHouseNo));

		if( BM::GUID::IsNotNull(m_kCoupleGuid) )	{ m_kAccepted = true; }
		if( 0 != (cStatus & CoupleS_Breaked) )		{ m_kBreaked = true; }
		if( kStartDate.day )						{ m_kStartDate = kStartDate; }
		if( kLimitDate.day )						{ m_kLimitDate = kLimitDate; }
		if( iQuest )								{ m_kQuest = iQuest; }
		if( iSweetHeartQuestTime )					{ m_kSweetHeartQuestTime = iSweetHeartQuestTime; }
		if( iLearnSkill )							{ m_kLearnSkill = iLearnSkill; }
		if( kCouplePanaltyLimitDate.day )			{ m_kCouplePanaltyLimitDate = kCouplePanaltyLimitDate; }
		if( cStatus )								{ m_kcStatus = cStatus; }
	}

protected:
	CLASS_DECLARATION_S_NO_SET(BM::GUID, CoupleGuid);
	CLASS_DECLARATION_S_NO_SET(std::wstring, CharName);
	CLASS_DECLARATION_S_NO_SET(BM::PgPackedTime, StartDate);
	CLASS_DECLARATION_S_NO_SET(int, DDay );
	CLASS_DECLARATION_S_NO_SET(BM::PgPackedTime, LimitDate);	
	CLASS_DECLARATION_S_NO_SET(bool, Accepted);
	CLASS_DECLARATION_S_NO_SET(bool, Breaked);
	CLASS_DECLARATION_S(BYTE, LinkStatus);
	CLASS_DECLARATION_S(int, GroundNo);
	CLASS_DECLARATION_S(int, Quest);
	CLASS_DECLARATION_S(int, QuestID);
	CLASS_DECLARATION_S(__int64, SweetHeartQuestTime);
	CLASS_DECLARATION_S(int, LearnSkill);
	CLASS_DECLARATION_S(BM::PgPackedTime, CouplePanaltyLimitDate);
	CLASS_DECLARATION_S(__int64, SweetHeartQuestTimeGap);
	CLASS_DECLARATION_S(bool, SweetHeartQuestUpdate);
	CLASS_DECLARATION_S(BYTE, cStatus);
	CLASS_DECLARATION_S(BM::GUID, ColorGuid);
	CLASS_DECLARATION_S(SHOMEADDR, HomeAddr);
} SCouple;

typedef enum
{
	CP_DeleteQuest				= 0,
	CP_SweetHeartQuestTime		= 1,
	CP_CouplePanaltyLimitDate	= 2,
	CP_Status					= 3,
	CP_LearnSkill				= 4,
	CP_ColorGuid				= 5,
}ECoupleDBType;

typedef enum eCoupleCommand
{
	CC_None = 0,
	CC_Auto_Match,
	CC_Req_Find,
	CC_Req_Couple,
	CC_Req_CoupleInstance,
	CC_Ans_Couple,
	CC_Req_Break,
	CC_Req_Info,
	CC_Req_Warp,
	CC_Req_Warp1,
	CC_Req_Warp2,
	CC_Nfy_TimeLimit,
	CC_Nfy_AddSkill,
	CC_Sys_Save,
	CC_Status,
	CC_LearnSkill,
	CC_Ans_Reject,
	CC_Ans_Notify,
	CC_Req_SweetHeart_Quest,
	CC_Ans_SweetHeart_Quest,
	CC_SweetHeartQuest_Cancel,
	CC_SweetHeartQuest_Complete,
	CC_SweetHeartQuestTimeInfo,
	CC_Req_SweetHeart_Complete,
	CC_Ans_SweetHeart_Complete,
	CC_CoupleSkill,
	CC_CoupleColorGuid,
	CC_NfyStatus,
	CC_SendGndCoupleStatus,
	CC_CoupleInit,
} ECoupleCommand;

typedef enum eMarryCommand
{
	MC_None = 0,
	MC_Init,
	MC_ReqMarry,
	MC_AnsMarry,
	MC_ReqSendMoney,
	MC_AnsSendMoney,
	MC_SetActionState,
	MC_ActionPlay,
	MC_ActionPlayEnd,
	MC_TotalMoney,
	MC_EffectCheck,
	MC_NpcTalk,
	MC_End,
	MC_TimeOut,
} EMarryCommand;

#define COUPLE_WARP_ITEM_NO (50400010)

typedef enum eCoupleCommandResult
{
	CoupleCR_None = 0,
	CoupleCR_Success,
	CoupleCR_Notify,
	CoupleCR_Pre,
	CoupleCR_NotSingle,
	CoupleCR_Failed,
	CoupleCR_NotFind,
	CoupleCR_Reject,
	CoupleCR_NotGender,
	CoupleCR_Same,
	CoupleCR_NotChannel,
	CoupleCR_ReqWait,
	CoupleCR_NotMapMove,
	CoupleCR_QuestComplete,
	CoupleCR_SweetHeart_Panalty,
} ECoupleCommandResult;

typedef enum eUserMarketResult : BYTE
{
	UMR_SUCCESS					= 0,
	UMR_DB_ERROR				= 1,
	UMR_NEED_COST				= 2,
	UMR_NOT_FOUND_ITEM			= 3,
	UMR_INVALID_ITEM			= 4,
	UMR_MARKETITEM_FULL			= 5,
	UMR_INVALID_REGTIME			= 6,
	UMR_NET_ERROR				= 7,
	UMR_NOT_FOUND_ARTICLE		= 8,
	UMR_DEALING_FULL			= 9,
	UMR_DEALING_ALREADY_READ	= 10,
	UMR_ALREADY_OPENED			= 11,
	UMR_NOT_FOUND_MARKET		= 12,
	UMR_NOT_FOUND_DEALING		= 13,
	UMR_NEED_EDIT_MODE			= 14,
	UMR_MARKET_CLOSED			= 15,
	UMR_ZERO_COUNT				= 16,
	UMR_OVER_COUNT				= 17,
	UMR_MY_ARTICLES				= 18,
	UMR_NOT_USED				= 19,
	UMR_INVALID_CHAR			= 20, // 잘못된 캐릭 정보임
	UMR_NOT_ENOUGH_CASH			= 21, // 캐쉬가 부족함
	UMR_INVENTORY_FULL			= 30, // 인벤토리 가득참
	UMR_TOO_MANY_COST_VALUE		= 31, // 너무 높은 가격?
	UMR_MARKET_NOT_EMPTY		= 32, // 마켓이 비워지지 않았다.
	UMR_USE_BADWORD				= 33, // 금칙어는 사용 할수 없음
	UMR_NOT_SUPPORT				= 34, // 지원하지 않는 기능
	UMR_SYSTEM_ERROR			= 35, // system error
	UMR_DEALING_ING				= 36, // Dealing Packet Ing
	UMR_DEALING_END				= 37, // Dealing Packet End
	UMR_MAX,
}EUserMarketResult;

typedef struct tagUserMarketDealingInfo
{
	tagUserMarketDealingInfo()
	{
		dwItemNo = 0;
		wItemNum = 0;
		i64Enchant_01 = 0;
		i64Enchant_02 = 0;
		i64Enchant_03 = 0;
		i64Enchant_04 = 0;
		i64SellCost = 0;
		cCostType = 0;
		cItemState = 0;
	}

	BM::GUID			kDealingGuid;
	std::wstring		kDealerName;
	DWORD				dwItemNo;
	WORD				wItemNum;
	__int64				i64Enchant_01;
	__int64				i64Enchant_02;
	__int64				i64Enchant_03;
	__int64				i64Enchant_04;
	__int64				i64SellCost;
	char				cCostType;
	BM::DBTIMESTAMP_EX	kItemGenTime;
	char				cItemState;
	int					iCostRate;

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(kDealingGuid);
		kPacket.Push(kDealerName);
		kPacket.Push(dwItemNo);
		kPacket.Push(wItemNum);
		kPacket.Push(i64Enchant_01);
		kPacket.Push(i64Enchant_02);
		kPacket.Push(i64Enchant_03);
		kPacket.Push(i64Enchant_04);
		kPacket.Push(i64SellCost);
		kPacket.Push(cCostType);
		kPacket.Push(cItemState);
		kPacket.Push(kItemGenTime);
		kPacket.Push(iCostRate);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kDealingGuid);
		kPacket.Pop(kDealerName);
		kPacket.Pop(dwItemNo);
		kPacket.Pop(wItemNum);
		kPacket.Pop(i64Enchant_01);
		kPacket.Pop(i64Enchant_02);
		kPacket.Pop(i64Enchant_03);
		kPacket.Pop(i64Enchant_04);
		kPacket.Pop(i64SellCost);
		kPacket.Pop(cCostType);
		kPacket.Pop(cItemState);
		kPacket.Pop(kItemGenTime);
		kPacket.Pop(iCostRate);
	}

}SUserMarketDealingInfo;

typedef std::list<SUserMarketDealingInfo> CONT_USER_MARKET_DEALINGINFO_LIST;

typedef struct tagUserMarketDealingList
{
	tagUserMarketDealingList()
	{
	}

	CONT_USER_MARKET_DEALINGINFO_LIST kCont;
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(kCont.size());
		CONT_USER_MARKET_DEALINGINFO_LIST::iterator itor = kCont.begin();
		while(itor != kCont.end())
		{
			(*itor).WriteToPacket(kPacket); ++itor;
		}
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		int iSize = 0;
		kPacket.Pop(iSize);

		SUserMarketDealingInfo kDealing;

		for(int i = 0;i < iSize;i++)
		{
			kDealing.ReadFromPacket(kPacket);	
			kCont.push_back(kDealing);
		}
	}
}SUserMarketDealingList;


typedef struct tagSendMailInfo
{
	tagSendMailInfo()
	{
		Money(0);
		PaymentType(false);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(m_kFromGuid);
		kPacket.Pop(m_kTargetGuid);
		kPacket.Pop(m_kFromName);
		kPacket.Pop(m_kTargetName);
		kPacket.Pop(m_kMailTitle);
		kPacket.Pop(m_kMailText);
		kPacket.Pop(m_kMoney);
		kPacket.Pop(m_kPaymentType);
		kPacket.Pop(m_kItemGuid);
		kPacket.Pop(m_kItemNo);
		kPacket.Pop(m_kItemNum);
	}
	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(m_kFromGuid);
		kPacket.Push(m_kTargetGuid);
		kPacket.Push(m_kFromName);
		kPacket.Push(m_kTargetName);
		kPacket.Push(m_kMailTitle);
		kPacket.Push(m_kMailText);
		kPacket.Push(m_kMoney);
		kPacket.Push(m_kPaymentType);
		kPacket.Push(m_kItemGuid);
		kPacket.Push(m_kItemNo);
		kPacket.Push(m_kItemNum);
	}

	CLASS_DECLARATION_S(BM::GUID,FromGuid);
	CLASS_DECLARATION_S(BM::GUID,TargetGuid);
	CLASS_DECLARATION_S(std::wstring,FromName);
	CLASS_DECLARATION_S(std::wstring,TargetName);
	CLASS_DECLARATION_S(std::wstring,MailTitle);
	CLASS_DECLARATION_S(std::wstring,MailText);
	CLASS_DECLARATION_S(__int64, Money);
	CLASS_DECLARATION_S(bool, PaymentType);
	CLASS_DECLARATION_S(BM::GUID,ItemGuid);
	CLASS_DECLARATION_S(int,ItemNo);
	CLASS_DECLARATION_S(short,ItemNum);
}SSendMailInfo;

int const SEND_MAIL_COST = 30;					// 메일 전송시 수수료
double const USERMARKET_COST_RATE = 0.01;		// 마켓 등록 물품 판매 수수료 비율

//=======================================================================================================
//	우편 관련 프로토콜 선언
//=======================================================================================================

typedef struct tagPT_C_M_POST_REQ_MAIL_MODIFY
{
	tagPT_C_M_POST_REQ_MAIL_MODIFY(BM::GUID const& rkNpcGuid)
		: m_kNpcGuid(rkNpcGuid)
	{
		m_kMailModifyType = PMMT_NOT_READ;
	}

	CLASS_DECLARATION_S(BM::GUID,NpcGuid);
	CLASS_DECLARATION_S(BM::GUID,MailGuid);
	CLASS_DECLARATION_S(EPostMailModifyType,MailModifyType);
	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_C_M_POST_REQ_MAIL_MODIFY);
		kPacket.Push(m_kNpcGuid);
		kPacket.Push(m_kMailGuid);
		kPacket.Push(m_kMailModifyType);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kNpcGuid);
		kPacket.Pop(m_kMailGuid);
		kPacket.Pop(m_kMailModifyType);
	}
}SPT_C_M_POST_REQ_MAIL_MODIFY;

typedef struct tagPT_C_M_POST_REQ_MAIL_RECV
{
	tagPT_C_M_POST_REQ_MAIL_RECV()
	{
		MailIndex(0);
		ReqCount(0);
	}
	CLASS_DECLARATION_S(__int64,MailIndex);
	CLASS_DECLARATION_S(char,ReqCount);

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_C_M_POST_REQ_MAIL_RECV);
		kPacket.Push(m_kMailIndex);
		kPacket.Push(m_kReqCount);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kMailIndex);
		kPacket.Pop(m_kReqCount);
	}
}SPT_C_M_POST_REQ_MAIL_RECV;

typedef struct tagPT_C_M_POST_REQ_MAIL_SEND
{
	tagPT_C_M_POST_REQ_MAIL_SEND(BM::GUID const& rkNpcGuid)
		: m_kNpcGuid(rkNpcGuid)
	{
		Money(0);
		PayMent(false);
	}

	CLASS_DECLARATION_S(BM::GUID, NpcGuid);
	CLASS_DECLARATION_S(std::wstring,ToName);
	CLASS_DECLARATION_S(std::wstring,Title);
	CLASS_DECLARATION_S(std::wstring,Text);
	CLASS_DECLARATION_S(SItemPos,Pos);
	CLASS_DECLARATION_S(__int64,Money);
	CLASS_DECLARATION_S(bool,PayMent);

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_C_M_POST_REQ_MAIL_SEND);
		kPacket.Push(m_kNpcGuid);
		kPacket.Push(m_kToName);
		kPacket.Push(m_kTitle);
		kPacket.Push(m_kText);
		kPacket.Push(m_kPos);
		kPacket.Push(m_kMoney);
		kPacket.Push(m_kPayMent);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kNpcGuid);
		kPacket.Pop(m_kToName);
		kPacket.Pop(m_kTitle);
		kPacket.Pop(m_kText);
		kPacket.Pop(m_kPos);
		kPacket.Pop(m_kMoney);
		kPacket.Pop(m_kPayMent);
	}
}SPT_C_M_POST_REQ_MAIL_SEND;

typedef struct tagPT_C_M_POST_REQ_MAIL_MIN
{
	tagPT_C_M_POST_REQ_MAIL_MIN()
	{
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_C_M_POST_REQ_MAIL_MIN);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
	}
}SPT_C_M_POST_REQ_MAIL_MIN;

typedef struct tagPT_M_C_POST_ANS_MAIL_MODIFY
{
	tagPT_M_C_POST_ANS_MAIL_MODIFY()
	{
		Result(PMMR_SUCCESS);
		MailIndex(0);
		ModifyType(PMMT_NOT_READ);
	}

	CLASS_DECLARATION_S(EPostMailModifyResult,Result);
	CLASS_DECLARATION_S(__int64,MailIndex);
	CLASS_DECLARATION_S(EPostMailModifyType,ModifyType);
	CLASS_DECLARATION_S(BM::DBTIMESTAMP_EX,LimitTime);
	
	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_M_C_POST_ANS_MAIL_MODIFY);
		kPacket.Push(m_kResult);
		kPacket.Push(m_kMailIndex);
		kPacket.Push(m_kModifyType);
		kPacket.Push(m_kLimitTime);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
		if(m_kResult)
			return;
		kPacket.Pop(m_kMailIndex);
		kPacket.Pop(m_kModifyType);
		kPacket.Pop(m_kLimitTime);
	}
}SPT_M_C_POST_ANS_MAIL_MODIFY;


typedef struct tagPT_M_C_POST_ANS_MAIL_SEND
{
	tagPT_M_C_POST_ANS_MAIL_SEND()
	{
		Result(PMSR_SUCCESS);
	}

	CLASS_DECLARATION_S(EPostMailSendResult,Result);

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_M_C_POST_ANS_MAIL_SEND);
		kPacket.Push(m_kResult);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
	}
}SPT_M_C_POST_ANS_MAIL_SEND;

typedef struct tagPT_M_C_POST_ANS_MAIL_MIN
{
	tagPT_M_C_POST_ANS_MAIL_MIN()
	{
		MinIndex(0);
	}

	CLASS_DECLARATION_S(__int64, MinIndex);
	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_M_C_POST_ANS_MAIL_MIN);
		kPacket.Push(m_kMinIndex);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kMinIndex);
	}
}SPT_M_C_POST_ANS_MAIL_MIN;

//=======================================================================================================
//	경매 관련 프로토콜 선언
//=======================================================================================================

typedef enum E_USER_MARKET_ARTICLE_TYPE : BYTE
{
	UMAT_ITEM = 0,
	UMAT_CASH = 1,
	UMAT_MONEY = 2,
}eUserMarketArticleType;

typedef struct tagARTICLEINFO
{
	CLASS_DECLARATION_S(SItemPos, Pos);
	CLASS_DECLARATION_S(bool, CostType);
	CLASS_DECLARATION_S(__int64, ArticleCost);
	CLASS_DECLARATION_S(eUserMarketArticleType, ArticleType);
	CLASS_DECLARATION_S(__int64, SellMoney);
}SARTICLEINFO;

typedef std::list<SARTICLEINFO> CONT_ARTICLEINFO;

typedef enum E_MARKET_GRADE
{
	MG_START = 0,
	MG_NORMAL = 0,
	MG_GOOD = 1,
	MG_HISTORY = 2,
	MG_END,
}EMarketGrade;

typedef enum E_MARKET_STATE
{
	MS_CLOSE = 0,	// 닫겼음(의미 없음 닫기면 삭제됨)
	MS_OPEN = 1,	// 열렸음(현재 장사중임)
	MS_EDIT = 2,	// 상점 목록 수정중(이때는 판매/검색 불가)
}EMarketState;

//오픈마켓 오픈(등록)
typedef struct tagPT_C_M_UM_REQ_MARKET_OPEN
{
	CLASS_DECLARATION_S(std::wstring,MarketName);			// 상점 이름 입력 하지 않으면 캐릭터 이름으로 대체된다.
	CLASS_DECLARATION_S(SItemPos,MarketItemPos);				// 상점 오픈 아이템 없으면? (0,0)

	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_UM_REQ_MARKET_OPEN);
		kPacket.Push(m_kMarketName);
		kPacket.Push(m_kMarketItemPos);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kMarketName);
		kPacket.Pop(m_kMarketItemPos);
	}
}SPT_C_M_UM_REQ_MARKET_OPEN;

typedef struct tagPT_M_C_UM_ANS_MARKET_OPEN
{
	CLASS_DECLARATION_S(EUserMarketResult,Error);
	CLASS_DECLARATION_S(__int64,OnlineTime);
	CLASS_DECLARATION_S(__int64,OfflineTime);
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kError);
		if(m_kError)
		{
			return;
		}
		kPacket.Pop(m_kOnlineTime);
		kPacket.Pop(m_kOfflineTime);
	}
}SPT_M_C_UM_ANS_MARKET_OPEN;

typedef struct tagPT_C_M_REQ_MARKET_CLOSE
{
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_UM_REQ_MARKET_CLOSE);
	}
}SPT_C_M_REQ_MARKET_CLOSE;

typedef struct tagPT_M_C_ANS_MARKET_CLOSE
{
	CLASS_DECLARATION_S(EUserMarketResult,Error);

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kError);
	}
}SPT_M_C_ANS_MARKET_CLOSE;

//오픈마켓 상태 변경 (판매 시작/종료)
typedef struct tagPT_C_M_UM_REQ_MARKET_MODIFY_STATE
{
	CLASS_DECLARATION_S(BYTE,MarketState);
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_UM_REQ_MARKET_MODIFY_STATE);
		kPacket.Push(m_kMarketState);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kMarketState);
	}
}SPT_C_M_UM_REQ_MARKET_MODIFY_STATE;

typedef struct tagPT_M_C_UM_ANS_MARKET_MODIFY_STATE
{
	CLASS_DECLARATION_S(EUserMarketResult,Error);
	CLASS_DECLARATION_S(BYTE,MarketState);
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_M_C_UM_ANS_MARKET_MODIFY_STATE);
		kPacket.Push(m_kError);
		kPacket.Push(m_kMarketState);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kError);
		kPacket.Pop(m_kMarketState);
	}
}SPT_M_C_UM_ANS_MARKET_MODIFY_STATE;

// 마켓 상태 변경 (유지 시간)
typedef enum E_MARKET_MODIFY_ITEM_TYPE : BYTE
{
	EMMIT_NONE		= 0,
	EMMIT_NO_MSG	= 1,
}eMarketModifyItemType;
typedef struct tagPT_C_M_UM_REQ_USE_MARKET_MODIFY_ITEM
{
	tagPT_C_M_UM_REQ_USE_MARKET_MODIFY_ITEM(eMarketModifyItemType const eType=EMMIT_NONE):m_kType(eType),m_kMarketGrade(0){}
	CLASS_DECLARATION_S(eMarketModifyItemType,Type);
	CLASS_DECLARATION_S(SItemPos,Pos);
	CLASS_DECLARATION_S(std::wstring,MarketName);
	CLASS_DECLARATION_S(BYTE,MarketGrade);
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_UM_REQ_USE_MARKET_MODIFY_ITEM);
		kPacket.Push(Type());
		kPacket.Push(Pos());
		kPacket.Push(MarketName());
		kPacket.Push(MarketGrade());
	}
}SPT_M_C_UM_NOTI_USE_MARKET_MODIFY_ITEM;

typedef struct tagPT_M_C_UM_ANS_USE_MARKET_MODIFY_ITEM
{
	tagPT_M_C_UM_ANS_USE_MARKET_MODIFY_ITEM():m_kType(EMMIT_NONE){}
	CLASS_DECLARATION_S(EUserMarketResult,Error);
	CLASS_DECLARATION_S(eMarketModifyItemType,Type);
	CLASS_DECLARATION_S(__int64,OnlineTime);
	CLASS_DECLARATION_S(__int64,OfflineTime);
	CLASS_DECLARATION_S(int,MarketHP);
	CLASS_DECLARATION_S(std::wstring,MarketName);
	CLASS_DECLARATION_S(int,MarketGrade);

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kError);
		kPacket.Pop(m_kType);
		
		if(UMR_SUCCESS != m_kError)
		{
			return;
		}

		kPacket.Pop(m_kOnlineTime);
		kPacket.Pop(m_kOfflineTime);
		kPacket.Pop(m_kMarketHP);
		kPacket.Pop(m_kMarketName);
		kPacket.Pop(m_kMarketGrade);
	}
}SPT_M_C_UM_ANS_USE_MARKET_MODIFY_ITEM;

//아이템 등록
typedef struct tagPT_C_M_UM_REQ_ARTICLE_REG
{
	tagPT_C_M_UM_REQ_ARTICLE_REG()
	{
		CostType(false);
		ArticleCost(0);
		SellMoney(0);
		ArticleType(UMAT_ITEM);
	}

	CLASS_DECLARATION_S(SItemPos,Pos);
	CLASS_DECLARATION_S(bool,CostType);
	CLASS_DECLARATION_S(__int64,ArticleCost);
	CLASS_DECLARATION_S(short,SellMoney);
	CLASS_DECLARATION_S(eUserMarketArticleType,ArticleType);

	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_UM_REQ_ARTICLE_REG);
		kPacket.Push(m_kPos);
		kPacket.Push(m_kCostType);
		kPacket.Push(m_kArticleCost);
		kPacket.Push(m_kSellMoney);
		kPacket.Push(m_kArticleType);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kPos);
		kPacket.Pop(m_kCostType);
		kPacket.Pop(m_kArticleCost);
		kPacket.Pop(m_kSellMoney);
		kPacket.Pop(m_kArticleType);
	}
}SPT_C_M_UM_REQ_ARTICLE_REG;

typedef struct tagPT_C_M_UM_REQ_ARTICLE_DEREG
{
	CLASS_DECLARATION_S(BM::GUID,ArticleGuid);

	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_UM_REQ_ARTICLE_DEREG);
		kPacket.Push(m_kArticleGuid);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kArticleGuid);
	}
}SPT_C_M_UM_REQ_ARTICLE_DEREG;

typedef struct tagMARKET_KEY
{
	tagMARKET_KEY() 
		: kMarketName(_T(""))
		, kCharName(_T(""))
		, kOwnerId(BM::GUID::NullData())
		, iGrade(0)
		, iHP(0)
	{
	}
	std::wstring	kMarketName,
					kCharName;
	BM::GUID		kOwnerId;
	int				iGrade,
					iHP;
	size_t			kCheckCount;

	size_t min_size() const
	{
		return	sizeof(size_t) +
				sizeof(size_t) + 
				sizeof(BM::GUID) +
				sizeof(int) +
				sizeof(int) +
				sizeof(size_t);
	}

	bool operator < (tagMARKET_KEY const & kKey) const
	{
		if(iGrade > kKey.iGrade)
		{
			return true;
		}
		else if(iGrade == kKey.iGrade)
		{
			if(iHP > kKey.iHP)
			{
				return true;
			}
			else if(iHP == kKey.iHP)
			{
				if(kCheckCount < kKey.kCheckCount)
				{
					return true;
				}
			}
		}
		return false;
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(kMarketName);
		kPacket.Push(kCharName);
		kPacket.Push(kOwnerId);
		kPacket.Push(iGrade);
		kPacket.Push(iHP);
		kPacket.Push(kCheckCount);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kMarketName);
		kPacket.Pop(kCharName);
		kPacket.Pop(kOwnerId);
		kPacket.Pop(iGrade);
		kPacket.Pop(iHP);
		kPacket.Pop(kCheckCount);
	}
}SMARKET_KEY;

typedef std::map<BM::GUID,SMARKET_KEY>	CONT_MARKET_KEY;
typedef	std::list<tagMARKET_KEY> CONT_MARKET_KEY_LIST;

typedef enum E_SEARCH_MODE_TYPE : BYTE
{
	SMT_INVTYPE		= 0,
	SMT_EQUIPPOS	= 1,
	SMT_CASH		= 2,
	SMT_MONEY		= 3,
	SMT_PET			= 4,
}eSearchModeType;

typedef struct tagPT_C_M_UM_REQ_MARKET_QUERY
{
	CLASS_DECLARATION_S(std::wstring,SearchName);
	CLASS_DECLARATION_S(bool,SearchNameType);
	CLASS_DECLARATION_S(int,SearchItemMode);
	CLASS_DECLARATION_S(eSearchModeType,SearchItemModeType);
	CLASS_DECLARATION_S(int,LevelMin);
	CLASS_DECLARATION_S(int,LevelMax);
	CLASS_DECLARATION_S(int,ItemGrade);
	CLASS_DECLARATION_S(int,ItemDisplayGrade);
	CLASS_DECLARATION_S(__int64,ClassLimit);
	CLASS_DECLARATION_S(bool,CostType);
	CLASS_DECLARATION_S(__int64,ArticleIndex);

	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_UM_REQ_MARKET_QUERY);
		kPacket.Push(m_kSearchName);
		kPacket.Push(m_kSearchNameType);
		kPacket.Push(m_kSearchItemMode);
		kPacket.Push(m_kSearchItemModeType);
		kPacket.Push(m_kLevelMin);
		kPacket.Push(m_kLevelMax);
		kPacket.Push(m_kItemGrade);
		kPacket.Push(m_kItemDisplayGrade);
		kPacket.Push(m_kClassLimit);
		kPacket.Push(m_kCostType);
		kPacket.Push(m_kArticleIndex);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kSearchName);
		kPacket.Pop(m_kSearchNameType);
		kPacket.Pop(m_kSearchItemMode);
		kPacket.Pop(m_kSearchItemModeType);
		kPacket.Pop(m_kLevelMin);
		kPacket.Pop(m_kLevelMax);
		kPacket.Pop(m_kItemGrade);
		kPacket.Pop(m_kItemDisplayGrade);
		kPacket.Pop(m_kClassLimit);
		kPacket.Pop(m_kCostType);
		kPacket.Pop(m_kArticleIndex);
	}
}SPT_C_M_UM_REQ_MARKET_QUERY;

typedef struct tagPT_C_M_UM_REQ_MY_MARKET_QUERY
{
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_UM_REQ_MY_MARKET_QUERY);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
	}
}SPT_C_M_UM_REQ_MY_MARKET_QUERY;

typedef struct tagPT_C_M_UM_REQ_MY_VENDOR_QUERY
{
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_UM_REQ_MY_VENDOR_QUERY);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
	}
}SPT_C_M_UM_REQ_MY_VENDOR_QUERY;

typedef struct tagPT_C_M_UM_REQ_ARTICLE_BUY
{
	CLASS_DECLARATION_S(BM::GUID,MarketGuid);
	CLASS_DECLARATION_S(BM::GUID,ArticleGuid);
	CLASS_DECLARATION_S(WORD,BuyNum);

	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_UM_REQ_ARTICLE_BUY);
		kPacket.Push(m_kMarketGuid);
		kPacket.Push(m_kArticleGuid);
		kPacket.Push(m_kBuyNum);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kMarketGuid);
		kPacket.Pop(m_kArticleGuid);
		kPacket.Pop(m_kBuyNum);
	}
}SPT_C_M_UM_REQ_ARTICLE_BUY;

typedef struct tagPT_C_M_UM_REQ_DEALINGS_READ
{
	CLASS_DECLARATION_S(BM::GUID,DealingGuid);
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_UM_REQ_DEALINGS_READ);
		kPacket.Push(m_kDealingGuid);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
	}
}SPT_C_M_UM_REQ_DEALINGS_READ;

typedef struct tagPT_C_M_UM_REQ_MINIMUM_COST_QUERY
{
	CLASS_DECLARATION_S(DWORD,ItemNo);
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_UM_REQ_MINIMUM_COST_QUERY);
		kPacket.Push(m_kItemNo);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kItemNo);
	}
}SPT_C_M_UM_REQ_MINIMUM_COST_QUERY;

typedef struct tagPT_M_C_UM_ANS_ARTICLE_DEREG
{
	CLASS_DECLARATION_S(EUserMarketResult,Result);
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_M_C_UM_ANS_ARTICLE_DEREG);
		kPacket.Push(m_kResult);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
	}
}SPT_M_C_UM_ANS_ARTICLE_DEREG;

typedef struct tagPT_M_C_UM_ANS_ARTICLE_BUY
{
	CLASS_DECLARATION_S(EUserMarketResult,Result);
	CLASS_DECLARATION_S(BM::GUID,ArticleGuId);
	CLASS_DECLARATION_S(WORD,LeftItemNum);
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
		kPacket.Pop(m_kArticleGuId);
		kPacket.Pop(m_kLeftItemNum);
	}
}SPT_M_C_UM_ANS_ARTICLE_BUY;


typedef struct tagPT_C_M_UM_REQ_MARKET_ARTICLE_QUERY
{
	CLASS_DECLARATION_S(BM::GUID,MarketGuId);
	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_C_M_UM_REQ_MARKET_ARTICLE_QUERY);
		kPacket.Push(m_kMarketGuId);
	}
}SPT_C_M_UM_REQ_MARKET_ARTICLE_QUERY;


typedef struct tagPT_M_C_UM_ANS_DEALINGS_READ
{
	CLASS_DECLARATION_S(EUserMarketResult,Result);
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_M_C_UM_ANS_DEALINGS_READ);
		kPacket.Push(m_kResult);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
	}
}SPT_M_C_UM_ANS_DEALINGS_READ;

typedef struct tagPT_M_C_UM_ANS_MINIMUM_COST_QUERY
{
	CLASS_DECLARATION_S(__int64,Result);
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_M_C_UM_ANS_MINIMUM_COST_QUERY);
		kPacket.Push(m_kResult);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
	}
}SPT_M_C_UM_ANS_MINIMUM_COST_QUERY;

struct SCreateCharacterInfo
{
	SCreateCharacterInfo()
		: iClass(0), iLv(0), iExp(0), iHP(0), iMP(0), iExtraSkillPoint(0), iMapNo(0)
		, pkIngQuest(NULL), pkEndQuest(NULL), pkEndQuest2(NULL), pkEndQuestExt(NULL), pkEndQuestExt2(NULL)
		, iJumpingCharEvent_MaxRewardCount(0)
		, iJumpingCharEvent_ItemGroupNo(0)
	{}

	int iClass;
	short iLv;
	__int64 iExp;
	int iHP;
	int iMP;
	int iExtraSkillPoint;
	int iMapNo;
	BYTE const* pkIngQuest;
	BYTE const* pkEndQuest;
	BYTE const* pkEndQuest2;
	BYTE const* pkEndQuestExt;
	BYTE const* pkEndQuestExt2;
	int iJumpingCharEvent_MaxRewardCount;
	int iJumpingCharEvent_ItemGroupNo;
};

typedef struct tagReqCreateCharacter
{
	struct SJumpingCharEvent
	{
		SJumpingCharEvent() : iEventNo(0), iClass(0) {}

		int iEventNo;
		int iClass;
	};

	TCHAR szName[MAX_CHARACTER_NAME_LEN + 1];
	BYTE byGender;
	int iClass;
	BYTE byFiveElement_Body;
	int iDefaultHair;
	int iDefaultHairColor;
	int iDefaultFace;
	int iDefaultShirts;
	int iDefaultGlove;
	int iDefaultPants;
	int iDefaultBoots;
	int iSetNo; // by reOiL CustomSet
	SJumpingCharEvent kJumpingCharEvent;
	
	bool MakeCorrect()
	{
		StrMakeCorrect(szName);
		if (iClass < 1 || iClass > 4)
		{// 1~4 
			if(51 > iClass || 54 < iClass)
			{// 51~ 54 만 가능
				return false;
			}
		}
		return true;
	}

	DEFAULT_TBL_PACKET_FUNC();
}SReqCreateCharacter;

//==========================================================================================================
//	환혼 크래프트 프로토콜
//==========================================================================================================
// Item Enchant 정보에서 4bit를 차지하고 있으니, 15 이하 값만 사용할 것
typedef enum ePropertyType		// AT_OFFENSE_PROPERTY / AT_DEFENCE_PROPERTY
{
	E_PPTY_NONE		= 0,//무속성
	E_PPTY_FIRE		= 1,//화
	E_PPTY_WATER	= 2,//수
	E_PPTY_WOOD		= 3,//목
	E_PPTY_METAL	= 4,//금
	E_PPTY_EARTH	= 5,//토
	E_PPTY_BASIC_MAX = 6,
	E_PPTY_CURSE	= 7,// 저주. 클라이언트만 쓰자
	E_PPTY_EXPAND	= 10,// 확장 속성
	E_PPTY_NUM		= 11,

	E_PPTY_MAX = 16,
	// Item Enchant 정보에서 4bit를 차지하고 있으니, 15 이하 값만 사용할 것
}EPropertyType;

int const PROPERTY_LEVEL_LIMIT = 4;																				// 속성 최대 강화 가능 레벨
int const PROPERTY_TABLE_LIMIT = PROPERTY_LEVEL_LIMIT * E_PPTY_BASIC_MAX;						// 한개 속성에 대응되는 속성의 적용률 최대 갯수
#define CALC_PROPERTY_RATE_POS(Property,Level) static_cast<int>(Property * PROPERTY_LEVEL_LIMIT + Level - 1)	// 대응되는 속성의 레벨별 적용률 인덱스를 구한다.

typedef enum eItemRarityUpgradeResult // 로그DB에서도 사용함으로 숫자 함부로 바꾸지 말것
{
	IRUR_NONE						= -1,
	IRUR_SUCCESS					= 0,
	IRUR_NOT_FOUND_TARGET_ITEM		= 1,	// 인첸트 대상 아이템이 없다.
	IRUR_INVALID_PROPERTY			= 2,	// 정의 되지 않은 속성 이다.
	IRUR_NOT_ABLE_UPGRADE_ITEM		= 3,	// 인첸트 할 수 없는 아이템 이다.
	IRUR_NOT_ENOUGH_CRYSTALSTONE	= 4,	// 속성 크리스탈 수량이 부족하다.
	IRUR_NOT_ENOUGH_MONEY			= 5,	// 인첸트 비용이 부족하다.
	IRUR_NOT_ENOUGH_SOUL			= 6,	// 환혼 수량이 부족하다.
	IRUR_IS_SEALDING				= 7,	// 봉인 해제가 필요하다.
	IRUR_FAIL						= 8,	// 실패
	IRUR_FAIL_AND_BROKEN			= 9,	// 실패해서 깨짐
	IRUR_NOT_ABLE_PROPERTY_ITEM		= 10,	// 속성을 부여 할 수 없는 아이템이다.(무기,상의만 가능)
	IRUR_DB_ERROR					= 11,	// 아이템 정보 저장에 실패했음?
}EItemRarityUpgradeResult;

typedef enum eItemSocketSystemResult
{
	ISOC_NONE = -1,
	ISOC_SUCCESS = 0,
	ISOC_NOT_FOUND_TARGET_ITEM,		// 인첸트 대상 아이템이 없다.
	ISOC_INVALID_PROPERTY,			// 정의 되지 않은 속성 이다.
	ISOC_NOT_ABLE_UPGRADE_ITEM,		// 인첸트 할 수 없는 아이템 이다.
	ISOC_NOT_ENOUGH_CRYSTALSTONE,	// 속성 크리스탈 수량이 부족하다.
	ISOC_NOT_ENOUGH_MONEY,			// 인첸트 비용이 부족하다.
	ISOC_NOT_ENOUGH_SOUL,			// 환혼 수량이 부족하다.
	ISOC_IS_SEALDING,				// 봉인 해제가 필요하다.
	ISOC_FAIL,						// 실패
	ISOC_FAIL_AND_BROKEN,			// 실패해서 깨짐
	ISOC_NOT_ABLE_PROPERTY_ITEM,	// 속성을 부여 할 수 없는 아이템이다.(무기,상의만 가능)
}EItemSocketSystemResult;

typedef enum eItemDischargeResult
{
	IDR_SUCCESS = 0,
	IDR_NOT_FOUND_TARGET_ITEM,		// 봉인 해제 할 아이템을 찾을수 없다.
	IDR_NOT_SEALD_ITEM,				// 봉인된 아이템이 아니다.
	IDR_NOT_FOUND_DISCHARGE_ITEM,	// 봉인 해제서(?) 아이템을 찾을수 없다.
}EItemDischargeResult;

typedef struct tagPT_C_M_REQ_ITEM_RARITY_UPGRADE
{
	CLASS_DECLARATION_S(BM::GUID, NpcGuid);
	CLASS_DECLARATION_S(EPropertyType,PropertyType);
	CLASS_DECLARATION_S(SItemPos,TargetItemPos);
	CLASS_DECLARATION_S(bool,UseInsuranceItem);
	CLASS_DECLARATION_S(SItemPos,InsuranceItemPos);
	CLASS_DECLARATION_S(bool,UseSuccessRateItem);
	CLASS_DECLARATION_S(SItemPos,SuccessRateItemPos);
	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_C_M_REQ_ITEM_RARITY_UPGRADE);
		kPacket.Push(m_kNpcGuid);
		kPacket.Push(m_kPropertyType);
		kPacket.Push(m_kTargetItemPos);
		kPacket.Push(m_kUseInsuranceItem);
		kPacket.Push(m_kInsuranceItemPos);
		kPacket.Push(m_kUseSuccessRateItem);
		kPacket.Push(m_kSuccessRateItemPos);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kNpcGuid);
		kPacket.Pop(m_kPropertyType);
		kPacket.Pop(m_kTargetItemPos);
		kPacket.Pop(m_kUseInsuranceItem);
		kPacket.Pop(m_kInsuranceItemPos);
		kPacket.Pop(m_kUseSuccessRateItem);
		kPacket.Pop(m_kSuccessRateItemPos);
	}
}SPT_C_M_REQ_ITEM_RARITY_UPGRADE;

typedef struct tagPT_M_C_ANS_ITEM_RARITY_UPGRADE
{
	CLASS_DECLARATION_S(EItemRarityUpgradeResult,Result);
	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_M_C_ANS_ITEM_RARITY_UPGRADE);
		kPacket.Push(m_kResult);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
	}
}SPT_M_C_ANS_ITEM_RARITY_UPGRADE;

typedef struct tagPT_C_M_REQ_ITEM_DISCHARGE
{
	CLASS_DECLARATION_S(SItemPos,TargetItemPos);
	CLASS_DECLARATION_S(SItemPos,DischargeItemPos);
	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_C_M_REQ_ITEM_DISCHARGE);
		kPacket.Push(m_kTargetItemPos);
		kPacket.Push(m_kDischargeItemPos);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kTargetItemPos);
		kPacket.Pop(m_kDischargeItemPos);
	}
}SPT_C_M_REQ_ITEM_DISCHARGE;

typedef struct tagPT_M_C_ANS_ITEM_DISCHARGE
{
	CLASS_DECLARATION_S(EItemDischargeResult,Result);
	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(PT_M_C_ANS_ITEM_DISCHARGE);
		kPacket.Push(m_kResult);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
	}
}SPT_M_C_ANS_ITEM_DISCHARGE;

typedef struct tagGameServerPath
{
	tagGameServerPath()
	{
		kSrcPath.clear();
		kFilename.clear(); 
	}

	tagGameServerPath(std::wstring const &kInSrcPath, std::wstring const &kInFileName)
	{
		kSrcPath = kInSrcPath;
		kFilename = kInFileName;
	}
	std::wstring kSrcPath;//rsync path '/' 넣지 말것 폴더구조가 같아야 하므로.
	std::wstring kFilename;
	
	size_t min_size() const
	{
		return sizeof(size_t) * 2;
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(kSrcPath);
		kPacket.Push(kFilename);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kSrcPath);
		kPacket.Pop(kFilename);
	}
}SGameServerPath;

typedef std::map< SERVER_IDENTITY, SSingleServerStatus > CONT_SERVER_STATE;
typedef std::map< SERVER_IDENTITY, SExSingleServerStatus > CONT_SERVER_STATE_EX;
typedef std::map< SERVER_IDENTITY, PROCESS_INFORMATION_EX > CONT_SERVER_PROCESS;
typedef std::map< tagTBL_KEY< CEL::E_SESSION_TYPE >, SGameServerPath > CON_SYNC_PATH;

typedef std::map< BM::GUID, std::map< BM::GUID, POINT3 > > CONT_RESERVED_STONE_LIST;//Owner, < StoneGuid, Pos >

int const SYSTEM_INVENTORY_TIME_LIMIT = 1800000;	// 클라이언트에서 사용하는 기본 시스템 인벤토리 아이템 삭제 시간(TICK)
int const SYSTEM_INVENTORY_TIME_LIMIT_PLUSE = SYSTEM_INVENTORY_TIME_LIMIT + 20000; // 서버에서 사용하는 시스템 인벤토리 삭제 시간 (차후 네트웍크 딜레이 발생시를 대비해서 서버상에는 클라이언트 처리 시간보다 조금 더 시간 지연을 더해준다.)

typedef struct tagSSys2InvInfo
{
	tagSSys2InvInfo()
	{
		bAnyPos = 0;
	}
	BM::GUID kItemGuid;
	SItemPos kItemPos;
	bool bAnyPos;
}SSys2InvInfo;

typedef std::list< SSys2InvInfo > CONT_SYS2INV_INFO;

typedef struct tagPT_C_M_REQ_SYSTEM_INVENTORY_RECV
{
	tagPT_C_M_REQ_SYSTEM_INVENTORY_RECV(CONT_SYS2INV_INFO const &rkContSysItem)
		: m_kContSysItem(rkContSysItem)
	{
	}

	CLASS_DECLARATION_S_NO_SET(CONT_SYS2INV_INFO,ContSysItem);
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_REQ_SYSTEM_INVENTORY_RECV);
		kPacket.Push(m_kContSysItem);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kContSysItem);
	}
}SPT_C_M_REQ_SYSTEM_INVENTORY_RECV;

typedef struct tagPT_M_C_ANS_SYSTEM_INVENTORY_RECV
{
	CLASS_DECLARATION_S(HRESULT,Result);
	CLASS_DECLARATION_S(CONT_SYS2INV_INFO,ContSysItem);
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_M_C_ANS_SYSTEM_INVENTORY_RECV);
		kPacket.Push(m_kResult);
		kPacket.Push(m_kContSysItem);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
		kPacket.Pop(m_kContSysItem);
	}
}SPT_M_C_ANS_SYSTEM_INVENTORY_RECV;

typedef std::list<BM::GUID> CONT_ITEMGUID;
typedef struct tagPT_C_M_REQ_SYSTEM_INVENTORY_REMOVE
{
	tagPT_C_M_REQ_SYSTEM_INVENTORY_REMOVE(CONT_ITEMGUID const &rkVec)
		: m_kContItemGuid(rkVec)
	{
	}

	CLASS_DECLARATION_S(CONT_ITEMGUID,ContItemGuid);
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_REQ_SYSTEM_INVENTORY_REMOVE);
		kPacket.Push(m_kContItemGuid);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kContItemGuid);
	}
}SPT_C_M_REQ_SYSTEM_INVENTORY_REMOVE;

typedef struct tagPT_M_C_ANS_SYSTEM_INVENTORY_REMOVE
{
	CLASS_DECLARATION_S(HRESULT,Result);
	CLASS_DECLARATION_S(CONT_ITEMGUID,ContItemGuid);
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_M_C_ANS_SYSTEM_INVENTORY_REMOVE);
		kPacket.Push(m_kResult);
		kPacket.Push(m_kContItemGuid);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kResult);
		kPacket.Pop(m_kContItemGuid);
	}
}SPT_M_C_ANS_SYSTEM_INVENTORY_REMOVE;

typedef struct tagSearchPeopleInfo
{
	BM::GUID		kCharGuid;
	int				iGndNo;	
	int				iLevel;
	std::wstring	Name;

	tagSearchPeopleInfo()
	{
		Clear();
	}

	void Clear()
	{
		kCharGuid = BM::GUID::NullData();
		iGndNo = 0;
		iLevel = 0;
		Name.clear();
	}

	size_t min_size()const
	{
		return 
			sizeof(kCharGuid)+
			sizeof(iGndNo)+
			sizeof(iLevel)+
			sizeof(size_t);//Name);
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(kCharGuid);
		kPacket.Push(iGndNo);
		kPacket.Push(iLevel);
		kPacket.Push(Name);
	}
	
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kCharGuid);
		kPacket.Pop(iGndNo);
		kPacket.Pop(iLevel);
		kPacket.Pop(Name);
	}
}SSearchPeopleInfo;
typedef std::vector<SSearchPeopleInfo> CONT_SEARCH_UNIT_INFO;

typedef std::list< BM::GUID > CONT_GUID_LIST;

typedef enum
{
	PMET_NONE = 0,
	PMET_PARTY,
	PMET_FRIEND,
	PMET_ChannelChat,
	PMET_GUILD,
	PMET_EMPORIA,
	PMET_COUPLE,
	PMET_SendToContents,
	PMET_SendToCenter,
	PMET_MissionMgr,
	PMET_SendToPacketHandler,
	PMET_RealmChat,
	PMET_PvP,
	PMET_OpenMarket,
	PMET_MissionRank,
	PMET_Portal,
	PMET_GMProcess,
	PMET_Boss,
	PMET_Notice,
	PMET_OXQuizEvent,
	PMET_CouponEventDoc,
	PMET_CouponEventView,
	PMET_RELOAD_DATA,
	PMET_MYHOME,
	PMET_MARRY,
	PMET_EVENTQUEST,
	PMET_BATTLESQUARE,
	PMET_LuckyStarEvent,
	PMET_REALM_EVENT,
	PMET_HARDCORE_DUNGEON,
	PMET_WEB_HELPER,
	PMET_PVP_LEAGUE,
	PMET_EXPEDITION
}EContentsMessageType;

size_t const MAX_MISSION_RANK_INPUTMEMO_LEN = 15;// 미션결과창 나온후 한마디 입력글자수

typedef enum E_CASH_SHOP_RESULT
{
	// Database에서 error 사용함
	// 중간에 값을 끼워 넣지 말것.
	CSR_SUCCESS = 0,
	CSR_INVALID_ARTICLE,
	CSR_NOT_ENOUGH_CASH,
	CSR_INVALID_CHARACTER,
	CSR_DB_ERROR,
	CSR_NOT_FOUND_GIFT,
	CSR_NOT_RECV_GIFT,
	CSR_NOT_REG_RANK,
	CSR_HAVE_DEREG_TIME,
	CSR_BUY_INVENTORY_FULL,
	CSR_GIFT_INVENTORY_FULL,
	CSR_USE_BADWORD,		// 금칙어를 사용 했습니다.
	CSR_NOT_HAVE_TIMELIMIT,// 시간 제한이 없는 아이템이다.
	CSR_NOT_MATCH_TIMELIMIT,// 시간 타입이 동일하지 않다.
	CSR_TIMELIMIT_OVERFLOW,// 충전할수 있는 사용시간이 초과되었음
	CSR_NEED_RELOAD_DATA,	// 캐시샵 데이터를 다시 읽어야 한다.
	CSR_NOT_ENOUGH_BONUS,	// 마일리지 부족
	CSR_SYSTEM_ERROR,
	CSR_BLOCKED_USER,		// block 되어 있어 아이템 구매 할 수 없음.
	CSR_NOT_ENOUGH_LEVEL,	// 레벨 제한으로 구입할 수 없다.
	CSR_NOT_BUY_LIMIT,		// 한정판매 수량 초과(계정당)
	CSR_NOT_DATE_LIMIT,		// 한정판매 기간 초과 또는 시간이 안된경우
	CSR_NOT_BUY_LIMIT_OVEW,	// 한정판매 수량 초과(전체 목록)
}ECashShopResult;

typedef enum E_CASH_SHOP_CATEGORY_TYPE
{
	CSCT_LARGE,
	CSCT_MIDDLE,
	CSCT_SMALL,
}ECashSopType;

BYTE const CASH_LOG_FLAG = 100;

struct REQ_BUY_ARTICLE
{
	REQ_BUY_ARTICLE()
	{
		iArticleIdx = 0;
		bTimeType = 0;
		iUseTime = 0;
	}
	int iArticleIdx;
	BYTE bTimeType;
	int iUseTime;
};

typedef std::vector<REQ_BUY_ARTICLE> CONT_REQ_BUY_ARTICLE;

//캐쉬샵 아이템 구매 요청
typedef struct tagPT_C_M_CS_REQ_BUY_ARTICLE
{
	tagPT_C_M_CS_REQ_BUY_ARTICLE(BM::GUID const &kStoreValueKey)
	{
		m_kStoreValueKey = kStoreValueKey;
	}

	CONT_REQ_BUY_ARTICLE kContBuyArticle;
	__int64 i64Bonus;
	BYTE iBuyType;
	BM::GUID m_kStoreValueKey;

	void WriteToPacket(BM::Stream & kPacket, WORD const PacketType = 0) const
	{
		kPacket.Push((0==PacketType)?(PT_C_M_CS_REQ_BUY_ARTICLE):(PacketType));
		kPacket.Push(iBuyType);
		kPacket.Push(kContBuyArticle);
		kPacket.Push(i64Bonus);
		kPacket.Push(m_kStoreValueKey);
	}
}SPT_C_M_CS_REQ_BUY_ARTICLE;

typedef struct tagPT_M_C_CS_ANS_BUY_ARTICLE
{
	int iError;
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iError);
	}
}SPT_M_C_CS_ANS_BUY_ARTICLE;

//캐쉬샵 아이템 판매 요청
typedef struct tagPT_C_M_CS_REQ_SEL_ARTICLE
{
	SItemPos kPos;
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(PT_C_M_CS_REQ_SEL_ARTICLE);
		kPacket.Push(kPos);
	}
}SPT_C_M_CS_REQ_SEL_ARTICLE;

typedef struct tagPT_M_C_CS_ANS_SEL_ARTICLE
{
	int iError;
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iError);
	}
}SPT_M_C_CS_ANS_SEL_ARTICLE;

//캐쉬샵 아이템 선물 요청
typedef struct tagPT_C_M_CS_REQ_SEND_GIFT
{
	tagPT_C_M_CS_REQ_SEND_GIFT(BM::GUID const &kStoreValueKey)
	{
		m_kStoreValueKey = kStoreValueKey;
	}

	int iArticleIdx;
	BYTE bTimeType;
	int iUseTime;
	std::wstring kRecverName,
				 kComment;
	__int64 i64BonusUse;

	BM::GUID m_kStoreValueKey;

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(PT_C_M_CS_REQ_SEND_GIFT);
		kPacket.Push(iArticleIdx);
		kPacket.Push(bTimeType);
		kPacket.Push(iUseTime);
		kPacket.Push(kRecverName);
		kPacket.Push(kComment);
		kPacket.Push(i64BonusUse);
		kPacket.Push(m_kStoreValueKey);
	}
}SPT_C_M_CS_REQ_SEND_GIFT;

typedef struct tagPT_M_C_CS_ANS_SEND_GIFT
{
	int iError;
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iError);
	}
}SPT_M_C_CS_ANS_SEND_GIFT;

//캐쉬샵 내 선물 목록 요청
typedef struct tagPT_C_M_CS_REQ_ENTER_CASHSHOP
{
	tagPT_C_M_CS_REQ_ENTER_CASHSHOP(BM::GUID const &kStoreValueKey)
	{
		m_kStoreValueKey = kStoreValueKey;
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(PT_C_M_CS_REQ_ENTER_CASHSHOP);
		kPacket.Push(m_kStoreValueKey);
	}
	BM::GUID m_kStoreValueKey;
}SPT_C_M_CS_REQ_ENTER_CASHSHOP;

typedef struct tagCASHGIFTINFO
{
	BM::GUID			kGiftGuId;
	std::wstring		kCharName;	// bType = 0: Recver Name, bType = 1: Sender Name
	BM::DBTIMESTAMP_EX	kSendTime;
	int					iCashItemIdx;
	std::wstring		kComment;
	BYTE				bType;		// 0: send 1: recv
	BYTE				bTimeType;	// cash item time type
    int					iUseTime;	// cash item use time

	tagCASHGIFTINFO()
	{
		iCashItemIdx = 0;
		bType = 0;		// 0: send 1: recv
		bTimeType = 0;	// cash item time type
		iUseTime = 0;	// cash item use time
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(kGiftGuId);
		kPacket.Push(kCharName);	// bType = 0: Recver Name, bType = 1: Sender Name
		kPacket.Push(kSendTime);
		kPacket.Push(iCashItemIdx);
		kPacket.Push(kComment);
		kPacket.Push(bType);		// 0: send 1: recv
		kPacket.Push(bTimeType);	// cash item time type
		kPacket.Push(iUseTime);	// cash item use time
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kGiftGuId);
		kPacket.Pop(kCharName);	// bType = 0: Recver Name, bType = 1: Sender Name
		kPacket.Pop(kSendTime);
		kPacket.Pop(iCashItemIdx);
		kPacket.Pop(kComment);
		kPacket.Pop(bType);		// 0: send 1: recv
		kPacket.Pop(bTimeType);	// cash item time type
		kPacket.Pop(iUseTime);	// cash item use time
	}
}SCASHGIFTINFO;

typedef std::vector<SCASHGIFTINFO> CONT_CASHGIFTINFO;

typedef struct tagCASHRANKINFO
{
	std::wstring	kName;
	__int64			i64Cash;

	tagCASHRANKINFO()
	{
		i64Cash = 0;
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(kName);
		kPacket.Push(i64Cash);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kName);
		kPacket.Pop(i64Cash);
	}
}SCASHRANKINFO;

typedef std::vector<SCASHRANKINFO> CONT_CASHRANKINFO;

typedef struct tagPT_M_C_CS_ANS_ENTER_CASHSHOP
{
	int iError;
	CONT_CASHGIFTINFO kContGift;
	CONT_CASHRANKINFO kContRank;
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(PT_M_C_CS_ANS_ENTER_CASHSHOP);
		kPacket.Push(iError);
		if(iError)
		{
			return;
		}

		kPacket.Push(kContGift.size());
		for(CONT_CASHGIFTINFO::const_iterator iter = kContGift.begin();iter != kContGift.end();++iter)
		{
			(*iter).WriteToPacket(kPacket);
		}

		kPacket.Push(kContRank.size());
		for(CONT_CASHRANKINFO::const_iterator iter = kContRank.begin();iter != kContRank.end();++iter)
		{
			(*iter).WriteToPacket(kPacket);
		}
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iError);
		if(iError)
		{
			return;
		}

		{
			CONT_CASHGIFTINFO::size_type kCount;
			CONT_CASHGIFTINFO::value_type kGift;
			kPacket.Pop(kCount);
			for(CONT_CASHGIFTINFO::size_type i = 0;i < kCount;i++)
			{
				kGift.ReadFromPacket(kPacket);
				kContGift.push_back(kGift);
			}
		}

		{
			CONT_CASHRANKINFO::size_type kCount;
			CONT_CASHRANKINFO::value_type kRank;
			kPacket.Pop(kCount);
			for(CONT_CASHRANKINFO::size_type i = 0;i < kCount;i++)
			{
				kRank.ReadFromPacket(kPacket);
				kContRank.push_back(kRank);
			}
		}
	}
}SPT_M_C_CS_ANS_ENTER_CASHSHOP;

//캐쉬샵 나가기
typedef struct tagPT_C_M_CS_REQ_EXIT_CASHSHOP
{
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(PT_C_M_CS_REQ_EXIT_CASHSHOP);
	}
}SPT_C_M_CS_REQ_EXIT_CASHSHOP;

typedef struct tagPT_M_C_CS_ANS_EXIT_CASHSHOP
{
	int iError;
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iError);
	}
}SPT_M_C_CS_ANS_EXIT_CASHSHOP;

//캐쉬샵 선물 수령 요청
typedef struct tagPT_C_M_CS_REQ_RECV_GIFT
{
	BM::GUID kGiftGuId;
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(PT_C_M_CS_REQ_RECV_GIFT);
		kPacket.Push(kGiftGuId);
	}
}SPT_C_M_CS_REQ_RECV_GIFT;

typedef struct tagPT_M_C_CS_ANS_RECV_GIFT
{
	int iError;
	BM::GUID kGiftGuId;
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iError);
		kPacket.Pop(kGiftGuId);
	}
}SPT_M_C_CS_ANS_RECV_GIFT;

// 캐쉬 랭킹 상태 수정
typedef struct tagPT_C_M_CS_REQ_MODIFY_VISABLE_RANK
{
	BYTE bState;
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(PT_C_M_CS_REQ_MODIFY_VISABLE_RANK);
		kPacket.Push(bState);
	}
}SPT_C_M_CS_REQ_MODIFY_VISABLE_RANK;

typedef struct tagPT_M_C_CS_ANS_MODIFY_VISABLE_RANK
{
	int iError;
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iError);
	}
}SPT_M_C_CS_ANS_MODIFY_VISABLE_RANK;


enum E_ACHIEVEMENT_RESULT
{
	AR_SUCCESS,
	AR_INVALID_ACHIEVEMENT,
	AR_INVALID_PLAYER,
	AR_NOT_HAVE_ITEM,
	AR_INVENTORY_FULL,
	AR_DB_ERROR,
};

typedef struct tagPT_C_M_REQ_ACHIEVEMENT_TO_ITEM
{
	int iAchievementIdx;

	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_REQ_ACHIEVEMENT_TO_ITEM);
		kPacket.Push(iAchievementIdx);
	}

}SPT_C_M_REQ_ACHIEVEMENT_TO_ITEM;

typedef struct tagPT_M_C_ANS_ACHIEVEMENT_TO_ITEM
{
	int iError;
	int iSaveIdx;
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iError);
		kPacket.Pop(iSaveIdx);
	}
}SPT_M_C_ANS_ACHIEVEMENT_TO_ITEM;

enum E_COUPON_RESULT_TYPE
{
	CRT_SUCCESS,
	CRT_INVALID_COUPON,	// 잘못된 쿠폰이다.
	CRT_INVALID_EVENT,	// 잘못된 이벤트 타입이다.
	CRT_EXPIRE,			// 사용 기간이 만료되었다.
	CRT_DB_ERROR,		// 디비 에러 ㅡㅡ;;
	CRT_TOO_MANY_FAILED,// 쿠폰 입력에 10회 이상 실패 했다. 이경우 1시간 후에 다시 쿠폰 입력을 진행할수 있다.
	CRT_NOT_HAVE_EVENT_COUPON,//이벤트 쿠폰을 받지 않았다.
	CRT_ALREADY_USED,	// 이미 사용된 쿠폰 번호 입니다.
	CRT_ONLY_ONCE_USE_MEMBER,	// 계정당 1번만 사용할 수 있다.
	CRT_ONLY_ONCE_USE_CHARACTER,// 캐릭터당 1번만 사용할 수 있다.
};

typedef struct tagCOUPON_REWARD
{
	int iItemNo;
	short siItemCount;
	BYTE bTimeType;
	short siUseTime;
	BM::GUID kItemGuid;
}SCOUPON_REWARD;

typedef std::vector<SCOUPON_REWARD> CONT_COUPON_REWARD;

typedef struct tagPT_C_M_REQ_USER_MAP_MOVE
{
	int iMapNo;
	SItemPos kItemPos;

	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_REQ_USER_MAP_MOVE);
		kPacket.Push(iMapNo);
		kPacket.Push(kItemPos);
	}
}SPT_C_M_REQ_USER_MAP_MOVE;

typedef struct tagPT_M_C_ANS_USER_MAP_MOVE
{
	HRESULT hResult;
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(hResult);
	}
}SPT_M_C_ANS_USER_MAP_MOVE;

typedef struct tagPT_C_M_REQ_RENTALSAFE_EXTEND
{
	int		 kInvType;
	SItemPos kItemPos;

	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_REQ_RENTALSAFE_EXTEND);
		kPacket.Push(kInvType);
		kPacket.Push(kItemPos);
	}

}SPT_C_M_REQ_RENTALSAFE_EXTEND;

typedef struct tagPT_M_C_ANS_RENTALSAFE_EXTEND
{
	HRESULT kResult;
	bool	bIsShareSafe;
	tagPT_M_C_ANS_RENTALSAFE_EXTEND():kResult(E_FAIL),bIsShareSafe(false){}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kResult);
		kPacket.Pop(bIsShareSafe);
	}
}SPT_M_C_ANS_RENTALSAFE_EXTEND;

typedef struct tagPT_C_M_REQ_MOVETOPARTYMEMBER
{
	BM::GUID kMemberGuid;
	SItemPos kItemPos;

	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_REQ_MOVETOPARTYMEMBER);
		kPacket.Push(kMemberGuid);
		kPacket.Push(kItemPos);
	}
}SPT_C_M_REQ_MOVETOPARTYMEMBER;

typedef struct tagPT_M_C_ANS_MOVETOPARTYMEMBER
{
	HRESULT kResult;

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kResult);
	}
}SPT_M_C_ANS_MOVETOPARTYMEMBER;

typedef struct tagPT_C_M_REQ_MOVETOPARTYMASTERGROUND
{
	BM::GUID kMemberGuid;

	void WriteToPacket(BM::Stream& kPacket)
	{
		kPacket.Push(PT_C_M_REQ_MOVETOPARTYMASTERGROUND);
		kPacket.Push(kMemberGuid);
	}
}SPT_C_M_REQ_MOVETOPARTYMASTERGROUND;

typedef struct tagPT_M_C_ANS_MOVETOPARTYMASTERGROUND
{
	HRESULT kResult;

	void ReadFromPacket(BM::Stream& kPacket)
	{
		kPacket.Pop(kResult);
	}
}SPT_M_C_ANS_MOVETOPARTYMASTERGROUND;

int const MAX_COUPON_EVENT_REWARD = 10;
typedef struct tagCOUPON_EVENT_REWARD
{
	int iIDX;
	int iEventKey;
	std::wstring kRewardTitle;
	int iItemNo[MAX_COUPON_EVENT_REWARD];

	size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(int)+
			sizeof(size_t)+//kRewardTitle
			sizeof(iItemNo);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iIDX);
		kPacket.Push(iEventKey);
		kPacket.Push(kRewardTitle);
		kPacket.Push(iItemNo,sizeof(iItemNo));
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iIDX);
		kPacket.Pop(iEventKey);
		kPacket.Pop(kRewardTitle);
		kPacket.PopMemory(iItemNo,sizeof(iItemNo));
	}
}SCOUPON_EVENT_REWARD;

typedef struct tagCOUPON_EVENT_INFO
{
	int					iEventKey;
	BM::PgPackedTime	kStartTime;
	BM::PgPackedTime	kEndTime;
	std::wstring		kTitle;
	std::wstring		kDiscription;
	
	size_t min_size()const
	{
		return 
			sizeof(iEventKey)+
			sizeof(kStartTime)+
			sizeof(kEndTime)+
			sizeof(size_t);//kTitle)+
			sizeof(size_t);//kDiscription);
	}

	void WriteToPacket(BM::Stream & rkPacket) const
	{
		rkPacket.Push(iEventKey);
		rkPacket.Push(kStartTime);
		rkPacket.Push(kEndTime);
		rkPacket.Push(kTitle);
		rkPacket.Push(kDiscription);
	}

	void ReadFromPacket(BM::Stream & rkPacket)
	{
		rkPacket.Pop(iEventKey);
		rkPacket.Pop(kStartTime);
		rkPacket.Pop(kEndTime);
		rkPacket.Pop(kTitle);
		rkPacket.Pop(kDiscription);
	}
}SCOUPON_EVENT_INFO;

typedef std::vector<SCOUPON_EVENT_INFO> CONT_COUPON_EVENT_INFO;
typedef std::vector<SCOUPON_EVENT_REWARD> CONT_COUPON_EVENT_REWARD;

typedef struct tagPT_C_M_REQ_COUPON_EVENT_SYNC
{
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_REQ_COUPON_EVENT_SYNC);
	}
}SPT_C_M_REQ_COUPON_EVENT_SYNC;

typedef struct tagPT_M_C_ANS_COUPON_EVENT_SYNC
{
	CONT_COUPON_EVENT_INFO kContCouponEvent;
	CONT_COUPON_EVENT_REWARD kContCouponEventReward;

	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TWriteArray_M(kPacket,kContCouponEvent);
		PU::TWriteArray_M(kPacket,kContCouponEventReward);
	}

}SPT_M_C_ANS_COUPON_EVENT_SYNC;

typedef enum eClientDisconnectedCause
{
	// BYTE를 사용 함으로 최대 255개의 이유가 생길 수 있다.
	CDC_Normal					= 0,	// 정상 연결 해제 종료
	CDC_SpeedHack				= 1,	// 스피드 핵에 의한 강제 끊김
	CDC_SuspectedSpeedHack		= 2,	// 스피드 핵으로 의심되어서 끊음
	CDC_LaziPing				= 3,	// 오랫동안 핑 패킷이 도착 안함
	CDC_ChangeChannel			= 4,	// 채널 이동(다른 switch에 접속하기 위한 연결 해제)
	CDC_MapServerDown			= 5,	// 맵서버 다운(Disconnect)으로 인한
	CDC_MapServerClear			= 6,	// PgGround::Clear()
	CDC_CenterRemoveUserByGnd	= 7,	// Center / PgServerSetMgr::ProcessRemoveUser_ByGround()
	CDC_CharInfoErr				= 8,	// 캐릭터 정보 오류
	CDC_CharMapErr				= 9,	// 맵이동 오류
	CDC_NetworkErr				= 10,	// 네트워크 오류
	CDC_GM_CmdKick				= 11,	// GM Kick(Command)
	CDC_GM_ToolKick				= 12,	// WEB GM Tool Kick
	CDC_GM_CharEdit_Kick		= 13,	// WEB GM Tool User modify info
	CDC_GM_AccountBlock			= 14,	// WEB GM Tool Account block
	CDC_GM_RPCKick				= 15,	// GM Tool China RPC
	CDC_Invalid_Macro_Time		= 16,	// 매크로 입력 타이밍이 아닐경우 메시지를 전송한경우
	CDC_MoveSpeed_Hack			= 17,	// 이동속도 검사에서 딱 걸렸어~
	CDC_HyperMove_Hack			= 18,	// HypeMove check (대점프단, Telemove 등)
	//19번은 비었음 다른걸로 채워야 함. CDC_ActionCount_Hack case가 늘어남에 따라 뒷번호로 이동
	CDC_MaxTarget_Hack			= 20,
	CDC_Projectile_Hack			= 21,
	CDC_Blowup_Hack				= 22,
	CDC_ClientNotify_Hack		= 23,
	CDC_RandomSeedCallCounter	= 24,
	CDC_CheckAntiHack			= 25,	// 안티핵 체크에 걸렸다.
	CDC_CheckAntiHack_C			= 26,
	CDC_CheckAntiHack_NoReply	= 27,	// 안티핵 체크에 응답이 없다.
	CDC_TryLoginFromOther		= 28,	// 다른 사람이 로그인을 시도 했다.
	CDC_ActionCount_Hack_A		= 29,	// Action Count Hack A - Map Move 시 체크
	CDC_ActionCount_Hack_B		= 30,	// Action Count Hack B - 이전 액션보다 적은 시간이 올 경우
	CDC_ActionCount_Hack_C		= 31,	// Action Count Hack C - 액션 카운트에 걸렸을 경우
	CDC_CheckAntiHack_NoReply2	= 32,	// 안티핵 체크에 응답이 없다(상황:응답 대기중에 연결이 종료 되었다) // 클라로 메시지 없음 
	CDC_CheckHellDart			= 33,	// HellDart
	CDC_CheckAntiHack_Format	= 34,	// 포멧이 안맞는다
	CDC_PlayerPlayTimeOver		= 35,	// 피로도시스템 - 접속 허용시간 오버
	CDC_NotAuthSession			= 36,	// 인증되지 않은 세션
	CDC_GM_GSMKick				= 37,	// NC Gsm Notify Kick
} EClientDisconnectedCause;

#define USER_DISCONNECT_CAUTION_LOG(_F, _L, _cause, _rmd, _ext)			CAUTION_LOG(BM::LOG_LV5, __FL2__(_F, _L) << _T("User Disconnected Cause[") << #_cause << _T("] User[M-G: ") << _rmd.guidMember << _T(" C-G: ") << _rmd.guidCharacter << _T(" A-N: ") << _rmd.ID() << _T(" IP: ") << _rmd.addrRemote.ToString() << _T("]") << _ext);
#define USER_DISCONNECT_CAUTION_HACK_LOG(_F, _L, _cause, _rmd, _ext)	CAUTION_LOG(BM::LOG_LV5, __FL2__(_F, _L) << _T("[HACKING] Disconnected Cause[") << #_cause << _T("] User[M-G: ") << _rmd.guidMember << _T(" C-G: ") << _rmd.guidCharacter << _T(" A-N: ") << _rmd.ID() << _T(" IP: ") << _rmd.addrRemote.ToString() << _T("]") << _ext);
#define USER_DISCONNECT_HACK_LOG(_F, _L, _cause, _rmd, _ext)	HACKING_LOG(BM::LOG_LV5, __FL2__(_F, _L) << _T("[HACKING] Disconnected Cause[") << #_cause << _T("] User[M-G: ") << _rmd.guidMember << _T(" C-G: ") << _rmd.guidCharacter << _T(" A-N: ") << _rmd.ID() << _T(" IP: ") << _rmd.addrRemote.ToString() << _T("]") << _ext);

#define CASE_USER_DISCONNECT_CAUSE(_F, _L, _cause, _rmd, _iMsgNo, _ext) case _cause:\
	{	\
		iMsgNo = _iMsgNo;	\
		USER_DISCONNECT_CAUTION_LOG(_F, _L, _cause, _rmd, _ext);	\
	}break;

#define CASE_USER_DISCONNECT_HACK_CAUSE(_F, _L, _cause, _rmd, _iMsgNo, _hreturn, _ext) case _cause:\
	{	\
		iMsgNo = _iMsgNo;	\
		hReturn = (_hreturn);	\
		USER_DISCONNECT_CAUTION_HACK_LOG(_F, _L, _cause, _rmd, _ext);	\
	}break;

#define CASE_USER_DISCONNECT_HACK(_F, _L, _cause, _rmd, _iMsgNo, _hreturn, _ext) case _cause:\
	{	\
		iMsgNo = _iMsgNo;	\
		hReturn = (_hreturn);	\
		USER_DISCONNECT_HACK_LOG(_F, _L, _cause, _rmd, _ext);	\
	}break;


typedef struct tagPT_C_M_REQ_SUMMONPARTYMEMBER
{
	BM::GUID kCharGuid;
	SItemPos kItemPos;

	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_REQ_SUMMONPARTYMEMBER);
		kPacket.Push(kCharGuid);
		kPacket.Push(kItemPos);
	}
}SPT_C_M_REQ_SUMMONPARTYMEMBER;

typedef struct tagPT_M_C_REQ_SUMMONPARTYMEMBER
{
	BM::GUID kCharGuid;
	SGroundKey kGndKey;
	SItemPos kItemPos;

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kCharGuid);
		kPacket.Pop(kGndKey);
		kPacket.Pop(kItemPos);
	}
}SPT_M_C_REQ_SUMMONPARTYMEMBER;

typedef struct tagPT_M_C_ANS_SUMMONPARTYMEMBER
{
	HRESULT hResult;

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(hResult);
	}
}SPT_M_C_ANS_SUMMONPARTYMEMBER;

typedef struct tagPT_C_M_REQ_MOVETOSUMMONER
{
	bool		bYesNo;
	BM::GUID	kCharGuid;
	SGroundKey	kGndKey;
	SItemPos	kItemPos;

	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(bYesNo);
		kPacket.Push(kCharGuid);
		kPacket.Push(kGndKey);
		kPacket.Push(kItemPos);
	}
}SPT_C_M_REQ_MOVETOSUMMONER;

typedef struct tagPT_M_C_ANS_MOVETOSUMMONER
{
	HRESULT hResult;

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(hResult);
	}
}SPT_M_C_ANS_MOVETOSUMMONER;

/*===================================================================================================================
	장착창 아이템을 기본 인벤토리로 복귀 시키는 동작
===================================================================================================================*/
enum E_ERROR_FIT_TO_INV
{
	EF2I_SUCCESS = 0,
	EF2I_NOT_FOUND_ITEM,				// 아이템을 찾을수 없음
	EF2I_ALREADY_PRIMARYINV,			// 이미 기본 인벤에 있음
	EF2I_CANT_UNEQUIP_OLDITEM,			// 이전 아이템을 해지 할 수 없음
	EF2I_NOT_HAVE_EMPTY_INV,			// 기본 인벤에 빈공간이 없음
};

typedef struct tagPT_C_M_REQ_FIT2PRIMARYINV
{
	SItemPos kItemPos;
	void WriteToPacket(BM::Stream & kPacket)
	{
		kPacket.Push(PT_C_M_REQ_FIT2PRIMARYINV);
		kPacket.Push(kItemPos);
	}
}SPT_C_M_REQ_FIT2PRIMARYINV;

typedef struct tagPT_M_C_ANS_FIT2PRIMARYINV
{
	HRESULT hResult;

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(hResult);
	}
}SPT_M_C_ANS_FIT2PRIMARYINV;


typedef enum eOXQUIZ_EVENT_RESULT
{
	OXQES_SUCCESS = 0,
	OXQES_NOT_FOUND_EVENT,
	OXQES_NOT_OPENED,
	OXQES_ALREADY_JOINED,
	OXQES_NOT_QUIZ_TIME,
	OXQES_NOT_JOINED_EVENT,
	OXQES_PLAYER_FULL,
}EOXQUIZ_EVENT_RESULT;

typedef enum eLUCKEYSTAR_EVENT_RESULT
{
	LSER_SUCCESS = 0,
	LSER_NOT_FOUND_EVENT,
	LSER_NOT_SELECT_STAR,
	LSER_NOT_OPENED,
	LSER_ALREADY_JOINED,
	LSER_NOT_JOINED_EVENT,
	LSER_NOT_FIND_USERINFO,
	LSER_PLAYER_FULL,
	LSER_HAVE_NOT_ENOUGH_MONEY,
	LSER_RESULT_MIN_AGO,
}ELUCKYSTAR_EVENT_RESULT;

template<typename T>
struct SAddOnAbil
{
	SAddOnAbil():wType(0),kValue(T()){}
	WORD	wType;
	T		kValue;
	DEFAULT_TBL_PACKET_FUNC()
};

typedef std::vector<SAddOnAbil<int> >		CONT_ABIL32;
typedef std::vector<SAddOnAbil<__int64> >	CONT_ABIL64;

typedef enum eDETECTION_HACK_TYPE
{
	DHT_TELEMOVE,			// 순간 이동
	DHT_HIDE_IN_BOX,		// 박스 안에서 공격
	DHT_AUTO_PICKUP,		// 아이템 자동 집기
	DHT_MOVE_SPEED,			// 이속 해킹
	DHT_ATTACK_SPEED,		// 공속 해킹
	DHT_CALL_SHOP,			// 상점 호출
	DHT_CALL_QUEST,			// 퀘스트 완료
	DHT_BLOWUP_POS,			// BlowUp좌표 차이가 남
}EDETECTION_HACK_TYPE;

int const MAX_MACRO_PASSWORD_LEN = 10;// 매크로 검사 패스워드 길이
int const MIN_COUPON_KEY_LEN = 8;//쿠폰 넘버 최소 길이
int const MAX_COUPON_KEY_LEN = 32;// 쿠폰 넘버 최대 길이
int const MAX_OPENMARKET_SEARCH_STRING_LEN = 50;// 오픈 마켓 텍스트 검색키 길이
int const MAX_CASHSHOP_GIFT_COMMENT = 50;// 캐시샵 선물 하기 코멘트 
int const MAX_MARKET_NAME_LEN = 50;// 오픈마켓 상점 이름길이
int const MAX_SYSTEMINVEN_NUM = 24;// 시스템 인벤 하나에 최대 들어가는 아이템 종류 갯수
int const MAX_SMS_LEN = 200;
int const MAX_CARD_COMMENT_LEN = 50;// 캐릭터 카드 자기 소개 길이
int const MAX_PORTAL_COMMENT_LEN = 50;// 포탈 코맨트 길이
int const MAX_USER_PORTAL_NUM = 1;	// 최대 기억 가능 포탈 개수 지금은 기획에서 1개만 저장 하자고 하지만 -_-;; 못 믿겠다.
int const ENABLE_RECOMMEND_LEVEL = 6;// 캐릭터 추천가능 레벨

typedef struct tagMATCH_CARD_INFO
{
	tagMATCH_CARD_INFO():bSex(0),bAge(0),iMatchPoint(0){}
	explicit tagMATCH_CARD_INFO(BM::GUID const & __kGuid,std::wstring const & __kName,BYTE const __kSex,BYTE const __kAge,int const __kMatchPoint):
	kCharGuid(__kGuid),kName(__kName),bSex(__kSex),bAge(__kAge),iMatchPoint(__kMatchPoint){}
	BM::GUID kCharGuid;
	std::wstring kName;
	BYTE bSex;
	BYTE bAge;
	int iMatchPoint;

	size_t min_size()const
	{
		return sizeof(kCharGuid)+
				sizeof(size_t)+
				sizeof(bSex)+
				sizeof(bAge) + 
				sizeof(iMatchPoint);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(kCharGuid);
		kPacket.Push(kName);
		kPacket.Push(bSex);
		kPacket.Push(bAge);
		kPacket.Push(iMatchPoint);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kCharGuid);
		kPacket.Pop(kName);
		kPacket.Pop(bSex);
		kPacket.Pop(bAge);
		kPacket.Pop(iMatchPoint);
	}
}SMATCH_CARD_INFO;

typedef enum eCharacterCardRecommendType : BYTE
{
	CCRT_NONE			= 0,
	CCRT_RECOMMEND		= 1,
	CCRT_POPULARER		= 2
} ECharacterCardRecommendType;

typedef std::vector<SMATCH_CARD_INFO> CONT_MATCH_CARD_INFO;
const int MAX_MATCH_CARD_VIEW_NUM = 10;

typedef enum eMODIFY_CASH_TYPE : BYTE
{
	MCT_NONE = 0,
	MCT_GAME_CS_BUY_ARTICLE	= 1,
	MCT_GM_TOOL				= 2,
	MCT_GM_SERVER			= 3,
	MCT_AP_SERVER			= 4,
	MCT_GAME_OM_BUY_ARTICLE	= 5,
	MCT_GAME_OM_SELL_ARTICLE = 6,
	MCT_GAME_COUPON			= 7,
	MCT_GAME_CS_SEND_GIFT	= 8,
	MCT_GAME_CS_ADD_TIME	= 9,
	//MCT_ADD_CASH			= 10,
	MCT_GAME_OM_BUY_ARTICLE_ROLLBACK	= 11,
	MCT_GAME_OM_SELL_ARTICLE_ROLLBACK	= 12,
	MCT_GAME_COUPON_ROLLBACK			= 13,
	MCT_GAME_CS_SEND_GIFT_ROLLBACK		= 14,
	MCT_GAME_CS_ADD_TIME_ROLLBACK		= 15,
	MCT_ADD_CASH_ROLLBACK				= 16,
	MCT_GAME_CS_BUY_ARTICLE_ROLLBACK	= 17,
	MCT_GAME_Cash_Pack_ITEM	= 18,
	MCT_GAME_Cash_Pack_ITEM_ROLLBACK = 19,
	MCT_GAME_OM_REG_CASH				= 20,
	MCT_GAME_OM_REG_CASH_ROLLBACK		= 21,
	MCT_GAME_OM_UNREG_CASH				= 22,
	MCT_GAME_OM_UNREG_CASH_ROLLBACK		= 23,
	MCT_GAME_OM_PAYBACK					= 24,
	MCT_GAME_OM_PAYBACK_ROLLBACK		= 25,
	MCT_GAME_OM_BUYCASH					= 26,
	MCT_GAME_OM_BUYCASH_ROLLBACK		= 27,
	MCT_GAME_GM_PAY						= 28,// 인 게임 가챠 캐시소비
}EMODIFY_CASH_TYPE;

//79000030 환혼
int const ITEM_SOUL_NO = 79000030;

int const SAFE_FOAM_EFFECTNO = 402001;

typedef enum eCharacterInDBState // DB, DR2_User.dbo.TB_CharacterBasic 테이블의 State 필드 값에 대한 Enum 정의
{
	CIDBS_Normal				= 0x00,
	CIDBS_Deleted				= 0x01,
	CIDBS_NeedRename			= 0x04,
	CIDBS_ReserveDelete			= 0x08,
	CIDBS_NeedRestore			= 0x10, // Need resetore after quest
} ECharacterInDBState;

typedef enum eUserCharacterRealmMergeReturn
{
	UCRMR_None			= 0,
	UCRMR_Success		= 1,
	UCRMR_BadName		= 2,
	UCRMR_Duplicate		= 3,
	UCRMR_MaxLen		= 4,
	UCRMR_Failed		= 5,
	UCRMR_MaxSlot		= 6,
	UCRMR_EmptyName		= 7,
} EUserCharacterRealmMergeReturn;

int const MAX_OPENMARKET_ARTICLE_NUM = 20;
int const MAX_ITEM_MAKING_NUM = 100;

typedef struct tagSAuthRequest
{
	tagSAuthRequest()
	{}

	explicit tagSAuthRequest(std::wstring const & _ID, std::wstring const& _PW, BM::GUID const& _Request, CEL::ADDR_INFO const & _Address )
	{
		strID = _ID;
		strPW = _PW;
		kRequestGuid = _Request;
		kAddress = _Address;
		iUID = 0;
		byGMLevel = 0;
		bIsPCCafe = false;
		byPCCafeGrade = 0;
	}

	tagSAuthRequest const& operator=(tagSAuthRequest const& rhs)
	{
		strID = rhs.strID;
		strPW = rhs.strPW;
		strErrorMsg = rhs.strErrorMsg;
		kRequestGuid = rhs.kRequestGuid;
		kAddress = rhs.kAddress;
		iUID = rhs.iUID;
		byGMLevel = rhs.byGMLevel;
		bIsPCCafe = rhs.bIsPCCafe;
		byPCCafeGrade = rhs.byPCCafeGrade;
		return (*this);
	}

	void WriteToPacket(BM::Stream& rkPacket) const
	{
		rkPacket.Push(strID);
		rkPacket.Push(strPW);
		rkPacket.Push(strErrorMsg);
		rkPacket.Push(kRequestGuid);
		kAddress.WriteToPacket(rkPacket);
		rkPacket.Push(iUID);
		rkPacket.Push(byGMLevel);
		rkPacket.Push(bIsPCCafe);
		rkPacket.Push(byPCCafeGrade);
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(strID);
		rkPacket.Pop(strPW);
		rkPacket.Pop(strErrorMsg);
		rkPacket.Pop(kRequestGuid);
		kAddress.ReadFromPacket(rkPacket);
		rkPacket.Pop(iUID);
		rkPacket.Pop(byGMLevel);
		rkPacket.Pop(bIsPCCafe);
		rkPacket.Pop(byPCCafeGrade);
	}

	std::wstring strID;
	std::wstring strPW;
	std::wstring strErrorMsg;
	CEL::ADDR_INFO kAddress;
	int iUID;
	BYTE byGMLevel;
	BM::GUID kRequestGuid;
	bool bIsPCCafe; // 가맹PC방 접속?
	BYTE byPCCafeGrade;
} SAuthRequest;

typedef struct tagSAuthSyncData
{
	tagSAuthSyncData()
	{}

	explicit tagSAuthSyncData( std::wstring const & _ID, std::wstring const& _PW, BM::GUID const& _Request )
		:	strID(_ID)
		,	strPW(_PW)
		,	kRequestGuid(_Request)
	{
	}

	tagSAuthSyncData const& operator=(tagSAuthSyncData const& rhs)
	{
		strID = rhs.strID;
		strPW = rhs.strPW;
		kRequestGuid = rhs.kRequestGuid;
		return (*this);
	}

	void WriteToPacket(BM::Stream& rkPacket) const
	{
		rkPacket.Push(strID);
		rkPacket.Push(strPW);
		rkPacket.Push(kRequestGuid);
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(strID);
		rkPacket.Pop(strPW);
		rkPacket.Pop(kRequestGuid);
	}

	std::wstring strID;
	std::wstring strPW;
	BM::GUID kRequestGuid;
} SAuthSyncData;

enum EUnicodeFilterFunctionCode
{
	UFFC_CHARACTER_NAME				= 0x00000001,
	UFFC_GUILD_NAME					= 0x00000002,
	UFFC_PARTY_NAME					= 0x00000004,
	UFFC_PVP_ROOM_NAME				= 0x00000008,
	UFFC_OPEM_MARKET_NAME			= 0x00000010,
	UFFC_CHAR_CARD_COMMENT			= 0x00000020,
	UFFC_GUILD_NOTICE				= 0x00000040,
	UFFC_MISSION_RANK_COMMENT		= 0x00000080,
	UFFC_MAIL_SUBJECT				= 0x00000100,
	UFFC_MAIL_CONTENTS				= 0x00000200,
	UFFC_CASHSHOP_PRESENT_COMMENT	= 0x00000400,
	UFFC_CHAT						= 0x00000800,
};

size_t const MAX_BEST_MARKET_LIST_NUM = 100;

typedef struct tagReqGiveLimitedItem
{
	tagReqGiveLimitedItem()
		:	m_nOwnerLevel(0)
		,	m_kTryLimitEventNo(0)
	{}
	SGroundKey m_kGndKey;
	POINT3 m_kptPos;
	BM::GUID m_kOwnerGuid;
	short m_nOwnerLevel;
	int m_kTryLimitEventNo;
}SREQ_GIVE_LIMITED_ITEM;

typedef struct _BuyItemInfo {
	_BuyItemInfo()
	{
		iItemNo = 0;
		iItemHour = 0;
		i64Cash = i64Mileage = 0i64;
		iPriceIdx = 0;
	}

	_BuyItemInfo const& operator=(_BuyItemInfo const& rhs)
	{
		iPriceIdx = rhs.iPriceIdx;
		iItemNo = rhs.iItemNo;
		strItemName = rhs.strItemName;
		iItemHour = rhs.iItemHour;
		i64Cash = rhs.i64Cash;
		i64Mileage = i64Mileage;
		return (*this);
	}

	void ReadFromPacket(BM::Stream& kPacket)
	{
		kPacket.Pop(iItemNo);
		kPacket.Pop(strItemName);
		kPacket.Pop(iItemHour);
		kPacket.Pop(i64Cash);
		kPacket.Pop(i64Mileage);
		kPacket.Pop(iPriceIdx);
	}

	void WriteToPacket(BM::Stream& kPacket) const
	{
		kPacket.Push(iItemNo);
		kPacket.Push(strItemName);
		kPacket.Push(iItemHour);
		kPacket.Push(i64Cash);
		kPacket.Push(i64Mileage);
		kPacket.Push(iPriceIdx);
	}

	size_t min_size()const
	{
		return sizeof(int) * 3 + sizeof(__int64) * 2;
	}

	void Log(BM::vstring &vLogString) const
	{
		vLogString << _T("Item=") << iItemNo << _COMMA_ << strItemName << _T(", ItemHour=") << iItemHour
			<< _T(", Cash=") << i64Cash << _T(", Mileage=") << i64Mileage << (", PriceIdx=") << iPriceIdx;
	}

	int iItemNo;
	std::wstring strItemName;
	int iItemHour;	// 기간제아이템의 시간값
	__int64 i64Cash;	//가격
	__int64 i64Mileage;	// 마일리지 가격
	int iPriceIdx;	// 가격 테이블 인덱스
} SBuyItemInfo;
typedef std::vector<SBuyItemInfo> CONT_BUYITEM;

typedef struct tagFAVORITEHOME
{
	tagFAVORITEHOME():m_kVisitCount(0){}
	CLASS_DECLARATION_S(int,VisitCount);
	CLASS_DECLARATION_S(std::wstring,Name);
	size_t min_size()const
	{
		return sizeof(int) + sizeof(size_t);
	}
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(m_kVisitCount);
		kPacket.Push(m_kName);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kVisitCount);
		kPacket.Pop(m_kName);
	}
}SFAVORITEHOME;

typedef std::map<BM::GUID,SFAVORITEHOME> CONT_FAVORITE;

typedef struct tagTODAYVISITOR
{
	tagTODAYVISITOR():m_kHasMyHome(false){}
	CLASS_DECLARATION_S(BM::GUID,OwnerGuid);
	CLASS_DECLARATION_S(std::wstring,Name);
	CLASS_DECLARATION_S(bool,HasMyHome);
	size_t min_size()const
	{
		return sizeof(BM::GUID) + sizeof(bool) + sizeof(size_t);
	}
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(m_kOwnerGuid);
		kPacket.Push(m_kName);
		kPacket.Push(m_kHasMyHome);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kOwnerGuid);
		kPacket.Pop(m_kName);
		kPacket.Pop(m_kHasMyHome);
	}
}STODAYVISITOR;

typedef std::map<BM::PgPackedTime,STODAYVISITOR> CONT_TODAYVISITOR;

typedef struct tagVISITORCOMMENT
{
	CLASS_DECLARATION_S(std::wstring,Name);
	CLASS_DECLARATION_S(std::wstring,Text);
	CLASS_DECLARATION_S(BM::PgPackedTime,Time);
	size_t min_size()const
	{
		return sizeof(size_t) + sizeof(size_t) + sizeof(BM::PgPackedTime);
	}
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(m_kName);
		kPacket.Push(m_kText);
		kPacket.Push(m_kTime);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kName);
		kPacket.Pop(m_kText);
		kPacket.Pop(m_kTime);
	}
}SVISITORCOMMENT;

typedef std::map<BM::GUID,SVISITORCOMMENT> CONT_VISITORCOMMENT;

typedef enum E_EANABLE_VISITOR_BIT : BYTE
{
	EVB_ALL		= 0,
	EVB_FRIEND	= 0x01,
	EVB_COUPLE	= 0x02,
	EVB_GUILD	= 0x04,
	EVB_PARTY	= 0x08,
}eEanbleVisitorBit;

typedef struct tagEQUIPMYHOMEITEM
{
	int		m_kItemNo;
	POINT3	m_kPos;
	short	m_kDir;

	size_t min_size()const
	{
		return sizeof(int) + sizeof(POINT3) + sizeof(short);
	}
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(m_kItemNo);
		kPacket.Push(m_kPos);
		kPacket.Push(m_kDir);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_kItemNo);
		kPacket.Pop(m_kPos);
		kPacket.Pop(m_kDir);
	}
}SEQUIPMYHOMEITEM;

typedef std::map<BM::GUID,SEQUIPMYHOMEITEM> CONT_EQUIPMYHOMEITEM;

typedef enum E_MYHOME_AUCTION_STATE : BYTE
{
	MAS_NOT_BIDDING	= 0,	// 경매에 참가되지 않은 집
	MAS_IS_BIDDING	= 1,	// 경매 진행중인 집
	MAS_IS_BLOCK	= 2,	// 세금 미납으로 블럭된 집
}eMyHomeAuctionState;

typedef enum E_MYHOME_ENABLE_VISIT : BYTE
{
	MEV_ONLY_OWNER	= 0x00,
	MEV_COUPLE		= 0x01,
	MEV_GUILD		= 0x02,
	MEV_FRIEND		= 0x04,
	MEV_ALL			= 0x08, // 방문만 가능한 유저
	MEV_VIP_COUPLE	= 0x10,
	MEV_VIP_GUILD	= 0x20,
	MEV_VIP_FRIEND	= 0x40,
	MEV_VIP_ALL		= 0x80, // 효과를 사용 가능한 유저
}eMyHomeEanbleVisit;

typedef enum E_MYHOME_SIDE_JOB
{
	MSJ_NONE		= 0x00000000,
	MSJ_REPAIR		= 0x00000001,
	MSJ_ENCHANT		= 0x00000002,
	MSJ_SOULCRAFT	= 0x00000004,
	MSJ_SOCKET		= 0x00000008,
}eMyHomeSideJob;

typedef enum E_MYHOME_SIDE_JOB_RATE_TYPE
{
	MSJRT_NONE = 0,
	MSJRT_GOLD = 1,
	MSJRT_SOUL = 2,
	MSJRT_RATE = 3,
}eMyHomeSideJobRateType;

typedef struct tagMYHOME_SIDE_JOB
{
	tagMYHOME_SIDE_JOB():kJobRateType(MSJRT_NONE), iJobRate(0), i64TotalSellCost(0){}
	explicit tagMYHOME_SIDE_JOB(eMyHomeSideJobRateType const __jobratetype, int const __jobrate, BM::PgPackedTime const & __endtime, __int64 const __sellcost = 0):
	kJobRateType(__jobratetype), iJobRate(__jobrate), kEndTime(__endtime), i64TotalSellCost(__sellcost){}
	eMyHomeSideJobRateType	kJobRateType;
	int						iJobRate;
	BM::PgPackedTime		kEndTime;
	__int64					i64TotalSellCost;
}SMYHOME_SIDE_JOB;

typedef std::map<eMyHomeSideJob,SMYHOME_SIDE_JOB> CONT_MYHOME_SIDE_JOB;

typedef struct tagMYHOME
{
	tagMYHOME():
	siStreetNo(0),
	iHouseNo(0),
	iHomeColor(0),
	iHomeStyle(0),
	i64HomePrice(0),
	bAuctionState(0),
	iGroundNo(0),
	bEnableVisitBit(0),
	i64FirstBiddingCost(0),
	i64LastBiddingCost(0),
	iHomeFence(0),
	iHomeGarden(0),
	iEquipItemCount(0),
	iTotalVisitLogCount(0),
	iTodayHitCount(0),
	iTotalHitCount(0)
	{
		kBidderGuid.Clear();
	}

	short					siStreetNo;
	int						iHouseNo;
	int						iHomeColor;
	int						iHomeStyle;
	int						iHomeFence;
	int						iHomeGarden;
	int						iRoomWall;
	int						iRoomFloor;
	int						iLvLimitMin,
							iLvLimitMax;
	__int64					i64HomePrice;
	BM::PgPackedTime		kAuctionTime;
	BYTE					bAuctionState;
	int						iGroundNo;
	BM::GUID				kHomeInSideGuid;
	BM::GUID				kHomeOutSideGuid;
	BM::GUID				kOwnerGuid;
	std::wstring			kName;
	BYTE					bEnableVisitBit;
	BM::GUID				kBidderGuid;
	__int64					i64FirstBiddingCost;
	__int64					i64LastBiddingCost;
	BM::PgPackedTime		kPayTexTime;
	BM::PgPackedTime		kPayTexNotiTime;
	std::wstring			kBidderName;

	int						iEquipItemCount;
	CONT_MYHOME_SIDE_JOB	kContSideJob;
	int						iTotalVisitLogCount;
	int						iTodayHitCount;
	int						iTotalHitCount;

	int SideJobFlag() const
	{
		int iJobFlag = 0;
		for(CONT_MYHOME_SIDE_JOB::const_iterator job_iter = kContSideJob.begin();job_iter != kContSideJob.end();++job_iter)
		{
			iJobFlag |= (*job_iter).first;
		}
		return iJobFlag;
	}

	size_t min_size()const
	{
		return	sizeof(short) + 
				sizeof(int) + 
				sizeof(int) + 
				sizeof(int) + 
				sizeof(int) + 
				sizeof(int) + 
				sizeof(int) + 
				sizeof(int) + 
				sizeof(int) + 
				sizeof(int) + 
				sizeof(__int64) + 
				sizeof(BM::PgPackedTime) + 
				sizeof(BYTE) + 
				sizeof(int) + 
				sizeof(BM::GUID) +
				sizeof(BM::GUID) +
				sizeof(BM::GUID) +
				sizeof(size_t) +
				sizeof(BYTE) +
				sizeof(BM::GUID) +
				sizeof(__int64) +
				sizeof(__int64) +
				sizeof(BM::PgPackedTime) +
				sizeof(BM::PgPackedTime) +
				sizeof(size_t) +
				sizeof(int) +
				sizeof(size_t) +
				sizeof(int) + 
				sizeof(int) +
				sizeof(int);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(siStreetNo);
		kPacket.Push(iHouseNo);
		kPacket.Push(iHomeColor);
		kPacket.Push(iHomeStyle);
		kPacket.Push(iHomeFence);
		kPacket.Push(iHomeGarden);
		kPacket.Push(iRoomWall);
		kPacket.Push(iRoomFloor);
		kPacket.Push(iLvLimitMin);
		kPacket.Push(iLvLimitMax);
		kPacket.Push(i64HomePrice);
		kPacket.Push(kAuctionTime);
		kPacket.Push(bAuctionState);
		kPacket.Push(iGroundNo);
		kPacket.Push(kHomeInSideGuid);
		kPacket.Push(kHomeOutSideGuid);
		kPacket.Push(kOwnerGuid);
		kPacket.Push(kName);
		kPacket.Push(bEnableVisitBit);
		kPacket.Push(kBidderGuid);
		kPacket.Push(i64FirstBiddingCost);
		kPacket.Push(i64LastBiddingCost);
		kPacket.Push(kPayTexTime);
		kPacket.Push(kPayTexNotiTime);
		kPacket.Push(kBidderName);
		kPacket.Push(iEquipItemCount);
		kPacket.Push(iTotalVisitLogCount);
		kPacket.Push(iTodayHitCount);
		kPacket.Push(iTotalHitCount);

		PU::TWriteTable_AA(kPacket,kContSideJob);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(siStreetNo);
		kPacket.Pop(iHouseNo);
		kPacket.Pop(iHomeColor);
		kPacket.Pop(iHomeStyle);
		kPacket.Pop(iHomeFence);
		kPacket.Pop(iHomeGarden);
		kPacket.Pop(iRoomWall);
		kPacket.Pop(iRoomFloor);
		kPacket.Pop(iLvLimitMin);
		kPacket.Pop(iLvLimitMax);
		kPacket.Pop(i64HomePrice);
		kPacket.Pop(kAuctionTime);
		kPacket.Pop(bAuctionState);
		kPacket.Pop(iGroundNo);
		kPacket.Pop(kHomeInSideGuid);
		kPacket.Pop(kHomeOutSideGuid);
		kPacket.Pop(kOwnerGuid);
		kPacket.Pop(kName);
		kPacket.Pop(bEnableVisitBit);
		kPacket.Pop(kBidderGuid);
		kPacket.Pop(i64FirstBiddingCost);
		kPacket.Pop(i64LastBiddingCost);
		kPacket.Pop(kPayTexTime);
		kPacket.Pop(kPayTexNotiTime);
		kPacket.Pop(kBidderName);
		kPacket.Pop(iEquipItemCount);
		kPacket.Pop(iTotalVisitLogCount);
		kPacket.Pop(iTodayHitCount);
		kPacket.Pop(iTotalHitCount);

		kContSideJob.clear();
		PU::TLoadTable_AA(kPacket,kContSideJob);
	}

	void MyHomeReadFromDB(CEL::DB_DATA_ARRAY::iterator & iter)
	{
		(*iter).Pop(siStreetNo);		++iter;
		(*iter).Pop(iHouseNo);			++iter;
		(*iter).Pop(iHomeColor);		++iter;
		(*iter).Pop(iHomeStyle);		++iter;
		(*iter).Pop(iHomeFence);		++iter;
		(*iter).Pop(iHomeGarden);		++iter;
		(*iter).Pop(iRoomWall);			++iter;
		(*iter).Pop(iRoomFloor);		++iter;
		(*iter).Pop(i64HomePrice);		++iter;
		
		BM::DBTIMESTAMP_EX kTime;
		(*iter).Pop(kTime);				++iter;	
		if(true == kTime.IsNull())
		{
			kTime.SetLocalTime();
		}
		kAuctionTime = static_cast<BM::PgPackedTime>(kTime);

		(*iter).Pop(bAuctionState);		++iter;
		(*iter).Pop(iGroundNo);			++iter;
		(*iter).Pop(kHomeInSideGuid);	++iter;
		(*iter).Pop(kHomeOutSideGuid);	++iter;
		(*iter).Pop(kOwnerGuid);		++iter;
		(*iter).Pop(kName);				++iter;
		(*iter).Pop(bEnableVisitBit);	++iter;
		(*iter).Pop(kBidderGuid);		++iter;
		(*iter).Pop(i64FirstBiddingCost);++iter;
		(*iter).Pop(i64LastBiddingCost);++iter;
		(*iter).Pop(kTime);				++iter; 
		if(true == kTime.IsNull())
		{
			kTime.SetLocalTime();
		}
		kPayTexTime = static_cast<BM::PgPackedTime>(kTime);
		(*iter).Pop(kTime);				++iter; 
		if(true == kTime.IsNull())
		{
			kTime.SetLocalTime();
		}
		kPayTexNotiTime = static_cast<BM::PgPackedTime>(kTime);

		(*iter).Pop(kBidderName);		++iter;
		(*iter).Pop(iLvLimitMin);		++iter;
		(*iter).Pop(iLvLimitMax);		++iter;
		(*iter).Pop(iTotalVisitLogCount);++iter;
		(*iter).Pop(iTodayHitCount);	++iter;
		(*iter).Pop(iTotalHitCount);	++iter;
	}
}SMYHOME;

typedef std::map<SHOMEADDR,SMYHOME>	CONT_MYHOME;

int const DEFAULT_ENABLE_USE_EXPCARD_NUM = 2;

int const DEFAULT_MILEAGE_USECASH_100 = 100;
int const DEFAULT_MILEAGE_USECASH_1000 = 1000;

int const DEFAULT_CALC_MILEAGE_100 = 3;
int const DEFAULT_CALC_MILEAGE_1000 = 10;

short const MAX_MARKET_ARITCLE_MONEY_COUNT = 30000;
int const MARKET_ARTICLE_MONEYNO = 73000220;// 마켓 등록용 머니 아이템 번호
int const MARKET_ARTICLE_CASHNO = 73000230;// 마켓 등록용 캐시 아이템 번호


int const MAX_HOME_VISITLOG_LEN = 80;

typedef struct tagHOMEVISITLOG
{
	BM::GUID kLogGuid;
	BM::GUID kVisitorGuid;
	std::wstring kVisitorName;
	std::wstring kVisitLog;
	BM::PgPackedTime kLogTime;
	bool bPrivate;

	tagHOMEVISITLOG():bPrivate(false){}

	size_t min_size() const
	{
		return sizeof(BM::GUID) + sizeof(BM::GUID) + sizeof(size_t) + sizeof(size_t) + sizeof(BM::PgPackedTime);
	}

	void WriteToPacket(BM::Stream & rkPacket) const
	{
		rkPacket.Push(kLogGuid);
		rkPacket.Push(kVisitorGuid);
		rkPacket.Push(kVisitorName);
		rkPacket.Push(kVisitLog);
		rkPacket.Push(kLogTime);
		rkPacket.Push(bPrivate);
	}

	void ReadFromPacket(BM::Stream & rkPacket)
	{
		rkPacket.Pop(kLogGuid);
		rkPacket.Pop(kVisitorGuid);
		rkPacket.Pop(kVisitorName);
		rkPacket.Pop(kVisitLog);
		rkPacket.Pop(kLogTime);
		rkPacket.Pop(bPrivate);
	}
}SHOMEVISITLOG;

typedef std::list<SHOMEVISITLOG> CONT_HOMEVISITLOG;

typedef enum E_MYHOME_AUCTION_TEX_RATE
{
	MATR_REG = 5,					// 경매 등록 수수료
	MATR_END = 10,					// 경매 종료 수수료
	MATR_CANCEL = 50,				// 경매 취소 수수료
	MATR_WEEK = 5,					// 방세 10%
	MATR_ATTACHMENT = 30,			// 차압 되면 30%
}eMyHomeAuctionTexRate;

__int64 const MYHOME_TEX_LIMIT = 3000000;// 마이홈 세금징수 기준 홈 가격(300 골드)
__int64 const MYHOME_TEX_MIN = 300000; // 마이홈 MYHOME_TEX_LIMIT 이하 징수 세금(30 골드)

typedef enum E_MYHOME_AUCTION_TIME
{
	MAT_6	= 6,
	MAT_12	= 12,
	MAT_18	= 18,
	MAT_1D	= 24,
	MAT_6D	= 24*6,
}eMyHomeAuctionTime;

typedef enum E_MYHOME_MAIL_TEXT
{
	MMT_BIDDING_RETURN_MAIL_FROM		= 2000000001,	// 입찰 실패로 환불
	MMT_BIDDING_RETURN_MAIL_TITLE		= 2000000002,
	MMT_BIDDING_RETURN_MAIL_TEXT		= 2000000003,

	MMC_AUCTION_SUCCESS_MAIL_FROM		= 2000000004,	// 경매 성공
	MMC_AUCTION_SUCCESS_MAIL_TITLE		= 2000000005,
	MMC_AUCTION_SUCCESS_MAIL_TEXT		= 2000000006,

	MMC_TEX_NOTI_MAIL_FROM				= 2000000007,	// 세금 지급 통지
	MMC_TEX_NOTI_MAIL_TITLE				= 2000000008,
	MMC_TEX_NOTI_MAIL_TEXT				= 2000000009,

	MMC_BIDDING_SUCCESS_MAIL_FROM		= 2000000013,	// 입찰 성공
	MMC_BIDDING_SUCCESS_MAIL_TITLE		= 2000000014,
	MMC_BIDDING_SUCCESS_MAIL_TEXT		= 2000000015,

	MMC_AUCTION_FAIL_MAIL_FROM			= 2000000016,	// 경매 실패
	MMC_AUCTION_FAIL_MAIL_TITLE			= 2000000017,
	MMC_AUCTION_FAIL_MAIL_TEXT			= 2000000018,

	MMC_ATTACHMENT_NOTI_MAIL_FROM		= 2000000019,	// 차압 통보
	MMC_ATTACHMENT_NOTI_MAIL_TITLE		= 2000000020,
	MMC_ATTACHMENT_NOTI_MAIL_TEXT		= 2000000021,

	MMC_SIDEJOB_NOTI_MAIL_FROM			= 2000000022,	// 아르바이트 통보
	MMC_SIDEJOB_NOTI_MAIL_TITLE			= 2000000023,
	MMC_SIDEJOB_NOTI_MAIL_TEXT			= 2000000024,

	MMC_SELL_NOTI_MAIL_FROM				= 2000000019,
	MMC_SELL_NOTI_MAIL_TITLE			= 2000000035,
	MMC_SELL_NOTI_MAIL_TEXT				= 2000000036,
}eMyHomeMailText;

enum EQuestShareRet
{
	QSR_None			= 0,
	QSR_Success			= 1,
	QSR_CantShare		= 2,
	QSR_OnlyParty		= 3,
	QSR_OnlyHave		= 4,
	QSR_Notify			= 5,
	QSR_Failed			= 6,
	QSR_Max				= 7,
	QSR_Ing				= 8,
	QSR_No				= 9,
	QSR_Yes				= 10,
};
struct SQuestShareRet
{
	SQuestShareRet()
		: kGuid(), cRet()
	{
	}

	SQuestShareRet(BM::GUID const& rkGuid, BYTE const Ret)
		: kGuid(rkGuid), cRet(Ret)
	{
	}
	SQuestShareRet(SQuestShareRet const& rhs)
		: kGuid(rhs.kGuid), cRet(rhs.cRet)
	{
	}

	BM::GUID kGuid;
	BYTE cRet;
};
typedef std::vector< SQuestShareRet > ContQuestShareRet;

int const MYHOME_PAY_TEX_TIME = 7 * 24 * 60 * 60; // 마이홈 세금 납입기간 7일
int const MAX_MYHOME_PAY_TEX_TIME = 4 * MYHOME_PAY_TEX_TIME; // 마이홈 강제 차압 기간 4주
int const DEFAULT_MYHOME_AUCTION_REG_TIME = MAT_6D * 60 * 60; // 시스템에서 강제로 입력하는 경매 기간
int const MYHOME_CLASS_INSIDE_NO = 1000; // 마이홈 내부 유닛 클레스 번호
int const MYHOME_CLASS_OUTSIDE_NO = 1001; // 마이홈 외부 유닛 클레스 번호
int const NEXT_BIDDING_COST_RATE = 10;// 입찰 금액 증가율
int const UPTOGOLD = 9999;	// 골드 -> 동화 환산

typedef struct tagINVITATIONCARD
{
	std::wstring		kHomeOwnerName;
	short				siStreetNo;
	int					iHouseNo;
	BM::PgPackedTime	kLimitTime;
	tagINVITATIONCARD():siStreetNo(0),iHouseNo(0){}

	size_t min_size() const
	{
		return sizeof(size_t) + sizeof(short) + sizeof(int) + sizeof(BM::PgPackedTime);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(kHomeOwnerName);
		kPacket.Push(siStreetNo);
		kPacket.Push(iHouseNo);
		kPacket.Push(kLimitTime);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kHomeOwnerName);
		kPacket.Pop(siStreetNo);
		kPacket.Pop(iHouseNo);
		kPacket.Pop(kLimitTime);
	}
}SINVITATIONCARD;

typedef std::list<SINVITATIONCARD> CONT_INVITATIONCARD;

typedef struct tagVISITORINFO
{
	BM::GUID kOwnerGuid;
	std::wstring kName;
	BM::PgPackedTime kVistTime;
	SHOMEADDR	kAddr;

	size_t min_size() const
	{
		return sizeof(BM::GUID) + sizeof(size_t) + sizeof(BM::PgPackedTime) + sizeof(SHOMEADDR);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(kOwnerGuid);
		kPacket.Push(kName);
		kPacket.Push(kVistTime);
		kPacket.Push(kAddr);
	}

	void ReadFromPacket(BM::Stream & kPacket) 
	{
		kPacket.Pop(kOwnerGuid);
		kPacket.Pop(kName);
		kPacket.Pop(kVistTime);
		kPacket.Pop(kAddr);
	}
}SVISITORINFO;

typedef std::list<SVISITORINFO> CONT_VISITORINFO;

int const DEFAULT_USER_MARKET_USEABLE_CASH = 1;// 기본적으로 오픈마켓에서 캐시로 물품 매매가 가능하다.

typedef tagQuadKey< int, int, int, int > SItemExtOptionKey;//타입, pos, lv, 그룹


typedef enum : BYTE
{
	ERegenBag_Type_None				= 0,
	ERegenBag_Type_Monster			= 1,
	ERegenBag_Type_OjbUnit			= 2,
	ERegenBag_Type_InstanceItem		= 3,
	ERegenBag_Type_SummonNPC		= 4,
} ERegenBagControlType;

typedef struct tagACHIEVEMENTRANK
{
	std::wstring kName;
	int iRank;
	int iTotalPoint;
	std::map<int,int> kContPoint;

	tagACHIEVEMENTRANK():iRank(0),iTotalPoint(0){}

	size_t min_size() const
	{
		return sizeof(size_t) + sizeof(int) + sizeof(int) + sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & rkPacket)const
	{
		rkPacket.Push(kName);
		rkPacket.Push(iRank);
		rkPacket.Push(iTotalPoint);
		PU::TWriteTable_AA(rkPacket,kContPoint);
	}

	void ReadFromPacket(BM::Stream & rkPacket)
	{
		kContPoint.clear();
		rkPacket.Pop(kName);
		rkPacket.Pop(iRank);
		rkPacket.Pop(iTotalPoint);
		PU::TLoadTable_AA(rkPacket,kContPoint);
	}
}SACHIEVEMENTRANK;

typedef std::map<BM::GUID,SACHIEVEMENTRANK> CONT_ACHIEVEMENT_RANK;


typedef struct tagPLAYERPLAYTIMEINFO_SUB
{
	tagPLAYERPLAYTIMEINFO_SUB() :bType(0),bTypeSub(0),
		iValue1(0),iValue2(0),iValue3(0)
	{}
	
	BYTE bType;
	BYTE bTypeSub;
	int iValue1;
	int iValue2;
	int iValue3;

	size_t min_size()const
	{
		return sizeof(bType)+
			   sizeof(bTypeSub)+
			   sizeof(iValue1)+
			   sizeof(iValue2)+
			   sizeof(iValue3);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(bType);
		kPacket.Push(bTypeSub);
		kPacket.Push(iValue1);
		kPacket.Push(iValue2);
		kPacket.Push(iValue3);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(bType);
		kPacket.Pop(bTypeSub);
		kPacket.Pop(iValue1);
		kPacket.Pop(iValue2);
		kPacket.Pop(iValue3);
	}
}SPLAYERPLAYTIMEINFO_SUB;

typedef struct tagPLAYERPLAYTIMEINFO
{
	typedef std::vector<SPLAYERPLAYTIMEINFO_SUB> CONT_SUB;

	tagPLAYERPLAYTIMEINFO() :bResetType(0),iResetValue1(0),sApplyUser(0),iPlayTime(0),iFlag(0),bUse(0) {}

	BYTE bResetType;
	int iResetValue1;
	BM::PgPackedTime kResetValue2;
	short sApplyUser;
	int iPlayTime;
	int iFlag;
	BYTE bUse;
	BM::PgPackedTime kLastApply;
	
	CONT_SUB kContSub;

	size_t min_size()const
	{
		return sizeof(bResetType)+
			   sizeof(iResetValue1)+
			   sizeof(kResetValue2)+
			   sizeof(sApplyUser)+
			   sizeof(iPlayTime)+
			   sizeof(iFlag)+
			   sizeof(bUse)+
			   sizeof(kLastApply);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(bResetType);
		kPacket.Push(iResetValue1);
		kPacket.Push(kResetValue2);
		kPacket.Push(sApplyUser);
		kPacket.Push(iPlayTime);
		kPacket.Push(iFlag);
		kPacket.Push(bUse);
		kPacket.Push(kLastApply);
		kPacket.Push(kContSub);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(bResetType);
		kPacket.Pop(iResetValue1);
		kPacket.Pop(kResetValue2);
		kPacket.Pop(sApplyUser);
		kPacket.Pop(iPlayTime);
		kPacket.Pop(iFlag);
		kPacket.Pop(bUse);
		kPacket.Pop(kLastApply);
		kPacket.Pop(kContSub);
	}	

	void Clear()
	{
		bResetType = 0;
		iResetValue1 = 0;
		kResetValue2.Clear();
		sApplyUser = 0;
		iPlayTime = 0;
		iFlag = 0;
		bUse = false;
		kLastApply.Clear();
		kContSub.clear();
	}
}SPLAYERPLAYTIMEINFO;

//
typedef enum eWorldEnvironmentStatusType
{
	WEST_NONE		= 0,
	WEST_SNOW		= 1,
	WEST_SAKURA		= 2,
	WEST_SUNFLOWER	= 3,
	WEST_ICECREAM	= 4,
	WEST_ETC3		= 5,
	WEST_ETC4		= 6,
	WEST_MAX,				// 이빨 빠짐/중복 enum값 주의 (최대 사용 가능 값 : 31)
	// 최대 31번 까지 사용가능
} EWorldEnvironmentStatusType;

//
extern char const* szWORLD_ENVIRONMENT_STATUS_ELEMENT_NAME;
class TiXmlElement;
typedef struct tagWorldEnvironmentStatus
{
	tagWorldEnvironmentStatus();
	tagWorldEnvironmentStatus(EWorldEnvironmentStatusType const& reType, __int64 const& riStartGameTime, __int64 const& riDurationSec);
	tagWorldEnvironmentStatus(tagWorldEnvironmentStatus const& rhs);
	~tagWorldEnvironmentStatus();

	bool ParseXml(TiXmlElement const* pkRootNode);
	bool IsCanTime(__int64 const iCurGameTime) const;
	bool ReadFromPacket(BM::Stream& rkPacket);
	void WriteToPacket(BM::Stream& rkPacket);

	EWorldEnvironmentStatusType eType;
	__int64 iStartGameTime;
	__int64 iDurationSec;
} SWorldEnvironmentStatus;
typedef std::map< EWorldEnvironmentStatusType, SWorldEnvironmentStatus > CONT_WORLD_ENVIRONMENT_STATUS;
typedef std::set< EWorldEnvironmentStatusType > CONT_WORLD_ENVIRONMENT_STATUS_TYPE;

//
typedef struct tagWorldEnvironmentStatusBitFlag
{
	tagWorldEnvironmentStatusBitFlag();
	tagWorldEnvironmentStatusBitFlag(DWORD const& rdwEnvStatus);
	tagWorldEnvironmentStatusBitFlag(tagWorldEnvironmentStatusBitFlag const& rhs);
	tagWorldEnvironmentStatusBitFlag(CONT_WORLD_ENVIRONMENT_STATUS const& rkCont);
	void Set(EWorldEnvironmentStatusType const eType);
	bool Is(EWorldEnvironmentStatusType const eType) const;
	void Compare(DWORD const& dwOldEnvStatus, CONT_WORLD_ENVIRONMENT_STATUS_TYPE& rkContAdd, CONT_WORLD_ENVIRONMENT_STATUS_TYPE& rkContDel) const;
	void Update(CONT_WORLD_ENVIRONMENT_STATUS const& rkCont);
	static bool Is(DWORD const dwEnvStatus, EWorldEnvironmentStatusType const eType);
	static void Compare(DWORD const dwOldEnvStatus, DWORD const dwNewEnvStatus, CONT_WORLD_ENVIRONMENT_STATUS_TYPE& rkContAdd, CONT_WORLD_ENVIRONMENT_STATUS_TYPE& rkContDel);

	DWORD dwEnvStatus;
} SWorldEnvironmentStatusBitFlag;

//
typedef struct tagRealmQuestInfo
{
	tagRealmQuestInfo()
		: kRealmQuestID(), bCurStatus(false), iCurCount(0), iNextCount(0)
	{
	}
	tagRealmQuestInfo(std::wstring const& rkRealmQuestID, bool const& rbCurStatus, size_t const& riCurCount, size_t const& riNextCount)
		: kRealmQuestID(rkRealmQuestID), bCurStatus(rbCurStatus), iCurCount(riCurCount), iNextCount(riNextCount)
	{
	}
	tagRealmQuestInfo(BM::Stream& rkPacket)
	{
		ReadFromPacket(rkPacket);
	}
	tagRealmQuestInfo(tagRealmQuestInfo const& rhs)
		: kRealmQuestID(rhs.kRealmQuestID), bCurStatus(rhs.bCurStatus), iCurCount(rhs.iCurCount), iNextCount(rhs.iNextCount)
	{
	}
	~tagRealmQuestInfo()
	{
	}

	void WriteToPacket(BM::Stream& rkPacket) const
	{
		rkPacket.Push( kRealmQuestID );
		rkPacket.Push( bCurStatus );
		rkPacket.Push( iCurCount );
		rkPacket.Push( iNextCount );
	}
	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop( kRealmQuestID );
		rkPacket.Pop( bCurStatus );
		rkPacket.Pop( iCurCount );
		rkPacket.Pop( iNextCount );
	}
	size_t min_size() const
	{
		return sizeof(*this);
	}

	std::wstring kRealmQuestID;
	bool bCurStatus;
	size_t iCurCount;
	size_t iNextCount;
} SRealmQuestInfo;
typedef std::map< std::wstring, SRealmQuestInfo > CONT_REALM_QUEST_INFO;

// 캐시샵 인게임 가챠 결과 메일 
int const SHOP_GAMBLEMACHINE_RESULT_FROM = 2000000025; 
int const SHOP_GAMBLEMACHINE_RESULT_TITLENO = 2000000026;
int const SHOP_GAMBLEMACHINE_RESULT_TEXTNO = 2000000027;

// 럭키스타 결과 메일
int const LUCK_STAR_RESULT_FROM = 2000000028;

// OX 퀴즈 결과 메일
int const OXQUIZ_RESULT_FROM = 2000000029;

// 쿠폰 이벤트 결과 메일
int const COUPON_RESULT_FROM = 2000000030;

// 이벤트 캐시 아이템 선물 이벤트 결과 메일
int const EVENT_CASHITEM_GIFT_RESULT_FROM = 2000000031;

// 마이홈 아파트 구매 기본 가격
int const DEFAULT_MYHOME_APT_COST = 200000; // 20 골드

typedef std::set<SItemPos> CONT_ITEMPOS;
size_t const MAX_MYHOME_BUFF_NUM = 100;

int const MAX_DICE_ITEM_OPTION_LEVEL = 15;

// 아이템 옵션 다이트 베이스 테이블 번호
int const DICE_ITEM_OPTION_BASE = 6000;

int const MAX_GAMBLEMACHINE_MIXUP_NUM = 5;

// 캐시샵 커스튬 조합 결과 메일 
int const SHOP_GAMBLEMACHINE_MIXUP_RESULT_FROM		= 2000000032; 
int const SHOP_GAMBLEMACHINE_MIXUP_RESULT_TITLENO	= 2000000033;
int const SHOP_GAMBLEMACHINE_MIXUP_RESULT_TEXTNO	= 2000000034;

int const DEFAULT_MYHOME_APT_LEVELLIMIT = 25;

// 채집 2차 가공 메일 발송
int const JOB_SKILL_ITEM_UPGRADE_RESULT_ITEM_FROM = 2000000037;
int const JOB_SKILL_ITEM_UPGRADE_RESULT_ITEM_TITLE= 2000000038;
int const JOB_SKILL_ITEM_UPGRADE_RESULT_ITEM_TEXT = 2000000039;


typedef struct tagHomeChatGuest
{
	BM::GUID kID;
	std::wstring kName;
	bool bEnablePrivateMsg;
	bool bNotStanding;
	bool bRoomMaster;
	int iClass;
	int	iGender;
	tagHomeChatGuest():bEnablePrivateMsg(false),bNotStanding(false),bRoomMaster(false),iClass(0),iGender(0){}
	tagHomeChatGuest(BM::GUID const & __id, std::wstring const & __name, bool const __enableprivatemsg, bool const __notstanding, bool const __roommaster, int const __class, int const __gender):
	kID(__id),kName(__name),bEnablePrivateMsg(__enableprivatemsg), bNotStanding(__notstanding), bRoomMaster(__roommaster),iClass(__class),iGender(__gender){}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(kID);
		kPacket.Push(kName);
		kPacket.Push(bEnablePrivateMsg);
		kPacket.Push(bNotStanding);
		kPacket.Push(bRoomMaster);
		kPacket.Push(iClass);
		kPacket.Push(iGender);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kID);
		kPacket.Pop(kName);
		kPacket.Pop(bEnablePrivateMsg);
		kPacket.Pop(bNotStanding);
		kPacket.Pop(bRoomMaster);
		kPacket.Pop(iClass);
		kPacket.Pop(iGender);
	}

	size_t min_size() const
	{
		return sizeof(BM::GUID) + sizeof(size_t) + sizeof(bool) + sizeof(bool) + sizeof(bool) + sizeof(int) + sizeof(int);
	}
}SHomeChatGuest;

typedef std::map<BM::GUID,SHomeChatGuest> CONT_MYHOMECHATGUEST;

typedef std::set<BM::GUID> CONT_KICK_PLAYER_LIST;

typedef struct tagHomeChatRoom
{
	BM::GUID		kRoomGuid;
	std::wstring	kRoomTitle,
					kRoomNoti,
					kRoomPassword;
	size_t			kMaxUserCount;
	CONT_MYHOMECHATGUEST kContGuest;
	CONT_KICK_PLAYER_LIST kContKick;

	tagHomeChatRoom():kMaxUserCount(0){}
	tagHomeChatRoom(BM::GUID const & __guid, std::wstring const & __title, std::wstring const & __noti, std::wstring const & __pswd, size_t const __maxusercount = 0):
	kRoomGuid(__guid), kRoomTitle(__title), kRoomNoti(__noti), kRoomPassword(__pswd), kMaxUserCount(__maxusercount){}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(kRoomGuid);
		kPacket.Push(kRoomTitle);
		kPacket.Push(kRoomNoti);
		kPacket.Push(kRoomPassword);
		kPacket.Push(kMaxUserCount);
		PU::TWriteTable_AM(kPacket,kContGuest);
		PU::TWriteArray_A(kPacket,kContKick);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kRoomGuid);
		kPacket.Pop(kRoomTitle);
		kPacket.Pop(kRoomNoti);
		kPacket.Pop(kRoomPassword);
		kPacket.Pop(kMaxUserCount);
		PU::TLoadTable_AM(kPacket,kContGuest);
		PU::TLoadArray_A(kPacket,kContKick);
	}

	size_t min_size() const
	{
		return sizeof(BM::GUID) + sizeof(size_t) + sizeof(size_t) + sizeof(size_t) + sizeof(size_t) + sizeof(size_t) + sizeof(size_t);
	}
}SHomeChatRoom;

typedef std::map<BM::GUID,SHomeChatRoom> CONT_MYHOMECHATROOM;

typedef enum E_ENCHANT_SHIFT_ITEM_POS
{
	ESIP_SOURCE = 0,
	ESIP_TARGET,
	ESIP_SHIFT,
	ESIP_INSURANCEP2 = ESIP_SHIFT,// +2 는 보험이 없으므로 사용 안됨 ( 경계선 표시로 사용 )
	ESIP_INSURANCEP1,// +1
	ESIP_INSURANCEP0,// +0
	ESIP_INSURANCEM1,// -1
	ESIP_INSURANCEM2,// -2
	ESIP_INSURANCEM3,// DESTROY

	ESIP_NUM
}eEnchantShiftItemPos;

typedef enum eClientCustomDataType : WORD {
	ECCDT_NONE				= 0,
	ECCDT_SKILLTABNO		= 1,//전략스킬 탭
}EClientCustomDataType;

typedef enum eSkillTabType : BYTE
{
	ESTT_BASIC		= 0,
	ESTT_SECOND		= 1,
	ESTT_MAX
}ESkillTabType;

typedef enum eSkillTabOpenType
{
	ESTOT_BASIC		= 0x0,
	ESTOT_SECOND	= 0x1,
}ESkillTabOpenType;

typedef enum eSuperGroundMode
{
	SGM_NORMAL		= 0,
	SGM_ENCHANT		= 1,
	SGM_RARE		= 2,
} ESuperGroundMode;

typedef enum eAchievementsCategory
{
	AC_TOTAL			= 0,
	AC_CATEGORY_01		= 1,
	AC_CATEGORY_02		= 2,
	AC_CATEGORY_03		= 3,
	AC_CATEGORY_04		= 4,
	AC_CATEGORY_05		= 5,
	AC_CATEGORY_06		= 6,
} EAchievementsCategory;

typedef enum eAwakeChargeInfo : BYTE
{
	EACS_NONE	   = 0, 
	EACS_UI_CHANGE = 1,
	EACS_AWAKE_CHARGE_END = 2,
}EAwakeChargeInfo;

size_t const MAX_JOBSKILL3_CREATEITEM_RESOURCE_LEN = 20;
typedef std::vector<std::pair<int,int> > CONT_JS3_RESITEM_INFO;

//
// 채널 이동하면서 맵 이동할때 사용.
typedef struct tagChannelMapMove
{
	short iChannelNo;
	int iGroundNo;

	tagChannelMapMove()
		: iChannelNo(0), iGroundNo(0)
	{
	}

	tagChannelMapMove(short Channel, int Ground)
		: iChannelNo(Channel), iGroundNo(Ground)
	{
	}

	tagChannelMapMove(tagChannelMapMove const & rhs)
		: iChannelNo(rhs.iChannelNo), iGroundNo(rhs.iGroundNo)
	{
	}

	bool operator == (tagChannelMapMove const & rhs)
	{
		return ((iChannelNo == rhs.iChannelNo) && (iGroundNo == rhs.iGroundNo));
	}
}SChannelMapMove;

// 플레이어가 원정대용 채널로 이동할 때 원래 있던 맵, 위치, 채널 정보를 저장하기 위한 구조체
typedef struct tagRecentPlayerNormalMapData
{
	int GroundNo; // 맵번호
	short ChannelNo; // 채널
	POINT3 RecentPos; // 위치

	tagRecentPlayerNormalMapData()
		: GroundNo(0), ChannelNo(0), RecentPos(0, 0, 0)
	{
	}
	tagRecentPlayerNormalMapData(int GndNo, short ChanNo, POINT3 Pos)
		: GroundNo(GndNo), ChannelNo(ChanNo), RecentPos(Pos)
	{
	}

	tagRecentPlayerNormalMapData(tagRecentPlayerNormalMapData const & rhs)
		: GroundNo(rhs.GroundNo), ChannelNo(rhs.ChannelNo), RecentPos(rhs.RecentPos)
	{
	}

	bool operator == (tagRecentPlayerNormalMapData const & rhs)
	{
		return (GroundNo == rhs.GroundNo) && (ChannelNo == rhs.ChannelNo) && (RecentPos == rhs.RecentPos);
	}

	void operator = (tagRecentPlayerNormalMapData const & rhs)
	{
		GroundNo = rhs.GroundNo;
		ChannelNo = rhs.ChannelNo;
		RecentPos = rhs.RecentPos;
	}

	void ReadFromPacket(BM::Stream & Packet)
	{
		Packet.Pop(GroundNo);
		Packet.Pop(ChannelNo);
		Packet.Pop(RecentPos);
	}

	void WriteToPacket(BM::Stream & Packet) const
	{
		Packet.Push(GroundNo);
		Packet.Push(ChannelNo);
		Packet.Push(RecentPos);
	}
}SRecentPlayerNormalMapData;

typedef struct tagExpeditionListInfo
{
	BM::GUID ExpeditionGuid;
	BM::GUID MasterGuid;
	std::wstring MasterName;
	int MapNo;
	char CurMemberCount;
	char MaxMemberCount;

	tagExpeditionListInfo()
		: MapNo(0), CurMemberCount(0), MaxMemberCount(0)
	{
	}

	tagExpeditionListInfo(
		BM::GUID const & Expedition, BM::GUID const & Master, 
		std::wstring const & Name, int Map, char CurMember, char MaxMember )
		: ExpeditionGuid(Expedition)
		, MasterGuid(Master)
		, MasterName(Name)
		, MapNo(Map)
		, CurMemberCount(CurMember)
		, MaxMemberCount(MaxMember)
	{
	}

	tagExpeditionListInfo(tagExpeditionListInfo const & rhs)
		: ExpeditionGuid(rhs.ExpeditionGuid)
		, MasterGuid(rhs.MasterGuid)
		, MasterName(rhs.MasterName)
		, MapNo(rhs.MapNo)
		, CurMemberCount(rhs.CurMemberCount)
		, MaxMemberCount(rhs.MaxMemberCount)
	{
	}

	void Clear()
	{
		int MapNo = 0;
		char CurMemberCount = 0;
		char MaxMemberCount = 0;
	}

	bool operator == (tagExpeditionListInfo const & rhs)
	{
		return (ExpeditionGuid == rhs.ExpeditionGuid);
	}

	void operator = (tagExpeditionListInfo const & rhs)
	{
		ExpeditionGuid = rhs.ExpeditionGuid;
		MasterGuid = rhs.MasterGuid;
		MasterName = rhs.MasterName;
		MapNo = rhs.MapNo;
		CurMemberCount = rhs.CurMemberCount;
		MaxMemberCount = rhs.MaxMemberCount;
	}

	size_t min_size() const
	{
		return 
			sizeof( ExpeditionGuid ) +
			sizeof( MasterGuid ) +
			sizeof( size_t ) + // MasterName
			sizeof( MapNo ) +
			sizeof( CurMemberCount ) +
			sizeof( MaxMemberCount );
	}

	void ReadFromPacket(BM::Stream & Packet)
	{
		Packet.Pop(ExpeditionGuid);
		Packet.Pop(MasterGuid);
		Packet.Pop(MasterName);
		Packet.Pop(MapNo);
		Packet.Pop(CurMemberCount);
		Packet.Pop(MaxMemberCount);
	}

	void WriteToPacket(BM::Stream & Packet) const
	{
		Packet.Push(ExpeditionGuid);
		Packet.Push(MasterGuid);
		Packet.Push(MasterName);
		Packet.Push(MapNo);
		Packet.Push(CurMemberCount);
		Packet.Push(MaxMemberCount);
	}

} SExpeditionListInfo;
typedef std::list<SExpeditionListInfo> ExpeditionInfoList;

typedef struct tagMemberPremiumData
{
	tagMemberPremiumData():iServiceNo(0){}

	int iServiceNo;
	BM::DBTIMESTAMP_EX kStartDate;
	BM::DBTIMESTAMP_EX kEndDate;
	BM::Stream kCustomData;
} SMemberPremiumData;

typedef enum eGemStoreMenu
{ // 보석상인 메뉴
	EGM_NONE			= 0,
	EGM_ANTIQUE			= 6,
	EGM_SOCKETCARD		= 7,
	EGM_ELUNIUM			= 11,
	EGM_PURESILVER_KEY	= 13,
}EGemStoreMenu;

const int MAX_BLOCKCHAT_LIST = 20;	// 최대 채팅차단 가능 수.
typedef std::map<std::wstring, BYTE> CONT_BLOCKCHAT_LIST;	// 채팅차단 리스트.
typedef enum eChatBlockType : BYTE
{	// 채팅 차단 타입.
	ECBT_NONE		= 0,
	ECBT_NORMAL		= 1,
	ECBT_CASHCHAT	= 2,
	ECBT_BOTH		= 3,
}EChatBlockType;

typedef enum eChatBlockUpdateType : BYTE
{
	ECBUT_REGIST		= 1,
	ECBUT_UNREGIST		= 2,
	ECBUT_CHANGE_OPTION	= 3,
}EChatBlockUpdateType;

const short EXPEDITION_CHANNEL_NO = 100;

// 커뮤니티 이벤트 맵이동 실패 에러 코드.
typedef enum eErrorEventMapMove : BYTE
{
	EEMM_LIMIT_NONE						= 0,
	EEMM_LIMIT_MINIMUM_PARTYMEMBER		= 1,		// 파티 최소 인원수 미달.
	EEMM_LIMIT_LEVEL_MIN				= 2,		// 최소 레벨이 맞지 않음.
	EEMM_LIMIT_LEVEL_MAX				= 3,		// 최대 레벨이 맞지 않음.
	EEMM_LIMIT_CLASS					= 4,		// 직업이 맞지 않음.
	EEMM_LIMIT_QUEST					= 5,		// 요구 퀘스트가 없음.
	EEMM_LIMIT_ITEM						= 6,		// 요구 아이템이 없음.
	EEMM_LIMIT_MAP_FULL					= 7,		// 해당 이벤트 맵 인원초과.
	EEMM_LIMIT_EVENT_TIME				= 8,		// 이벤트 진행 시간이 아님.
	EEMM_NO_PARTY_MEMBER_SAME_GROUND	= 9,		// 파티 멤버가 같은 맵에 있지 않다.
	EEMM_SUCCESS						= 10,		// 성공.
} EErrorEventMapMove;

const int EVENT_START_WAIT_TIME_MILISEC = 600000;
const int EVENT_START_WAIT_TIME_SEC = 600;

// 인던입장
struct SIndunPartyInfo
{
	BM::GUID kPartyGuid;
	BM::GUID kMasterGuid;
	std::wstring kMasterName;
	int iMasterClass;
	int iMasterLevel;
	int iMasterMapNo;
	std::wstring kPartyTitle;
	std::wstring kPartySubName;
	BYTE iNowUser;
	BYTE iMaxUser;
	int iPartyOption;
	BYTE byPartyState;
	int WorldGndNo;
	int Position;
	int Difficulty;

	size_t min_size() const
	{
		return sizeof( kPartyGuid ) +
			sizeof( kMasterGuid ) +
			sizeof( size_t ) + // MasterName
			sizeof( iMasterClass ) +
			sizeof( iMasterLevel ) +
			sizeof( iMasterMapNo ) +
			sizeof( size_t ) + // PartyTitle
			sizeof( size_t ) + // PartySubName
			sizeof( iNowUser ) +
			sizeof( iMaxUser ) +
			sizeof( iPartyOption ) +
			sizeof( byPartyState ) +
			sizeof( WorldGndNo ) +
			sizeof( Position ) +
			sizeof( Difficulty );
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(kPartyGuid);
		kPacket.Push(kMasterGuid);
		kPacket.Push(kMasterName);
		kPacket.Push(iMasterClass);
		kPacket.Push(iMasterLevel);
		kPacket.Push(iMasterMapNo);
		kPacket.Push(kPartyTitle);
		kPacket.Push(kPartySubName);
		kPacket.Push(iNowUser);
		kPacket.Push(iMaxUser);
		kPacket.Push(iPartyOption);
		kPacket.Push(byPartyState);
		kPacket.Push(WorldGndNo);
		kPacket.Push(Position);
		kPacket.Push(Difficulty);
	}
	bool ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(kPartyGuid);
		kPacket.Pop(kMasterGuid);
		kPacket.Pop(kMasterName);
		kPacket.Pop(iMasterClass);
		kPacket.Pop(iMasterLevel);
		kPacket.Pop(iMasterMapNo);
		kPacket.Pop(kPartyTitle);
		kPacket.Pop(kPartySubName);
		kPacket.Pop(iNowUser);
		kPacket.Pop(iMaxUser);
		kPacket.Pop(iPartyOption);
		kPacket.Pop(byPartyState);
		kPacket.Pop(WorldGndNo);
		kPacket.Pop(Position);
		kPacket.Pop(Difficulty);
		return kPacket.RemainSize() == 0;
	}
};

typedef std::vector<SIndunPartyInfo> CONT_INDUN_PARTY;
typedef std::map<short, CONT_INDUN_PARTY> CONT_INDUN_PARTY_CHANNEL;	//first: channel, second: 인던파티 정보

typedef enum eAchievementType
{
	E_ACHI_NONE = 0,
	E_ACHI_LEVELUP,
	E_ACHI_ENCHANT_SUCCESS,
	E_ACHI_ENCHANT_FAIL,
	E_ACHI_GET_ITEM_MESSAGE,
}EAchievementType;

typedef std::map<std::wstring, std::wstring> CONT_GENERIC_NOTICE;

#pragma pack() 

#endif // WEAPON_LOHENGRIN_PACKET_PACKETSTRUCT_H