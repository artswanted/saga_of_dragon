#pragma	once

#include	"defines.h"

//────────────────────────────────────────
//	패치
//────────────────────────────────────────
//	온라인 패치
bool	PatchIDCheck(std::wstring& PatchFileName, const bool IsOnline = true);
// Patch.ID를 읽어 화면에 현재 ver를 보여준다
bool	LoadPatchIDVerFromFile();	
//────────────────────────────────────────
//	기타
//────────────────────────────────────────

