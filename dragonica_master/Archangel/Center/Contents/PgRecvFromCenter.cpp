#include "stdafx.h"
#include <set>
#include "PgRecvFromMap.h"
#include "variant/ItemBagMgr.h"
#include "Lohengrin/PacketType.h"
#include "Lohengrin/PacketStruct.h"
#include "PgTask_Contents.h"
#include "Lohengrin/PacketType.h"
#include "Lohengrin/PacketStruct.h"
#include "Variant/PgEventView.h"
#include "PgTask_Contents.h"
#include "Variant/PgMCtrl.h"
#include "tinyxml/tinyxml.h"
#include "Variant/PgControlDefMgr.h"
#include "PgMissionMgr.h"

void CALLBACK OnRecvFromCenter(BM::Stream * const pkPacket)
{
	SERVER_IDENTITY kRecvSI;
	PACKET_ID_TYPE wType = 0;
	pkPacket->Pop(wType);

	switch( wType )
	{
	case PT_T_N_NFY_MISSION_RANK_DATA:
		{
			SMissionKey kKey;
			CONT_MISSION_RANK kData;
			pkPacket->Pop(kKey);
			PU::TLoadArray_M(*pkPacket,kData);
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("unhandled packet [") << wType << _T("]") );
//			g_kRankMgr.InitRank(kKey, kData);
		}break;
	case PT_T_N_NFY_MISSION_REPORT_DATA:
		{
/*			CONT_MISSION_REPORT kMissionRank;
			PU::TLoadTable_MM(*pkPacket,kMissionRank);

			CONT_MISSION_REPORT::iterator rank_itr;
			for ( rank_itr=kMissionRank.begin(); rank_itr!=kMissionRank.end(); ++rank_itr )
			{
				PgMission_Rank_Base* pkRank = g_kRankMgr.GetRank(rank_itr->first);
				if ( pkRank )
				{
					assert(pkRank->GetData().Init(rank_itr->first,rank_itr->second));		
				}
			}
*/			//INFO_LOG(BM::LOG_LV6,_T("[Recv Mission REPORT] Count : %u"),kMissionRank.size());
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("unhandled packet [") << wType << _T("]") );
		}break;
	case PT_T_N_ANS_COUPLE_COMMAND:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("[PT_T_N_ANS_COUPLE_COMMAND] Cannnot handle message") );
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			OnRecvWrappedFromCenter(wType, pkPacket);
		}break;
	}
}
