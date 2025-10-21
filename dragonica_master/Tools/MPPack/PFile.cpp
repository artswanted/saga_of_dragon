#include "StdAfx.h"
#include "DataPack/Common.h"
#include "PFile.h"
#include <shlwapi.h>

CPFile::CPFile(void)
{

}

CPFile::~CPFile(void)
{
	Clear();
}

//	이건 읽어오는거고
bool CPFile::Load(const std::wstring& wstrFileName)
{
	if(!wstrFileName.size()){ return false; }
	if( !::PathFileExistsW(wstrFileName.c_str()) )
	{
		return	false;
	}

	BM::PgDataPackManager::PackFileCache_new kCache;
	if(kCache.Open(wstrFileName))
	{
		m_kHashSize	= BM::PgDataPackManager::ReadHeader(kCache);
		m_kHash.insert(kCache.mFolderHash.begin(), kCache.mFolderHash.end());
	}
	return	true;
}

bool CPFile::LoadFile(const std::wstring& wstrFileName)
{
	if(!wstrFileName.size())
	{ 
		return false; 
	}
	m_ffile.open(wstrFileName.c_str(), std::ios_base::in | std::ios_base::app | std::ios_base::ate | std::ios_base::binary);
	return true;
}

//	이건 만드는 거고
bool CPFile::Init(const std::wstring& wstrFileName, const BM::FolderHash& Hash)
{
	if(!wstrFileName.size())
	{ 
		return false; 
	}

	if(!Hash.size())
	{ 
		return false; 
	}
	
	m_kHash.insert(Hash.begin(), Hash.end());
	m_ffile.open(wstrFileName.c_str(), std::ios_base::in | std::ios_base::app | std::ios_base::ate | std::ios_base::binary);
	m_kHashSize	= BM::PgDataPackManager::MakeHeader(m_kHash, m_ffile);

	return	true;
}

//	이건 자원 해제하는 거고
void CPFile::Clear()
{
	if(m_ffile.is_open())
	{
		m_ffile.close();
	}
}

//	이건 폴더 찾는거고
bool CPFile::FindFolder(const std::wstring& wstrFolderName)
{
	if(m_kHash.size())
	{
		m_Folder_iter = m_kHash.find(wstrFolderName);
		if(m_Folder_iter == m_kHash.end())
		{ 
			return false; 
		}
	}
	else
	{
		return	false;
	}

	return	true;
}

//	이건 파일 찾는거고
bool CPFile::FindFile(const std::wstring& wstrFileName)
{
	if(m_Folder_iter == m_kHash.end())
	{ 
		return false; 
	}

	FOLDER_STATE& kFolderState = m_Folder_iter->second;
	if(kFolderState->kFileHash.size())
	{	
		m_File_iter = kFolderState->kFileHash.find(wstrFileName);
		if(m_File_iter == kFolderState->kFileHash.end())
		{ 
			return false; 
		}
	}
	else
	{
		return	false;
	}

	return	true;
}

//	이건 데이타 받아오는 거고
bool CPFile::GetData(std::vector<char>& vecData)
{
	if(m_Folder_iter == m_kHash.end())
	{ 
		return false; 
	}

	FOLDER_STATE& kFolderState = m_Folder_iter->second;
	if(m_File_iter == kFolderState->kFileHash.end())
	{ 
		return false; 
	}

	FILE_STATE& kFileState = m_File_iter->second;

	size_t offset = m_kHashSize + kFileState.offset;
	size_t	pDataSize = 0;

	//	사이즈 알아보자 - 압축?
	if(kFileState.bIsZipped)
	{
		pDataSize = kFileState.zipped_file_size;
	}
	else
	{
		pDataSize = kFileState.org_file_size;
	}

	//	사이즈 알아보자 - 암호화?
	if(kFileState.bIsEncrypt)
	{
		pDataSize += BM::ENC_HEADER_SIZE;
	}

	//	복사하자
	vecData.resize(pDataSize);
	m_ffile.seekg(static_cast<std::streamoff>(offset));
	m_ffile.read(&vecData.at(0), static_cast<std::streamsize>(pDataSize));

	return	true;
}

//	이건 데이타 쓰는 거고
bool CPFile::SaveData(const std::vector<char>& vecData)
{
	if( !m_ffile.is_open() )
	{
		return false;
	}

	if(!vecData.size())
	{ 
		return false; 
	}

	FOLDER_STATE& kFolderState = m_Folder_iter->second;
	if( m_File_iter == kFolderState->kFileHash.end() )
	{
		return false;
	}

	FILE_STATE& kFileState = m_File_iter->second;

	size_t offset = m_kHashSize + kFileState.offset;
	m_ffile.seekg(static_cast<std::streamoff>(offset));
	m_ffile.write(&vecData.at(0), static_cast<std::streamsize>(vecData.size()));

	return	true;
}