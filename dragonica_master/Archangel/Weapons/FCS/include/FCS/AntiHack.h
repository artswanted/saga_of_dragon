#ifndef WEAPON_FCS_ANTIHACKING_ANTIHACK_H
#define WEAPON_FCS_ANTIHACKING_ANTIHACK_H

#include "Lohengrin/ActArg.h"

//	관련된 정보에서 이값을 vector index 로 사용하니
//	값을 1부터 순서대로 설정 할 것
typedef enum
{
	EAHP_None = 0,
	EAHP_CheckGameTimeOver = 1,		// GameTime 검사 (client 시간이 더 빠름)
	EAHP_CheckGameTimeUnder = 2,	// GameTime 검사 (client 시간이 너무 많이 느림)
	EAHP_MaxTarget = 3,				// Action target 검사
	EAHP_SkillRange = 4,			// Skill Range 검사
	EAHP_MoveSpeed = 5,				// 이동속도 검사
	EAHP_Projectile = 6,			// 발사체 Damage 검사
	EAHP_OpenMarket = 7,			// OpenMarket 검사
	EAHP_CashShop = 8,				// CashShop 검사
	EAHP_NPCRange = 9,				// NPC 거리 검사
	EAHP_ClientNotify = 10,			// Client Hack Notify
	EAHP_AntiMacro = 11,			// Macro 방지 시스템 검사
	EAHP_SelectCharacter = 12,		// SelectedCharacter 검사
	EAHP_ActionCount = 13,			// Action count 검사
	EAHP_HyperMove = 14,			// HyperMove 검사 (대점프단, Telemove 등)
	EAHP_Blowup = 15,				// Blowup Packet 검사
	EAHP_RandomSeedCallCounter = 16,// RandomSeedCallCounter 검사
	EAHP_POLICY_MAX = 16,
} EAntiHack_Policy;

typedef enum
{
	EHack_Var_Use = 0,
	EHack_Var_Mutiflier = 1,
	EHack_Var_Add = 2,
	EHack_Var_BlockDurationTime = 3,
	EHack_Var_MaxIndex = 4,
} EAntiHack_Variable;

typedef enum
{
	EHack_CVar_SkillRange_AddedRange = 410,
	EHack_CVar_SkillRange_Z_Added = 411,
	EHack_CVar_SkillRange_Rate = 412,

	EHack_CVar_MoveSpeed_BoundRate = 510,
	EHack_CVar_MoveSpeed_HackingCounter = 511,// 2초 안에 이속 해킹이 몇번 이상 들어오면 해킹으로 간주할 지

	EHack_CVar_ActionCount_CheckTime = 1310,
	EHack_CVar_ActionCount_ActionCount = 1311,
	EHack_CVar_ActionCount_ForcedResultView = 1312,
	EHack_CVar_ActionCount_ResultToPlayer = 1313,
	EHack_CVar_ActionCount_Use_HackingPoint = 1314, //해킹 점수 주는 기능을 사용할 것인가?

	EHack_CVar_HyperMove_WrongDistance_HackIndex_Add = 1410,
	EHack_CVar_RandomSeedCallCounter_CheckValue = 1610, // 마지막으로 받은 Client의 SeedCount가 CheckValue값 이상으로 차이 나는 경우
} EAntiHack_CVariable;

#pragma pack(1)

typedef struct
{
	BM::GUID kMemberGuid;
	DWORD dwBlockToTime;
} SHackUserInfo;

typedef struct tagSHackPolicy
{
	tagSHackPolicy()
	{
		bUse = false;
		fHackIndexMultiflier = 0.0f;
		sHackIndexAdd = 0;
		iBlockTimeSec = 0;
		sMaxHackIndex = 0;
	}

	tagSHackPolicy const operator=(tagSHackPolicy const& rhs)
	{
		bUse = rhs.bUse;
		fHackIndexMultiflier = rhs.fHackIndexMultiflier;
		sHackIndexAdd = rhs.sHackIndexAdd;
		iBlockTimeSec = rhs.iBlockTimeSec;
		kCustomValue = rhs.kCustomValue;
		sMaxHackIndex = rhs.sMaxHackIndex;
		return (*this);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(bUse);
		kPacket.Pop(fHackIndexMultiflier);
		kPacket.Pop(sHackIndexAdd);
		kPacket.Pop(iBlockTimeSec);
		kPacket.Pop(sMaxHackIndex);
		kCustomValue.ReadFromPacket(kPacket);
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(bUse);
		kPacket.Push(fHackIndexMultiflier);
		kPacket.Push(sHackIndexAdd);
		kPacket.Push(iBlockTimeSec);
		kPacket.Push(sMaxHackIndex);
		kCustomValue.WriteToPacket(kPacket);
	}

	size_t min_size() const
	{
		return sizeof(bool) + sizeof(float) + sizeof(short)*2 + sizeof(int);
	}

	bool bUse;					// 사용할 것인가?
	float fHackIndexMultiflier;	// Hacking index : 해킹한 값에 곱해 줄 값
	short sHackIndexAdd;		// Hacking index : 그냥 더해 주는 값
	int iBlockTimeSec;			// 얼마동안 block 시킬것인가? (seconds 단위), -1이면 영구적,
	short sMaxHackIndex;		// Hacking Index max
	SActArg kCustomValue;
} SHackPolicy;

#pragma pack()

class PgAntiHackMgr
{
public:
	PgAntiHackMgr();
	~PgAntiHackMgr();

	void Locked_SetMaxHackIndex(short const sMax);
	short Locked_GetMaxHackIndex() const;

	bool Locked_SetPolicy(EAntiHack_Policy const & ePolicy, SHackPolicy const & kPolicyInfo);
	bool Locked_GetPolicy_Enabled(EAntiHack_Policy const & ePolicy) const;
	float Locked_GetPolicy_IndexMutiflier(EAntiHack_Policy const & ePolicy) const;
	short Locked_GetPolicy_IndexAdd(EAntiHack_Policy const & ePolicy) const;
	int Locked_GetPolicy_BlockTime(EAntiHack_Policy const & ePolicy) const;
	short Locked_GetPolicy_MaxHackIndex(EAntiHack_Policy const & ePolicy) const;

	template<typename T>
	HRESULT Locked_GetPolicy_GetValue(EAntiHack_Policy const & ePolicy, int const id, T &rkOutValue)const
	{
		BM::CAutoMutex kLock(m_kMutex);
		if (ePolicy > EAHP_POLICY_MAX && ePolicy <= 0)
		{
			return E_FAIL;
		}
		return m_kPolicy[ePolicy].kCustomValue.Get(id, rkOutValue);
	}

	/*
	template<typename T>
	HRESULT Locked_GetPolicy_SetValue(int const id, T const &rkValue)
	{
		BM::CAutoMutex kLock(m_kMutex, true);
		if (ePolicy > EAHP_POLICY_MAX && ePolicy <= 0)
		{
			return E_FAIL;
		}
		return m_kPolicy[ePolicy].kCustomValue.Set(id, rkValue);
	}
	*/


	void Locked_ReadFromPacket(BM::Stream& rkPacket);
	void Locked_WriteToPacket(BM::Stream& rkPacket) const;

	typedef std::map<BM::GUID, SHackUserInfo> CONT_HACKUSER;	// <MemberGuid, SHackUserInfo>
	typedef std::vector<SHackPolicy> VEC_ANTIHACK_POLICY;	// [EAntiHack_Policy]
protected:
	// 각종 설정 값들
	short m_sMaxIndex;


private:
	mutable ACE_RW_Thread_Mutex m_kMutex;
	CONT_HACKUSER m_kHackUser;
	VEC_ANTIHACK_POLICY m_kPolicy;
};

#define g_kAntiHackMgr SINGLETON_STATIC(PgAntiHackMgr)

#endif // WEAPON_FCS_ANTIHACKING_ANTIHACK_H