#pragma once

#using <System.Xml.dll>
using namespace System::Xml;
using namespace System::Collections;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{ namespace Framework
{
	public __gc class SDeleteList : public Object
	{
	public:
		SDeleteList(){}
		~SDeleteList(){}
		void Set(int iElevatorIndex, int iPointIndex)
		{
			m_iEIndex = iElevatorIndex;
			m_iPIndex = iPointIndex;
		}
		int GetElevatorIndex() { return m_iEIndex; }
		int GetPointIndex() { return m_iPIndex; }
	private:
		int m_iEIndex;
		int m_iPIndex;
	};

	public __gc class SPointList : public Object
	{
	public:
		SPointList(){}
		~SPointList(){}
		void Set(float fTime, float fX, float fY, float fZ, MEntity* pEntity)
		{
			m_fTime = fTime;
			m_fPosX = fX;
			m_fPosY = fY;
			m_fPosZ = fZ;
			m_pmEntity = pEntity;
		}
	public:
		float m_fPosX;
		float m_fPosY;
		float m_fPosZ;
		float m_fTime;
		MEntity* m_pmEntity;
	};

	public __gc class SElevatorList : public Object
	{
	public:
		SElevatorList()
		{
			m_pmPointList = new ArrayList;
		}
		~SElevatorList()
		{
			m_pmPointList->Clear();
		}
	public:
		Guid m_kGuid;
		String* m_strClassNo;
		String* m_strRidable;
		float m_fSpeed;
		ArrayList* m_pmPointList;
	};

	public __gc class STranslateData : public Object
	{
	public:
		STranslateData(){}
		~STranslateData(){}

		void Set(int iElevatorIndex, int iPointIndex)
		{
			m_iTranslateElevatorIndex = iElevatorIndex;
			m_iTranslatePointIndex = iPointIndex;
		}

	public:
		int m_iTranslateElevatorIndex;
		int m_iTranslatePointIndex;
	};

	public __gc class MPgElevator : public Object
	{
	public:
		MPgElevator();
		~MPgElevator();

	public:
		ArrayList* m_pmElevatorList;
		ArrayList* m_pmDeleteList;
		String* m_strFileName;
		XmlDocument* m_pkDocument;

		// ÀÓ½Ã ÀúÀå º¯¼ö
		int m_iSelectElevatorList;
		float m_fPointTime;

		bool m_bSync;

		bool m_bAdd;
		int m_iAddElevatorIndex;
		int m_iAddPointIndex;

		bool m_bDelete;
		int m_iDeleteElevatorIndex;
		int m_iDeletePointIndex;

		bool m_bTranslate;
		ArrayList *m_pmTranslateList;

	public:
		MEntity* AddElevatorEntity(MEntity* pkEntity);
		void RemoveElevatorEntity(String* strTemplateID);
		void RemoveElevatorEntity(MEntity* pkEntity);
		void RemoveElevatorPoint(MEntity* pkEntity);

		void SetXmlDocument(XmlDocument* pkTargetDoc) { m_pkDocument = pkTargetDoc; };

		// renew
		void Clear();
		Guid CreateElevator(String* strClassNo, String* strRidable, float fSpeed, Guid kGuid);
		void DeleteElevator(int iIndex);
		void ModifyElevator(int iIndex, String* strClassNo, String* strRidable, float fSpeed);
		void CreatePoint(int iElevatorIndex, int iPointNum, float fTime, float fPosX, float fPosY, float fPosZ, MEntity* pEntity);
		void DeletePoint(int iElevatorIndex, int iPointIndex);
		void ModifyPoint(int iElevatorIndex, int iPointNum, float fTime);
		MEntity* AddpointEntity(int iElevatorIndex, int iPointNum, float fX, float fY, float fZ);// ÆÇ³Ú
		void DeletePointEntity(MEntity* pkEntity);// ºä
		void RearrangeElevatorList();
		MEntity* AddPointEntity(MEntity* pkEntity);
		MEntity* TranslatePointEntity(MEntity* pkEntity, NiPoint3 kPos);
		void SetSyncData(bool bSync);
		bool GetSyncData();
	};
}}}}