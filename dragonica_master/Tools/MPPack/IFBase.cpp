#include "StdAfx.h"
#include "IFBase.h"
#include "PackUtil.h"

namespace IFUtil
{
	void WindowSettingType1(HWND m_hWnd, int iCmdShow)
	{
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_TEXT1), iCmdShow);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_TEXT2), iCmdShow);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_TEXT3), iCmdShow);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_TEXT1), iCmdShow);
		::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_TEXT1), L"");
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_TEXT2), iCmdShow);
		::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_TEXT2), L"");
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_TEXT3), iCmdShow);
		::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_TEXT3), L"");
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_OPEN1), iCmdShow);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_OPEN2), iCmdShow);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_OPEN3), iCmdShow);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_START), iCmdShow);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_PRG_STATE), SW_HIDE);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_STATE), SW_HIDE);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_HIDE);
	}

	void WindowSettingType2(HWND m_hWnd)
	{
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_TEXT1), SW_SHOW);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_TEXT2), SW_HIDE);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_TEXT3), SW_SHOW);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_TEXT1), SW_SHOW);
		::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_TEXT1), L"");
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_TEXT2), SW_HIDE);
		::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_TEXT2), L"");
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_TEXT3), SW_SHOW);
		::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_TEXT3), L"");
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_OPEN1), SW_SHOW);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_OPEN2), SW_HIDE);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_OPEN3), SW_SHOW);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_START), SW_SHOW);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_PRG_STATE), SW_HIDE);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_STATE), SW_HIDE);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_HIDE);
	}

	void WindowSettingType3(HWND m_hWnd)
	{
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_TEXT1), SW_HIDE);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_TEXT2), SW_HIDE);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_TEXT3), SW_SHOW);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_TEXT1), SW_HIDE);
		::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_TEXT1), L"");
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_TEXT2), SW_HIDE);
		::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_TEXT2), L"");
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_TEXT3), SW_SHOW);
		::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_TEXT3), L"");
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_OPEN1), SW_HIDE);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_OPEN2), SW_HIDE);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_OPEN3), SW_SHOW);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_START), SW_SHOW);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_PRG_STATE), SW_HIDE);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_STATE), SW_HIDE);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_HIDE);
	}

	void SetVisiblePatchIDControlType1(HWND m_hWnd, bool bVisible)
	{
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_PATCHOUT), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_MAJOR), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_MAJOR), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_MINOR), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_MINOR), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_TINY), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_TINY), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_FIXID), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_FIXEDID), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_PATCHID), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_PREVID), false);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_PREVPATCHID), false);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_RDO_PREVID), false);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_RDO_PATCHID), false);
	}

	void SetVisiblePatchIDControlType2(HWND m_hWnd, bool bVisible)
	{
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_PATCHOUT), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_MAJOR), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_MAJOR), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_MINOR), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_MINOR), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_TINY), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_TINY), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_BTN_FIXID), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_FIXEDID), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_PATCHID), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_PREVID), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_STC_PREVPATCHID), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_RDO_PREVID), bVisible);
		::ShowWindow(GetDlgItem(m_hWnd, IDC_RDO_PATCHID), bVisible);
	}
};

void IFDefault::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType1(m_hWnd, SW_HIDE);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, false);
	::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_HIDE);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());
}

bool IFDefault::Input(WPARAM wParam, LPARAM lParam)
{
	return false;
}

void IFDiff::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType1(m_hWnd, SW_SHOW);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, false);
	::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_SHOW);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT1), Static1.c_str());
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT2), Static2.c_str());
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFDiff::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN1:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT1, L"");	}return true;
	case IDC_BTN_OPEN2:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT2, L"");	}return true;
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT3, L"");	}return true;
	}
	return false;
}

DWORD IFDiff::GetOption()
{
	return 0x00000000;
}

void IFPack::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType2(m_hWnd);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, true);
	//::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_SHOW);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT1), Static1.c_str());
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFPack::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN1:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT1, L"");	}return true;
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT3, L"");	}return true;
	}
	return false;
}

void IFCreateList::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType2(m_hWnd);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, false);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());
	::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_HIDE);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT1), Static1.c_str());
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFCreateList::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN1:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT1, L"");	}return true;
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT3, L"");	}return true;
	}
	return false;
}

DWORD IFCreateList::GetOption()
{
	return 0x00000000;
}

void IFHeaderMerge::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType1(m_hWnd, SW_SHOW);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, false);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());
	::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_HIDE);
	
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT1), Static1.c_str());
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT2), Static2.c_str());
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFHeaderMerge::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN1:	{ OpenSelectDialog(PackUtil::EODT_FILE_SELECT, m_hWnd, IDC_EDT_TEXT1, L"dat");	}return true;
	case IDC_BTN_OPEN2:	{ OpenSelectDialog(PackUtil::EODT_FILE_SELECT, m_hWnd, IDC_EDT_TEXT2, L"dat");	}return true;
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT3, L"");	}return true;
	}
	return false;
}

void IFBind::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType2(m_hWnd);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, true);
	//::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_SHOW);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT1), Static1.c_str());
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFBind::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN1:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT1, L"");	}return true;
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT3, L"");	}return true;
	}
	return false;
}

void IFUnBind::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType2(m_hWnd);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, false);
	//::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_SHOW);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT1), Static1.c_str());
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFUnBind::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN1:	{ OpenSelectDialog(PackUtil::EODT_FILE_SELECT, m_hWnd, IDC_EDT_TEXT1, L"dat");	}return true;
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT3, L"");	}return true;
	}
	return false;
}

void IFDMakeINB::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType2(m_hWnd);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, false);
	::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_HIDE);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT1), Static1.c_str());
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFDMakeINB::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN1:	{ OpenSelectDialog(PackUtil::EODT_FILE_SELECT, m_hWnd, IDC_EDT_TEXT1, L"INI");	}return true;
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT3, L"");	}return true;
	}
	return false;
}

void IFExportList::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType2(m_hWnd);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, false);
	::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_HIDE);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT1), Static1.c_str());
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFExportList::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN1:	{ OpenSelectDialog(PackUtil::EODT_FILE_SELECT, m_hWnd, IDC_EDT_TEXT1, L"dat");	}return true;
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT3, L"");	}return true;
	}
	return false;
}

void IFCreateID::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType3(m_hWnd);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, true);
	::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_HIDE);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFCreateID::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT3, L"");	}return true;
	}
	return false;
}

void IFDatVersionUp::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType3(m_hWnd);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, true);
	::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_HIDE);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFDatVersionUp::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FILE_SELECT, m_hWnd, IDC_EDT_TEXT3, L"dat");	}return true;
	}
	return false;
}

void IFDatVersionCheck::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType3(m_hWnd);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, true);
	::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_HIDE);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFDatVersionCheck::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FILE_SELECT, m_hWnd, IDC_EDT_TEXT3, L"*");	}return true;
	}
	return false;
}

void IFDatConvert::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType3(m_hWnd);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, true);
	::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_HIDE);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFDatConvert::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FILE_SELECT, m_hWnd, IDC_EDT_TEXT3, L"DAT");	}return true;
	}
	return false;
}

void IFMakeManualPatch::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType2(m_hWnd);
	IFUtil::SetVisiblePatchIDControlType2(m_hWnd, true);
	::ShowWindow(GetDlgItem(m_hWnd, IDC_LST_OPTION), SW_HIDE);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT1), Static1.c_str());
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFMakeManualPatch::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN1:	{ OpenSelectDialog(PackUtil::EODT_FILE_SELECT, m_hWnd, IDC_EDT_TEXT1, L"DAT");	}return true;
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT3, L"");	}return true;
	}
	return false;
}

void IFMakeAutoPatch::ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3)
{
	IFUtil::WindowSettingType1(m_hWnd, SW_SHOW);
	IFUtil::SetVisiblePatchIDControlType1(m_hWnd, true);

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_EDT_CONTENTS), Contents.c_str());

	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT1), Static1.c_str());
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT2), Static2.c_str());
	::SetWindowTextW(GetDlgItem(m_hWnd, IDC_STC_TEXT3), Static3.c_str());
}

bool IFMakeAutoPatch::Input(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_BTN_OPEN1:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT1, L"");	}return true;
	case IDC_BTN_OPEN2:	{ OpenSelectDialog(PackUtil::EODT_FILE_SELECT, m_hWnd, IDC_EDT_TEXT2, L"DAT");	}return true;
	case IDC_BTN_OPEN3:	{ OpenSelectDialog(PackUtil::EODT_FOLDER_SELECT, m_hWnd, IDC_EDT_TEXT3, L"");	}return true;
	}
	return false;
}