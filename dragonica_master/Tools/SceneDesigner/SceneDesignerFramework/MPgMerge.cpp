#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "MUtility.h"
#include "IEntityPathService.h"

MScene* MPgMerge::ParsingGSA(String* pkPathStr)
{
	// Delete PaletteList in dll before Load
	//RemoveAllPaletteInDll(strFormat);

	// Create the appropriate file format handler based on strFormat
    const char* pcFormat = "GSA";
    NiEntityStreaming* pkEntityStreaming =
        NiFactories::GetStreamingFactory()->GetPersistent(pcFormat);
	
	if(NULL == pkEntityStreaming)
	{
		return false;
	}

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkEntityStreaming->SetErrorHandler(spErrors);

    // Load the scene
    const char* pcFilename = MStringToCharPointer(pkPathStr);
    NiBool bSuccess = pkEntityStreaming->Load(pcFilename);
    MFreeCharPointer(pcFilename);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);
    pkEntityStreaming->SetErrorHandler(NULL);

    if (!bSuccess)
    {
        return NULL;
    }
    
    assert(pkEntityStreaming->GetSceneCount() == 1);
	MScene* pmScene = MSceneFactory::Instance->Get(pkEntityStreaming->GetSceneAt(0));//SceneFactory->Get(pkEntityStreaming->GetSceneAt(0));
	
	//m_strScenePathName = pkPathStr;
	// PGProperty Load
    pcFilename = MStringToCharPointer(pkPathStr);
    pkEntityStreaming->Load(pcFilename);
    MFreeCharPointer(pcFilename);
	// End PGProperty Load

	pkEntityStreaming->RemoveAllScenes(); // Because static instance	

	MEntity* pmEntities[] = pmScene->GetEntities();
	MSelectionSet* pmSets[] = pmScene->GetSelectionSets();
	
	
	for(int i=0;pmEntities->Count>i;++i)
	{
		String* propnames[] = pmEntities[i]->GetPropertyNames();
		String* LightName = "Light Type";
		bool bIsLight = propnames->Contains(LightName);
		
		if(false!=bIsLight)//true == MLightManager::EntityIsLight(pmEntities[i]))
		{
			pmScene->RemoveEntity(pmEntities[i], false);
			for(int j=0;pmSets->Length>j;++j)
			{
				if(NULL != pmSets[j]->GetEntityByName(pmEntities[i]->Name))
				{
					pmSets[j]->RemoveEntity(pmEntities[i]);
				}
			}
			//continue;
		}
	}

    return pmScene;
}

String* MPgMerge::ParsingPGPropertyType(MEntity* pkTarget)
{
	return pkTarget->GetNiEntityInterface()->GetPGProperty();
	//return ;
}