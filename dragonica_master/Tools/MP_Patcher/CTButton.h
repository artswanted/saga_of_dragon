#pragma once

#include	"defines.h"

class CCTButton : public CHBitmap
{
public:
	typedef enum eBTNSTATE
	{
		eNORMAL_SHAPE	=0,	// 기본
		eOVER_SHAPE		=1,	// 마우스 오버
		eCLICKL_SHAPE	=2,	// 클릭
		eDEACT_SHAPE	=3,	// 비활성화
	}eBTNSTATE;
	
	typedef enum E_BTN_TYPE
	{
		eBTN_NOMAL		= 0,
		eBTN_START		= 1,
		eBTN_EXIT		= 2,
		eBTN_RECOVERY	= 3,
		eBTN_SUB_CANCEL = 4,
		eBTN_OPTION		= 5,
		eBTN_DIVISION   = 6, //by reOiL Folding
		eBTN_LOCALE_EU	= 7,
		eBTN_LOCALE_RU	= 8,
		eBTN_LOCALE_FR	= 9,

		eBTN_PREV		= 10,
		eBTN_NEXT		= 11,
	};

	typedef enum E_TRAY_CMD
	{
		eTRAY_SHOW_WND = 100,
		eTRAY_EXIT_APP
	};

public:
	CCTButton(void);
	virtual ~CCTButton(void);

	void Init(HINSTANCE Inst, HWND Parent, int CtlID, int iType, POINT const& kPos,
		 	  int iShapeCnt, int iInitShape, std::wstring BtnText, std::wstring kLink,
			  std::vector<char> const& kData);
	void OwnerDraw(LPDRAWITEMSTRUCT lpDIS);		
	bool CheckMouseOver(int iX, int iY);
	bool OpenLink() const;

	int GetControlID()  const { return m_iCtlID; }
	int GetType()		const { return m_iType;  }
	bool Visible()		const { return m_bVisible; }
	void SetLoc(POINT const& Pt);
	void Visible(bool IsVisible);
	void SetBtnText(std::wstring const& Text) { m_wstrBtnText = Text; };
	void SetBtnTextFlag(UINT const Flag)	{ m_kBtnTextFlag = Flag; };
	void SetInvalidate() { ::InvalidateRect(m_hWnd, NULL, false); };

	void SetParentBitmap(CHBitmap* Bitmap)
	{ 
		m_hParentBitmap = Bitmap; 
	};

protected:
	void	DrawButtonImg();
	int const BtnHgt() const;		// 보여질 버튼의 세로 길이

private:
	HWND	m_hParent;
	HWND	m_hWnd;

	int		m_iCtlID;
//	POINT	m_kPos;
	int		m_kiButtonState;
	std::wstring	m_wstrBtnText;
	UINT	m_kBtnTextFlag;
	int		m_iShapeCnt;			// 이미지 하나에 표현된 버튼이 총 몇개인가
	int		m_iType;
	bool	m_bVisible;
	std::wstring  m_kLink;
	CHBitmap*	m_hParentBitmap;
};