#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGJOBSKILLCHECKER_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGJOBSKILLCHECKER_H

#include "bm/twrapper.h"

namespace JSMgr
{
	class PgJobSkillVerify
	{
	public:
		PgJobSkillVerify();
		~PgJobSkillVerify();

		bool Check();

	private:
		void Check_Skill(bool &rbRet);
		void Check_Expertness(bool &rbRet);
		void Check_SaveIdx(bool &rbRet);
		void Check_Tool(bool &rbRet);
		void Check_Location(bool &rbRet);
		void Check_Shop(bool &rbRet);

		void ShowLogList();

	private:
		CONT_DEF_JOBSKILL_SKILL						const* m_pkDefJobSkill_Skill;
		CONT_DEF_JOBSKILL_SKILLEXPERTNESS			const* m_pkDefJobSkill_Expertness;
		CONT_DEF_JOBSKILL_SAVEIDX					const* m_pkDefJobSkill_SaveIdx;
		CONT_DEF_JOBSKILL_TOOL						const* m_pkDefJobSkill_Tool;
		CONT_DEF_JOBSKILL_LOCATIONITEM				const* m_pkDefJobSkill_LocationItem;
		CONT_DEF_JOBSKILL_PROBABILITY_BAG			const* m_pkDefJobSkill_Probability_Bag;
		CONT_DEF_JOBSKILL_SHOP						const* m_pkDefJobSkill_Shop;

		std::list<std::wstring> m_kContErrLog;
	};
};

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGJOBSKILLCHECKER_H