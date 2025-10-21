#ifndef CONTENTS_CONTENTSSERVER_DOC_PGDOC_PETINFO_H
#define CONTENTS_CONTENTSSERVER_DOC_PGDOC_PETINFO_H

#include "Variant/Inventory.h"
#include "Variant/skill.h"
#include "PgDoc.h"

class PgDoc_PetInfo
	:	public PgDoc
{
public:
	PgDoc_PetInfo();
	virtual ~PgDoc_PetInfo();

	PgDoc_PetInfo( PgDoc_PetInfo const & );
	PgDoc_PetInfo& operator = ( PgDoc_PetInfo const & );

	virtual EDocType DocType(void)const{return TDOC_PET;}

	bool IsLoadedInven()const{return m_bLoadedInven;}

	PgInventory const * GetInven()const;
	PgInventory * GetInven();

	void SetInven( PgInventory const &rkInv );
	void RefreshSkillCoolTimeMap();
	void SetSkillCoolTimeMap( CSkill::MAP_COOLTIME const &kSkillCoolTimeMap ){m_kSkillCoolTimeMap = kSkillCoolTimeMap;}
	void GetSkillCoolTimeMap( CSkill::MAP_COOLTIME &kSkillCoolTimeMap )const{kSkillCoolTimeMap = m_kSkillCoolTimeMap;}

	void WriteToPacket( BM::Stream &kPacket );

	HRESULT ItemProcess( SPMO const &kOrder, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);

private:
	bool					m_bLoadedInven;
	PgInventory				m_kInv;
	CSkill::MAP_COOLTIME	m_kSkillCoolTimeMap;
};

typedef std::map< BM::GUID, PgDoc_PetInfo >			CONT_DOC_PET_INFO;

#endif // CONTENTS_CONTENTSSERVER_DOC_PGDOC_PETINFO_H