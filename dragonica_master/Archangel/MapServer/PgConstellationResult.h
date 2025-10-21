#ifndef MAP_MAPSERVER_MAP_GROUND_PGCONSTELLATIONRESULT_H
#define MAP_MAPSERVER_MAP_GROUND_PGCONSTELLATIONRESULT_H

class PgIndun;

const size_t MAX_REWARD_CHOICE = 9;

class PgConstellationResult
{
public:
	enum EREWARD_STEP
	{
		ER_NONE = 0,
		ER_STAND_BY,
		ER_STEP1,
		ER_STEP2,
		ER_STEP3,
	};

protected:
	typedef struct tagRewardItemInfo
	{
		enum EPLAYER_REWARD_STEP
		{
			EPRS_READY = 0,
			EPRS_BEGIN,
			EPRS_PROCESS,
			EPRS_COMPLETE,
		};

		int iRandRewardItemNo;
		int iRewardItemNo;
		int iAddChoiceRewardItemNo[MAX_REWARD_CHOICE];
		size_t iAddChoiceRewardItemCount[MAX_REWARD_CHOICE];
		int iAddChoiceRewardItemIndex[MAX_REWARD_CHOICE];
		int iAddChoiceRewardBagNo;
		EPLAYER_REWARD_STEP eStep;

		tagRewardItemInfo() : iRandRewardItemNo(0), iRewardItemNo(0), iAddChoiceRewardBagNo(0), eStep(EPRS_READY)
		{
			for( int index = 0; index < MAX_REWARD_CHOICE; ++index )
			{
				iAddChoiceRewardItemNo[index] = 0;
				iAddChoiceRewardItemCount[index] = 0;
				iAddChoiceRewardItemIndex[index] = 0;
			}
		}

	}SRewardItemInfo;
	typedef std::map<PgPlayer*, SRewardItemInfo> CONT_REWARD_INFO;
	CONT_REWARD_INFO m_kContRewardInfo;

	EREWARD_STEP m_eRewardStep;
	size_t m_iEnteringResultTime;
	static const size_t m_iInitEnteringResultTime;

	mutable Loki::Mutex m_kMutex;

public:
	PgConstellationResult() : m_eRewardStep(ER_STEP1)
	{}
	~PgConstellationResult() {}

	void OnInit(PgIndun* pkGround);
	void State(EREWARD_STEP eStep, PgIndun* pkGround);
	void SetDelayTime(size_t iSec);
	void OnTick1s(PgIndun* pkGround, bool const IsLastStage);
	void Recv_PT_C_M_REQ_CONSTELLATION_CASH_REWARD(PgPlayer* pkReqPlayer, PgIndun* pkGround);
	void Recv_PT_C_M_REQ_CONSTELLATION_CASH_REWARD_COMPLETE(PgPlayer* pkReqPlayer, PgIndun* pkGround);
	void OnSendMapLoadComplete(PgPlayer *pkUser);
	void OnReleaseUnit(CUnit* pkUnit);

protected:
	int GetCurChoiceRewardIndex(PgPlayer* pkReqPlayer, SRewardItemInfo const* pkRewardItemInfo = NULL) const;
	void SendFirstReward(PgIndun* pkGround);
	void SendSecondaryReward(PgPlayer* pkReqPlayer, PgIndun* pkGround);
	bool ProcessFirstReward(PgIndun* pkGround);
	bool ProcessSecondaryReward(PgPlayer* pkReqPlayer, PgIndun* pkGround);
	bool IsComplete(void);

};

#endif //MAP_MAPSERVER_MAP_GROUND_PGCONSTELLATIONRESULT_H
