#include "stdafx.h"

#include "BM/ThreadObject.h"
#include "../IPFilter.h"


tagIpRange::tagIpRange(IPTYPE const in_start_, IPTYPE const in_end_, std::wstring const &kInNationName, std::wstring const &kInNationCode)
{
	start_ = in_start_;
	end_ = in_end_;
	strNationName = kInNationName;
	strNationCode = kInNationCode;
}

bool tagIpRange::operator < (tagIpRange const &rhs)const
{
	if (start_ < rhs.start_) { return true; }
	if (start_ > rhs.start_) { return false; }

	if (end_ < rhs.end_) { return true; }
	if (end_ > rhs.end_) { return false; }

	return false;
}

bool tagIpRange::operator == (tagIpRange const &rhs)const
{
	if(	start_ == rhs.start_
	&&	end_ == rhs.end_) 
	{
		return true; 
	}
	return false;
}

bool tagIpRange::IsIn(IPTYPE const in_value_)const
{
	if(	start_ <= in_value_
	&&	end_ >= in_value_ )
	{
		return true;
	}
	return false;
}

void tagIpRange::WriteToPacket(BM::Stream &kPacket)const
{
	kPacket.Push(start_);
	kPacket.Push(end_);
	kPacket.Push(strNationName);
	kPacket.Push(strNationCode);
}

void tagIpRange::ReadFromPacket(BM::Stream &kPacket)
{
	kPacket.Pop(start_);
	kPacket.Pop(end_);
	kPacket.Pop(strNationName);
	kPacket.Pop(strNationCode);
}

bool PgIPBandFilter::RegIPRange(SIpRange const &kIPRange, bool const IsBlockIP, SIpRange &kUsedRange)
{
	CONT_IP *pkContIP = NULL;
	if(IsBlockIP)
	{
		pkContIP = &m_kContBlockIP;
	}
	else
	{
		pkContIP = &m_kContAcceptIP;
	}

//	if(!Find_T(*pkContIP, DWORDToAddr(kIPRange.start_), kUsedRange))
	{
		pkContIP->push_back( kIPRange );
		return true;
	}
	return false;
}

void PgIPBandFilter::Clear(bool const bIsBlockIP)
{
	if(bIsBlockIP)
	{
		m_kContBlockIP.clear();
	}
	else
	{
		m_kContAcceptIP.clear();
	}
}

bool PgIPBandFilter::Build(bool const bIsBlockIP)
{
	if(bIsBlockIP)
	{
		std::sort(m_kContBlockIP.begin(), m_kContBlockIP.end(), Func_Order);
	}
	else
	{
		std::sort(m_kContAcceptIP.begin(), m_kContAcceptIP.end(), Func_Order);
	}
	return true;
}

bool PgIPBandFilter::IsBlockAddr(in_addr const &in_addr_, SIpRange &out_range)const
{
	return Find_T(m_kContBlockIP, in_addr_, out_range);
}

bool PgIPBandFilter::IsAcceptAddr(in_addr const &in_addr_, SIpRange &out_range)const
{
	return Find_T(m_kContAcceptIP, in_addr_, out_range);
}

template< typename ITOR, typename VALUE_TYPE>
bool ip_range_search(ITOR itor_start, ITOR itor_end, VALUE_TYPE const &value, VALUE_TYPE &out_value, size_t const before_range, ITOR itor_real_end)
{
	size_t const range = itor_end - itor_start;

	if(before_range)// -> 찾을때 문제됨.
	{
		size_t const mid_pos = range/2;
		ITOR itor_mid = itor_start + mid_pos;
		
		if(itor_mid != itor_real_end)
		{
			if( (*itor_mid).end_ < value.start_ )
		//	if( (*itor_mid).IsOverRange(value) )
			{
				return ip_range_search( itor_mid+((itor_end == itor_mid)?0:1), itor_end, value, out_value, range, itor_real_end);
			}
		//	else if (value < *itor_mid )
			else if ((*itor_mid).start_ > value.start_ )
			{
				return ip_range_search(itor_start, itor_mid-((itor_start == itor_mid)?0:1), value, out_value, range, itor_real_end);//시작부터 중간까지 서치.
			}
			else//(_Mid == _Val)
			{
				out_value = (*itor_mid);
				return true;
			}
		}
	}

	return false;
}

bool PgIPBandFilter::Find_T(PgIPBandFilter::CONT_IP const &kContIP, in_addr const &in_addr_, SIpRange &out_range)
{
	SIpRange kKey(PgIPBandFilter::AddrToDWORD(in_addr_), 0);
	return ip_range_search( kContIP.begin(), kContIP.end(), kKey, out_range, kContIP.size(), kContIP.end());
}

void PgIPBandFilter::WriteToPacket(BM::Stream &kPacket)const
{
	PU::TWriteArray_M(kPacket, m_kContBlockIP);
	PU::TWriteArray_M(kPacket, m_kContAcceptIP);
}

void PgIPBandFilter::WriteToPacket(BM::Stream &kPacket, bool const bIsBlockIP)const
{
	if(bIsBlockIP)
	{
		kPacket.Push((bool)true);
		PU::TWriteArray_M(kPacket, m_kContBlockIP);
	}
	else
	{
		kPacket.Push((bool)false);
		PU::TWriteArray_M(kPacket, m_kContAcceptIP);
	}
}

void PgIPBandFilter::ReadFromPacket(BM::Stream &kPacket, bool const bIsAll)
{
	if(bIsAll)
	{
		m_kContBlockIP.clear();
		m_kContAcceptIP.clear();
		PU::TLoadArray_M(kPacket, m_kContBlockIP);
		PU::TLoadArray_M(kPacket, m_kContAcceptIP);
	}
	else
	{
		bool bIsBlockIP;
		kPacket.Pop(bIsBlockIP);
		if(bIsBlockIP)
		{
			m_kContBlockIP.clear();
			PU::TLoadArray_M(kPacket, m_kContBlockIP);
		}
		else
		{
			m_kContAcceptIP.clear();
			PU::TLoadArray_M(kPacket, m_kContAcceptIP);
		}
	}
}

DWORD PgIPBandFilter::AddrToDWORD(in_addr const &addr_)
{//이것의 내부를 바꾸면 영역 체크가 의도하지 않은 결과를 뱉기도 하므로 절대 유의.
	DWORD  dwTemp = 0;
	dwTemp += addr_.S_un.S_un_b.s_b1 << 24;
	dwTemp += addr_.S_un.S_un_b.s_b2 << 16;
	dwTemp += addr_.S_un.S_un_b.s_b3 << 8;
	dwTemp += addr_.S_un.S_un_b.s_b4;
	return dwTemp;
}

in_addr PgIPBandFilter::DWORDToAddr(DWORD const &addr_)
{//이것의 내부를 바꾸면 영역 체크가 의도하지 않은 결과를 뱉기도 하므로 절대 유의.
	in_addr ret;
	ret.S_un.S_un_b.s_b1 = (BYTE)((addr_ >> 24	)& 0xFF);
	ret.S_un.S_un_b.s_b2 = (BYTE)((addr_ >> 16	)& 0xFF);
	ret.S_un.S_un_b.s_b3 = (BYTE)((addr_ >> 8	)& 0xFF);
	ret.S_un.S_un_b.s_b4 = (BYTE)((addr_ >> 0	)& 0xFF);
	return ret;
}

bool PgIPBandFilter::IsExist(SIpRange const &a, SIpRange const &b)
{
	return a.IsIn(b.start_);
}

bool PgIPBandFilter::Func_Order(SIpRange const &a, SIpRange const &b)
{
	return a < b;
}

void PgIPBandFilter::GetCont(bool const bIsBlockIP, CONT_IP &kOutCont)
{
	if(bIsBlockIP)
	{
		kOutCont = m_kContBlockIP;
	}
	else
	{
		kOutCont = m_kContAcceptIP;
	}
}

void PgIPBandFilter::SetCont(bool const bIsBlockIP, CONT_IP const &kInCont)
{
	if(bIsBlockIP)
	{
		m_kContBlockIP = kInCont;
	}
	else
	{
		m_kContAcceptIP = kInCont;
	}
}