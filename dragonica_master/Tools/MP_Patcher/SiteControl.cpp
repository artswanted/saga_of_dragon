#include "SiteControl.h"
#include <commctrl.h>

wchar_t const* const DAT = L".//res.dat";
int const SITE_LIST_ID = 1280;
int const NOT_EXIST_SV = 9999;

tagSiteInfo	SiteControl::m_kSelectServer;
ContSiteInfoList SiteControl::m_kContAllSite;

SiteControl g_kSiteControl1;
SiteControl g_kSiteControl2;

SiteControl::SiteControl()
	: m_hParent(NULL)
	, m_hInstance(NULL)
	, m_bIsVisible(true)
	, m_kStartServer(0)
{
}

SiteControl::~SiteControl(void)
{
	Clear();
}

void SiteControl::Clear()
{
	CONT_BUTTON::iterator	iter = m_kBtnContainer.begin();
	while( iter != m_kBtnContainer.end() )
	{
		CONT_BUTTON::mapped_type& kElement = iter->second;
		SAFE_DELETE(kElement);
		iter = m_kBtnContainer.erase(iter);
	}
	
	iter = m_kPageBtnContainer.begin();
	while( iter != m_kPageBtnContainer.end() )
	{
		CONT_BUTTON::mapped_type& kElement = iter->second;
		SAFE_DELETE(kElement);
		iter = m_kPageBtnContainer.erase(iter);
	}
}

bool SiteControl::ParseXML(HWND hParent, HINSTANCE hInstance, TiXmlElement const* pkElement)
{//XML을 파싱한다
	m_hParent = hParent;
	m_hInstance = hInstance;

	while( pkElement )
	{
		char const* pcTagName = pkElement->Value();
		if( strcmp(pcTagName, "SITECONTROL") == 0 )
		{
			TiXmlAttribute const* pkAttr = pkElement->FirstAttribute();
			while( pkAttr )
			{
				char const* pcAttrName = pkAttr->Name();
				char const* pcAttrValue = pkAttr->Value();

				if( strcmp(pcAttrName, "X") == 0 )
				{
					m_kControlInfo.wX = static_cast<WORD>(atoi(pcAttrValue));
				}
				else if( strcmp(pcAttrName, "Y") == 0 )
				{
					m_kControlInfo.wY = static_cast<WORD>(atoi(pcAttrValue));
				}
				else if( strcmp(pcAttrName, "W") == 0 )
				{
					m_kControlInfo.wWidth = static_cast<WORD>(atoi(pcAttrValue));
				}
				else if( strcmp(pcAttrName, "BTN_COUNT") == 0 )
				{
					m_kControlInfo.wItemCount = static_cast<WORD>(atoi(pcAttrValue));
				}
				pkAttr = pkAttr->Next();
			}

			TiXmlElement const* pkSubEle = pkElement->FirstChildElement();
			while( pkSubEle )
			{
				char const* pcTagSubName = pkSubEle->Value();
				if( strcmp(pcTagSubName, "BTN") == 0 || strcmp(pcTagSubName, "PREV_BTN") == 0 || strcmp(pcTagSubName, "NEXT_BTN") == 0 )
				{
					tagListBtnInfo	BtnInfo;

					TiXmlAttribute const* pkAttr = pkSubEle->FirstAttribute();
					while( pkAttr )
					{
						char const* pcAttrName = pkAttr->Name();
						char const* pcAttrValue = pkAttr->Value();

						if( strcmp(pcAttrName, "ID") == 0 )
						{
							BtnInfo.ID = atoi(pcAttrValue);
						}
						else if( strcmp(pcAttrName, "TYPE") == 0 )
						{
							BtnInfo.iType = atoi(pcAttrValue);
						}
						else if( strcmp(pcAttrName, "STATE_CNT") == 0 )
						{
							BtnInfo.iShape = atoi(pcAttrValue);
						}
						else if( strcmp(pcAttrName, "IMG") == 0 )
						{
							BtnInfo.BgName = UNI(pcAttrValue);
						}
						pkAttr = pkAttr->Next();
					}

					//버튼을 먼저 추가한다
					CONT_BUTTON* pkContainer = NULL;
					if( strcmp(pcTagSubName, "BTN") == 0 )
					{
						pkContainer = &m_kBtnContainer;
					}
					else if( strcmp(pcTagSubName, "PREV_BTN") == 0 )
					{
						BtnInfo.iType = CCTButton::eBTN_PREV;
						pkContainer = &m_kPageBtnContainer;
					}
					else if( strcmp(pcTagSubName, "NEXT_BTN") == 0 )
					{
						BtnInfo.iType = CCTButton::eBTN_NEXT;
						pkContainer = &m_kPageBtnContainer;
					}

					if( !pkContainer )
					{
						return false;
					}

					CCTButton* pkElement = new CCTButton;
					auto Result = pkContainer->insert(std::make_pair(BtnInfo.ID, pkElement));
					if( Result.second )
					{	//이미지 정보를 불러온다
						CONT_BUTTON::mapped_type &kElement = Result.first->second;
						std::vector< char >	kData;
						g_PProcess.LoadRes(BtnInfo.BgName.c_str(), kData);
						kElement->Init(m_hInstance, m_hParent, BtnInfo.ID, BtnInfo.iType, POINT(), BtnInfo.iShape, 0, std::wstring(), std::wstring(), kData);
					}
					else
					{
						return false;
					}
				}
				else if( strcmp(pcTagSubName, "SITE") == 0 )
				{
					int iIDX = 0;
					tagSiteInfo	SiteInfo;
					TiXmlAttribute const* pkAttr = pkSubEle->FirstAttribute();
					while( pkAttr )
					{
						char const* pcAttrName = pkAttr->Name();
						char const* pcAttrValue = pkAttr->Value();

						if( strcmp(pcAttrName, "IDX") == 0 )
						{
							iIDX = atoi(pcAttrValue);
							SiteInfo.iSVIndex = iIDX;
						}
						else if( strcmp(pcAttrName, "NAME") == 0 )
						{
							SiteInfo.kSVName = UNI(pcAttrValue);
						}
						else if( strcmp(pcAttrName, "IP") == 0 )
						{
							SiteInfo.kSVIP = UNI(pcAttrValue);
						}
						else if( strcmp(pcAttrName, "PORT") == 0 )
						{
							SiteInfo.iSVPort = atoi(pcAttrValue);
						}
						pkAttr = pkAttr->Next();
					}
					auto Result = m_kSiteContainer.insert(std::make_pair(iIDX, SiteInfo));
					if( Result.second )
					{
						m_kContAllSite.push_back( SiteInfo );
						if( SiteControl::GetSelectedSiteInfo().IsEmpty() )
						{
							m_kSelectServer = SiteInfo;
						}
					}
					else
					{
						//중복
					}
				}
				pkSubEle = pkSubEle->NextSiblingElement();
			}
		}
		pkElement = pkElement->NextSiblingElement();
	}
	return Initialize();
}

bool SiteControl::Initialize()
{//컨트롤의 위치를 설정
	if( m_kSiteContainer.empty() )
	{
		return false;
	}

	int IDX = 0;
	CONT_SITE_INFO::iterator	Site_iter = m_kSiteContainer.begin();
	CONT_BUTTON::iterator		btn_iter = m_kBtnContainer.begin();
	while( btn_iter != m_kBtnContainer.end() )
	{
		CONT_BUTTON::mapped_type &kElement = btn_iter->second;
		int const iSlotWidth = m_kControlInfo.wWidth / m_kControlInfo.wItemCount;
		POINT Pt;
		Pt.x = m_kControlInfo.wX + (IDX * iSlotWidth);
		Pt.y = m_kControlInfo.wY;
		kElement->SetLoc(Pt);
		m_kBtnToSiteContainer.insert(std::make_pair(btn_iter->first, 0));
		++IDX;
		++btn_iter;
	}

	btn_iter = m_kPageBtnContainer.begin();
	while( btn_iter != m_kPageBtnContainer.end() )
	{
		CONT_BUTTON::mapped_type &kElement = btn_iter->second;

		POINT Pt;
		switch( kElement->GetType() )
		{
		case CCTButton::eBTN_PREV:
			{
				Pt.x = m_kControlInfo.wX - kElement->GetWidth();
			}break;
		case CCTButton::eBTN_NEXT:
			{
				Pt.x = m_kControlInfo.wX + m_kControlInfo.wWidth;
			}break;
		}
		Pt.y = m_kControlInfo.wY;
		kElement->SetLoc(Pt);
		if( m_kSiteContainer.size() <= m_kControlInfo.wItemCount )
		{
			kElement->Visible(false);
		}
		++btn_iter;
	}

	SetSiteSlot(0);
	return true;
}

bool SiteControl::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{//그리는 곳이다
	if( !m_bIsVisible )
	{
		return false;
	}

	CONT_BUTTON::iterator iter = m_kPageBtnContainer.find(lpDIS->CtlID);
	if( iter != m_kPageBtnContainer.end() )
	{
		CONT_BUTTON::mapped_type &kElement = iter->second;
		kElement->OwnerDraw(lpDIS);
	}

	iter = m_kBtnContainer.find(lpDIS->CtlID);
	if( iter != m_kBtnContainer.end() )
	{
		CONT_BUTTON::mapped_type &kElement = iter->second;
		kElement->OwnerDraw(lpDIS);
	}
	return true;
}

bool SiteControl::Command(WPARAM wParam, LPARAM lParam)
{
	int iCmdID = LOWORD(wParam);

	CONT_BUTTON::iterator iter = m_kPageBtnContainer.find(iCmdID);
	if( iter != m_kPageBtnContainer.end() )
	{//페이지 버튼을 눌렀다
		CONT_BUTTON::mapped_type &kElement = iter->second;

		switch( kElement->GetType() )
		{
		case CCTButton::eBTN_PREV:
			{
				SetSiteSlot(-1);
			}return true;
		case CCTButton::eBTN_NEXT:
			{
				SetSiteSlot(1);
			}return true;
		}
	}
	else
	{//아니다 사이트를 선택했다
		CONT_BUTTON::iterator iter = m_kBtnContainer.find(iCmdID);
		if( iter != m_kBtnContainer.end() )
		{
			CONT_BTN_TO_SITE::iterator	site_iter = m_kBtnToSiteContainer.find(iCmdID);
			if( site_iter != m_kBtnToSiteContainer.end() )
			{
				CONT_BTN_TO_SITE::mapped_type &kElement = site_iter->second;
				if( NOT_EXIST_SV == kElement )
				{
					return false;
				}
				CONT_SITE_INFO::const_iterator	info_iter = m_kSiteContainer.find(kElement);
				if( info_iter != m_kSiteContainer.end() )
				{
					m_kSelectServer = info_iter->second;
				}
				return true;
			}
		}
	}
	return false;
}

bool SiteControl::CheckMouseOver(int iX, int iY)
{
	CONT_BUTTON::iterator iter = m_kPageBtnContainer.begin();
	while( iter != m_kPageBtnContainer.end() )
	{
		CONT_BUTTON::mapped_type &kElement = iter->second;

		if( kElement->CheckMouseOver(iX, iY) )
		{
			return true;
		}
		++iter;
	}

	iter = m_kBtnContainer.begin();
	while( iter != m_kBtnContainer.end() )
	{
		CONT_BUTTON::mapped_type &kElement = iter->second;

		CONT_BTN_TO_SITE::iterator	site_iter = m_kBtnToSiteContainer.find(iter->first);
		if( site_iter != m_kBtnToSiteContainer.end() )
		{
			if( NOT_EXIST_SV != site_iter->second )
			{
				if( kElement->CheckMouseOver(iX, iY) )
				{
					return true;
				}
			}
		}
		++iter;
	}
	return true;
}

void SiteControl::SetParentBitmap(CHBitmap* pBitmap)
{
	CONT_BUTTON::iterator iter = m_kPageBtnContainer.begin();
	while( iter != m_kPageBtnContainer.end() )
	{
		CONT_BUTTON::mapped_type &kElement = iter->second;
		kElement->SetParentBitmap(pBitmap);
		++iter;
	}

	iter = m_kBtnContainer.begin();
	while( iter != m_kBtnContainer.end() )
	{
		CONT_BUTTON::mapped_type &kElement = iter->second;
		kElement->SetParentBitmap(pBitmap);
		++iter;
	}
}

void SiteControl::Visible(bool bIsVisible)
{//컨트롤을 보이거나 감추기
	m_bIsVisible = bIsVisible;

	CONT_BUTTON::iterator iter;
	if( m_kSiteContainer.size() > m_kControlInfo.wItemCount )
	{
		iter = m_kPageBtnContainer.begin();
		while( iter != m_kPageBtnContainer.end() )
		{
			CONT_BUTTON::mapped_type &kElement = iter->second;
			kElement->Visible(m_bIsVisible);
			++iter;
		}
	}

	iter = m_kBtnContainer.begin();
	while( iter != m_kBtnContainer.end() )
	{
		CONT_BUTTON::mapped_type &kElement = iter->second;
		kElement->Visible(m_bIsVisible);
		++iter;
	}
}

tagSiteInfo const SiteControl::GetSelectedSiteInfo()
{//선택한 사이트의 정보
	return m_kSelectServer;
}

tagSiteInfo const SiteControl::GetFirstSiteInfo()
{
	if( m_kContAllSite.empty() )
	{
		return tagSiteInfo();
	}
	return m_kContAllSite.front();
}

bool SiteControl::IsSelectedSiteButton(int const iCtrlID)
{
	CONT_BTN_TO_SITE::iterator	site_iter = m_kBtnToSiteContainer.find(iCtrlID);
	if( m_kBtnToSiteContainer.end() != site_iter )
	{
		CONT_BTN_TO_SITE::mapped_type const& rkElement = site_iter->second;
		if( NOT_EXIST_SV != rkElement )
		{
			CONT_SITE_INFO::const_iterator	info_iter = m_kSiteContainer.find(rkElement);
			if( m_kSiteContainer.end() != info_iter )
			{
				CONT_SITE_INFO::mapped_type const& rkSiteInfo = info_iter->second;
				if( rkSiteInfo == m_kSelectServer )
				{
					return true;
				}
			}
		}
	}
	return false;
}

void SiteControl::SetSiteSlot(int const iCurrent)
{//한번에 다 표시가 안될경우에 동작한다
	m_kStartServer += iCurrent;
	if( m_kStartServer < 0 )
	{
		m_kStartServer = 0;
		return;
	}

	if( m_kStartServer > m_kSiteContainer.size() - m_kControlInfo.wItemCount )
	{
		m_kStartServer = m_kSiteContainer.size() - m_kControlInfo.wItemCount;
	}

	CONT_SITE_INFO::iterator	site_iter = m_kSiteContainer.find(m_kStartServer);
	CONT_BTN_TO_SITE::iterator	BtoS_iter = m_kBtnToSiteContainer.begin();
	while( BtoS_iter != m_kBtnToSiteContainer.end() )
	{
		std::wstring kSVName;
		if( site_iter != m_kSiteContainer.end() )
		{
			BtoS_iter->second = site_iter->first;
			kSVName = site_iter->second.kSVName;
			++site_iter;
		}
		else
		{
			BtoS_iter->second = NOT_EXIST_SV;
		}

		CONT_BUTTON::iterator	iter = m_kBtnContainer.find(BtoS_iter->first);
		if( iter != m_kBtnContainer.end() )
		{
			CONT_BUTTON::mapped_type &kElement = iter->second;
			//kElement->SetBtnText(kSVName);
			kElement->SetBtnTextFlag(DT_CENTER | DT_BOTTOM);
			kElement->SetInvalidate();
		}
		++BtoS_iter;
	}
}