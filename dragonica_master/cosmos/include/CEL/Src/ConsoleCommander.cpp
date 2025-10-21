#include "stdafx.h"
#include "../ConsoleCommander.h"

using namespace CEL;

CConsoleCommander::CConsoleCommander(void)
{
	StopSignal(false);
	m_pCloseFunc = NULL;
}

CConsoleCommander::~CConsoleCommander(void)
{
}

void CConsoleCommander::MainLoof()
{
	HANDLE const hIn  = ::GetStdHandle(STD_INPUT_HANDLE);
	HANDLE const hOut = ::GetStdHandle(STD_OUTPUT_HANDLE);

	DWORD const MAX_INPUT_BUFFER = 100;
	INPUT_RECORD aIRBuffer[MAX_INPUT_BUFFER] = {0,};

	while( !StopSignal() )
	{
		DWORD dwResult = 0;

		if( ::PeekConsoleInput( hIn, aIRBuffer, 1, &dwResult) )
		{
			dwResult = __min(MAX_INPUT_BUFFER, dwResult);
			DWORD dwIndex = 0;
			
			while(dwIndex != dwResult)
			{
				INPUT_RECORD const& rIR = aIRBuffer[dwIndex];
				if( KEY_EVENT == rIR.EventType )
				{
					KEY_EVENT_RECORD const& rKER = rIR.Event.KeyEvent;
					if(rKER.bKeyDown)
					{
						WORD const& wKey = rKER.wVirtualKeyCode;
						Execute( wKey, wKey);
					}
				}
				++dwIndex;
			}
		}

		::memset( aIRBuffer, 0, sizeof(aIRBuffer) );

		::FlushConsoleInputBuffer(hIn);
		::Sleep( 8 );
	}

	if(m_pCloseFunc)
	{
		m_pCloseFunc();
	}
}
