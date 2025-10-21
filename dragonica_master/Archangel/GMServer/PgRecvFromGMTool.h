#pragma once

extern void CALLBACK OnAcceptFromGMTool(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisConnectToGMTool(CEL::CSession_Base *pkSession);
extern void CALLBACK OnRecvFromGMTool(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
