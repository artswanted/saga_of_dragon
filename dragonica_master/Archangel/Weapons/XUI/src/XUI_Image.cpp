#include "stdafx.h"
#include "XUI_Image.h"

using namespace XUI;


CXUI_Image::tagOverlayImgInfo::tagOverlayImgInfo()
	: pImg(NULL), iIndex(-1), ptImgSize(), wsImgName(), kUVInfo()
{
}


CXUI_Image::tagOverlayImgInfo::~tagOverlayImgInfo()
{
}



CXUI_Image::CXUI_Image(void)
{
}

CXUI_Image::~CXUI_Image(void)
{
	ReleaseOverlayImg();
}

void CXUI_Image::VInit()
{
#ifdef XUI_USE_GENERAL_OFFSCREEN
	if (NotUseOffscreen())
		UseOffscreen(false);
	else
		UseOffscreen(true);
#endif
	CXUI_Wnd::VInit();
}

void CXUI_Image::operator =(CXUI_Image const& rhs)
{
	CXUI_Wnd::operator =(rhs);

	ReleaseOverlayImg();
	CONT_OVERLAYIMG::const_reverse_iterator iter = rhs.m_kContOverlayImg.rbegin();
	for( ; iter != rhs.m_kContOverlayImg.rend(); ++iter)
	{
		OverlayImgInfo kImg = *iter;
		AddOverlayImg(kImg.wsImgName, kImg.ptImgSize, kImg.kUVInfo.U, kImg.kUVInfo.V, kImg.kUVInfo.Index);
	}

}

void CXUI_Image::ReleaseOverlayImg(void)
{
	CONT_OVERLAYIMG::iterator iter = m_kContOverlayImg.begin();
	for( ; iter != m_kContOverlayImg.end(); ++iter)
	{
		if((*iter).pImg)
		{
			m_spRscMgr->ReleaseRsc((*iter).pImg);
		}
	}

	m_kContOverlayImg.clear();
}

bool CXUI_Image::AddOverlayImg(std::wstring& wsSrcImgName, POINT2 ptSrcSize, size_t uiU, size_t uiV, size_t uiUVIndex)
{
	if(wsSrcImgName.empty())
	{
		return false;
	}
	OverlayImgInfo NewImg;
	NewImg.pImg = m_spRscMgr->GetRsc(wsSrcImgName);
	if(!NewImg.pImg)
	{
		return false;
	}
	NewImg.wsImgName = wsSrcImgName;
	NewImg.ptImgSize = ptSrcSize;
	NewImg.kUVInfo.Set(uiU, uiV, uiUVIndex);

	m_kContOverlayImg.push_back(NewImg);

	SetInvalidate();
	return true;
}

bool CXUI_Image::VDisplayOverlayImg(SRenderInfo& rkDefRenderInfo)
{
	bool bSuccess = true;
	CONT_OVERLAYIMG::iterator iter = m_kContOverlayImg.begin();
	for( ; iter != m_kContOverlayImg.end(); ++iter)
	{
		if(!(*iter).pImg)
		{
			bSuccess = false;
			continue;
		}

		SRenderInfo kRenderInfo;
		kRenderInfo = rkDefRenderInfo;
		kRenderInfo.kSizedScale.ptSrcSize = (*iter).ptImgSize;
		//kRenderInfo.kSizedScale.ptDrawSize.Set(40, 40);
		kRenderInfo.kUVInfo = (*iter).kUVInfo;
		if(!m_spRenderer->RenderSprite( (*iter).pImg, (*iter).iIndex, kRenderInfo))
		{
			bSuccess = false;
			continue;
		}
	}

	return bSuccess;
}

