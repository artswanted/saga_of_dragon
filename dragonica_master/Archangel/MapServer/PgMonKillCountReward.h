#ifndef MAP_MAPSERVER_ACTION_PGMONKILLCOUNTREWARD_H
#define MAP_MAPSERVER_ACTION_PGMONKILLCOUNTREWARD_H

class PgMonKillCountReward
{
	typedef std::vector< SMonKillRewardItem > ContReward;
	typedef std::map< int, ContReward > ContKillCountReward;
public:
	PgMonKillCountReward();
	~PgMonKillCountReward();

	bool Init();
	bool Check(int const iCount, int const iLevel, SMonKillRewardItem &rkOut) const;

	CLASS_DECLARATION_S_NO_SET(bool, MonsterKillSystem);
	CLASS_DECLARATION_S_NO_SET(int, MonsterKillResetMax);	// 최대 카운트( = 리셋 카운트)
	CLASS_DECLARATION_S_NO_SET(int, MonsterKillPerMsg);		// 얼마당 클라이언트로 패킷 쏴줄 거냐?

	CLASS_DECLARATION_S_NO_SET(int, CanCountLvMin);
	CLASS_DECLARATION_S_NO_SET(int, CanCountLvMax);
private:
	ContKillCountReward m_kReward;
};

#define g_kMonKillCountReward Loki::SingletonHolder< PgMonKillCountReward >::Instance()

#endif // MAP_MAPSERVER_ACTION_PGMONKILLCOUNTREWARD_H