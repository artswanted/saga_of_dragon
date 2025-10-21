/*****************************************************************

    MODULE    : Component.h

    PURPOSE   : A base class for all components registered with FxStudio.

    CREATED   : 5/12/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/
#ifndef FXSTUDIO_COMPONENT_H_
#define FXSTUDIO_COMPONENT_H_

#include "FxStudioDefines.h"

#include "PropertyType.h"
#include "FxInstance.h"

namespace FxStudio
{
	class FxInstance;
	

	//	Class: Component
	//
	//		All Fx components must derive from this class.
	//
	class FXSTUDIO_API Component
	{
	public :

		typedef char (InternalDataType)[16];

	public :
		
		//	Constructor: Component
		//
		//		The internalData will be passed into the <ComponentFactory::CreateComponent>
		//		function to be passed to this constructor.
		//
		//	Parameters:
		//		internalData - The data passed into the component factory.
		//
		Component(const InternalDataType& internalData);

		//	Destructor: Component
		//
		//		Nothing happens in this destructor.  It is recommended that the derived
		//		classes release their resources in their destructor.
		//
		virtual ~Component() {}

		//	Function: GetTypeName
		//
		//		Returns the type name of the component.
		//
		//	Returns:
		//		The type name of the component, as it appears in Designer.
		//
		//	Remarks:
		//		This function does not need to be overriden. It exists to allow
		//		component types to be identified when iterating the components of
		//		an Fx.  This function is not used by the internal Runtime code, and
		//		can be overridden to return anything that may help in the game code.
		//
		virtual const char* GetTypeName() const;

		//	Function: OnPropertyChanged
		//
		//		Called when an Fx bank is reloaded.
		//
		//	Parameters:
		//		szPropertyName - The name of the property which has changed.
		//
		//	Returns:
		//		Return true to indicate that the property has been handled and the Fx
		//		does not need to be restarted.
		//		If the property is only used during creation of the component, this
		//		function should return false so that the Fx can be restarted.
		//
		//	Remarks:
		//		This function will most often be called when previewing an Fx.  All 
		//		properties will have been set to their new value by the time this function
		//		is called.  The old value of the changed property is not available.
		//
		//		If a property is not bound to a <BaseProperty> class and this function
		//		returns false, the Fx will be considered invalid and will be stopped and
		//		restarted by the Preview system.  If nothing is done to handle the change
		//		in property value, it is best to let this function return false.
		//
		//		This function does not need to be overridden.  The primary reason to
		//		override this function is if a property can be dynamically changed but is
		//		usually not applied every frame for performance reasons.
		//
		virtual bool OnPropertyChanged(const char* /* szPropertyName */) { return false; }

		//	Function: Update
		//
		//		Called every time the internal Fx's Update is called while the component
		//		is active.
		//
		//	Parameters:
		//		fDeltaTime - The amount of time to advance the component in seconds.
		//
		//	Remarks:
		//		This function may be called more than once per Fx Update if the Fx changes
		//		phases or loops in the current phase.
		//
		//		fDeltaTime is the delta time passed to the Fx Update, trimmed so that it
		//		will not include time before the component becomes active or exceed the
		//		duration of the component.  Note that the accumulation of fDeltaTime can 
		//		exceed the component's duration due to looping phases.
		//
		//		This function is not called if the Fx is outside the LOD range.  When
		//		the Fx re-enters the range, fDeltaTime will include the time spent
		//		while out of range.  E.g. If a component is out of range for 1 second
		//		and then re-enters the range and gets a 0.33 second delta update, fDeltaTime
		//		will be 1.33 seconds.
		//
		virtual void Update( float /* fDeltaTime */ ) {}
		
		//	Function: Render
		//
		//		Called to render the component.
		//
		//	Parameters:
		//		pRenderData - The data passed into <Manager::Render> or 
		//		<FxInstance::Render>.
		//
		//	Remarks:
		//		This function is called if the component is active, and the Fx is in
		//		playing in auto-render mode and is within its LOD range.
		//
		virtual void Render( void* /*pRenderData*/ ) {}

		//	Function: Activate
		//
		//		Called to put the component in the active state.
		//
		//	Remarks:
		//		Use this function to put the component in an updating and renderable state.
		//		It is recommended that you do not allocate resources from this function
		//		because a looping Fx may call this function many times over its life time.
		//		Instead any necessary allocations should occur when the component is
		//		allocated and the resources should be put into a non-rendering state.
		//		Then this function can simply put the resources into a rendering/active
		//		state.  However, the call semantics are similar to a constructor, so that
		//		resources could be allocated from this function.
		//
		//		This function is called when the Fx's time equals or exceeds the start time
		//		of the component.  <IsActive> is set to true _after_ this
		//		function is called.  <Update> will be called with the amount
		//		time which has elapsed since the start of the component.  This function is
		//		not called while the Fx is out of LOD range, but will be called when the Fx
		//		re-enters its LOD range.
		//		
		//		The "equals or exceeds the start time" criteria insures that Activate will
		//		be called when the Fx enters a looping phase but not when the looping phase
		//		repeats if the component starts at the beginning of the looping phase.
		//
		virtual void Activate() {}

		//	Function: Deactivate
		//
		//		Called to put the component in the inactive state.
		//
		//	Remarks:
		//		Use this function to put the component into a non-updating and non-rendering
		//		state.  It is recommended that resources not be released from this function
		//		because a looping Fx may call this function many times over its lifetime.
		//		Instead, the resources should simply be put into a non-rendering/non-active
		//		state from the function.  However, the call semantics are similar to a
		//		destructor so that resources could be released from this function.
		//
		//		This function is called when the Fx's time exceeds the end time of the
		//		component.  It will be called once for every call to <Activate> and
		//		<Activate> will not be called again before its corresponding <Deactivate>
		//		has been called.  <IsActive> is set to false _after_ this function is
		//		called. This function is not called while the Fx is out of LOD range, but 
		//		it will be called when the Fx re-enters its LOD range or when the Fx is
		//		terminated.
		//
		//		The "exceeds the end time" criteria insures that Deactivate will
		//		be called only after an Fx stops looping over a phase but not when the
		//		looping phase repeats if the component ends at the end of the looping phase.
		//
		virtual void Deactivate() {}

		//	Function: StayAlive
		//
		//		Called after a component is deactivate to allow the component
		//		to keep the Fx alive while it the component is "fading away."
		//
		//	Parameters:
		//		fDeltaTime - The amount of time which has elapsed since the last call
		//					 to StayAlive or since the component time has passed.
		//		bWithinLOD - Set to false if the Fx is outside its LOD range.
		//
		//	Returns:
		//		True if StayAlive needs to be called again.  If it never returns false,
		//		the Fx will not end.
		//
		//	Remarks:
		//		This function can be used to allow a component to persist beyond its
		//		end time.  This functionality is sometimes called "smooth shut-down" or
		//		"fade away."  Be careful with this function, it can cause Fx to persist
		//		for very long times!
		//
		//		If this function is returning true, it will be called while the Fx is 
		//		outside its LOD range so that it can choose to return false and allow the
		//		Fx to expire while outside its LOD range.
		//
		//		A looping Fx may cause a component to be re-activated while in its StayAlive
		//		mode.
		//
		//		If an Fx is directly released, it will be stopped with no regard to the
		//		last return value of this function.
		//
		virtual bool StayAlive(float /* fDeltaTime */, bool /* bWithinLOD */) { return false; }

		//	Function: Reset
		//
		//		Called when an Fx restarts after being in a stopped state.
		//
		//	Remarks:
		//		This function will be called when an <FxInstance::Play> is called
		//		on a stopped Fx.  It should be used to reset any persistant state of the
		//		component without needing to re-allocate any resources.  It is especially
		//		useful when creating a pre-allocated Fx pool.
		//
		//		It will be called on the Fx restarts, whether the component had been
		//		previously activated or not.  An Fx can only be restarted after it has 
		//		been stopped, so this function cannot be called while the component is
		//		active.
		//
		virtual void Reset() {}

		
		//	Function: OnPause
		//
		//		Called when the Fx is paused or played from a paused state.
		//
		//	Parameters:
		//		bPause - If true the Fx is was playing and is being paused.  If
		//				 false, the Fx was paused and is now going to play.
		//
		//	Remarks:
		//		This function will only be called on active components.  Note
		//		that this function is not called if a paused Fx is stopped, only if
		//		the Fx is played from a paused state.  The <FxInstance::IsPaused> will 
		//		always return true when this function is called.
		//
		virtual void OnPause(bool /* bPause */) {}

		/////////////////////////////////////////////////////////////////////
		//	Group: LOD Related Functions
		/////////////////////////////////////////////////////////////////////

		//	Function: OnLODChange
		//
		//		Called when the Fx changes LOD levels.
		//
		//	Parameters:
		//		nOldLOD - The previous LOD level of the Fx.
		//
		//	Remarks:
		//		This is called whenever the Fx changes LOD levels.  The current
		//		LOD level can be determined by calling <GetLOD>.
		//
		virtual void OnLODChange( int /* nOldLOD */ ) {}

		//	Function: OnLeaveLODRange
		//
		//		Called when the Fx leaves its LOD range.
		//
		//	Parameters:
		//		bOutside - True if the LOD distance is more than the maximum LOD range.
		//				   Otherwise, the LOD distance is less than the minimum LOD range.
		//
		//	Remarks:
		//		The component should be put into a non-rendering, non-updated state when
		//		this function is called.
		//
		//		This is only called if the component is active.
		//
		virtual void OnLeaveLODRange( bool /* bOutside */ ) { }

		//	Function: OnEnterLODRange
		//
		//		Called when the Fx re-enters its LOD range.
		//
		//	Parameters:
		//		bFromOutside - True if the Fx's LOD distance had been greater than its
		//					   maximum LOD range.  Otherwise, the Fx's LOD distance was
		//					   less than its minimum LOD range.
		//
		//	Remarks:
		//		This function will be called only if <OnLeaveLODRange> had been called on
		//		it when the Fx had gone outside its LOD range.
		//
		virtual void OnEnterLODRange( bool /* bFromOutside */ ) { }


		/////////////////////////////////////////////////////////////////////
		//	Group: Property Related Functions
		/////////////////////////////////////////////////////////////////////

		//	Function: GetInteger
		//
		//		Returns the current value of a <PropertyType::Integer> property.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		nDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		int GetInteger(const char* szName, int nDefaultValue = 0) const;

		//	Function: GetIntegerAtLOD
		//
		//		Returns the current value of a <PropertyType::Integer> property at a
		//		specific LOD.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		nDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		int GetIntegerAtLOD(const char* szName, int nLODLevel, int nDefaultValue = 0) const;

		//	Function: GetIntegerRange
		//
		//		Returns the current value of a <PropertyType::IntegerRange> property.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		vDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		const FxStudio::IntegerRange& GetIntegerRange(const char* szName, const FxStudio::IntegerRange& vDefaultValue = FxStudio::IntegerRange()) const;

		//	Function: GetIntegerRangeAtLOD
		//
		//		Returns the current value of a <PropertyType::IntegerRange> property at a
		//		specific LOD.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		vDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		const FxStudio::IntegerRange& GetIntegerRangeAtLOD(const char* szName, int nLODLevel, const FxStudio::IntegerRange& vDefaultValue = FxStudio::IntegerRange()) const;

		//	Function: GetFloat
		//
		//		Returns the current value of a <PropertyType::Float> property.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		fDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		float GetFloat(const char* szName, float fDefaultValue = 0.0f) const;

		//	Function: GetFloatAtLOD
		//
		//		Returns the current value of a <PropertyType::Float> property at a
		//		specific LOD.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		fDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		float GetFloatAtLOD(const char* szName, int nLODLevel, float fDefaultValue = 0.0f) const;

		//	Function: GetFloat
		//
		//		Returns the current value of a <PropertyType::Float> property.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		fDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		const FxStudio::FloatRange& GetFloatRange(const char* szName, const FxStudio::FloatRange& vDefaultValue = FloatRange() ) const;

		//	Function: GetFloatRangeAtLOD
		//
		//		Returns the current value of a <PropertyType::FloatRange> property at a
		//		specific LOD.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		vDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		const FxStudio::FloatRange& GetFloatRangeAtLOD(const char* szName, int nLODLevel, const FxStudio::FloatRange& vDefaultValue = FloatRange() ) const;

		//	Function: GetString
		//
		//		Returns the current value of a <PropertyType::String> property.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		szDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		const char* GetString(const char* szName, const char* szDefaultValue = "") const;

		//	Function: GetStringAtLOD
		//
		//		Returns the current value of a <PropertyType::String> property at a
		//		specific LOD.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		szDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		const char* GetStringAtLOD(const char* szName, int nLODLevel, const char* szDefaultValue = "") const;

		//	Function: GetVector3
		//
		//		Returns the current value of a <PropertyType::Vector3> property.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		vDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		const FxStudio::Vector3& GetVector3(const char* szName, const FxStudio::Vector3& vDefaultValue = FxStudio::Vector3()) const;

		//	Function: GetVector3AtLOD
		//
		//		Returns the current value of a <PropertyType::Vector3> property at a
		//		specific LOD.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		vDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		const FxStudio::Vector3& GetVector3AtLOD(const char* szName, int nLODLevel, const FxStudio::Vector3& vDefaultValue = FxStudio::Vector3()) const;

		//	Function: GetVector4
		//
		//		Returns the current value of a <PropertyType::Vector4> property.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		vDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		const FxStudio::Vector4& GetVector4(const char* szName, const FxStudio::Vector4& vDefaultValue = FxStudio::Vector4()) const;

		//	Function: GetVector4AtLOD
		//
		//		Returns the current value of a <PropertyType::Vector4> property at a
		//		specific LOD.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		vDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		const FxStudio::Vector4& GetVector4AtLOD(const char* szName, int nLODLevel, const FxStudio::Vector4& vDefaultValue = FxStudio::Vector4()) const;

		//	Function: GetFixedFunction
		//
		//		Returns the current value of a <PropertyType::FixedFunction> property.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		fDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		float GetFixedFunction(const char* szName, float fDefaultValue = 0.0f) const;

		//	Function: GetFixedFunctionAtLOD
		//
		//		Returns the current value of a <PropertyType::FixedFunction> property at a
		//		specific LOD.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		fDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		float GetFixedFunctionAtLOD(const char* szName, int nLODLevel, float fDefaultValue = 0.0f) const;

		//	Function: GetFixedFunction
		//
		//		Returns the value of a <PropertyType::FixedFunction> property
		//		at a specific time.
		//
		//	Parameters:
		//		fUnitTime - The time re-scaled to be a percentage of the component's
		//					 duration.
		//		szName - The fully qualified name of the property.
		//		fDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		The unit time is a value which goes from 0 to 1, with 0 being the
		//		component's start-time and 1 being the component's end-time.
		//		<GetUnitTime> can be used to convert from Fx time to unit time.
		//
		float GetFixedFunction(float fUnitTime, const char* szName, float fDefaultValue = 0.0f) const;

		//	Function: GetFixedFunctionAtLOD
		//
		//		Returns the value of a <PropertyType::FixedFunction> property
		//		at a specific time and LOD level.
		//
		//	Parameters:
		//		fUnitTime - The time re-scaled to be a percentage of the component's
		//					 duration.
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		fDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		The unit time is a value which goes from 0 to 1, with 0 being the
		//		component's start-time and 1 being the component's end-time.
		//		<GetUnitTime> can be used to convert from Fx time to unit time.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		float GetFixedFunctionAtLOD(float fUnitTime, const char* szName, int nLODLevel, float fDefaultValue = 0.0f) const;

		//	Function: GetFixedFunctionData
		//
		//		Returns the data of a <PropertyType::FixedFunction> property.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//
		//	Returns:
		//		The property data, as a pointer to a <FixedFunctionData> structure.
		//		Returns NULL if it was not found.
		//
		//	Remarks:
		//		Will look-up the property by name and return the data used to determine
		//		its time-varying value.  If the property is not found 
		//		<ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		const FxStudio::FixedFunctionData* GetFixedFunctionData(const char* szName) const;

		//	Function: GetFixedFunctionDataAtLOD
		//
		//		Returns the data of a <PropertyType::FixedFunction> property.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//
		//	Returns:
		//		The property data, as a pointer to a <FixedFunctionData> structure.
		//		Returns NULL if it was not found.
		//
		//	Remarks:
		//		Will look-up the property by name and return the data used to determine
		//		its time-varying value.  If the property is not found 
		//		<ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		const FxStudio::FixedFunctionData* GetFixedFunctionDataAtLOD(const char* szName, int nLODLevel) const;

		//	Function: GetKeyFramedColorARGB
		//
		//		Returns the current value of a <PropertyType::ColorARGBKeyFrame> property.
		//
		//	Parameters:
		//		nChannel - The channel being requested, set to zero if it is single
		//				   channeled.
		//		szName - The fully qualified name of the property.
		//		nDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		int GetKeyFramedColorARGB(unsigned int nChannel, const char* szName, int nDefaultValue = 0) const;

		//	Function: GetKeyFramedColorARGBAtLOD
		//
		//		Returns the current value of a <PropertyType::ColorARGBKeyFrame> property
		//		at a specific LOD.
		//
		//	Parameters:
		//		nChannel - The channel being requested, set to zero if it is single
		//				   channeled.
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		nDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		int GetKeyFramedColorARGBAtLOD(unsigned int nChannel, const char* szName, int nLODLevel, int nDefaultValue = 0) const;

		//	Function: GetKeyFramedColorARGB
		//
		//		Returns the value of a <PropertyType::ColorARGBKeyFrame> property
		//		at a specific time.
		//
		//	Parameters:
		//		nChannel - The channel being requested, set to zero if it is single
		//				   channeled.
		//		fUnitTime - The time re-scaled to be a percentage of the component's
		//					 duration.
		//		szName - The fully qualified name of the property.
		//		nDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		The unit time is a value which goes from 0 to 1, with 0 being the
		//		component's start-time and 1 being the component's end-time.
		//		<GetUnitTime> can be used to convert from Fx time to unit time.
		//
		int GetKeyFramedColorARGB(unsigned int nChannel, float fUnitTime, const char* szName, int nDefaultValue = 0) const;

		//	Function: GetKeyFramedColorARGBAtLOD
		//
		//		Returns the value of a <PropertyType::ColorARGBKeyFrame> property
		//		at a specific time and LOD level.
		//
		//	Parameters:
		//		nChannel - The channel being requested, set to zero if it is single
		//				   channeled.
		//		fUnitTime - The time re-scaled to be a percentage of the component's
		//					 duration.
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		nDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		The unit time is a value which goes from 0 to 1, with 0 being the
		//		component's start-time and 1 being the component's end-time.
		//		<GetUnitTime> can be used to convert from Fx time to unit time.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		int GetKeyFramedColorARGBAtLOD(unsigned int nChannel, float fUnitTime, const char* szName, int nLODLevel, int nDefaultValue = 0) const;

		//	Function: GetColorARGBKeyFrameData
		//
		//		Returns the data of a <PropertyType::ColorARGBKeyFrame> property.
		//
		//	Parameters:
		//		nChannel - The channel being requested, set to zero if it is single
		//				   channeled.
		//		szName - The fully qualified name of the property.
		//
		//	Returns:
		//		The property data, as a pointer to a <ColorARGBKeyFrameData> structure.
		//		Returns NULL if it was not found.
		//
		//	Remarks:
		//		Will look-up the property by name and return the data used to determine
		//		its time-varying value.  If the property is not found 
		//		<ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//
		const FxStudio::ColorARGBKeyFrameData* GetColorARGBKeyFrameData(unsigned int nChannel, const char* szName) const;

		//	Function: GetColorARGBKeyFrameDataAtLOD
		//
		//		Returns the data of a <PropertyType::ColorARGBKeyFrame> property.
		//
		//	Parameters:
		//		nChannel - The channel being requested, set to zero if it is single
		//				   channeled.
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//
		//	Returns:
		//		The property data, as a pointer to a <ColorARGBKeyFrameData> structure.
		//		Returns NULL if it was not found.
		//
		//	Remarks:
		//		Will look-up the property by name and return the data used to determine
		//		its time-varying value.  If the property is not found 
		//		<ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		const FxStudio::ColorARGBKeyFrameData* GetColorARGBKeyFrameDataAtLOD(unsigned int nChannel, const char* szName, int nLODLevel) const;

		//	Function: GetKeyFramedFloat
		//
		//		Returns the current value of a <PropertyType::FloatKeyFrame> property.
		//
		//	Parameters:
		//		nChannel - The channel being requested, set to zero if it is single
		//				   channeled.
		//		szName - The fully qualified name of the property.
		//		fDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		float GetKeyFramedFloat(unsigned int nChannel, const char* szName, float fDefaultValue = 0.0f) const;

		//	Function: GetKeyFramedFloatAtLOD
		//
		//		Returns the current value of a <PropertyType::FloatKeyFrame> property
		//		at a specific LOD.
		//
		//	Parameters:
		//		nChannel - The channel being requested, set to zero if it is single
		//				   channeled.
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		fDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		float GetKeyFramedFloatAtLOD(unsigned int nChannel, const char* szName, int nLODLevel, float fDefaultValue = 0.0f) const;

		//	Function: GetKeyFramedFloat
		//
		//		Returns the value of a <PropertyType::FloatKeyFrame> property
		//		at a specific time.
		//
		//	Parameters:
		//		nChannel - The channel being requested, set to zero if it is single
		//				   channeled.
		//		fUnitTime - The time re-scaled to be a percentage of the component's
		//					 duration.
		//		szName - The fully qualified name of the property.
		//		fDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		The unit time is a value which goes from 0 to 1, with 0 being the
		//		component's start-time and 1 being the component's end-time.
		//		<GetUnitTime> can be used to convert from Fx time to unit time.
		//
		float GetKeyFramedFloat(unsigned int nChannel, float fUnitTime, const char* szName, float fDefaultValue = 0.0f) const;

		//	Function: GetKeyFramedFloatAtLOD
		//
		//		Returns the value of a <PropertyType::FloatKeyFrame> property
		//		at a specific time and LOD level.
		//
		//	Parameters:
		//		nChannel - The channel being requested, set to zero if it is single
		//				   channeled.
		//		fUnitTime - The time re-scaled to be a percentage of the component's
		//					 duration.
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		fDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		Will look-up the property by name and return its value.  If the property
		//		is not found <ErrorHandler::HandleError> will be called.
		//
		//		The unit time is a value which goes from 0 to 1, with 0 being the
		//		component's start-time and 1 being the component's end-time.
		//		<GetUnitTime> can be used to convert from Fx time to unit time.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		float GetKeyFramedFloatAtLOD(unsigned int nChannel, float fUnitTime, const char* szName, int nLODLevel, float fDefaultValue = 0.0f) const;

		// Will return null if the property does not exist.
		//	Function: GetFloatKeyFrameData
		//
		//		Returns the data of a <PropertyType::FloatKeyFrame> property.
		//
		//	Parameters:
		//		nChannel - The channel being requested, set to zero if it is single
		//				   channeled.
		//		szName - The fully qualified name of the property.
		//
		//	Returns:
		//		The property data, as a pointer to a <FloatKeyFrameData> structure.
		//		Returns NULL if it was not found.
		//
		//	Remarks:
		//		Will look-up the property by name and return the data used to determine
		//		its time-varying value.  If the property is not found 
		//		<ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//
		const FxStudio::FloatKeyFrameData* GetFloatKeyFrameData(unsigned int nChannel, const char* szName) const;

		//	Function: GetFloatKeyFrameDataAtLOD
		//
		//		Returns the data of a <PropertyType::FloatKeyFrame> property.
		//
		//	Parameters:
		//		nChannel - The channel being requested, set to zero if it is single
		//				   channeled.
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//
		//	Returns:
		//		The property data, as a pointer to a <FloatKeyFrameData> structure.
		//		Returns NULL if it was not found.
		//
		//	Remarks:
		//		Will look-up the property by name and return the data used to determine
		//		its time-varying value.  If the property is not found 
		//		<ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		const FxStudio::FloatKeyFrameData* GetFloatKeyFrameDataAtLOD(unsigned int nChannel, const char* szName, int nLODLevel) const;

		//	Function: GetBoolean
		//
		//		Returns the current value of a <PropertyType::Integer> property
		//		as a boolean.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		bDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		This is equivalent to "<GetInteger> != 0".
		//		If the property is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		bool  GetBoolean(const char* szName, bool bDefaultValue = false) const;

		//	Function: GetBooleanAtLOD
		//
		//		Returns the current value of a <PropertyType::Integer> property
		//		at a specific LOD as a boolean.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//		nLODLevel - The LOD level being requested.
		//		bDefaultValue - The value to be used if the property was not found.
		//
		//	Returns:
		//		The value of the property.
		//
		//	Remarks:
		//		This is equivalent to "<GetIntegerAtLOD> != 0".
		//		If the property is not found <ErrorHandler::HandleError> will be called.
		//
		//		The property name is fully qualified, it includes the owning property 
		//		groups with back-slash separators.
		//
		//		If nLODLevel is too large, the largest LOD level will be used.
		//
		bool  GetBooleanAtLOD(const char* szName, bool bDefaultValue = false) const;

		//	Function: IsLODProperty
		//
		//		Determines if a property has LOD specific values.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//
		//	Returns:
		//		True if the property has LOD specific values.
		//
		//	Remarks:
		//		This function can be used to determine if the value might change when
		//		the LOD level changes.
		//
		//		A property may not have LOD data because it was not set-up to allow LOD
		//		in the component definition file, or because the Fx owning it does not
		//		have an LOD category.
		//
		bool IsLODProperty(const char* szName) const;

		//	Function: GetNumberChannels
		//
		//		Determines the number of channels defined for a key-framed property.
		//
		//	Parameters:
		//		szName - The fully qualified name of the property.
		//
		//	Returns:
		//		The number of channels for the property.
		//		Returns 1 for a non-key-framed property.
		//		Returns 0 if the property does not exist.
		//
		//	Remarks:
		//		This function can be used to determine how many channels a property was
		//		defined to have in Designer. If the property is not found 
		//		<ErrorHandler::HandleError> will be called.
		//
		unsigned int GetNumberChannels(const char* szName) const;

		/////////////////////////////////////////////////////////////////////
		//	Group: Data Access Functions
		/////////////////////////////////////////////////////////////////////

		//	Function: GetFxInstance
		//
		//		Gets the FxInstance for the owning Fx.
		//
		//	Returns:
		//		The FxInstance for the Fx which owns this component.
		//
		const FxInstance& GetFxInstance() const;
		FxInstance		  GetFxInstance();

		//	Function: GetTime
		//
		//		Gets the current Fx time.
		//
		//	Parameters:
		//		bClamp - If true, the value returned will be less than or equal to the Fx duration.
		//               If false, the value can be greater than the Fx duration if a component is 
		//               in StayAlive mode, see <StayAlive>.
		//
		//	Returns:
		//		The cursor time for the Fx.
		//
		//	Remarks:
		//		This is equivalent to <FxInstance::GetTime>.  It has less overhead
		//		than calling GetFxInstance().GetTime().
		//
		float	GetTime(bool bClamp = true) const;
		
		//	Function: GetLOD
		//
		//		Gets the current Fx LOD level.
		//
		//	Returns:
		//		The current LOD level.
		//
		//	Remarks:
		//		This is equivalent to <FxInstance::GetLOD>.  It has less overhead
		//		than calling GetFxInstance().GetLOD().
		//
		int		GetLOD() const;

		//	Function: GetLODFloat
		//
		//		Gets the current interpolated Fx LOD level.
		//
		//	Returns:
		//		The current interpolated LOD level.
		//
		//	Remarks:
		//		This is equivalent to <FxInstance::GetLODFloat>.  It has less overhead
		//		than calling GetFxInstance().GetLODFloat().
		//
		float   GetLODFloat() const;

		//	Function: IsAdvancingPhase
		//
		//		Determines if the Fx will be progressing to the next phase.
		//
		//	Returns:
		//		True if the current phase will not loop.
		//
		//	Remarks:
		//		This is equivalent to <FxInstance::IsAdvancingPhase>.  It has less
		//		overhead than calling GetFxInstance().IsAdvancingPhase().
		//
		bool	IsAdvancingPhase() const;

		//	Function: IsActive
		//
		//		Returns true if the component is active.
		//
		//	Returns:
		//		True if the component is active.
		//
		//	Remarks:
		//		This function must look up the <ComponentData> in the owning Fx.  If it
		//		must be called often, it would be better to create a bool which is set
		//		in <Activate> and <Deactivate>.
		//
		bool IsActive() const;

		//	Function: GetStartTime
		//
		//		Determines the start-time of the component in terms of the Fx cursor time.
		//
		//	Returns:
		//		The component start-time.
		//
		float GetStartTime() const;

		//	Function: GetEndTime
		//
		//		Determines the end-time of the component in terms of the Fx cursor time.
		//
		//	Returns:
		//		The component end-time.
		//
		float GetEndTime() const;
		
		//	Function: GetTrackGroupIndex
		//
		//		Determine the index of the track-group which contains this component.
		//
		//	Returns:
		//		The track-group index.
		//
		//	Remarks:
		//		This function can be used to test if another component is in the
		//		same track-group as this component.  The track-groups are the groupings
		//		of time-tracks in Designer.
		//
		unsigned int GetTrackGroupIndex() const;

		//	Function: GetUnitTime
		//
		//		Determines the current time scaled relative to the component's start and
		//		end times.
		//
		//	Returns:
		//		The scaled time, will be from 0 to 1.
		//
		//	Remarks:
		//		The return value is clamped to be between 0 and 1, inclusive.  It will
		//		be 0 before the component starts and 1 after the component ends.
		//
		float GetUnitTime() const;

		//	Function: GetUnitTime
		//
		//		Determines the current time scaled relative to the component's start and
		//		end times.
		//
		//	Parameters:
		//		fTime - The current Fx cursor time.
		//
		//	Returns:
		//		The scaled time, will be from 0 to 1.
		//
		//	Remarks:
		//		The return value is clamped to be between 0 and 1, inclusive.  It will
		//		be 0 for times less than the component start time and 1 for times greater
		//		than the component end time.
		//
		float GetUnitTime(float fTime) const;

	private :

		friend class BaseProperty;

		InternalDataType	m_InternalData;
	};

	//	Class: ComponentFactory
	//
	//		Each component should have a unique factory class which derives
	//		from this class.  A single instance of that factory class should
	//		then exist in the executable.
	//
	class FXSTUDIO_API ComponentFactory
	{
	public :

		//	Constructor: ComponentFactory
		//
		//		Will create and register the factory within the Runtime library.
		//
		//	Parameters:
		//		bRegisterSelf - Set to false if the factory should not register
		//						itself when created.
		//
		//	Remarks:
		//		The simplest way to set-up a factory is to inherit from this
		//		class and create a static instance of that class in the game code.
		//		Watch out for dead symbol stripping in the linker, it is best
		//		to have single file which holds all the instances and is compiled
		//		into the main executable or dll of the game's client application.
		//
		explicit ComponentFactory(bool bRegisterSelf = true);

		//	Destructor: ComponentFactory
		//
		//		The destructor will call <DeregisterSelf> so that the class
		//		automatically frees itself from the Runtime library.
		//
		virtual ~ComponentFactory();

		//	Function: GetComponentName
		//
		//		Used to determine which component type is created by this class.
		//
		//	Returns:
		//		The component type name, as it appears in Designer.
		//
		//	Remarks:
		//		The name comparison is caseless, but otherwise the name should be
		//		exactly as it was entered in Designer.
		//
		virtual const char* GetComponentName() const = 0;
		
		//	Function: CreateComponent
		//
		//		Used to allocate and initialize a new component.
		//
		//	Parameters:
		//		internalData - This must be passed to the <Component> constructor.
		//		pUserData - This is the data passed into <Manager::CreateFx>.
		//
		//	Returns:
		//		A pointer to the newly created component.
		//		NULL if the component could not be created.
		//
		//	Remarks:
		//		A newly created Fx will use this function to create its components.
		//		If this function returns NULL, the Fx will still be created but will not
		//		have this component available for use.
		//
		virtual FxStudio::Component* CreateComponent(const FxStudio::Component::InternalDataType& internalData, void* pUserData) = 0;

		//	Function: DestroyComponent
		//
		//		Used to free a component which was created with the corresponding
		//		<CreateComponent>.
		//
		//	Parameters:
		//		pComponent - The component to be released.  This will never be NULL.
		//
		//	Remarks:
		//		This function will always be called to release a component pointer created
		//		in <CreateComponent>.
		//
		virtual void DestroyComponent(FxStudio::Component* pComponent) = 0;
		

		//	Function: RegisterSelf
		//
		//		Registers the factory with the Runtime system.
		//
		//	Remarks:
		//		This function must be called for the factory to be used.  The constructor
		//		will call this automatically unless prevented.  Nothing happens if 
		//		this function is called after the factory is registered.
		//
		void RegisterSelf();

		//	Function: DeregisterSelf
		//
		//		Removes the factory from the Runtime system.
		//
		//	Remarks:
		//		This function will remove a registered factory from the Runtime system.
		//		The destructor calls this function.
		//		Nothing happens if the factory is not registered.
		//
		void DeregisterSelf();

		//	Function: IsRegistered
		//
		//		Determines if the factory is registered with the Runtime system.
		//
		//	Returns:
		//		True if the factory is registered.
		//
		bool IsRegistered() const;

		//	Function: GetFactory
		//
		//		Determines the appropriate factory for the given component type name.
		//
		//	Parameters:
		//		szComponentName - The type name of the component, as it appears in Designer.
		//
		//	Returns:
		//		The component factory corresponding to the type name.
		//		Returns NULL if the factory was not found.
		//
		//	Remarks:
		//		This is the function used by the Runtime system to find the appropriate
		//		component factory.
		//
		static ComponentFactory* GetFactory(const char* szComponentName);

	private :

		// This class cannot be copied.  External pointers to this class exist, so this
		// functionality cannot be supported.
		ComponentFactory(const ComponentFactory&);
		ComponentFactory& operator=(const ComponentFactory&);

		ComponentFactory* m_pPreviousFactory;
		ComponentFactory* m_pNextFactory;

	};


	inline float Component::GetUnitTime(float fTime) const
	{
		const float fDuration = GetEndTime() - GetStartTime();
		
		if( fDuration > 0.0f )
		{
			const float fUnitTime = (fTime - GetStartTime())/fDuration;
			if( fUnitTime < 0.0f )
				return 0.0f;
			if( fUnitTime > 1.0f )
				return 1.0f;

			return fUnitTime;
		}

		return 0.0f;
	}

	inline float Component::GetUnitTime() const
	{
		return GetUnitTime(GetTime(true));
	}

	inline bool  Component::GetBoolean(const char* szName, bool bDefaultValue /* = false */) const
	{
		return GetInteger(szName, bDefaultValue) != 0;
	}
	
	inline bool  Component::GetBooleanAtLOD(const char* szName, bool bDefaultValue /* = false */) const
	{
		return GetIntegerAtLOD(szName, bDefaultValue) != 0;
	}

}

#endif //FXSTUDIO_COMPONENT_H_