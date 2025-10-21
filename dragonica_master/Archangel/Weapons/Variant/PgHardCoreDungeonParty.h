#ifndef WEAPON_VARIANT_PGHARDCOREDUNGEONPARTY_H
#define WEAPON_VARIANT_PGHARDCOREDUNGEONPARTY_H

#include "Variant/PgPartyContents.h"
#include "Variant/PgClassPetDefMgr.h"
#include "Variant/PgPet.h"
#include "Variant/PgSummoned.h"

typedef enum
{
	E_HC_TYPE_NONE = 0,
	E_HC_TYPE_BOSS = 1,
}E_HARDCORE_TYPE;

typedef enum
{
	E_HCT_VOTE,		// 준비중, 의사 결정진행중
	H_HCT_READY,	// 맵이동 진행중
	E_HCT_PLAY,		// 게임중 
	E_HCT_BOSS,		// 보스방 입장
}E_HARDCORE_PARTY_STATE;

typedef enum
{
	E_HCT_V_NONE				= 0,
	E_HCT_V_OK					= 1,
	E_HCT_V_CANCEL				= 2,	
	E_HCT_V_CASHSHOP			= 3,
	E_HCT_V_OPENMARKET			= 4,
	E_HCT_V_MISSION				= 5,
	E_HCT_V_INDUN				= 6,
	E_HCT_V_ERROR				= 7,
	E_HCT_V_MYHOME				= 8,
	E_HCT_V_EMPORIA				= 9,
	E_HCT_V_BATTLESQUARE		= 10,
	E_HCT_V_MARRY				= 11,
	E_HCT_V_HARDCORE_DUNGEON	= 12,
}E_HARDCORE_VOTE_STATE;

class PgPartyContents_HardCoreDungeon
	:	public PgPartyContents
{
public:
	struct SMemberState
	{
		SMemberState( BYTE const _byState = E_HCT_V_NONE )
			:	byState(_byState)
		{}

		BM::GUID	kVolatileID;
		BYTE		byState;
	};

	typedef std::map< BM::GUID, SMemberState >	CONT_MEMBER_STATE;

public:

	class PgExtFunction
	{
	public:
		PgExtFunction(){}
		virtual ~PgExtFunction(){}

		virtual void Release( BYTE const byState, CONT_MEMBER_STATE const &kMemberState ) = 0;

	private:
		PgExtFunction( PgExtFunction const & );
		PgExtFunction& operator = ( PgExtFunction const & );
	};

public:
	PgPartyContents_HardCoreDungeon(void);
	virtual ~PgPartyContents_HardCoreDungeon(void);

	virtual void Init( PgParty &rkParty );
	virtual HRESULT IsInvite(void)const{return PCR_Fail_HardCoreVote1;}
	virtual HRESULT IsJoin(void)const{return PCR_Fail_HardCoreVote2;}
	virtual HRESULT IsChangeMaster(void)const{return PCR_Fail_HardCoreVote3;}
	virtual HRESULT IsChangeOption(void)const{return PCR_Fail_HardCoreVote3;}
	virtual bool Leave( BM::GUID const &kCharGuid, bool const bIsMaster );

	void Init( BM::GUID const &kMasterGuid );
	void Release();

	BYTE GetState(void)const{return m_byState;}

	void SetMode( int const iMode ){m_iMode=iMode;}
	int GetMode(void)const{return m_iMode;}

	SGroundKey const &GetDungeonGndKey(void)const{return m_kDungeonGndKey;}
	void SetDungeonGndKey( SGroundKey const &kGndKey ){m_kDungeonGndKey = kGndKey;} 

	HRESULT SetWaitMove();
	HRESULT SetMove();

	HRESULT SetMemberState( BM::GUID const &kCharGuid, BYTE const byState );
	BYTE GetMemberState( BM::GUID const &kCharGuid )const;
	bool IsAllSetState(void)const;

	HRESULT SetMemberMoveReady( BM::GUID const &kCharGuid, BM::GUID const &kVolatileID );

	void WriteToPacket( BM::Stream &kPacket )const;
	bool ReadFromPacket( BM::Stream &kPacket );
	void WriteToPacket_JoinDungeonOrder( BM::Stream &rkPacket )const;
	void ReadFromPacket_MapMoveOrder( BM::Stream &rkPacket );

	template< typename FN >
	void ForEach_Member( FN &kFunction )
	{
		CONT_MEMBER_STATE::iterator member_itr = m_kContMemberState.begin();
		while ( member_itr != m_kContMemberState.end() )
		{
			if ( !kFunction( member_itr->first, member_itr->second ) )
			{
				member_itr = m_kContMemberState.erase( member_itr );
			}
			else
			{
				++member_itr;
			}
		}
	}

	template< typename T_FUNC >
	HRESULT SetExtFunction()
	{
		T_FUNC * pkFuncInstance = new T_FUNC;
		PgExtFunction * pkExtFunc = dynamic_cast<PgExtFunction*>(pkFuncInstance);
		if ( pkExtFunc )
		{
			SAFE_DELETE( m_pkExtFunction );
			m_pkExtFunction = pkExtFunc;
			return true;
		}

		SAFE_DELETE(pkFuncInstance);
		return false;
	}

protected:
	BYTE				m_byState;//E_HARDCORE_PARTY_STATE 사용
	int					m_iMode;
	SGroundKey			m_kDungeonGndKey;
	CONT_MEMBER_STATE	m_kContMemberState;

	CONT_PET_MAPMOVE_DATA		m_kContPetMapMoveData;
	CONT_UNIT_SUMMONED_MAPMOVE_DATA	m_kContUnitSummonedMapMoveData;
	CONT_PLAYER_MODIFY_ORDER	m_kModifyOrder;

	PgExtFunction		*m_pkExtFunction;
};

class PgHardCoreDungeonParty
	:	public PgPartyContents_HardCoreDungeon
{
public:
	PgHardCoreDungeonParty(void);
	virtual ~PgHardCoreDungeonParty(void);

	HRESULT Init( BM::GUID const &kPartyID, BM::GUID const &kMasterID, VEC_GUID const &kMemberList );

private:
	BM::GUID		m_kPartyID;
	BM::GUID		m_kMasterID;
	__int64			m_i64BeginTime;//
};

#endif // WEAPON_VARIANT_PGHARDCOREDUNGEONPARTY_H