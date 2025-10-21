#ifndef WEAPON_VARIANT_GUILD_PGEMPORIAFUNCTION_H
#define WEAPON_VARIANT_GUILD_PGEMPORIAFUNCTION_H

#include "Lohengrin/PacketStruct2.h"
#include "PgEventView.h"

typedef enum
{
	EFUNC_CAPSULE_CREATURE		= 1,//캡슐생성기
	EFUNC_SOULCRAFT				= 2,//길드 소울 크래프트
	EFUNC_GOOD_SMITHY			= 3,//좋은 대장간
	EFUNC_WELL_OF_CURE			= 4,// 치유의 샘
	EFUNC_BLACKMARKET			= 5,//블랙마켓
}E_EMPORIA_FUNC;

typedef enum
{
	EFUNC_ERROR					= 0,//정의되지 않은 에러(서버에러?)
	EFUNC_SUCCESS				= 1,// 성공
	EFUNC_ACCESSDENIED			= 2,// 권한없음(오너가 아님)
	EFUNC_GUILDEXP				= 3,// 길드 경험치 부족
	EFUNC_NOT					= 4,// 설치 할 수 없는 구조물
}E_EMPORIA_FUNC_RESULT;

typedef struct tagEmporiaFunctionInfo
{
	tagEmporiaFunctionInfo()
		:	i64RentalDate(0i64)
		,	i64ExpirationDate(0i64)
		,	i64ExtValue(0i64)
	{}

	explicit tagEmporiaFunctionInfo( BM::DBTIMESTAMP_EX const &kRentalDate, BM::DBTIMESTAMP_EX const &kExpirationDate, __int64 const _i64ExtValue=0i64 )
		:	i64ExtValue(_i64ExtValue)
	{
		CGameTime::DBTimeEx2SecTime( kRentalDate, i64RentalDate, CGameTime::DEFAULT );
		CGameTime::DBTimeEx2SecTime( kExpirationDate, i64ExpirationDate, CGameTime::DEFAULT );
	}

	__int64 GetRemainTime(__int64 const iType = CGameTime::MINUTE )const
	{
		__int64 i64Temp = i64ExpirationDate - g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
		return i64Temp / iType;
	}

	void AddTime( __int64 const i64Week, __int64 const iType = CGameTime::OneWeek )
	{
		if ( 0i64 == i64RentalDate )
		{
			i64RentalDate = i64ExpirationDate = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
		}
		i64ExpirationDate += (iType * i64Week);
	}

	DEFAULT_TBL_PACKET_FUNC();

	__int64 i64RentalDate;
	__int64 i64ExpirationDate;
	__int64	i64ExtValue;
}SEmporiaFunction;

typedef std::map< short, SEmporiaFunction >		CONT_EMPORIA_FUNC;
typedef std::set< CONT_EMPORIA_FUNC::key_type >	CONT_EMPORIA_FUNCKEY;

typedef struct tagReqUnKeepEmporia_Server
:	public tagReqUnKeepEmporia
{
	tagReqUnKeepEmporia_Server()
		:	i64UseExp(0i64)
	{}

	tagReqUnKeepEmporia_Server( SGroundKey const &kEmpGndKey, tagReqUnKeepEmporia const &rhs )
		:	SReqUnKeepEmporia(rhs)
		,	kEmporiaGndKey(kEmpGndKey)
		,	i64UseExp(0i64)
	{}

	SGroundKey			kEmporiaGndKey;
	BM::GUID			kReqGuid;
	BM::GUID			kReqCharGuid;
	__int64				i64UseExp;
	SEmporiaFunction	kFunc;
	
	void WriteToPacket( BM::Stream &kPacket )const
	{
		tagReqUnKeepEmporia::WriteToPacket( kPacket );
		kEmporiaGndKey.WriteToPacket( kPacket );
		kPacket.Push( kReqGuid );
		kPacket.Push( kReqCharGuid );
		kPacket.Push( i64UseExp );
		kFunc.WriteToPacket( kPacket );
	}

	void ReadFromPacket( BM::Stream &kPacket )
	{
		tagReqUnKeepEmporia::ReadFromPacket( kPacket );
		kEmporiaGndKey.ReadFromPacket( kPacket );
		kPacket.Pop( kReqGuid );
		kPacket.Pop( kReqCharGuid );
		kPacket.Pop( i64UseExp );
		kFunc.ReadFromPacket( kPacket );
	}
}SReqUnKeepEmporia_Server;

class PgEmporiaFunction
{
public:
	PgEmporiaFunction(void);
	PgEmporiaFunction( PgEmporiaFunction const &rhs );
	virtual ~PgEmporiaFunction();

	PgEmporiaFunction& operator=( PgEmporiaFunction const &rhs );

	void Clear();
	void Swap( PgEmporiaFunction &rhs );

	void WriteToPacket( BM::Stream &kPacket )const;
	void ReadFromPacket( BM::Stream &kPacket );

	bool AddFunction( CONT_EMPORIA_FUNC::key_type const iType, CONT_EMPORIA_FUNC::mapped_type const &kFunction );
	bool RemoveFunction( CONT_EMPORIA_FUNC::key_type const iType );
	bool GetFunction( CONT_EMPORIA_FUNC::key_type const iType, CONT_EMPORIA_FUNC::mapped_type &kFunction )const;
	bool IsHaveFunction( CONT_EMPORIA_FUNC::key_type const iType )const{return m_kContFunc.find(iType) != m_kContFunc.end();}

	void GetFunctionCont( CONT_EMPORIA_FUNC &rkCont )const{rkCont = m_kContFunc;}

//	bool IsAddingFunction()const{return BM::GUID::IsNotNull(m_kReqAddingFuncID); }
//	bool NewAddingFunction( BM::GUID &kReqAddingFuncID );
//	bool ClearAddingFunction( BM::GUID const &kRecvGuid );

//	BM::GUID const& GetAddingFuncID()const{return m_kReqAddingFuncID;}

	size_t CheckDelete( __int64 const i64NowTime, CONT_EMPORIA_FUNCKEY &kContDeleteKey );

	CONT_EMPORIA_FUNC::const_iterator BeginFunc()const{return m_kContFunc.begin();}
	CONT_EMPORIA_FUNC::const_iterator EndFunc()const{return m_kContFunc.end();}

protected:
	CONT_EMPORIA_FUNC				m_kContFunc;

//	BM::GUID						m_kReqAddingFuncID;
};

#endif // WEAPON_VARIANT_GUILD_PGEMPORIAFUNCTION_H