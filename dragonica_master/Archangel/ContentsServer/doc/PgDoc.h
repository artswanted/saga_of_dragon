#ifndef CONTENTS_CONTENTSSERVER_DOC_PGDOC_H
#define CONTENTS_CONTENTSSERVER_DOC_PGDOC_H

typedef enum
{
	TDOC_NONE,
	TDOC_PLAYER,
	TDOC_MYHOME,
	TDOC_PET,
}EDocType;

class PgDoc
{
public:
	PgDoc(void);
	virtual ~PgDoc(void);

	PgDoc( PgDoc const & );
	PgDoc& operator = ( PgDoc const & );

	virtual EDocType DocType(void)const = 0;

	void SetDeletable();
	bool CanDelete(ACE_Time_Value const &kNowTime)const;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private://Doc 전용 데이터
	ACE_Time_Value m_kCanDeleteTime;//Copy 될 필요 없는 값.
//	std::set< BM::GUID > m_kContWorkQuery;//진행중인 쿼리.

//<Doc 전용 데이터
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
typedef std::map<BM::GUID, PgDoc* > CONT_DOC;

#endif // CONTENTS_CONTENTSSERVER_DOC_PGDOC_H