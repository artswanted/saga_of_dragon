#include "StdAfx.h"
#include "PgPremiumMgr.h"
#include "Lohengrin/GameTime.h"
#include "PgControlDefMgr.h"
#include "PgEventView.h"
#include "PgPlayer.h"

PgPremiumMgr::PgPremiumMgr(void)
	: m_iServiceNo(0)
{
}

PgPremiumMgr::~PgPremiumMgr(void)
{
}

void PgPremiumMgr::Clear()
{
	m_iServiceNo = 0;
	m_kStartDate.Clear();
	m_kEndDate.Clear();
	m_kContDef.clear();
	m_kContCustom.clear();
}

bool PgPremiumMgr::IsUserService(bool const bTimeCheck)const
{
	if(false==bTimeCheck)
	{
		return (0<m_iServiceNo);
	}
	return (0<m_iServiceNo) && (0i64<GetEndSecTime());
}

__int64 PgPremiumMgr::GetEndSecTime()const
{
	__int64 const kNowSec = g_kEventView.GetLocalSecTime();
	__int64 kEndSec = 0i64;
	CGameTime::DBTimeEx2SecTime(m_kEndDate, kEndSec);
	return std::max(kEndSec-kNowSec, 0i64);
}

void PgPremiumMgr::AddEndDate(int const iDay)
{
	BM::DBTIMESTAMP_EX kNowTime;
	g_kEventView.GetLocalTime( kNowTime );

	if(m_kEndDate > kNowTime)
	{
		kNowTime = m_kEndDate;
	}

	BM::PgPackedTime kEndDate(kNowTime);
	CGameTime::AddTime(kEndDate, iDay * CGameTime::MINUTE );
	m_kEndDate = BM::DBTIMESTAMP_EX(kEndDate);
}

int PgPremiumMgr::SetService(int const iNo, BM::Stream & rkPacket)
{
	GET_DEF(PgDefPremiumMgr, kDefPremium);
	if(kDefPremium.CreateCopy(iNo, m_kStartDate, m_kEndDate, m_kContDef, m_kContCustom))
	{
		m_iServiceNo = iNo;
		FromDB(rkPacket);
		return 0;
	}
	return -1;
}

//프리미엄 서비스 등록(컨텐츠)
int PgPremiumMgr::SetupService(int const iNo, PgPlayer * pkPlayer, BM::Stream & rkFromDB)
{
	if(!pkPlayer)
	{
		return -1;
	}

	GET_DEF(PgDefPremiumMgr, kDefPremium);
	if(kDefPremium.CreateCopy(iNo, m_kStartDate, m_kEndDate, m_kContDef, m_kContCustom))
	{
		m_iServiceNo = iNo;
		FromDB(rkFromDB);

		CONT_PREMIUM_TYPE::const_iterator c_it = m_kContCustom.begin();
		while(c_it != m_kContCustom.end())
		{
			(*c_it).second->Setup(pkPlayer);
			++c_it;
		}		
		return 0;
	}
	return -1;
}

void PgPremiumMgr::FromDB(BM::Stream & rkPacket)
{
	EPremiumServiceType eType = PST_NONE;
	size_t iWRSize = 0;
	while(0 < rkPacket.RemainSize())
	{
		rkPacket.Pop(eType);
		rkPacket.Pop(iWRSize);
		CONT_PREMIUM_TYPE::const_iterator c_it = m_kContCustom.find(eType);
		if(c_it!=m_kContCustom.end())
		{
			(*c_it).second->FromDB(rkPacket, iWRSize);
		}
		else
		{
			rkPacket.RdPos( rkPacket.RdPos()+iWRSize );
		}
	}
}

void PgPremiumMgr::ToDB(BM::Stream & rkPacket)const
{
	CONT_PREMIUM_TYPE::const_iterator c_it = m_kContCustom.begin();
	while(c_it != m_kContCustom.end())
	{
		rkPacket.Push((*c_it).first);

		size_t const iWRPos = rkPacket.WrPos();
		size_t iWRSize = 0;
		rkPacket.Push(iWRSize);		

		//
		(*c_it).second->ToDB(rkPacket);

		//
		iWRSize = rkPacket.WrPos() - iWRPos - sizeof(iWRSize);
		rkPacket.ModifyData( iWRPos, &iWRSize, sizeof(iWRSize) );
		++c_it;
	}
}

void PgPremiumMgr::CopyTo(PgPremiumMgr & rhs)const
{
	rhs.m_kContDef.clear();
	rhs.m_kContCustom.clear();

	rhs.m_iServiceNo = m_iServiceNo;
	rhs.m_kStartDate = m_kStartDate;
	rhs.m_kEndDate = m_kEndDate;
	rhs.m_kContDef = m_kContDef;

	CONT_PREMIUM_TYPE::mapped_type kData;
	CONT_PREMIUM_TYPE::const_iterator c_it = m_kContCustom.begin();
	while(c_it != m_kContCustom.end())
	{
		kData.reset( (*c_it).second->Copy() );
		if(kData)
		{
			rhs.m_kContCustom.insert(std::make_pair((*c_it).first, kData));
		}

		++c_it;
	}
}

void PgPremiumMgr::ReadFromPacket(BM::Stream & rkPacket)
{
	Clear();

	BM::Stream kCustomData;
	rkPacket.Pop(kCustomData.Data());
	kCustomData.PosAdjust();

	rkPacket.Pop(m_iServiceNo);
	SetService(m_iServiceNo, kCustomData);
	rkPacket.Pop(m_kStartDate);
	rkPacket.Pop(m_kEndDate);
}

void PgPremiumMgr::WriteToPacket(BM::Stream & rkPacket) const
{
	BM::Stream kCustomData;
	ToDB(kCustomData);
	rkPacket.Push(kCustomData.Data());
	rkPacket.Push(m_iServiceNo);	
	rkPacket.Push(m_kStartDate);
	rkPacket.Push(m_kEndDate);
}

bool IsShareRentalPremium(int const kInvType , PgPlayer * pkPlayer)
{
	if(!pkPlayer)
	{
		return false;
	}

	switch(kInvType)
	{
	case IT_SHARE_RENTAL_SAFE1:
	case IT_SHARE_RENTAL_SAFE2:
	case IT_SHARE_RENTAL_SAFE3:
	case IT_SHARE_RENTAL_SAFE4:
		{
			if( S_PST_ShareRentalSafeExpand const* pkPremium = pkPlayer->GetPremium().GetType<S_PST_ShareRentalSafeExpand>() )
			{
				return pkPremium->IsRental(kInvType);
			}
		}break;
	}
	return false;
}
