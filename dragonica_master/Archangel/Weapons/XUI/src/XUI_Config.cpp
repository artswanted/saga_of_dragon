#include "stdafx.h"
#include "XUI_Config.h"

using namespace XUI;

#define REGIST_SOUND(index) \
	if( ATTR_SOUND_L_BTN_DOWN_##index == wstrName ){RegSndInfo(index, MEI_BTN_0, wstrValue);}	\
	else if( ATTR_SOUND_R_BTN_DOWN_##index == wstrName ){RegSndInfo(index, MEI_BTN_1, wstrValue);} \
	else if( ATTR_SOUND_MOUSE_OVER_##index == wstrName ){RegSndInfo(index, MEI_X, wstrValue);}

#define REGIST_STATIC_FORM(index) \
	if( ATTR_IMG_BODY_##index == wstrName )	{RegStaicInfo(index, STI_BODY, wstrValue);}	\
	else if( ATTR_BODY_W_##index == wstrName )	{RegStaicInfo(index, STI_BODY_W, wstrValue);}	\
	else if( ATTR_BODY_H_##index == wstrName )	{RegStaicInfo(index, STI_BODY_H, wstrValue);}	\
	else if( ATTR_IMG_TB_##index == wstrName )	{RegStaicInfo(index, STI_TB, wstrValue);}	\
	else if( ATTR_TB_W_##index == wstrName )		{RegStaicInfo(index, STI_TB_W, wstrValue);}	\
	else if( ATTR_TB_H_##index == wstrName )		{RegStaicInfo(index, STI_TB_H, wstrValue);}	\
	else if( ATTR_IMG_LR_##index == wstrName )	{RegStaicInfo(index, STI_LR, wstrValue);}	\
	else if( ATTR_LR_W_##index == wstrName )		{RegStaicInfo(index, STI_LR_W, wstrValue);}	\
	else if( ATTR_LR_H_##index == wstrName )		{RegStaicInfo(index, STI_LR_H, wstrValue);}	\
	else if( ATTR_IMG_CORNER_##index == wstrName){RegStaicInfo(index, STI_CORNER, wstrValue);}	\
	else if( ATTR_CORNER_W_##index == wstrName )	{RegStaicInfo(index, STI_CORNER_W, wstrValue);}	\
	else if( ATTR_CORNER_H_##index == wstrName )	{RegStaicInfo(index, STI_CORNER_H, wstrValue);}	

#define REGIST_BTN_T_COLOR_SET(index) \
	if( ATTR_BTN_UP_TEXT_COLOR_##index == wstrName ){ m_kBtnTextColor.resize(index+1);_stscanf_s(wstrValue.c_str(), L"%x", m_kBtnTextColor.at(index).dwColor);m_kBtnTextColor.at(index).Set(4, m_kBtnTextColor.at(index).Get(0) );m_kBtnTextColor.at(index).Set(5, m_kBtnTextColor.at(index).Get(0) );}	\
	else if( ATTR_BTN_OVER_TEXT_COLOR_##index == wstrName ){ m_kBtnTextColor.resize(index+1);_stscanf_s(wstrValue.c_str(), L"%x", m_kBtnTextColor.at(index).dwColor + 1); }	\
	else if( ATTR_BTN_DOWN_TEXT_COLOR_##index == wstrName ){ m_kBtnTextColor.resize(index+1);_stscanf_s(wstrValue.c_str(), L"%x", m_kBtnTextColor.at(index).dwColor + 2); }	\
	else if( ATTR_BTN_DIS_TEXT_COLOR_##index == wstrName ){ m_kBtnTextColor.resize(index+1);_stscanf_s(wstrValue.c_str(), L"%x", m_kBtnTextColor.at(index).dwColor + 3); }

CXUI_Config::CXUI_Config(void)
:	m_kDefaultBtnTextColor(0xFFFFFFFF)
{
}

CXUI_Config::~CXUI_Config(void)
{
}

void CXUI_Config::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	BM::vstring vValue(wstrValue);

	REGIST_SOUND(0);
	REGIST_SOUND(1);
	REGIST_SOUND(2);
	REGIST_SOUND(3);
	REGIST_SOUND(4);
	REGIST_SOUND(5);

	REGIST_STATIC_FORM(1);//ATTR_STATIC_SET(1) 과 한쌍 
	REGIST_STATIC_FORM(2);
	REGIST_STATIC_FORM(3);
	REGIST_STATIC_FORM(4);
	REGIST_STATIC_FORM(5);
	REGIST_STATIC_FORM(6);
	REGIST_STATIC_FORM(7);
	REGIST_STATIC_FORM(8);
	REGIST_STATIC_FORM(9);
	REGIST_STATIC_FORM(10);
	REGIST_STATIC_FORM(11);
	REGIST_STATIC_FORM(12);
	REGIST_STATIC_FORM(13);
	REGIST_STATIC_FORM(14);
	REGIST_STATIC_FORM(15);
	REGIST_STATIC_FORM(16);
	REGIST_STATIC_FORM(17);
	REGIST_STATIC_FORM(18);
	REGIST_STATIC_FORM(19);
	REGIST_STATIC_FORM(20);
	REGIST_STATIC_FORM(21);
	REGIST_STATIC_FORM(22);
	REGIST_STATIC_FORM(23);
	REGIST_STATIC_FORM(24);
	REGIST_STATIC_FORM(25);
	REGIST_STATIC_FORM(26);
	REGIST_STATIC_FORM(27);
	REGIST_STATIC_FORM(28);
	REGIST_STATIC_FORM(29);
	REGIST_STATIC_FORM(30);
	REGIST_STATIC_FORM(31);
	REGIST_STATIC_FORM(32);
	REGIST_STATIC_FORM(33);
	REGIST_STATIC_FORM(34);
	REGIST_STATIC_FORM(35);
	REGIST_STATIC_FORM(36);
	REGIST_STATIC_FORM(37);
	REGIST_STATIC_FORM(38);
	REGIST_STATIC_FORM(39);
	REGIST_STATIC_FORM(40);
	REGIST_STATIC_FORM(41);
	REGIST_STATIC_FORM(42);
	REGIST_STATIC_FORM(43);
	REGIST_STATIC_FORM(44);
	REGIST_STATIC_FORM(45);
	REGIST_STATIC_FORM(46);
	REGIST_STATIC_FORM(47);
	REGIST_STATIC_FORM(48);
	REGIST_STATIC_FORM(49);

	bool bPrepare = false;
	if( ATTR_TOOL_TIP_IMG_BODY == wstrName ) { ToolTipImgBody(wstrValue); bPrepare = true; }
	else if( ATTR_TOOL_TIP_IMG_TOP_BOTTOM == wstrName ){ToolTipImgTB(wstrValue); bPrepare = true;}
	else if( ATTR_TOOL_TIP_IMG_LEFR_RIGHT == wstrName ){ToolTipImgLR(wstrValue); bPrepare = true;}
	else if( ATTR_TOOL_TIP_IMG_CORNER == wstrName ){ToolTipImgCorner(wstrValue); bPrepare = true;}
	else if( ATTR_TOOL_TIP_IMG_LOCK == wstrName ){ToolTipImgLock(wstrValue); bPrepare = true;}
	else if( ATTR_TOOL_TIP_IMG_CLOSE == wstrName ){ToolTipImgClose(wstrValue); bPrepare = true;}

	else if( ATTR_BTN_UP_TEXT_COLOR_DFT == wstrName )
	{	
		_stscanf_s(wstrValue.c_str(), _T("%x"), m_kDefaultBtnTextColor.dwColor );
		m_kDefaultBtnTextColor.Set( 4, m_kDefaultBtnTextColor.Get(0) );
		m_kDefaultBtnTextColor.Set( 5, m_kDefaultBtnTextColor.Get(0) );
	}
	else if( ATTR_BTN_OVER_TEXT_COLOR_DFT == wstrName )
	{ _stscanf_s(wstrValue.c_str(), _T("%x"), m_kDefaultBtnTextColor.dwColor + 1); }
	else if( ATTR_BTN_DOWN_TEXT_COLOR_DFT == wstrName )
	{ _stscanf_s(wstrValue.c_str(), _T("%x"), m_kDefaultBtnTextColor.dwColor + 2); }
	else if( ATTR_BTN_DIS_TEXT_COLOR_DFT == wstrName )
	{ _stscanf_s(wstrValue.c_str(), _T("%x"), m_kDefaultBtnTextColor.dwColor + 3); }

	/*else if( ATTR_BTN_UP_TEXT_COLOR_0 == wstrName )
	{ _stscanf_s(wstrValue.c_str(), _T("%x"), &m_dwBtnTextColor[0][0]); }
	else if( ATTR_BTN_OVER_TEXT_COLOR_0 == wstrName )
	{ _stscanf_s(wstrValue.c_str(), _T("%x"), &m_dwBtnTextColor[0][1]); }
	else if( ATTR_BTN_DOWN_TEXT_COLOR_0 == wstrName )
	{ _stscanf_s(wstrValue.c_str(), _T("%x"), &m_dwBtnTextColor[0][2]); }
	else if( ATTR_BTN_DIS_TEXT_COLOR_0 == wstrName )
	{ _stscanf_s(wstrValue.c_str(), _T("%x"), &m_dwBtnTextColor[0][3]); }

	else if( ATTR_BTN_UP_TEXT_COLOR_1 == wstrName )
	{ _stscanf_s(wstrValue.c_str(), _T("%x"), &m_dwBtnTextColor[1][0]); }
	else if( ATTR_BTN_OVER_TEXT_COLOR_1 == wstrName )
	{ _stscanf_s(wstrValue.c_str(), _T("%x"), &m_dwBtnTextColor[1][1]); }
	else if( ATTR_BTN_DOWN_TEXT_COLOR_1 == wstrName )
	{ _stscanf_s(wstrValue.c_str(), _T("%x"), &m_dwBtnTextColor[1][2]); }
	else if( ATTR_BTN_DIS_TEXT_COLOR_1 == wstrName )
	{ _stscanf_s(wstrValue.c_str(), _T("%x"), &m_dwBtnTextColor[1][3]); }*/
	REGIST_BTN_T_COLOR_SET(0);
	REGIST_BTN_T_COLOR_SET(1);
	REGIST_BTN_T_COLOR_SET(2);
	REGIST_BTN_T_COLOR_SET(3);
	REGIST_BTN_T_COLOR_SET(4);
	REGIST_BTN_T_COLOR_SET(5);
	REGIST_BTN_T_COLOR_SET(6);
	REGIST_BTN_T_COLOR_SET(7);
	REGIST_BTN_T_COLOR_SET(8);
	REGIST_BTN_T_COLOR_SET(9);
	REGIST_BTN_T_COLOR_SET(10);
	REGIST_BTN_T_COLOR_SET(11);
	REGIST_BTN_T_COLOR_SET(12);
	REGIST_BTN_T_COLOR_SET(13);
	REGIST_BTN_T_COLOR_SET(14);
	REGIST_BTN_T_COLOR_SET(15);
	REGIST_BTN_T_COLOR_SET(16);
	REGIST_BTN_T_COLOR_SET(17);
	REGIST_BTN_T_COLOR_SET(18);
	REGIST_BTN_T_COLOR_SET(19);
	REGIST_BTN_T_COLOR_SET(20);
	REGIST_BTN_T_COLOR_SET(21);
	REGIST_BTN_T_COLOR_SET(22);
	REGIST_BTN_T_COLOR_SET(23);
	REGIST_BTN_T_COLOR_SET(24);
	REGIST_BTN_T_COLOR_SET(25);
	REGIST_BTN_T_COLOR_SET(26);
	REGIST_BTN_T_COLOR_SET(27);
	REGIST_BTN_T_COLOR_SET(28);
	REGIST_BTN_T_COLOR_SET(29);
	REGIST_BTN_T_COLOR_SET(30);

	if (m_spRscMgr && bPrepare)
		m_spRscMgr->VPrepareResource(wstrValue);
}

bool CXUI_Config::RegSndInfo(int const iBtnType, E_MOUSE_EVENT_INDEX const eMEI, std::wstring const& rstrValue)
{
	auto ret = m_mapSndInfo.insert( std::make_pair(iBtnType, SOUND_INFO()) );

	switch(eMEI)
	{
	case MEI_X:{ret.first->second.wstrSndMouseOver = rstrValue;}break;
	case MEI_BTN_0:{	ret.first->second.wstrSndLBtnDown = rstrValue;}break;
	case MEI_BTN_1:{	ret.first->second.wstrSndRBtnDown = rstrValue;}break;
	default:	{return false;}break;
	}
	return true;
}

bool CXUI_Config::SndInfo(int const iBtnType, E_MOUSE_EVENT_INDEX const eMEI, std::wstring const* &pOutResult)const
{
	SOUND_INFO_HASH::const_iterator itor = m_mapSndInfo.find(iBtnType);

	if(itor!= m_mapSndInfo.end())
	{
		switch(eMEI)
		{
		case MEI_X:{pOutResult = &itor->second.wstrSndMouseOver;}break;
		case MEI_BTN_0:{pOutResult = &itor->second.wstrSndLBtnDown;}break;
		case MEI_BTN_1:{pOutResult = &itor->second.wstrSndRBtnDown;}break;
		default:	{return false;}break;
		}
	}

	if(pOutResult && pOutResult->size())
	{
		return true;
	}
	return false;
}

DWORD CXUI_Config::GetBtnTextColor(int i)
{
	return m_kDefaultBtnTextColor.Get(i);
}

DWORD CXUI_Config::GetBtnTextColorByType(int iType, int i)
{
	size_t const iVecIndex = static_cast<size_t>(iType);
	if ( iVecIndex < m_kBtnTextColor.size() )
	{
		return m_kBtnTextColor.at(iVecIndex).Get(i);
	}
	return 0xFFFFFFFF;
}

bool CXUI_Config::RegStaicInfo(int const iStaticType, E_STATIC_TYPE_INDEX const eSTI, std::wstring const& rstrValue)
{
	auto ret = m_mapStaticInfo.insert( std::make_pair(iStaticType, STATIC_INFO()) );

	bool bPrepare = false;
	switch(eSTI)
	{
	case STI_BODY		:{ret.first->second.wstrImgBody = rstrValue; bPrepare = true;}break;	
	case STI_BODY_W		:{ret.first->second.wstrImgBody_W = rstrValue;}break;	
	case STI_BODY_H		:{ret.first->second.wstrImgBody_H = rstrValue;}break;	

	case STI_TB			:{ret.first->second.wstrImgTB = rstrValue; bPrepare = true;}break;	
	case STI_TB_W		:{ret.first->second.wstrImgTB_W = rstrValue;}break;	
	case STI_TB_H		:{ret.first->second.wstrImgTB_H = rstrValue;}break;	

	case STI_LR			:{ret.first->second.wstrImgLR = rstrValue; bPrepare = true;}break;	
	case STI_LR_W		:{ret.first->second.wstrImgLR_W = rstrValue;}break;	
	case STI_LR_H		:{ret.first->second.wstrImgLR_H = rstrValue;}break;	

	case STI_CORNER		:{ret.first->second.wstrImgCorner = rstrValue; bPrepare = true;}break;	
	case STI_CORNER_W	:{ret.first->second.wstrImgCorner_W = rstrValue;}break;	
	case STI_CORNER_H	:{ret.first->second.wstrImgCorner_H = rstrValue;}break;	
	default:	{return false;}break;
	}

	if (m_spRscMgr && bPrepare)
		m_spRscMgr->VPrepareResource(rstrValue);

	return true;
}

bool CXUI_Config::StaticInfo(int const iStaticType, E_STATIC_TYPE_INDEX const eSTI, std::wstring const* &pOutResult)const
{
	STATIC_INFO_HASH::const_iterator itor = m_mapStaticInfo.find(iStaticType);

	if(itor!= m_mapStaticInfo.end())
	{
		switch(eSTI)
		{
		case STI_BODY		:{pOutResult = &itor->second.wstrImgBody;}break;	
		case STI_BODY_W		:{pOutResult = &itor->second.wstrImgBody_W;}break;	
		case STI_BODY_H		:{pOutResult = &itor->second.wstrImgBody_H;}break;	

		case STI_TB			:{pOutResult = &itor->second.wstrImgTB;}break;	
		case STI_TB_W		:{pOutResult = &itor->second.wstrImgTB_W;}break;	
		case STI_TB_H		:{pOutResult = &itor->second.wstrImgTB_H;}break;	

		case STI_LR			:{pOutResult = &itor->second.wstrImgLR;}break;	
		case STI_LR_W		:{pOutResult = &itor->second.wstrImgLR_W;}break;	
		case STI_LR_H		:{pOutResult = &itor->second.wstrImgLR_H;}break;	

		case STI_CORNER		:{pOutResult = &itor->second.wstrImgCorner;}break;	
		case STI_CORNER_W	:{pOutResult = &itor->second.wstrImgCorner_W;}break;	
		case STI_CORNER_H	:{pOutResult = &itor->second.wstrImgCorner_H;}break;

		default:	{return false;}break;
		}
	}

	if(pOutResult && pOutResult->size())
	{
		return true;
	}
	return false;
}