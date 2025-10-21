#ifndef WEAPON_VARIANT_BASIC_MISSION_PGMISSION_RESULT_H
#define WEAPON_VARIANT_BASIC_MISSION_PGMISSION_RESULT_H

#include "PgMission_Report.h"

class PgMission_Result
{
public:
	enum eViewType
	{
		VIEWTYPE_NONE			= 0x00,
		VIEWTYPE_RESULT			= 0x01,
		VIEWTYPE_RANK			= 0x02,
		VIEWTYPE_DEFENCE		= 0x04,
		VIEWTYPE_DEFENCE7		= 0x08,
		VIEWTYPE_DEFENCE8		= 0x10,
	};

	PgMission_Result();
	virtual ~PgMission_Result();

	virtual void Clear();
	virtual void WriteToPacket(BM::Stream& rkPacket)const;
	virtual void ReadFromPacket(BM::Stream& rkPacket);

	CONT_MISSION_RANK& GetRankData(){return m_kConRankData;}
	CONT_MISSION_RANK::const_iterator GetFirstElementFromRankData() const{ return m_kConRankData.begin(); }
	bool const IsRankEnd(CONT_MISSION_RANK::const_iterator kit) const{return m_kConRankData.end() == kit;}
	size_t const GetRankCount() const { return m_kConRankData.size(); }

protected:
	CONT_MISSION_RANK	m_kConRankData;
};

//Client용
class PgMission_ResultC
	:	public PgMission_Result
{
public:
	PgMission_ResultC();
	virtual ~PgMission_ResultC();

	virtual void Clear();
		
	void ReadFromPacket(BM::Stream& rkPacket);

	bool IsRecvGameDefence()	{	return (m_kView>=VIEWTYPE_DEFENCE);	}
	bool IsRecvDefence8()	{	return 0!=(m_kView&VIEWTYPE_DEFENCE8);	}
	bool IsRecvDefence7()	{	return 0!=(m_kView&VIEWTYPE_DEFENCE7);	}
	bool IsRecvDefence()	{	return 0!=(m_kView&VIEWTYPE_DEFENCE);	}
	bool IsRecvResult()	{	return 0!=(m_kView&VIEWTYPE_RESULT);	}
	bool IsRecvRank()	{	return 0!=(m_kView&VIEWTYPE_RANK);}//이미 랭크데이터를 받았다!
	bool IsSendMemo()	{	return m_kMemoGuid != BM::GUID::NullData();	}//10위안에 들어서 메모를 보내야 하느냐?
	BM::GUID const &GetSendMemoID()const{	return m_kMemoGuid;	}//메모를 보낼때 이걸먼저보내고, wstring를 보내야함
	size_t GetUserCount()const{	return m_kConUserInfo.size();	}
	VEC_MS_PLAYER_INFO_CLIENT::const_iterator GetFirstElementFromUserInfo() const { return m_kConUserInfo.begin(); }
	bool const GetInfoFromUserInfoAt(size_t const iAt, SMissionPlayerInfo_Client& rkInfo)const;
	int const GetPoint(int const iAt) const;
	int	const GetBonusPoint(int const iAt) const;
	int MissionLevel()const{	return m_kMissionKey.iLevel;	}

	BM::GUID const &GetMemoGuid() const { return m_kMemoGuid; }

	int const GetSense()const;
	int const GetAbility()const;
	int const GetPenalty()const;
	int const GetGadaCoin()const;
	int const GetRetContNo()const;
	int const GetTotalScore()const;
	int const GetMissionType()const;
	int const GetMissionPoint()const;
	int const GetGadaCoinNeedCount()const;
	int GetAddItemResultNo() const { return iAddItemResult_No; }

	CLASS_DECLARATION(int, m_iClearTime, ClearTime);
	CLASS_DECLARATION(int, m_iClearTime_Top, ClearTime_Top);
	CLASS_DECLARATION(int, m_iClearTime_Avg, ClearTime_Avg);
	CLASS_DECLARATION(int, m_iMissionNo, MissionNo);
	CLASS_DECLARATION(int, m_iStageCount, StageCount);
	CLASS_DECLARATION(SMissionKey, m_kMissionKey, MissionKey);
	CLASS_DECLARATION_S(BYTE,View);
	CLASS_DECLARATION(WORD,m_iStanding,Standing);// 나의 순위(0이면 순위에 들지 못한거다)
	CLASS_DECLARATION(int,m_iClearTimePoint,ClearTimePoint);//플레이 시간에 따른 점수
	CLASS_DECLARATION(bool,m_bOwner,IsOwner);//미션오너이냐?
	CLASS_DECLARATION(bool, m_bLastStage, IsLastStage);
	CLASS_DECLARATION(bool, m_bResultStage, IsResultStage);
	CLASS_DECLARATION(__int64, m_i64BonusExp, BonusExp);
	CLASS_DECLARATION(int, m_iSuccessCount, SuccessCount);
	CLASS_DECLARATION(bool, m_bPrevStage_UseSelectItem, UseSelectItem);
	CLASS_DECLARATION(int, m_iMonsterNo, MonsterNo);
	CLASS_DECLARATION(int, m_iItemNo, ItemNo);
	CLASS_DECLARATION(int, m_iDropRate, DropRate);
	CLASS_DECLARATION(bool, m_bConstellationMission, IsConstellationMission);

protected:
	VEC_MS_PLAYER_INFO_CLIENT	m_kConUserInfo;
	int							m_kPoint[MPOINT_MAX];	// 0:콤보점수, 1:피격점수, 2:시간점수, 3:추가EXP점수
	int							m_kBonusPoint[MBONUSPOINT_MAX];	// 0:파티점수, 1:PC방점수, 2:이벤트점수, 3:미션점수
	BM::GUID					m_kMemoGuid;			

	int							kTotalScore;
	SMissionSense				m_kSense;
	SMissionAbility				m_kAbility;
	SMissionPenalty				m_kPenalty;
	size_t						kGadaCoinCount;
	size_t						kGadaCoinNeedCount;
	int							iRetContNo;
	int							iType;
	int							iWinTeam;
	int							iWinItemNo;
	int							iAddItemResult_No;
	int							iPoint;
};

#endif // WEAPON_VARIANT_BASIC_MISSION_PGMISSION_RESULT_H