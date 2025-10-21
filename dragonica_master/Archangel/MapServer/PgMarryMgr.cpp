#include "stdafx.h"
#include "PgMarryMgr.h"
#include "Global.h"

void PgMarryMgrImpl::Clear()
{
	i64TotalMoney(0);
	bActionState(EM_NONE);
	iTextGroup(0);
	m_kGndKey.Clear();
	m_kCharGuid = BM::GUID::NullData();
	m_kCoupleGuid = BM::GUID::NullData();
	m_kLastCheckTime = 0;
	m_kTotalLastCheckTime = 0;
	m_kMarryCheckTime = 0;
}

void PgMarryMgrImpl::Init(SGroundKey const & kGndKey, BM::GUID &kCharGuid, BM::GUID &kCoupleGuid, std::wstring &kName, std::wstring &kCoupleName, int iGender1, int iGender2)
{
	GndKey(kGndKey);
	CharGuid(kCharGuid);
	CoupleGuid(kCoupleGuid);
	CharName(kName);
	CoupleName(kCoupleName);
	Gender1(iGender1);
	Gender2(iGender2);

	DWORD const dwNowTime = BM::GetTime32();

	m_kTotalLastCheckTime = dwNowTime;
	m_kMarryCheckTime = dwNowTime;
	SetActionState(EM_TALK);
}

bool PgMarryMgrImpl::SetMoney(__int64 const iMoney)
{
	if( 0 < iMoney )
	{
		m_ki64TotalMoney += iMoney;
	}
	else
	{
		return false;
	}
	return true;
}

bool PgMarryMgrImpl::GetTotalMoney(__int64 & iTotalMoney) const
{
	iTotalMoney = i64TotalMoney();
	return true;
}

void PgMarryMgrImpl::Tick()
{
	if( !BM::TimeCheck(m_kMarryCheckTime, 5000) )
	{
		return;
	}

	if( 0 < m_kTotalLastCheckTime )
	{
		if( BM::TimeCheck(m_kTotalLastCheckTime, 60000*30) )
		{
			SetActionState(EM_MARRY_ACTION);
		}
	}

	switch( bActionState() )
	{
	case EM_MARRY:
		{
			// 결혼
			BM::Stream kPacket;

			kPacket.Push( PT_T_C_NFY_MARRY );
			GndKey().WriteToPacket(kPacket);
			kPacket.Push( static_cast<BYTE>(EM_MARRY) );
			kPacket.Push( CharGuid() );
			kPacket.Push( CoupleGuid() );
			kPacket.Push( GndKey() );
			SendToGround(GndKey(), kPacket);

			SetActionState(EM_NONE);
		}break;
	case EM_NONE:
		{
		}break;
	case EM_TALK:
		{
			int const iTextNo = GetTextNo( iTextGroup() );
			if( 0 < iTextNo )
			{
				++m_kiTextGroup;

				SMS_HEADER kSMSHeader;
				kSMSHeader.eType = static_cast<eSMSType>(SMS_IN_GROUND);
				kSMSHeader.eCType = static_cast<eCardType>(MARRY_TALK);
				kSMSHeader.iUseItemNo = iTextNo;

				BM::Stream kPacket(PT_T_C_NFY_BULLHORN);
				kSMSHeader.WriteToPacket(kPacket);
				SendToGround(GndKey(), kPacket);
			}
			else
			{
				SetActionState(EM_ACTION);
			}
		}break;
	case EM_ACTION:
		{
			BM::Stream kPacket;

			kPacket.Push( PT_T_C_NFY_MARRY );
			GndKey().WriteToPacket(kPacket);
			kPacket.Push( static_cast<BYTE>(EM_ACTION) );

			kPacket.Push(PT_C_M_REQ_MARRY_COMMAND);
			kPacket.Push((BYTE)MC_ActionPlay);
			kPacket.Push(GndKey());

			SendToGround(GndKey(), kPacket);

			SetActionState(EM_MONSTER);
		}break;
	case EM_MONSTER:
		{
			int const iMaxMonsterCount = 100;
			__int64 iTotalMoney = 0;

			if( true == GetTotalMoney(iTotalMoney) )
			{
				__int64 const iResultGold = iTotalMoney; //static_cast<__int64>(70 * (iTotalMoney/100.0f));

				int iMonsterCount = static_cast<int>( (iResultGold / iGoldToBronze) );
				if( 0 < iMonsterCount )
				{
					if( iMaxMonsterCount < iMonsterCount )
					{
						iMonsterCount = iMaxMonsterCount;
					}
					int const iMonsterNo = 6001000;

					BM::Stream kPacket(PT_T_C_NFY_MARRY);
					GndKey().WriteToPacket(kPacket);
					kPacket.Push(static_cast<BYTE>(EM_MONSTER));
					kPacket.Push(iMonsterNo);
					kPacket.Push(iMonsterCount);
					kPacket.Push(CharGuid());
					kPacket.Push(GndKey());

					SendToGround(GndKey(), kPacket);
				}
			}

			m_kLastCheckTime = BM::GetTime32();

			SetActionState(EM_ACTION_END);
		}break;
	case EM_ACTION_END:
		{
			if( BM::TimeCheck(m_kLastCheckTime, 60000*3) )
			{
				SetActionState(EM_MARRY_ACTION);
			}
		}break;
	case EM_MARRY_ACTION:
		{
			// 결혼 연출
			BM::Stream kPacket;

			kPacket.Push( PT_T_C_NFY_MARRY );
			GndKey().WriteToPacket(kPacket);
			kPacket.Push( static_cast<BYTE>(EM_MARRY_ACTION) );

			kPacket.Push(PT_C_M_REQ_MARRY_COMMAND);
			kPacket.Push((BYTE)MC_ActionPlayEnd);
			kPacket.Push(GndKey());

			SendToGround(GndKey(), kPacket);


			// 부부정보 갱신
			if( BM::GUID::IsNotNull(CharGuid()) )
			{
				BM::Stream kNfyPacket(PT_C_N_REQ_COUPLE_COMMAND, CharGuid());
				kNfyPacket.Push( (BYTE)CC_SendGndCoupleStatus );
				SendToCoupleMgr(kNfyPacket);
			}

			if( BM::GUID::IsNotNull(CoupleGuid()) )
			{
				BM::Stream kNfyPacket(PT_C_N_REQ_COUPLE_COMMAND, CoupleGuid());
				kNfyPacket.Push( (BYTE)CC_SendGndCoupleStatus );
				SendToCoupleMgr(kNfyPacket);
			}

			Clear();
		}break;
	default:
		{
		}break;
	}
}

bool PgMarryMgrImpl::SetActionState(BYTE eType)
{
	bActionState( eType );
	return true;
}

bool PgMarryMgrImpl::GetActionState(BYTE & eType) const
{
	eType = bActionState();
	return true;
}

int PgMarryMgrImpl::GetTextNo(int iTextGroup)
{
	CONT_MARRYTEXT const *kContMarryText = NULL;
	g_kTblDataMgr.GetContDef(kContMarryText);
	if( kContMarryText )
	{
		CONT_MARRYTEXT::const_iterator iter = kContMarryText->find(iTextGroup);
		if( kContMarryText->end() != iter )
		{
			CONT_MARRYTEXT::mapped_type const iTextNo = (*iter).second;

			return static_cast<int>(iTextNo);
		}
	}
	return 0;
}

bool PgMarryMgrImpl::IsMarryCouple(BM::GUID const &kCharGuid)
{
	bool bMarryCouple = false;

	if( BM::GUID::IsNotNull(CharGuid()) )
	{
		if( CharGuid() == kCharGuid )
		{
			bMarryCouple = true;
		}
	}
	if( BM::GUID::IsNotNull(CoupleGuid()) )
	{
		if( CoupleGuid() == kCharGuid )
		{
			bMarryCouple = true;
		}
	}
	return bMarryCouple;
}

void PgMarryMgrImpl::GetMarryCoupleGuid(BM::GUID &kCharGuid, BM::GUID &kCoupleGuid)
{
	kCharGuid = CharGuid();
	kCoupleGuid = CoupleGuid();
}

int PgMarryMgrImpl::GetEffectMALE()
{
	return 90000501;
}

int PgMarryMgrImpl::GetEffectFEMALE()
{
	return 90000601;
}

bool PgMarryMgrImpl::IsMarryUse()
{
	return (0 < i64TotalMoney());
}

//===============================================================================================================================================================================================

void PgMarryMgr::Clear()
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_,true);
	Instance()->Clear();
}

void PgMarryMgr::Init(SGroundKey const & kGndKey, BM::GUID &kCharGuid, BM::GUID &kCoupleGuid, std::wstring &kName, std::wstring &kCoupleName, int iGender1, int iGender2)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_,true);
	Instance()->Init(kGndKey, kCharGuid, kCoupleGuid, kName, kCoupleName, iGender1, iGender2);
}

bool PgMarryMgr::SetMoney(__int64 const iMoney)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_,true);
	return Instance()->SetMoney(iMoney);
}

bool PgMarryMgr::GetTotalMoney(__int64 & iTotalMoney)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->GetTotalMoney(iTotalMoney);
}

void PgMarryMgr::Tick()
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	Instance()->Tick();
}

bool PgMarryMgr::SetActionState(BYTE eType)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_,true);
	return Instance()->SetActionState(eType);
}

bool PgMarryMgr::GetActionState(BYTE & eType)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->GetActionState(eType);
}

bool PgMarryMgr::IsMarryCouple(BM::GUID const &kCharGuid)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->IsMarryCouple(kCharGuid);
}

int PgMarryMgr::GetEffectMALE()
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->GetEffectMALE();
}

int PgMarryMgr::GetEffectFEMALE()
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->GetEffectFEMALE();
}

bool PgMarryMgr::IsMarryUse()
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->IsMarryUse();
}

void PgMarryMgr::GetMarryCoupleGuid(BM::GUID &kCharGuid, BM::GUID &kCoupleGuid)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->GetMarryCoupleGuid(kCharGuid, kCoupleGuid);
}