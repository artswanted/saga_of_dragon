#ifndef MAP_MAPSERVER_UTIL_PGHACKDUALKEYBOARD_H
#define MAP_MAPSERVER_UTIL_PGHACKDUALKEYBOARD_H

#include "BM/TWrapper.h"

namespace HackDualKeyboard
{
	//
	typedef struct tagMatchResult
	{
		tagMatchResult();
		tagMatchResult(tagMatchResult const& rhs);
		//void AddPoint(size_t const iPoint);
		void AddCount();
		void Reset();

		BM::GUID kLhsKey;
		size_t iLhsTotalCount;
		BM::GUID kRhsKey;
		size_t iRhsTotalCount;
		size_t iMatchCount;
		DWORD dwEleapsedTime;
		DWORD dwTotalEleapsedTime;
		//size_t iMatchPoint;
	} SMatchResult;
	typedef std::pair< BM::GUID, BM::GUID > MATCH_KEY;
	typedef std::map< MATCH_KEY, SMatchResult > CONT_MATCH_RESULT;
	typedef std::list< SMatchResult > CONT_MATCH_RESULT_LIST;
	typedef std::map< SGroundKey, CONT_MATCH_RESULT_LIST > CONT_MATCH_MAP_RESULT;

	//
	bool IsCanReset(DWORD const dwLimitTime, SMatchResult const& rkMatchResult);
	bool IsCanDetection(size_t const iMatchPercent, SMatchResult const& rkMatchResult);

	//
	typedef enum eHackActionCheckType
	{
		HACT_NONE		= 0,
		HACT_MOVE		= 1,
		HACT_MOVE_LEFT	= 2,
		HACT_MOVE_RIGHT	= 3,
		HACT_MOVE_TOP	= 4,
		HACT_MOVE_BOTTOM	= 5,
		HACT_ATTACK		= 6,
		HACT_JUMP		= 7,
		HACT_DASH		= 8,
		HACT_DASH_JUMP	= 9,
		HACT_JUMP_DASH	= 10,
		HACT_ACTION_SKILL	= 11,
	} EHackActionCheckType;

	//
	typedef struct tagActionItem
	{
		tagActionItem();
		tagActionItem(EHackActionCheckType const Type, DWORD const Time);

		void operator =(tagActionItem const& rhs);

		EHackActionCheckType eType;
		DWORD dwTime;
	}SActionItem;
	typedef std::list< SActionItem > CONT_ACTION_ITEM;

	//
	class PgActionHistory
	{
	public:
		PgActionHistory();
		PgActionHistory(PgActionHistory const& rhs);
		~PgActionHistory();

		void Add(EHackActionCheckType const eType, DWORD const dwTime);
		void Check(PgActionHistory const& rhs, SMatchResult& rkOutMatchResult) const;
		void Flush();
		size_t GetTotalCount() const;

	protected:
		bool CheckTime(DWORD& dwDiff, CONT_ACTION_ITEM::value_type const& lhs, CONT_ACTION_ITEM::value_type const& rhs, SMatchResult& rkOutMatchResult) const;

	private:
		size_t m_iPoint;
		CONT_ACTION_ITEM m_kContItem;
	};
	typedef std::map< BM::GUID, PgActionHistory > CONT_ACTION_HISTORY;

	//
	class PgActionCheckMgr_Impl
	{
	public:
		PgActionCheckMgr_Impl();
		PgActionCheckMgr_Impl(PgActionCheckMgr_Impl const& rhs);
		~PgActionCheckMgr_Impl();

		void Add(BM::GUID const& rkKeyGuid);
		void Del(BM::GUID const& rkKeyGuid, CONT_MATCH_RESULT_LIST& rkContOut);
		void Action(BM::GUID const& rkKeyGuid, EHackActionCheckType const eType, DWORD const dwTime);
		void Tick(DWORD const dwTimeLine);
		void GetResult(CONT_MATCH_RESULT_LIST& rkContOut, size_t const iMatchPercent, DWORD const dwLimitTime);

	private:
		SMatchResult MakeNewResult(CONT_ACTION_HISTORY::const_iterator lhs, CONT_ACTION_HISTORY::const_iterator rhs) const;

	private:
		Loki::Mutex m_kMutex;
		DWORD m_dwTimeLine;
		CONT_ACTION_HISTORY m_kContHistory;
		CONT_MATCH_RESULT m_kContResult;
	};

	//
	class PgActionCheckMgr
	{
		typedef std::map< SGroundKey, PgActionCheckMgr_Impl > CONT_HACK_ACTION_CHECKER;
	public:
		PgActionCheckMgr();
		~PgActionCheckMgr();

		void Reg(SGroundKey const& rkGndKey, DWORD const dwTime);
		void Unreg(SGroundKey const& rkGndKey);
		void Add(SGroundKey const& rkGndKey, BM::GUID const& rkCharGuid);
		void Del(SGroundKey const& rkGndKey, BM::GUID const& rkCharGuid, CONT_MATCH_RESULT_LIST& rkContOut);
		void Action(SGroundKey const& rkGndKey, BM::GUID const& rkKeyGuid, EHackActionCheckType const eType, DWORD const dwTime);
		void Tick(DWORD const dwTimeLine, CONT_MATCH_MAP_RESULT& rkContOut, size_t const iLimitMatchRate, DWORD const dwLimitTime);

	private:
		ACE_RW_Thread_Mutex m_kThreadMutex;
		CONT_HACK_ACTION_CHECKER m_kCont;
	};

	enum EActionCheckMgrMsg : int
	{
		ACMM_REG_GROUND = 0,
		ACMM_UNREG_GROUND = 1,
		ACMM_ADD_USER,
		ACMM_DEL_USER,
		ACMM_ACTION,
		ACCM_TICK,
	};
};

#endif // MAP_MAPSERVER_UTIL_PGHACKDUALKEYBOARD_H