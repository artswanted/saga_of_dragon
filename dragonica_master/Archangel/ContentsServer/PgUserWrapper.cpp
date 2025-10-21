#include "stdafx.h"
#include "PgUserWrapper.h"
#include "PgServerSetMgr.h"

// 아래 함수 구현은 다른 채널의 Member에 대해서 작동 하지 않는다.
//HRESULT GetPlayerByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut, bool const bFindOtherChannel)
HRESULT GetPlayerByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut)
{
	return g_kRealmUserMgr.Locked_GetPlayerInfo(rkGuid, bIsMemberGuid, rkOut);
}

//HRESULT GetPlayerByName(std::wstring const &rkName, SContentsUser &rkOut, bool const bFindOtherChannel)
HRESULT GetPlayerByName(std::wstring const &rkName, SContentsUser &rkOut)
{
	return g_kRealmUserMgr.Locked_GetPlayerInfo(rkName, rkOut);
}

//HRESULT WritePlayerInfoToPacket_ByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, BM::Stream &rkPacket, bool const bFindOtherChannel)
HRESULT WritePlayerInfoToPacket_ByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, BM::Stream &rkPacket)
{
	SContentsUser kUserInfo;
	if( S_OK == ::GetPlayerByGuid(rkGuid, bIsMemberGuid, kUserInfo) )
	{
		kUserInfo.WriteToPacket(rkPacket);
		return S_OK;
	}
	kUserInfo.WriteToPacket(rkPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

//HRESULT WritePlayerInfoToPacket_ByName(std::wstring const &rkName, BM::Stream &rkPacket, bool const bFindOtherChannel)
HRESULT WritePlayerInfoToPacket_ByName(std::wstring const &rkName, BM::Stream &rkPacket)
{
	SContentsUser kUserInfo;
	if( S_OK == ::GetPlayerByName(rkName, kUserInfo) )
	{
		kUserInfo.WriteToPacket(rkPacket);
		return S_OK;
	}
	kUserInfo.WriteToPacket(rkPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

//HRESULT GetCharacterNameByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, std::wstring &rkOut, bool const bFindOtherChannel)
HRESULT GetCharacterNameByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, std::wstring &rkOut)
{
	SContentsUser kUserInfo;
	if( S_OK == ::GetPlayerByGuid(rkGuid, bIsMemberGuid, kUserInfo) )
	{
		rkOut = kUserInfo.kName;
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

//HRESULT GetCharGuidToMembGuid(VEC_GUID const &rkIn, VEC_GUID &rkOut, bool const bFindOtherChannel)
HRESULT GetCharGuidToMembGuid(VEC_GUID const &rkIn, VEC_GUID &rkOut)
{
	size_t iSuccessCount = 0;
	rkOut.clear();
	VEC_GUID::const_iterator iter = rkIn.begin();
	while(rkIn.end() != iter)
	{
		SContentsUser kPlayerInfo;
		if( S_OK == ::GetPlayerByGuid((*iter), false, kPlayerInfo) )
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
	if(0 != iSuccessCount)
	{
		return S_OK;
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
}

