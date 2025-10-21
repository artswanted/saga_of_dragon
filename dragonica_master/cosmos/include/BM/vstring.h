#pragma once

#define WIN32_LEAN_AND_MEAN
#include <string>
#include <list>
#include <windows.h>
#include <oledb.h>
#include "BM/guid.h"

namespace BM
{
	class vstring;

	struct SReplaceStr
	{
		template< typename T >
		SReplaceStr( std::wstring const &kFindStr, T const &kReplaceData )
			:	m_kFindStr(kFindStr)
			,	m_kReplaceStr(static_cast<std::wstring>(vstring(kReplaceData)))
		{}

		template< >
		SReplaceStr( std::wstring const &kFindStr, std::wstring const &kReplaceData )
			:	m_kFindStr(kFindStr)
			,	m_kReplaceStr(kReplaceData)
		{}

		std::wstring	m_kFindStr;
		std::wstring	m_kReplaceStr;

	private:
		SReplaceStr();
	};

	class vstring
	{//std::string을 절대 상속 받아 쓰지말것. 
	public:
		vstring();

		template< typename T >
		explicit vstring(T const& rhs)
		{
			operator=(rhs);
		}
		
		template< typename T >
		explicit vstring(T const* const& rhs)
		{
			operator=(rhs);
		}

		template< typename T >
		explicit vstring( T const &rhs, wchar_t *pkFormat )
		{
			Set( rhs, pkFormat );
		}
		
		virtual ~vstring(void);

	public://변환
		operator int const()	const	throw();
		operator long const()	const	throw();
		operator __int64 const()	const	throw();
		operator unsigned long const()	const	throw();
		operator unsigned __int64 const()	const	throw();
		
		operator float const()		const	throw();
		operator double const()		const	throw();
		operator bool const()		const	throw();
		operator wchar_t const*()	const	throw();
		operator vstring const&()	const	throw();
		operator std::string const() const	throw();
		operator std::wstring const&()	const	throw();
		
	public://연산자
		vstring& operator = (char const& rhs);
		vstring& operator = (wchar_t const& rhs);
		vstring& operator = (short const& rhs);
		vstring& operator = (int const& rhs);
		vstring& operator = (long const& rhs);
		vstring& operator = (__int64 const& rhs);
		vstring& operator = (unsigned char const& rhs);
		vstring& operator = (unsigned short const& rhs);
		vstring& operator = (unsigned int const& rhs);
		vstring& operator = (unsigned long const& rhs);
		vstring& operator = (unsigned __int64 const& rhs);
		vstring& operator = (float const& rhs);
		vstring& operator = (double const& rhs);
		vstring& operator = (bool const& rhs);

		vstring& operator = (void const* const& rhs);
		vstring& operator = (char const* const& rhs);
		vstring& operator = (wchar_t const* const& rhs);

		vstring& operator = (std::string const& rhs);
		vstring& operator = (std::wstring const& rhs);

		vstring& operator = (::GUID const& rhs);
		vstring& operator = (::DBTIMESTAMP const& rhs);
		vstring& operator = (BM::GUID const& rhs);
		vstring& operator = (BM::DBTIMESTAMP_EX const& rhs);
		vstring& operator = (vstring const& rhs);
		
		template< typename T1 >
		vstring& operator += (T1 const& rhs)
		{
			vstring kTemp(rhs);
			m_wstr += kTemp.m_wstr;
			return *this;
		}
		
		template< typename T1 >
		vstring& operator += (T1 const* const& rhs)
		{
			vstring kTemp(rhs);
			m_wstr += kTemp.m_wstr;
			return *this;
		}

		template<typename T>
		vstring & operator <<(T const& rhs)
		{//+= 과 같은역할
			return operator +=(rhs);
		}

		template< >
		vstring & operator <<( SReplaceStr const &kReplaceStr )
		{
			Replace( kReplaceStr.m_kFindStr, kReplaceStr.m_kReplaceStr );
			return *this;
		}

		template<typename T>
		vstring & operator <<(T const* const& rhs)
		{//+= 과 같은역할
			return operator +=(rhs);
		}

		
		
		template< typename T1 >
		vstring operator + (T1 const& rhs)const
		{
			vstring org(*this);
			vstring kTemp(rhs);
			org += kTemp;
			return org;
		}

		template< typename T1 >
		vstring operator + (T1 const* const& rhs)const
		{
			vstring org(*this);
			vstring kTemp(rhs);
			org += kTemp;
			return org;
		}

		bool operator == (vstring const& vstrData)const;
		bool operator == (std::string const& strData)const;
		bool operator == (std::wstring const& strData)const;

		bool operator < (vstring const& rhs)const;

		size_t size() const;

	public://함수
		// static
		errno_t static ConvToUnicode(std::string const& strSrc, std::wstring& wstrTgt);
		errno_t static ConvToMultiByte(std::wstring const& wstrSrc, std::string &strTgt);
		
		std::wstring static ConvToUnicode(std::string const& strSrc);
		std::wstring static ConvToUnicodeFromUTF8(std::string const& strSrc);
		std::string static ConvToMultiByte(std::wstring const& wstrSrc);

		template<typename T_STRING>
		static void ConvToUPR(T_STRING &wstrSrc);

		template<typename T_STRING>
		static void ConvToLWR(T_STRING &strSrc);

		static std::wstring ConvToHex(DWORD const dwIn);
		static std::wstring ConvToHex(char const* pData, size_t const iArrayCount, size_t const iLimitArray = 1000, bool const bAddHeader=true);
		static std::wstring ConvToHex(BYTE const* pData, size_t const iArrayCount, size_t const iLimitArray = 1000, bool const bAddHeader=true);

		template<typename T_STRING>
		static void CutTextByKey(T_STRING const& kOrg, T_STRING const& kKey, std::list< T_STRING > &rOut, bool bEmptyLineUse = false);

		// member
		int icmp(vstring const& vstrData) const;
		int icmp(std::wstring const& strData) const;

		template<typename T>
		HRESULT Set( T const &rhs, wchar_t *pkFormat );

		template<typename T_RETURN>
		HRESULT GetNumber( std::wstring const& kFormat, T_RETURN& kReturn);

		template< typename T_REPLACE >
		void Replace( std::wstring const &kFindStr, T_REPLACE const &kReplaceData );

		template< >
		void vstring::Replace( std::wstring const &kFindStr, std::wstring const &kReplaceData );

		template< >
		void vstring::Replace( std::wstring const &kFindStr, std::string const &kReplaceData );

		template< >
		void vstring::Replace( std::wstring const &kFindStr, vstring const &kReplaceData );

	private:
		std::wstring m_wstr;
	};
};

#define UNI(str) BM::vstring::ConvToUnicode(str).c_str()
#define MB(wstr) BM::vstring::ConvToMultiByte(wstr).c_str()
#define UPR(str) BM::vstring::ConvToUPR(str)
#define LWR(str) BM::vstring::ConvToLWR(str)
#define HEX(integer) BM::vstring::ConvToHex(integer)
#define CHAR2HEX(str, size)	BM::vstring::ConvToHex(str, size)

#include "vstring.inl"
