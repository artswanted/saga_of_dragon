#ifndef WEAPON_VARIANT_USERINFO_LIMITCLASS_H
#define WEAPON_VARIANT_USERINFO_LIMITCLASS_H

#include "Variant/constant.h"

class PgLimitClass
{
public:
	PgLimitClass();
	explicit PgLimitClass( int const iClass, short const nLevelLimit );
	explicit PgLimitClass( __int64 const i64ClassLimit, short const nLevelLimit );
	~PgLimitClass();

	PgLimitClass operator + ( PgLimitClass const &rhs )const;
	PgLimitClass operator - ( PgLimitClass const &rhs )const;
	PgLimitClass& operator +=( PgLimitClass const &rhs );
	PgLimitClass& operator -=( PgLimitClass const &rhs );

	void SetClass( __int64 const i64ClassFlag ){m_i64ClassLimit=i64ClassFlag;}
	void SetLevel( short const nLevel ){m_nLevelLimit=nLevel;}

	__int64 GetClass()const{return m_i64ClassLimit;}
	short GetLevel()const{return m_nLevelLimit;}

	bool IsUsing(void)const{return 0i64!=m_i64ClassLimit && 0<m_nLevelLimit;}
	bool IsAccess( PgLimitClass const &kLimit )const;
	bool IsAccessClass( int const iClass )const{return IS_CLASS_LIMIT(m_i64ClassLimit, iClass);}
	bool IsAccessClassLimit( __int64 const iClassLimit )const{return IS_CLASS_LIMIT2(m_i64ClassLimit, iClassLimit);}
	bool IsAccessLevel( short const nLevel )const{return 0<m_nLevelLimit && m_nLevelLimit<=nLevel;}

	DEFAULT_TBL_PACKET_FUNC();

private:
	__int64		m_i64ClassLimit;
	short		m_nLevelLimit;
};

#endif // WEAPON_VARIANT_USERINFO_LIMITCLASS_H