void CALLBACK OnConnectToMCtrl(CEL::CSession_Base *pkSession);
void CALLBACK OnDisconnectFromMCtrl(CEL::CSession_Base *pkSession);
void CALLBACK OnRecvFromMCtrl(CEL::CSession_Base *pkSession, CEL::CPacket * const pkPacket);

void GetServerListFromMCtrl(CEL::CPacket * const pkPacket);

extern BM::GUID g_kMCtrlConnectorGuid; //Connector Guid
extern BM::GUID g_kMCtrlSessionGuid;
