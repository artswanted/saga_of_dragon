#pragma once
extern void CALLBACK OnConnectToMMC(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisconnectFromMMC(CEL::CSession_Base *pkSession);
extern void CALLBACK OnRecvFromMMC(CEL::CSession_Base *pkSession, BM::CPacket * const pkPacket);

extern void GetServerListFromManagementServer(BM::CPacket * const pkPacket);