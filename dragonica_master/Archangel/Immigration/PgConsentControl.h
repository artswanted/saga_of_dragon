#ifndef IMMIGRATION_IMMIGRATIONSERVER_TASK_OBJ_PGCONSENTCONTROL_H
#define IMMIGRATION_IMMIGRATIONSERVER_TASK_OBJ_PGCONSENTCONTROL_H

class PgConsentControl
{
public:
	PgConsentControl();
	~PgConsentControl();

	typedef enum
	{
		ECONSENT_2ND_NONE = 0,
		ECONSENT_2ND_UPDATECHECKTIME = 1,
		ECONSENT_2ND_SETCONNECT = 2,
		ECONSENT_2ND_SETDISCONNECT = 3,
	} ECONSENT_TASK_TYPE;	// SEventMessage::SecondaryType

	void HandleMessage(SEventMessage *pkMsg);
	void Locked_Timer10s(DWORD dwUSerData);
protected:
	void SetConnect(void);
	void SetDisConnect(void);
	void UpdateCheckTime(bool const bEnd);
	void Check(void);

private:
	mutable Loki::Mutex	m_kMutex;

	bool		m_bInit;

	// ConsentServer에 접속은 되었다(Greeting을 보냈는지 안보냈는지는 m_u64ConsentAnsGreetingSendTime변수로 판단
	bool		m_bConnectConsentServer;

	// 이 값이 0이면 Consent서버로 Greeting을 보냈다는 의미
	// 초기화는 최대값으로 해야한다.
	ACE_UINT64	m_u64ConsentAnsGreetingSendTime;
};

#endif // IMMIGRATION_IMMIGRATIONSERVER_TASK_OBJ_PGCONSENTCONTROL_H