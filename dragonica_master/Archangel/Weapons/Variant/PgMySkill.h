#ifndef WEAPON_VARIANT_SKILL_EFFECT_PGMYSKILL_H
#define WEAPON_VARIANT_SKILL_EFFECT_PGMYSKILL_H

#include "IGObject.h"
#include "Unit.h"

typedef enum eSkillDivideType// AT_SKILL_DIVIDE_TYPE: 스킬의 시발점(무엇으로 부터 생성되었는가)
{
	SDT_None = 0,
	//DB사용 값 순서 바꾸지 말것
	SDT_Normal	= 1,	//직업별 스킬
	SDT_Item	= 2,	//아이템 스킬
	SDT_Guild	= 3,	//길드 스킬
	SDT_Couple	= 4,	//커플 스킬
	SDT_Wedding	= 5,	//결혼 스킬
	SDT_Special = 6,	//스페셜 스킬(아이템으로 또는 특정 상황에서 배우는 전직업 공통 스킬들)
	SDT_Job     = 7,	//직업 스킬

	//SYSTEM 사용 전용
	SDT_TOGGLE	= 254,	//
	SDT_LIMIT	= 255,	//
} ESkillDivideType;

typedef std::set< int > ContSkillDivideType;

#pragma pack(1)
typedef int SKILLNO_TYPE;
typedef struct tagMySkill
{
	int iSkillNo;
	BYTE cDivideType;//== ESkillDivideType
	BYTE byIndex;

	tagMySkill()
		:iSkillNo(0), cDivideType(0), byIndex(0)
	{
	}

	tagMySkill(int const SkillNo, BYTE const DevideType, BYTE const _byIndex )
		:iSkillNo(SkillNo)
		,byIndex(_byIndex)
	{
		cDivideType = (DevideType)? DevideType: SDT_Normal;
	}

	bool operator == (int const& iSkillNo) const
	{
		return this->iSkillNo == iSkillNo;
	}

	operator int () const
	{
		return iSkillNo;
	}

	DEFAULT_TBL_PACKET_FUNC();
}SMySkill;

typedef std::vector< SMySkill > CONT_MYSKILL;
typedef std::vector< SKILLNO_TYPE > ContSkillNo;

typedef std::map<int,BYTE>	CONT_SKILLEXTEND;



const short MAX_SKILLSET_USEITEM = 100;
const short MAX_SKILLSET_GROUP = 10;
typedef struct tagUSER_SKILLSET {
	typedef BYTE KEY_TYPE;

	tagUSER_SKILLSET() : bySetNo(0)
	{
		memset(byDelay,0,sizeof(byDelay));
		memset(byType,0,sizeof(byType));
		memset(iSkillNo,0,sizeof(iSkillNo));
		memset(iCheckSkillNo,0,sizeof(iCheckSkillNo));
	}

	KEY_TYPE bySetNo;
	BYTE byDelay[MAX_SKILLSET_GROUP];
	BYTE byType[MAX_SKILLSET_GROUP];
	int iSkillNo[MAX_SKILLSET_GROUP];
	int iCheckSkillNo[MAX_SKILLSET_GROUP];			//사용가능한지 체크용

	DEFAULT_TBL_PACKET_FUNC();
} SUSER_SKILLSET;
#pragma pack()

typedef std::map<SUSER_SKILLSET::KEY_TYPE,SUSER_SKILLSET> CONT_USER_SKILLSET;

typedef enum eSKILLSET_PACKETTYTE: BYTE
{
	ESPT_NONE 		= 0,
	ESPT_EMPTY 		= 1,
	ESPT_NOT_EMPTY	= 2,
}ESKILLSET_PACKETTYTE;

class PgSaveSkillSetPacket
{
public:
	PgSaveSkillSetPacket();
	PgSaveSkillSetPacket(CONT_USER_SKILLSET const & kContSkillSet);

	void WriteToPacket(BM::Stream &rkPacket)const;
	bool ReadFromPacket(BM::Stream &rkPacket);
	bool IsEmptySkillGroup(CONT_USER_SKILLSET::mapped_type const & rkSkillSet)const;
	void Clear();
	CONT_USER_SKILLSET const & ContSkillSet();

private:
	CONT_USER_SKILLSET m_kContSkillSet;
};

class PgMySkill 
	: public IGObject
{
	class PgRemoveFilterIfNot
	{
	public:
		PgRemoveFilterIfNot(ESkillDivideType const& rkDivideType)
			:m_eDivideType(rkDivideType)
		{}

		bool operator () (SMySkill const& rkSkill)
		{
			return rkSkill.cDivideType != m_eDivideType;
		}
	private:
		ESkillDivideType m_eDivideType;
	};

public :
	typedef enum eSkillOverLevelType
	{
		SOLT_ONLY_ONE_SKILL = 0, // 스킬 1개 레벨 + @
		SOLT_ALL_SKILL = 1,		// 스킬 전체 레벨 + @
		SOLT_CLASS_SKILL = 2,	// 특정 클래스 스킬 레벨 + @
		SOLT_MAX = 3,

	} ESkillOverLevelType;

public:
	PgMySkill();
	virtual ~PgMySkill();

	PgMySkill& operator=( PgMySkill const& rhs );

	void Swap( PgMySkill & rhs );

public:
	virtual int CallAction(WORD wActCode, SActArg* pActArg) { return 0; }
	virtual int Enter(SActArg* pActArg){return 0;}
	virtual int Leave(SActArg* pActArg){return 0;}

	void Add( int const iSkill, BYTE const byIndex );
	void Add(size_t const iBufferSize, BYTE const* pkBuffer);

	void InitSkillExtend(size_t const iExtendBufferSize, BYTE const * pkExtendBuffer);
	bool SetExtendLevel(int const iSkillNo,BYTE const bExtendLevel);
	BYTE GetExtendLevel(int const iSkillNo) const;
	size_t SaveExtend(size_t const iBufferSize, BYTE* pkBuffer)const;

	bool Delete(int const iSkillNo);
	bool EraseSkill(ESkillDivideType const eSkillDivideType,  int* piOutputSP = NULL);
	int GetSkillNo(ESkillType eType, size_t const iIndex);
	int GetLearnedSkill(int const iSkillNo, bool const bOverSkill = false) const;
	int GetNeedSP(int const iNewSkill, bool const bLearnNewSkill = true);
	void GetHaveSkills(ESkillDivideType const kSkillDivideType, ContSkillNo& rkOut)const;
	void Init(void);
	bool IsExist(int const iSkillNo, bool const bOverSkill = false) const;
	ELearnSkillRet LearnSkillCheck(int const m_iSkillNo, int const m_iLevel, int const m_iClass);
	HRESULT LearnNewSkill(int const iSkillNo);

	size_t Save(size_t const iBufferSize, BYTE* pkBuffer, ContSkillDivideType const& rkExceptionDivideType = ContSkillDivideType());
	bool SetToggle(bool const bOn, int const iSkillNo);

	void WriteToPacket( EWRITETYPE const kWriteType, BM::Stream& rkPacket);
	void ReadFromPacket(BM::Stream& rkPacket);

	void ClearOverSkillLevel(eSkillOverLevelType const eType = SOLT_MAX);
	void AddOverSkillLevel(eSkillOverLevelType const eType, int const iTypeValue = 0, int const iLevel = 0);
	void SwapOverSkillLevel(eSkillOverLevelType const eType, CONT_SKILL_OPT& kSkillOpt);
	CONT_SKILL_OPT const GetCalcOverSkillList(eSkillOverLevelType const eType) const; // 함수 내부에서 리턴하는 컨테이너는 최대레벨을 검사후 변경된 컨테이너를 리턴한다.

	int GetOverSkill(int const iSkillNo) const; // 오버된 해당 스킬 번호를 리턴
	int GetOverSkillLevel(int const iSkillNo) const; // 해당 스킬이 얼만큼 + @ 인지를 돌려준다. +Level 를 리턴
	bool GetSkillIndex( int const iSkillNo, BYTE &rkOutIndex )const;	
	
protected:
	bool Add(ESkillType const eSkillType, SMySkill const& rkSkill);
	bool Delete(ESkillType const eSkillType, int const iSkillNo);
	int Exist(CONT_MYSKILL const& rkVector, int const iSkillNo) const;
	bool Get(CONT_MYSKILL const& rkVector, int const iSkillNo, SMySkill& rkOut )const;
	int ExistBasicSkill(const CONT_MYSKILL& rkVector, int const iSkillNo) const;
	HRESULT GetContainer(ESkillType const eMST, CONT_MYSKILL*& pkCont);
	HRESULT GetContainer(ESkillType const eMST, CONT_MYSKILL const*& pkCont) const;
	bool LearnedSkill(ESkillType const eSkillType, int const iSkillNo);

	//Over Skill Level
	

	CONT_SKILL_OPT	m_kClassSkillOverCont;		// 직업 스킬 + @ (클래스 번호, + 레벨)
	CONT_SKILL_OPT	m_kSkillOverCont;			// 스킬 + @ (스킬 번호, + 레벨)
	int				m_kAllSkillOver;			// 전체 스킬 + @ (값이 Over 레벨)

private:
	CLASS_DECLARATION_S(CONT_USER_SKILLSET,ContSkillSet);//SkillSet

	//->DB 에서 읽은값.
	CONT_MYSKILL m_kGeneral;
	CONT_MYSKILL m_kActive;
	CONT_MYSKILL m_kPassive;
	CONT_MYSKILL m_kToggle;//배운 토글

	//실시간 값.
	CONT_MYSKILL m_kToggleOn;//켜진 토글

	CONT_SKILLEXTEND m_kSkillExtend;// 스킬 배우기 확장
};

#endif // WEAPON_VARIANT_SKILL_EFFECT_PGMYSKILL_H