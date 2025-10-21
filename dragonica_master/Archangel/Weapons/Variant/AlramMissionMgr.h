#ifndef WEAPON_VARIANT_ALRAMMISSION_ALRAMMISSIONMGR_H
#define WEAPON_VARIANT_ALRAMMISSION_ALRAMMISSIONMGR_H

#include "Lohengrin/dbtables3.h"
#include "Variant/AlramMission.h"

class PgAlramMissionMgr
{
	typedef std::vector< int >							VEC_ALRAM_ID;
	typedef std::map< int, VEC_ALRAM_ID >				CONT_ALRAM_ID_BYCLASS;
	typedef std::vector< TBL_DEF_ALRAM_MISSION >		VEC_ALRAM_MISSION;
	typedef std::map< int, TBL_DEF_ALRAM_MISSION * >	CONT_ALRAM_MISSION;

public:
	PgAlramMissionMgr();
	~PgAlramMissionMgr();

	static ALRAM_MISSION::E_ALRAMTYPE GetAlramType( T_GNDATTR const kGndAttr );

	HRESULT Init( ALRAM_MISSION::E_ALRAMTYPE const kType, CONT_DEF_ALRAM_MISSION const &kContDefAlramMission );
	HRESULT GetNewActionByID( int const iID, PgAlramMission &rkOutMission )const;
	HRESULT GetNewAction( int const iClass, PgAlramMission &rkOutMission )const;
	HRESULT GetNextAction( PgAlramMission &rkOutMission )const;

protected:
	TBL_DEF_ALRAM_MISSION const * GetDef( int const iID )const;

private:
	ALRAM_MISSION::E_ALRAMTYPE	m_kAlramType;
	VEC_ALRAM_ID				m_kVecAlramID;
	CONT_ALRAM_ID_BYCLASS		m_kContAlramIDByClass;

	CONT_ALRAM_MISSION			m_kContAlramMission;
	VEC_ALRAM_MISSION			m_kVecAlramMission;
};

class PgAlramMissionMgr_Warpper
	:	public TWrapper< PgAlramMissionMgr >
{
public:
	PgAlramMissionMgr_Warpper();
	~PgAlramMissionMgr_Warpper();

	HRESULT Init( ALRAM_MISSION::E_ALRAMTYPE const kType, CONT_DEF_ALRAM_MISSION const &kContDefAlramMission );
	HRESULT GetNewActionByID( int const iID, PgAlramMission &rkOutMission )const;
	HRESULT GetNewAction( int const iClass, PgAlramMission &rkOutMission )const;
	HRESULT GetNextAction( PgAlramMission &rkOutMission )const;
};

#endif // WEAPON_VARIANT_ALRAMMISSION_ALRAMMISSIONMGR_H