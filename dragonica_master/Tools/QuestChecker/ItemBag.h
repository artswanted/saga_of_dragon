#pragma once

namespace ItemBag
{
	typedef std::map< int, std::wstring > CONT_MEMO;
	typedef std::map< std::pair< int, short >, std::wstring > CONT_BAG_MEMO;
	extern CONT_BAG_MEMO kContBagMemo; // DB에 입력된 기획자 메모, 백
	extern CONT_MEMO kContBagGrpMemo; // DB에 입력된 기획자 메모, 백그룹
	extern CONT_MEMO kContContainerMemo; // DB에 입력된 기획자 메모, 컨테이너

	void CreateItemBagDlg(HWND const hMainDlg, BOOL const bEnable);
	BOOL CALLBACK ItemBagDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
	void MakeItemBagResultProcess();
};