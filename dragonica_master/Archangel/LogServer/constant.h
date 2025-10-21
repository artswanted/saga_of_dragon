//!  stdafx.h : 자주 사용하지만 자주 변경되지는 않는
//!  표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
//!  들어 있는 포함 파일입니다.
//! 

#ifndef LOG_LOGSERVER_CONSTANT_H
#define LOG_LOGSERVER_CONSTANT_H

typedef enum eDBQueryType
{
	DQT_WRITE_LOG = 2,
	DQT_DEF_WRITEMODE,
	DQT_CHECK_LOG_TABLE,
	DQT_LOAD_DEF_CONVERT2NC,
	DQT_CHECK_LOG_TABLE_NEXTDAY,

	// NCOnly
	DQT_LINKTEST_NCLOGD_CHECK_LOG_TABLE,
	DQT_LINKTEST_NCLOGD_WRITE_LOG,
};

#endif // LOG_LOGSERVER_CONSTANT_H