#ifndef WEAPON_VARIANT_SKILL_EFFECT_PGUNITEFFECTMGR_H
#define WEAPON_VARIANT_SKILL_EFFECT_PGUNITEFFECTMGR_H

#include "IDObject.h"
#include "Effect.h"
#include "BM/Observer.h"
//유닛에 붙는 이펙트 정보.

typedef std::map< int, CEffect* > ContEffect;//이펙트 번호.

typedef struct tagContEffectItor
{
	tagContEffectItor()
	{
		byCurrentCont =0;
	}

	ContEffect::iterator kItor;
	BYTE byCurrentCont;	// EEffectType
}ContEffectItor;

typedef BM::TObjectPool<CEffect> EffectPool;
typedef BM::TObjectPool<CItemEffect> ItemEffectPool;


typedef enum
{
	E_AERESULT_NONE = 0,
	E_AERESULT_NEWCREATE = 1,
	E_AERESULT_SENDPACKET = 2,
	E_AERESULT_DELEFFECT = 4,
	E_AERESULT_RESTART = 8,
	E_AERESULT_RESTART_NOT_SEND_CLIENT = 16
} eAddEffectResult;
typedef BYTE EAddEffectResult;

class PgUnitEffectMgr
	: public CAbilObject
	//,	public BM::CSubject< BM::Stream* >//옵저버 패턴 사용
{
public:
	PgUnitEffectMgr(void);
	virtual ~PgUnitEffectMgr(void);

	void Clear();
	size_t Size() const;
	size_t SaveToDB(BYTE * const pkBuffer, size_t const iBufferSize) const;
	void LoadFromDB(BYTE const * const pkBuffer, size_t const iBufferSize);
	void WriteToPacket(BM::Stream &rPacket, bool const bIsSimple)const;
	void ReadFromPacket(BM::Stream &rPacket, bool const bIsSimple);
	void GetFirstEffect(ContEffectItor& rkItor) const;
	void GetFirstItemEffect(ContEffectItor& rkItor) const;
	CEffect* GetNextEffect(ContEffectItor& rkItor) const;
	CEffect const *FindEffect(int iEffectNo)const;
	CEffect* FindInGroup(int iEffectNo, bool bInGroup = false)const;
	CEffect* FindInGroupItemEffect(int iEffectKey, bool bInGroup = false);
	CEffect* FindInGroupItemEffectByEffectNo(int iEffectNo, bool bInGroup = false);
	bool const FindInGroupItem(int iEffectNo, bool bInGroup = false) const;
	//CEffect* AddEffect(BYTE byType, int iEffectNo, int iValue, SActArg* pkArg, unsigned long ulElapsedTime, unsigned long ulInterval,
	//	BM::GUID const &rkCaster);
	//CEffect* AddEffect(BYTE byType, int const iEffectNo, int iValue, SActArg* pkArg, unsigned long ulElapsedTime, unsigned long ulInterval,
	//	EAddEffectResult& eResult, unsigned long ulEndTime = 0);
	CEffect* AddEffect(SEffectCreateInfo& rkCreate, EAddEffectResult& eResult);
	bool DeleteEffect(int const iEffectKey);

protected:
	void AddEffect(CEffect* pkEffect);
	ContEffect& GetContainer(EEffectType const eType);
	void LoadFromDB_v1(BYTE const * const pkBuffer, size_t const iBufferSize);
	void LoadFromDB_v2(BYTE const * const pkBuffer, size_t const iBufferSize);

	static short const s_EffectSaveVersion = 2;
private:
	ContEffect m_kContEffect;		// std::map<EffectID, CEffect>
	ContEffect m_kContItemEffect;	// std::map<ItemID, CEffect>
	static EffectPool ms_kEffectPool;
	static ItemEffectPool ms_kItemEffectPool;
	CLASS_DECLARATION_S(bool, MustAllClear);
	CLASS_DECLARATION_S(bool, MustCurseClear);
	CLASS_DECLARATION( bool, m_bDamaged, Damaged );
	CLASS_DECLARATION_S(int, TargetGroundNo);
};

#endif // WEAPON_VARIANT_SKILL_EFFECT_PGUNITEFFECTMGR_H