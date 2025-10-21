#include "stdafx.h"
#include <string.h>
#include "Tinyxml/Tinyxml.h"
#include "BM/LocalMgr.h"
#include "PacketStruct.h"
#include "Variant/PgStringUtil.h"

tagClientTryLogin::tagClientTryLogin()
{
	Clear();
}

void tagClientTryLogin::Clear()
{
	m_kID = m_kPW = L"";
	::memset( szVersion1, 0, sizeof(szVersion1) );
	::memset( szVersion2, 0, sizeof(szVersion2) );

	RealmNo(0);
	ChannelNo(0);
	PatchVersion(0);
	m_kConnArea.Clear();
}

void tagClientTryLogin::MakeCorrect()
{
	szVersion1[_countof(szVersion1)-1] = _T('\0');
	szVersion2[_countof(szVersion2)-1] = _T('\0');

	std::wstring wstrID = ID();
	PgStringUtil::TrimAll<std::wstring>( wstrID, L" ", wstrID );
	SetID( wstrID );

	if (ID().length() > GetIDMaxLength())
	{
		SetID(L"too_long");
	}
	if (PW().length() > GetPWMaxLength())
	{
		SetPW(L"too_long");
	}
}

bool tagClientTryLogin::SetID(std::wstring const &wstrInID)
{
	ID(wstrInID);

	switch( g_kLocal.ServiceRegion() )
	{
	case LOCAL_MGR::NC_JAPAN:
		{
			LWR(m_kID);
		}break;
	default:
		{
			UPR(m_kID);
		}break;
	}

	return true;
}

bool tagClientTryLogin::SetPW(std::wstring const &wstrInPW)
{
	PW(wstrInPW);
	return true;
}
/*
bool tagClientTryLogin::SetName(std::wstring const &wstrName)
{
	SAFE_STRNCPY( szName, wstrName.c_str() );
	return true;
}
*/

bool tagClientTryLogin::SetVersion( TCHAR const* pszVersion1, TCHAR const* pszVersion2 )
{
	if ( pszVersion1 )
	{
		SAFE_STRNCPY( szVersion1, pszVersion1);
	}
	else
	{
		szVersion1[0] = _T('\0');
	}
	
	if ( pszVersion2 )
	{
		SAFE_STRNCPY( szVersion2, pszVersion2 );
	}
	else
	{
		szVersion2[0] = _T('\0');
	}
	return true;
}

tagClientTryLogin::tagClientTryLogin(const tagClientTryLogin& rhs)
:	m_kConnArea(rhs.m_kConnArea)
{
//	::memset( szName, 0, sizeof(szName) );
	SetVersion( rhs.szVersion1, rhs.szVersion2 );
	ID(rhs.ID());
	PW(rhs.PW());
	ChannelNo(rhs.ChannelNo());
	RealmNo(rhs.RealmNo());
	PatchVersion(rhs.PatchVersion()); 
}

BM::vstring tagClientTryLogin::ToString() const
{
	return BM::vstring(L"SClientTryLogin[ID:")<<ID()<<L","<<C2L(m_kConnArea)<<L",Ver1:"<<szVersion1<<L",Ver2:"<<szVersion2<<L",R:"<<RealmNo()<<L",C:"<<ChannelNo()<<L",PatchVer:"<<PatchVersion()<<L"]";
}

bool tagClientTryLogin::ReadFromPacket(BM::Stream& kPacket)
{
	kPacket.Pop(m_kID);
	kPacket.Pop(m_kPW);
	m_kConnArea.ReadFromPacket(kPacket);
	kPacket.PopMemory(szVersion1, sizeof(szVersion1));
	kPacket.PopMemory(szVersion2, sizeof(szVersion2));
	kPacket.Pop(m_kRealmNo);
	kPacket.Pop(m_kChannelNo);
	
	return kPacket.Pop(m_kPatchVersion);
}

void tagClientTryLogin::WriteToPacket(BM::Stream& kPacket) const
{
	kPacket.Push(ID());
	kPacket.Push(PW());
	m_kConnArea.WriteToPacket(kPacket);
	kPacket.Push(szVersion1, sizeof(szVersion1));
	kPacket.Push(szVersion2, sizeof(szVersion2));
	kPacket.Push(RealmNo());
	kPacket.Push(ChannelNo());
	kPacket.Push(PatchVersion());
}

int tagClientTryLogin::GetIDMaxLength()
{
	if (g_kLocal.ServiceRegion() == LOCAL_MGR::NC_VIETNAM)
	{
		return 50;
	}
	return 20;
}

int tagClientTryLogin::GetPWMaxLength()
{
	if (g_kLocal.ServiceRegion() == LOCAL_MGR::NC_VIETNAM)
	{
		return 64;
	}
	return 20;
}

size_t tagClientTryLogin::min_size() const
{
	return sizeof(size_t) * 2	// ID(), PW()
		+ m_kConnArea.min_size()
		+ sizeof(TCHAR) * MAX_PATCH_VERSION * 2
		+ sizeof(short) * 2
		+ sizeof(__int64);
}


///
//BM::vstring& operator <<(BM::vstring& lhs, CEL::SESSION_KEY const& rhs)
//{
//	lhs += BM::vstring(L" SESSION_KEY[WG:")<<rhs.WorkerGuid()<<L",SG:"<<rhs.SessionGuid()<<L"]";
//	return lhs;
//}
//BM::vstring& operator <<(BM::vstring& lhs, SConnectionArea const& rhs)
//{
//	lhs += BM::vstring(L" SConnectionArea[A:")<<rhs.nArea<<L",CT:"<<rhs.nCustomType<<L"]"
//	return lhs;
//}
//bm::vstring& operator <<(BM::vstring& lhs, SClientTryLogin const& rhs)
//{
//	lhs += bm::vstring(l" sclienttrylogin[id:")<<rhs.szid<<l",ver1:"<<rhs.szversion1<<l",ver2:"<<rhs.szversion2<<rhs.m_kconnarea<<l",r:"<<rhs.realmno()<<l",c:"<<rhs.channelno()<<l",pver:"<<rhs.patchversion()<<l"]"
//	return lhs;
//}


char const* szWORLD_ENVIRONMENT_STATUS_ELEMENT_NAME = "WORLD_ENV_STATUS";
//
tagWorldEnvironmentStatus::tagWorldEnvironmentStatus()
	: eType(WEST_NONE), iStartGameTime(0), iDurationSec(0)
{
}
tagWorldEnvironmentStatus::tagWorldEnvironmentStatus(EWorldEnvironmentStatusType const& reType, __int64 const& riStartGameTime, __int64 const& riDurationSec)
	: eType(reType), iStartGameTime(riStartGameTime), iDurationSec(riDurationSec)
{
}
tagWorldEnvironmentStatus::tagWorldEnvironmentStatus(tagWorldEnvironmentStatus const& rhs)
	: eType(rhs.eType), iStartGameTime(rhs.iStartGameTime), iDurationSec(rhs.iDurationSec)
{
}
tagWorldEnvironmentStatus::~tagWorldEnvironmentStatus()
{
}

bool tagWorldEnvironmentStatus::ParseXml(TiXmlElement const* pkRootNode)
{
	if( !pkRootNode )
	{
		return false;
	}
	if( 0 != strcmp(szWORLD_ENVIRONMENT_STATUS_ELEMENT_NAME, pkRootNode->Value()) )
	{
		return false;
	}
	char const* szDurationSec = pkRootNode->Attribute("DURATION_SEC");
	char const* szType = pkRootNode->GetText();
	if( !szDurationSec
	||	!szType )
	{
		return false;
	}
	if( 0 == strcmp("SNOW", szType) )
	{
		eType = WEST_SNOW;
	}
	else if( 0 == strcmp("SAKURA", szType) )
	{
		eType = WEST_SAKURA;
	}
	else if( 0 == strcmp("SUNFLOWER", szType) )
	{
		eType = WEST_SUNFLOWER;
	}
	else if( 0 == strcmp("ICECREAM", szType) )
	{
		eType = WEST_ICECREAM;
	}
	else if( 0 == strcmp("ETC3", szType) )
	{
		eType = WEST_ETC3;
	}
	else if( 0 == strcmp("ETC4", szType) )
	{
		eType = WEST_ETC4;
	}
	else
	{
		return false;
	}
	iDurationSec = static_cast< __int64 >(BM::vstring(szDurationSec));
	if( 0 >= iDurationSec )
	{
		return false;
	}
	return true;
}
bool tagWorldEnvironmentStatus::IsCanTime(__int64 const iCurGameTime) const
{
	return (iStartGameTime + iDurationSec) >= iCurGameTime;
}
bool tagWorldEnvironmentStatus::ReadFromPacket(BM::Stream& rkPacket)
{
	if( rkPacket.Pop( eType )
	&&	rkPacket.Pop( iStartGameTime )
	&&	rkPacket.Pop( iDurationSec ) )
	{
		return true;
	}
	return false;
}
void tagWorldEnvironmentStatus::WriteToPacket(BM::Stream& rkPacket)
{
	rkPacket.Push( eType );
	rkPacket.Push( iStartGameTime );
	rkPacket.Push( iDurationSec );
}

//
tagWorldEnvironmentStatusBitFlag::tagWorldEnvironmentStatusBitFlag()
	: dwEnvStatus(0)
{
}
tagWorldEnvironmentStatusBitFlag::tagWorldEnvironmentStatusBitFlag(DWORD const& rdwEnvStatus)
	: dwEnvStatus(rdwEnvStatus)
{
}
tagWorldEnvironmentStatusBitFlag::tagWorldEnvironmentStatusBitFlag(tagWorldEnvironmentStatusBitFlag const& rhs)
	: dwEnvStatus(rhs.dwEnvStatus)
{
}
tagWorldEnvironmentStatusBitFlag::tagWorldEnvironmentStatusBitFlag(CONT_WORLD_ENVIRONMENT_STATUS const& rkCont)
	: dwEnvStatus(0)
{
	Update(rkCont);
}
void tagWorldEnvironmentStatusBitFlag::Set(EWorldEnvironmentStatusType const eType)
{
	dwEnvStatus |= (1 << eType);
}
bool tagWorldEnvironmentStatusBitFlag::Is(EWorldEnvironmentStatusType const eType) const
{
	return Is(dwEnvStatus, eType);
}
void tagWorldEnvironmentStatusBitFlag::Compare(DWORD const& dwOldEnvStatus, CONT_WORLD_ENVIRONMENT_STATUS_TYPE& rkContAdd, CONT_WORLD_ENVIRONMENT_STATUS_TYPE& rkContDel) const
{
	Compare(dwOldEnvStatus, dwEnvStatus, rkContAdd, rkContDel);
}
void tagWorldEnvironmentStatusBitFlag::Update(CONT_WORLD_ENVIRONMENT_STATUS const& rkCont)
{
	dwEnvStatus = 0;
	CONT_WORLD_ENVIRONMENT_STATUS::const_iterator iter = rkCont.begin();
	while( rkCont.end() != iter )
	{
		Set((*iter).first);
		++iter;
	}
}
bool tagWorldEnvironmentStatusBitFlag::Is(DWORD const dwEnvStatus, EWorldEnvironmentStatusType const eType)
{
	return 0 != (dwEnvStatus & (1 << eType));
}
void tagWorldEnvironmentStatusBitFlag::Compare(DWORD const dwOldEnvStatus, DWORD const dwNewEnvStatus, CONT_WORLD_ENVIRONMENT_STATUS_TYPE& rkContAdd, CONT_WORLD_ENVIRONMENT_STATUS_TYPE& rkContDel)
{
	size_t iBegin = WEST_NONE;
	if( WEST_MAX != iBegin )
	{
		EWorldEnvironmentStatusType const eType = static_cast< EWorldEnvironmentStatusType >(iBegin);
		bool const bNew = Is(dwNewEnvStatus, eType);
		bool const bOld = Is(dwOldEnvStatus, eType);
		if( false == bOld && true == bNew )
		{
			rkContAdd.insert(eType);
		}
		if( true == bOld && false == bNew )
		{
			rkContDel.insert(eType);
		}
		++iBegin;
	}
}