#include "stdafx.h"
#include "Packinfo.h"
#include <assert.h>
#include "BM/FileSupport.h"
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

using namespace BM;

bool BM::g_bUseFormattedFile = false;
int const PACK_VERSION = 1;

template< typename T1, typename T2 >
bool TInsert(T1 &t1, T2 &t2)
{
	T1::iterator itor = t1.begin();
	while(itor != t1.end())
	{
		std::wstring wstrConv =	(*itor);
		BM::ConvFolderMark(wstrConv);
		BM::DetachFolderMark(wstrConv);

		LWR(wstrConv);//소문자로 전환

		auto ret = t2.insert( std::make_pair(wstrConv, T2::mapped_type()));
		if(!ret.second)
		{
			assert(NULL);
			return false;
		}
		++itor;
	}
	return true;
}

template <class Type>
class ElementLwr
{
public:
   void operator ( ) ( Type& elem ) const
   {
      LWR(elem);
   }
};

bool CPackInfo::Init()
{
	const TCHAR PACK_CONFIG_FILE_NAME[] =  _T(".\\DataPackConfig.ini");

	PACK_INIT_INFO kInfo;

	TCHAR szOption[MAX_PATH] = {0,};
	::GetPrivateProfileString( _T("PACK_OPTION"), _T("IGNORE_VERSION_FOLDER"),	_T(""), szOption, sizeof(szOption), PACK_CONFIG_FILE_NAME);
	BM::ParseOptionElement(szOption, kInfo.lstIgnoreFolder);
	for_each(kInfo.lstIgnoreFolder.begin(), kInfo.lstIgnoreFolder.end(), ElementLwr<std::wstring>());

	::GetPrivateProfileString( _T("PACK_OPTION"), _T("IGNORE_VERSION_ALLFOLDER"),	_T(""), szOption, sizeof(szOption), PACK_CONFIG_FILE_NAME);
	BM::ParseOptionElement(szOption, kInfo.lstIgnoreAllFolder);
	for_each(kInfo.lstIgnoreAllFolder.begin(), kInfo.lstIgnoreAllFolder.end(), ElementLwr<std::wstring>());

	::GetPrivateProfileString( _T("PACK_OPTION"), _T("IGNORE_VERSION_EXTENTION"),	_T(""), szOption, sizeof(szOption), PACK_CONFIG_FILE_NAME);
	BM::ParseOptionElement(szOption, kInfo.lstIgnoreExtention);
	for_each(kInfo.lstIgnoreExtention.begin(), kInfo.lstIgnoreExtention.end(), ElementLwr<std::wstring>());

	::GetPrivateProfileString( _T("PACK_OPTION"), _T("IGNORE_VERSION_FILENAME"),	_T(""), szOption, sizeof(szOption), PACK_CONFIG_FILE_NAME);
	BM::ParseOptionElement(szOption, kInfo.lstIgnoreFileName);
	for_each(kInfo.lstIgnoreFileName.begin(), kInfo.lstIgnoreFileName.end(), ElementLwr<std::wstring>());

	::GetPrivateProfileString( _T("PACK_OPTION"), _T("ENCRYPT_EXTENTION"),	_T(".LUA;.XML;"), szOption, sizeof(szOption), PACK_CONFIG_FILE_NAME);
	BM::ParseOptionElement(szOption, kInfo.lstEncryptExtention);
	for_each(kInfo.lstEncryptExtention.begin(), kInfo.lstEncryptExtention.end(), ElementLwr<std::wstring>());

	::GetPrivateProfileString( _T("PACK_OPTION"), _T("PACK_FOLDER"),	_T("XML;SCRIPT;"), szOption, sizeof(szOption), PACK_CONFIG_FILE_NAME);
	BM::ParseOptionElement(szOption, kInfo.lstPackFolder);
	for_each(kInfo.lstPackFolder.begin(), kInfo.lstPackFolder.end(), ElementLwr<std::wstring>());

	::GetPrivateProfileString( _T("PACK_OPTION"), _T("NONE_PACK_FOLDER"),	_T(""), szOption, sizeof(szOption), PACK_CONFIG_FILE_NAME);
	BM::ParseOptionElement(szOption, kInfo.lstNonePackFolder);
	for_each(kInfo.lstNonePackFolder.begin(), kInfo.lstNonePackFolder.end(), ElementLwr<std::wstring>());

	if(TInsert(kInfo.lstIgnoreFolder, m_IgnoreFolder))
	if(TInsert(kInfo.lstIgnoreAllFolder, m_IgnoreAllFolder))
	if(TInsert(kInfo.lstIgnoreExtention, m_IgnoreExtention))
	if(TInsert(kInfo.lstIgnoreFileName, m_IgnoreFileName))
	if(TInsert(kInfo.lstEncryptExtention, m_EncryptExtention))
	if(TInsert(kInfo.lstPackFolder, m_PackFolder))
	if(TInsert(kInfo.lstNonePackFolder, m_NonePackFolder))
	{
		return true;
	}

	return false;
}

HRESULT CPackInfo::FolderState(std::wstring const& kStr, PgFolderInfo &rkOutInfo)const
{
	std::wstring wstrConv =	kStr;

	BM::ConvFolderMark(wstrConv);
	LWR(wstrConv);
	BM::DetachFolderMark(wstrConv);
	std::wstring wstrLastFolder = BM::GetLastFolderName(wstrConv);

	if(IsIgnoreFolder(wstrConv))
	{
		return E_FAIL;
	}
	if(IsIgnoreAllFolder(wstrLastFolder))
	{
		return E_FAIL;
	}

	if(IsPackFolder(wstrConv))
	{
		rkOutInfo.bIsUsePack = true;
		return S_OK;
	}
	if(IsNonePackFolder(wstrConv))
	{
		rkOutInfo.bIsUsePack = false;
		return S_OK;
	}

	return S_OK;
}

HRESULT CPackInfo::FileState(std::wstring const& kStr, PgFileInfo &rkOutInfo)const
{
	std::wstring wstrConv = kStr;
	LWR(wstrConv);
	
	std::wstring::size_type const pos = wstrConv.rfind(_T('.'));

	std::wstring kExtention;
	if(std::wstring::npos != pos)
	{
		kExtention = wstrConv.substr(pos);
	}

	if(IsIgnoreFileName(wstrConv)){return E_FAIL;}
	if(IsIgnoreExtention(kExtention)){return E_FAIL;}
	
	if(IsEncryptExtention(kExtention))
	{
		rkOutInfo.bIsEncrypt = true;
	}
	else
	{
		rkOutInfo.bIsEncrypt = false;
	}
	return S_OK;
}

bool CPackInfo::IsIgnoreFolder(std::wstring const& kStr)const
{
	ElementHash::const_iterator itor = m_IgnoreFolder.find(kStr);
	if(itor != m_IgnoreFolder.end())
	{
		return true;
	}
	return false;
}

bool CPackInfo::IsIgnoreAllFolder(std::wstring const& kStr)const
{
	ElementHash::const_iterator itor = m_IgnoreAllFolder.find(kStr);
	if(itor != m_IgnoreAllFolder.end())
	{
		return true;
	}
	return false;
}

bool CPackInfo::IsIgnoreExtention(std::wstring const& kStr)const
{
	ElementHash::const_iterator itor = m_IgnoreExtention.find(kStr);
	if(itor != m_IgnoreExtention.end())
	{
		return true;
	}
	return false;
}

bool CPackInfo::IsIgnoreFileName(std::wstring const& kStr)const
{
	ElementHash::const_iterator itor = m_IgnoreFileName.find(kStr);
	if(itor != m_IgnoreFileName.end())
	{
		return true;
	}
	return false;
}

bool CPackInfo::IsEncryptExtention(std::wstring const& kStr)const
{
	ElementHash::const_iterator itor = m_EncryptExtention.find(kStr);
	if(itor != m_EncryptExtention.end())
	{
		return true;
	}
	return false;
}

bool CPackInfo::IsPackFolder(std::wstring const& kStr)const
{
	ElementHash::const_iterator itor = m_PackFolder.find(kStr);
	if(itor != m_PackFolder.end())
	{
		return true;
	}
	return false;
}

bool CPackInfo::IsNonePackFolder(std::wstring const& kStr)const
{
	ElementHash::const_iterator itor = m_NonePackFolder.find(kStr);
	if(itor != m_NonePackFolder.end())
	{
		return true;
	}
	return false;
}

namespace BM {
	void TWriteTable_DataPack(BM::Stream &rkPacket, FileHash const &rkTbl)
	{
		rkPacket.Push(rkTbl.size());

		FileHash::const_iterator itor = rkTbl.begin();

		while(itor != rkTbl.end())
		{
			std::wstring kKey = (*itor).first;
			LWR(kKey);
			rkPacket.Push(kKey);
			(*itor).second.WriteToPacket(rkPacket);
			++itor;
		}
	}


	void TLoadTable_DataPack(BM::Stream &rkPacket, FileHash &kTbl)
	{
		size_t cont_size = 0;
		rkPacket.Pop(cont_size);

		while(cont_size--)
		{
			FileHash::key_type kKey;
			FileHash::mapped_type kValue;

			rkPacket.Pop(kKey);
			LWR(kKey);
			kValue.ReadFromPacket(rkPacket);

			kTbl.insert(std::make_pair(kKey, kValue));
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
bool tagFolderInfo::Insert(std::wstring const& rkStr, PgFileInfo const& Info)
{
	std::wstring wstrLwrName = rkStr;
	LWR(wstrLwrName);
	auto ret = kFileHash.insert(std::make_pair(wstrLwrName, Info));
	return ret.second;
}

tagFolderInfo::~tagFolderInfo()
{
#ifdef _DEBUG
	DWORD time = ::timeGetTime();
	BM::vstring log;
#endif

	kFileHash.clear();

#ifdef _DEBUG
	time = (::timeGetTime() - time);
	log = L"Assign FolderInfo : ";
	log += (int)(time/1000);
	log += L"s\n";
	OutputDebugString(log);
#endif

}

void tagFolderInfo::operator = (struct tagFolderInfo &rhs)
{
#ifdef _DEBUG
	DWORD time = ::timeGetTime();
	BM::vstring log;
#endif

	this->bIsUsePack = rhs.bIsUsePack;
	this->kFileHash = rhs.kFileHash;

#ifdef _DEBUG
	time = (::timeGetTime() - time);
	log = L"Assign FolderInfo : ";
	log += (int)(time/1000);
	log += L"s\n";
	OutputDebugString(log);
#endif
}


bool tagFolderInfo::Find(std::wstring const& rkFileName, PgFileInfo const *&pOut)const
{
	std::wstring wstrLwrName = rkFileName;
	LWR(wstrLwrName);

	FileHash::const_iterator file_itor = kFileHash.find(wstrLwrName);
	if(file_itor != kFileHash.end())
	{
		pOut = &(*file_itor).second;
		return true;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
tagPackHeaderExtInfo::tagPackHeaderExtInfo()
{
	i64FormatIdentifier = DATA_FORMAT_IDENTIFIER;
	iFormatVersion = DATA_FORMAT_VERSION;
	i64GameVersion = PACK_VERSION;
	bIsZipped = false;
	org_size = 0;
	packet_size = 0;
	bFormatted = BM::g_bUseFormattedFile;
	bCurrentVersion = BM::g_bUseFormattedFile;
}

size_t tagPackHeaderExtInfo::ThisSize()const
{
	size_t additional_size = 0;
	if (bFormatted)
	{
		additional_size = sizeof(i64FormatIdentifier) + sizeof(iFormatVersion) + sizeof(kReserved);
	}

	return sizeof(i64GameVersion) + sizeof(org_size) + sizeof(packet_size) + sizeof(bIsZipped)
		+ sizeof(size_t)/*벡터 사이즈*/ + vecPacketEncHeader.size() + additional_size;
}

size_t tagPackHeaderExtInfo::HeaderSize()const
{
	return ThisSize() + packet_size;
}

bool tagPackHeaderExtInfo::CheckFormatted() const
{
	if (DATA_FORMAT_IDENTIFIER != i64FormatIdentifier)
	{
		return false;
	}

	return true;
}

bool tagPackHeaderExtInfo::CheckFormatVersion() const
{
	if (DATA_FORMAT_VERSION != iFormatVersion)
	{
		return false;
	}

	return true;
}

bool tagPackHeaderExtInfo::ConvertFormat()
{
	return true;
}

size_t tagPackHeaderExtInfo::WriteToMem(char* &pDest)
{
	size_t offset = 0;

	if (bFormatted == true || g_bUseFormattedFile)
	{
		::memcpy(pDest, &i64FormatIdentifier, 8);
		pDest+=8; 
		offset+=8;

		::memcpy(pDest, &iFormatVersion, 4);
		pDest+=4;
		offset+=4;
		bFormatted = true;
		bCurrentVersion = true;
	}
	
	::memcpy(pDest, &i64GameVersion, 8);
	pDest+=8; 
	offset+=8;

	size_t enc_header_size = vecPacketEncHeader.size();
	::memcpy(pDest, &enc_header_size, 4); 
	pDest+=4; 
	offset+=4;

	::memcpy(pDest, &vecPacketEncHeader.at(0), vecPacketEncHeader.size()); 
	pDest+=vecPacketEncHeader.size(); 
	offset+=vecPacketEncHeader.size();
	
	::memcpy(pDest, &bIsZipped, 1); 
	pDest+=1; 
	offset+=1;
	
	::memcpy(pDest, &org_size, 4); 
	pDest+=4; 
	offset+=4;
	
	::memcpy(pDest, &packet_size, 4); 
	pDest+=4; 
	offset+=4;

	if (bFormatted == true || g_bUseFormattedFile)
	{
		char reserved[16] = {0,};
		::memcpy(pDest, &reserved, 16); 
		pDest+=16; 
		offset+=16;
	}
	
	return offset;
}

size_t tagPackHeaderExtInfo::ReadFromMem(char* &pDest)
{
	size_t offset = 0;

	::memcpy(&i64FormatIdentifier, pDest, 8);
	::memcpy(&iFormatVersion, pDest+8, 4);

	if (CheckFormatted())
	{
		bFormatted = true;
		if (CheckFormatVersion())
		{
			bCurrentVersion = true;
		}
		else
		{
			// 옛날 버전 뭔가 다른게 필요하면 처리하자.
			bCurrentVersion = false;
		}
		pDest+=12;
		offset+=12;
	}
	else
	{
		// format을 정하기 전인 아주 예전 포맷
		i64FormatIdentifier = DATA_FORMAT_IDENTIFIER;
		iFormatVersion = DATA_FORMAT_VERSION;
		bFormatted = false;
		bCurrentVersion = false;
	}
	
	::memcpy(&i64GameVersion, pDest, 8);
	pDest+=8; 
	offset+=8;

	size_t enc_header_size = 0;
	::memcpy(&enc_header_size, pDest, 4); 
	pDest+=4; 
	offset+=4;

	vecPacketEncHeader.resize(enc_header_size);
	::memcpy(&vecPacketEncHeader.at(0), pDest, enc_header_size); 
	pDest+=enc_header_size; 
	offset+=enc_header_size;
	
	::memcpy(&bIsZipped, pDest, 1); 
	pDest+=1; 
	offset+=1;
	
	::memcpy(&org_size, pDest, 4); 
	pDest+=4; 
	offset+=4;
	
	::memcpy(&packet_size, pDest, 4); 
	pDest+=4;
	offset+=4;

	if (bFormatted)
	{
		// reserved 16 byte;
		pDest+=16;
		offset+=16;
	}
	
	return offset;
}

size_t tagPackHeaderExtInfo::WriteToFile(std::fstream &fflie)
{
	std::streamoff offset = 0;

	if (bFormatted == true || g_bUseFormattedFile)
	{
		fflie.seekp(offset);
		fflie.write((char*)&i64FormatIdentifier, 8);
		offset+=8;

		fflie.seekp(offset);
		fflie.write((char*)&iFormatVersion, 4);
		offset+=4;
		bFormatted = true;
		bCurrentVersion = true;
	}

	fflie.seekp(offset);
	fflie.write((char*)&i64GameVersion, 8);
	offset+=8;

	size_t enc_header_size = vecPacketEncHeader.size();
	
	fflie.seekp(offset);
	fflie.write((char*)&enc_header_size, 4);
	offset+=4;

	fflie.seekp(offset);
	fflie.write((char*)&vecPacketEncHeader.at(0), (std::streamsize)vecPacketEncHeader.size());
	offset+=(std::streamsize)vecPacketEncHeader.size();
	
	fflie.seekp(offset);
	fflie.write((char*)&bIsZipped, 1);
	offset+=1;
	
	fflie.seekp(offset);
	fflie.write((char*)&org_size, 4);
	offset+=4;
	
	fflie.seekp(offset);
	fflie.write((char*)&packet_size, 4);
	offset+=4;

	if (bFormatted == true || g_bUseFormattedFile)
	{
		char reserved[16] = {0,};
		fflie.seekp(offset);
		fflie.write(reserved, 16);
		offset+=16;
	}

	return offset;
}

size_t tagPackHeaderExtInfo::ReadFromFile(std::fstream &fflie)
{
	std::streamoff offset = 0;

	fflie.seekg(offset);
	fflie.read((char*)&i64FormatIdentifier, 8);	
	offset+=8;

	fflie.seekg(offset);
	fflie.read((char*)&iFormatVersion, 4);
	offset+=4;

	if (CheckFormatted())
	{
		bFormatted = true;
		if (CheckFormatVersion())
		{
			bCurrentVersion = true;
		}
		else
		{
			// 옛날 버전 뭔가 다른게 필요하면 처리하자.
			bCurrentVersion = false;
		}
	}
	else
	{
		// format을 정하기 전인 아주 예전 포맷
		i64FormatIdentifier = DATA_FORMAT_IDENTIFIER;
		iFormatVersion = DATA_FORMAT_VERSION;
		offset = 0;
		bFormatted = false;
		bCurrentVersion = false;

	}

	fflie.seekg(offset);
	fflie.read((char*)&i64GameVersion, 8);
	offset+=8;

	size_t enc_header_size = 0;
	fflie.seekg(offset);
	fflie.read((char*)&enc_header_size, 4);
	offset+=4;

	vecPacketEncHeader.resize(enc_header_size);

	fflie.seekg(offset);
	fflie.read((char*)&vecPacketEncHeader.at(0), (std::streamsize)vecPacketEncHeader.size());
	offset+=(std::streamsize)vecPacketEncHeader.size();
	
	fflie.seekg(offset);
	fflie.read((char*)&bIsZipped, 1);
	offset+=1;
	
	fflie.seekg(offset);
	fflie.read((char*)&org_size, 4);
	offset+=4;
	
	fflie.seekg(offset);
	fflie.read((char*)&packet_size, 4);
	offset+=4;

	if (bFormatted)
	{
		// reserved 16 byte;
		// 아직 쓰는 정보가 없어서 읽을 필요가 없다.
		offset+=16;
	}

	return offset;
}

size_t tagPackHeaderExtInfo::ReadFromFile(BM::IFStream &fflie)
{
	size_t offset = 0;

	fflie.Read(offset, (char*)&i64FormatIdentifier, 8);
	offset+=8;

	fflie.Read(offset, (char*)&iFormatVersion, 4);
	offset+=4;

	if (CheckFormatted())
	{
		bFormatted = true;
		if (CheckFormatVersion())
		{
			bCurrentVersion = true;
		}
		else
		{
			// 옛날 버전 뭔가 다른게 필요하면 처리하자.
			bCurrentVersion = false;
		}
	}
	else
	{
		// format을 정하기 전인 아주 예전 포맷
		i64FormatIdentifier = DATA_FORMAT_IDENTIFIER;
		iFormatVersion = DATA_FORMAT_VERSION;
		offset = 0;
		bFormatted = false;
		bCurrentVersion = false;
	}

	fflie.Read(offset, (char*)&i64GameVersion, 8);
	offset+=8;

	size_t enc_header_size = 0;
	fflie.Read(offset, (char*)&enc_header_size, 4);
	offset+=4;

	vecPacketEncHeader.resize(enc_header_size);

	fflie.Read(offset, (char*)&vecPacketEncHeader.at(0), vecPacketEncHeader.size());
	offset+=vecPacketEncHeader.size();
	
	fflie.Read(offset, (char*)&bIsZipped, 1);
	offset+=1;
	
	fflie.Read(offset, (char*)&org_size, 4);
	offset+=4;
	
	fflie.Read(offset, (char*)&packet_size, 4);
	offset+=4;

	if (bFormatted)
	{
		// reserved 16 byte;
		// 아직 쓰는 정보가 없어서 읽을 필요가 없다.
		offset+=16;
	}

	return offset;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
void tagFIleInfo::operator = (const WIN32_FIND_DATA &rhs)
{
	unsigned __int64 SizeHigh = rhs.nFileSizeHigh;
	unsigned __int64 AccValue = (unsigned __int64)MAXDWORD+1;

	unsigned __int64 iChildSize = (SizeHigh * AccValue) + rhs.nFileSizeLow;

	org_file_size = (size_t)iChildSize;
	zipped_file_size = (size_t)iChildSize;
}

bool tagFIleInfo::operator == (const tagFIleInfo &rhs)const//파일 비교.
{
	if(org_file_size != rhs.org_file_size){return false;}
	if(bIsEncrypt != rhs.bIsEncrypt){return false;}
	if (BM::g_bCalcSHA2CheckSum)
	{
		if(0 != memcmp(ucSHA2CheckSum, rhs.ucSHA2CheckSum, PG_SHA2_LENGTH * sizeof(unsigned char))){return false;}
	}
	else
	{
		if(iOrgCRC != rhs.iOrgCRC){return false;}
	}
	return true;
}

bool tagFIleInfo::operator != (const tagFIleInfo &rhs)const//파일 비교.
{
	if(*this == rhs)
	{
		return false;
	}
	return true;
}

size_t tagFIleInfo::WriteSize()const
{
	return zipped_file_size;
}

size_t tagFIleInfo::OrgSize()const
{
	return org_file_size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace PU
{
	void TWriteTable_AM(BM::Stream &rkPacket, BM::FolderHash const &rkTbl)
	{
		rkPacket.Push(rkTbl.size());

		BM::FolderHash::const_iterator itor = rkTbl.begin();

		while(itor != rkTbl.end())
		{
			rkPacket.Push((*itor).first);
			(*itor).second->WriteToPacket(rkPacket);
			++itor;
		}
	}

	bool TLoadTable_AM(BM::Stream &rkPacket, BM::FolderHash &kTbl, size_t const max_count)
	{
		size_t cont_size = 0;
		if(	rkPacket.Pop(cont_size)
		&&	cont_size <= max_count)
		{
			if( rkPacket.RemainSize() >= cont_size*4 )
			{
				while(cont_size--)
				{
					BM::FolderHash::key_type kKey;
					BM::PgFolderInfo* pValue;
					pValue = BM::PgFolderInfo::New();
					
					rkPacket.Pop(kKey);
					pValue->ReadFromPacket(rkPacket);

					LWR(kKey);
					kTbl.insert(std::make_pair(kKey, BM::PgFolderInfoPtr(pValue)));
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_CONT).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkPacket.RemainSize())
			}
		}
		return false;
	}
}