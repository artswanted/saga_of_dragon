#pragma once

#include "PgPremiumType.h"

class PgPlayer;

class PgPremiumMgr
{
public:
	PgPremiumMgr();
	~PgPremiumMgr();
	void Clear();

	bool IsUserService(bool const bTimeCheck=false)const;
	__int64 GetEndSecTime()const;

	int SetupService(int const iNo, PgPlayer * pkPlayer, BM::Stream & rkFromDB);
	int SetService(int const iNo, BM::Stream & rkCustomData);

	int GetServiceNo()const {return m_iServiceNo;}
	BM::DBTIMESTAMP_EX const& StartDate()const {return m_kStartDate;}
	BM::DBTIMESTAMP_EX const& EndDate()const {return m_kEndDate;}
	void StartDate(BM::DBTIMESTAMP_EX const& kStartDate) {m_kStartDate = kStartDate;}
	void EndDate(BM::DBTIMESTAMP_EX const& kEndDate) {m_kEndDate = kEndDate;}
	void AddEndDate(int const iDay);

	int AddType(int const iTypeNo);

	void CopyTo(PgPremiumMgr & rhs)const;
	void ReadFromPacket(BM::Stream & rkPacket);
	void WriteToPacket(BM::Stream & rkPacket) const;

	void ToDB(BM::Stream & rkPacket)const;

	template<typename T>
	T const* GetType()const
	{
		EPremiumServiceType const eType = T::Type();
		CONT_DEF_PREMIUM_TYPE::const_iterator def_it = m_kContDef.find(eType);
		if(def_it != m_kContDef.end())
		{
			CONT_DEF_PREMIUM_TYPE::mapped_type kFun = def_it->second;
			return dynamic_cast<T const*>( kFun.get() );
		}

		CONT_PREMIUM_TYPE::const_iterator c_it = m_kContCustom.find(eType);
		if(c_it != m_kContCustom.end())
		{
			CONT_PREMIUM_TYPE::mapped_type kFun = c_it->second;
			return dynamic_cast<T const*>( kFun.get() );
		}

		return NULL;
	}

	template<typename T>
	T * SetType()
	{
		EPremiumServiceType const eType = T::Type();
		CONT_PREMIUM_TYPE::iterator it = m_kContCustom.find(eType);
		if(it != m_kContCustom.end())
		{
			CONT_PREMIUM_TYPE::mapped_type kFun = it->second;
			return dynamic_cast<T*>( kFun.get() );
		}

		return NULL;
	}

private:
	void FromDB(BM::Stream & rkPacket);

private:
	int m_iServiceNo;
	BM::DBTIMESTAMP_EX m_kStartDate;
	BM::DBTIMESTAMP_EX m_kEndDate;

	CONT_DEF_PREMIUM_TYPE m_kContDef;
	CONT_PREMIUM_TYPE m_kContCustom;
};


bool IsShareRentalPremium(int const kInvType , PgPlayer * pkPlayer);
