#pragma once

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MPgZoneControl : public MDisposable
    {
    public:
		void Render(MRenderingContext* pmRenderingContext);
		void AdjustMesh();

		__property static MPgZoneControl* get_Instance();
		
		__property void set_ZoneDraw(bool kZoneDraw);
		__property void set_PlayerBase(bool kPlayerBase);
		
		void SetAnchor(MEntity* pkTarget);
		MEntity* GetAnchor();
		void SetPlayer(MEntity* pkTarget);
		MEntity* GetPlayer();
		void SetWolrdBound(NiPoint3* pkMinPos, NiPoint3* pkMaxPos);
		void ComputeBound();
		        
    protected:
        virtual void Do_Dispose(bool bDisposing);

    private public:
		static void Init();
		static void Shutdown();
		static bool InstanceIsValid();		

		void CreateMesh();

	private:
		//싱글톤 인스턴스
		static MPgZoneControl*	ms_pmThis = NULL;

		//
		__property static ISelectionService* get_SelectionService();
        static ISelectionService* ms_pmSelectionService;

		//존을 그리기 위한 리소스
		static NiPoint3*	m_pkZoneVertexArray = NULL;	
		static NiMaterialProperty*	m_pkMaterial = NULL;
		static NiLines*		m_pkZoneLine = NULL;	
		static NiBool*		m_pkConnect = NULL;
		static MEntity*		m_pkAnchor = NULL;
		static MEntity*		m_pkPlayer = NULL;
		
		//영역
		static NiPoint3*	m_pkMinPos = NULL;
		static NiPoint3*	m_pkMaxPos = NULL;
		static NiPoint3*	m_pkZoneSize = NULL;
		static NiPoint3*	m_pkZoneCount = NULL;

		//그리느냐 마느냐...
		static bool			m_bZoneDraw=false;
		static bool			m_bPlayerBase=false;

		//생성자
		MPgZoneControl();
    };
}}}}