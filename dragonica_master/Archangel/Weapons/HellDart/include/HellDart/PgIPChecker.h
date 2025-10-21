#ifndef WEAPON_HELLDART_PGIPCHECKER_H
#define WEAPON_HELLDART_PGIPCHECKER_H

#include "Lohengrin/dbtables.h"
/*
typedef struct
:	public SConnectionArea
{
	std::wstring wstrBeginIP;
	std::wstring wstrEndIP;

	void WriteToPacket( BM::Stream& kPacket )const
	{
		SConnectionArea::WriteToPacket( kPacket );
		kPacket.Push(wstrBeginIP);
		kPacket.Push(wstrEndIP);
	}

	void ReadFromPacket( BM::Stream& kPacket )
	{
		SConnectionArea::ReadFromPacket( kPacket );
		kPacket.Pop(wstrBeginIP);
		kPacket.Pop(wstrEndIP);
	}
}TBL_DEF_CHECKIP;*/

#include "CEL/IPFilterWrapper.h"
namespace PgIPCheckerUtil
{
	SIpRange QueryResultToSIPRange(std::wstring const& kStartIP, std::wstring const& kEndIP, std::wstring const& kNationKey, std::wstring const& kNationName);
}

class PgIPChecker
	:	public PgIPBandFilter_Wrapper
{
public:
	PgIPChecker();
	virtual ~PgIPChecker();

public:
	bool Q_DQT_LOAD_IP_FOR_BLOCK( CEL::DB_RESULT &rkResult );
	bool Q_DQT_LOAD_IP_FOR_ACCPET( CEL::DB_RESULT &rkResult );
	bool Q_DQT_ADD_IP_FOR_ACCEPT( CEL::DB_RESULT &rkResult );

protected:
	bool Q_DQT_LOAD_IP_COMMON( CEL::DB_RESULT &rkResult, bool const bIsBlockIP);
};

#endif // WEAPON_HELLDART_PGIPCHECKER_H