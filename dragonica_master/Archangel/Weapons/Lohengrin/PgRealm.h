#ifndef WEAPON_LOHENGRIN_REALM_PGREALM_H
#define WEAPON_LOHENGRIN_REALM_PGREALM_H

#include "PgChannel.h"

typedef std::map< short, PgChannel > CONT_CHANNEL;

class PgRealm
{
public:
	explicit PgRealm(short const nRealmNo = 0, std::wstring const &wstrRealmName = _T(""));
	PgRealm(PgRealm const &rhs);
	
	PgRealm& operator = (const PgRealm &rhs);
	~PgRealm(void);

public:
	bool Build();
	bool AddServer(TBL_SERVERLIST const &rkServerList);
	bool SetRestrictions( CONT_DEF_RESTRICTIONS const &kRestrictDef, SRestrictOnChannel const &kOptions );

	short RealmNo() const { return m_nRealmNo; }

	void WriteToPacket(BM::Stream &rkPacket, ERealm_SendType const eSendType) const;
	void ReadFromPacket(BM::Stream &rkPacket, ERealm_SendType const eSendType);
	
	void GetChannelCont(CONT_CHANNEL &rkOutCont)const;
	HRESULT GetChannel(CONT_CHANNEL::key_type const nChannelNo, CONT_CHANNEL::mapped_type &rkOutCont)const;
	HRESULT GetChannel(CONT_CHANNEL::key_type const nChannelNo, CONT_CHANNEL::mapped_type const* &pkChannel )const;
	short AliveChannel(short const nChannel, bool const bAlive);
	void IsAlive(bool const bAlive);
	void UpdateUserCount(short const sChannel, int const iMax, int const iCurrent);
	void UpdatePrimeChannel(short const sChannel);
	short GetPrimeChannel(void)const;
	HRESULT SetNotice(short const sChannel, std::wstring const& kNotice, std::wstring const& kNoticeInGame);

	typedef enum : short
	{
		EREALM_STATE_NONE = 0x0000,		// 서비스 중지 상태 (Default)
		EREALM_STATE_NORMAL = 0x0001,	// 보통 상태 (운영상태)
		EREALM_STATE_NEW = 0x0002,		// 새로운 Realm
		EREALM_STATE_EVENT = 0x0004,	// Event 진행중
		EREALM_STATE_RECOMMAND = 0x0008,	// 추천 Realm
	} EREALM_STATE;	// bitflag 로 사용 (State 변수값)

protected:
	// PrimieChannel 을 다시 구한다.
	// PrimeChannel 이란 ? 가끔 중심이 되는 Center가 필요할 때가 있는데 이러한 Channel을 의미
	short UpdatePrimeChannel();
protected:
	short m_nRealmNo;
	CLASS_DECLARATION_S(std::wstring, Name);
	CLASS_DECLARATION_NO_SET(bool, m_bIsAlive, IsAlive);
	CONT_CHANNEL m_kContChannel; // 채널리스트
	CLASS_DECLARATION_S(short, State);	// EREALM_STATE 값

protected:
	//mutable Loki::Mutex m_kMutex;
};

#endif // WEAPON_LOHENGRIN_REALM_PGREALM_H