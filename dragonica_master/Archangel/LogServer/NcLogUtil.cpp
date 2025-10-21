#pragma once

#include "stdafx.h"
#include "Variant\PgParty.h"
#include "NcLogUtil.h"


//namespace NcLogUtil
//{
//
//int ConvertFunc(int iValue, E_NC_LOG_CONVERT_FUNC_TYPE eFuncType)
//{
//	switch (eFuncType)
//	{
//	case E_NC_LOG_FUNC_NONE:
//		{
//			return iValue;
//		}break;
//	default:
//		{
//			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Not implemented not yet") << static_cast<int>(eFuncType));
//		}break;
//	}
//	return iValue;
//}
//
//__int64 ConvertFunc(__int64 i64Value, E_NC_LOG_CONVERT_FUNC_TYPE eFuncType)
//{
//	switch (eFuncType)
//	{
//	case E_NC_LOG_FUNC_NONE:
//		{
//			return i64Value;
//		}break;
//	case E_NC_LOG_FUNC_LOGOUT_TYPE:
//		{
//			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Not implemented not yet") << static_cast<int>(eFuncType));
//		}break;
//	case E_NC_LOG_FUNC_SEC2MIN:
//		{
//			return std::max<__int64>(i64Value / 60, 1);
//		}break;
//	case E_NC_LOG_FUNC_INVEN_TYPE:
//		{
//			// 원래 int 값에서 상위 2byte 값이 Inventory type (EInvType)
//			return static_cast<__int64>(static_cast<int>(i64Value) & 0xFFFF0000);
//		}break;
//	case E_NC_LOG_FUNC_PARTY_OPEN_OPT:	// 파티 공개 여부 값
//		{
//			__int64 const i64Check = ( i64Value & static_cast<__int64>(POT_Public) );
//			return ( POT_Private == i64Check ) ? 1i64 : 0i64;	// 0:공개, 1:비공개
//		}break;
//	case E_NC_LOG_FUNC_PARTY_ITEM_OPT:
//		{
//			__int64 const i64Check = ( i64Value & static_cast<__int64>(POI_Order) );
//			return ( POI_Order == i64Check ) ? 2i64 : 1i64;// 1: 아무나, 2: 순차적으로 사이좋게
//		}break;
//	default:
//		{
//			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Not implemented not yet") << static_cast<int>(eFuncType));
//		}break;
//	}
//	return i64Value;
//}
//
//std::wstring const ConvertFunc(std::wstring const& wstrValue, E_NC_LOG_CONVERT_FUNC_TYPE eFuncType)
//{
//	switch (eFuncType)
//	{
//	case E_NC_LOG_FUNC_NONE:
//		{
//			return wstrValue;
//		}break;
//	case E_NC_LOG_FUNC_ITEM_ENCHANT1_STRING:
//		{
//			return std::wstring(L"Enchant1=") + wstrValue + std::wstring(L";");
//		}break;
//	case E_NC_LOG_FUNC_ITEM_ENCHANT2_STRING:
//		{
//			return std::wstring(L"Enchant2=") + wstrValue + std::wstring(L";");
//		}break;
//	case E_NC_LOG_FUNC_GROUNDNO_STRING:
//		{
//			return std::wstring(L"[") + wstrValue + std::wstring(_T("] "));
//		}break;
//	case E_NC_LOG_FUNC_GROUNDGUID_STRING:
//		{
//			return std::wstring(L"GUID=") + wstrValue + std::wstring(L" ");
//		}break;
//	default:
//		{
//			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Not implemented not yet") << static_cast<int>(eFuncType));
//		}break;
//	}
//	return wstrValue;
//}
//
//};
//
