#include "stdafx.h"
#include "PgNifMan.h"

void PgNifMan::Init()
{
}

void PgNifMan::Destroy()
{
	DeleteAllNif();
}

NiNodePtr PgNifMan::GetNif(const std::string NifFilePath)	//	Nif 파일을 로딩한 후, 클론 한 오브젝트를 리턴한다.
{
	NiNodePtr	spSourceNif = 0;
	NifContainer::iterator itr = m_NifContainer.find(NifFilePath);

	if(itr == m_NifContainer.end())
	{
		// 경로를 절대 경로로 바꾼다.

		std::string kNifPath;// = "D:/work/Dragonica_Exe/SFreedom_Dev/data/4_item/1_armor/1_male/04_u_armor/101_upa_5001_chn.nif";
		//if(!PgUtility::ToAbsolutePath(NifFilePath.c_str(), "D:/work/Dragonica_Exe/SFreedom_Dev/", kNifPath))
		if(!PgUtility::ToSuitableRelativePath(NifFilePath.c_str(), RELATIVE_PATH, kNifPath))
		{
		//	return 0;
		}

		NiStream kStream;
		if(kStream.Load(kNifPath.c_str()) == false)
		{
			//	로딩 실패 메세지
			return 0;
		}

		spSourceNif = (NiNode*)kStream.GetObjectAt(0);
		m_NifContainer.insert(std::make_pair(NifFilePath, spSourceNif));
		spSourceNif->SetDefaultCopyType(NiObjectNET::COPY_EXACT);
		spSourceNif->UpdateProperties();
		spSourceNif->UpdateEffects();
		spSourceNif->Update(0,true);
	}
	else
		spSourceNif = itr->second;

	NiNodePtr	spClone = (NiNode*)spSourceNif->Clone();
	spClone->UpdateProperties();
	spClone->UpdateEffects();
	spClone->Update(0,true);

	return spClone;
}

NiNodePtr PgNifMan::GetNif_DeepCopy(const std::string NifFilePath)	//	Nif 파일을 로딩한 후, 클론 한 오브젝트를 리턴한다.(이때에는 DeepCopy를 사용한다)
{
	NiNodePtr	spSourceNif = 0;
	NifContainer::iterator itr = m_NifContainer.find(NifFilePath);

	if(itr == m_NifContainer.end())
	{
		NiStream kStream;
		if(kStream.Load(NifFilePath.c_str()) == false)
		{
			//	로딩 실패 메세지
			return NULL;
		}
		spSourceNif = (NiNode*)kStream.GetObjectAt(0);
		m_NifContainer.insert(std::make_pair(NifFilePath, spSourceNif));
		spSourceNif->SetDefaultCopyType(NiObjectNET::COPY_EXACT);
		spSourceNif->UpdateProperties();
		spSourceNif->Update(0,true);
	}
	else
		spSourceNif = itr->second;

	NiNodePtr	spClone = NiDynamicCast(NiNode, spSourceNif->CreateDeepCopy());
	spClone->UpdateProperties();
	spClone->Update(0,true);

	return spClone;
}

bool PgNifMan::DeleteNif(const std::string NifFilePath)	//	Nif 파일을 컨테이너에서 제거한다.
{
	NifContainer::iterator itr = m_NifContainer.find(NifFilePath);
	if(itr == m_NifContainer.end()) return false;	//	존재하지 않음

	m_NifContainer.erase(itr);
	return true;

}

bool PgNifMan::IsExist(const std::string NifFilePath)	//	Nif 파일이 컨테이너에 존재하는가?
{
	NifContainer::iterator itr = m_NifContainer.find(NifFilePath);
	if(itr == m_NifContainer.end()) return false;	//	존재하지 않음

	return true;
}

bool PgNifMan::DeleteAllNif()	//	컨테이너 클리어
{
	m_NifContainer.clear();
	return true;
}
