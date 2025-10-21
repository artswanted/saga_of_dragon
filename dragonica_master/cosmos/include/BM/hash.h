#pragma once

#include <cassert>
#include <MAP>
#include <vector>

namespace BM
{
//#pragma pack(1)
	template< typename T_FIRST , typename T_SECOND >
	class Hash
		: public std::map< T_FIRST , T_SECOND >
	{
	public:
		typedef typename T_FIRST FIRST;
		typedef typename T_SECOND SECOND;

		typedef typename Hash< T_FIRST , T_SECOND > THIS_TYPE;
		typedef typename THIS_TYPE::iterator ITOR;
		typedef typename THIS_TYPE::const_iterator CONST_ITOR;
		typedef typename THIS_TYPE::reverse_iterator REV_ITOR;
	public:
		Hash(){}
		virtual ~Hash(){}

	public:
		virtual bool Insert(const T_FIRST &First, const T_SECOND &Second, const bool bIsOverWrite = false)
		{
			T_SECOND *pSecond = Find( First );

			if( pSecond && bIsOverWrite )
			{
				Remove( First );
				pSecond = Find(First);
				assert( !pSecond );
			}

			if(!pSecond)
			{
				insert( std::make_pair( First, Second ) );
				pSecond = Find( First );
				if(pSecond)
				{
					return true;
				} // if(pSecond)
				else
				{
	//				assert(NULL && "CCS_Hash Insert Fail");
					return false;
				}
			}
			else
			{
	//			assert(NULL && "CCS_Hash Element Already Insert");
				return false;
			}
		}

		virtual bool Remove(const T_FIRST First)
		{
			ITOR Itor = find(First);

			if( end() != Itor )
			{
				erase( Itor );
				return true;
			}

			return false;
		}
			
		virtual T_SECOND* Find(const T_FIRST &First)//find 후에 erase와 같은 itor에 변화가 생기는 액션이 생기면 받아놓은 데이터가 망가질 수 있다.
		{
			ITOR Itor = find(First);

			if( end() != Itor )
			{
				return &((*Itor).second);
			}
			return NULL;
		}

		virtual bool GetSecond(const T_FIRST &First, T_SECOND &rSecond)//값을 복사해서 받는다. Second 가 포인터 형이 아니라면 사용치 말것.(성능저하)
		{
			ITOR Itor = find(First);

			if( end() != Itor )
			{
				rSecond = (*Itor).second;
				return true;
			}
			return false;
		}

		virtual T_SECOND const * Const_Find(const T_FIRST &First) const 
		{
			CONST_ITOR Itor = find(First);

			if( end() != Itor )
			{
				return &((*Itor).second);
			}
			return NULL;
		}

		virtual T_SECOND* RandomElement()
		{
			if( empty() ){return NULL;}

			std::vector< SECOND* > vecItor;
			vecItor.reserve( size() );
			
			ITOR itor = begin();
			while( end() != itor )
			{
				vecItor.push_back( &((*itor).second) );
				++itor;
			}

			SECOND* pElement = vecItor.at( (rand()%vecItor.size()) );
			return pElement;
		}

		virtual const T_SECOND* Const_RandomElement()const
		{
			if( empty() ){return NULL;}

			std::vector< const SECOND* > vecItor;
			vecItor.reserve( size() );
			
			CONST_ITOR itor = begin();
			while( end() != itor )
			{
				vecItor.push_back( &((*itor).second) );
				++itor;
			}

			const SECOND* pElement = vecItor.at( (rand()%vecItor.size()) );
			return pElement;
		}
	};
//#pragma pack()
}