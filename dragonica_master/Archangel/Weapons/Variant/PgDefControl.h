#ifndef WEAPON_VARIANT_ABIL_PGDEFCONTROL_H
#define WEAPON_VARIANT_ABIL_PGDEFCONTROL_H

#include "Global.h"
#include "DefAbilType.h"
#include "PgDynamicDefMgr.h"

template<typename T_DEF,typename T_UNIT,typename T_TUNNINGDEF>
class PgDefControl
{
private:
	PgDefControl(){}

public:
	PgDefControl(int const kID,const T_DEF* pkDef,T_UNIT* pkUnit, int const iWeight=0,const T_TUNNINGDEF* pkTunningDef=NULL)
		:	m_pkRateDef(NULL)
		,	m_pkDef(pkDef)
		,	m_pkUnit(pkUnit)
		,	m_iWeight(iWeight)
		,	m_kGrade(EMGRADE_NONE)
		,	m_pkTunningDef(pkTunningDef)
	{
		if ( m_pkUnit )
		{
			SetGrade();
			SetID(kID);
		}		
	}

	~PgDefControl(){}

	int PgDefControl::GetAbil(WORD const wAbilType) const
	{
		int iBagicValue = 0;
		bool bIsTunningData = false;

		if ( m_pkTunningDef )
		{
			if ( m_pkTunningDef->FindAbil(wAbilType, iBagicValue) )
			{
				bIsTunningData = true;
			}
			if ( !m_pkUnit )
			{
				return iBagicValue;
			}
		}

		if ( !bIsTunningData )
		{
			if ( !m_pkDef )
			{
				if ( m_pkUnit )
				{
					iBagicValue = m_pkUnit->GetAbil(wAbilType);
				}
				else
				{
					return iBagicValue;
				}
			}
			else
			{
				iBagicValue = m_pkDef->GetAbil(wAbilType);
				if ( !m_pkUnit )
				{
					return iBagicValue;
				}
			}
		}

		if ( IsAddRate() && CheckAbil(wAbilType) )
		{
			int iRateValue = m_pkRateDef->GetAbil(wAbilType,m_kGrade);
			if ( 0 != iRateValue )
			{
				iRateValue *= m_iWeight;
				m_pkUnit->SetAbil(GetRateAbil(wAbilType),iRateValue);

				float const fRate = static_cast<float>(iRateValue) / ABILITY_RATE_VALUE_FLOAT;
				int const iResultValue = iBagicValue + static_cast<int>(static_cast<float>(iBagicValue) * fRate);
				m_pkUnit->SetAbil( GetCalculateAbil(wAbilType), iResultValue );
			}
		}

		return iBagicValue;
	}

	bool CheckAbil(WORD const wAbilType) const
	{
		return (wAbilType > DAT_RC_TYPE_MIN) && (wAbilType < DAT_RC_TYPE_MAX);
	}

	bool IsAddRate() const
	{
		return m_iWeight && m_pkRateDef;
	}

	bool SetUnit(T_UNIT* pkUnit)
	{	
		m_pkUnit=pkUnit;
		if(pkUnit)
		{
			SetGrade();
			return true;
		}
		m_pkRateDef = NULL;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool SetID(int const kID)
	{
		GET_DEF(PgDynamicDefMgr, kDynamicDefMgr);
		m_pkRateDef = kDynamicDefMgr.GetDef(kID);	
		if(m_pkRateDef)
		{
			return true;
		}
		else
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	void SetWeight(int const iWeight)	{	m_iWeight=iWeight;	}

private:

	void SetGrade()
	{
		m_kGrade = m_pkUnit->GetAbil(AT_GRADE);
		if ( EMGRADE_NONE == m_kGrade )
		{
			m_kGrade = EMGRADE_NORMAL;
		}
	}

	void SetDieExp()
	{
		// 몬스터가 주는 경험치를 찾아서 셋팅
		if ( m_pkDef && IsAddRate() )
		{
			__int64 i64RateValue = m_pkRateDef->GetAbil64(DAT_DIE_EXP,m_kGrade);
			if ( 0 != i64RateValue )
			{
				__int64 i64Value = m_pkDef->GetAbil64(AT_DIE_EXP);
				i64Value += (i64Value*i64RateValue) / (__int64)ABILITY_RATE_VALUE;
				m_pkUnit->SetAbil64(AT_DIE_EXP,i64Value);
			}
		}

	}
	
private:
	T_UNIT*						m_pkUnit;
	const T_DEF*				m_pkDef;
	const T_TUNNINGDEF*			m_pkTunningDef;
	int							m_iWeight;
	const PgAddAbilRateDef*		m_pkRateDef;
	BYTE						m_kGrade;
};

#endif // WEAPON_VARIANT_ABIL_PGDEFCONTROL_H