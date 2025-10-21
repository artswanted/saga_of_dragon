#ifndef WEAPON_LOHENGRIN_REALM_PGRESTRICT_H
#define WEAPON_LOHENGRIN_REALM_PGRESTRICT_H

#include "Lohengrin/dbtables.h"

typedef std::vector<int>				CONT_RESTRICT_OPTION;

#pragma pack (1)
typedef struct tagRestrictionsOnChannel
{
	tagRestrictionsOnChannel()
		:	nRealmNo(0)
		,	nChannelNo(0)
		,	iMaxUserCount(0)
	{}

	short					nRealmNo;
	short					nChannelNo;
	int						iMaxUserCount;
	CONT_RESTRICT_OPTION	kContOptions;
}SRestrictOnChannel;

typedef enum
{
	// 번호 함부로 바꾸면 안된다.
	RESTRICT_NONE			= 0,
	RESTRICT_GENDER			= 1,	// 성별 제한
	RESTRICT_AGE			= 2,	// 나이제한(만)
	RESTRICT_AGEKOREAN		= 3,	// 한국식 나이
	RESTRICT_IP				= 4,	// IP 제한
}ERESTRICT_TYPE;

typedef struct tagRestrictChecker
{
	tagRestrictChecker( short const _kType=RESTRICT_NONE )
		:	kType(_kType)
		,	iErrorMsg(0)
		,	iValue1(0)
		,	iValue2(0)
	{}

	short			kType;//ERESTRICT_TYPE 사용
	int				iErrorMsg;
	int				iValue1;
	int				iValue2;
	
	DEFAULT_TBL_PACKET_FUNC();
}SRestrictChecker;
#pragma pack ()

typedef std::map<int,SRestrictChecker>	CONT_DEF_RESTRICTIONS;


class PgRestrict
{
	typedef std::vector<SRestrictChecker>	ContChecker;
public:
	PgRestrict();
	virtual ~PgRestrict();

public:
	CLASS_DECLARATION_S(int, MaxUser);
	CLASS_DECLARATION_S(int, CurrentUser);

	bool Set( CONT_DEF_RESTRICTIONS const &kRestrictDef, SRestrictOnChannel const &kOptions );
	int IsAccess( SAuthInfo const &kAuthInfo )const;

protected:
	bool IsAccess( SRestrictChecker const &kChecker, SAuthInfo const &kAuthInfo )const;

	void WriteToPacket( BM::Stream &kPacket )const;
	void ReadFromPacket( BM::Stream &kPacket );

private:
	ContChecker		m_kContCheck;

};

#endif // WEAPON_LOHENGRIN_REALM_PGRESTRICT_H