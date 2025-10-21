#ifndef MAP_MAPSERVER_MAP_ENVIRONMENT_PGWORLDENVIRONMENTSTATUS_H
#define MAP_MAPSERVER_MAP_ENVIRONMENT_PGWORLDENVIRONMENTSTATUS_H

class PgWorldEnvironmentStatus
{
public:
	PgWorldEnvironmentStatus();
	~PgWorldEnvironmentStatus();

	bool Update(SWorldEnvironmentStatus const& rkNewEnvStatus);
	bool Del(EWorldEnvironmentStatusType const& reType);
	bool Tick();
	DWORD GetFlag() const;

private:
	PgWorldEnvironmentStatus(PgWorldEnvironmentStatus const& rhs); // 호출 금지
	void operator =(PgWorldEnvironmentStatus const& rhs);

private:
	//mutable Loki::Mutex m_kMutex;
	CONT_WORLD_ENVIRONMENT_STATUS m_kContEnv;
	SWorldEnvironmentStatusBitFlag m_kResult;
};

#endif // MAP_MAPSERVER_MAP_ENVIRONMENT_PGWORLDENVIRONMENTSTATUS_H