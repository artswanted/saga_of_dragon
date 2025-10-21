#pragma once
#include "BM/ClassSupport.h"

class PgDailyReward
{
private:
    CLASS_DECLARATION_S(int, DayNo);
    CLASS_DECLARATION_S(BM::DBTIMESTAMP_EX, RewardDate);

public:
	void ReadFromPacket(BM::Stream & rkPacket);
	void WriteToPacket(BM::Stream & rkPacket) const;
    bool CanGetReward() const;
    bool IsExpired() const;
    void IncreaseRewardDate();
};
