#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_GLOBAL_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_GLOBAL_H

extern bool GetDefString(int const iTextNo, std::wstring& rkOut);
extern bool GetDefString( int const iTextNo, BM::vstring& rkOut );
extern bool GetItemString(int const iItemNo, std::wstring & rkOut);
extern HRESULT ReadRealmStateFromIni(LPCTSTR lpszFilename);

extern bool IsUseLevelRank();

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_GLOBAL_H