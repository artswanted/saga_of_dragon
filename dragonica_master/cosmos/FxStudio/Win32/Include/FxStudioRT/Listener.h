/*****************************************************************

    MODULE    : Listener.h

    PURPOSE   : The base class for all listeners.  Handles management
	            of the listener list.

    CREATED   : 5/21/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/
#ifndef FXSTUDIO_LISTENER_H_
#define FXSTUDIO_LISTENER_H_

#include "FxStudioDefines.h"

//
// Title: Listener
//

namespace FxStudio
{
	//	Class: BaseListener
	//
	//		This class is used as a base class for all other Listener classes
	//		in the Runtime.
	//
	class FXSTUDIO_API BaseListener
	{
	public :

		//	Constructor: BaseListener
		//
		//		Creates an un-attached listener.
		//
		BaseListener()
			: m_pPreviousListener(0)
			, m_pNextListener(0)
		{}

		//	Destructor: ~BaseListener
		//
		//		Safely detaches the Listener before its memory is released.
		//
		//	Remarks:
		//		The destructor will call detach, so that it is safe to be destroyed while
		//		attached.  Likewise, any object being listened to will detach all of its
		//		Listeners before it is destroyed.
		//
		virtual ~BaseListener();

		//	Function: Attach
		//
		//		Attaches a Listener to the head of a chain of Listeners.
		//
		//	Parameters:
		//		pNextListener - The previous head of the chain of Listeners.
		//
		//	Remarks:
		//		This function will usually be called by the class being listened to (the
		//		"subject").
		//
		void Attach(BaseListener* pNextListener);		

		//	Function: Detach
		//
		//		Removes the Listener from the chain of Listeners which recieve events from
		//		the subject.
		//
		//	Remarks:
		//		This function can be used to stop listening to an object without
		//		destroying the Listener instance.
		//
		void Detach();

		//	Function: IsAttached
		//
		//		Determines if this Listener is currently attached.
		//
		//	Returns:
		//		True if attached to a live object.
		//
		//	Remarks:
		//		Can be used to determine if this Listener is active.
		//
		bool IsAttached() const;


		//	Function: GetNext
		//
		//		This function is used by the object being listened to, it should not
		//		be called by any other code.
		//
		//	Returns:
		//		The next Listener attached to this listener.  NULL if this is the last
		//		Listener in the chain.
		//
		BaseListener* GetNext() { return m_pNextListener; }

	private :

		BaseListener* m_pPreviousListener;
		BaseListener* m_pNextListener;
	};
}

#endif // FXSTUDIO_LISTENER_H_