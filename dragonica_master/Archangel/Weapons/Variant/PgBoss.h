#ifndef WEAPON_VARIANT_UNIT_PGBOSS_H
#define WEAPON_VARIANT_UNIT_PGBOSS_H

#include "PgMonster.h"

#define MAX_SP_ATTACK_NUM			10

typedef struct tagBOSS_ABIL
{
	tagBOSS_ABIL()
	{
		iAiDelay =0;
		iCurrentHPGage =0;
		iCurrentAction =0;
		iAppear =0;
	}
	int iAiDelay;			// AI Delay				AT_AI_DELAY
	int iCurrentHPGage;		// HP 게이지 바			AT_C_HP_GAGE
	int iCurrentAction;		// 진행중인 Action		AT_CURRENT_ACTION
	int iAppear;			// 보스의 리젠상태등...
}BOSS_ABIL;

class PgBoss : public PgMonster
{
public:
	PgBoss(void);
	virtual ~PgBoss(void);

public:
//	virtual HRESULT Create(void* pkInfo);
	virtual int GetAbil(WORD const Type) const;
	virtual bool SetAbil(WORD const Type, int const iValue, bool const bIsSend = false, bool const bBroadcast = false);
	//virtual bool IsAttackable(EAbilType eType, CUnit *pkTarget,int& iSkillNo);

	virtual void Init();
	bool SetSPData(int iType , int iValue);
	int GetSPData(int iType);

	virtual void VOnDie(){CUnit::VOnDie();}
protected :
	virtual EUnitType UnitType()const{ return UT_BOSSMONSTER; }
private: // Boss 
	BOSS_ABIL m_kInfo;
	int m_iSPData[MAX_SP_ATTACK_NUM];
};

#endif // WEAPON_VARIANT_UNIT_PGBOSS_H