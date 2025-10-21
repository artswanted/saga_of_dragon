#include	"AmendPatch.h"
#include	"PatchCommon.h"

size_t g_nPatchHeaderSize = 0;

bool	PatchIDCheck(std::wstring& PatchFileName, const bool IsOnline)
{//	패치 아이디 체크
	//	버전 정보를 읽자
	BM::VersionInfo	const kNowVersionInfo = GetPatchID(L"patch.id");
	BM::VersionInfo	kNewVersionInfo;
	if(IsOnline)
	{
		kNewVersionInfo = GetPatchID(L"patch_new.id");
	}
	else
	{
		kNewVersionInfo = GetPatchID(L"m_patch.id");
	}

	//	버젼 정보에서 패치넘버만 남겨라
	wchar_t kMajorVer[30] = {0,};
	wchar_t kMinorVer[30] = {0,};
	wchar_t kTinyVer[30] = {0,};

	_i64tow_s(kNowVersionInfo.Version.i16Major, kMajorVer, 30, 10);
	_i64tow_s(kNowVersionInfo.Version.i16Minor, kMinorVer, 30, 10);
	_i64tow_s(kNowVersionInfo.Version.i32Tiny, kTinyVer, 30, 10);
	
	std::wstring wstrClientVersion;
	wstrClientVersion = WSTR_GAME_NAME + L" " + WSTR_VERSION + L": " + std::wstring(kMajorVer) + L"." + std::wstring(kMinorVer) + L"." + std::wstring(kTinyVer);

	g_PProcess.SetIsClientVer(wstrClientVersion);

	//	패치할까
	if(kNowVersionInfo.iVersion < kNewVersionInfo.iVersion)
	{
		g_PProcess.SetWorkMsg(L"Recovery...");
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK
		throw	eTHROW_FAILED;
	}

	if(kNowVersionInfo.iVersion == kNewVersionInfo.iVersion)
	{
		g_PProcess.SetIsVersion(kNowVersionInfo.iVersion);
		g_PProcess.SetWorkMsg(L"Start the Game...");
		g_PProcess.SetTotalPer(1.0f);
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK
		throw	eTHROW_FAILED;
	}

	if(kNowVersionInfo.iVersion > kNewVersionInfo.iVersion)
	{
		g_PProcess.SetWorkMsg(L"Client Version High...");
		g_PProcess.SetTotalPer(1.0f);
	}
	
	wchar_t kAryTemp[30] = {0,};
	if(IsOnline)
	{
		//	패치 파일 이름
		PatchFileName = std::wstring(_i64tow(kNowVersionInfo.Version.i32Tiny, kAryTemp, 10)) + L"To" + std::wstring(_i64tow(kNewVersionInfo.Version.i32Tiny, kAryTemp, 10)) + L".dat";
	}
	else
	{
		//	패치 파일 이름
		PatchFileName = std::wstring(_i64tow(kNewVersionInfo.Version.i16Minor, kAryTemp, 10)) + L"To" + std::wstring(_i64tow(kNewVersionInfo.Version.i32Tiny, kAryTemp, 10)) + L".dat";
	}
	
	g_PProcess.SetIsVersion(kNewVersionInfo.iVersion);

	 return	true;
}

bool LoadPatchIDVerFromFile()
{// Patch.ID를 읽어 화면에 현재 ver를 읽어와 런처에 세팅한다
	BM::VersionInfo const kNowVersionInfo = GetPatchID(L"patch.id");	
	wchar_t kMajorVer[30] = {0,};
	wchar_t kMinorVer[30] = {0,};
	wchar_t kTinyVer[30] = {0,};
	_i64tow_s(kNowVersionInfo.Version.i16Major, kMajorVer, 30, 10);
	_i64tow_s(kNowVersionInfo.Version.i16Minor, kMinorVer, 30, 10);
	_i64tow_s(kNowVersionInfo.Version.i32Tiny, kTinyVer, 30, 10);
	__int64 const& i64OldVer = g_PProcess.GetIsOldVersion();
	if(kNowVersionInfo.iVersion == i64OldVer)
	{// 최초 실행시 읽어들인 버전과 다운 받은 새버전이 동일함
		return false;
	}

	std::wstring wstrClientVersion;
	wstrClientVersion = WSTR_GAME_NAME + L" " + WSTR_VERSION + L": " + std::wstring(kMajorVer) + L"." + std::wstring(kMinorVer) + L"." + std::wstring(kTinyVer);
	g_PProcess.SetIsClientVer(wstrClientVersion);
	return true;
}