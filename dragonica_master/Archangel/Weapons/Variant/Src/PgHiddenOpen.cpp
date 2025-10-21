#include "StdAfx.h"
#include "constant.h"
#include "BM/Guid.h"
#include "TableDataManager.h"
#include "Variant/PgEventview.h"
#include "PgHiddenOpen.h"

PgHiddenOpen& PgHiddenOpen::operator=( PgHiddenOpen const &rhs )
{
	for(int i=0; i<MAX_HIDDENOPEN_BYTES; ++i)
	{
		m_byteHiddenOpen[i] = rhs.m_byteHiddenOpen[i];
	}
	kLastHiddenUpdate = rhs.kLastHiddenUpdate;
	return *this;
}

void PgHiddenOpen::Init()
{
	for(int i=0; i<MAX_HIDDENOPEN_BYTES; ++i)
	{
		m_byteHiddenOpen[i] = 0;
	}
	g_kEventView.GetLocalTime( kLastHiddenUpdate );
}

bool const PgHiddenOpen::IsComplete(int const iIdx) const
{
	int iByteOffset = 0;
	BYTE bValue = 0;

	if(CalcIDToOffset(iIdx, iByteOffset, bValue))
	{
		return ((m_byteHiddenOpen[iByteOffset] & bValue) == bValue);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgHiddenOpen::Complete(int const iIdx)
{
	int iByteOffset1 = 0;
	BYTE bValue1 = 0;

	if( CalcIDToOffset(iIdx, iByteOffset1, bValue1) )
	{
		m_byteHiddenOpen[iByteOffset1] |= bValue1;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgHiddenOpen::Reset(int const iIdx)
{
	int iByteOffset1 = 0;
	BYTE bValue1 = 0;

	if( CalcIDToOffset(iIdx, iByteOffset1, bValue1) )
	{
		m_byteHiddenOpen[iByteOffset1] &= ~bValue1;		
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgHiddenOpen::WriteToPacket(BM::Stream & kPacket) const
{
	kPacket.Push( m_byteHiddenOpen, MAX_HIDDENOPEN_BYTES );
	kPacket.Push( kLastHiddenUpdate );
}

void PgHiddenOpen::ReadFromPacket(BM::Stream & kPacket)
{
	kPacket.PopMemory( m_byteHiddenOpen, MAX_HIDDENOPEN_BYTES );
	kPacket.Pop( kLastHiddenUpdate );
}

bool PgHiddenOpen::CalcIDToOffset(int const iIdx, int & iByteOffset, BYTE & bValue) const
{
	if(iIdx < MAX_HIDDENOPEN_NUM)
	{
		int iBitPos = iIdx * HIDDENOPEN_BIT_NUM;
		iByteOffset = (iBitPos / 8);
		bValue = (0x01 << (iBitPos % 8));
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
