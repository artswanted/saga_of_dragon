#ifndef MAP_MAPSERVER_ACTION_MISSION_PGMISSIONMAN_H
#define MAP_MAPSERVER_ACTION_MISSION_PGMISSIONMAN_H

#pragma warning (push, 4)


typedef std::map< int, PgMissionInfo* > MissionContainer;
typedef std::map< int, int > MissionKeyContainer;

////////////////////////////////////////////////////////////////////
class PgMissionManager
{
public:
	PgMissionManager();
	~PgMissionManager();

	bool Create();
	void Reload();
	void Clear();


	bool GetMission(int const iMissionID, PgMissionInfo const *& pkOut)const;
	bool GetMissionKey(int const iMissionKey, PgMissionInfo const *& pkOut)const;
	bool GetMissionContainer(const MissionContainer*& pkOut) const;

protected:
	HRESULT CreateInfo();//정보별 Create

	bool Add(TCHAR const* pkXmlPath);

private:
	mutable Loki::Mutex		m_kMutex;

	MissionContainer		m_kMissionMap;
	MissionKeyContainer		m_kMissionKeyMap;
};

#define g_kMissionMan Loki::SingletonHolder<PgMissionManager>::Instance()

#endif // MAP_MAPSERVER_ACTION_MISSION_PGMISSIONMAN_H