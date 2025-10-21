#ifndef WEAPON_LOHENGRIN_DB_DB_H
#define WEAPON_LOHENGRIN_DB_DB_H

typedef std::list<CEL::INIT_DB_DESC> CONT_DB_INIT_DESC;

typedef enum eDBType
{
	DT_SITE_CONFIG		= 1,
	DT_DEF				= 2,//고정형 DB
	DT_MEMBER			= 4,//멤버디비
	DT_PLAYER			= 5,//플레이어 정보 디비
	DT_LOG				= 9,//로그디비
	DT_GM				= 10,//지엠 디비
	DT_LOCAL			= 11,//로컬 디비
}EDBType;

typedef enum
{
	EConfigType_None = 0,
	EConfigType_Ini = 1,
	EConfigType_Inb = 2
} EDBConfig_Type;

extern bool ReadDBConfig(EDBConfig_Type const eConfigType, std::wstring const &rkPatch, SERVER_IDENTITY const &rkSI, CONT_DB_INIT_DESC &rkContOut);

#endif // WEAPON_LOHENGRIN_DB_DB_H