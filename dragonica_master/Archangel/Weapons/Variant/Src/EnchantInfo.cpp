#include "stdafx.h"
#include "EnchantInfo.h"
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
bool tagItemTimeInfo::ConvTime(DBTIMESTAMP &rkTime)const
{
	rkTime.year = (short)(year + 2000);
	rkTime.month = mon;
	rkTime.day = day;
	rkTime.hour = hour;
	rkTime.minute = min;
	rkTime.second = 0;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

tagEnchantInfo::tagEnchantInfo()
{
	Clear();
}

void tagEnchantInfo::Clear()
{
	::memset(this, 0, sizeof(*this));//가상함수 없으므로 이렇게 함.
}

BYTE tagEnchantInfo::PlusLv()const
{
	return static_cast<BYTE>(m_kPlusLv + (m_kPlusLvExt<<4));
}

void tagEnchantInfo::PlusLv(BYTE const kInLv)
{
	m_kPlusLv = (0x0F & kInLv);
	m_kPlusLvExt = ((0x10 & kInLv)>>4);
}

bool tagEnchantInfo::IsEmpty()const
{
	if(	!Field_1()
	&&	!Field_2() 
	&&	!Field_3()
	&&	!Field_4())
	{
		return true;
	}
	return false;
}

bool tagEnchantInfo::IsPlus()const
{
	if(PlusLv()
	&&	PlusType() )
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;	
}

bool tagEnchantInfo::IsMagic() const
{
//	return (Detail() > 3) ? true : false;
	return 0;
}

int tagEnchantInfo::OptionCount() const
{
	int iRet = 0;
	if(BasicType1()	&& BasicLv1()){++iRet;}
	if(BasicType2()	&& BasicLv2()){++iRet;}
	if(BasicType3()	&& BasicLv3()){++iRet;}
	if(BasicType4()	&& BasicLv4()){++iRet;}

	return iRet;
}

void tagEnchantInfo::BasicOptionTypes(std::list< int > &kOutCont) const
{
	if(BasicType1()	&& BasicLv1()){kOutCont.push_back((int)BasicType1());}
	if(BasicType2()	&& BasicLv2()){kOutCont.push_back((int)BasicType2());}
	if(BasicType3()	&& BasicLv3()){kOutCont.push_back((int)BasicType3());}
	if(BasicType4()	&& BasicLv4()){kOutCont.push_back((int)BasicType4());}
}
void tagEnchantInfo::RareOptionTypes(std::list< int > &kOutCont) const
{
	if(RareOptType1() && RareOptLv1()){kOutCont.push_back((int)RareOptType1());}
	if(RareOptType2() && RareOptLv2()){kOutCont.push_back((int)RareOptType2());}
	if(RareOptType3() && RareOptLv3()){kOutCont.push_back((int)RareOptType3());}
	if(RareOptType4() && RareOptLv4()){kOutCont.push_back((int)RareOptType4());}
}

bool tagEnchantInfo::DestroySocket()
{
	if(GSS_GEN_NONE == GenSocketState())
	{
		return false;
	}

	GenSocketState(GSS_GEN_NONE);
	MonsterCard(0);
	return true;
}

bool tagEnchantInfo::EanbleGenSocket(E_ITEM_GRADE const kItemGrade) const
{
	switch(kItemGrade)
	{
	case IG_RARE:
	case IG_UNIQUE:
	case IG_ARTIFACT:
	case IG_LEGEND:
		{
			return true;
		}break;
	}

	return false;
}

bool tagEnchantInfo::AdjustValue()
{
	if(!BasicType1() || !BasicLv1())
	{
		BasicType1(0);
		BasicLv1(0);
	}

	if(!BasicType2() || !BasicLv2())
	{
		BasicType2(0);
		BasicLv2(0);
	}

	if(!BasicType3() || !BasicLv3())
	{
		BasicType3(0);
		BasicLv3(0);
	}

	if(!BasicType4() || !BasicLv4())
	{
		BasicType4(0);
		BasicLv4(0);
	}
	return true;
}
