#pragma once

// 몬스터 백은 관리 하지 않기 때문에 ifdef로 막아 두었음.
//#define PG_USE_MONSTERBAG

#ifdef PG_USE_MONSTERBAG
namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MonsterBagData : public Object
	{
	public:
		int m_iMonsterBagNo;
		String *m_pkMonsterBagMemo;
		int m_aiMonsterNo __gc[];
		int m_aiMonsterRate __gc[];

	public:
		MonsterBagData(){}
		~MonsterBagData(){}

		void Initialize(int iMonsterBagNo, String *pkMonsterBagMemo, int aiMonsterNo __gc[], int aiMonsterRate __gc[])
		{
			m_iMonsterBagNo = iMonsterBagNo;
			m_pkMonsterBagMemo = pkMonsterBagMemo;
			m_aiMonsterNo = new int __gc[10];
			m_aiMonsterRate = new int __gc[10];

			for (int i=0 ; i<aiMonsterNo->Count ; i++)
			{
				m_aiMonsterNo[i] = aiMonsterNo[i];
			}

			for (int i=0 ; i<aiMonsterRate->Count ; i++)
			{
				m_aiMonsterRate[i] = aiMonsterRate[i];
			}
		}
	};
}}}}
#endif

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class SettedMonsterData : public Object
	{
	public:
		String *m_strMemo;
		int m_iMapNo;
		int m_iMonParentBagNo;
		int m_iPointGroup;
		int m_iRegenPeriod;
		int m_iMoveRange;
		float m_fPosX;
		float m_fPosY;
		float m_fPosZ;
		float m_fRotAxZ;
		MEntity *m_pkEntity;
		Guid m_kGuid;

	public:
		SettedMonsterData(){}
		~SettedMonsterData(){}
		void Initialize(Guid kGuid, String *strMemo, int iMapNo,
	        int iMonParentBagNo, int iPointGroup, int iRegenPeriod, float fPosX, float fPosY, float fPosZ, float fRotAxZ, int iMoveRange)
	    {
			m_kGuid = kGuid;
	        m_strMemo = strMemo;
	        m_iMapNo = iMapNo;
	        m_iMonParentBagNo = iMonParentBagNo;
			m_iPointGroup = iPointGroup;
	        m_iRegenPeriod = iRegenPeriod;
	        m_fPosX = fPosX;
	        m_fPosY = fPosY;
	        m_fPosZ = fPosZ;
			m_fRotAxZ = fRotAxZ;
			m_iMoveRange = iMoveRange;
			m_pkEntity = 0;
	    }
	};
}}}}

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MonsterTemplate : public Object
	{
	public:
		MonsterTemplate(){};
		~MonsterTemplate(){};

		__property void set_MonsterBagNo(int iMonsterBag)
		{
			m_iMonsterBagNo = iMonsterBag;
		}
		__property int get_MonsterBagNo()
		{
			return m_iMonsterBagNo;
		}
		__property void set_MonsterTemplateName(String* pkTemName)
		{
			m_pkMonsterTemplateName = pkTemName;
		}
		__property String* get_MonsterTemplateName()
		{
			return m_pkMonsterTemplateName;
		}

	private:
		int m_iMonsterBagNo;
		String* m_pkMonsterTemplateName;
	};
}}}}

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MonsterBagControl : public Object
	{
	public:
		__value enum BAGTYPE
		{
			NONE		= 0,
			PARENT		= 1, 
			DEFAULT		= 2, 
			EASY		= 3, 
			NORMAL		= 4, 
			HARD		= 5, 
			ULTRA		= 6, 
			LEVEL05		= 7, 
			LEVEL06		= 8,
		};
		int m_iParentBagNo;
		int m_iDefaultBagNo;
		int m_iEasyBagNo;
		int m_iNormalBagNo;
		int m_iHardBagNo;
		int m_iUltraBagNo;
		int m_iLevel05Bag;
		int m_iLevel06Bag;
        bool m_IsEditing;

	public:
		MonsterBagControl()
			: m_iParentBagNo(0)
			, m_iDefaultBagNo(0)
			, m_iEasyBagNo(0)
			, m_iNormalBagNo(0)
			, m_iHardBagNo(0)
			, m_iUltraBagNo(0)
			, m_iLevel05Bag(0)
			, m_iLevel06Bag(0)
        	, m_IsEditing(false)
		{}
		~MonsterBagControl(){}
		void Initilaize(int iParentBagNo, int iDefaultBagNo, int iEasyBagNo, int iNormalBagNo, int iHardBagNo, int iUltraBagNo,
			int iLevel05Bag, int iLevel06Bag)
		{
			m_iParentBagNo = iParentBagNo;
			m_iDefaultBagNo = iDefaultBagNo;
			m_iEasyBagNo = iEasyBagNo;
			m_iNormalBagNo = iNormalBagNo;
			m_iHardBagNo = iHardBagNo;
			m_iUltraBagNo = iUltraBagNo;
			m_iLevel05Bag = iLevel05Bag;
			m_iLevel06Bag = iLevel06Bag;
		}
	};
}}}}

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	const int iNotRotated = 255;
	public __gc class MPgMonster
	{
	//몬스터 템플릿이름을 저장하기 위해...현재 사용하지 않음
	private:
		ArrayList* m_pmMonsterTemplate;
		Hashtable* m_pmMonsterBagControl;

	public:
		void AddMonsterTemplate(int iMonsterBag, String* pkTemplateName);
		String* GetMonsterTemplate(int iMonsterBag);

	public:
		MPgMonster(void);
	public:
		virtual ~MPgMonster(void);

	public:
		ArrayList *m_pmMonsterBagList;
		//ArrayList *m_pmMonsterTemplate;

		ArrayList *m_pmSettedMonsterList;
		SettedMonsterData *m_pkWaitData;

        NiMaterialProperty* m_pkRectangleMaterial;
        NiColor* m_pkHighlightColor;
		int m_iEntityCount;
        NiScreenConsole* m_pkMonsterDataConsole;

		bool m_bAddedMonsterData;
		SettedMonsterData *m_pkAddedMonsterData;
		bool m_bDeletedMonsterData;
		ArrayList *m_pmDeletedMonsterList;

		bool m_bIsActiveText;

		MonsterBagControl::BAGTYPE m_kGroupNum;
		String* m_strXmlPath;
		String* m_strKfmPath;

	public:
		void Initialize();
		void Clear();
		void CreateMonsterDataConsole();
		void Render(MRenderingContext* pmRenderingContext);
		void RenderMonsterDataConsole(MRenderingContext* pmRenderingContext, SettedMonsterData *pkData);
		bool IsHidden(const int iParentBagNo);

#ifdef PG_USE_MONSTERBAG
		// Monster Bag Data
		// Not available
		void AddMonsterBagData(int iMonsterBagNo, String *strMonsterBagMemo, int aiMonsterNo __gc[], int aiMonsterRate __gc[]);
		void DeleteMonsterBagData(int iMonsterBagNo);
		void ClearMonsterBagData();
		void ModifyMonsterBagData(int iArrayIdx, int iMonsterBagNo, String *strMonsterBagMemo, int aiMonsterNo __gc[], int aiMonsterRate __gc[]);
		__property MonsterBagData* get_GetMonsterBagData(int iMonsterBagNo);
#endif

		// Setted Monster Data
		MEntity* AddSettedMonsterData(Guid kGuid, String *strMemo, int iMapNo, int m_iMonParentBagNo, int iPointGroup, 
			int iRegenPeriod, float fPosX, float fPosY, float fPosZ, int iRotAxZ, int iMoveRange, String* strTemplateName);
		void DeleteSettedMonsterData(Guid kGuid);
		void ClearSettedMonsterData();
		void StackSettedMonsterData(String *strMemo, int iMapNo, int m_iMonParentBagNo, int iPointGroup, int iRegenPeriod, int iMoveRange);
		void StackSettedMonsterData_RegenPeriod(int iRegenPeriod);
		void CompleateStackedMonsterData(Guid kGuid, float fPosX, float fPosY, float fPosZ, MEntity* pkEntity);
		bool IsMonsterData(String *strName);
		Guid GetDeletedMonsterGuidData(int iDeletedMonsterIdx);
		void DeleteToDeletedMonsterGuidData(Guid kGuid);
		__property bool get_AddedMonsterData();
		__property void set_AddedMonsterData(bool bAdded);
		__property bool get_DeletedMonsterData();
		__property void set_DeletedMonsterData(bool bDeleted);
		void ModifyPointGroup(MEntity* pkEntity, int iPointGroup);
		void ModifyRegenPeriod(MEntity* pkEntity, int iPeriod);
		void ModifyParentBagNo(MEntity* pkEntity, int iBagNo);
		void ModifyMemo(MEntity* pkEntity, String *strMemo);		
		bool GetMonsterData(MEntity* pkEntity);
		int GetMonsterData_PointGroup(MEntity* pkEntity);
		int GetMonsterData_RegenPeriod(MEntity* pkEntity);
		int GetMonsterData_ParentBagNo(MEntity* pkEntity);
		String* GetMonsterData_Memo(MEntity* pkEntity);
		MPoint3* GetMonsterPos(MEntity* pkEntity);
		int GetMonsterRotAxZ(MEntity* pkEntity);
		void ChangeHideText();
		void SetSettedMonsterListGroupVisible();
		void SetMonsterBagControl(Hashtable * pkMonsterBagControl);
	};
}}}}
