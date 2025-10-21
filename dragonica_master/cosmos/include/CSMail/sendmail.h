#pragma once

#include "BM/STLSupport.h"
#include <tchar.h>


#ifdef _MT_
	#pragma comment(lib, "CSMail_MT.lib")
#endif

#ifdef _MTd_
	#pragma comment(lib, "CSMail_MTd.lib")
#endif

#ifdef _MTo_
	#ifdef _ANTI_HACK_
		#pragma comment(lib, "CSMail_MToAH.lib")
	#else
		#pragma comment(lib, "CSMail_MTo.lib")
	#endif
#endif

#ifdef _MD_
	#pragma comment(lib, "CSMail_MD.lib")
#endif

#ifdef _MDd_
	#pragma comment(lib, "CSMail_MDd.lib")
#endif

#ifdef _MDo_
	#ifdef _ANTI_HACK_
		#pragma comment(lib, "CSMail_MDoAH.lib")
	#else
		#pragma comment(lib, "CSMail_MDo.lib")
	#endif
#endif

#pragma comment( lib, "ws2_32.lib")

namespace CS_MAIL
{
	typedef std::list< std::wstring > STRING_LIST;

	typedef struct tagTargetList
	{
		tagTargetList()
		{
			lstTarget.clear();
		}

		void Insert(const STRING_LIST::value_type &rTarget)
		{
			lstTarget.push_back( rTarget );
		}

		STRING_LIST lstTarget;
	}TARGET_LIST;

	class CSendMail
	{
	public:
		CSendMail();
		virtual ~CSendMail();

	public:
		bool Init(std::wstring const& strFileName = _T("./MailConfig.ini"), bool const bIsINI = true );

		//	송신자, 수신자, 제목, 내용 입력.
		bool Send(	std::wstring const& strSender, std::wstring const& strTarget, 
					std::wstring const& strTitle, std::wstring const& strBody, TARGET_LIST const& TL_AttachList = TARGET_LIST() );
		//	제목, 내용만을 입력. 스크립트의 주소록에 있는 수신인에게 발신.
		bool SendAuto( std::wstring const& strTitle, std::wstring const& strBody, TARGET_LIST const& TL_AttachList = TARGET_LIST() );

	protected:
		bool LoadINI(std::wstring const& strFileName = _T("./MailConfig.ini"));
		bool SendAction( void *pData );

	protected:
		std::wstring m_strID;
		std::wstring m_strPW;

		std::wstring m_strServerInetAddr;
		std::wstring m_strServerHostName;
		std::wstring m_strSenderMailAddr;
		
		TARGET_LIST	m_TargetList;
		TARGET_LIST	m_AttachList;//이게 왜 static 이었나?
	};
};