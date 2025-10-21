#ifndef WEAPON_VARIANT_UNIT_PGPET_H
#define WEAPON_VARIANT_UNIT_PGPET_H

#include "PgControlUnit.h"

typedef struct tagPetMapMoveData
{
	tagPetMapMoveData(void)
		:	iMP(-1)
	{}

	tagPetMapMoveData( PgInventory const &rkInven )
		:	kInventory(rkInven)
		,	iMP(-1)
	{
	}

	mutable PgInventory		kInventory;
	CSkill::MAP_COOLTIME	kSkillCoolTime;
	int						iMP;
	BM::Stream				kAddOnPacket;

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kInventory.WriteToPacket( kPacket, WT_DEFAULT );
		PU::TWriteTable_AA( kPacket, kSkillCoolTime );
		kPacket.Push( iMP );
		kPacket.Push( kAddOnPacket.Data() );
	}

	bool ReadFromPacket( BM::Stream &kPacket )
	{
		if ( kPacket.RemainSize() < min_size() )
		{
			return false;
		}

		kInventory.ReadFromPacket( kPacket, WT_DEFAULT );
		PU::TLoadTable_AA( kPacket, kSkillCoolTime );
		kPacket.Pop( iMP );

		kAddOnPacket.Clear();
		kPacket.Pop( kAddOnPacket.Data() );
		kAddOnPacket.PosAdjust();
		return true;
	}

	size_t min_size(void)const
	{
		return	sizeof(iMP)
			+	sizeof(size_t)
			+	sizeof(size_t);
	}

}SPetMapMoveData;
typedef std::map< BM::GUID, SPetMapMoveData >		CONT_PET_MAPMOVE_DATA;

typedef enum ePetStateCheckType
{
	PET_STATE_CHK_NONE		= 0x00,
	PET_STATE_CHK_EXP		= 0x01,
}E_PET_CHECK_TYPE;

class PgPlayer;
class PgPet
	:	public PgControlUnit
{
public:
	static __int64 const ms_i64ExpUpPeriodSecTime = CGameTime::MINUTE / CGameTime::SECOND;
	
	static __int64 const ms_i64FatigueToDieSecTime = CGameTime::HOUR / CGameTime::SECOND * 3i64;// 3시간후에 뒤짐
	static __int64 const ms_i64RenamePetMoney = 50000i64;// 펫 이름 교체 비용
	static __int64 const ms_i64ChangeColorPetMoney = 100000i64;

public:
	PgPet(void);
	virtual ~PgPet(void);

	virtual void Init();
	HRESULT Create( PgBase_Item const &kPetItem, CUnit * pkOwner, CONT_PET_MAPMOVE_DATA::mapped_type &kPetData );

	virtual EUnitType UnitType()const{return UT_PET;}
	virtual int AutoHeal( unsigned long ulElapsedTime, float const fMultiplier=1.0f );

	virtual void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType=WT_DEFAULT)const;
	virtual EWRITETYPE ReadFromPacket(BM::Stream &rkPacket);

	void WriteToPacket_SPetMapMoveData( BM::Stream &rkPacket );

	virtual int GetAbil(WORD const Type) const;
	virtual bool SetAbil(WORD const Type, int const iInValue, bool const bIsSend = false, bool const bBroadcast = false);
	virtual bool AddAbil(WORD const Type, int const iValue);

	__int64 GetPeriodEndSecTime(void)const{return m_i64PeriodSecTime;}
	void GetPeriodEndTime( SYSTEMTIME &rkOutSysTemTime )const{CGameTime::SecTime2SystemTime( GetPeriodEndSecTime(), rkOutSysTemTime, CGameTime::SECOND );}
	void SetPeriodEndSecTime( __int64 const i64PeriodSecTime ){m_i64PeriodSecTime=i64PeriodSecTime;}

	__int64 GetHealthSecTime(void)const{return m_i64HealthSecTime;}
	__int64 GetMentalSecTime(void)const{return m_i64MentalSecTime;}

	BYTE UpdateNextConditionCheckSecTime( __int64 const i64CurSecTime );//return 은E_PET_CHECK_TYPE
	__int64 GetNextExpUpSecTime(void)const{return m_i64NextExpUpSecTime;}

	BYTE GetPetType(void)const{return m_byPetType;}

	void SetState_Health( PgItem_PetInfo::SStateValue const &kValue );
	void SetState_Mental( PgItem_PetInfo::SStateValue const &kValue );

	void SetAddPetToOwner(){m_bAddPetToOwner=true;}

	//매우중요, 펫은 맵이동시에 오너한테 패킷을 보내주는 타이밍을 나중에(맵로딩 컴플리트를 보낼때) 해야해서...
	//이걸 제대로 안하면 Effect에 버그가 생긴다....................
	virtual bool CanNfyAddEffect()const{return m_bAddPetToOwner;}

	//라이딩펫은 주인의 능력치에 영향을 받으므로 주인을 참조해야 한다.
	void SetCallerUnit(CUnit* pkCaller);
protected:
	bool SetBasicAbil(void);
	void SetBasicAbil(PgClassPetDef const &kPetDef);

	virtual bool DoLevelup( SClassKey const& rkNewLevelKey );
	virtual bool DoBattleLevelUp( short const nBattleLv );

	virtual void VOnRefreshAbil();

protected:
	__int64			m_i64PeriodSecTime;//second
	__int64			m_i64HealthSecTime;//
	__int64			m_i64MentalSecTime;//

	__int64			m_i64NextExpUpSecTime;//second
	BYTE			m_byPetType;//EPetType사용
	bool			m_bAddPetToOwner;

protected:
	PgPlayer*		m_pkCaller;
};

#endif // WEAPON_VARIANT_UNIT_PGPET_H