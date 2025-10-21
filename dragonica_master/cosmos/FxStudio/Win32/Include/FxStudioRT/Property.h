/*****************************************************************

    MODULE    : Property.h

    PURPOSE   : A property class for efficient access to component data.

    CREATED   : 8/5/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/
#ifndef FXSTUDIO_PROPERTY_H_
#define FXSTUDIO_PROPERTY_H_

#include "FxStudioDefines.h"

#include "PropertyType.h"
#include "Component.h"

//
// Title: Property
//

namespace FxStudio
{
	//	Class: BaseProperty
	//
	//		All properties inherit from this class.
	//
	class FXSTUDIO_API BaseProperty
	{
	public :

		typedef char (InternalDataType)[28];

	public :

		//	Constructor: BaseProperty
		//
		//		This constructor creates an unbound Property class.
		//
		BaseProperty();

		//	Constructor: BaseProperty
		//
		//		This constructor creates a Property class bound to a specific component 
		//		property.
		//
		//	Parameters:
		//		szPropertyName - The name of the property, as it appears in Designer.
		//						 It is case insensitive.
		//		pComponent - The component owning the property.
		//		propertyType - The expected type of the property.
		//		bAllowDynamicReload - Set to true if changes to this property will
		//							  be handled every frame.
		//
		//	Remarks:
		//		It is recommended that this constructor be used when the Property is a
		//		member variable of a Component.  This constructor calls <BindBase> to bind
		//		the Property to its data.
		//
		//		No virtual functions will be called on the component pointer, so it
		//		is safe to pass it into the Property member variable from the constructor.
		//
		BaseProperty(const char* szPropertyName, const Component* pComponent, PropertyType::Enum propertyType, bool bAllowDynamicReload = true);
		
		//	Destructor: ~BaseProperty
		//
		//		Releases the Property class from the property data.
		//
		virtual ~BaseProperty();
		

		//	Function: HasLOD
		//
		//		Determines if the property has LOD specific data.
		//
		//	Returns:
		//		True if the property has LOD specific data.
		//
		bool HasLOD() const;

		//	Function: GetNumberChannels
		//
		//		Determines the number of channels of data defined for the property.
		//
		//	Returns:
		//		Returns the number of channels if it is a key-framed property.
		//		Returns 1 otherwise.
		//
		//	Remarks:
		//		This function is only useful for key-framed properties.
		//
		unsigned int GetNumberChannels() const;

		//	Function: IsValid
		//
		//		Determines if the Property class has been successfully bound to a 
		//		component property.
		//
		//	Returns:
		//		True if the Property class has been successfully bound.
		//
		bool IsValid() const;

		//	Function: AllowDynamicReload
		//
		//		Determines if the property is read often enough to not require
		//		a previewed Fx to restart when it changes.
		//
		//	Returns:
		//		True if the property is read often.
		//
		//	Remarks:
		//		This is simply returning the value passed into the constructor
		//		or <BindBase>.  It is used internally.  There should be no need
		//		for it to be called outside the Runtime.
		//
		bool AllowDynamicReload() const { return m_bAllowDynamicReload; }

		//	Function: GetPropertyName
		//
		//		Returns the bound property's name, exactly as it appears in Designer.
		//
		//	Returns:
		//		The property's name.  If the Property class is not bound, it will return
		//		an empty string.  It never returns NULL.
		//
		//	Remarks:
		//		This is the pointer to the property name as it was packed into the Fx bank.
		//
		const char* GetPropertyName() const;

	protected :

		//	Function: BindBase
		//
		//		This function is called to bind the class to its property data.
		//
		//	Parameters:
		//		szPropertyName - The name of the property, as it appears in Designer.
		//						 It is case insensitive.
		//		pComponent - The component owning the property.
		//		propertyType - The expected type of the property.
		//		bAllowDynamicReload - Set to true if changes to this property will
		//							  be handled every frame.
		//
		//	Returns:
		//		True if successfully bound.
		//
		//	Remarks:
		//		If this function fails, it will call <ErrorHandler::HandleError> and return
		//		false.
		//
		bool BindBase(const char* szPropertyName, const Component* pComponent, PropertyType::Enum propertyType);

		//	Function: GetValuePtr
		//
		//		Returns a pointer to the data value.
		//
		//	Parameters:
		//		nChannel - The channel to be accessed.
		//		propertyType - The expected type of the property.
		//
		//	Returns:
		//		Pointer to the value if this call has been successfully bound to its data.
		//
		//	Remarks:
		//		The property type is not held as a member variable in order to keep
		//		the memory requirements of the Property class as low as possible.
		//
		const void* GetValuePtr(PropertyType::Enum propertyType) const { return GetValuePtr(0, propertyType); }
		const void* GetValuePtr(unsigned int nChannel, PropertyType::Enum propertyType) const;

		//	Function: GetValueAtLODPtr
		//
		//		Returns a pointer to the data value at a specific LOD.
		//
		//	Parameters:
		//		nChannel - The channel to be accessed.
		//		nLODLevel - The LOD level being requested.
		//		propertyType - The expected type of the property.
		//		
		//
		//	Returns:
		//		Pointer to the value if this call has been successfully bound to its data.
		//
		//	Remarks:
		//		The property type is not held as a member variable in order to keep
		//		the memory requirements of the Property class as low as possible.
		//
		const void* GetValueAtLODPtr(int nLODLevel, PropertyType::Enum propertyType) const { return GetValueAtLODPtr(0, nLODLevel, propertyType); }
		const void* GetValueAtLODPtr(unsigned int nChannel, int nLODLevel, PropertyType::Enum propertyType) const;

		const Component*	m_pComponent;
		bool				m_bAllowDynamicReload;
		char				m_Padding[3];

	private :

		// This class cannot be copied.
		BaseProperty(const BaseProperty&);
		BaseProperty& operator=(const BaseProperty&);

		InternalDataType m_Internal;
	};

	///////////////////////////////////////////////////////////////
	//	Class: IntegerProperty
	//
	//		Binds to a <PropertyType::Integer> property.
	//
	class FXSTUDIO_API IntegerProperty : public BaseProperty
	{
	public :

		IntegerProperty() 	
		{ }

		IntegerProperty(const char* szPropertyName, const Component* pComponent, bool bAllowDynamicReload = true)
			: BaseProperty(szPropertyName, pComponent, PropertyType::Integer, bAllowDynamicReload)
		{ }

		bool Bind(const char* szPropertyName, const Component* pComponent)
		{
			return BindBase(szPropertyName, pComponent, PropertyType::Integer);
		}

		int GetValue(int nDefaultValue = 0) const;
		int GetValueAtLOD(int nLODLevel, int nDefaultValue = 0) const;

		operator int() const { return GetValue(); }

	};

	///////////////////////////////////////////////////////////////
	//	Class: BooleanProperty
	//
	//		Binds to a <PropertyType::Integer> property, which
	//		is interpreted as a Boolean value.
	//
	class FXSTUDIO_API BooleanProperty : public BaseProperty
	{
	public :

		BooleanProperty() 	
		{ }

		BooleanProperty(const char* szPropertyName, const Component* pComponent, bool bAllowDynamicReload = true)
			: BaseProperty(szPropertyName, pComponent, PropertyType::Integer, bAllowDynamicReload)
		{ }

		bool Bind(const char* szPropertyName, const Component* pComponent)
		{
			return BindBase(szPropertyName, pComponent, PropertyType::Integer);
		}

		bool GetValue(bool bDefaultValue = false) const;
		bool GetValueAtLOD(int nLODLevel, bool bDefaultValue = false) const;

		operator bool() const { return GetValue(); }

	};

	///////////////////////////////////////////////////////////////
	//	Class: FloatProperty
	//
	//		Binds to a <PropertyType::Float> property.
	//
	class FXSTUDIO_API FloatProperty : public BaseProperty
	{
	public :

		FloatProperty() { }

		FloatProperty(const char* szPropertyName, const Component* pComponent, bool bAllowDynamicReload = true)
			: BaseProperty(szPropertyName, pComponent, PropertyType::Float, bAllowDynamicReload)
		{
		}

		bool Bind(const char* szPropertyName, const Component* pComponent)
		{
			return BindBase(szPropertyName, pComponent, PropertyType::Float);
		}

		float GetValue(float fDefaultValue = 0.0f) const;
		float GetValueAtLOD(int nLODLevel, float fDefaultValue = 0.0f) const;

		operator float() const { return GetValue(); }

	};


	///////////////////////////////////////////////////////////////
	//	Class: StringProperty
	//
	//		Binds to a <PropertyType::String> property.
	//
	class FXSTUDIO_API StringProperty : public BaseProperty
	{
	public :

		StringProperty() { }

		StringProperty(const char* szPropertyName, const Component* pComponent, bool bAllowDynamicReload = true)
			: BaseProperty(szPropertyName, pComponent, PropertyType::String, bAllowDynamicReload)
		{
		}

		bool Bind(const char* szPropertyName, const Component* pComponent)
		{
			return BindBase(szPropertyName, pComponent, PropertyType::String);
		}

		const char* GetValue(const char* szDefaultValue = "") const;
		const char* GetValueAtLOD(int nLODLevel, const char* szDefaultValue = "") const;

		operator const char*() const { return GetValue(); }

	};

	///////////////////////////////////////////////////////////////
	//	Class: Vector3Property
	//
	//		Binds to a <PropertyType::Vector3> property.
	//
	class FXSTUDIO_API Vector3Property : public BaseProperty
	{
	public :

		Vector3Property() { }

		Vector3Property(const char* szPropertyName, const Component* pComponent, bool bAllowDynamicReload = true)
			: BaseProperty(szPropertyName, pComponent, PropertyType::Vector3, bAllowDynamicReload)
		{
		}

		bool Bind(const char* szPropertyName, const Component* pComponent)
		{
			return BindBase(szPropertyName, pComponent, PropertyType::Vector3);
		}

		const FxStudio::Vector3& GetValue(const FxStudio::Vector3& vDefaultValue = FxStudio::Vector3()) const;
		const FxStudio::Vector3& GetValueAtLOD(int nLODLevel, const FxStudio::Vector3& vDefaultValue = FxStudio::Vector3()) const;

		operator const FxStudio::Vector3&() const { return GetValue(); }

	};

	///////////////////////////////////////////////////////////////
	//	Class: Vector4Property
	//
	//		Binds to a <PropertyType::Vector4> property.
	//
	class FXSTUDIO_API Vector4Property : public BaseProperty
	{
	public :

		Vector4Property() { }

		Vector4Property(const char* szPropertyName, const Component* pComponent, bool bAllowDynamicReload = true)
			: BaseProperty(szPropertyName, pComponent, PropertyType::Vector4, bAllowDynamicReload)
		{
		}

		bool Bind(const char* szPropertyName, const Component* pComponent)
		{
			return BindBase(szPropertyName, pComponent, PropertyType::Vector4);
		}

		const FxStudio::Vector4& GetValue(const FxStudio::Vector4& vDefaultValue = FxStudio::Vector4()) const;
		const FxStudio::Vector4& GetValueAtLOD(int nLODLevel, const FxStudio::Vector4& vDefaultValue = FxStudio::Vector4()) const;

		operator const FxStudio::Vector4&() const { return GetValue(); }

	};
	
	///////////////////////////////////////////////////////////////
	//	Class: FloatRangeProperty
	//
	//		Binds to a <PropertyType::FloatRange> property.
	//
	class FXSTUDIO_API FloatRangeProperty : public BaseProperty
	{
	public :

		FloatRangeProperty() { }

		FloatRangeProperty(const char* szPropertyName, const Component* pComponent, bool bAllowDynamicReload = true)
			: BaseProperty(szPropertyName, pComponent, PropertyType::FloatRange, bAllowDynamicReload)
		{
		}

		bool Bind(const char* szPropertyName, const Component* pComponent)
		{
			return BindBase(szPropertyName, pComponent, PropertyType::FloatRange);
		}

		const FxStudio::FloatRange& GetValue(const FxStudio::FloatRange& vDefaultValue = FxStudio::FloatRange()) const;
		const FxStudio::FloatRange& GetValueAtLOD(int nLODLevel, const FxStudio::FloatRange& vDefaultValue = FxStudio::FloatRange()) const;

		operator const FxStudio::FloatRange&() const { return GetValue(); }
	};

	///////////////////////////////////////////////////////////////
	//	Class: IntegerRangeProperty
	//
	//		Binds to a <PropertyType::IntegerRange> property.
	//
	class FXSTUDIO_API IntegerRangeProperty : public BaseProperty
	{
	public :

		IntegerRangeProperty() { }

		IntegerRangeProperty(const char* szPropertyName, const Component* pComponent, bool bAllowDynamicReload = true)
			: BaseProperty(szPropertyName, pComponent, PropertyType::IntegerRange, bAllowDynamicReload)
		{
		}

		bool Bind(const char* szPropertyName, const Component* pComponent)
		{
			return BindBase(szPropertyName, pComponent, PropertyType::IntegerRange);
		}

		const FxStudio::IntegerRange& GetValue(const FxStudio::IntegerRange& vDefaultValue = FxStudio::IntegerRange()) const;
		const FxStudio::IntegerRange& GetValueAtLOD(int nLODLevel, const FxStudio::IntegerRange& vDefaultValue = FxStudio::IntegerRange()) const;

		operator const FxStudio::IntegerRange&() const { return GetValue(); }
	};

	///////////////////////////////////////////////////////////////
	//	Class: FixedFunctionProperty
	//
	//		Binds to a <PropertyType::FixedFunction> property.
	//
	class FXSTUDIO_API FixedFunctionProperty : public BaseProperty
	{
	public :

		FixedFunctionProperty() { }

		FixedFunctionProperty(const char* szPropertyName, const Component* pComponent, bool bAllowDynamicReload = true)
			: BaseProperty(szPropertyName, pComponent, PropertyType::FixedFunction, bAllowDynamicReload)
		{
		}

		bool Bind(const char* szPropertyName, const Component* pComponent)
		{
			return BindBase(szPropertyName, pComponent, PropertyType::FixedFunction);
		}

		float GetValue(float fDefaultValue = 0.0f) const
		{
			if( !m_pComponent )
				return fDefaultValue;

			return GetValueAtTime(m_pComponent->GetUnitTime(), fDefaultValue);
		}

		float GetValueAtLOD(int nLODLevel, float fDefaultValue = 0.0f) const
		{
			if( !m_pComponent )
				return fDefaultValue;

			return GetValueAtLOD(m_pComponent->GetUnitTime(), nLODLevel, fDefaultValue);
		}

		float GetValueAtTime(float fUnitTime, float fDefaultValue = 0.0f) const;
		float GetValueAtLOD(float fUnitTime, int nLODLevel, float fDefaultValue = 0.0f) const;

		const FxStudio::FixedFunctionData* GetData() const
		{
			return static_cast<const FixedFunctionData*>(GetValuePtr(PropertyType::FixedFunction));
		}

		const FxStudio::FixedFunctionData* GetDataAtLOD(int nLODLevel) const
		{
			return static_cast<const FixedFunctionData*>(GetValueAtLODPtr(nLODLevel, PropertyType::FixedFunction));
		}

		operator float() const { return GetValue(); }

	};

	///////////////////////////////////////////////////////////////
	//	Class: ColorARGBKeyFrameProperty
	//
	//		Binds to a <PropertyType::ColorARGBKeyFrame> property.
	//		The maximum number of channels can be determined with
	//		<BaseProperty::GetNumberChannels>.
	//
	class FXSTUDIO_API ColorARGBKeyFrameProperty : public BaseProperty
	{
	public :

		ColorARGBKeyFrameProperty() { }

		ColorARGBKeyFrameProperty(const char* szPropertyName, const Component* pComponent, bool bAllowDynamicReload = true)
			: BaseProperty(szPropertyName, pComponent, PropertyType::ColorARGBKeyFrame, bAllowDynamicReload)
		{
		}

		bool Bind(const char* szPropertyName, const Component* pComponent)
		{
			return BindBase(szPropertyName, pComponent, PropertyType::ColorARGBKeyFrame);
		}

		// BaseProperty has the function "GetNumberChannels()" for determining the number of channels.

		int GetValue(unsigned int nChannel, int nDefaultValue = 0) const
		{
			if( !m_pComponent )
				return nDefaultValue;

			return GetValueAtTime(nChannel, m_pComponent->GetUnitTime(), nDefaultValue);
		}

		int GetValueAtLOD(unsigned int nChannel, int nLODLevel, int nDefaultValue = 0) const
		{
			if( !m_pComponent )
				return nDefaultValue;

			return GetValueAtLOD(nChannel, m_pComponent->GetUnitTime(), nLODLevel, nDefaultValue);
		}

		int GetValueAtTime(unsigned int nChannel, float fUnitTime, int nDefaultValue = 0) const;
		int GetValueAtLOD(unsigned int nChannel, float fUnitTime, int nLODLevel, int nDefaultValue = 0) const;

		const FxStudio::ColorARGBKeyFrameData* GetData(unsigned int nChannel) const
		{
			return static_cast<const ColorARGBKeyFrameData*>(GetValuePtr(nChannel, PropertyType::ColorARGBKeyFrame));
		}

		const FxStudio::ColorARGBKeyFrameData* GetDataAtLOD(unsigned int nChannel, int nLODLevel) const
		{
			return static_cast<const ColorARGBKeyFrameData*>(GetValueAtLODPtr(nChannel, nLODLevel, PropertyType::ColorARGBKeyFrame));
		}

		operator int() const { return GetValue(0); }

		int operator[](unsigned int nChannel) const { return GetValue(nChannel); }
	};

	// --------------------- FloatKeyFrameProperty ----------------------------

	///////////////////////////////////////////////////////////////
	//	Class: FloatKeyFrameProperty
	//
	//		Binds to a <PropertyType::FloatKeyFrame> property.
	//		The maximum number of channels can be determined with
	//		<BaseProperty::GetNumberChannels>.
	//
	class FXSTUDIO_API FloatKeyFrameProperty : public BaseProperty
	{
	public :

		FloatKeyFrameProperty() { }

		FloatKeyFrameProperty(const char* szPropertyName, const Component* pComponent, bool bAllowDynamicReload = true)
			: BaseProperty(szPropertyName, pComponent, PropertyType::FloatKeyFrame, bAllowDynamicReload)
		{
		}

		bool Bind(const char* szPropertyName, const Component* pComponent)
		{
			return BindBase(szPropertyName, pComponent, PropertyType::FloatKeyFrame);
		}

		// BaseProperty has the function "GetNumberChannels()" for determining the number of channels.

		float GetValue(unsigned int nChannel, float fDefaultValue = 0.0f) const
		{
			if( !m_pComponent )
				return fDefaultValue;

			return GetValueAtTime(nChannel, m_pComponent->GetUnitTime(), fDefaultValue);
		}

		float GetValueAtLOD(unsigned int nChannel, int nLODLevel, float fDefaultValue = 0.0f) const
		{
			if( !m_pComponent )
				return fDefaultValue;

			return GetValueAtLOD(nChannel, m_pComponent->GetUnitTime(), nLODLevel, fDefaultValue);
		}

		float GetValueAtTime(unsigned int nChannel, float fUnitTime, float fDefaultValue = 0.0f) const;
		float GetValueAtLOD(unsigned int nChannel, float fUnitTime, int nLODLevel, float fDefaultValue = 0.0f) const;

		const FxStudio::FloatKeyFrameData* GetData(unsigned int nChannel) const
		{
			return static_cast<const FloatKeyFrameData*>(GetValuePtr(nChannel, PropertyType::FloatKeyFrame));
		}

		const FxStudio::FloatKeyFrameData* GetDataAtLOD(unsigned int nChannel, int nLODLevel) const
		{
			return static_cast<const FloatKeyFrameData*>(GetValueAtLODPtr(nChannel, nLODLevel, PropertyType::FloatKeyFrame));
		}

		operator float() const { return GetValue(0); }

		float operator[](unsigned int nChannel) const { return GetValue(nChannel); }
	};
}

#endif //FXSTUDIO_PROPERTY_H_