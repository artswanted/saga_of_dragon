#pragma once

#include "PackTask.h"
#include "List.h"

enum E_WORK_TYPE
{
	EWT_UNKNOWN_END	= 0,
	EWT_KNOWN_END	= 1,
};

struct SWorkProgressInfo
{
	E_WORK_TYPE	Type;
	E_WORK_STATE State;
	int iTotal;
	int iNow;
};

struct SIFTextData
{
	std::wstring kJobName;
	std::wstring kContents;
	std::wstring kStatic1;
	std::wstring kStatic2;
	std::wstring kStatic3;
};

enum E_WORK_MESSAGE_TYPE
{
	EWMT_NOTICE_HEADER	= 0,
	EWMT_ALREADY_PROCESS,
	EWMT_END,
};

typedef std::map< E_PROCESS_TYPE, SIFTextData >	CONT_IF_TEXTDATA;
typedef std::map< unsigned int, std::wstring > CONT_MESSAGE;

class IFBase;
class MainWorker : public IOutPutLoger
{
public:
	bool Initialize(std::wstring const& kCmdLine);
	void SetSelectedJob(E_PROCESS_TYPE const Type);
	bool Input(WPARAM wParam, LPARAM lParam);
	void StartWork();
	void Terminate();
	bool SetIniData();
	std::wstring const& GetMSG(E_WORK_MESSAGE_TYPE const Type);
	void StartButtonLock(bool bDisable);

	virtual void ShowMessage(E_WORK_STATE const State, E_PROCESS_TYPE const Type, std::wstring const& kMessage);
	virtual void LogMessage(E_PROCESS_TYPE const Type, E_LOG_MSG const LogTypeNo, std::wstring const& kAddonText = L"");

	std::wstring GetWndText(DWORD ControlID);

	E_PROCESS_TYPE GetSelectedJob() const { return m_kSelectJob; };
	HWND GethWnd() const { return m_hMainWnd; };
	void SethWnd(HWND const hWnd) { m_hMainWnd = hWnd; };

	MainWorker(void);
	~MainWorker(void);
private:
	bool ParseCmdLine(std::wstring const& kCmdLine);
	IFBase* GetIFControl(E_PROCESS_TYPE const Type);
	void UpdateVersion();
	void UpdatePrevVersion(BM::VersionInfo VInfo);
	void UpdatePatchVersion(BM::VersionInfo VInfo);

	E_VERSION_TYPE m_eVersionType;
	SWorkProgressInfo m_kProgInfo;
	E_PROCESS_TYPE m_kSelectJob;
	PackTask m_kTaskWorker;
	bool m_bIsAutomatic;
	DWORD m_dwWorkFlag;
	IFBase* m_pIFControl;
	LogList* m_pkLogList;
	OptionList* m_pkOptionList;

	CONT_IF_TEXTDATA m_kIFTextData;
	CONT_MESSAGE m_kMessage;
	CONT_MESSAGE m_kLogMessage;

	HWND m_hMainWnd;
};
