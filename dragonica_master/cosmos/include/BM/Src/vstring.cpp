#include "stdafx.h"
#include "BM/vstring.h"
#include "BM/guid.h"
#include "BM/localmgr.h"

using namespace BM;

//////////////////////////////////////////////////////////////////////////////////////////
// for Vietnam
class CVietnamCharFinder
{
private:
	struct VietChar_
	{
		int	m_Compchar;
		int	m_Base;
		int	m_Tone;
		VietChar_() : m_Compchar(0), m_Base(0), m_Tone(0)
		{
		}

		explicit VietChar_(int compchar, int base, int tone)
		{
			m_Compchar = compchar;
			m_Base = base;
			m_Tone = tone;
		}

		bool IsEmpty() const
		{
			return ((0==m_Compchar) && (0==m_Base) && (0==m_Tone));
		}
	};
public:

	typedef std::map<int, VietChar_>	ContVietnam;
	typedef std::list<int>				ContTone;
	typedef std::list<int>				ContBase;
	typedef std::list<int>				ContChar;

	CVietnamCharFinder() { Init(); }

	errno_t ConvToMultiByte(std::wstring const& wstrSrc, std::string &strTgt) const
	{
		size_t const src_size = wstrSrc.size();
		size_t const buffer_size = (src_size*4);

		wchar_t const* pSrc = wstrSrc.c_str();
		wchar_t* pDst = new wchar_t[buffer_size];
		::memset(pDst, 0, sizeof(wchar_t)*buffer_size);
		wchar_t* pCrs = pDst;
		VietChar_ temp;

		char* pMBBuffer = new char[buffer_size];

		while(*pSrc)
		{
			if(true==HasTone(*pSrc, temp))
			{
				*pCrs = temp.m_Base;
				++pCrs;
				*pCrs = temp.m_Tone;
			}
			else
			{
				*pCrs = *pSrc;
			}
			++pSrc;
			++pCrs;
		}

		size_t size = 0;
		errno_t const err = ::wcstombs_s( &size, pMBBuffer, buffer_size, pDst, buffer_size );
		if( !err && size )
		{
			strTgt = pMBBuffer;
		}
		SAFE_DELETE_ARRAY(pDst);
		SAFE_DELETE_ARRAY(pMBBuffer);

		return err;
	}

	errno_t ConvToUnicode(std::string const& strSrc, std::wstring& wstrTgt) const
	{
		char const* pSrc = strSrc.c_str();
		size_t const src_size = strSrc.size();
		size_t const buffer_size = (src_size*4);
		char* pDst = new char[buffer_size];
		char* pCrs = pDst;
		wchar_t* pUNIBuffer = new wchar_t[buffer_size];

		while(pSrc)
		{
			*pCrs = *pSrc;
			++pSrc;
			++pCrs;
			if(IsTone(*pSrc))
			{
				*pCrs = ComposeTone(*pCrs, *pSrc);
				++pSrc;
			}

		}

		size_t size = 0;
		errno_t const err = ::mbstowcs_s( &size, pUNIBuffer, buffer_size, pDst, buffer_size );
		if( !err && size )
		{
			wstrTgt = pUNIBuffer;
		}
		SAFE_DELETE_ARRAY(pDst);
		SAFE_DELETE_ARRAY(pUNIBuffer);

		return err;
	}

private:
	void Init()
	{//http://www.wazu.jp/gallery/Test_Vietnamese.html
		m_ContTone.push_back(0x0300);
		m_ContTone.push_back(0x0301);
		m_ContTone.push_back(0x0309);
		m_ContTone.push_back(0x0303);
		m_ContTone.push_back(0x0323);

		m_ContBase.push_back(0x0041);
		CharPush(0x00C1); CharPush(0x00C0); CharPush(0x1EA2); CharPush(0x00C3); CharPush(0x1EA0);
		m_ContBase.push_back(0x0061);
		CharPush(0x00E1); CharPush(0x00E0); CharPush(0x1EA3); CharPush(0x00E3); CharPush(0x1EA1);
		m_ContBase.push_back(0x0102);
		CharPush(0x1EAE); CharPush(0x1EB0); CharPush(0x1EB2); CharPush(0x1EB4); CharPush(0x1EB6);
		m_ContBase.push_back(0x0103);
		CharPush(0x1EAF); CharPush(0x1EB1); CharPush(0x1EB3); CharPush(0x1EB5); CharPush(0x1EB7);
		m_ContBase.push_back(0x00C2);
		CharPush(0x1EA4); CharPush(0x1EA6); CharPush(0x1EA8); CharPush(0x1EAA); CharPush(0x1EAC); 	 	
		m_ContBase.push_back(0x00E2);
		CharPush(0x1EA5); CharPush(0x1EA7); CharPush(0x1EA9); CharPush(0x1EAB); CharPush(0x1EAD);
		m_ContBase.push_back(0x0045);
		CharPush(0x00C9); CharPush(0x00C8); CharPush(0x1EBA); CharPush(0x1EBC); CharPush(0x1EB8);
		m_ContBase.push_back(0x0065);
		CharPush(0x00E9); CharPush(0x00E8); CharPush(0x1EBB); CharPush(0x1EBD); CharPush(0x1EB9);
		m_ContBase.push_back(0x00CA);
		CharPush(0x1EBE); CharPush(0x1EC0); CharPush(0x1EC2); CharPush(0x1EC4); CharPush(0x1EC6);
		m_ContBase.push_back(0x00EA);
		CharPush(0x1EBF); CharPush(0x1EC1); CharPush(0x1EC3); CharPush(0x1EC5); CharPush(0x1EC7);
		m_ContBase.push_back(0x0049);
		CharPush(0x00CD); CharPush(0x00CC); CharPush(0x1EC8); CharPush(0x0128); CharPush(0x1ECA);
		m_ContBase.push_back(0x0069);
		CharPush(0x00ED); CharPush(0x00EC); CharPush(0x1EC9); CharPush(0x0129); CharPush(0x1ECB);
		m_ContBase.push_back(0x004F);
		CharPush(0x00D3); CharPush(0x00D2); CharPush(0x1ECE); CharPush(0x00D5); CharPush(0x1ECC);
		m_ContBase.push_back(0x006F);
		CharPush(0x00F3); CharPush(0x00F2); CharPush(0x1ECF); CharPush(0x00F5); CharPush(0x1ECD);
		m_ContBase.push_back(0x00D4);
		CharPush(0x1ED0); CharPush(0x1ED2); CharPush(0x1ED4); CharPush(0x1ED6); CharPush(0x1ED8);
		m_ContBase.push_back(0x00F4);
		CharPush(0x1ED1); CharPush(0x1ED3); CharPush(0x1ED5); CharPush(0x1ED7); CharPush(0x1ED9);
		m_ContBase.push_back(0x01A0);
		CharPush(0x1EDA); CharPush(0x1EDC); CharPush(0x1EDE); CharPush(0x1EE0); CharPush(0x1EE2);
		m_ContBase.push_back(0x01A1);
		CharPush(0x1EDB); CharPush(0x1EDD); CharPush(0x1EDF); CharPush(0x1EE1); CharPush(0x1EE3);
		m_ContBase.push_back(0x0055);
		CharPush(0x00DA); CharPush(0x00D9); CharPush(0x1EE6); CharPush(0x0168); CharPush(0x1EE4);
		m_ContBase.push_back(0x0075);
		CharPush(0x00FA); CharPush(0x00F9); CharPush(0x1EE7); CharPush(0x0169); CharPush(0x1EE5);
		m_ContBase.push_back(0x01AF);
		CharPush(0x1EE8); CharPush(0x1EEA); CharPush(0x1EEC); CharPush(0x1EEE); CharPush(0x1EF0);
		m_ContBase.push_back(0x01B0);
		CharPush(0x1EE9); CharPush(0x1EEB); CharPush(0x1EED); CharPush(0x1EEF); CharPush(0x1EF1);
		m_ContBase.push_back(0x0059);
		CharPush(0x1EF2); CharPush(0x00DD); CharPush(0x1EF6); CharPush(0x1EF8); CharPush(0x1EF4);
		m_ContBase.push_back(0x0079);
		CharPush(0x1EF3); CharPush(0x00FD); CharPush(0x1EF7); CharPush(0x1EF9); CharPush(0x1EF5);
					   
		ContChar::const_iterator char_it = m_ContChar.begin();
		ContBase::const_iterator base_it = m_ContBase.begin();
		ContTone::const_iterator tone_it = m_ContTone.begin();

		int counting = 0;
		ContChar::value_type letter = 0;
		ContBase::value_type basechar = 0;
		ContTone::value_type tone = 0;
		while(m_ContChar.end()!=char_it)
		{
			letter = (*char_it);
			basechar = (*base_it);
			tone = (*tone_it);
			m_ContVietnam.insert(std::make_pair(letter, VietChar_(letter, basechar, tone)));
			++counting; ++char_it; ++tone_it;
			if(0==counting%5)	//precomposed characters
			{
				++base_it;
				if(base_it==m_ContBase.end())
				{
					base_it = m_ContBase.begin();
				}
				tone_it = m_ContTone.begin();
			}
		}
	}

	bool IsTone(char const input) const
	{
		ContTone::const_iterator it = std::find(m_ContTone.begin(), m_ContTone.end(), static_cast<int>(input));
		if(m_ContTone.end()!=it) { return true; }
		return false;
	}

	char ComposeTone(char const base, char const tone) const
	{
		ContVietnam::const_iterator it = 
			std::find_if(m_ContVietnam.begin(), m_ContVietnam.end(), CharFinder(base, tone));
		if(m_ContVietnam.end()!=it)
		{
			return static_cast<char>((*it).second.m_Compchar);
		}

		return 0;
	}

	bool HasTone(wchar_t const input, VietChar_& output) const
	{
		ContVietnam::const_iterator it = m_ContVietnam.find(input);
		if(m_ContVietnam.end()!=it)
		{
			output = (*it).second;
			return true;
		}
		return false;
	}

	void CharPush(int const value)
	{
		if(value) { m_ContChar.push_back(value); }
	}

	struct CharFinder
	{
		explicit CharFinder(ContBase::value_type const& base, ContTone::value_type const& tone)
		{
			m_base = base;
			m_tone = tone;
		}

		bool operator () (ContVietnam::value_type const& rhs) const
		{
			return (m_base == rhs.second.m_Base) && (m_tone == rhs.second.m_Tone);
		}

		ContBase::value_type m_base;
		ContTone::value_type m_tone;
	};

	ContVietnam		m_ContVietnam;
	ContTone		m_ContTone;
	ContBase		m_ContBase;
	ContChar		m_ContChar;
};

CVietnamCharFinder g_VietnamCharFinder;
//
//////////////////////////////////////////////////////////////////////////////////////////

vstring::vstring(void)
{
}

vstring::~vstring(void)
{
}
//////////////////////////////////////////////////////////////////////////////////////////
// CASTING
vstring::operator int const() const throw()
{
	return ::_wtoi(m_wstr.c_str());
}

vstring::operator __int64 const() const throw()
{
	return ::_wtoi64(m_wstr.c_str());
}

vstring::operator unsigned __int64 const() const throw()
{
	return ::_wcstoui64(m_wstr.c_str(), NULL, 10);
}

vstring::operator long const() const throw()
{
	return ::_wtol(m_wstr.c_str());
}

vstring::operator unsigned long const() const throw()
{
	return ::wcstoul(m_wstr.c_str(), NULL, 10);
}

vstring::operator float const()		const	throw()
{
	return static_cast<float>(operator double const());
}

vstring::operator double const() const throw()
{	
	return ::_wtof(m_wstr.c_str());
}

vstring::operator bool const() const throw()
{
	return m_wstr.empty();
}

vstring::operator wchar_t const*()	const	throw()
{
	return m_wstr.c_str();
}

vstring::operator std::string const() const	throw()
{
	return MB(m_wstr);
}

vstring::operator std::wstring const&() const	throw()
{
	return m_wstr;
}

vstring::operator vstring const&() const	throw()
{
	return (*this);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Operator ==
bool vstring::operator==(vstring const& vstrData)const
{
	return (m_wstr == vstrData.m_wstr);
}

bool vstring::operator == (std::string const& strData)const
{
	return (m_wstr == UNI(strData));
}

bool vstring::operator==(std::wstring const& strData)const
{
	return (m_wstr == strData);
}

////////////////////////////////////////////
//	operator=()
vstring& vstring::operator = (char const& rhs)
{
	std::string kStr(1, rhs);
	return operator = (UNI(kStr));
}

vstring& vstring::operator = (short const& rhs)
{
	return operator = (static_cast<int const>(rhs));
}

vstring& vstring::operator = (int const& rhs)
{
	wchar_t wszTemp[MAX_PATH] = {0,};
	errno_t const err = ::_itow_s(rhs, wszTemp, MAX_PATH, 10);
	if( err )
	{
		return *this;
	}
	return operator = (wszTemp);
}

vstring& vstring::operator = (long const& rhs)
{
	return operator = (static_cast<__int64 const>(rhs));
}

vstring& vstring::operator = (__int64 const& rhs)
{
	wchar_t wszTemp[MAX_PATH] = {0,};
	errno_t const err = ::_i64tow_s(rhs, wszTemp, MAX_PATH, 10);
	if( err )
	{
		return *this;
	}
	return operator = (wszTemp);
}

vstring& vstring::operator = (unsigned char const& rhs)
{
	return operator = (static_cast<unsigned int const>(rhs));
}

vstring& vstring::operator = (unsigned short const& rhs)
{
	return operator = (static_cast<unsigned int const>(rhs));
}

vstring& vstring::operator = (unsigned int const& rhs)
{
	return operator = (static_cast<unsigned __int64 const>(rhs));
}

vstring& vstring::operator = (unsigned long const& rhs)
{
	return operator = (static_cast<unsigned __int64 const>(rhs));
}

vstring& vstring::operator = (unsigned __int64 const& rhs)
{
	wchar_t wszTemp[MAX_PATH] = {0,};
	errno_t const err = ::_ui64tow_s(static_cast<unsigned __int64>(rhs), wszTemp, MAX_PATH, 10);
	if(err)
	{
		return *this;
	}
	return operator = (wszTemp);
}

vstring& vstring::operator = (bool const& rhs)
{
	if( rhs )
	{
		return operator = (L"true");
	}
	return operator = (L"false");
}

vstring& vstring::operator = (wchar_t const& rhs)
{
	std::wstring kWStr(1, rhs);
	return operator = (kWStr);
}

vstring& vstring::operator = (float const& rhs)
{
	wchar_t wszTemp[MAX_PATH] = {0,};
	::swprintf_s( wszTemp, MAX_PATH, _T("%0.4f"), rhs );

//	char buffer[_CVTBUFSIZE] = {0,};
//	errno_t err = _gcvt_s(buffer, _CVTBUFSIZE, rhs, 20);//float 은 플롯 대로 컨버팅. 더블로 캐스팅 하면 결과가 다르게 나옴.
//
//	m_wstr = UNI(buffer);
	return operator = (wszTemp);
}

vstring& vstring::operator = (double const& rhs)
{
	char buffer[_CVTBUFSIZE] = {0,};
	errno_t err = _gcvt_s(buffer, _CVTBUFSIZE, rhs, 20);//double은 더블대로
	return operator = (UNI(buffer));
}

vstring& vstring::operator = (void const* const& rhs)
{
	wchar_t wszTemp[MAX_PATH] = {0,};
	::swprintf_s( wszTemp, MAX_PATH, _T("0x%p"), rhs );
	return operator = (wszTemp);
}

vstring& vstring::operator = (char const* const& rhs)
{
	if( !rhs )
	{
		return operator = (L"Null szString");
	}
	return operator = (UNI(rhs));
}

vstring& vstring::operator = (wchar_t const* const& rhs)
{
	if(rhs)
	{
		m_wstr = rhs;
	}
	else
	{
		m_wstr = _T("Null wszString");
	}
	return *this;
}

vstring& vstring::operator = (std::string const& rhs)
{
	return operator = (UNI(rhs));
}

vstring& vstring::operator = (std::wstring const& rhs)
{
	m_wstr = rhs;
	return *this;
}

vstring& vstring::operator = (::GUID const& rhs)
{
	return operator = (BM::GUID(rhs));
}

vstring& vstring::operator = (DBTIMESTAMP const& rhs)
{
	return operator = (BM::DBTIMESTAMP_EX(rhs));
}

vstring& vstring::operator = (BM::GUID const& rhs)
{
	return operator = (rhs.str());
}

vstring& vstring::operator = (BM::DBTIMESTAMP_EX const& rhs)
{
	return operator =(rhs.operator const std::wstring());
}

vstring& vstring::operator = (vstring const& rhs)
{
	return operator =(rhs.m_wstr);
}

////////////////////////////////////////////
//	operator<()
bool vstring::operator < (vstring const& rhs)const
{
	return m_wstr < rhs.m_wstr;
}

////////////////////////////////////////////
//	etc

// etc static
errno_t vstring::ConvToUnicode(std::string const& strSrc, std::wstring& wstrTgt)
{
	size_t const src_size = strSrc.size();
	size_t const BUFFER_SIZE = (src_size*4);

	if(0 < src_size)
	{
		wchar_t* pUNIBuffer = new wchar_t[BUFFER_SIZE];
		
		size_t size = 0;
		errno_t const err = ::mbstowcs_s( &size, pUNIBuffer, BUFFER_SIZE, strSrc.c_str(), BUFFER_SIZE );
		if( !err && size )
		{
			wstrTgt = pUNIBuffer;
		}
		else
		{
			if (g_kLocal.ServiceRegion() == LOCAL_MGR::NC_VIETNAM)
			{
				g_VietnamCharFinder.ConvToUnicode(strSrc, wstrTgt);
			}
		}
		SAFE_DELETE_ARRAY(pUNIBuffer);
		return err;
	}
	return EINVAL;
}

std::wstring vstring::ConvToUnicode(std::string const& strSrc)
{
	std::wstring strOut;
	vstring::ConvToUnicode(strSrc, strOut);
	return strOut;
}

std::wstring vstring::ConvToUnicodeFromUTF8(std::string const& strSrc)
{
	std::wstring strOut;
	size_t const src_size = strSrc.size();

	if(0 < src_size)
	{
		int conv_length = ::MultiByteToWideChar(CP_UTF8, 0, strSrc.c_str(), src_size + 1, NULL, NULL);
		if (0 < conv_length)
		{
			wchar_t* pUNIBuffer = new wchar_t[conv_length + 1];
			int ret = ::MultiByteToWideChar(CP_UTF8, 0, strSrc.c_str(), src_size + 1, pUNIBuffer, conv_length);
			if (0 < ret)
			{
				pUNIBuffer[conv_length] = 0;
				strOut = pUNIBuffer;
			}
			SAFE_DELETE_ARRAY(pUNIBuffer);			
		}
	}
	return strOut;
}

errno_t vstring::ConvToMultiByte(std::wstring const& wstrSrc, std::string &strTgt)
{
	size_t const src_size = wstrSrc.size();
	size_t const buffer_size = (src_size*4);

	if(0 < src_size)
	{
		char* pMBBuffer = new char[buffer_size];
		if(pMBBuffer)
		{
			size_t size = 0;
			errno_t const err = ::wcstombs_s( &size, pMBBuffer, buffer_size, wstrSrc.c_str(), buffer_size );
			if( !err && size )
			{
				strTgt = pMBBuffer;
			}
			else
			{
				if (g_kLocal.ServiceRegion() == LOCAL_MGR::NC_VIETNAM)
				{
					g_VietnamCharFinder.ConvToMultiByte(wstrSrc, strTgt);
				}
			}

			SAFE_DELETE_ARRAY(pMBBuffer);
			return err;
		}
	}

	return EINVAL;
}

std::string vstring::ConvToMultiByte(std::wstring const& wstrSrc)
{
	std::string strOut;
	vstring::ConvToMultiByte(wstrSrc, strOut);
	return strOut;
}


size_t vstring::size() const
{
	return m_wstr.size();
}

std::wstring vstring::ConvToHex(DWORD const dwIn)
{
	wchar_t kTemp[MAX_PATH] = {0, };
	swprintf_s(kTemp, MAX_PATH, _T("0x%08X"), dwIn);
	return std::wstring(kTemp);
}

std::wstring vstring::ConvToHex(BYTE const* pData, size_t const iArrayCount, size_t const iLimitArray, bool const bAddHeader)
{
	return ConvToHex((char const* )(pData), iArrayCount, iLimitArray, bAddHeader);
}

std::wstring vstring::ConvToHex(char const* pData, size_t const iArrayCount, size_t const iLimitArray, bool const bAddHeader)
{
	if( !pData )
	{
		return std::wstring(L"Null Pointer");
	}

	size_t const iMultiply = 2; // 'a' --> 0x?? 로 두배 뻥튀기
	size_t const iOneBytePerChar = 2;

	std::wstring kTemp;
	if (bAddHeader)
	{
		kTemp += L"0x";
	}
	kTemp.reserve( kTemp.size() + (iArrayCount * iMultiply) ); // '0x' + 'xxxxx................'

	size_t iCur = 0;
	char const* const pEnd = (pData + iArrayCount);
	char const* pBegin = pData;
	wchar_t szTemp[10] = {0 ,};
	while( pEnd != pBegin )
	{
		if( iLimitArray < iCur )
		{
			kTemp.append( std::wstring(L".....") );
			break;
		}

		_stprintf_s(szTemp, 10, L"%02X", BYTE(*pBegin));
		kTemp.push_back( szTemp[0] );
		kTemp.push_back( szTemp[1] );

		++iCur;
		++pBegin;
	}
	return kTemp;
}

// etc member
int vstring::icmp(vstring const& vstrData) const
{
	return icmp( vstrData.m_wstr );
}

int vstring::icmp(std::wstring const& strData) const
{
	return ::_wcsicmp( m_wstr.c_str(), strData.c_str() );
}
