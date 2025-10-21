#ifndef LOG_LOGSERVER_LOGUTIL_H
#define LOG_LOGSERVER_LOGUTIL_H

#include "Collins/Log.h"


namespace DR2LogUtil
{
	extern int iGameID;

	typedef enum
	{
		ELogValue_None = 0,
		ELogValue_INT,
		ELogValue_I64,
		ELogValue_STRING,
	} ELogValue_Type;

	//short const DR2_COLUMN_MAX_SIZE = 24;
	typedef enum
	{
		EColumn_None			= -1,
		EColumn_MemberKey		= 0,
		EColumn_UID				= 1,
		EColumn_CharacterKey	= 2,
		EColumn_ID				= 3,
		EColumn_Name			= 4,
		EColumn_RealmNo			= 5,
		EColumn_ChannelNo		= 6,
		EColumn_Job				= 7,
		EColumn_Level			= 8,
		EColumn_GroundNo		= 9,
		EColumn_Message1		= 10,
		EColumn_Message2		= 11,
		EColumn_Message3		= 12,
		EColumn_Message4		= 13,
		EColumn_iValue1			= 14,
		EColumn_iValue2			= 15,
		EColumn_iValue3			= 16,
		EColumn_iValue4			= 17,
		EColumn_iValue5			= 18,
		EColumn_i64Value1		= 19,
		EColumn_i64Value2		= 20,
		EColumn_i64Value3		= 21,
		EColumn_i64Value4		= 22,
		EColumn_i64Value5		= 23,
		// 특수 변수(DB에 저장되지는 않음)
		EColumn_iUnitGender		= 100, // 캐릭터 성별
		EColumn_iGameID			= 101, // Only NC(게임 아이디)
		//EColumn_Max				= DR2_COLUMN_MAX_SIZE
		EColumn_LogType			= 1000,
		EColumn_SubLogType		= 1001,
		EColumn_OrderType		= 1002,
		EColumn_ActionType		= 1003,
	} EDR2ColumnIndex;

	ELogValue_Type GetDR2ColumnType(int const iColumnIndex);

	bool GetColumnValue(PgLogCont const& rkLogCont, PgLog const &rkLog, int const iColumnIndex, int& );
	bool GetColumnValue(PgLogCont const& rkLogCont, PgLog const &rkLog, int const iColumnIndex, __int64&);
	bool GetColumnValue(PgLogCont const& rkLogCont, PgLog const &rkLog, int const iColumnIndex, std::wstring& );
};

#endif // LOG_LOGSERVER_LOGUTIL_H