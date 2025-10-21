#include "stdafx.h"
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <tchar.h>
#include "BM/ThreadObject.h"
#include "BM/PgFileMapping.h"
#include "BM/hsel.h"
#include "BM/STLSupport.h"
#include "BM/Filesupport.h"
#include "BM/Stream.h"
#include "BM/SHA2.h"

namespace BM
{
	bool g_bCalcSHA2CheckSum = false;
	DWORD g_dwCRCTime = 0;
	DWORD g_dwSHA2Time = 0;

	int ParseOptionElement(std::wstring const& wstrOption, std::list< std::wstring > &rkOut)
	{//a;b;c;d; 형식을 다 떼어줌.
		std::wstring const wstrSrc = wstrOption +_T(";");//a;b 였을때 b를 파싱 못하는거 방지.

		int iCount = 0;
		std::wstring::size_type start_pos = 0;

		std::wstring::size_type end_pos = wstrSrc.find(_T(";"));

		while(end_pos != std::wstring::npos)
		{
			std::wstring wstrConvSrc = wstrSrc.substr(start_pos, end_pos-start_pos);
			start_pos = end_pos+1;
			end_pos = wstrSrc.find(_T(";"), start_pos);

			if(wstrConvSrc.size())
			{
				UPR(wstrConvSrc);
				rkOut.push_back(wstrConvSrc);
			}
			++iCount;
		}

		return iCount;
	}

	size_t const GetFileSize(std::wstring const& strFileName)
	{
		struct stat file_stats = {0,};
		int fd = 0;
		errno_t const err = ::_wsopen_s( &fd, strFileName.c_str(), _O_RDONLY | _O_BINARY, _SH_DENYWR, _S_IREAD );
		if( err){return 0;}

		if( -1 == fd){return 0;}

		int const ret = ::fstat(fd, &file_stats);

		if( ret == -1 ){return 0;}

		::_close(fd);

		size_t const file_size = file_stats.st_size;
		return file_size;
	}

	bool EncStream(std::vector< char > &vecData, std::vector< char > &rkOutEncryptInfo)
	{
		HSEL::CHSEL hsel;
		if(HSEL::CHSEL::INIT_SUCCESS != hsel.Init(HSEL::CHSEL::ENCRYPT_SECURE|HSEL::CHSEL::SWAP_ON|HSEL::CHSEL::KEY_ROLL_DYNAMIC|HSEL::CHSEL::KEY_INIT_AUTO))
		{
			return false;
		}

		HSEL::CHSEL::INIT_INFO kInitInfo;
		hsel.GetInitInfo(kInitInfo);

		if(!vecData.size())
		{//사이즈 없으면 InitInfo라도 받아라
			return true;
		}

		DWORD dwCRC = 0;
		if(!hsel.Encrypt( &vecData.at(0), (DWORD)vecData.size(), dwCRC))
		{
//			//__asm int 3;
			return false;
		}
		
		rkOutEncryptInfo.resize(sizeof(kInitInfo)+ sizeof(dwCRC));
		
		::memcpy(&rkOutEncryptInfo.at(0), &kInitInfo, sizeof(kInitInfo));//암호화 초기 정보와
		::memcpy(&rkOutEncryptInfo.at(sizeof(kInitInfo)), &dwCRC, sizeof(dwCRC));//CRC 정보를 가져가라.
		
		return true;
	}

	bool DecStream(std::vector< char > &vecData, std::vector< char > &rkEncryptInfo)
	{
		HSEL::CHSEL::INIT_INFO *pkInitInfo = NULL;
		DWORD *pkdwCRC = NULL;

		pkInitInfo = (HSEL::CHSEL::INIT_INFO*)&rkEncryptInfo.at(0);
		pkdwCRC = (DWORD*)&rkEncryptInfo.at(sizeof(HSEL::CHSEL::INIT_INFO));
		
		HSEL::CHSEL hsel;
		if(HSEL::CHSEL::INIT_SUCCESS != hsel.InitManual(*pkInitInfo))
		{
//			//__asm int 3;
			return false;
		}

		if(!vecData.size())
		{//사이즈 없으면 그냥 돌려
			return true;
		}

		DWORD dwCRC = 0;
		if( !hsel.Decrypt( &vecData.at(0), (DWORD)vecData.size(), dwCRC))
		{
//			//__asm int 3;
			return false;
		}
		
		if( *pkdwCRC == dwCRC)
		{
			return true;
		}
		
//		//__asm int 3;
		return false;
	}

	bool EncSave(std::wstring const& strFileName, std::vector< char > const& vecData)
	{
		if(vecData.size())
		{
			return EncSave(strFileName, &vecData.at(0), vecData.size() );
		}
		
//		//__asm int 3;
		return false;
	}

	bool EncSave(std::wstring const& strFileName, void const* pData, size_t const data_size)
	{//암호화해 저장
		if(!pData//포인터가 NULL.
		|| !data_size)//데이터 사이즈가 없거나
		{
			//__asm int 3;
			return false;
		}

		std::vector< char > vecBuf;
		std::vector< char > vecEncryptInfo;
		BM::Stream kCompressInfo;

		std::fstream ffile;
		ffile.open( strFileName.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::ate | std::ios_base::binary );//in 모드임.

		if(!ffile.is_open())
		{
			goto __FAILED;
		}
		else
		{
			vecBuf.reserve(data_size+sizeof(char));//압축여부 저장 길이 추가

			char bIsCompressed = true;
			if(!BM::Compress((char*)pData, static_cast<unsigned long>(data_size), vecBuf))
			{//압축 실패면 원 데이터로 간다.
				vecBuf.resize(data_size);
				::memcpy( &vecBuf.at(0), pData, data_size);
				bIsCompressed = false;
			}
			
			//따로저장 하자 그냥.
			kCompressInfo.Push(data_size);//압축전 사이즈.
			kCompressInfo.Push(vecBuf.size());//압축후 사이즈.
			kCompressInfo.Push(bIsCompressed);//압축여부 저장.

			EncStream(kCompressInfo.Data(), vecEncryptInfo);
			std::streamoff write_offset = 0;

			ffile.seekp(write_offset);
			ffile.write(&vecEncryptInfo.at(0), (std::streamsize)vecEncryptInfo.size());
			write_offset += (std::streamoff)vecEncryptInfo.size();
			
			ffile.seekp(write_offset);
			ffile.write(&kCompressInfo.Data().at(0), (std::streamsize)kCompressInfo.Data().size());
			write_offset += (std::streamoff)kCompressInfo.Data().size();

			EncStream(vecBuf, vecEncryptInfo);

			ffile.seekp(write_offset);
			ffile.write(&vecEncryptInfo.at(0), (std::streamsize)vecEncryptInfo.size());
			write_offset += (std::streamoff)vecEncryptInfo.size();

			ffile.seekp(write_offset);
			ffile.write(&vecBuf.at(0), (std::streamsize)vecBuf.size());
			write_offset += (std::streamoff)vecBuf.size();
			
			ffile.close();
			return true;
		}
	__FAILED:
		{
			if(ffile.is_open())
			{
				ffile.close();
			}
			//__asm int 3;
			return false;
		}
	}

	bool DecLoad(std::wstring const& strFileName, std::vector< char > &vecOut )
	{
//		size_t const file_size = GetFileSize(strFileName);

		HSEL::CHSEL::INIT_INFO kInitInfo;

		BM::IFStream ffile(strFileName);
		if(ffile.IsOpen())
		{
			size_t const encryptinfo_size = sizeof(kInitInfo) + sizeof(DWORD);

			std::vector< char > vecEncryptInfo;
			BM::Stream kCompressInfo;
			
			kCompressInfo.Data().resize(sizeof(size_t) + sizeof(size_t) + sizeof(bool));//압축 전 후 사이즈, 압축 여부
			vecEncryptInfo.resize(encryptinfo_size);

////////////////////
			size_t offset = 0;
			ffile.Read(offset, &vecEncryptInfo.at(0), vecEncryptInfo.size());
			offset += vecEncryptInfo.size();

			ffile.Read(offset, &kCompressInfo.Data().at(0), kCompressInfo.Data().size());
			offset += kCompressInfo.Data().size();

			if(!DecStream(kCompressInfo.Data(), vecEncryptInfo))
			{
				goto __FAILED;
			}

////////////////////

			kCompressInfo.PosAdjust();
			size_t org_data_size = 0;
			size_t zipped_data_size = 0;

			bool bIsCompressed = false;
			kCompressInfo.Pop(org_data_size);
			kCompressInfo.Pop(zipped_data_size);
			kCompressInfo.Pop(bIsCompressed);

			if(org_data_size)
			{
				vecOut.resize(zipped_data_size);

				ffile.Read(offset, &vecEncryptInfo.at(0), vecEncryptInfo.size());
				offset += vecEncryptInfo.size();

				ffile.Read(offset, &vecOut.at(0), vecOut.size());
				offset += vecOut.size();

				if(!DecStream(vecOut, vecEncryptInfo))
				{
					goto __FAILED;
				}

				if(bIsCompressed)
				{
					std::vector< char > kUncompressedData;
					kUncompressedData.swap(vecOut);
					vecOut.resize(org_data_size);
					if(!BM::UnCompress(vecOut, kUncompressedData))
					{
						goto __FAILED;
					}
				}
			}
			return true;
		}
	__FAILED:
		{
			//__asm int 3;
			return false;
		}
	}

	bool DecLoadFromMem(std::vector< char > const& vecData, std::vector< char >& vecOut)
	{
		if( vecData.empty() )
		{
			return false;
		}

		HSEL::CHSEL::INIT_INFO kInitInfo;
		size_t const encryptinfo_size = sizeof(kInitInfo) + sizeof(DWORD);

		std::vector< char > vecEncryptInfo;
		BM::Stream kCompressInfo;
		
		kCompressInfo.Data().resize(sizeof(size_t) + sizeof(size_t) + sizeof(bool));//압축 전 후 사이즈, 압축 여부
		vecEncryptInfo.resize(encryptinfo_size);

		if( vecData.size() < vecEncryptInfo.size() )
		{
			return false;
		}

		size_t offset = 0;
		::memcpy(&vecEncryptInfo.at(0), &vecData.at(offset), vecEncryptInfo.size());
		offset += vecEncryptInfo.size();

		if( vecData.size() < ( offset + kCompressInfo.Data().size() ) )
		{
			return false;
		}

		::memcpy(&kCompressInfo.Data().at(0), &vecData.at(offset), kCompressInfo.Data().size());
		offset += kCompressInfo.Data().size();

		if(!DecStream(kCompressInfo.Data(), vecEncryptInfo))
		{
			return false;
		}

		kCompressInfo.PosAdjust();
		size_t org_data_size = 0;
		size_t zipped_data_size = 0;

		bool bIsCompressed = false;
		kCompressInfo.Pop(org_data_size);
		kCompressInfo.Pop(zipped_data_size);
		kCompressInfo.Pop(bIsCompressed);

		if(org_data_size)
		{
			vecOut.resize(zipped_data_size);
			if( vecData.size() < (offset + vecEncryptInfo.size()) )
			{
				return false;
			}
			::memcpy(&vecEncryptInfo.at(0), &vecData.at(offset), vecEncryptInfo.size());
			offset += vecEncryptInfo.size();

			if( vecData.size() < (offset + vecOut.size()) )
			{
				return false;
			}
			::memcpy(&vecOut.at(0), &vecData.at(offset), vecOut.size());
			offset += vecOut.size();

			if(!DecStream(vecOut, vecEncryptInfo))
			{
				return false;
			}

			if(bIsCompressed)
			{
				std::vector< char > kUncompressedData;
				kUncompressedData.swap(vecOut);
				vecOut.resize(org_data_size);
				if(!BM::UnCompress(vecOut, kUncompressedData))
				{
					return false;
				}
			}
		}
		return true;
	}

	bool DecLoadFromMem(size_t const FileOrgSize, size_t const FileZipSize, bool const bIsCompressed, bool const bIsEncript, std::vector< char > const& vecData, std::vector< char >& vecOut)
	{
		if( FileOrgSize )
		{
			vecOut.resize(FileZipSize);

			std::vector< char > vecEncryptInfo;
			size_t offset = 0;

			if( bIsEncript )
			{
				HSEL::CHSEL::INIT_INFO kInitInfo;
				size_t const encryptinfo_size = sizeof(kInitInfo) + sizeof(DWORD);

				vecEncryptInfo.resize(encryptinfo_size);

				if( vecData.size() < (offset + vecEncryptInfo.size()) )
				{
					return false;
				}
				::memcpy(&vecEncryptInfo.at(0), &vecData.at(offset), vecEncryptInfo.size());
				offset += vecEncryptInfo.size();
			}

			if( vecData.size() < (offset + vecOut.size()) )
			{
				return false;
			}
			::memcpy(&vecOut.at(0), &vecData.at(offset), vecOut.size());
			offset += vecOut.size();

			if( bIsEncript )
			{
				if(!DecStream(vecOut, vecEncryptInfo))
				{
					return false;
				}
			}

			if(bIsCompressed)
			{
				std::vector< char > kUncompressedData;
				kUncompressedData.swap(vecOut);
				vecOut.resize(FileOrgSize);
				if(!BM::UnCompress(vecOut, kUncompressedData))
				{
					return false;
				}
			}
		}
		return true;
	}

	bool FileToMem(std::wstring const& strFileName, std::vector< char > &vecOut)
	{
		static Loki::Mutex s_kMutex;
		BM::CAutoMutex kLock(s_kMutex);

		vecOut.resize(0);

		size_t const file_size = GetFileSize(strFileName);

		BM::IFStream ffile(strFileName);
		
		if(ffile.IsOpen())
		{
			if(file_size)
			{
				vecOut.reserve(file_size);
				vecOut.resize(file_size);

				ffile.Read(0, &vecOut.at(0), static_cast<std::streamsize>(vecOut.size()));
			}
		}
		else
		{
			goto __FAILED;
		}

		return true;

	__FAILED:
		{
			//__asm int 3;
			return false;
		}
	}

	bool MemToFile(std::wstring const& strFileName, std::vector< char > const& vecData, bool bReserveFolder)
	{
		assert(vecData.size());
		if(vecData.size())
		{
			return MemToFile(strFileName, &vecData.at(0), vecData.size(), bReserveFolder);
		}
		return false;
	}
	
	bool MemToFile(std::wstring const& strFileName, void const *pData, size_t const szSize, bool bReserveFolder)
	{
		if(!pData//포인터가 NULL.
		|| !szSize)//데이터 사이즈가 없거나
		{
			return false;
		}

		static Loki::Mutex s_kMutex;
		BM::CAutoMutex kLock(s_kMutex);//Safe Lock

		if(bReserveFolder)
		{
			BM::ReserveFolder(strFileName);
		}

		std::fstream ffile;
		ffile.open( strFileName.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::ate | std::ios_base::binary );//in 모드임.
		if( !ffile.is_open())
		{
			assert(NULL);
			goto __FAILED;
		}
		
		ffile.seekp(0);
		ffile.write((char const*)pData, (std::streamsize)szSize);

		ffile.close();
		return true;

	__FAILED:
		{
			if(ffile.is_open())
			{
				ffile.close();
			}
			//__asm int 3;
			return false;
		}
	}
	
	void EraseWhiteString( std::wstring &wstrData )
	{
		std::wstring::iterator itor = wstrData.begin();
		while( itor != wstrData.end() )
		{
			if( L'\t'== (*itor)
			||	L'\r'== (*itor)
			||	L' ' == (*itor)
			||	L'\n' == (*itor))
			{
				itor = wstrData.erase(itor);
				continue;
			}
			else
			{
				break;
			}
		}
	}

	void ClearComment(std::wstring const& wstrFindSource, std::wstring &strOut)
	{
		std::wstring strSource = wstrFindSource;

		std::wstring const strStartMark = L";";
		std::wstring const strEndMark = L"\n";

		while(1)
		{
			std::wstring::size_type const start_pos = strSource.find(strStartMark);
			if(std::wstring::npos == start_pos)
			{
				break;
			}

			std::wstring::size_type const end_pos = strSource.find(strEndMark, start_pos);
			
			if(std::wstring::npos == end_pos)
			{
				strSource.erase(start_pos);
			}
			else
			{
				strSource.erase(start_pos, end_pos-start_pos);
			}
		}
		strOut.swap(strSource);
	}

	int GetFromINB(std::wstring const& wstrFindSection, std::wstring const& wstrFindElement, BM::vstring& vstrResult, BM::vstring const& vstrDefault, std::wstring const& wstrFindSource)	// use for reading INB file
	{
		vstrResult = _T("");//초기화 시켜주고

		// ; 마크 이후를 날려 버리는 코드가 필요함.!!!
		std::wstring const strHead = L"[";
		std::wstring const strTail = L"]";
		std::wstring const strEqualMark = L"=";
		std::wstring const strEndMark = L"\r\n";

		std::wstring strSecion = strHead + wstrFindSection + strTail;
		std::wstring strElement = wstrFindElement;
		
		std::wstring strSource;
		std::wstring strSourceUpr;
		ClearComment(wstrFindSource, strSource);//주석 제거.

		strSourceUpr = strSource;

		UPR(strSecion);
		UPR(strElement);
		UPR(strSourceUpr);//대문자화 시켜서 찾도록

		std::wstring strRange;//Goto 로 전방선언
		
		std::wstring::size_type const section_pos = strSourceUpr.find( strSecion );//섹션 확인.
		if( std::wstring::npos == section_pos )//섹션을 찾는다.
		{
			goto __FAILED;
		}

		std::wstring::size_type const section_end_pos = strSourceUpr.find(strEndMark, section_pos + strSecion.length() );
		if( std::wstring::npos == section_end_pos )//섹션 해당 줄은 스킵.
		{
			goto __FAILED;
		}
		
		std::wstring::size_type const element_pos = strSourceUpr.find( strElement, section_end_pos );
		if( std::wstring::npos == element_pos )//엘레멘트를 찾는다.
		{
			goto __FAILED;
		}

		std::wstring::size_type const equal_mark_pos = strSourceUpr.find( strEqualMark, element_pos + strElement.length() );
		if( std::wstring::npos == equal_mark_pos )//엘레멘트 이후에 "=" 를 찾는다.
		{
			goto __FAILED;
		}

		std::wstring::size_type const end_mark_pos = strSourceUpr.find( strEndMark, equal_mark_pos );
		if( std::wstring::npos == equal_mark_pos )//엘레멘트 이후에 "=" 를 찾는다.
		{
			goto __FAILED;
		}
		
	//	std::wstring 
		strRange = strSource.substr( equal_mark_pos+1, end_mark_pos - equal_mark_pos - 1);// '=' 이후. '\n' 이전
		EraseWhiteString( strRange );
		
	//	std::wstring 
	//	strReverse = std::wstring(strRange.rbegin(), strRange.rend());
	//	EraseWhiteString( strReverse );

	//	std::wstring 
	//		strResult = std::wstring(strReverse.rbegin(), strReverse.rend() );
		
		vstrResult = strRange;
		return (int)vstrResult;
	__FAILED:
		{
			vstrResult = vstrDefault;
			return (int)vstrResult;
		}
	}

	__int64 GetStreamCRC(char const* lpStream, __int64 const StreamSize)
	{//이 함수 바꾸면, PW 저장, 파일 저장 다 바뀌어야 하니까 조심하세요.
#ifdef _DEBUG
		DWORD time = ::timeGetTime();
#endif
		size_t const block_size = sizeof(__int64);

		__int64 iCRCValue	=	0;

		__int64 iGabCRC1	=	0;
		__int64 iGabCRC2	=	0;
		__int64 iGabCRC3	=	0;
		__int64 iGabCRC4	=	0;

		__int64 iBlockCount		= (StreamSize/block_size);	//블럭수 가지고 오기 == __int32 iBlockCount = iStreamSize/블럭사이즈;
		__int64 iRemainCount	= (StreamSize%block_size);	//짜투리 길이 받기==__int32 iRemainCount = iStreamSize%블럭사이즈; 
		__int64 iRemainPos		= (iBlockCount*block_size);	//짜투리 바이트 단위 위치 iBlockCount*블럭사이즈

		__int64 const *lpBlock	= reinterpret_cast<__int64 const*>(lpStream);

		while(iBlockCount)
		{
			iCRCValue	^= (*(lpBlock));
			lpBlock++;
			iBlockCount--;
		}
		while(iRemainCount)
		{
			iCRCValue	^=	lpStream[iRemainPos];
			
			iRemainPos++;
			iRemainCount--;
		}

#ifdef _DEBUG
		g_dwCRCTime += (::timeGetTime() - time);
#endif
		return iCRCValue;// ^ iGabCRC1 ^ iGabCRC2 ^ iGabCRC3 ^ iGabCRC4;
	}

	bool GetFileCRC(std::wstring const& strFileName, unsigned __int64 &rkCRC)
	{
		std::vector< char > vecOut;
		if(FileToMem(strFileName, vecOut))
		{
			rkCRC = 0;
			if(vecOut.size())
			{
				rkCRC = GetStreamCRC(&vecOut.at(0), vecOut.size());
			}
			return true;
		}

		return false;
	}

	bool GetStreamSHA2(char const* lpStream, __int64 const StreamSize, unsigned char* pSHA2CheckSum)
	{
		if (NULL == lpStream || NULL == pSHA2CheckSum)
			return false;

#ifdef _DEBUG
		DWORD time = ::timeGetTime();
#endif
		unsigned char const *lpBlock	= reinterpret_cast<unsigned char const *>(lpStream);

		SHA256_CTX	context;

		SHA256_Init(&context);
		SHA256_Update(&context, lpBlock, StreamSize);
		SHA256_Final(pSHA2CheckSum, &context);

#ifdef _DEBUG
		g_dwSHA2Time += (::timeGetTime() - time);
#endif
		return true;
	}

	bool GetFileSHA2(std::wstring const& strFileName, unsigned char* pSHA2CheckSum)
	{
		if (NULL == pSHA2CheckSum)
		{
			return false;
		}

		std::vector< char > vecOut;
		if(FileToMem(strFileName, vecOut))
		{
			if(vecOut.size())
			{
				return GetStreamSHA2(&vecOut.at(0), vecOut.size(), pSHA2CheckSum);
			}
		}
		
		return false;
	}


	void ReserveFolder(std::wstring const& wstrFileName)
	{
		std::wstring wstrTemp = wstrFileName;

		ConvFolderMark(wstrTemp);// 컨버팅 된다.

		std::wstring::size_type pos_slash = wstrTemp.find(_T('\\'));
		while(true)
		{
			std::wstring strMkDir = wstrTemp.substr(0, pos_slash);

			pos_slash = wstrTemp.find(_T('\\'), pos_slash+1);

			int const iRet = ::_wmkdir(strMkDir.c_str());

			if(pos_slash == std::wstring::npos)
			{
				break;
			}
		}
	}

	void DivFolderAndFileName(std::wstring const& kLocation, std::wstring &rkFolder, std::wstring &rkFileName)
	{//실패 하거나 할것이 없다.
		std::wstring wstrDir(kLocation);

		BM::ConvFolderMark(wstrDir);// 컨버팅 된다.

		std::wstring::size_type pos = std::wstring::npos;

		pos = wstrDir.rfind(_T('\\'));//폴더 구분 표시2
		if(pos != std::wstring::npos){goto __PARSE_COMPLETE;}
		
		pos = wstrDir.rfind(_T(':'));//드라이브나
		if(pos != std::wstring::npos){goto __PARSE_COMPLETE;}

	__PARSE_COMPLETE:
		{
			rkFileName = wstrDir.substr(pos+1);
			rkFolder = wstrDir.substr(0,pos+1);
		}
	}

	std::wstring const GetParentFolder(std::wstring const& kFullPath)
	{
		std::wstring const kLast = BM::GetLastFolderName(kFullPath); 
		std::wstring::size_type const rpos = kFullPath.rfind(kLast);
		std::wstring kRet = kFullPath.substr(0, rpos);
		DetachFolderMark(kRet);

		return kRet;
	}

	std::wstring const GetLastFolderName(std::wstring const& kTotalDir)
	{
		std::wstring wstrDir(kTotalDir);
		AddFolderMark(wstrDir);// 컨버팅 된다.

		const static std::wstring kNullString = _T("");

		std::wstring::size_type start_pos = std::wstring::npos;
		std::wstring::size_type end_pos = std::wstring::npos;

		end_pos = wstrDir.rfind(_T('\\'));//폴더 구분 표시1
		if(	end_pos != std::wstring::npos
		&&	end_pos > 0 )
		{
			start_pos = wstrDir.rfind(_T('\\'), end_pos-1);//폴더 구분 표시2
			if(start_pos != std::wstring::npos){goto __PARSE_COMPLETE;}
			
			start_pos = wstrDir.rfind(_T(':'), end_pos-1);//드라이브나
			if(start_pos != std::wstring::npos){goto __PARSE_COMPLETE;}
		}

		return kNullString;

	__PARSE_COMPLETE:
		{
			std::wstring kRet;
			kRet = wstrDir.substr(start_pos+1,end_pos-start_pos-1);
			return kRet;
		}
	}
	
	void ConvFolderMark(std::wstring &kTotalDir)// '\\'로 변환
	{
		std::wstring::iterator itor = kTotalDir.begin();
		while(itor != kTotalDir.end())
		{
			if((*itor) == _T('/'))
			{
				(*itor)	= _T('\\');
			}
			++itor;
		}
	}

	void AddFolderMark(std::wstring &kTotalDir)// '\\' 없으면 붙임
	{
		ConvFolderMark(kTotalDir);
		
		if(kTotalDir.size())
		{
			if(_T('\\') != kTotalDir.at(kTotalDir.size()-1))
			{
				kTotalDir += _T('\\');
			}
		}
	}

	void DetachFolderMark(std::wstring &kTotalDir)// '\\' 이 있으면 땜.
	{
		ConvFolderMark(kTotalDir);
		
		if(kTotalDir.size())
		{
			if(_T('\\') == kTotalDir.at(kTotalDir.size()-1))
			{
				kTotalDir.resize(kTotalDir.size()-1);
			}
		}
	}

	void ConvFolderMark(std::string &kTotalDir)// '\\'로 변환
	{
		std::string::iterator itor = kTotalDir.begin();
		while(itor != kTotalDir.end())
		{
			if((*itor) == '/')
			{
				(*itor)	= '\\';
			}
			++itor;
		}
	}

	void AddFolderMark(std::string &kTotalDir)// '\\' 없으면 붙임
	{
		ConvFolderMark(kTotalDir);

		if(kTotalDir.size())
		{
			if('\\' != kTotalDir.at(kTotalDir.size()-1))
			{
				kTotalDir += '\\';
			}
		}
	}
	
	void DetachFolderMark(std::string &kTotalDir)// '\\' 이 있으면 땜.
	{
		ConvFolderMark(kTotalDir);
		
		if(kTotalDir.size())
		{
			if(_T('\\') == kTotalDir.at(kTotalDir.size()-1))
			{
				kTotalDir.resize(kTotalDir.size()-1);
			}
		}
	}
}


namespace BM
{
	IFStream::IFStream()
	{	
		m_hFileHandle = INVALID_HANDLE_VALUE;
		m_dwError = 0;
		m_bMemLoaded = false;
		m_kFileBuffer.clear();
	}

	IFStream::IFStream(std::wstring const & kFilename)
	{
		m_hFileHandle = INVALID_HANDLE_VALUE;
		m_dwError = 0;
		m_bMemLoaded = false;
		m_kFileBuffer.clear();
		Open(kFilename);
	}

	IFStream::~IFStream()
	{	
		if(INVALID_HANDLE_VALUE != m_hFileHandle)
		{
			CloseHandle(m_hFileHandle);
			m_hFileHandle = INVALID_HANDLE_VALUE;
		}
		m_dwError = 0;
		m_bMemLoaded = false;
	}

	void IFStream::Open(std::wstring const & kFilename)
	{	// open wide-named file (old style) -- EXTENSION
		BM::CAutoMutex kLock(m_kMutex);
		Close();
		
		HANDLE const hRetHandle = CreateFileW(kFilename.c_str(), FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_READONLY,NULL);

		if(INVALID_HANDLE_VALUE != hRetHandle)
		{
			m_kFileName = kFilename;
			m_hFileHandle = hRetHandle;
			return;
		}

#ifdef _DEBUG
		BM::vstring str;
		str = L"------------- " + kFilename + L" Opened -------------\n";
		OutputDebugString(str);
#endif

		m_dwError = GetLastError();
	}

	bool IFStream::IsOpen() const
	{	// test if C stream has been opened
		BM::CAutoMutex kLock(m_kMutex);
		if(INVALID_HANDLE_VALUE != m_hFileHandle)
		{
			return true;
		}
		return false;
	}

	void IFStream::Close()
	{	// close the C stream
		BM::CAutoMutex kLock(m_kMutex);
		if(INVALID_HANDLE_VALUE != m_hFileHandle)
		{
			CloseHandle(m_hFileHandle);
			m_hFileHandle = INVALID_HANDLE_VALUE;
			m_kFileBuffer.clear();
			m_bMemLoaded = false;
#ifdef _DEBUG
			BM::vstring str;
			str = L"------------- " + m_kFileName + L" Closed -------------\n";
			OutputDebugString(str);
#endif
		}
		m_dwError = 0;
	}

	DWORD IFStream::Read(size_t const offset, void * const pBuf, size_t const copy_size)
	{//읽은 사이즈를 리턴함
		BM::CAutoMutex kLock(m_kMutex);
		if(INVALID_HANDLE_VALUE != m_hFileHandle)
		{
			if (m_bMemLoaded)
			{
				size_t mem_copy_size = copy_size;
				if (offset + copy_size > m_kFileBuffer.size())
				{
					mem_copy_size = m_kFileBuffer.size() - offset;
				}

				memcpy(pBuf, &m_kFileBuffer.at(offset), mem_copy_size);
				return mem_copy_size;
			}
			else
			{
				DWORD const dwFilePtr = SetFilePointer(m_hFileHandle, (LONG)offset, NULL, FILE_BEGIN);
				if(INVALID_SET_FILE_POINTER != dwFilePtr)
				{
					DWORD dwReaded = 0;
					BOOL const bRet = ReadFile(m_hFileHandle, pBuf, (DWORD)copy_size, &dwReaded, NULL);

					if (FALSE == bRet)
					{
						m_dwError = GetLastError();
					}
					
					return dwReaded;
				}
			}
		}
		
		//__asm int 3;//잘못된 포인트 지정.
		return 0;
	}

	bool IFStream::LoadFileToMem()
	{
		BM::CAutoMutex kLock(m_kMutex);
		if(INVALID_HANDLE_VALUE == m_hFileHandle)
		{
			return false;
		}

		ClearFileFromMem();

		DWORD fileSize = ::GetFileSize(m_hFileHandle, NULL);
		if (INVALID_FILE_SIZE == fileSize)
		{
			return false;
		}
		
		m_kFileBuffer.resize(fileSize);
		DWORD readSize = Read(0, &m_kFileBuffer.at(0), fileSize);

		if (readSize == fileSize)
		{
			m_bMemLoaded = true;
			return true;
		}
		
		return false;
	}

	void IFStream::ClearFileFromMem()
	{
		m_kFileBuffer.clear();
		m_bMemLoaded = false;
	}


}