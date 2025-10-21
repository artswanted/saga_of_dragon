#ifndef WEAPON_XUI_XUI_WND_H
#define WEAPON_XUI_XUI_WND_H

//#define CHECK_XUI_PERFORMANCE

#include "XUI_Stat.h"

#include FT_FREETYPE_H

#define XUI_LOCATION_RECT_CALC_ONTICK
#define MAX_PRIORITY 100

#define CLASS_DECLARATION_S_INVALIDATE(Type, FuncName)	\
protected:		\
	Type m_k##FuncName; \
public:			\
	Type const& FuncName()const{return m_k##FuncName;}	\
	void FuncName(Type const& Value){ if( m_k##FuncName != Value) { SetInvalidate(); } m_k##FuncName = Value;  }

#define CLASS_DECLARATION_INVALIDATE(Type, ValueName, FuncName)	\
protected:		\
	Type ValueName; \
public:			\
	Type const& FuncName()const{return ValueName;}	\
	void FuncName(Type const& Value){ if(ValueName != Value) { SetInvalidate(); }; ValueName = Value;}

#define XUI_LOG(x)	\
	if (m_pLogFunc != NULL && x != NULL) \
	{	\
	m_pLogFunc(x); \
	}

namespace XUI
{
	class CXUI_Config;

	typedef struct tagEventKey
	{
		tagEventKey()
		{
			ulEventType = 0;
			ulValue = 0;
		}

		tagEventKey(unsigned long ulInEventType, unsigned long ulInValue)
		{
			ulEventType = ulInEventType;
			ulValue = ulInValue;
		}

		unsigned long ulEventType;
		unsigned long ulValue;//입력값
		bool operator <(const tagEventKey &rhs)
		{
			if( ulEventType < rhs.ulEventType )	{return true;}
			if( ulEventType > rhs.ulEventType )	{return false;}
			if( ulValue < rhs.ulValue )	{return true;}
			if( ulValue > rhs.ulValue )	{return false;}
			return false;
		}
	}SEventKey;



	//	typedef std::map< int, std::wstring > HOT_KEY_HASH;
	typedef std::map< int, std::wstring > HOT_KEY_HASH;

	typedef std::map< int, DWORD > SCRIPT_TIME_MAP;

	typedef enum eInputEventIndex
	{
		IEI_NONE	= 0,
		IEI_MS_DOWN,//마우스 다운
		IEI_MS_UP,
		IEI_MS_MOVE,

		IEI_KEY_DOWN,//키보드 다운
		IEI_KEY_DOWN_PRESS,//쭉~~~
		IEI_KEY_UP,

		IEI_JS_DOWN,//조이스틱 다운
		IEI_JS_UP,
	}E_INPUT_EVENT_INDEX;

	typedef enum eMouseEventIndex
	{
		MEI_NONE	= 0,

		MEI_X		= 1,
		MEI_Y		= 2,
		MEI_Z		= 3,

		MEI_BTN_0	= 4,
		MEI_BTN_1	= 5,
		MEI_BTN_2	= 6,
		MEI_BTN_3	= 7,
		MEI_BTN_4	= 8,
		MEI_BTN_5	= 9,
		MEI_BTN_6	= 10,
		MEI_BTN_7	= 11,

		MEI_L_BTN	= MEI_BTN_0,
		MEI_R_BTN	= MEI_BTN_1,
		MEI_C_BTN	= MEI_BTN_2,//Center버튼
	}E_MOUSE_EVENT_INDEX;

	typedef enum eStaticTypeIndex
	{
		STI_NONE		= 0,

		STI_BODY		= 1,
		STI_BODY_W		= 2,
		STI_BODY_H		= 3,

		STI_TB			= 4,
		STI_TB_W		= 5,
		STI_TB_H		= 6,

		STI_LR			= 7,
		STI_LR_W		= 8,
		STI_LR_H		= 9,

		STI_CORNER		= 10,
		STI_CORNER_W	= 11,
		STI_CORNER_H	= 12,

	}E_STATIC_TYPE_INDEX;

	typedef enum eXUIType
	{
		E_XUI_NONE = 0,

		E_XUI_ROOT,

		//>폼
		E_XUI_FORM,
		E_XUI_DIALOG,
		//<폼
		//>컨트롤
		E_XUI_BTN, 
		E_XUI_TAB,
		E_XUI_TAB_ELEMENT,
		E_XUI_ANI_BAR, 
		E_XUI_EDIT,
		E_XUI_BUILD,
		E_XUI_ICON, 
		E_XUI_DOCK,
		E_XUI_CURSOR,
		E_XUI_MSG_BOX,
		E_XUI_EDIT_MULTILINE,

		//<컨트롤

		E_XUI_IMG, 

		E_XUI_SCRIPT, //스크립트를 가지는 무형의 객체
		E_XUI_HOTKEY, //핫키를 먹이는 무형개체

		E_XUI_ATTR, //속성을 가지는 그것

		E_XUI_TOOL_TIP,
		E_XUI_WARNING,

		E_XUI_LIST,		//리스트
		E_XUI_LIST2,
		E_XUI_TREE,		//트리
		E_XUI_CHECKBTN,	//체크버튼
		E_XUI_CONFIG,
		E_XUI_CUSTOM, //커스텀 데이터

		E_XUI_STATIC_FORM, //스태틱 폼
		//>>더미
		E_XUI_DUMMY,
		//<<
		E_XUI_LOCAL,	// 로컬 관련

		E_XUI_HOTKEY_MODIFIER,
		E_XUI_UNIQUE_UI_TYPE_TO_ERRNO,

		E_XUI_FULL_FROM,
	}E_XUI_TYPE;

	typedef enum eXUIValue
	{
		EXV_DEFAULT_SCREEN_WIDTH = 1024,
		EXV_DEFAULT_SCREEN_HEIGHT = 768,
	}E_XUI_VALUE;

	typedef enum eXUIFontFlag
	{
		XTF_NONE			= 0,
		XTF_BOLD			= 0x00000001,
		XTF_ITALIC			= 0x00000002,
		XTF_UNDERLINE		= 0x00000004,
		XTF_OUTLINE			= 0x00000008,
		XTF_ALIGN_CENTER	= 0x00000010,
		XTF_ALIGN_RIGHT		= 0x00000020,
		XTF_ALIGN_Y_CENTER	= 0x00000040,
		XTF_FULL_FLAG		= 0xFFFFFFFF,
	}EXUIFontFlag;

	typedef enum EValuetype
	{
		VT_NONE = 0,
		VT_EDIT_TEXT = 1,

		VT_BAR_NOW = 2,
		VT_BAR_MAX = 3,
		VT_BAR_CUR = 4,
		//		VT_BAR_COLOR = 4,

		VT_STATIC_TEXT	= 5,//Static Text
		VT_X			= 6,//Window Position
		VT_Y			= 7,
		VT_T_X			= 8,//Text Posision
		VT_T_Y			= 9,

		VT_ICON_GRP		= 10,
		VT_ICON_KEY		= 11,

		VT_BAR_TEXT		= 12,
		VT_TEXT_COLOR	= 13,	
		VT_NOTICE_TEXT	= 14,
		VT_UV_INDEX		= 15,
		VT_BAR_COUNT	= 16,	
		VT_BAR_START_TIME = 17,
		VT_BAR_FILL_TIME = 18,

		VT_IMG_W			= 20,
		VT_IMG_H			= 21,

		VT_ALPHA			= 22,//AlphaMax(f)
	}eValueType;

	typedef enum eXUIState	//상태
	{
		ST_NONE		= 0,
		ST_STOP			= 1,
		ST_OPENING	= 2,
		ST_ENDING	= 3,
	}E_State;

	typedef enum eAppearEffect	//등장,퇴장효과
	{
		AE_NONE			= 0,
		AE_CUTTON_LEFT	= 1,
		AE_CUTTON_RIGHT = 2,
		AE_START_MENU	= 3,
		AE_RESIZE		= 4,
		AE_W3			= 5,
	}E_AppearEffect;	

	typedef enum eIdleEffect	//지속효과
	{
		IE_NONE			= 0,
		IE_VIBRATE		= 1,
		IE_RESIZE		= 2,
		IE_W3			= 3,
	}E_IdleEffect;	

	typedef struct tagButtonColorType
	{
		tagButtonColorType()
		{
			::memset( dwColor, 0, sizeof(dwColor) );
		}

		tagButtonColorType( DWORD const dwDefalutColor )
		{
			for ( int i = 0 ; i<BUTTON_STATE_NUM; ++i )
			{
				dwColor[i] = dwDefalutColor;
			}
		}

		bool Set( size_t const iIndex, DWORD _dwColor )
		{
			if ( iIndex < BUTTON_STATE_NUM )
			{
				dwColor[iIndex] = _dwColor;
				return true;
			}
			return false;
		}

		DWORD Get( size_t const iIndex )const
		{
			if ( iIndex < BUTTON_STATE_NUM )
			{
				return dwColor[iIndex];
			}
			return 0xFFFFFFFF;
		}

		DWORD	dwColor[BUTTON_STATE_NUM];
	}SButtonColorType;
	typedef std::vector<SButtonColorType>		VEC_BUTTON_COLOR;

	class CXUI_Wnd;

	typedef bool (CALLBACK *LP_SCRIPT_FUNC)( std::string const& rScript, XUI::CXUI_Wnd* pWnd);//아래는 이벤트가 일어날때 불러지는 콜백 함수 디자인.
	typedef bool (CALLBACK *LP_CURSOR_CALLBACK)(CXUI_Wnd *pkCursor, POINT2 const& kPtPoint, SIconInfo const& kIconInfo,  eInputEventIndex const kBtn1State, eInputEventIndex const kBtn2State);//아래는 이벤트가 일어날때 불러지는 콜백 함수 디자인.
	typedef void (CALLBACK *LP_LOG_FUNC)(const wchar_t* pcOut);
	typedef bool (CALLBACK *LP_TEXT_CLICK_CALLBACK)(XUI::CXUI_Wnd* pWnd,int iButton,std::wstring const &kOriginalString,PgExtraDataPack const &kExtraDataPack,bool bIsDoubleClick);
	typedef DWORD (CALLBACK *LP_HOTKEY_TO_REALKEY_CHANGER)(DWORD const& dwHotKey);
	typedef void (CALLBACK *LP_UNIQUE_EXIST_FAILED_NOTICE)(std::wstring const& kType);

	class CXUI_Wnd//윈도우는 하나의 이미지를 쥐고 있고. 이미지는 UV까지는 가능하지만 크기 변경이 되진 않는다. 
	{	//	노말 0 
		//  마우스 오버 1
		//	버튼 다운 2 
		// 위의 순서로 그림을 그리도록.
	public:
		CXUI_Wnd(void);
		virtual ~CXUI_Wnd(void);

		typedef std::map< std::wstring, std::wstring > SCRIPT_HASH;
		typedef std::wstring ID_TYPE;
		typedef std::list< CXUI_Wnd* > CONT_CONTROL;
		typedef std::list< CXUI_Wnd* > WND_LIST;
		typedef std::list< std::wstring >	CONT_ALIVE_UNIQUE_EXIST_UI;

		void operator = ( const CXUI_Wnd &rhs);
	public:
		virtual POINT2 const& Size()const{return m_Size;}
		virtual CXUI_Wnd* VCreate()const = 0;

		virtual E_XUI_TYPE VType(){return E_XUI_NONE;}

		virtual void Close();//닫을때는 이걸 이용.

		virtual void VBuildResource(){}
		virtual bool VDisplay();
		virtual bool VDisplayOverlayImg(SRenderInfo& rkDefRenderInfo) { return true; }
		virtual bool VDisplayEnd();
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);//이벤트를 처리 했다면 true 리턴
		bool VPeekEvent_Default(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);//Wnd기본 메시지 처리
		virtual void VRefresh();

		virtual bool VRegistChild( CXUI_Wnd *pWnd );
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);

		virtual void VScale(float const fScale);
		virtual void VAlign();
		virtual void VInit();
		virtual bool VOnTick( DWORD const dwCurTime );
		virtual void VOnLocationChange();
		virtual void VOnClose();
		virtual void VOnCall();

		virtual CXUI_Wnd* VAcquireFocus(CXUI_Wnd* pkWnd);	// 이전 포커스를 반환하자
		virtual void VLoseFocus(bool const bUpToParent = false);

		virtual void UpWheal()	{}	//구현 없음(휠 업)
		virtual void DownWheal()	{}	//구현 없음(휠 다운)

		void ClearOutside();
		bool HaveOutside()const;
	public:
#ifdef XUI_LOCATION_RECT_CALC_ONTICK
		CLASS_DECLARATION_S(POINT3I, TotalLocation);
		POINT3I GetTotalLocation();
#else
		POINT3I TotalLocation();
#endif

		virtual	void Location(POINT2 const& rPT, bool const bIsModifyBoundingBox = true);
		virtual void Location(POINT3I const& rPT, bool const bIsModifyBoundingBox = true);
		virtual void Location(int const x, int const y, bool const bIsModifyBoundingBox = true);
		virtual void Location(int const x, int const y, int const z, bool const bIsModifyBoundingBox = true);

		virtual bool PickWnd(POINT2 const& kptPos, CXUI_Wnd *&pkOutWnd);

		void GetParentRect(RECT &rkOutRc);
		bool GetParentDrawRect(RECT &rkOutRc);
#ifdef XUI_LOCATION_RECT_CALC_ONTICK
		void CalcParentRect(RECT &rkOutRc);
		bool CalcParentDrawRect(RECT &rkOutRc);
#endif

		bool GetClip();// 최상위 부모까지 탐색하며 CanClip속성을 가져온다.

		virtual void Size(POINT2 const& rPT, bool const bIsModifyBoundingBox = true );
		void Size( int const width, int const height, bool const bIsModifyBoundingBox = true );//바운드 박스까지 셋팅	

		int  Width()const { return Size().x; }
		int  Height()const { return Size().y; }


		virtual	void SetBoundingBox(POINT2 const& ptLocation, POINT2 const& ptSize);

		unsigned int GetContCount();	//	m_contControls에 들어있는 자식 윈도우 갯수를 리턴한다.
		CXUI_Wnd*	GetContAt(int iIndex);	//	m_contControls 의 iIndex 번째 윈도우를 리턴한다.

		bool ContainsPoint(POINT2 const& rPT)const;
		void RegistScript( SCRIPT_HASH::key_type const& rKey, SCRIPT_HASH::mapped_type const& rScript );
		//		void RegistHotKey( const HOT_KEY_HASH::key_type &rKey, const HOT_KEY_HASH::mapped_type &rScript );

		bool DoScript( unsigned int const scriptId );
		bool DoHotKey( SCRIPT_HASH::key_type const& rScriptKey );

		virtual bool IsFocus()const	{ return false; }

		bool SetColor(DWORD dwColor);
		DWORD GetColor(DWORD dwColor)	{ return m_dwColor; }
		CLASS_DECLARATION(bool, m_bColorChange, ColorChange);
		CLASS_DECLARATION(int, m_iColorType, ColorType);
		CLASS_DECLARATION_S(bool, Invalidate);
		void SetInvalidate(bool bInvalidateParent = true);
		virtual void SetColorSet();	//XUI에 디폴트로 지정된 셋팅 값들을 로딩

		CLASS_DECLARATION_INVALIDATE(bool,m_bGrayScale,GrayScale);

		void SetState( const E_State eState );
		E_State GetState()	{	return m_eState; }

		void UVUpdate( size_t const iIndex );

		CXUI_Wnd* GetDragableWnd();	//가장 가까운 부모중 드래그 가능한 wnd를 반환. 최상위 까지 간다는 보장 없음
		void Drag(CXUI_Wnd* pkWnd, POINT3I const& rPT);

		void SortChildGreater();

		void RemoveControl( ID_TYPE const& ID );

		virtual bool const& Visible()const{return m_bVisible;}	
		virtual void Visible(bool const& Value);

		virtual	bool	CheckVisible();

		void CursorToThis();//커서를 이동.
		CLASS_DECLARATION_S(bool, HideOtherWnd);
		CLASS_DECLARATION_S(bool, NotHideWnd);
		CLASS_DECLARATION_S(bool, NeedCorrectionLocation);
		//virtual int HideAllUI();
		//virtual int ShowAllUI();
		void ResetModifier();

	private:
		static bool		m_bDefaultResolution;// 기본해상도(1024*768)인가?
		static POINT2	m_sptResolutionSize;
		static POINT2	m_sptGab; // 해상도 변경에 따른 UI 보정할 간격

	public:
		static void					SetIsDefaultResolution( const bool bDefaultResolution )	{ m_bDefaultResolution = bDefaultResolution; }
		static const bool			GetIsDefaultResolution()								{ return m_bDefaultResolution; }
		static void					SetResolutionSize( const POINT2& sptResolutionSize )	{ m_sptResolutionSize.x = sptResolutionSize.x; m_sptResolutionSize.y = sptResolutionSize.y; }
		static const POINT2&		GetResolutionSize()										{ return m_sptResolutionSize; }
		static void					SetGab( const POINT2& sptGab )							{ m_sptGab = sptGab; }
		static const POINT2&		GetGab()												{ return m_sptGab; }


	protected://실시간 변동값.

		CLASS_DECLARATION_NO_SET( bool, m_bIsMouseDown, IsMouseDown );
		virtual	void IsMouseDown(bool const& value)	{	m_bIsMouseDown = value;	}

		bool AddControl( CXUI_Wnd* pControl );
		void Close(bool const bDelUniqueUIType);

		void DisplayControl();
		void DisplayEffect();
		void PeekEventControl(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);

		CXUI_Wnd* GetControl( ID_TYPE const& ID );
		CXUI_Wnd* GetControl( ID_TYPE const& ID, E_XUI_TYPE const nControlType );
		CXUI_Wnd* GetControlAtPoint( POINT2 const& pt );
		bool GetControlList_ByType(E_XUI_TYPE const nControlType, std::vector<CXUI_Wnd*> &rkVec);

		virtual void RemoveAllControls(bool const bIsClosed = false);

		virtual bool SetEditFocus(bool const bIsJustFocus);
		virtual void InvalidateChild();

		DWORD GetStateColor()const;

		virtual CXUI_Wnd* VClone();
		CXUI_Wnd* DeepCopy();

		bool IncAlpha();

		void RenderText(POINT2 const& pt, std::wstring const& wstrText, std::wstring const& wstrFont, DWORD const dwTextFlag, bool bDrawOffscreen = false);//프리셋 마다 함수를 갈라야 됨.
		void RenderText(POINT2 const& pt, CXUI_Style_String const& kText, DWORD const dwTextFlag, bool bDrawOffscreen = false);//프리셋 마다 함수를 갈라야 됨.
		CXUI_Wnd* OffscreenOwner();
		CXUI_Wnd* GetOffscreenOwner();
		void* GetParentOffscreen();
		void CalcOffscreenLocation();
		POINT3I OffscreenLocation();

		static bool CheckExistUniqueLiveType(std::wstring const& UniqueExistType);
		static void DeleteExistUniqueListType(std::wstring const& UniqueExistType);
		static bool IsExistUniqueLiveType(std::wstring const& UniqueExistType);

		virtual	bool	CheckTextClick(int iButton,POINT2 const &pt,bool bIsDoubleClick);

	private:
		void DisplayToolTip();
		void UpdateTwinkle( DWORD const dwCurTime );

	protected:
		CLASS_DECLARATION_NO_SET( POINT3I, m_Location, Location );
		//CLASS_DECLARATION_NO_SET( POINT3I, m_OffscreenLocation, OffscreenLocation );
		//CLASS_DECLARATION_NO_SET( POINT2, m_Size, Size );
		POINT2 m_Size;

		CLASS_DECLARATION( POINT2, m_TextPos, TextPos );
		CLASS_DECLARATION_NO_SET( std::wstring, m_Font, Font );
		CLASS_DECLARATION(std::wstring, m_ToolTip, ToolTip);
		CLASS_DECLARATION_NO_SET( DWORD, m_dwFontFlag, FontFlag );
		CLASS_DECLARATION_NO_SET( DWORD, m_dwFontColor, FontColor );
		CLASS_DECLARATION( DWORD, m_dwOutLineColor, OutLineColor );

		CLASS_DECLARATION( ID_TYPE, m_ID, ID );
		CLASS_DECLARATION( bool, m_bCanDrag, CanDrag );
		CLASS_DECLARATION( bool, m_bCanClip, CanClip );
		CLASS_DECLARATION( bool, m_bCanColorChange, CanColorChange );
		CLASS_DECLARATION( bool, m_bEnable, Enable );//사용 가능하냐.
		bool m_bVisible;//보이냐.	

		CLASS_DECLARATION_S( POINT2, ImgPos );
		CLASS_DECLARATION_INVALIDATE( POINT2, m_ImgSize, ImgSize );
		CLASS_DECLARATION_INVALIDATE( SUVInfo, m_UVInfo, UVInfo );//UV 정보
		CLASS_DECLARATION_NO_SET( std::wstring, m_DefaultImgName, DefaultImgName);
		CLASS_DECLARATION_PTR( void*, m_pDefaultImg, DefaultImg );
		CLASS_DECLARATION_PTR( void*, m_pOffscreen, Offscreen );
		CLASS_DECLARATION_PTR( void*, m_pNewOffscreen, NewOffscreen );
		CLASS_DECLARATION( bool, m_bUseNewOffscreen, UseNewOffscreen);
		CLASS_DECLARATION( bool, m_bUseOffscreen, UseOffscreen);
		CLASS_DECLARATION( bool, m_bUseParentOffscreen, UseParentOffscreen);
		CLASS_DECLARATION_INVALIDATE( int, m_ImgIdx, ImgIdx );//-1 이 초기값이므로 DWORD 로 쓰지말것.

		CLASS_DECLARATION( int, m_Align_x, AlignX );//정렬
		CLASS_DECLARATION( int, m_Align_y, AlignY );//정렬

		CLASS_DECLARATION( bool, m_bAbsoluteX, IsAbsoluteX );
		CLASS_DECLARATION( bool, m_bAbsoluteY, IsAbsoluteY );

		CLASS_DECLARATION_PTR( CXUI_Wnd*, m_pParent, Parent );//Parent

		CLASS_DECLARATION( POINT3I, m_ptLastMouseDown, LastMouseDownPos );//Parent

		CLASS_DECLARATION( E_AppearEffect, m_AppearEffect, AppearEffect );	//등장효과 by 하승봉
		CLASS_DECLARATION( E_IdleEffect, m_IdleEffect, IdleEffect );		//지속효과 by 하승봉
		CLASS_DECLARATION( unsigned char, m_ucSpeed, EffectSpeed );	// 이펙트의 속도
		CLASS_DECLARATION( POINT3I, m_ptTargetPos, TargetPos );

		CLASS_DECLARATION( bool, m_bDBLClick, DBLClick );					//상위클래스에서 더블클릭을 하위로 알려주기 위해
		CLASS_DECLARATION_S( int, BuildIndex );

		void SetCustomData(BM::Stream const& kData);
		void SetCustomData(std::wstring const& kData);
		void SetCustomData(std::vector< char > const& kData);
		void SetCustomData(void const* pData, size_t const size);

		void GetCustomData(BM::Stream& kData)const;
		void GetCustomData(std::wstring& kData)const;
		bool GetCustomData(std::vector< char > &kData)const;
		bool GetCustomData(void *pData, size_t const size)const;

		void ClearCustomData(){m_kCustomData.clear();}
		size_t GetCustomDataSize()const;
		void OffTwinkle();	//마우스나 키보드에 의한 하이라이트 취소

		CLASS_DECLARATION_S( int, Priority );//우선순위. 기본적으로 0. 숫자가 높을수록 위에 그림
		//		CLASS_DECLARATION_S( bool, IsMouseOver );
		bool IsMouseOver()const;

		CLASS_DECLARATION_NO_SET(std::wstring, m_Text, Text);
		CLASS_DECLARATION_S(CXUI_Style_String, StyleText);
		CLASS_DECLARATION_S(POINT2, TextRect);

		CLASS_DECLARATION_S(bool, PassEvent);								//인풋을 XUI에서 먹지 않고 게임으로 돌려줄건지
		CLASS_DECLARATION_S(bool, IsDoNotLostFocus);

		CLASS_DECLARATION_S(std::wstring, CallSoundID);
		CLASS_DECLARATION_S(std::wstring, CloseSoundID);

		CLASS_DECLARATION_S(DWORD, EventRepeateDelay );						//인풋이 연속적으로 들오지 않게 하기 위해서

		DWORD m_dwColor;
		DWORD m_dwPastColor;
		E_State m_eState;

		bool m_bConvRealKey;//옵션에 따라 핫키를 리얼키로 바꾸기 위해

		SCRIPT_TIME_MAP m_kScriptTimeMap;
		//static void DoScriptFocused();
	protected:
		//		HOT_KEY_HASH m_kHotKeyHash;
		SCRIPT_HASH m_mapScript;
		bool m_bScriptArray[SCRIPT_MAX_NUM];
		std::wstring m_scriptArray[SCRIPT_MAX_NUM];
		std::list<std::pair<void*,bool>> m_kOffscreenDrawList;
		RECT m_rcBoundingBox;//충돌처리 영역. SetSize 로 셋팅
		//WND_LIST m_HideList;	//하이드 시킬 윈도우 리스트

		CONT_CONTROL m_contControls;
		static void CorrectionScreenPoint( POINT3I &ptOrg, POINT3I const& ptAdd );

		//static POINT2 m_sptSrcSize;//스크린 사이즈
		static POINT3I m_sMousePos;//마우스 위치
		static CXUI_Wnd* m_spWndMouseOver;
		static CXUI_Wnd* m_spWndMouseFocus;
		static HWND m_sHwnd;
		static CXUI_Wnd* m_spWndScrollBtn;

		static CXUI_Resource_Manager_Base*	m_spRscMgr;
		static CXUI_Renderer_Base*			m_spRenderer;
		static CXUI_MediaPlayer_Base*		m_spMediaPlayer;
		static CXUI_TextTable_Base*			m_spTextTable;
		static LP_SCRIPT_FUNC				m_pScriptFunc;//스크립트 엔진과 연동될 함수.
		static LP_CURSOR_CALLBACK			m_pCursorCallBackFunc;
		static LP_TEXT_CLICK_CALLBACK		m_pTextClickCallBackFunc;
		static LP_LOG_FUNC					m_pLogFunc;
		static CXUI_HotKeyModifier*			m_spHotKeyModifier;
		static LP_HOTKEY_TO_REALKEY_CHANGER m_spHotKeyToRealKey;
		static CXUI_Config* m_spConfig;
		static bool m_bIsOverWnd;
		static CONT_ALIVE_UNIQUE_EXIST_UI	m_kContAliveUniqueExistUI;
		static LP_UNIQUE_EXIST_FAILED_NOTICE m_pUniqueExistCheckNotice;


#ifdef CHECK_XUI_PERFORMANCE
		static XUIStatGroup m_kXUIStatGroup;
#endif
		DWORD GetLastTickTime()const{return m_dwLastTickTime;}
	private:
		DWORD	m_dwLastTickTime;

	public:
		virtual	void Text(std::wstring const& szString);
		void FontColor(DWORD const dwNewColor);
		void FontFlag(DWORD const dwNewFlag);
		void Font(std::wstring const& rkNewFaceName);

		XUI::CXUI_2DString *m_pText;	//	leesg213

	protected:
		bool m_bUpdatedText;
		virtual	void ReloadText();
		virtual bool CheckEventRepeate(int const iEvent);

	protected:
		//워드랩하기 위해 freetype 라이브러리를 사용.
		//		static FT_Face		m_sFontFace;
		//		static FT_Library	m_sftlib;
		//		int m_iFontHeight;//윈도우마다 폰트 크기는 다름.

		CLASS_DECLARATION_S( BM::GUID, OwnerGuid );
		CLASS_DECLARATION_S( int, OwnerState );
		CLASS_DECLARATION_S( DWORD, CalledTime );
		CLASS_DECLARATION_S( DWORD, AliveTime );
		CLASS_DECLARATION_S( DWORD, TickInterval )
			CLASS_DECLARATION_S( bool, IsAliveTimeIgnoreMouseOver );
		CLASS_DECLARATION_S_INVALIDATE( float, Alpha );
		CLASS_DECLARATION_S( float, AlphaMax );
		CLASS_DECLARATION_S( bool, UseDrawRect );
		CLASS_DECLARATION_S( bool, IsModal );
		CLASS_DECLARATION_S( bool, IsUseAlphaAni );
		CLASS_DECLARATION_S( bool, IsIgnoreParentRect );
		CLASS_DECLARATION_S( int, BtnType );
		CLASS_DECLARATION_S( bool, NoWordWrap );
		CLASS_DECLARATION_S(bool, InvertBold);
		CLASS_DECLARATION_S(RECT, ParentRect);
		CLASS_DECLARATION_S(RECT, ParentDrawRect);
		CLASS_DECLARATION_S( bool, NoFocus );
		CLASS_DECLARATION_S(bool, TypingEffect);
		CLASS_DECLARATION_S( DWORD, LastUsedTime);
		CLASS_DECLARATION_NO_SET( bool, m_bIsTwinkle, IsTwinkle);
		CLASS_DECLARATION_S_INVALIDATE( bool, NowTwinkleOn);			//현재 깜빡임이 켜졌는지
		CLASS_DECLARATION_S( DWORD, TwinkleStartTime);		//깜빡임 시작 시간
		CLASS_DECLARATION_NO_SET( DWORD, m_dwTwinkleTime, TwinkleTime);			//깜빡임 지속 시간
		CLASS_DECLARATION_NO_SET( DWORD, m_dwTwinkleInterTime, TwinkleInterTime);		//깜빡임 시간 간격
		CLASS_DECLARATION_S( bool, ReservTwinkleOff);
		CLASS_DECLARATION_S( bool, IgnoreTwinkle);			//깜빡임 무시

		CLASS_DECLARATION_S( int, WheelValue);

		CLASS_DECLARATION_S( float, Scale );
		CLASS_DECLARATION_S(POINT2F, ScaleCenter);

		CLASS_DECLARATION( float, m_fRotationDeg, RotationDeg );
		CLASS_DECLARATION_S(POINT2F, RotationCenter);

		CLASS_DECLARATION_S(bool, UseInputCheck);
		CLASS_DECLARATION_S(std::wstring, UniqueExistType );

		CLASS_DECLARATION_S(bool, IsOutsideWnd);	//외부폼인경우
		CLASS_DECLARATION_S(bool, IsRemoveOutside);	//Close시 외부폼 지우기

		void TypingEffectReset();

	public:
		void SetTwinkle(bool const bValue);
		void TwinkleTime(DWORD const dwTime);
		void TwinkleInterTime(DWORD const dwTime);
		void RefreshCalledTime();
		void RefreshLastTickTime();

		void ReleaseDefaultImg();
		void ReleaseOffscreen();
		void DefaultImgName(std::wstring const& wstrImgName);
		void DefaultImgTexture(void *pkTex);

		POINT2 AdjustText(std::wstring const& wstrFont, std::wstring const& kSrc, CXUI_Style_String &kOut,int iCropWidth=-1);

		CLASS_DECLARATION_S( bool, IsClosed );
	protected:
		std::vector< char > m_kCustomData;
	};

	class DisplayControl_Func
	{
	public:
		DisplayControl_Func(){}

		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1)->VDisplay();
		}
	};

	class InitControl_Func
	{
	public:
		InitControl_Func(){}

		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1)->VInit();
		}
	};

	class OnCallControl_Func
	{
	public:
		OnCallControl_Func(){}

		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1)->VOnCall();
		}
	};

	class ScaleControl_Func
	{
	public:
		ScaleControl_Func(float const fScale):m_fScale(fScale){}

		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1)->VScale(m_fScale);
		}
		float const m_fScale;
	};

	class PeekEventControl_Func
	{
	public:
		PeekEventControl_Func(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
			:m_rET(rET), m_rPT(rPT), m_dwValue(dwValue)
		{}
		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1)->VPeekEvent(m_rET, m_rPT, m_dwValue);
		}
		E_INPUT_EVENT_INDEX const& m_rET;
		POINT3I const& m_rPT;
		DWORD const& m_dwValue;
	};

	class RefreshControl_Func
	{
	public:
		RefreshControl_Func(){}

		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1)->VRefresh();
		}
	};

	class BoundingBoxControl_Func
	{
	public:
		BoundingBoxControl_Func(){}

		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1)->SetBoundingBox( (t1)->Location(), (t1)->Size() );
		}
	};

	class OnCloseControl_Func
	{
	public:
		OnCloseControl_Func(){}

		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1)->VOnClose();
		}
	};

	class SafeDeleterControl_Func
	{
	public :
		SafeDeleterControl_Func(){}
		template< typename T1 >
		void operator ()(T1 * &t1)
		{
			SAFE_DELETE(t1);
		}
	};

	class RemoveControl_Func
	{
	private:
		bool m_bIsClosed;
	public :
		RemoveControl_Func(bool const bIsClosed = false) : m_bIsClosed(bIsClosed) {}
		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1)->IsClosed(m_bIsClosed);
			(t1)->RemoveAllControls(m_bIsClosed);
		}
	};

	class SetColorSet_Func
	{
	public :
		SetColorSet_Func(){}
		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1)->SetColorSet();
		}
	};

	class TwinkleControl_Func
	{
	public:
		TwinkleControl_Func(bool const bValue):m_bTwinkle(bValue){}

		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1)->SetTwinkle(m_bTwinkle);
		}
		bool const m_bTwinkle;
	};

	class TwinkleTimeControl_Func
	{
	public:
		TwinkleTimeControl_Func(DWORD const dwValue):m_dwTwinkleT(dwValue){}

		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1)->TwinkleTime(m_dwTwinkleT);
		}
		DWORD const m_dwTwinkleT;
	};

	class TwinkleInterTimeControl_Func
	{
	public:
		TwinkleInterTimeControl_Func(DWORD const dwValue):m_dwTwinkleT(dwValue){}

		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1)->TwinkleInterTime(m_dwTwinkleT);
		}
		DWORD const m_dwTwinkleT;
	};

	class VAlignControl_Func
	{
	public:
		VAlignControl_Func(){}

		template< typename T >
		void operator()( T& t )
		{
			(t)->VAlign();
		}
	};

	typedef struct tagListItem
	{
		tagListItem()
		{
			m_iIndex = 0;
			m_wstrKey = _T("");
			m_pWnd = NULL;
			//			m_pData = NULL;
			//			m_dwColor = 0xffffffff;
			//			m_bSelected = false;
			//			pPrev = NULL;
			//			pNext = NULL;
		}

		int			m_iIndex;//리스트에 들어가는 index
		std::wstring m_wstrKey;//구분자.
		CXUI_Wnd*	m_pWnd;
		//		void*			m_pData;
		//		DWORD			m_dwColor;
		//		bool			m_bSelected;

		//		tagListItem *pPrev;
		//		tagListItem *pNext;
	}SListItem;

#define XUI_TT(i)		(m_spTextTable->GetTextW(i))
	//#define XUI_TTSTR(i)	(_ttoi(i.c_str()) ? (XUI_TT(_ttoi(i.c_str()))) : i)

#ifdef XUI_LOCATION_RECT_CALC_ONTICK
	inline bool CXUI_Wnd::GetParentDrawRect(RECT &rkOutRc)
	{
		rkOutRc = ParentDrawRect();
		return true;
	}

	inline void CXUI_Wnd::GetParentRect(RECT &rkOutRc)
	{
		rkOutRc = ParentRect();
	}
#endif
}

#endif // WEAPON_XUI_XUI_WND_H