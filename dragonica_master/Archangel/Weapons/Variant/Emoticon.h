#ifndef WEAPON_VARIANT_EMOTION_H
#define WEAPON_VARIANT_EMOTION_H

#include "XUI/xui_font.h"

namespace Emoticon
{
	extern bool CheckNumber(wchar_t const kChar);
	extern std::wstring GetEmoticonNo(std::wstring & kStr,size_t & kPos);
	extern bool FilterEmoticon(PgPlayer const * pkPlayer,std::wstring const & kOrgString, std::set<int> & kCont = std::set<int>());
	extern bool AddGMEmoticon(PgPlayer const * pkPlayer,std::wstring& kOrgString, const int iEmoticonNo);
	extern bool AddVIPEmoticon(PgPlayer* pkPlayer, std::wstring& kOrgString, const int iEmoticonNo );
};

class PgInventory;
extern void GetExtraDataPackInfo( PgInventory *pkInv, BM::Stream *pkPacket, XUI::PgExtraDataPackInfo &kSendExtraDataPackInfo );

#endif // WEAPON_VARIANT_EMOTION_H