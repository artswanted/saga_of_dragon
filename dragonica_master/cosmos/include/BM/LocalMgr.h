#pragma once

#include "BM/ClassSupport.h"

namespace LOCAL_MGR
{
	typedef enum eNationCode
	{
		NC_NOT_SET	= 0,//NOTSET일 경우는 무조건 에러처리합니다.
		NC_KOREA	= 1,
		NC_CHINA	= 2,
		NC_TAIWAN	= 3,	
		NC_THAILAND	= 4,
		NC_HONGKONG	= 5,
		NC_USA		= 6,
		NC_JAPAN	= 7,
		NC_EU		= 8,//유럽
		NC_SINGAPORE		= 9,
		NC_FRANCE	= 10,
		NC_GERMANY	= 11,
		NC_MALAYSIA	= 12,
		NC_PHILIPPINES		= 13,
		NC_AUSTRALIA		= 14,
		NC_INDONESIA		= 15,
		NC_VIETNAM			= 16,
		NC_NEW_ZEALAND		= 17,
		NC_LAOS		= 18,
		NC_CAMBODIA	= 19,
		NC_MACAO	= 20,
		NC_CANADA	= 21,
		NC_MEXICO	= 22,
		NC_BRAZIL	= 23,
		NC_ENGLAND	= 24,
		NC_RUSSIA	= 25,
		NC_SPAIN	= 26,


		NC_DEVELOP	= 63,	// 어떤 국가에도 
	}NATION_CODE;

	typedef enum eServiceType
	{
		ST_NOT_SET			= 0x00000000,
		ST_DEVELOP			= 0x00000001,//		//개발자 모드
		ST_CUSTOM1			= 0x00000002,
		ST_CUSTOM2			= 0x00000004,
		ST_CUSTOM3			= 0x00000008,
		ST_CUSTOM4			= 0x00000010,
		ST_EVENT1			= 0x00000020,//	Event
		ST_EVENT2			= 0x00000040,//	Event
		ST_EVENT3			= 0x00000080,//	Event
		ST_EVENT4			= 0x00000100,//	Event

		ST_WALLOWDEFEND		= 0x00010000,// 중독방지
	}SERVICE_TYPE;

	class CLocal
	{
	public:
		CLocal();
		virtual ~CLocal();
	public://SetNationCode 류의 함수는 만들지 않습니다. 게임 서비스 중에 쓸일이 없기 때문입니다.
		//NATION_CODE	NationCode()const {return m_eNationCode;}
		DWORD ServiceType()const {return m_dwServiceTypeFlag;}

		bool LoadFromINI(std::wstring const& strINIFileName = _T(".\\Local.ini"), wchar_t const* szDefaultLocale = NULL);
		bool LoadFromINB(std::wstring const& strINBFileName, wchar_t const* szDefaultLocale = NULL);

		std::string const& GetLocale()const;
		void Clear();

		bool IsAbleServiceType(DWORD const dwFlag) const;
		bool IsAbleServiceName(wchar_t const* szServiceName) const;
		bool IsAbleNationCode(wchar_t const* szNationCode) const;
		DWORD ServiceRegion() const { return m_eServiceRegion; }
		bool IsServiceRegion(DWORD const dwRegion) const { return (dwRegion == m_eServiceRegion); }
		DWORD NationCode() const { return m_eNationCode; }
		char const* GetSystemNationCode();
	protected:
		void NationCode(NATION_CODE const eNC){m_eNationCode = eNC;}
		NATION_CODE GetNationCode(wchar_t const* szNation) const;
		char const* GetNationCodeStr() const;
	protected:
		void ServiceType(DWORD const dwST){m_dwServiceTypeFlag = dwST;}
		bool CheckValue();

		void	SetLCNumeric(BM::vstring const &kLCNumeric);

	protected:
		NATION_CODE		m_eNationCode;		// 실제 서비스 되는 국가
		NATION_CODE		m_eServiceRegion;	// SP의 국가(서비스 주체자의 구별)
		DWORD			m_dwServiceTypeFlag;
		std::string		m_strLanguage;

	protected:
		CLocal operator=(CLocal const& old);//대입연산자 사용 불가.
		CLocal(CLocal const& old);//복사생성자 사용 불가.
	};
};

#define g_kLocal SINGLETON_STATIC(LOCAL_MGR::CLocal)