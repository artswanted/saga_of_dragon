template< typename T_INPUT >
Stream::Stream(DEF_STREAM_TYPE const wStreamType, T_INPUT const& input)
{
	CommonInit();

	Reserve( sizeof(wStreamType) + sizeof(T_INPUT) );
	Push(wStreamType);
	Push(input);
}

template < typename T_ELEMENT >
bool Stream::Pop(T_ELEMENT &rElement)
{
	size_t const element_size = sizeof(T_ELEMENT);
	return PopMemory( &rElement, element_size );
}

template< typename T_VALUE >
bool Stream::Pop( std::vector< T_VALUE > &vecout, size_t const max_count)
{
	vecout.clear();
	size_t arr_size = 0;
	if( Pop( arr_size ) 
	&&	arr_size <= max_count)
	{
		if( arr_size ) 
		{
			if( RemainSize() >= arr_size*sizeof(T_VALUE) )
			{
				vecout.resize(arr_size);
				return PopMemory(&vecout.at(0), vecout.size() * sizeof(std::vector< T_VALUE >::value_type));
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}

template< typename T_VALUE >
bool Stream::Pop( std::list< T_VALUE > &out, size_t const max_count)
{
	out.clear();
	size_t arr_size = 0;
	if( Pop( arr_size ) 
	&&	arr_size <= max_count)
	{
		if( arr_size )
		{
			if( RemainSize() >= arr_size*sizeof(T_VALUE) )
			{
				while( arr_size )
				{
					std::list< T_VALUE >::value_type element;
					if(! Pop( element ) ){return false;}
					out.push_back( element );

					--arr_size;
				}
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}

template< typename T_KEY, typename T_VALUE >
bool Stream::Pop( std::map< T_KEY, T_VALUE > &out, size_t const max_count)
{
	out.clear();
	size_t arr_size = 0;
	if( Pop( arr_size ) 
	&&	arr_size <= max_count)
	{
		if( arr_size )
		{
			if( RemainSize() >= arr_size*(sizeof(T_KEY)+sizeof(T_VALUE)) )
			{
				while( arr_size )
				{
					std::map< T_KEY, T_VALUE >::key_type first;
					std::map< T_KEY, T_VALUE >::mapped_type second;

					if(! Pop( first ) ){return false;}
					if(! Pop( second ) ){return false;}
					
					auto ret = out.insert( std::make_pair( first, second ) );

					if( !ret.second )
					{
						assert(false);
						return false;
					}
					--arr_size;
				}
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}

template< typename T_KEY, typename T_VALUE >
bool Stream::Pop( std::multimap< T_KEY, T_VALUE > &input, size_t const max_count)
{
	out.clear();
	size_t arr_size = 0;
	if( Pop( arr_size ) 
	&&	arr_size <= max_count)
	{
		if( arr_size )
		{
			if( RemainSize() >= arr_size*(sizeof(T_KEY)+sizeof(T_VALUE)) )
			{
				while( arr_size )
				{
					std::map< T_KEY, T_VALUE >::key_type first;
					std::map< T_KEY, T_VALUE >::mapped_type second;

					if(! Pop( first ) ){return false;}
					if(! Pop( second ) ){return false;}
					
					std::inserter(out, out.end()) = std::make_pair( first, second );
					--arr_size;
				}
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}

template< typename T_KEY, typename T_VALUE, typename T_HASHER >
bool Stream::Pop( std::unordered_map< T_KEY, T_VALUE, T_HASHER > &out, size_t const max_count)
{
	out.clear();
	size_t arr_size = 0;
	if( Pop( arr_size ) 
	&&	arr_size <= max_count)
	{
		if( arr_size )
		{
			if( RemainSize() >= arr_size*(sizeof(T_KEY)+sizeof(T_VALUE)) )
			{
				while( arr_size )
				{
					std::unordered_map< T_KEY, T_VALUE, T_HASHER >::key_type first;
					std::unordered_map< T_KEY, T_VALUE, T_HASHER >::mapped_type second;

					if(! Pop( first ) ){return false;}
					if(! Pop( second ) ){return false;}
					
					std::unordered_map< T_KEY, T_VALUE, T_HASHER >auto ret = out.insert( std::make_pair( first, second ) );

					if( !ret.second )
					{
						assert(false);
						return false;
					}
					--arr_size;
				}
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}

template< typename T_VALUE >
bool Stream::Pop( std::set< T_VALUE > &out, size_t const max_count)
{
	out.clear();

	size_t arr_size = 0;
	if( Pop(arr_size) 
	&&	arr_size <= max_count)
	{
		if( arr_size )
		{
			if( RemainSize() >= arr_size*sizeof(T_VALUE) )
			{
				while(arr_size)
				{
					std::set< T_VALUE >::value_type kValue;
					if( !Pop(kValue) ) {return false;};
					
					out.insert( kValue );
					--arr_size;
				}
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}

template< typename T_VALUE >
bool Stream::Pop( std::deque< T_VALUE > &out, size_t const max_count)
{
	std::vector< T_VALUE > vec;
	if( Pop(vec) )
	{
		out.assign(vec.begin(), vec.end());
		return true;
	}
	return false;
}

template< typename T_INPUT >
void Stream::Push( T_INPUT const& input )
{
	size_t const in_size = sizeof( T_INPUT );

//	여기는 자주 호출 되므로. reserve 가 좋지 않다  Reserve( Size() + in_size );//메모리 예약

	Push( (void const*)&input, in_size);
}

template< >
void Stream::Push( std::string const& input )
{
	size_t const in_size = input.length();
	
	Reserve( Size() + (sizeof(in_size) + in_size));//메모리 예약

	Push( in_size );
	Push( input.c_str(), in_size);
}

template< >
void Stream::Push( std::wstring const& input )
{
	size_t const in_size = input.length();
	
	Reserve( Size() + (sizeof(in_size) + in_size*sizeof(std::wstring::value_type)));//메모리 예약

	Push( in_size );
	Push( input.c_str(), in_size * sizeof(std::wstring::value_type) );
}

template< >
void Stream::Push( BM::vstring const& input )
{
	this->Push( static_cast<std::wstring>(input) );
}

template< >
void Stream::Push(  Stream const& input )
{
	if( this == &input)
	{
		return;
	}

	if( input.wr_pos_ > input.rd_pos_  )// 0이하가 되는 경우 막기위함.
	{
		size_t const in_size = input.wr_pos_ - input.rd_pos_;

		Reserve( Size() + in_size );//메모리 예약

		Push( &input.m_vecData.at( input.rd_pos_ ), in_size );
	}
}

template< typename T_VALUE >
void Stream::Push( std::vector< T_VALUE > const& input )
{
	size_t const in_size = input.size();
	Reserve( Size() + (sizeof(in_size) + in_size * sizeof(T_VALUE)));//메모리 예약

	Push( in_size );
	if( in_size )
	{
		Push( &input.at(0), input.size() * sizeof(std::vector< T_VALUE >::value_type) );
	}
}

template< typename T_VALUE >
void Stream::Push( std::list< T_VALUE > const& input )
{
	size_t const in_size = input.size();
	Reserve( Size() + (sizeof(in_size) + in_size * sizeof(T_VALUE)) );//메모리 예약

	Push( in_size );
	if( in_size )
	{
		std::list< T_VALUE >::const_iterator itor = input.begin();
		while( input.end() != itor )
		{
			Push( (*itor) );
			++itor;
		}
	}
}

template< typename T_KEY, typename T_VALUE >
void Stream::Push( std::map< T_KEY, T_VALUE > const& input )
{
	size_t const in_size = input.size();
	Reserve( Size() + (sizeof(in_size) + in_size * (sizeof(T_KEY) + sizeof(T_VALUE))));//메모리 예약

	Push( in_size );
	if( in_size )
	{
		std::map< T_KEY, T_VALUE >::const_iterator itor = input.begin();
		while( input.end() != itor )
		{
			Push( (*itor).first );
			Push( (*itor).second );
			++itor;
		}
	}
}

template< typename T_KEY, typename T_VALUE >
void Stream::Push( std::multimap< T_KEY, T_VALUE > const& input )
{
	size_t const in_size = input.size();
	Reserve( Size() + (sizeof(in_size) + in_size * (sizeof(T_KEY) + sizeof(T_VALUE))));//메모리 예약

	Push( in_size );
	if( in_size )
	{
		Reserve( Size() + (in_size * (sizeof(T_KEY) + sizeof(T_VALUE))));//메모리 예약

		std::map< T_KEY, T_VALUE >::const_iterator itor = input.begin();
		while( input.end() != itor )
		{
			Push( (*itor).first );
			Push( (*itor).second );
			++itor;
		}
	}
}

template< typename T_KEY, typename T_VALUE, typename T_HASHER >
void Stream::Push( std::unordered_map< T_KEY, T_VALUE, T_HASHER > const& input )
{
	size_t const in_size = input.size();
	Reserve( Size() + (sizeof(in_size) + in_size * (sizeof(T_KEY) + sizeof(T_VALUE))));//메모리 예약

	Push( in_size );
	if( in_size )
	{
		std::unordered_map< T_KEY, T_VALUE, T_HASHER >::const_iterator itor = input.begin();
		while( input.end() != itor )
		{
			Push( (*itor).first );
			Push( (*itor).second );
			++itor;
		}
	}
}

template< typename T_VALUE >
void Stream::Push( std::set< T_VALUE > const& input)
{
	size_t const in_size = input.size();
	Reserve( Size() + (sizeof(in_size) + in_size * (sizeof(T_VALUE))));//메모리 예약

	Push(in_size);
	std::set< T_VALUE >::const_iterator iter = input.begin();
	while(input.end() != iter)
	{
		Push( (*iter) );
		++iter;
	}
}

template< typename T_VALUE >
void Stream::Push( std::deque< T_VALUE > const& input )
{
	std::vector< T_VALUE > vec(input.begin(),input.end());
	Push(vec);
}
