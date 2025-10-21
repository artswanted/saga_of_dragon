#ifndef WEAPON_XUI_XUI_IMAGE_H
#define WEAPON_XUI_XUI_IMAGE_H

#include "XUI_Control.h"

namespace XUI
{
	class CXUI_Image
		:	public CXUI_Control
	{
	public:
		CXUI_Image(void);
	public:
		~CXUI_Image(void);
		
		virtual E_XUI_TYPE VType(){return E_XUI_IMG;}

		virtual CXUI_Wnd* VCreate()const{return new CXUI_Image;}
		virtual CXUI_Wnd* VClone()
		{
			CXUI_Image *pWnd = new CXUI_Image;

			pWnd->operator =(*this);
			return pWnd;
		}
		virtual void VInit();
		virtual bool VDisplayOverlayImg(SRenderInfo& rkDefRenderInfo);

		void operator =(CXUI_Image const& rhs);

	protected:
		typedef struct tagOverlayImgInfo
		{
			std::wstring wsImgName;
			void* pImg;
			int iIndex;
			SUVInfo kUVInfo;
			POINT2 ptImgSize;

			tagOverlayImgInfo();
			~tagOverlayImgInfo();
		}OverlayImgInfo;

		typedef std::vector<OverlayImgInfo> CONT_OVERLAYIMG;
		CONT_OVERLAYIMG m_kContOverlayImg;

	public:
		bool AddOverlayImg(std::wstring& wsSrcImgName, POINT2 ptSrcSize, size_t uiU, size_t uiV, size_t uiUVIndex);
		void ReleaseOverlayImg(void);
	};
}

#endif // WEAPON_XUI_XUI_IMAGE_H