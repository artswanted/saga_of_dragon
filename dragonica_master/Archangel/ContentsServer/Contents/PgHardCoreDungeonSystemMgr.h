#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGHARDCOREDUNGEONSYSTEMMGR_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGHARDCOREDUNGEONSYSTEMMGR_H

class PgHardCoreDungeonSystem;

class PgHardCoreDungeonSystemMgr
{
public:
	typedef std::map< int, PgHardCoreDungeonSystem* >	CONT_SYSTEM;

public:
	PgHardCoreDungeonSystemMgr(void);
	~PgHardCoreDungeonSystemMgr(void);

	HRESULT Regist( CONT_SYSTEM::key_type const iID );
	void OnTick();

	bool IsOpen( CONT_SYSTEM::key_type const iID )const;
	int IsCanJoin( CONT_SYSTEM::key_type const iID, int const iLevel, SGroundKey &rkOutGndKey )const;

	HRESULT RecvOpenResult( CONT_SYSTEM::key_type const iID, HRESULT const hRet, SGroundKey const &kGndKey );
	HRESULT RecvClose( SGroundKey const &kGndKey );
	void RecvAllClose();

private:
	PgHardCoreDungeonSystem* GetSystem( int const IID );

private:
	CONT_SYSTEM		m_kContSystem;
};

class PgHardCoreDungeonSystemMgr_Wrapper
	:	public TWrapper< PgHardCoreDungeonSystemMgr >
{
public:
	PgHardCoreDungeonSystemMgr_Wrapper(void);
	virtual ~PgHardCoreDungeonSystemMgr_Wrapper(void);

	HRESULT Regist( PgHardCoreDungeonSystemMgr::CONT_SYSTEM::key_type const iID );
	void OnTick();
	void RecvAllClose();

	HRESULT ProcessPacket( BM::Stream * const pkPacket );
	
};

#define g_kHardCoreDungeonMgr SINGLETON_STATIC(PgHardCoreDungeonSystemMgr_Wrapper)

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGHARDCOREDUNGEONSYSTEMMGR_H