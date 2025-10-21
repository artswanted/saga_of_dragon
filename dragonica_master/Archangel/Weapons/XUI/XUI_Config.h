#ifndef WEAPON_XUI_XUI_CONFIG_H
#define WEAPON_XUI_XUI_CONFIG_H

#include <string>
#include "BM/ClassSupport.h"
#include "XUI_Wnd.h"

namespace XUI
{
	typedef struct tagSoundInfo
	{
		std::wstring wstrSndLBtnDown;
		std::wstring wstrSndRBtnDown;
		std::wstring wstrSndMouseOver;
	}SOUND_INFO;

	typedef struct tagStaticFormInfo
	{
		std::wstring wstrImgBody;
		std::wstring wstrImgBody_W;
		std::wstring wstrImgBody_H;
		std::wstring wstrImgTB;
		std::wstring wstrImgTB_W;
		std::wstring wstrImgTB_H;
		std::wstring wstrImgLR;
		std::wstring wstrImgLR_W;
		std::wstring wstrImgLR_H;
		std::wstring wstrImgCorner;
		std::wstring wstrImgCorner_W;
		std::wstring wstrImgCorner_H;

//		POINT2 ptBody;
//		POINT2 ptTB;
//		POINT2 ptLR;
//		POINT2 ptCorner;

/*		tagStaticFormInfo()
		{
			wstrImgBody.clear();
			wstrImgTB.clear();
			wstrImgLR.clear();
			wstrImgCorner.clear();

			ptBody.x = 0;	ptBody.y = 0;
			ptTB.x = 0;		ptTB.y = 0;
			ptLR.x = 0;		ptLR.y = 0;
			ptCorner.x = 0;	ptCorner.y = 0;
		}*/
	}STATIC_INFO;

	class CXUI_Config
		: public CXUI_Wnd
	{
		typedef std::map< int, SOUND_INFO > SOUND_INFO_HASH;

		typedef std::map< int, STATIC_INFO > STATIC_INFO_HASH;

	public:
		CXUI_Config(void);
		virtual ~CXUI_Config(void);
	
	public:
		bool SndInfo(int const iBtnType, E_MOUSE_EVENT_INDEX const eMEI, std::wstring const *&pOutResult)const;
		bool StaticInfo(int const iStaticType, E_STATIC_TYPE_INDEX const eSTI, std::wstring const* &pOutResult)const;
		DWORD GetBtnTextColor(int i);
		DWORD GetBtnTextColorByType(int iType, int i);

	protected:
		virtual E_XUI_TYPE VType(){return E_XUI_CONFIG;}
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual CXUI_Wnd* VCreate()const{return new CXUI_Config;}
	protected:
		bool RegSndInfo(int const iBtnType, E_MOUSE_EVENT_INDEX const eMEI, std::wstring const& rstrValue);
		bool RegStaicInfo(int const iStaticType, E_STATIC_TYPE_INDEX const eSTI, std::wstring const& rstrValue);

		SOUND_INFO_HASH m_mapSndInfo;
		STATIC_INFO_HASH m_mapStaticInfo;
		
		CLASS_DECLARATION_S(std::wstring, ToolTipImgBody);
		CLASS_DECLARATION_S(std::wstring, ToolTipImgTB);
		CLASS_DECLARATION_S(std::wstring, ToolTipImgLR);
		CLASS_DECLARATION_S(std::wstring, ToolTipImgCorner);
		CLASS_DECLARATION_S(std::wstring, ToolTipImgLock);
		CLASS_DECLARATION_S(std::wstring, ToolTipImgClose);

		SButtonColorType	m_kDefaultBtnTextColor;
		VEC_BUTTON_COLOR	m_kBtnTextColor;
	};
}

#endif // WEAPON_XUI_XUI_CONFIG_H