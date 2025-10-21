#ifndef WEAPON_VARIANT_ACTION_BATTLE_PGCOMBOCOUNTER_H
#define WEAPON_VARIANT_ACTION_BATTLE_PGCOMBOCOUNTER_H

float const g_fComboContinueTime = 3.f;
DWORD const g_dwComboContinueTime = 3 * 1000;

typedef unsigned int COMBO_TYPE;

enum eComboStyle
{
	ECOMBO_NORMAL = 0,
	ECOMBO_TOTAL = 1,
	ECOMBO_MAX = 2,
//	ECOMBO_AIR = 3,
//	ECOMBO_SKILL = 4,
//	...
	ECOUNT_TOTAL_HIT = 5,
	ECOUNT_TOTAL_DAMAGE = 6,
};
typedef BYTE ECOMBO_STYLE;

typedef struct tagComboCounter
{
	DWORD dwLastComboTime;
	COMBO_TYPE iTotalCombo;
	COMBO_TYPE iMaxCombo;	// 최대 콤보
	//COMBO_TYPE iNormalCombo;
	//COMBO_TYPE iAirCombo;
	//COMBO_TYPE iSkillCombo;
	//...

	tagComboCounter()
	{
		Clear();
	}

	void Clear()
	{
		dwLastComboTime = 0;
		iTotalCombo = 0;
		iMaxCombo = 0;
	}

	void Update()
	{
		if ( iTotalCombo > iMaxCombo )
		{
			iMaxCombo = iTotalCombo;
		}
	}

	void ResetCombo(const COMBO_TYPE iCombo=1)
	{
		Update();
		iTotalCombo = iCombo;
	}
} SComboCounter;

class PgComboCounter
{
public:

	PgComboCounter();
	~PgComboCounter();

	void Clear(bool const bOwner=true);

	COMBO_TYPE AddCombo(const COMBO_TYPE iAddedCombo, bool& bResetOut, DWORD const dwDelay);
	void AddDamage(const COMBO_TYPE iCount=1){	m_iTotalDamageCount+=iCount;}
	COMBO_TYPE GetComboCount(const ECOMBO_STYLE kStyle=ECOMBO_TOTAL);

	CLASS_DECLARATION_S(BM::GUID, OwnerGuid);
protected:
	DWORD m_dwDelayTime;
	SComboCounter	m_kComboCounter;
	COMBO_TYPE		m_iTotalHitCount;		// 총 몇대 때렸는냐?
	COMBO_TYPE		m_iTotalDamageCount;	// 총 몇대 맞았느냐?
};

#endif // WEAPON_VARIANT_ACTION_BATTLE_PGCOMBOCOUNTER_H