// CSIME.h: interface for the CCSIME class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
//*
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <imm.h>
#include <tchar.h>

#include <string>
#include <map>
#include <vector>

#include "Loki/Singleton.h"

#define CSIME_LOG(x)	\
	if (m_pLogFunc != NULL && x != NULL) \
	{	\
		m_pLogFunc(x); \
	}

namespace CS
{
	struct SOnlyNumeric
	{
		bool operator()( wchar_t const& w )
		{
			return w < L'0' || w > L'9';
		}
	};

	typedef struct CARETDATA
	{
		__int32 iCaretPos;
		__int32 iSelectStart;
		__int32 iSelectEnd;
		__int32 iImeCursor;
		__int32 iImeDeltaStart;
		CARETDATA()
		{
			iCaretPos = 0;
			iSelectStart = 0;
			iSelectEnd = 0;
			iImeCursor = 0;
			iImeDeltaStart = 0;
		}
	}t_CaretData,*LPCARETDATA;

	class CCandidateData
	{
	public:
		CCandidateData(int const iIndex, std::wstring const& str)
		{
			m_iIndex  = iIndex;
			m_strCadidate = str;
		};
		~CCandidateData(){};

		__int32 GetIndex()const{return m_iIndex;}
		std::wstring const& GetString()const{return m_strCadidate;}
	
	private:
		__int32 m_iIndex;
		std::wstring m_strCadidate;
	};

	class CCSIME
	{
		friend struct ::Loki::CreateStatic< CCSIME >;
	public:
		typedef enum eKeyState
		{
			KEY_STATE_DOWN	= 1,
			KEY_STATE_UP	= 2,
		}EKeyState;

		typedef std::vector< CCandidateData > CANDI_BUF;
		typedef CANDI_BUF::iterator CANDI_ITOR;
		typedef CANDI_BUF::reverse_iterator CANDI_R_ITOR;

		typedef __int32 (CALLBACK *LPKBFUNC)(int const iKey, CCSIME *pIME, eKeyState const KeyState);//키보드가 눌러졌을때 호출 되는 함수
		typedef void (CALLBACK *LP_LOG_FUNC)(wchar_t const* pcOut);
		typedef bool (CALLBACK *LP_ON_IME_CHAR)(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);
		typedef bool (CALLBACK *LP_ON_COMMAND)(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);
		
		typedef std::map< __int32, LPKBFUNC > KB_HASH;

		typedef enum eLANGUAGE_TYPE
		{
			LT_DEFAULT = 0,
			LT_TRADITIONAL_CHINESE,	
			LT_JAPANESE,
			LT_KOREAN,
			LT_SIMPLIFIED_CHINESE
		}ELANGUAGE_TYPE; 

	public:
		CCSIME();
		virtual ~CCSIME();
		bool const Init(HWND const hParentWnd, HINSTANCE const hInstance, short int const nMenuSize, DWORD dwStyle = 0, LP_LOG_FUNC pLogFunc = 0);
	
		bool CALLBACK HookMessage(HWND &hWnd, UINT &message, WPARAM &wParam, LPARAM &lParam, bool const bDontPost, bool& bSkipDefProc);

		bool const IsShowCandidate() const {return m_bIsShowCandidate;}//윈도우가 보여주는 기본창을 볼 수 있게 할 것인가?
		bool const IsSymbolMode() const {return m_bIsSymbolMode;}//입력기 상태중 심볼 입력 모드
		bool const IsFullShape() const {return m_bIsFullShape;}//전자, 반자 모드
		int const IsOnlyNumeric() const {return m_iIsNumeric;}//숫자 입력 모드인가? (1:맨앞에 0불허, 2:맨앞에 0허용)
		bool const IsMultiLine() const {return m_bIsMultiLine;}//엔터 입력을 받는가?(멀티라인을 쓰는가)
		bool const IsNowComp() const {return m_bIsComp;}//지금 조합중인가?

		std::wstring const& GetIMEName() const { return m_strIMEName; }//현재 입력기의 이름을 리턴
		const CARETDATA& GetCaretPos();
		const eLANGUAGE_TYPE GetLanguageType() const{ return m_eLanguageType; }//언어타입을 리턴. eLANGUAGE_TYPE를 참조
		DWORD const GetIME_CMODE();

		DWORD GetLimitLength() const {return m_dwLimitLength;}
		DWORD GetMaxLineCount() const {return m_dwMaxLineCount;}
		
		//문자열 관련
		std::wstring const& GetResultStr();
		std::wstring const& GetCompStr()  const { return m_strCompStr; }//조합의 결과물을 리턴
		std::wstring const& GetCompReadStr() const { return m_strCompReadStr; }//조합중인 상태의 문자를 리턴
		int GetCandidateIndex() const;

		const CANDI_BUF& GetCadidateData() const {return m_vtCandlist;}//Candidate를 리턴
		void SetString(std::wstring const& wstrText = _T("") );//강제로 내용을 셋팅. IsOnlyNumeric인데 숫자가 아닌 문자를 넣으면 ""로 셋팅 됨

		//모드 관련
		void SetShowCandidate(bool const bIsShow) { m_bIsShowCandidate = bIsShow; }
		void SetEnableIME(bool const bIsEnable);
		void SetEnableIME(bool const bIsEnable, HWND hWnd, HIMC &hIMC);//해당 윈도우의 IME 켜기 끄기
		void SetNextIME();//다른 입력기로 전환.
		void SetNativeIME();//자국어 입력기로 전환.
		void SetEnglishIME(bool const bIsForce);//영어 입력기로 전환
		void SetOpenStatus(bool bOpen);//IME창 안보이도록.

		bool GetCaretPosChanged() { return m_bCaretPosChanged; }
		void SetCaretPosChanged(bool bChanged = true) { m_bCaretPosChanged = bChanged; }
		bool GetTextChanged() { return m_bTextChanged; }
		void SetTextChanged(bool bChanged = true) { m_bTextChanged = bChanged; }
		
		void SetDisableEnglish();//영어입력 강제로 셋팅 되었을 경우 풀어줌

		void SetShapeIME();//전/반자 모드 전환
		void SetSymbolIME();//심볼 모드로 전환
		void SetCaretPos(bool const bIsHome);//캐럿을 맨앞 또는 맨뒤로 설정
		void SetCaretPos(__int32 const iStartPos, __int32 const iEndPos );//캐럿의 영역 설정, (iStartPos = 0 , iEndPos = -1 )--> 모든 영역선택 //캐럿은 iEndPos 로 설정
		void SetOnlyNumeric( int const iIsNumeric, bool const bDoClear = true );//숫자 전용 모드로 셋팅
		void SetMultiLine(bool const bIsMultiLine, DWORD const dwMaxCaretPerLine= 1, DWORD const dwMaxLineConut = 1, bool const bDoClear = true);
		void SetLimitLength(DWORD const dwLen, bool const bDoClear = true);//입력받는 최대 길이 셋팅. 
		void SetPasswordMode(bool const bIsPasswordMode);

		void	SetOnImeCharCallBackFunc(LP_ON_IME_CHAR kFunc)	{	m_kOnImeCharCallBackFunc = kFunc;	}
		void	SetOnCommandCallBackFunc(LP_ON_COMMAND kFunc)	{	m_kOnCommandCallBackFunc = kFunc;	}

		//키보드 이벤트 바인딩
		bool FuncBind(__int32 const iIndex, LPKBFUNC const lpFunc);//함수 바인딩

		HWND const GetEditHWND() const {return m_hEWnd;}

		void SetIMEName(const TCHAR* szIMEName) { m_strIMEName = szIMEName; }//내부에서만 사용 
		void SetConversionStatus( bool const bIsOn = true, DWORD const dwNewConvMode = 0, DWORD const dwNewSentMode = 0);//입력기의 모드 전환 
		void SetLanguageType();//입력기의 언어타입 셋팅

		void AllocCandList( DWORD const dwSize );//
		void FreeCandList();

		bool SetIMECandiDateList(HWND const hWnd);//m_lpCL 에 값을 셋팅 한다.
//		bool SetIMEConversionList(HWND const hWnd);//m_lpCL 에 값을 셋팅 한다.

		bool IsCorrectString(std::wstring const& strSrc);
		bool IsCanInputAtMultiLine(std::wstring const& strSrc, int const nVirtKey);

		bool CorrectAlphaNumeric(std::wstring &str);
		
	protected:
		void GetWindowText(std::wstring &str);
		//키보드 이벤트 함수 실행
		__int32 Execute(__int32 const iIndex, eKeyState const KeyState);

		//Edit Wnd 프로시저에서 사용
		bool CALLBACK HandleMessage(HWND &hWnd, UINT &message, WPARAM &wParam, LPARAM &lParam);//윈도우 프로시저 에서 불려지는 콜백

		void Clear();
		void ClearCandidateBuf();//벡터 버퍼를 지움

		LRESULT	CALLBACK OnWM_IME_SETCONTEXT(HWND const& hWnd, UINT const& message, WPARAM &wParam, LPARAM &lParam);
		LRESULT	CALLBACK OnWM_IME_COMPOSITION(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);//조합중인 것.
		LRESULT	CALLBACK OnWM_IME_NOTIFY(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);
		LRESULT	CALLBACK OnWM_INPUTLANGCHANGEREQUEST(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);
		LRESULT	CALLBACK OnWM_INPUTLANGCHANGE(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);
		LRESULT	CALLBACK OnWM_IME_STARTCOMPOSITION(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);
		LRESULT	CALLBACK OnWM_IME_ENDCOMPOSITION(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);
		LRESULT	CALLBACK OnWM_IME_CONTROL(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);
		LRESULT	CALLBACK OnWM_IME_CHAR(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);
		LRESULT	CALLBACK OnWM_IME_SELECT(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);
		LRESULT	CALLBACK OnWM_COMMAND(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);

	protected:
		static LRESULT CALLBACK EditWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static WNDPROC m_oldEditWndProc;

		LPCANDIDATELIST m_lpCL;	//Candidate버퍼 포인터
		CANDI_BUF m_vtCandlist;
		KB_HASH m_mapKBHash;
		LP_ON_IME_CHAR	m_kOnImeCharCallBackFunc;
		LP_ON_COMMAND	m_kOnCommandCallBackFunc;

		bool m_bIsShowCandidate;//Candidate윈도우를 보이게 하는 플래그 
		bool m_bIsSymbolMode;//심볼 모드인지 알아 보는 플래그
		bool m_bIsFullShape;//전/반자 플래그
		int m_iIsNumeric;//숫자전용 모드 플래그
		bool m_bIsMultiLine;//엔터 입력을 받을 것인가?
		bool m_bIsComp;//현재 조합중인가?
		bool m_bIsForceEnglishIME;
		bool m_bCaretPosChanged;
		bool m_bTextChanged;
		bool m_bPasswordMode;

		std::wstring m_strIMEName;//IME 의 이름.
		
		CARETDATA	m_CaretData;//셀렉트된 캐럿의 위치
		
		eLANGUAGE_TYPE m_eLanguageType;
	
		//문자열
		std::wstring m_strCompStr;//조합중인 문자가 저장됨
		std::wstring m_strCompReadStr;//조합을 위해서 읽히고 있는 문자열
		std::wstring m_strResult;//결과물. 에디트 박스에 기록되는 최종 스트링
		DWORD m_dwLimitLength;//최대 입력 길이 생성시에 MAX_PATH로 셋팅
		DWORD m_dwMaxCaretPerLine;//멀티라인시 1줄당 최대 캐럿
		DWORD m_dwMaxLineCount;//멀티라인시 라인카운트

		HWND m_hEWnd;//입력을 받고있는 IME 윈도우의 핸들 //Init 시에 생성.
		HIMC m_hImcSave;//IME Enable/Disable 때문에 있는 IME 핸들 저장용변수

		Loki::Mutex m_kMutex;

		std::vector< TCHAR > m_vecCandListBuf;
		LP_LOG_FUNC m_pLogFunc;
	};
}
