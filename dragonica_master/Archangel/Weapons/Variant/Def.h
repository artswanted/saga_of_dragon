#ifndef WEAPON_VARIANT_BASIC_CONSTANT_DEF_H
#define WEAPON_VARIANT_BASIC_CONSTANT_DEF_H

#include <math.h>

typedef struct tagVector
{
	float X, Y, Z;

	tagVector( float fX=0.0f, float fY=0.0f, float fZ=0.0f )
		: X(fX), Y(fY), Z(fZ)
	{}

	// YonMy Vector Guide Method 20070810------------------>
	bool IsFloatEqual( float const fLeft, float const fRight )
	{
		return abs(fLeft - fRight) < 0.001f;
	}

	tagVector& operator=( tagVector const& vec )
	{
		X = vec.X;	Y = vec.Y;	Z = vec.Z;
		return *this;
	}

	bool operator==( tagVector const& vec )
	{
		//  float의 오차 허용 범위를 적용한다.
		return ( IsFloatEqual(X, vec.X) && IsFloatEqual(Y, vec.Y) && IsFloatEqual(Z, vec.Z) );
	}

	tagVector& operator+( tagVector const& vec )
	{
		X += vec.X;	Y += vec.Y;	Z += vec.Z;
		return *this;
	}

	tagVector& operator-( tagVector const& vec )
	{
		X -= vec.X;	Y -= vec.Y;	Z -= vec.Z;
		return *this;
	}

	float Length( tagVector const& vec )
	{
		return sqrt( LengthSQ( vec ) );
	}

	float LengthSQ( tagVector const& vec )
	{
		return ( abs(X - vec.X) + abs(Y - vec.Y) + abs(Z - vec.Z) );
	}

}SVector;

#define GV_DISTANCE_UNIT		0.3937f // 1cm = 0.3937gb
#define METRIC_TO_GB(a)	((float)a * GV_DISTANCE_UNIT)

#endif // WEAPON_VARIANT_BASIC_CONSTANT_DEF_H