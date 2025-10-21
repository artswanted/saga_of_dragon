#pragma once
#define WIN32_LEAN_AND_MEAN
#include <guiddef.h>
#include <objbase.h>
#include <string>
#include <oledb.h>
#include <unordered_map>
#include <tchar.h>
#include "BM/ClassSupport.h"
#include "ace/Time_value.h"

namespace BM
{
#pragma pack(1)
	class GUID
	{
	public:
		GUID();
		explicit GUID(::GUID const& rGuid);
		explicit GUID(std::string const& strGuid);
		explicit GUID(std::wstring const& strGuid);
		~GUID();
	public:
		void Clear();
		bool Generate();
		operator const ::GUID& ()const;
		operator const ::GUID* ()const;

		//operator size_t ()const;

		bool operator < (BM::GUID const& rhs) const;
		bool operator > (BM::GUID const& rhs) const;
		bool operator == (BM::GUID const& rhs) const;
		bool operator != (BM::GUID const& rhs) const;
		
		bool Set(std::string const& strGuid);
		bool Set(std::wstring const& strGuid);
		bool SetUniqueidentifier( char const* szGuid );
		std::wstring str() const;

		bool IsNull()const;
		bool IsNotNull()const;
		
		static BM::GUID Create();
		static BM::GUID const&  NullData();
		static bool IsNull(BM::GUID const& rkGuid);
		static bool IsNotNull(BM::GUID const& rkGuid);
	protected:
		::GUID m_Guid;
	};

	class CGuid_hash_compare
	{
	public:

		enum {
			bucket_size = 4,
			min_buckets = 8
		};

		size_t operator()(BM::GUID const & kGuid) const
		{
			::GUID const rkGuid = kGuid.operator const ::GUID&();
			size_t hash_value = 0;
			hash_value ^= rkGuid.Data1;
			hash_value ^= rkGuid.Data2;
			hash_value ^= rkGuid.Data3;

			size_t const* pHi = (size_t const*)&rkGuid.Data4[0];
			size_t const* pLow = (size_t const*)&rkGuid.Data4[4];

			hash_value ^= (*pHi);
			hash_value ^= (*pLow);

			return hash_value;
		}

		bool operator()(const BM::GUID & lhs, const BM::GUID & rhs) const
		{
			return (lhs < rhs);
		}
	};

	struct tagPackedTime;

	//시간관련 작업은 DBTIMESTAMP_EX 로 할것. tagPacked 는 세이브 할때나 써라.
	class DBTIMESTAMP_EX
	{//계산 때문에 음수가 필요함.
	public:

		short year;
		short month;
		short day;
		short hour;
		short minute;
		short second;
		long fraction;

		typedef enum eLimitValue
		{
			LIMIT_YEAR=		9999,
			LIMIT_MONTH=    12,
			LIMIT_DAY=		31,
			LIMIT_HOUR=		24,
			LIMIT_MIN=		59,
			LIMIT_SEC=      59
		}E_LIMIT_VALUE;

		DBTIMESTAMP_EX();
		explicit DBTIMESTAMP_EX( ::DBTIMESTAMP const& rhs );
		explicit DBTIMESTAMP_EX( SYSTEMTIME const& rhs );
		explicit DBTIMESTAMP_EX( BM::tagPackedTime const& rhs );
		~DBTIMESTAMP_EX(){}//상속 해서 쓰지 마세요.

		void Clear();

		operator DBTIMESTAMP const()	const	throw();
		operator const ACE_Time_Value()	const	throw();
		operator std::wstring const()	const	throw();

		void operator = ( DBTIMESTAMP const& rhs );
		bool operator == ( DBTIMESTAMP_EX const& rhs )const;
		bool operator != ( DBTIMESTAMP_EX const& rhs )const;

		void operator = ( BM::tagPackedTime const& rhs );
		void operator = ( SYSTEMTIME const& rhs );

		bool operator < ( DBTIMESTAMP_EX const&  rhs ) const;
		bool operator > ( DBTIMESTAMP_EX const&  rhs ) const;
		void SetLocalTime();

		bool IsNull() const;
		bool IsCorrect() const;
		void DateFromString(const char* sFormat, const char* sData);
	};
	
	typedef struct tagPackedTime
	{//비트 플래그라. -값 되면 영역 표현하는데 한계가 있다.
		CLASS_DECLARATION_S_BIT(DWORD, Year, 6);	//0 이면 2000년
		CLASS_DECLARATION_S_BIT(DWORD, Month, 4);
		CLASS_DECLARATION_S_BIT(DWORD, Day, 5);
		CLASS_DECLARATION_S_BIT(DWORD, Hour, 5);
		CLASS_DECLARATION_S_BIT(DWORD, Min, 6);
		CLASS_DECLARATION_S_BIT(DWORD, Sec, 6);
		
		typedef enum eMaxValue
		{
			MAX_YEAR		= 64	-1,
			MAX_MONTH		= 16	-1,
			MAX_DAY			= 32	-1,
			MAX_HOUR		= 32	-1,
			MAX_MIN			= 64	-1,
			MAX_SEC			= 64	-1,
			
			BASE_YEAR		= 2000, //이 구조체는 2000~2063 년 까지 표현 가능
			GAB_FROM_TM_YEAR= BASE_YEAR-1900, //tm 구조체 기준은 1900
		}EMaxValue;

		tagPackedTime();
		explicit tagPackedTime(DBTIMESTAMP_EX const& rhs);
		explicit tagPackedTime(SYSTEMTIME const& rhs);
		void Clear();
		void SetLocalTime();
		bool IsNull() const;
		static tagPackedTime LocalTime();

		bool operator < (BM::tagPackedTime const& rhs)const;
		bool operator == (BM::tagPackedTime const& rhs)const;
		void operator = (DBTIMESTAMP const& rhs);
		void operator = (DBTIMESTAMP_EX const& rhs);
		void operator = (const struct tm &rhs);
		void operator = (SYSTEMTIME const& rhs);
		//+,- Operator 를 만들지 말것. 단위에 한계가 있다.
		operator DBTIMESTAMP() const;
		operator SYSTEMTIME() const;

		void SetTime(DWORD const dwTime)
		{
			*(DWORD*)(this) = dwTime;
		}

		DWORD GetTime() const
		{
			return *(DWORD*)(this);
		}

	}PgPackedTime;
#pragma pack ()
}
