#ifndef CONTENTS_CONTENTSSERVER_DOC_PGJOBCHECKER_H
#define CONTENTS_CONTENTSSERVER_DOC_PGJOBCHECKER_H

class PgJobChecker
{
protected:
	static volatile LONG ms_lAddJobCount;
	static volatile LONG ms_lCompleteJobCount;
	static DWORD ms_dwJobStartTime;
	
public:
	static void AddJob();
	static void CompleteJob();
	static void DisplayJobState();
	static void ResetJobState();

public:
	typedef std::map< __int64, size_t >	CONT_COUNT;

	void Add( __int64 const iType );
	void Display()const;

public:
	PgJobChecker();
	PgJobChecker( PgJobChecker const &rhs );
	virtual ~PgJobChecker();

	PgJobChecker& operator=( PgJobChecker const &rhs );

	CONT_COUNT		m_kContJobChecker;
	size_t			m_iTotalCount;
	DWORD			m_dwStartTime;
};

#endif // CONTENTS_CONTENTSSERVER_DOC_PGJOBCHECKER_H