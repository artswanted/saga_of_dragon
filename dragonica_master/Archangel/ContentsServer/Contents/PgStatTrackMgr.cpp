#include "stdafx.h"
#include "PgStatTrackMgr.h"

PgStatTrackMgr::PgStatTrackMgr()
{
	m_kStatTrackKills.clear();
}

PgStatTrackMgr::~PgStatTrackMgr()
{
	Locked_FlushInfo();
}

void PgStatTrackMgr::Locked_FlushInfo()
{
	//TODO: Check what will be if user logout
	CONT_STAT_TRACK_KILLS kKills;
	{
		BM::CAutoMutex kLock(m_kMutex);
		kKills.swap(m_kStatTrackKills);
	}
	CONT_STAT_TRACK_KILLS::const_iterator kIter = kKills.begin();
	while (kIter != kKills.end())
	{
		FlushOne(kIter);
		++kIter;
	}
}

void PgStatTrackMgr::Locked_RecevInfo(BM::Stream &kPacket)
{
	BM::GUID kItemGUID = BM::GUID::NullData();
	EStatTrackKillType usKillType;
	int iKillCount = 0;
	kPacket.Pop(kItemGUID);
	kPacket.Pop(usKillType);
	kPacket.Pop(iKillCount);
	{
		BM::CAutoMutex kLock(m_kMutex);
		CONT_STAT_TRACK_KILLS::iterator kFind = m_kStatTrackKills.find(kItemGUID);
		TBL_PAIR_KEY_INT kPair(0,0);
		switch (usKillType)
		{
			case STKT_PLAYER:  { kPair.kSecKey = iKillCount;  } break;
			case STKT_MONSTER: { kPair.kPriKey = iKillCount;  } break;
			default:		   { CAUTION_LOG(BM::LOG_LV5, _T("Undefined kill type[") << static_cast<unsigned short>(usKillType) << "]"); } break;
		}

		if (kFind != m_kStatTrackKills.end())
		{
			kFind->second.kPriKey += kPair.kPriKey;
			kFind->second.kSecKey += kPair.kSecKey;
		}
		else
		{
			m_kStatTrackKills.insert(std::make_pair(kItemGUID, kPair));
		}
	}
}

void PgStatTrackMgr::Locked_FlushOneForce(BM::Stream& kPacket)
{
	BM::GUID kGUID;
	if (kPacket.Pop(kGUID))
	{
		BM::CAutoMutex kLock(m_kMutex);
		CONT_STAT_TRACK_KILLS::const_iterator kIter = m_kStatTrackKills.find(kGUID);
		if (kIter != m_kStatTrackKills.end())
		{
			FlushOne(kIter);
			m_kStatTrackKills.erase(kGUID);
		}
	}
}

void PgStatTrackMgr::FlushOne(CONT_STAT_TRACK_KILLS::const_iterator kIter)
{
	CEL::DB_QUERY kQueryItem(DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Item_StatTrack_Update]");
	kQueryItem.PushStrParam(kIter->first.str());				// ItemGUID
	kQueryItem.PushStrParam((int)AT_STAT_TRACK);					// Type01
	kQueryItem.PushStrParam(1);								// Value01
	kQueryItem.PushStrParam((int)AT_STAT_TRACK_KILL_COUNT_MON);		// Type02
	kQueryItem.PushStrParam(kIter->second.kPriKey);					// Value02
	kQueryItem.PushStrParam((int)AT_STAT_TRACK_KILL_COUNT_PLAYER);	// Type03
	kQueryItem.PushStrParam(kIter->second.kSecKey);					// Value03
	kQueryItem.PushStrParam(0);								// Type04
	kQueryItem.PushStrParam(0);								// Value04
	kQueryItem.PushStrParam(0);								// Type05
	kQueryItem.PushStrParam(0);								// Value05

	g_kCoreCenter.PushQuery(kQueryItem);
}
