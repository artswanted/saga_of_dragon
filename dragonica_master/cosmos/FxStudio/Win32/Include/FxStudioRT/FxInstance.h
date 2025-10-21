/*****************************************************************

    MODULE    : FxInstance.h

    PURPOSE   : A handle class to be used to work with spawned Fx.

    CREATED   : 5/6/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/
#ifndef FXSTUDIO_FXINSTANCE_H_
#define FXSTUDIO_FXINSTANCE_H_

#include "FxStudioDefines.h"

#include "Listener.h"

//
// Title: FxInstance
//

namespace FxStudio
{
	namespace Internal
	{
		class Fx;
	}

	struct Vector3;
	class Component;
	class Manager;

	//	Struct: ComponentData
	//
	//		This is the data returned when iterating through an Fx's component data.
	//
	struct FXSTUDIO_API ComponentData
	{
		//	Variable: m_pComponent
		//		The pointer to the component.  This can be NULL if the compnent failed to create.
		//		Components will most often fail to create because their factory is not registered.
		Component*	m_pComponent;

		//	Variable: m_bActive
		//		Is true if the component is activated.
		bool		m_bActive;

		//	Variable: m_bStayAlive
		//		Is true if the component is returning true from calls to <Component::KeepAlive>.
		bool		m_bStayAlive;

		ComponentData()
			: m_pComponent(0)
			, m_bActive(false)
			, m_bStayAlive(false) {}
	};

	//	Class: FxListener
	//
	//		Used to receive callbacks relevant to the internal Fx.
	//
	class FXSTUDIO_API FxListener : public BaseListener
	{
	public :

		virtual ~FxListener() {}

		//	Function: OnFxDestroyed
		//
		//		Called just before the Fx is destroyed.
		//
		//	Remarks:
		//		If any clean-up should occur when an Fx is destroyed, this call-back
		//		provides a may means to be notified of the event.
		//
		virtual void OnFxDestroyed()  {}
	};

	//	Class: FxInstance
	//
	//		This class is used to control created Fx.  It is actually a handle to the internal Fx 
	//		instance.  It can be freely copied and deleted.  If the internal Fx is destroyed, the 
	//		functions in this class will become null operations.
	//
	class FXSTUDIO_API FxInstance
	{
	public :

		//	Variable: Empty
		//
		//		A static instance of an empty class so that we do not have to 
		//		allocate on the stack.
		//
		static const FxInstance Empty;

		// Constructor: FxInstance
		//		
		//		Creates an empty FxInstance.
		//
		FxInstance();

		//	Destructor: ~FxInstance
		//
		//		Destroys the FxInstance with no impact on the internal Fx.
		//
		//	Remarks:
		//		If this is the last FxInstance referring to the internal Fx, and the 
		//		internal Fx is either not auto-released or auto-updated, the internal 
		//		Fx will be destroyed in the next Manager update.  The automatic destruction 
		//		needs to happen to prevent Fx from lingering.
		//
		~FxInstance();

		// about: Copying 
		//
		//		Copying is fully supported.  A copy of an FxInstance will be refering to the
		//		same internal Fx as the original.
		//
		FxInstance(const FxInstance& rhs);
		FxInstance& operator=(const FxInstance& rhs);

		//	Function: ReleaseFx
		//
		//		Releases the internal Fx.
		//
		//	Remarks:
		//		This will stop and destroy the internal Fx handled by this FxInstance.  
		//		It is equivalent to calling Manager’s RelaseFx function.
		//
		void ReleaseFx();

		//	Function: Clear
		//
		//		Makes the FxInstance equivalent to a newly created empty FxInstance.
		//
		//	Remarks:
		//		This is equivalent to just assigning an empty FxInstance to this instance.
		//		It does not release the internal Fx unless that Fx is not auto-updated or
		//		not auto-released.
		//
		void Clear();

		//	Function: IsValid
		//
		//		Determines if the FxInstance represents an internal Fx or is an empty FxInstance.
		//
		//	Returns:
		//		True if the FxInstance refers to an internal Fx.
		//
		bool IsValid() const { return m_bIsValid; }
		
		//	Function: GetData
		//
		//		Returns a pointer to the internal Fx being referenced.
		//
		//	Returns:
		//		The pointer to the internal Fx.
		//
		//	Remarks:
		//		This method can be used to sort FxInstance by a unique value or to check if two 
		//		FxInstances are equal.  Note that equality and less-than operators are provided 
		//		as well.  This function should only be used if those two operators are not 
		//		sufficient.  This value only changes if a new FxInstance is assigned to this
		//		class.  This value does not change when the Fx expires, use <IsValid> to
		//		determine if the internal Fx is still valid.
		//		
		const void* GetData() const { return m_pData; }
		
		//	Function: Update
		//
		//		Directly updates the internal Fx.
		//
		//	Parameters:
		//		fDeltaTime - Time in seconds to advance the Fx.
		//
		//	Remarks:
		//		This function can be used to update an Fx which is not being auto-updated or to 
		//		advance a newly create Fx by an arbitrary time.  A paused Fx will ignore calls to 
		//		this function.  This function will not release the Fx when finished, instead the 
		//		Manager’s next update will release the Fx if it is in auto-release mode.
		//
		//		Be careful that Update is not called multiple times through multiple FxInstances.
		//
		//		It is recommended that you use the Manager's update function.
		//
		void Update(float fDeltaTime);

		//	Function: Render
		//
		//		Can be used to explicitly render an Fx.
		//
		//	Parameters:
		//		pRenderData - The data to be passed into the <Component::Render> function.
		//
		//	Remarks:
		//		This function should only be needed if the Fx is not in auto-render mode.
		//		This is the same function called by the <Manager::Render>.
		//
		void Render(void* pRenderData);

		//	Function: Play
		//
		//		Starts a paused Fx.  Restarts a stopped Fx.
		//
		//	Remarks:
		//		If the Fx has stopped, this will call Reset on each component of the Fx and 
		//		restart  the Fx from the beginning.  If the Fx is paused, this will cause the Fx
		//		to continue playing.  Does nothing if the Fx is already playing.
		//
		void Play();

		//	Function: IsPlaying
		//
		//		Determines if the Fx is currently in a playing state.
		//
		//	Returns:
		//		True if the effect is playing.
		//
		//	Remarks:
		//		A playing Fx will be updated if it is auto-updated and rendered if it is 
		//		auto-rendered.
		//
		bool IsPlaying() const { return !IsPaused() && !IsStopped(); }

		//	Function: Pause
		//
		//		Pauses a playing Fx.
		//
		//	Remarks:
		//		If an Fx is playing, it will be paused.  It will no longer receive updates if it 
		//		is auto-updated.  Does nothing if the Fx is already paused or is stopped.
		//
		void Pause();

		//	Function: IsPaused
		//
		//		Determines if the Fx is currently in a paused state.
		//
		//	Returns:
		//		True if the FxInstance is valid and the internal Fx is paused. 
		//
		//	Remarks:
		//		A paused Fx will be not be updated if it is auto-updated.  Direct calls to FxInstance's 
		//		<Update> will also be ignored.
		//
		bool IsPaused() const;

		//	Function: Stop
		//
		//		Stops a paused or playing Fx.
		//
		//	Parameters:
		//		bForce - If set to true, <Component::StayAlive> will not be called.
		//
		//	Remarks:
		//		The Fx components will all be deactivated and the Fx time will be set to its 
		//		duration.  If the Fx is auto-released and none of the components are in keep
		//		alive mode, it will be released on the next Manager update.  
		//		If bForce is true, <Component::StayAlive> will not be called on the components.
		//
		void Stop(bool bForce = false);

		//	Function: IsStopped
		//
		//		Determines if the Fx has completed.
		//
		//	Returns:
		//		True if the effect is stopped or FxInstance is invalid.
		//
		//	Remarks:
		//		A stopped Fx has either ran to completion or had Stop called on it.
		//
		bool IsStopped() const;

		//	Function: Listen
		//
		//		Register a listener to the Fx.
		//
		//	Parameters:
		//		pListener - Pointer to the <FxListener> derived class
		//					being registered with the Fx.
		//
		//	Remarks:
		//		Use this function to attach an <FxListener> derived class to 
		//		the FxInstance.  The listener will be attached to the Fx, not the FxInstance.  
		//		The FxInstance must refer to a valid Fx before calling this function.
		//		
		void Listen(FxListener* pListener);

		//	Function: SetAutoRelease
		//
		//		Dictates that Fx should be automatically released by the Manager.
		//
		//	Parameters:
		//		bManaged - If false the effect will not be automatically released by the Manager.
		//
		//	Remarks:
		//		Newly created Fx will be set to auto-release.  Setting the Fx to be not 
		//		auto-released will prevent the Manager from releasing the Fx once it has 
		//		stopped. This is useful if the Fx will be used repeatedly, such as in a 
		//		pre-allocated bank of Fx.  Note that once the Fx will still be released if 
		//		the bank that contains its data is unloaded.  Also, the Fx will be automatically
		//		released if it has no FxInstance referring to it when it stops.
		//
		void SetAutoRelease(bool bManaged);

		//	Function: IsAutoRelease
		//
		//		Determines if the Fx is in auto-release mode.
		//
		//	Returns:
		//		True if the effect instance will be auto released by the Manager when it stops.
		//
		bool IsAutoRelease() const;

		//	Function: SetAutoUpdate
		//
		//		Dictates that the Fx should be automatically updated by the Manager.
		//
		//	Parameters:
		//		bAutoUpdate - If true the internal Fx will receive update messages 
		//					  from the Manager.
		//
		//	Remarks:
		//		A newly created Fx will be set to auto-update.  A non-auto-updated Fx will still
		//		be released by the Manager if it is runs to completion or is stopped.  Use 
		//		<SetAutoRelease> to prevent automatic clean-up.  An Fx which is not 
		//		auto-updated will be automatically released if it has no FxInstance referring to 
		//		it when Manager is updated.
		//
		void SetAutoUpdate(bool bAutoUpdate);

		//	Function: IsAutoUpdate
		//
		//		Determines if the Fx is in auto-update mode.
		//
		//	Returns:
		//		True if the Fx is set to automatically update when Manager is updated.
		//
		bool IsAutoUpdate() const;

		//	Function: SetAutoRender
		//
		//		Dictates that the Manager should call attempt to render this Fx.
		//
		//	Parameters:
		//		bAutoRender - If true, Manager will attempt to render the internal Fx.
		//
		//	Remarks:
		//		A newly created Fx will be set to auto-render.  When in auto-render mode
		//		the Manager will call Component::Render on each active component in the Fx.
		//
		void SetAutoRender(bool bAutoRender);

		//	Function: IsAutoRender
		//
		//		Determines if the Fx is in auto-render mode.
		//
		//	Returns:
		//		True  if the Fx is in auto-render mode.
		//
		bool IsAutoRender() const;

		//	Function: GetManager
		//
		//		Gets the Manager which created this Fx.
		//
		//	Returns:
		//		The pointer to the owning Manager.  NULL if the FxInstance is invalid.
		//
		Manager*		GetManager();
		const Manager*	GetManager() const;

		//	Function: GetTime
		//
		//		Gets the current cursor time of the Fx.
		//
		//	Parameters:
		//		bClamp - If true, the value returned will be less than or equal to the Fx duration.
		//               If false, the value can be greater than the Fx duration if a component is 
		//               in StayAlive mode, see <Component::StayAlive>.
		//
		//	Returns:
		//		The current cursor time of the Fx.
		//
		//	Remarks:
		//		This is the time as displayed along the time line in FxStudio.  If a phase 
		//		is looping, this time will jump back to the beginning of the loop.
		//
		float GetTime(bool bClamp = true) const;

		//	Function: GetDuration
		//
		//		Gets the duration of the Fx in terms of its cursor time.
		//
		//	Returns:
		//		The duration in terms of the cursor time.
		//
		//	Remarks:
		//		This is the maximal value of the <GetTime> function.  If the Fx has looping
		//		phases, the time spent loop is not taken into account.
		//
		float GetDuration() const;

		//	Function: GetRunTime
		//
		//		Gets the accumulated time the Fx has been running.
		//
		//	Returns:
		//		The accumulated time.
		//
		//	Remarks:
		//		This is the accumulation of all fDeltaTime's passed to the Fx through its <Update>
		//		while it was not paused.  If restarted after the Fx has Stop, this value will be
		//		reset to zero.
		//
		float GetRunTime() const;

		//	Function: GetName
		//
		//		Gets the name of the Fx.
		//
		//	Returns:
		//		The name of the Fx.  Empty string if the FxInstance is not valid.
		//		Never returns NULL.
		//
		//	Remarks:
		//		This is the name used to create the Fx with <Manager::CreateFx>.
		//
		const char* GetName() const;

		//	Function: GetId
		//
		//		Gets the bank-relative id of the Fx.
		//
		//	Returns:
		//		The id of the Fx.  <Manager::InvalidFxId> if the Fx is not valid.
		//
		//	Remarks:
		//		This is the id used to create the Fx with <Manager::CreateFx>.
		//
		unsigned int GetId() const;

		//	Function: GetBankName
		//
		//		Gets the name of the bank which owns this Fx.
		//
		//	Returns:
		//		The name of the bank which owns the Fx data.  
		//		Empty string if the Fx is not valid.
		//		Never returns NULL.
		//
		//	Remarks:
		//		This is name of the bank used to create the Fx with <Manager::CreateFx>.
		//
		const char* GetBankName() const;

		//	Function: GetBankByteStream
		//
		//		Gets the byte-stream of the bank which owns this Fx.
		//
		//	Returns:
		//		The byte-steram of the bank which owns the Fx data.
		//		Returns NULL if the Fx is not valid.
		//
		//	Remarks:
		//		This is the byte-stream used to create the Fx with <Manager::CreateFx>.
		//
		const void* GetBankByteStream() const;

		//	Function: GetNumLODLevels
		//
		//		Gets the number of LOD levels supported by this Fx.
		//
		//	Returns:
		//		The number of LOD levels.
		//		Returns 0 if the Fx does not have an LOD category, or is not valid.
		//
		int GetNumLODLevels() const;

		//	Function: GetLODCategoryName
		//
		//		Gets the name of the LOD category assigned to this Fx.
		//
		//	Returns:
		//		The name of the LOD category.
		//		Returns an empty string if the Fx does not have a category or is invalid.
		//		Never returns NULL.
		//
		//	Remarks:
		//		The LOD category name matches the string that appears in Designer.
		//
		const char* GetLODCategoryName() const;

		//	Function: SetLODDistance
		//
		//		Sets the distance to be used to determine the LOD level of the current Fx.
		//
		//	Parameters:
		//		fDistance - The distance in the same units as used in Designer.
		//
		//	Remarks:
		//		This function should be called each frame to update the Fx's LOD category.
		//		The distance should usually be the distance from the Fx to the camera.
		//
		void	SetLODDistance(float fDistance);

		//	Function: GetLOD
		//
		//		Gets the current LOD level for the Fx.
		//
		//	Returns:
		//		The current LOD level for the Fx.
		//		Returns the highest LOD level if the Fx is outside its LOD range.
		//		Returns 0 if the Fx is inside its lowest LOD range.
		//		Returns 0 if the Fx does not have LOD categories or is invalid.
		//
		//	Remarks:
		//		This function will usually be used by Components to determine
		//		if the which value specialization to use.
		//
		int		GetLOD() const;

		//	Function: GetLODFloat
		//
		//		Returns the interpolated value LOD level for the Fx.
		//
		//	Returns:
		//		The current interpolated LOD level.
		//
		//	Remarks:
		//		This value is the current LOD level plus the fractional amount to the next LOD
		//		level.  floor(<GetLODFloat>) will always equal <GetLOD>.  The fractional value is
		//		scaled evenly between each level with no regard for the actual difference in each
		//		level's minimum and maximum distance.
		//
		float	GetLODFloat() const;

		//	Function: InLODRange
		//
		//		Determines if the Fx LOD distance is within the minimum and maximum distances 
		//		of its LOD category.
		//
		//	Returns:
		//		True if the Fx is inside its LOD category or does not have an LOD category.
		//		Returns false if the Fx is not valid.
		//
		//	Remarks:
		//		When false, the Fx component's will not receive updates.  When this value changes
		//		the Fx component's will have <Component::OnLeaveLODRange> or 
		//		<Component::OnEnterLODRange> called.
		//
		bool	InLODRange() const;

		//	Function: InsideLODRange
		//
		//		Determine if the Fx LOD distance is less than the minimum distance of its
		//		LOD category.
		//
		//	Returns:
		//		True if the Fx is too close to be within its LOD range.
		//		Returns false if the Fx is not valid or does not have an LOD category.
		//
		//	Remarks:
		//		This function cannot be true if <InLODRange> is true or if <OutsideLODRange> is
		//		true.
		//
		bool	InsideLODRange() const;

		//	Function: OutsideLODRange
		//
		//		Determine if the Fx LOD distance is greater than the maximum distance of its
		//		LOD category.
		//
		//	Returns:
		//		True if the Fx is too far to be within its LOD range.
		//		Returns false if the Fx is not valid or does not have an LOD category.
		//
		//	Remarks:
		//		This function cannot be true if <InLODRange> is true or if <InsideLODRange> is
		//		true.
		//
		bool	OutsideLODRange() const;


		//	Function: GetPhaseName
		//
		//		Gets the name of the current phase for the Fx.
		//
		//	Returns:
		//		The name of the current phase.
		//		Returns an empty string if the Fx does not have phases or is invalid.
		//		Never returns NULL.
		//
		//	Remarks:
		//		This is the name of the phase as it appears in Designer.  A stopped Fx does
		//		not have a phase.  A newly created Fx will be in its first phase, if that phase
		//		starts at time zero.
		//
		const char*		GetPhaseName() const;


		//	Function: GetPhaseIndex
		//
		//		Gets the numeric index of the current phase for the Fx.
		//
		//	Returns:
		//		The numeric order index of the current phase.
		//		Returns 0 if the Fx does not have phases or is invalid.
		//
		//	Remarks:
		//		An Fx always starts at index zero.  A stopped Fx will be at one past
		//		its last phase.  One past its last phase is still zero if the Fx does
		//		not have phases.
		//
		unsigned int	GetPhaseIndex() const;

		//	Function: GetPhaseDuration
		//
		//		Gets the duration of the current phase.
		//
		//	Returns:
		//		The duration of the current phase in seconds.
		//		Returns 0 if the Fx does not have phases or is invalid.
		//
		float			GetPhaseDuration() const;

		//	Function: GetPhasePlayCount
		//
		//		Gets the number of times the Fx is to play the current phase.
		//
		//	Returns:
		//		The number of times the current phase should play.
		//		Returns -1 if the phase is infinitely looping.
		//		Returns 0 if the Fx does not have phases or is invalid.
		//
		//	Remarks:
		//		This is the total play count, as it appears in Designer.  To get the number
		//		of times the Fx has already played through the current phase, see <GetPlayCount>.
		//
		int				GetPhasePlayCount() const;

		//	Function: GetPhaseCount
		//
		//		Gets the total number of phases for the Fx.
		//
		//	Returns:
		//		The phase count for the Fx.
		//		Returns 0 if the Fx does not have phases or is invalid.
		//
		unsigned int	GetPhaseCount() const;

		//	Function: GetPhaseName
		//
		//		Gets the name of the indexed phase.
		//
		//	Parameters:
		//		nIndex - The phase index.
		//
		//	Returns:
		//		The name of the indexed phase.
		//		Returns empty string if the index is greater than the phase count or
		//		if the Fx is invalid.
		//		Never returns NULL.
		//
		const char*		GetPhaseName(unsigned int nIndex) const;

		//	Function: GetPhaseDuration
		//
		//		Gets the duration of the indexed phase.
		//
		//	Parameters:
		//		nIndex - The phase index.
		//
		//	Returns:
		//		The duration of the indexed phase in seconds.
		//		Returns 0 if the index is greater than the phase count or
		//		if the Fx is invalid.
		//
		//	Remarks:
		//		This is the name of the phase as it appears in Designer.
		//
		float			GetPhaseDuration(unsigned int nIndex) const;

		//	Function: GetPhasePlayCount
		//
		//		Gets the play count of the indexed phase.
		//
		//	Parameters:
		//		nIndex - The phase index.
		//
		//	Returns:
		//		The play count of the indexed phase in seconds.
		//		Returns -1 if the phase loops infinitely.
		//		Returns 0 if the index is greater than the phase count or
		//		if the Fx is invalid.
		//
		//	Remarks:
		//		This is the total play count, as it appears in Designer.
		//
		int				GetPhasePlayCount(unsigned int nIndex) const;

		//	Function: SetPhase
		//
		//		Sets the phase for the Fx.
		//
		//	Parameters:
		//		szPhaseName - The name of the phase.
		//
		//	Returns:
		//		True if successfully set.
		//		False if Fx is invalid or does not have the named phase.
		//
		//	Remarks:
		//		If the phase exists, the Fx will jump to the beginning of the named phase, even
		//		if the Fx is already playing that phase.  If the named phase does not exist, 
		//		<ErrorHandler::HandleError> will be called with the reason set to
		//		<ErrorData::PhaseDoesNotExist>.
		//
		bool	SetPhase(const char* szPhaseName);

		//	Function: SetPhaseByIndex
		//
		//		Sets the phase by index for the Fx.
		//
		//	Parameters:
		//		nIndex - The index of the phase.
		//
		//	Returns:
		//		True if successfully set.
		//		False if Fx is invalid or does not have the named phase.
		//
		//	Remarks:
		//		If the phase exists, the Fx will jump to the beginning of the named phase, even
		//		if the Fx is already playing that phase.  If the named phase does not exist, 
		//		<ErrorHandler::HandleError> will be called with the reason set to
		//		<ErrorData::PhaseIndexDoesNotExist>.
		//
		bool	SetPhaseByIndex(unsigned int nIndex);

		//	Function: IsAdvancingPhase
		//
		//		Determines if the Fx will play into the next phase at the end of the
		//		current phase.
		//
		//	Returns:
		//		True if the Fx will not loop at the end of the current phase.
		//		Returns true if the Fx does not have phases or is invalid.
		//
		bool	IsAdvancingPhase() const;

		//	Function: AdvancePhase
		//
		//		Advances the Fx to its next phase.
		//
		//	Parameters:
		//		bImmediate - If true, the Fx will jump to the next phase.  Otherwise, the Fx 
		//					  will play through the end of the current phase before advancing 
		//					  to the next.
		//
		//	Remarks:
		//		Use this function to advance past an infinitely looping phase.  
		//
		void	AdvancePhase(bool bImmediate = false);

		//	Function: IsPlayingToEnd
		//
		//		Determines if the Fx is currently in a mode where it will ignore all looping phases.
		//
		//	Returns:
		//		True if the Fx will ignore all looping phases.
		//
		//	Remarks:
		//		Use <SetPlayToEnd> to change the state of the Fx.  When an Fx is re-played this
		//		state will be reset to false.
		//
		bool	IsPlayingToEnd() const;


		//	Function: SetPlayToEnd
		//
		//		Allows the Fx to play straight through all looping phases until it is finished.
		//
		//	Parameters:
		//		bEnable - If true, the Fx will ignore all looping phases and play until it is stopped.
		//					If false, the Fx will once again loop when told to by a phase.
		//
		//	Remarks:
		//		Use this function to get a potentially looping Fx to finish.  If the Fx is restarted
		//		after it is finished, it will not play straight through again.
		//
		void	SetPlayToEnd(bool bEnable = true);


		//	Function: GetPlayCount
		//
		//		Gets the number of times the Fx has played through the current phase.
		//
		//	Returns:
		//		The number of times the Fx has played through the current phase.
		//		Returns 0 if the Fx does not have phases or is invalid.
		//
		//	Remarks:
		//		This is the number of times the current phase has been repeated.  It does 
		//		not include the current time.
		//
		int		GetPlayCount() const;


		//	Function: SetInput
		//
		//		Sets an integer input value.
		//
		//	Parameters:
		//		szInputName - The name of the integer input.
		//		nValue      - The new value for the input.
		//
		//	Returns:
		//		True if successful.
		//		Returns false if the input is not used or if the Fx is invalid.
		//
		//	Remarks:
		//		Will call <ErrorHandler::HandleError> if the input is not an integer value.
		//
		bool	SetInput(const char* szInputName, int nValue);

		//	Function: SetInput
		//
		//		Sets a float input value.
		//
		//	Parameters:
		//		szInputName - The name of the float input.
		//		fValue      - The new value for the input.
		//
		//	Returns:
		//		True if successful.
		//		Returns false if the input is not used or if the Fx is invalid.
		//
		//	Remarks:
		//		Will call <ErrorHandler::HandleError> if the input is not a float value.
		//
		bool	SetInput(const char* szInputName, float fValue);

		//	Function: SetInput
		//
		//		Sets a Vector3 input value.
		//
		//	Parameters:
		//		szInputName - The name of the float input.
		//		vValue      - The new value for the input.
		//
		//	Returns:
		//		True if successful.
		//		Returns false if the input is not used or if the Fx is invalid.
		//
		//	Remarks:
		//		Will call <ErrorHandler::HandleError> if the input is not a Vector value.
		//
		bool	SetInput(const char* szInputName, const Vector3& vValue);
		
		//	Function: SetInput
		//
		//		Sets a string input value.
		//
		//	Parameters:
		//		szInputName - The name of the float input.
		//		szValue     - The new value for the input.
		//
		//	Returns:
		//		True if successful.
		//		Returns false if the input is not used or if the Fx is invalid.
		//
		//	Remarks:
		//		Will call <ErrorHandler::HandleError> if the input is not a string value.
		//		The string is _not_ copied.  The caller must manage the string's memory.
		//
		bool	SetInput(const char* szInputName, const char* szValue);


		//	Function: GetIntegerInput
		//
		//		Gets a pointer to the storage of the input value.
		//
		//	Parameters:
		//		szInputName - The name of the input.
		//
		//	Returns:
		//		A pointer to the storage of the input value.
		//		Returns NULL if the input is not used or if the Fx is invalid.
		//
		//	Remarks:
		//		This returns a pointer to storage within the Fx.  It can be used
		//		to directly set an input value.  This pointer becomes invalid
		//		when the Fx is released.  Be very careful about Fx lifetime when
		//		storing this return value!  <FxListener::OnFxDestroyed> should be used
		//		to clear the stored pointer when the Fx is released.
		//
		//		Calls <ErrorHandler::HandleError> if the input is not an integer.
		//
		int*				GetIntegerInput(const char* szInputName);
		const int*			GetIntegerInput(const char* szInputName) const;

		//	Function: GetFloatInput
		//
		//		Gets a pointer to the storage of the input value.
		//
		//	Parameters:
		//		szInputName - The name of the input.
		//
		//	Returns:
		//		A pointer to the storage of the input value.
		//		Returns NULL if the input is not used or if the Fx is invalid.
		//
		//	Remarks:
		//		This returns a pointer to storage within the Fx.  It can be used
		//		to directly set an input value.  This pointer becomes invalid
		//		when the Fx is released.  Be very careful about Fx lifetime when
		//		storing this return value!  <FxListener::OnFxDestroyed> should be used
		//		to clear the stored pointer when the Fx is released.
		//
		//		Calls <ErrorHandler::HandleError> if the input is not a float.
		//
		float*				GetFloatInput(const char* szInputName);
		const float*		GetFloatInput(const char* szInputName) const;

		//	Function: GetVector3Input
		//
		//		Gets a pointer to the storage of the input value.
		//
		//	Parameters:
		//		szInputName - The name of the input.
		//
		//	Returns:
		//		A pointer to the storage of the input value.
		//		Returns NULL if the input is not used or if the Fx is invalid.
		//
		//	Remarks:
		//		This returns a pointer to storage within the Fx.  It can be used
		//		to directly set an input value.  This pointer becomes invalid
		//		when the Fx is released.  Be very careful about Fx lifetime when
		//		storing this return value!  <FxListener::OnFxDestroyed> should be used
		//		to clear the stored pointer when the Fx is released.
		//
		//		Calls <ErrorHandler::HandleError> if the input is not a Vector3.
		//
		FxStudio::Vector3*			GetVector3Input(const char* szInputName);
		const FxStudio::Vector3*	GetVector3Input(const char* szInputName) const;

		//	Function: GetStringInput
		//
		//		Gets a pointer to the storage of the input value.
		//
		//	Parameters:
		//		szInputName - The name of the input.
		//
		//	Returns:
		//		A pointer to the storage of the input value.
		//		Returns NULL if the input is not used or if the Fx is invalid.
		//
		//	Remarks:
		//		This returns a pointer to storage within the Fx.  It can be used
		//		to directly set an input value.  This pointer becomes invalid
		//		when the Fx is released.  Be very careful about Fx lifetime when
		//		storing this return value!  <FxListener::OnFxDestroyed> should be used
		//		to clear the stored pointer when the Fx is released.
		//
		//		Notice that the input is a pointer to a pointer. Just set it equal
		//		to the array which is storing the string data and update the array.
		//
		//		Calls <ErrorHandler::HandleError> if the input is not a string.
		//
		const char* *			GetStringInput(const char* szInputName);
		const char* const *		GetStringInput(const char* szInputName) const;
		
		//	Function: GetNumComponents
		//
		//		Gets the number of components created by this Fx.
		//
		//	Returns:
		//		The number of components created.
		//
		//	Remarks:
		//		This value is equivalent to <EndComponents> - <BeginComponents>.
		//
		int		GetNumComponents() const;

		//	Function: BeginComponents
		//
		//		Gets a pointer to the beginning of the array of component data.
		//
		//	Returns:
		//		A pointer to the beginning of the component data.
		//
		//	Remarks:
		//		<ComponentData> will be created whether the component is successfully
		//		allocated or not.  Be sure to check that the component pointer within the 
		//		comonent data is not NULL before using it.
		//
		const FxStudio::ComponentData* BeginComponents() const;


		//	Function: EndComponents
		//
		//		Gets a pointer to the end (one past the final element) of the array of component 
		//		data.
		//
		//	Returns:
		//		A pointer to the end of the array of component data.
		//
		//	Remarks:
		//		The value returned is equivalent to <BeginComponents> + <GetNumComponents>.
		//
		const FxStudio::ComponentData* EndComponents() const;

	private :

		friend class Internal::Fx;

		void* m_pData;

		mutable const FxInstance* m_pPreviousInstance;
		mutable const FxInstance* m_pNextInstance;
		mutable bool              m_bIsValid;
	};

	
	//	Function: Equality Operator
	//
	//		Determines if two FxInstance's refer to the same Fx.
	//
	//	Returns:
	//		True if both FxInstance's refer to the same Fx or are both invalid.
	//
	inline bool operator==(const FxInstance& lhs, const FxInstance& rhs)
	{
		return lhs.GetData() == rhs.GetData();
	}

	//	Function: Inequality Operator
	//
	//		Determines if two FxInstance's refer to the different Fx.
	//
	//	Returns:
	//		False if both FxInstance's refer to the same Fx or are both invalid.
	//
	inline bool operator!=(const FxInstance& lhs, const FxInstance& rhs)
	{
		return lhs.GetData() != rhs.GetData();
	}

	//	Function: Less-Than Operator
	//
	//		Provides a strictly-weak ordering of FxInstances.
	//
	//	Returns:
	//		True if one FxInstance is less than the other.
	//
	//	Remarks:
	//		This function can be used to sort a list of FxInstance.  FxInstances 
	//		which refer to invalid Fx will be considered less-than all others.
	inline bool operator<(const FxInstance& lhs, const FxInstance& rhs)
	{
		return lhs.GetData() < rhs.GetData();
	}
}


#endif // FXSTUDIO_FXINSTANCE_H_
