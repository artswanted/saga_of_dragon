// FileDownDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServerControl.h"
#include "FileDownDlg.h"
#include "PgNetwork.h"
#include "GeneralDlg.h"
#include "PgMCTTask.h"
#include "PgServerCmdMgr.h"

// CFileDownDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFileDownDlg, CDialog)

CFileDownDlg::CFileDownDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileDownDlg::IDD, pParent)
	, m_bUseOptionGroup(true)
	, m_bUseTypeGroup(true)
{
	
}

CFileDownDlg::~CFileDownDlg()
{
	m_kContDateSelectWnd.clear();
}

void CFileDownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_kChkType1);
	DDX_Control(pDX, IDC_CHECK2, m_kChkType2);
	DDX_Control(pDX, IDC_RADIO1, m_kRadioAllFile);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_kCtrlStartTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_kCtrlEndTime);
	DDX_Control(pDX, IDC_LIST1, m_kListDownload);
	DDX_Control(pDX, IDC_STATIC_TOTAL_INFO, m_kTextTotalInfo);
	DDX_Control(pDX, IDC_GROUP_OPTION, m_kGroupOption);
	DDX_Control(pDX, IDC_RADIO2, m_kRadioDateSelect);
	DDX_Control(pDX, IDC_STATIC_PERIOD, m_kTextPeriod);

	//! ListBox에 다운로드 정보를 넣어주자
	CONT_SERVER_STRING::iterator DownloadList_itor = m_kContDownloadList.begin();
	for( ; DownloadList_itor != m_kContDownloadList.end() ; ++DownloadList_itor )
	{
		m_kListDownload.AddString( DownloadList_itor->operator wchar_t const*() );
	}

	//! 체크박스와 스태틱 텍스트 정보 업데이트
	m_kTextTotalInfo.SetWindowText(m_wstrTotalInfo.c_str());
	m_kChkType1.SetWindowTextW(m_wstrTypeName1.c_str());
	m_kChkType2.SetWindowTextW(m_wstrTypeName2.c_str());
	DDX_Control(pDX, IDC_GROUP_TYPE, m_kGroupType);
}


BEGIN_MESSAGE_MAP(CFileDownDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFileDownDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFileDownDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO2, &CFileDownDlg::OnBnClickedDateSelect)
	ON_BN_CLICKED(IDC_RADIO1, &CFileDownDlg::OnBnClickedAllFile)
END_MESSAGE_MAP()

BOOL CFileDownDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_kRadioAllFile.SetCheck(true);//디폴트 AllFile

	//! 
	m_kContDateSelectWnd.push_back(&m_kCtrlStartTime);
	m_kContDateSelectWnd.push_back(&m_kCtrlEndTime);
	m_kContDateSelectWnd.push_back(&m_kTextPeriod);
	for(int i=0; i<m_kContDateSelectWnd.size(); ++i)
	{
		m_kContDateSelectWnd[i]->EnableWindow(false);
	}

	m_kGroupOption.ShowWindow(m_bUseOptionGroup);
	m_kRadioAllFile.ShowWindow(m_bUseOptionGroup);
	m_kRadioDateSelect.ShowWindow(m_bUseOptionGroup);
	m_kCtrlStartTime.ShowWindow(m_bUseOptionGroup);
	m_kCtrlEndTime.ShowWindow(m_bUseOptionGroup);
	m_kTextPeriod.ShowWindow(m_bUseOptionGroup);

	m_kChkType1.EnableWindow(m_bUseTypeGroup);
	m_kChkType2.EnableWindow(m_bUseTypeGroup);
	m_kGroupType.EnableWindow(m_bUseTypeGroup);

	return true;
}

void CFileDownDlg::Init(int iKeyValue)
{
	//! 각 파일 타입별 체크박스 이름을 초기화
	m_iKeyValue = iKeyValue;
	m_bUseTypeGroup = true;
	m_bUseOptionGroup = false;
	if(iKeyValue == FT_DUMP)
	{
		m_bUseTypeGroup = false;//! 덤프는 타입 옵션 없음.
		return;
	}
	else if(iKeyValue == FT_CATION_HACK)
	{
		m_wstrTypeName1 = L"Caution Log";
		m_wstrTypeName2 = L"Hack Log";
	}
	else
	{
		m_wstrTypeName1 = L"Info Log";
		m_wstrTypeName2 = L"CEL Log";
		m_bUseOptionGroup = true;//! info/Cel Log 는 날짜 선택 기능 사용 ( 미구현 )
	}

	SLogInfo kLogInfo;// 선택된 항목의 총 사이즈/카운트를 표시하기 위해 계산된 데이터를 얻는다
	g_kLogCopyMgr.Locked_GetTotalInfo(iKeyValue, kLogInfo);

	//! 파일 크기에 따라 KB/MB/GB 단위로 바꾸자
	double dFileSize1 = static_cast<double>(kLogInfo.m_dwSize1);
	std::wstring wstrFileType1 = ::TansformFileSize(dFileSize1);
	double dFileSize2 = static_cast<double>(kLogInfo.m_dwSize2);
	std::wstring wstrFileType2 = ::TansformFileSize(dFileSize2);
	
	//! 계산된 데이터를 스트링에 넣는다
	wchar_t wcharTotalInfo[80];
	_stprintf_s(wcharTotalInfo, _countof(wcharTotalInfo), _T("[TOTAL] %s : %0.1f %s / %s : %0.1f %s"), m_wstrTypeName1.c_str(), dFileSize1, wstrFileType1.c_str(), m_wstrTypeName2.c_str(), dFileSize2, wstrFileType2.c_str());
	m_wstrTotalInfo = wcharTotalInfo;
}
void CFileDownDlg::SetDownloadList(CONT_SERVER_STRING const& rkDownloadList, SLogCopyer::CONT_LOG_INFO const& rkLogInfo)
{
	if(rkDownloadList.empty())
	{//! 목록이 비어있으면 아무것도 안함.
		return;
	}

	m_kContDownloadList = rkDownloadList;
	m_kContLogInfo = rkLogInfo;
}

void CFileDownDlg::SetDownloadList(CONT_SERVER_STRING const& rkDownloadList, SLogCopyer::CONT_DUMP_INFO const& rkDumpInfo)
{
	if(rkDownloadList.empty())
	{//! 목록이 비어있으면 아무것도 안함.
		return;
	}

	m_kContDownloadList = rkDownloadList;
	m_kContDumpInfo = rkDumpInfo;
}
// CFileDownDlg 메시지 처리기입니다.

void CFileDownDlg::OnBnClickedOk()
{
	//! File Type 
	g_kLogCopyMgr.FileType(m_iKeyValue);

	if(m_iKeyValue == FT_DUMP)
	{//! 덤프는 refresh할때 이미 파일 정보가 함께 오기때문에, 걍 다운로드 태스크로 진행
		SLogCopyer::CONT_LOG_FILE_INFO kFileList;
		SLogCopyer::CONT_DUMP_INFO::iterator DumpInfo_itor = m_kContDumpInfo.begin();
		while(DumpInfo_itor != m_kContDumpInfo.end())
		{//! 컨테이너에서 모든 파일 정보를 뽑는다.
			kFileList.insert(DumpInfo_itor->second.begin(), DumpInfo_itor->second.end());
			++DumpInfo_itor;
		}
		
		//! 메시지에 담아서
		BM::CPacket* pkPacket = new BM::CPacket;
		PU::TWriteTable_AA(*pkPacket, kFileList);

		SEventMessage kEventMsg;	
		kEventMsg.PriType(PMET_SMC_ANS_LOG_FILE_INFO);
		kEventMsg.Push(pkPacket);
		g_kTask.PutMsg(kEventMsg);// 태스크로 보내자 
	}
	else
	{//! MMC에 파일 정보 요청 패킷을 보낸다
		g_kServerCmdMgr.OnReqLogFileInfo(m_iKeyValue, m_kChkType1.GetCheck(), m_kChkType2.GetCheck(), m_kContLogInfo);

		INFO_LOG( BM::LOG_LV2, L"REQ MCT_GET_LOG_FILE_INFO" );
	}
	OnOK();
}

void CFileDownDlg::OnBnClickedCancel()
{//! 취소 버튼
	OnCancel();
}

void CFileDownDlg::OnBnClickedDateSelect()
{//! 날짜 선택 라디오 버튼 클릭
	for(int i=0; i<m_kContDateSelectWnd.size(); ++i)
	{
		m_kContDateSelectWnd[i]->EnableWindow(true);
	}
}

void CFileDownDlg::OnBnClickedAllFile()
{//! 모든 파일 라디오 버튼 클릭
	for(int i=0; i<m_kContDateSelectWnd.size(); ++i)
	{
		m_kContDateSelectWnd[i]->EnableWindow(false);
	}
}
