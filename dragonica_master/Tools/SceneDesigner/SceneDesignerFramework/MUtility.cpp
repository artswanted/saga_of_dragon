// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Precompiled Header
#include "SceneDesignerFrameworkPCH.h"

#include "MUtility.h"
#include <NiVersion.h>
#include "ServiceProvider.h"
#include <NiFactories.h>

using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

//---------------------------------------------------------------------------
IMessageService* MUtility::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
String* MUtility::GetStreamingFormatDescriptions()[]
{
	NiTPrimitiveArray<const char*> kKeys;
    NiFactories::GetStreamingFactory()->GetKeys(kKeys);
	unsigned int uiKeySize = kKeys.GetSize();

	String* astrKeys[] = new String*[uiKeySize];

	for (unsigned int ui = 0; ui < uiKeySize; ui++)
	{
		const char* pcKey = kKeys.GetAt(ui);
		NiEntityStreaming* pkStreamingHandler = 
			NiFactories::GetStreamingFactory()->GetPersistent(pcKey);

		astrKeys[ui] = pkStreamingHandler->GetFileDescription();
	}

	return astrKeys;
}
//---------------------------------------------------------------------------
String* MUtility::GetStreamingFormatExtensions()[]
{
	NiTPrimitiveArray<const char*> kKeys;
	NiFactories::GetStreamingFactory()->GetKeys(kKeys);
	unsigned int uiKeySize = kKeys.GetSize();

	String* astrKeys[] = new String*[uiKeySize];

	for(unsigned int ui = 0; ui < uiKeySize; ui++)
	{
		astrKeys[ui] = kKeys.GetAt(ui);
	}

	return astrKeys;
}
//---------------------------------------------------------------------------
float MUtility::GetCurrentTimeInSec()
{
    return NiGetCurrentTimeInSec();
}
//---------------------------------------------------------------------------
String* MUtility::GetGamebryoVersion()
{
    return GAMEBRYO_SDK_VERSION_STRING;
}
//---------------------------------------------------------------------------
String* MUtility::GetGamebryoBuildDate()
{
    return GAMEBRYO_BUILD_DATE_STRING;
}
//---------------------------------------------------------------------------
int MUtility::GetVersionFromString(String* strVersion)
{
    const char* pcVersion = MStringToCharPointer(strVersion);
    int iVersion = NiStream::GetVersionFromString(pcVersion);
    MFreeCharPointer(pcVersion);

    return iVersion;
}
//---------------------------------------------------------------------------
void MUtility::SetImageSubFolder(String* strPath)
{
    const char* pcPath = MStringToCharPointer(strPath);
	NiDevImageConverter::SetPlatformSpecificSubdirectory(pcPath, 1);
    MFreeCharPointer(pcPath);
}
//---------------------------------------------------------------------------
float MUtility::RadiansToDegrees(float fRadians)
{
    return (fRadians * 180.0f) / NI_PI;
}
//---------------------------------------------------------------------------
float MUtility::DegreesToRadians(float fDegrees)
{
    return (fDegrees * NI_PI) / 180.0f;
}
//---------------------------------------------------------------------------
unsigned char MUtility::FloatToRGB(float fColor)
{
    return (unsigned char) (fColor * 255.0f);
}
//---------------------------------------------------------------------------
float MUtility::RGBToFloat(unsigned char ucColor)
{
    return ((float) ucColor) / 255.0f;
}
//---------------------------------------------------------------------------
void MUtility::AddErrorInterfaceMessages(MessageChannelType eChannel,
    NiEntityErrorInterface* pkErrors)
{
    unsigned int uiErrorCount = pkErrors->GetErrorCount();
    for (unsigned int ui = 0; ui < uiErrorCount; ui++)
    {
        MessageService->AddMessage(eChannel, new Message(String::Format(
            "{0} {{{1}}} [{2}]",
            new String(pkErrors->GetErrorMessage(ui)),
            new String(pkErrors->GetEntityName(ui)),
            new String(pkErrors->GetPropertyName(ui))),
            pkErrors->GetErrorDescription(ui), NULL));
    }
}
//---------------------------------------------------------------------------
Guid MUtility::IDToGuid(const NiUniqueID& kID)
{
    int iSize = sizeof(kID.m_aucValue);
    System::Byte amBytes[] = new Byte[iSize];
    for (int i = 0; i < iSize; i++)
    {
        amBytes[i] = kID.m_aucValue[i];
    }
    return Guid(amBytes);
}
//---------------------------------------------------------------------------
void MUtility::GuidToID(Guid mGuid, NiUniqueID& kUniqueID)
{
    Byte pmGuidBytes[] = mGuid.ToByteArray();
    int iSize = pmGuidBytes->Count;
    for (int i = 0; i<iSize; i++)
    {
        kUniqueID.m_aucValue[i] = pmGuidBytes[i];
    }
}
//---------------------------------------------------------------------------
