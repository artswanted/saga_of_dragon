#include "stdafx.h"
#include "BM/guid.h"
#include "BM/Threadobject.h"
#include "BM/vstring.h"
#include "Loki/Threads.h"
#include <time.h>
#include <tchar.h>
#include <oledb.h>
#include "ACE/Time_Value.h"

BM::GUID::GUID()
{
	Clear();
}

BM::GUID::GUID(std::string const& strGuid)
{
	Set(strGuid);
}

BM::GUID::GUID(std::wstring const& strGuid)
{
	Set(strGuid);
}

BM::GUID::GUID(::GUID const& rGuid)
{
	m_Guid.Data1 = rGuid.Data1;
	m_Guid.Data2 = rGuid.Data2;
	m_Guid.Data3 = rGuid.Data3;
	::memcpy( m_Guid.Data4, rGuid.Data4,sizeof(rGuid.Data4) );
}

BM::GUID::~GUID()
{
}

void BM::GUID::Clear()
{
	m_Guid.Data1 = 0;
	m_Guid.Data2 = 0;
	m_Guid.Data3 = 0;
	ZeroMemory( m_Guid.Data4, sizeof(m_Guid.Data4) );
}

BM::GUID BM::GUID::Create()
{
	BM::GUID kTemp;
	kTemp.Generate();
	return kTemp;
}

bool BM::GUID::Set(std::string const& strGuid)
{
	return Set( UNI(strGuid) );
}

bool BM::GUID::Set(std::wstring const& strGuid)
{ 
	if( strGuid.size() != (sizeof( m_Guid )*2 +4) )//바이트수 * 2(16진법이니까) + 4(하이픈 갯수)
	{
		return false;
	}

	std::wstring const& wstrText = strGuid;
	
	std::wstring::size_type offset = 0;
	std::wstring::size_type count =0;
	std::wstring::size_type pos = std::wstring::npos;//! ! 
	std::wstring::size_type start= std::wstring::npos;//! ! 시작점
	
	int Data1 = 0;
	int Data2 = 0;
	int Data3 = 0;
	int Data4 = 0;
	int Data5 = 0;
	int Data6 = 0;

	pos = wstrText.find(L'-', offset );
	if( std::wstring::npos == pos){return false;}
	start = offset;
	count = pos - start;
	offset = pos+1;		

	std::wstring wstrData1 = wstrText.substr(start, count);
	if( EOF == ::swscanf_s(wstrData1.c_str(), _T("%08x"), &Data1) ){return false;}
	
	m_Guid.Data1 = Data1;

	pos = wstrText.find(L'-', offset );
	if( std::wstring::npos == pos){return false;}
	start = offset; 
	count = pos - start;
	offset = pos+1;	

	std::wstring wstrData2 = wstrText.substr(start, count);
	if( EOF == ::swscanf_s(wstrData2.c_str(), _T("%04x"), &Data2)){return false;}

	m_Guid.Data2 = LOWORD(Data2);

	pos = wstrText.find(L'-', offset );
	if( std::wstring::npos == pos){return false;}
	start = offset; 
	count = pos - start;
	offset = pos+1;	

	std::wstring wstrData3 = wstrText.substr(start, count);
	if( EOF == ::swscanf_s(wstrData3.c_str(), _T("%04x"), &Data3)){return false;}

	m_Guid.Data3 = LOWORD(Data3);

	pos = wstrText.find(L'-', offset );
	if( std::wstring::npos == pos){return false;}
	start = offset; 
	count = pos - start;
	offset = pos+1;	

	std::wstring wstrData4 = wstrText.substr(start, count);
	if( EOF == ::swscanf_s(wstrData4.c_str(), _T("%04x"), &Data4)){return false;}
	m_Guid.Data4[0] = HIBYTE(LOWORD(Data4));
	m_Guid.Data4[1] = LOBYTE(LOWORD(Data4));

	std::wstring wstrData5 = wstrText.substr(offset, 4);
	if( EOF == ::swscanf_s(wstrData5.c_str(), _T("%04x"), &Data5)){return false;}

	m_Guid.Data4[2] = HIBYTE(LOWORD(Data5));
	m_Guid.Data4[3] = LOBYTE(LOWORD(Data5));
	offset+=4;
	
	std::wstring wstrData6 = wstrText.substr(offset, 8);
	if( EOF == ::swscanf_s(wstrData6.c_str(), _T("%08x"), &Data6)){return false;}

	m_Guid.Data4[4] = HIBYTE(HIWORD(Data6));
	m_Guid.Data4[5] = LOBYTE(HIWORD(Data6));
	m_Guid.Data4[6] = HIBYTE(LOWORD(Data6));
	m_Guid.Data4[7] = LOBYTE(LOWORD(Data6));

	offset+=8;
	return true;
}

bool BM::GUID::SetUniqueidentifier( char const *szGuid )
{
	if ( szGuid )
	{
		std::string strGuid(szGuid+1);//{요거다음꺼부터
		strGuid.resize( (sizeof(m_Guid)*2 +4) );//}는 제외하고.
		return Set(strGuid);
	}
	return false;
}

bool BM::GUID::IsNull()const
{
	return BM::GUID::IsNull(*this);
}

bool BM::GUID::IsNotNull()const
{
	return BM::GUID::IsNotNull(*this);
}

bool BM::GUID::Generate()
{
	if( S_OK == ::CoCreateGuid( (::GUID*)this ) )
	{
		return true;
	}
	return false;
}

bool BM::GUID::operator < (BM::GUID const& rhs) const
{
	__int64 const *pSrc1 = (__int64 const *)&m_Guid.Data1;
	__int64 const *pTgt1 = (__int64 const *)&rhs.m_Guid.Data1;

	if( *pSrc1 < *pTgt1 )	{return true;}
	if( *pSrc1 > *pTgt1 )	{return false;}

	__int64 const *pSrc2 = (__int64 const *)&m_Guid.Data4;
	__int64 const *pTgt2 = (__int64 const *)&rhs.m_Guid.Data4;

	if( *pSrc2 < *pTgt2 )	{return true;}
	if( *pSrc2 > *pTgt2 )	{return false;}

	return false;
}

bool BM::GUID::operator > (BM::GUID const& rhs) const
{
	return (rhs < *this);
}

bool BM::GUID::operator == (BM::GUID const& rhs) const
{
	if( 0 == ::memcmp( &m_Guid, &rhs.m_Guid, sizeof( m_Guid )) )
	{
		return true;
	}
	return false;
}

bool BM::GUID::operator != (BM::GUID const& rhs) const
{
	if( 0 == ::memcmp( &m_Guid, &rhs.m_Guid, sizeof( m_Guid )) )
	{
		return false;
	}
	return true;
}

BM::GUID::operator const ::GUID& ()const
{
	return *reinterpret_cast< const ::GUID* >( &m_Guid );
}

BM::GUID::operator const ::GUID* ()const
{
	return reinterpret_cast< const ::GUID* >( &m_Guid );
}

/*
BM::GUID::operator size_t() const//해쉬키로 쓰도록 size_t 타입으로 합산해버림.
{
	size_t i = 0;
	i ^= m_Guid.Data1;
    i ^= m_Guid.Data2;
    i ^= m_Guid.Data3;

	size_t const* pHi = (size_t const*)&m_Guid.Data4[0];
	size_t const* pLow = (size_t const*)&m_Guid.Data4[4];

	i ^= (*pHi);
	i ^= (*pLow);

	return i;
}
*/

std::wstring BM::GUID::str() const
{//임시데이터 리턴이므로 wchar_t*를 리턴 할 수 없다.
	wchar_t wszMsg[0xFF] = {0,};
	::swprintf_s(wszMsg, 0xFF, L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", 
		m_Guid.Data1, 
		m_Guid.Data2, 
		m_Guid.Data3, 
		m_Guid.Data4[0], m_Guid.Data4[1],
		m_Guid.Data4[2], m_Guid.Data4[3],
		m_Guid.Data4[4], m_Guid.Data4[5],
		m_Guid.Data4[6], m_Guid.Data4[7]
		);

	std::wstring wstrTemp = wszMsg;
	return wstrTemp;
}

BM::GUID const& BM::GUID::NullData()
{
	static BM::GUID const kNull;
	return kNull;
}

bool BM::GUID::IsNull(BM::GUID const& rkGuid)
{
	return rkGuid == BM::GUID::NullData();
}

bool BM::GUID::IsNotNull(BM::GUID const& rkGuid)
{
	return rkGuid != BM::GUID::NullData();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BM::tagPackedTime::tagPackedTime()
{
	Clear();
}

BM::tagPackedTime::tagPackedTime(DBTIMESTAMP_EX const& rhs)
{
	Clear();
	*this = rhs;
}

BM::tagPackedTime::tagPackedTime (SYSTEMTIME const& rhs)
{
	Clear();
	Year(rhs.wYear-BASE_YEAR);//2000년 이후.
	Month(rhs.wMonth);
	Day(rhs.wDay);
	Hour(rhs.wHour);
	Min(rhs.wMinute);
	Sec(rhs.wSecond);
}

void BM::tagPackedTime::Clear()
{
	Year(0);
	Month(0);
	Day(0);
	Hour(0);
	Min(0);
	Sec(0);
}

void BM::tagPackedTime::SetLocalTime()
{
	SYSTEMTIME kTime;
	::GetLocalTime(&kTime);

	(*this) = kTime;
}

BM::tagPackedTime BM::tagPackedTime::LocalTime()
{
	tagPackedTime kTime;
	kTime.SetLocalTime();
	return kTime;
}

bool BM::tagPackedTime::operator < (BM::tagPackedTime const& rhs)const
{
	if( Year() < rhs.Year() ){return true;}
	if( Year() > rhs.Year() ){return false;}

	if( Month() < rhs.Month() ){return true;}
	if( Month() > rhs.Month() ){return false;}
	
	if( Day() < rhs.Day() ){return true;}
	if( Day() > rhs.Day() ){return false;}

	if( Hour() < rhs.Hour() ){return true;}
	if( Hour() > rhs.Hour() ){return false;}

	if( Min() < rhs.Min() ){return true;}
	if( Min() > rhs.Min() ){return false;}

	if( Sec() < rhs.Sec() ){return true;}
	if( Sec() > rhs.Sec() ){return false;}

	return false;
}

void BM::tagPackedTime::operator = (DBTIMESTAMP const& rhs)
{
	Year((rhs.year%BASE_YEAR));
	Month(rhs.month);
	Day(rhs.day);
	Hour(rhs.hour);
	Min(rhs.minute);
	Sec(rhs.second);
}

void BM::tagPackedTime::operator = (DBTIMESTAMP_EX const& rhs)
{
	*this = (DBTIMESTAMP)rhs;
}

void BM::tagPackedTime::operator = (const struct tm &rhs)
{
	Year(rhs.tm_year-GAB_FROM_TM_YEAR);//2000 년이 기준이기 때문에 100 을 제거 
	Month(rhs.tm_mon+1);
	Day(rhs.tm_mday);
	Hour(rhs.tm_hour);
	Min(rhs.tm_min);
	Sec(rhs.tm_sec);
}

void BM::tagPackedTime::operator = (SYSTEMTIME const& rhs)
{
	Year(rhs.wYear-BASE_YEAR);//2000년 이후.
	Month(rhs.wMonth);
	Day(rhs.wDay);
	Hour(rhs.wHour);
	Min(rhs.wMinute);
	Sec(rhs.wSecond);
}

bool BM::tagPackedTime::operator == (BM::tagPackedTime const& rhs)const
{
	if(	Year()	== rhs.Year()
	&&	Month() == rhs.Month()
	&&	Day()	== rhs.Day()
	&&	Hour()	== rhs.Hour()
	&&	Min()	== rhs.Min()
	&&	Sec()	== rhs.Sec())
	{
		return true;
	}
	return false;
}

BM::tagPackedTime::operator DBTIMESTAMP() const
{
	DBTIMESTAMP kTime;
	
	kTime.year = (SHORT)(Year()+BASE_YEAR);
	kTime.month = (USHORT)Month();
	kTime.day = (USHORT)Day();
	kTime.hour = (USHORT)Hour();
	kTime.minute = (USHORT)Min();
	kTime.second = (USHORT)Sec();
	kTime.fraction = 0;
	return kTime;
}

BM::tagPackedTime::operator SYSTEMTIME() const
{
	SYSTEMTIME Time;
	
	Time.wYear = (SHORT)(Year()+BASE_YEAR);
	Time.wMonth = (USHORT)Month();
	Time.wDay = (USHORT)Day();
	Time.wHour = (USHORT)Hour();
	Time.wMinute = (USHORT)Min();
	Time.wSecond = (USHORT)Sec();
	Time.wMilliseconds = 0;
	return Time;
}

bool BM::tagPackedTime::IsNull() const
{
	return ( Month()==0 || Day()==0 ) ? true : false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BM::DBTIMESTAMP_EX::DBTIMESTAMP_EX()
{
	Clear();
}

BM::DBTIMESTAMP_EX::DBTIMESTAMP_EX( ::DBTIMESTAMP const& rhs )
{
	*this = rhs;
}

BM::DBTIMESTAMP_EX::DBTIMESTAMP_EX( SYSTEMTIME const& rhs )
{
	*this = rhs;
}

BM::DBTIMESTAMP_EX::DBTIMESTAMP_EX( BM::tagPackedTime const& rhs )
{
	*this = rhs;
}

void BM::DBTIMESTAMP_EX::Clear()
{
	year	= 0;	month	= 0;	day		= 0;	
	hour	= 0;	minute	= 0;	second	= 0;	
	fraction= 0;
}
		
void BM::DBTIMESTAMP_EX::operator = ( DBTIMESTAMP const& rhs )
{
	year = rhs.year;
	month = rhs.month;
	day = rhs.day;
	hour = rhs.hour;
	minute = rhs.minute;
	second = rhs.second;
	fraction = rhs.fraction;
}

BM::DBTIMESTAMP_EX::operator DBTIMESTAMP const()	const	throw()
{
	DBTIMESTAMP kTemp;

	kTemp.year = year;
    kTemp.month = month;
    kTemp.day = day;
    kTemp.hour= hour;
    kTemp.minute =minute;
    kTemp.second = second;
    kTemp.fraction = fraction;
	return kTemp;
}

BM::DBTIMESTAMP_EX::operator const ACE_Time_Value()	const	throw()
{
	FILETIME kFileTime;

    SYSTEMTIME st;
    st.wDayOfWeek    = 0;
    st.wYear         = year;
    st.wMonth        = month;
    st.wDay          = day;
    st.wHour         = hour;
    st.wMinute       = minute;
    st.wSecond       = second;
    st.wMilliseconds = (WORD)(fraction / 1000000);

	SystemTimeToFileTime(&st, &kFileTime);

	return ACE_Time_Value(kFileTime);
}
		
bool BM::DBTIMESTAMP_EX::operator == ( DBTIMESTAMP_EX const& rhs )const
{
	if(0 == ::memcmp(this, &rhs, sizeof(rhs)))
	{
		return true;
	}
	return false;
}

bool BM::DBTIMESTAMP_EX::operator != ( DBTIMESTAMP_EX const& rhs )const
{
	if(*this == rhs)
	{
		return false;
	}
	return true;
}

bool BM::DBTIMESTAMP_EX::IsNull() const 
{
	if( 0 == year
	&&	0 == month
	&&	0 == day
	&&	0 == hour
	&&	0 == minute
	&&	0 == second
	&&	0 == fraction
	)
	{
		return true;
	}
	return false;
}

bool BM::DBTIMESTAMP_EX::operator < ( DBTIMESTAMP_EX const&  rhs ) const
{
	if( year < rhs.year ){return true;}
	if( year > rhs.year ){return false;}
	//!-- >  year == rhs.year 의 조건이 만족 된다.
	if( month < rhs.month ){return true;}
	if( month > rhs.month ){return false;}
	
	if( day < rhs.day ){return true;}
	if( day > rhs.day ){return false;}

	if( hour < rhs.hour ){return true;}
	if( hour > rhs.hour ){return false;}

	if( minute < rhs.minute ){return true;}
	if( minute > rhs.minute ){return false;}

	if( second < rhs.second ){return true;}
	if( second > rhs.second ){return false;}

	if( fraction < rhs.fraction ){return true;}
	if( fraction > rhs.fraction ){return false;}

	return false;
}

bool BM::DBTIMESTAMP_EX::operator > ( DBTIMESTAMP_EX const&  rhs ) const
{
	return (rhs < *this);
}

void BM::DBTIMESTAMP_EX::SetLocalTime()
{
	(*this) = tagPackedTime::LocalTime();
}

void BM::DBTIMESTAMP_EX::operator = ( BM::tagPackedTime const& rhs )
{
	if (true == rhs.IsNull())
	{
		Clear();
	}
	else
	{
		year = (SHORT)(rhs.Year() + BM::tagPackedTime::BASE_YEAR);
		month = (USHORT)rhs.Month();
		day = (USHORT)rhs.Day();
		hour = (USHORT)rhs.Hour();
		minute = (USHORT)rhs.Min();
		second = (USHORT)rhs.Sec();
		fraction = 0;
	}
}

void BM::DBTIMESTAMP_EX::operator = ( SYSTEMTIME const& rhs )
{
	year = rhs.wYear;
	month = rhs.wMonth;
	day = rhs.wDay;
	hour = rhs.wHour;
	minute = rhs.wMinute;
	second = rhs.wSecond;
	fraction = 0;
}

void BM::DBTIMESTAMP_EX::DateFromString(const char* sFormat, const char* sData)
{
    int iDay, iMonth, iYear, iHour, iMinute, iSecond = 0;
    sscanf_s(sData, sFormat, &iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond);
    day = iDay;
    month = iMonth;
    year = iYear;
    hour = iHour;
    minute = iMinute;
    second = iSecond;
}

bool BM::DBTIMESTAMP_EX::IsCorrect() const
{
	if( (!year) || (year >= LIMIT_YEAR) ){return false;}
	if( (!month) || (month > LIMIT_MONTH) ){return false;}
	if( (!day) || (day > LIMIT_DAY ) ){return false;}
	if( hour > LIMIT_HOUR ){return false;}
	if( minute > LIMIT_MIN ){return false;}
	if( second > LIMIT_SEC ){return false;}

	return true;
}

BM::DBTIMESTAMP_EX::operator std::wstring const()	const	throw()
{
	if( IsNull() )
	{
		return _T("NULL");
	}

	BM::vstring ret;
	ret += BM::vstring(year);	ret += _T("-");
	ret += BM::vstring(month);	ret += _T("-");
	ret += BM::vstring(day);	ret += _T(" ");
	ret += BM::vstring(hour);	ret += _T(":");
	ret += BM::vstring(minute);	ret += _T(":");
	ret += BM::vstring(second);	

	return ret;
}