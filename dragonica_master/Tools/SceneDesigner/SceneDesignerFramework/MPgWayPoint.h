#pragma once

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class WayPointGroup : public Object
	{
	public:
		String *m_strName;
		ArrayList *m_pmSettedWayPointList;

	public:
		WayPointGroup();
		~WayPointGroup();
	};
}}}}

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class WayPointData : public Object
	{
	public:
		String *m_strName;
		int m_iIndex;
		float m_fPosX;
		float m_fPosY;
		float m_fPosZ;
		float m_fRadius;
		MEntity *m_pkEntity;

	public:
		WayPointData(){}
		~WayPointData(){}
		void Initialize(MEntity *pkEntity, String *strName, int iIndex, float fRadius, 
			float fPosX, float fPosY, float fPosZ)
	    {
			m_pkEntity = pkEntity;
			m_strName = strName;
			m_iIndex = iIndex;
			m_fRadius = fRadius;
	        m_fPosX = fPosX;
	        m_fPosY = fPosY;
	        m_fPosZ = fPosZ;
			m_pkEntity = 0;
	    }
		void SetPos(NiPoint3 kPoint)
		{
			m_fPosX = kPoint.x;
			m_fPosY = kPoint.y;
			m_fPosZ = kPoint.z;
		}
	};
}}}}

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MPgWayPoint
	{
	public:
		MPgWayPoint(void);
	public:
		virtual ~MPgWayPoint(void);

	public:
		bool m_bIsLoaded;
		WayPointGroup *m_pkCurrentGroup;
		ArrayList *m_pmWayPointGroup;

        NiMaterialProperty* m_pkRectangleMaterial;
        NiColor* m_pkHighlightColor;
		int m_iEntityCount;
        NiScreenConsole* m_pkWayPointConsole;
		String *m_strCurrentGroupName;

	public:
		void Initialize();
		void Clear();
		void CreateWayPointConsole();
		void Render(MRenderingContext* pmRenderingContext);
		void RenderWayPointConsole(MRenderingContext* pmRenderingContext, WayPointData *pkData);
		void MakeFile(String *strFilePath);
		void LoadFile(String *strFilePath);
		void AddGroup(String *strGroupName);
		void DelGroup(String *strGroupName);
		void Array(String *strGroupName);
		void ChangeGroup(String *strGroupName);
		void ChangeGroupName(String *strOrgGroupName, String *strNewGroupName);
		void ChangeIndex(MEntity *pkEntity, int iIndex);
		void ChangeRadius(MEntity *pkEntity, float fRadius);
		void ApplyPosition();
		void ShowCircle();
		void DeleteCircle();
		int GetGroupCount();
		String* GetGroupName(int iIndex);
		void SetCurrentGroupName(String *strGroupName) { m_strCurrentGroupName = strGroupName; }

		WayPointData* AddWayPoint(
			String *strGroupName, String *strName, int iIndex, float fRadius, float fPosX, float fPosY, float fPosZ);
		void AddWayPoint(MEntity *pkEntity, String *strName, int iIndex, float fRadius, float fPosX, float fPosY, float fPosZ);
		void DeleteWayPoint(MEntity *pkEntity);	// delete current group  entity
		void ClearWayPointData_CurrentGroup();
		void ClearWayPointData_All();
		bool IsWayPoint(String *strName);
		bool IsWayPoint(MEntity *pkEntity);

		void SetEntityHide(bool bHide);
	};
}}}}
