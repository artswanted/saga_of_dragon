#include "stdafx.h"
#include "PgGroundItemBox.h"
//#include "ItemDefMgr.h"
#include "PgControlDefMgr.h"

PgGroundItemBox::PgGroundItemBox(void)
{
	Money(0);
}

PgGroundItemBox::~PgGroundItemBox(void)
{
}

void PgGroundItemBox::Init()
{
	CUnit::Init();

	Money(0);

	SetID(BM::GUID::Create());//자체 GUID
	m_kOwners.clear();
	
	CreateDate(BM::GetTime32());//생성 날짜.
	m_kItemArray.clear();
	m_kActionInstanceID = 0;

	//Owner, Pos, Item 이게 필요함.
	AdjustImage();
	IsInstanceItem(false);
	IsDropBearItem(false);
	CollectRemainTime(0);
	Team(0);
	m_kGenInfo.Clear();
}

void PgGroundItemBox::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
{
	CUnit::WriteToPacket(rkPacket, kWriteType);

	rkPacket.Push(m_kOwners);//오너들
	rkPacket.Push(GetPos());//포지션
	rkPacket.Push(m_kCreateDate);//생성 날짜.
	PU::TWriteArray_M(rkPacket, m_kItemArray);
	rkPacket.Push(m_kCaller);
	rkPacket.Push(m_kActionInstanceID);
	rkPacket.Push(m_kMoney);
	rkPacket.Push(m_kIsInstanceItem);
	rkPacket.Push(m_kIsDropBearItem);
	rkPacket.Push(m_kCollectRemainTime);
}

EWRITETYPE PgGroundItemBox::ReadFromPacket(BM::Stream &rkPacket)
{
	EWRITETYPE kWriteType = CUnit::ReadFromPacket(rkPacket);
	
	POINT3 ptPos;
	m_kOwners.clear();
	rkPacket.Pop(m_kOwners);
	rkPacket.Pop(ptPos);//포지션
	rkPacket.Pop(m_kCreateDate);//생성 날짜.
	PU::TLoadArray_M(rkPacket, m_kItemArray);
	rkPacket.Pop(m_kCaller);
	rkPacket.Pop(m_kActionInstanceID);
	rkPacket.Pop(m_kMoney);
	rkPacket.Pop(m_kIsInstanceItem);
	rkPacket.Pop(m_kIsDropBearItem);
	rkPacket.Pop(m_kCollectRemainTime);

	SetPos(ptPos);

	AdjustImage();
	return kWriteType;
}

void PgGroundItemBox::Send(BM::Stream const &Packet, E_SENDTYPE_TYPE eType)
{//없음.
}

void PgGroundItemBox::Invalidate()
{
	Init();
}

bool PgGroundItemBox::IsCheckZoneTime(DWORD dwElapsed) 
{//얘는 움직이질 않아
	return true;
}

void PgGroundItemBox::SendAbil(const EAbilType eAT, E_SENDTYPE_TYPE eType) 
{//어빌 없음
}

int PgGroundItemBox::Tick(unsigned long ulElapsedTime, SActArg *pActArg) 
{//뭐 할거 있겠나? --> 소유권 풀어주는건 넣어도 되겠다
	return 0; 
}

void PgGroundItemBox::SendAbiles(WORD const* pkAbils, BYTE byNum, E_SENDTYPE_TYPE eType)
{//어빌 없음
}

int PgGroundItemBox::CallAction(WORD wActCode, SActArg *pActArg)
{//액션 없음
	return 0;
}

void PgGroundItemBox::SendAbil64(const EAbilType eAT, E_SENDTYPE_TYPE eType)
{//어빌이 없는데?
}

int PgGroundItemBox::AdjustImage()
{
	int iResNo = 0;
	switch(m_kItemArray.size())
	{//아이템이 한개도 없으면 곤란.
	case 0:
		{
			int const iMoney = Money();
			if(iMoney)//돈만 있네.
			{
				if(10 > iMoney )
				{
					iResNo = 400001;//코인. ClassNo.xml 에 있는 값 쓴다.
				}
				else if(50 > iMoney)
				{
					iResNo = 400005;//코인. ClassNo.xml 에 있는 값 쓴다.
				}
				else
				{
					iResNo = 400008;//코인. ClassNo.xml 에 있는 값 쓴다.
				}
			}
			else
			{
				iResNo = 400001;//이런일은 없어야 된다.
			}
		}break;
	case 1:
		{//아이템이 하나면 대표 이미지.
			GET_DEF(CItemDefMgr, kItemDefMgr);
			PgBase_Item const &rkItem = m_kItemArray.front();
			CItemDef const *pDef = kItemDefMgr.GetDef(rkItem.ItemNo());
			
			if(pDef)
			{
				iResNo = pDef->GetAbil(AT_DROPITEMRES);
				if (0==iResNo)
				{
					iResNo = 400009;//기본 상자
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Can't Find Def") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Get ItemDef Failed!"));
			}
		}break;
	default:
		{//기타 상황은 박스로 처리.
			iResNo = 400001;////드랍박스인데 이건 하드코딩.
		}break;
	}

	SetAbil(AT_CLASS, iResNo);
	return iResNo;
}

bool PgGroundItemBox::AddItem(PgBase_Item const &kItem)
{//갯수 제한 같은거 해야 하지 않겠어?
	m_kItemArray.push_back(kItem);
	AdjustImage();
	return true;
}

bool PgGroundItemBox::AddMoney(int const iMoney)
{
	Money(Money() + iMoney);
	AdjustImage();
	return true;
}

HRESULT PgGroundItemBox::PopItem(PgBase_Item& kItem, bool const bIsTest)
{
	if(m_kItemArray.size())
	{
		kItem = m_kItemArray.front();
		if(!bIsTest)
		{
			m_kItemArray.pop_front();
		}
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgGroundItemBox::Owner(VEC_GUID const& rkOwners)
{
	if( rkOwners.size() )
	{
		m_kOwners = rkOwners;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGroundItemBox::IsOwner(BM::GUID const &rkGuid)
{
	if ( m_kOwners.empty() )
	{
		// 소유권이 존재하지 않으면 모두가 오너
		return true;
	}

	VEC_GUID::const_iterator guid_iter = m_kOwners.begin();
	while(m_kOwners.end() != guid_iter)
	{
		VEC_GUID::value_type const& rkElement = (*guid_iter);
		if(rkGuid == rkElement)
		{
			return true;
		}
		++guid_iter;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}