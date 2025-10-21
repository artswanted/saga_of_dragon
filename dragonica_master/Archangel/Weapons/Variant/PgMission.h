#ifndef WEAPON_VARIANT_BASIC_MISSION_PGMISSION_H
#define WEAPON_VARIANT_BASIC_MISSION_PGMISSION_H

#include "constant.h"
#include "Lohengrin/PacketType.h"
#include "Lohengrin/dbtables.h"
#include "Variant/PgPlayer_MissionReport.h"
#include "item.h"
#include "PgConstellation.h"
#include "Variant/PgMissionMutator.h"

#define MISSION_SCOREUP_MAX		100
#define MISSION_TOTALSCORE_MAX	120

#define MPOINT_RESULT_MAX		999

#define MPOINT_VALUE_PCROOM		5
#define MPOINT_VALUE_EVENT		10
#define MPOINT_VALUE_MISSION	10

#pragma pack(1)

typedef enum eMissionCountMax
{
	MISSION_RANK		= 9,
	MISSION_COMBO		= 5,
	MISSION_DOWN		= 7,
	MISSION_NORMALSCORE	= 100,	// �⺻����
	MISSION_COMBO_BONUS = 20, 
	MISSION_SCORE		= 4,
	MISSION_BONUSSCORE  = 30,
	MISSION_SCORE_PERSENT = 1000,
	MISSION_SCORE_CALC  = 10,
	MISSION_TOTAL_UP	= 5,	// 5% ������ ����Ǵ� �ۼ�Ʈ(%)
	MISSION_OVERHIT		= 60,	// 60% �ʰ��� ����
	MISSION_OVERHITLEVEL = 10,	// 10���� �̻��϶��� ������
	MISSION_STYLELEVEL = 10,	// 10���� �̻��϶��� ������
}EMissionCountMax;

typedef enum eMissionScoreType
{
	EMission_ATTACK			= 1,
	EMission_COMBO			= 2,
	EMission_DEMAGE			= 3,
	EMission_TIMESEC		= 4,
	EMission_TIME			= 5,
	EMission_OVERHIT		= 6,
	EMission_BACKATTACK		= 7,
	EMission_COUNTER		= 8,
	EMission_DIE			= 9,
	EMission_STYLE			= 10,
}EMissionScoreType;

typedef enum eMissionUpdateType
{
	EMission_Update_Sense	= 1,
	EMission_Update_Ability	= 2,
	EMission_Update_Total	= 3,
	EMission_Update_Ani		= 4,
}EMissionUpdateType;

int const ms_aiResultRank[MISSION_RANK] = {115, 110, 90, 70, 60, 50, 40, 30, 0};// �̼� ���
int const ms_TotalScore_TotalUpPersent = ((MISSION_TOTALSCORE_MAX * MISSION_TOTAL_UP)/100);	// Sense ������ ä������ %
int const ms_aiDownValue[MISSION_DOWN] = {0, 2, 3, 4, 5, 6, 8};
int const ms_iRewardItemMax = 4;	// ���� ������ �ִ� ����(�հ�)

typedef enum eMissionPoint
{	
	MPOINT_MONSTER	= 0,//�������
	MPOINT_COMBO	= 1,//�޺�����	
	MPOINT_ATTACK	= 2,//����Ÿ�ݼ�
	MPOINT_DEMAGE	= 3,//�ǰ�����
	MPOINT_DIE		= 4,//��Ȱ����	
	MPOINT_BONUSEXP = 5,//�߰�EXP����
	MPOINT_TIME		= 6,//�ð�
	MPOINT_TOTALSCORE = 7,
	MPOINT_MAX,
}EMissionPoint;

typedef enum eMissionBonusPoint
{
	MBONUSPOINT_RANK		= 0,
	MBONUSPOINT_REMAINDER	= 1,
	MBONUSPOINT_PCROOM		= 2,
	MBONUSPOINT_PARTY		= 3,
	MBONUSPOINT_EVENT		= 4,
	MBONUSPOINT_AVATA		= 5,
	MBONUSPOINT_MEMBER		= 6,
	MBONUSPOINT_COUNT		= 7,
	MBONUSPOINT_PET			= 8,
	MBONUSPOINT_MAX,
}EMissionBonusPoint;

enum eMissionBonusType
{
	MBONUS_NONE		= 0x00,
	MBONUS_PARTY	= 0x01,	//��Ƽ
	MBONUS_PCROOM	= 0x02,	//PC��
	MBONUS_EVENT	= 0x04,	//�̺�Ʈ
	MBONUS_PREMIUM	= 0x08,	//�����̾�
	MBONUS_MISSION1	= 0x10,	//�̼�1
	MBONUS_MISSION2	= 0x20,	//�̼�2
	MBONUS_MISSION3 = 0x40,	//�̼�3
};
typedef WORD EMissionBonusType;

enum eMissionRank
{
	MRANK_NONE	= 0,
	MRANK_SSS	= 1,
	MRANK_SS	= 2,
	MRANK_S		= 3,
	MRANK_A		= 4,
	MRANK_B		= 5,
	MRANK_C		= 6,
	MRANK_D		= 7,
	MRANK_E		= 8,
	MRANK_F		= 9,
	MRANK_COUNT = 10,
};
typedef BYTE EMissionRank;

enum eDefenceMsg
{
	D_OK			= 0,
	D_STAGE_END		= 1,
	D_WAVE_END		= 2,
	D_FAIL			= 3
};
typedef BYTE EDefenceMsg;

enum eDefenceMonsterGen
{
	D_RANDOM		= 0,
	D_LEFT			= 1,
	D_RIGHT			= 2
};
typedef BYTE EDefenceMonsterGen;

typedef enum eDefence7MissionSlot
{
	MDEFENCE7_MISSION_SLOT1		= 0,
	MDEFENCE7_MISSION_SLOT2,
	MDEFENCE7_MISSION_SLOT3,
	MDEFENCE7_MISSION_SLOT4,
	MDEFENCE7_MISSION_SLOT5,
	MDEFENCE7_MISSION_SLOT6,
	MDEFENCE7_MISSION_SLOT7,
	MDEFENCE7_MISSION_SLOT8,
	MDEFENCE7_MISSION_SLOT_MAX,
}EDefence7MissionSlot;

typedef enum eMissionMapSelectType
{
	MMST_NONE = 0,
	MMST_AUTO = 1,
	MMST_SELECT = 2,
	MMST_AUTO_CHAOS = 3,
	MMST_SELECT_CHAOS = 4,
}EMissionMapSelectType;

typedef enum eGuardianType
{
	GT_STORM		= 0x0001,
	GT_BLAZE		= 0x0002,
	GT_GAIA			= 0x0004,
	GT_LIGHTNING	= 0x0008,
	GT_FREEZE		= 0x0010,
	GT_BLACKHOLE	= 0x0020,
	GT_ALLTYPE		= 0x0040,
}EGardianType;

typedef std::vector< PgBase_Item > RESULT_SITEM_BAG_LIST_CONT;

typedef struct tagMissionPlayerInfo_Client
{
	tagMissionPlayerInfo_Client()
	{
		Clear();
	}

	void Clear()
	{
		kCharGuid.Clear();
		iLevel = 0;
		kClass = 0;
		wstrName.clear();
		iAccExp = 0;
		kRank = MRANK_NONE;
		kContItem.clear();
		iResultBagNo = 0;
		iScore = 0;
	}

	size_t min_size()const
	{
		return 
			sizeof(kCharGuid)+
			sizeof(iLevel)+
			sizeof(kClass)+
			sizeof(size_t)+//wstrName);
			sizeof(iAccExp)+
			sizeof(kRank)+
			sizeof(size_t)+//PU::TLoadArray_M(rkPacket, kContItem);
			sizeof(iResultBagNo)+
			sizeof(iScore);
	}

	void WriteToPacket(BM::Stream& rkPacket, __int64 const iBonusExp=0 )const
	{
		rkPacket.Push(kCharGuid);
		rkPacket.Push(iLevel);
		rkPacket.Push(kClass);
		rkPacket.Push(wstrName);
		__int64 iTemp = iAccExp+iBonusExp;
		if( 0 > iTemp )
		{
			iTemp = 0;
		}
		rkPacket.Push((iTemp));
		rkPacket.Push(kRank);
		PU::TWriteArray_M(rkPacket, kContItem);
		rkPacket.Push(iResultBagNo);
		rkPacket.Push(iScore);
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(kCharGuid);
		rkPacket.Pop(iLevel);
		rkPacket.Pop(kClass);
		rkPacket.Pop(wstrName);
		rkPacket.Pop(iAccExp);

		rkPacket.Pop(kRank);
		PU::TLoadArray_M(rkPacket, kContItem);
		rkPacket.Pop(iResultBagNo);
		rkPacket.Pop(iScore);
	}

	BM::GUID					kCharGuid;
	int							iLevel;
	BYTE						kClass;
	std::wstring				wstrName;
	__int64						iAccExp;
	EMissionRank				kRank;
	RESULT_SITEM_BAG_LIST_CONT	kContItem;
	int							iResultBagNo;
	int							iScore;
}SMissionPlayerInfo_Client;

typedef struct tagMissionSense
{
	tagMissionSense()
	{
		Clear();
	}

	void Clear()
	{
		kAttackScore = 0;
		kComboScore = 0;
	}

	int GetSensePoint()const
	{
		return (kAttackScore + kComboScore);
	}

	int			kAttackScore;
	int			kComboScore;
}SMissionSense;

typedef struct tagMissionAbility
{
	tagMissionAbility()
	{
		Clear();
	}

	void Clear()
	{
		kOverHitScore = 0;
		kBackAttackScore = 0;
		kStyleScore = 0;
		kCounterScore = 0;
	}

	int GetAbilityPoint()const
	{
		return (kOverHitScore + kBackAttackScore + kStyleScore + kCounterScore);
	}

	int			kOverHitScore;
	int			kBackAttackScore;
	int			kStyleScore;
	int			kCounterScore;
}SMissionAbility;

typedef struct tagMissionPenalty
{
	tagMissionPenalty()
	{
		Clear();
	}

	void Clear()
	{
		kDemageScore = 0;
		kDieScore = 0;
	}

	int GetPenaltyPoint()const
	{
		return (kDemageScore + kDieScore);
	}

	int			kDemageScore;
	int			kDieScore;
}SMissionPenalty;

struct SMissionPlayerInfo
	:	public SMissionPlayerInfo_Client
{
	SMissionPlayerInfo()
	{
		Clear();
	}

	void Get(tagMissionPlayerInfo_Client &rhs)const
	{
		rhs.kCharGuid = kCharGuid;
		rhs.iLevel = iLevel;
		rhs.kClass = kClass;
		rhs.wstrName = wstrName;
		rhs.iAccExp = iAccExp;
		rhs.kRank = kRank;
		rhs.kContItem = kContItem;
	}

	void Clear()
	{
		SMissionPlayerInfo_Client::Clear();
		kBonusType = MBONUS_NONE;
		::memset(kPoint,0,sizeof(kPoint));
		::memset(kBonusPoint,0,sizeof(kBonusPoint));
		kDie = false;
		ResetComboCount();
		m_kSense.Clear();
		m_kAbility.Clear();
		m_kPenalty.Clear();
		kTotalScore = 0;
		kGadaCoinCount = 0;
		kGadaCoinNeedCount = 0;
		iRetContNo = 0;
		iType = 0;
		iNewRank = 0;
        iTeam = 0;
	}

	void ResetComboCount()
	{
		kComboCount = MISSION_COMBO_BONUS;
	}

	int GetTotalPoint()const
	{
		int iPoint = 0;		

		iPoint = kTotalScore;

		return iPoint;
	}

	void WriteToPacket(BM::Stream& rkPacket)const
	{
		SMissionPlayerInfo_Client::WriteToPacket(rkPacket);
		rkPacket.Push(kPoint,sizeof(kPoint));
		rkPacket.Push(kBonusPoint,sizeof(kBonusPoint));
		rkPacket.Push(kBonusType);
		
		rkPacket.Push(kTotalScore);
		rkPacket.Push(m_kSense);
		rkPacket.Push(m_kAbility);
		rkPacket.Push(m_kPenalty);
		rkPacket.Push(kGadaCoinCount);
		rkPacket.Push(kGadaCoinNeedCount);
		rkPacket.Push(iRetContNo);		
		rkPacket.Push(iType);
		rkPacket.Push(iNewRank);
	}

	void ReadFromPacket(BM::Stream& rkPacket)
	{
		SMissionPlayerInfo_Client::ReadFromPacket(rkPacket);
		rkPacket.PopMemory(kPoint,sizeof(kPoint));
		rkPacket.PopMemory(kBonusPoint,sizeof(kBonusPoint));
		rkPacket.Pop(kBonusType);
		
		rkPacket.Pop(kTotalScore);
		rkPacket.Pop(m_kSense);
		rkPacket.Pop(m_kAbility);
		rkPacket.Pop(m_kPenalty);
		rkPacket.Pop(kGadaCoinCount);
		rkPacket.Pop(kGadaCoinNeedCount);
		rkPacket.Pop(iRetContNo);	
		rkPacket.Pop(iType);
		rkPacket.Pop(iNewRank);
	}
	
	void CalcPoint()
	{
		for(int i=MPOINT_COMBO; i<MPOINT_MAX; i++)
		{
			if( kPoint[i] < 0 ) kPoint[i] = 0;
			if( kPoint[i] > MPOINT_RESULT_MAX ) kPoint[i] = MPOINT_RESULT_MAX;
		}
	}
	BYTE				kBonusType;
	int					kPoint[MPOINT_MAX];
	int					kBonusPoint[MBONUSPOINT_MAX];
	bool				kDie;
	int					kComboCount;

	int					kTotalScore;
	SMissionSense		m_kSense;
	SMissionAbility		m_kAbility;
	SMissionPenalty		m_kPenalty;
	size_t				kGadaCoinCount;
	size_t				kGadaCoinNeedCount;
	int					iRetContNo;
	int					iType;
	int					iNewRank;
    int                 iTeam;
};
#pragma pack()

typedef std::vector<SMissionPlayerInfo>			VEC_MS_PLAYER_INFO;
typedef std::vector<SMissionPlayerInfo_Client>	VEC_MS_PLAYER_INFO_CLIENT;


typedef struct tagMissionStageKey
{
	tagMissionStageKey()
	{
		Clear();
	}

	void Clear()
	{
		kBitFalg = 0x00;
		iGroundNo = 0;
	}

	bool operator==(const tagMissionStageKey& rhs)const
	{
		if( 0 == ::memcmp(this,&rhs,sizeof(tagMissionStageKey)) )
		{
			return true;
		}
		return false;
	}

	BYTE kBitFalg;
	int iGroundNo;
}SMissionStageKey;

typedef std::vector<SMissionStageKey>	ConStage;

class PgMission_Base
{
public:
	static std::wstring GetMissionRankName( EMissionRank const kRank );

public:
	PgMission_Base();
	virtual ~PgMission_Base();

	PgMission_Base(const PgMission_Base& rhs);
	PgMission_Base& operator=(const PgMission_Base& rhs);
	bool operator==(const PgMission_Base& rhs)const;
	bool operator!=(const PgMission_Base& rhs)const;

	virtual bool Clone(const PgMission_Base& rhs);
	virtual void Clear();
	bool Create(const TBL_DEF_MISSION_ROOT& rkMissionRoot,
				int const iLevel,
				const CONT_DEF_MISSION_CANDIDATE* pkMissionCandi=NULL,
				const CONT_DEF_MISSION_RESULT* pkMissionResult=NULL);

	int GetKey()const{ return m_kKey.iKey;	}
	int GetLevel()const {	return m_kKey.iLevel;	}
	SMissionKey const &GetMissionKey()const {	return m_kKey;	}
	int GetMissionNo()const	{	return m_iNo;	}
	size_t GetStageCount()const{	return m_iStageCount;	}
	size_t GetStageCountExceptBonus() const { return m_iStageCountExceptBonus; }
	size_t GetTotalStageCount()const	{	return m_kStage.size();	}
	int GetStageGroundNo( size_t const iAt )const
	{	
		if ( iAt < m_kStage.size() )
		{
			return m_kStage[iAt].iGroundNo;	
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	BYTE GetStageBitFlag( size_t const iAt )const
	{
		if ( iAt < m_kStage.size() )
		{
			return m_kStage[iAt].kBitFalg;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	const ConStage& GetStage()const{return m_kStage;}
	const ConStage::value_type& GetStage(size_t const iAt)const{	return m_kStage.at(iAt);	}
	int GetAbilRateBagNo()const{	return m_iAbilRateBagNo;	}

	void WriteToPacket(BM::Stream& rkPacket)const;
	void ReadFromPacket(BM::Stream& rkPacket);

	int GetModeType() const {return m_iModeType;}

protected:
	SMissionKey	m_kKey;
	int			m_iNo;
	int			m_iAbilRateBagNo;
	TBL_DEF_MISSION_RESULT m_kMissionResult;
	int			m_iCandidateNo;

	// ��������
	CLASS_DECLARATION(int, m_iLevelLimit_Min, LevelLimit_Min);
	CLASS_DECLARATION(int, m_iLevelLimit_Max, LevelLimit_Max);
	
	size_t		m_iStageCountExceptBonus;
	size_t		m_iStageCount;
	ConStage	m_kStage;

	// Type - Item = 0, Scenario = 1
	int			m_iModeType;
};

class PgMission
	:	public PgMission_Base
	,	public PgMissionMutator
{
public:
	// ������꿡 ���� ����
	static WORD const ms_kPointCalc = 10;
	static WORD const ms_kGradeValue[EMGRADE_MONSTERMAX];
	static int const ms_aiDownRank[MISSION_DOWN];

	int PgMission::ms_iAttackScore[MISSION_SCORE];
	int PgMission::ms_iComboScore[MISSION_SCORE];
	int PgMission::ms_iDemageScore[MISSION_SCORE];
	int PgMission::ms_iTimeSec[MISSION_SCORE];	
	int PgMission::ms_iTimeScore[MISSION_SCORE];	
	int PgMission::ms_iDieScore[MISSION_SCORE];
	int PgMission::ms_iOverHitScore;		
	int PgMission::ms_iCounterScore;
	int PgMission::ms_iStyleScore;
	int PgMission::ms_iBackAttack;
	int PgMission::ms_iDefenceChangeTargetRate;
	int PgMission::ms_iDefenceObjectAttackMonCount;
	int PgMission::ms_iDefence7ChangeTargetRate;
	int PgMission::ms_iDefence7ObjectAttackMonCount;

	typedef std::map<BM::GUID,SMissionPlayerInfo>	ConUser;

	PgMission();
	virtual ~PgMission();

	void Clear();
	bool CloneMission(PgMission* pkMission);

	BM::GUID const &GetID()const	{	return m_kGuidID;	}

	ConUser::mapped_type* GetMissionUser(BM::GUID const &rkCharGuid)
	{
		BM::CAutoMutex kLock(m_kMissionMutex);
		ConUser::iterator user_itr = m_kConUser.find(rkCharGuid);
		if ( user_itr != m_kConUser.end() )
		{
			return &(user_itr->second);
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	void RegistMissionUser(VEC_GUID& rkGuid)
	{
		VEC_GUID::iterator user_itr;
		for ( user_itr=rkGuid.begin(); user_itr!=rkGuid.end(); ++user_itr )
		{
			AddMissionUser(*user_itr);
		}
	}

	void AddMissionUser(BM::GUID const &rkCharGuid)
	{
		BM::CAutoMutex kLock(m_kMissionMutex);
		m_kConUser.insert(std::make_pair(rkCharGuid,ConUser::mapped_type()));
	}

	void AddMissionUser(BM::GUID const &rkCharGuid, ConUser::mapped_type& rkUserInfo )
	{
		BM::CAutoMutex kLock(m_kMissionMutex);
		auto kPair = m_kConUser.insert(std::make_pair(rkCharGuid,rkUserInfo));
		if ( !kPair.second )
		{
			kPair.first->second = rkUserInfo;
		}
	}

	HRESULT RestartUser(BM::GUID const &rkCharGuid);
	HRESULT IsAbleOutUser(BM::GUID const &rkCharGuid);
	HRESULT IsGadaCoinUse(BM::GUID const &rkCharGuid);
	void GadaCoinUseClear();

	HRESULT ReqNextStageUser(BM::GUID const &rkCharGuid);
	HRESULT IsNextStageUser();

	void SetTriggerType(int const iType){m_eTriggerType = iType;}
	int GetTriggerType(){return m_eTriggerType;}
	void SetOwner(BM::GUID const &rkCharGuid){m_kOwnerGuid = rkCharGuid;}
	void SetOwnerLevel(int const &iLv){m_iOwnerLv = iLv;}
	BM::GUID const &GetOwner()const{	return m_kOwnerGuid;	}
	int GetOwnerLevel()const{return m_iOwnerLv;}

	void GetGroundKey(SGroundKey& rkKey);
	size_t GetUserCount()const{	return m_kConUser.size();	}
	ConUser& GetUserList()	{	return m_kConUser;	}

	void WriteToPacket(BM::Stream& rkPacket)const;
	void ReadFromPacket(BM::Stream& rkPacket);
	void WriteToPacket_UserList(BM::Stream& rkPacket, bool bInfo=false)const;
	void ReadFromPacket_UserList(BM::Stream& rkPacket);

	void AddPlayTime(DWORD const dwPlayTime);
	DWORD GetPlayTime()const{return m_dwPlayTime;}
	int GetPlayTimePoint()const{return m_iPlayTimePoint/ms_kPointCalc;}

	void SetTimeMin(int iMin){m_iTimeMin = iMin;};
	void SetTimeSec(int iSec){m_iTimeSec = iSec;};

	void SetConstellationMission(Constellation::SConstellationMission const& constellationMission);
	Constellation::SConstellationMission const& GetConstellationMission()const { return m_kConstellationMission; }

protected:
	bool ChangeOwner(BM::GUID const &rkReqOwner);

	Loki::Mutex			m_kMissionMutex;
	BM::GUID			m_kGuidID;
	BM::GUID			m_kOwnerGuid;
	int					m_iOwnerLv;
	ConUser				m_kConUser;
	int					m_eTriggerType;

	DWORD				m_dwPlayTime;	// �̼� �÷��� �ð�
	int					m_iPlayTimePoint;
	int					m_iRegistGndNo;

	// Mission Time
	int					m_iTimeMin;
	int					m_iTimeSec;

	SET_GUID			m_kReqRestartUser;// �̼�������� ��û�� ����
	SET_GUID			m_kReqDefenceNextStageUser;// ���� �������� �̵��ϱ� ���� ����
	SET_GUID			m_kReqGadaCoinUse;			// �������� ����� ����
	Constellation::SConstellationMission m_kConstellationMission;
};

//
class PgMission_Pack
{
public:
	typedef std::vector<PgMission_Base*>				ConBase;

	PgMission_Pack()
	{
		m_kConBase.reserve(MAX_MISSION_LEVEL);
		Clear();
	}

	~PgMission_Pack(){}

	void Clear()
	{
		m_kConBase.resize(MAX_MISSION_LEVEL,NULL);
	}

	void Set(size_t const iLevel, PgMission_Base* pkBase=NULL)
	{
		if ( iLevel < MAX_MISSION_LEVEL )
		{
			m_kConBase[iLevel] = pkBase;
		}
	}

	PgMission_Base* Get(size_t const iLevel)
	{
		if ( iLevel >= MAX_MISSION_LEVEL )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
			return NULL;
		}
		return m_kConBase.at(iLevel);
	}

	size_t TotalMissionCount() const
	{
		size_t iCount = 0;
		ConBase::const_iterator iter = m_kConBase.begin();
		while(m_kConBase.end() != iter)
		{
			const ConBase::value_type pkElement = (*iter);
			if( pkElement )
			{
				iCount += pkElement->GetTotalStageCount();
			}
			++iter;
		}
		return iCount;
	}

	bool WriteToPacket_MissionInfo( BM::Stream& rkPacket, PgPlayer_MissionData const &kPlayerMissionData, int const iPlayerLevel );

protected:
	ConBase						m_kConBase;
};

//
class PgMissionContMgr
{
public:
	typedef std::map<SMissionKey, PgMission_Base*>		ConBase;
	typedef std::unordered_map<int,PgMission_Pack>		ConPack;	//MissionKey,

	PgMissionContMgr();
	virtual ~PgMissionContMgr();

	void Clear();
	virtual bool Build( CONT_DEF_MISSION_RESULT const &rkResult,
						CONT_DEF_MISSION_CANDIDATE const &rkCandi,
						CONT_DEF_MISSION_ROOT const &rkRoot); //���̺� �޾Ƽ� �̼� ���� ����

	virtual void swap(PgMissionContMgr& rkRight);
	virtual EDataCompareRet Compare(PgMissionContMgr& rkRight);

	void GetMissionBase(ConBase const *&pkBase) const { pkBase = &m_kConBase; }

protected:
	PgMission_Base* New(PgMission_Base*& pkBase)
	{	
		if ( !pkBase )
		{
			pkBase = m_kBasePool.New();
		}
		if ( pkBase )
		{
			pkBase->Clear();
		}
		return pkBase;
	}

	void Delete(PgMission_Base*& pkBase)
	{
		if ( pkBase )
		{
			m_kBasePool.Delete(pkBase);
		}
	}

	BM::TObjectPool<PgMission_Base>		m_kBasePool;

	ConBase			m_kConBase;
	ConPack			m_kConPack;
};

typedef struct tagMissionReport_Update
{
	tagMissionReport_Update(){}
	tagMissionReport_Update(bool bDoUpdate, VEC_GUID const &rkGuidList, SMissionKey const &rkKey, BYTE byBitFlag)
	{
		DoUpdate(bDoUpdate);
		m_kGuidList = rkGuidList;
		MissionKey(rkKey);
		StageBitFlag(byBitFlag);
	}

	CLASS_DECLARATION_S(bool, DoUpdate);
	CLASS_DECLARATION_S(VEC_GUID, GuidList);
	CLASS_DECLARATION_S(SMissionKey, MissionKey);
	CLASS_DECLARATION_S(BYTE, StageBitFlag);
	
	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(DoUpdate());
		PU::TWriteArray_A(kPacket, m_kGuidList);
		kPacket.Push(MissionKey());
		kPacket.Push(StageBitFlag());
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(m_kDoUpdate);
		PU::TLoadArray_A(kPacket, m_kGuidList);
		kPacket.Pop(m_kMissionKey);
		kPacket.Pop(m_kStageBitFlag);
	}

} MissionReport_Update;

typedef struct tagPlayerModifyOrderData_AddMissionEvent //IMET_END_MISSION_EVENT
{
	tagPlayerModifyOrderData_AddMissionEvent(){}
	tagPlayerModifyOrderData_AddMissionEvent(int const eType, int const iValue)
	{
		Type(eType);
		Value(iValue);
	}
	CLASS_DECLARATION_S(int, Value);
	CLASS_DECLARATION_S(int, Type);
	DEFAULT_TBL_PACKET_FUNC();
}SPMOD_AddMissionEvent;

#endif // WEAPON_VARIANT_BASIC_MISSION_PGMISSION_H