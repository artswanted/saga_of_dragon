#include "StdAfx.h"
#include "constant.h"
#include "PgUnitEffectMgr.h"
#include "DefAbilType.h"
#include "PgControlDefMgr.h"


EffectPool PgUnitEffectMgr::ms_kEffectPool(20, 20);
ItemEffectPool PgUnitEffectMgr::ms_kItemEffectPool(20, 10);

PgUnitEffectMgr::PgUnitEffectMgr(void)
:	m_bDamaged(false), m_kTargetGroundNo(0)
{
}

PgUnitEffectMgr::~PgUnitEffectMgr(void)
{
	Clear();
}

void PgUnitEffectMgr::Clear()
{
	ContEffect::iterator itor = m_kContEffect.begin();
	while(itor != m_kContEffect.end())
	{
		ms_kEffectPool.Delete((*itor).second);
		++itor;
	}
	ContEffect::iterator itor2 = m_kContItemEffect.begin();
	while(itor2 != m_kContItemEffect.end())
	{
		CItemEffect* pkItemEffect = dynamic_cast<CItemEffect*>((*itor2).second);
		ms_kItemEffectPool.Delete(pkItemEffect);
		++itor2;
	}
	MustAllClear(false);
	m_kContEffect.clear();
	m_kContItemEffect.clear();
	m_bDamaged = false;
	MustCurseClear(false);
	TargetGroundNo(0);

	CAbilObject::Clear();
}

void PgUnitEffectMgr::WriteToPacket(BM::Stream &rPacket, bool const bIsSimple)const
{
	rPacket.Push(TargetGroundNo());

	size_t const effect_count = m_kContEffect.size() + m_kContItemEffect.size();
	size_t const iPos = rPacket.WrPos();
	size_t iWriteCount = 0;
	rPacket.Push(effect_count);

	ContEffectItor kItor;
	GetFirstEffect(kItor);
	CEffect* pkEffect = NULL;
	while ((pkEffect = GetNextEffect(kItor)) != NULL)
	{
		EEffectType eType = (EEffectType) pkEffect->GetType();
		if( EFFECT_TYPE_NONE == eType )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Effect Type is EFFECT_TYPE_NONE, ID["<<pkEffect->GetEffectNo()<<L"]");
		}
		//if (eType != EFFECT_TYPE_PASSIVE)	// Passive Effect는 전송하지 않는다.
		if (pkEffect->IsSendingTypeDuringMapMove())
		{
			rPacket.Push(pkEffect->GetType());
			rPacket.Push(pkEffect->GetEffectNo());
			rPacket.Push(pkEffect->GetValue());
			int const iSubEffectSize = pkEffect->GetSubEffectSize();
			rPacket.Push(iSubEffectSize);
			for(int i = 0; i < iSubEffectSize; ++i)
			{
				rPacket.Push(pkEffect->GetSubEffectNoByIndex(i));
			}

			if(!bIsSimple)
			{
				BM::DBTIMESTAMP_EX const& rkExpireTime = pkEffect->ExpireTime();
				if (false == rkExpireTime.IsNull())
				{
					BM::DBTIMESTAMP_EX kLocalTime;
					g_kEventView.GetLocalTime(kLocalTime);
					__int64 const i64RemainTimeSec = CGameTime::GetElapsedTime(rkExpireTime, kLocalTime, CGameTime::SECOND);
					unsigned long const ulEndTime = g_kEventView.GetServerElapsedTime() + static_cast<unsigned long>(i64RemainTimeSec * 1000);
					pkEffect->SetEndTime(ulEndTime);
				}

				rPacket.Push(pkEffect->GetTime());
				rPacket.Push(pkEffect->GetInterval());
				rPacket.Push(pkEffect->GetStartTime());
				rPacket.Push(pkEffect->GetEndTime());
				rPacket.Push(pkEffect->ExpireTime());
			}

			switch(eType)
			{
			case EFFECT_TYPE_ITEM:
				{
					CItemEffect* pkItemEffect = dynamic_cast<CItemEffect*>(pkEffect);
					if (pkItemEffect != NULL)
					{
						rPacket.Push(pkItemEffect->ItemNo());
						if(0 == pkItemEffect->ItemNo())
						{
							CAUTION_LOG(BM::LOG_LV1, __FL__<<L"EFFECT_TYPE_ITEM ItemNo == "<<pkItemEffect->ItemNo()<<L" ! Type:"<<pkEffect->GetType()<<L" EffectNo:"<<pkEffect->GetEffectNo()<<L" Value:"<<pkEffect->GetValue()<<L" Time:"<<pkEffect->GetTime()<<L" Interval:"<<pkEffect->GetInterval()<<L"");
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("ItemNo is 0"));
						}
					}
					else
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"EFFECT_TYPE_ITEM but cannot dynamic_cast<CItemEffect*> EffectID["<<pkEffect->GetEffectNo()<<L"]");
						rPacket.Push((int)0);
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkItemEffect is NULL"));
					}
				}break;
			default:
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalide CaseType"));
				}break;
			}

			int iToggleOff = 0;
			pkEffect->GetActArg(ACTARG_TOGGLESKILL, iToggleOff);
			rPacket.Push(iToggleOff);

			++iWriteCount;
		}
	}
 	if (iWriteCount != effect_count)
 	{
 		rPacket.ModifyData((int)iPos, &iWriteCount, sizeof(size_t));
 	}

	if(bIsSimple)
	{
		CAbilObject::WriteToPacket(rPacket);
	}
}

void PgUnitEffectMgr::ReadFromPacket(BM::Stream &rPacket, bool const bIsSimple)//이펙트 키로 넣는거지.
{
	Clear();

	int iTargetGroundNo = 0;
	rPacket.Pop(iTargetGroundNo);

	size_t effect_count = 0;

	int iEffectNo = 0, iEffectValue = 0;
	unsigned long lTime = 0, lInterval = 0, lStartTime = 0, lEndTime = 0;
	BYTE byType;
	BM::DBTIMESTAMP_EX kExpireTime;

	rPacket.Pop(effect_count);
	int iItemNo;
	while(effect_count--)
	{
		bool bSuccess = true;
		SActArg kActArg;
		EAddEffectResult eResult;

		if(iTargetGroundNo)
		{
			kActArg.Set(ACTARG_GROUND_NO, iTargetGroundNo);
		}

		rPacket.Pop(byType);
		rPacket.Pop(iEffectNo);
		rPacket.Pop(iEffectValue);
		int iSubEffectSize = 0;
		rPacket.Pop(iSubEffectSize);

		std::vector<int> kSubEffectNoCount;

		int iSubEffectNo = 0;
		for(int i = 0; i < iSubEffectSize; ++i)
		{
			rPacket.Pop(iSubEffectNo);
			kSubEffectNoCount.push_back(iSubEffectNo);
		}

		if(!bIsSimple)
		{
			rPacket.Pop(lTime);
			rPacket.Pop(lInterval);
			rPacket.Pop(lStartTime);
			rPacket.Pop(lEndTime);
			rPacket.Pop(kExpireTime);
		}
		else
		{
			// 변수 초기화
			lTime = lInterval = lStartTime = lEndTime = 0;
			kExpireTime.Clear();
		}
		switch((EEffectType)byType)
		{
		case EFFECT_TYPE_ITEM:
			{
				rPacket.Pop(iItemNo);
				bSuccess = (iItemNo > 0) ? true : false;
				kActArg.Set(ACTARG_ITEMNO, iItemNo);

				if(!bSuccess)
				{
					CAUTION_LOG(BM::LOG_LV1, __FL__<<L"EFFECT_TYPE_ITEM ItemNo == "<<iItemNo<<L" ! Type:"<<static_cast<int>(byType)<<L" EffectNo:"<<iEffectNo<<L" Value:"<<iEffectValue<<L" Time:"<<lTime<<L" Interval:"<<lInterval<<L"");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bSuccess is false"));
				}
			}break;
		default:
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalide CaseType"));
			}break;
		}

		int iToggleOff = 0;
		rPacket.Pop(iToggleOff);
		if(0 < iToggleOff)
		{
			kActArg.Set(ACTARG_TOGGLESKILL, iToggleOff);
		}		
		
		if(bSuccess)
		{
			SEffectCreateInfo kCreate;
			kCreate.eType = static_cast<EEffectType>(byType);
			kCreate.iEffectNum = iEffectNo;
			kCreate.iValue = iEffectValue;
			kCreate.kActArg = kActArg;
			kCreate.dwElapsedTime = lTime;
			kCreate.dwInterval = lInterval;
			kCreate.dwStartTime = lStartTime;
			kCreate.dwEndTime = lEndTime;
			kCreate.kWorldExpireTime = kExpireTime;
			CEffect* pkEffect = AddEffect(kCreate, eResult);
			if (pkEffect != NULL)
			{
				pkEffect->swap_SubEffectCont(kSubEffectNoCount);
			}
		}
	}

	if(bIsSimple)
	{
		CAbilObject::ReadFromPacket(rPacket);
	}
}

void PgUnitEffectMgr::GetFirstEffect(ContEffectItor& rkItor) const
{
	rkItor.kItor = (ContEffect::iterator&) m_kContEffect.begin();
	rkItor.byCurrentCont = (BYTE)EFFECT_TYPE_NORMAL;
}

void PgUnitEffectMgr::GetFirstItemEffect(ContEffectItor& rkItor) const
{
	rkItor.kItor = (ContEffect::iterator&) m_kContItemEffect.begin();
	rkItor.byCurrentCont = (BYTE)EFFECT_TYPE_ITEM;
}

CEffect* PgUnitEffectMgr::GetNextEffect(ContEffectItor& rkItor) const
{
	if (rkItor.byCurrentCont == (BYTE) EFFECT_TYPE_NORMAL)
	{
		if (rkItor.kItor != m_kContEffect.end())
		{
			CEffect* pkEffect = rkItor.kItor->second;
			++(rkItor.kItor);
			return pkEffect;
		}
		rkItor.byCurrentCont = (BYTE) EFFECT_TYPE_ITEM;
		rkItor.kItor = (ContEffect::iterator&) m_kContItemEffect.begin();
	}
	if (rkItor.byCurrentCont == (BYTE) EFFECT_TYPE_ITEM)
	{
		if (rkItor.kItor != m_kContItemEffect.end())
		{
			CEffect* pkEffect = rkItor.kItor->second;
			++(rkItor.kItor);
			return pkEffect;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

//CEffect* PgUnitEffectMgr::AddEffect(BYTE byType, int const iEffectNo, int iValue, SActArg* pkArg, unsigned long ulElapsedTime, unsigned long ulInterval,
//EAddEffectResult& eResult, unsigned long ulEndTime)
CEffect* PgUnitEffectMgr::AddEffect(SEffectCreateInfo& rkCreate, EAddEffectResult& eResult)
{
	int iEffectKey = rkCreate.iEffectNum;
	if(0 >= iEffectKey)
	{
		return NULL;
	}

	{//Check
		GET_DEF(CEffectDefMgr, kEffectDefMgr);
		CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(rkCreate.iEffectNum);
		if (pkEffectDef)
		{
			int iMapNo = 0;
			rkCreate.kActArg.Get(ACTARG_GROUND_NO, iMapNo);
			if(false == pkEffectDef->IsUseMapNo(iMapNo))
			{
				return NULL;
			}
		}
	}

	EEffectType eEffectType = rkCreate.eType;
	if( EFFECT_TYPE_NONE == eEffectType )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Effect Type is EFFECT_TYPE_NONE, ID["<< rkCreate.iEffectNum <<L"]");
	}
	if (eEffectType == EFFECT_TYPE_NORMAL)
	{
		// EffectDef 에서 EffectType 값 읽어 오자
		GET_DEF(CEffectDefMgr, kEffectDefMgr);
		eEffectType = (EEffectType) kEffectDefMgr.GetAbil(rkCreate.iEffectNum, AT_TYPE);
	}

	int iTempKey = 0;
	if (S_OK == rkCreate.kActArg.Get(ACTARG_ITEMNO, iTempKey))
	{
		iEffectKey = iTempKey;
	}
	
	eResult = E_AERESULT_NONE;
	// 같은 종류의 Effect가 있는지 검사한다.
	CEffect* pkNew = FindInGroup(iEffectKey, true);
	if (pkNew != NULL)
	{
		int iLevel = (eEffectType != EFFECT_TYPE_ITEM) ? CEffect::GetLevel(iEffectKey) : CItemEffect::GetLevel(iEffectKey);
		if (pkNew->Level() > iLevel)
		{
			// 높은 레벨의 Effect가 이미 있다.
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
			return NULL;
		}
		else if (pkNew->Level() == iLevel)
		{
			// Level과 EffectNo가 동일하면 동일한 효과의 Effect라고 간주하고 기존의 Effect에 값을 변경하자.
			GET_DEF(CEffectDefMgr, kEffectDefMgr);
			int const iNoDup = (EEffectType) kEffectDefMgr.GetAbil(rkCreate.iEffectNum, AT_EFFECT_NO_DUPLICATE);	//같은 레벨의 이펙트가 걸려 있으면 다시 걸지 않겠다는 뜻
			if(0==iNoDup)
			{
				pkNew->Begin( rkCreate, true );
				eResult = EAddEffectResult(eResult|E_AERESULT_RESTART); 
			}
			else
			{
				eResult = EAddEffectResult(eResult|E_AERESULT_RESTART_NOT_SEND_CLIENT); 				
			}
		}
		else
		{
			// 새로운 Effect보다 낮은 Level의 Effect가 추가 요청 들어 왔으므로 기존의 Effect를 지우고 새로운 Effect를 생성해야 한다.
			// 지워진것을 Unit에 알려줘야 Unit->Ground에 알려주어서 Effect_EndXXXX 루아 함수를 호출 해 줄 수 있다.
			if (rkCreate.eOption & SEffectCreateInfo::ECreateOption_CallbyServer)
			{
				eResult = EAddEffectResult(eResult|E_AERESULT_DELEFFECT);
				rkCreate.kActArg.Set(ACTARG_DEL_EFFECT, pkNew->GetKey());
			}
			pkNew = NULL;
		}
	}

	if (pkNew == NULL)
	{
		// 진짜로 새로 생성시켜야 한다.
		switch(eEffectType)
		{
		case EFFECT_TYPE_ITEM:
			{
				pkNew = ms_kItemEffectPool.New();
				pkNew->Init();
				auto ret = m_kContItemEffect.insert(std::make_pair(iEffectKey,pkNew));
				if (!ret.second)
				{
					CItemEffect* pkItemEffect = dynamic_cast<CItemEffect*>(pkNew);
					ms_kItemEffectPool.Delete(pkItemEffect);
					pkNew = NULL;
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
				}
			}break;
		default:
			{
				pkNew = ms_kEffectPool.New();
				pkNew->Init();
				auto ret = m_kContEffect.insert(std::make_pair(iEffectKey, pkNew));
				if (!ret.second)
				{
					ms_kEffectPool.Delete(pkNew);
					pkNew = NULL;
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
				}
			}break;
		}
		if (pkNew != NULL)
		{
			SEffectCreateInfo kNewCreate = rkCreate;
			kNewCreate.eType = eEffectType;
			kNewCreate.kActArg.Remove(ACTARG_DEL_EFFECT);
			pkNew->Begin( kNewCreate, false );
			eResult = EAddEffectResult(eResult|E_AERESULT_NEWCREATE);
		}
	}
	return pkNew;
}

bool PgUnitEffectMgr::DeleteEffect(int const iEffectKey)
{
	ContEffect::iterator itor = m_kContEffect.find(iEffectKey);
	if (m_kContEffect.end() == itor)
	{
		itor = m_kContItemEffect.find(iEffectKey);
		if (itor == m_kContItemEffect.end())
		{
			goto __ITEM_EFFECT_DELETE;
		}
	}

	CEffect* pkEffect = itor->second;
	// Packet을 먼저 보내고 Effect를 지워야 한다.
	// 이유 : Effect_EndXXXX() Lua 함수 안에서 CEffect 개체가 필요하다.
	// EFFECT_TYPE_PASSIVE : Passive에 의해 생성된 Effect이며 Client로 전송할 필요가 없다.
	//if (pkEffect != NULL && pkEffect->GetAbil(AT_TYPE) != EFFECT_TYPE_PASSIVE)
	//{
	//	BM::Stream kDPacket(PT_U_G_NFY_DELETE_EFFECT);
	//	kDPacket.Push(iEffectNo);
	//	VNotify(&kDPacket);
	//}
	EEffectType eType = (EEffectType) itor->second->GetType();
	switch(eType)
	{
	case EFFECT_TYPE_ITEM:
		{
			m_kContItemEffect.erase(itor);
			CItemEffect* pkItemEffect = dynamic_cast<CItemEffect*>(pkEffect);
			ms_kItemEffectPool.Delete(pkItemEffect);
			goto __ITEM_EFFECT_DELETE;
		}break;
	default:
		{
			m_kContEffect.erase(itor);
			ms_kEffectPool.Delete(pkEffect);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return true ;

__ITEM_EFFECT_DELETE:
	ContEffect::iterator iter = m_kContItemEffect.begin();
	while( iter != m_kContItemEffect.end() )
	{
		if( iter->second->GetEffectNo() == iEffectKey )
		{
			CEffect* pkEffect = iter->second;
			m_kContItemEffect.erase(iter);
			CItemEffect* pkItemEffect = dynamic_cast<CItemEffect*>(pkEffect);
			ms_kItemEffectPool.Delete(pkItemEffect);
			return true;
		}
		++iter;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

size_t PgUnitEffectMgr::Size() const
{
	return m_kContEffect.size();
}

CEffect const * PgUnitEffectMgr::FindEffect(int iEffectNo)const
{
	ContEffect::const_iterator itr = m_kContEffect.find(iEffectNo);
	if ( itr != m_kContEffect.end() )
	{
		return itr->second;
	}

	itr = m_kContItemEffect.find(iEffectNo);
	if ( itr != m_kContItemEffect.end() )
	{
		return itr->second;
	}
	return NULL;
}

CEffect* PgUnitEffectMgr::FindInGroup(int iEffectKey, bool bInGroup)const
{
	ContEffectItor kItor;
	GetFirstEffect(kItor);
	CEffect* pkEffect = NULL;
	while ((pkEffect = GetNextEffect(kItor)) != NULL)
	{
		if (pkEffect->IsSame(iEffectKey, bInGroup))
		{
			return pkEffect;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

CEffect* PgUnitEffectMgr::FindInGroupItemEffect(int iEffectKey, bool bInGroup)
{
	ContEffectItor kItor;
	GetFirstItemEffect(kItor);
	CEffect* pkEffect = NULL;
	while ((pkEffect = GetNextEffect(kItor)) != NULL)
	{		
		if (pkEffect->IsSame(iEffectKey, bInGroup))
		{
			return pkEffect;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

CEffect* PgUnitEffectMgr::FindInGroupItemEffectByEffectNo(int iEffectNo, bool bInGroup)
{
	ContEffect::const_iterator iter = m_kContItemEffect.begin();
	while( iter != m_kContItemEffect.end() )
	{
		CEffect& rkCEffect = *(iter->second);
		if( rkCEffect.GetEffectNo() == iEffectNo )
		{
			if (rkCEffect.IsSame(rkCEffect.GetKey(), bInGroup))
			{
				return &rkCEffect;
			}
		}
		++iter;
	}
	return NULL;
}

bool const PgUnitEffectMgr::FindInGroupItem(int iEffectKey, bool bInGroup) const
{
	ContEffect::const_iterator itor = m_kContItemEffect.find(iEffectKey);
	if (m_kContItemEffect.end() != itor)
	{
		return true;
	}

	ContEffect::const_iterator iter = m_kContItemEffect.begin();
	while( iter != m_kContItemEffect.end() )
	{
		if( iter->second->GetEffectNo() == iEffectKey )
		{
			return true;
		}
		++iter;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

ContEffect& PgUnitEffectMgr::GetContainer(EEffectType const eType)
{
	switch(eType)
	{
	case EFFECT_TYPE_ITEM:
		{
			return m_kContItemEffect;
		}break;
	default:
		{
			return m_kContEffect;
		}break;
	}
	return m_kContEffect;
}

size_t PgUnitEffectMgr::SaveToDB(BYTE * const pkBuffer, size_t const iBufferSize) const
{
	BM::Stream kSavePacket;
	kSavePacket.Push(s_EffectSaveVersion);

	size_t iSavedCount = 0;
	ContEffectItor kItor;
	GetFirstEffect(kItor);
	CEffect* pkEffect = NULL;
	while ((pkEffect = GetNextEffect(kItor)) != NULL)
	{
		CEffect::ESAVE_DB_TYPE const eSaveType= pkEffect->GetDBSaveType();
		if (eSaveType == CEffect::ESAVE_DB_TYPE_NOSAVE)
		{
			continue;
		}
		BM::Stream kTempSave;
		kTempSave.Push(eSaveType);

		switch (eSaveType)
		{
		case CEffect::ESAVE_DB_TYPE_PLAYTIME_ELAPSED_5S:
			{
				EEffectType eType = (EEffectType) pkEffect->GetType();
				WORD const wTimeSec = static_cast<WORD>(pkEffect->GetTime() / 5000);
				kTempSave.Push(static_cast<BYTE>(eType));	// 1
				kTempSave.Push(pkEffect->GetEffectNo());	// 5
				kTempSave.Push(wTimeSec);		// 7
				if (EFFECT_TYPE_ITEM == eType)
				{
					CItemEffect* pkItemEffect = dynamic_cast<CItemEffect*>(pkEffect);
					if (pkItemEffect != NULL)
					{
						kTempSave.Push(pkItemEffect->ItemNo());	// 11
						if(0 == pkItemEffect->ItemNo())
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"EFFECT_TYPE_ITEM ItemNo == "<<pkItemEffect->ItemNo()<<L" ! Type:"<<pkEffect->GetType()<<L" EffectNo:"<<pkEffect->GetEffectNo()<<L" Value:"<<pkEffect->GetValue()<<L" Time:"<<pkEffect->GetTime()<<L" Interval:"<<pkEffect->GetInterval()<<L"");
						}
					}
					else
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"EFFECT_TYPE_ITEM but cannot dynamic_cast<CItemEffect*> EffectID["<<pkEffect->GetEffectNo()<<L"]");
						kTempSave.Push((int)0);
					}
				}
			}break;
		case CEffect::ESAVE_DB_TYPE_WORLDTIME_EXPIRE:
			{
				EEffectType eType = (EEffectType) pkEffect->GetType();
				kTempSave.Push(static_cast<BYTE>(eType));	// 1
				kTempSave.Push(pkEffect->GetEffectNo());	// 5
				BM::PgPackedTime kExpireTime(pkEffect->ExpireTime());
				kTempSave.Push(kExpireTime);
				if (EFFECT_TYPE_ITEM == eType)
				{
					CItemEffect* pkItemEffect = dynamic_cast<CItemEffect*>(pkEffect);
					if (pkItemEffect != NULL)
					{
						kTempSave.Push(pkItemEffect->ItemNo());	// 11
						if(0 == pkItemEffect->ItemNo())
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"EFFECT_TYPE_ITEM ItemNo == "<<pkItemEffect->ItemNo()<<L" ! Type:"<<pkEffect->GetType()<<L" EffectNo:"<<pkEffect->GetEffectNo()<<L" Value:"<<pkEffect->GetValue()<<L" Time:"<<pkEffect->GetTime()<<L" Interval:"<<pkEffect->GetInterval()<<L"");
						}
					}
					else
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"EFFECT_TYPE_ITEM but cannot dynamic_cast<CItemEffect*> EffectID["<<pkEffect->GetEffectNo()<<L"]");
						kTempSave.Push((int)0);
					}
				}
			}break;
		default:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Unknown Effect Save Type =") << static_cast<BYTE>(eSaveType));
			}break;
		}

		if (kSavePacket.Size() + kTempSave.Size() > iBufferSize)
		{
			CAUTION_LOG(BM::LOG_LV4, __FL__ << _T("Buffer size is too small SavePacketSize=") << kSavePacket.Size() << _T(", BufferSize=") << iBufferSize);
			break;
		}
		else
		{
			kSavePacket.Push(kTempSave);
			++iSavedCount;
		}
	}
	memcpy_s(pkBuffer, iBufferSize, &(kSavePacket.Data().at(0)), std::min(iBufferSize, kSavePacket.Size()));
	return iSavedCount;
}

void PgUnitEffectMgr::LoadFromDB(BYTE const * const pkBuffer, size_t const iBufferSize)
{
	BM::Stream kLoadPacket;
	kLoadPacket.Push(pkBuffer, iBufferSize);
	short sVersion;

	kLoadPacket.Pop(sVersion);

	switch (sVersion)
	{
	case 0:
		{
			// 아무것도 저장 안되어 있으니, Load 할 것도 없다.
		}break;
	case 1:
		{
			LoadFromDB_v1(pkBuffer, iBufferSize);
		}break;
	case 2:
		{
			LoadFromDB_v2(pkBuffer, iBufferSize);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Cannot load Effect, Unknown Effect Save Version =") << sVersion);
		}break;
	}
}

void PgUnitEffectMgr::LoadFromDB_v1(BYTE const * const pkBuffer, size_t const iBufferSize)
{
	BM::Stream kLoadPacket;
	kLoadPacket.Push(pkBuffer, iBufferSize);
	short sVersion;

	kLoadPacket.Pop(sVersion);
	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	
	int const iMaxSaveEffect = 100;
	int iLoadCount = 0;
	HRESULT hError = S_OK;
	while(iLoadCount++ < iMaxSaveEffect)
	{
		BYTE byType;
		int iEffectNum;
		WORD wTimeSec;
		SActArg kActArg;
		EAddEffectResult eResult;
		if ( !kLoadPacket.Pop(byType) )
		{
			hError = E_CANNOT_READ;
			break;
		}
		if (byType == 0)
		{
			// 읽을것이 없다.
			break;
		}
		if ( !kLoadPacket.Pop(iEffectNum) )
		{
			hError = E_CANNOT_READ;
			break;
		}
		if ( !kLoadPacket.Pop(wTimeSec) )
		{
			hError = E_CANNOT_READ;
			break;
		}

		switch((EEffectType)byType)
		{
		case EFFECT_TYPE_ITEM:
			{
				int iItemNo;
				if ( !kLoadPacket.Pop(iItemNo) || iItemNo <= 0)
				{
					hError = E_CANNOT_READ;
					break;
				}
				kActArg.Set(ACTARG_ITEMNO, iItemNo);
			}break;
		default:
			{
			}break;
		}

		if( NULL != kEffectDefMgr.GetDef(iEffectNum) )
		{
			SEffectCreateInfo kCreate;
			kCreate.eType = static_cast<EEffectType>(byType);
			kCreate.iEffectNum = iEffectNum;
			kCreate.iValue = 0;
			kCreate.kActArg = kActArg;
			kCreate.dwElapsedTime = wTimeSec * 2000;
			CEffect* pkEffect = AddEffect(kCreate, eResult);
		}
	}

	if (hError != S_OK)
	{
		CAUTION_LOG(BM::LOG_LV3, __FL__ << _T("LoadEffect failed Error=") << hError << _T(", LoadedEffectCount=") << iLoadCount);
	}
}

void PgUnitEffectMgr::LoadFromDB_v2(BYTE const * const pkBuffer, size_t const iBufferSize)
{
	BM::Stream kLoadPacket;
	kLoadPacket.Push(pkBuffer, iBufferSize);
	short sVersion;

	kLoadPacket.Pop(sVersion);
	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	
	int const iMaxSaveEffect = 100;
	int iLoadCount = 0;
	HRESULT hError = S_OK;
	while(iLoadCount++ < iMaxSaveEffect)
	{
		BYTE byType;
		int iEffectNum;
		DWORD dwTime = 0;
		BM::PgPackedTime kWorldExpireTime;
		SActArg kActArg;
		EAddEffectResult eResult;
		
		CEffect::ESAVE_DB_TYPE eSaveType = CEffect::ESAVE_DB_TYPE_NOSAVE;
		if ( !kLoadPacket.Pop(eSaveType) )
		{
			hError = E_CANNOT_READ;
			break;
		}
		if (eSaveType == CEffect::ESAVE_DB_TYPE_NOSAVE)
		{
			// 읽을것이 없다.
			break;
		}

		switch (eSaveType)
		{
		case CEffect::ESAVE_DB_TYPE_PLAYTIME_ELAPSED_5S:
			{
				WORD wTimeSec;
				kLoadPacket.Pop(byType);
				kLoadPacket.Pop(iEffectNum);
				kLoadPacket.Pop(wTimeSec);
				dwTime = wTimeSec * 5000;
				if (EFFECT_TYPE_ITEM == byType)
				{
					int iItemNo = 0;
					kLoadPacket.Pop(iItemNo);
					kActArg.Set(ACTARG_ITEMNO, iItemNo);
				}
			}break;
		case CEffect::ESAVE_DB_TYPE_WORLDTIME_EXPIRE:
			{
				kLoadPacket.Pop(byType);
				kLoadPacket.Pop(iEffectNum);
				kLoadPacket.Pop(kWorldExpireTime);
				if (EFFECT_TYPE_ITEM == byType)
				{
					int iItemNo = 0;
					kLoadPacket.Pop(iItemNo);
					kActArg.Set(ACTARG_ITEMNO, iItemNo);
				}
			}break;
		default:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Unknown Effect Save Type =") << static_cast<BYTE>(eSaveType));
				return;	// 더이상 진행하면, 서버에 문제가 발생할지 모른다..
			}break;
		}
		if( NULL != kEffectDefMgr.GetDef(iEffectNum) )
		{
			SEffectCreateInfo kEffectCreate;
			kEffectCreate.kWorldExpireTime = kWorldExpireTime;	// 자동 type 변환
			if (kWorldExpireTime.Year() == 0)
			{
				// 버그 : BM::DBTIMESTAMP_EX::operator = ( BM::tagPackedTime const& rhs )
				//	위 함수를 버그 수정하면, 다른곳에 버그 생길지 몰라, 일단 이렇게 수정한다.
				//	위 함수 버그 수정은 trunk에만 올려서 테스트 진행 해야 한다.
				kEffectCreate.kWorldExpireTime.year = 0;
			}
			if (false == kWorldExpireTime.IsNull())
			{
				BM::DBTIMESTAMP_EX kLocalTime;
				kLocalTime.SetLocalTime();
				if (kLocalTime > kEffectCreate.kWorldExpireTime)
				{
					// Expire 됨.
					continue;
				}
			}
			kEffectCreate.eType = static_cast<EEffectType>(byType);
			kEffectCreate.iEffectNum = iEffectNum;
			kEffectCreate.kActArg = kActArg;
			kEffectCreate.dwElapsedTime = dwTime;
			kEffectCreate.dwInterval = 0;
			CEffect* pkEffect = AddEffect(kEffectCreate, eResult);
		}
	}

	if (hError != S_OK)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("LoadEffect failed Error=") << hError << _T(", LoadedEffectCount=") << iLoadCount);
	}
}