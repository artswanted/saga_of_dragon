#ifndef WEAPON_LOHENGRIN_PACKET_PACKETSTRUCT4MAP_H
#define WEAPON_LOHENGRIN_PACKET_PACKETSTRUCT4MAP_H

// Packet naming postfix
// C = Client
// L = Login
// M = Map
// S = Switch
// T = Center
// N = Contents
// C2C = BroadCast

typedef struct tagNfyMapLoadC2C
{
	tagNfyMapLoadC2C( BM::GUID const kGuid=BM::GUID::NullData(), unsigned char const per=0 )
		:	kCharGuid( kGuid )
		,	ucPer( per )
	{}
	tagNfyMapLoadC2C( tagNfyMapLoadC2C const& rkPair )
		:	kCharGuid(rkPair.kCharGuid )
		,	ucPer( rkPair.ucPer)
	{}
	BM::GUID		kCharGuid;
	unsigned char	ucPer;				//(0~100)
} SNfyMapLoad;

typedef struct tagNfyJoinIndunM2N
{
	tagNfyJoinIndunM2N( BM::GUID const &kGuid=BM::GUID::NullData(), BM::GUID const &kMemGuid=BM::GUID::NullData(), __int64 const iExp=0 )
		:	kCharGuid(kGuid)
		,	kMemberGuid(kMemGuid)
		,	iNowExp(iExp)
	{}
	tagNfyJoinIndunM2N( tagNfyJoinIndunM2N const& rkPair )
		:	kCharGuid(rkPair.kCharGuid)
		,	kMemberGuid(rkPair.kMemberGuid)
		,	iNowExp(rkPair.iNowExp)
	{}
	BM::GUID	kCharGuid;
	BM::GUID	kMemberGuid;
	__int64		iNowExp;	
} SNfyJoinIndun;
typedef std::vector<SNfyJoinIndun>	VEC_JOININDUN;

typedef struct tagNfyResultIndunM2N
{
	tagNfyResultIndunM2N( BM::GUID const &kGuid=BM::GUID::NullData(), __int64 const iExp=0, int iLv=0 )
		:	kCharGuid(kGuid)
		,	iNowExp(iExp)
		,	iLevel(iLv)
	{}
	tagNfyResultIndunM2N( tagNfyResultIndunM2N const& rkPair )
		:	kCharGuid(rkPair.kCharGuid)
		,	iNowExp(rkPair.iNowExp)
		,	iLevel(rkPair.iLevel)
	{}
	BM::GUID		kCharGuid;
	__int64			iNowExp;
	int				iLevel;
} SNfyResultIndun;
typedef std::vector<SNfyResultIndun> VEC_RESULTINDUN;

typedef struct tagNfyResultItemM2N
{
	tagNfyResultItemM2N( BM::GUID const &kGuid=BM::GUID::NullData() )
		:	kCharGuid(kGuid)
		,	fParticipationRate(0)
	{}

	tagNfyResultItemM2N( tagNfyResultItemM2N const& rkPair )
		:	kCharGuid(rkPair.kCharGuid)
		,	fParticipationRate(rkPair.fParticipationRate)
	{
		kItemList = rkPair.kItemList;
	}

	tagNfyResultItemM2N( BM::Stream& rkPacket )
	{
		ReadFromPacket( rkPacket );
	}

	tagNfyResultItemM2N& operator=(const tagNfyResultItemM2N& rkPair)
	{
		kCharGuid = rkPair.kCharGuid;
		kItemList = rkPair.kItemList;
		fParticipationRate = rkPair.fParticipationRate;
		return *this;
	}

	void WriteFromPacket( BM::Stream& rkPacket )
	{
		if ( kItemList.empty() )
		{
			return;
		}

		size_t iTemp = kItemList.size();
		while ( iTemp > 4 )
		{
			kItemList.pop_back();
			--iTemp;
		}
		rkPacket.Push( kCharGuid );
		PU::TWriteArray_M(rkPacket, kItemList);
		rkPacket.Push( fParticipationRate );
	}

	void ReadFromPacket( BM::Stream& rkPacket )
	{
		kItemList.clear();
		rkPacket.Pop(kCharGuid);
		PU::TLoadArray_M(rkPacket, kItemList);
		rkPacket.Pop(fParticipationRate);
	}

	void Clear()
	{
		kCharGuid = BM::GUID::NullData();
		kItemList.clear();
		fParticipationRate = 0;
	}

	BM::GUID				kCharGuid;
	CONT_ITEM_CREATE_ORDER	kItemList;
	float					fParticipationRate;
}SNfyResultItem;
typedef std::vector<SNfyResultItem> VEC_RESULTITEM;

typedef struct tagNfyResultItemListM2N
{
	tagNfyResultItemListM2N( int const iMax=0 )
		:	iMaxItemCount(iMax)
	{}

	void push_back( tagNfyResultItemM2N const& rkPair )
	{
		if ( !rkPair.kItemList.empty() )
		{
			kResultItemList.push_back( rkPair );
		}
	}

	void CalcParticipationRate(int const iMax)
	{
		if(0 == iMax)
		{
			return;
		}

		VEC_RESULTITEM::iterator it = kResultItemList.begin();
		while(it != kResultItemList.end())
		{
			(*it).fParticipationRate /= iMax;
			int const iCount = std::max(1, static_cast<int>(ceil(iMaxItemCount * (*it).fParticipationRate)));
			if(iCount < (*it).kItemList.size())
			{
				(*it).kItemList.resize(iCount);
			}
			++it;
		}
	}


	bool empty()	{	return kResultItemList.empty();	}

	void WriteFromPacket( BM::Stream& rkPacket )
	{
		if ( iMaxItemCount > 4 )
		{
			iMaxItemCount = 4;
		}

		rkPacket.Push( iMaxItemCount );
		
		rkPacket.Push( kResultItemList.size() );
		VEC_RESULTITEM::iterator itr;
		for ( itr=kResultItemList.begin(); itr!=kResultItemList.end(); ++itr )
		{
			itr->WriteFromPacket( rkPacket );
		}
	}

	void ReadFromPacket( BM::Stream& rkPacket )
	{
		kResultItemList.clear();
		size_t iSize = 0;
		rkPacket.Pop(iMaxItemCount);
		
		rkPacket.Pop(iSize);
		while ( 0 != iSize-- ){	kResultItemList.push_back( SNfyResultItem(rkPacket) );	}
	}

	void Clear()
	{
		iMaxItemCount = 0;
		kResultItemList.clear();
	}
	int					iMaxItemCount;
	VEC_RESULTITEM		kResultItemList;
}SNfyResultItemList;

// 발사체 Action 정보
typedef struct tagSProjectileAction
{
	tagSProjectileAction()
	{
		iActionID = 0;
		iActionInstanceID = 0;
		uProjectileUID = 0;
	}

	BM::GUID kCasterGuid;	// 발사한 Unit Guid
	int iActionID;			// Action ID (=Skill ID)
	int iActionInstanceID;	// ActionInstance ID
	unsigned int uProjectileUID;	// 발사체 UID	
	int		iPenetrationCount;	// 관통 개체수
}SProjectileAction;

typedef enum : BYTE
{
	EFollow_None = 0,
	EFollow_Request = 1,
	EFollow_Req_Refused = 2,
	EFollow_Failed = 3,
	EFollow_Cancel = 4,
	EFollow_TakePerson = 5,
	EFollow_CancelTakePerson = 6,
} EPlayer_Follow_Mode;

int const MAX_FOLLOWING_MODE_LENGTH = 10;
int const FRAN_REWORD_MONEY = 1;		// %수치

////////////////////////////////////////
// 원정대 특별 보상 구조체.

typedef struct tagSpecialResultItem
{
	tagSpecialResultItem()
		: BagGrpNo(0)
	{
	}

	tagSpecialResultItem(int GrpNo)
	{
		BagGrpNo = GrpNo;
	}

	tagSpecialResultItem(tagSpecialResultItem const & rhs)
		: BagGrpNo(rhs.BagGrpNo)
	{
		ResultItem = rhs.ResultItem;
	}

	size_t min_size() const
	{
		return sizeof(BagGrpNo) + ResultItem.min_size();
	}

	void operator = (tagSpecialResultItem const & rhs)
	{
		BagGrpNo = rhs.BagGrpNo;
		ResultItem = rhs.ResultItem;
	}

	void ReadFromPacket(BM::Stream & Packet)
	{
		Packet.Pop(BagGrpNo);
		ResultItem.ReadFromPacket(Packet);
	}

	void WriteToPacket(BM::Stream & Packet) const
	{
		Packet.Push(BagGrpNo);
		ResultItem.WriteToPacket(Packet);
	}

	int	BagGrpNo;
	PgBase_Item ResultItem;
}SSPECIALRESULTITEM;
typedef std::list<SSPECIALRESULTITEM> CONT_SPECIALITEM;

#endif // WEAPON_LOHENGRIN_PACKET_PACKETSTRUCT4MAP_H