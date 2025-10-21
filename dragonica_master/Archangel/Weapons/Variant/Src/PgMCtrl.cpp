#include "stdafx.h"
#include "BM/ExceptionFilter.h"
#include "CEL/Common.h"
#include "CEL/ConsoleCommander.h"
#include "Lohengrin/packettype.h"
#include "Lohengrin/packetstruct.h"
#include "Lohengrin/loggroup.h"
#include "Lohengrin/ProcessConfig.h"
#include "Lohengrin/PgRealmManager.h"
#include "constant.h"
#include "PgMCtrl.h"
//using namespace CEL;

bool (CALLBACK *g_kTerminateFunc)(WORD const&) = NULL;
//bool (*g_kSetMaxUserFunc)(int const&) = NULL;
int (*g_kGetMaxUserFunc)() = NULL;
int (*g_kGetNowUserFunc)() = NULL;
void (*g_kSetMaxUserFunc)(int const&) = NULL;
bool (*g_kCheckServerState)() = NULL;

bool MMCCommandProcess(BM::Stream* const pkPacket)
{
	eMCtrlCommand eCommand = MCC_None;
	SERVER_IDENTITY kTargetSI;

	pkPacket->Pop(eCommand);
	kTargetSI.ReadFromPacket(*pkPacket);

	SERVER_IDENTITY const &kSI = g_kProcessCfg.ServerIdentity();
	if(kSI == kTargetSI)
	{
		DWORD dwProcessID = 0;

		switch(eCommand)
		{
		case MCC_Shutdown_Service:
			{
				INFO_LOG( BM::LOG_LV1, __FL__ << _T("Receive stop signal") );
//				bMctrlService = false;
				if( g_kTerminateFunc )
				{
					g_kTerminateFunc(E_Terminate_By_MControl);//모든 서버 공통 함수 포인터
				}
				else
				{
					CAUTION_LOG(BM::LOG_LV1, __FL__ << _T(" Can't find common server terminate process function") );
					g_kConsoleCommander.StopSignal(true);//함수 포인터가 없으면
				}
			}break;
		case MCC_GetProcessID:
			{
				dwProcessID = ::GetCurrentProcessId();
			}// No break
		case MCC_Heart_Beat:
			{
				int iNowUser = 0;
				int iMaxUser = 0;

				if(g_kGetMaxUserFunc != NULL)
				{
					iMaxUser = g_kGetMaxUserFunc();
				}
				else
				{
					iMaxUser = 0;
				}

				if(g_kGetNowUserFunc != NULL)
				{
					iNowUser = g_kGetNowUserFunc();
				}
				else
				{
					iNowUser = 0;
				}

				SERVER_IDENTITY const& rkServerId = g_kProcessCfg.ServerIdentity();
				switch(rkServerId.nServerType)
				{
				case CEL::ST_LOG:
					{
						if ( rkServerId.nRealm )
						{
							ReadyToService(CEL::ST_CONTENTS, iNowUser, iMaxUser, dwProcessID );
							break;
						}
					} // no break
				case CEL::ST_GMSERVER:
				case CEL::ST_LOGIN:
				case CEL::ST_CONTENTS:
				case CEL::ST_CONSENT:
					{
						ReadyToService(CEL::ST_IMMIGRATION, iNowUser, iMaxUser, dwProcessID );
					}break;
				case CEL::ST_IMMIGRATION:
					{
						ReadyToService(CEL::ST_MACHINE_CONTROL, iNowUser, iMaxUser, dwProcessID );
					}break;
				case CEL::ST_CENTER:
					{
						ReadyToService(CEL::ST_CONTENTS, iNowUser, iMaxUser, dwProcessID );
					}break;
				case CEL::ST_SWITCH:
				case CEL::ST_MAP:
					{
						ReadyToService(CEL::ST_CENTER, iNowUser, iMaxUser, dwProcessID );
					}break;
				default:
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
					}break;
				}
			}break;
		case MCC_CHANGE_MAXUSER:
			{
				if(g_kSetMaxUserFunc)
				{
					int iChange = 0;
					pkPacket->Pop(iChange);
					g_kSetMaxUserFunc(iChange);
				}
			}break;
		default:
			{
				INFO_LOG(BM::LOG_LV1, __FL__ << _T("Invalid Command Type : ") << eCommand);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
				return false;
			}break;
		}
	}
	else
	{
		CONT_SERVER_HASH kServerInfo;
		g_kProcessCfg.Locked_GetServerInfo(kTargetSI.nServerType, kServerInfo);
		if(!kServerInfo.empty())
		{
			BM::Stream kPacket(PT_MCTRL_MMC_A_NFY_SERVER_COMMAND);
			kPacket.Push(eCommand);
			kTargetSI.WriteToPacket(kPacket);
			kPacket.Push(*pkPacket);
			switch(kSI.nServerType)
			{
			case CEL::ST_IMMIGRATION:
				{
					switch(kTargetSI.nServerType)
					{
					case CEL::ST_LOG:
						{
							if ( kTargetSI.nRealm )
							{
								CONT_SERVER_HASH kCont;
								g_kProcessCfg.Locked_GetServerInfo( CEL::ST_CONTENTS, kCont );

								CONT_SERVER_HASH::const_iterator server_itr = kCont.begin();
								for ( ; server_itr != kCont.end() ; ++server_itr )
								{
									SERVER_IDENTITY const &kSI = server_itr->first;
									if( kTargetSI.nRealm == kSI.nRealm )
									{
										g_kProcessCfg.Locked_SendToServer( kSI, kPacket );
									}
								}

								break;
							}
						} // no break
					case CEL::ST_GMSERVER:
					case CEL::ST_LOGIN:
					case CEL::ST_CONSENT:
						{
							g_kProcessCfg.Locked_SendToServer(kTargetSI, kPacket);
						}break;
					case CEL::ST_CONTENTS:
					case CEL::ST_CENTER:
					case CEL::ST_SWITCH:
					case CEL::ST_MAP:
						{
							g_kProcessCfg.Locked_SendToContentsServer(kTargetSI.nRealm, kPacket);
						}break;
					default:
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Incorrect server type : ") << kTargetSI.nServerType);
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
						}break;
					}						
				}break;
			case CEL::ST_CONTENTS:
				{
					if ( CEL::ST_LOG == kTargetSI.nServerType )
					{
						g_kProcessCfg.Locked_SendToServer(kTargetSI, kPacket);
					}
					else
					{
						if( -1 == kTargetSI.nChannel )
						{// -1 이라면 모든 채널로 보냄.
							g_kProcessCfg.Locked_SendToChannelServerType( CEL::ST_CENTER, kPacket, kSI.nChannel, true );
						}
						else
						{// 0이상이면 해당 채널로 보냄. 
							CONT_SERVER_HASH kCont;
							g_kProcessCfg.Locked_GetServerInfo( CEL::ST_CENTER, kCont );

							CONT_SERVER_HASH::const_iterator server_itr = kCont.begin();
							for ( ; server_itr != kCont.end() ; ++server_itr )
							{
								SERVER_IDENTITY const &kSI = server_itr->first;
								if( kTargetSI.nChannel == kSI.nChannel )
								{
									g_kProcessCfg.Locked_SendToServer(kSI, kPacket );
								}
							}
						}
					}
				}break;
			case CEL::ST_CENTER:
				{
					g_kProcessCfg.Locked_SendToServer(kTargetSI, kPacket);
				}break;
			default:break;
			}
		}
		else
		{
			INFO_LOG(BM::LOG_LV1, __FL__ << _T("Can't find server identify Channel<") << kTargetSI.nChannel << _T("> ServerNo<") << kTargetSI.nServerNo << _T("> ServerType<") << kTargetSI.nServerType << _T(">") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("kServerInfo Empty!"));
		}
	}
	return true;
}

//Start Service 하는 부분에 각 서머마다 넣어준다.
void WriteServerInfo(BM::Stream &rkPacket)
{
	g_kProcessCfg.ServerIdentity().WriteToPacket(rkPacket);
}

void WriteServicePacket(BM::Stream &rkPacket)
{
	WriteServerInfo(rkPacket);

	bool bMctrlService = true;

	if(g_kCheckServerState != NULL) 
	{
		bMctrlService = g_kCheckServerState();
	}

	rkPacket.Push(bMctrlService);//서버가 살아 있는지
	rkPacket.Push(PgExceptionFilter::IsExcepted());//Exception 상황인지 아닌지
}

bool ReadyToService(CEL::eSessionType const rkType, int const iConnectionUser, int const iMaxUser, DWORD const dwProcessID )
{
	BM::Stream kPacket(PT_MCTRL_A_MMC_ANS_SERVER_COMMAND, IMCT_STATE_NFY);
	WriteServicePacket(kPacket);
	kPacket.Push(iConnectionUser);
	kPacket.Push(iMaxUser);
	kPacket.Push(dwProcessID);
	return g_kProcessCfg.Locked_SendToServerType(rkType, kPacket);
}
/*
bool ReadyToService(int iConnectionUser = 0, int iMaxUser = 0)
{
	BM::Stream kPacket(PT_MCTRL_A_MMC_ANS_SERVER_COMMAND, IMCT_STATE_NFY);
	WriteServicePacket(kPacket);
	return g_kProcessCfg.Locked_SendToServerType(CEL::ST_CONTENTS, kPacket);
}
*/