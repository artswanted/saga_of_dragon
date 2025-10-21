#include "stdafx.h"
#include "Lohengrin/PacketStruct.h"
#include "Collins/Log.h"
#include "Variant/PgLogUtil.h"
#include "global.h"
#include "PgGlobalMarryMgr.h"

__int64 const i100Gold = 1000000i64; // 100골드
__int64 const i1Gold = 10000i64; // 1골드

PgGlobalMarryMgr::PgGlobalMarryMgr()
{
	Clear();
}

PgGlobalMarryMgr::~PgGlobalMarryMgr()
{
	Clear();
}

void PgGlobalMarryMgr::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);

	i64TotalMoney(0);
	m_kGndKey.Clear();
	m_kCharGuid = BM::GUID::NullData();
	m_kCoupleGuid = BM::GUID::NullData();
	m_kNpcTalkCheckTime = 0;
	m_kVillageGndKey.Clear();
	m_kMarryStartCheckTime = 0;
}

bool PgGlobalMarryMgr::IsMarryUse()
{
	BM::CAutoMutex kLock(m_kMutex);

	return ( 0 < i64TotalMoney() );
}

void PgGlobalMarryMgr::SetGndKey(SGroundKey const &kGroundKey)
{
	GndKey( kGroundKey );
}

bool PgGlobalMarryMgr::IsMarryCouple(BM::GUID const &kCharGuid)
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

void PgGlobalMarryMgr::Init(SGroundKey const & kGndKey, BM::GUID &kCharGuid, BM::GUID &kCoupleGuid, std::wstring &kName, std::wstring &kCoupleName, int iGender1, int iGender2)
{
	BM::CAutoMutex kLock(m_kMutex);

	GndKey(kGndKey);
	CharGuid(kCharGuid);
	CoupleGuid(kCoupleGuid);
	CharName(kName);
	CoupleName(kCoupleName);
	Gender1(iGender1);
	Gender2(iGender2);
	m_kNpcTalkCheckTime = BM::GetTime32();
	m_kMarryStartCheckTime = BM::GetTime32();
	VillageGndKey(kGndKey);
}

void PgGlobalMarryMgr::SetMoney(__int64 i64SendAdd)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( 0 < i64SendAdd )
	{
		i64TotalMoney(i64SendAdd);
	}
}

void PgGlobalMarryMgr::AddMoney(__int64 i64SendAdd)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( 0 < i64SendAdd )
	{		
		if( i100Gold > i64TotalMoney() )
		{
			m_ki64TotalMoney += i64SendAdd;
			m_ki64TotalMoney = std::min<__int64>(m_ki64TotalMoney, i100Gold);
		}
	}
}

bool PgGlobalMarryMgr::IsAddTotalMoneyMax(__int64 i64SendAdd)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( i100Gold >= (i64TotalMoney() + (i64SendAdd * i1Gold)) )
	{
		return true;
	}
	return false;
}

void PgGlobalMarryMgr::Tick()
{
	BM::CAutoMutex kLock(m_kMutex);

	if( !GndKey().IsEmpty() )
	{
		if( BM::TimeCheck(m_kNpcTalkCheckTime, 10000) )
		{
			BM::Stream kPacket(PT_T_M_MARRY_COMMAND, static_cast<BYTE>(MC_NpcTalk));
			kPacket.Push( GndKey() );
			kPacket.Push( CharName() );
			kPacket.Push( CoupleName() );
			// 마을 NPC에게 머릿말 보여주자.
			if( !VillageGndKey().IsEmpty() )
			{
				g_kServerSetMgr.Locked_SendToGround(VillageGndKey(),kPacket,true);
			}
		}
	}
	
	if( (!GndKey().IsEmpty()) && (!VillageGndKey().IsEmpty()) )
	{
		// 결혼 신청을 하고 시작 안하면 같다.
		if( (GndKey() == VillageGndKey()) && (0 != MarryStartCheckTime()) )
		{
			DWORD const dwEndDelayTime = (60000*30);

			DWORD const iDiffTime = (BM::GetTime32() - MarryStartCheckTime());
			if( iDiffTime >= dwEndDelayTime )
			{
				if( (BM::GUID::IsNotNull(CharGuid())) && (BM::GUID::IsNotNull(CoupleGuid())) )
				{
					BM::Stream kPacket(PT_T_M_MARRY_COMMAND, static_cast<BYTE>(MC_TimeOut));
					kPacket.Push(CharGuid());
					kPacket.Push(CoupleGuid());

					g_kServerSetMgr.Locked_SendToGround(VillageGndKey(), kPacket, true);
				}

				if( (!CharName().empty()) && (!CoupleName().empty()) )
				{
					BM::Stream kRetPacket(PT_M_C_ANS_MARRY_COMMAND);
					kRetPacket.Push( static_cast<BYTE>(MC_End) );
					kRetPacket.Push(CharName());
					kRetPacket.Push(CoupleName());

					if( BM::GUID::IsNotNull(CharGuid()) )
					{
						g_kServerSetMgr.Locked_SendToUser(CharGuid(), kRetPacket, false);
					}

					if( BM::GUID::IsNotNull(CoupleGuid()) )
					{
						g_kServerSetMgr.Locked_SendToUser(CoupleGuid(), kRetPacket, false);
					}
				}

				Clear();
			}
		}
	}
}

bool PgGlobalMarryMgr::ProcessMsg(unsigned short const usType, BM::Stream* const pkMsg)
{
	BM::CAutoMutex kLock(m_kMutex);

	switch(usType)
	{
	case PT_C_M_REQ_MARRY_COMMAND:
		{
			BYTE cCmdType = 0;
			pkMsg->Pop( cCmdType );

			switch( cCmdType )
			{
			case MC_Init:
				{
					BM::GUID		kOwnerGuid;
					SGroundKey		kCastGndKey;
					__int64			i64SendAdd;
					BM::GUID		kCharGuid;
					BM::GUID		kCoupleGuid;
					std::wstring	kName;
					std::wstring	kCoupleName;
					int				iGender1;
					int				iGender2;

					pkMsg->Pop(kOwnerGuid);
					pkMsg->Pop(kCastGndKey);

					pkMsg->Pop(i64SendAdd);
					pkMsg->Pop(kCharGuid);
					pkMsg->Pop(kCoupleGuid);
					pkMsg->Pop(kName);
					pkMsg->Pop(kCoupleName);
					pkMsg->Pop(iGender1);
					pkMsg->Pop(iGender2);

					SetMoney(i64SendAdd);
					Init(kCastGndKey, kCharGuid, kCoupleGuid, kName, kCoupleName, iGender1, iGender2);

					// 전체 공지를 한다.
					BM::Stream kPacket(PT_T_M_MARRY_COMMAND, cCmdType);
					kPacket.Push(kName);
					kPacket.Push(kCoupleName);
					
					g_kServerSetMgr.Locked_SendToGround(kCastGndKey,kPacket,true);
				}break;
			case MC_AnsSendMoney:
				{
					BM::GUID	kOwnerGuid;
					SGroundKey	kCastGndKey;
					int iGoldMoney = 0;

					pkMsg->Pop(kOwnerGuid);
					pkMsg->Pop(kCastGndKey);
					pkMsg->Pop(iGoldMoney);

					AddMoney(iGoldMoney);
				}break;
			case MC_ReqMarry://결혼신청
				{
					bool bUse = IsMarryUse();

					BM::GUID	kOwnerGuid;
					SGroundKey	kCastGndKey;
					int iGoldMoney = 0;

					pkMsg->Pop(kOwnerGuid);
					pkMsg->Pop(kCastGndKey);
					pkMsg->Pop(iGoldMoney);

					int iMsgNo = 0;
					if( true == bUse )
					{						
						if( true == IsMarryCouple( kOwnerGuid ) )
						{
							// 이미 결혼 신청 상태
							iMsgNo = 450131;
						}
						else
						{
							// 딴넘이 하네.
							iMsgNo = 450126;
						}			
					}

					BM::Stream kPacket(PT_T_M_MARRY_COMMAND, cCmdType);
					kPacket.Push(kOwnerGuid);
					kPacket.Push(kCastGndKey);
					kPacket.Push(iGoldMoney);
					kPacket.Push(iMsgNo);
					
					g_kServerSetMgr.Locked_SendToGround(kCastGndKey,kPacket,true);
				}break;
			case MC_ReqSendMoney://기부하기
				{
					bool bUse = IsMarryUse();

					BM::GUID	kOwnerGuid;
					SGroundKey	kCastGndKey;
					int iGoldMoney = 0;

					pkMsg->Pop(kOwnerGuid);
					pkMsg->Pop(kCastGndKey);
					pkMsg->Pop(iGoldMoney);

					int iMsgNo = 0;
					if( false == bUse )//누가 쓴다.
					{						
						// 결혼식을 안하네? 진행 중 일경우만 내자
						iMsgNo = 450130;
					}
					else if( false == IsAddTotalMoneyMax(iGoldMoney) )
					{
						// 기부금 초과 햇삼
						iMsgNo = 450147;
					}

					BM::Stream kPacket(PT_T_M_MARRY_COMMAND, cCmdType);
					kPacket.Push(kOwnerGuid);
					kPacket.Push(kCastGndKey);
					kPacket.Push(iGoldMoney);					
					kPacket.Push(iMsgNo);
					
					g_kServerSetMgr.Locked_SendToGround(kCastGndKey,kPacket,true);
				}break;
			case MC_SetActionState://시작하기
				{
					bool bUse = IsMarryUse();

					BM::GUID	kOwnerGuid;
					SGroundKey	kCastGndKey;
					int iType = 0;

					pkMsg->Pop(kOwnerGuid);
					pkMsg->Pop(kCastGndKey);
					pkMsg->Pop(iType);

					int iMsgNo = 0;
					if( false == IsMarryCouple(kOwnerGuid) )
					{
						// 결혼 신청은 하지?
						iMsgNo = 450132;						
					}
					else
					{
						// 내가 신청중이면 시작 하자
						SetGndKey( kCastGndKey );
					}

					if( static_cast<BYTE>(EM_TALK) == static_cast<BYTE>(iType) )
					{
						// 자료 옮겨라(시작)
						BM::Stream kPacket(PT_T_M_MARRY_COMMAND, cCmdType);
						kPacket.Push(kOwnerGuid);
						kPacket.Push(kCastGndKey);					
						kPacket.Push(iType);

						kPacket.Push(i64TotalMoney());
						kPacket.Push(CharGuid());
						kPacket.Push(CoupleGuid());
						kPacket.Push(CharName());
						kPacket.Push(CoupleName());
						kPacket.Push(Gender1());
						kPacket.Push(Gender2());
						kPacket.Push(iMsgNo);
						
						g_kServerSetMgr.Locked_SendToGround(kCastGndKey,kPacket,true);
					}
					else if( static_cast<BYTE>(EM_NONE) == static_cast<BYTE>(iType) )
					{
						if( true == IsMarryCouple(kOwnerGuid) ) // 결혼 신청을 한 경우 처리
						{
							// 결혼식장 / 마을 일수도 있다.(초기화 시키는 경우)
							if( false == GndKey().IsEmpty() )
							{
								// 결혼식 시작 했던 경우 지워주자(결혼식 취소)
								// 결혼식장 초기화
								BM::Stream kPacket(PT_T_M_MARRY_COMMAND, cCmdType);
								kPacket.Push(kOwnerGuid);
								kPacket.Push(GndKey());					
								kPacket.Push(iType);

								kPacket.Push(i64TotalMoney());
								kPacket.Push(CharGuid());
								kPacket.Push(CoupleGuid());
								kPacket.Push(CharName());
								kPacket.Push(CoupleName());
								kPacket.Push(Gender1());
								kPacket.Push(Gender2());
								kPacket.Push(iMsgNo);
								
								g_kServerSetMgr.Locked_SendToGround(kCastGndKey,kPacket,true);
							}						

							// 커플에게 통보(취소 통보)
							iMsgNo = 450139;

							BM::Stream kRetPacket(PT_M_C_ANS_MARRY_COMMAND);
							kRetPacket.Push( static_cast<BYTE>(MC_SetActionState) );
							kRetPacket.Push(iMsgNo);

							g_kServerSetMgr.Locked_SendToUser(CharGuid(), kRetPacket, false);
							g_kServerSetMgr.Locked_SendToUser(CoupleGuid(), kRetPacket, false);

							Clear();
						}
					}
				}break;
			case MC_ActionPlay:
				{					
					SGroundKey	kCastGndKey;

					pkMsg->Pop(kCastGndKey);

					BM::Stream kPacket(PT_T_M_MARRY_COMMAND, cCmdType);
					kPacket.Push(kCastGndKey);
					kPacket.Push(CharName());
					kPacket.Push(Gender1());
					kPacket.Push(CoupleName());
					kPacket.Push(Gender2());

					g_kServerSetMgr.Locked_SendToGround(kCastGndKey,kPacket,true);
				}break;
			case MC_ActionPlayEnd:
				{
					SGroundKey	kCastGndKey;

					pkMsg->Pop(kCastGndKey);

					BM::Stream kPacket(PT_T_M_MARRY_COMMAND, cCmdType);
					kPacket.Push(kCastGndKey);
					// 마지막 연출. 하객들에게 통보한다.
					g_kServerSetMgr.Locked_SendToGround(kCastGndKey,kPacket,true);

					// 전체 공지를 한다.
					BM::Stream kNoticePacket(PT_T_M_MARRY_COMMAND, static_cast<BYTE>(MC_End));
					kNoticePacket.Push(CharName());
					kNoticePacket.Push(CoupleName());
					
					g_kServerSetMgr.Locked_SendToGround(kCastGndKey,kNoticePacket,true);

					Clear();
				}break;
			case MC_TotalMoney:
				{
					// 기부함 총액 보기
					BM::GUID	kOwnerGuid;
					SGroundKey	kCastGndKey;

					pkMsg->Pop(kOwnerGuid);
					pkMsg->Pop(kCastGndKey);

					BM::Stream kPacket(PT_T_M_MARRY_COMMAND, cCmdType);
					kPacket.Push(kOwnerGuid);
					kPacket.Push(kCastGndKey);
					kPacket.Push( i64TotalMoney() );

					g_kServerSetMgr.Locked_SendToGround(kCastGndKey,kPacket,true);
				}break;
			case MC_EffectCheck:
				{
					// 복장을 신랑/신부인지 체크해서 갈아 입히기
					BM::GUID	kOwnerGuid;
					SGroundKey	kCastGndKey;

					pkMsg->Pop(kOwnerGuid);
					pkMsg->Pop(kCastGndKey);

					BM::Stream kPacket(PT_T_M_MARRY_COMMAND, cCmdType);
					kPacket.Push(kOwnerGuid);
					kPacket.Push(kCastGndKey);
					kPacket.Push( IsMarryCouple( kOwnerGuid ) );

					g_kServerSetMgr.Locked_SendToGround(kCastGndKey,kPacket,true);
				}break;
			default:
				{
				}break;
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("invalid packet type=") << usType);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}
	}
	return true;
}
