/*****************************************************************

    MODULE    : ErrorData.h

    PURPOSE   : The ErrorData class is used to describe global errors.

    CREATED   : 5/2/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/

#ifndef FXSTUDIO_ERRORDATA_H_
#define FXSTUDIO_ERRORDATA_H_

#include <utility>

#include "FxStudioDefines.h"

namespace FxStudio
{
	class Manager;
	class Component;
	class FxInstance;

	//	Struct: ErrorData
	//
	//		This struct is used to pass data to <ErrorHandler::HandleError>.  Each
	//		error will have a unique Section and Reason.  Depending on the section and
	//		reason, various parameters will be set to provide more information about
	//		the error.
	//
	struct FXSTUDIO_API ErrorData
	{
		// Enumerations: Section
		//
		//		UnknownSection - This value should never be encountered.
		//
		//		Preview - The error occured within the Preview system.
		//
		//		CreateManager - The error occured while calling <CreateManager> in Manager.h.
		//
		//		FxBankLoading - The error occured while loading an Fx bank.
		//		FxBankUsage   - The error occured while using an Fx bank.
		//
		//		CreateFx      - The error occured while calling <Manager::CreateFx>.
		//
		//		Instance      - The error occured while using an <FxInstance>.
		//
		//      FxComponent   - The error occured while using a <Component>.
		//
		enum Section
		{
			UnknownSection = -1,

			Preview,

			CreateManager,

			FxBankLoading,
			FxBankUsage,
			FxBankInfo,

			CreateFx,

			Instance,

			FxComponent
		};

		// Enumerations: Reason
		//
		//		UnknownReason - This should never be encountered.
		//
		//		ExpiredOrInvalidKey - The SDK's license key is invalid or has expired.
		//
		//		OutOfMemory - A call to <Allocator::AllocateBytes> returned NULL.
		//
		//		MissingHandshake - A connection to the Preview system failed to receive the correct
		//						   handshake message.
		//
		//		ReadPastEnd - Attempted to read past the end of a memory buffer has occured.
		//		IncorrectDataType - Attempted to read memory as the wrong type data.
		//		IncorrectPlatform - Attempted to read an FxBank built for a different platform.
		//		IncorrectVersion  - Attempted to load data which is from a different version of the
		//							FxStudio suite.
		//		InconsistentData - The Fx bank memory is not self-consistent.  Usually indicates 
		//						   memory corruption.
		//
		//		BankDoesNotExist - Attempted to create an Fx from a bank which is not loaded.
		//		BankIndexIsTooLarge - Attempted to access more banks than are loaded.
		//		
		//		FxDoesNotExist - Attempted to create an Fx which does not exist.
		//		FxIndexIsTooLarge - Attempted to access more Fx than are defined.
		//
		//		PropertyDoesNotExist - Attempted to access a Component property which does not exist.
		//		ComponentDoesNotExist - Attempted to access a component which does not exist.
		//
		//		WrongPropertyType - Attempted to use a property whose type does not match the 
		//							requested type.
		//		WrongInputType - Attempted to use an input whose type does not match the requested type.
		//
		//		PhaseDoesNotExist - Attempted to access a phase which does not exist.
		//		PhaseIndexDoesNotExist - Attempted to acces more phases than exist.
		//
		//		PlayingInvalidFx - <FxInstance::Play> was called with an invalid FxInstance.
		//		PausingInvalidFx - <FxInstance::Pause> was called with an invalid FxInstance.
		//		ListeningToInvalidFx - <FxInstance::Listen> was called with an invalid FxInstance.
		//		SettingPhaseOnInvalidFx - <FxInstance::SetPhase> or <FxInstance::SetPhaseByIndex> was called with an invalid Fx.
		//
		enum Reason
		{
			UnknownReason = -1,

			ExpiredOrInvalidKey,

			OutOfMemory,

			MissingHandshake,

			ReadPastEnd,
			IncorrectDataType,
			IncorrectPlatform,
			IncorrectVersion,
			InconsistentData,

			BankDoesNotExist,
			BankIndexIsTooLarge,

			FxDoesNotExist,
			FxIndexIsTooLarge,

			PropertyDoesNotExist,
			ComponentDoesNotExist,

			WrongPropertyType,
			WrongInputType,

			PhaseDoesNotExist,
			PhaseIndexDoesNotExist,

			PlayingInvalidFx,
			PausingInvalidFx,
			ListeningToInvalidFx,
			SettingPhaseOnInvalidFx

		};

		// Constructor: ErrorData
		//
		//		Creates an ErrorData struct with all of its member variables cleared.
		//
		ErrorData();

		// Variable: Section
		//
		//		Indicates the section, or function call, which triggered the error.
		//
		Section m_Section;

		// Variable: Reason
		//
		//		Indicates the reason for the error.
		//
		Reason  m_Reason;

		//	Function: CreateMessage
		//
		//		Creates a descriptive error message for the given error data.
		//
		//	Parameters:
		//		szDestination - A character buffer to hold the message.
		//		nDestinationCapacity - The size of szDestination.
		//
		//	Returns:
		//		True if a full message could be generated.
		//		False if the combination of section and reason are invalid.
		//
		//	Remarks:
		//		This will always return a message string.  However, if it returns false the message
		//		is usually "An unknown error occured while...".  Please report any failure to create
		//		a message from data passed into <ErrorHandler::HandleError> to support@aristen.com.
		//
		bool CreateMessage(char* szDestination, std::size_t nDestinationCapacity) const;

		//	Variable: m_pManager
		//		Will be set to the current <Manager>, if possible.
		const Manager*		m_pManager;

		//	Variable: m_pInstance
		//		Will be set to the current <FxInstance>, if possible.
		const FxInstance*	m_pInstance;

		//	Variable: m_pComponent
		//		Will be set to the current <Component>, if possible.
		const Component*	m_pComponent;

		//	Variable: m_nParameter1
		//		The passed in value which caused the error, if appropriate.
		int		m_nParameter1;

		//	Variable: m_nParameter2
		//		The expected value, if appropriate.
		int		m_nParameter2;

		//	Variable: m_szParameter1
		//		The passed in value which caused the error, if appropriate.
		const char* m_szParameter1;

		//	Variable: m_szParameter2
		//		The expected value, if appropriate.
		const char* m_szParameter2;

		//	Variable: m_nParameter1
		//		The passed in value which caused the error, if appropriate.
		const void* m_pParameter1;
	};


	//	Class: ErrorHandler
	//
	//		Inherit from this class if you wish to handle errors that occur in the Runtime.
	//		It can be registered via <RegisterErrorHandler> below.
	//
	class FXSTUDIO_API ErrorHandler
	{
	public :

		virtual ~ErrorHandler() { }

		//	Function: HandleError
		//
		//		Called by the Runtime when an error occurs.
		//
		//	Parameters:
		//		error_data - The relevant data for the error.
		//
		//	Remarks:
		//		This function will be called whenever an error occurs.  The error_data
		//		is only valid for the duration of the function call.  If you need to store
		//		the data, you must copy it.
		//
		virtual void HandleError(const FxStudio::ErrorData& error_data) = 0;
	};


	//	Function: RegisterErrorHandler
	//
	//		Registers a <ErrorHandler> derived class as the current error handler.
	//
	//	Parameters:
	//		error_handler - The new error handler.
	//
	//	Remarks:
	//		It is expected that this error handler will remain valid for the duration
	//		of the SDK usage.  It is a good practice to store the handler returned by
	//		<GetErrorHandler> first and restore that error handler when the current
	//		handler becomes inactive.
	//
	FXSTUDIO_API void RegisterErrorHandler(ErrorHandler& error_handler);

	//	Function: GetErrorHandler
	//
	//		Gets the current error handler.
	//
	//	Returns:
	//		The current error handler.
	//
	FXSTUDIO_API ErrorHandler& GetErrorHandler();
}

#endif // FXSTUDIO_ERRORDATA_H_