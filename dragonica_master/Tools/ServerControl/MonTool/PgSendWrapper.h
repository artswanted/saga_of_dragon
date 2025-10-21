#pragma once
extern bool SetSendWrapper(const SERVER_IDENTITY &kRecvSI);

extern bool SendToServerType(const CEL::E_SESSION_TYPE eServerType, const BM::CPacket &rkPacket);
extern bool SendToServer(SERVER_IDENTITY const &kSI, BM::CPacket const &rkPacket);
