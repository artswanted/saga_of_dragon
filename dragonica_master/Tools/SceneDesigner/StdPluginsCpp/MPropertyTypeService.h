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

#pragma once

using namespace System::Collections;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MPropertyTypeService : public MDisposable,
        public IPropertyTypeService
    {
    public:
        MPropertyTypeService();

    private:
        Hashtable* m_pmNameToType;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // IService members.
    public:
        __property String* get_Name();
        bool Initialize();
        bool Start();

    // IPropertyTypeService members.
    public:
        void RegisterType(PropertyType* pmType);
        void UnregisterType(PropertyType* pmType);
        void Clear();
        PropertyType* LookupType(String* strName);
        PropertyType* GetAllPropertyTypes()[];
    };
}}}}
