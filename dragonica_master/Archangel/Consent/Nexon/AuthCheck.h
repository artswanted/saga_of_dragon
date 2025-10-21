#pragma once

#ifndef EXTERN_C
	#ifdef __cplusplus
		#define EXTERN_C    extern "C"
	#else
		#define EXTERN_C    extern
	#endif
#endif

#ifndef	AUTHCHECK_EXPORTS
	#define AUTHCHECK_API		EXTERN_C __declspec(dllimport)
#else
	#define	AUTHCHECK_API		EXTERN_C __declspec(dllexport)
#endif

#ifndef IN
	#define IN
#endif

#ifndef OUT
	#define OUT
#endif

enum AuthCheckError
{
	AUTHCHECK_ERROR_OK 	= 0,							//	OK
	AUTHCHECK_ERROR_INVALID_ARGUMENT,					//	Argument is not valid
	AUTHCHECK_ERROR_INVALID_PASSPORT,					//	NexonPassport is not valid
	AUTHCHECK_ERROR_TIMEOUT,							//	[obsolete] same as AUTHCHECK_ERROR_EXPIRED
	AUTHCHECK_ERROR_EXPIRED = AUTHCHECK_ERROR_TIMEOUT,	//	NexonPassport is valid, but arleady expired
	AUTHCHECK_ERROR_AUTH_SERVER_SHUTDOWN,				//	Authentication server is shutdown
	AUTHCHECK_ERROR_UNSUPPORTED_LOCALE,					//	Unsupported Locale ID
	AUTHCHECK_ERROR_SET_LOCALE_NEEDED,					//	call AuthCheck_SetLocale first
	AUTHCHECK_ERROR_NO_SERVER_INFORMATION,				//	no server information in config file
	AUTHCHECK_ERROR_USER_IP_MISMATCHED,					//	client ip address is mismatched with sso servers data
	AUTHCHECK_ERROR_NO_DECRYPTION_KEY,					//	There is no passport decryption key. Check AuthCheck_LoadKey function or key file.
};

#ifndef _NMLOCALEID_DEFINED_
#define _NMLOCALEID_DEFINED_
enum	NMLOCALEID
{
	kLocaleID_Null			= 0x00000000,

	kLocaleID_KR			= 0x00000001,	//	Korea, Republic of
	kLocaleID_KR_Test		= 0x10000001,	//	Test

	kLocaleID_JP			= 0x00000100,	//	Japan
	kLocaleID_JP_Test		= 0x10000100,
	kLocaleID_JP2			= 0x00000101,	//	Japan arad
	kLocaleID_TW			= 0x00000102,	//	Taiwan CSO
	kLocaleID_JP3			= 0x00000103,	//	Japan TalesWeaver
	kLocaleID_JP4			= 0x00000104,	//	Japan All(new IDC)
	kLocaleID_CN_CNC		= 0x00000111,	//	China / CNC
	kLocaleID_CN_CT			= 0x00000112,	//	China / CT
	kLocaleID_US			= 0x00000200,	//	United States
	kLocaleID_EU			= 0x00000300,	//	Europe
};
#endif

#ifndef _NMPASSPORTTYPE_DEFINED_
#define _NMPASSPORTTYPE_DEFINED_
enum	PassportType
{
	kPassportType_Undefined,
	kPassportType_Nexon,
	kPassportType_Guest,
};
#endif

/*
returns :	AUTHCHECK_ERROR_OK
			AUTHCHECK_ERROR_UNSUPPORTED_LOCALE
*/
AUTHCHECK_API
AuthCheckError AuthCheck_SetLocale
( 
	IN		NMLOCALEID		uLocale
);

AUTHCHECK_API
void AuthCheck_SetTimeout
( 
	IN		UINT32			uMilliseconds
);

AUTHCHECK_API
void AuthCheck_SetIntranetMode
( 
	IN		BOOL			bSet
);

AUTHCHECK_API
void AuthCheck_SetSoapHostA
( 
	IN		const char *	szHost						// 32
);

AUTHCHECK_API
void AuthCheck_SetSoapHostW
( 
	IN		const WCHAR *	szHost						// 32
);

AUTHCHECK_API
PassportType AuthCheck_GetPassportTypeA
(
	IN		const char *	szPassport					// 1024
);

AUTHCHECK_API
PassportType AuthCheck_GetPassportTypeW
(
	IN		const WCHAR	*	szPassport					// 1024
);

AUTHCHECK_API
void AuthCheck_LoadKeyA
(
	IN		const char *	pszKeyConfigFileName
);

AUTHCHECK_API
void AuthCheck_LoadKeyW
(
	IN		const WCHAR *	pszKeyConfigFileName
);

/*
returns :	AUTHCHECK_ERROR_OK
			AUTHCHECK_ERROR_INVALID_ARGUMENT
			AUTHCHECK_ERROR_INVALID_PASSPORT
			AUTHCHECK_ERROR_EXPIRED
*/
AUTHCHECK_API
AuthCheckError AuthCheck_LocalCheckPassportA
( 
	IN		const char*		szPassport,					// 1024
	OUT		char*			pszId			= NULL,		// 32
	OUT		UINT32*			pnNexonSN		= NULL,
	OUT		char*			pszLoginIp		= NULL,		// 32
	OUT		UINT32*			puSex			= NULL,
	OUT		UINT32*			puAge			= NULL,
	OUT		UINT32*			puPwdHash		= NULL,
	OUT		UINT32*			puSsnHash		= NULL,
	OUT		UINT32*			puFlag0			= NULL,
	OUT		UINT32*			puFlag1			= NULL,
	OUT		UINT32*			pnServerHash	= NULL,
	OUT		char*			pszServerName	= NULL,		// 32
	OUT		char*			pszNationCode	= NULL		// 3
);

/*
returns :	AUTHCHECK_ERROR_OK
			AUTHCHECK_ERROR_INVALID_ARGUMENT
			AUTHCHECK_ERROR_INVALID_PASSPORT
			AUTHCHECK_ERROR_EXPIRED
*/
AUTHCHECK_API
AuthCheckError AuthCheck_LocalCheckPassportW
( 
	IN		const WCHAR*	szPassport,					// 1024
	OUT		WCHAR*			pszId			= NULL,		// 32
	OUT		UINT32*			pnNexonSN		= NULL,
	OUT		WCHAR*			pszLoginIp		= NULL,		// 32
	OUT		UINT32*			puSex			= NULL,
	OUT		UINT32*			puAge			= NULL,
	OUT		UINT32*			puPwdHash		= NULL,
	OUT		UINT32*			puSsnHash		= NULL,
	OUT		UINT32*			puFlag0			= NULL,
	OUT		UINT32*			puFlag1			= NULL,
	OUT		UINT32*			pnServerHash	= NULL,
	OUT		WCHAR*			pszServerName	= NULL,		// 32
	OUT		WCHAR*			pszNationCode	= NULL		// 3
);

/*
returns :	AUTHCHECK_ERROR_OK
AUTHCHECK_ERROR_INVALID_ARGUMENT
AUTHCHECK_ERROR_INVALID_PASSPORT
AUTHCHECK_ERROR_EXPIRED
*/
AUTHCHECK_API
AuthCheckError AuthCheck_LocalCheckPassport2A
( 
 IN			const char*		szPassport,					// 1024
 OUT		char*			pszId			= NULL,		// 32
 OUT		UINT32*			pnNexonSN		= NULL,
 OUT		char*			pszLoginIp		= NULL,		// 32
 OUT		UINT32*			puSex			= NULL,
 OUT		UINT32*			puAge			= NULL,
 OUT		UINT32*			puPwdHash		= NULL,
 OUT		UINT32*			puSsnHash		= NULL,
 OUT		UINT32*			puFlag0			= NULL,
 OUT		UINT32*			puFlag1			= NULL,
 OUT		UINT32*			pnServerHash	= NULL,
 OUT		char*			pszServerName	= NULL,		// 32
 OUT		char*			pszMeta			= NULL		// 1024
 );

/*
returns :	AUTHCHECK_ERROR_OK
AUTHCHECK_ERROR_INVALID_ARGUMENT
AUTHCHECK_ERROR_INVALID_PASSPORT
AUTHCHECK_ERROR_EXPIRED
*/
AUTHCHECK_API
AuthCheckError AuthCheck_LocalCheckPassport2W
( 
 IN			const WCHAR*	szPassport,					// 1024
 OUT		WCHAR*			pszId			= NULL,		// 32
 OUT		UINT32*			pnNexonSN		= NULL,
 OUT		WCHAR*			pszLoginIp		= NULL,		// 32
 OUT		UINT32*			puSex			= NULL,
 OUT		UINT32*			puAge			= NULL,
 OUT		UINT32*			puPwdHash		= NULL,
 OUT		UINT32*			puSsnHash		= NULL,
 OUT		UINT32*			puFlag0			= NULL,
 OUT		UINT32*			puFlag1			= NULL,
 OUT		UINT32*			pnServerHash	= NULL,
 OUT		WCHAR*			pszServerName	= NULL,		// 32
 OUT		WCHAR*			pszMeta			= NULL		// 1024
 );

/*
returns :	AUTHCHECK_ERROR_OK
			AUTHCHECK_ERROR_INVALID_ARGUMENT
			AUTHCHECK_ERROR_INVALID_PASSPORT
			AUTHCHECK_ERROR_EXPIRED
			AUTHCHECK_ERROR_AUTH_SERVER_SHUTDOWN
*/
AUTHCHECK_API
AuthCheckError AuthCheck_CheckPassportA
( 
	IN		const char*		szPassport,					// 1024
	IN		const char*		szUserIp,					// 32
	IN		BOOL			bRemoteOnly		= TRUE,
	OUT		char*			pszId			= NULL,		// 32
	OUT		UINT32*			pnNexonSN		= NULL,
	OUT		char*			pszLoginIp		= NULL,		// 32
	OUT		UINT32*			puSex			= NULL,
	OUT		UINT32*			puAge			= NULL,
	OUT		UINT32*			puPwdHash		= NULL,
	OUT		UINT32*			puSsnHash		= NULL,
	OUT		UINT32*			puFlag0			= NULL,
	OUT		UINT32*			puFlag1			= NULL,
	OUT		char*			pszMeta			= NULL,		// 1024
	OUT		char*			pszNationCode	= NULL		// 3
);

/*
returns :	AUTHCHECK_ERROR_OK
			AUTHCHECK_ERROR_INVALID_ARGUMENT
			AUTHCHECK_ERROR_INVALID_PASSPORT
			AUTHCHECK_ERROR_EXPIRED
			AUTHCHECK_ERROR_AUTH_SERVER_SHUTDOWN
*/
AUTHCHECK_API
AuthCheckError AuthCheck_CheckPassportW
( 
	IN		const WCHAR*	szPassport,					// 1024
	IN		const WCHAR*	szUserIp,					// 32
	IN		BOOL			bRemoteOnly		= TRUE,
	OUT		WCHAR*			pszId			= NULL,		// 32
	OUT		UINT32*			pnNexonSN		= NULL,
	OUT		WCHAR*			pszLoginIp		= NULL,		// 32
	OUT		UINT32*			puSex			= NULL,
	OUT		UINT32*			puAge			= NULL,
	OUT		UINT32*			puPwdHash		= NULL,
	OUT		UINT32*			puSsnHash		= NULL,
	OUT		UINT32*			puFlag0			= NULL,
	OUT		UINT32*			puFlag1			= NULL,
	OUT		WCHAR*			pszMeta			= NULL,		// 1024
	OUT		WCHAR*			pszNationCode	= NULL		// 3
);

AUTHCHECK_API
UINT32 AuthCheck_GetHashValueA
( 
	const char* szString
);

AUTHCHECK_API
UINT32 AuthCheck_GetHashValueW
(
	const WCHAR* wszString
);

/*
returns :	AUTHCHECK_ERROR_OK
			AUTHCHECK_ERROR_INVALID_ARGUMENT
			AUTHCHECK_ERROR_INVALID_PASSPORT
*/
AUTHCHECK_API
AuthCheckError AuthCheck_CheckGuestPassportA
( 
	IN	const char *	szPassport,					// 1024
	OUT	INT32 *			pnGameCode		= NULL,
	OUT	INT64 *			pnGuestSN		= NULL,
	OUT char *			pszGuestId		= NULL,		// 32
	OUT char *			pszLoginIp		= NULL,		// 32
	OUT SYSTEMTIME *	pTimeCreated	= NULL,
	OUT SYSTEMTIME *	pTimeToExpire	= NULL
);

/*
returns :	AUTHCHECK_ERROR_OK
			AUTHCHECK_ERROR_INVALID_ARGUMENT
			AUTHCHECK_ERROR_INVALID_PASSPORT
*/
AUTHCHECK_API
AuthCheckError AuthCheck_CheckGuestPassportW
( 
	IN	const WCHAR *	szPassport,					// 1024
	OUT	INT32 *			pnGameCode		= NULL,
	OUT	INT64 *			pnGuestSN		= NULL,
	OUT WCHAR *			pszGuestId		= NULL,		// 32
	OUT WCHAR *			pszLoginIp		= NULL,		// 32
	OUT SYSTEMTIME *	pTimeCreated	= NULL,
	OUT SYSTEMTIME *	pTimeToExpire	= NULL
);

AUTHCHECK_API
UINT32 AuthCheck_LoginA
( 
 IN	const char*		szID,						// 32
 IN	const char*		szPwd,						// 32
 IN	const char*		szIP,						// 32
 IN	UINT32			gameCode,
 IN	UINT32			locale,
 OUT char*			pszPassport			= NULL,	// 1024
 OUT UINT32*			pnUnreadNoteCount	= NULL,
 OUT UINT32*			pnStatusFlag		= NULL,
 OUT UINT32*			pnUpdateInterval	= NULL
 );

AUTHCHECK_API
UINT32 AuthCheck_LoginW
( 
 IN	const WCHAR*	szID,						// 32
 IN	const WCHAR*	szPwd,						// 32
 IN	const WCHAR*	szIP,						// 32
 IN	UINT32			gameCode,
 IN	UINT32			locale,
 OUT WCHAR*			pszPassport			= NULL,	// 1024
 OUT UINT32*			pnUnreadNoteCount	= NULL,
 OUT UINT32*			pnStatusFlag		= NULL,
 OUT UINT32*			pnUpdateInterval	= NULL
 );

AUTHCHECK_API
UINT32 AuthCheck_LogoutA
( 
 IN	const char*		szPassport,		// 1024
 IN	const char*		szIP			// 32
 );

AUTHCHECK_API
UINT32 AuthCheck_LogoutW
( 
 IN	const WCHAR*	szPassport,		// 1024
 IN	const WCHAR*	szIP			// 32
 );

#if defined( UNICODE ) || defined( _UNICODE )
	#define AuthCheck_SetSoapHost			AuthCheck_SetSoapHostW
	#define AuthCheck_GetPassportType		AuthCheck_GetPassportTypeW
	#define AuthCheck_LoadKey				AuthCheck_LoadKeyW
	#define AuthCheck_LocalCheckPassport	AuthCheck_LocalCheckPassportW
	#define AuthCheck_LocalCheckPassport2	AuthCheck_LocalCheckPassport2W
	#define AuthCheck_CheckPassport			AuthCheck_CheckPassportW
	#define AuthCheck_GetHashValue			AuthCheck_GetHashValueW
	#define AuthCheck_CheckGuestPassport	AuthCheck_CheckGuestPassportW
	#define AuthCheck_Login					AuthCheck_LoginW
	#define AuthCheck_Logout				AuthCheck_LogoutW
#else
	#define AuthCheck_SetSoapHost			AuthCheck_SetSoapHostA
	#define AuthCheck_GetPassportType		AuthCheck_GetPassportTypeA
	#define AuthCheck_LoadKey				AuthCheck_LoadKeyA
	#define AuthCheck_LocalCheckPassport	AuthCheck_LocalCheckPassportA
	#define AuthCheck_LocalCheckPassport2	AuthCheck_LocalCheckPassport2A
	#define AuthCheck_CheckPassport			AuthCheck_CheckPassportA
	#define AuthCheck_GetHashValue			AuthCheck_GetHashValueA
	#define AuthCheck_CheckGuestPassport	AuthCheck_CheckGuestPassportA
	#define AuthCheck_Login					AuthCheck_LoginA
	#define AuthCheck_Logout				AuthCheck_LogoutA
#endif
