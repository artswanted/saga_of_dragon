#ifndef WEAPON_XUI_XUI_MSGBOX_H
#define WEAPON_XUI_XUI_MSGBOX_H

#include "XUI_Wnd.h"
#include "BM/GUID.h"

namespace XUI 
{
	typedef struct tagMessageBoxInfo
	{
		int iBoxType;//메세지 박스 타입
		BM::GUID kOrderGuid;//주문자정보
		BM::GUID kExtGuid;//확장 GUID
	}PgMessageBoxInfo;

	class CXUI_MsgBox
		: public CXUI_Wnd
	{
	public:
		CXUI_MsgBox(void);
	public:
		virtual ~CXUI_MsgBox(void);

		virtual E_XUI_TYPE VType(){return E_XUI_MSG_BOX;}
		virtual void VInit();
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
	
		virtual CXUI_Wnd* VCreate()const{return new CXUI_MsgBox;}

		virtual CXUI_Wnd* VClone()
		{
			CXUI_MsgBox *pWnd = new CXUI_MsgBox;

			pWnd->operator =(*this);
			pWnd->BoxInfo(BoxInfo());
			return pWnd;
		}
	public:
		CLASS_DECLARATION(PgMessageBoxInfo, m_kBoxInfo, BoxInfo);
	};
};

#endif // WEAPON_XUI_XUI_MSGBOX_H