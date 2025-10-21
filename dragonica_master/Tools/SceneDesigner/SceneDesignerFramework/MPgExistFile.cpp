//
// HandOver, 강정욱 2008.01.29
//
// 맵툴에서 Palettes혹은 맵상에 Object가 추가 되어있고.. 실제 경로를 보았을때
// 그 파일이 없을경우.. 그 파일을 찾아준다.
// 그리고 없는 파일일 경우 제거 할 수 있다.
// Palettes의 경우는 제거 하지 않아도 되는데
// 맵상에 Object일경우는 제거 해주거나. 올바른 경로로 맞추어 주어야 맵 로딩이 된다.
//
// UI : NotExistFilesPanel.cs
//
#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "MPgExistFile.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

MPgExistFile::MPgExistFile(void)
{
}

MPgExistFile::~MPgExistFile(void)
{
}

bool MPgExistFile::Initialize()
{
	m_pmNotExistFiles = new ArrayList;
	m_pmNotExistFiles2 = new ArrayList;
	m_bToRefreshList = false;

	return true;
}
void MPgExistFile::AddNotExistFile(String *pkFilePath, MEntity* pkEntity)
{
	ExistFiles* pkData = new ExistFiles;
	pkData->m_pkFilePath = pkFilePath;
	pkData->m_pkEntity = pkEntity;
	
	m_pmNotExistFiles->Add(pkData);
}
void MPgExistFile::AddNotExistFile2(String *pkFilePath, MEntity* pkEntity)
{
	ExistFiles* pkData = new ExistFiles;
	pkData->m_pkFilePath = pkFilePath;
	pkData->m_pkEntity = pkEntity;
	
	m_pmNotExistFiles2->Add(pkData);
}
unsigned int MPgExistFile::GetNotExistFileCount()
{
	return m_pmNotExistFiles->Count;
}
unsigned int MPgExistFile::GetNotExistFile2Count()
{
	return m_pmNotExistFiles2->Count;
}

ExistFiles* MPgExistFile::GetNotExistFile(unsigned int uiIdx, bool bPop)
{
	if (uiIdx >= (unsigned int)m_pmNotExistFiles->Count)
	{
		return 0;
	}

	ExistFiles* pkData = dynamic_cast<ExistFiles *>(m_pmNotExistFiles->get_Item(uiIdx));
	if (pkData)
	{
		if (bPop)
		{
			m_pmNotExistFiles->RemoveAt(uiIdx);
		}
		return pkData;
	}
	
	return 0;
}

ExistFiles* MPgExistFile::GetNotExistFile2(unsigned int uiIdx, bool bPop)
{
	if (uiIdx >= (unsigned int)m_pmNotExistFiles2->Count)
	{
		return 0;
	}

	ExistFiles* pkData = dynamic_cast<ExistFiles *>(m_pmNotExistFiles2->get_Item(uiIdx));
	if (pkData)
	{
		if (bPop)
		{
			m_pmNotExistFiles2->RemoveAt(uiIdx);
		}
		return pkData;
	}
	
	return 0;
}

void MPgExistFile::ClearNotExistFile()
{
	m_pmNotExistFiles->Clear();
	m_pmNotExistFiles2->Clear();
}

bool MPgExistFile::FindNotExistFile()
{
	bool bFind = false;

	// 화면에 찍힌 물체들 찾기
	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
	for (int i=0 ; i<pkEntities->Count ; i++)
	{
		MEntity *pkEntity = pkEntities[i];
		if (!pkEntity)
		{
			continue;
		}
		NiEntityInterface* pkEntityInterface = pkEntity->GetNiEntityInterface();
		if (!pkEntityInterface)
		{
			continue;
		}

		// 없는 파일 체크.
		NiFixedString kPath;
		if (pkEntityInterface->GetPropertyData("NIF File Path", kPath, 0))
		{
			if (_access(kPath, 0) != -1)
			{
				// Succese
			}
			else
			{
				// Failed
				MFramework::Instance->ExistFile->AddNotExistFile(kPath, pkEntity);
				MFramework::Instance->ExistFile->m_bToRefreshList = true;
				bFind = true;
			}
		}
	}

	// 팔레트용 찾기.
	for (unsigned int i=0 ; i<MFramework::Instance->PaletteManager->PaletteCount ; i++)
	{
		MPalette* palette = MFramework::Instance->PaletteManager->GetPalettes()[i];

		for (unsigned int j=0 ; j<palette->EntityCount ; j++)
		{
			MEntity *pkEntity = palette->GetEntities()[j];
			if (!pkEntity)
			{
				continue;
			}
			NiEntityInterface* pkEntityInterface = pkEntity->GetNiEntityInterface();
			if (!pkEntityInterface)
			{
				continue;
			}
			// 없는 파일 체크.
			NiFixedString kPath;
			if (pkEntityInterface->GetPropertyData("NIF File Path", kPath, 0))
			{
				if (_access(kPath, 0) != -1)
				{
					// Succese
				}
				else
				{
					// Failed
					MFramework::Instance->ExistFile->AddNotExistFile2(kPath, pkEntity);
					MFramework::Instance->ExistFile->m_bToRefreshList = true;
					bFind = true;
				}
			}
		}
	}

	return bFind;
}

void MPgExistFile::RefreshPostfixTexture()
{
	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
	for (int i=0 ; i<pkEntities->Count ; i++)
	{
		MEntity* pkEntity = pkEntities[i];
		if (!pkEntity)
		{
			continue;
		}
		NiEntityInterface* pkEntityInterface = pkEntity->GetNiEntityInterface();
		if (!pkEntityInterface)
		{
			continue;
		}
		String* pkPostfixTexture = pkEntityInterface->GetPGPostfixTexture();
		pkEntity->SetPGPostfixTexture(pkPostfixTexture, false);
	}
}
