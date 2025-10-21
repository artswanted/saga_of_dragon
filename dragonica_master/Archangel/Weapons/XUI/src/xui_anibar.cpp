#include "stdafx.h"
#include "XUI_AniBar.h"
#include "BM/vstring.h"

using namespace XUI;

CXUI_AniBar::CXUI_AniBar(void)
:	m_bIsReverse(false)
,	m_bIsVertical(false)
{
	Max(0);
	Now(0);
	IsAccel(false);
	Accel(0);
	
	typedef struct tagImgData
	{
		tagImgData()
		{
			iImgIndex = -1;
			pData = NULL;
		}
		
		int iImgIndex;
		void *pData;
	}ImgData;

	BarImg(NULL);
	BarImgIdx(-1);
	m_iBarCount = 1;
	m_dwStartTime = 0;
	m_dwFinTime = 0;
	Current(0);
	m_dwFillTime = 0;
	m_fFrameTime = 0;
	MinusTime(0);
	BarType(BT_DEFAULT);
	m_bIsScriptRun = false;
	m_fRate = 0.0f;
	IsSmooth(true);
}

CXUI_AniBar::~CXUI_AniBar(void)
{
	void *pImg = BarImg();
	if( pImg )
	{//본인이 가지고 계심.
		m_spRscMgr->ReleaseRsc(pImg);
	}
}

bool CXUI_AniBar::VOnTick( DWORD const dwCurTime )
{	//돌 때 마다 애니메이션 스크립트 실행
	DWORD const dwTickDelta = dwCurTime - GetLastTickTime();

	if ( !CXUI_Wnd::VOnTick(dwCurTime) )
	{
		return false;
	}

	if (!IsSmooth())
	{
		Current(Now());
		return false;
	}

	if(Current() != Now())
	{
		if( FinTime() == 0 && Current() > Now() )	//이미 한번 꽉 찼던 게이지
		{
			float fMinusAmount = ( (Current() - Now()) * dwTickDelta )/MinusTime();
			Current((int)(Current() - fMinusAmount));
			MinusTime(MinusTime() - dwTickDelta);
			if( MinusTime() <= 0 )
			{
				MinusTime(1000);
			}
			if( Current() <= Now() )
			{
				Current(Now());
			}
		}
		else if( FinTime() <= dwCurTime )	//셋팅된 시간, 누적된시간 비교
		{
			Current(Now());
			FinTime(0);
		}
		else
		{
			int iVal = (int)((float)(Max())*(dwCurTime - m_dwStartTime)/(m_dwFillTime));
			Current(iVal);

			if( Current() > Now() )
			{
				Current(Now());
				FinTime(0);
			}
		}
	}
//g_iAccValue = (GAUGE_LENGTH*g_fAccTime)/(g_fCastFinTime-g_fCastStartTime);
	return true;
}

bool CXUI_AniBar::VDisplay()
{
	//라인을 쭉 찍으면 됨.
	//
	if(!CXUI_Wnd::VDisplay()){return false;}
	if( Max() <= 0 || Now() <= 0 )		{ return false; }	//0으로 나눌 순 없으므로

	void *pImg = BarImg();
	if( pImg && m_spRenderer )
	{	
		IncAlpha();

		int iSkipCount = 0;
		bool bIsAdjust = false;
		float fRate = 0.0f;

		switch( BarType() )
		{
		case BT_WARNING_FLIP:
		case BT_UV_CONTROL:
			{
				fRate = static_cast<float>(Now())/static_cast<float>(Max());
			}break;
		default:
			{
				//1. BarCount 기준으로 current 값이 어떤 Count에 어떤 Pos인지 계산.
				//2. Rate 계산
				//3. Draw
				int const iCur = Current();//
				int const iDivValue = Max()/BarCount();//1줄이 가지는 절대 값.

				iSkipCount = iCur / iDivValue;//몇줄 스킵 되는가.

				if(iSkipCount == BarCount())//카운트랑 같다는 = Max 
				{
					bIsAdjust = true;
				}

				int const iDrawValue = (iCur%iDivValue) + ((bIsAdjust)?iDivValue:0) ;//찍힐 길이는 얼마인가..
				fRate = (float)iDrawValue/(float)iDivValue;
			}break;
		}

		Rate(fRate);

		if(!IsScriptRun() && bIsAdjust && fRate >= 0.99999f)
		{
				DoScript(SCRIPT_ON_BAR_FULL);
				IsScriptRun(true);
		}
		if( fRate > 1.0f )
		{
			fRate = 1.0f;
		}
		
		if(BarType() == BT_FILL_GAUGE)
		{
			fRate = 1.0f - fRate;
		}


		SRenderInfo kRenderInfo;
		kRenderInfo.bGrayScale = GrayScale();
		SSizedScale &rSS = kRenderInfo.kSizedScale;
		rSS.ptSrcSize = BarImgSize();

		if( IsVertical() )
		{
			rSS.ptDrawSize = POINT2((LONG)(BarImgSize().x / BarCount()), (LONG)(BarImgSize().y * fRate));
		}
		else
		{
			rSS.ptDrawSize = POINT2((LONG)(BarImgSize().x * fRate), (LONG)(BarImgSize().y / BarCount()));
		}

		if ( true == IsReverse() )
		{
			if( IsVertical() )
			{
				rSS.ptDrawSize.y *= -1;
			}
			else
			{
				rSS.ptDrawSize.x *= -1;
			}
		}

		switch( BarType() )
		{
		case BT_WARNING_FLIP:
			{
				float const fInterval = 1.0f / static_cast<float>(BarCount());
				int iIndex = 0;
				while(true)
				{
					if( (iIndex * fInterval) >= fRate )
					{
						break;
					}
					++iIndex;
				}
				UVUpdate((BarCount() - iIndex) + 1);// UV_INDEX는 1부터 시작하지
			}break;
		case BT_UV_CONTROL:
			{//이놈은 그냥 밖에서 지정하는 UV값 쓴다.
			}break;
		default:
			{
				UVInfo(SUVInfo(UVInfo().U, UVInfo().V, iSkipCount+ ((bIsAdjust)?0:1)));
			}break;
		}
		kRenderInfo.kUVInfo = UVInfo();
		kRenderInfo.kLoc = TotalLocation();
		GetParentDrawRect(kRenderInfo.rcDrawable);
		kRenderInfo.fAlpha = Alpha();
		kRenderInfo.bTwinkle = IsTwinkle() && NowTwinkleOn() && !IgnoreTwinkle();

		m_spRenderer->RenderSprite( pImg, m_BarImgIdx, kRenderInfo);
	}
	return true;
}

bool CXUI_AniBar::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
	if( !Visible() || IsClosed() ){return false;}//자식도 안함.
	if( !Enable() ){return false;}

	if( VPeekEvent_Default(rET, rPT, dwValue) )
	{
		return false;
	}
	return false;
}

void CXUI_AniBar::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);

//	if( ATTR_BAR_COLOR == wstrName ){	BarColor((int)vValue);	}

	if( ATTR_BAR_IMG == wstrName )
	{
		ChangeBarImage( wstrValue );
	}
	else if( ATTR_BAR_IMG_W == wstrName )	{	BarImgSize( POINT2((int)vValue, BarImgSize().y) );	}
	else if( ATTR_BAR_IMG_H == wstrName )	{	BarImgSize( POINT2(BarImgSize().x, (int)vValue) );	}
	else if( ATTR_BAR_COUNT == wstrName )	{	BarCount((int)vValue);	}
	else if( ATTR_BAR_TYPE == wstrName )	{	BarType((int)vValue);	}
	else if( ATTR_BAR_SMOOTH == wstrName )	
	{
		if ((int)vValue)
		{
			IsSmooth(true);	
		}
		else
		{
			IsSmooth(false);
		}
	}
	else if( ATTR_BAR_REVERSE == wstrName )
	{
		IsReverse( 0 != static_cast<int>(vValue) );
	}
	else if( ATTR_BAR_IS_VERTICAL == wstrName )
	{
		IsVertical( 0 != static_cast<int>(vValue) );
	}
}

void CXUI_AniBar::Now(int const iValue)
{
	if( Now() != iValue )
	{
		MinusTime(1000);

		SetInvalidate();
	}

	m_kNow = iValue;

}

/*! 
	\brief 애니바의 속성 설정
    \param eVT 속성 타입 
	\param wstrValue 속성 값
	\last update 2006.12.22 by 하승봉
*/
/*
bool CXUI_AniBar::SetValue(EValuetype const eVT, std::wstring const& wstrValue)
{
	if( CXUI_Wnd::SetValue(eVT, wstrValue) )
	{
		return true;
	}

	BM::vstring vValue(wstrValue);

	switch(eVT)
	{
	case VT_BAR_NOW:
		{
			if( Now() != (int) vValue )
			{
				MinusTime(1000);
			}

			Now( (int) vValue );
		}break;
	case VT_BAR_MAX:
		{
			Max( (int) vValue );
		}break;
	case VT_BAR_CUR:
		{
			Current( (int) vValue );
		}break;
//	case VT_BAR_COLOR:
//		{
//			BarColor( (int) vValue );
//		}break;
	case VT_BAR_TEXT:
		{
			Text(wstrValue);
		}break;
	case VT_BAR_START_TIME:
		{
			StartTime( (int) vValue );
		}break;
	case VT_BAR_FILL_TIME:
		{
			FillTime( (int) vValue );
			FinTime( StartTime() + FillTime() );
		}break;
	case VT_BAR_COUNT:
		{
			BarCount( (int) vValue );
		}break;

	default:
		{
			return false;
		}break;
	}

	return true;
}
*/
void CXUI_AniBar::FillTime(DWORD const dwInValue)
{
	m_dwFillTime = dwInValue;
	FinTime( StartTime() + FillTime() );
}

/*
시간지나면 조금씩 올라가는 모양으로 할려고 했는데.. 
내가 가야하는 타겟. 
도달 시간.

	if(pkPilot)
	{//	pWnd->Visible(true);
		int const iNow = (1000*dwAccTime)/(dwFinTime-dwStartTime));
		int const iMax = 1000;//pkPilot->MaxHP();

		BM::vstring vstrNow = iNow;
		BM::vstring vstrMax = iMax;
		pWnd->SetValue(XUI::VT_BAR_NOW, vstrNow);
		pWnd->SetValue(XUI::VT_BAR_MAX, vstrMax);
		return true;
	}
	else
	{
//		pWnd->Visible(false);
	}
*/
/*
bool CXUI_AniBar::GetValue(EValuetype const eVT, std::wstring &wstrOut)
{
	if( CXUI_Wnd::GetValue(eVT, wstrOut) )
	{
		return true;
	}
	
	std::wstring kTemp;
	kTemp.resize(256, 0);
	switch( eVT )
	{
	case VT_BAR_NOW:
		{
			_itot_s(Now(), &kTemp[0], kTemp.size(), 10);
			wstrOut = kTemp;
		}break;
	case VT_BAR_MAX:
		{
			_itot_s(Max(), &kTemp[0], kTemp.size(), 10);
			wstrOut = kTemp;
		}break;
	case VT_BAR_TEXT:
		{
			wstrOut = Text();
		}break;
	case VT_BAR_START_TIME:
		{
			_itot_s(StartTime(), &kTemp[0], kTemp.size(), 10);
			wstrOut = kTemp;
		}break;
	case VT_BAR_FILL_TIME:
		{
			_itot_s(FillTime(), &kTemp[0], kTemp.size(), 10);
			wstrOut = kTemp;
		}break;
	case VT_BAR_COUNT:
		{
			_itot_s(BarCount(), &kTemp[0], kTemp.size(), 10);
			wstrOut = kTemp;
		}break;
	default:
		{
			return false;
		}break;
	}
	return false;
}
*/

bool CXUI_AniBar::ChangeBarImage( std::wstring const &wstrPath )
{
	CXUI_Resource_Manager_Base::T_MAPPED* pRsc = m_spRscMgr->GetRsc( wstrPath );
	if ( pRsc )
	{
		BarImgName( wstrPath );
		BarImg( pRsc );
		return true;
	}
	return false;
}

CXUI_Wnd* CXUI_AniBar::VClone()
{
	CXUI_AniBar *pWnd = new CXUI_AniBar;
	pWnd->operator =(*this);
	return pWnd;
}

void CXUI_AniBar::operator = ( const CXUI_AniBar &rhs)
{
	CXUI_Wnd::operator =(rhs);

	Now(rhs.Now());
	FillTime(rhs.FillTime());
	Max(rhs.Max());
	Current(rhs.Current());

	IsAccel(rhs.IsAccel());
	Accel(rhs.Accel());

	BarImgSize(rhs.BarImgSize());
	BarImgName(rhs.BarImgName());
	BarImg(m_spRscMgr->GetRsc( BarImgName()));
	BarImgIdx(rhs.BarImgIdx());
	BarCount(rhs.BarCount());
	StartTime(rhs.StartTime());
	CloseTime(rhs.CloseTime());
	FinTime(rhs.FinTime());
	FillTime(rhs.FillTime());
	FrameTime(rhs.FrameTime());
	MinusTime(rhs.MinusTime());
	BarType(rhs.BarType());
	IsScriptRun(rhs.IsScriptRun());
	Rate(rhs.Rate());
	m_bIsReverse = rhs.m_bIsReverse;
	IsSmooth(rhs.IsSmooth());
	IsVertical(rhs.IsVertical());
}