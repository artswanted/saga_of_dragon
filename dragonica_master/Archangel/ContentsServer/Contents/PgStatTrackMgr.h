#pragma once
#include <Loki/Threads.h>
#include <map>


#include "BM/GUID.h"
#include "Lohengrin/dbtables.h"
#include "Variant/StatTrackInfo.h"

/**
 * \brief Stat track manager flush all stat track info to database
 */
class PgStatTrackMgr
{
protected:
	//First Item guid, Second<MonsterKillCount, PlayerKillCount>
	typedef std::map<BM::GUID, TBL_PAIR_KEY_INT> CONT_STAT_TRACK_KILLS;
public:
	PgStatTrackMgr();
	~PgStatTrackMgr();

public:
	
	/**
	 * \brief Flush all stat track info to database
	 * \see m_kStatTrackKills
	 */
	void Locked_FlushInfo();

	/**
	 * \brief Read data from map packet with stat track kills
	 * Packet struct:
	 * BM::GUID => ItemGuid
	 * unsiged short => usKillType
	 * int => iKillCount
	 * \param kPacket 
	 */
	void Locked_RecevInfo(BM::Stream &kPacket);

	/**
	 * \brief Force flush user item when he logout
	 * \param kPacket 
	 */
	void Locked_FlushOneForce(BM::Stream &kPacket);
private:
	/**
	 * \brief Flush one item to DB
	 * \param kIter Pointer to item in  m_kStatTrackKills
	 */
	void FlushOne(CONT_STAT_TRACK_KILLS::const_iterator kIter);
	
private:
	/**
	 * \brief First Item guid, Second<MonsterKillCount, PlayerKillCount>
	 */
	CONT_STAT_TRACK_KILLS m_kStatTrackKills;

	/**
	 * \brief Mutex
	 */
	mutable Loki::Mutex m_kMutex;
};

#define g_kStatTrackMgr SINGLETON_STATIC(PgStatTrackMgr)