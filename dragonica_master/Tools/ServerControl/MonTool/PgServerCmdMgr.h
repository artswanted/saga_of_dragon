#pragma once
#include "PgServerStateDoc.h"
#include "PgLogCopyMgr.h"

struct ContServerIDSort
{
	bool operator()( SERVER_IDENTITY const &_Left, SERVER_IDENTITY const &_Right ) const
	{
		switch ( _Left.nServerType )
		{
		case CEL::ST_MAP:
		case CEL::ST_SWITCH:
			{
				return true;
			}break;
		case CEL::ST_CENTER:
			{
				if ( CEL::ST_MAP == _Right.nServerType || CEL::ST_SWITCH == _Right.nServerType )
				{
					return false;
				}
				return true;
			}break;
		default:
			{
				return _Left.nServerType < _Right.nServerType;
			}break;
		}
	}
};

typedef struct tagCmdServerInfo
{
	tagCmdServerInfo(EMMC_CMD_TYPE const eCmdType, short const nServerType)
	{
		m_eCmdType = eCmdType;
		m_nServerType = nServerType;
	}
	EMMC_CMD_TYPE m_eCmdType;
	short m_nServerType;
	ContServerID	m_kContServer;
}SCmdServerInfo;

typedef std::map< int, SCmdServerInfo > CONT_MCT_COMMAND;

class PgServerCmdMgr
{
public:
	PgServerCmdMgr(void);
	~PgServerCmdMgr(void);

	void ProcessInfoCmd(EMMC_CMD_TYPE eCmdType, CEL::CSession_Base *pkSession, BM::CPacket * const pkPacket);

	BM::vstring CmdLog(short nCmdType, SERVER_IDENTITY const & rkSI);
	std::wstring GetCmdTypeName(EMMC_CMD_TYPE eCmdType);

	bool AddReserveServerList(EMMC_CMD_TYPE const eCmdType, short const nServerType, CONT_SERVER_STATE_FOR_VIEW const & rkServerList);
	bool AddReserveServerList(EMMC_CMD_TYPE const eCmdType);
	bool UpdateReserveCommand(EMMC_CMD_TYPE const eCmdType, SERVER_IDENTITY const & rkSI);
	bool RunReserveCommand();

	bool OnCommand(EMMC_CMD_TYPE const eCmdType, ContServerID const & rkContServerId, CONT_SERVER_STRING const & rkContServerString);
	bool OnCommand(EMMC_CMD_TYPE const eCmdType);

	bool OnRefreshLog(int const iPriKey, int const iSecKey);
	bool OnReqGetFile(int const iFileType, std::wstring const & wstrFileName, std::wstring const & wstrOldFileName);
	bool OnChangeMaxUser(SERVER_IDENTITY const &rkSI, int const iMaxUser);
	bool OnReqLogFileInfo(int const iKeyValue, int const iCheckType1, int const iCheckType2, SLogCopyer::CONT_LOG_INFO const rkContLogInfo);
	bool OnReqNotice(short const nRealm, short const nChannel, std::wstring const & wstrContents);

	int GetMaxIndex() const { return m_iCmdMaxIndex; }
private:
	bool CmdServerOff();
	bool CmdServerOn();
	bool CmdServerReSync();

private:
	CONT_MCT_COMMAND m_kContReserveServer;
	int m_iCmdIndex;
	int m_iSiteNo;
	int m_iCmdMaxIndex;
};

#define g_kServerCmdMgr SINGLETON_STATIC(PgServerCmdMgr)