#pragma once

#include <list>
#include <map>
#include <string>
#include <tchar.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <vector>
#include <unordered_map>

#include "BM/FileSupport.h"
#include "BM/Stream.h"
#include "BM/sha2.h"
#include "Loki/SmartPtr.h"

#define PG_SHA2_LENGTH	SHA256_DIGEST_LENGTH

namespace BM
{
#pragma pack(1)
	extern bool g_bUseFormattedFile;

	__int64 const DATA_FORMAT_IDENTIFIER = 0xFFF0FEF1FDF2FCF3;
	__int32 const DATA_FORMAT_VERSION = 0x01000000;	// 1.0.0.0

	typedef struct tagPackHeaderExtInfo//변수 추가때는 Write, Read, size 다 고쳐야됨
	{//헤더에 기록됨. 암호화 헤더와 함께
		tagPackHeaderExtInfo();

		__int64	i64FormatIdentifier;
		__int32	iFormatVersion;
		__int64  i64GameVersion;//버전

		size_t org_size;//압축전 사이즈
		size_t packet_size;//압축후 사이즈
		char kReserved[16];
		bool bIsZipped;
		bool bFormatted;
		bool bCurrentVersion;

		std::vector< char > vecPacketEncHeader;
		
		bool CheckFormatted() const;
		bool CheckFormatVersion() const;
		bool ConvertFormat();

		size_t WriteToMem(char* &pDest);
		size_t ReadFromMem(char* &pDest);

		size_t WriteToFile(std::fstream &fflie);
		size_t ReadFromFile(std::fstream &fflie);

		size_t ReadFromFile(BM::IFStream &fflie);

		size_t ThisSize()const;
		size_t HeaderSize()const;
	}PgPackHeaderExtInfo;

	typedef struct tagFIleInfo
	{
		tagFIleInfo()
		{
			org_file_size = 0;
			iOrgCRC = 0;
			offset = 0;
			
			zipped_file_size = 0;//zipped 안됐다면 OrgFileSize와 동일하게 유지 할것

			bIsEncrypt = false;
			bIsZipped = false;

			memset(ucSHA2CheckSum, 0, PG_SHA2_LENGTH * sizeof(unsigned char));
		}
		
		size_t WriteSize()const;
		size_t OrgSize()const;

		void operator = (const WIN32_FIND_DATA &rhs);
		bool operator == (const tagFIleInfo &rhs)const;//파일 비교.
		bool operator != (const tagFIleInfo &rhs)const;//파일 비교.

		size_t org_file_size;
		unsigned __int64 iOrgCRC;//CRC는 64비트 처리
		size_t offset;
		size_t zipped_file_size;
		bool bIsEncrypt;
		bool bIsZipped;
		unsigned char ucSHA2CheckSum[PG_SHA2_LENGTH];

		void WriteToPacket(BM::Stream &Stream)const
		{
			Stream.Push(org_file_size);
			if (BM::g_bCalcSHA2CheckSum)
			{
				Stream.Push(ucSHA2CheckSum);
			}
			else
			{
				Stream.Push(iOrgCRC);//CRC는 64비트 처리
			}
			Stream.Push(offset);
			Stream.Push(zipped_file_size);
			Stream.Push(bIsEncrypt);
			Stream.Push(bIsZipped);
			// SHA2 Checksum은 넣지 않는다.
		}
		
		void ReadFromPacket(BM::Stream &Stream)
		{
			Stream.Pop(org_file_size);
			if (BM::g_bCalcSHA2CheckSum)
			{
				Stream.Pop(ucSHA2CheckSum);
			}
			else
			{
				Stream.Pop(iOrgCRC);//CRC는 64비트 처리
			}
			Stream.Pop(offset);
			Stream.Pop(zipped_file_size);
			Stream.Pop(bIsEncrypt);
			Stream.Pop(bIsZipped);
			// SHA2 Checksum은 넣지 않는다.
		}
	}PgFileInfo;

	typedef std::map<std::wstring, PgFileInfo> FileHash;

	void TWriteTable_DataPack(BM::Stream &rStream, FileHash const &rkTbl);
	void TLoadTable_DataPack(BM::Stream &rStream, FileHash &kTbl);
	
	typedef struct tagFolderInfo
	{
		tagFolderInfo() {bIsUsePack = false;}
		~tagFolderInfo();
		bool Insert(std::wstring const& rkStr, PgFileInfo const& Info);
		bool Find(std::wstring const& rkFileName, PgFileInfo const *&pOut)const;

		bool bIsUsePack;
		FileHash kFileHash;
		
		size_t min_size() const
		{
			return sizeof(bool) +
				sizeof(size_t);
		}

		void WriteToPacket(BM::Stream &Stream)const
		{
			Stream.Push(bIsUsePack);
			TWriteTable_DataPack(Stream, kFileHash);
		}
		
		void ReadFromPacket(BM::Stream &Stream)
		{
			Stream.Pop(bIsUsePack);
			TLoadTable_DataPack(Stream, kFileHash);
		}

		static struct tagFolderInfo* New()
		{
			return new struct tagFolderInfo;
		}

		struct tagFolderInfo* Clone()
		{
			struct tagFolderInfo* pInfo = New();
			*pInfo = *this;
			return pInfo;
		}

		void operator = (struct tagFolderInfo &rhs);
	}PgFolderInfo;

	typedef Loki::SmartPtr  // yet, but let's overlook that for now...
    <
      PgFolderInfo,                  // note, T still varies
	  Loki::RefCounted,         // but everything else is fixed
      Loki::DisallowConversion,
      Loki::NoCheck,
      Loki::DefaultSPStorage
    >
    PgFolderInfoPtr;

	typedef std::map<std::wstring, PgFolderInfoPtr> FolderHash;

	typedef struct tagPackInitInfo
	{
		typedef std::list<std::wstring> ElementList;

		ElementList lstIgnoreFolder;
		ElementList lstIgnoreAllFolder;
		ElementList lstIgnoreExtention;
		ElementList lstIgnoreFileName;

		ElementList lstEncryptExtention;
		ElementList lstPackFolder;
		ElementList lstNonePackFolder;
	}PACK_INIT_INFO;
#pragma pack()

	class CPackInfo
	{
	public:
		CPackInfo(){}
		virtual ~CPackInfo(){}
	public:

		typedef enum eValue
		{
			PI_NONE				= 0x00000000,
			PI_IGNORE			= 0x00000001,
			PI_USING				= 0x00000002,
			PI_PACK				= 0x00000004,
			PI_NONE_PACK		= 0x00000008,
			PI_ENCRYPT			= 0x00000010,
			PI_NONE_ENCRYPT	= 0x00000020,
		}EValue;
		
		bool Init();
		
		HRESULT FolderState(std::wstring const& kStr, PgFolderInfo &rkOutInfo)const;
		HRESULT FileState(std::wstring const& kStr, PgFileInfo &rkOutInfo)const;
		bool IsIgnoreFolder(std::wstring const& kStr)const;
		bool IsIgnoreAllFolder(std::wstring const& kStr)const;
		bool IsIgnoreExtention(std::wstring const& kStr)const;
		bool IsIgnoreFileName(std::wstring const& kStr)const;
		bool IsEncryptExtention(std::wstring const& kStr)const;
		bool IsPackFolder(std::wstring const& kStr)const;
		bool IsNonePackFolder(std::wstring const& kStr)const;

	protected:
		typedef std::map<std::wstring, int> ElementHash;

		ElementHash m_IgnoreFolder;
		ElementHash m_IgnoreAllFolder;
		ElementHash m_IgnoreExtention;
		ElementHash m_IgnoreFileName;

		ElementHash m_EncryptExtention;
		ElementHash m_PackFolder;
		ElementHash m_NonePackFolder;
	};
}

namespace PU
{
	void TWriteTable_AM(BM::Stream &rStream, BM::FolderHash const &rkTbl);
	bool TLoadTable_AM(BM::Stream &rStream, BM::FolderHash &kTbl, size_t const max_count = INT_MAX);
}