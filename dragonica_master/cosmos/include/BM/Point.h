#pragma once
#include <math.h>

template< typename T >
class T_PT2
{
public:
	T_PT2()
	{
		x = 0, y = 0;
	}
	
	explicit T_PT2( POINT const& pt )
	{
		Set(pt.x, pt.y);
	}

	explicit T_PT2(T const in_x, T const in_y )
	{
		Set(in_x, in_y);
	}

	void Set(T const in_x, T const in_y)
	{
		x = in_x, y = in_y;
	}

	void Clear()
	{
		Set(0,0);
	}

	void operator += ( T_PT2< T > const& rhs)
	{
		x += rhs.x;
		y += rhs.y;
	}

	void operator -= ( T_PT2< T > const& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
	}

	void operator /= ( T_PT2< T > const& rhs)
	{
		if( 0 == rhs.x || 0 == rhs.y ){ return; }
		x /= rhs.x;
		y /= rhs.y;
	}

	void operator *= ( T_PT2< T > const& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
	}

	void operator *= (double const dbValue)
	{
		x *= dbValue;
		y *= dbValue;
	}

	T_PT2< T > operator + ( T_PT2< T > const& rhs) const
	{
		T_PT2< T > temp = *this;
		temp += rhs;
		return temp;
	}

	T_PT2< T > operator * ( double const& rhs) const
	{
		T_PT2< T > temp = *this;
		temp *= rhs;
		return temp;
	}

	T_PT2< T > operator - ( T_PT2< T > const& rhs) const
	{
		T_PT2< T > temp = *this;
		temp -= rhs;
		return temp;
	}

	T_PT2< T > operator / ( T_PT2< T > const& rhs) const
	{
		T_PT2< T > temp = *this;
		temp /= rhs;
		return temp;
	}

	operator ::POINT&()
	{
		return *(POINT*)this;
	}

	operator ::POINT const& () const
	{
		::POINT const *p = (::POINT const *)this;
		return *p;
	}

	bool operator < (T_PT2< T > const& rhs)const
	{
		if( x < rhs.x )	{return true;}
		if( x > rhs.x )	{return false;}

		if( y < rhs.y )	{return true;}
		if( y > rhs.y )	{return false;}
		return false;
	}

	bool operator == (T_PT2< T > const& rhs)const
	{
		if(	x == rhs.x
		&&	y == rhs.y )	{return true;}
		return false;
	}

	bool operator != (T_PT2< T > const& rhs)const
	{
		if(*this == rhs)
		{
			return false;
		}
		return true;
	}
	
	T	x;
	T	y;

	static T_PT2< T > const& NullData()
	{
		static T_PT2< T > const kNull;
		return kNull;
	}
};

typedef T_PT2< LONG > POINT2;
typedef T_PT2< char > POINT2BY;
typedef T_PT2< float > POINT2F;

template< typename T >
class T_PT3
{
public:
	T_PT3()
	{
		Set(0,0,0);
	}

	explicit T_PT3(T const in_x, T const in_y, T const in_z)
	{
		Set(in_x, in_y, in_z);
	}
	
	void Set(T const in_x, T const in_y, T const in_z)
	{
		x = in_x, y = in_y, z = in_z;
	}
	
	operator POINT2 const ()const throw()
	{
		return POINT2(x,y);
	}

	void operator += (T_PT3< T > const& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
	}

	void operator -= (T_PT3< T > const& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
	}
	
	void operator += (POINT2 const& rhs)
	{
		x += rhs.x;
		y += rhs.y;
	}

	void operator = (POINT2 const& rhs)
	{
		x = rhs.x;
		y = rhs.y;
	}

	template<typename T2>
	void operator = (T_PT3<T2> const&  rhs)
	{
		x = (T) rhs.x;
		y = (T) rhs.y;
		z = (T) rhs.z;
	}

	T_PT3< T > operator + (T_PT3< T > const& rhs)const
	{
		T_PT3< T > temp = *this;
		temp += rhs;
		return temp;
	}

	T_PT3< T > operator + (POINT2 const& rhs)const
	{
		T_PT3< T > temp = *this;
		temp += rhs;
		return temp;
	}
	
	T_PT3< T > operator - ()const
	{
		T_PT3< T > temp(-x,-y,-z);
		return temp;
	}

	T_PT3< T > operator - (T_PT3< T > const& rhs)const
	{
		T_PT3< T > temp = *this;
		temp -= rhs;
		return temp;
	}

	T_PT3< T > operator - (POINT2 const& rhs)const
	{
		T_PT3< T > temp = *this;
		temp.x -= rhs.x;
		temp.y -= rhs.y;
		return temp;
	}

	bool operator == (T_PT3< T > const& rhs)const
	{
		if(	x == rhs.x
		&&	y == rhs.y 
		&&	z == rhs.z)	
		{
			return true;
		}
		return false;
	}

	bool operator != (T_PT3< T > const& rhs)const
	{
		if(*this == rhs)
		{
			return false;
		}
		return true;
	}

	void Clear()
	{
		Set(0,0,0);
	}

	bool operator < (T_PT3< T > const& rhs)const
	{
		if( x < rhs.x )	{return true;}
		if( x > rhs.x )	{return false;}

		if( y < rhs.y )	{return true;}
		if( y > rhs.y )	{return false;}

		if( z < rhs.z )	{return true;}
		if( z > rhs.z )	{return false;}
		
		return false;
	}

	bool operator > (T_PT3< T > const& rhs)const
	{
		return (rhs < *this);
	}

	bool operator <= (T_PT3< T > const& rhs)const
	{
		if( x <= rhs.x 
		&&	y <= rhs.y 
		&&	z <= rhs.z ) 
		{
			return true;
		}

		return false;
	}
	bool operator >= (T_PT3< T > const& rhs)const
	{
		if( x >= rhs.x 
		&& y >= rhs.y 
		&& z >= rhs.z ) 
		{
			return true;
		}

		return false;
	}

	void operator *= (T_PT3< T > const& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
	}

	void operator /= ( T_PT3< T > const& rhs)
	{
		if( 0 == rhs.x || 0 == rhs.y || 0 == rhs.z ){ return; }
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
	}

	T_PT3< T > operator * (T_PT3< T > const& rhs) const
	{
		T_PT3< T > temp = *this;
		temp *= rhs;
		return temp;
	}

	T_PT3< T > operator / ( T_PT3< T > const& rhs) const
	{
		T_PT3< T > temp = *this;
		temp /= rhs;
		return temp;
	}

	void operator *= (T const kValue)
	{
		x *= kValue;
		y *= kValue;
		z *= kValue;
	}

	void operator /= (T const kValue)
	{
		x /= kValue;
		y /= kValue;
		z /= kValue;
	}

	void operator += (T const kValue)
	{
		x += kValue;
		y += kValue;
		z += kValue;
	}

	void operator -= (T const kValue)
	{
		x -= kValue;
		y -= kValue;
		z -= kValue;
	}

	T_PT3< T > operator * (T const& kValue)const
	{
		T_PT3< T > temp = *this;
		temp *= kValue;
		return temp;
	}

	T_PT3< T > operator / (T const& kValue)const
	{
		T_PT3< T > temp = *this;
		temp /= kValue;
		return temp;
	}

	T_PT3< T > operator + (T const& kValue)const
	{
		T_PT3< T > temp = *this;
		temp += kValue;
		return temp;
	}

	T_PT3< T > operator - (T const& kValue)const
	{
		T_PT3< T > temp = *this;
		temp -= kValue;
		return temp;
	}

	T	x;
	T	y;
	T	z;

	static T_PT3< T > const& NullData()
	{
		static T_PT3< T > const kNull;
		return kNull;
	}

	static T Distance( T_PT3< T > const& kPt1, T_PT3< T > const& kPt2)
	{
		return (T)sqrt(
			   pow(static_cast<float>(kPt1.x - kPt2.x), 2)
			+  pow(static_cast<float>(kPt1.y - kPt2.y), 2)
			+  pow(static_cast<float>(kPt1.z - kPt2.z), 2));
	}

	inline static T SqrDistance(T_PT3< T > const& kPt1, T_PT3< T > const& kPt2)
	{
		return pow(static_cast<float>(kPt1.x - kPt2.x), 2)
			+  pow(static_cast<float>(kPt1.y - kPt2.y), 2)
			+  pow(static_cast<float>(kPt1.z - kPt2.z), 2);
	}

	void Normalize()
	{
		float const fScalar = (T)sqrt(pow(static_cast<float>(x),2)+ pow(static_cast<float>(y),2) + pow(static_cast<float>(z),2));
		if(0 == fScalar)
		{
			return;
		}
		x /= fScalar;
		y /= fScalar;
		z /= fScalar;
	}

	T_PT3<T> Cross(T_PT3< T > const& kPt1) const
	{
		return T_PT3<T>(y*kPt1.z-z*kPt1.y, z*kPt1.x-x*kPt1.z, x*kPt1.y-y*kPt1.x);
	}
};

typedef T_PT3<float>	POINT3;
typedef T_PT3<int>		POINT3I;
typedef T_PT3<char>		POINT3BY;
typedef T_PT3<short>	POINT3SH;

template< typename _T >
BM::vstring& operator <<(BM::vstring& lhs, T_PT2< _T > const& rhs)
{
	lhs += BM::vstring(L" POINT2[X:")<<rhs.x<<L",Y:"<<rhs.y<<L"]";
	return lhs;
}

template< typename _T >
BM::vstring& operator <<(BM::vstring& lhs, T_PT3< _T > const& rhs)
{
	lhs += BM::vstring(L" POINT3[X:")<<rhs.x<<L",Y:"<<rhs.y<<L",Z:"<<rhs.z<<L"]";
	return lhs;
}

// char, unsigned char만 특수화 (그냥 출력하면, 문자로 출력된다)
template<>	BM::vstring& operator <<(BM::vstring& lhs, T_PT3< char > const& rhs);
template<>	BM::vstring& operator <<(BM::vstring& lhs, T_PT3< unsigned char > const& rhs);