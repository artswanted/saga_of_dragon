#include "stdafx.h"
#include "ItemBag.h"

#include "Variant/PgControlDefMgr.h"
#include "Variant/ItemBagMgr.h"
#include "ListControl.h"
#include "QCCore.h"
#include "CheckMgr.h"
#include "LogMgr.h"

//
namespace ItemBag
{
	HWND hWnd = NULL;

	//
	void CreateItemBagDlg(HWND const hMainDlg, BOOL const bEnable)
	{
		if( DevelopeFunction::iUseDebugDevelop )
		{
			::EnableWindow(GetDlgItem(hMainDlg, IDC_BUILD_BAG_RATE), bEnable);
		}
		else
		{
			::ShowWindow(GetDlgItem(hMainDlg, IDC_BUILD_BAG_RATE), SW_HIDE);
		}
	}

	typedef std::map< int, std::list< short > > CONT_BAG_AND_LEVEL;
	CONT_BAG_AND_LEVEL kContBagAndLevel;

	CONT_BAG_MEMO kContBagMemo; // DB에 입력된 기획자 메모, 백
	CONT_MEMO kContBagGrpMemo; // DB에 입력된 기획자 메모, 백그룹
	CONT_MEMO kContContainerMemo; // DB에 입력된 기획자 메모, 컨테이너

	std::wstring const kFileItemBagName(L"ItemBagRate.html");
	std::wstring const kFileItemBagGrpName(L"ItemBagGrpRate");
	std::wstring const kFileItemBagContainerName(L"ItemBagContainerRate"); // html은 나중에 붙임

	template< typename _T_CONT >
	std::wstring GetMemo(_T_CONT const& kCont, typename _T_CONT::key_type const& rkKey)
	{
		_T_CONT::const_iterator find_iter = kCont.find(rkKey);
		if( kCont.end() != find_iter )
		{
			return (*find_iter).second;
		}
		return std::wstring();
	}

	class HtmlItemBag
	{
	public:
		HtmlItemBag(){}
		~HtmlItemBag(){}
		static std::wstring MakeTableHeader()
		{
			BM::vstring kTempHtml;
			kTempHtml << L"<TH>" << TextHelper::GetText(700630) << L"</TH>\n";
			kTempHtml << L"<TH>" << TextHelper::GetText(700631) << L"</TH>\n";
			kTempHtml << L"<TH>" << TextHelper::GetText(700632) << L"</TH>\n";
			kTempHtml << L"<TH>" << TextHelper::GetText(700633) << L"</TH>\n";
			kTempHtml << L"<TH>" << TextHelper::GetText(700634) << L"</TH>\n";
			kTempHtml << L"<TH>" << TextHelper::GetText(700635) << L"</TH>\n";
			kTempHtml << L"<TH>" << TextHelper::GetText(700636) << L"</TH>\n";
			kTempHtml << L"<TH>" << TextHelper::GetText(700637) << L"</TH>\n";
			kTempHtml << L"<TH>" << TextHelper::GetText(700638) << L"</TH>\n";
			kTempHtml << L"<TH>" << TextHelper::GetText(700639) << L"</TH>\n";
			kTempHtml << L"<TH>" << TextHelper::GetText(700640) << L"</TH>\n";
			kTempHtml << L"<TH>" << TextHelper::GetText(700641) << L"</TH>\n";
			kTempHtml << L"<TH>" << TextHelper::GetText(700642) << L"</TH>\n";
			return kTempHtml;
		}
		static void MakeTableLineHead(BM::vstring& kTempHtml, BM::vstring& kTempLines, int& iLastBagNo, int& iCount, CONT_DEF_ITEM_BAG::key_type const& rkKey, std::wstring kHeadTail = std::wstring())
		{
			if( rkKey.first == iLastBagNo )
			{
				kTempLines << L"<TR>";
			}
			else
			{
				if( 0 != iLastBagNo )
				{
					kTempHtml << L"<TR><TD ROWSPAN='" << iCount << "' ALIGN='CENTER'>" << kTempLines;
					kTempLines = BM::vstring();
				}
				iLastBagNo = rkKey.first;
				iCount = 0;
				kTempLines << L"<A NAME='" << rkKey.first << L"'>#" << rkKey.first << kHeadTail << L"</A></TD>";
			}
		}
		static float GetRate(int const iRate, int const iTotalRate, float const iScale = 1.0f)
		{
			return (iRate * 10000.f) / (iTotalRate*100.f) * iScale;
		}
		static std::wstring GetRateStr(int const iRate, int const iTotalRate, float const iScale = 1.0f)
		{
			wchar_t szTemp[MAX_PATH] = {0, };
			swprintf_s(szTemp, L"%.2f", GetRate(iRate, iTotalRate, iScale));
			return std::wstring(szTemp);
		}
		static BM::vstring MakeTableLine(CONT_DEF_ITEM_BAG::key_type const& rkKey, PgItemBag const& rkItemBag, float const iScale = 1.0f)
		{
			BM::vstring kTemp;
			kTemp << L"<TD ALIGN='CENTER'>" << rkKey.second << L"</TD>";
			kTemp << L"<TD>" << GetMemo(kContBagMemo, rkKey) << L"</TD>";

			size_t const iMaxCount = 10;
			size_t iCur = 0;
			PgItemBag::BagElementCont::const_iterator ele_iter = rkItemBag.GetElements().begin();
			while( rkItemBag.GetElements().end() != ele_iter )
			{
				++iCur;
				PgItemBag::BagElementCont::value_type const& rkElements = (*ele_iter);
				
				kTemp << L"<TD ALIGN='CENTER'>";
				kTemp << DefStringHelper::GetItemName(rkElements.iItemNo) << L"<BR>(" << rkElements.iItemNo << L")<BR> *" << rkElements.nCount << L"<BR>";
				if( rkItemBag.TotalRaiseRate()
				&&	rkElements.nCount )
				{
					kTemp << L"<B>" << rkElements.nRate << L"</B>/" << rkItemBag.TotalRaiseRate() << L"<BR>";
					kTemp << L"(" << GetRateStr(rkElements.nRate, rkItemBag.TotalRaiseRate(), iScale) << L"%)";
				}
				else
				{
					kTemp << rkElements.nRate << L" / " << rkItemBag.TotalRaiseRate();
					kTemp << L"<BR>(0%)";
				}
				kTemp << L"</TD>";

				++ele_iter;
			}
			for( ; iMaxCount > iCur; ++ iCur )
			{
				kTemp << L"<TD>&nbsp;</TD>";
			}
			kTemp << L"</TR>\n";
			return kTemp;
		}
	};
	class HtmlItemBagGrp
	{
	public:
		static std::wstring MakeLine(CONT_DEF_ITEM_BAG_GROUP::key_type const& rkKey, CONT_DEF_ITEM_BAG_GROUP::mapped_type const& rkBagGrp, std::wstring kHeadTail = std::wstring(), float const iScale = 1.f)
		{
			GET_DEF(CItemBagMgr, kItemBagMgr);
			CONT_DEF_ITEM_BAG_GROUP const* pkDefBagGrp = NULL;
			CONT_DEF_SUCCESS_RATE_CONTROL const* pkDefSuccessRate = NULL;
			g_kTblDataMgr.GetContDef(pkDefBagGrp);
			g_kTblDataMgr.GetContDef(pkDefSuccessRate);

			BM::vstring kTempHtml;
			
			CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator rate_iter = pkDefSuccessRate->find(rkBagGrp.iSuccessRateNo);
			if( 0 != rkBagGrp.iSuccessRateNo
			&&	pkDefSuccessRate->end() != rate_iter )
			{
				CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const& rkRate = (*rate_iter).second;
				kTempHtml << L"<TR><TD ALIGN='CENTER'><A NAME='@" << rkKey << L"'>@" << rkKey << L"</A>" << kHeadTail << L"</TD>\n";
				kTempHtml << L"<TD>" << GetMemo(kContBagGrpMemo, rkKey) << L"</TD><TD>";
				//
				kTempHtml << L"<TABLE BOLDER='1' WIDTH='100%'>";
				kTempHtml << L"<TR>" << HtmlItemBag::MakeTableHeader() << L"</TR>";

				for( size_t iCur = 0; MAX_SUCCESS_RATE_ARRAY > iCur; ++iCur )
				{
					PgItemBag kItemBag;
					if( 0 != rkRate.aRate[iCur] )
					{
						BM::vstring kCurRate;
						kCurRate << L"(<A HREF='ItemBagRate.html#" << rkBagGrp.aiBagNo[iCur] << L"'>##</A>)";
						kCurRate << L"<BR><BR><B>" << rkRate.aRate[iCur] << L"</B>/" << rkRate.iTotal << L"";
						kCurRate << L"<BR>(" << HtmlItemBag::GetRateStr(rkRate.aRate[iCur], rkRate.iTotal, iScale) << L"%)";
						CONT_BAG_AND_LEVEL::const_iterator baglevel_iter = kContBagAndLevel.find(rkBagGrp.aiBagNo[iCur]);
						if( 0 != rkBagGrp.aiBagNo[iCur]
						&&	kContBagAndLevel.end() != baglevel_iter )
						{
							BM::vstring kTempLines;
							int iLastBagNo = 0;
							int iCount = 0;
							CONT_BAG_AND_LEVEL::mapped_type::const_iterator level_iter = (*baglevel_iter).second.begin();
							while( (*baglevel_iter).second.end() != level_iter )
							{
								CONT_DEF_ITEM_BAG::key_type const kKey = std::make_pair(rkBagGrp.aiBagNo[iCur], (*level_iter));
								if( S_OK == kItemBagMgr.GetItemBag(kKey.first, kKey.second, kItemBag) )
								{
									HtmlItemBag::MakeTableLineHead(kTempHtml, kTempLines, iLastBagNo, iCount, kKey, kCurRate);
									++iCount;
									kTempLines += HtmlItemBag::MakeTableLine(kKey, kItemBag, (HtmlItemBag::GetRate(rkRate.aRate[iCur], rkRate.iTotal, iScale)/100.f));
								}
								++level_iter;
							}
							HtmlItemBag::MakeTableLineHead(kTempHtml, kTempLines, iLastBagNo, iCount, std::make_pair(-1, 0), kCurRate); // flush
						}
					}
				}

				kTempHtml << L"</TABLE>\n";
				kTempHtml << L"</TD></TR>\n";
			}
			return kTempHtml;
		}
	};

	std::wstring MakeTrimFileName(std::wstring const& rkFileName, size_t const iCurFileCount)
	{
		BM::vstring const kFileName = BM::vstring(rkFileName) << iCurFileCount << L".html";
		return kFileName;
	}

	std::wstring MakeItemBag()
	{
		CONT_BAG_AND_LEVEL kTemp;
		BM::vstring kTempHtml;
		kTempHtml << L"<HTML><link href='style.css' rel='stylesheet' type='text/css'/><HEAD></HEAD><BODY>\n";
		kTempHtml << L"<TABLE BORDER='1' WIDTH='100%'><TR>" << HtmlItemBag::MakeTableHeader() << L"</TR>\n";

		int iLastBagNo = 0;
		int iCount = 0;
		BM::vstring kTempLines;

		GET_DEF(CItemBagMgr, kItemBagMgr);
		CONT_DEF_ITEM_BAG const* pkDefItemBag = NULL;
		g_kTblDataMgr.GetContDef(pkDefItemBag);
		CONT_DEF_ITEM_BAG::const_iterator iter = pkDefItemBag->begin();
		while( pkDefItemBag->end() != iter )
		{
			CONT_DEF_ITEM_BAG::key_type const& rkKey = (*iter).first;
			CONT_DEF_ITEM_BAG::mapped_type const& rkVal = (*iter).second;
			PgItemBag kItemBag;
			if( 0 != rkKey.first
			&&	S_OK == kItemBagMgr.GetItemBag(rkKey.first, rkKey.second, kItemBag) )
			{
				{
					auto kRet = kTemp.insert( std::make_pair(rkKey.first, CONT_BAG_AND_LEVEL::mapped_type()) );
					(*kRet.first).second.push_back(rkKey.second);
				}

				HtmlItemBag::MakeTableLineHead(kTempHtml, kTempLines, iLastBagNo, iCount, rkKey);
				++iCount;
				kTempLines += HtmlItemBag::MakeTableLine(rkKey, kItemBag);
			}
			++iter;
		}
		HtmlItemBag::MakeTableLineHead(kTempHtml, kTempLines, iLastBagNo, iCount, std::make_pair(-1, 0)); // flush
		kTempHtml << L"</TABLE></BODY></HTML>\n";
		kContBagAndLevel.swap(kTemp);
		return kTempHtml;
	}
	void MakeItemBagGrp(std::wstring const& rkFoler, std::wstring const& rkFileName)
	{
		CONT_DEF_ITEM_BAG_GROUP const* pkDefBagGrp = NULL;
		g_kTblDataMgr.GetContDef(pkDefBagGrp);

		std::wstring const kTargetFileName = rkFileName + L".html";

		BM::vstring kTempHtml, kListHtml;
		kTempHtml << L"<HTML><link href='style.css' rel='stylesheet' type='text/css'/><HEAD></HEAD><BODY>\n";
		kTempHtml << L"<TABLE BORDER='1' WIDTH='100%'><TR>\n";
		kTempHtml << L"<TH>" << TextHelper::GetText(700643) << L"</TH>\n";
		kTempHtml << L"<TH>" << TextHelper::GetText(700632) << L"</TH>\n";
		kTempHtml << L"<TH>" << TextHelper::GetText(700645) << L"</TH></TR>\n";
		kListHtml << kTempHtml;
		CONT_DEF_ITEM_BAG_GROUP::const_iterator iter = pkDefBagGrp->begin();
		while( pkDefBagGrp->end() != iter )
		{
			CONT_DEF_ITEM_BAG_GROUP::key_type const& rkKey = (*iter).first;
			CONT_DEF_ITEM_BAG_GROUP::mapped_type const& rkBagGrp = (*iter).second;
			{
				kListHtml << L"<TR><TD ALIGN='RIGHT'>@" << rkKey << L"</TD>";
				kListHtml << L"<TD>" << GetMemo(kContBagGrpMemo, rkKey) << L"</TD>";
				kListHtml << L"<TD>(<A HREF='" << kTargetFileName << L"#@"<< rkKey << L"'>@@</A>)</TD></TR>";
			}
			kTempHtml << HtmlItemBagGrp::MakeLine(rkKey, rkBagGrp);
			++iter;
		}
		std::wstring const kTempTail = L"</TABLE></BODY></HTML>\n";
		kTempHtml << kTempTail;
		kListHtml << kTempTail;
		HtmlUtil::MakeFile(rkFoler + kTargetFileName, kTempHtml);
		HtmlUtil::MakeFile(rkFoler + rkFileName + L"_List.html", kListHtml);
	}
	
	void MakeItemContainer(std::wstring const& rkFoler, std::wstring const& rkFileName)
	{
		CONT_DEF_ITEM_CONTAINER const* pkDefContainer = NULL;
		CONT_DEF_SUCCESS_RATE_CONTROL const* pkDefSuccessRate = NULL;
		CONT_DEF_ITEM_BAG_GROUP const* pkDefBagGrp = NULL;
		g_kTblDataMgr.GetContDef(pkDefContainer);
		g_kTblDataMgr.GetContDef(pkDefSuccessRate);
		g_kTblDataMgr.GetContDef(pkDefBagGrp);

		size_t const iMaxFileSize = 1024 * 1024; // * 2(unicode), 2 MByte 마다 파일을 자른다
		size_t iCurFileCount = 1;
		BM::vstring kTempHtml;
		BM::vstring kTempList;
		BM::vstring const kTempHtmlTail(std::wstring(L"</TABLE></BODY></HTML>\n"));
		kTempHtml << L"<HTML><link href='style.css' rel='stylesheet' type='text/css'/><HEAD></HEAD><BODY>\n";
		kTempHtml << L"<TABLE BORDER='1' WIDTH='100%'><TR>\n";
		kTempHtml << L"<TH>" << TextHelper::GetText(700644) << L"</TH>\n";
		kTempHtml << L"<TH>" << TextHelper::GetText(700645) << L"</TH></TR>\n";
		kTempList = kTempHtml;

		BM::vstring kTempHtmlBody;
		std::wstring kFileName = MakeTrimFileName(rkFileName, iCurFileCount);
		CONT_DEF_ITEM_CONTAINER::const_iterator iter = pkDefContainer->begin();
		while( pkDefContainer->end() != iter )
		{
			CONT_DEF_ITEM_CONTAINER::key_type const& rkKey = (*iter).first;
			CONT_DEF_ITEM_CONTAINER::mapped_type const& rkContainer = (*iter).second;

			kTempHtmlBody << L"<TR><TD ALIGN='CENTER'><A NAME='$" << rkKey << L"'/>$" << rkKey << L"</TD>\n";
			kTempHtmlBody << L"<TD><TABLE BORDER='1' WIDTH='100%'><TR>\n";
			kTempHtmlBody << L"<TH>" << TextHelper::GetText(700643) << L"</TH>\n";
			kTempHtmlBody << L"<TH>" << TextHelper::GetText(700645) << L"</TH></TR>\n";

			kTempList << L"<TR><TD ALIGN='RIGHT'><A NAME='$" << rkKey << L"'/>$" << rkKey << L"</TD>\n";
			kTempList << L"<TD>" << GetMemo(kContContainerMemo, rkKey) << L" <A HREF='" << kFileName << L"#$" << rkKey << L"'>$$</A></TD><TR>";

			CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator rate_iter = pkDefSuccessRate->find(rkContainer.iSuccessRateControlNo);
			if( pkDefSuccessRate->end() != rate_iter )
			{
				CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const& rkRate = (*rate_iter).second;
				for( size_t iCur = 0; MAX_SUCCESS_RATE_ARRAY > iCur; ++iCur )
				{
					BM::vstring kCurRate;
					kCurRate << L"(<A HREF='ItemBagGrpRate.html#@" << rkContainer.aiItemBagGrpNo[iCur] << L"'>@@</A>)";
					kCurRate << L"<BR><BR><B>" << rkRate.aRate[iCur] << L"</B>/" << rkRate.iTotal << L"";
					kCurRate << L"<BR>(" << HtmlItemBag::GetRateStr(rkRate.aRate[iCur], rkRate.iTotal) << L"%)";

					CONT_DEF_ITEM_BAG_GROUP::const_iterator baggrp_iter = pkDefBagGrp->find(rkContainer.aiItemBagGrpNo[iCur]);
					if( rkContainer.aiItemBagGrpNo[iCur]
					&&	pkDefBagGrp->end() != baggrp_iter )
					{
						kTempHtmlBody << HtmlItemBagGrp::MakeLine((*baggrp_iter).first, (*baggrp_iter).second, kCurRate, (HtmlItemBag::GetRate(rkRate.aRate[iCur], rkRate.iTotal) * 0.01f));
					}
					else
					{
						if( rkRate.aRate[iCur] )
						{
							kTempHtmlBody << L"<TR><TD ALIGN='CENTER'>0(@@)";
							kTempHtmlBody << L"<BR><BR><B>" << rkRate.aRate[iCur] << L"</B>/" << rkRate.iTotal << L"";
							kTempHtmlBody << L"<BR>(" << HtmlItemBag::GetRateStr(rkRate.aRate[iCur], rkRate.iTotal) << L"%)</TD><TD>&nbsp;</TD></TR>";
						}
					}
				}
			}
			kTempHtmlBody << L"</TABLE></TD></TR>";

			if( iMaxFileSize < kTempHtmlBody.size() )
			{
				HtmlUtil::MakeFile(rkFoler + kFileName, (kTempHtml + kTempHtmlBody + kTempHtmlTail));
				++iCurFileCount;
				kTempHtmlBody = BM::vstring();
				kFileName = MakeTrimFileName(rkFileName, iCurFileCount);
			}

			++iter;
		}
		HtmlUtil::MakeFile(rkFoler + kFileName, (kTempHtml + kTempHtmlBody + kTempHtmlTail));
		HtmlUtil::MakeFile((rkFoler + rkFileName + L"_List.html"), (kTempList + kTempHtmlTail));
	}

	void MakeItemBagResultProcess()
	{
		SReloadDef kReloadDef;
		g_kTblDataMgr.GetReloadDef(kReloadDef, PgControlDefMgr::EDef_ItemBagMgr);
		g_kControlDefMgr.Update(kReloadDef, PgControlDefMgr::EDef_ItemBagMgr);

		g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"ItemBag Rate Parsing.... making .html"));
		//
		std::wstring const kFolder(L".\\ItemDocument\\");
		::_wmkdir(kFolder.c_str());

		HtmlUtil::MakeCSS(kFolder);
		
		HtmlUtil::MakeFile(kFolder + kFileItemBagName, MakeItemBag());
		g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, (kFileItemBagName + L".... complete make a file").c_str()));
		
		MakeItemBagGrp(kFolder, kFileItemBagGrpName);
		g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, (kFileItemBagGrpName + L".... complete make a file").c_str()));
		
		MakeItemContainer(kFolder, kFileItemBagContainerName);
		g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, (kFileItemBagContainerName + L".... complete make a file").c_str()));

		//::ShellExecute(NULL, L"open", kFileName.c_str(), NULL, NULL, SW_SHOW);
		g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"ItemBag Rate Parsing.... Complete"));
	}
}