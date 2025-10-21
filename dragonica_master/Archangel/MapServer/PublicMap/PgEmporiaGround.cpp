#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"
#include "Lohengrin/PacketType.h"
#include "Variant/PgEventView.h"
#include "PgEffectAbilTable.h"
#include "PgAction.h"
#include "PgEmporiaGround.h"

PgEmporiaGround::PgEmporiaGround()
{

}

PgEmporiaGround::~PgEmporiaGround()
{

}

void PgEmporiaGround::Clear()
{
	m_kEmporiaInfo = PgEmporia();
	PgGround::Clear();
}

bool PgEmporiaGround::Clone( PgEmporiaGround *pkGround )
{
	BM::CAutoMutex kLock( m_kRscMutex );
	m_kEmporiaInfo = pkGround->m_kEmporiaInfo;
	return PgGround::Clone( dynamic_cast<PgGround*>(pkGround) );
}

bool PgEmporiaGround::Clone( PgGround *pkGround )
{
	BM::CAutoMutex kLock( m_kRscMutex );
	PgEmporiaGround *pkEmporia = dynamic_cast<PgEmporiaGround*>(pkGround);
	if ( pkEmporia )
	{
		return Clone( pkEmporia );
	}

	PgGround::Clone( pkGround );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgEmporiaGround::ReadFromPacket_BaseInfo( BM::Stream &kPacket )
{
	BM::CAutoMutex kLock( m_kRscMutex );
	m_kEmporiaInfo.ReadFromPacket( kPacket );

	SEmporiaGuildInfo kOwner;
	m_kEmporiaInfo.GetOwner( kOwner );

	SGroundOwnerInfo kGroundOwnerInfo( SGroundOwnerInfo::OWNER_GUILD, kOwner.kGuildID );
	SetChangeGroundOwnerInfo( kGroundOwnerInfo, true );
}

void PgEmporiaGround::SendMapLoadComplete( PgPlayer *pkUser )
{
	PgGround::SendMapLoadComplete( pkUser );

	BM::Stream kPacket( PT_M_C_NFY_EMPORIA_FUNCTION );
	m_kEmporiaInfo.WriteToPacket_Function( kPacket );
	pkUser->Send( kPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );
}

bool PgEmporiaGround::IsAccess( PgPlayer *pkPlayer )
{
	BM::CAutoMutex kLock( m_kRscMutex );
	return m_kEmporiaInfo.IsAccess( pkPlayer->GuildGuid() );
}

int PgEmporiaGround::CheckEmporiaFunctionAccess( CUnit *pkUnit, CONT_EMPORIA_FUNC::key_type const nFuncNo )const
{
	BM::CAutoMutex kLock( m_kRscMutex );

	PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if ( pkPlayer )
	{
		if ( !m_kEmporiaInfo.IsOwner( pkPlayer->GuildGuid() ) )
		{
			// 오너 길드가 아닌 경우에는 이용 할 수 없다.
			return 70097;
		}

		int iCheckLevel = 20;
		g_kVariableContainer.Get( EVar_Kind_Guild, EVar_Emporia_CanUseFunction, iCheckLevel );
		if ( iCheckLevel > pkPlayer->GetAbil( AT_LEVEL ) )
		{
			// 레벨이 안되서 이용 할 수 없습니다.
			return 72024;
		}

		SEmporiaFunction kFunc;
		if ( true == m_kEmporiaInfo.GetFunc().GetFunction( nFuncNo, kFunc ) )
		{
			__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );

			if ( i64NowTime < kFunc.i64ExpirationDate )
			{
				switch ( nFuncNo )
				{
				case EFUNC_CAPSULE_CREATURE:
					{// 캡슐 생성기

						// 하루에 한번만 사용 할수 있는데 사용이 가능한지 체크를 하고
						SYSTEMTIME kNowTime;
						CGameTime::SecTime2SystemTime( i64NowTime, kNowTime, CGameTime::DEFAULT );

						kNowTime.wHour = 0;
						kNowTime.wMinute = 0;
						kNowTime.wSecond =0;
						kNowTime.wMilliseconds = 0;

						__int64 i64NowStartTime = 0i64;
						CGameTime::SystemTime2SecTime( kNowTime, i64NowStartTime, CGameTime::DEFAULT );

						if ( kFunc.i64ExtValue >= i64NowStartTime )
						{
							// 하루에 한번만 이용이 가능 합니다.
							return 70095;
						}

						// 길드 오너인지 체크해야 하는데 Contents에서 체크 할 수 있다.
						BM::Stream kPacket( PT_M_N_REQ_GET_GUILD_MEMBER_GRADE, g_kProcessCfg.ChannelNo() );
						GroundKey().WriteToPacket( kPacket );
						kPacket.Push( pkPlayer->GetID() );
						kPacket.Push( PT_C_M_REQ_USE_EMPORIA_FUNCTION );
						kPacket.Push( nFuncNo );
						::SendToGuildMgr( kPacket );
						return -1;// 리턴값이 0보다 작으면 컨텐츠로 체크를 요청
					}break;
				default:
					{
					}break;
				}
				return 0;// 리턴값이 0이면 OK
			}
			else
			{
				// 기간이 만료 되었습니다.
				return 1973;
			}
		}
	}
	// 설치 하지 않은 기능은 이용 할 수 없습니다.
	return 70094;
}

void PgEmporiaGround::ProcessEmporiaFunction( CUnit *pkUnit, CONT_EMPORIA_FUNC::key_type const nFuncNo, BM::Stream * const pkPacket, BYTE const byGuildGrade )
{
	BM::CAutoMutex kLock( m_kRscMutex );

	PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if ( pkPlayer )
	{
		SEmporiaFunction kFunc;
		if ( true == m_kEmporiaInfo.GetFunc().GetFunction( nFuncNo, kFunc ) )
		{
			__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );

			if ( i64NowTime < kFunc.i64ExpirationDate )
			{
				switch( nFuncNo )
				{
				case EFUNC_CAPSULE_CREATURE:
					{// 캡슐 생성기	

						// 하루에 한번만 사용 할수 있는데 사용이 가능한지 체크를 하고
						SYSTEMTIME kNowTime;
						CGameTime::SecTime2SystemTime( i64NowTime, kNowTime, CGameTime::DEFAULT );

						kNowTime.wHour = 0;
						kNowTime.wMinute = 0;
						kNowTime.wSecond =0;
						kNowTime.wMilliseconds = 0;

						__int64 i64NowStartTime = 0i64;
						CGameTime::SystemTime2SecTime( kNowTime, i64NowStartTime, CGameTime::DEFAULT );

						if ( kFunc.i64ExtValue < i64NowStartTime )
						{// 하루가 지났네~~ 사용이 가능해
							if ( GMG_Owner < byGuildGrade )
							{
								// 권한이 없습니다.
								pkPlayer->SendWarnMessage( 70096 );
							}
							else
							{
								PgBase_Item kItem;
								SItemPos kItemPos;

								PgAction_PopEmporiaCapsule kAction( pkPlayer, m_kEmporiaInfo, nFuncNo );
								if ( true == kAction.PopItem( kItem ) )
								{
									CONT_PLAYER_MODIFY_ORDER kOrder;

									SPMOD_Insert_Fixed kAddData( kItem, kItemPos, true );
									SPMO kIMO( IMET_INSERT_FIXED, pkPlayer->GetID(), kAddData );
									kOrder.push_back(kIMO);

									SPMO kIMOFunc( IMET_EMPORIA_FUNCTION, pkPlayer->GetID() );
									kIMOFunc.m_kPacket.Push( m_kEmporiaInfo.GetKey() );
									kIMOFunc.m_kPacket.Push( nFuncNo );
									kIMOFunc.m_kPacket.Push( i64NowTime );// New Use Time
									kOrder.push_back( kIMOFunc );

									BM::Stream kAddOnPacket;
									kAddOnPacket.Push( kFunc.i64ExtValue );// Old Use Time

									PgAction_ReqModifyItem kItemModifyAction( CIE_EmporiaFunction, GroundKey(), kOrder );
									if ( true == kItemModifyAction.DoAction( pkUnit, NULL ) )
									{
										kFunc.i64ExtValue = i64NowTime;
										m_kEmporiaInfo.GetFunc().AddFunction( nFuncNo, kFunc );

										//아이템 지급 알림
										pkPlayer->SendWarnMessage(790510,EL_Normal);
									}
								}
							}
						}
						else
						{
							// 하루에 한번만 이용이 가능 합니다.
							pkPlayer->SendWarnMessage( 70095 );
						}
					}break;
				case EFUNC_WELL_OF_CURE:
					{
						SActArg kActArg;
						PgGroundUtil::SetActArgGround(kActArg, this);

						int const iEffectNo = PgAction_PopEmporiaCapsule::GetEmporiaFuncValue( m_kEmporiaInfo.GetKey(), nFuncNo );
						if ( iEffectNo )
						{
							SEffectCreateInfo kCreate;
							kCreate.eType = EFFECT_TYPE_BLESSED;
							kCreate.iEffectNum = iEffectNo;
							kCreate.kActArg = kActArg;
							kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
							pkPlayer->AddEffect( kCreate );
						}
					}break;
				default:
					{

					}break;
				}
			}
		}
	}
}
