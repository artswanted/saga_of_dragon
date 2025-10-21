/*****************************************************************

    MODULE    : Manager.h

    PURPOSE   : The Manager class is used to manage Fx and FxBanks.

    CREATED   : 5/2/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/
#ifndef FXSTUDIO_MANAGER_H_
#define FXSTUDIO_MANAGER_H_

#include <utility>

#include "FxStudioDefines.h"

#include "ErrorData.h"
#include "FxInstance.h"

namespace FxStudio
{
	class FxVisitor;

	// Class: Manager
	//
	//		This is the main controller of Fx.  Multiple Manager's can exist at the same
	//		time, however only the allocator and error handler will be shared 
	//		between them.
	//
	class FXSTUDIO_API Manager
	{
	public :


		// Enumeration: InvalidFxId
		//
		//		Id used to indicate an invalid Fx value from <GetFxId>.  This value can 
		//		also be used to indicate an empty value in packed data.
		//
		enum
		{
			InvalidFxId = 0xFFFFFFFF
		};

	public :

		//	Function: GetBankVersion
		//
		//		Gets the current bank file version Id expected by this library.
		//
		//	Returns:
		//		The bank file version Id.
		//
		//	Remarks:
		//		The processor embeds the version Id it was build against into every bank
		//		file it packs.  The Manager will refuse to load a bank file whose version
		//		Id does not match this version Id.
		//
		virtual unsigned int GetBankVersion() const = 0;

		//	Function: LoadBank
		//
		//		Loads or reloads an Fx bank from a memory block.
		//
		//	Parameters:
		//		pByteStream - The beginning of the memory block.
		//		nStreamLen  - The size of the memory block.
		//
		//	Returns:
		//		True if the bank was successfully loaded.
		//
		//	Remarks:
		//		The Fx bank holds all of the constant data for created Fx.  The memory
		//		that is passed in must remain valid until the bank is unloaded, either 
		//		through <UnloadBank> or by loading another bank with the same name.
		//
		//		If a bank of the same name exists, LoadBank will perform a dynamic
		//		reload of the FxBank data.  This is how the "real-time" updating of
		//		previewed Fx is done.  All Fx created from the bank being reloaded will
		//		attempt to update their data dynamically.  If they cannot successfully
		//		reload themselves, they will be released.
		//
		//		If a bank fails to load, this function will return false and 
		//		<ErrorHandler::HandleError> will be called with <ErrorData::Section> set 
		//		to <ErrorData::FxBankLoading>.
		//
		virtual bool LoadBank(const void* pByteStream, std::size_t nStreamLen) = 0;


		//	Function: UnloadBank
		//
		//		Unloads the named bank.
		//
		//	Parameters:
		//		szBankName - The name of the bank to unload.
		//
		//	Returns:
		//		True if successfully loaded.
		//		False only if the bank is not already loaded.
		//
		//	Remarks:
		//		Any Fx created from this bank will be automatically released.
		//
		virtual bool UnloadBank(const char* szBankName) = 0;

		//	Function: UnloadBank
		//
		//		Unloads the bank corresponding to the memory location.
		//
		//	Parameters:
		//		pByteStream - The memory location used to load the bank.
		//
		//	Returns:
		//		True if successfully loaded.
		//		False only if the bank is not already loaded or if pByteStream is NULL.
		//
		//	Remarks:
		//		Any Fx created from this bank will be automatically released.
		//
		virtual bool UnloadBank(const void* pByteStream) = 0;

		//	Function: UnloadAllBanks
		//
		//		Unloads all currently loaded banks.
		//
		//	Remarks:
		//		Any Fx create by this Manager will be automatically released.
		//
		virtual void UnloadAllBanks() = 0;

		//	Function: IsBankLoaded
		//
		//		Determines if a bank with the given name is already loaded.
		//
		//	Parameters:
		//		szBankName - The name of the bank.
		//
		//	Returns:
		//		True if the bank is already loaded by this Manager.
		//
		virtual bool IsBankLoaded(const char* szBankName) const = 0;

		//	Function: IsBankLoaded
		//
		//		Determines if a bank is already loaded from the given memory location.
		//
		//	Parameters:
		//		pByteStream - The start of the memory block for the Fx bank.
		//
		//	Returns:
		//		True if the bank is already loaded by this Manager.
		//
		virtual bool IsBankLoaded(const void* pByteStream)const = 0;

		//	Function: GetBankCount
		//
		//		Determines the number of currently loaded Fx banks.
		//
		//	Returns:
		//		The count of loaded Fx banks.
		//
		virtual unsigned int  GetBankCount() const = 0;

		//	Function: GetBankNameByOffset
		//
		//		Gets the name of the indexed bank.
		//
		//	Parameters:
		//		nIndex - The index of the bank. Must be between 0 and <GetBankCount>.
		//
		//	Returns:
		//		The name of the bank.
		//		Returns an empty string if the index is too large.
		//		Never returns NULL.
		//
		//	Remarks:
		//		This function can be used to get a list of currently loaded bank names.
		//
		//		If the index is too large, <ErrorHandler::HandleError> will be called.
		//
		virtual const char*   GetBankNameByOffset(unsigned int nIndex) const = 0;

		//	Function: GetBankName
		//
		//		Gets the name of a loaded Fx bank.
		//
		//	Parameters:
		//		pByteStream - The pointer to the memory used to load the Fx bank.
		//
		//	Returns:
		//		The name of the bank.
		//		Returns an empty string if the bank does not exist.
		//		Never returns NULL.
		//
		//	Remarks:
		//		Notices that the Fx bank must be loaded before the name can be determined.
		//		<IsBankLoaded> can be used to determine if the
		//		bank is loaded.
		//
		//		If the bank is not loaded, <ErrorHandler::HandleError> will be called.
		//
		virtual const char*   GetBankName(const void* pByteStream) const = 0;

		//	Function: Update
		//
		//		Updates the Manager and all of its Fx.
		//
		//	Parameters:
		//		fDeltaTime - The amount of time, in seconds, which has passed since 
		//					 the last update.  The time should be greater than or equal to
		//					 zero.
		//
		//	Remarks:
		//		This function should be called each frame.  It will update all auto-updated
		//		Fx.  It will also release all auto-released Fx which have expired.
		//
		//		This function can be called with a delta time of zero in order to do
		//		a "maintenance" update, activate any newly created Fx and release any
		//		expired Fx.
		//
		virtual void Update(float fDeltaTime) = 0;

		//	Function: Render
		//
		//		Calls <Component::Render> on all active Fx components.
		//
		//	Parameters:
		//		pRenderData - The data to be passed to <Component::Render>.
		//
		//	Remarks:
		//		This can be used to trigger a render on all active, auto-rendered Fx
		//		components.  The same action could be performed via <VisitFx>, however
		//		this is simpler.
		//
		virtual void Render(void* pRenderData) = 0;

		//	Function: VisitFx
		//
		//		Provides a mechanism to walk all Fx owned by the Manager.
		//
		//	Parameters:
		//		visitor - The <FxVisitor> derived class containing the call-back to be
		//				  called for each Fx.
		//
		//	Remarks:
		//		This function can be used to act on, or accumulate data from, every
		//		existing Fx.
		//
		virtual void VisitFx( FxStudio::FxVisitor& visitor ) const = 0;

		//	Function: CreateFx
		//
		//		Creates a new instance of the named Fx from the named bank.
		//
		//	Parameters:
		//		szBankName - The name of the Fx bank to search for the Fx.  If this is
		//					 NULL, all Fx banks will be searched.
		//		szFxName   - The name of the Fx to be created.  An empty or NULL string 
		//					 will cause an empty FxInstance to be returned.
		//		pUserData  - The data passed to <ComponentFactory::CreateComponent> when
		//					 creating the Fx's components.
		//
		//	Returns:
		//		The <FxInstance> representing the new Fx.
		//
		//	Remarks:
		//		All Fx must be created via any one of these function overloads.
		//
		//		If a "fire-and-forget" Fx is desired, the returned FxInstance can be
		//		ignored.  However, if you wish to pause, reset, or do any other manipulation
		//		with the Fx you will need to hold onto this returned FxInstance.
		//
		virtual const FxInstance& CreateFx(const char* szBankName, const char* szFxName, void* pUserData ) = 0;

		//	Function: CreateFx
		//
		//		Creates a new instance of the named Fx from the loaded bank.
		//
		//	Parameters:
		//		pByteStream - The pointer to the memory used to load the Fx bank.  If this
		//					  is NULL, all Fx banks will be searched.
		//		szFxName   - The name of the Fx to be created.  An empty or NULL string 
		//					 will cause an empty FxInstance to be returned.
		//		pUserData  - The data passed to <ComponentFactory::CreateComponent> when
		//					 creating the Fx's components.
		//
		//	Returns:
		//		The <FxInstance> representing the new Fx.
		//
		//	Remarks:
		//		All Fx must be created via any one of these function overloads.
		//
		//		If a "fire-and-forget" Fx is desired, the returned FxInstance can be
		//		ignored.  However, if you wish to pause, reset, or do any other manipulation
		//		with the Fx you will need to hold onto this returned FxInstance.
		//
		virtual const FxInstance& CreateFx(const void* pByteStream, const char* szFxName, void* pUserData ) = 0;

		//	Function: CreateFx
		//
		//		Creates a new instance of the Id-ed Fx from the loaded bank.
		//
		//	Parameters:
		//		szBankName - The name of the Fx bank to use.  This parameter must be
		//					 an existing bank.
		//		nFxId		- The Id of the Fx to be created. <Manager::InvalidFxId> will
		//					  cause this function to just return an empty FxInstance.
		//		pUserData  - The data passed to <ComponentFactory::CreateComponent> when
		//					 creating the Fx's components.
		//
		//	Returns:
		//		The <FxInstance> representing the new Fx.
		//
		//	Remarks:
		//		All Fx must be created via any one of these function overloads.
		//
		//		The Fx Id's are unique and stable numeric Id's per bank.  They are stable
		//		in the sense that adding more Fx or removing any Fx will not change the Id.
		//		If an Fx is removed and re-added, it will be given the same Id it had before
		//		being removed.  The Id's can be read from the fxbank files or any other
		//		external tools which can load the Fx bank by linking against the Runtime
		//		system.
		//
		//		If a "fire-and-forget" Fx is desired, the returned FxInstance can be
		//		ignored.  However, if you wish to pause, reset, or do any other manipulation
		//		with the Fx you will need to hold onto this returned FxInstance.
		//
		virtual const FxInstance& CreateFx(const char* szBankName, unsigned int nFxId, void* pUserData ) = 0;

		//	Function: CreateFx
		//
		//		Creates a new instance of the Id-ed Fx from the loaded bank.
		//
		//	Parameters:
		//		szBankName - The pointer to the memory used to load the Fx bank.  This
		//					 parameter must be an existing bank.
		//		nFxId		- The Id of the Fx to be created. <Manager::InvalidFxId> will
		//					  cause this function to just return an empty FxInstance.
		//		pUserData  - The data passed to <ComponentFactory::CreateComponent> when
		//					 creating the Fx's components.
		//
		//	Returns:
		//		The <FxInstance> representing the new Fx.
		//
		//	Remarks:
		//		All Fx must be created via any one of these function overloads.
		//
		//		The Fx Id's are unique and stable numeric Id's per bank.  They are stable
		//		in the sense that adding more Fx or removing any Fx will not change the Id.
		//		If an Fx is removed and re-added, it will be given the same Id it had before
		//		being removed.  The Id's can be read from the fxbank files or any other
		//		external tools which can load the Fx bank by linking against the Runtime
		//		system.
		//
		//		If a "fire-and-forget" Fx is desired, the returned FxInstance can be
		//		ignored.  However, if you wish to pause, reset, or do any other manipulation
		//		with the Fx you will need to hold onto this returned FxInstance.
		//
		virtual const FxInstance& CreateFx(const void* pByteStream, unsigned int nFxId, void* pUserData ) = 0;
		
		//	Function: CreateFx
		//
		//		Creates the first Fx of the given name found in any bank.
		//
		//	Parameters:
		//		szFxName   - The name of the Fx to be created.  An empty or NULL string 
		//					 will cause an empty FxInstance to be returned.
		//		pUserData  - The data passed to <ComponentFactory::CreateComponent> when
		//					 creating the Fx's components.
		//
		//	Returns:
		//		The <FxInstance> representing the new Fx.
		//
		//	Remarks:
		//		All Fx must be created via any one of these function overloads.
		//
		//		If a "fire-and-forget" Fx is desired, the returned FxInstance can be
		//		ignored.  However, if you wish to pause, reset, or do any other manipulation
		//		with the Fx you will need to hold onto this returned FxInstance.
		//
		virtual const FxInstance& CreateFx(const char* szFxName, void* pUserData ) = 0;

		//	Function: ReleaseFx
		//
		//		Immediately releases an Fx.
		//
		//	Parameters:
		//		fx - The FxInstance for the Fx to be released.
		//
		//	Remarks:
		//		This will immediately stop an Fx and remove all memory associated with it.
		//		Auto-release is ignored when this function is used.
		//
		//		This function does not need to be called for Fx in auto-release mode, the
		//		Manager's <Update> function will take care of releasing the Fx.
		//
		virtual void ReleaseFx(const FxInstance& fx) = 0;

		//	Function: ReleaseAllFx
		//
		//		Releases all Fx created by this Manager.
		//
		//	Remarks:
		//		This immediately releases all Fx with no regard to auto-release.  This
		//		can be used to stop and release all Fx without unloading the Fx banks.
		//
		virtual void ReleaseAllFx() = 0;


		//	Function: GetFxId
		//
		//		Determines the Fx Id from a given Fx name.
		//
		//	Parameters:
		//		szBankName - The bank to search for the Fx.  This must be a valid Fx bank.
		//		szFxName - The name of the Fx whose Id is being requested.
		//
		//	Returns:
		//		The Id for the named Fx.  If an Fx of that name does not exist, 
		//		<Manager::InvalidFxId> will be returned.
		//
		//	Remarks:
		//		Does not trigger an error if the named Fx is not found.
		//
		virtual unsigned int GetFxId(const char* szBankName, const char* szFxName) const = 0;

		//	Function: GetFxId
		//
		//		Determines the Fx Id from a given Fx name.
		//
		//	Parameters:
		//		pByteStream - The pointer to the bank to search for the Fx.  This must be
		//					  a valid Fx bank.
		//		szFxName - The name of the Fx whose Id is being requested.
		//
		//	Returns:
		//		The Id for the named Fx.  If an Fx of that name does not exist, 
		//		<Manager::InvalidFxId> will be returned.
		//
		//	Remarks:
		//		Does not trigger an error if the named Fx is not found.
		//
		virtual unsigned int GetFxId(const void* pByteStream, const char* szFxName) const = 0;

		//	Function: GetFxName
		//
		//		Determine the Fx name from the given Fx Id.
		//
		//	Parameters:
		//		szBankName - The bank to search for the Fx.  This must be a valid Fx bank.
		//		nFxId - The Id of the Fx whose name is being requested.
		//
		//	Returns:
		//		The name of the Fx.  If the Fx is not found, empty string will be returned.
		//		Never returns NULL.
		//
		//	Remarks:
		//		Does not trigger an error if the Fx Id is not found.
		//
		virtual const char* GetFxName(const char* szBankName, unsigned int nFxId) const = 0;

		//	Function: GetFxName
		//
		//		Determine the Fx name from the given Fx Id.
		//
		//	Parameters:
		//		pByteStream - The pointer to the loaded bank to search for the Fx.  This
		//					  must be a valid Fx bank.
		//		nFxId - The Id of the Fx whose name is being requested.
		//
		//	Returns:
		//		The name of the Fx.  If the Fx is not found, empty string will be returned.
		//		Never returns NULL.
		//
		//	Remarks:
		//		Does not trigger an error if the Fx Id is not found.
		//
		virtual const char* GetFxName(const void* pByteStream, unsigned int nFxId) const = 0;

		//	Function: GetFxDefinitionCount
		//
		//		Determines the number of Fx definitions for the given bank.
		//
		//	Parameters:
		//		szBankName - The name of the bank.
		//
		//	Returns:
		//		The number of Fx definitions for the given bank.
		//		Returns 0 if the bank does not exist.
		//
		//	Remarks:
		//		Will call <ErrorHandler::HandleError> if the bank does not exist.
		//
		virtual unsigned int GetFxDefinitionCount(const char* szBankName) const = 0;

		//	Function: GetFxDefinitionCount
		//
		//		Determines the number of Fx definitions for the given bank.
		//
		//	Parameters:
		//		pByteStream - The pointer to the loaded bank.
		//
		//	Returns:
		//		The number of Fx definitions for the given bank.
		//		Returns 0 if the bank does not exist.
		//
		//	Remarks:
		//		Will call <ErrorHandler::HandleError> if the bank does not exist.
		//
		virtual unsigned int GetFxDefinitionCount(const void* pByteStream) const = 0;

		//	Function: GetFxNameByOffset
		//
		//		Gets the name of the indexed Fx.
		//
		//	Parameters:
		//		szBankName - The name of the bank.
		//		nIndex - The index of the Fx. Must be between 0 and <GetFxDefinitionCount>.
		//
		//	Returns:
		//		The name of the Fx.  If the index is too large, or the bank does not exist,
		//		will return an empty string.  
		//		Never returns NULL.
		//
		//	Remarks:
		//		This returned string will be valid until the bank is unloaded.
		//
		//		Calls <ErrorHandler::HandleError> if the bank does not exist or if the
		//		index is too large.
		//
		virtual const char* GetFxNameByOffset(const char* szBankName, unsigned int nIndex) const = 0;

		//	Function: GetFxNameByOffset
		//
		//		Gets the name of the indexed Fx.
		//
		//	Parameters:
		//		pByteStream - The pointer to the loaded bank.
		//		nIndex - The index of the Fx. Must be between 0 and <GetFxDefinitionCount>.
		//
		//	Returns:
		//		The name of the Fx.  If the index is too large, or the bank does not exist,
		//		will return an empty string.  
		//		Never returns NULL.
		//
		//	Remarks:
		//		This returned string will be valid until the bank is unloaded.
		//
		//		Calls <ErrorHandler::HandleError> if the bank does not exist or if the
		//		index is too large.
		//
		virtual const char* GetFxNameByOffset(const void* pByteStream, unsigned int nIndex) const = 0;

		//	Function: GetFxIdByOffset
		//
		//		Gets the Id of the indexed Fx.
		//
		//	Parameters:
		//		szBankName - The name of the bank.
		//		nIndex - The index of the Fx. Must be between 0 and <GetFxDefinitionCount>.
		//
		//	Returns:
		//		The Id of the Fx.  If the index is too large, or the bank does not exist,
		//		will return <Manager::InvalidFxId>.
		//
		//	Remarks:
		//		Calls <ErrorHandler::HandleError> if the bank does not exist or if the
		//		index is too large.
		//
		virtual unsigned int GetFxIdByOffset(const char* szBankName, unsigned int nOffset) const = 0;

		//	Function: GetFxIdByOffset
		//
		//		Gets the Id of the indexed Fx.
		//
		//	Parameters:
		//		pByteStream - The pointer to the loaded bank.
		//		nIndex - The index of the Fx. Must be between 0 and <GetFxDefinitionCount>.
		//
		//	Returns:
		//		The Id of the Fx.  If the index is too large, or the bank does not exist,
		//		will return <Manager::InvalidFxId>.
		//
		//	Remarks:
		//		Calls <ErrorHandler::HandleError> if the bank does not exist or if the
		//		index is too large.
		//
		virtual unsigned int GetFxIdByOffset(const void* pByteStream, unsigned int nOffset) const = 0;

	protected :

		// Constructor: Manager
		//
		//		Constructs a clean Manager instance.
		//
		//	Remarks:
		//		New Manager instances cannot be new allocated.  <CreateManager> must be used
		//		to allocate Managers.
		//
		Manager() {}

		// Destructor: ~Manager
		//
		//		Releases all memory allocated through this class, including created Fx.
		//
		//	Remarks:
		//		All existing Fx will be terminated and released when the Manager
		//		is destoryed.  This function is not accessible outside the Runtime.
		//		Use <ReleaseManager> to delete a manager.
		//
		virtual ~Manager() {}


	};


	//	Function: CreateManager
	//
	//		Creates a new Manager instance.
	//
	//	Returns:
	//		The Manager instance.  
	//		Will only return NULL if memory could not be allocated or if the
	//		license key has expired.
	//
	FXSTUDIO_API Manager* CreateManager();

	//	Function: ReleaseManager
	//
	//		Releases all memory associated with a manager.
	//
	//	Remarks:
	//		This function must used to release the manager.
	//
	FXSTUDIO_API void ReleaseManager(Manager* pManager);


	// Inherit from this class to use Manager::VisitFx.


	// Class: FxVisitor
	//
	//		Inherit from this class in order to use <Manager::VisitFx>.
	//
	class FXSTUDIO_API FxVisitor
	{
	public :

		virtual ~FxVisitor() {}

		//	Function: VisitFx
		//
		//		Function called by Manager for each Fx it owns.
		//
		//	Parameters:
		//		fx - A valid FxInstance represting the Fx.
		//
		//	Remarks:
		//		This function should be overloaded to perform whatever
		//		action is desired by the call to <Manager::VisitFx>.
		//
		virtual void VisitFx(const FxStudio::FxInstance& fx) = 0;
	};
}

#endif // FXSTUDIO_MANAGER_H_