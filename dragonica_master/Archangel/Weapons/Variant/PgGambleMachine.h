#ifndef WEAPON_VARIANT_ITEM_PGGAMBLEMACHINE_H
#define WEAPON_VARIANT_ITEM_PGGAMBLEMACHINE_H

#include "Lohengrin/packetstruct.h"
#include "BM/twrapper.h"
#include "constant.h"

typedef enum : BYTE
{
	GCG_NONE	= 0,
	GCG_GOLD	= 1,
	GCG_SILVER	= 2,
	GCG_BRONZE	= 3,
	GCG_CASHSHOP = 4,// 캐시샵에서 사용하는 겜블 코인
	
	// 코스튬 조합
	GCG_COSTUMEMIX_NORMAL = 11,	// 노멀
	GCG_COSTUMEMIX_ADV	  = 12,	// 고급
	GCG_COSTUMEMIX_SPCL	  = 13,	// 스페셜
	GCG_COSTUMEMIX_ARTIFACT	= 14,	// 유물
	GCG_COSTUMEMIX_LEGND  	= 15,	// 레전드
}eGambleCoinGrade;

typedef struct tagGAMBLEMACHINEMIXUPELEMENT
{
	tagGAMBLEMACHINEMIXUPELEMENT():iGradeNo(0),iRate(0){}
	int iGradeNo;
	int iRate;
}SGAMBLEMACHINEMIXUPELEMENT;

typedef std::vector<SGAMBLEMACHINEMIXUPELEMENT> CONT_GAMEBLEMACHINEMIXUPLELEMENT;

typedef std::map<int,int> CONT_GAMBLEMACHINEMIXUPPOINT;

typedef struct tagGAMBLEMACHINEMIXUP
{
	tagGAMBLEMACHINEMIXUP():iMixPoint(0){}

	int iMixPoint;
	CONT_GAMEBLEMACHINEMIXUPLELEMENT kCont;

	size_t min_size() const
	{
		return sizeof(int) + sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iMixPoint);
		PU::TWriteArray_A(kPacket, kCont);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iMixPoint);
		PU::TLoadArray_A(kPacket, kCont);
	}
}SGAMBLEMACHINEMIXUP;

typedef std::map<int,SGAMBLEMACHINEMIXUP> CONT_GAMBLEMACHINEMIXUP;

typedef struct tagGAMBLEMACHINEITEM
{
	tagGAMBLEMACHINEITEM():
	iItemNo(0),
	siCount(0),
	iRate(0),
	bTimeType(0),
	siUseTime(0),
	bBroadcast(false),
	iGroupNo(0){}

	int iItemNo;
	short siCount;
	int iRate;
	BYTE bTimeType;
	short siUseTime;
	bool bBroadcast;
	int iGroupNo;
}SGAMBLEMACHINEITEM;

typedef struct tagGAMBLEMACHINEGROUPRES
{
	int iGroupNo;
	BYTE bU,bV;
	short siUVIndex;
	std::wstring kTitle;
	std::wstring kDescription;
	std::wstring kIconPath;
	int	iDisplayRank;

	tagGAMBLEMACHINEGROUPRES():iGroupNo(0),bU(0),bV(0),siUVIndex(0),iDisplayRank(0){}

	size_t min_size() const
	{
		return sizeof(int) + sizeof(BYTE) + sizeof(BYTE) + sizeof(short) + sizeof(size_t) + sizeof(size_t) + sizeof(int);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iGroupNo);
		kPacket.Push(bU);
		kPacket.Push(bV);
		kPacket.Push(siUVIndex);
		kPacket.Push(kTitle);
		kPacket.Push(kDescription);
		kPacket.Push(kIconPath);
		kPacket.Push(iDisplayRank);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iGroupNo);
		kPacket.Pop(bU);
		kPacket.Pop(bV);
		kPacket.Pop(siUVIndex);
		kPacket.Pop(kTitle);
		kPacket.Pop(kDescription);
		kPacket.Pop(kIconPath);
		kPacket.Pop(iDisplayRank);
	}

}SGAMBLEMACHINEGROUPRES;

typedef std::vector<SGAMBLEMACHINEITEM> CONT_GAMBLEITEM;

typedef struct tagGAMBLEITEMBAG
{
	tagGAMBLEITEMBAG():iTotalRate(0){}
	int iTotalRate;
	CONT_GAMBLEITEM	kCont;

	void Clear()
	{
		kCont.clear();
		iTotalRate = 0;
	}

	size_t min_size() const
	{
		return sizeof(int) + sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteArray_A(kPacket,kCont);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		Clear();
		PU::TLoadArray_A(kPacket,kCont);
		for(CONT_GAMBLEITEM::const_iterator iter = kCont.begin();iter != kCont.end();++iter)
		{
			iTotalRate += (*iter).iRate;
		}
	}
}SGAMBLEITEMBAG;

typedef std::map<eGambleCoinGrade,SGAMBLEITEMBAG>	CONT_GAMBLEMACHINE;
typedef std::map<BYTE,int>							CONT_GAMBLEMACHINECOST;
typedef std::vector<PgBase_Item>					CONT_GAMBLEMACHINERESULT;
typedef std::map<int,SGAMBLEMACHINEGROUPRES>		CONT_GAMBLEMACHINEGROUPRES;

class PgGambleMachineImpl
{
//신종족 전용의 아이템 컨테이너를 따로 관리하기 위해 분리
//현재에도 컨테이너가 매우 많이 중첩된 구조라서 더 이상 중첩시키지 않고 별도 컨테이너로 선언.
	CONT_GAMBLEMACHINE	m_kCont, m_kCont1;
	CONT_GAMBLEMACHINECOST m_kContCost;
	CONT_GAMBLEMACHINEGROUPRES m_kContGroup;
	CONT_GAMBLEMACHINEMIXUP m_kContMixup;
	CONT_GAMBLEMACHINEMIXUPPOINT m_kContMixupPoint;

public:

	PgGambleMachineImpl();
	~PgGambleMachineImpl();

public:

	void WriteToPacket(BM::Stream & kPacket);
	void ReadFromPacket(BM::Stream & kPacket);
	HRESULT GetGambleResult(eGambleCoinGrade const kGcg, CONT_GAMBLEMACHINERESULT & kContResult, ERACE_TYPE eRaceType, bool & bBroadcast, SGAMBLEMACHINEITEM & kGambleItem = SGAMBLEMACHINEITEM());
	HRESULT GetGambleRoulette(eGambleCoinGrade const kGcg, CONT_GAMBLEMACHINERESULT & kContResult, CONT_GAMBLEITEM & kContDumy, ERACE_TYPE eRaceType, bool & bBroadcast);
	HRESULT GetMixupRoulette(int const iMixPoint, CONT_GAMBLEMACHINERESULT & kContResult, CONT_GAMBLEITEM & kContDumy, ERACE_TYPE eRaceType, bool & bBroadcast);
	bool GetGambleMachineCost(eGambleCoinGrade const kGcg,int & iCost);
	CONT_GAMBLEMACHINE* GetCont(ERACE_TYPE eRaceType)
	{ return (eRaceType == ERACE_DRAGON) ?  &m_kCont1 : &m_kCont; }
	void GetCont(CONT_GAMBLEMACHINE & kCont, ERACE_TYPE eRaceType){ kCont = *GetCont(eRaceType); }
	void GetContRes(CONT_GAMBLEMACHINEGROUPRES & kCont){kCont = m_kContGroup;}
	void GetContMixup(CONT_GAMBLEMACHINEMIXUP & kCont){kCont = m_kContMixup;}
	void GetContMixupPoint(CONT_GAMBLEMACHINEMIXUPPOINT & kCont){kCont = m_kContMixupPoint;}
	int GetMixupPoint(int const iGrade) const;
	int	GetMaxMixupPoint() const;
	bool CheckEnableMixupPoint(int const iMixPoint) const;
	bool MakeEnablePoint(int const iMixPoint,int & iEnablePoint) const;
	bool IsCanCostumeMixup(ERACE_TYPE eRaceType);

private:

	HRESULT MakeGambleResult(SGAMBLEMACHINEITEM const & kResultItem, CONT_GAMBLEMACHINERESULT & kContResult);
};

class PgGambleMachine : public TWrapper<PgGambleMachineImpl>
{
public:
	PgGambleMachine(){}
	~PgGambleMachine(){}
public:
	void WriteToPacket(BM::Stream & kPacket);
	void ReadFromPacket(BM::Stream & kPacket);
	HRESULT GetGambleResult(eGambleCoinGrade const kGcg, CONT_GAMBLEMACHINERESULT & kContResult, ERACE_TYPE eRaceType, bool & bBroadcast);
	HRESULT GetGambleRoulette(eGambleCoinGrade const kGcg, CONT_GAMBLEMACHINERESULT & kContResult, CONT_GAMBLEITEM & kContDumy, ERACE_TYPE eRaceType, bool & bBroadcast);
	HRESULT GetMixupRoulette(int const iMixPoint, CONT_GAMBLEMACHINERESULT & kContResult, CONT_GAMBLEITEM & kContDumy, ERACE_TYPE eRaceType, bool & bBroadcast);
	bool GetGambleMachineCost(eGambleCoinGrade const kGcg,int & iCost);
	void GetCont(CONT_GAMBLEMACHINE & kCont, ERACE_TYPE eRaceType);
	void GetContRes(CONT_GAMBLEMACHINEGROUPRES & kCont);
	void GetContMixup(CONT_GAMBLEMACHINEMIXUP & kCont);
	void GetContMixupPoint(CONT_GAMBLEMACHINEMIXUPPOINT & kCont);
	int GetMixupPoint(int const iGrade);
	bool CheckEnableMixupPoint(int const iMixPoint);
	int	GetMaxMixupPoint() const;
	bool MakeEnablePoint(int const iMixPoint,int & iEnablePoint);
	bool IsCanCostumeMixup(ERACE_TYPE eRaceType);
};

#define g_kGambleMachine SINGLETON_STATIC(PgGambleMachine)

#endif // WEAPON_VARIANT_ITEM_PGGAMBLEMACHINE_H