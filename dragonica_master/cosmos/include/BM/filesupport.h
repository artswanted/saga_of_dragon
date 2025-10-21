#pragma once
#include <vector>
#include "Loki/threads.h"
#include "BM/vstring.h"

namespace BM
{
	extern int ParseOptionElement(std::wstring const& wstrOption, std::list<std::wstring> &rkOut);
	
	extern bool EncSave(std::wstring const& strFileName, std::vector< char > const& vecData);
	extern bool EncSave(std::wstring const& strFileName, void const* pData, size_t const data_size);

	extern bool DecLoad(std::wstring const& strFileName, std::vector< char > &vecOut );
	extern bool DecLoadFromMem(std::vector< char > const& vecData, std::vector< char >& vecOut);
	extern bool DecLoadFromMem(size_t const FileOrgSize, size_t const FileZipSize, bool const bIsCompressed, bool const bIsEncript, std::vector< char > const& vecData, std::vector< char >& vecOut);
	
	extern size_t const GetFileSize(std::wstring const& strFileName);
	
	extern bool FileToMem(std::wstring const& strFileName, std::vector< char > &vecOut);
	
	extern bool MemToFile(std::wstring const& strFileName, std::vector< char > const& vecData, bool bReserveFolder = true);
	extern bool MemToFile(std::wstring const& strFileName, void const *pData, size_t const szSize, bool bReserveFolder = true);
	
	extern void EraseWhiteString( std::wstring &wstrData );

	extern int GetFromINB(std::wstring const& wstrFindSection, std::wstring const& wstrFindElement, BM::vstring& vstrResult, BM::vstring const& vstrDefault, std::wstring const& wstrFindSource);	// use for reading INB file
	
	extern __int64 GetStreamCRC(char const* lpStream, __int64 const StreamSize);
	extern bool GetStreamSHA2(char const* lpStream, __int64 const StreamSize, unsigned char* pSHA2CheckSum);
	extern bool GetFileCRC(std::wstring const& strFileName, unsigned __int64 &rkCRC);
	extern bool GetFileSHA2(std::wstring const& strFileName, unsigned char* pSHA2CheckSum);

	extern bool EncStream(std::vector< char > &vecData, std::vector< char > &rkOutEncryptInfo);
	extern bool DecStream(std::vector< char > &vecData, std::vector< char > &rkEncryptInfo);
	extern void ReserveFolder(std::wstring const& wstrFileName);//파일이 요구하는 폴더를 생성 끝에 \ 마크로 폴더냐 아니냐를 구분하므로, 폴더 끝에는 꼭 붙일것.
	extern std::wstring const GetLastFolderName(std::wstring const& kTotalDir);
	extern void DivFolderAndFileName(std::wstring const& kLocation, std::wstring &rkFolder, std::wstring &rkFileName);
	extern std::wstring const GetParentFolder(std::wstring const& kFullPath);

	extern void ConvFolderMark(std::wstring &kTotalDir);// '\\'로 변환
	extern void AddFolderMark(std::wstring &kTotalDir);// '\\' 없으면 붙임
	extern void DetachFolderMark(std::wstring &kTotalDir);// '\\' 이 있으면 땜.
	
	extern void ConvFolderMark(std::string &kTotalDir);// '\\'로 변환
	extern void AddFolderMark(std::string &kTotalDir);// '\\' 없으면 붙임
	extern void DetachFolderMark(std::string &kTotalDir);// '\\' 이 있으면 땜.
	extern bool g_bCalcSHA2CheckSum;
	extern DWORD g_dwCRCTime;
	extern DWORD g_dwSHA2Time;
};

namespace BM
{
	class IFStream
	{
	public:
		explicit IFStream();
		explicit IFStream(std::wstring const & kFilename);
		virtual  ~IFStream();

		void Open(std::wstring const & kFilename);		
		bool IsOpen() const;
		void Close();
		DWORD Read(size_t const offset, void * const pBuf, size_t const copy_size);
		bool LoadFileToMem();
		void ClearFileFromMem();
		std::wstring GetFileName() const { return m_kFileName; }
		DWORD GetLastError() { return m_dwError; }

	private:
		HANDLE m_hFileHandle;
		mutable Loki::Mutex m_kMutex;
		std::wstring m_kFileName;
		DWORD m_dwError;
		bool m_bMemLoaded;
		std::vector< char > m_kFileBuffer;
	};
}