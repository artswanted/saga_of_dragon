#include "stdafx.h"
#include "PgNcGmCommand.h"
#include "variant\GM_const.h"
#include "PgGmTask.h"
#include "PgGmUserMgr.h"



PgNcGmCommand::PgNcGmCommand(void)
{
}

PgNcGmCommand::~PgNcGmCommand(void)
{
}

void PgNcGmCommand::LocaleGMCommandProcess(BM::Stream * const pkPacket)
{
	EGMCommandType eCommand;
	pkPacket->Pop(eCommand);

	switch (eCommand)
	{
	case ENC_CHARACTER_INFO:
		{
			RecvCharacterBaseInfo(pkPacket);
		}break;
	case ENC_CHARACTER_LIST1:
	case ENC_CHARACTER_LIST2:
		{
			BM::Stream kPacket(eCommand);
			kPacket.Push(*pkPacket);

		}break;
	case ENC_CAN_USING_NAME:
		{
			RecvIsUseName(pkPacket);
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("Unknown GMCommand received CommandType[") << (int)eCommand << _T("]") );
		}break;
	}
}

bool PgNcGmCommand::ReqCharacterBaseInfo(BM::GUID const &rkReqKey, const wchar_t *szString)
{
	if( (rkReqKey == NULL) || (wcscmp(szString, L"") == 0) )
	{
		return false;
	}

//	BM::Stream kPacket(PT_GM_T_REQ_GMCOMMAND, ENC_CHARACTER_INFO);
//	std::wstring kName = szString;
//	kPacket.Push(rkReqKey);
//	kPacket.Push(kName);
//	return g_kChannelMgr.SendToCenter(g_kProcessCfg.ChannelNo(), kPacket);
	return true;
}

bool PgNcGmCommand::ReqServerInfo()
{
	return true;
}

bool PgNcGmCommand::ReqCharacterList(BM::GUID const &rkReqKey, void* input, void* input2)
{
	if( (rkReqKey == NULL) )
	{
		return false;
	}

//	int iValue = *(int*)input;
//	int iCommandType = *(int*)input2;
//	BM::Stream kPacket(PT_GM_T_REQ_GMCOMMAND, iCommandType);
//	kPacket.Push(rkReqKey);
//	kPacket.Push(iValue);
//	return g_kProcessCfg.Locked_SendToCenter(kPacket);
	return true;
}

bool PgNcGmCommand::ReqIsUseName(BM::GUID const &rkReqKey, const wchar_t *szName)
{
	if( rkReqKey == NULL)
	{
		return false;
	}
 
	std::wstring kName = szName;
//	BM::Stream kPacket(PT_GM_T_REQ_GMCOMMAND, ENC_CAN_USING_NAME);
//	kPacket.Push(rkReqKey);
//	kPacket.Push(kName);
//	return g_kProcessCfg.Locked_SendToCenter(kPacket);
	return true;
}

void PgNcGmCommand::RecvCharacterBaseInfo(BM::Stream *pkPacket)
{
	int iError;

	pkPacket->Pop(iError);

	SNcGMCommandPack kNcPacket;
	kNcPacket.ReadFromPacket(pkPacket);
}

void PgNcGmCommand::RecvCharacterList(BM::Stream *pkPacket)
{
	int iType = 0;
	int iCount = 0;
	SNcGMCommandPack kNcPack;

	pkPacket->Pop(iType);
	pkPacket->Pop(iCount);	

	for(int i = 0; i < iCount; ++i)
	{
		kNcPack.ReadFromPacket(pkPacket);
		//엔씨로 보내는 구문
	}
}

void PgNcGmCommand::RecvIsUseName(BM::Stream *pkPacket)
{
	int iErrorCode = 0;
	pkPacket->Pop(iErrorCode);
	//엔씨로 보내는 구문
}

