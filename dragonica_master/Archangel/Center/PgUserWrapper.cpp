#include "stdafx.h"
#include "PgUserWrapper.h"

// ѕЖ·Ў ЗФјц ±ёЗцАє ґЩёҐ Г¤іОАЗ MemberїЎ ґлЗШј­ АЫµї ЗПБц ѕКґВґЩ.
HRESULT GetPlayerByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut, bool const bFindOtherChannel)
{
	if( bFindOtherChannel )
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Not implemented for FindOtherChannel option") );
		return E_FAIL;
	}
	return g_kServerSetMgr.Locked_GetPlayerInfo(rkGuid, bIsMemberGuid, rkOut);
}

HRESULT GetPlayerByName(std::wstring const &rkName, SContentsUser &rkOut, bool const bFindOtherChannel)
{
	if( bFindOtherChannel )
	{
		return E_FAIL;
//		return g_kServerSetMgr.Locked_GetPlayerInfo_OtherChannel(rkName, rkOut);
	}
	return g_kServerSetMgr.Locked_GetPlayerInfo(rkName, rkOut);
}

HRESULT WritePlayerInfoToPacket_ByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, BM::Stream &rkPacket, bool const bFindOtherChannel)
{
	SContentsUser kUserInfo;
	if( S_OK == ::GetPlayerByGuid(rkGuid, bIsMemberGuid, kUserInfo, bFindOtherChannel) )
	{
		kUserInfo.WriteToPacket(rkPacket);
		return S_OK;
	}
	kUserInfo.WriteToPacket(rkPacket);

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT WritePlayerInfoToPacket_ByName(std::wstring const &rkName, BM::Stream &rkPacket, bool const bFindOtherChannel)
{
	SContentsUser kUserInfo;
	if( S_OK == ::GetPlayerByName(rkName, kUserInfo, bFindOtherChannel) )
	{
		kUserInfo.WriteToPacket(rkPacket);
		return S_OK;
	}
	kUserInfo.WriteToPacket(rkPacket);

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT GetCharacterNameByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, std::wstring &rkOut, bool const bFindOtherChannel)
{
	SContentsUser kUserInfo;
	if( S_OK == ::GetPlayerByGuid(rkGuid, bIsMemberGuid, kUserInfo, bFindOtherChannel) )
	{
		rkOut = kUserInfo.kName;
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT GetCharGuidToMembGuid(VEC_GUID const &rkIn, VEC_GUID &rkOut, bool const bFindOtherChannel)
{
	size_t iSuccessCount = 0;
	rkOut.clear();
	VEC_GUID::const_iterator iter = rkIn.begin();
	while(rkIn.end() != iter)
	{
		SContentsUser kPlayerInfo;
		if( S_OK == ::GetPlayerByGuid((*iter), false, kPlayerInfo, bFindOtherChannel) )
		{
			std::back_inserter(rkOut) = kPlayerInfo.kMemGuid;
			++iSuccessCount;
		}
		else
		{
			std::back_inserter(rkOut) = BM::GUID::NullData();
		}
		++iter;
	}

	return (0 != iSuccessCount)? S_OK: E_FAIL;
}