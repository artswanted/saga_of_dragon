#ifndef WEAPON_VARIANT_BASIC_MISSION_PGMISSION_REPORT_H
#define WEAPON_VARIANT_BASIC_MISSION_PGMISSION_REPORT_H

#include "PgMission.h"

class PgMission_Report
{
public:
	PgMission_Report()
	{}
	PgMission_Report(TBL_MISSION_REPORT const& rkReport)
	{
		Init(rkReport);
	}
	~PgMission_Report(){}

	void Init(TBL_MISSION_REPORT const& rkReport)
	{
		m_kReport = rkReport;
	}

	bool operator<(PgMission_Report const& rhs)const
	{
		return m_kReport < rhs.m_kReport;
	}
	bool operator>(PgMission_Report const& rhs)const
	{
		return m_kReport > rhs.m_kReport;
	}
	bool operator==(PgMission_Report const& rhs)const
	{
		return m_kReport==rhs.m_kReport;
	}

	// 리턴값은 TopTime이 갱신되었느냐?
	bool Update(int const iPlayTime)
	{
		// 평균시간 계산
		++m_kReport.i64ClearCount;
		m_kReport.i64ClearTime_Total += (__int64)iPlayTime;
		if ( m_kReport.i64ClearTime_Total < 0 )
		{
			// OverFlow
			unsigned __int64 ui64Temp = (unsigned __int64)m_kReport.i64ClearTime_Total;
			ui64Temp /= (unsigned __int64)m_kReport.i64ClearCount;
			m_kReport.iClearTime_Avg = (int)ui64Temp;
			m_kReport.i64ClearTime_Total = (__int64)ui64Temp;
			m_kReport.i64ClearCount = 1;
		}
		else
		{
			m_kReport.iClearTime_Avg = (int)(m_kReport.i64ClearTime_Total/m_kReport.i64ClearCount);
		}
		
		if ( !m_kReport.iClearTime_Top || m_kReport.iClearTime_Top > iPlayTime )
		{
			m_kReport.iClearTime_Top = iPlayTime;
			return true;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	virtual void WirteToPacket(BM::Stream& rkPacket)const
	{
		m_kReport.WriteToPacket(rkPacket);
	}
	virtual void ReadFromPacket(BM::Stream& rkPacket)
	{
		m_kReport.ReadFromPacket(rkPacket);
	}

	int GetClearTime_Top()const{	return m_kReport.iClearTime_Top;	}
	int GetClearTime_Avg()const{	return m_kReport.iClearTime_Avg;	}
	__int64 GetClearCount()const{	return m_kReport.i64ClearCount;	}

	TBL_MISSION_REPORT const& GetReport()const{	return m_kReport;	}

protected:
	TBL_MISSION_REPORT	m_kReport;
};

typedef struct tagMissionRankKey
{
	tagMissionRankKey(int const _iPoint=0, int const _iPlayTime=0)
		:	iPoint(_iPoint)
		,	iPlayTime(_iPlayTime)
	{}
	tagMissionRankKey(const TBL_MISSION_RANK& rhs)
		:	iPlayTime(rhs.iPlayTime)
		,	iPoint(rhs.iPoint)
	{}

	void Clear()
	{
		iPoint = 0;
		iPlayTime = 0;
	}

	bool operator<(const tagMissionRankKey& rhs)const
	{
		if ( iPoint == rhs.iPoint )
		{
			return iPlayTime < rhs.iPlayTime;
		}
		return iPoint > rhs.iPoint;
	}

	bool operator==(const tagMissionRankKey& rhs)const
	{
		return (iPlayTime==rhs.iPlayTime) && (iPoint==rhs.iPoint);
	}

	int iPlayTime;
	int iPoint;
}SMissionRankKey;


struct SPlayerRankInfo
	:	public SMissionPlayerInfo_Client
{
	SPlayerRankInfo()
	{
		Clear();
	}

	SPlayerRankInfo(const SMissionPlayerInfo& rhs, int const iCommonPoint, int const iPlayTime)
		:	kRankKey(rhs.GetTotalPoint()+iCommonPoint,iPlayTime)
		,	iStanding(0)
	{
		kCharGuid = rhs.kCharGuid;
		iLevel = rhs.iLevel;
		kClass = rhs.kClass;
		wstrName = rhs.wstrName;
		iAccExp = rhs.iAccExp;
		kRank = rhs.kRank;
		kContItem = rhs.kContItem;
		kMemoGuid.Clear();
		::memcpy(kPoint,rhs.kPoint,sizeof(kPoint));
		::memcpy(kBonusPoint,rhs.kBonusPoint,sizeof(kBonusPoint));
		kTotalScore = rhs.kTotalScore;
		::memcpy(&m_kSense, &rhs.m_kSense, sizeof(m_kSense));
		::memcpy(&m_kAbility, &rhs.m_kAbility, sizeof(m_kAbility));
		::memcpy(&m_kPenalty, &rhs.m_kPenalty, sizeof(m_kPenalty));
		kGadaCoinCount = rhs.kGadaCoinCount;
		kGadaCoinNeedCount = rhs.kGadaCoinNeedCount;
		iRetContNo = rhs.iRetContNo;
		iType = rhs.iType;
		iNewRank = rhs.iNewRank;
	}

	~SPlayerRankInfo(){}

	bool operator<(const SPlayerRankInfo& rhs)const
	{
		return kRankKey < rhs.kRankKey;
	}

	void WriteToPacket(BM::Stream& rkPacket, __int64 const iBonusExp=0) const
	{
		SMissionPlayerInfo_Client::WriteToPacket(rkPacket, iBonusExp);
		rkPacket.Push(kPoint, sizeof(int)*MPOINT_MAX);
		rkPacket.Push(kBonusPoint, sizeof(int)*MBONUSPOINT_MAX);
		rkPacket.Push(kRankKey);
		rkPacket.Push(iStanding);
		rkPacket.Push(kMemoGuid);

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
		rkPacket.PopMemory(kPoint, sizeof(int)*MPOINT_MAX);
		rkPacket.PopMemory(kBonusPoint, sizeof(int)*MBONUSPOINT_MAX);
		rkPacket.Pop(kRankKey);
		rkPacket.Pop(iStanding);
		rkPacket.Pop(kMemoGuid);

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

	int				kPoint[MPOINT_MAX];
	int				kBonusPoint[MBONUSPOINT_MAX];
	SMissionRankKey	kRankKey;
	WORD			iStanding;
	BM::GUID		kMemoGuid;

	int					kTotalScore;
	SMissionSense		m_kSense;
	SMissionAbility		m_kAbility;
	SMissionPenalty		m_kPenalty;
	size_t				kGadaCoinCount;
	size_t				kGadaCoinNeedCount;
	int					iRetContNo;
	int					iType;
	int					iNewRank;
};

typedef std::multiset<SPlayerRankInfo>	ConPlayRankInfo;

class PgMission_RankItem
{
public:
	PgMission_RankItem()
		:	m_iStanding(0)
	{}

	PgMission_RankItem(const TBL_MISSION_RANK& rhs,WORD const iStanding=1)
		:	m_kData(rhs)
		,	m_iStanding(iStanding)
	{
		m_kUniqueID.Generate();
	}

	PgMission_RankItem(SPlayerRankInfo const& rPlayerRankInfo)
	{
		Set(rPlayerRankInfo);
	}

	void Set(SPlayerRankInfo const& rPlayerRankInfo)
	{
		m_kData.iPlayTime = rPlayerRankInfo.kRankKey.iPlayTime;
		m_kData.iPoint = rPlayerRankInfo.kRankKey.iPoint;
		m_kData.iUserLevel = rPlayerRankInfo.iLevel;
		m_kData.kClass = rPlayerRankInfo.kClass;
		m_kData.kCharGuid = rPlayerRankInfo.kCharGuid;
		m_kData.wstrName = rPlayerRankInfo.wstrName;
		m_iStanding = rPlayerRankInfo.iStanding;
		if ( rPlayerRankInfo.kMemoGuid == BM::GUID::NullData() )
		{
			m_kUniqueID.Generate();
		}
		else
		{
			m_kUniqueID = rPlayerRankInfo.kMemoGuid;
		}
	}

	void RegistTime(SYSTEMTIME const& rkTime)
	{
		m_kData.dtRegistTime = rkTime;
	}

	void Clear()
	{
		m_kData.Clear();
		m_kUniqueID.Clear();
	}

	~PgMission_RankItem(){}

	bool operator<(const PgMission_RankItem& rhs)const
	{
		return m_kData < rhs.m_kData;
	}
	bool operator>(const PgMission_RankItem& rhs)const
	{
		return m_kData > rhs.m_kData;
	}
	bool operator==(const PgMission_RankItem& rhs)const
	{
		return m_kData == rhs.m_kData;
	}

	void WriteToPacket(BM::Stream& rkPacket)const
	{
		m_kData.WriteToPacket(rkPacket);
	}

	void ReadFromPacket(BM::Stream &rkPacket)
	{
		m_kData.ReadFromPacket(rkPacket);
	}

	bool UpdateMemo(BM::GUID const &rkUniqueID, std::wstring const &wstrMemo)
	{
		if( m_kUniqueID != rkUniqueID )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		m_kData.wstrMemo = wstrMemo;
		return true;
	}
	void AddRank(){++m_iStanding;}

	TBL_MISSION_RANK const &GetRank()const{return m_kData;}
	std::wstring const &GetMemo()const{return m_kData.wstrMemo;}
	BM::GUID const &GetID()const{return m_kUniqueID;}
	void SetID(BM::GUID const &rkID) { m_kUniqueID = rkID; }

	CLASS_DECLARATION(WORD,m_iStanding,Standing);
protected:
	TBL_MISSION_RANK	m_kData;
	BM::GUID			m_kUniqueID;
};

#endif // WEAPON_VARIANT_BASIC_MISSION_PGMISSION_REPORT_H