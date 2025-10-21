#ifndef WEAPON_VARIANT_PGSUMMONED_H
#define WEAPON_VARIANT_PGSUMMONED_H

#include "PgControlUnit.h"

typedef SEntityInfo		SummonedInfo_;

typedef struct tagSummonedMapMoveData
{
	tagSummonedMapMoveData(void)
		: iClass(0), iLevel(0), iHP(0), bUniqueClass(false), sNeedSupply(0), iLifeTime(0)
	{}

	CSkill::MAP_COOLTIME	kSkillCoolTime;
	int iClass;
	int iLevel;
	int iHP;
	bool bUniqueClass;
	short sNeedSupply;
	int iLifeTime;
	int iMaxLifeTime;
	BM::Stream				kAddOnPacket;

	void WriteToPacket( BM::Stream &kPacket )const
	{
		PU::TWriteTable_AA( kPacket, kSkillCoolTime );
		kPacket.Push(iClass);
		kPacket.Push(iLevel);
		kPacket.Push(iHP);
		kPacket.Push(bUniqueClass);
		kPacket.Push(sNeedSupply);
		kPacket.Push(iLifeTime);
		kPacket.Push(iMaxLifeTime);
		kPacket.Push( kAddOnPacket.Data() );
	}

	bool ReadFromPacket( BM::Stream &kPacket )
	{
		if ( kPacket.RemainSize() < min_size() )
		{
			return false;
		}

		PU::TLoadTable_AA( kPacket, kSkillCoolTime );
		kPacket.Pop(iClass);
		kPacket.Pop(iLevel);
		kPacket.Pop(iHP);
		kPacket.Pop(bUniqueClass);
		kPacket.Pop(sNeedSupply);
		kPacket.Pop(iLifeTime);
		kPacket.Pop(iMaxLifeTime);

		kAddOnPacket.Clear();
		kPacket.Pop( kAddOnPacket.Data() );
		kAddOnPacket.PosAdjust();
		return true;
	}

	size_t min_size(void)const
	{
		return	sizeof(size_t) /* SkillCoolTime */
			+ sizeof(iClass) + sizeof(iLevel) + sizeof(iHP);
	}

	bool IsNull()const
	{
		return 0==iClass;
	}

	static tagSummonedMapMoveData const& NullData()
	{
		static tagSummonedMapMoveData const kNull;
		return kNull;
	}
}SSummonedMapMoveData;
typedef std::map< BM::GUID, SSummonedMapMoveData >			CONT_SUMMONED_MAPMOVE_DATA;

typedef struct tagUserSummonedMapMoveData
{
	CONT_SUMMONED_MAPMOVE_DATA kSummonedMapMoveData;
	tagUserSummonedMapMoveData()
	{}

	void WriteToPacket( BM::Stream &kPacket )const
	{
		PU::TWriteTable_AM( kPacket, kSummonedMapMoveData );
	}

	bool ReadFromPacket( BM::Stream &kPacket )
	{
		return PU::TLoadTable_AM( kPacket, kSummonedMapMoveData );
	}

	size_t min_size(void)const
	{
		return	sizeof(size_t);
	}
}SUserSummonedMapMoveData;
typedef std::map< BM::GUID, SUserSummonedMapMoveData >	CONT_UNIT_SUMMONED_MAPMOVE_DATA;

class CMonsterDef;
class PgSummoned
	: public PgControlUnit
{
public:
	PgSummoned();
	virtual ~PgSummoned();

	virtual HRESULT Create(const void* pInfo);
	virtual EUnitType UnitType() const;
	virtual void Init();

	bool IsEternalLife()const { return m_bEternalLife; }
	unsigned long LifeTime() const;
	void LifeTime(unsigned long const kTime);
	virtual int AutoHeal( unsigned long ulElapsedTime, float const fMultiplier = 1.0f);

	void WriteToPacket_MapMoveData(BM::Stream &rkPacket);
	virtual void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType=WT_DEFAULT)const;
	virtual EWRITETYPE ReadFromPacket(BM::Stream &rkPacket);

	virtual bool SetAbil(WORD const Type, int const iValue, bool const bIsSend = false, bool const bBroadcast = false);
	virtual int GetAbil(WORD const Type) const;

	void SetAddSummonedToOwner(){m_bAddSummonedToOwner=true;}

	//매우중요, 소환체는 맵이동시에 오너한테 패킷을 보내주는 타이밍을 나중에(맵로딩 컴플리트를 보낼때) 해야해서...
	//PgPet의 m_bAddPetToOwner을 참고하고 작업 할껄
	virtual bool CanNfyAddEffect()const{return m_bAddSummonedToOwner;}

	void SetNPC(bool const bSet) { m_bNPC = bSet; }
	bool IsNPC() const { return m_bNPC; }

protected:
	virtual bool DoLevelup( SClassKey const& rkNewLevelKey ) { return true; }
	virtual bool DoBattleLevelUp( short const ) { return true; }

	void CopyAbilFromClassDef( CLASS_DEF_BUILT const *pDef);
private:
	bool SetInfo(SummonedInfo_ const * pInfo);
	void SetInfoFromDefaultSummonedInfo(DefaultSummonedInfo_ const &rkInfo);
	void CalcCreateSummonBasicAbil(WORD const wType, int const iRate);
//	void SetCommonAbil():

	unsigned long m_kLifeTime;
	bool m_bEternalLife;
	CLASS_DECLARATION_S(bool, UniqueClass);
	CLASS_DECLARATION_S(short, Supply);

	bool			m_bAddSummonedToOwner;
	bool			m_bNPC;
};

#endif //WEAPON_VARIANT_PGSUMMONED_H