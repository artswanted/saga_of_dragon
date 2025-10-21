#ifndef WEAPON_LOHENGRIN_UTILITY_PGPLAYLIMITER_H
#define WEAPON_LOHENGRIN_UTILITY_PGPLAYLIMITER_H

#include "dbtables.h"

struct PgPlayLimit_Sorter
{
	bool operator()( TBL_DEF_PLAYLIMIT_INFO const &lhs, TBL_DEF_PLAYLIMIT_INFO const &rhs )const
	{
		return lhs.i64EndTime <= rhs.i64BeginTime;
	}
};

class PgPlayLimit_Finder
{
public:
	explicit PgPlayLimit_Finder( CONT_DEF_PLAYLIMIT_INFO_LIST const &kLimitInfoList );
	~PgPlayLimit_Finder();

	HRESULT Find( __int64 const i64Time, CONT_DEF_PLAYLIMIT_INFO_LIST::value_type &rkOut )const;
	
protected:
	HRESULT Find( __int64 const i64Time, size_t left_index, size_t right_index, CONT_DEF_PLAYLIMIT_INFO_LIST::value_type &rkOut )const;

private:
	CONT_DEF_PLAYLIMIT_INFO_LIST const & m_rkLimitInfoList;

private:// Not Use
	PgPlayLimit_Finder();
	PgPlayLimit_Finder( PgPlayLimit_Finder const & );
	PgPlayLimit_Finder& operator = ( PgPlayLimit_Finder const & );

};

#endif // WEAPON_LOHENGRIN_UTILITY_PGPLAYLIMITER_H