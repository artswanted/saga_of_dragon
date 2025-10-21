#include "stdafx.h"
#include "Lohengrin/dbtables.h"
#include "Lohengrin/VariableContainer.h"
#include "TableDataManager.h"
#include "PgPlayer.h"
#include "Emoticon.h"

namespace Emoticon
{

bool CheckNumber(wchar_t const kChar)
{
	if(kChar < '0' || kChar > '9')
	{
		return false;
	}
	return true;
}

std::wstring GetEmoticonNo(std::wstring & kStr,size_t & kPos)
{
	for(size_t k = kPos;k < kStr.length();++k)
	{
		if('#' != kStr[k])
		{
			continue;
		}

		size_t const sidx = k+1;

		if(kStr.length() <= sidx)
		{
			kPos = std::wstring::npos;
			return std::wstring();
		}

		if(false == CheckNumber(kStr[sidx]))
		{
			continue;
		}

		std::wstring kCopyString;

		for(size_t i = sidx;i < kStr.length();++i)
		{
			if(true == CheckNumber(kStr[i]))
			{
				continue;
			}

			kPos = i;
			kCopyString.assign(&kStr.at(sidx),(i - sidx));
			return kCopyString;
		}

		kPos = kStr.length();
		kCopyString.assign(&kStr.at(sidx),kPos - sidx);
		return kCopyString;
	}

	kPos = std::wstring::npos;
	return std::wstring();
}

bool FilterEmoticon(PgPlayer const * pkPlayer,std::wstring const & kOrgString, std::set<int> & kCont)
{
	CONT_EMOTION const * pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if(NULL == pkDef || (true == pkDef->empty()))
	{// 검사용 컨테이너가 없거나 비었으면 검사 하지 않는다.
		return true;
	}

	std::wstring kCopyString = kOrgString;

	if(true == kCopyString.empty())
	{
		return true;
	}

//	std::set<int> kCont;
	size_t kPos = 0;

	while(true)
	{
		std::wstring kString = GetEmoticonNo(kCopyString,kPos);
		if(std::wstring::npos != kPos)
		{
			kCont.insert(static_cast<int>(_wtof(kString.c_str())));
			continue;
		}
		break;
	}

	for(std::set<int>::const_iterator iter = kCont.begin();iter != kCont.end();++iter)
	{
		CONT_EMOTION::const_iterator finditer = pkDef->find(CONT_EMOTION::key_type(ET_EMOTICON,(*iter)));
		if(finditer == pkDef->end())
		{
			return false;
		}

		if(0 == (*finditer).second)
		{
			// 0 번 그룹은 무료!!
			continue;
		}

		if(false == pkPlayer->Emoticon().Get((*finditer).second))
		{
			return false;
		}
	}

	return true;
}

bool AddGMEmoticon(PgPlayer const * pkPlayer,std::wstring& kOrgString, const int iEmoticonNo)
{
	if( !pkPlayer )
	{
		return false;
	}

	if( 0 < pkPlayer->GMLevel() )
	{
		BM::vstring kEmoticon;
		kEmoticon = BM::vstring(_T("#")) + iEmoticonNo;
		kEmoticon += BM::vstring(_T(" "));

		std::wstring kNewString = kEmoticon + kOrgString;
		kOrgString = kNewString;
	}
	else
	{
		return false;
	}

	return true;
}

bool AddVIPEmoticon(PgPlayer* pkPlayer, std::wstring& kOrgString, const int iEmoticonNo )
{
	if( NULL == pkPlayer 
		|| 0 == iEmoticonNo )
	{
		return false;
	}

	if( NULL == pkPlayer->GetPremium().GetType<S_PST_VIP_ChatDisplay>() )
	{
		return false;
	}

	BM::vstring kEmoticon;
	kEmoticon = BM::vstring(_T("#")) + iEmoticonNo;
	kEmoticon += BM::vstring(_T(" "));

	std::wstring kNewString = kEmoticon + kOrgString;
	kOrgString = kNewString;

	return true;
}

};//namespace Emotion

void GetExtraDataPackInfo( PgInventory *pkInv, BM::Stream *pkPacket, XUI::PgExtraDataPackInfo &kSendExtraDataPackInfo )
{
	XUI::PgExtraDataPackInfo kExtraDataPackInfo;
	kExtraDataPackInfo.PopFromPacket(*pkPacket);	

	if( !pkInv )
	{
		return;
	}

	size_t const iCount = kExtraDataPackInfo.GetExtraDataPackCount();
	for( size_t i=0; i<iCount; i++)
	{					
		XUI::CONT_EXTRA_DATA	&kDataCont = kExtraDataPackInfo.m_kExtraDataPackCont[i].m_kExtraDataCont;

		size_t const iCount2 = kDataCont.size();

		for( size_t j=0; j<iCount2; j++)
		{
			SItemPos kItemPos;
			XUI::PgExtraData const &kData = kDataCont[j];
			::memcpy(&kItemPos, &kData.m_kBinaryValue.at(0), sizeof(kItemPos));

			PgBase_Item kItem;
			if(S_OK == pkInv->GetItem(kItemPos, kItem))
			{
				if( !kItem.IsEmpty() )
				{
					if( (0 != kItem.ItemNo()) && (0 != kItem.Count()) )
					{
						GET_DEF(CItemDefMgr, kItemDefMgr);
						CItemDef const *pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
						if( pItemDef )
						{
							BM::Stream kItemPacket;
							kItem.WriteToPacket(kItemPacket);

							std::vector<char> kBinaryValue = kItemPacket.Data();

							XUI::PgExtraDataPack kSendExtraDataPack;
							kSendExtraDataPack.AddExtraData(_T("IT"), kBinaryValue);
							kSendExtraDataPackInfo.AddExtraDataPack(kSendExtraDataPack);
						}
					}
				}
			}
		}
	}
}
