#include "stdafx.h"
#include "PgBattlePassMgr.h"

PgBattlePassInfo::PgBattlePassInfo() : m_kSeasonNo(INVALID_BATTLE_PASS_SEASON)
{

}

void PgBattlePassInfo::Clear()
{
    m_kSeasonNo = INVALID_BATTLE_PASS_SEASON;
    m_kDayNo = 0;
    m_kGrade = 0;
}

void PgBattlePassInfo::ReadFromPacket(BM::Stream& kPacket)
{
    kPacket.Pop(m_kSeasonNo);
    kPacket.Pop(m_kDayNo);
    kPacket.Pop(m_kGrade);
}

void PgBattlePassInfo::WriteToPacket(BM::Stream& kPacket) const
{
    kPacket.Push(m_kSeasonNo);
    kPacket.Push(m_kDayNo);
    kPacket.Push(m_kGrade);
}

void PgBattlePassInfo::FromDB(CEL::DB_DATA_ARRAY::const_iterator &itor)
{
    itor->Pop(m_kSeasonNo); ++itor;
    itor->Pop(m_kDayNo); ++itor;
    itor->Pop(m_kGrade); ++itor;
}

void PgBattlePassMgr::Locked_ReadFromPacket(BM::Stream& kPacket)
{
    BM::CAutoMutex kLock(m_kMutex);
    kPacket.Pop(m_kSeasonNo);
    kPacket.Pop(m_kStartDate);
    kPacket.Pop(m_kEndDate);
}

void PgBattlePassMgr::Locked_WriteToPacket(BM::Stream& kPacket)
{
    BM::CAutoMutex kLock(m_kMutex);
    kPacket.Push(m_kSeasonNo);
    kPacket.Push(m_kStartDate);
    kPacket.Push(m_kEndDate);
}
