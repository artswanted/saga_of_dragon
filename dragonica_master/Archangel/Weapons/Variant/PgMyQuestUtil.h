#ifndef WEAPON_VARIANT_UTIL_PGMYQUESTUTIL_H
#define WEAPON_VARIANT_UTIL_PGMYQUESTUTIL_H

//
namespace MyEndQuestBitUtil
{
	extern int const iBytePerBit;
	extern int const iDefaultBit;
	extern int const iAllByteBit;

	template< size_t _T_ARRAY_SIZE >
	bool AddEndQuest(BYTE (&abyArray)[_T_ARRAY_SIZE], int const iQuestID)
	{
		int const iByteOffset = iQuestID / iBytePerBit;
		int const iBitOffset = iQuestID % iBytePerBit;

		if( 0 <= iByteOffset
		&&	_T_ARRAY_SIZE > iByteOffset )
		{
			BYTE const byValue = static_cast< BYTE >(iDefaultBit << iBitOffset);
			abyArray[iByteOffset] |= byValue;
		}
		else
		{
			return false;
		}
		return true;
	}
	template< size_t _T_ARRAY_SIZE >
	bool AddDeEndQuest(BYTE (&abyArray)[_T_ARRAY_SIZE], int const iQuestID)
	{
		int const iByteOffset = iQuestID / iBytePerBit;
		int const iBitOffset = iQuestID % iBytePerBit;

		if( 0 <= iByteOffset
		&&	_T_ARRAY_SIZE > iByteOffset )
		{
			BYTE const byValue = static_cast< BYTE >((iDefaultBit << iBitOffset) ^ iAllByteBit); // 쉬프트 하고, 뒤집는다.
			abyArray[iByteOffset] &= byValue; // 해당 비트만 0으로 초기화
		}
		else
		{
			return false;
		}
		return true;
	}
}

namespace MyQuestMigrationTool
{
	typedef std::set< int > CONT_ERASE_QUEST_ID;

	// 종료 퀘스트 정보를 다음 버젼에 맞게 변화 시킨다
	// PgMyQuest에 MigrationRevision을 저장한다
	// 불러온 MigrationRevision보다 낮으면 단계별로 적용 시킨다
	// 기존 퀘스트에 대해서 설정할 조건에 대한 분기지점을 만들지 않는다

	template< size_t T_ARRAY_SIZE >
	class PgQuestMigrationVer
	{
		typedef std::set< int > CONT_QUEST_ID;
	public:
		PgQuestMigrationVer(int const iMigrationRevision)
			: m_kMigrationRevision(iMigrationRevision), m_kContResetQuestID()
		{
			// 기본은 모두 설정하지 않고 통과
			::memset(m_byAnd, 0xFF, sizeof(m_byAnd));
			::memset(m_byOr, 0x00, sizeof(m_byOr));
		}
		PgQuestMigrationVer(PgQuestMigrationVer const& rhs)
			: m_kMigrationRevision(rhs.m_kMigrationRevision), m_kContResetQuestID(rhs.m_kContResetQuestID)
		{
			::memcpy(m_byAnd, rhs.m_byAnd, sizeof(m_byAnd));
			::memcpy(m_byOr, rhs.m_byOr, sizeof(m_byOr));
		}
		~PgQuestMigrationVer()
		{
		}

		bool Add(int const iQuestID, std::wstring const& rkMode, int const iTargetValue)
		{
			if( rkMode == L"ALLOW" )
			{
				// do nothing
			}
			else if( rkMode == L"SET" )
			{
				m_kContResetQuestID.insert( iQuestID ); // 리셋/강제 완료 되는 퀘스트는 저장
				if( 0 == iTargetValue )
				{
					return MyEndQuestBitUtil::AddDeEndQuest(m_byAnd, iQuestID);
				}
				else
				{
					return MyEndQuestBitUtil::AddEndQuest(m_byOr, iQuestID);
				}
			}
			else
			{
				return false;
			}
			return true;
		}
		bool Check(int const iCurRevision) const
		{
			return iCurRevision < m_kMigrationRevision;
		}
		void Do(BYTE (&m_byEndQuest)[T_ARRAY_SIZE]) const
		{
			BYTE* pkBegin = m_byEndQuest;
			BYTE const* pkBeginAnd = m_byAnd;
			BYTE const* pkBeginOr = m_byOr;
			while( PgArrayUtil::IsInArray(pkBegin, m_byEndQuest)
				&&	PgArrayUtil::IsInArray(pkBeginAnd, m_byAnd)
				&&	PgArrayUtil::IsInArray(pkBeginOr, m_byOr) )

			{
				*pkBegin = (*pkBegin & *pkBeginAnd) | *pkBeginOr;

				++pkBegin;
				++pkBeginAnd;
				++pkBeginOr;
			}
		}
		bool IsCanRun(int const iQuestID) const
		{
			return m_kContResetQuestID.end() == m_kContResetQuestID.find(iQuestID);
		}

	protected:

		CLASS_DECLARATION_S_NO_SET(int, MigrationRevision);
	private:
		BYTE m_byAnd[T_ARRAY_SIZE];
		BYTE m_byOr[T_ARRAY_SIZE];
		CONT_QUEST_ID m_kContResetQuestID;
	};
	typedef std::map< int, PgQuestMigrationVer< MAX_DB_ENDQUEST_ALL_SIZE > > CONT_END_QUEST_MIGRATION_VER;
	void SetMigrationVer(CONT_END_QUEST_MIGRATION_VER& rkCont);
};

#endif // WEAPON_VARIANT_UTIL_PGMYQUESTUTIL_H