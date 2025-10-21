#pragma once

#include "MPoint3.h"

using namespace System::Collections;


namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
namespace Framework
{
	public __gc class SettedObjectData : public Object
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
		int m_iRotAxZ;
		MEntity *m_pkEntity;
		Guid m_kGuid;
		String* m_strGroupNum;
		int m_nChangeFlag;

	public:
		SettedObjectData(){}
		~SettedObjectData(){}
		void Initialize(Guid kGuid, String *strMemo, int iMapNo,
			int iMonParentBagNo, int iPointGroup, int iRegenPeriod, float fPosX, float fPosY, 
			float fPosZ, int iRotAxZ,int iMoveRange, String* strGroup, int nChangeFlag)
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
			m_iRotAxZ = iRotAxZ;
			m_iMoveRange = iMoveRange;
			m_strGroupNum = strGroup;
			m_pkEntity = 0;
			m_nChangeFlag = nChangeFlag;
		}
	};
}}}}

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
	namespace Framework
{

	__value enum DB_CHANGE_VALUE
	{
		DCV_NORMAL = 0,
		DCV_INSERT,
		DCV_UPDATE,
		DCV_DELETE
	};
	public __gc class ObjectElementData : public Object
	{
	public:
		int m_nElementNo;
		int m_nObjectNo;
		float m_fRelativeX;
		float m_fRelativeY;
		float m_fRelativeZ;
		String* m_strPath;
		int m_nChangeFlag;

	public:
		ObjectElementData(){}
		~ObjectElementData(){}
		void Initialize(int nElementNo, int nObjectNo, float fRelativeX, float fRelativeY, float fRelativeZ, String* pPath, int nChangeFlag)
		{
			m_nElementNo = nElementNo;
			m_nObjectNo = nObjectNo;
			m_fRelativeX = fRelativeX;
			m_fRelativeY = fRelativeY;
			m_fRelativeZ = fRelativeZ;
			m_strPath = pPath->ToString();
			m_nChangeFlag = nChangeFlag;
		}
	};

	public __gc class ObjectBagData : public Object
	{
	public:
		int m_nBagNo;
		int m_nObjElement1;
		int m_nObjElement2;
		int m_nObjElement3;
		int m_nObjElement4;
		int m_nObjElement5;
		int m_nObjElement6;
		int m_nObjElement7;
		int m_nObjElement8;
		int m_nObjElement9;
		int m_nObjElement10;
		MEntity* m_pkEntity;
		int m_nChangeFlag;

	public:
		ObjectBagData(){}
		~ObjectBagData(){}
		void Initialize(int nBagNo, int nObjElement1, int nObjElement2, int nObjElement3, int nObjElement4
			, int nObjElement5, int nObjElement6, int nObjElement7, int nObjElement8, int nObjElement9
			, int nObjElement10, int nChangeFlag)
		{
			m_nBagNo = nBagNo;
			m_nObjElement1 = nObjElement1;
			m_nObjElement2 = nObjElement2;
			m_nObjElement3 = nObjElement3;
			m_nObjElement4 = nObjElement4;
			m_nObjElement5 = nObjElement5;
			m_nObjElement6 = nObjElement6;
			m_nObjElement7 = nObjElement7;
			m_nObjElement8 = nObjElement8;
			m_nObjElement9 = nObjElement9;
			m_nObjElement10 = nObjElement10;
			m_nChangeFlag = nChangeFlag;
		}
	};

	public __gc class MakingData : public Object
	{
	public:
		MakingData()
		{
			m_pmElementList = new ArrayList;
		}
		~MakingData(){}
	public:
		int m_nBagNo;
		int m_nRegenPeriod;
		int m_nPointGroup;
		MEntity* m_pkEntity;
		ArrayList* m_pmElementList;
	};

	public __gc class ObjectData : public Object
	{
	public:
		ObjectData();
		~ObjectData();
	public:
		int m_nIndex;
		int m_nObjectNumber;
		float m_fPosX;
		float m_fPosY;
		float m_fPosZ;
		String* m_strPath;
	};

	public __gc class GroupData : public Object
	{
	public:
		GroupData();
		~GroupData();
	public:
		String* m_strGroupName;
		MEntity* m_pkEntity;
		ArrayList* m_pmObjectData;
	};

	public __gc class MPgBreakObject : public Object
	{
	public:
		MPgBreakObject(void);
		~MPgBreakObject(void);

	public:
		ArrayList* m_pmGroupList;
		//GroupData* m_pMakingData;// 만드는 중인 데이타
		MakingData* m_pMakingData;

		String* m_strXmlPath;
		String* m_strFileName;

		bool m_bAddGroup;
		bool m_bDeleteGroup;

		int m_nMapNum;

		// DB
		ArrayList* m_pmSettedBreakObjectList;
		//ArrayList* m_pmDeletedBreakObjectList;

		ArrayList* m_pmSettedElementList;// 로드후 새로 추가된것도 넣는다.

		ArrayList* m_pmAllBagList;

	public:
		void Initalize();
		void Clear();

		bool AddObjectData(int nObjectNumber, float fPosX, float fPosY, float fPosZ, String* strPath);
		bool ModifyObjectData(int nIndex, int nObjectNumber, float fPosX, float fPosY, float fPosZ);
		bool RemoveObjectData(int nIndex);
		void ClearObjectData();
		int GetObjectDataObjectNumber(int nIndex);
		float GetObjectDataHeight(int nIndex);

		bool AddGroupData();
		bool AddGroupDataEntity(MEntity* pOrgEntity);
		MEntity* AddGroupDataEntityFromLoad(NiPoint3 pkPos, String* _strName, ArrayList* pArray);
		void RemoveGroupData(int nIndex);// 판넬
		void RemoveGroupData(MEntity* pkEntity);// 화면
		void GroupListSelect(int nIndex);
		__property bool get_AddGroup();
		__property void set_AddGroup(bool bAdd);
		__property bool get_DeleteGroup();
		__property void set_DeleteGroup(bool bDelete);
		void TranslationGroupData(MEntity* pkEntity, NiPoint3 kDelta);
		bool ModifyGroupData(String* strGroupName);
		String* GetGroupDataGroupName(int nIndex);

		bool SaveXml();
		void LoadXml();

		// DB
		bool AddElementData(int nElementNo, int nObjectNo, float fPosX, float fPosY, float fPosZ, String* strPath);
		bool RemoveElementData(int nIndex);
		void AddSettedElementList(int nElementNo, int nObjectNo, float fRelativeX, float fRelativeY, float fRelativeZ, String* strPath);
		bool AddNewElemntList(int nElementNo, int nObjectNo, float fRelativeX, float fRelativeY, float fRelativeZ, String* strPath);

		ObjectBagData* AddAllBagList(int nBagNo, int nObjElement1, int nObjElement2, int nObjElement3, int nObjElement4
			, int nObjElement5, int nObjElement6, int nObjElement7, int nObjElement8, int nObjElement9, int nObjElement10);
		ObjectBagData* AddNewBagList(int nBagNo, int nObjElement1, int nObjElement2, int nObjElement3, int nObjElement4
			, int nObjElement5, int nObjElement6, int nObjElement7, int nObjElement8, int nObjElement9, int nObjElement10);
		MEntity* AddBagListEntity(MEntity* pOrgEntity);
		void AddBagListEntityFromLoad();
		bool AddBagListFromMakingData();
		bool AddElementListFromMakingData();
		ObjectElementData* GetObjectElementData(int nElementNo);
		ObjectBagData* GetObjectBagData(int nBagNo);
		void ObjectBagFlagSettingAfterDBUpdate();
		void ObjectElementFlagSettingAfterDBUpdate();
		void SettedObjectFlagSettingAfterDBUpdate();

		MEntity* AddSettedBreakObjectData(String* strBagName, Guid kGuid, String *strMemo, int iMapNo, int iMonParentBagNo, int iPointGroup, 
			int iRegenPeriod, float fPosX, float fPosY, float fPosZ, int iRotAxZ, int iMoveRange, String* strGroupNum);
		int DeleteSettedBreakObjectData(Guid kGuid);
		void ModifySettedBreakObjectPos(MEntity* pkEntity, NiPoint3 kPosition);
		//Guid GetDeletedBreakObjectGuidData(int iDeletedMonsterIdx);
		//void DeleteToDeletedBreakObjectGuidData(Guid kGuid);
		MPoint3* GetBreakObjectPos(MEntity* pkEntity);
		MPoint3* GetBreakObjectPos(int nBagNo);
		int	GetBreakObjectRot(MEntity* pkEntity);

		void ClearSettedObjectData();
		bool IsBreakObjctData(String *strName);
		int GetSettedMonsterBagNo(int nIndex);

		void RemoveObjectBagData(int nBagNo);// 화면
		void RemoveSettedBreakObject(int nIndex);// 판넬

		bool ModifyElement(int nElement, int nObject, float fX, float fY,float fZ);
		bool RemoveElement(int nIndex);// 판넬

		void RefreshScreenBreakObject();
		SettedObjectData* GetSettedBreakObjectFromBagNo(int nBagNo);

		bool RemoveObjectBag(int nIndex);// 판넬
		void ObjectBagSelect(int nIndex);// 판넬

		void InsertSettedBreakObject(Guid kGuid, String *strMemo, int iMapNo,
			int iMonParentBagNo, int iPointGroup, int iRegenPeriod, float fPosX, float fPosY, 
			float fPosZ, int iRotAxZ, int iMoveRange, String* strGroup, MEntity* pkEntity);
	};
}}}}