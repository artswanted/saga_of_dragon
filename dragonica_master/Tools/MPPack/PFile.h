#pragma once

#include "DataPack/PgDataPackManager.h"

class CPFile
{
public:
	typedef	BM::FolderHash::const_iterator	FOLDER_CITER;
	typedef BM::FolderHash::key_type		FOLDER_DIR;
	typedef BM::FolderHash::mapped_type		FOLDER_STATE;
	typedef BM::FileHash::const_iterator	FILE_CITER;
	typedef BM::FileHash::key_type			FILE_DIR;
	typedef BM::FileHash::mapped_type		FILE_STATE;

	bool	Load(const std::wstring& wstrFileName);								//	이건 읽어오는거고
	bool	LoadFile(const std::wstring& wstrFileName);
	bool	Init(const std::wstring& wstrFileName, const BM::FolderHash& Hash);	//	이건 만드는 거고
	void	Clear();															//	이건 자원 해제하는 거고
	bool	FindFolder(const std::wstring& wstrFolderName);						//	이건 폴더 찾는거고
	bool	FindFile(const std::wstring& wstrFileName);							//	이건 파일 찾는거고
	bool	GetData(std::vector<char>& vecData);								//	이건 데이타 받아오는 거고
	bool	SaveData(const std::vector<char>& vecData);							//	이건 데이타 쓰는 거고
	const BM::FolderHash&	GetHash() const	{ return	m_kHash; }				//	이건 해쉬 보는거

	CPFile(void);
	virtual ~CPFile(void);

private:
	typedef	BM::FolderHash::iterator	FOLDER_ITER;
	typedef BM::FileHash::iterator		FILE_ITER;

//	PgFileMapping	m_kFile;
	std::fstream	m_ffile;
	BM::FolderHash	m_kHash;
	size_t			m_kHashSize;

	FOLDER_ITER		m_Folder_iter;
	FILE_ITER		m_File_iter;
};