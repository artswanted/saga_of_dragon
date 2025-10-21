#ifndef WEAPON_XUI_XUI_RENDERER_BASE_H
#define WEAPON_XUI_XUI_RENDERER_BASE_H

#include <tchar.h>
#include "BM/Point.h"

typedef struct tagUVInfo
{
	tagUVInfo()
	{
		Set(1,1,1);
	}

	tagUVInfo(size_t const InU, size_t const InY, size_t const InIndex)
	{
		Set( InU, InY, InIndex);
	}

	void Set( size_t const InU, size_t const InY, size_t const InIndex)
	{
		U = InU;
		V = InY;
		Index = InIndex;
	}

	bool	operator !=(tagUVInfo const& kInfo) const
	{
		if(kInfo.Index != Index)
		{
			return	true;
		}
		if(kInfo.U != U)
		{
			return	true;
		}
		if(kInfo.V != V)
		{
			return	true;
		}

		return	false;
	}

	size_t U;
	size_t V;
	size_t Index;//UV 번호
}SUVInfo;

typedef struct tagSizedScale
{
	tagSizedScale()
	{
	}
	tagSizedScale(POINT2 const inptDrawSize, POINT2 const inptSrcSize)
	{
		ptDrawSize = inptDrawSize;
		ptSrcSize = inptSrcSize;
	}

	POINT2 ptDrawSize;//그려질 크기(이값이 -이면 오른쪽(아래)부터 그린다)
	POINT2 ptSrcSize;//원본 이미지 크기
}SSizedScale;

typedef struct tagRenderInfo
{
	tagRenderInfo()
		:	fAlpha(1.0f)
		,	fScale(1.0f)
		,	dwColor(0xFFFFFFFF)
		,	bColorChange(false)
		,	bGrayScale(false)
		,	bTwinkle(false)
		,	fRotationDeg(0.0f)
	{
		SetRect(&rcDrawable,0,0,0,0);
		SetRect(&rcClip,0,0,0,0);
	}

	POINT3I kLoc;
	SUVInfo kUVInfo;
	SSizedScale kSizedScale;
	RECT rcDrawable;
	RECT rcClip;
	float fAlpha;
	DWORD dwColor;
	bool bColorChange;
	bool bGrayScale;
	bool bTwinkle;
	float fScale;
	POINT2F kScaleCenter;

	float fRotationDeg;
	POINT2F kRotationCenter;
	
}SRenderInfo;


typedef struct tagRenderTextInfo
{
	tagRenderTextInfo()
	{
		wstrText = _T("");
		wstrFontKey= _T("");
		SetRect(&rcDrawable,0,0,0,0);
		fAlpha = 1.0f;
		dwOutLineColor = 0xFF000000;
		dwTextFlag = 0;
		m_p2DString = NULL;
		dwDiffuseColor = 0xFFFFFFFF;
	}

	std::wstring wstrText;
	std::wstring wstrFontKey;
	POINT3I kLoc;
	RECT rcDrawable;
	float fAlpha;
	DWORD dwOutLineColor;//Outline Color
	DWORD dwDiffuseColor;//Texture Diffuse + FontColor (EX: WHITE + BLUE = BLUE, EX2: BLUE + WHILE = BLUE, EX3: BLUE + RED = BLACK)
	DWORD dwTextFlag;

	void *m_p2DString;	//	leesg213
}SRenderTextInfo;

namespace XUI
{
	class CXUI_Renderer_Base
	{
	public:
		CXUI_Renderer_Base(){}
		virtual ~CXUI_Renderer_Base(){}

		virtual bool RenderText(const SRenderTextInfo& rRenderTextInfo) = 0;
		virtual bool RenderSprite(void* &pSprite, int &iScrIndex, SRenderInfo const& rRenderInfo, bool bInsertList = true) = 0;
		virtual bool RenderSpriteToOffscreen(void* &pOffscreen, void* &pSprite, int &iScrIndex, SRenderInfo const& rRenderInfo) = 0;
		virtual bool RenderOffscreen(void* &pOffscreen) = 0;
		virtual bool InvalidateOffscreen(void* &pOffscreen) = 0;
		virtual bool InvalidateOffscreen(void* &pOffscreen, RECT& rectInvalidate) = 0;
		virtual bool EndRenderOffscreen(void* &pOffscreen, void** ppImages, int iNumImages, bool bClearOffscreen = true) = 0;
		virtual bool EndRenderOffscreen(void* &pOffscreen, std::list<std::pair<void*,bool>>& rkDrawList, bool bClearOffscreen = true) = 0;
	};
}

#endif // WEAPON_XUI_XUI_RENDERER_BASE_H