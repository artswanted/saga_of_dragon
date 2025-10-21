#ifndef WEAPON_VARIANT_ALRAMMISSIONACTION_ALRAMMISSION_TRAITS_H
#define WEAPON_VARIANT_ALRAMMISSIONACTION_ALRAMMISSION_TRAITS_H

#include "loki/typemanip.h"
#include "Variant/constant.h"
#include "Variant/AlramMissionActionDef.h"

namespace ALRAM_MISSION
{
	// Check EventType
	class PgAlramMissionActionCheck_Type
	{
	public:
		bool operator()( Loki::Int2Type<EVENT_KILL_ANYBODY>, eEventType const kType )const{return (EVENT_KILL_ANYBODY == kType);}
		bool operator()( Loki::Int2Type<EVENT_DIE>, eEventType const kType )const{return (EVENT_DIE == kType);}
		bool operator()( Loki::Int2Type<EVENT_HIT_ANYBODY>, eEventType const kType )const{return (EVENT_HIT_ANYBODY == kType);}
		bool operator()( Loki::Int2Type<EVENT_STRUCT>, eEventType const kType )const{return (EVENT_STRUCT == kType);}
		bool operator()( Loki::Int2Type<EVENT_USEITEM>, eEventType const kType )const{return (EVENT_USEITEM == kType);}
		bool operator()( Loki::Int2Type<EVENT_PICKUP_ITEM>, eEventType const kType )const{return (EVENT_PICKUP_ITEM == kType);}
	};

	template< typename T >
	struct SCheckObejct
	{
		explicit SCheckObejct( int const _iObjectType, T const &_kValue )
			:	iObjectType(_iObjectType)
			,	kValue(_kValue)
		{}

		int	iObjectType;
		T	kValue;
	};

	// Check ObjectType
	template< typename T > class PgAlramMissionActionCheck_Object;

	template< >
	class PgAlramMissionActionCheck_Object< CUnit* >
	{
	public:
		bool operator()( CUnit const *pkTarget, int const iObjectType, __int64 const i64ObjectValue )
		{
			if ( !pkTarget )
			{
				return false;
			}

			switch ( iObjectType )
			{
			case OBJECT_ME:
				{
					return true;
				}break;
			case OBJECT_CLASS:
				{
					if ( UT_PLAYER == pkTarget->UnitType() )
					{
						__int64 const i64ClassFlag = GET_CLASS_LIMIT( pkTarget->GetAbil(AT_CLASS) );
						return (i64ClassFlag & i64ObjectValue);
					}
				}break;
			case OBJECT_UNITTYPE:
				{
					return pkTarget->IsInUnitType( static_cast<eUnitType>(i64ObjectValue) );
				}break;
			case OBJECT_HADABIL:
				{
					for ( size_t i = 0 ; i < sizeof(__int64); i += sizeof(WORD) )
					{
						WORD const wAbilType = static_cast<WORD>(i64ObjectValue >> (i*8));
						if ( wAbilType )
						{
							if ( 0 == pkTarget->GetAbil(wAbilType) )
							{
								return false;
							}
						}
					}
					return true;
				}break;
			case OBJECT_HADABIL_CHKVALUE:
				{
					WORD const wAbilType = static_cast<WORD>( i64ObjectValue >> (sizeof(int) * 8) );
					int const iValue = pkTarget->GetAbil( wAbilType );
					return ( iValue == static_cast<int>(i64ObjectValue) );
				}break;
			case OBJECT_DRA_CLASS:
				{
					if ( UT_PLAYER == pkTarget->UnitType() )
					{
						int const classno = pkTarget->GetAbil(AT_CLASS);
						if(50 < classno)	//용족이면
						{
							__int64 const i64ClassFlag = GET_CLASS_LIMIT( classno-50 );
							return (i64ClassFlag & i64ObjectValue);
						}
						return false;
					}
				}break;
			}
			return false;
		}
	};

	template< >
	class PgAlramMissionActionCheck_Object< SCheckObejct< __int64 > >
	{
	public:
		bool operator()( SCheckObejct< __int64 > const &kCheckValue, int const iObjectType, __int64 const i64ObjectValue )
		{
			if ( kCheckValue.iObjectType == iObjectType )
			{
				switch ( iObjectType )
				{
				case OBJECT_ME:
					{
						return true;
					}break;
				case OBJECT_ITEMNO:
				case OBJECT_SKILL:
					{
						return kCheckValue.kValue == i64ObjectValue;
					}break;
				}
			}
			return false;
		}
	};

};//namespace ALRAM_MISSION

#endif // WEAPON_VARIANT_ALRAMMISSIONACTION_ALRAMMISSION_TRAITS_H