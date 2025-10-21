#pragma once

class PgNetwork
{
public:
	PgNetwork(void);
	virtual ~PgNetwork(void);
public:
	bool Initialize();
	bool ReadConfigFile();

	bool TryConnectServer();
	bool DisConnectServer();

	bool Send(const BM::CPacket& rkPacket);

protected:
	bool Terminate();

	CLASS_DECLARATION_S(bool, IsConnect);
	CLASS_DECLARATION_S(bool, IsServiceStart);
	CLASS_DECLARATION_S(CEL::SESSION_KEY, SessionKey);
	CLASS_DECLARATION_S(BM::GUID, ServerRegistGuid);
	CLASS_DECLARATION_S(BM::E_OUPUT_TYPE, LogOutputType);
	CLASS_DECLARATION_S(BM::E_LOG_LEVEL, LogLevelLimit);
};

#define g_kNetwork SINGLETON_STATIC(PgNetwork)
