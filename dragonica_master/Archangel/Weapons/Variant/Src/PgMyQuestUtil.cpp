#include "stdafx.h"
#include "TableDataManager.h"
#include "Global.h"
#include "PgMyQuestUtil.h"
#include "PgMyQuest.h"

//
namespace MyEndQuestBitUtil
{
	int const iBytePerBit = 8;
	int const iDefaultBit = 0x1;
	int const iAllByteBit = 0xff;
}

//
namespace MyQuestMigrationTool
{
	Loki::Mutex m_kMutex;
	CONT_END_QUEST_MIGRATION_VER kContMigrationVer;
	int iLatestRevisionNo = 0;
	void SetMigrationVer(CONT_END_QUEST_MIGRATION_VER& rkCont)
	{
		BM::CAutoMutex kLock(m_kMutex);
		kContMigrationVer.swap(rkCont);
		CONT_END_QUEST_MIGRATION_VER::const_iterator iter = kContMigrationVer.begin();
		while( kContMigrationVer.end() != iter )
		{
			iLatestRevisionNo = std::max(iLatestRevisionNo, (*iter).first);
			++iter;
		}
	}
	bool CheckMigrationVer(bool const bIsNewbiePlayer, ContUserQuest const& rkContUserQuest, int& iMigrationRevision, BYTE (&byEndQuest)[MAX_DB_ENDQUEST_ALL_SIZE], CONT_ERASE_QUEST_ID& rkContErase)
	{
		BM::CAutoMutex kLock(m_kMutex);
		if( iLatestRevisionNo <= iMigrationRevision )
		{
			return false;
		}

		int const iCurMigrationVer = iMigrationRevision;
		CONT_END_QUEST_MIGRATION_VER::const_iterator iter = kContMigrationVer.begin();
		while( kContMigrationVer.end() != iter )
		{
			CONT_END_QUEST_MIGRATION_VER::mapped_type const& rkMigrationVer = (*iter).second;
			if( bIsNewbiePlayer )
			{
				iMigrationRevision = rkMigrationVer.MigrationRevision(); // 최신 버젼으로 맞춤
			}
			else
			{
				if( rkMigrationVer.Check(iMigrationRevision) )
				{
					rkMigrationVer.Do(byEndQuest);

					// 삭제할 진행중인 퀘스트 검사
					ContUserQuest::const_iterator iter = rkContUserQuest.begin();
					while( rkContUserQuest.end() != iter )
					{
						ContUserQuest::mapped_type const& pkUserQuest = (*iter).second;
						if( pkUserQuest )
						{
							if( rkContErase.end() == rkContErase.find(pkUserQuest->iQuestID)
							&&	!rkMigrationVer.IsCanRun(pkUserQuest->iQuestID) )
							{
								rkContErase.insert(pkUserQuest->iQuestID);
							}
						}
						++iter;
					}
					iMigrationRevision = rkMigrationVer.MigrationRevision(); // 버젼 업데이트
				}
			}
			++iter;
		}
		return (bIsNewbiePlayer)? false: (iCurMigrationVer != iMigrationRevision);
	}
};