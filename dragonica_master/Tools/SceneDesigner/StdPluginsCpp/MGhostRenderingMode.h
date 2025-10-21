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

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MGhostRenderingMode : public MDisposable,
        public IRenderingMode
    {
    public:
        MGhostRenderingMode();

    private:
        NiAlphaAccumulator* m_pkAlphaAccumulator;
        NiEntityErrorInterface* m_pkErrors;
        NiAlphaProperty* m_pkAlphaProperty;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // IRenderingMode members.
    public:
        __property String* get_Name();
        __property bool get_DisplayToUser();
        void Update(float fTime);
        void Begin(MRenderingContext* pmRenderingContext);
        void Render(MEntity* pmEntity, MRenderingContext* pmRenderingContext);
        void Render(MEntity* amEntities[],
            MRenderingContext* pmRenderingContext);
        void End(MRenderingContext* pmRenderingContext);
    };
}}}}
