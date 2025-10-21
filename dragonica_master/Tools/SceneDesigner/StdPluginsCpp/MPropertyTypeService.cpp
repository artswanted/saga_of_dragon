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
#include "StdPluginsCppPCH.h"

#include "MPropertyTypeService.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MPropertyTypeService::MPropertyTypeService()
{
    m_pmNameToType = new Hashtable();
}
//---------------------------------------------------------------------------
void MPropertyTypeService::Do_Dispose(bool bDisposing)
{
}
//---------------------------------------------------------------------------
// IService members.
//---------------------------------------------------------------------------
String* MPropertyTypeService::get_Name()
{
    MVerifyValidInstance;

    return "Property Type Service";
}
//---------------------------------------------------------------------------
bool MPropertyTypeService::Initialize()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
bool MPropertyTypeService::Start()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
// IPropertyTypeService members.
//---------------------------------------------------------------------------
void MPropertyTypeService::RegisterType(PropertyType* pmType)
{
    MVerifyValidInstance;

    if (pmType != NULL)
    {
        m_pmNameToType->Item[pmType->Name] = pmType;
    }
}
//---------------------------------------------------------------------------
void MPropertyTypeService::UnregisterType(PropertyType* pmType)
{
    MVerifyValidInstance;

    if (pmType != NULL)
    {
        m_pmNameToType->Remove(pmType->Name);
    }
}
//---------------------------------------------------------------------------
void MPropertyTypeService::Clear()
{
    MVerifyValidInstance;

    m_pmNameToType->Clear();
}
//---------------------------------------------------------------------------
PropertyType* MPropertyTypeService::LookupType(String* strName)
{
    MVerifyValidInstance;

    return dynamic_cast<PropertyType*>(m_pmNameToType->Item[strName]);
}
//---------------------------------------------------------------------------
PropertyType* MPropertyTypeService::GetAllPropertyTypes()[]
{
    MVerifyValidInstance;

    PropertyType* amAllProperties[] = 
        new PropertyType*[m_pmNameToType->Count];

    m_pmNameToType->Values->CopyTo(amAllProperties, 0);
    return amAllProperties;
}
//---------------------------------------------------------------------------
