#pragma once
#include "BM/STLSupport.h"
#include "BM/Stream.h"

const int INVALID_BATTLE_PASS_SEASON = 0;
// Personal character BP info
struct PgBattlePassInfo
{
    PgBattlePassInfo();
    void Clear();
    void ReadFromPacket(BM::Stream& kPacket);
    void WriteToPacket(BM::Stream& kPacket) const;
    void FromDB(CEL::DB_DATA_ARRAY::const_iterator &itor);

    CLASS_DECLARATION_S(int, SeasonNo);
    CLASS_DECLARATION_S(int, DayNo);
    CLASS_DECLARATION_S(int, Grade);
};

class PgBattlePassMgr
{
public:

    PgBattlePassMgr()
    {
        BM::DBTIMESTAMP_EX kValue;
        // todo: load from database
        kValue.DateFromString("%d-%d-%d %d:%d:%d", "1900-01-01 19:00:00.000");
        m_kStartDate = kValue;
        kValue.DateFromString("%d-%d-%d %d:%d:%d", "2023-01-01 19:00:00.000");
        m_kEndDate = kValue;
        m_kSeasonNo = 1;
    }

    inline bool Locked_IsActive() const
    {
        BM::DBTIMESTAMP_EX kCurTime;
        kCurTime.SetLocalTime();
        BM::CAutoMutex kLock(m_kMutex);
        return m_kStartDate < kCurTime && m_kSeasonNo != 0 && kCurTime < m_kEndDate;
    }

    inline int Locked_SeasonNo() const
    {
        BM::CAutoMutex kLock(m_kMutex);
        return m_kSeasonNo;
    }

    void Locked_ReadFromPacket(BM::Stream& kPacket);
    void Locked_WriteToPacket(BM::Stream& kPacket);

protected:

    int m_kSeasonNo;
    BM::DBTIMESTAMP_EX m_kStartDate;
    BM::DBTIMESTAMP_EX m_kEndDate;
	mutable Loki::Mutex m_kMutex;
};

#define g_kBattlePassMgr SINGLETON_STATIC(PgBattlePassMgr)
