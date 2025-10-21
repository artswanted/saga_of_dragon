#include "StdAfx.h"
#include "lwMissionMutator.h"
#include "Variant\PgMissionMutator.h"
#include "ServerLib.h"

MUTATOR_LIST pkMutatorsList;

static inline bool mutator_is_enabled(int iMutatorNo)
{
	return pkMutatorsList.find(iMutatorNo) != pkMutatorsList.end();
}

static inline void mutator_push(int iMutatorNo)
{
	if (pkMutatorsList.find(iMutatorNo) == pkMutatorsList.end())
		pkMutatorsList.insert(iMutatorNo);
}

static inline void mutator_pop(int iMutatorNo)
{
	pkMutatorsList.erase(iMutatorNo);
}

bool lwMissionMutator::RegisterWrapper(lua_State *pkState)
{
	using namespace lua_tinker;
	def(pkState, "GetMutatorPacket", &lwGetMutatorPacket);
	def(pkState, "GetMutatotSelectedCount", &lwGetMutatotSelectedCount);
	def(pkState, "MakeMutatorUI", &lwMakeMutatorUI);
	def(pkState, "OneClickMutatorCheckBox", &lwOneClickMutatorCheckBox);
	def(pkState, "ClearMutatorList", &lwClearMutatorList);
	def(pkState, "SelectAllMutation", &lwSelectAllMutation);
	def(pkState, "mutator_push", &mutator_push);
	def(pkState, "mutator_pop", &mutator_pop);
	def(pkState, "mutator_is_enabled", &mutator_is_enabled);
	return true;
}

void lwOneClickMutatorCheckBox(lwUIWnd kSelf)
{
	int iMutatorNo = kSelf.GetCustomData<int>();
	if( iMutatorNo )
	{
		if( pkMutatorsList.find(iMutatorNo) == pkMutatorsList.end() )
		{
			pkMutatorsList.insert(iMutatorNo);
		}
		else
		{
			pkMutatorsList.erase(iMutatorNo);
		}
	}
}

lwPacket lwGetMutatorPacket()
{
	lwPacket pkOutPacket = lwNewPacket(NULL);
	for(MUTATOR_LIST::iterator iter = pkMutatorsList.begin(); iter != pkMutatorsList.end(); iter++)
		pkOutPacket.PushInt(*iter);
	return pkOutPacket;
}

unsigned int lwGetMutatotSelectedCount()
{
	return pkMutatorsList.size();
}

void lwMakeMutatorUI()
{
	XUI::CXUI_Wnd *pkMainWnd = lwCallUI("SFRM_MUTATOR_SELECT_WND").GetSelf();
	if( !pkMainWnd )
		return;

	XUI::CXUI_List *pkMutatorList = dynamic_cast<XUI::CXUI_List *>(pkMainWnd->GetControl(L"LST_MUTATOR"));
	if( !pkMutatorList )
		return;

	CONT_DEF_MISSION_MUTATOR const *pkMutatorDef = NULL;
	g_kTblDataMgr.GetContDef(pkMutatorDef);
	if( !pkMutatorDef )
		return;

	for(CONT_DEF_MISSION_MUTATOR::const_iterator iter = pkMutatorDef->begin(); iter != pkMutatorDef->end(); iter++)
	{
		int iMutatorNo = iter->second.iMutatorNo;
		XUI::SListItem* pkItem = pkMutatorList->AddItem(BM::vstring(iMutatorNo));
		if( !pkItem || !pkItem->m_pWnd)
			continue;

		XUI::CXUI_Wnd *pkItemTitleWnd = pkItem->m_pWnd->GetControl(L"FRM_TEXT_TITLE");
		if(pkItemTitleWnd)
		{
			BM::vstring vstrMutatorName;
			{
				wchar_t const* wstrText = NULL;
				GetDefString(iter->second.iMutatorNameNo, wstrText);
				if(iter->second.iMutatorType == MISSION_MUTATOR_TYPE_POSITIVE)
					vstrMutatorName += "{C=0xFF4CA64C/}";
				else if(iter->second.iMutatorType == MISSION_MUTATOR_TYPE_NEGATIVE)
					vstrMutatorName += "{C=0xFFFAB2B2/}";
				else
					vstrMutatorName += "{C=0xFFFFFFFF/}";

				if(wstrText)
					vstrMutatorName += wstrText;
				else
					vstrMutatorName += iter->second.iMutatorNameNo;
			}
			pkItemTitleWnd->Text(vstrMutatorName);
		}

		{// About
			XUI::CXUI_Wnd *pkItemTextAbout = pkItem->m_pWnd->GetControl(L"FRM_TEXT_ABOUT");
			wchar_t const* wstrText = NULL;
			GetDefString(iter->second.iDescription, wstrText);
			if(wstrText)
				pkItemTextAbout->Text(wstrText);
		}
		XUI::CXUI_CheckButton *pkChk = dynamic_cast<XUI::CXUI_CheckButton *>(pkItem->m_pWnd->GetControl(L"CHK_MUTATOR_STATE"));
		pkChk->SetCustomData(&iMutatorNo, sizeof(iMutatorNo));

		if( !pkChk )
			continue;

		if( pkMutatorsList.find(iMutatorNo) != pkMutatorsList.end() )
			pkChk->Check(true);
	}
}

void lwClearMutatorList()  // not work now
{
	pkMutatorsList.clear();
	XUI::CXUI_Wnd *pkMutatorWnd = XUIMgr.Get(L"SFRM_MUTATOR_SELECT_WND");
	if( pkMutatorWnd )
	{
		XUI::CXUI_List *pkMutatorList = dynamic_cast<XUI::CXUI_List *>(pkMutatorWnd->GetControl(L"LST_MUTATOR"));
		for(unsigned int i = 0; i < pkMutatorList->GetContCount(); i++)
		{
			XUI::CXUI_Wnd *pkMutatorNode = pkMutatorList->GetContAt(i);
			if( pkMutatorNode )
			{
				XUI::CXUI_CheckButton *pkChk = dynamic_cast<XUI::CXUI_CheckButton *>(pkMutatorNode->GetControl(L"CHK_MUTATOR_STATE"));
				if( pkChk )
					pkChk->Check(false);
			}
		}
	}
}

void lwSelectAllMutation() // not work now
{
	pkMutatorsList.clear();

	XUI::CXUI_Wnd *pkMutatorWnd = XUIMgr.Get(L"SFRM_MUTATOR_SELECT_WND");
	if( pkMutatorWnd )
	{
		XUI::CXUI_List *pkMutatorList = dynamic_cast<XUI::CXUI_List *>(pkMutatorWnd->GetControl(L"LST_MUTATOR"));
		for(unsigned int i = 0; i < pkMutatorList->GetContCount(); i++)
		{
			XUI::CXUI_Wnd *pkMutatorNode = pkMutatorList->GetContAt(i);
			if( pkMutatorNode )
			{
				XUI::CXUI_CheckButton *pkChk = dynamic_cast<XUI::CXUI_CheckButton *>(pkMutatorNode->GetControl(L"CHK_MUTATOR_STATE"));
				if( pkChk )
				{
					pkChk->Check(true);
					int iTemp = 0;
					pkChk->GetCustomData(&iTemp, sizeof(iTemp));
					pkMutatorsList.insert(iTemp);
				}
			}
		}
	}
}