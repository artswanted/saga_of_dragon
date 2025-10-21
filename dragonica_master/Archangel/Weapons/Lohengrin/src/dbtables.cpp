#include "stdafx.h"
#include "dbtables.h"
#include "dbtables2.h"
#include "dbtables3.h"

//
tagProbability::tagProbability()
	: iResultNo(0), iCount(0), iProbability(0)
{
}
tagProbability::tagProbability(int const& riResultNo, int const& riCount, int const& riProbability)
	: iResultNo(riResultNo), iCount(riCount), iProbability(riProbability)
{
}
tagProbability::tagProbability(tagProbability const& rhs)
	: iResultNo(rhs.iResultNo), iCount(rhs.iCount), iProbability(rhs.iProbability)
{
}
tagProbability::~tagProbability()
{
}

bool tagProbability::IsFail() const
{
	return	0 == iResultNo
		&&	0 != iCount
		&&	0 != iProbability;
}
bool tagProbability::IsRight() const
{
	return	0 != iResultNo
		&&	0 != iCount
		&&	0 != iProbability;
}
bool tagProbability::IsWrong() const
{
	return !(IsFail() || IsRight());
}
bool tagProbability::operator <(tagProbability const& rhs) const
{
	return iProbability < rhs.iProbability;
}
void tagProbability::ReadFromPacket(BM::Stream& rkPacket)
{
	rkPacket.Pop(iResultNo);
	rkPacket.Pop(iCount);
	rkPacket.Pop(iProbability);
}
void tagProbability::WriteToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push(iResultNo);
	rkPacket.Push(iCount);
	rkPacket.Push(iProbability);
}
size_t tagProbability::min_size() const
{
	return sizeof(*this);
}

//
tagProbabilityBag::tagProbabilityBag()
	: iNo(0), eType(PT_ITEM), iTotalProbability(0), kContProbability(), iTotalSumProbability(0), i01NeedSaveIdx(0)
{
}
//tagProbabilityBag::tagProbabilityBag(int const& riNo, EProbablityType const& reType, int const& riTotalProbability, CONT_PROBABILITY const& rkContProbability)
//	: iNo(riNo), eType(reType), iTotalProbability(riTotalProbability), kContProbability(rkContProbability), iTotalSumProbability(0), i01NeedSaveIdx(0)
//{
//}
tagProbabilityBag::tagProbabilityBag(tagProbabilityBag const& rhs)
	: iNo(rhs.iNo), eType(rhs.eType), iTotalProbability(rhs.iTotalProbability), kContProbability(rhs.kContProbability), iTotalSumProbability(rhs.iTotalSumProbability), i01NeedSaveIdx(rhs.i01NeedSaveIdx)
{
}
tagProbabilityBag::~tagProbabilityBag()
{
}
bool tagProbabilityBag::Add(SProbability const& rkNew)
{
	if( !rkNew.IsWrong() )
	{
		kContProbability.push_back( rkNew );
		iTotalSumProbability += rkNew.iProbability;
		return true;
	}
	return false;
}
SProbability tagProbabilityBag::Get(int const iAddValue) const
{
	// 확율이 적은게 맨앞, 높은 확율을 가지는 것이 맨 뒤
	int const iRandResult = BM::Rand_Index(iTotalProbability);
	int const iRand = std::max(0, ((PT_ITEM == eType)? iRandResult + iAddValue: iRandResult)); // 0 ~ (Total-1)
	if( iRand < iTotalSumProbability )
	{
		int iCurSumProbability = 0;
		CONT_PROBABILITY::const_iterator iter = kContProbability.begin();
		while( kContProbability.end() != iter )
		{
			CONT_PROBABILITY::value_type const& rkProbability = (*iter);
			iCurSumProbability += rkProbability.iProbability;
			if( iRand < iCurSumProbability )
			{
				return SProbability(rkProbability);
			}
			++iter;
		}
	}
	else
	{
		// iRand >= iTotalSumProbability
		if( !kContProbability.empty() )
		{
			return SProbability(*kContProbability.rbegin());
		}
	}
	return SProbability();
}
void tagProbabilityBag::ReadFromPacket(BM::Stream& rkPacket)
{
	rkPacket.Pop(iNo);
	rkPacket.Pop(eType);
	rkPacket.Pop(iTotalProbability);
	PU::TLoadArray_M(rkPacket, kContProbability);
	rkPacket.Pop(iTotalSumProbability);
	rkPacket.Pop(i01NeedSaveIdx);
}
void tagProbabilityBag::WriteToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push(iNo);
	rkPacket.Push(eType);
	rkPacket.Push(iTotalProbability);
	PU::TWriteArray_M(rkPacket, kContProbability);
	rkPacket.Push(iTotalSumProbability);
	rkPacket.Push(i01NeedSaveIdx);
}
size_t tagProbabilityBag::min_size() const
{
	return sizeof(iNo) + sizeof(eType) + sizeof(iTotalProbability) + sizeof(size_t) + sizeof(iTotalSumProbability);
}