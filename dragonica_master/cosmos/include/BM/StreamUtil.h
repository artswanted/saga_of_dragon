#pragma once

#include "BM/Approximate.h"

class BM::vstring;

namespace PU 
{
	class BM::vstring;
	struct type_size
	{
		template< typename T_VALUE >
		static size_t min_size( T_VALUE const * const )
		{	
		//	printf("normal\n");
			return sizeof(T_VALUE);
		}

		template< >
		static size_t min_size( BM::vstring const * const )
		{	
		//	printf("vstring\n");
			return sizeof(size_t);
		}

		template<	typename T_VALUE,
					template < typename _Ty > class _Allocator,
					template < typename _Ty, typename = _Allocator> class _Container,
					template < typename _Ty, typename = _Container< _Ty, _Allocator > >	class _RealContainer
				>
		static size_t min_size( _RealContainer< T_VALUE, _Container< T_VALUE, _Allocator<T_VALUE> > > const * const )
		{
		//	printf( "queue\n");
			return sizeof(size_t);
		}

		template<	typename T_KEY, typename T_VALUE,
					template < typename _Ty > class _Tr, 
					template < typename _Ty, typename _Val> class _Pair_, 
					template < typename _Ty > class _Allocator,
					template < typename _Ty, typename _Key, typename = _Tr,  typename = _Allocator > class CONT
				>
		static size_t min_size( CONT< T_KEY, T_VALUE, _Tr< T_KEY >, _Allocator< _Pair_< const T_KEY, T_VALUE > > > const * const )
		{
		//	printf( "map\n");
			return sizeof(size_t);
		}

		template<	typename T_KEY, typename T_VALUE,
					template < typename _Ty > class _Tr,
					template < typename _Ty, typename _Tr > class _Cmp,
					template < typename _Ty, typename _Val> class _Pair_, 
					template < typename _Ty > class _Allocator,
					template < typename _Ty, typename _Key, typename = _Cmp,  typename = _Allocator > class CONT 
				>
		static size_t min_size( CONT< T_KEY, T_VALUE, _Cmp< T_KEY, _Tr< T_KEY > >, _Allocator< _Pair_< const T_KEY, T_VALUE > > > const * const )
		{
		//	printf( "hashmap\n");
			return sizeof(size_t);
		}

		template<	typename T_VALUE,
					template < typename _Ty > class _Allocator,
					template < typename _Ty, typename = _Allocator > class CONT 
				>
		static size_t min_size( CONT< T_VALUE, _Allocator< T_VALUE > > const * const )
		{
		//	printf( "vector\n");
			return sizeof(size_t);
		}

		template<	typename T_KEY, 
					template < typename _Ty > class _Tr,
					template < typename _Ty > class _Allocator,
					template < typename _Ty, typename _Tr, typename = _Allocator > class CONT
				>
		static size_t min_size( CONT< T_KEY, _Tr< T_KEY >, _Allocator< T_KEY > >const * const )
		{
		//	printf( "set & string\n");
			return sizeof(size_t);
		}
	};

		// Naming 규칙
		//	_A : Push 또는 Pop 사용
		//	_M : ReadFromPacket 또는 WriteToPacket 사용
		//	_AM : first->Pop, second->ReadFromPacket
		//		frist->Push, second->WriteToPacket
		//	_MM : first->ReadFromPacket, second->ReadFromPacket
		//		frist->WriteToPacket, second->WriteToPacket
	template< typename T_CONT >
	void TWriteTable_AA(BM::Stream &rkStream, T_CONT &rkTbl)
	{
		rkStream.Push(rkTbl.size());

		T_CONT::const_iterator itor = rkTbl.begin();

		while(itor != rkTbl.end())
		{
			rkStream.Push((*itor).first);
			rkStream.Push((*itor).second);
			++itor;
		}
	}

	template< typename T_CONT >
	bool TLoadTable_AA(BM::Stream &rkStream, T_CONT &kTbl, size_t const max_count = INT_MAX)
	{
		size_t cont_size = 0;
		if(rkStream.Pop(cont_size)
		&&	cont_size <= max_count)
		{
			static T_CONT::key_type const * const pkKeyType = NULL;
			static T_CONT::mapped_type const * const pkValueType = NULL;

			if( rkStream.RemainSize() >= cont_size*( type_size::min_size(pkKeyType) + type_size::min_size(pkValueType) ) )
			{
				while(cont_size--)
				{
					T_CONT::key_type kKey;
					T_CONT::mapped_type kValue;
					
					rkStream.Pop(kKey);
					rkStream.Pop(kValue);

					kTbl.insert(std::make_pair(kKey, kValue));
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_CONT).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkStream.RemainSize())
			}
		}
		return false;
	}


	template< typename T_CONT >
	void TWriteTable_AM(BM::Stream &rkStream, T_CONT const &rkTbl)
	{
		rkStream.Push(rkTbl.size());

		T_CONT::const_iterator itor = rkTbl.begin();

		while(itor != rkTbl.end())
		{
			rkStream.Push((*itor).first);
			(*itor).second.WriteToPacket(rkStream);
			++itor;
		}
	}

	template< typename T_CONT >
	bool TLoadTable_AM(BM::Stream &rkStream, T_CONT &kTbl, size_t const max_count = INT_MAX)
	{
		size_t cont_size = 0;
		if(	rkStream.Pop(cont_size)
		&&	cont_size <= max_count)
		{
			static T_CONT::key_type const * const pkKeyType = NULL;
			static T_CONT::mapped_type const kValueForSize{};

			if( rkStream.RemainSize() >= cont_size*( type_size::min_size(pkKeyType) + kValueForSize.min_size() ) )
			{
				while(cont_size--)
				{
					T_CONT::key_type kKey;
					T_CONT::mapped_type kValue;
					
					rkStream.Pop(kKey);
					kValue.ReadFromPacket(rkStream);

					kTbl.insert(std::make_pair(kKey, kValue));
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_CONT).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkStream.RemainSize())
			}
		}
		return false;
	}

	template< typename T_CONT >
	bool TLoadTableAdd_AM(BM::Stream &rkStream, T_CONT &kTbl, size_t const max_count = INT_MAX)
	{
		size_t cont_size = 0;
		if(	rkStream.Pop(cont_size)
			&&	cont_size <= max_count)
		{
			static T_CONT::key_type const * const pkKeyType = NULL;
			static T_CONT::mapped_type const kValueForSize;

			if( rkStream.RemainSize() >= cont_size * ( type_size::min_size(pkKeyType) + kValueForSize.min_size() ) )
			{
				while(cont_size--)
				{
					T_CONT::key_type kKey;
					T_CONT::mapped_type kValue;

					rkStream.Pop(kKey);
					kValue.ReadFromPacket(rkStream);

					auto kPair = kTbl.insert(std::make_pair(kKey, kValue));
					if ( !kPair.second )
					{
						kPair.first->second = kValue;
					}
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_CONT).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkStream.RemainSize())
			}
		}
		return false;
	}

	template< typename T_CONT >
	void TWriteTable_MM(BM::Stream &rkStream, T_CONT &rkTbl)
	{
		rkStream.Push(rkTbl.size());

		T_CONT::const_iterator itor = rkTbl.begin();

		while(itor != rkTbl.end())
		{
			(*itor).first.WriteToPacket(rkStream);
			(*itor).second.WriteToPacket(rkStream);
			++itor;
		}
	}

	template< typename T_CONT >
	bool TLoadTable_MM(BM::Stream &rkStream, T_CONT &kTbl, size_t const max_count = INT_MAX)
	{
		size_t cont_size = 0;
		if(	rkStream.Pop(cont_size)
		&&	cont_size <= max_count)
		{
			static T_CONT::key_type const kKeyForSize{};
			static T_CONT::mapped_type const kValueForSize{};

			if( rkStream.RemainSize() >= cont_size*(kKeyForSize.min_size() + kValueForSize.min_size()))
			{
				while(cont_size--)
				{
					T_CONT::key_type kKey;
					T_CONT::mapped_type kValue;
					
					kKey.ReadFromPacket(rkStream);
					kValue.ReadFromPacket(rkStream);

					kTbl.insert(std::make_pair(kKey, kValue));
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_CONT).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkStream.RemainSize())
			}
		}
		return false;
	}

	template< typename T_CONT >
	bool TLoadTableAdd_MM(BM::Stream &rkStream, T_CONT &kTbl, size_t const max_count = INT_MAX)
	{
		size_t cont_size = 0;
		if(rkStream.Pop(cont_size)
		&&	cont_size <= max_count)
		{
			static T_CONT::key_type const kKeyForSize{};
			static T_CONT::mapped_type const kValueForSize{};

			if( rkStream.RemainSize() >= cont_size*(kKeyForSize.min_size() + kValueForSize.min_size()))
			{
				while(cont_size--)
				{
					T_CONT::key_type kKey;
					T_CONT::mapped_type kValue;

					kKey.ReadFromPacket(rkStream);
					kValue.ReadFromPacket(rkStream);

					auto kPair = kTbl.insert(std::make_pair(kKey, kValue));
					if ( !kPair.second )
					{
						kPair.first->second = kValue;
					}
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_CONT).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkStream.RemainSize())
			}
		}

		return false;
	}

	template< typename T_CONT >
	void TWriteKey_A(BM::Stream &rkStream, T_CONT &rkTbl)
	{
		rkStream.Push(rkTbl.size());
		T_CONT::const_iterator itor = rkTbl.begin();
		while(itor != rkTbl.end())
		{
			rkStream.Push((*itor));
			++itor;
		}
	}

	template< typename T_CONT >
	bool TLoadKey_A(BM::Stream &rkStream, T_CONT &rkTbl, size_t const max_count = INT_MAX)
	{
		size_t cont_size = 0;
		if(rkStream.Pop(cont_size)
		&&	cont_size <= max_count)
		{
			static T_CONT::key_type const * const pkKeyType = NULL;

			if( rkStream.RemainSize() >= cont_size*( type_size::min_size(pkKeyType) ) )
			{
				while(cont_size--)
				{
					T_CONT::key_type kKey;
					rkStream.Pop(kKey);
					rkTbl.insert(kKey);
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_CONT).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkStream.RemainSize())
			}
		}
		return false;
	}

	template< typename T_CONT >
	void TWriteKey_M(BM::Stream &rkStream, T_CONT &rkTbl)
	{
		rkStream.Push(rkTbl.size());
		T_CONT::const_iterator itor = rkTbl.begin();
		while(itor != rkTbl.end())
		{
			(*itor).WriteToPacket(rkStream);
			++itor;
		}
	}

	template< typename T_CONT >
	bool TLoadKey_M(BM::Stream &rkStream, T_CONT &rkTbl, size_t const max_count = INT_MAX)
	{
		size_t cont_size = 0;
		if(rkStream.Pop(cont_size)
		&&	cont_size <= max_count)
		{
			static T_CONT::key_type const kKeyForSize{};

			if( rkStream.RemainSize() >= cont_size*(kKeyForSize.min_size()))
			{
				while(cont_size--)
				{
					T_CONT::key_type kKey;
					kKey.ReadFromPacket(rkStream);

					rkTbl.insert(kKey);
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_CONT).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkStream.RemainSize())
			}
		}
		return false;
	}

	template< typename T_CONT >
	void TWriteArray_A(BM::Stream &rkStream, T_CONT &rkTbl)
	{
		rkStream.Push(rkTbl.size());

		T_CONT::const_iterator itor = rkTbl.begin();

		while(itor != rkTbl.end())
		{
			rkStream.Push(*itor);
			++itor;
		}
	}

	template< typename T_VALUE >
	bool TLoadArray_A(BM::Stream &rkStream, std::vector< T_VALUE > &kCont, size_t const max_count = INT_MAX)
	{
		size_t cont_size = 0;
		if( rkStream.Pop(cont_size) 
		&&	cont_size <= max_count)
		{
			static T_VALUE const * const pkValueType = NULL;

			if( rkStream.RemainSize() >= cont_size * type_size::min_size( pkValueType ) )
			{
				kCont.reserve(cont_size);
				while(cont_size--)
				{
					std::vector< T_VALUE >::value_type kValue;

					rkStream.Pop(kValue);
					kCont.push_back(kValue);
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_VALUE).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkStream.RemainSize())
			}
		}
		
		return false;
	}

	template< typename T_VALUE >
	bool TLoadArray_A(BM::Stream &rkStream, std::list< T_VALUE > &kCont, size_t const max_count = INT_MAX)
	{
		size_t cont_size = 0;
		if( rkStream.Pop( cont_size )
		&&	cont_size <= max_count)
		{
			static T_VALUE const * const pkValueType = NULL;

			if( rkStream.RemainSize() >= cont_size * type_size::min_size(pkValueType) )
			{
				while(cont_size--)
				{
					std::vector< T_VALUE >::value_type kValue;

					rkStream.Pop(kValue);
					kCont.push_back(kValue);
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_VALUE).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkStream.RemainSize())
			}
		}
		return false;
	}

	template< typename T_VALUE >
	bool TLoadArray_A(BM::Stream &rkStream, std::set< T_VALUE > &kCont, size_t const max_count = INT_MAX)
	{
		size_t cont_size = 0;
		if( rkStream.Pop(cont_size) 
		&&	cont_size <= max_count)
		{
			static T_VALUE const * const pkValueType = NULL;

			if( rkStream.RemainSize() >= cont_size * type_size::min_size(pkValueType) )
			{
				while(cont_size--)
				{
					std::set< T_VALUE >::value_type kValue;

					rkStream.Pop(kValue);
					kCont.insert(kValue);
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_VALUE).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkStream.RemainSize())
			}
		}
		
		return false;
	}

	template< typename T_VALUE, class _Pr >
	bool TLoadArray_A(BM::Stream &rkStream, BM::PgApproximateVector< T_VALUE, _Pr > &kCont, size_t const max_count = INT_MAX)
	{
		BM::PgApproximateVector< T_VALUE, _Pr >::CONT_VAR vec;
		if ( true == TLoadArray_A( rkStream, vec, max_count ) )
		{
			kCont.insert( vec );
			return true;
		}
		return false;
	}

	template< typename T_CONT >
	void TWriteArray_M(BM::Stream &rkStream, T_CONT &rkTbl)
	{
		rkStream.Push(rkTbl.size());

		T_CONT::const_iterator itor = rkTbl.begin();

		while(itor != rkTbl.end())
		{
			(*itor).WriteToPacket(rkStream);
			++itor;
		}
	}

	template< typename T_VALUE >
	bool TLoadArray_M(BM::Stream &rkStream, std::vector< T_VALUE > &kCont, size_t const max_count = INT_MAX)
	{
		size_t cont_size = 0;
		if( rkStream.Pop( cont_size )
		&&	cont_size <= max_count)
		{
			static T_VALUE const kValueForSize{};
			if( rkStream.RemainSize() >= ( cont_size * kValueForSize.min_size() ) )
			{
				kCont.reserve(cont_size);
				while(cont_size--)
				{
					std::vector< T_VALUE >::value_type kValue;

					kValue.ReadFromPacket(rkStream);
					kCont.push_back(kValue);
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_VALUE).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkStream.RemainSize())
			}
		}
		return false;
	}

	template< typename T_VALUE >
	bool TLoadArray_M(BM::Stream &rkStream, std::set< T_VALUE > &kCont, size_t const max_count = INT_MAX)
	{
		size_t cont_size = 0;
		if( rkStream.Pop( cont_size )
		&&	cont_size <= max_count)
		{
			static T_VALUE const kValueForSize;
			if( rkStream.RemainSize() >= ( cont_size * kValueForSize.min_size() ) )
			{
				while(cont_size--)
				{
					std::set< T_VALUE >::value_type kValue;

					kValue.ReadFromPacket(rkStream);
					kCont.insert(kValue);
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_VALUE).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkStream.RemainSize())
			}
		}
		return false;
	}

	template< typename T_VALUE >
	bool TLoadArray_M(BM::Stream &rkStream, std::list< T_VALUE > &kCont, size_t const max_count = INT_MAX)
	{
		size_t cont_size = 0;
		if( rkStream.Pop( cont_size )
		&&	cont_size <= max_count)
		{
			static T_VALUE const kValueForSize;
			if( rkStream.RemainSize() >= ( cont_size * kValueForSize.min_size() ) )
			{
				while(cont_size--)
				{
					std::vector< T_VALUE >::value_type kValue;

					kValue.ReadFromPacket(rkStream);
					kCont.push_back(kValue);
				}
				return true;
			}
			else
			{
//				CAUTION_LOG(BM::LOG_LV0, __FL__ << typeid(T_VALUE).name() << _T(" pop_size=") << cont_size << _T(" packet_remain=") << rkStream.RemainSize())
			}
		}
		return false;
	}

	template< typename T_VALUE, class _Pr >
	bool TLoadArray_M(BM::Stream &rkStream, BM::PgApproximateVector< T_VALUE, _Pr > &kCont, size_t const max_count = INT_MAX)
	{
		BM::PgApproximateVector< T_VALUE, _Pr >::CONT_VAR vec;
		if ( true == TLoadArray_M( rkStream, vec, max_count ) )
		{
			kCont.insert( vec );
			return true;
		}
		return false;
	}


	template< typename T_CONT >
	void TWriteTableArray_AMA(BM::Stream &rkStream, T_CONT const &rkTbl)
	{
		rkStream.Push(rkTbl.size());

		T_CONT::const_iterator itr = rkTbl.begin();
		for( ; itr != rkTbl.end() ; ++itr )
		{
			rkStream.Push( itr->first );
			PU::TWriteArray_A( rkStream, itr->second );
		}
	}

	template< typename T_CONT >
	bool TLoadTableArray_AMA(BM::Stream &rkStream, T_CONT &kTbl, size_t const max_count = INT_MAX, size_t const max_count2 = INT_MAX )
	{
		size_t cont_size = 0;
		if(	rkStream.Pop(cont_size)
			&&	cont_size <= max_count)
		{
			T_CONT::key_type kKey;
			T_CONT::mapped_type kElement;
			while(cont_size--)
			{
				if (!rkStream.Pop( kKey ) 
				||	!PU::TLoadArray_A( rkStream, kElement, max_count2 ))
				{
					return false;
				}

				kTbl.insert( std::make_pair( kKey, kElement ) );
				kElement.clear();
			}
			return true;
		}
		return false;
	}

	template< typename T_CONT >
	void TWriteTableArray_AMM(BM::Stream &rkStream, T_CONT const &rkTbl)
	{
		rkStream.Push(rkTbl.size());

		T_CONT::const_iterator itr = rkTbl.begin();
		for( ; itr != rkTbl.end() ; ++itr )
		{
			rkStream.Push( itr->first );
			PU::TWriteArray_M( rkStream, itr->second );
		}
	}

	template< typename T_CONT >
	bool TLoadTableArray_AMM(BM::Stream &rkStream, T_CONT &kTbl, size_t const max_count = INT_MAX, size_t const max_count2 = INT_MAX )
	{
		size_t cont_size = 0;
		if(	rkStream.Pop(cont_size)
			&&	cont_size <= max_count)
		{
			T_CONT::key_type kKey;
			T_CONT::mapped_type kElement;
			while(cont_size--)
			{
				if (!rkStream.Pop( kKey ) 
					||	!PU::TLoadArray_M( rkStream, kElement, max_count2 ))
				{
					return false;
				}

				kTbl.insert( std::make_pair( kKey, kElement ) );
				kElement.clear();
			}
			return true;
		}
		return false;
	}
}


#define PACKETPUSH_MAP_PM(packet, maptype, map) {	\
	(packet).Push((map).size());	\
	maptype::const_iterator itor = (map).begin();	\
	while ( (map).end() != itor )	\
	{	\
		(packet).Push(itor->first);	\
		itor->second.WriteToPacket((packet));	\
		++itor;	\
	}	\
	}

#define PACKETPOP_MAP_PM(packet, maptype, map) {	\
	size_t iSize = 0;	\
	(packet).Pop(iSize);	\
	(map).clear();	\
	for (size_t i=0; i<iSize; i++)	\
	{	\
		maptype::key_type kFirst;	\
		maptype::mapped_type kSecond;	\
		(packet).Pop(kFirst);	\
		kSecond.ReadFromPacket((packet));	\
		(map).insert(std::make_pair(kFirst, kSecond));	\
	}	\
	}



#define PACKETPUSH_MAP_MM(packet, maptype, map) {	\
	(packet).Push((map).size());	\
	maptype::const_iterator itor = (map).begin();	\
	while ( (map).end() != itor )	\
	{	\
		(packet).WriteToPacket(itor->first);	\
		itor->second.WriteToPacket((packet));	\
		++itor;	\
	}	\
	}

#define PACKETPOP_MAP_MM(packet, maptype, map) {	\
	size_t iSize = 0;	\
	(packet).Pop(iSize);	\
	(map).clear();	\
	for (size_t i=0; i<iSize; i++)	\
	{	\
		maptype::key_type kFirst;	\
		maptype::mapped_type kSecond;	\
		(packet).ReadFromPacket(kFirst);	\
		kSecond.ReadFromPacket((packet));	\
		(map).insert(std::make_pair(kFirst, kSecond));	\
	}	\
	}

#define PACKETPUSH_MAP_MP(packet, maptype, map) {	\
	(packet).Push((map).size());	\
	maptype::const_iterator itor = (map).begin();	\
	while ( (map).end() != itor )	\
	{	\
		(packet).WriteToPacket(itor->first);	\
		itor->second.Pop((packet));	\
		++itor;	\
	}	\
	}


#define PACKETPOP_MAP_MP(packet, maptype, map) {	\
	size_t iSize = 0;	\
	(packet).Pop(iSize);	\
	(map).clear();	\
	for (size_t i=0; i<iSize; i++)	\
	{	\
		maptype::key_type kFirst;	\
		maptype::mapped_type kSecond;	\
		(packet).ReadFromPacket(kFirst);	\
		kSecond.Pop((packet));	\
		(map).insert(std::make_pair(kFirst, kSecond));	\
	}	\
	}
