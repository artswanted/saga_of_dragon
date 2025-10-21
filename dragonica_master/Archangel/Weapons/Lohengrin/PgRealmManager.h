#ifndef WEAPON_LOHENGRIN_REALM_PGREALMMANAGER_H
#define WEAPON_LOHENGRIN_REALM_PGREALMMANAGER_H

#include <map>

//#include "PgChannel.h"
#include "PgRealm.h"
#include "Lohengrin/PacketStruct.h"
#include "BM/GUID.h"


//typedef std::map< short, PgChannel > CHANNEL_HASH;

typedef std::map< short, PgRealm > CONT_REALM;
#pragma pack(1)
typedef struct tagRealmElement
{
	tagRealmElement(short const nRealmNo, std::wstring const &kRealmName, std::wstring const &kRealmTblName, std::wstring const &_NoticeTblName)
	{
		RealmNo(nRealmNo);
		RealmName(kRealmName);
		RealmTblName(kRealmTblName);
		Notice_TblName(_NoticeTblName);
	}
	CLASS_DECLARATION_S(short, RealmNo);
	CLASS_DECLARATION_S(std::wstring, RealmName);
	CLASS_DECLARATION_S(std::wstring, RealmTblName);
	CLASS_DECLARATION_S(std::wstring, Notice_TblName);
}SRealmElement;
#pragma pack()

typedef std::map< int, SRealmElement >				CONT_REALM_CANDIDATE;	// <RealmNo, SRealmElement>
typedef std::vector< SRestrictOnChannel >			CONT_RESTRICT_ON_CHANNEL;
typedef std::map<short,CONT_RESTRICT_ON_CHANNEL>	CONT_RESTRICT_ON_CHANNEL_REALMS;	

class PgRealmManager
{
	friend struct ::Loki::CreateStatic< PgRealmManager >;
public:
	PgRealmManager(void);
	virtual ~PgRealmManager(void);

	PgRealmManager( PgRealmManager const & );
	PgRealmManager& operator = ( PgRealmManager const & );

public:
	//각 센터들이, 이미그레이션에 보고.
	//이미그레이션은 로그인 서버에 갱신
	
	void Clear();
	
	bool Init( CONT_REALM_CANDIDATE const &kRealmCandi, CONT_SERVER_HASH const &rkServerList);
	
	short AliveChannel(short const nRealm, short const nChannelNo, bool const bAlive);
	
	bool WriteToPacket(BM::Stream &rkPacket, short sRealm = 0, ERealm_SendType const eSendType = ERealm_SendType_Server )const;
	bool ReadFromPacket(BM::Stream &rkPacket);

	void GetRealmCont(CONT_REALM &kCont)const;
	HRESULT GetRealm(CONT_REALM::key_type const nRealmNo, PgRealm &rkOut)const;
	HRESULT GetChannel(CONT_REALM::key_type const nRealmNo, CONT_CHANNEL::key_type const nChannelNo, CONT_CHANNEL::mapped_type &rkOutChannel)const;
	void UpdateUserCount(short const sRealm, short const sChannel, int const iMax, int const iCurrent);
	void UpdatePrimeChannel(short const sRealm, short const sPrimeChannel);
	short GetPrimeChannel(short const sRealm);
	size_t GetDeadRealmCount(void)const;
	HRESULT SetRealmState(CONT_REALM::key_type const nRealmNo, short sState);
	short GetRealmState(CONT_REALM::key_type const nRealmNo)const;
	CONT_REALM::key_type RealmFirstNo()const;
	HRESULT SetNotice(short const sRealm, short const sChannel, std::wstring const& kNotice, std::wstring const& kNoticeInGame);

	//
	HRESULT Q_DQT_LOAD_DEF_RESTRICTIONS(CEL::DB_RESULT &rkResult);
	HRESULT Q_DQT_LOAD_RESTRICTIONS_ON_CHANNEL(CEL::DB_RESULT &rkResult);
	HRESULT Q_DQT_LOAD_CHANNEL_NOTICE(CEL::DB_RESULT &rkResult);

protected:
	void SetRestrictions( CONT_RESTRICT_ON_CHANNEL_REALMS const &kCont );
	void AliveRealm(short const nRealm, bool const bAlive);

private:
	CONT_REALM m_kContRealm;
	mutable Loki::Mutex m_kMutex;

	CONT_DEF_RESTRICTIONS m_kDefRestrictions;
};

#define g_kRealmMgr SINGLETON_STATIC(PgRealmManager)

#endif // WEAPON_LOHENGRIN_REALM_PGREALMMANAGER_H