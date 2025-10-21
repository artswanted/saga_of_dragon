#pragma once

#include <NiPhysX.h>

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MPgDrawPhysX : public MDisposable
    {
    public:
		void Render(MRenderingContext* pmRenderingContext);
		void CreatePhysX();
        
    protected:
        virtual void Do_Dispose(bool bDisposing);

    private public:
		static void Init();
		static void Shutdown();
		static bool InstanceIsValid();
		__property MPgDrawPhysX* get_Instance();

	private:
		static NiPhysXManager* ms_pkPhysXManager = NULL;
		static NiPhysXScene*	ms_pkPhysXScene = NULL;
		static MPgDrawPhysX*	ms_pmThis = NULL;
		static NiNode*			ms_pkScene = NULL;
		MPgDrawPhysX(){};
    };
}}}}