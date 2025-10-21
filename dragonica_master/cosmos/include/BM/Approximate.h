#pragma once

namespace BM
{

	class PgApproximate
	{
	public:
		static size_t const npos = SIZE_MAX;

	private:
		PgApproximate();
	};

	template< typename T, class _Pr >
	class PgTraitsApproximateFinder
	{
	public:
		size_t operator()( std::vector< T > const &kVec, T const &kValue );

	private:
		static size_t find( std::vector< T > const &kVec, T const &kValue, size_t const left_index, size_t const right_index );
	};

	template< typename T, class _FindPr >
	class PgTraitsApproximateFinderEx
	{
	public:
		size_t operator()( std::vector< T > const &kVec, _FindPr const &kValue );

	private:
		static size_t find( std::vector< T > const &kVec, _FindPr const &kValue, size_t const left_index, size_t const right_index );
	};

	// 근사치를 구하기 위한 클래스

	template< typename T, class _Pr = std::less< T > >
	class PgApproximateVector
	{
	public:
		typedef typename std::vector< T >					CONT_VAR;
		typedef typename CONT_VAR::size_type				size_type;
		typedef typename CONT_VAR::value_type				value_type;
		typedef typename CONT_VAR::iterator					iterator;
		typedef typename CONT_VAR::const_iterator			const_iterator;
		typedef typename CONT_VAR::reverse_iterator			reverse_iterator;
		typedef typename CONT_VAR::const_reverse_iterator	const_reverse_iterator;
		
	public:
		PgApproximateVector(void);
		~PgApproximateVector(void);
		PgApproximateVector( PgApproximateVector const &rhs );
		PgApproximateVector& operator = ( PgApproximateVector const &rhs );

		T const &operator[]( size_t const index )const{return at(index);}

		void clear(){m_kContVar.clear();}
		void reserve( size_t const size );

		bool insert( T const & kValue );
		bool insert( CONT_VAR const &kCont );

		template< class _FindPr >
		size_t find( _FindPr const &kValue )const;

		size_t find( T const & kValue, T & rkOutValue )const;

		size_type size(void)const{return m_kContVar.size();}
		T const &at( size_t const index )const;

		iterator begin(){return m_kContVar.begin();}
		iterator end(){return m_kContVar.end();}
		const_iterator begin()const{return m_kContVar.begin();}
		const_iterator end()const{return m_kContVar.end();}
		reverse_iterator rbegin(){return m_kContVar.rbegin();}
		reverse_iterator rend(){return m_kContVar.rend();}
		const_reverse_iterator rbegin()const{return m_kContVar.rbegin();}
		const_reverse_iterator rend()const{return m_kContVar.rend();}

	protected:
		CONT_VAR	m_kContVar;
	};

#include "Approximate.inl"

};//<- namespace BM
