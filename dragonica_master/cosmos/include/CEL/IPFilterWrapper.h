#pragma once
#define WIN32_LEAN_AND_MEAN

#include "BM/TWRapper.h"
#include "IPFilter.h"

class PgIPBandFilter_Wrapper
	: public TWrapper< PgIPBandFilter >
{
public:
	PgIPBandFilter_Wrapper();
	virtual ~PgIPBandFilter_Wrapper();

public:
	void Clear(bool const bIsBlockIP);
	bool Build(bool const bIsBlockIP);
	bool RegIPRange(SIpRange const &kIPRange, bool const IsBlockIP, SIpRange &kUsedRange);
	bool IsBlockAddr(in_addr const &in_addr_, SIpRange &out_range);
	bool IsAcceptAddr(in_addr const &in_addr_, SIpRange &out_range);

	void GetCont(bool const bIsBlockIP, PgIPBandFilter::CONT_IP &kOutCont);
	void SetCont(bool const bIsBlockIP, PgIPBandFilter::CONT_IP const &kInCont);

	void WriteToPacket(BM::Stream &kStream);
	void WriteToPacket(BM::Stream &kStream, bool const bIsBlockIP);
	void ReadFromPacket(BM::Stream &kStream, bool const bIsAll);
};