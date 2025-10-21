#pragma once

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class PathTarget : public MEntity
	{
	public:
		MEntity* m_pkLinkEntity;
	public:
        PathTarget(NiEntityInterface* pkEntity) : MEntity(pkEntity)
		{
			m_pkLinkEntity = 0;
		}
		void SetLinkEntity(MEntity* pkLinkEntity)
		{
			m_pkLinkEntity = pkLinkEntity;
		}
	};
}}}}

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MPgPath
	{
	public:
		// Target Entity
		float m_fHeight;
        ArrayList* m_pkTargetList;
		ArrayList* m_pkListForUndo;
        ArrayList* m_pkTargetListUp;
		
		// Draw Console
		NiLines *m_pkSideLineMarker;
		NiPoint3 *m_pkSideLineVerts;
		NiMaterialProperty* m_pkMaterial;
        NiScreenConsole* m_pkPathDataConsole;
		float m_fPathColorR, m_fPathColorG, m_fPathColorB;

		// Make Wall
		NiTransform *m_pkWorldTransform;
		bool m_bIsMakeLeft;
		bool m_bIsTypePath;
		bool m_bIsTypePhysX;
		int m_iMeshGroup;
		
	private:
        __property static ISelectionService* get_SelectionService();
        static ISelectionService* ms_pmSelectionService = NULL;

		__property static ICommandService* get_CommandService();
		static ICommandService* ms_pmCommandService = NULL;

		bool m_bMakeUp;
		bool m_bMakeSide;

		//! List for Undo Index
		unsigned int m_uiStartIndexForUndo, m_uiCountForUndo;

	public:
		MPgPath(void);
		virtual ~MPgPath(void);

	public:
		void Initialize();
		void InsertPath(MEntity *pkEntity);
		void CancelMakeUpSide();
		void Clear();
		void CreatePathDataConsole();
		void DeletePath(MEntity *pkEntity);
		void ExtractDot();
		void LoadEntityToPalette(String *pkFilename, String *pkSelectPaletteName);
		void MakeSide();
		void MakeUp();
		void MakeStraight();
		void CancelMakeStraight();
		void MakeWall(String *pkFilePath, String *pkSelectPaletteName);
		void ModifyPath();
		void Render(MRenderingContext* pmRenderingContext);
		void RenderPathDataConsole(MRenderingContext* pmRenderingContext);
		void SetAnchorScale(float fScale);

		bool IsMakeUp() { return (m_bMakeUp||m_bMakeSide); };
	};
}}}}
