#ifndef WEAPON_XUI_XUI_ANIBAR_H
#define WEAPON_XUI_XUI_ANIBAR_H

#include "XUI_Control.h"
namespace XUI
{
	class CXUI_AniBar//애니메이션 바. -> 연출 가능한 막대그래프 
		:	public CXUI_Wnd
	{
	public:
		CXUI_AniBar(void);
	public:
		virtual ~CXUI_AniBar(void);
	public:
		enum eBar_Type
		{
			BT_DEFAULT		= 0,
			BT_FILL_GAUGE	= 1,
			BT_WARNING_FLIP,
			BT_UV_CONTROL,
		};

		virtual E_XUI_TYPE VType(){return E_XUI_ANI_BAR;}
		virtual bool VOnTick( DWORD const dwCurTime );
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);
		virtual bool VDisplay();

		virtual CXUI_Wnd* VCreate()const{return new CXUI_AniBar;}

		void operator = ( const CXUI_AniBar &rhs);
		virtual CXUI_Wnd* VClone();
		//float Rate;
		//int itempCur;
		//float fTempMax;
	public:
		void Now(int const iValue);

		void FillTime(DWORD const dwInValue);
		bool ChangeBarImage( std::wstring const &wstrPath );

		CLASS_DECLARATION_S_INVALIDATE(int, Max );//실제 데이터 셋팅값
		CLASS_DECLARATION_S_NO_SET(int, Now );//실제 데이터 셋팅값
		CLASS_DECLARATION_S_INVALIDATE(int, Current );//현재올라가는 버퍼값 -> 디스플레이용
		CLASS_DECLARATION_S( std::wstring, BarImgName )
		CLASS_DECLARATION(bool, m_bIsAccel, IsAccel );
		CLASS_DECLARATION(int, m_iAccel, Accel );

//		CLASS_DECLARATION(int, m_BarColor, BarColor );
		
		CLASS_DECLARATION_PTR( void*, m_pBarImg, BarImg );
		CLASS_DECLARATION(POINT2, m_BarImgSize, BarImgSize );
		CLASS_DECLARATION_INVALIDATE(int, m_BarImgIdx, BarImgIdx );//-1 이 초기값이므로 DWORD 로 쓰지말것.
		CLASS_DECLARATION(int, m_iBarCount, BarCount );//바를 몇개 사용한건지

		CLASS_DECLARATION(DWORD, m_dwStartTime, StartTime );
		CLASS_DECLARATION(DWORD, m__dwCloseTime, CloseTime);
		CLASS_DECLARATION(DWORD, m_dwFinTime, FinTime );	//<-내부에서 filltime을 사용해 계산할 것
		CLASS_DECLARATION_NO_SET(DWORD, m_dwFillTime, FillTime );

		CLASS_DECLARATION(float, m_fFrameTime, FrameTime );
		CLASS_DECLARATION(float, m_fMinusTime, MinusTime );
		CLASS_DECLARATION(int, m_iBarType, BarType );//타입, 현재는 깍이는것만 있음

		CLASS_DECLARATION(bool, m_bIsScriptRun, IsScriptRun );	//스크립트가 실행됬는지

		CLASS_DECLARATION(float, m_fRate, Rate);
		CLASS_DECLARATION_S(bool, IsSmooth );//천천히 차오르거나 사라지는가
		CLASS_DECLARATION(bool, m_bIsReverse, IsReverse );//게이지가 왼쪽에서 오른쪽으로 단다.
		CLASS_DECLARATION(bool, m_bIsVertical, IsVertical );
	};
}

#endif // WEAPON_XUI_XUI_ANIBAR_H