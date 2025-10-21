#include "stdafx.h"
#include "LogUtil.h"


namespace DR2LogUtil
{
	int iGameID = 0; // NC 전용

	//
	ELogValue_Type GetDR2ColumnType(int const iColumnIndex)
	{
		switch( iColumnIndex )
		{
		case EColumn_MemberKey:
		case EColumn_CharacterKey:
			{
				return ELogValue_STRING; // 원본은 Guid(string 화)
			}break;
		case EColumn_RealmNo:
		case EColumn_ChannelNo:
		case EColumn_Job:
		case EColumn_Level:
			{
				return ELogValue_INT; // 원본은 Short
			}break;
		case EColumn_UID:
		case EColumn_GroundNo:
		case EColumn_iValue1:
		case EColumn_iValue2:
		case EColumn_iValue3:
		case EColumn_iValue4:
		case EColumn_iValue5:
			{
				return ELogValue_INT;
			}break;
		case EColumn_ID:
		case EColumn_Name:
		case EColumn_Message1:
		case EColumn_Message2:
		case EColumn_Message3:
		case EColumn_Message4:
			{
				return ELogValue_STRING;
			}break;
		case EColumn_i64Value1:
		case EColumn_i64Value2:
		case EColumn_i64Value3:
		case EColumn_i64Value4:
		case EColumn_i64Value5:
			{
				return ELogValue_I64;
			}break;
		case EColumn_None:
			{
				return ELogValue_None;
			}break;
		// 특수 변수들
		case EColumn_iUnitGender:
		case EColumn_iGameID:
			{
				return ELogValue_INT;
			}break;
		//
		default:
			{
			}break;
		}
		return ELogValue_None;
	}
	bool GetColumnValue(PgLogCont const& rkLogCont, PgLog const &rkLog, int const iColumnIndex, int &rkOut)
	{
		switch (iColumnIndex)
		{
			// short
		case EColumn_RealmNo:
			{
				rkOut = rkLogCont.RealmNo();
			}break;
		case EColumn_ChannelNo:
			{
				rkOut = rkLogCont.ChannelNo();
			}break;
		case EColumn_Job:
			{
				rkOut = rkLogCont.Class();
			}break;
		case EColumn_Level:
			{
				rkOut = rkLogCont.Level();
			}break;
			//
		case EColumn_UID:
			{
				rkOut = rkLogCont.UID();
			}break;
		case EColumn_GroundNo:
			{
				rkOut = rkLogCont.GroundNo();
			}break;
		case EColumn_iValue1:
		case EColumn_iValue2:
		case EColumn_iValue3:
		case EColumn_iValue4:
		case EColumn_iValue5:
			{
				rkOut = rkLog.m_kContInt.at(iColumnIndex-EColumn_iValue1);
			}break;
		case EColumn_iUnitGender:
			{
				rkOut = rkLogCont.UnitGender();
			}break;
		case EColumn_iGameID:
			{
				rkOut = iGameID;
			}break;
		default:
			{
				return false;
			}break;
		}
		return true;
	}
	bool GetColumnValue(PgLogCont const& rkLogCont, PgLog const &rkLog, int const iColumnIndex, __int64 &rkOut )
	{
		switch (iColumnIndex)
		{
		case EColumn_i64Value1:
		case EColumn_i64Value2:
		case EColumn_i64Value3:
		case EColumn_i64Value4:
		case EColumn_i64Value5:
			{
				rkOut = rkLog.m_kContInt64.at(iColumnIndex-EColumn_i64Value1);
			}break;
		default:
			{
				return false;
			}break;
		}
		return true;
	}
	bool GetColumnValue(PgLogCont const& rkLogCont, PgLog const &rkLog, int const iColumnIndex, std::wstring& rkOut )
	{
		switch (iColumnIndex)
		{
		case EColumn_ID:
			{
				rkOut = rkLogCont.ID();
			}break;
		case EColumn_Name:
			{
				rkOut = rkLogCont.Name();
			}break;
		case EColumn_Message1:
		case EColumn_Message2:
		case EColumn_Message3:
		case EColumn_Message4:
			{
				rkLog.Get(static_cast<size_t>(iColumnIndex - EColumn_Message1),  rkOut);
			}break;
		default:
			{
				return false;
			}break;
		}
		return true;
	}
};