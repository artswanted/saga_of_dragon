#ifndef WEAPON_XUI_XUI_EDIT_H
#define WEAPON_XUI_XUI_EDIT_H

#include <string>
#include "XUI_Control.h"
#include "XUI_Font.h"
#include "CSIME/CSIME.h"

namespace XUI
{
	class CXUI_Edit
		:	public CXUI_Control
	{
	public:
		class PgEditFocusGroup
		{
		public:
			typedef struct tagEditControl
			{
				tagEditControl(int const OrderLank, XUI::CXUI_Edit* pEdit)
					: iOrderLank(OrderLank), pkEdit(pEdit)
				{}

				bool operator < (tagEditControl const& rhs) const
				{
					return ( iOrderLank < rhs.iOrderLank );
				}

				bool operator > (tagEditControl const& rhs) const
				{
					return ( rhs < *this );
				}

				bool operator == (tagEditControl const& rhs) const
				{
					return ( iOrderLank == rhs.iOrderLank );
				}

				int iOrderLank;
				XUI::CXUI_Edit* pkEdit;
			}SEditInfo;
			typedef std::list< SEditInfo > CONT_EDIT_CHILD;

			bool AddControl(int const iOrderLank, XUI::CXUI_Edit* pkEdit);
			bool AddControl(CXUI_Edit* pkEdit);
			bool DelControl(XUI::CXUI_Edit* pkEdit);
			void FocusPassNextEdit(CXUI_Edit* pkEdit);
			void ClearData();

			int GetEditControlCount() const { return static_cast<int>(m_kContEditChild.size()); }
			
			bool operator == (XUI::CXUI_Wnd* rhs) const;
			bool operator == (ID_TYPE const& rhs) const;

			explicit PgEditFocusGroup(ID_TYPE const& kGrandParentID);
			~PgEditFocusGroup(void);

		protected:
			ID_TYPE	m_kParentID;
			CONT_EDIT_CHILD m_kContEditChild;
		};

		typedef std::list< PgEditFocusGroup > CONT_EDIT_GROUP;		


	public:
		CXUI_Edit(void);
	public:
		virtual ~CXUI_Edit(void);
	public:
		virtual E_XUI_TYPE VType(){return E_XUI_EDIT;}
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);
		virtual bool VDisplay();

		virtual void Visible(bool const& bIsVisible);
		virtual bool Visible();

		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);

		virtual void VOnCall();
		virtual bool SetEditFocus(bool const bIsJustFocus);
		virtual void VOnClose();
		virtual void VInit();
		virtual bool VOnTick( DWORD const dwCurTime );
		virtual void OnHookEvent();
		virtual void VLoseFocus(bool const bUpToParent = false);
		
		virtual CXUI_Wnd* VCreate()const{return new CXUI_Edit;}

		virtual CXUI_Wnd* VClone()
		{
			CXUI_Edit *pWnd = new CXUI_Edit;

			pWnd->operator =(*this);
			return pWnd;
		}

		void operator = ( const CXUI_Edit &rhs);
		
		virtual bool IsFocus()const;

		virtual void EditText(std::wstring const& wstrValue,bool bKeepTextBlock = false);
		std::wstring const& EditText()const;

		bool GetEditText_TextBlockApplied(CXUI_Style_String &koutString)const; 
		

		static CXUI_Edit* m_spFocusedEdit;
		static void ReleaseGlobalImage();

		static	void	SetFocusedEdit(CXUI_Edit *pkEdit);
		static	CXUI_Edit*	GetFocusedEdit()	{	return	m_spFocusedEdit;	}
		static	void	FocusingPassGroupEdit(CXUI_Edit* pkEdit);
		static	bool	AddGroupEdit(CXUI_Edit* pkEdit, int const iOrderLank);
		static	void	DelGroupEdit(CXUI_Edit* pkEdit);
		static	void	ClearGroupEdit();

		struct	stTextBlock
		{
			std::wstring::size_type	m_iPos;
			DWORD	m_dwTextColor;
			std::wstring	m_kText;
			int	m_iExtraDataLen;
			BYTE	*m_pkExtraData;
			stTextBlock():m_iPos(-1),m_dwTextColor(0),m_pkExtraData(0),m_iExtraDataLen(-1)
			{
			};
			stTextBlock(stTextBlock const &kTextBlock):
			m_iPos(kTextBlock.m_iPos),
				m_kText(kTextBlock.m_kText),
				m_dwTextColor(kTextBlock.m_dwTextColor),
				m_pkExtraData(NULL),
				m_iExtraDataLen(kTextBlock.m_iExtraDataLen)

			{
				if(m_iExtraDataLen>0)
				{
					m_pkExtraData = new BYTE[m_iExtraDataLen];
					memcpy(m_pkExtraData,kTextBlock.m_pkExtraData,m_iExtraDataLen);
				}
			}
			stTextBlock(std::wstring::size_type iPos,
				std::wstring const &kText,
				DWORD dwTextColor,
				int iExtraDataLen,
				BYTE *pkExtraData):
			m_iPos(iPos),
				m_kText(kText),
				m_dwTextColor(dwTextColor),
				m_pkExtraData(NULL),
				m_iExtraDataLen(iExtraDataLen)
			{
				if(iExtraDataLen>0)
				{
					m_pkExtraData = new BYTE[iExtraDataLen];
					memcpy(m_pkExtraData,pkExtraData,iExtraDataLen);
				}
			};


			virtual	~stTextBlock()
			{
				SAFE_DELETE_ARRAY(m_pkExtraData);
			}
		};

		typedef	std::list<stTextBlock> TextBlockCont;

protected:

		CLASS_DECLARATION_S(DWORD,CarotBlinkTime);
		CLASS_DECLARATION_S(bool,CarotBlink);
		CLASS_DECLARATION_S(bool, IsSecret);
		CLASS_DECLARATION_S(bool, IsEditDisable);
		CLASS_DECLARATION_S(int, IsOnlyNum);
		CLASS_DECLARATION_S(bool, IsMultiLine);
		CLASS_DECLARATION_S(bool, IsNativeIME);
		CLASS_DECLARATION_S(int, LastAddCount);
		CLASS_DECLARATION_S_NO_SET(int, OrderLank);

protected:

		std::wstring m_wstrInputText;
		std::wstring m_wstrPastInputText;
		std::wstring m_wstrRealString;

		int m_iPastCarotPos;

		TextBlockCont	m_kTextBlocks;

		XUI::CXUI_2DString *m_p2DString;	//	leesg213

		//캐럿 출력을 위해 추가 by 하승봉
		static int m_siCarotImgIdx;	//모든 에디트 박스에 하나씩만 있으면 되니까 스태틱으로 빼자
		static int m_siBlockImgIdx;
		static CONT_EDIT_GROUP m_kEditGroupCont;

		VEC_LINE m_kVecLine;
		
		CLASS_DECLARATION( std::wstring, m_wstrPreviewText, PreviewText );
		CLASS_DECLARATION_NO_SET( std::wstring, m_EditFont, EditFont );
		void EditFont(std::wstring const& value);

		CLASS_DECLARATION( int, m_iLimitLength, LimitLength );
		bool SetLimitLength(int const iLimitLength);

		CLASS_DECLARATION_NO_SET( POINT2, m_ptEditTextPos, EditTextPos );
		void EditTextPos(POINT2 const& value);

		void	ApplyTextBlockToText(std::wstring const &kSrcText,std::wstring::size_type const &kOffset,std::wstring::size_type const &kLength,std::wstring &koutText) const;
		void	AddTextBlock(int iPos,std::wstring const &kText,int iExtraDataLen,BYTE *pkExtraData,DWORD dwTextColor = 0xffffffff);
		void	RemoveAllTextBlock()
		{
			m_kTextBlocks.clear();
		}
		TextBlockCont&	GetTextBlocks()		{	return	m_kTextBlocks;	}

		static	bool	CALLBACK	OnImeCharCallBack(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);
		static	bool	CALLBACK	OnCommandCallBack(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam);
		static	unsigned	int	GetVirtualKeyByScanCode(BYTE byScanCode,unsigned int iDefaultKeyID);

		//int CalcWidth(std::wstring const text) const;

		//int GetWidth(unsigned short code) const;
		
		//에디트 범위 이상은 스크롤 하기 위해
		virtual int const MakeEditString(std::wstring & Val, int const iCarot);
		int m_iEndTextPos;	//가장 마지막 글자 위치.
		int m_iStartTextPos;
		std::wstring m_wstrRealText;

		static void* m_spTextBlockBgImg; //텍스트 선택영역 뒷배경용
		static void* m_spCarotImg;		 //캐럿 그림

		std::wstring m_wstrBlockPath;
		std::wstring m_wstrCarotPath;

		virtual void RenderBlock(std::wstring & Val);	//선택영역 블록 출력 
		virtual POINT2 CalcCaretPos(std::wstring const& wstrReal, CS::CARETDATA const& rkData, XUI::CXUI_Font* pFont, int const iLine = 1);
		virtual void RenderCarot(POINT2& pt);	//캐럿출력
		
		virtual int GetClickTextPos();					//마우스 클릭한 위치(int)얻기
		virtual void MoveCarotToClickPos(int &iStart, int &iEnd);				//원하는 위치로 캐럿 옮기기
		int const GetLineCount(std::wstring &krString, int const iCarot);
		void MakeSecretVecLine();
	};
}

#endif // WEAPON_XUI_XUI_EDIT_H