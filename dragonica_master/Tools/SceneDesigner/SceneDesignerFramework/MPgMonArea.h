//#pragma once
//
//#include "PgMonAreaComponent.h"
//
//namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
//    namespace Framework
//{
//	public __gc class MonAreaEntity : public Object
//	{
//	public:
//		MEntity* m_pkEntity;
//		int m_iAreaID;
//		ArrayList* m_aiLinkArea;
//	public:
//        MonAreaEntity()
//		{
//			m_pkEntity = 0;
//			m_aiLinkArea = new ArrayList;
//		}
//        ~MonAreaEntity(){}
//	};
//}}}}
//
//namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
//    namespace Framework
//{
//	public __gc class MPgMonArea
//	{
//	public:
//		MPgMonArea(void);
//		virtual ~MPgMonArea(void);
//		bool Initialize();
//		void Clear();
//
//		bool AddMonAreaTarget(MEntity *pkEntity);
//		void DeleteMonAreaTarget(MEntity *pkEntity, bool bRemoveInScene);
//		void MakeMonArea();
//		void MakeMonAreaCircle();
//		void DeleteMonArea(MEntity *pkMonArea, bool bRemoveInScene);
//		void RevertPoint();
//		void ExtractPoint();
//		void ErasePoint();
//
//		void SetMonAreaID(String* pkMonAreaID);
//		String* GetMonAreaID();
//		void SaveToXML(String* pkFilename);
//		NiPoint3 XMLPtToNiPoint(String* strPoint);
//		void LoadFromXML(String* pkFilename);
//		int GetSelectedAreaID(MEntity* pkEntity);
//		ArrayList* GetLinkAreaList(MEntity* pkEntity);
//		void SetAreaID(int iAreaID);
//		bool ModifyLinkAreaList(MEntity* pkEntity, int iOrg, int iAfter);
//		bool RemoveLinkAreaList(MEntity* pkEntity, int iOrg);
//
//		// Console Render
//		void CreateConsole();
//		void Render(MRenderingContext* pmRenderingContext);
//		void RenderConsole(MRenderingContext* pmRenderingContext, MonAreaEntity* pkData);
//
//	protected:
//		MEntity* CreateMonArea(NiEntityInterface* pkEntity1, NiEntityInterface* pkEntity2, bool bIsRectangle);
//		MEntity* CreateMonArea(NiPoint3 kPoint1, NiPoint3 kPoint2, bool bIsRectangle);
//		PgMonAreaComponent* GetMonAreaComponent(MEntity* pkEntity);
//		PgMonAreaComponent* GetMonAreaComponent(NiEntityInterface* pkInterface);
//
//	private:
//        __property static ISelectionService* get_SelectionService();
//        static ISelectionService* ms_pmSelectionService;
//
//	public:
//        ArrayList* m_pkTargetList;
//        ArrayList* m_pkAreaList;
//		String* m_pkMonAreaID;
//		int m_iCurrentAreaID;
//
//	private:
//		// Console Render
//        NiScreenConsole* m_pkConsole;
//        NiMaterialProperty* m_pkRectangleMaterial;
//        NiColor* m_pkHighlightColor;
//
//	};
//}}}}
