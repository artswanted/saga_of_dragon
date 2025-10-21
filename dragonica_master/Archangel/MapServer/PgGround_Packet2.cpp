#include "stdafx.h"
#include "BM/PgTask.h"
#include "Variant/Constant.h"
#include "Variant/PgAggroMeter.h"
#include "Variant/PgLogUtil.h"
#include "Variant/PgMission.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgEventView.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "Collins/Log.h"
#include "Global.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgAction.h"
#include "PgActionQuest.h"
#include "PgMonKillCountReward.h"
#include "PgTask_MapServer.h"
#include "PgPartyItemRule.h"
#include "SkillEffectAbilSystem/PgEffectAbilHandleManager.h"
#include "SkillEffectAbilSystem/PgSkillAbilHandleManager.h"
#include "PgAction_Pet.h"
#include "PgSuperGround.h"
#include "Effect/ChannelEffect.h"

void PgGround::ProcNotifyMsg(CUnit* pkUnit, WORD wType, BM::Stream* pkNfy)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	switch(wType)
	{
	case PT_U_G_ABIL64_CHANGE:
		{
			WORD wType;
			__int64 iValue;
			pkNfy->Pop(wType);
			pkNfy->Pop(iValue);

			switch(wType)
			{
			case AT_EXPERIENCE:
				{
					BYTE cCause = 0;
					pkNfy->Pop(cCause);

					PgAction_AddExp kAction(GroundKey(), iValue, static_cast<EAddExpCause>(cCause), this);
					kAction.DoAction(pkUnit, NULL);//경험치
				}break;
			case AT_MONEY:
				{
					BYTE cCause = 0;
					pkNfy->Pop(cCause);

					PgAction_ShareGold kMoneyAction(this, iValue, static_cast<EItemModifyParentEventType>(cCause), this->m_kLocalPartyMgr);//돈은 아이템서버가 지급.
					kMoneyAction.DoAction(pkUnit, NULL);
				}break;
			default:
				{
					pkUnit->SetAbil64((EAbilType)wType, iValue);
					pkUnit->SendAbil64((EAbilType)wType);
				}break;
			}
			
		}break;
	case PT_U_G_ABIL_CHANGE:
		{
			WORD wType;
			int iValue;
			pkNfy->Pop(wType);
			pkNfy->Pop(iValue);

			pkUnit->SetAbil((EAbilType)wType, iValue);
			pkUnit->SendAbil((EAbilType)wType);
		}break;
	case PT_U_G_NFY_ABIL_CHANGE://변경된것을 알려 온것이다.
		{
			WORD wType = 0;
			pkNfy->Pop(wType); //어떤 종류인지만
			
			//여기서 어빌을 설정하지 않는다. 다만 다른곳으로 알릴뿐
			switch( pkUnit->UnitType() ) 
			{
			case UT_PLAYER:
				{
					PgPlayer* pkUser = dynamic_cast<PgPlayer*>(pkUnit);
					if( pkUser )
					{
						PgAction_ChangeClassCheck kChangeClassCheck(GroundKey());
						kChangeClassCheck.DoAction(pkUser, NULL);

						switch( wType )
						{
						case AT_CLASS:
							{
								UpdatePartyUnitAbil(pkUser, PMCAT_Class);

								SendAllQuestInfo(pkUser);//지역 퀘스트 정보

								// 직업 변경 정보 센터로 갱신
								BM::Stream kTClassPacket(PT_M_T_REFRESH_CLASS_CHANGE, pkUser->GetID());
								kTClassPacket.Push(pkUser->GetAbil(AT_CLASS));
								::SendToCenter( kTClassPacket );
							}break;
						case AT_LEVEL:
							{
								UpdatePartyUnitAbil(pkUser, PMCAT_Level);

								// 레벨업 정보 센터로 갱신
								BM::Stream kTLevelPacket(PT_M_T_REFRESH_LEVELUP, pkUser->GetID());
								kTLevelPacket.Push(pkUser->GetAbil(AT_LEVEL));
								::SendToCenter( kTLevelPacket );

								ApplyChannelEffect(pkUser, this); // Update Channel Buff for level
							}break;
						case AT_TACTICS_LEVEL:
						default:
							{
							}break;
						}

						UpdateAbilGuild(pkUser, wType);

						PgAction_QuestAbil kQuestAction(GroundKey(), wType);
						kQuestAction.DoAction(pkUser, NULL); // level and etc
					}
				}break;
			case UT_PET:
				{
					PgPet *pkPet = dynamic_cast<PgPet*>(pkUnit);
					if ( pkPet )
					{
						switch( wType )
						{
						case AT_CLASS:
							{
								PgActPet_RefreshPassiveSkill kRefreshPassiveSkillAction( this );
								kRefreshPassiveSkillAction.DoAction( pkPet );
							}// break 사용 안함
						case AT_LEVEL:
							{
							}break;
						default:
							{

							}break;
						}
					}
				}break;
			}	
		}break;
	case PT_U_G_BROADCAST_AREA:
		{
			DWORD dwSendFlag = E_SENDTYPE_NONE;
			BM::GUID kIgnoreGuid;
			WORD wStreamType = 0;
			pkNfy->Pop(dwSendFlag);
			pkNfy->Pop(kIgnoreGuid);
			pkNfy->Pop(wStreamType);

			BM::Stream kSPacket(wStreamType);
			kSPacket.Push(*pkNfy);
			SendToArea( &kSPacket, pkUnit->LastAreaIndex(), kIgnoreGuid, SYNC_TYPE_RECV_ADD, dwSendFlag );
		}break;
	case PT_U_G_BROADCAST_GROUND:
		{
			DWORD dwSendFlag = E_SENDTYPE_NONE;
			WORD wStreamType = 0;
			pkNfy->Pop(dwSendFlag);
			pkNfy->Pop(wStreamType);

			BM::Stream kSPacket( wStreamType );
			kSPacket.Push(*pkNfy);
			Broadcast( kSPacket, NULL, dwSendFlag );
		}break;
	case PT_U_G_SEND_TO_USERS:
		{
			VEC_GUID kVec;
			BM::Stream kSendPacket;
			pkNfy->Pop( kVec );
			pkNfy->Pop( kSendPacket.Data() );
			kSendPacket.PosAdjust();

			SendToUser_ByGuidVec(kVec, kSendPacket);
		}break;
	case PT_U_G_NFY_UPDATE_QUEUE_EFFECT:
		{
			CONT_EFFECT_QUEUE kDoQueue;
			pkUnit->SwapEffectQueue(kDoQueue);

			CONT_EFFECT_QUEUE::iterator loop_iter = kDoQueue.begin();
			while( kDoQueue.end() != loop_iter )
			{
				EffectQueueData& kData = (*loop_iter);

				// 그라운드만 다시 덮어씀
				PgGroundUtil::SetActArgGround(kData.m_kArg, this);
				switch( kData.m_kEffectProcessType )
				{
				case EQT_ADD_EFFECT:
					{
						if( !pkUnit->AddEffect(kData.m_kEffectNo, kData.m_kValue, &kData.m_kArg, GetUnit(kData.m_kCasterGuid), kData.m_kEffectType) )
						{
							//AddEffect 실패 로그. 남기지 않아도 문제없기 때문에 주석처리( 2011. 07. 20. 김종수).
							// VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Update Effect Queue Cannot Add Effect[") << kData.m_kEffectNo << "] To User[" << pkUnit->Name() << "]");
						}
					}break;
				case EQT_DELETE_EFFECT:
					{
						if( !pkUnit->DeleteEffect(kData.m_kEffectNo, kData.m_bInGroup) )
						{
							//AddEffect 실패 로그. 남기지 않아도 문제없기 때문에 주석처리( 2011. 07. 20. 김종수).
							//VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Update Effect Queue Cannot Delete Effect[") << kData.m_kEffectNo << "] To User[" << pkUnit->Name() << "]" );
						}
					}break;
				default :
					{
						//값이 세팅되지 않았다는 로그
						VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Update Effect Queue Type None Effect[") << kData.m_kEffectNo << "] To User[" << pkUnit->Name() << "]" );
					}break;
				}

				++loop_iter;
			}
		}break;
	case PT_U_G_NFY_ADD_EFFECT:
		{
			int iEffectKey;
			int iValue;
			BM::GUID kCasterGuid;
			int iActionInstanceID;
			pkNfy->Pop(iEffectKey);
			if (iEffectKey == EFFECT_ALL_MYEFFECT_NUM)
			{
				// 전체 Effect에 대해서 호출해 주어야 한다.
				// 맵이동후에 처음 한번만 호출 됨.
				SActArg kArg;
				PgGroundUtil::SetActArgGround(kArg, this);

				ContEffectItor kItor;
				pkUnit->GetEffectMgr().GetFirstEffect(kItor);
				CEffect* pkEffect = NULL;
				PgUnitEffectMgr const& rkEffectMgr = pkUnit->GetEffectMgr();
				while ((pkEffect = rkEffectMgr.GetNextEffect(kItor)) != NULL)
				{
					switch(pkEffect->GetAbil(AT_TYPE))
					{
					case EFFECT_TYPE_PASSIVE:
						{// Passive Effect의 경우에는 맵이동시에 검사를 다시 해 주기 때문에 이곳에서 중복해서 할 필요 없다.
						}break;
					default:
						{
							g_kEffectAbilHandleMgr.EffectBegin(pkUnit, pkEffect, &kArg);
						}break;
					}
					//pkUnit->GetSkill()->EffectFilter(true, pkEffect->GetEffectNo());
				}
				//pkUnit->EffectQueueUpdate();
				//pkUnit->DoEquipEffect();
				if( pkUnit->IsUnitType(UT_PLAYER) )
				{
					::RefrashElemStatusEffect(pkUnit, this);
				}
			}
			else
			{
				EAddEffectResult eResult;
				int iEffectNo;
				pkNfy->Pop(iEffectNo);
				pkNfy->Pop(kCasterGuid);
				pkNfy->Pop(iActionInstanceID);
				pkNfy->Pop(iValue);
				pkNfy->Pop(eResult);
				SActArg kRecvedArg;
				kRecvedArg.ReadFromPacket(*pkNfy);

				CEffect* pkEffect = pkUnit->GetEffect(iEffectKey);
				if (eResult & E_AERESULT_NEWCREATE)	// 새로 생성된 것만 호출한다.
				{
					SActArg kArg;
					PgGroundUtil::SetActArgGround(kArg, this);
					{
						int iDamage = 0;
						kRecvedArg.Get(ACT_ARG_DAMAGE, iDamage);
						kArg.Set(ACT_ARG_DAMAGE, iDamage);
					}
					if( !pkEffect )
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"PT_U_G_NFY_ADD_EFFECT : Cannot find Added EffectKey["<<iEffectKey<<L"]");
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkEffect is NULL"));
					}
					else
					{
						BM::GUID kCasterGuid;
						kRecvedArg.Get(ACTARG_CUSTOM_UNIT1, kCasterGuid);
						if(kCasterGuid.IsNotNull())
						{// 캐스터 guid가 있다면 세팅해준다
							pkEffect->SetCaster(kCasterGuid);
						}
						g_kEffectAbilHandleMgr.EffectBegin(pkUnit, pkEffect, &kArg);
					}
				}

				if(eResult & E_AERESULT_RESTART)
				{
					//서브 이펙트가 있을 경우만 호출해 주어야 한다.
					//서브 이펙트가 없을 땐 굳이 호출하지 않아도 된다.
					if(0 < pkEffect->GetSubEffectSize()
						|| 0 < pkEffect->GetAbil(AT_MUST_REFRASH_EFFECT )
						)
					{
						SActArg kArg;
						PgGroundUtil::SetActArgGround(kArg, this);

						g_kEffectAbilHandleMgr.EffectEnd(pkUnit, pkEffect, &kArg);
						g_kEffectAbilHandleMgr.EffectBegin(pkUnit, pkEffect, &kArg);
					}
				}
				if (eResult & E_AERESULT_SENDPACKET)
				{
					BM::Stream kAPacket(PT_M_C_NFY_ADD_EFFECT, pkUnit->GetID());
					kAPacket.Push(iEffectKey);
					kAPacket.Push(iEffectNo);
					kAPacket.Push(kCasterGuid);
					kAPacket.Push(iActionInstanceID);
					kAPacket.Push(iValue);

					if ( pkEffect )
					{
						kAPacket.Push(pkEffect->GetStartTime());
						kAPacket.Push(pkEffect->GetEndTime());
						kAPacket.Push(pkEffect->ExpireTime());
					}

					pkUnit->Send( kAPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );
					SendToArea( &kAPacket, pkUnit->LastAreaIndex(), pkUnit->GetID(), SYNC_TYPE_RECV_ADD, E_SENDTYPE_MUSTSEND );
				}
			}
			PgWorldEventMgr::DoMonsterEffect(GroundKey().GroundNo(), pkUnit);
		}break;
	case PT_U_G_NFY_DELETE_EFFECT:
		{
			int iEffectKey = 0;
			bool bSendNft = false;
			int iDeleteCause = 0;
			pkNfy->Pop(iEffectKey);
			pkNfy->Pop(bSendNft);
			pkNfy->Pop(iDeleteCause);

			int iEffectNo = 0;
//			char chFunction[100];
//			SActArg kArg;
			CEffect* pkEffect = pkUnit->GetEffect(iEffectKey);			
			if( !pkEffect )
			{
				pkEffect = pkUnit->GetItemTypeEffect(iEffectKey);
			}
			if (pkEffect != NULL)
			{
				pkEffect->SetDelete();
				pkEffect->SetDeleteCause(iDeleteCause);
				SActArg kArg;
				PgGroundUtil::SetActArgGround(kArg, this);

				g_kEffectAbilHandleMgr.EffectEnd(pkUnit, pkEffect, &kArg);
				iEffectNo = pkEffect->GetEffectNo();				
			}

			if (bSendNft)
			{
				BM::Stream kDPacket(PT_M_C_NFY_DELETE_EFFECT, pkUnit->GetID());
				kDPacket.Push(iEffectNo);

				pkUnit->Send( kDPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );// 본인은 받아야 한다.
				SendToArea( &kDPacket, pkUnit->LastAreaIndex(), pkUnit->GetID(), SYNC_TYPE_RECV_ADD, E_SENDTYPE_MUSTSEND );
			}
			
			// Toggle Skill에 의한 Effect는 ToggleOff로 세팅해 주어야 한다.
			if (pkUnit->IsUnitType(UT_PLAYER))
			{
				PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
				if (pkPlayer == NULL)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"dynamic_cast<PgPlayer*> failed UnitType["<<pkUnit->UnitType()<<L"], Guid["<<pkUnit->GetID()<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPlayer is NULL"));
					break;
				}
				if (pkEffect != NULL)
				{
					int iToggleOff = 0;
					pkEffect->GetActArg(ACTARG_TOGGLESKILL, iToggleOff);
					if (iToggleOff > 0)
					{
						if (pkPlayer->GetMySkill()->SetToggle(false, iToggleOff))
						{
							GET_DEF(CSkillDefMgr, kSkillDefMgr);
							CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iToggleOff);
							if(pkSkill)
							{
								{// 연관된 자식 스킬의 이펙트가 걸려있는지 확인한후에 Delete 시켜주고
									SkillFuncUtil::DoChildSkillEffect(pkPlayer, iToggleOff, true, NULL);
								}
								int const iClass = pkPlayer->GetAbil(AT_CLASS);
								if( IsClass_OwnSubPlayer(iClass) )
								{// SubPlayer를 소유하고 있는 클래스일경우
									if( pkSkill->GetAbil(AT_DEL_SUB_PLAYER) )
									{// SubPlayer를 삭제하는 토글 스킬이 꺼진것이라면
										BM::GUID kGuid = pkPlayer->SubPlayerID();
										if( kGuid.IsNull() )
										{// SubPlayer를 생성함
											kGuid.Generate();
											CreateSubPlayer(pkPlayer, kGuid);
										}
									}
								}
							}

							// Client에게 알려주기
							BM::Stream kTPacket(PT_M_C_NFY_TOGGLSKILL_ON_OFF, iToggleOff);
							kTPacket.Push(false);
							kTPacket.Push(pkUnit->GetID());
							pkUnit->Send(kTPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND);
							// Skill 구조체에 ToggleOff 알리기
							pkPlayer->GetSkill()->ToggleOnOff(iToggleOff, ESS_TOGGLE_OFF, g_kEventView.GetServerElapsedTime(), false);
						}
					}
				}
			}

			//pkUnit->EffectQueueUpdate();
			PgWorldEventMgr::DoMonsterEffect(GroundKey().GroundNo(), pkUnit);
		}break;
	case PT_U_G_RUN_ACTION://Unit 에서 Ground 로 보내는 패킷
		{
			short int sActionNo = 0;
			pkNfy->Pop(sActionNo);
			switch(sActionNo)
			{
			case GAN_Refresh_All_Quest:
				{
					SendAllQuestInfo(dynamic_cast<PgPlayer*>(pkUnit));//지역 퀘스트 정보
				}break;
			case GAN_Resurrect_Spawn_Num:
				{
					
					if(pkUnit->IsUnitType(UT_PLAYER))
					{
						//
						PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
						pkPlayer->SetAbil(AT_HP, pkUnit->GetAbil(AT_C_MAX_HP), true, true);
						pkPlayer->SetAbil(AT_MP, pkUnit->GetAbil(AT_C_MAX_MP), true, false);
						pkUnit->SetState(US_IDLE);
						
						//
						short sSpawnNum = 0;
						pkNfy->Pop(sSpawnNum);

						bool bRet = SendToSpawnLoc(pkUnit, (int)sSpawnNum, false);

						INFO_LOG(BM::LOG_LV1, __FL__<<L"Can't Resurrect");
					}
					else
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Resurrect this Unit(is not PgPlayer)");
					}
				}break;
			case GAN_SetAbiles:
				{
					short int sAbil[10];
					BYTE byCount = 0;
					pkNfy->Pop(byCount);
					for (int i=0; i<(int)byCount; i++)
					{
						pkNfy->Pop(sAbil[i]);
					}
					pkUnit->SendAbiles((WORD*)sAbil, (BYTE) byCount);
				}break;
			case GAN_AddEffect:
				{
					int iEffectNo = 0;

					pkNfy->Pop( iEffectNo );

					SEffectCreateInfo kCreate;
					kCreate.eType = EFFECT_TYPE_NORMAL;
					kCreate.iEffectNum = iEffectNo;
					PgGroundUtil::SetActArgGround(kCreate.kActArg, this);
					kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
					pkUnit->AddEffect(kCreate);
				}break;
			case GAN_DelEffect:
				{
					int iEffectNo = 0;

					pkNfy->Pop( iEffectNo );

					pkUnit->DeleteEffect(iEffectNo);
				}break;
			case GAN_SumitLog:
				{
					int iLogType = 0;
						
					pkNfy->Pop( iLogType );

					PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
					if( pkPlayer )
					{
						switch( iLogType )
						{
						case LOG_USER_DIE:
							{
								BM::GUID kCasterGuid;
								pkNfy->Pop( kCasterGuid );

								ELogUserDieType eKillerType = LUDT_Unknown;
								BM::GUID kCasterLogGuid;
								std::wstring kCasterName;

								CUnit const* pkCaster = GetUnit(kCasterGuid);
								if( pkCaster
								&&	pkPlayer->GetID() != pkCaster->GetID() ) // 시전자, 부활자가 서로 다를 때
								{
									switch( pkCaster->UnitType() )
									{
									case UT_MONSTER:			{ eKillerType = LUDT_Monster; }break;
									case UT_BOSSMONSTER:		{ eKillerType = LUDT_BossMonster; }break;
									case UT_PLAYER:				{ eKillerType = LUDT_Player; }break;
									case UT_ENTITY:				{ eKillerType = LUDT_Entity; }break;
									}

									kCasterName = pkCaster->Name();
									kCasterLogGuid = pkCaster->GetID();

									BM::GUID const kTempGuid = GlobalHelper::FindOwnerGuid(pkCaster);
									if( pkCaster->GetID() != kTempGuid ) // 주인이 따로 있으면
									{
										CUnit const* pkLogCaster = GetUnit(kTempGuid);
										if( pkLogCaster )
										{
											kCasterName = pkLogCaster->Name();
											kCasterLogGuid = kTempGuid;
										}
									}
								}

								PgPlayerLogUtil::DeathLog(pkPlayer, eKillerType, GroundKey(), kCasterName, kCasterGuid);
							}break;
						case LOG_USER_ALIVE:
							{
								std::wstring kCasterName;
								BM::GUID kCasterGuid;
								ELogUserResurrectType eResurrectType = LURT_Self;

								CUnit const* pkCaster = GetUnit(pkPlayer->GetTarget());
								if( pkCaster && pkUnit )
								{
									switch( pkCaster->UnitType() )
									{
									case UT_MONSTER:			{ eResurrectType = LURT_Monster; }break;
									case UT_BOSSMONSTER:		{ eResurrectType = LURT_BossMonster; }break;
									case UT_PLAYER:				{ eResurrectType = LURT_Player; }break;
									case UT_ENTITY:				{ eResurrectType = LURT_Entity; }break;
									}

									kCasterName = pkCaster->Name();
									kCasterGuid = pkCaster->GetID();
									BM::GUID const kTempGuid = GlobalHelper::FindOwnerGuid(pkCaster);
									if( pkCaster->GetID() != kTempGuid ) // 주인이 따로 있으면
									{
										CUnit const* pkLogCaster = GetUnit(kTempGuid);
										if( pkLogCaster )
										{
											kCasterName = pkLogCaster->Name();
											kCasterGuid = pkLogCaster->GetID();
										}
									}
								}

								PgPlayerLogUtil::AliveLog(pkPlayer, eResurrectType, GroundKey(), kCasterName, kCasterGuid, 0, 0);
							}break;
						default:
							{
								CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("[%s]-[%d] LogType[") << iLogType << _T("] is Invalid no"));
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
							}break;
						}
					}
				}break;
			case GAN_SendQuestDialog:
				{
					BM::GUID kNpcGuid;
					EQuestShowDialogType eQuestShowDialogType = QSDT_None;
					int iQuestID = 0;
					int iDialogID = 0;

					pkNfy->Pop( kNpcGuid );
					pkNfy->Pop( eQuestShowDialogType );
					pkNfy->Pop( iQuestID );
					pkNfy->Pop( iDialogID );

					SendShowQuestDialog(pkUnit, kNpcGuid, eQuestShowDialogType, iQuestID, iDialogID);
				}break;
			case GAN_MapMove:
				{
					SReqMapMove_MT kRMM;
					VEC_GUID kVec;

					pkNfy->Pop( kRMM );
					pkNfy->Pop( kVec );

					PgReqMapMove kReqMapMove(this, kRMM, NULL);

					VEC_GUID::const_iterator guid_iter = kVec.begin();
					while( kVec.end() != guid_iter )
					{
						CUnit* pkUnit = GetUnit( (*guid_iter) );
						if( pkUnit )
						{
							PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
							if( pkPlayer )
							{
								kReqMapMove.Add( pkPlayer );
							}
						}
						++guid_iter;
					}
					kReqMapMove.DoAction();
				}break;
			case GAN_PushVolatileInven_Item:
				{
					PgBase_Item kItem;
					kItem.ReadFromPacket(*pkNfy);
					PushBSInvenItem(pkUnit, kItem);
				}break;
			case GAN_PushMissionDefenceInven_Item:
				{
					PgBase_Item kItem;
					kItem.ReadFromPacket(*pkNfy);
					PushMissionDefenceInvenItem(pkUnit, kItem);
				}break;
			case GAN_SuperGroundFeather:
				{
					int iNewHP = 0, iNewMP = 0;
					pkNfy->Pop(iNewHP);
					pkNfy->Pop(iNewMP);
					if( 0 != (GATTR_FLAG_SUPER & GetAttr()) )
					{
						PgSuperGround* pkSuperGround = dynamic_cast< PgSuperGround* >(this);
						if( pkSuperGround )
						{
							pkSuperGround->UseSuperGroundFeather(pkUnit, iNewHP, iNewMP);
						}
					}
				}break;
			case GAN_SuperGroundMoveFloor:
				{
					int iMoveFloor = 0;
					pkNfy->Pop(iMoveFloor);
					if( 0 != (GATTR_FLAG_SUPER & GetAttr()) )
					{
						PgSuperGround* pkSuperGround = dynamic_cast< PgSuperGround* >(this);
						if( pkSuperGround )
						{
							pkSuperGround->NextFloor(pkSuperGround->NowFloor() + iMoveFloor, 0, true);
						}
					}
				}break;
			case GAN_BroadCastSmallArea:
				{
					BM::Stream kBroadPacket;
					pkNfy->Pop( kBroadPacket.Data() );
					kBroadPacket.PosAdjust();

					SendToArea(&kBroadPacket, pkUnit->LastAreaIndex(), pkUnit->GetID(), SYNC_TYPE_RECV_ADD, E_SENDTYPE_NONE);
					//Broadcast(kBroadPacket, pkUnit, E_SENDTYPE_BROADCAST);
				}break;
			case GAN_SaveCombo:
				{
					BM::GUID kCharGuid;
					pkNfy->Pop( kCharGuid );

					CUnit* pkUnit = GetUnit( kCharGuid );
					if( pkUnit )
					{
						PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
						if( pkPlayer )
						{
							PgComboCounter* pkCounter = NULL;
							if( GetComboCounter( pkPlayer->GetID(), pkCounter ) )
							{
								COMBO_TYPE const iLatestCombo = pkCounter->GetComboCount();

								// NC용
								BM::Stream kNPacket(PT_A_N_NFY_CHARACTER_COMBO_INFO);
								kNPacket.Push( static_cast< int >(g_kProcessCfg.RealmNo()) );
								kNPacket.Push( pkPlayer->GetID() );
								kNPacket.Push( iLatestCombo );
								kNPacket.Push( BM::DBTIMESTAMP_EX(BM::PgPackedTime::LocalTime()) );
								::SendToRealmContents(PMET_WEB_HELPER, kNPacket);

								// 공통 TB_UserCharacter_Point
								if( iLatestCombo > pkPlayer->GetAbil( AT_MAX_COMBO ) )
								{
									CONT_PLAYER_MODIFY_ORDER kOrder;
									SPMOD_AddRankPoint kSetRank( E_RANKPOINT_MAXCOMBO, iLatestCombo );
									kOrder.push_back( SPMO( IMET_ADD_RANK_POINT, pkPlayer->GetID(), kSetRank ) );

									PgAction_ReqModifyItem kItemModifyAction(CIE_Rank_Point, GroundKey(), kOrder, BM::Stream(), true);//랭킹은 사망 관련 없음.
									kItemModifyAction.DoAction(pkPlayer, NULL);
								}
							}
						}
					}					
				}break;
			case GAN_SumitOrder:
				{
					EItemModifyParentEventType eCause = IMEPT_NONE;
					CONT_PLAYER_MODIFY_ORDER kOrder;

					pkNfy->Pop( eCause );
					kOrder.ReadFromPacket(*pkNfy);

					PgAction_ReqModifyItem kItemModifyAction(eCause, GroundKey(), kOrder);
					kItemModifyAction.DoAction(pkUnit, NULL);
				}break;
			default:
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<sActionNo<<L" Action is Invalid no");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
				};
			}
		}break;
	case PT_P_G_SEND_PACKET:
		{
			BM::Stream kSPacket;
			kSPacket.Push(*pkNfy);
			pkUnit->Send(kSPacket);
		}break;
	case PT_U_G_NFY_CHANGED_INVEN:
		{
			// Inventory가 변경되었다..
			// PassiveSkill 중에는 Inven 변화를 알아야 할 것들이 있다.
			if (pkUnit->IsUnitType(UT_PLAYER))
			{
				PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
				PgMySkill* pkMySkill = pkPlayer->GetMySkill();
				size_t iIndex = 0;
				int iSkillNo = 0;
				while ((iSkillNo = pkMySkill->GetSkillNo(EST_PASSIVE, iIndex)) > 0)
				{		
					// iSkillNo : 실제 스킬 번호
					GET_DEF(CSkillDefMgr, kSkillDefMgr);
					CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

					// pkEffect : 실제로 걸려 있는 이펙트 번호
					CEffect const* pkEffect = pkPlayer->GetEffect(iSkillNo, true);
					if(pkSkill && pkEffect)
					{
						// 스킬 번호와 이펙트 번호는 1:1 매칭으로 같은 번호를 사용
						// 같지 않을 경우 OverSkill인 경우
						//OverSkill이 있을 경우
						int const iOverLevel = pkMySkill->GetOverSkillLevel(iSkillNo);
						// 실제 걸려있는 이펙트가 오버스킬된 경우와 다른 경우 다시 걸어준다.
						if(pkEffect->GetEffectNo() != (pkSkill->No() + iOverLevel))
						{
							SActArg kArg;
							PgGroundUtil::SetActArgGround(kArg, this);

							// 실제 걸려있는 번호에 해당되는 스킬 Def를 얻어 온다.
							pkSkill = kSkillDefMgr.GetDef(pkEffect->GetEffectNo());
							if(pkSkill)
							{
								// 실제 레벨에 해당되는 패시브 스킬을 삭제
								g_kSkillAbilHandleMgr.SkillEnd( pkUnit, pkSkill, &kArg); 
							}
							else
							{
								// 해당되는 스킬Def를 찾을 수 없다.~
								INFO_LOG(BM::LOG_LV4, __FL__ << _T("Passive Skill Cannot End. SkillDef[") << pkEffect->GetEffectNo() << _T("] is not Find")); // 디버그용
							}

							// OverLevel된 패시브 스킬을 걸어 준다.
							g_kSkillAbilHandleMgr.SkillPassive( pkUnit, iSkillNo + iOverLevel, &kArg );

							// OverLevel 된 SkillDef를 얻어 온다.
							pkSkill = kSkillDefMgr.GetDef(iSkillNo + iOverLevel);
						}						
					}

					if(pkSkill)
					{
						SActArg kArg;
						g_kSkillAbilHandleMgr.SkillPCheck(pkUnit, pkSkill, &kArg);
					}
					else
					{
						// 해당되는 스킬Def를 찾을 수 없다.~
						INFO_LOG(BM::LOG_LV4, __FL__ << _T("Passive cannot SkillPCheck. SkillDef[") << pkEffect->GetEffectNo() << _T("] is not Find")); // 디버그용
					}

					++iIndex;
				}
				// 착용중인 Weapon Type 구하기
				int iWeaponType = 0;
				PgBase_Item kItem;
				if (SUCCEEDED(pkUnit->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_WEAPON), kItem)))
				{
					GET_DEF(CItemDefMgr, kItemDefMgr);
					CItemDef* pkDef = (CItemDef*) kItemDefMgr.GetDef(kItem.ItemNo());
					if (pkDef == NULL)
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Cannot find ItemDef....ItemNo["<<kItem.ItemNo()<<L"]");
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkDef is NULL"));
					}
					else
					{
						iWeaponType = pkDef->GetAbil(AT_WEAPON_TYPE);
					}
				}
				// Effect중에 WeaponLimit 있는 것을 검사한다.
				PgUnitEffectMgr const& rkEffectMgr= pkUnit->GetEffectMgr();
				ContEffectItor kItor;
				std::vector<int> kDelete;
				rkEffectMgr.GetFirstEffect(kItor);
				CEffect* pkEffect = NULL;
				while ((pkEffect = rkEffectMgr.GetNextEffect(kItor)) != NULL)
				{
					int iWeaponLimit = 0;
					pkEffect->GetActArg(ACTARG_WEAPONLIMIT, iWeaponLimit);
					if (iWeaponLimit > 0 && (iWeaponType & iWeaponLimit) == 0)	// WeaponLimit에 위배된다.
					{
						INFO_LOG(BM::LOG_LV8, __FL__<<L"Effect deleted....ID["<<pkEffect->GetEffectNo()<<L"]");
						kDelete.push_back(pkEffect->GetEffectNo());
					}
				}
				int iSize = kDelete.size();
				for (int i=0; i<iSize; ++i)
				{
					int iEffect = kDelete.at(i);
					pkUnit->DeleteEffect(iEffect);
				}
			}
		}break;
	case PT_U_G_NFY_EXP_SHARE_PARTY_MEMBER:
		{
			VEC_GUID kVec;
			__int64 iAddExp = 0;
			int iMonsterLv = 0;
			EAddExpCause kCause;
			POINT3 kFromPos;

			pkNfy->Pop( kVec );
			pkNfy->Pop( iAddExp );
			pkNfy->Pop( iMonsterLv );
			pkNfy->Pop( kCause );
			pkNfy->Pop( kFromPos );

			VEC_GUID::const_iterator iter = kVec.begin();
			while(kVec.end() != iter)
			{
				BM::GUID const & rkCharGuid = (*iter);
				CUnit* pkUnit = GetUnit(rkCharGuid);
				if( pkUnit
				&&	pkUnit->IsUnitType(UT_PLAYER) )
				{
					const COMBO_TYPE iCurComboCount = GetCurrentCombo(pkUnit->GetID());
					int const iBonusRate = CalcComboBonusRate(iCurComboCount);

					__int64 OriginalExp = iAddExp;
					PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pkUnit);
					if( pPlayer )
					{
						float DefenceBonus = pPlayer->DefenceMssionExpBonusRate();	// 전략 피로도 보너스 비율.
						if( DefenceBonus > 0 )
						{
							OriginalExp = OriginalExp * DefenceBonus;	// 경험치 보너스 비율 적용. 소수점 이하 버림.
						}
					}

					PgAction_ShareExpParty kAction_PartyShareExp(OriginalExp, kCause, iMonsterLv, kFromPos, this, kVec.size(), this->m_kLocalPartyMgr, iBonusRate);
					kAction_PartyShareExp.DoAction(pkUnit, NULL);
				}
				++iter;
			}
		}break;
	case PT_U_G_NFY_GOLD_SHARE_PARTY_MEMBER:
		{
			VEC_GUID kVec;
			int iResultPerGold = 0;
			EItemModifyParentEventType kCause;
			pkNfy->Pop(kVec);
			pkNfy->Pop(iResultPerGold);
			pkNfy->Pop(kCause);

			VEC_GUID::const_iterator iter = kVec.begin();
			while(kVec.end() != iter)
			{
				BM::GUID const & rkCharGuid = (*iter);
				CUnit* pkUnit = GetUnit(rkCharGuid);
				if( pkUnit )
				{
					__int64 const iFinalGold = PgAction_ShareGold::PrivateShareGold(iResultPerGold,std::max(pkUnit->GetAbil(AT_ADD_MONEY_PER),0), GetEventAbil() );
					PgAction_ReqAddMoney kGoldAction(kCause, iFinalGold, GroundKey());//파티 쉐어링은 여기서
					kGoldAction.DoAction(pkUnit, NULL);
				}
				++iter;
			}
		}break;
	//진정관련
	case PT_C_GM_REQ_RECEIPT_PETITION:
	case PT_C_GM_REQ_REMAINDER_PETITION:
		{
			BM::Stream kPacket(wType);
			kPacket.Push(g_kProcessCfg.RealmNo());
			kPacket.Push(g_kProcessCfg.ChannelNo());
			kPacket.Push(*pkNfy);
			SendToCenter(kPacket);//센터로 보냄.
		}break;
	case PT_U_G_NFY_ADD_EXP:
		{
			__int64 iAddExp = 0;
			int iMonsterLv = 0;
			pkNfy->Pop(iAddExp);
			pkNfy->Pop(iMonsterLv);
			if( pkUnit->IsUnitType(UT_ENTITY) )
			{
				BM::GUID const & rkCaller = dynamic_cast<PgEntity*>(pkUnit)->Caller();
				CUnit* pkCaller = GetUnit(rkCaller);
				if( pkCaller
				&&	pkCaller->IsUnitType(UT_PLAYER) )
				{
					int const iDiffLv = (iMonsterLv)? iMonsterLv - pkCaller->GetAbil(AT_LEVEL): 0;
					if( iCanAddExpMinLevel <= iDiffLv
					&&	iCanAddExpMaxLevel >= iDiffLv )//레벨차가 범위 안이면 ok
					{
						{
							CONT_PLAYER_MODIFY_ORDER kOrder;

							SPMOD_AddAbil kAddAbilData(AT_EXPERIENCE, iAddExp);
							kOrder.push_back(SPMO(IMET_ADD_ABIL64, pkUnit->GetID(), kAddAbilData));

							PgAction_ReqModifyItem kItemModifyAction(MCE_EXP, GroundKey(), kOrder);
							kItemModifyAction.DoAction(pkUnit, NULL);
			//			pkCaller->AddExp(iAddExp);
						}

						
					}
				}	
			}
		}break;
	case PT_U_G_NFY_ADD_AGGRO_METER:
		{
			BYTE cMeterType = 0;
			DWORD dwTimeStamp = 0;
			BM::GUID kMonsterGuid;
			BM::GUID kCasterGuid;
			int iVal = 0;
			unsigned int iComboCount = 0;
			int iSkillNo = 0;
			DWORD dwComboDelay = 0;
			pkNfy->Pop(cMeterType);
			pkNfy->Pop(dwTimeStamp);
			pkNfy->Pop(kMonsterGuid);//항상 몬스터
			pkNfy->Pop(kCasterGuid);//Player 또는 Player의 소환물
			pkNfy->Pop(iVal);//데미지
			pkNfy->Pop(iComboCount);//콤보 카운트
			pkNfy->Pop(iSkillNo);
			pkNfy->Pop(dwComboDelay);

			BM::GUID kPlayerGuid;

			PgAggroMeter* pkMeter = NULL;
			bool const bFindMeter = GetMeter(kMonsterGuid, pkMeter);
			CUnit* pkMonster = GetUnit(kMonsterGuid);
			CUnit* pkCaster = NULL;
			if( bFindMeter
			&&	pkMonster )
			{
				VEC_GUID kGuidVec;
				BM::GUID kOwnerGuid;

				pkCaster = GetUnit(kCasterGuid);
				if( pkCaster )
				{
					static EUnitType const eFindCallerType = static_cast<EUnitType>(UT_ENTITY|UT_SUB_PLAYER|UT_SUMMONED);
					if( pkCaster->IsUnitType(UT_PLAYER) )
					{
						PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
						if( pkPC
						&&	BM::GUID::NullData() != pkPC->PartyGuid() )
						{
							//g_kLocalPartyMgr.GetPartyMemberGround(pkPC->PartyGuid(), GroundKey(), kGuidVec, pkCaster->GetID());
							bool bRet = GetPartyMemberGround(pkPC->PartyGuid(), GroundKey(), kGuidVec, pkCaster->GetID());							
						}

						kPlayerGuid = pkCaster->GetID();
					}
					else if( pkCaster->IsInUnitType(eFindCallerType) )
					{
						if( pkCaster->GetID() != pkCaster->Caller() )
						{
							assert(pkCaster->Caller() != BM::GUID::NullData());
							kOwnerGuid = pkCaster->Caller();
							kPlayerGuid = pkCaster->Caller();
						}
					}

					//Monster Aggro Meter
					pkMeter->AddVal((EAggroMeterType)cMeterType, SAggroOwner(kCasterGuid, kOwnerGuid), iVal, &kGuidVec);
				}
			}

			//ComboSkip 처리유무
			bool bSkipCombo = false;
			if(pkCaster && pkCaster->IsUnitType(UT_ENTITY) && ENTITY_GUARDIAN==pkCaster->GetAbil(AT_ENTITY_TYPE))
			{
				bSkipCombo = true;
			}

			// Player Combo Counter
			PgPlayer* pkPlayer = GetUser(kPlayerGuid);
			if( pkPlayer && !bSkipCombo )
			{
				PgComboCounter* pkCounter = NULL;
				if( GetComboCounter(kPlayerGuid, pkCounter) )
				{
					switch(cMeterType)
					{
					case AMT_Damage: // 플레이어가 때렸다
						{
							bool bResetRet = false;
							COMBO_TYPE const iPreCombo = pkCounter->GetComboCount();
							COMBO_TYPE const iAddedCombo = (iComboCount)? iComboCount: 1;
							COMBO_TYPE const iNewComboCount = pkCounter->AddCombo(iAddedCombo, bResetRet, dwComboDelay);
							if( bResetRet )
							{
								int const iComboCount = static_cast<int>(pkCounter->GetComboCount(ECOMBO_MAX));
								if( iComboCount > pkPlayer->GetAbil( AT_MAX_COMBO) )
								{
									CONT_PLAYER_MODIFY_ORDER kOrder;
									SPMOD_AddRankPoint kSetRank( E_RANKPOINT_MAXCOMBO, iComboCount );
									kOrder.push_back( SPMO( IMET_ADD_RANK_POINT, pkPlayer->GetID(), kSetRank ) );

									PgAction_ReqModifyItem kItemModifyAction(CIE_Rank_Point, GroundKey(), kOrder, BM::Stream(), true);//랭킹은 사망 관련 없음.
									kItemModifyAction.DoAction(pkPlayer, NULL);
								}
								BM::Stream kNPacket(PT_A_N_NFY_CHARACTER_COMBO_INFO);
								kNPacket.Push( static_cast< int >(g_kProcessCfg.RealmNo()) );
								kNPacket.Push( kPlayerGuid );
								kNPacket.Push( iPreCombo );
								kNPacket.Push( BM::DBTIMESTAMP_EX(BM::PgPackedTime::LocalTime()) );
								::SendToRealmContents(PMET_WEB_HELPER, kNPacket);

								BM::Stream kPacket(PT_M_C_NFY_CHANGE_COMBO_COUNT); // 리셋 공지
								kPacket.Push( static_cast< unsigned int >(0) );
								kPacket.Push( static_cast< int >(0) );
								pkPlayer->Send(kPacket);
							}

							//if( LOCAL_MGR::NC_DEVELOP == g_kLocal.ServiceRegion() )
							//{
							//	INFO_LOG(BM::LOG_LV1, iNewComboCount); // 디버그용
							//}
							BM::Stream kPacket(PT_M_C_NFY_CHANGE_COMBO_COUNT); // 매번 전송한다.
							kPacket.Push( iNewComboCount );
							kPacket.Push( iSkillNo );
							pkPlayer->Send(kPacket);
						}break;
					case AMT_Hit: // 플레이어가 맞았다
						{
							pkCounter->AddDamage();
						}break;
					}
					/*if ( this->GetAttr() & GATTR_MISSION )
					{
						//int m_iCombo = pkCounter->GetComboCount(ECOMBO_MAX);
						//int m_iTotalHit = pkCounter->GetComboCount(ECOUNT_TOTAL_HIT);
						int m_iDamage = pkCounter->GetComboCount(ECOUNT_TOTAL_DAMAGE);

						BM::Stream kPacket(PT_M_C_NFY_CHANGE_MISSIONSCORE_COUNT);
						//kPacket.Push(m_iCombo);
						kPacket.Push(static_cast<int>(0));
						//kPacket.Push(static_cast<int>(m_iTotalHit));
						kPacket.Push(static_cast<int>(m_iDamage));
						pkPlayer->Send(kPacket);
					}*/
				}
			}
		}break;
	case PT_U_G_NFY_ADD_KILLCOUNTER:
		{
			if( pkUnit
			&&	pkUnit->IsInUnitType(UT_MONSTER) )
			{
				CUnit* pkTarget = pkUnit;
				BM::GUID kPlayerGuid;

				pkNfy->Pop( kPlayerGuid );

				PgPlayer* pkPlayer = GetUser(kPlayerGuid);
				if( pkPlayer )
				{
					int const iDiffLv = pkTarget->GetAbil(AT_LEVEL) - pkPlayer->GetAbil(AT_LEVEL);

					bool bCanIncreaseCount = 0 == (GATTR_FLAG_NO_KILLCOUNT_UI & GetAttr());
					bCanIncreaseCount = bCanIncreaseCount && g_kMonKillCountReward.CanCountLvMin() <= iDiffLv;
					bCanIncreaseCount = bCanIncreaseCount && g_kMonKillCountReward.CanCountLvMax() >= iDiffLv;
					if( bCanIncreaseCount && pkPlayer->GetAbil(AT_DUEL) <= 0 )
					{
						int const iCurCount = pkPlayer->GetAbil(AT_MONSTER_KILL_COUNT)+1; // 현재 증가 카운트
						bool bSendAbil = 0 == (iCurCount % g_kMonKillCountReward.MonsterKillPerMsg());

						int iNextCount = iCurCount;
						if( g_kMonKillCountReward.MonsterKillResetMax() < iCurCount ) // 초기화 검사
						{
							bSendAbil = true;
							iNextCount = iCurCount - g_kMonKillCountReward.MonsterKillResetMax();
						}
						pkPlayer->SetAbil(AT_MONSTER_KILL_COUNT, iNextCount, bSendAbil);

						{
							PgAction_QuestKillCount kAction(GroundKey(), iNextCount);
							kAction.DoAction(pkPlayer, NULL);
						}

						// 보상 검사
						SMonKillRewardItem kReward;
						if( g_kMonKillCountReward.Check(iCurCount, pkPlayer->GetAbil(AT_LEVEL), kReward) )
						{
							// 아이템을 넣자
							PgBase_Item kItem;
							CONT_ITEM_CREATE_ORDER kOrderList;
							if( SUCCEEDED(CreateSItem(kReward.iItemNo, kReward.iCount, GIOT_NONE, kItem)) )
							{
								kOrderList.push_back(kItem);
							}
							if( !kOrderList.empty() )
							{
								PgAction_CreateItem kCreateAction(CIE_KillCount, GroundKey(), kOrderList);
								kCreateAction.DoAction(pkPlayer, NULL);							
							}

							if( !bSendAbil )
							{
								pkPlayer->SendAbil(AT_MONSTER_KILL_COUNT);
							}
						}
					}
				}
			}
		}break;
	case PT_U_G_NFY_STAT_TRACK_INFO:
		{
			if (pkUnit)
			{
				BM::GUID kPlayerGuid;
				pkNfy->Pop(kPlayerGuid);
				
				PgPlayer* pkPlayer = GetUser(kPlayerGuid);
				if (pkPlayer)
				{
					int const iDiffLv = pkUnit->GetAbil(AT_LEVEL) - pkPlayer->GetAbil(AT_LEVEL);
					bool bCanIncreaseCount = false;
					bCanIncreaseCount = g_kMonKillCountReward.CanCountLvMin() <= iDiffLv;
					bCanIncreaseCount = g_kMonKillCountReward.CanCountLvMax() >= iDiffLv;
					if (bCanIncreaseCount)
					{
						PgInventory * kInventory = pkPlayer->GetInven();
						if (kInventory)
						{
							const SItemPos kItemPos(IT_FIT, EQUIP_POS_WEAPON);
							PgBase_Item kItem;
							if (S_OK == kInventory->GetItem(kItemPos, kItem))
							{
								SStatTrackInfo kStatTrackInfo = kItem.StatTrackInfo();
								if (kStatTrackInfo.HasStatTrack())
								{
									BM::Stream kPacket(PT_M_S_NFY_STAT_TRACK_INFO);
									const EStatTrackKillType eStatTrackKill = pkUnit->IsInUnitType(UT_MONSTER) ? STKT_MONSTER : STKT_PLAYER;
									kStatTrackInfo.IncKillCount(eStatTrackKill);
									kItem.StatTrackInfo(kStatTrackInfo);
									kInventory->Modify(kItemPos, kItem);

									kPacket.Push(kItem.Guid());
									kPacket.Push(eStatTrackKill);
									kPacket.Push(1); // 1 kill
									SendToRealmContents(PMET_SendToPacketHandler, kPacket);
									{
										BM::Stream kPlayerPacket(PT_M_C_NFY_STAT_TRACK_INFO);
										kPlayerPacket.Push(kItem.Guid());
										kPlayerPacket.Push(kStatTrackInfo);
										pkPlayer->Send(kPlayerPacket);
									}

								}
							}
						}
					}
				}
			}
		}break;
	case PT_U_G_NFY_QUESTREWARD_ADDSKILL:
		{
			BM::GUID kMineGuid;
			BYTE cCmdType = 0;
			BYTE cCmdRet = 0;

			pkNfy->Pop(cCmdType);
			pkNfy->Pop(cCmdRet);			
			pkNfy->Pop( kMineGuid );

			CUnit *pkUnit = GetUnit(kMineGuid);
			if ( pkUnit )
			{
				std::list<int> kSkillStack;
				PU::TLoadArray_A((*pkNfy), kSkillStack);

				for(std::list<int>::const_iterator it = kSkillStack.begin(); it!=kSkillStack.end(); ++it)
				{
					if( 0 < (*it) )
					{
						BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, cCmdType);
						kPacket.Push( cCmdRet );
						kPacket.Push( (*it) );

						PgAction_LearnSkill kAction((*it), GroundKey(), cCmdType, kPacket);
						if ( LS_RET_SUCCEEDED == kAction.DoAction(pkUnit) )
						{
							CONT_PLAYER_MODIFY_ORDER kOrder;

							BM::PgPackedTime kDate;
							SPMOD_SetSweetHeart kSetData(CC_CoupleSkill, pkUnit->GetID(), (*it), kDate);
							kOrder.push_back( SPMO(IMET_SET_SWEETHEART, pkUnit->GetID(), kSetData) );

							PgAction_ReqModifyItem kItemModifyAction(CIE_CoupleLearnSkill, GroundKey(), kOrder);
							kItemModifyAction.DoAction(pkUnit, NULL);

							BM::Stream kPacket(PT_C_N_REQ_COUPLE_COMMAND, pkUnit->GetID());
							kPacket.Push( (BYTE)CC_CoupleSkill );
							kPacket.Push( (*it) );
							SendToCoupleMgr(kPacket);
						}
					}
				}
			}
		}break;
	case PT_U_G_NFY_MARRY_MONEY:
		{
			BYTE cCmdType = 0;

			pkNfy->Pop(cCmdType);

			switch( cCmdType )
			{
			case MC_AnsMarry:
				{
					__int64 i64SendAdd = 0;
					BM::GUID kCharGuid;
					BM::GUID kCoupleGuid;					
					std::wstring kName;
					std::wstring kCoupleName;
					int iGender1 = 0;
					int iGender2 = 0;

					pkNfy->Pop( i64SendAdd );
					pkNfy->Pop( kCharGuid );
					pkNfy->Pop( kCoupleGuid );
					pkNfy->Pop( kName );
					pkNfy->Pop( kCoupleName );
					pkNfy->Pop( iGender1 );
					pkNfy->Pop( iGender2 );
					
					if( kCoupleGuid.IsNotNull() )
					{
						m_kMarryMgr.Clear();
						m_kMarryMgr.Init( GroundKey(), kCharGuid, kCoupleGuid, kName, kCoupleName, iGender1, iGender2 );

						if( 0 < i64SendAdd )
						{
							if( m_kMarryMgr.SetMoney(i64SendAdd) )
							{
							}
						}
					}
				}break;
			case MC_AnsSendMoney:
				{
					__int64 i64SendAdd = 0;

					pkNfy->Pop( i64SendAdd );
					
					if( 0 < i64SendAdd )
					{
						if( m_kMarryMgr.SetMoney(i64SendAdd) )
						{
						}
					}
				}
			default:
				{
				}break;
			}
		}break;
	case PT_U_G_REQ_MAP_MOVE:
		{
			PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if ( pkPlayer )
			{
				SReqMapMove_MT kRMM;
				pkNfy->Pop(kRMM);

				PgReqMapMove kMapMove( this, kRMM, NULL );
				if ( kMapMove.Add( pkPlayer ) )
				{
					kMapMove.DoAction();
				}
			}
		}break;
	case PT_U_G_NFY_SYNCTYPE_CHANGE:
		{
			BYTE byAddType = SYNC_TYPE_NONE;
			BYTE byRemoveType = SYNC_TYPE_NONE; 
			pkNfy->Pop( byAddType );
			pkNfy->Pop( byRemoveType );

			PgSmallArea *pkUnitArea = GetArea( pkUnit->LastAreaIndex() );
			if ( !pkUnitArea )
			{
				CAUTION_LOG( BM::LOG_LV3, _T("PT_U_G_NFY_SYNCTYPE_CHANGE But... LastArea Error!!! Name<") << pkUnit->Name() << _T("> CharGuid<") << pkUnit->GetID() << _T(">") );
				AdjustArea( pkUnit, true, true );
			}
			else
			{
				PgPet *pkPet = NULL;
				if ( UT_PLAYER == pkUnit->UnitType() )
				{
					PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
					if ( pkPlayer )
					{
						pkPet = GetPet( pkPlayer );
						if ( pkPet )
						{
							pkPet->SetSyncType( pkUnit->GetSyncType(), true );// only state change
						}
						PgSubPlayer* pkSubPlayer = GetSubPlayer( pkPlayer );
						if( pkSubPlayer )
						{// only state change
							pkSubPlayer->SetSyncType( pkUnit->GetSyncType(), true );
						}
					}
				}

				if ( byAddType )
				{
					SendAreaData( pkUnit, pkUnitArea, NULL, byAddType );

					if ( pkPet )
					{
						SendAreaData( pkPet, pkUnitArea, NULL, byAddType );
					}
				}
				
				if ( SYNC_TYPE_RECV_ADD & byRemoveType )
				{
					// 다른 Unit을 Add받는 기능이 제거 되었다면...
					// 기존에 있는 Unit을 제거하라고 보내주어야 한다.
					UNIT_PTR_ARRAY kDelUnitArray;
					for (ESmallArea_Direction eDir=SaDir_Begin; eDir<SaDir_Max; eDir = ESmallArea_Direction(eDir+1))
					{
						PgSmallArea* pkArea = pkUnitArea->GetAdjacentArea(eDir);
						if ( pkArea != NULL )
						{
							pkArea->GetUnitList( kDelUnitArray, pkUnit, UT_NONETYPE, SYNC_TYPE_SEND_ADD );
						}
					}

					if ( kDelUnitArray.size() )
					{
						BM::Stream kDPacket(PT_M_C_NFY_REMOVE_CHARACTER, kDelUnitArray.size() );

						UNIT_PTR_ARRAY::const_iterator unit_itr = kDelUnitArray.begin();
						for ( ; unit_itr!=kDelUnitArray.end() ; ++unit_itr )
						{
							kDPacket.Push( unit_itr->pkUnit->GetID() );
						}

						pkUnit->Send( kDPacket, E_SENDTYPE_SELF );
					}
				}

				if ( SYNC_TYPE_SEND_ADD & byRemoveType )
				{
					// 다른 Unit에게 Add주는 기능이 제거 되었다면...
					// 다른 플레이어에게 RemoveCharacter을 보내야 한다.
					size_t const iSize = ( pkPet ? 2 : 1 );

					BM::Stream kDPacket(PT_M_C_NFY_REMOVE_CHARACTER, iSize );
					kDPacket.Push( pkUnit->GetID() );

					if ( 1 < iSize )
					{
						kDPacket.Push( pkPet->GetID() );
					}
					
					pkUnitArea->Broadcast_Adjacent( &kDPacket, pkUnit->GetID(), SYNC_TYPE_RECV_ADD, E_SENDTYPE_NONE );
				}
			}
		}break;
	case PT_U_G_NFY_SEND_CALLER:
		{
			BM::GUID kCallerID;
			DWORD dwSendFlag = 0;
			pkNfy->Pop( kCallerID );
			pkNfy->Pop( dwSendFlag );

			CUnit *pkUnit = PgObjectMgr2::GetUnit( UT_PLAYER, kCallerID );
			if ( pkUnit )
			{
				BM::Stream::DEF_STREAM_TYPE wStreamType = 0;
				pkNfy->Pop( wStreamType );

				BM::Stream kSPacket( wStreamType );
				kSPacket.Push( *pkNfy );
				pkUnit->Send( kSPacket, E_SENDTYPE_SELF|dwSendFlag );
			}
		}break;
	case PT_U_G_NFY_PT_M_T_USEITEM:
		{
			PgBase_Item kItem;
			kItem.ReadFromPacket(*pkNfy);

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const *pkDef = kItemDefMgr.GetDef(kItem.ItemNo());
			if(!pkDef)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("[PT_U_G_NFY_PT_M_T_USEITEM] Cannot find ItemDef itemno=") << kItem.ItemNo());
				return;
			}

			int const iRange = pkDef->GetAbil(AT_DETECT_RANGE);
			int const iTargetType = pkDef->GetAbil(AT_TARGET_TYPE);

			if(ESTARGET_NONE == iTargetType)
			{
				return;
			}

			UNIT_PTR_ARRAY kUnitArray;
			GetUnitTargetList(pkUnit, kUnitArray, iTargetType, iRange, iRange);

			BM::Stream kPacket(PT_M_C_NFY_USEITEM);
			kPacket.Push(pkUnit->Name());
			kPacket.Push(kItem.ItemNo());
			kPacket.Push(GroundKey().GroundNo());
			Broadcast( kPacket, BM::GUID::NullData());

			for(UNIT_PTR_ARRAY::iterator itor_unit = kUnitArray.begin();itor_unit != kUnitArray.end();++itor_unit)
			{
				if((*itor_unit).pkUnit->GetID() == pkUnit->GetID())
				{
					continue;
				}

				BM::Stream kPacket = (*pkNfy);
				PgAction_UseItem kAction( this, kItem, 1, kPacket );
				kAction.DoAction((*itor_unit).pkUnit,NULL);
			}

		}break;
	case PT_C_M_REQ_DEFENCE_TIMEPLUS:
	case PT_C_M_REQ_DEFENCE_POTION:
		{
			// 미션 내에서만 사용 가능한 아이템
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet type=") << wType << _T(", Unit=") << pkUnit->GetID());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
		}break;
	}//switch(wType)
}
