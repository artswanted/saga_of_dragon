#pragma once

// 패널과 뷰상의 데이터는 서로 연동이 되지 않는다.
// 연동이 되게 만들어야 하는데, 이 클래스는 그 데이터를 잠시동안이나마
// 보관 할수 있는 클래스 이다.
namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class ModifiedNpcData : public Object
	{
	public:
		String *m_pkSrcName;
		String *m_pkDestName;
		bool m_bIsNamed;
		bool m_bIsTranslte;
		bool m_bIsRotate;
		float m_fPosX;
		float m_fPosY;
		float m_fPosZ;
		float m_fRotateX;
		float m_fRotateY;
		float m_fRotateZ;

	public:
		ModifiedNpcData()
		{
			m_pkSrcName = 0;
			m_pkDestName = 0;
			m_bIsNamed = false;
			m_bIsTranslte = false;
		}
		~ModifiedNpcData() {}

		void Initialize(String *pkSrcName, String *pkDestName)
		{
			m_pkSrcName = pkSrcName;
			m_pkDestName = pkDestName;
			m_bIsNamed = true;
		}
		void Initialize(String *pkSrcName, float fPosX, float fPosY, float fPosZ)
		{
			m_pkSrcName = pkSrcName;
			m_fPosX = fPosX;
			m_fPosY = fPosY;
			m_fPosZ = fPosZ;
			m_bIsTranslte = true;
		}
		void SetRotate(String *pkSrcName, float fRotateX, float fRotateY, float fRotateZ)
		{
			m_pkSrcName = pkSrcName;
			m_fRotateX = fRotateX;
			m_fRotateY = fRotateY;
			m_fRotateZ = fRotateZ;
			m_bIsRotate = true;
		}
	};
}}}}

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MPgNpc
	{
	public:
		MPgNpc(void);
	public:
		virtual ~MPgNpc(void);

	public:
		bool m_bDeletedNpcData;
		ArrayList *m_pmDeletedList;
		bool m_bModifiedData;
		ArrayList *m_pmModifiedList;
		MEntity *m_pkSelectedEntity;
		bool m_bSelectedData; // 마우스로 선택한 엔피시가 있는가

	public:
		bool Initialize();
		void Clear();
		bool IsNpcData(String *pkName);
		bool IsNpcData(MEntity *pkEntity);
		void InsertDeletedNpcData(String *pkName);
		int CountDeletedNpcData();
		String* PopDeletedNpcData();

		void InsertNpc(MPoint3* pkTranslation, MPoint3* pkDirection, MEntity* pkNpcEntity);
		void InsertModifiedNpcData(String *pkSrcName, String *pkDestName);
		void InsertModifiedNpcData(String *pkSrcName, float fPosX, float fPosY, float fPosZ);
		void InsertModifiedNpcData_Rotation(String *pkSrcName, const NiMatrix3& rkMatrix);
		bool GetModifiedNpcData_IsTranslate(String *pkSrcName);
		bool GetModifiedNpcData_IsRotate(String *pkSrcName);
		bool GetModifiedNpcData_IsNamed(String *pkSrcName);
		String* GetModifiedNpcData_DestName(String *pkSrcName);
		float GetModifiedNpcData_PosX(String *pkSrcName);
		float GetModifiedNpcData_PosY(String *pkSrcName);
		float GetModifiedNpcData_PosZ(String *pkSrcName);
		float GetModifiedNpcData_RotateX(String *pkSrcName);
		float GetModifiedNpcData_RotateY(String *pkSrcName);
		float GetModifiedNpcData_RotateZ(String *pkSrcName);
		void ClearModifiedNpcData();
		void SetSelectEntity(MEntity *pkEntity);

	public:
		__property bool get_IsDeletedNpcData();
		__property void set_IsDeletedNpcData(bool bDelete);
		__property bool get_IsModifiedNpcData();
		__property void set_IsModifiedNpcData(bool bModified);
		__property bool get_IsSelectedData();
		__property void set_IsSelectedData(bool bSelect);
	};
}}}}
