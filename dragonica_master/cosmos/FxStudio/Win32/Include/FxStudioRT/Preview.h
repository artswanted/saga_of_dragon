/*****************************************************************

    MODULE    : Preview.h

    PURPOSE   : Used to control the preview communication thread.

    CREATED   : 5/30/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/
#ifndef FXSTUDIO_PREVIEW_H_
#define FXSTUDIO_PREVIEW_H_

#include "FxStudioDefines.h"

#include "FxInstance.h"
#include "Manager.h"

namespace FxStudio
{
	//	Class: Preview
	//
	//		This class implements the preview system in the Runtime.  It should
	//		be used as the base of a game-specific preview class.  The game specific
	//		code is responsible for spawning the Fx with the appropriate game object data.
	//
	class FXSTUDIO_API Preview
	{
	public :
		enum
		{
			//	Constant: DefaultPort
			//		This is the default port for the preview system.  If a port is not
			//		specified, this is the port value that will be used.
#if !defined(__CWCC__)
			DefaultPort = 63500
#else
			// The Wii reserves the upper 35000 ports.
			DefaultPort = 13500
#endif
		};

#if !defined(__CWCC__)
		typedef char (InternalDataType)[184];
#else
		// The Wii uses much more memory because the stack space for 
		// running the seperate thread is included.
		typedef char (InternalDataType)[5056];
#endif

	public:

		//	Constructor: Preview
		//
		//		Sets-up the internal data of the preview system.  Does _not_ start-up
		//		the system, use <Start> for that.  All memory allocation occurs in <Start>.
		//
		Preview();

		//	Destructor: ~Preview
		//
		//		Shuts down the preview thread and network connection if it is still up.
		//
		virtual ~Preview();


		//	Function: GetVersion
		//
		//		Returns the current preview system version.
		//
		//	Returns:
		//		The current preview system version.
		//
		//	Remarks:
		//		The Designer and the Runtime must have the same preview system version in
		//		order to connect.  <ErrorHandler::HandleError> will be called if a
		//		connection fails due to a version mis-match.
		//
		static unsigned int GetVersion();

		//	Function: Start
		//
		//		Starts up the preview thread so that the Runtime is ready to connect
		//		to Designer.
		//
		//	Parameters:
		//		nPort - The port to listen for connections on.
		//
		//	Remarks:
		//		This function must be called before Designer can connect to the Runtime
		//		so that Fx can be played from Designer.  A separate thread will be spawned
		//		to handle the networking communication.  Memory will not be allocated in
		//		that other thread, so that <Allocator> need not be thread safe.
		//
		//		The memory needed for message buffers is allocated in this function.  If
		//		the allocations fail, <ErrorHandler::HandleError> will be called and
		//		<IsRunning> will be false.
		//
		//		The preview system uses its own internal Manager.  That Manager will
		//		be created in this function as well.
		//
		virtual void Start(int nPort = DefaultPort);

		//	Function: Terminate
		//
		//		Disconnects from Designer and shuts down the networking thread.
		//
		//	Remarks:
		//		All allocated memory will be released when this is called.
		//
		//		The preview system's internal Manager will be released in this function.
		//
		virtual void Terminate();

		//	Function: Update
		//
		//		Updates the preview system.
		//
		//	Parameters:
		//		fDeltaTime - The amount of time which has passed since the last update.
		//
		//	Remarks:
		//		This function should be called every frame.  If the system is connected
		//		to designer, the messages will be parsed and acted upon in this function.
		//		The previewed Fx is updated in this function as well.
		//
		virtual void Update(float fDeltaTime);

		//	Function: Render
		//
		//		Renders the previewed Fx.
		//
		//	Parameters:
		//		pRenderData - The render data to be passed to the previewed Fx.
		//
		//	Remarks:
		//		This is simply a convenience function.
		//
		virtual void Render(void* pRenderData) { if (GetManager()) GetManager()->Render(pRenderData); }

		//	Function: GetWindowHandle
		//
		//		Override this function to enable embedding of the application
		//		into the Designer window.
		//
		//	Returns:
		//		The handle to the game's client window.
		//
		//	Remarks:
		//		This is only useful on the Windows platforms.  It should return the
		//		HWINDOW of the application.  The handle is passed to Designer, so that
		//		Designer can arrange itself around the window.  If the application can 
		//		not support windowed mode, do not override it.  If the handle is NULL, 
		//		Designer will not allow embedding.
		//
		virtual const void* GetWindowHandle() const { return 0; }

		//	Function: IsWindowResizable
		//
		//		Override this function to enable resizing of the application
		//		while embedded into the Designer window.
		//
		//	Returns:
		//		True if the game's client window can be resized.
		//
		//	Remarks:
		//		This is only useful on the Windows platforms.  It should return true
		//		if the application can be resized dynamically.  If true, Designer will
		//		position and resize the application window to a location which fits well 
		//		with Designer's GUI.  The application must be able to support a windowed 
		//		mode which can be resized and repositioned through WM_SIZE events.  If the 
		//		application can not support re-sizing, do not override this function.  
		//
		virtual bool IsWindowResizable() const { return false; }

		//	Function: IsRunning
		//
		//		Determines if the preview thread is running.
		//
		//	Returns:
		//		True if the preview thread is running.
		//
		//	Remarks:
		//		If the preview thread is running, it is either ready for a new
		//		connection or it is connected to Designer.
		//
		bool IsRunning() const;

		//	Function: IsConnected
		//
		//		Determines if the preview thread has a connection to Designer.
		//
		//	Returns:
		//		True if the preview thread is connected to a Designer application.
		//
		bool IsConnected() const;

		//	Function: GetListenPort
		//
		//		Returns the port being listened to for new connections.
		//
		//	Returns:
		//		The port through which new connections must arrive.
		//
		//	Remarks:
		//		This is only the port through which a connnection is initiated.  Once
		//		connected, a new port will be negotiated.  <GetConnectedPort> will return
		//		that new port.  Preview can only handle one connection at a time.
		//
		int	GetListenPort() const;

		//	Function: GetConnectedPort
		//
		//		Returns the currently connected port.
		//
		//	Returns:
		//		If connected, returns the current connection port.
		//		If not-connected, returns 0.
		//
		//	Remarks:
		//		This will be the port that was negotiated once the connection has been
		//		established.
		//
		int	GetConnectedPort() const;

		//	Function: GetConnectedAddress
		//
		//		Returns the remote address of the connection.
		//
		//	Returns:
		//		The remote address of the connection.
		//		Returns empty string if there is no connection.
		//		Will never return NULL.
		//
		//	Remarks:
		//		The returned value is only stable until the another networking call, it
		//		should be copied if it is to be stored.
		//
		const char*		GetConnectedAddress() const;

		//	Function: HasMessageData
		//
		//		Determines if there are received messages waiting to be processed.
		//
		//	Returns:
		//		True if there are messages waiting to be processed.
		//
		//	Remarks:
		//		This funciton can be used to loop until all message data has been processed.
		//		In order to do that, you should loop and call <Update> with fDeltaTime set
		//		to 0 while this function returns true.
		//		There should be no need to loop until all messages have been read, calling
		//		<Update> once per frame should be frequent enough.
		//
		bool HasMessageData() const;

		//	Function: SetPaused
		//
		//		Pauses the previewed Fx.
		//
		//	Parameters:
		//		bPaused - True if the preview should be paused.
		//
		//	Remarks:
		//		Call this function to pause the previewed Fx.  If an Fx is not playing
		//		any newly previewed Fx will start paused.
		//
		void SetPaused(bool bPaused);

		//	Function: IsPaused
		//
		//		Determines if the preview system is paused.
		//
		//	Returns:
		//		True if the preview system has paused the previewing Fx.
		//
		//	Remarks:
		//		This will return a value if it is playing an Fx or not.
		//
		bool IsPaused() const;

		//	Function: GetPreviewFx
		//
		//		Gets the Fx currently being previewed.
		//
		//	Returns:
		//		An FxInstance referring to the Fx being previewed.
		//
		//	Remarks:
		//		This instance will be invalidated and new Fx will be created when the
		//		preview Fx restarts.  So call this function whenever the previewed Fx
		//		is needed, rather than holding onto its return value.
		//
		const FxInstance&  GetPreviewFx() const;

		//	Function: SpawnFx
		//
		//		Should spawn the requested Fx so that it can be previewed.
		//
		//	Parameters:
		//		manager - The <Manager> which should be used to spawn the Fx.
		//		pByteStream - The Fx bank which should be used to spawn the Fx.
		//		szFxName - The Fx name which should be used to spawn the Fx.
		//
		//	Returns:
		//		Should return the newly created Fx.
		//
		//	Remarks:
		//		This function is called whenever a previewed Fx needs to be created.  That
		//		means that a new Fx is being requested or the old previewed Fx has finished
		//		and a new one should be started.  If it is a respawn situation, 
		//		<GetPreviewFx> will be valid and return the old Fx during this function 
		//		call.
		//
		virtual FxStudio::FxInstance SpawnFx(FxStudio::Manager& manager, const unsigned char* pByteStream, const char* szFxName) = 0;

		//	Function: SetFxTime
		//
		//		Used to set the time of a previewed Fx.
		//
		//	Parameters:
		//		fxInstance - The previewed Fx.
		//		fTime - The desired time for the Fx.  Will always be greater or equal to zero, it may be longer than the Fx time.
		//
		//	Remarks:
		//		Override this function if the system should do something different to
		//		play an Fx from a time other than zero.  For instance, the components
		//		may need to be updated floor(fTime*30) times in order to simulate running for
		//		the requested duration at 30 frames per second.
		//		
		//
		virtual void SetFxTime(FxStudio::FxInstance fxInstance, float fTime)
		{
			// Restart the Fx if its time is already past
			// the desired time.
			if( fxInstance.GetTime() > fTime )
			{
				fxInstance.Stop(true);
				fxInstance.Play();
			}

			// The default implementation simply updates by the requested time.
			// Notice that you need to disable looping of the phases when using
			// Update to advance the Fx time.
			const float fTimeDelta = fTime - fxInstance.GetTime();
			const bool bPlayToEnd = fxInstance.IsPlayingToEnd();
			fxInstance.SetPlayToEnd(true);
			fxInstance.Update(fTimeDelta);
			fxInstance.SetPlayToEnd(bPlayToEnd);
		}

		//	Function: GetManager
		//
		//		Gets the preview system's Manager.
		//
		//	Returns:
		//		The preview system's Manager if it exists.
		//		Returns NULL if the Manager has not been created.
		//
		//	Remarks:
		//		The Manager is created when <Start> is called.  It is released when
		//		<Terminate> is called.
		//
		FxStudio::Manager* GetManager();
		const FxStudio::Manager* GetManager() const;

		
	private :
		
		// This class cannot be copied.
		Preview(const Preview&);
		Preview& operator=(const Preview&);

		InternalDataType m_InternalData;

	};
}


#endif // FXSTUDIO_PREVIEW_H_