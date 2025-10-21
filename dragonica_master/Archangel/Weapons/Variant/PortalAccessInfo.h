#pragma once

struct SPortalAccessInfo
{
	struct SLevel
	{
		SLevel() : bOk(false), iMin(0), iMax(0) {}
		bool bOk;
		int iMin;
		int iMax;
	};

	struct STime
	{
		STime() : bOk(false) {}
		bool bOk;
	};

	struct SQuest
	{
		SQuest() : bOk(false) {}
		SQuest(PgPortalAccess::SQKey const& _data, bool const _ok) : bOk(_ok), kData(_data) {}
		SQuest & operator=(SQuest const& rhs)
		{
			bOk = rhs.bOk;
			kData = rhs.kData;
			return (*this);
		}

		bool bOk;
		PgPortalAccess::SQKey kData;
	};
	typedef std::vector<SQuest> CONT_OR_QUEST;
	typedef std::map<int, CONT_OR_QUEST> CONT_AND_QUEST;

	struct SItem
	{
		SItem() : bOk(false) {}
		SItem(PgPortalAccess::SKey const& _data, bool const _ok) : bOk(_ok), kData(_data) {}
		SItem & operator=(SItem const& rhs)
		{
			bOk = rhs.bOk;
			kData = rhs.kData;
			return (*this);
		}

		bool bOk;
		PgPortalAccess::SKey kData;
	};
	typedef std::vector<SItem> CONT_OR_ITEM;
	typedef std::map<int, CONT_OR_ITEM> CONT_AND_ITEM;

	struct SEffect
	{
		SEffect() : bOk(false), iNo(0) {}

		bool bOk;
		int iNo;
	};
	struct SEffectInfo
	{
		SEffectInfo() : bHave(false) {}

		typedef std::vector<SEffect> CONT_EFFECT;
		CONT_EFFECT kCont;
		bool bHave;
	};
	typedef std::vector<SEffectInfo> CONT_EFFECT_OR;
	typedef std::vector<std::pair<CONT_EFFECT_OR,bool> > CONT_EFFECT;	//second: bOk

	SPortalAccessInfo() : bMoveLock(false)
	{
	}

	bool IsAccess()const;

	bool bMoveLock;
	SLevel kLevel;
	STime kTime;
	CONT_AND_QUEST kContQuest;
	CONT_AND_ITEM kContItem;
	CONT_EFFECT kContEffect;
};
