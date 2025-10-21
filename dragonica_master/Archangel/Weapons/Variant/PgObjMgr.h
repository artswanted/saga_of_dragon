#ifndef WEAPON_VARIANT_BASIC_PGOBJMGR_H
#define WEAPON_VARIANT_BASIC_PGOBJMGR_H

template< typename T_ID, typename T_OBJ >
class PgObjMgr
{
public:
	typedef std::map< typename T_ID, typename T_OBJ* >	CONT_OBJ;

public:
	PgObjMgr(void);
	~PgObjMgr(void);

	template< typename T >
	bool New( T_ID const &kID, T ** pOutObj = NULL );
	bool Delete( T_ID const &kID );
	T_OBJ* Get( T_ID const &kID )const;

	template< typename FN >
	void ForEach( FN &kFunction );

private:
	CONT_OBJ		m_kContObj;
};

#include "PgObjMgr.hpp"

#endif // WEAPON_VARIANT_BASIC_PGOBJMGR_H