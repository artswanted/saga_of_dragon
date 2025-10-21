#include "stdafx.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgAction.h"
#include "PgEmporiaGround.h"

bool PgEmporiaGround::RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket )
{
	switch(usType)
	{
	case PT_N_M_NFY_EMPORIA_INFO:
		{
			ReadFromPacket_BaseInfo( *pkPacket );

			BM::Stream kPacket( PT_M_C_NFY_EMPORIA_FUNCTION );
			m_kEmporiaInfo.WriteToPacket_Function( kPacket );
			Broadcast( kPacket );
		}break;
	case PT_N_M_NFY_EMPORIA_FUNCTION_REMOVE:
		{
			CONT_EMPORIA_FUNCKEY kContDelFunc;
			pkPacket->Pop( kContDelFunc );

			PgEmporiaFunction &kFunc = m_kEmporiaInfo.GetFunc();
			CONT_EMPORIA_FUNCKEY::const_iterator itr = kContDelFunc.begin();
			for ( ; itr != kContDelFunc.end() ; ++itr )
			{
				kFunc.RemoveFunction( *itr );
			}

			BM::Stream kPacket( PT_M_C_NFY_EMPORIA_FUNCTION_REMOVE, kContDelFunc );
			Broadcast( kPacket );
		}break;
	case PT_N_M_ANS_GET_GUILD_MEMBER_GRADE:
		{
			BM::GUID kCharGuid;
			bool bSuccess = false;
			BYTE byMemberGrade = GMG_Membmer;
			BM::Stream::DEF_STREAM_TYPE kType = 0;

			pkPacket->Pop( kCharGuid );
			pkPacket->Pop( bSuccess );
			pkPacket->Pop( byMemberGrade );
			pkPacket->Pop( kType );

			CUnit *pkUnit = GetUnit( kCharGuid );
			if ( pkUnit )
			{
				switch ( kType )
				{
				case PT_C_M_REQ_USE_EMPORIA_FUNCTION:
					{
						CONT_EMPORIA_FUNC::key_type nFuncNo = 0;
						pkPacket->Pop( nFuncNo );

						if ( true == bSuccess )
						{
							ProcessEmporiaFunction( pkUnit, nFuncNo, pkPacket, byMemberGrade );
						}
					}break;
				}
			}
		}break;
	default:
		{
			return PgGround::RecvGndWrapped( usType, pkPacket );
		}
	}
	return true;
}

bool PgEmporiaGround::VUpdate( CUnit* pkUnit, WORD const usType, BM::Stream* pkNfy )
{
	switch(usType)
	{
	case PT_C_M_REQ_USE_EMPORIA_FUNCTION:
		{
			CONT_EMPORIA_FUNC::key_type nFuncNo = 0;
			pkNfy->Pop( nFuncNo );
			
			int const iErrorCode = CheckEmporiaFunctionAccess( pkUnit, nFuncNo );
			if ( iErrorCode > 0 )
			{
				// 리턴값이 0보다 크면 에러메세지다
				pkUnit->SendWarnMessage( iErrorCode );
			}
			else if ( 0 == iErrorCode )
			{
				// 리턴값이 0보다 작으면 Contents서버로부터 길드 등급을 요청 한 거다.
				ProcessEmporiaFunction( pkUnit, nFuncNo, pkNfy, GMG_Membmer );
			}
		}break;
	case PT_C_M_REQ_ITEM_RARITY_UPGRADE:
		{
			BM::GUID kNpcGuid;
			pkNfy->Pop(kNpcGuid);
//			CUnit* pkNpcUnit = GetNPC(kNpcGuid);
//			if( PgGroundUtil::IsCanTalkableRange(pkUnit, kNpcGuid, pkNpcUnit, GetGroundNo(), NMT_SoulCraft, __F_P__(PT_C_M_REQ_ITEM_RARITY_UPGRADE), __LINE__) )
			
			int const iErrorCode = CheckEmporiaFunctionAccess( pkUnit, EFUNC_SOULCRAFT );
			if ( iErrorCode > 0 )
			{
				pkUnit->SendWarnMessage( iErrorCode );
			}
			else if ( 0 == iErrorCode )
			{
				int const iAddRate = PgAction_PopEmporiaCapsule::GetEmporiaFuncValue( m_kEmporiaInfo.GetKey(), EFUNC_SOULCRAFT );
				PgAction_ItemRarityUpgrade kAction( GroundKey(), *pkNfy, iAddRate );
				kAction.DoAction(pkUnit, NULL);
			}
		}break;
	case PT_C_M_REQ_ITEM_PLUS_UPGRADE:
		{
			BM::GUID kNpcGuid;
			pkNfy->Pop(kNpcGuid);

			int const iErrorCode = CheckEmporiaFunctionAccess( pkUnit, EFUNC_GOOD_SMITHY );
			if ( iErrorCode > 0 )
			{
				pkUnit->SendWarnMessage( iErrorCode );
			}
			else if ( 0 == iErrorCode )
			{
				int const iAddRate = PgAction_PopEmporiaCapsule::GetEmporiaFuncValue( m_kEmporiaInfo.GetKey(), EFUNC_SOULCRAFT );
				PgAction_ItemPlusUpgrade kAction( GroundKey(), *pkNfy, iAddRate );
				kAction.DoAction( pkUnit, NULL );
			}
		}break;
	case PT_C_M_REQ_STORE_ITEM_LIST:
		{
			BM::GUID kStoreGuid;
            BYTE byType = 0;
			pkNfy->Pop(kStoreGuid);
			pkNfy->Pop(byType);

			int const iErrorCode = CheckEmporiaFunctionAccess( pkUnit, EFUNC_BLACKMARKET );
			if ( iErrorCode > 0 )
			{
				pkUnit->SendWarnMessage( iErrorCode );
			}
			else
			{
				PgAction_ReqStoreItemList kAction( kStoreGuid, byType );
				kAction.DoAction( pkUnit, NULL );
			}
		}break;
	case PT_C_M_REQ_STORE_ITEM_BUY:
		{
			BM::GUID kStoreGuid;
            BYTE byType = 0;
			BYTE bySecondType = 0;
			int iItemNo = 0;
			int iCount = 1;// 아이템은 한개로 강제
			pkNfy->Pop(kStoreGuid);
            pkNfy->Pop(byType);
			pkNfy->Pop(bySecondType);
			pkNfy->Pop(iItemNo);
//			pkNfy->Pop(iCount);

			int const iErrorCode = CheckEmporiaFunctionAccess( pkUnit, EFUNC_BLACKMARKET );
			if ( iErrorCode > 0 )
			{
				pkUnit->SendWarnMessage( iErrorCode );
			}
			else
			{
				PgAction_ReqStoreItemBuy kAction( kStoreGuid, byType, bySecondType, iItemNo, iCount, GroundKey(), GetEventAbil() );
				kAction.DoAction(pkUnit, NULL);
			}
		}break;
	case PT_C_M_REMOVEITEM:	// 아이템을 버렸다.
		{
			SItemPos kPos;
			int iDeleteType = 0;
			pkNfy->Pop(kPos);
			pkNfy->Pop(iDeleteType);

			int const iChangeSoulRate = ( 0 == CheckEmporiaFunctionAccess( pkUnit, EFUNC_SOULCRAFT ) ? 5000 : 0 );
			
			ContItemRemoveOrder kContOrder;

			SItemRemoveOrder kElement;
			kElement.kCasterPos = kPos;
			kContOrder.push_back(kElement);

			PgAction_ReqRemoveInvItem kAction(GroundKey(), kContOrder,(iDeleteType ? IRT_BREAK_REMOVE : IRT_NORMAL_REMOVE), iChangeSoulRate);
			kAction.DoAction(pkUnit, NULL);
		}break;
	default:
		{
			return PgGround::VUpdate( pkUnit, usType, pkNfy );
		}break;
	}
	return true;
}