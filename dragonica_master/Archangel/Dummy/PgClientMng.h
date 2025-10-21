#pragma once

class PgClientMng
{
public:
	PgClientMng(void);
	virtual ~PgClientMng(void);

	void Init();
	short int GetThreadClientNum() { return m_sThreadClientNum; }

public :
	// Notify & Do function
	void ConnectedLogin(CEL::CSession_Base* pkSession, unsigned long ulNow);
	void LoginConnectTimeOut(CLIENT_INFO* pkClient);
	unsigned long DoLogin(CLIENT_INFO* pkClient);
	void LoginResult(CEL::CSession_Base* pkSession, ETryLoginResult eResult);
	void LoginTimeOut(CLIENT_INFO* pkClient);
	void ConnectSwitchTimeOut(CLIENT_INFO* pkClient);
	void ReservedSwitchInfo(CEL::CSession_Base* pkSession, SSwitchReserveResult* pkResult);
	void ConnectedSwitch(CEL::CSession_Base* pkSession, unsigned long ulNow);
	void TryAccessSwitch(CLIENT_INFO* pkClient);
	//void AnsCharacterList(CEL::CSession_Base* pkSession, SPlayerDBSimpleData* pkSimple);
	void ReqCharacterList(CEL::CSession_Base* pkSession, ETryLoginResult eRet);
	void DoSelectCharacter(CLIENT_INFO* pkClient);
	void AnsSelectCharacter(CEL::CSession_Base* pkSession);
	void SelectCharacterTimeOut(CLIENT_INFO* pkClient);
	void AnsReservePlayer(CEL::CSession_Base* pSession, SAnsReservePlayer* pkARP);
	void ReqEnterMapTimeOut(CLIENT_INFO* pkClient);
	void ReqMapLoaded(CLIENT_INFO* pkClient);
	void NfyMapLoaded(CEL::CSession_Base* pkSession);
	void RestartClient(CLIENT_INFO* pkClient);

	void DisconnectedLogin(CEL::CSession_Base* pkSession);
	void DisconnectedSwitch(CEL::CSession_Base* pkSession);

	void MakeStatistic1m();
	void WriteStatistic();

protected:
	bool GetClient(const CEL::SESSION_KEY& rkSession, ObjectLock<CLIENT_INFO*>* pkClient);
	void CollectData(CLIENT_INFO* pkClient, unsigned long ulNowTime, bool bSuccess = true);
	STAT_DATA* GetStatistic(EClientStatus eStatus, unsigned long ulInterval);


private:
	//MAP_CLIENT_INFO m_kClientMap;
	VECTOR_CLIENT_INFO m_kReadyClient;
//	OBJLOCK_REFCOUNT m_kClientLock;
	short int m_sThreadClientNum;

	// 통계 Data
	OBJLOCK_REFCOUNT m_kDataLock;
	VECTOR_ELAPSED_TIME_INFO m_kCollect;
	VECTOR_STAT_DATA m_kStatistic1m;
};

typedef Loki::SingletonHolder< PgClientMng, ::Loki::CreateStatic > Singleton_PgClientMng;

#define g_ClientMng Singleton_PgClientMng::Instance()