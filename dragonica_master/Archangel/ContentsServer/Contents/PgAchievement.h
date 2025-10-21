#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGACHIEVEMENT_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGACHIEVEMENT_H

#include "Lohengrin/packetstruct.h"

class PgAchievementMgr
{
public:
	PgAchievementMgr();
	~PgAchievementMgr();

	void Broadcast(EAchievementType const Type, BM::Stream & rPacket);

private:
	void WriteToPacket_LevelUp(BM::Stream & rPacket, BM::Stream & rOut);
	void WriteToPacket_EnchantSuccess(BM::Stream & rPacket, BM::Stream & rOut);
	void WriteToPacket_GetItemMessage(BM::Stream & rPacket, BM::Stream & rOut)const;
};

#define g_kAchievementMgr SINGLETON_STATIC(PgAchievementMgr)

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGACHIEVEMENT_H