#ifndef WEAPON_VARIANT_ACTION_BATTLE_PGAGGROMETERMGR_H
#define WEAPON_VARIANT_ACTION_BATTLE_PGAGGROMETERMGR_H

class PgAggroMeterMgr
{
	typedef std::map< BM::GUID, PgAggroMeter* > ContAggroMeter;
public:
	PgAggroMeterMgr();
	virtual ~PgAggroMeterMgr();

	void Clear();
	bool GetMeter(BM::GUID const &rkGuid, PgAggroMeter*& pkAggroMeter);

protected:
	bool AddMeter(BM::GUID const &rkGuid, int const iHP);
	bool DelMeter(BM::GUID const &rkGuid);

	Loki::Mutex m_kMeterMutex;
	ContAggroMeter m_kMap;
};

#endif // WEAPON_VARIANT_ACTION_BATTLE_PGAGGROMETERMGR_H