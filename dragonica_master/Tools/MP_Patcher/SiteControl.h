#pragma once

#include "defines.h"

extern int const SITE_LIST_ID;
extern int const NOT_EXIST_SV;

struct tagListBtnInfo
{
	int ID;
	int T_X;
	int T_Y;
	int iType;
	int iShape;
	std::wstring BgName;
};

struct tagSiteInfo
{
	tagSiteInfo()
		: kSVName(), kSVIP(), iSVPort(0), iSVPing(0), iSVIndex(0)
	{
	}

	std::wstring kSVName;
	std::wstring kSVIP;
	int	iSVPort;
	int iSVPing;
	int iSVIndex;

	bool operator ==(tagSiteInfo const& rhs) const
	{
		return	kSVName == rhs.kSVName
			&&	kSVIP == rhs.kSVIP
			&&	iSVPort == rhs.iSVPort
			&&	iSVIndex == rhs.iSVIndex;
	}

	bool IsEmpty() const
	{
		return	kSVName.empty()
			&&	kSVIP.empty()
			&&	0 == iSVPort
			&&	0 == iSVPing
			&&	0 == iSVIndex;
	}
};

struct tagSiteControlInfo
{
	tagSiteControlInfo() : wX(0), wY(0), wWidth(0), wItemCount(3){};
	WORD	wX, wY, wWidth;
	WORD	wItemCount;
};

typedef std::map< int, tagSiteInfo >	CONT_SITE_INFO;
typedef std::list< tagSiteInfo > ContSiteInfoList;


class CCTButton;
class SiteControl
{
public:
	typedef	std::map< UINT, CCTButton* >	CONT_BUTTON;
	typedef std::map< int, int >			CONT_BTN_TO_SITE;

	bool ParseXML(HWND hParent, HINSTANCE hInstance, TiXmlElement const* pkElement);
	bool Initialize();
	bool DrawItem(LPDRAWITEMSTRUCT lpDIS);
	bool Command(WPARAM wParam, LPARAM lParam);
	bool CheckMouseOver(int iX, int iY);
	void SetParentBitmap(CHBitmap* pBitmap);

	void Visible(bool bIsVisible);
	bool Visible() const { return m_bIsVisible; };

	static tagSiteInfo const GetSelectedSiteInfo();
	static tagSiteInfo const GetFirstSiteInfo();
	bool IsSelectedSiteButton(int const iCtrlID);
	void Clear();

	SiteControl();
	~SiteControl(void);
protected:
	void SetSiteSlot(int const iCurrent);

private:
	HWND				m_hParent;
	HINSTANCE			m_hInstance;
	CONT_BUTTON			m_kBtnContainer;
	CONT_BTN_TO_SITE	m_kBtnToSiteContainer;
	CONT_BUTTON			m_kPageBtnContainer;
	CONT_SITE_INFO		m_kSiteContainer;
	tagSiteControlInfo	m_kControlInfo;
	bool	m_bIsVisible;
	int		m_kStartServer;
	static tagSiteInfo	m_kSelectServer;
	static ContSiteInfoList m_kContAllSite;
};

extern SiteControl g_kSiteControl1;
extern SiteControl g_kSiteControl2;