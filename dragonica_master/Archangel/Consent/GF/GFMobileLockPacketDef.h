#ifndef CONSENT_CONSENTSERVER_LINKAGE_GF_MOBILELOCK_GFMOBILELOCKPACKETDEF_H
#define CONSENT_CONSENTSERVER_LINKAGE_GF_MOBILELOCK_GFMOBILELOCKPACKETDEF_H

// 대만에서 정한 패킷 타입 이니까 여기에 정의한다.
// Weapon에 영향받지 않도록..
PACKET_DEF( PT_AP_GF_REQ_MOBILE_AUTH,	0x0E01 )
PACKET_DEF( PT_GF_AP_ANS_MOBILE_AUTH,	0xD601 )

PACKET_DEF( PT_AP_GF_REQ_PING,			0x0E02 )


#pragma pack(1)

int const MAX_STRING_LEN_MOBILELOCK = 20;

typedef struct tagPT_AP_GF_REQ_MOBILE_AUTH
{
	explicit tagPT_AP_GF_REQ_MOBILE_AUTH( SYSTEMTIME const * const pkSystemTime=NULL, std::wstring const * const pwstrID=NULL, BM::GUID const &_kSessionID = BM::GUID::NullData() )
		:	kSessionID(_kSessionID)
	{
		if ( pwstrID )
		{
			::swprintf_s( wszAccount, MAX_STRING_LEN_MOBILELOCK, pwstrID->c_str() );
		}
		else
		{
			::memset( wszAccount, 0, sizeof(wszAccount) );
		}
		
		SetLoginTime( pkSystemTime );
	}

	explicit tagPT_AP_GF_REQ_MOBILE_AUTH( SYSTEMTIME const * const pkSystemTime, wchar_t *wszID, BM::GUID const &_kSessionID = BM::GUID::NullData() )
		:	kSessionID(_kSessionID)
	{
		if ( wszID )
		{
			::swprintf_s( wszAccount, MAX_STRING_LEN_MOBILELOCK, wszID );
		}
		else
		{
			::memset( wszAccount, 0, sizeof(wszAccount) );
		}

		SetLoginTime( pkSystemTime );
	}

	void SetLoginTime( SYSTEMTIME const * const pkSystemTime )
	{
		if ( pkSystemTime )
		{
			::swprintf_s( wszLoginTime, _T("%04u/%02u/%02u %02u:%02u:%02u"), pkSystemTime->wYear, pkSystemTime->wMonth, pkSystemTime->wDay, pkSystemTime->wHour, pkSystemTime->wMinute, pkSystemTime->wSecond );
		}
		else
		{
			::memset( wszLoginTime, 0, sizeof(wszLoginTime) );
		}
	}

	wchar_t	wszAccount[MAX_STRING_LEN_MOBILELOCK];
	wchar_t wszLoginTime[MAX_STRING_LEN_MOBILELOCK];
	BM::GUID kSessionID;

	DEFAULT_TBL_PACKET_FUNC();
}S_PT_AP_GF_REQ_MOBILE_AUTH;

typedef struct tagPT_GF_AP_ANS_MOBILE_AUTH
{
	tagPT_GF_AP_ANS_MOBILE_AUTH()
		:	byRet(0)
	{
		::memset( wszAccount, 0, sizeof(wszAccount) );
	}

	wchar_t		wszAccount[MAX_STRING_LEN_MOBILELOCK];
	BYTE		byRet;
	BM::GUID	kSessionID;	

	DEFAULT_TBL_PACKET_FUNC();
}S_PT_GF_AP_ANS_MOBILE_AUTH;

#pragma pack()

#endif // CONSENT_CONSENTSERVER_LINKAGE_GF_MOBILELOCK_GFMOBILELOCKPACKETDEF_H