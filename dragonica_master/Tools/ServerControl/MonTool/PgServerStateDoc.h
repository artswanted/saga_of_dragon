#pragma once

typedef std::map< int, SExSingleServerStatus > CONT_SERVER_STATE_FOR_VIEW;

#include "Lohengrin/PgRealmManager.h"

struct SControlInfo
{
	SControlInfo()
	{}

	void Clear()
	{
		kContServerStateView.clear();
//		kContServerOldStateView.clear();
	}

	PgRealmManager					kRealmMgr;
	CONT_SERVER_STATE_FOR_VIEW		kContServerStateView;
//	CONT_SERVER_STATE_FOR_VIEW		kContServerOldStateView;
};
typedef std::map< int, SControlInfo >		CONT_CONTROL_INFO;

typedef struct tagRealmUserRecord
{
	tagRealmUserRecord()
		:	iUserCount(0)
	{
	}

	unsigned int iUserCount;
	std::map< short, unsigned int >	kContChannel;

}SRealmUserRecord;

class PgServerStateDoc
{
	typedef std::set<std::wstring>				CONT_SMC_STATE;	//SI, IP

public:
	PgServerStateDoc(void);
	virtual ~PgServerStateDoc(void);

public:
	void Clear();

	bool RefreshServerState( BM::CPacket& rkPacket );
	bool GetServerInfo( int const iSiteNo, SERVER_IDENTITY& rkSI, CONT_SERVER_STATE_EX::mapped_type& rkOutInfo )const;
	bool GetServerInfo( int const iSiteNo, int const iServerIdx, CONT_SERVER_STATE_EX::mapped_type& rkOutInfo )const;
	bool GetServerList( int const iSiteNo, CONT_SERVER_STATE_FOR_VIEW &rkServerList )const;
	bool GetServerList(short nServerType, CONT_SERVER_STATE_FOR_VIEW const & rkServerList, ContServerID& rkOutServerIDList);

	int GetServerCount(int const iSiteNo, short const nServerType, short const nRealmNo = 0);

	bool IsAliveSMC(std::wstring const& kMachineIP) const;

	Loki::Mutex& GetLock(void)const{return m_kMutex;}
	CONT_CONTROL_INFO::const_iterator GetControlInfo_Begin(void)const{return m_kControlInfo.begin();}
	CONT_CONTROL_INFO::const_iterator GetControlInfo_End(void)const{return m_kControlInfo.end();}

	size_t GetCCU(CEL::E_SESSION_TYPE const eServerType, int const iSite = 1, int const iRealm = 0, int const iChannel = 0);
	bool WriteCCU( SYSTEMTIME const &kNowTime )const;

protected:
	bool GetServerList(int iCmdIndex, CEL::E_SESSION_TYPE kServerType, CONT_SERVER_STATE_FOR_VIEW const &rkServerList);

public:
	CLASS_DECLARATION_S(bool, bControlLevel);
	CLASS_DECLARATION(std::wstring, m_wstrMMCIP, MMC_IP);
	CLASS_DECLARATION(WORD, m_nMMCPort, MMC_PORT);
	CLASS_DECLARATION(std::wstring, m_wstrRecordPath, RecordPath);
	CLASS_DECLARATION(WORD, m_wRecordCCUTime, RecordCCUTimeMin);

	int GetConsentSite(void)const{return m_iConsentSite;}
	DWORD GetLastRefreshTime(void)const{return m_dwLastRefreshTime;}

protected:
	bool	m_bUseBeep;
	int		m_iConsentSite;
	DWORD	m_dwLastRefreshTime;
	
	CONT_CONTROL_INFO	m_kControlInfo;
	CONT_SMC_STATE		m_kContLiveSMC;

	mutable Loki::Mutex m_kMutex;
};

namespace ServerConfigUtil
{
	std::wstring GetServerTypeName(short nServerType);
};

#define g_kGameServerMgr Loki::SingletonHolder<PgServerStateDoc>::Instance()