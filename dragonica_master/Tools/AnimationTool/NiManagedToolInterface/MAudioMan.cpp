#include "stdafx.h"
#include "MAudioMan.h"
#include "MSharedData.h"
#using <System.Xml.dll>

using namespace System::Xml;
using namespace System::Collections;
using namespace NiManagedToolInterface;

MAudioMan::MAudioMan()
{
	m_pkEventContainer = NiExternalNew EventContainer;
}

MAudioMan::~MAudioMan()
{
}

void MAudioMan::Init()
{
	MSharedData *pkData = MSharedData::Instance;
	pkData->Lock();
	pkData->SetAudioMan(this);
	pkData->Unlock();
}

bool MAudioMan::CreateAudioSystem(IntPtr hWnd)
{
	NiMilesAudioSystem *pkAudioSystem = (NiMilesAudioSystem *)NiAudioSystem::GetAudioSystem();
	pkAudioSystem->SetHWnd((HWND) hWnd.ToInt32());

	if(!pkAudioSystem->Startup("../../Miles"))
	{
		pkAudioSystem->SetHWnd(0);
		NiMessageBox("Couldn't initialize audio system. No Sound!", "Init Audio System Error");
		return false;
	}

	pkAudioSystem->SetBestSpeakerTypeAvailable();
	pkAudioSystem->SetUnitsPerMeter(50.0f);

	if(pkAudioSystem && pkAudioSystem->GetHWnd())
	{
		NiMilesListener *pkListener = pkAudioSystem->GetListener();

		MSharedData *pkData = MSharedData::Instance;
		pkData->Lock();
		NiNode *pkCameraRoot = (NiNode *)pkData->GetScene(MSharedData::MAIN_CAMERA_INDEX);
		if(pkCameraRoot)
		{
			pkCameraRoot->AttachChild(pkListener);
			pkCameraRoot->Update(0.0f);
		}
		pkData->Unlock();
	}

	return true;
}

void MAudioMan::DeleteContents()
{
	MEventMan::DeleteContents();
	m_pkEventContainer->clear();
}

bool MAudioMan::ShutDown()
{
	NiMilesAudioSystem *pkAudioSystem = (NiMilesAudioSystem *)NiAudioSystem::GetAudioSystem();
	if(!pkAudioSystem && pkAudioSystem->GetHWnd())
	{
		return false;
	}

	pkAudioSystem->Shutdown();
	return true;
}

NiAudioSourcePtr MAudioMan::GetAudioSource(const char *pcSoundName, float fVolume, float fMinDist, float fMaxDist)
{
	NiMilesAudioSystem *pkAudioSystem = (NiMilesAudioSystem*)NiAudioSystem::GetAudioSystem();
	if(!pkAudioSystem)
	{
		return 0;
	}
	
	NiAudioSourcePtr spAudioSource = 0;
	EventContainer::iterator itr = m_pkEventContainer->find(pcSoundName);
	if(itr == m_pkEventContainer->end())
	{
		spAudioSource = pkAudioSystem->CreateSource();
		if(!spAudioSource)
		{
			return 0;
		}

		const char *pcFilePath = GetEventPath(pcSoundName);
		if(!pcFilePath || !NiFile::Access(pcFilePath, NiFile::READ_ONLY))
		{
			return 0;
		}

		spAudioSource->SetFilename(pcFilePath);
		if(!spAudioSource->Load())
		{
			return 0;
		}

		itr = m_pkEventContainer->insert(std::make_pair(pcSoundName, spAudioSource)).first;
	}

	spAudioSource = (NiAudioSource *) itr->second->Clone();
	if(!spAudioSource)
	{
		return 0;
	}

	spAudioSource->SetGain(fVolume);
	spAudioSource->SetMinMaxDistance(fMinDist, fMaxDist);
	spAudioSource->Update(0.0f);
	
	return spAudioSource;
}