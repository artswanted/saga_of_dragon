#ifndef WEAPON_VARIANT_USERINFO_PGMYQUEST_H
#define WEAPON_VARIANT_USERINFO_PGMYQUEST_H

#include "idobject.h"
#include "Lohengrin/LockUtil.h"

//
typedef std::map< int, SUserQuestState* > ContUserQuest;
typedef std::vector< SUserQuestState > ContUserQuestState;
typedef std::vector< SUserQuestState* > ContUserQuestStatePtr;
//typedef std::vector< int > ContQuestIDVec;
typedef std::vector< int > VEC_INT;
typedef std::map< int, int > ContQuestClearCount; // < QuestID, ClearCount >

int const MAX_ING_SCENARIO_QUEST	= (3);
int const MAX_ING_QUESTNUM			= (10);
int const MAX_ING_SOUL_QUEST		= (1);
int const MAX_TOTAL_ING_QUEST		= (MAX_ING_SCENARIO_QUEST + MAX_ING_QUESTNUM);
//int const MAX_TOTAL_ING_QUEST		= (MAX_ING_SCENARIO_QUEST + MAX_ING_QUESTNUM + MAX_ING_SOUL_QUEST);

class PgMyQuest// : public IDObject
{
	static int const MAX_INGQUEST_SIZE		= (MAX_DB_INGQUEST_SIZE);		// == MAX_DB_INGQUEST_SIZE
	static int const MAX_ENDQUEST_SIZE		= (MAX_DB_ENDQUEST_SIZE);		// == MAX_DB_ENDQUEST_SIZE
	static int const MAX_ENDQUEST_EXT_SIZE	= (MAX_DB_ENDQUEST_EXT_SIZE);	// == 
	static int const MAX_ENDQUEST_EXT2_SIZE	= (MAX_DB_ENDQUEST_EXT2_SIZE);	// ==
	static int const MAX_ENDQUEST_ALL_SIZE	= (MAX_DB_ENDQUEST_ALL_SIZE);	// == MAX_DB_ENDQUEST_ALL_SIZE

	typedef std::set< int > ContQuestIDSet;
	typedef PgEternalArray< BYTE, MAX_INGQUEST_SIZE > PgIngBuffer;
	typedef PgEternalArray< BYTE, MAX_ENDQUEST_SIZE > PgEndBuffer;
	typedef PgEternalArray< BYTE, MAX_ENDQUEST_EXT_SIZE > PgEndExtBuffer;
	typedef PgEternalArray< BYTE, MAX_ENDQUEST_EXT2_SIZE > PgEndExt2Buffer;
	typedef PgEternalArray< BYTE const, MAX_INGQUEST_SIZE > PgIngBufferR;
	typedef PgEternalArray< BYTE const, MAX_ENDQUEST_SIZE > PgEndBufferR;
	typedef PgEternalArray< BYTE const, MAX_ENDQUEST_EXT_SIZE > PgEndExtBufferR;
	typedef PgEternalArray< BYTE const, MAX_ENDQUEST_EXT2_SIZE > PgEndExt2BufferR;
public:
	PgMyQuest(void);
	virtual ~PgMyQuest(void);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Ing 관리/
	SUserQuestState* BeginQuest(int const iQuestID, int const iQuestType);
	SUserQuestState* AddIngQuest(const SUserQuestState* pkInfo, int const iQuestType);
	bool DropQuest(int const iQuestID);
	bool UpdateQuest(SUserQuestState const &rkUpdated);//클라이언트 전용.
	void RemoveInstanceQuest();
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//End 관리
	void AddEndQuest(int const iQuestID, wchar_t const* szFunc);
	void AddEndQuest(ContQuestID const &rkVec, wchar_t const* szFunc, int const iIgnoreQuestID = 0);
	
	void DeEndQuest(int const iQuestID, wchar_t const* szFunc);//특수!!! 해당 퀘스트를 안했다 설정.
	void DeEndQuest(ContQuestID const &rkVec, wchar_t const* szFunc);
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Ing & End 관리
	//bool CompleteQuest(int const iQuestID, int const iQuestType, ContQuestID const* pkVec=0);
////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Clear();
	void ClearIngQuest();
	SUserQuestState const* Get(int const iQuestID) const;
	bool GetQuestList(ContUserQuestState &rkVec) const;//Ing 리스트

	size_t GetIngScenarioQuestNum() const			{ return m_kScenarioQuestID.size(); }
	size_t GetIngQuestNum() const					{ return m_kIngQuest.size(); }

	bool IsEndedQuest(int const iQuestID) const;
	bool IsEndedQuestVec(VEC_INT const& rkQuestID) const;
	bool IsEndedQuestVec_All(VEC_INT const& rkQuestID) const;
	bool IsIngQuest(int const iQuestID) const;
	bool IsIngQuestVec(VEC_INT const& rkQuestID) const;
	bool IsIngQuestVec_All(VEC_INT const& rkQuestID) const;
	size_t GetFlagQusetCount(ContQuestID const &rkVec, bool const bClear)const;//체크용 함수.

	void ReadFromBuff(PgIngBufferR const& rkIngQuest, PgEndBufferR const& rkEndQuest1, PgEndBufferR const& rkEndQuest2, PgEndExtBufferR const& rkEndQuestExt, PgEndExt2BufferR const& rkEndQuestExt2);
	void WriteToBuff(PgIngBuffer& rkIngQuest, PgEndBuffer& rkEndQuest1, PgEndBuffer& rkEndQuest2, PgEndExtBuffer& rkEndQuestExt, PgEndExt2Buffer& rkEndQuestExt2)const;
	void WriteIngToBuff(PgIngBuffer& rkIngQuest)const; // Only Ing
	void WriteEndToBuff(PgEndBuffer& rkEndQuest1, PgEndBuffer& rkEndQuest2, PgEndExtBuffer& rkEndQuestExt, PgEndExt2Buffer& rkEndQuestExt2)const; // Only End

	void WriteToPacket(BM::Stream &rkPacket) const; // Ing + End
	void ReadFromPacket(BM::Stream &rkPacket);

	void Copy(PgMyQuest& rhs) const;

	void DayLoopQuestTime(BM::PgPackedTime const& rkLocalTime);
	int RandomQuestCompleteCount() const;	// 랜덤 퀘스트 완료 수량
	int TacticsQuestCompleteCount() const;	// 랜덤 용병 퀘스트 완료 수량

	void CheckMigration(CONT_DEF_QUEST_REWARD const* pkDefQuestReawrd, bool const bIsNewbiePlayer);
protected:
	SUserQuestState* _Get(int const iQuestID);//내부용.
	int GetRandomQuestCompleteCount(ContQuestID const& rkContQuest) const;

protected:
	class PgInstanceFilter
	{
	public:
		PgInstanceFilter(ContUserQuestStatePtr &rkOut, ContQuestIDSet const &rkInstanceSet)
		: m_kOutVec(rkOut), m_kInstanceSet(rkInstanceSet)
		{}
		~PgInstanceFilter() {};

		void operator () (const ContUserQuest::_Mybase::value_type &rkPair)
		{
			const ContUserQuest::key_type &rkKey = rkPair.first;
			const ContUserQuest::mapped_type pkElement = rkPair.second;
			if( m_kInstanceSet.end() == m_kInstanceSet.find(rkKey) )
			{
				std::back_inserter(m_kOutVec) = pkElement;
			}
		}

	private:
		ContUserQuestStatePtr &m_kOutVec;
		ContQuestIDSet const &m_kInstanceSet;
	};

private:
	void operator =(PgMyQuest const& rhs)
	{
		// 쌩짜 복사 금지
	}
	
	CLASS_DECLARATION_S_NO_SET(BM::PgPackedTime, DayLoopQuestTime);
	CLASS_DECLARATION_S(ContQuestID, ContRandomQuest);
	CLASS_DECLARATION_S(bool, BuildedRandomQuest);				// 랜덤 퀘스트
	CLASS_DECLARATION_S(int, BuildedRandomQuestPlayerLevel);
	CLASS_DECLARATION_S_NO_SET(int, MigrationRevision);
	CLASS_DECLARATION_S(ContQuestID, ContTacticsQuest);			// 랜덤 용병 퀘스트
	CLASS_DECLARATION_S(bool, BuildedTacticsQuest);
	CLASS_DECLARATION_S(int, BuildedTacticsQuestPlayerLevel);

	// 현상수배 퀘스트
	CLASS_DECLARATION_S(ContQuestID, ContWantedQuest);					// 빌드된 현상수배 퀘스트 ID의 컨테이너
	CLASS_DECLARATION_S(bool, BuildedWantedQuest);						// 현상수배 퀘스트가 빌드되었는가?
	CLASS_DECLARATION_S(int, BuildedWantedQuestPlayerLevel);			// 플레이어의 레벨을 가지고 현상수배 퀘스트 빌드
	CLASS_DECLARATION_S(ContQuestClearCount, ContWantedQuestClearCount);// 퀘스트 클리어 카운트
	CLASS_DECLARATION_S(bool, ExistCanBuildWantedQuest);				// 플레이어 레벨에서 빌드가 가능한 퀘스트가 있는가?

public:
	void InitWantedQuestClearCount();
	void AddWantedQuestClearCount(int const iQuestID);
	int	 GetWantedQuestClearCount(int const iQuestID) const;

private:
	BYTE m_byEndQuest[MAX_DB_ENDQUEST_ALL_SIZE];
	ContUserQuest m_kIngQuest;
	ContQuestIDSet m_kInstanceQuestID;
	ContQuestIDSet m_kScenarioQuestID;
};

#endif // WEAPON_VARIANT_USERINFO_PGMYQUEST_H