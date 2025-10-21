/*****************************************************************

    MODULE    : PropertyType.h

    PURPOSE   : The ErrorData class is used to describe global errors.

    CREATED   : 5/15/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/

#ifndef FXSTUDIO_PROPERTYTYPE_H_
#define FXSTUDIO_PROPERTYTYPE_H_

#include "FxStudioDefines.h"

namespace FxStudio
{
	// Namespace: PropertyType
	//
	//		The property type enumerations are put into their
	//		own namespace so that they can have simple names without
	//		clashing with other symbols.
	//
	namespace PropertyType
	{
		// Enumerations: Enum
		//
		//		Integer - Supports int values.
		//				  "Integer Value", "Color Value", "Boolean", "Check Box", 
		//				  "Radio Button" and "Drop Down List" pack to this.
		//
		//		IntegerRange - Supports <IntegerRange> values.
		//					   "Integer Range" packs to this.
		//
		//		IntegerArray - Will support an integer array type.
		//					   Not yet available.
		//
		//		ColorARGBKeyFrame - Supports <ColorARGBKeyFrameData> values.
		//							"Color Ramp" packs to this.
		//
		//		Float - Supports float values.
		//				"Float Value" packs to this.
		//	
		//		FloatRange - Supports <FloatRange> values.
		//					 "Float Range" packs to this.
		//
		//		FloatArray - Will support a float array type.
		//					 Not yet available.
		//
		//		FloatKeyFrame - Supports <FloatKeyFrameData> values.
		//						"Float Ramp" packs to this.
		//
		//		String - Supports const char* values.
		//				 "Text" and "Custom String" pack to this.
		//				 
		//		StringArray - Will support an array of const char* values.
		//					  Not yet available.
		//
		//		Vector3 - Supports <Vector3> values.
		//				  "Vector3" packs to this.
		//
		//		Vector3Range - Will support a <Vector3> range value.
		//					   Not yet available.
		//
		//		Vector3Array - Will support a <Vector3> array.
		//					   Not yet available.
		//
		//		FixedFunction - Supports <FixedFunction> values.
		//						"FixedFunction" packs to this.
		//
		//		Vector4 - Supports <Vector4> values.
		//				  "SweptAngle" packs to this with the following parameters :
		//						x : start of the planar angle sweep
		//						y : end of the planar angle sweep
		//						z : start of the declination angle sweep
		//						w : end of the declination angle sweep
		//
		enum Enum
		{
            Integer = 0,
            IntegerRange,
            IntegerArray,
			ColorARGBKeyFrame,
            Float,
            FloatRange,
            FloatArray,
			FloatKeyFrame,
            String,
            StringArray,
            Vector3,
            Vector3Range,
            Vector3Array,
            FixedFunction,
            Vector4,

			Count,

			Invalid = 0xFFFFFFFF
		};

	}

	///////////////////////////////////////////////////////////////
	//	Struct: Vector3
	//
	//		Holds a 3-dimensional float vector, stored as 3 floats.
	//		This struct can be safely cast to most other Vector3 implementations.
	//
	struct FXSTUDIO_API Vector3
	{
		float m_fX;
		float m_fY;
		float m_fZ;

		Vector3()
			: m_fX(0)
			, m_fY(0)
			, m_fZ(0) {}

		Vector3(float fX, float fY, float fZ)
			: m_fX(fX)
			, m_fY(fY)
			, m_fZ(fZ) {}
	};

	inline bool operator==(const Vector3& lhs, const Vector3& rhs)
	{
		return lhs.m_fX == rhs.m_fX
			&& lhs.m_fY == rhs.m_fY
			&& lhs.m_fZ == rhs.m_fZ;
	}

	///////////////////////////////////////////////////////////////
	//	Struct: Vector4
	//
	//		Holds a 4-dimensional float vector, stored as 4 floats.
	//		The four floats are in x, y, z, w order.
	//
	struct FXSTUDIO_API Vector4
	{
		float m_fX;
		float m_fY;
		float m_fZ;
		float m_fW;

		Vector4()
			: m_fX(0)
			, m_fY(0)
			, m_fZ(0)
			, m_fW(0) {}

		Vector4(float fX, float fY, float fZ, float fW)
			: m_fX(fX)
			, m_fY(fY)
			, m_fZ(fZ)
			, m_fW(fW) {}
	};

	inline bool operator==(const Vector4& lhs, const Vector4& rhs)
	{
		return lhs.m_fX == rhs.m_fX
			&& lhs.m_fY == rhs.m_fY
			&& lhs.m_fZ == rhs.m_fZ
			&& lhs.m_fW == rhs.m_fW;
	}
	
	///////////////////////////////////////////////////////////////
	//	Struct: IntegerRange
	//
	//		Holds the minimum and maximum value of an integer range.
	//
	struct FXSTUDIO_API IntegerRange
	{
		int m_nMin;
		int m_nMax;

		IntegerRange()
			: m_nMin(0)
			, m_nMax(0) {}

		IntegerRange(int nMin, int nMax)
			: m_nMin(nMin)
			, m_nMax(nMax) {}
	};

	inline bool operator==(const IntegerRange& lhs, const IntegerRange& rhs)
	{
		return lhs.m_nMin == rhs.m_nMin
			&& lhs.m_nMax == rhs.m_nMax;
	}

	///////////////////////////////////////////////////////////////
	//	Struct: FloatRange
	//
	//		Holds the minimum and maximum value of a float range.
	//
	struct FXSTUDIO_API FloatRange
	{
		float m_fMin;
		float m_fMax;

		FloatRange()
			: m_fMin(0.0f)
			, m_fMax(0.0f) {}

		FloatRange(float fMin, float fMax)
			: m_fMin(fMin)
			, m_fMax(fMax) {}
	};

	inline bool operator==(const FloatRange& lhs, const FloatRange& rhs)
	{
		return lhs.m_fMin == rhs.m_fMin
			&& lhs.m_fMax == rhs.m_fMax;
	}
	
	///////////////////////////////////////////////////////////////
	//	Struct: ColorARGB
	//
	//		This is utility struct to allow an ARGB integer to be
	//		easily un-packed.
	//
	struct ColorARGB
	{
#ifdef FXSTUDIO_LITTLE_ENDIAN
		unsigned char m_blue;		
		unsigned char m_green;
		unsigned char m_red;
		unsigned char m_alpha;
#else
		unsigned char m_alpha;
		unsigned char m_red;
		unsigned char m_green;
		unsigned char m_blue;		
#endif
	};

	///////////////////////////////////////////////////////////////
	//	Struct: ColorARGBKeyFrame
	//
	//		The key-frame element of <ColorARGBKeyFrameData>.
	//
	struct FXSTUDIO_API ColorARGBKeyFrame
	{
		float	m_fUnitTime;
		
		union
		{
			int m_nValue;
			ColorARGB m_color;		
		};	
	};

	///////////////////////////////////////////////////////////////
	//	Struct: ColorARGBKeyFrameData
	//
	//		This is the structure used to hold the ColorARGB key-framed data.
	//
	struct FXSTUDIO_API ColorARGBKeyFrameData
	{
		int					m_nNumFrames;
		ColorARGBKeyFrame	m_Keyframes[1];

		const ColorARGBKeyFrame* Begin() const { return m_Keyframes; }
		const ColorARGBKeyFrame* End()   const { return m_Keyframes + m_nNumFrames; }

		int Evaluate(float fUnitTime) const;
	};

	FXSTUDIO_API bool operator==(const ColorARGBKeyFrameData& lhs, const ColorARGBKeyFrameData& rhs);

	///////////////////////////////////////////////////////////////
	//	Structs: FloatKeyFrameData
	//
	//		This structure holds the key-framed float data.
	//
	struct FXSTUDIO_API FloatKeyFrameData
	{
		//	Enumerations: Type
		//		Linear - Keyframes are points to be linear interpolated.
		//		Cubic - Keyframes are to be interpolated with a cubic polynomial.
		//
		enum Type
		{
			Linear = 0,
			Cubic,

			Count,

			Invalid = 0xFFFFFFFF
		};


		Type m_eType;

		float Evaluate(float fUnitTime) const;
	};

	FXSTUDIO_API bool operator==(const FloatKeyFrameData& lhs, const FloatKeyFrameData& rhs);

	///////////////////////////////////////////////////////////////
	//	StructStruct: LinearFloatKeyFrame
	//
	//		The key-frame element of <LinearFloatKeyFrameData>.
	//
	struct FXSTUDIO_API LinearFloatKeyFrame
	{
		float	m_fUnitTime;
		float	m_fValue;
	};

	///////////////////////////////////////////////////////////////
	//	Structs: LinearFloatKeyFrameData
	//
	//		This structure holds the key-framed float data when packed as linear interpolated keyframes.
	//
	struct FXSTUDIO_API LinearFloatKeyFrameData : public FloatKeyFrameData
	{
		int					m_nNumFrames;
		LinearFloatKeyFrame	m_Keyframes[1];

		const LinearFloatKeyFrame* Begin() const { return m_Keyframes; }
		const LinearFloatKeyFrame* End()   const { return m_Keyframes + m_nNumFrames; }

		float Evaluate(float fUnitTime) const;
	};

	FXSTUDIO_API bool operator==(const LinearFloatKeyFrameData& lhs, const LinearFloatKeyFrameData& rhs);

	///////////////////////////////////////////////////////////////
	//	StructStruct: CubicFloatKeyFrame
	//
	//		The key-frame element of <CubicFloatKeyFrameData>.
	//		Represents a cubic equation, y = A*t*t*t + B*t*t + C*t + D.
	//		When evaluating the cubic, t is the zero at the start of the
	//		keyframe and is equal to m_fEndUnitTime at the end of the
	//		keyframe.
	//
	struct FXSTUDIO_API CubicFloatKeyFrame
	{
		// The keyframe is applicable when the time is
		// less than or equal to this this time.  This time ranges
		// from zero to one over the duration of the component, ie.
		// it is equivalent to <Component::GetUnitTime>.
		float	m_fEndUnitTime;


		float	m_fA;
		float	m_fB;
		float	m_fC;
		float	m_fD;
	};

	///////////////////////////////////////////////////////////////
	//	Structs: CubicFloatKeyFrameData
	//
	//		This structure holds the key-framed float data when packed as cubic interpolated keyframes.
	//
	struct FXSTUDIO_API CubicFloatKeyFrameData : public FloatKeyFrameData
	{
		int					m_nNumFrames;
		CubicFloatKeyFrame	m_Keyframes[1];

		const CubicFloatKeyFrame* Begin() const { return m_Keyframes; }
		const CubicFloatKeyFrame* End()   const { return m_Keyframes + m_nNumFrames; }

		float Evaluate(float fUnitTime) const;
	};

	FXSTUDIO_API bool operator==(const CubicFloatKeyFrameData& lhs, const CubicFloatKeyFrameData& rhs);

	///////////////////////////////////////////////////////////////
	//	Struct: FixedFunctionData
	//
	//		This structure holds the fixed function data.
	//
	struct FXSTUDIO_API FixedFunctionData
	{
		//	Enumerations: Type
		//		Linear - a linear ramp function.
		//		Quadratic - a quadratic function.
		//		Sinusoidal - a sinusoidal function.
		//
		enum Type
		{
			Linear = 0,
			Quadratic,
			Sinusoidal,

			Count,

			Invalid = 0xFFFFFFFF
		};


		// Struct: LinearData
		//		This is the data for a <Linear> function.  It represents the function 
		//		"x = a*t + b"
		//
		struct LinearData
		{
			float a;
			float b;
		};

		// Struct: QuadraticData
		//		This is the data for a <Quadratic> function.  It represents the function 
		//		"x = a*t*t + b*t + c"
		//
		struct QuadraticData
		{
			float a;
			float b;
			float c;
		};

		// Struct: SinusoidalData
		//		This is the data for a <Sinusoidal> function.  It represents the function 
		//		"x = a*sin(b*t + c) + d"
		//
		struct SinusoidalData
		{
			float a;
			float b;
			float c;
			float d;
		};

		// --------------- Data -----------------

		Type m_Type;

		union
		{
			LinearData		 m_Linear;
			QuadraticData	 m_Quadratic;
			SinusoidalData   m_Sinusoidal;

			// If you want to walk the list of coefficents,
			// take the address of this value.  The size
			// of the array depends on the function type.
			float			 m_Coefficients[1];
		};

		//	Function: Evaluate
		//
		//		Utility function to evaluate this data at a given time.
		//
		//	Parameters:
		//		fUnitTime - The time scaled to a value from 0 to 1.
		//
		//	Returns:
		//		The correct value for the given unit time.
		//
		//	Remarks:
		//		The unit time corresponds to <Component::GetUnitTime>.  
		//		0 corresponds to the component's start time.
		//		1 corresponds to the component's end time.
		//
		float Evaluate(float fUnitTime) const;
	};

	// This function will check if two fixed functions are equal by comparing their
	// types and then comparing their coefficients.
	FXSTUDIO_API bool operator==(const FixedFunctionData& lhs, const FixedFunctionData& rhs);
}

#endif // FXSTUDIO_PROPERTYTYPE_H_