#ifndef CONTENTS_CONTENTSSERVER_DATABASE_PGDBSAVEACTION_H
#define CONTENTS_CONTENTSSERVER_DATABASE_PGDBSAVEACTION_H

class PgDBSaveAction
{
public:
	explicit PgDBSaveAction( BM::GUID const &kOwnerGuid ):m_kOwnerGuid(kOwnerGuid){}

protected:
	BM::GUID const m_kOwnerGuid;

private:
	PgDBSaveAction();
};

// PgDBSave_AddRankPoint
// class PgDBSave_AddRankPoint
// 	:	public PgDBSaveAction
// {
// public:
// 	explicit PgDBSave_AddRankPoint( BM::GUID const &kOwnerGuid );
// 	~PgDBSave_AddRankPoint(){}
// 
// 	HRESULT DoAction( int const iType, int const iAddValue );
// 
// private:
// 
// };

#endif // CONTENTS_CONTENTSSERVER_DATABASE_PGDBSAVEACTION_H