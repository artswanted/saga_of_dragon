#pragma once

class PgAMPool
{
	typedef std::map<std::string, NiActorManager *> AMContainer;

public:
	PgAMPool(void);
	~PgAMPool(void);

	void Destroy();
public:
	NiActorManager *LoadActorManager(const char *pcKFMPath);

protected:
	AMContainer m_kContainer;
};

#define g_kAMPool Loki::SingletonHolder< PgAMPool >::Instance()