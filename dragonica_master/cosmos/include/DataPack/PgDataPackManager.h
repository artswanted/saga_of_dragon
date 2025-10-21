#pragma once
#include "BM/BM.h"
#include "BM/HSEL.h"
#include "CEL/CEL.h"
#include "BM/PgFileMapping.h"
#include "PackInfo.h"

namespace BM
{
	//팩 파일 관리
	//파일 포인터 관리
	//버퍼링.
	DWORD const GFL_ONLY_FOLDER		= 0x00000001;
	DWORD const GFL_ONLY_FILE			= 0x00000002;
	DWORD const GFL_RECURSIVE			= 0x00000010;

	DWORD const GFL_RECURSIVE_ALL_FILE		= GFL_ONLY_FILE|GFL_RECURSIVE;
	DWORD const GFL_RECURSIVE_ALL_FOLDER	= GFL_ONLY_FOLDER|GFL_RECURSIVE;
	DWORD const GFL_RECURSIVE_ALL				= GFL_ONLY_FILE|GFL_ONLY_FOLDER|GFL_RECURSIVE;

	size_t const ENC_HEADER_SIZE = sizeof(HSEL::CHSEL::INIT_INFO)+sizeof(DWORD);

	DWORD const DIFF_ONLY_LEFT = 0x00000001;
	DWORD const DIFF_ONLY_RIGHT = 0x00000002;
	DWORD const DIFF_ONLY_DIFF = 0x00000004;
	DWORD const DIFF_ONLY_IDENTICAL = 0x00000008;

	typedef struct tagDVersion
	{
		__int64 i32Tiny : 32;
		__int64 i16Minor : 16;
		__int64 i16Major : 16;
	}SDVersion;

	typedef union
	{	
		SDVersion	Version;
		__int64		iVersion;
	}VersionInfo;

	class PgDataPackManager
	{
	public:
		typedef struct _PackFileCache_new
		{
			_PackFileCache_new() { mHeaderSize = 0; }
			_PackFileCache_new(_PackFileCache_new const&  rhs)
			{
//				mHeaderSize = mHeaderSize;
//				mFolderHash = mFolderHash;
//				m_kStream = 
			}
			
			~_PackFileCache_new()
			{
				if (m_kStream.IsOpen())
				{
					m_kStream.Close();

#ifdef _DEBUG
					BM::vstring log;
					log = L"----------------PgPackFileCache " + GetFileName() + L" Closed----------------\n";
					OutputDebugString(log);
#endif
				}
			}

			_PackFileCache_new& operator=(const _PackFileCache_new&rhs)
			{
#ifdef _DEBUG
				BM::vstring log;
				log = L"----------------assign " + rhs.GetFileName() + L" to " + GetFileName() + L"----------------\n";
				OutputDebugString(log);
#endif
				return *this;
			}

			size_t ReadHeaderExtInfo(PgPackHeaderExtInfo &rOut)
			{
				BM::CAutoMutex kLock(m_kFileLock);
				return rOut.ReadFromFile(m_kStream);//읽기
			}

			size_t ReadData(size_t const offset, char* const& pOutData, size_t const read_size )
			{
				BM::CAutoMutex kLock(m_kFileLock);
				return m_kStream.Read(offset, pOutData, read_size);
			}

			bool Open(std::wstring const&  kFileName)
			{
				BM::CAutoMutex kLock(m_kFileLock);

				m_kStream.Open(kFileName);
				if(m_kStream.IsOpen())
				{
#ifdef _DEBUG
					BM::vstring log;
					log = L"----------------PgPackFileCache " + kFileName + L" Opended----------------\n";
					OutputDebugString(log);
#endif
					return true;
				}
				return false;
			}

			void SetHeaderSIze(size_t const in_size)
			{
				BM::CAutoMutex kLock(m_kFileLock);
				mHeaderSize = in_size;
			}
			size_t GetHeaderSize()const
			{
				BM::CAutoMutex kLock(m_kFileLock);
				return mHeaderSize;
			}

			void SetFolderHash(BM::Stream &Stream)
			{
				BM::CAutoMutex kLock(m_kFileLock);
				{
					size_t folder_size = 0;
					Stream.Pop(folder_size);//Pop 1

					size_t i = 0;
					while(folder_size != i)
					{
						FolderHash::key_type key;
						PgFolderInfoPtr pFolderInfo;
						pFolderInfo = PgFolderInfo::New();
						size_t file_count = 0;

						Stream.Pop(key);//Pop 2
						pFolderInfo->ReadFromPacket(Stream);

						LWR(key);					
						mFolderHash.insert(std::make_pair(key, pFolderInfo));
						++i;
					}
				}
			}

			bool LoadFileToMem() { return m_kStream.LoadFileToMem(); }
			void ClearFileFromMem() { return m_kStream.ClearFileFromMem(); }

			std::wstring GetFileName() const { return m_kStream.GetFileName(); }

		public:
			FolderHash		mFolderHash;
		protected:
			BM::IFStream	m_kStream;
//			FolderHash		mFolderHash;
			size_t			mHeaderSize;

			mutable Loki::Mutex m_kFileLock;
		}PackFileCache_new;

		typedef std::map< std::wstring, PackFileCache_new > CacheHash;
	public:
		PgDataPackManager();
		virtual ~PgDataPackManager();

		static bool Init(const CPackInfo &rkInfo);
		static void Clear();
	public://유저 편의기능
		
		static bool PackFolders(std::wstring const& rkTargetDir);
		static bool PackTargetFolder(std::wstring const& rkTargetDir, std::wstring const& rkResultFileName);

	public:	
		static bool RecursiveProcessPackState(BM::FolderHash &rkFolderHash);
		static bool GetFileListWithoutCRC(std::wstring const& rkDirHeader, FolderHash &rkInHash, DWORD &dwFileCount, DWORD const dwFlag = GFL_RECURSIVE_ALL, std::wstring const& rkStateStandardDir = _T(""));
		static bool GetFileList(std::wstring const& rkDirHeader, FolderHash &rkInHash, DWORD const dwFlag = GFL_RECURSIVE_ALL, std::wstring const& rkStateStandardDir = _T(""));
		static size_t MakeHeader(FolderHash &rkInHash, std::fstream &ffile, bool const bIsWriteToFile = true, __int64 const iVersion = 0);
		static bool PackFile(FolderHash &rkInHash, std::wstring const& rkFileName);
		//static void PrintFolderHash(FolderHash& rkInHash);
		
		static PackFileCache_new* GetPackFileCache(std::wstring const& kFileName, bool bLoadFileToMemory = false);
		static bool IsFileExistInPack(std::wstring const& rkParentName, std::wstring const& rkChildName);
		static bool LoadFromPack(std::wstring const& rkParentName, std::wstring const& rkChildName, std::vector< char > &rkOutData, bool const bIsRealRead = true);		

		static unsigned __int64 BuildOffset(FolderHash &rkFolderHash);//offset값 지정.

		static bool ClearBuffer(std::wstring const& strClearName);
	//	static PgFileMapping* GetMappingFile();
		static void SetOnLoop(bool bLoop);
		static bool GetOnLoop();
	
		static bool MakeDiffList(const FolderHash &rkOrgFolderHash,
													const FolderHash &rkNewFolderHash,
													FolderHash &kResultHash,
													DWORD const dwDiffFlag
													);

		static bool	UpdatePackVersion(std::wstring const& wstrPackedName, VersionInfo const& iVer);
		static bool	CheckPackVersion(std::wstring const& wstrPackedName, VersionInfo const& iVer);
		static bool GetPackVersion(std::wstring const& wstrPackedName, VersionInfo &iVer);
		static bool ConvertToFormattedDataFile(std::wstring const& wstrPackedName);
		static bool UpdateFileCRC(CacheHash::mapped_type &kCache, bool bUseSHA = false);

		static size_t ReadHeader(CacheHash::mapped_type &kCache);
		static size_t ReadHeader(char* pData, FolderHash &rkInHash);

	protected:
		
		static CacheHash m_sCacheHash;
		static CPackInfo m_kPackInfo;
		
		// 패쳐시 GetList 같은 루프는 취소해도 중단 되지 않으므로 이 변수를 스위치로 쓴다.
		static bool m_bIsOnLoop;
		static Loki::Mutex m_kLock;
	};
}