#ifndef WEAPON_VARIANT_ALRAMMISSION_ALRAMMISSIONCHECKER_H
#define WEAPON_VARIANT_ALRAMMISSION_ALRAMMISSIONCHECKER_H

#include "PgPlayer.h"
#include "AlramMissionAction_Traits.h"

namespace ALRAM_MISSION
{
	template < eEventType kType >
	class PgClassAlramMissionEvent
	{
	public:
		template< typename T_RET, typename T >
		void operator()( PgPlayer *pkPlayer, int const iCount, T_RET * const pkRetFunc, T const &kCheck )
		{
			PgAlramMission &rkAlramMission = pkPlayer->GetAlramMission();

			switch ( rkAlramMission.GetState() )
			{
			case PgAlramMission::STATE_CHECKTIME:
				{
					HRESULT const hRet = rkAlramMission.OnTick( g_kEventView.GetServerElapsedTime() );
					if ( RET_NONE != hRet )
					{
						return;
					}
				} // break을 사용하지 않음
			case PgAlramMission::STATE_IDLE:
				{
					try
					{
						TBL_DEF_ALRAM_MISSION const * const pkDef = rkAlramMission.GetDef();
						if ( !pkDef )
						{
							throw RET_FAIL;
						}

						int iParam;
						size_t index = 0;
						size_t iSuccessCount = 0;
						VEC_ALRAM_MISSION_ACTION::const_iterator action_itr = pkDef->kActionList.begin();
						for ( ; action_itr != pkDef->kActionList.end() ; ++action_itr, ++index )
						{
							if ( !rkAlramMission.GetParam( index, iParam ) )
							{
								throw RET_FAIL;
							}

							if ( action_itr->iCount <= iParam )
							{
								if ( action_itr->byFailType )
								{
									throw RET_FAIL;
								}
								++iSuccessCount;
							}
							else
							{
								if (	true == PgAlramMissionActionCheck_Type()( Loki::Int2Type<kType>(), static_cast<eEventType>(action_itr->iActionType) ) 
									&&	true == PgAlramMissionActionCheck_Object< T >()( kCheck, action_itr->iObjectType, action_itr->i64ObjectValue ))
								{
									iParam += iCount;
									if ( iParam >= action_itr->iCount )
									{
										if ( action_itr->byFailType )
										{
											throw RET_FAIL;
										}

										iParam = action_itr->iCount;
										++iSuccessCount;
									}

									rkAlramMission.SetParam( index, iParam );

									BM::Stream kEventPacket( PT_M_C_NFY_ALRAMMISSION_PARAM, index );
									kEventPacket.Push( iParam );
									pkPlayer->Send( kEventPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );
								}
								else
								{
									if ( PgAlramMission::TYPE_FAIL2 == action_itr->byFailType )
									{// 이건 성공으로 쳐줘야 한다.
										++iSuccessCount;
									}
								}
							}

							if ( iSuccessCount == pkDef->kActionList.size() )
							{
								rkAlramMission.SetState( PgAlramMission::STATE_SUCCEED );
								pkRetFunc->CallAlramReward( pkPlayer );
							}
						}
					}
					catch( eEventResult eRet )
					{
						if ( RET_FAIL == eRet )
						{
							rkAlramMission.SetState( PgAlramMission::STATE_FAILED );
							BM::Stream kFaildPacket( PT_M_C_NFY_ALRAMMISSION_END );
							pkPlayer->Send( kFaildPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );
						}
					}
				}break;
			case PgAlramMission::STATE_SUCCEED:
			case PgAlramMission::STATE_FAILED:
			default:
				{
				}break;
			}
		}

		template< typename T_RET, typename T, typename T2 >
		void operator()( PgPlayer *pkPlayer, int const iCount, T_RET * const pkRetFunc, T const &kCheck, T2 const &kCheck2 )
		{
			PgAlramMission &rkAlramMission = pkPlayer->GetAlramMission();

			switch ( rkAlramMission.GetState() )
			{
			case PgAlramMission::STATE_CHECKTIME:
				{
					HRESULT const hRet = rkAlramMission.OnTick( g_kEventView.GetServerElapsedTime() );
					if ( RET_NONE != hRet )
					{
						return;
					}
				} // break을 사용하지 않음
			case PgAlramMission::STATE_IDLE:
				{
					try
					{
						TBL_DEF_ALRAM_MISSION const * const pkDef = rkAlramMission.GetDef();
						if ( !pkDef )
						{
							throw RET_FAIL;
						}

						int iParam;
						size_t index = 0;
						size_t iSuccessCount = 0;
						VEC_ALRAM_MISSION_ACTION::const_iterator action_itr = pkDef->kActionList.begin();
						for ( ; action_itr != pkDef->kActionList.end() ; ++action_itr, ++index )
						{
							if ( !rkAlramMission.GetParam( index, iParam ) )
							{
								throw RET_FAIL;
							}

							if ( action_itr->iCount <= iParam )
							{
								if ( action_itr->byFailType )
								{
									throw RET_FAIL;
								}
								++iSuccessCount;
							}
							else
							{
								if ( true == PgAlramMissionActionCheck_Type()( Loki::Int2Type<kType>(), static_cast<eEventType>(action_itr->iActionType) ) )
								{
									if (	true == PgAlramMissionActionCheck_Object< T >()( kCheck, action_itr->iObjectType, action_itr->i64ObjectValue ) 
										||	true == PgAlramMissionActionCheck_Object< T2 >()( kCheck2, action_itr->iObjectType, action_itr->i64ObjectValue )
										)
									{
										iParam += iCount;
										if ( iParam >= action_itr->iCount )
										{
											if ( action_itr->byFailType )
											{
												throw RET_FAIL;
											}

											iParam = action_itr->iCount;
											++iSuccessCount;
										}

										rkAlramMission.SetParam( index, iParam );

										BM::Stream kEventPacket( PT_M_C_NFY_ALRAMMISSION_PARAM, index );
										kEventPacket.Push( iParam );
										pkPlayer->Send( kEventPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );
									}
									else
									{
										if ( PgAlramMission::TYPE_FAIL2 == action_itr->byFailType )
										{// 이건 성공으로 쳐줘야 한다.
											++iSuccessCount;
										}
									}
								}
								else
								{
									if ( PgAlramMission::TYPE_FAIL2 == action_itr->byFailType )
									{// 이건 성공으로 쳐줘야 한다.
										++iSuccessCount;
									}
								}
							}

							if ( iSuccessCount == pkDef->kActionList.size() )
							{
								rkAlramMission.SetState( PgAlramMission::STATE_SUCCEED );
								pkRetFunc->CallAlramReward( pkPlayer );
							}
						}
					}
					catch( eEventResult eRet )
					{
						if ( RET_FAIL == eRet )
						{
							rkAlramMission.SetState( PgAlramMission::STATE_FAILED );
							BM::Stream kFaildPacket( PT_M_C_NFY_ALRAMMISSION_END );
							pkPlayer->Send( kFaildPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );
						}
					}
				}break;
			case PgAlramMission::STATE_SUCCEED:
			case PgAlramMission::STATE_FAILED:
			default:
				{
				}break;
			}
		}
	};


};

#endif // WEAPON_VARIANT_ALRAMMISSION_ALRAMMISSIONCHECKER_H