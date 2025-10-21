#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGHARDCOREDUNGEONSYSTEM_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGHARDCOREDUNGEONSYSTEM_H

class PgHardCoreDungeonSystem
{
public:
	explicit PgHardCoreDungeonSystem( int const iMode );
	~PgHardCoreDungeonSystem(void);

	bool IsOpen()const;
	int IsCanJoin( int const iLevel, SGroundKey &rkOutGndKey )const;

	HRESULT ReqOpen( __int64 i64BeginTime, __int64 i64EndTime, int const iLimitLevel, int const iGroundNo, int const iBossGroundNo );
	HRESULT RecvOpenResult( HRESULT const hRet, SGroundKey const &kGndKey );
	HRESULT RecvClose( SGroundKey const &kGndKey );
	void SetClose();

private:
	mutable Loki::Mutex	m_kMutex;

	int	const	m_iMode;
	__int64		m_i64BeginTime;
	__int64		m_i64EndTime;
	int			m_iLimitLevel;

	SGroundKey	m_kGroundKey;
	int			m_iBossGroundNo;

private:
	PgHardCoreDungeonSystem(void);
};

inline bool PgHardCoreDungeonSystem::IsOpen()const
{
	BM::CAutoMutex kLock( m_kMutex );
	return BM::GUID::IsNotNull(m_kGroundKey.Guid());
}

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGHARDCOREDUNGEONSYSTEM_H