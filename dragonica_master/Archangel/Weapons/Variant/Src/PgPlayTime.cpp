#include "stdafx.h"
#include "Global.h"
#include "PgEventView.h"
#include "PgPlayTime.h"
#include "BM/LocalMgr.h"

namespace PgPlayerPlayTimeUtil
{
	class PgFindNomal
	{
	public:
		PgFindNomal(int const iValue) : m_iValue(iValue) {}

		bool operator () (PgDefPlayerPlayTimeImpl::SSUBBODY const& rhs) const
		{
			if( rhs.iStart <= m_iValue && m_iValue <= rhs.iEnd)	
			{
				return true;
			}

			return false;
		}

	private:
		int m_iValue;
	};

	class PgFindLastValue
	{
	public:
		PgFindLastValue(int const iValue) : m_iValue(iValue) {}

		bool operator () (PgDefPlayerPlayTimeImpl::SSUBBODY const& rhs) const
		{
			if( rhs.iEnd==0 && rhs.iStart <= m_iValue )
			{
				return true;
			}

			return false;
		}

	private:
		int m_iValue;
	};
};


PgPlayerPlayTime::PgPlayerPlayTime()
{
	Clear();
}

PgPlayerPlayTime::~PgPlayerPlayTime()
{

}

PgPlayerPlayTime& PgPlayerPlayTime::operator=( PgPlayerPlayTime const &rhs )
{
	m_eDefendMode = rhs.m_eDefendMode;
	m_iLastAccConnSec_Member = rhs.m_iLastAccConnSec_Member;
	m_iLastAccDisConnSec_Member = rhs.m_iLastAccDisConnSec_Member;
	m_i64TotalPlayTimeSec = rhs.m_i64TotalPlayTimeSec;
	m_i64SelectCharacterSec = rhs.m_i64SelectCharacterSec;
	m_i64LastSpecificRewardSec = rhs.m_i64LastSpecificRewardSec;
	m_iExpRate = rhs.m_iExpRate;
	m_iMoneyRate = rhs.m_iMoneyRate;
	m_iDropRate = rhs.m_iDropRate;
	m_eEtcBoolean = rhs.m_eEtcBoolean;
	return *this;
}

void PgPlayerPlayTime::Clear()
{
	m_eDefendMode = WDM_NONE;

	m_iLastAccConnSec_Member = 0;
	m_iLastAccDisConnSec_Member = 0;

	m_i64TotalPlayTimeSec = 0;
	m_i64SelectCharacterSec = 0;
	m_i64LastSpecificRewardSec = 0;
	GroundEnterTimeSec(0);

	m_iExpRate = 100;
	m_iMoneyRate = 100;
	m_iDropRate = 100;
	m_eEtcBoolean = PPTC_NONE;
}

bool PgPlayerPlayTime::IsTimeOverKick()const
{
	if( (0 != (WDM_TIMEOVER&m_eDefendMode))
	&& (g_kDefPlayTime.IsEnable(PgDefPlayerPlayTimeImpl::EDPTF_KICK)) )
	{
		return true;
	}
	
	return false;
}

void PgPlayerPlayTime::SetPlayTime(int const iAccConnSec, int const iAccDisSec)
{
	m_iLastAccConnSec_Member = iAccConnSec;
	m_iLastAccDisConnSec_Member = iAccDisSec;
}

void PgPlayerPlayTime::SetAccTime(unsigned short const usAge, BM::DBTIMESTAMP_EX const &dtUserBirth,  BYTE const bGmLevel, int& iAccConnSec, int& iAccDisSec, BM::PgPackedTime const& kLastLogout)
{
	m_eDefendMode = WDM_NONE;

	if( !IsAdult(dtUserBirth) )
	{//! 성인이 아니라면,
		m_eDefendMode |= WDM_CHILDFLAG;
	}

	if( bGmLevel > 0 )
	{
		m_eDefendMode |= WDM_GMFLAG;
	}
	
	if ( g_kDefPlayTime.IsEnable(PgDefPlayerPlayTimeImpl::EDPTF_VIEWCLOCK) )
	{
		m_eDefendMode |= WDM_VIEWCLOCKFLAG;
	}

	if( g_kDefPlayTime.IsUse() )
	{
		m_eDefendMode |= WDM_USEFLAG;

		if( g_kDefPlayTime.IsEnable(PgDefPlayerPlayTimeImpl::EDPTAU_CHILD) )
		{
			if( (false==IsChildCheck())
			&& (true==(m_eDefendMode & WDM_USEFLAG)) )
			{
				m_eDefendMode ^= WDM_USEFLAG;	
			}
		}

		if( g_kDefPlayTime.IsEnable(PgDefPlayerPlayTimeImpl::EDPTAU_GMEXCEPT) )
		{
			if( (true==IsGmCheck()) 
			&& (true==(m_eDefendMode & WDM_USEFLAG)) )
			{
				m_eDefendMode ^= WDM_USEFLAG;
			}
		}
	}

	if( IsUse() )
	{
		if(g_kDefPlayTime.ResetType()==PgDefPlayerPlayTimeImpl::EDPTRT_ACCDISCON)
		{
			if( g_kDefPlayTime.ResetValue1() <= iAccDisSec )
			{
				iAccConnSec = 0;
				iAccDisSec = 0;
			}
		}
		else if(g_kDefPlayTime.ResetType()==PgDefPlayerPlayTimeImpl::EDPTRT_DAY)
		{
			if( kLastLogout < g_kDefPlayTime.LastApplyTime() )
			{
				iAccConnSec = 0;
				iAccDisSec = 0;
			}
		}
	}

	m_iLastAccConnSec_Member = iAccConnSec;
	m_iLastAccDisConnSec_Member = iAccDisSec;
	m_i64SelectCharacterSec = g_kEventView.GetLocalSecTime(CGameTime::SECOND);
	m_i64LastSpecificRewardSec = m_i64SelectCharacterSec;

	Update_PlayTime(BM::GetTime32());
}

void PgPlayerPlayTime::Update_PlayTime(DWORD const dwCurTime)
{
	if ( IsUse() )
	{
		int iAccConSec = GetAccConnSec_Member();
		m_iExpRate = g_kDefPlayTime.GetExpRate(iAccConSec);
		m_iMoneyRate = g_kDefPlayTime.GetMoneyRate(iAccConSec);
		m_iDropRate = g_kDefPlayTime.GetDropRate(iAccConSec);
		m_eEtcBoolean = PPTC_NONE;
		if( !g_kDefPlayTime.IsTakeUpItem(iAccConSec) )
		{
			m_eEtcBoolean |= PPTC_NOT_TAKEITEM;
		}
		if( !g_kDefPlayTime.IsQuestFinish(iAccConSec) )
		{
			m_eEtcBoolean |= PPTC_NOT_QUESTFINISH;
		}
		
		if ( g_kDefPlayTime.IsTimeOver(iAccConSec) )
		{
			m_eDefendMode |= WDM_TIMEOVER;
		}
	}
}

void PgPlayerPlayTime::WriteToPacket_PlayTime(BM::Stream& rkPacket)const
{
	rkPacket.Push(m_iLastAccConnSec_Member);
	rkPacket.Push(m_iLastAccDisConnSec_Member);
	rkPacket.Push(m_i64SelectCharacterSec);
	rkPacket.Push(m_i64LastSpecificRewardSec);
	rkPacket.Push(m_i64TotalPlayTimeSec);
	rkPacket.Push(m_eDefendMode);
	rkPacket.Push(m_iExpRate);
	rkPacket.Push(m_iMoneyRate);
	rkPacket.Push(m_iDropRate);
	rkPacket.Push(m_eEtcBoolean);
}

void PgPlayerPlayTime::ReadFromPacket_PlayTime(BM::Stream& rkPacket)
{
	rkPacket.Pop(m_iLastAccConnSec_Member);
	rkPacket.Pop(m_iLastAccDisConnSec_Member);
	rkPacket.Pop(m_i64SelectCharacterSec);
	rkPacket.Pop(m_i64LastSpecificRewardSec);
	rkPacket.Pop(m_i64TotalPlayTimeSec);
	rkPacket.Pop(m_eDefendMode);
	rkPacket.Pop(m_iExpRate);
	rkPacket.Pop(m_iMoneyRate);
	rkPacket.Pop(m_iDropRate);
	rkPacket.Pop(m_eEtcBoolean);
}

void PgPlayerPlayTime::WriteToPacket_PlayTimeSimple(BM::Stream& rkPacket)const
{
	rkPacket.Push(m_eDefendMode);
	rkPacket.Push(m_iExpRate);
	rkPacket.Push(m_iMoneyRate);
	rkPacket.Push(m_iDropRate);
	rkPacket.Push(m_eEtcBoolean);
}

void PgPlayerPlayTime::ReadFromPacket_PlayTimeSimple(BM::Stream& rkPacket)
{
	rkPacket.Pop(m_eDefendMode);
	rkPacket.Pop(m_iExpRate);
	rkPacket.Pop(m_iMoneyRate);
	rkPacket.Pop(m_iDropRate);
	rkPacket.Pop(m_eEtcBoolean);
}

__int64 PgPlayerPlayTime::GetThisConnSec_Character()const
{
	if ( m_i64SelectCharacterSec )
	{
		return g_kEventView.GetLocalSecTime(CGameTime::SECOND) - m_i64SelectCharacterSec;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

__int64 PgPlayerPlayTime::GetLastSpecificReward() const
{
	if( m_i64LastSpecificRewardSec )
	{
		return g_kEventView.GetLocalSecTime(CGameTime::SECOND) - m_i64LastSpecificRewardSec;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

int PgPlayerPlayTime::GetAccConnSec_Member()const
{
	return (int)GetThisConnSec_Character() + m_iLastAccConnSec_Member;
}

__int64 PgPlayerPlayTime::GetTotalConnSec_Character()const
{
	return GetThisConnSec_Character() + m_i64TotalPlayTimeSec;
}

__int64 PgPlayerPlayTime::GetSelectCharacterSec()const
{
	return m_i64SelectCharacterSec;
}

__int64 PgPlayerPlayTime::GetLastSpecificRewardSec() const
{
	return m_i64LastSpecificRewardSec;
}

void PgPlayerPlayTime::SetSelectCharacterSec(__int64 SelectCharacterSec)
{
	m_i64SelectCharacterSec = SelectCharacterSec;
}

void PgPlayerPlayTime::SetLasSpecificRewardSec(__int64 LasSpecificRewardSec)
{
	m_i64LastSpecificRewardSec = LasSpecificRewardSec;
}

__int64 PgPlayerPlayTime::GetTotalPlayTimeSec()const
{
	return m_i64TotalPlayTimeSec;
}

int PgPlayerPlayTime::GetExpRate()
{
	return m_iExpRate;
}

int PgPlayerPlayTime::GetMoneyRate()
{
	return m_iMoneyRate;
}

int PgPlayerPlayTime::GetDropRate()
{
	return m_iDropRate;
}

EPPTCondition PgPlayerPlayTime::GetEtcBoolean()
{
	return m_eEtcBoolean;
}

bool PgPlayerPlayTime::IsQuestBegin()const
{
	if( IsQuestFinish() )
		return true;
	if( !(g_kDefPlayTime.IsEnable(PgDefPlayerPlayTimeImpl::EDPTF_NOQUESTBEGIN)) )
		return true;
	
	return false;
}

PgDefPlayerPlayTimeImpl::PgDefPlayerPlayTimeImpl()
{}

PgDefPlayerPlayTimeImpl::~PgDefPlayerPlayTimeImpl()
{}

void PgDefPlayerPlayTimeImpl::Clear()
{
	m_kDef.Clear();
	m_kNextResetTime.Clear();	
	m_ContExp.clear();
	m_ContMoney.clear();
	m_ContItem.clear();
	m_ContQuest.clear();
	m_ContDrop.clear();
}

void PgDefPlayerPlayTimeImpl::SetDef(SPLAYERPLAYTIMEINFO const & kDef)
{
	m_kDef = kDef;
}

bool PgDefPlayerPlayTimeImpl::Build()
{
	if(!m_kDef.bUse)
	{
		return false;
	}

	//분을 초단위로 변환, 분단위보다 초단위로 더 많이 사용
	m_kDef.iResetValue1 *= 60;

	switch(ResetType())
	{
	case EDPTRT_ACCDISCON:
		{
			m_kNextResetTime.Clear();
		}break;
	case EDPTRT_MIN:
		{
			SYSTEMTIME kNowTime;
			g_kEventView.GetLocalTime( &kNowTime );	//현재 시간 구하기

			SYSTEMTIME kInitTime = kNowTime; 

			__int64 i64NowTime = 0i64;
			CGameTime::SystemTime2SecTime( kNowTime, i64NowTime, CGameTime::SECOND );			
			kInitTime.wHour			= 0;
			kInitTime.wMinute		= 0;
			kInitTime.wSecond		= 0;
			kInitTime.wMilliseconds	= 0;

			__int64 i64InitTime = 0i64;
			CGameTime::SystemTime2SecTime( kInitTime, i64InitTime, CGameTime::SECOND );

			__int64 i64DiffTime = i64NowTime - i64InitTime;
			int iCount = 0;
			if(ResetValue1())
			{		
				iCount = static_cast<int>(i64DiffTime) / ResetValue1();
			}
			++iCount;

			SYSTEMTIME kNextTime = kNowTime;
			CGameTime::AddTime(kNextTime, CGameTime::SECOND*ResetValue1()*iCount);

			m_kNextResetTime = kNextTime;
		}break;
	case EDPTRT_DAY:
		{
			SYSTEMTIME kNowTime;
			g_kEventView.GetLocalTime( &kNowTime );	//현재 시간 구하기

			m_kNextResetTime = m_kDef.kResetValue2;
			m_kNextResetTime.Year(kNowTime.wYear%100);
			m_kNextResetTime.Month(kNowTime.wMonth);
			m_kNextResetTime.Day(kNowTime.wDay);

			SYSTEMTIME kNextTime = m_kNextResetTime;

			BM::PgPackedTime kNowTmp(kNowTime);
			BM::PgPackedTime kNextTmp(m_kNextResetTime);
			if(kNextTmp < kNowTmp)
			{
				CGameTime::AddTime(kNextTime, CGameTime::OneDay);
			}

			m_kNextResetTime = kNextTime;
		}break;
	}

	m_ContExp.clear();
	m_ContMoney.clear();
	m_ContItem.clear();
	m_ContQuest.clear();
	m_ContDrop.clear();
	m_ContMsg.clear();
	for(SPLAYERPLAYTIMEINFO::CONT_SUB::const_iterator c_iter = m_kDef.kContSub.begin();
		m_kDef.kContSub.end()!=c_iter;++c_iter)
	{
		if(EDPTS_T_CONTITION==(*c_iter).bType)
		{
			SSUBBODY kBody;
			kBody.bType  = (*c_iter).bTypeSub;
			kBody.iStart = (*c_iter).iValue1*60;
			kBody.iEnd   = (*c_iter).iValue2*60;
			kBody.iValue = (*c_iter).iValue3;
			switch((*c_iter).bTypeSub)
			{
			case EDPTS_TS_EXP:   { m_ContExp.insert(kBody);   }break;
			case EDPTS_TS_MONEY: { m_ContMoney.insert(kBody); }break;
			case EDPTS_TS_ITEM:  { m_ContItem.insert(kBody);  }break;
			case EDPTS_TS_QUEST: { m_ContQuest.insert(kBody); }break;
			case EDPTS_TS_DROP:  { m_ContDrop.insert(kBody);  }break;
			}
		}
		else if(EDPTS_T_MESSAGE==(*c_iter).bType)
		{
			SSUBMSG kMsg;
			kMsg.bType  = (*c_iter).bTypeSub;
			kMsg.iSec   = (*c_iter).iValue1*60;
			kMsg.iStyle = (*c_iter).iValue2;
			kMsg.iNo    = (*c_iter).iValue3;

			m_ContMsg.insert(kMsg);
		}
	}

	m_kDef.kContSub.swap(SPLAYERPLAYTIMEINFO::CONT_SUB());
	return true;
}

bool PgDefPlayerPlayTimeImpl::IsUse() const
{
	return m_kDef.bUse;
}

PgDefPlayerPlayTimeImpl::EDefPlayTimeResetType PgDefPlayerPlayTimeImpl::ResetType() const
{
	return static_cast<EDefPlayTimeResetType>(m_kDef.bResetType);
}

int PgDefPlayerPlayTimeImpl::ResetValue1() const
{
	return m_kDef.iResetValue1;
}

BM::PgPackedTime const& PgDefPlayerPlayTimeImpl::ResetValue2() const
{
	return m_kDef.kResetValue2;
}

BM::PgPackedTime const& PgDefPlayerPlayTimeImpl::LastApplyTime() const
{
	return m_kDef.kLastApply;
}

void PgDefPlayerPlayTimeImpl::LastApplyTime(BM::PgPackedTime const & kTime)
{
	m_kDef.kLastApply = kTime;
}

bool PgDefPlayerPlayTimeImpl::IsEnable(EDefPlayTimeFlag kFlag) const
{
	return (0 != (m_kDef.iFlag&kFlag));
}

bool PgDefPlayerPlayTimeImpl::IsEnable(EDefPlayTimeApplyUser kFlag) const
{
	return (0 != (m_kDef.sApplyUser&kFlag));
}

int PgDefPlayerPlayTimeImpl::PlayTimeMin() const
{
	return m_kDef.iPlayTime;
}

int PgDefPlayerPlayTimeImpl::PlayTimeSec() const
{
	return m_kDef.iPlayTime*60;
}

int PgDefPlayerPlayTimeImpl::CalcRemainSec(int const iAccConSec, int const iAccDisSce) const
{
	switch(ResetType())
	{
	case EDPTRT_ACCDISCON:
		{
			return ResetValue1() - iAccDisSce;
		}break;
	case EDPTRT_MIN:
	case EDPTRT_DAY:
		{
			SYSTEMTIME kNowTime;
			g_kEventView.GetLocalTime( &kNowTime );	//현재 시간 구하기

			__int64 i64NowTime = 0i64;
			CGameTime::SystemTime2SecTime( kNowTime, i64NowTime, CGameTime::SECOND );
			
			__int64 i64NextTime = 0i64;
			CGameTime::SystemTime2SecTime( m_kNextResetTime.operator SYSTEMTIME(), i64NextTime, CGameTime::SECOND );

			__int64 i64DiffTime = i64NextTime - i64NowTime;
			
			return static_cast<int>(i64DiffTime);
		}break;			
	}
	return 0;
}

bool PgDefPlayerPlayTimeImpl::IsResetTime() const
{
	switch(ResetType())
	{
	case EDPTRT_ACCDISCON:
		{
			return false;
		}break;
	case EDPTRT_MIN:
		{
			return true;
		}break;
	case EDPTRT_DAY:
		{
			return true;
		}break;			
	}

	return false;
}

BM::PgPackedTime const & PgDefPlayerPlayTimeImpl::GetNextResetTime() const
{
	return m_kNextResetTime;
}

void PgDefPlayerPlayTimeImpl::SetNextResetTime(BM::PgPackedTime &kTime)
{
	m_kNextResetTime = kTime;
}

void PgDefPlayerPlayTimeImpl::UpdateNextResetTime(BM::PgPackedTime const & kNowTime)
{
	switch(ResetType())
	{
	case EDPTRT_MIN:
		{
			SYSTEMTIME kNextTime = m_kNextResetTime;
			CGameTime::AddTime(kNextTime, CGameTime::SECOND*ResetValue1());

			m_kNextResetTime = kNextTime;
		}break;
	case EDPTRT_DAY:
		{
			SYSTEMTIME kNextTime = m_kNextResetTime.operator SYSTEMTIME(); 
			CGameTime::AddTime(kNextTime, CGameTime::OneDay);

			m_kNextResetTime = kNextTime;
		}break;
	}
}

void PgDefPlayerPlayTimeImpl::WriteToPacket(BM::Stream& kPacket) const
{
	m_kDef.WriteToPacket(kPacket);
	kPacket.Push(m_kNextResetTime);
	kPacket.Push(m_ContExp);
	kPacket.Push(m_ContMoney);
	kPacket.Push(m_ContItem);
	kPacket.Push(m_ContQuest);
	kPacket.Push(m_ContDrop);
	kPacket.Push(m_ContMsg);
}

void PgDefPlayerPlayTimeImpl::ReadFromPacket(BM::Stream& rkPacket)
{
	m_kDef.ReadFromPacket(rkPacket);
	rkPacket.Pop(m_kNextResetTime);
	rkPacket.Pop(m_ContExp);
	rkPacket.Pop(m_ContMoney);
	rkPacket.Pop(m_ContItem);
	rkPacket.Pop(m_ContQuest);
	rkPacket.Pop(m_ContDrop);
	rkPacket.Pop(m_ContMsg);
}

int PgDefPlayerPlayTimeImpl::GetExpRate(int const iAccConnSec) const
{
	CONT_BODY::const_iterator c_iter =  
			find_if(m_ContExp.begin(), m_ContExp.end(), PgPlayerPlayTimeUtil::PgFindNomal(iAccConnSec));
	if(c_iter==m_ContExp.end())
	{
		CONT_BODY::const_reverse_iterator c_riter =
			find_if(m_ContExp.rbegin(), m_ContExp.rend(), PgPlayerPlayTimeUtil::PgFindLastValue(iAccConnSec));
		if(c_riter!=m_ContExp.rend())
		{
			return (*c_riter).iValue;
		}
	}
	else
	{
		return (*c_iter).iValue;
	}

	return 100;
}

int PgDefPlayerPlayTimeImpl::GetMoneyRate(int const iAccConnSec) const
{
	CONT_BODY::const_iterator c_iter =  
			find_if(m_ContMoney.begin(), m_ContMoney.end(), PgPlayerPlayTimeUtil::PgFindNomal(iAccConnSec));
	if(c_iter==m_ContMoney.end())
	{
		CONT_BODY::const_reverse_iterator c_riter =
			find_if(m_ContMoney.rbegin(), m_ContMoney.rend(), PgPlayerPlayTimeUtil::PgFindLastValue(iAccConnSec));
		if(c_riter!=m_ContMoney.rend())
		{
			return (*c_riter).iValue;
		}
	}
	else
	{
		return (*c_iter).iValue;
	}

	return 100;
}

int PgDefPlayerPlayTimeImpl::GetDropRate(int const iAccConnSec) const
{
	CONT_BODY::const_iterator c_iter =  
			find_if(m_ContDrop.begin(), m_ContDrop.end(), PgPlayerPlayTimeUtil::PgFindNomal(iAccConnSec));
	if(c_iter==m_ContDrop.end())
	{
		CONT_BODY::const_reverse_iterator c_riter =
			find_if(m_ContDrop.rbegin(), m_ContDrop.rend(), PgPlayerPlayTimeUtil::PgFindLastValue(iAccConnSec));
		if(c_riter!=m_ContDrop.rend())
		{
			return (*c_riter).iValue;
		}
	}
	else
	{
		return (*c_iter).iValue;
	}

	return 100;
}

bool PgDefPlayerPlayTimeImpl::IsTakeUpItem(int const iAccConnSec) const
{
	CONT_BODY::const_iterator c_iter =  
			find_if(m_ContItem.begin(), m_ContItem.end(), PgPlayerPlayTimeUtil::PgFindNomal(iAccConnSec));
	if(c_iter==m_ContItem.end())
	{
		CONT_BODY::const_reverse_iterator c_riter =
			find_if(m_ContItem.rbegin(), m_ContItem.rend(), PgPlayerPlayTimeUtil::PgFindLastValue(iAccConnSec));
		if(c_riter!=m_ContItem.rend())
		{
			return static_cast<bool>((*c_riter).iValue);
		}
	}
	else
	{
		return static_cast<bool>((*c_iter).iValue);
	}

	return true;
}

bool PgDefPlayerPlayTimeImpl::IsQuestFinish(int const iAccConnSec) const
{
	CONT_BODY::const_iterator c_iter =  
			find_if(m_ContQuest.begin(), m_ContQuest.end(), PgPlayerPlayTimeUtil::PgFindNomal(iAccConnSec));
	if(c_iter==m_ContQuest.end())
	{
		CONT_BODY::const_reverse_iterator c_riter =
			find_if(m_ContQuest.rbegin(), m_ContQuest.rend(), PgPlayerPlayTimeUtil::PgFindLastValue(iAccConnSec));
		if(c_riter!=m_ContQuest.rend())
		{
			return static_cast<bool>((*c_riter).iValue);
		}
	}
	else
	{
		return static_cast<bool>((*c_iter).iValue);
	}

	return true;
}

bool PgDefPlayerPlayTimeImpl::IsTimeOver(int const iAccConnSec) const
{
	return (IsUse() && (iAccConnSec > m_kDef.iPlayTime*60));
}

int PgDefPlayerPlayTimeImpl::GetNextMsgSec(int const iAccConnSec, SSUBMSG & kMsg) const
{
	int iNextSec = 0;
	for(PgDefPlayerPlayTimeImpl::CONT_MSG::const_iterator it = m_ContMsg.begin();it != m_ContMsg.end();++it)
	{
		int iTempSec = -1;

		switch( (*it).bType )
		{
		case 1:
			{//몇 분 마다
				if( (*it).iSec > 0 )
				{
					int iCnt = iAccConnSec / (*it).iSec;
					++iCnt;

					iTempSec = (*it).iSec * iCnt;
				}
			}break;
		case 2:
			{//종료시간을 기준으로 (+,-)분 전/후
				iTempSec = g_kDefPlayTime.PlayTimeSec() + (*it).iSec;
			}break;
		case 3:
			{//종료후 몇 분간격 반복메시지
				int iDiff = iAccConnSec - g_kDefPlayTime.PlayTimeSec();
				if(iDiff > 0)
				{
					int iCnt = iDiff / (*it).iSec;
					++iCnt;
					iTempSec = g_kDefPlayTime.PlayTimeSec() + ((*it).iSec*iCnt);
				}
			}break;
		case 4:
			{//플레이시간을 기준
				iTempSec =  ((*it).iSec >= iAccConnSec ? (*it).iSec : 0);
			}break;
		}

		if(iAccConnSec < iTempSec)
		{
			if(iNextSec==0)
			{
				iNextSec = iTempSec;
				kMsg = (*it);
			}
			else
			{
				if(iNextSec > iTempSec)
				{
					iNextSec = iTempSec;
					kMsg = (*it);
				}
			}
		}
	}

	return iNextSec;
}

bool PgPlayerPlayTime::IsAdult(BM::DBTIMESTAMP_EX const &dtUserBirth)
{
	if( dtUserBirth.IsNull() )
	{//! 생년월일 등록 안되있다면 미성년자로 취급
		return false;
	}
	if( g_kLocal.IsServiceRegion(LOCAL_MGR::NC_CHINA) )
	{
		BM::DBTIMESTAMP_EX kDefaultDate;
		kDefaultDate.year = 2000; kDefaultDate.month = 1; kDefaultDate.day = 1;
		if (kDefaultDate == dtUserBirth)
		{
			return false;
		}
	}
	BM::DBTIMESTAMP_EX dtDiffTime, dtLocalTime;
	dtLocalTime.SetLocalTime();
	dtLocalTime.year -= 18;
	if ( dtLocalTime < dtUserBirth )
	{
		return false;
	}
	return true;
}

PgDefPlayerPlayTime::PgDefPlayerPlayTime()
{
}

PgDefPlayerPlayTime::~PgDefPlayerPlayTime()
{
}


void PgDefPlayerPlayTime::WriteToPacket(BM::Stream& kPacket)const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	Instance()->WriteToPacket(kPacket);
}

void PgDefPlayerPlayTime::ReadFromPacket(BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	Instance()->ReadFromPacket(rkPacket);
}

bool PgDefPlayerPlayTime::IsEnable(PgDefPlayerPlayTimeImpl::EDefPlayTimeFlag kFlag) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->IsEnable(kFlag);
}

bool PgDefPlayerPlayTime::IsUse() const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->IsUse();
}

bool PgDefPlayerPlayTime::IsEnable(PgDefPlayerPlayTimeImpl::EDefPlayTimeApplyUser kFlag) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->IsEnable(kFlag);
}

PgDefPlayerPlayTimeImpl::EDefPlayTimeResetType PgDefPlayerPlayTime::ResetType() const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->ResetType();
}

int PgDefPlayerPlayTime::ResetValue1() const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->ResetValue1();
}

int PgDefPlayerPlayTime::GetExpRate(int const iAccConnSec) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetExpRate(iAccConnSec);
}

int PgDefPlayerPlayTime::GetMoneyRate(int const iAccConnSec) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetMoneyRate(iAccConnSec);
}

int PgDefPlayerPlayTime::GetDropRate(int const iAccConnSec) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetDropRate(iAccConnSec);
}

bool PgDefPlayerPlayTime::IsTakeUpItem(int const iAccConnSec) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->IsTakeUpItem(iAccConnSec);
}

bool PgDefPlayerPlayTime::IsQuestFinish(int const iAccConnSec) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->IsQuestFinish(iAccConnSec);
}

bool PgDefPlayerPlayTime::IsTimeOver(int const iAccConnSec) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->IsTimeOver(iAccConnSec);
}

int PgDefPlayerPlayTime::PlayTimeSec() const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->PlayTimeSec();
}

void PgDefPlayerPlayTime::LastApplyTime(BM::PgPackedTime const & kTime)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	Instance()->LastApplyTime();
}

BM::PgPackedTime const& PgDefPlayerPlayTime::LastApplyTime() const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->LastApplyTime();
}

void PgDefPlayerPlayTime::SetDef(SPLAYERPLAYTIMEINFO const & kDef)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->SetDef(kDef);
}

bool PgDefPlayerPlayTime::Build()
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->Build();
}

BM::PgPackedTime const & PgDefPlayerPlayTime::GetNextResetTime() const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetNextResetTime();
}

void PgDefPlayerPlayTime::UpdateNextResetTime(BM::PgPackedTime const & kNowTime)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->UpdateNextResetTime(kNowTime);
}

int PgDefPlayerPlayTime::CalcRemainSec(int const iAccConSec, int const iAccDisSce) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->CalcRemainSec(iAccConSec, iAccDisSce);
}

int PgDefPlayerPlayTime::PlayTimeMin() const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->PlayTimeMin();
}

int PgDefPlayerPlayTime::GetNextMsgSec(int const iAccConnSec, PgDefPlayerPlayTimeImpl::SSUBMSG & kMsg) const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetNextMsgSec(iAccConnSec, kMsg);
}

