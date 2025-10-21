#ifndef WEAPON_LOHENGRIN_REALM_PGCHANNEL_H
#define WEAPON_LOHENGRIN_REALM_PGCHANNEL_H

#include "Lohengrin/dbtables.h"
#include "Lohengrin/PgRestrict.h"
/*
typedef enum eSiteStateSendFlag
{
	ESSF_CMP_BASE		= 0xFF,
	ESSF_SERVER_LIST	= 0x01,
	
	ECHNMNG_WHOLE = 0,
	ECHNMNG_ONE_REALM_MIN = 1,
	ECHNMNG_ONE_REALM_MAX = 100,
}ESiteStateSendFlag;
*/
#pragma pack(1)
typedef struct tagChannelState
{
	tagChannelState(int const in_max_user=0)
		:	max_user(in_max_user)
		,	now_user(0)
	{
	}

	int	max_user;
	int	now_user;

	bool CanAdd(int const add_count)const 
	{
		return max_user>=(now_user+add_count);
	}

	float GetPercent()const
	{
		return (float)now_user / (float)max_user;
	}

	// -를 넣어주면 감소가 됨
	bool Add(int const add_count)
	{
		now_user+=add_count;
		if ( now_user < 0 )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"now_user["<<now_user<<L"] Modify["<<add_count<<L"] Count error");
			now_user = 0;
		}
		return CanAdd(0);
	}

	bool Refresh(int const refresh_count,int& _out_iModify)
	{
		_out_iModify = refresh_count - now_user; 
		now_user = refresh_count;
		return CanAdd(0);
	}

	void WriteToPacket(BM::Stream& rkPacket)const
	{
		rkPacket.Push(max_user);
		rkPacket.Push(now_user);
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(max_user);
		rkPacket.Pop(now_user);
	}
}SChannelState;

typedef enum : BYTE
{
	ERealm_SendType_None			= 0x00,
	ERealm_SendType_AllChannel		= 0x01,
	ERealm_SendType_ServerInfo		= 0x02,
	ERealm_SendType_UserCountFake	= 0x04,	// Channel MaxUser/CurrentUser count fake !!
	ERealm_SendType_Notice			= 0x08,
	ERealm_SendType_Restrict		= 0x10,

	ERealm_SendType_Client_Realm	= ERealm_SendType_UserCountFake | ERealm_SendType_Notice,
	ERealm_SendType_Client_Channel	= ERealm_SendType_AllChannel | ERealm_SendType_UserCountFake | ERealm_SendType_Notice,
	ERealm_SendType_Server			= ERealm_SendType_AllChannel | ERealm_SendType_ServerInfo | ERealm_SendType_Notice,
	ERealm_SendType_Sync_Notice		= ERealm_SendType_AllChannel | ERealm_SendType_Notice,
	ERealm_SendType_All				= 0xFF,
} ERealm_SendType;

typedef std::map<TBL_KEY_INT,SChannelState>	ContChannelState;//first : Logical Channel No

#pragma pack()

class PgChannel
	: public PgRestrict
{
	friend class PgRealm;
public:
	PgChannel();
	PgChannel(const PgChannel &rhs);
	virtual ~PgChannel(void);

	void operator = (const PgChannel &rhs);

public:
	bool Build();
	bool AddServer(TBL_SERVERLIST const & slServer);//스위치만 추가해라

	void WriteToPacket(BM::Stream &rkPacket, ERealm_SendType const eSendType)const;
	void ReadFromPacket(BM::Stream &rkPacket, ERealm_SendType const eSendType);

	void IsAlive(bool const bIsAlive);
	
	HRESULT GetServerIdentity(CEL::E_SESSION_TYPE const eServerType, SERVER_IDENTITY &kOutSI);
	void UpdateUserCount(int const iMax, int const iCurrent);
	//서버 IDENTITY 뽑기.
protected://유저에게 가야 하는 정보
	//mutable Loki::Mutex m_kMutex;

	CONT_SERVER_HASH m_kServers;	// 채널을 구성하는 서버들	// 스위치
		
	CLASS_DECLARATION_NO_SET(bool, m_bIsAlive, IsAlive);
	CLASS_DECLARATION_S(int, MaxUser);
	CLASS_DECLARATION_S(int, CurrentUser);

	CLASS_DECLARATION_S(std::wstring, ChannelName);
	CLASS_DECLARATION_S(short, ChannelNo);

	// PrimeChannel 이란 ? 가끔 중심이 되는 Center가 필요할 때가 있는데 이러한 Channel을 의미
	CLASS_DECLARATION_S(bool, PrimeChannel);
	CLASS_DECLARATION_S(std::wstring, Notice_ChnList);
	CLASS_DECLARATION_S(std::wstring, Notice_InGame);
};

#endif // WEAPON_LOHENGRIN_REALM_PGCHANNEL_H