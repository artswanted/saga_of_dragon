#ifndef WEAPON_VARIANT_ACTION_PGACTIONRESULT_H
#define WEAPON_VARIANT_ACTION_PGACTIONRESULT_H

#include "BM/GUID.h"
#include "IDObject.h"

class CUnit;
enum eElementDmgType;

struct SElemDmgSortData
{
	SElemDmgSortData(eElementDmgType const eType, int const iDmgVal)
		:eDmgType(eType)
		,iDmg(iDmgVal)
	{}

	eElementDmgType eDmgType;
	int iDmg;

	bool operator<(SElemDmgSortData const& rhs) const
	{
		if(iDmg > rhs.iDmg)
		{
			return true;
		}
		else if(iDmg < rhs.iDmg)
		{
			return false;
		}

		if(eDmgType < rhs.eDmgType)
		{//iDmg값이 같으면 타입이 작은 값일수록 우선순위가 결정됨
			return true;
		}
		return false;
	}
};

class PgActionResult : public CAbilObject
{
public:
	enum eBlockedType : BYTE
	{// 블럭 리액션 타입
		EBT_NONE = 0,
		EBT_NONE_REACTION = 1,
		EBT_REACTION = 2,
	};

	struct	stStatusEffect
	{
		int	m_iEffectID,m_iEffectValue;
		stStatusEffect()
		{
			m_iEffectID = m_iEffectValue = 0;
		};
		stStatusEffect(int iEffectID, int iEffectValue)
		{
			m_iEffectID = iEffectID;
			m_iEffectValue = iEffectValue;
		}
	};

public:
	PgActionResult();
	~PgActionResult();

	void SetValue(int const iValue);
	int GetValue();
	void AddEffect(int const iEffect);
	std::vector<int>& GetResultEffect();
	size_t GetEffectNum()const;
	int GetEffect(int const iEffectIndex);

	void AddStatusEffect(int const iEffect, int iEffectValue);
	size_t GetStatusEffectNum()const;
	stStatusEffect* GetStatusEffect(int const iEffectIndex);
	void	ClearStatusEffect();

	void Init();
	void SetBlocked(BYTE const byBlockType);
	BYTE GetBlocked()const;
	void SetDodged(bool const bDodged);
	bool GetDodged()const;
	void SetComic(bool const bComic);
	bool GetComic()const;
	void SetDead(bool const bDead);
	bool GetDead()const;
	void SetInvalid(bool const bInvalid);
	bool GetInvalid()const;
	bool GetMissed()const;
	void SetMissed(bool const bMissed);
	bool GetCritical();
	void SetCritical(bool const bCritical);
	void SetAbsorbValue(int const iAbsorbValue);
	int  GetAbsorbValue() const;
	bool GetEndure()const;
	void SetEndure(bool const bEndure);
	bool GetRestore()const;
	void SetRestore(bool const bRestore);
	
	bool GetDefenceIgnore() const;
	void SetDefenceIgnore(bool const bDefence);

	void SetCollision(BYTE const byCollision);
	BYTE GetCollision()const;
	int GetRemainHP()const;
	void SetRemainHP(int const iHP);
	bool IsValidHP()const;
	void SetDamageAction(bool const bDamageAction);
	bool IsDamageAction()const;
	
	bool	IsRealEffect()const{return m_bIsRealEffect;}
	void	SetRealEffect(bool const bReal)	{	m_bIsRealEffect = bReal;	}
	void	CopyEffectTo(PgActionResult &kTarget);
	void	ClearEffect();

	void ReadFromPacket(BM::Stream &rkPacket);
	void WriteToPacket(BM::Stream &rkPacket)const;

	void SetElemDmg(CUnit* pkCaster,  int const iDestroy, int const iFire=0, int const iIce=0, int const iNature=0, int const iCurse=0);
protected:
	bool m_bMissed;	// Action missed
	BYTE m_byBlockType;	// Action blocked
	bool m_bComic;	// Comic Effect triggered ?
	bool m_bDead;	// Dead ?
	bool m_bInvalid;	// Invalid Target ?
	bool m_bDodged;	// 회피했는가?
	bool m_bCritical;	// Critical 인가?
	bool m_bValidHP;	// m_iRemainHP 값이 유효한 값인가?
	bool m_bEndure;		// 데미지 액션을 확률로 회피하였는가?
	bool m_bRestore;	// 클라의 내용이 잘못 됐으므로, 다시 복구 해라.! Server -> 세팅 하여 -> Client에서 복구
	bool m_bDamageAction;	// 데미지액션 유무
	bool m_bDefenceIgnore; // 방어관통 유무

	int m_iValue;	// Action Result value (Damage or Heal value)
	int m_iRemainHP;
	int m_iAbsorbValue;	//흡수한 데미지
	bool m_bIsRealEffect;	// Client에서만 사용하는 값이니, 서버쪽은 수정하지 말것.
	BYTE m_byCollision;
	
	std::vector<int> m_vecEffect;	// Result Effect
	std::vector<stStatusEffect> m_vecStatusEffect;	// Result Status Effect
	
	CLASS_DECLARATION(bool, m_bElemDmgDestroy, ElemDmgDestroy);
	CLASS_DECLARATION(bool, m_bElemDmgFire, ElemDmgFire);
	CLASS_DECLARATION(bool, m_bElemDmgIce, ElemDmgIce);
	CLASS_DECLARATION(bool, m_bElemDmgNature, ElemDmgNature);
	CLASS_DECLARATION(bool, m_bElemDmgCurse, ElemDmgCurse);
};

class PgActionResultVector
{
public:
	PgActionResultVector();
	virtual ~PgActionResultVector();

	PgActionResult* GetResult(BM::GUID const &rkGuid, bool const bMake = false);
	PgActionResult* GetResult(int const iIndex, BM::GUID& rkGuid);
	int GetValidCount() const;
	int GetTotalCount() const; // Valid / InValid 한 결과를 모두 리턴

	void ReadFromPacket(BM::Stream& rkPacket);
	void WriteToPacket(BM::Stream& rkPacket) const;

	bool Remove( BM::GUID const &rkGuid );
protected:
	void Clear();

private:
	typedef std::map<BM::GUID, PgActionResult*> ACTIONRESULT_MAP;
	ACTIONRESULT_MAP m_kResult;

	typedef BM::TObjectPool<PgActionResult> ACTIONRESULT_POOL;
	static ACTIONRESULT_POOL m_kPool;
};

#endif // WEAPON_VARIANT_ACTION_PGACTIONRESULT_H