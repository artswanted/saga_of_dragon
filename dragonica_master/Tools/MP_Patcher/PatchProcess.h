#pragma once

#include	"defines.h"

BOOL CALLBACK OptionProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK SubProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
void OnSubIDLE(HWND hWnd);

class PgAXPlay;
class CConfigXml;
class CHBitmap;
class CCTButton;
class Window;
class CTEdit;

typedef struct tagSBRInfo
{
	POINT kPos;
	int	  iWidth;
	int	  iHeight;
	std::wstring kLink;
	tagSBRInfo()
	{
		kPos.x = kPos.y = 0;
		iWidth=0;
		iHeight=0;
	}
}SBRInfo;

typedef struct tagSPrograssBarInfo
{
	std::wstring kImg;
	COLORREF kTextColor;
	POINT kPos;
	
	tagSPrograssBarInfo()
	{
		kPos.x = kPos.y = 0;
		kTextColor = 0;
	}
}SPrograssBarInfo;

typedef struct tagSBtnInfo
{
	POINT kPos;
	int	iID;	
	int	iShapeNum;
	int iType;
	std::wstring kImg;
	std::wstring kLink;
	std::wstring kText;
	tagSBtnInfo()
	{
		kPos.x = kPos.y = 0;
		iID = 0 ;	
		iShapeNum = 3;
		iType = 0;
	}
}SBtnInfo;

typedef struct tagSEditInfo
{
	int iID;
	int iType;
	int TR, TG, TB;
	int BGR, BGG, BGB;
	int iLimitLen;
	POINT kPos;
	POINT kSize;
	tagSEditInfo()
	{
		iID = 0;
		iType = 0;
		TR = TG = TB = 0;
		BGR = BGG = BGB = 0;
		iLimitLen = 0;
		kPos.x = kPos.y = 0;
		kSize.x = kSize.y = 0;
	}

	void operator = (tagSEditInfo const& rhs)
	{
		iID = rhs.iID;
		iType = rhs.iType;
		TR = rhs.TR;
		TG = rhs.TG;
		TB = rhs.TB;
		BGR = rhs.BGR;
		BGG = rhs.BGG;
		BGB = rhs.BGB;
		iLimitLen = rhs.iLimitLen;
		kPos.x = rhs.kPos.x;
		kPos.y = rhs.kPos.y;
		kSize.x = rhs.kSize.x;
		kSize.y = rhs.kSize.y;
	}

}SEditInfo;

typedef struct tagSMsgInfo
{
	POINT kPos;
	COLORREF kTextColor;
	tagSMsgInfo()
	{
		kPos.x =0;
		kPos.y =0;
		kTextColor=0;
	}
}SMsgInfo;

typedef struct tagSEnvData
{
	typedef std::map<int, SBtnInfo> CONT_BTNINFO;
	typedef std::map<int, SEditInfo> CONT_EDITINFO;
	std::wstring kDir;			// 디렉토리
	std::wstring kBgImg;		// 메인 배경
	std::wstring kSubBgImg;		// 메인 배경
	
//	POINT kMsgPos;				
//	POINT kVerPos;				

	SMsgInfo kMainMsgInfo;		// 패치중 메세지
	SMsgInfo kVerMsgInfo;		// 버전정보 표시위치

	SPrograssBarInfo kProgressImg1;		// 프로그래스 이미지1,2
	SPrograssBarInfo kProgressImg2;	
	SBRInfo		 kBRInfo;		// 브라우저 X,Y,WID, HGT,LINK
	CONT_BTNINFO kContBtnInfo;	// 버튼 X,Y,이미지이름, 버튼 갯수, LINK	
	CONT_EDITINFO kContEditInfo;

	tagSEnvData()
	{}
	void Clear()
	{
		kContBtnInfo.clear();
	}
}SEnvData;

typedef enum eMsg
{// 패쳐에서 사용하는 메세지
		E_FAILEDINITWEBBR						= 0,	// 웹브라우져 초기화 실패
		E_CONECTINGSERVER 						= 1,	// 서버 접속 시도
		E_PATCHERALREADYRUN 					= 2,	// 패쳐 이미 실행시
		E_CLIENTALREADYRUN 						= 3,	// 클라이언트 이미 실행중
		E_MERGINGFILE 							= 4,	// 파일 머지중
		E_RECVSERVERLIST 						= 5,	// 서버 리스트에서 받는중 
		E_CREATECLIENTLIST 						= 6,	// 클라이언트 리스트 생성중
		E_COMPARELIST 							= 7,	// 리스트 비교중    
		E_COMPLETECOMPAREDOUPDATE 				= 8,	// 비교 완료. 업데이트 할것
		E_COMPLETECOMPAREPLAY 					= 9,	// 비교 완료 게임 할것 	
		E_CALTOTALSIZE 							= 10,	// 총 크기 계산		
		E_DIVLIST 								= 11,	// 목록 분할
		E_PATCHINGFILE							= 12,	// 패치 중인 파일 
		E_FAILEDDOWNLOADFILE 					= 13,	// 파일 다운로드 실패
		E_FILELOADTOMEMORY 						= 14,	// 파일을 메모리로 읽음
		E_FILEMOVETODIR 						= 15,	// 해당 디렉토리로 파일을 옮김
		E_SIZEHADERANDPACK 						= 16,	// 팩의 데이타 크기 및 헤더 크기    
		E_CREATEALLOCFILE 						= 17,	// 파일 생성 및 할당     
		E_DOWNLOADINGFILE 						= 18,	// 파일 받는중		
		E_FAILEDTOLOADDATA 						= 19,	// 데이타 로드 실패
		E_COPYTOPACK							= 20,	// 팩으로 복사 
		E_MERGEORIGINALWITHDOWNLOADEDPACK		= 21,	// 원본 파일과 다운로드 받은 파일을 합침     
		E_COMPARELISTONLY 						= 22,	// 리스트 만 비교
		E_STARTPATCH 							= 23,	// 패치 시작	
		E_COMPLETEPATCH							= 24,	// 패치 완료
		E_STARTOFFPATCH 						= 25,	// 오프라인 패치 시작
		E_FAILEDLOADINI 						= 26,	// ini 파일 읽기 실패 
		E_FAILEDINITMGR 						= 27,	// 매니저 초기화 실패		
		E_BROKENCRC 							= 28,	// CRC 정보가 깨졌음		
		E_FAILEDCALLPATCHERPATCH 				= 29,	// 패처 교환 프로그램 호출을 실패
		E_FILELISTEMPTY 						= 30,	// 얻어온 리스트가 비었음 
		E_CALULRATIONGFILESIZE 					= 31,	// 사이즈 계산증
		E_INITPATCHER 							= 32,	// 패쳐(패치 아님) 초기화
		E_FAILEDCONNECTSERVER 					= 33,	// 서버 접속 실패		
		E_DECODINGPATCHID						= 34,	// 패치 아이디 암호 해독
		E_SUBWNDRUNNING							= 35,	// 서브 패쳐 윈도우 안닫혔음
		E_FAILEDTOPATCH							= 36,	// 패치 실패
		E_PATCHCANCELED							= 37,	// 패치 취소
		E_WANTTOUPDATE							= 38,	// 새버전 찾음 업데이트 할건가?
		E_RECOVERY_CONFIRM						= 39,	// 최신버전임에도 리커버리(풀체킹)를 할 것인가?
		E_TRY_CANCEL							= 40,	// 취소중 메세지
		E_CANNOT_RUN_CLIENT						= 41,	// 클라이언트 프로그램 실행 오류
		E_FAILEDTOPATCH_AND_TRY_RECOVERY		= 42,	// 버전정보를 찾을 수 없어서 실패 했을 때 리커버리 모드로 들어갈 것인지 물어봄.
		E_SUCCESS_CHANGE_LANG					= 43,	// Change language
		E_FAILED_INIT_LIBUPDATE					= 44,   // Failed initializate libupdate
		E_LIBUPDATE_ALLOCATING					= 45,   // Allocate space for torrent
		E_LIBUPDATE_CHECK						= 46,   // Check files
		E_LIBUPDATE_DOWNLOAD					= 47,   // Download
		E_MSG_MAX
}EMsg;

typedef std::map< HWND, CTEdit* > CONT_EDIT_LIST;
enum ELangChangeMessage
{
	ELCM_NO,
	ELCM_YES,
};
class CPatchProcess
{
//-----------------------------------------------------------------------------
//  변수
//-----------------------------------------------------------------------------
public:
	typedef	enum	eProcess_State
	{
		eProcess_Preparing,
		eProcess_Connecting,
		eProcess_Comparing,
		eProcess_NP_Patching,
		eProcess_P_Patching,
	}EProcess_State;

	typedef enum eLocal_Set
	{
		LS_CHINA,
		LS_SINGAPORE,
		LS_KOREA,
		LS_TAIWAN,
		LS_EUROPE,
		LS_FRANCE,
		LS_GERMANY,
		LS_AMERICA,
		LS_JAPAN,
		LS_THAILAND,
		LS_VIETNAM,
		LS_RUSSIA,
	}ELOCAL_SET;

	typedef	enum eBitmapType
	{
		HBT_DLG_NORMAL,
		HBT_DLG_SUB,
		HBT_DLG_CHS,
		HBT_PROGRESS1,
		HBT_PROGRESS2,
		HBT_END,
	}EBitmapType;

	typedef	enum eUI_Image
	{
		UI_BGTOP,
		UI_BGLEFT,
		UI_BGRIGHT,
		UI_BGSTATE,
		UI_BTN,
		UI_PROG1,
		UI_PROG2,
		UI_BUFFER,
		UI_END,
	}EUI_Image;


	
	typedef enum eURL
	{
		eURL_LUNCHER=0,
		eURL_HOME	=eURL_LUNCHER+1,
		eURL_FORUM	=eURL_HOME	 +1,

		eURL_SIZE
	}eURL;

	typedef	std::map< UINT, CHBitmap* >	CONT_IMG;
	typedef	std::map< UINT, CCTButton* > CONT_BUTTON;
	
	typedef std::map<size_t, std::wstring> CONT_URL;
	typedef std::map<size_t, std::wstring> CONT_MSG;
	typedef std::vector<std::wstring> CONT_DEFAULT_MSG;
//-----------------------------------------------------------------------------
//  인라인
//-----------------------------------------------------------------------------
public:
	bool			GetIsAuto()      const { return m_bIsAuto;		}
	bool			GetIsRecover()   const { return m_bIsRecover;	}
	bool			GetIsListCheck() const { return m_bIsListCheck; }
	bool			GetIsGameGard()  const { return m_UseGameGard;	}
	WORD			GetPort()		 const { return m_wPort;		}
	ELOCAL_SET		GetLocal()		 const { return m_eLocal;		}
	std::wstring	GetAddr()		 const { return m_wstrAddr;		}
	std::wstring	GetID()			 const { return m_wstrID;		}
	std::wstring	GetPW()			 const { return m_wstrPW;		}
	std::wstring	GetURL()		 const { return m_wstrUrl;		}
	std::vector<char> GetCRC()		 const { return m_vecCRC;		}
	size_t			GetTotalSize()	 const { return m_TotalSize;	}
	size_t			GetFileSize()	 const { return m_FileSize;		}
	eProcess_State	GetProcess()	 const { return m_eProcess;		}
	bool			GetNotPatched()	 const { return m_bNotPatched;	}
	Window const*	GetPopPatchDlg() const;
	bool			CheckEnd()		 const { return m_bEndThread;	}
	bool			IsDestroying()	 const { return m_bIsDestroying;	}
	bool			GetUsePatcherPatch() const { return m_bUsePatcherPatch; }
	bool			GetUseNewPatchMethod() const { return m_bUseNewPatchMethod; }
	bool			GetUseLibUpdate() const { return m_bUseLibUpdate; }

	__int64	GetIsVersion() const { return m_iVersion; }
	void SetIsVersion(__int64 const iVer)	{ m_iVersion = iVer; }

	__int64	GetIsStartVersion() const { return m_iStartVersion; }
	void SetIsStartVersion(__int64 const iVer)	{ m_iStartVersion = iVer; }
	
	// 최초 실행시 읽어 들인 Patch.ID 버전
	void SetIsOldVersion(__int64 const iVer)	{ m_iLoadTimeVersion = iVer; }
	__int64	GetIsOldVersion() const				{ return m_iLoadTimeVersion; }	
	void SetMinimumVersion(__int64 const iVer)	{ m_iMinimumVersion = iVer; }
	__int64	GetMinimumVersion() const			{ return m_iMinimumVersion; }	

	void SetIsClientVer(std::wstring const& Ver) { m_wstrClientVersion = Ver; }

	void SetProcessState(EProcess_State State)	{ m_eProcess = State; }
	void SetIsRecover(bool const bIsRecover)	{ m_bIsRecover = bIsRecover; }
	void SetTotalPer(float Per)	{ m_fTotalPercent = Per; }	
	void SetTotalSize(size_t size);
	void SetFileSize(size_t size);
	void SetTotalCnt(size_t size);
	void SetFileCnt(size_t size);
	void ThreadClear();
	void SetNotPatched(bool bPatched) { m_bNotPatched = bPatched; }
	void SetEnd(bool bEnd) { m_bEndThread = bEnd; }
	void SetDestroying(bool bDestroying) { m_bIsDestroying= bDestroying; }

//-----------------------------------------------------------------------------
//  함수
//-----------------------------------------------------------------------------
public:
	CPatchProcess(void);
	virtual ~CPatchProcess(void);
	void Init(const std::wstring& CmdLine);
	bool SubInit();
	void SubDisplay(bool bDisplay);
	void SubDestroy();
	bool SubWindowVisible() const;
	HWND SubWindowHandle() const;
	HWND GetVisibleWindowHandle(void) const;
	bool OptionCommand(WPARAM wParam, LPARAM lParam);
	bool TrackCommand(WPARAM wParam, LPARAM lParam);
	void SetWorkMsg(const std::wstring& OutText);
	void InitVariable();

	bool DrawDlgFrame(HDC hDC);
	void OwnerDraw(LPDRAWITEMSTRUCT lpDIS);
	void CheckMouseOver(int iX, int iY);
	void SubCheckMouseOver(int iX, int iY);
	void Command(WPARAM wParam, LPARAM lParam);
	void Paint(HDC hDC);
	void SubPaint(HDC hDC);
	bool StartGame();
	wchar_t const * const GetMsg(EMsg const& eMsgID);
	
	std::wstring const& GetSubAddress() { return m_wstrSubAddress; }
	bool UpdateClient();
	void LoadResDat();
	void SetReloadResDat(bool const bReload) { m_bReloadResDat = bReload; }
	bool IsReloadResDat() const { return m_bReloadResDat; }
	bool FullChecking();

	CTEdit* GetEdit( HWND ControlWnd );
	CTEdit* GetNextEdit( HWND ControlWnd );
	SEditInfo const& GetEditInfo( HWND ControlWnd );
	int const GetButtonID( int const Type );
	bool LoadRes(std::wstring filename, std::vector<char> &kData);
	void Exit();
	enum EOperatedError
	{
		Ok,
		PatcherExist,
		ClientExist,
	};
	EOperatedError OperatedCheck();

private:
	ELOCAL_SET ReadiniLocal();
	bool CmdLineParse(const std::wstring& CmdLine);

	void ConnectInit();
	bool InitOption();	
	bool MakeExeCRC(const std::wstring& FileName);
	void ControlUpdate();

	bool ButtonInit(int BtnID, int iType, int iX, int iY, 
					   int iShapeCnt, int iInitShape, 
					   std::wstring const& wstrText, std::wstring const& kLink,
					   std::vector<char> const& kData);

	bool BitmapLoadAndSetPos(UINT Type, POINT const& kPos, std::vector<char> const& kData);
	bool BitmapParse(std::wstring const& FileName, CHBitmap& BitmapInfo);
	bool SetWindowFrame();
	UINT	GetLocalImageNumber(bool bSubType = false);
	std::wstring GetSizeState(size_t const& FileSize);	
	
	bool CheckMsgLanguageType(std::wstring& kLanguage);
	bool LoadMsgFromINI(std::wstring& kLanguage);
	void DrawProgress(HDC hDC, CHBitmap const& kImg, float const fPercent, SPrograssBarInfo const& ProgInfo );	
	void ChangeLanguage(const char* nation_name, const char* service_region, const char* language, ELangChangeMessage eMsg = ELCM_YES);
	void SetLocale(std::wstring kNationName);

	//std::wstring GetUrl(eURL const eUrl) const;

	//-------------------------------------------------------	
	//bool TempBitmapLoad(UINT Type, std::vector<char> const& kData, std::wstring kTempFullPath);
	//bool TempButtonInit(int BtnID, int iX, int iY, int iShapeCnt, int iInitShape, std::wstring const& wstrText, std::wstring kTempFullPath);
	
private:
	//	프로세스 설정
	eProcess_State	m_eProcess;
	ELOCAL_SET		m_eLocal;	
	//	옵션
	bool 		m_bIsAuto;
	bool 		m_bIsRecover;
	bool 		m_bIsListCheck;
	bool 		m_UseGameGard;
	bool		m_bUsePatcherPatch;
	bool		m_bUseNewPatchMethod;
	bool		m_bUseLibUpdate;
	//	핸들
	HANDLE			m_PackThread;
	HWND			m_hWndOpDlg;
	//	이미지
	CONT_IMG		m_kContImg;
	CONT_BUTTON		m_kContBtn;
	//	서버 정보
	std::wstring	m_wstrAddr;
	WORD			m_wPort;
	std::wstring	m_wstrID;
	std::wstring	m_wstrPW;
	std::wstring	m_wstrUrl;
	std::vector<char> m_vecCRC;
	std::wstring	m_wstrClientVersion;
	std::wstring	m_wstrSubAddress;
	// ini로 부터 읽어들이는 사이트 주소
	//CONT_URL m_ContUrl;
	//	버젼정보
	__int64			m_iVersion;

	//	리커버리 시작 버젼 정보
	__int64			m_iStartVersion;

	//런처 최초 실행시 읽어들인 버전 정보
	__int64			m_iLoadTimeVersion;
	// 지원하는 최소 버전(이 버전보다 작으면 무조건 Recovery Patch
	__int64			m_iMinimumVersion;

	//	출력 정보
	std::wstring	m_wstrTextState;
	float			m_fTotalPercent;
	float			m_fFilePercent;
	size_t			m_TotalSize;
	size_t			m_FileSize;
	size_t			m_CntFileSize;
	size_t			m_CntTotalSize;
	bool			m_bUsePack;
	bool			m_bNotPatched;
	bool			m_bEndThread;
	bool			m_bIsDestroying;
	//	브라우저
	PgAXPlay*	m_pkPlay;
	//	옵션
	CConfigXml*	m_pkConfigXml;	
	//  옵션 정보
	SEnvData	m_kEnviData;
	
	CONT_MSG	m_kContMsg;
	Window*		m_kSubApp;
	CONT_DEFAULT_MSG m_kContDefalutMsg;
	CONT_EDIT_LIST	m_kContEdit;
	
	bool m_bReloadResDat;
};