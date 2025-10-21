#ifndef GM_GMSERVER_GM_PGGMUSERMGR_H
#define GM_GMSERVER_GM_PGGMUSERMGR_H

#include "Variant/GM_const.h"

class PgGMUserMgr
{
private:

public:
	PgGMUserMgr(void);
	virtual ~PgGMUserMgr(void);

	CLASS_DECLARATION_S(BM::GUID, ConnectorRegistOrder);
	CLASS_DECLARATION_S(BM::GUID, Connector);

	bool TryAuth(int iErr, SAuthInfo const &kLogin);
	bool WaitLogin(BM::Stream* const pkPacket);
	void WriteToPacketGmUser(BM::Stream& rkPacket);
	void BroadCast(BM::Stream &rkPacket);
	bool SendToLogin(BM::GUID const &rkMemberId, BM::Stream const &rkPacket);

	void Clear();
	void DeleteLoginedGM(BM::Stream *pkPacket);


protected:
	mutable Loki::Mutex m_kMutex;

//	BM::TObjectPool<SGMLoginedData> m_GMUserDataPool;

	CONT_GM_AUTH_DATA_ID			m_kGmUserWaitLogin;	//Login대기
	CONT_GM_AUTH_DATA				m_kGMUserLogined;	//Login완료한 유저들
};

#define g_kGMUserMgr SINGLETON_STATIC(PgGMUserMgr)

#endif // GM_GMSERVER_GM_PGGMUSERMGR_H