#pragma once
#define WIN32_LEAN_AND_MEAN
typedef DWORD IPTYPE;//IPv4 용.

#include "BM/GUID.h"
#include "BM/Stream.h"

typedef struct tagIpRange
{
	tagIpRange(IPTYPE const in_start_ = 0, IPTYPE const in_end_ = 0, std::wstring const &kInNationName = L"", std::wstring const &kInNationCode= L"");
	
	bool operator == (tagIpRange const &rhs)const;

	bool operator < (tagIpRange const &rhs)const;
	bool IsIn(IPTYPE const in_value_)const;

	IPTYPE start_;
	IPTYPE end_;
	std::wstring strNationName;
	std::wstring strNationCode;
	
	void WriteToPacket(BM::Stream &kStream)const;
	void ReadFromPacket(BM::Stream &kStream);

	size_t min_size()const
	{
		return 
			sizeof(start_)
		+	sizeof(end_)
		+	sizeof(size_t)//string 타입
		+	sizeof(size_t);//string 타입
	}

}SIpRange;

class PgIPBandFilter
{
public:
	typedef std::vector< SIpRange > CONT_IP;
public:
	PgIPBandFilter(){}
	virtual ~PgIPBandFilter(){}

public:	
	void Clear(bool const bIsBlockIP);
	bool Build(bool const bIsBlockIP);
	void GetCont(bool const bIsBlockIP, CONT_IP &kOutCont);
	void SetCont(bool const bIsBlockIP, CONT_IP const &kInCont);
	bool RegIPRange(SIpRange const &kIPRange, bool const IsBlockIP, SIpRange &kUsedRange);
	bool IsBlockAddr(in_addr const &in_addr_, SIpRange &out_range)const;
	bool IsAcceptAddr(in_addr const &in_addr_, SIpRange &out_range)const;

	void WriteToPacket(BM::Stream &kStream)const;
	void WriteToPacket(BM::Stream &kStream, bool const bIsBlockIP)const;
	void ReadFromPacket(BM::Stream &kStream, bool const bIsAll);

public:	
	static DWORD AddrToDWORD(in_addr const &addr_);
	static in_addr DWORDToAddr(DWORD const &addr_);
	static bool IsExist(SIpRange const &a, SIpRange const &b);
	static bool Func_Order(SIpRange const &a, SIpRange const &b);

protected:
	static bool Find_T(CONT_IP const &kContIP, in_addr const &in_addr_, SIpRange &out_range);
	
	CONT_IP m_kContBlockIP;
	CONT_IP m_kContAcceptIP;
};
