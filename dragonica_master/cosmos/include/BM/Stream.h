#pragma once
#include <assert.h>
#include "BM/ClassSupport.h"
#include "BM/STLSupport.h"
#include "BM/GUID.h"
#include "BM/vstring.h"

class PgBase_Item;
class PgItemWrapper;

namespace BM
{
#pragma pack (1)
	class Stream
	{
	public:
		typedef unsigned short DEF_STREAM_TYPE;

	public:
		typedef std::vector< char > STREAM_DATA;
	public:
		Stream();
		Stream(DEF_STREAM_TYPE const wStreamType);
		Stream(Stream const& kStream);

		template< typename T_INPUT >
		Stream(DEF_STREAM_TYPE const wStreamType, T_INPUT const& input);
		
		~Stream();

		void operator = (Stream const&  rhs);

	public:
		void Swap(BM::Stream &rhs);

		void Reset();
		void Reserve(size_t const reserve_size);
		void Resize(size_t const resize_size);
	
		void PosAdjust();//시작점과 끝점으로 포지션 리셋.
		void Reset(DEF_STREAM_TYPE const wStreamType);
		bool IsEmpty()const;

	public:
		template< typename T_INPUT >
		void Push( T_INPUT const& input );
		
		template< >
		void Push( std::string const& input );
		
		template< >
		void Push( std::wstring const& input );

		template< >
		void Push( BM::vstring const& input );

		template< >
		void Push( Stream const& input );
		
		template< typename T_VALUE >
		void Push( std::vector< T_VALUE > const& input );

		template< typename T_VALUE >
		void Push( std::list< T_VALUE > const& input );
		
		template< typename T_KEY, typename T_VALUE >
		void Push( std::map< T_KEY, T_VALUE > const& input );

		template< typename T_KEY, typename T_VALUE >
		void Push( std::multimap< T_KEY, T_VALUE > const& input );

		template< typename T_KEY, typename T_VALUE, typename T_HASHER >
		void Push( std::unordered_map< T_KEY, T_VALUE, T_HASHER > const& input );

		void Push( void const *pData, size_t const in_size );

		template< typename T_VALUE >
		void Push( std::set< T_VALUE > const& input);

		template< typename T_VALUE >
		void Push( std::deque< T_VALUE > const& input);

	public://! 이하 Pop
		bool PopMemory(void *pBuffer, size_t const out_size);
		
		template < typename T_ELEMENT >
		bool Pop(T_ELEMENT &rElement);
		
		template< typename T_VALUE >
		bool Pop( std::vector< T_VALUE > &vecout, size_t const max_count = INT_MAX);
		
		template< typename T_VALUE >
		bool Pop( std::list< T_VALUE > &out, size_t const max_count = INT_MAX);

		template< typename T_KEY, typename T_VALUE >
		bool Pop( std::map< T_KEY, T_VALUE > &out, size_t const max_count = INT_MAX);

		template< typename T_KEY, typename T_VALUE >
		bool Pop( std::multimap< T_KEY, T_VALUE > &input, size_t const max_count = INT_MAX);

		template< typename T_KEY, typename T_VALUE, typename T_HASHER >
		bool Pop( std::unordered_map< T_KEY, T_VALUE, T_HASHER > &out, size_t const max_count = INT_MAX);

		template< typename T_VALUE >
		bool Pop( std::set< T_VALUE > &out, size_t const max_count = INT_MAX); 

		template< typename T_VALUE >
		bool Pop( std::deque< T_VALUE > &vecout, size_t const max_count = INT_MAX);
		
		bool Pop(std::string &rElement, size_t const max_count = INT_MAX);
		bool Pop(std::wstring &rElement, size_t const max_count = INT_MAX);
		bool Pop(BM::vstring &rElement, size_t const max_count = INT_MAX);
	
	public:
		size_t Size() const;//벡터자체의 사이즈
		size_t RemainSize() const;//앞으로 Pop 할 수 있는 사이즈
		void Clear();//벡터도 지우고, pos 도 클리어.

		void SetStopSignal( bool const bIsStopSignal );
		bool IsStopSignal() const ;

		STREAM_DATA& Data();
		STREAM_DATA const& Data()const;
		HRESULT ModifyData(size_t const iPos, void const *pData, size_t const in_size);//지정된 장소부터 데이터를 덮어씀

		size_t RdPos()const{return rd_pos_;}
		size_t WrPos()const{return wr_pos_;}

		void RdPos(size_t const ndw_rd_pos)
		{
			rd_pos_ = ndw_rd_pos;
		}
	
		void WrPos(size_t const ndw_wr_pos)
		{
			wr_pos_ = ndw_wr_pos;
		}
	protected:
		void CommonInit();

	protected:
		STREAM_DATA m_vecData;
		bool m_bIsStopSignal;
		size_t rd_pos_;
		size_t wr_pos_;

	private://체크용 못사용하게 막기
		template< >
		void Push( PgBase_Item const& );

		template< >
		void Push( PgItemWrapper const& );

		template<>
		bool Pop( PgBase_Item& );

		template< >
		bool Pop( PgItemWrapper& );
	};
	
#pragma pack ()

	#include "Stream.inl"
};

#include "BM/StreamUtil.h"

