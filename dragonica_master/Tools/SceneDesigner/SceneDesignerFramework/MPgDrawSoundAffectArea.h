#pragma once

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MPgDrawSoundAffectArea : public MDisposable
    {
    public:
		void Render(MRenderingContext* pmRenderingContext);
		void CreateAreaMesh();
        
    protected:
        virtual void Do_Dispose(bool bDisposing);

    private public:
		static void Init();
		static void Shutdown();
		static bool InstanceIsValid();
		__property static MPgDrawSoundAffectArea* get_Instance();

	private:
		__property static ISelectionService* get_SelectionService();
        static ISelectionService* ms_pmSelectionService=NULL;
		static int const ms_iMaxVerts = 512;
		static MPgDrawSoundAffectArea*	ms_pmThis = NULL;
		static NiLines* m_pkMaxDist=NULL;
		static NiLines* m_pkAttenDist=NULL;
		static NiPoint3* m_pkMaxDistVerts=NULL;
		static NiPoint3* m_pkAttenDistVerts=NULL;
		static NiBool* m_pkMaxDistConnect=NULL;
		static NiBool* m_pkAttenDistConnect=NULL;
		static NiMaterialProperty* m_pkMaxDistMaterial=NULL;
		static NiMaterialProperty* m_pkAttenDistMaterial=NULL;
		MPgDrawSoundAffectArea(){};
    };
}}}}