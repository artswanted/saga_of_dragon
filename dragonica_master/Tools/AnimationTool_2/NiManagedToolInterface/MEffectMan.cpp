#include "stdafx.h"
#include "MEffectMan.h"
#include "MSharedData.h"

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
	AttachedEffectSlot::iterator itr = m_pkAttachedEffectSlot->begin();
	while(itr != m_pkAttachedEffectSlot->end())
	{
		pkEffect = dynamic_cast<PgEffect *>(*itr);
		DetachFrom(pkEffect);
		++itr;
	}

	m_pkAttachedEffectSlot->clear();

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

MEffectMan::PgEffect *MEffectMan::GetEffect(NiKFMTool::Effect *pkEffectData)
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
		PathContainer::iterator pathItr = m_pkPathContainer->find(std::string(kEffectName));
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
	pkEffect->SetLoop(pkEffectData->GetLoop());
	return pkEffect;
}

MEffectMan::PgEffect *MEffectMan::LoadEffect(const char *pcEffectPath)
{
	NiDevImageConverter::SetPlatformSpecificSubdirectory(RELATIVE_PATH "/Data/5_Effect/9_Tex",0);
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

bool MEffectMan::AttachToPoint(PgEffect *pkEffect, NiNode *pkTargetPoint, bool bUseLock)
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

	NiPoint3 const& rLoc = pkTargetPoint->GetWorldTranslate();
	pkEffectRoot->SetTranslate( rLoc );
	pkEffectRoot->SetWorldTranslate( rLoc );
	
	MSharedData* pkSharedData = MSharedData::Instance;
	if(pkSharedData)
	{
		if(bUseLock)
		{
			pkSharedData->Lock();
		}
		NiNode	*pkBackground = NiDynamicCast(NiNode, pkSharedData->GetScene(MSharedData::CHARACTER_INDEX));
		if(NULL!=pkBackground)
		{
			pkBackground->AttachChild(pkEffectRoot);
			pkBackground->Update(0.0f);
			pkBackground->UpdateProperties();
			pkBackground->UpdateEffects();
		}
		if(bUseLock)
		{
			pkSharedData->Unlock();
		}

		if (!pkBackground)
		{
			return false;
		}
	}

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
}

void MEffectMan::Update(float fAccumTime)
{
	PgEffect *pkEffect = 0;
	AttachedEffectSlot::iterator itr = m_pkAttachedEffectSlot->begin();
	while(itr != m_pkAttachedEffectSlot->end())
	{
		pkEffect = dynamic_cast<PgEffect *>(*itr);
		if(pkEffect && !pkEffect->GetLoop() && pkEffect->Expired(fAccumTime))
		{
			itr = m_pkAttachedEffectSlot->erase(itr);
			DetachFrom(pkEffect);
			continue;
		}
		++itr;
	}
}

MEffectMan::PgEffect::PgEffect()
{
}

MEffectMan::PgEffect::PgEffect(NiAVObjectPtr spEffect) : 
	m_spEffectRoot(spEffect), 
	m_fBeginTime(0.0f), m_fEndTime(0.0f),
	m_bLoop(false)
{
}

MEffectMan::PgEffect::~PgEffect()
{
	m_spEffectRoot = 0;
}

NiAVObject *MEffectMan::PgEffect::GetEffectRoot()
{
	return m_spEffectRoot;
}

void MEffectMan::PgEffect::SetEffectRoot(NiAVObject *pkEffect)
{
	m_spEffectRoot = pkEffect;
}

float MEffectMan::PgEffect::GetEndTime()
{
	return m_fEndTime;
}

void MEffectMan::PgEffect::SetEndTime(float fTime)
{
	m_fEndTime = fTime;
}

void MEffectMan::PgEffect::SetBeginTime(float fTime)
{
	m_fBeginTime = fTime;
}

float MEffectMan::PgEffect::GetBeginTime()
{
	return m_fBeginTime;
}

bool MEffectMan::PgEffect::GetLoop()
{
	return m_bLoop;
}

void MEffectMan::PgEffect::SetLoop(bool bLoop)
{
	m_bLoop = bLoop;
}

bool MEffectMan::PgEffect::Expired(float fAccumTime)
{
	return m_fBeginTime + m_fEndTime < fAccumTime;
}

void MEffectMan::PgEffect::Analyze(NiObjectNET *pkRoot)
{
	if(pkRoot == NULL)
	{
		return;
	}

	// Gather all time controllers from this object
	NiTimeController* pkController = pkRoot->GetControllers();
	while(pkController != NULL)
	{
		float fEndTime = pkController->GetEndKeyTime();
		if(m_fEndTime < fEndTime)
		{
			m_fEndTime = fEndTime;
		}

		pkController = pkController->GetNext();
	}

	if(NiIsKindOf(NiAVObject, pkRoot))
	{
		// NiProperties can have time controllers, so search them too
		NiAVObject* pkObj = (NiAVObject*) pkRoot;
		NiPropertyList* pkPropList = &(pkObj->GetPropertyList());
		NiTListIterator kIter = pkPropList->GetHeadPos();
		while(pkPropList != NULL && !pkPropList->IsEmpty() && kIter)
		{
			NiProperty* pkProperty = pkPropList->GetNext(kIter);
			if(pkProperty)
			{
				Analyze(pkProperty);
			}
		}
	}

	if(NiIsKindOf(NiNode, pkRoot))
	{
		NiNode* pkNode = (NiNode*) pkRoot;
		// Search all of the children
		for(unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
		{
			NiAVObject* pkObj = pkNode->GetAt(ui);
			Analyze(pkObj);
		}
		// NiDynamicEffects can have time controllers, so search them too
		const NiDynamicEffectList* pkEffectList = &(pkNode->GetEffectList());

		NiTListIterator kIter = pkEffectList->GetHeadPos();
		while(pkEffectList != NULL && !pkEffectList->IsEmpty() && kIter)
		{
			NiDynamicEffect* pkEffect = pkEffectList->GetNext(kIter);
			if(pkEffect)
			{
				Analyze(pkEffect);
			}
		}
	}
}

MEffectMan::PgEffect *MEffectMan::PgEffect::Clone()
{
	NiAVObject *pkNewEffectRoot = (NiAVObject *)m_spEffectRoot->Clone();
	if(!pkNewEffectRoot)
	{
		return 0;
	}

	PgEffect *pkCloned = NiExternalNew PgEffect(pkNewEffectRoot);
	if(!pkCloned)
	{
		return 0;
	}

	pkCloned->SetBeginTime(m_fBeginTime);
	pkCloned->SetEndTime(m_fEndTime);
	pkCloned->SetLoop(m_bLoop);

	return pkCloned;
}