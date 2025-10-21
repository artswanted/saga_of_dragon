#include "stdafx.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgWarGround.h"
#include "PgWarMode.h"

PgPvPSetting PgWarMode::ms_kSetting;

PgWarMode::PgWarMode()
:	m_dwGamePlayTime(0)
,	m_bEndGame(false)
,	m_bNoEnemy(false)
,	m_iWinTeam(TEAM_NONE)
,	m_iBattleLevel(0)
,	m_i64NextEventTime(0i64)
,	m_kGamePoint(0)
{

}

bool PgWarMode::ParseXml( char const* szPath )
{
	TiXmlDocument kXmlDoc(szPath);
	if( !kXmlDoc.LoadFile(szPath) )
	{
		INFO_LOG(BM::LOG_LV5, __FL__<<L"LoadFile Error: "<<UNI(szPath) );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	char *pTemp = NULL;
	TiXmlElement const *pkElement = kXmlDoc.FirstChildElement();
	while ( pkElement )
	{
		if ( !strcmp( pkElement->Value(), "PVP") )
		{
			TiXmlElement const *pkChildElement = pkElement->FirstChildElement();
			while( pkChildElement )
			{
				if ( !strcmp( pkChildElement->Value(), "TIME") )
				{
					TiXmlAttribute const *pkAttr = pkChildElement->FirstAttribute();
					while( pkAttr )
					{
						if ( !strcmp( pkAttr->Name(), "READY") )
						{
							// Ready타임은 1초를 더해서 파싱해야 한다.
							ms_kSetting.SetTime( PVP_TIME_READY, 1000 + 1000 * (DWORD)::strtol( pkAttr->Value(), &pTemp, 10 ));
						}
						else if ( !strcmp( pkAttr->Name(), "ROUNDTERM") )
						{
							ms_kSetting.SetTime( PVP_TIME_ROUNDTERM, 1000 * (DWORD)::strtol( pkAttr->Value(), &pTemp, 10 ));
						}
						else if ( !strcmp( pkAttr->Name(), "RESULT") )
						{
							ms_kSetting.SetTime( PVP_TIME_RESULT, 1000 * (DWORD)::strtol( pkAttr->Value(), &pTemp, 10 ));
						}
						else if ( !strcmp( pkAttr->Name(), "REVIVE") )
						{
							ms_kSetting.SetTime( PVP_TIME_REVIVE, 1000 * (DWORD)::strtol( pkAttr->Value(), &pTemp, 10 ));
						}
						else if ( !strcmp( pkAttr->Name(), "ENTRYSELECT") )
						{
							ms_kSetting.SetTime( PVP_TIME_ENTRYSELECT, 1000 * (DWORD)::strtol( pkAttr->Value(), &pTemp, 10 ));
						}
						else
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"<TIME "<<UNI(pkAttr->Name())<<L"=\""<<UNI(pkAttr->Value())<<L"\"> : Attribute Error" );
						}

						pkAttr = pkAttr->Next();
					}
				}
				else if ( !strcmp( pkChildElement->Value(), "TYPE") )
				{	
					TiXmlAttribute const *pkAttr = pkChildElement->FirstAttribute();
					if ( !strcmp( pkAttr->Name(),"VALUE" ) )
					{
						EPVPTYPE kType = (EPVPTYPE)(::strtol( pkAttr->Value(), &pTemp, 10 ));

						bool const bParseOK = ParseXml( kType, pkChildElement->FirstChildElement() );
						if( !bParseOK )
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"<TYPE VALUE=\""<<kType<<L"\"> : Parse Failed" );
						}
					}
					else
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"<TYPE "<<UNI(pkAttr->Name())<<L"=\""<<UNI(pkAttr->Value())<<L"\"> : Attribute Error" );
					}
				}
				pkChildElement = pkChildElement->NextSiblingElement();
			}
		}
		pkElement = pkElement->NextSiblingElement();
	}
	return true;
}

bool PgWarMode::ParseXml( EPVPTYPE const kType, TiXmlElement const *pkElement )
{
	char* pTemp = NULL;
	while ( pkElement )
	{
		if ( !strcmp( pkElement->Value(), "POINT") )
		{
			// 재귀 호출
			ParseXml( kType, pkElement->FirstChildElement() );
		}
		else if ( !strcmp( pkElement->Value(), "KILL") )
		{
			TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();
			while ( pkAttr )
			{
				if ( !strcmp( pkAttr->Name(),"DEFAULT" ) )
				{
					int iPoint = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					ms_kSetting.Set( kType, PVP_ST_KILL_POINT, iPoint );
				}
				else if ( !strcmp( pkAttr->Name(), "WEIGHT" ) )
				{
					int iPoint = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					ms_kSetting.Set( kType, PVP_ST_KILL_POINT_WEIGHT, iPoint );
				}
				pkAttr = pkAttr->Next();
			}
		}
		else if ( !strcmp( pkElement->Value(), "DEATH") )
		{
			TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();
			while ( pkAttr )
			{
				if ( !strcmp( pkAttr->Name(),"DEFAULT" ) )
				{
					int iPoint = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					ms_kSetting.Set( kType, PVP_ST_DEATH_POINT, iPoint );
				}
				else if ( !strcmp( pkAttr->Name(), "WEIGHT" ) )
				{
					int iPoint = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					ms_kSetting.Set( kType, PVP_ST_DEATH_POINT_WEIGHT, iPoint );
				}
				pkAttr = pkAttr->Next();
			}
		}
		else if ( !strcmp( pkElement->Value(), "RANK") )
		{
			TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();
			int iRank = 0;
			int iBonus = 0;
			while ( pkAttr )
			{
				if ( !strcmp( pkAttr->Name(),"ID" ) )
				{
					iRank = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
				}
				else if ( !strcmp( pkAttr->Name(), "POINT" ) )
				{
					iBonus = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
				}
				pkAttr = pkAttr->Next();
			}
			ms_kSetting.Set( kType, PVP_ST_RANK_POINT_CHK+iRank, iBonus );
		}
		else if ( !strcmp( pkElement->Value(), "WIN") )
		{
			TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();
			while ( pkAttr )
			{
				if ( !strcmp( pkAttr->Name(),"ROUND" ) )
				{
					int iTemp = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					ms_kSetting.Set( kType, PVP_ST_WIN_ROUND_POINT, iTemp );
				}
				else if ( !strcmp( pkAttr->Name(), "GAME" ) )
				{
					int iTemp = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					ms_kSetting.Set( kType, PVP_ST_WIN_GAME_POINT, iTemp );
				}
				pkAttr = pkAttr->Next();
			}
		}
		else if ( !strcmp( pkElement->Value(), "LOSE") )
		{
			TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();
			while ( pkAttr )
			{
				if ( !strcmp( pkAttr->Name(),"ROUND" ) )
				{
					int iTemp = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					ms_kSetting.Set( kType, PVP_ST_LOSE_ROUND_POINT, iTemp );
				}
				else if ( !strcmp( pkAttr->Name(), "GAME" ) )
				{
					int iTemp = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					ms_kSetting.Set( kType, PVP_ST_LOSE_GAME_POINT, iTemp );
				}
				pkAttr = pkAttr->Next();
			}
		}
		else if ( !strcmp( pkElement->Value(), "SPECIAL") )
		{
			WORD usPoint = 0;
			WORD usCount = 0;
			BYTE kSType = PVP_SPT_MAX;

			TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();
			while ( pkAttr )
			{
				if ( !strcmp( pkAttr->Name(),"POINT" ) )
				{
					usPoint = (WORD)(::strtol( pkAttr->Value(), &pTemp, 10 ));
				}
				else if ( !strcmp( pkAttr->Name(), "KILL" ) )
				{
					usCount = (WORD)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					kSType = PVP_SPT_KILL;
				}
				else if ( !strcmp( pkAttr->Name(), "DEATH" ) ) 
				{
					usCount = (WORD)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					kSType = PVP_SPT_DEATH;
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"Not Found Attribute <SPECIAL "<<UNI(pkAttr->Name())<<L"=\""<<UNI(pkAttr->Value())<<L"\"" );
				}
				pkAttr = pkAttr->Next();
			}
			ms_kSetting.SetSpecial( kType, kSType, usCount, usPoint );
		}
		else if ( !strcmp( pkElement->Value(), "TIME") )
		{
			TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();
			int iValue = 0;
			while ( pkAttr )
			{
				if ( !strcmp( pkAttr->Name(),"READY" ) )
				{
					iValue = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					ms_kSetting.Set( kType, PVP_ST_TIME_READY, iValue );
				}
				pkAttr = pkAttr->Next();
			}
		}
		else if ( !strcmp( pkElement->Value(), "OCCUPY_POINT") )
		{
			TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();
			int iValue = 0;
			while ( pkAttr )
			{
				if ( !strcmp( pkAttr->Name(),"VALUE" ) )
				{
					iValue = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					ms_kSetting.Set( kType, PVP_ST_OCCUPY_POINT, iValue );
				}
				else if ( !strcmp( pkAttr->Name(),"NOT_IDLE_RATE" ) )
				{
					iValue = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					ms_kSetting.Set( kType, PVP_ST_OCCUPY_POINT_NOT_IDLE_RATE, iValue );
				}
				pkAttr = pkAttr->Next();
			}
		}
		else if ( !strcmp( pkElement->Value(), "VICTORY_POINT") )
		{
			TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();
			int iIntervalTime = 0;
			while ( pkAttr )
			{
				if ( !strcmp( pkAttr->Name(),"INTERVAL_TIME" ) )
				{
					iIntervalTime = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
					ms_kSetting.Set( kType, PVP_ST_VICTORY_INTERVAL_TIME, iIntervalTime );
				}
				pkAttr = pkAttr->Next();
			}
		}
		else if( !strcmp( pkElement->Value(), "INSITEM" ) )
		{
			TiXmlAttribute const * pAttr = pkElement->FirstAttribute();
			int ItemPoint = 0;
			while( pAttr )
			{
				if( !strcmp( pAttr->Name(), "POINT") )
				{
					ItemPoint = (int)(::strtol(pAttr->Value(), &pTemp, 10));
				}
				pAttr = pAttr->Next();
			}
			ms_kSetting.Set( kType, PVP_ST_INSITEM_POINT, ItemPoint);
		}
		else if ( !strcmp( pkElement->Value(), "NEXT_ENTITY") )
		{
			TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();
			int iLevel = 0;
			int iValue = 0;
			while ( pkAttr )
			{
				if ( !strcmp( pkAttr->Name(),"LEVEL" ) )
				{
					iLevel = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
				}
				else if ( !strcmp( pkAttr->Name(),"VALUE" ) )
				{
					iValue = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
				}
				pkAttr = pkAttr->Next();
			}
			ms_kSetting.Set( kType, PVP_ST_NEXT_EXP_ENTITY_LV0+iLevel, iValue );
		}
		else if ( !strcmp( pkElement->Value(), "DEFAULT_ENTITY_EFFECT") )
		{
			TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();
			int iValue = 0;
			while ( pkAttr )
			{
				if ( !strcmp( pkAttr->Name(),"NO" ) )
				{
					iValue = (int)(::strtol( pkAttr->Value(), &pTemp, 10 ));
				}
				pkAttr = pkAttr->Next();
			}
			ms_kSetting.Set( kType, PVP_ST_DEFAULT_ENTITY_EFFECT, iValue );
		}
		else if( !strcmp( pkElement->Value(), "DROPREDBEAR" ) )
		{
			TiXmlAttribute const * pAttr = pkElement->FirstAttribute();
			int iValue = 0;
			while( pAttr )
			{
				if( !strcmp( pAttr->Name(), "ITEMNO" ) )
				{
					iValue = (int)(::strtol(pAttr->Value(), &pTemp, 10));
				}
				pAttr = pAttr->Next();
			}
			ms_kSetting.Set( kType, PVP_ST_LOVE_MODE_DROP_RED_BEAR, iValue );
		}
		else if( !strcmp( pkElement->Value(), "DROPBLUEBEAR" ) )
		{
			TiXmlAttribute const * pAttr = pkElement->FirstAttribute();
			int iValue = 0;
			while( pAttr )
			{
				if( !strcmp( pAttr->Name(), "ITEMNO" ) )
				{
					iValue = (int)(::strtol(pAttr->Value(), &pTemp, 10));
				}
				pAttr = pAttr->Next();
			}
			ms_kSetting.Set( kType, PVP_ST_LOVE_MODE_DROP_BLUE_BEAR, iValue );
		}
		else if( !strcmp( pkElement->Value(), "CAPTUREPOINT" ) )
		{
			TiXmlAttribute const * pAttr = pkElement->FirstAttribute();
			int iValue = 0;
			while( pAttr )
			{
				if( !strcmp(pAttr->Name(), "POINT") )
				{
					iValue = (int)(::strtol(pAttr->Value(), &pTemp, 10));
				}
				pAttr = pAttr->Next();
			}
			ms_kSetting.Set( kType, PVP_ST_CAPTURE_DEC_VALUE, iValue );
		}
		else if( !strcmp( pkElement->Value(), "DESTROYFENCE" ) )
		{
			TiXmlAttribute const * pAttr = pkElement->FirstAttribute();
			int ItemPoint = 0;
			while( pAttr )
			{
				if( !strcmp( pAttr->Name(), "POINT") )
				{
					ItemPoint = (int)(::strtol(pAttr->Value(), &pTemp, 10));
				}
				pAttr = pAttr->Next();
			}
			ms_kSetting.Set( kType, PVP_ST_DESTROYFENCE, ItemPoint);
		}
		else if( !strcmp( pkElement->Value(), "BEARUSERKILL" ) )
		{
			TiXmlAttribute const * pAttr = pkElement->FirstAttribute();
			int ItemPoint = 0;
			while( pAttr )
			{
				if( !strcmp( pAttr->Name(), "POINT") )
				{
					ItemPoint = (int)(::strtol(pAttr->Value(), &pTemp, 10));
				}
				pAttr = pAttr->Next();
			}
			ms_kSetting.Set( kType, PVP_ST_BEARUSERKILL, ItemPoint);
		}
		else if( !strcmp( pkElement->Value(), "GETROUNDSCORE" ) )
		{
			TiXmlAttribute const * pAttr = pkElement->FirstAttribute();
			int ItemPoint = 0;
			while( pAttr )
			{
				if( !strcmp( pAttr->Name(), "POINT") )
				{
					ItemPoint = (int)(::strtol(pAttr->Value(), &pTemp, 10));
				}
				pAttr = pAttr->Next();
			}
			ms_kSetting.Set( kType, PVP_ST_GETROUNDSCORE, ItemPoint);
		}
		else if( !strcmp( pkElement->Value(), "PICKUPBEAR" ) )
		{
			TiXmlAttribute const * pAttr = pkElement->FirstAttribute();
			int ItemPoint = 0;
			while( pAttr )
			{
				if( !strcmp( pAttr->Name(), "POINT") )
				{
					ItemPoint = (int)(::strtol(pAttr->Value(), &pTemp, 10));
				}
				pAttr = pAttr->Next();
			}
			ms_kSetting.Set( kType, PVP_ST_PICKUPBEAR, ItemPoint);
		}
		else if( !strcmp( pkElement->Value(), "EVENTSCENE" ) )
		{
			TiXmlAttribute const * pAttr = pkElement->FirstAttribute();
			while( pAttr )
			{
				if( !strcmp( pAttr->Name(), "TIME") )
				{
					ms_kSetting.SetTime(PVP_TIME_EVENT_SCENE_RESULT, 1000 * (DWORD)(::strtol(pAttr->Value(), &pTemp, 10)));
				}
				pAttr = pAttr->Next();
			}
		}
		else if( !strcmp( pkElement->Value(), "REVIVE" ) )
		{
			TiXmlAttribute const * pAttr = pkElement->FirstAttribute();
			while( pAttr )
			{
				if( !strcmp( pAttr->Name(), "TIME") )
				{
					ms_kSetting.SetTime( PVP_TIME_REVIVE, 1000 * (DWORD)::strtol( pAttr->Value(), &pTemp, 10 ));
				}
				pAttr = pAttr->Next();
			}
		}

		pkElement = pkElement->NextSiblingElement();
	}
	return true;
}

int PgWarMode::GetEnemyTeam( int const iTeam )
{
	switch ( iTeam )
	{
	case TEAM_RED:{return TEAM_BLUE;}break;
	case TEAM_BLUE:{return TEAM_RED;}break;
	default:{}break;
	}
	return TEAM_NONE;
}

HRESULT PgWarMode::Init( BM::Stream &kPacket, PgWarGround *pkGnd )
{
	m_bEndGame = false;
	m_bNoEnemy = false;
	m_iWinTeam = TEAM_NONE;
	m_i64NextEventTime = 0i64;
	m_iBattleLevel = 0;

	GndKey(pkGnd->GroundKey());

	ClearObserver();
	BM::CObserver< BM::Stream* >* pkObserver = dynamic_cast< BM::CObserver<BM::Stream*>* >(pkGnd);
	if ( pkObserver )
	{
		VAttach(pkObserver);
		return S_OK;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__<<L"Critical Error" );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgWarMode::SetRoundWinTeam(int WinTeam, PgWarGround * pGnd)
{
	//
}