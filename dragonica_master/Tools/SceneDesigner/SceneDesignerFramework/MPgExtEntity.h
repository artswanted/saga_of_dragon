#pragma once

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MPgExtEntity
	{
	public:
		__value typedef enum eExtEntityType
		{
			ExtEntityType_Normal,
			ExtEntityType_Monster,
			ExtEntityType_Npc,
			ExtEntityType_WayPoint,
			ExtEntityType_WayPoint_Circle,
			ExtEntityType_Path,
			//ExtEntityType_MonAreaTarget,
			//ExtEntityType_MonArea,
			ExtEntityType_ShineStone,
			ExtEntityType_DecalTarget,
			ExtEntityType_DecalData,
			ExtEntityType_BreakObject,
			ExtEntityType_ElevatorTarget,
		}EExtEntityType;

	public:
		MPgExtEntity(void);
		virtual ~MPgExtEntity(void);

	public:
		void Clear();
		eExtEntityType IsExtEntityType(MEntity* pkEntity);
		bool DeleteExtEntity(MEntity* pkOrgEntity);
		bool AddExtEntity(MEntity* pkEntity);
		bool TranslateExtEntity(MEntity* pkEntity, NiPoint3 kDeltaPosition);
		bool RotateExtEntity(MEntity* pkEntity, const NiMatrix3 &rkMatrix);
		bool SelectExtEntity(MEntity* pkEntity);

	public:
		eExtEntityType GetExtEntityType_Normal() { return ExtEntityType_Normal; }
		eExtEntityType GetExtEntityType_Monster() { return ExtEntityType_Monster; }
		eExtEntityType GetExtEntityType_Npc() { return ExtEntityType_Npc; }
		eExtEntityType GetExtEntityType_WayPoint() { return ExtEntityType_WayPoint; }
		eExtEntityType GetExtEntityType_WayPoint_Circle() { return ExtEntityType_WayPoint_Circle; }
		eExtEntityType GetExtEntityType_Path() { return ExtEntityType_Path; }
		eExtEntityType GetExtEntityType_DecalTarget() { return ExtEntityType_DecalTarget; }
		eExtEntityType GetExtEntityType_DecalData() { return ExtEntityType_DecalData; }
		eExtEntityType GetExtEntityType_BreakObject() { return ExtEntityType_BreakObject; }
		eExtEntityType GetExtEntityType_ElevatorTarget() { return ExtEntityType_ElevatorTarget; }
		//eExtEntityType GetExtEntityType_MonAreaTarget() { return ExtEntityType_MonAreaTarget; }
		//eExtEntityType GetExtEntityType_MonArea() { return ExtEntityType_MonArea; }

	public:
		bool m_bHaveWaitAddEntity;
		bool m_bHaveWaitDeleteEntity;
		bool m_bHaveWaitTranslateEntity;
		bool m_bHaveWaitSelectEntity;
		ArrayList *m_pmWaitAddEntities;
		ArrayList *m_pmWaitDeleteEntities;
		ArrayList *m_pmWaitTranslateEntities;
		ArrayList *m_pmWaitSelectEntities;

		EExtEntityType m_eWaitType;
	};
}}}}
