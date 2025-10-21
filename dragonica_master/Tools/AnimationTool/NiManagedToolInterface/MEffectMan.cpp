#include "stdafx.h"
#include "MEffectMan.h"
#include "MSharedData.h"
#include "NiAnimationEventEffect.H"

using namespace NiManagedToolInterface;

MEffectMan::MEffectMan()
{
	m_pkEventContainer = NiExternalNew EventContainer;
	m_pkAttachedEffectSlot = NiExternalNew AttachedEffectSlot;
}

MEffectMan::~MEffectMan()
{
}

void MEffectMan::Init()
{
	MSharedData *pkData = MSharedData::Instance;
	pkData->Lock();
	pkData->SetEffectMan(this);
	pkData->Unlock();
}

void MEffectMan::DeleteContents()
{
	MEventMan::DeleteContents();
	PgEffect *pkEffect = 0;

	DetachAllEffects();

	EventContainer::iterator eventItr = m_pkEventContainer->begin();
	while(eventItr != m_pkEventContainer->end())
	{
		pkEffect = dynamic_cast<PgEffect *>(eventItr->second);
		NiDelete pkEffect;
		++eventItr;
	}
	m_pkEventContainer->clear();
}

bool MEffectMan::ShutDown()
{
	return true;
}
PgEffect *MEffectMan::FindEffectWithTextKey(std::string const &kTextKey)
{
	for(AttachedEffectSlot::iterator itor = m_pkAttachedEffectSlot->begin(); itor != m_pkAttachedEffectSlot->end(); ++itor)
	{
		PgEffect *pkEffect = *itor;
	
		NiStringExtraData *pkData = NiDynamicCast(NiStringExtraData,pkEffect->GetEffectRoot()->GetExtraData(PgEffect::EXTRA_DATA_EFFECT_TEXT_KEY.c_str()));
		if(!pkData)
		{
			continue;
		}
		
		if(pkData->GetValue() == kTextKey.c_str())
		{
			return	pkEffect;
		}
	}

	return	NULL;
}
PgEffect *MEffectMan::GetEffect(NiAnimationEventEffect *pkEffectData)
{
	if(!m_pkEventContainer)
	{
		return 0;
	}

	NiFixedString kEffectName = pkEffectData->GetEffectName();
	// 캐쉬에 있는지 체크한다.
	PgEffect *pkEffect = 0;
	EventContainer::iterator itr = m_pkEventContainer->find(std::string(kEffectName));
	if(itr == m_pkEventContainer->end())
	{
		std::string strEffectName((char const*)kEffectName);
		PathContainer::iterator pathItr = m_pkPathContainer->find(strEffectName);
		if(pathItr == m_pkPathContainer->end())
		{
			return 0;
		}

		// 이펙트를 로딩한다.
		pkEffect = LoadEffect(pathItr->second.c_str());
		if(!pkEffect)
		{
			return 0;
		}
		
		// 캐쉬 컨테이너에 추가한다.
		itr = m_pkEventContainer->insert(std::make_pair(kEffectName, pkEffect)).first;
	}

	// PgEffect를 Clone해서 반환해준다.
	pkEffect = dynamic_cast<PgEffect *>(itr->second->Clone());
	if(!pkEffect)
	{
		return 0;
	}

	pkEffect->GetEffectRoot()->SetScale(pkEffectData->GetScale());	
	pkEffect->SetLoop(false);
	return pkEffect;
}

PgEffect *MEffectMan::LoadEffect(const char *pcEffectPath)
{
	NiDevImageConverter::SetPlatformSpecificSubdirectory(RELATIVE_PATH "Data/5_Effect/9_Tex",0);
	NiStream kStream;
	if (!kStream.Load(pcEffectPath))
	{
		assert(!"nif stream loading failed");
		NiDevImageConverter::SetPlatformSpecificSubdirectory(0,0);
		return 0;
	}

	NiDevImageConverter::SetPlatformSpecificSubdirectory(0,0);
	NiNode *pkRoot = (NiNode *)kStream.GetObjectAt(0);
	assert(pkRoot);

	// PhysX 오브젝트는 숨긴다.
	NiAVObject *pkPhysX = pkRoot->GetObjectByName("PhysX");
	if(pkPhysX)
	{
		pkPhysX->SetAppCulled(true);
	}


	// char_root, Dummy01, Scene Root순으로 Effect를 가져온다.
	NiAVObjectPtr spAVObject = pkRoot->GetObjectByName("char_root");
	if(!spAVObject)
	{
		spAVObject = pkRoot->GetObjectByName("Dummy01");
		if(!spAVObject)
		{
			spAVObject = pkRoot->GetObjectByName("Scene Root");
		}
	}

	spAVObject->SetTranslate(0.0f, 0.0f, 0.0f);
	//spAVObject->SetRotate(NiQuaternion::IDENTITY);
	kStream.RemoveAllObjects();

	PgEffect *pkNewEffect = NiExternalNew PgEffect(spAVObject);
	if(!pkNewEffect)
	{
		return 0;
	}

	// Effect의 EndTime을 구한다.
	pkNewEffect->Analyze(pkNewEffect->GetEffectRoot());
	return pkNewEffect;
}

void MEffectMan::AddToAttachedEffectSlot(PgEffect *pkEffect)
{
	m_pkAttachedEffectSlot->push_back(pkEffect);
}

void MEffectMan::RemoveFromAttachedEffectSlot(PgEffect *pkEffect)
{
	AttachedEffectSlot::iterator itr = std::find(m_pkAttachedEffectSlot->begin(), m_pkAttachedEffectSlot->end(), pkEffect);
	if(itr == m_pkAttachedEffectSlot->end())
	{
		return;
	}

	m_pkAttachedEffectSlot->erase(itr);
}

bool MEffectMan::AttachTo(PgEffect *pkEffect, NiNode *pkTargetPoint)
{
	if(!pkEffect || !pkTargetPoint)
	{
		return false;
	}

	NiAVObject *pkEffectRoot = pkEffect->GetEffectRoot();
	if(!pkEffectRoot)
	{
		return false;
	}

	pkTargetPoint->AttachChild(pkEffectRoot);
	pkTargetPoint->Update(0.0f);
	pkTargetPoint->UpdateProperties();
	pkTargetPoint->UpdateEffects();

	NiTimeController::StartAnimations(pkEffectRoot, 0.0f);
	AddToAttachedEffectSlot(pkEffect);
	return true;
}

void MEffectMan::DetachFrom(PgEffect *pkEffect)
{
	if(!pkEffect)
	{
		return;
	}

	NiAVObject *pkEffectRoot = pkEffect->GetEffectRoot();
	if(!pkEffectRoot)
	{
		return;
	}

	NiNode *pkParent = pkEffectRoot->GetParent();
	if(pkParent)
	{
		pkParent->DetachChild(pkEffectRoot);
		pkParent->Update(0.0f);
		pkParent->UpdateProperties();
		pkParent->UpdateEffects();
	}

	RemoveFromAttachedEffectSlot(pkEffect);
	NiDelete pkEffect;
}

void MEffectMan::DetachAllEffects()
{
	PgEffect *pkEffect = 0;
	AttachedEffectSlot::iterator itr = m_pkAttachedEffectSlot->begin();
	while(itr != m_pkAttachedEffectSlot->end())
	{
		pkEffect = dynamic_cast<PgEffect *>(*itr);
		if(!pkEffect)
		{
			++itr;
			continue;
		}
		itr = m_pkAttachedEffectSlot->erase(itr);
		DetachFrom(pkEffect);
	}
	m_pkAttachedEffectSlot->clear();
}

void MEffectMan::Update(float fAccumTime)
{
	PgEffect *pkEffect = 0;
	AttachedEffectSlot::iterator itr = m_pkAttachedEffectSlot->begin();
	while(itr != m_pkAttachedEffectSlot->end())
	{
		pkEffect = dynamic_cast<PgEffect *>(*itr);
		if(pkEffect)
		{
			if(false == pkEffect->Update(fAccumTime))
			{
				itr = m_pkAttachedEffectSlot->erase(itr);
				DetachFrom(pkEffect);
				continue;
			}
		}
		++itr;
	}
}
