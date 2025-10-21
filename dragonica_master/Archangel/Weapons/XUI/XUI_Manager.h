#ifndef WEAPON_XUI_XUI_MANAGER_H
#define WEAPON_XUI_XUI_MANAGER_H

#include <map>
#include <string>

#include "XUI_Config.h"
#include "XUI_Wnd.h"
#include "XUI_Form.h"
#include "XUI_Dummy.h"
#include "XUI_Dialog.h"
#include "XUI_Button.h"
#include "XUI_Script.h"
#include "XUI_AniBar.h"
#include "XUI_Edit_MultiLine.h"
#include "XUI_Image.h"
#include "XUI_Tab.h"
#include "XUI_Tab_Element.h"
#include "XUI_ICON.h"
#include "XUI_Cursor.h"
#include "XUI_Builder.h"
#include "XUI_ToolTip.h"
#include "XUI_Warning.h"
#include "XUI_List.h"
#include "XUI_List2.h"
#include "XUI_Tree.h"
#include "XUI_CheckButton.h"
#include "XUI_MsgBox.h"
#include "XUI_StaticForm.h"
#include "XUI_Scroll.h"
#include "XUI_HotKeyModifier.h"

#include "BM/ObjectPool.h"
#include "Loki/Singleton.h"
#include "CSIME/CSIME.h"
#include "tinyxml/tinyxml.h"

//GEL 에 종속적임.
namespace XUI 
{
	typedef std::list< CXUI_Wnd::ID_TYPE > ContWndIDList;

	class PgHideStack
	{
	protected:
		typedef std::list< CXUI_Wnd::WND_LIST > ContHideStack;
		typedef std::map< std::wstring, CXUI_Wnd::WND_LIST > ContHidePart;

		ContHideStack m_kHideStack;
		ContHidePart m_kHidePart;

		PgHideStack() {}
		virtual ~PgHideStack() {}

		void PushHide(CXUI_Wnd* pWnd)
		{
			if(!m_kHideStack.empty())
			{
				ContHideStack::reverse_iterator it = m_kHideStack.rbegin();
				ContHideStack::value_type & kStack = (*it);
				kStack.insert(kStack.end(), pWnd);
			}
			else
			{
				CXUI_Wnd::WND_LIST kList;
				kList.push_back(pWnd);
				PushHide(kList);
			}
		}
		void PushHide(CXUI_Wnd::WND_LIST const& rkWndList)
		{
			m_kHideStack.push_front( rkWndList );
		}
		void PopHide(CXUI_Wnd::WND_LIST &rkOut)
		{
			if( m_kHideStack.empty() )
			{
				return;
			}

			rkOut = m_kHideStack.front();
			m_kHideStack.pop_front();
		}
		bool PushHidePart(std::wstring HideKey, CXUI_Wnd::WND_LIST const& rkWndList)
		{
			auto result = m_kHidePart.insert(std::make_pair(HideKey, rkWndList));
			return result.second;
		}
		void PopHidePart(std::wstring HideKey, CXUI_Wnd::WND_LIST &rkOut)
		{
			ContHidePart::iterator iter = m_kHidePart.find(HideKey);
			if( iter != m_kHidePart.end() )
			{
				rkOut.insert(rkOut.end(), iter->second.begin(), iter->second.end());
				m_kHidePart.erase(iter);
			}
		}

		bool EmptyHide() const	{ return m_kHideStack.empty(); }
		bool EmptyHidePart() const	{ return m_kHidePart.empty(); }
	};

	typedef struct tagXUIInitInfo
	{
		tagXUIInitInfo()
		{
			pRscMgr = NULL;
			pRenderer = NULL;
			pMediaPlayer = NULL;
			pHotKeyModifier = NULL;
			pScriptFunc = NULL;//스크립트 엔진과 연동될 함수.
			pCursorCallBackFunc = NULL;//커서 버튼 이벤트 들어올때 호출.
			pHotKeyToRealKey = NULL;
			pUniqueExistCheckNotice = NULL;
			pLogFunc = NULL;
			hWnd = 0;
			hInstance = 0;
		}

		CXUI_Resource_Manager_Base	*pRscMgr;//스프라이트등의 리소스 관리자
		CXUI_Renderer_Base			*pRenderer;//렌더러
		CXUI_MediaPlayer_Base		*pMediaPlayer;
		CXUI_HotKeyModifier			*pHotKeyModifier;
		CXUI_TextTable_Base			*pTextTable;//텍스트 테이블
		LP_SCRIPT_FUNC				pScriptFunc;//스크립트 엔진과 연동될 함수.
		LP_CURSOR_CALLBACK		pCursorCallBackFunc;
		LP_TEXT_CLICK_CALLBACK	pTextClickCallBackFunc;
		LP_HOTKEY_TO_REALKEY_CHANGER	pHotKeyToRealKey;
		LP_UNIQUE_EXIST_FAILED_NOTICE	pUniqueExistCheckNotice;

		LP_LOG_FUNC				pLogFunc;
		HWND hWnd;
		HINSTANCE hInstance;
	}XUI_INIT_INFO;

	typedef struct tagUIXMLData
	{
		typedef struct tagXMLChildData
		{
			tagXMLChildData(std::wstring const& rkstrName, std::wstring const& rkstrValue)
			{
				strName = rkstrName;
				strValue = rkstrValue;
			}
			std::wstring strName;
			std::wstring strValue;
		}SXMLChildData;

		typedef std::list<SXMLChildData> AttrList;
		typedef std::map<std::wstring, std::wstring> ScriptHash;//스크립트
		typedef std::map<std::wstring, tagUIXMLData> ChildHash;

		tagUIXMLData()
		{
			Type(_T(""));
			m_pkParent = NULL;
		}

		void RemoveChild(std::wstring const& rkName)
		{
			m_kChildHash.erase(rkName);
		}

		tagUIXMLData* RegistChild(std::wstring const& rkName, tagUIXMLData const& rkValue)
		{
			tagUIXMLData kTempData = rkValue;

			kTempData.Parent(this);

			ChildHash::iterator kIter = m_kChildHash.find(rkName);
			if(m_kChildHash.end() == kIter)
			{//없는것이다.
				auto ret = m_kChildHash.insert(std::make_pair(rkName, kTempData));
				if(ret.second)
				{
					return &ret.first->second;
				}
				assert(NULL);
			}
			return &kIter->second;
		}

		tagUIXMLData* GetChild(std::wstring const& rkName)
		{
			ChildHash::iterator itor = m_kChildHash.find(rkName);
			if(itor != m_kChildHash.end())
			{
				return &(*itor).second;
			}
			assert(NULL);
			return NULL;
		}

		bool RegistAttr(std::wstring const& rkName, std::wstring const& rkValue)
		{
			tagXMLChildData Data(rkName,rkValue);
			m_kAttrList.push_back(Data);
			return true;
			//			auto ret = m_kAttrList.insert(std::make_pair(rkName, rkValue));
			//			return ret.second;
		}

		bool RegistScript(std::wstring const& rkName, std::wstring const& rkValue)
		{
			auto ret = m_kScriptHash.insert(std::make_pair(rkName, rkValue));
			return ret.second;
		}
		bool RegistGlobalScript(std::wstring const& rkName, std::wstring const& rkValue)
		{
			auto ret = m_kGlobalScriptHash.insert(std::make_pair(rkName, rkValue));
			return ret.second;
		}
		void Parent(tagUIXMLData* pkParent)
		{
			m_pkParent = pkParent;
		}

		tagUIXMLData* Parent()
		{
			return m_pkParent;
		}

		std::wstring const& ID()const
		{
			AttrList::const_iterator itor = m_kAttrList.begin();
			while(itor != m_kAttrList.end())
			{
				if(ATTR_ID == (*itor).strName)
				{
					return (*itor).strValue;
				}
				++itor;
			}
			static std::wstring wstrNULL;
			return wstrNULL;
		}

		void Clear()
		{
			Type(_T(""));
			m_kAttrList.clear();//first = Attr, second = Value
			m_kChildHash.clear();
			m_kScriptHash.clear();

			m_pkParent = NULL;
		}

		CLASS_DECLARATION(std::wstring, m_Type, Type);


		AttrList m_kAttrList;//first = Attr, second = Value
		ChildHash m_kChildHash;
		ScriptHash m_kScriptHash;
		ScriptHash m_kGlobalScriptHash;

		tagUIXMLData *m_pkParent;
	}PgUIXMLData;

	typedef struct tagUniqueUITypeErrInfo
	{
		tagUniqueUITypeErrInfo()
			: iTT_No(0), kErrSoundPath()
		{}

		int iTT_No;
		std::wstring kErrSoundPath;
	}SUniqueUITypeErrInfo;

	class CXUI_Manager
		:	public CXUI_Wnd, protected PgHideStack
	{
		friend class CXUI_Tree;
		friend struct ::Loki::CreateUsingNew< CXUI_Manager >;
		typedef std::map< std::wstring, E_XUI_TYPE > KEY_WORD_HASH;//XML 키워드

		typedef std::map< CXUI_Wnd::ID_TYPE, CXUI_Wnd* > XUI_HASH;
		typedef std::list< ID_TYPE > SAVED_ACTIVEID_LIST;				//재로딩 하기전에 액티브리스트 인덱스만 저장

		typedef std::map< int, DWORD > COLOR_HASH;
		typedef std::map<std::wstring, SUniqueUITypeErrInfo > kUniqueUITypeErrCont;
	private:



	public:

		enum	CallBackEventType
		{
			CBET_NONE=0,
			CBET_ON_CALL,
			CBET_ON_CLOSE
		};

		typedef void (*F_XUI_Manager_CallBackFunc) (CXUI_Wnd *pkWnd,CallBackEventType kEventType); 

	protected:

		F_XUI_Manager_CallBackFunc	m_kXUI_Manager_CallBackFunc;

	protected:
		CXUI_Manager(void);
		virtual ~CXUI_Manager(void);

		virtual CXUI_Wnd* VCreate()const {return NULL;};
	public:
		void ClearXMLData();
		bool InitXUI(const XUI_INIT_INFO &rInfo, bool bInitIme = true);
		bool InitIME(HWND hWnd, HINSTANCE hInstance);
		void Terminate();
		bool ReadXML(std::wstring const& wstrFileName);
		bool ParseXML(TiXmlDocument &rkDoc);
		bool BuildXUI();
		static DWORD GetFrameCount() { return m_FrameCount; }
		static CXUI_Wnd* Create(ID_TYPE const& ID, bool const bIsModal, ID_TYPE const& rkNewID = ID_TYPE());//element 만들때 사용.
		static CXUI_Wnd* AddChildUI(CXUI_Wnd * const pkParent, ID_TYPE const& rkElementID, ID_TYPE const& rkNewID, bool const bRemove);
		bool RegistCreator(ID_TYPE const& ID, CXUI_Wnd &rkBase);

		void Destroy();

		void SetCallBackFunc(F_XUI_Manager_CallBackFunc kCallBackFunc)	{	m_kXUI_Manager_CallBackFunc = kCallBackFunc;	}
		F_XUI_Manager_CallBackFunc	GetCallBackFunc()	{	return	m_kXUI_Manager_CallBackFunc;	}

		CXUI_Wnd* PickWnd(ID_TYPE const& ID);
		CXUI_Wnd* PickWnd(POINT2 const& ptkPickPos, bool const bIsPickToTop);

		void CorrectionLocationByResolution( const POINT2 ptGab ); // 해상도에 맞게 Location 변경

		//생성 해주고 구조 만든다.
		virtual bool VDisplay();
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue, int const *piKey);//얘는 키 눌러서 뭐 해야되나?
		virtual void VScale(float const fScale);
		virtual void VOnTick();
		virtual void VOnResize(POINT2 const& ptScrSize);
		virtual void VRefresh();

		virtual bool CALLBACK VHookMessage(HWND &hWnd, UINT &message, WPARAM &wParam, LPARAM &lParam, bool& bSkipDefProc);//윈도우 프로시저 에서 불러주어야함.
		//OnResize ->리사이즈 이벤트
		//IME 핸들링.
		//에디트 박스는 string을 가지고

		CXUI_Wnd* Get(ID_TYPE const& ID)const;
		CXUI_Wnd* Call(ID_TYPE  const& ID, bool const bIsModal = false, ID_TYPE const& rkNewID = ID_TYPE());
		CXUI_Wnd* Activate(ID_TYPE const& ID, bool const bIsModal = false, ID_TYPE const& rkNewID = ID_TYPE());
		bool IsActivate(ID_TYPE const& ParentID, CXUI_Wnd*& rkWnd) const;

		bool Close( ID_TYPE const& ID );
		void CloseAll();

		//		bool GetControlValue(ID_TYPE const& ParentID, ID_TYPE const& ControlID, EValuetype const eVT, std::wstring &wstrOut);
		//		bool SetControlValue(ID_TYPE const& ParentID, ID_TYPE const& ControlID, EValuetype const eVT, std::wstring const& wstrValue);

		bool SetEditFocus(ID_TYPE const& ParentID, ID_TYPE const& ControlID);
		CXUI_Wnd*	GetFocusedEdit();

		bool SendScriptEvent(ID_TYPE const& ParentID, ID_TYPE const& ControlID, std::wstring const& wstrScriptKey);

		POINT3I const& MousePos()const{return CXUI_Wnd::m_sMousePos;}
		bool ClearEditFocus();

		void ClearAllControl();
		void ReleaseAllControl();
		void ReCallControl();
		void PrintControls();
		void FlushUnusedControls();

		virtual CXUI_Wnd* VClone(){return NULL;}

		//POINT2 const& ScreenSize()const{return m_sptSrcSize;}

		int HideAllUI(bool const bAdd_LastContainer=false);
		int ShowAllUI();
		int HidePartUI(std::wstring const& HideKey, ContWndIDList const& rkList);
		int ShowPartUI(std::wstring const& HideKey);
		bool AddColor(int iIdx, DWORD dwColor);
		static DWORD GetColor(int iIdx);
		void SetColorSet();

		WND_LIST&	GetActiveList()	{	return	m_lstActive;	}

		//
		void RegistGlobalScript(SCRIPT_HASH::key_type const& rKey, SCRIPT_HASH::mapped_type const& rScript);
		bool DoGlobalScript(SCRIPT_HASH::key_type const& rScriptKey);
		CLASS_DECLARATION(bool, m_bBlockGlobalScript, BlockGlobalScript);	// Global Script Blocking

		CLASS_DECLARATION(DWORD, m_dwDblClickTick, DblClickTick);//더블클릭 Tick
		CLASS_DECLARATION(POINT3I, m_kDblClickBound, DblClickBound);//더블클릭 바운드 Rectangle Width(x), Height(y)
		CLASS_DECLARATION_S(int, MouseSensitivity);//마우스 속도
		const CXUI_Wnd* GetWndMouseOver() { return CXUI_Wnd::m_spWndMouseOver; };
		CLASS_DECLARATION_S(HWND, HWNDforIME);
		CLASS_DECLARATION_S(HINSTANCE, HINSTANCEforIME);
		CS::CCSIME& GetIME() const;//Candidate를 리턴
		//CLASS_DECLARATION(std::wstring, m_strLocal, LocalName);
		void InsertActiveList(CXUI_Wnd *pWnd);
		void ClearMediaPlayer()	{ m_spMediaPlayer = NULL; }

		SUniqueUITypeErrInfo const& ConvertUniqueTypeUIToErrNo(std::wstring const& kUIType) const;

		static void LocalName(std::wstring strName);
		static std::wstring LocalName();

	protected:
		static CXUI_Wnd* CreateElement(ID_TYPE const& TAG);//
		static CXUI_Wnd* Build(tagUIXMLData const *pUIRef, CXUI_Wnd* pParent = NULL);
		static CXUI_Wnd* BuildLocal(tagUIXMLData const *pUIRef, CXUI_Wnd* pWnd);

		void InitControls(); 
		void ApplyAlign(); 
	protected:
		void ExplorerAttr(void *pNode, PgUIXMLData* pTarget);
		void ExplorerNode(void *p, PgUIXMLData* pParent);
		void ReserveCreator();

		KEY_WORD_HASH::mapped_type TagToType(const KEY_WORD_HASH::key_type &strTag);

		bool CheckHotKeyToModifier(TiXmlNode *pNode, PgUIXMLData* pParent);
	protected:
		WND_LIST m_lstActive;//순서대로 찍어져야 하므로 이것은 list가 좋다.
		DWORD m_dwLastXUIFlushTime;

		static XUI_HASH m_kBufferdHash;//모달리스로 띄워지거나 했을때 Create를 하지 않고 버퍼에서 불러씀

		SAVED_ACTIVEID_LIST		m_SavedList;
		kUniqueUITypeErrCont	m_kUniqueUITypeErrCont;

		static PgUIXMLData		m_kXMLData;
		static KEY_WORD_HASH	m_mapKeyWord;
		static XUI_HASH			m_mapCreator;
		static COLOR_HASH		m_ColorMap;
		static DWORD			m_FrameCount;

		ContWndIDList			m_kReserveCloseSet;

		static std::wstring		m_strLocal;

		friend struct ::Loki::CreateStatic< CXUI_Manager >;

	private:
		bool m_bHideUI_Add_LastContainer;
	};
}

#define XUIMgr SINGLETON_STATIC(XUI::CXUI_Manager)

#ifdef _MT_
#pragma comment(lib, "XUI_MT.lib")
#pragma comment(lib, "freetype_MT.lib")
#endif

#ifdef _MTd_
#pragma comment(lib, "XUI_MTd.lib")
#pragma comment(lib, "freetype_MTd.lib")
#endif

#ifdef _MTo_
#pragma comment(lib, "freetype_MTo.lib")

#ifdef _ANTI_HACK_
#pragma comment(lib, "XUI_MToAH.lib")
#else
#pragma comment(lib, "XUI_MTo.lib")
#endif
#endif

#ifdef _MD_
#pragma comment(lib, "XUI_MD.lib")
#pragma comment(lib, "freetype_MD.lib")
#endif

#ifdef _MDd_
#pragma comment(lib, "XUI_MDd.lib")
#pragma comment(lib, "freetype_MDd.lib")
#endif

#ifdef _MDo_
#pragma comment(lib, "freetype_MDo.lib")
#ifdef _ANTI_HACK_
#pragma comment(lib, "XUI_MDoAH.lib")
#else
#pragma comment(lib, "XUI_MDo.lib")
#endif
#endif

#pragma comment (lib, "winmm.lib")



#endif // WEAPON_XUI_XUI_MANAGER_H