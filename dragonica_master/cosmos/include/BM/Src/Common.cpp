#include "stdafx.h"
#include "Ace/TIme_Value.h"
#include "BM/ThreadObject.h"
#include <boost/random.hpp>
#include <ctime>

namespace BM
{
	bool TimeCheck(UINT64 &ui64TImeValue, UINT64 const &ui64Interval)
	{
		UINT64 const ui64NowTime = GetTime64();
		if( (ui64NowTime - ui64TImeValue ) < ui64Interval)
		{//필요 시간을 충족치 못했다.
			return false;
		}
		ui64TImeValue = ui64NowTime;
		return true;
	}

	bool TimeCheck(DWORD &dwTImeValue, DWORD const dwInterval)
	{
		DWORD const dwNowTime = GetTime32();
		if( (dwNowTime - dwTImeValue ) < dwInterval)
		{//필요 시간을 충족치 못했다.
			return false;
		}
		dwTImeValue = dwNowTime;
		return true;
	}
	
//	static DWORD dwGab = ULONG_MAX - ::timeGetTime()	-300000;
//	static DWORD dwValue = ULONG_MAX - ::timeGetTime()	-300000;

	DWORD GetTime32()
	{
		DWORD const dwRet = ::timeGetTime();// + dwValue;
		return dwRet;
	}

	UINT64 GetTime64()
	{ 
		ACE_Time_Value const kTime = ACE_OS::gettimeofday();
		ACE_UINT64 ui64Time = 0;
		kTime.msec(ui64Time);
		return ui64Time;
	}

	INT64 DiffTime(UINT64 const& i64Start, UINT64 const& i64End)
	{
		if( i64Start < i64End )
		{
			return static_cast<INT64>(i64End - i64Start);
		}
		
		if( i64Start > i64End )
		{//시작이 크므로. 음수로 전환
			return static_cast<INT64>(i64Start-i64End) * -1;//음수.
		}

		return 0;
	}

	bool Compress(char const *pSrc, unsigned long src_size, std::vector< char > &vecDest)
	{
		vecDest.resize(src_size);

		unsigned long m_ulXMLCRC = crc32(0, Z_NULL, 0);
		m_ulXMLCRC = crc32(m_ulXMLCRC, (BYTE*)pSrc, src_size);

		int const iError = compress((BYTE*)&vecDest.at(0), &src_size, (BYTE*)pSrc, src_size);
		if (Z_OK == iError)
		{
			vecDest.resize(src_size);//결과 사이즈
			return true;
		}
		else
		{
			std::vector< char > vecTemp;
			vecTemp.swap(vecDest);
			return false;
		}
	}

	bool Compress(std::vector< char > const& vecSrc, std::vector< char > &vecDest)
	{
		if(vecSrc.size())
		{
			return Compress(&vecSrc.at(0), (unsigned long)vecSrc.size(), vecDest);
		}
		return false;
	}
	bool UnCompress(std::vector< char > const& vecSrc, char const *pDest, unsigned long dest_size)
	{
		DWORD dwSize = (DWORD) vecSrc.size();
		int const iError = uncompress((BYTE*)&vecSrc.at(0), &dwSize, (BYTE*)pDest, dest_size);
		if (Z_OK != iError)
		{
			return false;
		}
		return true;
	}

	bool UnCompress(std::vector< char > const& vecSrc, std::vector< char > &vecDest)
	{
		if(vecDest.size())
		{
			return UnCompress(vecSrc, &vecDest.at(0), (unsigned long)vecDest.size());
		}
		return false;
	}

	HRESULT MacAddress(__int64 &iOut)
	{
		iOut = 0;

		WKSTA_TRANSPORT_INFO_0 *pwkti = NULL;                  // Allocate data structure for Netbios
		DWORD dwEntriesRead = 0;
		DWORD dwTotalEntries = 0;
		BYTE *pbBuffer = NULL;

		// Get MAC address via NetBios's enumerate function
		NET_API_STATUS dwStatus = NetWkstaTransportEnum(
			NULL,                       // [in]  server name
			0,                          // [in]  data structure to return
			&pbBuffer,                  // [out] pointer to buffer
			MAX_PREFERRED_LENGTH,       // [in]  maximum length
			&dwEntriesRead,             // [out] counter of elements actually enumerated
			&dwTotalEntries,            // [out] total number of elements that could be enumerated
			NULL);                      // [in/out] resume handle

		assert(dwStatus == NERR_Success);
		if(dwStatus != NERR_Success)
		{
			return E_FAIL;
		}

		pwkti = (WKSTA_TRANSPORT_INFO_0 *)pbBuffer;     // type cast the buffer

		for(DWORD dwLoop = 0 ; dwLoop < dwEntriesRead; dwLoop++)            // first address is 00000000, skip it
		{       
			unsigned char *MACRt = (unsigned char *)&iOut;// enumerate MACs and print
			swscanf_s((wchar_t *)pwkti[dwLoop].wkti0_transport_address, L"%2hx%2hx%2hx%2hx%2hx%2hx", 
			&MACRt[0], &MACRt[1], &MACRt[2], &MACRt[3], &MACRt[4], &MACRt[5]);
	/*		for (int i = 0 ; i <= 5 ; i++)
			{
				printf("Mac Number[%d] : %x\n" , i , MACRt[i]);
			}
	*/  }
		// Release pbBuffer allocated by above function
		dwStatus = NetApiBufferFree(pbBuffer);
		assert(dwStatus == NERR_Success);
		if(dwStatus != NERR_Success)
		{
			return E_FAIL;
		}
		
		return S_OK;
	}

	HRESULT IPAddress(int &iOut)
	{
		iOut = 0;
		char name[100] = "";

		if( 0 != gethostname(name, sizeof(name)))
		{
			return E_FAIL;
		}

		struct hostent* hostinfo = gethostbyname(name);
		if(hostinfo)
		{
			iOut = (*(struct in_addr *)*hostinfo->h_addr_list).S_un.S_addr;
			return S_OK;
		}
	//    strcpy(ip, inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list));
		return E_FAIL;
	}

	HRESULT LZOInit()
	{
		if(lzo_init() != LZO_E_OK)
		{
			return E_FAIL;
		}
		return S_OK;
	}

	bool LZOCompress(char const *pSrc, unsigned long src_size, std::vector< char > &vecDest)
	{
		lzo_align_t wrkmem [ ((LZO1X_1_MEM_COMPRESS) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ];

		size_t const need_len = (src_size + src_size / 16 + 64 + 3);
		std::vector< char > kTemp(need_len);

		lzo_uint out_len;
		int const r = lzo1x_1_compress((BYTE const*)pSrc, src_size, (BYTE*)&kTemp.at(0), &out_len, wrkmem);
		if(r == LZO_E_OK)
		{
			if(out_len >= src_size)/* check for an incompressible block */
			{
				return false;
			}
			kTemp.resize(out_len);
			vecDest.swap(kTemp);
			return true;
		}
		return false;
	}

	bool LZOCompress(std::vector< char > const& vecSrc, std::vector< char > &vecDest)
	{
		if(vecSrc.size())
		{
			return LZOCompress(&vecSrc.at(0), (unsigned long)vecSrc.size(), vecDest);
		}
		return false;
	}

	bool LZOUnCompress(std::vector< char > const& vecSrc, std::vector< char > const& vecDest)
	{
		if(	vecDest.empty()
		||	vecSrc.empty() )
		{//비었네.
			return false;
		}

//		BM::CAutoMutex kLock(kLZOMutex);
//		dest_size --> 이미 설정되어야함.
		lzo_uint new_len = (lzo_uint)vecDest.size();
		int const r = lzo1x_decompress_safe((BYTE const*)&vecSrc.at(0), (lzo_uint)vecSrc.size(), (BYTE*)&vecDest.at(0), &new_len, NULL);

		if(	LZO_E_OK  == r
		&&	vecDest.size() == new_len)
		{
//			printf("decompressed %lu bytes back into %lu bytes\n",
//			(unsigned long) out_len, (unsigned long) in_len);
			return true;
		}
		return false;
	}

	int Rand_Range(int const iInMax, int const iInMin)//범위값 뽑을때 주로 쓰는, max 값을 포함하는 Rand
	{
		int const iMax = __max(iInMax, iInMin);
		int const iMin = __min(iInMax, iInMin);
		static boost::mt19937 __gen((boost::uint32_t)time(0));
		boost::uniform_int<> dist(iMin, iMax);
		return dist(__gen);
	}

	int Rand_Index(int const iInMax)//인덱스 뽑을때 주로 쓰는, max 값을 포함하지 않는 Rand
	{//랜덤 오브젝트를 돌릴때. unsigned 를 리턴 하면 쓰는쪽에서 문제가 생겨 오류 생김.
		return Rand_Range(iInMax - 1, 0);
	}

	double Rand_Unit()//0.0~1.0 까지의 값을 돌려주는 Rand
	{
		static boost::mt19937 __gen((boost::uint32_t)::time(0));
		boost::uniform_real<> dist(0, 1);
		return dist(__gen);
	}

	bool IsCorrectType(bool const &k)
	{
		if(	k == true
		||	k == false)
		{
			return true;
		}
		return false;
	}

}

