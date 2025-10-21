#include "stdafx.h"
#include "PgMobileSuit.H"
#include "PgInputSystem.H"
#include "WindowsX.H"
#include "PgOption.h"

NiImplementRTTI(PgInputSystem, NiDI8InputSystem);
NiImplementRTTI(PgInputMouse, NiInputDI8Mouse);
NiImplementRTTI(PgInputKeyboard, NiInputDI8Keyboard);
NiImplementRTTI(PgInputGamePad, NiInputDI8GamePad);

//////////////////////////////////////////////////////////////////////////////////////////////////
//	class	PgInputSystem
//////////////////////////////////////////////////////////////////////////////////////////////////

NiInputGamePad::Button PgInputSystem::eButtons[NiInputGamePad::NIGP_NUMBUTTONS]={
	NiInputGamePad::NIGP_LUP,        NiInputGamePad::NIGP_LDOWN,
	NiInputGamePad::NIGP_LLEFT,        NiInputGamePad::NIGP_LRIGHT,
	NiInputGamePad::NIGP_L1,        NiInputGamePad::NIGP_L2,
	NiInputGamePad::NIGP_R1,        NiInputGamePad::NIGP_R2,
	NiInputGamePad::NIGP_RUP,        NiInputGamePad::NIGP_RDOWN,
	NiInputGamePad::NIGP_RLEFT,        NiInputGamePad::NIGP_RRIGHT,
	NiInputGamePad::NIGP_A,        NiInputGamePad::NIGP_B,
	NiInputGamePad::NIGP_START,        NiInputGamePad::NIGP_SELECT
};

NiInputGamePad::Button PgInputSystem::eAxisToButtons[NiInputGamePad::NIGP_NUMBUTTONS]={
	NiInputGamePad::NIGP_LLEFT,        NiInputGamePad::NIGP_LRIGHT,
	NiInputGamePad::NIGP_LUP,        NiInputGamePad::NIGP_LDOWN,
	NiInputGamePad::NIGP_RLEFT,        NiInputGamePad::NIGP_RRIGHT,
	NiInputGamePad::NIGP_RDOWN,        NiInputGamePad::NIGP_RUP,
	NiInputGamePad::NIGP_NONE, NiInputGamePad::NIGP_NONE,
	NiInputGamePad::NIGP_NONE, NiInputGamePad::NIGP_NONE,
	NiInputGamePad::NIGP_NONE, NiInputGamePad::NIGP_NONE,
	NiInputGamePad::NIGP_NONE, NiInputGamePad::NIGP_NONE,
};

bool PgInputSystem::bAxisToButtonsPressedBefore[NiInputGamePad::NIGP_NUMBUTTONS];
bool PgInputSystem::bAxisToButtonsPressed[NiInputGamePad::NIGP_NUMBUTTONS];

PgInputSystem::PgInputSystem(CreateParams* pkParams)
{
	if(pkParams)
	{
		SetActionMap(pkParams->GetActionMap());
		
		m_spInputForGamePad = NiInputSystem::Create(pkParams);
		m_spInputForGamePad->SetActionMap(pkParams->GetActionMap());

		for (unsigned int i = 0; i < NiInputGamePad::NIGP_NUMBUTTONS; i++)
		{
			bAxisToButtonsPressedBefore[i] = false;
			bAxisToButtonsPressed[i] = false;
		}
	}

	m_fGamePadAxisSensitivityThreshold = 0.8f;
}

PgInputSystem::~PgInputSystem()
{
	if(m_spInputForGamePad)
	{
		m_spInputForGamePad = NULL;
	}
}

NiInputErr PgInputSystem::CheckEnumerationStatus(void)
{
	return	NIIERR_OK;
}
void	PgInputSystem::ReleaseAllKey()
{
	if(m_spKeyboard)
	{
		PgInputKeyboard* pkKeyboard = NiDynamicCast(PgInputKeyboard,m_spKeyboard);
		pkKeyboard->ReleaseAllKey(this);
	}
	if(m_spMouse)
	{
		PgInputMouse* pkMouse = NiDynamicCast(PgInputMouse,m_spMouse);
		pkMouse->ReleaseAllKey(this);
	}

	if (m_spInputForGamePad)
	{
		for (unsigned int i = 0; i < NiInputGamePad::NIGP_NUMBUTTONS; i++)
		{
			HandleButtonRelease(eAxisToButtons[i]);
			bAxisToButtonsPressed[i] = false;
			bAxisToButtonsPressedBefore[i] = false;
		}
	}
}
void PgInputSystem::SetConversionStatus(HWND hWnd, bool const bIsOn, DWORD const dwNewConvMode, DWORD const dwNewSentMode)
{
	DWORD dwConversion = 0, dwSentence = 0;

	HIMC hIMC = ::ImmGetContext(hWnd);
	if(!hIMC){return;}
	
	::ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
	
	if(dwNewConvMode || dwNewSentMode)
	{
		if(bIsOn)
		{
			if(	!(dwConversion & dwNewConvMode)
			||	!(dwNewSentMode & dwSentence))
			{
				::ImmSetConversionStatus( hIMC, dwConversion | dwNewConvMode, dwSentence | dwNewSentMode );
			}
		}
		else
		{
			if(	(dwConversion & dwNewConvMode)
			||	(dwNewSentMode & dwSentence))
			{
				::ImmSetConversionStatus( hIMC, dwConversion & ~dwNewConvMode, dwSentence & ~dwNewSentMode );
			}
		}
	}
	::ImmReleaseContext(hWnd, hIMC);
}

bool	PgInputSystem::HandleWindowMessage(unsigned int uiMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uiMsg)
	{
	case WM_IME_NOTIFY:
		{
			if(wParam == IMN_SETCONVERSIONMODE)
			{
				if(g_pkApp)
				{
					if(XUIMgr.GetFocusedEdit() == NULL)
					{
						SetConversionStatus(g_pkApp->GetWindowReference(), true, IME_CMODE_ALPHANUMERIC, NULL);
					}
				}
			}
		}
		break;
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			if(m_spKeyboard)
			{
				PgInputKeyboard* pkKeyboard = NiDynamicCast(PgInputKeyboard,m_spKeyboard);
				return	pkKeyboard->HandleWindowMessage(this,uiMsg,wParam,lParam);
			}
		}
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
		{
			if(m_spMouse)
			{
				PgInputMouse* pkMouse = NiDynamicCast(PgInputMouse,m_spMouse);
				return	pkMouse->HandleWindowMessage(this,uiMsg,wParam,lParam);
			}
		}
		break;

	}

	return	false;	//	false means that uiMsg has not been handled
}

//패드 버튼이 Down됐을 때 처리
void PgInputSystem::HandleButtonDown(NiInputGamePad::Button eButton)
{
	if(NiInputGamePad::NIGP_NONE == eButton)
	{
		return;
	}

	PgInputKeyboard* pkKeyboard = NiDynamicCast(PgInputKeyboard, m_spKeyboard);
	if(pkKeyboard)
	{
		NiInputKeyboard::KeyCode kKeycode = static_cast<NiInputKeyboard::KeyCode>(g_kGlobalOption.GetUKeyToKey(m_kPadSetting[eButton].m_iUkey));
		NiAction* pkAction = pkKeyboard->GetMappedAction(kKeycode);

		if(pkAction)
		{
			if(!pkKeyboard->KeyIsDown(kKeycode))
			{
				pkKeyboard->RecordKeyPress(kKeycode);
				this->ReportAction(pkKeyboard,pkAction->GetAppData(),0,1,pkAction->GetContext());
			}
		}
	}
}
//패드 버튼이 Release됐을 때 처리
void PgInputSystem::HandleButtonRelease(NiInputGamePad::Button eButton)
{
	if(NiInputGamePad::NIGP_NONE == eButton)
	{
		return;
	}

	//g_kGlobalOption.GetUKeyToKey(1124);

	PgInputKeyboard* pkKeyboard = NiDynamicCast(PgInputKeyboard, m_spKeyboard);
	if(pkKeyboard)
	{
		NiInputKeyboard::KeyCode kKeycode = static_cast<NiInputKeyboard::KeyCode>(g_kGlobalOption.GetUKeyToKey(m_kPadSetting[eButton].m_iUkey));
		NiAction* pkAction = pkKeyboard->GetMappedAction(kKeycode);

		if(pkAction)
		{
			//if(pkKeyboard->KeyIsDown(m_kPadSetting[eButton].m_iUkey))
			if(pkKeyboard->KeyIsDown(kKeycode))
			{
				//pkKeyboard->RecordKeyRelease(m_kPadSetting[eButton].m_iUkey);
				pkKeyboard->RecordKeyRelease(kKeycode);
				this->ReportAction(pkKeyboard,pkAction->GetAppData(),0,0,pkAction->GetContext());
			}
		}
	}
}

//패드 저장을 되돌리기 위해 백업해두는 함수
void PgInputSystem::BackupPadSettingTable()
{
	for(int iIndex = 0 ; NiInputGamePad::NIGP_NUMBUTTONS > iIndex ; ++iIndex)
	{
		m_kTempPadSetting[iIndex] = m_kPadSetting[iIndex];
	}
}

//저장 내용을 되돌림
void PgInputSystem::RestorePaddSettingTable()
{
	for(int iIndex = 0 ; NiInputGamePad::NIGP_NUMBUTTONS > iIndex ; ++iIndex)
	{
		m_kPadSetting[iIndex] = m_kTempPadSetting[iIndex];
	}
}

bool PgInputSystem::GetAxisToButtonPressed(NiInputGamePad::Button eButton)
{
	for (int iIndex = 0; NiInputGamePad::NIGP_NUM_DEFAULT_AXIS*2 > iIndex; ++iIndex)
	{
		if (eAxisToButtons[iIndex] != NiInputGamePad::NIGP_NONE && eAxisToButtons[iIndex] == eButton)
		{
			return bAxisToButtonsPressed[eAxisToButtons[iIndex]];
		}
	}
	return false;
}

//해당 키를 해당 버튼에 바인드시킴
void PgInputSystem::BindButtonPairWithNiKeycode(NiInputGamePad::Button eButton, int iUkey, int iUIButtonIndex)
{
	if((eButton > NiInputGamePad::NIGP_NONE) && (eButton<NiInputGamePad::NIGP_NUMBUTTONS))
	{
		m_kPadSetting[eButton].m_iUkey = iUkey;
		m_kPadSetting[eButton].m_iUIButtonIndex = iUIButtonIndex;
	}
}

//해당 키가 바인드된 UI버튼의 인덱스를 찾아온다. 0이면 없는 것
int PgInputSystem::GetUIButtonIndex(NiInputGamePad::Button eButton)
{
	if((eButton > NiInputGamePad::NIGP_NONE) && (eButton<NiInputGamePad::NIGP_NUMBUTTONS))
	{
		return m_kPadSetting[eButton].m_iUIButtonIndex;
	}
	return 0;
}

//해당 키가 이미 패드에 바인드 되어 있는가 바인드 되어 있으면 바인드 된 버튼을 돌려줌
int PgInputSystem::IsAlreadyBind(NiInputGamePad::Button kPadButton)
{
	if((kPadButton > NiInputGamePad::NIGP_NONE) && (kPadButton<NiInputGamePad::NIGP_NUMBUTTONS))
	{
		if(0 != m_kPadSetting[kPadButton].m_iUkey)
		{
			return m_kPadSetting[kPadButton].m_iUIButtonIndex;
		}
	}
	return 0;
}

//인자로 받은 UI버튼 인덱스를 사용하는 버튼을 리턴한다.
NiInputGamePad::Button PgInputSystem::GetPadButtonRefPressedIndex(int const iUIButtonIndex)
{
	for(int iIndex=NiInputGamePad::NIGP_LUP ; NiInputGamePad::NIGP_NUMBUTTONS > iIndex; ++iIndex)
	{
		if(iUIButtonIndex == m_kPadSetting[iIndex].m_iUIButtonIndex)
		{
			return static_cast<NiInputGamePad::Button>(iIndex);
		}
	}
	return NiInputGamePad::NIGP_NONE;
}

//게임패드 핸들링
void PgInputSystem::HandleGamePad(NiInputGamePad* pkTargetPad)
{
	if(NULL == pkTargetPad)
	{
		return;
	}

	pkTargetPad->StartRumble();

	unsigned int uiAxisCount = pkTargetPad->GetAxisCount();

	int iAxisLow = 0;
	int iAxisHigh = 0;
	float const fAxisThreshold = m_fGamePadAxisSensitivityThreshold;
	pkTargetPad->GetAxisRange(iAxisLow, iAxisHigh);

	for (unsigned int uiAxisToButtonPressed = 0; NiInputGamePad::NIGP_NUMBUTTONS > uiAxisToButtonPressed; ++uiAxisToButtonPressed)
	{
		bAxisToButtonsPressedBefore[uiAxisToButtonPressed] = bAxisToButtonsPressed[uiAxisToButtonPressed];
	}

	for (unsigned int uiAxisIndex = 0; NiInputGamePad::NIGP_NUM_DEFAULT_AXIS > uiAxisIndex; ++uiAxisIndex)
	{
		if (eAxisToButtons[uiAxisIndex*2] == NiInputGamePad::NIGP_NONE)
			continue;

		if (pkTargetPad->GetAxisValue(uiAxisIndex) != 0)
		{
			//_PgOutputDebugString("GamePad Axis %d, value %d\n", uiAxisIndex, pkTargetPad->GetAxisValue(uiAxisIndex));
			if (pkTargetPad->GetAxisValue(uiAxisIndex) > iAxisHigh * fAxisThreshold)
			{
				bAxisToButtonsPressed[eAxisToButtons[uiAxisIndex*2+1]] = true;
			}
			else if (pkTargetPad->GetAxisValue(uiAxisIndex) < iAxisLow * fAxisThreshold)
			{
				bAxisToButtonsPressed[eAxisToButtons[uiAxisIndex*2]] = true;
			}
			else
			{
				bAxisToButtonsPressed[eAxisToButtons[uiAxisIndex*2]] = false;
				bAxisToButtonsPressed[eAxisToButtons[uiAxisIndex*2+1]] = false;
			}
		}
		else
		{
			bAxisToButtonsPressed[eAxisToButtons[uiAxisIndex*2]] = false;
			bAxisToButtonsPressed[eAxisToButtons[uiAxisIndex*2+1]] = false;
		}
	}

	for (unsigned int uiAxisToButton = 0; NiInputGamePad::NIGP_NUM_DEFAULT_AXIS * 2 > uiAxisToButton; ++uiAxisToButton)
	{
		if (eAxisToButtons[uiAxisToButton] != NiInputGamePad::NIGP_NONE)
		{
			if (bAxisToButtonsPressedBefore[eAxisToButtons[uiAxisToButton]] == false && 
				bAxisToButtonsPressed[eAxisToButtons[uiAxisToButton]] == true)
			{
				_PgOutputDebugString("GamePad Axis %d, %d Down\n", uiAxisToButton, eAxisToButtons[uiAxisToButton]);
				HandleButtonDown(eAxisToButtons[uiAxisToButton]);
			}
			else if (bAxisToButtonsPressedBefore[eAxisToButtons[uiAxisToButton]] == true&& 
				bAxisToButtonsPressed[eAxisToButtons[uiAxisToButton]] == false)
			{
				_PgOutputDebugString("GamePad Axis %d, %d Up\n", uiAxisToButton, eAxisToButtons[uiAxisToButton]);
				HandleButtonRelease(eAxisToButtons[uiAxisToButton]);
			}
		}
	}

	//현재 버튼의 눌린 상태를 가져온다.
	unsigned int uiCurrentButtons = pkTargetPad->GetCurrentButtons();

	for(unsigned int uiButtonIndex=0; NiInputGamePad::NIGP_NUMBUTTONS > uiButtonIndex ; ++uiButtonIndex)
	{
		if(pkTargetPad->ButtonIsDown(eButtons[uiButtonIndex]))
		{//버튼이 눌린 상태면
			//pkTargetPad->RecordButtonPress(eButtons[uiButtonIndex]);
			HandleButtonDown(eButtons[uiButtonIndex]);
		}
	}
	for(unsigned int uiButtonIndex=0; NiInputGamePad::NIGP_NUMBUTTONS > uiButtonIndex ; ++uiButtonIndex)
	{
		if(pkTargetPad->ButtonWasReleased(eButtons[uiButtonIndex]))
		{//버튼이 떼어진 상태면
			//pkTargetPad->RecordButtonRelease(eButtons[uiButtonIndex]);
			HandleButtonRelease(eButtons[uiButtonIndex]);
		}
	}
}


NiInputMouse* PgInputSystem::OpenMouse()
{
    // Check if there is already an instance of the mouse.
    if (m_spMouse)
        return m_spMouse;

    // Create the NiInputDI8Mouse instance.
    PgInputMouse* pkMouse = NiNew PgInputMouse();
    if (!pkMouse)
	{
        m_eLastError = NIIERR_DEVICECREATEFAIL;
        NiDelete pkMouse;
        return 0;
    }

    m_spMouse = (NiInputMouse*)pkMouse;
    return m_spMouse;
}

NiInputKeyboard* PgInputSystem::OpenKeyboard()
{
     // Check if there is already an instance of the keyboard.
    if (m_spKeyboard)
        return m_spKeyboard;

     // Create the NiInputDI8Keyboard instance.
    PgInputKeyboard* pkKeyboard = NiNew PgInputKeyboard();
    if (!pkKeyboard)
    {
        m_eLastError = NIIERR_DEVICECREATEFAIL;
        NiDelete pkKeyboard;
        return 0;
    }

    m_spKeyboard = (NiInputKeyboard*)pkKeyboard;

	MapNiActionsToKeyboard();
    return m_spKeyboard;
}

NiInputGamePad* PgInputSystem::OpenGamePad(unsigned int uiPort, unsigned int uiSlot)
{
	if(NULL==m_aspGamePads)
	{
		return NULL;
	}
	if(NULL != m_aspGamePads[uiPort])
	{
		return m_aspGamePads[uiPort];
	}
    /*
	PgInputGamePad* pkGamePad = NiNew PgInputGamePad(uiPort);
	if(!pkGamePad)
	{
		m_eLastError = NIIERR_DEVICECREATEFAIL;
		NiDelete pkGamePad;
		return NULL;
	}

	m_aspGamePads[uiPort] = (NiInputGamePad*)pkGamePad;
	MapNiActionsToGamePads();
	return m_aspGamePads[uiPort];	
	/*/
	if(NULL != m_spInputForGamePad->OpenGamePad(uiPort, uiSlot))
	{
		MapNiActionsToGamePads();
		m_aspGamePads[uiPort] = m_spInputForGamePad->OpenGamePad(uiPort, uiSlot);
		MapNiActionsToGamePads();
		return m_aspGamePads[uiPort];
	}

	m_eLastError = NIIERR_DEVICECREATEFAIL;
	return NULL;
	//*/
}

void PgInputSystem::SetGamePadAxisSensitivityThreshold(float fValue)
{
	if (fValue < 0.0f)
	{
		m_fGamePadAxisSensitivityThreshold = 0.0f;
	}
	else if (fValue > 1.0f)
	{
		m_fGamePadAxisSensitivityThreshold = 1.0f;
	}
	else
	{
		m_fGamePadAxisSensitivityThreshold = fValue;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//	class	PgInputMouse
//////////////////////////////////////////////////////////////////////////////////////////////////
PgInputMouse::PgInputMouse()	:	NiInputMouse(NiNew NiInputDevice::Description(NIID_MOUSE,0,0,""))
{
	m_ptMousePos = NiPoint2::ZERO;
}
void	PgInputMouse::ReleaseAllKey(PgInputSystem *pkInputSystem)
{
	if(ButtonIsDown(NiInputMouse::NIM_LEFT))
	{
		RecordButtonRelease(NiInputMouse::NIM_LEFT);
	//	pkInputSystem->ReportAction(this,NiAction::MOUSE_BUTTON_LEFT,0,0,NULL);
	}
	if(ButtonIsDown(NiInputMouse::NIM_RIGHT))
	{
		RecordButtonRelease(NiInputMouse::NIM_RIGHT);
	//	pkInputSystem->ReportAction(this,NiAction::MOUSE_BUTTON_RIGHT,0,0,NULL);
	}
	if(ButtonIsDown(NiInputMouse::NIM_MIDDLE))
	{
		RecordButtonRelease(NiInputMouse::NIM_MIDDLE);
	//	pkInputSystem->ReportAction(this,NiAction::MOUSE_BUTTON_MIDDLE,0,0,NULL);
	}
}

bool	PgInputMouse::HandleWindowMessage(NiInputSystem* pkInputSystem,unsigned int uiMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uiMsg)
	{
	case WM_LBUTTONDOWN:
		{
			RecordButtonPress(NiInputMouse::NIM_LEFT);
			pkInputSystem->ReportAction(this,NiAction::MOUSE_BUTTON_LEFT, 0, 1, NULL);
			return	true;
		}
		break;
	case WM_LBUTTONUP:
		{
			RecordButtonRelease(NiInputMouse::NIM_LEFT);
			pkInputSystem->ReportAction(this,NiAction::MOUSE_BUTTON_LEFT, 0, 0, NULL);
			return	true;
		}
		break;
	case WM_RBUTTONDOWN:
		{
			RecordButtonPress(NiInputMouse::NIM_RIGHT);
			pkInputSystem->ReportAction(this,NiAction::MOUSE_BUTTON_RIGHT, 0, 1, NULL);
			return	true;
		}
		break;
	case WM_RBUTTONUP:
		{
			RecordButtonRelease(NiInputMouse::NIM_RIGHT);
			pkInputSystem->ReportAction(this,NiAction::MOUSE_BUTTON_RIGHT, 0, 0, NULL);
			return	true;
		}
		break;
	case WM_MBUTTONDOWN:
		{
			RecordButtonPress(NiInputMouse::NIM_MIDDLE);
			pkInputSystem->ReportAction(this,NiAction::MOUSE_BUTTON_MIDDLE, 0, 1, NULL);
			return	true;
		}
		break;
	case WM_MBUTTONUP:
		{
			RecordButtonRelease(NiInputMouse::NIM_MIDDLE);
			pkInputSystem->ReportAction(this,NiAction::MOUSE_BUTTON_MIDDLE, 0, 0, NULL);
			return	true;
		}
		break;
	case WM_MOUSEMOVE:
		{
			int ixPos = GET_X_LPARAM(lParam); 
			int iyPos = GET_Y_LPARAM(lParam); 
			if (ixPos==m_ptMousePos.x && iyPos==m_ptMousePos.y)
			{
				break;
			}

			if (g_pkApp->GetUseSmallWindow())
			{
				float fxPos = ixPos * g_pkApp->GetWindowXScale();
				float fyPos = iyPos * g_pkApp->GetWindowYScale();

				ixPos = (int)fxPos;
				iyPos = (int)fyPos;
				//_PgOutputDebugString("=========MouseMove (%d, %d), (%f, %f), (%d, %d)\n", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), fxPos, fyPos, ixPos, iyPos);
			}

			if(m_ptMousePos != NiPoint2::ZERO)
			{
				pkInputSystem->ReportAction(this, NiAction::MOUSE_AXIS_X, 0, ixPos, NULL);
				pkInputSystem->ReportAction(this, NiAction::MOUSE_AXIS_Y, 0, iyPos, NULL);
			}

			m_ptMousePos = NiPoint2((float)ixPos,(float)iyPos);
			return	true;
		}
		break;
	case WM_MOUSEWHEEL:
		{
			int	const izDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			pkInputSystem->ReportAction(this, NiAction::MOUSE_AXIS_Z, 0, izDelta, NULL);
			return	true;
		}
		break;
	}
	return	false;
}

NiInputErr PgInputMouse::UpdateActionMappedDevice(NiInputSystem* pkInputSystem)
{
	return	NIIERR_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//	class	PgInputKeyboard
//////////////////////////////////////////////////////////////////////////////////////////////////
PgInputKeyboard::PgInputKeyboard()	
	:	NiInputKeyboard(NiNew NiInputDevice::Description(NIID_KEYBOARD,1,0,""))
{

	memset(m_aeWinKeyToNIIMapping,0,sizeof(m_aeWinKeyToNIIMapping));

    m_aeWinKeyToNIIMapping[VK_ESCAPE] = KEY_ESCAPE;
    m_aeWinKeyToNIIMapping['1'] = KEY_1;
    m_aeWinKeyToNIIMapping['2'] = KEY_2;
    m_aeWinKeyToNIIMapping['3'] = KEY_3;
    m_aeWinKeyToNIIMapping['4'] = KEY_4;
    m_aeWinKeyToNIIMapping['5'] = KEY_5;
    m_aeWinKeyToNIIMapping['6'] = KEY_6;
    m_aeWinKeyToNIIMapping['7'] = KEY_7;
    m_aeWinKeyToNIIMapping['8'] = KEY_8;
    m_aeWinKeyToNIIMapping['9'] = KEY_9;
    m_aeWinKeyToNIIMapping['0'] = KEY_0;
    m_aeWinKeyToNIIMapping[VK_OEM_MINUS] = KEY_MINUS;
    m_aeWinKeyToNIIMapping[VK_OEM_PLUS] = KEY_EQUALS;
    m_aeWinKeyToNIIMapping[VK_BACK] = KEY_BACK;
    m_aeWinKeyToNIIMapping[VK_TAB] = KEY_TAB;
    m_aeWinKeyToNIIMapping['Q'] = KEY_Q;
    m_aeWinKeyToNIIMapping['W'] = KEY_W;
    m_aeWinKeyToNIIMapping['E'] = KEY_E;
    m_aeWinKeyToNIIMapping['R'] = KEY_R;
    m_aeWinKeyToNIIMapping['T'] = KEY_T;
    m_aeWinKeyToNIIMapping['Y'] = KEY_Y;
    m_aeWinKeyToNIIMapping['U'] = KEY_U;
    m_aeWinKeyToNIIMapping['I'] = KEY_I;
    m_aeWinKeyToNIIMapping['O'] = KEY_O;
	m_aeWinKeyToNIIMapping['P'] = KEY_P;
	m_aeWinKeyToNIIMapping[VK_OEM_4] = KEY_LBRACKET;
	m_aeWinKeyToNIIMapping[VK_OEM_6] = KEY_RBRACKET;
    m_aeWinKeyToNIIMapping[VK_RETURN] = KEY_RETURN;
    m_aeWinKeyToNIIMapping[VK_LCONTROL] = KEY_LCONTROL;
    m_aeWinKeyToNIIMapping['A'] = KEY_A;
    m_aeWinKeyToNIIMapping['S'] = KEY_S;
    m_aeWinKeyToNIIMapping['D'] = KEY_D;
    m_aeWinKeyToNIIMapping['F'] = KEY_F;
    m_aeWinKeyToNIIMapping['G'] = KEY_G;
    m_aeWinKeyToNIIMapping['H'] = KEY_H;
    m_aeWinKeyToNIIMapping['J'] = KEY_J;
    m_aeWinKeyToNIIMapping['K'] = KEY_K;
    m_aeWinKeyToNIIMapping['L'] = KEY_L;
    m_aeWinKeyToNIIMapping[VK_OEM_1] = KEY_SEMICOLON;
    m_aeWinKeyToNIIMapping[VK_OEM_7] = KEY_APOSTROPHE;
    m_aeWinKeyToNIIMapping[VK_OEM_3] = KEY_GRAVE;
    m_aeWinKeyToNIIMapping[VK_LSHIFT] = KEY_LSHIFT;
    m_aeWinKeyToNIIMapping[VK_OEM_5] = KEY_BACKSLASH;
    m_aeWinKeyToNIIMapping['Z'] = KEY_Z;
    m_aeWinKeyToNIIMapping['X'] = KEY_X;
    m_aeWinKeyToNIIMapping['C'] = KEY_C;
    m_aeWinKeyToNIIMapping['V'] = KEY_V;
    m_aeWinKeyToNIIMapping['B'] = KEY_B;
    m_aeWinKeyToNIIMapping['N'] = KEY_N;
    m_aeWinKeyToNIIMapping['M'] = KEY_M;
    m_aeWinKeyToNIIMapping[VK_OEM_COMMA] = KEY_COMMA;
	m_aeWinKeyToNIIMapping[VK_OEM_PERIOD] = KEY_PERIOD;
    m_aeWinKeyToNIIMapping[VK_OEM_2] = KEY_SLASH;
    m_aeWinKeyToNIIMapping[VK_RSHIFT] = KEY_RSHIFT;
    m_aeWinKeyToNIIMapping[VK_MULTIPLY] = KEY_MULTIPLY;
    m_aeWinKeyToNIIMapping[VK_LMENU] = KEY_LMENU;
    m_aeWinKeyToNIIMapping[VK_SPACE] = KEY_SPACE;
    m_aeWinKeyToNIIMapping[VK_CAPITAL] = KEY_CAPITAL;
    m_aeWinKeyToNIIMapping[VK_F1] = KEY_F1;
    m_aeWinKeyToNIIMapping[VK_F2] = KEY_F2;
    m_aeWinKeyToNIIMapping[VK_F3] = KEY_F3;
    m_aeWinKeyToNIIMapping[VK_F4] = KEY_F4;
    m_aeWinKeyToNIIMapping[VK_F5] = KEY_F5;
    m_aeWinKeyToNIIMapping[VK_F6] = KEY_F6;
    m_aeWinKeyToNIIMapping[VK_F7] = KEY_F7;
    m_aeWinKeyToNIIMapping[VK_F8] = KEY_F8;
    m_aeWinKeyToNIIMapping[VK_F9] = KEY_F9;
    m_aeWinKeyToNIIMapping[VK_F10] = KEY_F10;
    m_aeWinKeyToNIIMapping[VK_NUMLOCK] = KEY_NUMLOCK;
    m_aeWinKeyToNIIMapping[VK_SCROLL] = KEY_SCROLL;
    m_aeWinKeyToNIIMapping[VK_NUMPAD7] = KEY_NUMPAD7;
    m_aeWinKeyToNIIMapping[VK_NUMPAD8] = KEY_NUMPAD8;
    m_aeWinKeyToNIIMapping[VK_NUMPAD9] = KEY_NUMPAD9;
    m_aeWinKeyToNIIMapping[VK_SUBTRACT] = KEY_SUBTRACT;
    m_aeWinKeyToNIIMapping[VK_NUMPAD4] = KEY_NUMPAD4;
    m_aeWinKeyToNIIMapping[VK_NUMPAD5] = KEY_NUMPAD5;
    m_aeWinKeyToNIIMapping[VK_NUMPAD6] = KEY_NUMPAD6;
    m_aeWinKeyToNIIMapping[VK_ADD] = KEY_ADD;
    m_aeWinKeyToNIIMapping[VK_NUMPAD1] = KEY_NUMPAD1;
    m_aeWinKeyToNIIMapping[VK_NUMPAD2] = KEY_NUMPAD2;
    m_aeWinKeyToNIIMapping[VK_NUMPAD3] = KEY_NUMPAD3;
    m_aeWinKeyToNIIMapping[VK_NUMPAD0] = KEY_NUMPAD0;
    m_aeWinKeyToNIIMapping[VK_DECIMAL] = KEY_DECIMAL;
    m_aeWinKeyToNIIMapping[VK_OEM_102] = KEY_OEM_102;
    m_aeWinKeyToNIIMapping[VK_F11] = KEY_F11;
    m_aeWinKeyToNIIMapping[VK_F12] = KEY_F12;
    m_aeWinKeyToNIIMapping[VK_F13] = KEY_F13;
    m_aeWinKeyToNIIMapping[VK_F14] = KEY_F14;
    m_aeWinKeyToNIIMapping[VK_F15] = KEY_F15;
    m_aeWinKeyToNIIMapping[VK_KANA] = KEY_KANA;
    //m_aeWinKeyToNIIMapping[VK_ABNT_C1] = KEY_ABNT_C1;
    m_aeWinKeyToNIIMapping[VK_CONVERT] = KEY_CONVERT;
    m_aeWinKeyToNIIMapping[VK_NONCONVERT] = KEY_NOCONVERT;
   // m_aeWinKeyToNIIMapping[VK_YEN] = KEY_YEN;
  //  m_aeWinKeyToNIIMapping[VK_ABNT_C2] = KEY_ABNT_C2;
   // m_aeWinKeyToNIIMapping[VK_NUMPADEQUALS] = KEY_NUMPADEQUALS;
 //   m_aeWinKeyToNIIMapping[VK_PREVTRACK] = KEY_PREVTRACK;
  //  m_aeWinKeyToNIIMapping[VK_AT] = KEY_AT;
  //  m_aeWinKeyToNIIMapping[VK_COLON] = KEY_COLON;
  //  m_aeWinKeyToNIIMapping[VK_UNDERLINE] = KEY_UNDERLINE;
	m_aeWinKeyToNIIMapping[VK_KANJI] = KEY_KANJI;
  //  m_aeWinKeyToNIIMapping[VK_STOP] = KEY_STOP;
  //  m_aeWinKeyToNIIMapping[VK_AX] = KEY_AX;
 //   m_aeWinKeyToNIIMapping[VK_UNLABELED] = KEY_UNLABELED;
 //   m_aeWinKeyToNIIMapping[VK_NEXTTRACK] = KEY_NEXTTRACK;
 //   m_aeWinKeyToNIIMapping[VK_NUMPADENTER] = KEY_NUMPADENTER;
    m_aeWinKeyToNIIMapping[VK_RCONTROL] = KEY_RCONTROL;
 //   m_aeWinKeyToNIIMapping[VK_MUTE] = KEY_MUTE;
 //   m_aeWinKeyToNIIMapping[VK_CALCULATOR] = KEY_CALCULATOR;
 //   m_aeWinKeyToNIIMapping[VK_PLAYPAUSE] = KEY_PLAYPAUSE;
//    m_aeWinKeyToNIIMapping[VK_MEDIASTOP] = KEY_MEDIASTOP;
//    m_aeWinKeyToNIIMapping[VK_VOLUMEDOWN] = KEY_VOLUMEDOWN;
//    m_aeWinKeyToNIIMapping[VK_VOLUMEUP] = KEY_VOLUMEUP;
//    m_aeWinKeyToNIIMapping[VK_WEBHOME] = KEY_WEBHOME;
//    m_aeWinKeyToNIIMapping[VK_NUMPADCOMMA] = KEY_NUMPADCOMMA;
    m_aeWinKeyToNIIMapping[VK_DIVIDE] = KEY_DIVIDE;
//    m_aeWinKeyToNIIMapping[VK_SYSRQ] = KEY_SYSRQ;
    m_aeWinKeyToNIIMapping[VK_RMENU] = KEY_RMENU;
    m_aeWinKeyToNIIMapping[VK_PAUSE] = KEY_PAUSE;
    m_aeWinKeyToNIIMapping[VK_HOME] = KEY_HOME;
    m_aeWinKeyToNIIMapping[VK_UP] = KEY_UP;
    m_aeWinKeyToNIIMapping[VK_PRIOR] = KEY_PRIOR;
    m_aeWinKeyToNIIMapping[VK_LEFT] = KEY_LEFT;
    m_aeWinKeyToNIIMapping[VK_RIGHT] = KEY_RIGHT;
    m_aeWinKeyToNIIMapping[VK_END] = KEY_END;
    m_aeWinKeyToNIIMapping[VK_DOWN] = KEY_DOWN;
    m_aeWinKeyToNIIMapping[VK_NEXT] = KEY_NEXT;
    m_aeWinKeyToNIIMapping[VK_INSERT] = KEY_INSERT;
    m_aeWinKeyToNIIMapping[VK_DELETE] = KEY_DELETE;
    m_aeWinKeyToNIIMapping[VK_LWIN] = KEY_LWIN;
    m_aeWinKeyToNIIMapping[VK_RWIN] = KEY_RWIN;
    m_aeWinKeyToNIIMapping[VK_APPS] = KEY_APPS;
//    m_aeWinKeyToNIIMapping[VK_POWER] = KEY_POWER;
    m_aeWinKeyToNIIMapping[VK_SLEEP] = KEY_SLEEP;
 //   m_aeWinKeyToNIIMapping[VK_WAKE] = KEY_WAKE;
 //   m_aeWinKeyToNIIMapping[VK_WEBSEARCH] = KEY_WEBSEARCH;
 //   m_aeWinKeyToNIIMapping[VK_WEBFAVORITES] = KEY_WEBFAVORITES;
//    m_aeWinKeyToNIIMapping[VK_WEBREFRESH] = KEY_WEBREFRESH;
 //   m_aeWinKeyToNIIMapping[VK_WEBSTOP] = KEY_WEBSTOP;
//    m_aeWinKeyToNIIMapping[VK_WEBFORWARD] = KEY_WEBFORWARD;
 //   m_aeWinKeyToNIIMapping[VK_WEBBACK] = KEY_WEBBACK;
//    m_aeWinKeyToNIIMapping[VK_MYCOMPUTER] = KEY_MYCOMPUTER;
 //   m_aeWinKeyToNIIMapping[VK_MAIL] = KEY_MAIL;
//    m_aeWinKeyToNIIMapping[VK_MEDIASELECT] = KEY_MEDIASELECT;
}
DWORD	PgInputKeyboard::GetWinKeyFromNIKey(KeyCode kNiKeyCode)	const
{
	for(int i=0;i<MAX_WIN_KEY_NUM;i++)
	{
		if(m_aeWinKeyToNIIMapping[i] == kNiKeyCode)
		{
			return	i;
		}
	}

	return	0;
}

NiInputKeyboard::KeyCode PgInputKeyboard::GetNiKey(int iVirtualKey) const
{
	if(iVirtualKey<0 || iVirtualKey>=MAX_WIN_KEY_NUM)
	{
		return NiInputKeyboard::KEY_NOKEY;
	}

	return m_aeWinKeyToNIIMapping[iVirtualKey];
}

bool	PgInputKeyboard::HandleKeyDown(NiInputSystem* pkInputSystem,unsigned int uiKeyID)
{
	if(uiKeyID >=MAX_WIN_KEY_NUM )
	{
		return	false;
	}

	NiAction* pkAction = GetMappedAction(m_aeWinKeyToNIIMapping[uiKeyID]);

	if(pkAction)
	{
		if(KeyIsDown(m_aeWinKeyToNIIMapping[uiKeyID]) == false)
		{

			RecordKeyPress(m_aeWinKeyToNIIMapping[uiKeyID]);
			pkInputSystem->ReportAction(this,pkAction->GetAppData(),0,1,pkAction->GetContext());
			return	true;
		}
	}
	return	false;
}

bool	PgInputKeyboard::HandleKeyUp(NiInputSystem* pkInputSystem,unsigned int uiKeyID)
{
	if(uiKeyID >=MAX_WIN_KEY_NUM )
	{
		return	false;
	}

	NiAction* pkAction = GetMappedAction(m_aeWinKeyToNIIMapping[uiKeyID]);

	if(pkAction)
	{
		if(KeyIsDown(m_aeWinKeyToNIIMapping[uiKeyID]))
		{
			RecordKeyRelease(m_aeWinKeyToNIIMapping[uiKeyID]);
			pkInputSystem->ReportAction(this,pkAction->GetAppData(),0,0,pkAction->GetContext());
			return	true;
		}
	}
	return	false;
}

bool	PgInputKeyboard::HandleWindowMessage(NiInputSystem* pkInputSystem,unsigned int uiMsg,WPARAM wParam,LPARAM lParam)
{
	BYTE	byScanCode = (BYTE)((lParam&0xff0000)>>16);
	unsigned	int	iVirtualKey = GetVirtualKeyByScanCode(byScanCode,wParam);

	switch(uiMsg)
	{
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		{

			switch(wParam)
			{
			case VK_F4:
				return	false;
			}

		}
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			((GetAsyncKeyState(VK_LMENU)&0x8000)!=0) ? HandleKeyDown(pkInputSystem,VK_LMENU) : HandleKeyUp(pkInputSystem,VK_LMENU);
			((GetAsyncKeyState(VK_RMENU)&0x8000)!=0) ? HandleKeyDown(pkInputSystem,VK_RMENU) : HandleKeyUp(pkInputSystem,VK_RMENU);
			((GetAsyncKeyState(VK_LSHIFT)&0x8000)!=0) ? HandleKeyDown(pkInputSystem,VK_LSHIFT) : HandleKeyUp(pkInputSystem,VK_LSHIFT);
			((GetAsyncKeyState(VK_RSHIFT)&0x8000)!=0) ? HandleKeyDown(pkInputSystem,VK_RSHIFT) : HandleKeyUp(pkInputSystem,VK_RSHIFT);
			((GetAsyncKeyState(VK_LCONTROL)&0x8000)!=0) ? HandleKeyDown(pkInputSystem,VK_LCONTROL) : HandleKeyUp(pkInputSystem,VK_LCONTROL);
			((GetAsyncKeyState(VK_RCONTROL)&0x8000)!=0) ? HandleKeyDown(pkInputSystem,VK_RCONTROL) : HandleKeyUp(pkInputSystem,VK_RCONTROL);
		}
		break;
	}

	switch(uiMsg)
	{
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			return	HandleKeyDown(pkInputSystem,iVirtualKey);
		}break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		{
			switch(wParam)
			{
			case VK_MULTIPLY:
			case VK_SNAPSHOT:
				{
					return false;
				}
			}

			return	HandleKeyUp(pkInputSystem,iVirtualKey);
		}break;
	}
	return	false;
}
void	PgInputKeyboard::ReleaseAllKey(PgInputSystem *pkInputSystem)
{
	for(int i=0;i<MAX_WIN_KEY_NUM;i++)
	{
		HandleKeyUp(pkInputSystem,i);
	}
}
NiInputErr PgInputKeyboard::UpdateActionMappedDevice(NiInputSystem* pkInputSystem)
{
	return	NIIERR_OK;
}
NiAction* PgInputKeyboard::GetMappedAction(unsigned long uiKeyScanCode)
{
    if (!m_pkMappedActions || m_pkMappedActions->IsEmpty())
        return NULL;

    NiTListIterator kPos = m_pkMappedActions->GetHeadPos();

	while(kPos)
	{
        NiAction *pkAction = m_pkMappedActions->GetNext(kPos);
        if (!pkAction)
            continue;

		if(pkAction->GetControl() == uiKeyScanCode)
		{
			return	pkAction;
		}
	}

	return	NULL;
}
unsigned	int	PgInputKeyboard::GetVirtualKeyByScanCode(BYTE byScanCode,unsigned int iDefaultKeyID)
{
	if(byScanCode>=88)
	{
		return	iDefaultKeyID;
	}

	if(iDefaultKeyID>=VK_NUMPAD0 && iDefaultKeyID<=VK_DIVIDE)
	{
		return	iDefaultKeyID;
	}

	static const int iKeyID[]=
	{VK_ESCAPE,'1','2','3','4','5','6','7','8','9','0',-1,-1,VK_BACK,VK_TAB,'Q','W','E','R','T','Y','U','I','O','P',-1,-1,VK_RETURN,-1,
	'A','S','D','F','G','H','J','K','L',-1,-1,-1,-1,-1,'Z','X','C','V','B','N','M',-1,-1,-1,-1,-1,-1,VK_SPACE,-1,VK_F1,VK_F2,VK_F3,
	VK_F4,VK_F5,VK_F6,VK_F7,VK_F8,VK_F9,VK_F10,-1,-1,VK_HOME,VK_UP,VK_PRIOR,-1,VK_LEFT,VK_CLEAR,VK_RIGHT,
	-1,VK_END,VK_DOWN,VK_NEXT,VK_INSERT,VK_DELETE,-1,-1,-1,VK_F11,VK_F12};
	if(LOCAL_MGR::NC_FRANCE == g_kLocal.ServiceRegion())
	{// 프랑스 키보드의 , 키는 
		if(50 == byScanCode) 
		{// 디폴트 키로 사용한다
			return iDefaultKeyID;
		}
	}
	if(iKeyID[byScanCode-1]==-1) return iDefaultKeyID;
	return	iKeyID[byScanCode-1];
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//	class	PgInputGamePad
//////////////////////////////////////////////////////////////////////////////////////////////////
PgInputGamePad::PgInputGamePad(unsigned int uiPort)
: NiInputGamePad(NiNew NiInputDevice::Description(NIID_GAMEPAD,uiPort,0,""), 0, 0)
{
	unsigned int uiMaxButtonCount = this->NiInputGamePad::GetMaxButtonCount();
	memset(m_aePadButtonToNIIMapping, 0, sizeof(m_aePadButtonToNIIMapping));
}

NiAction* PgInputGamePad::GetMappedAction(unsigned long uiKeyScanCode)
{
    if (!m_pkMappedActions || m_pkMappedActions->IsEmpty())
        return NULL;

    NiTListIterator kPos = m_pkMappedActions->GetHeadPos();

	while(kPos)
	{
        NiAction *pkAction = m_pkMappedActions->GetNext(kPos);
        if (!pkAction)
            continue;

		if(pkAction->GetControl() == uiKeyScanCode)
		{
			return	pkAction;
		}
	}

	return	NULL;
}

bool PgInputGamePad::HandleGamePad()
{
	return false;
}

void PgInputGamePad::ConfigButtons()
{
}

unsigned int PgInputGamePad::GetMotorCount() const
{
	return 0;
}

// The maximum allowable value for the given motor.
unsigned int PgInputGamePad::GetRumbleRange(unsigned int uiMotor) const
{
	return 0;
}

// uiValue in [0..value returned by GetRumbleRange]
// If bCommit is true, the value and all uncommited values for other 
// motors will be set on the device.
void PgInputGamePad::SetRumbleValue(unsigned int uiMotor, unsigned int uiValue,	bool bCommit)
{
}

// Access the current value for a motor, even if that value is uncommited.
void PgInputGamePad::GetRumbleValue(unsigned int uiMotor, unsigned int& uiValue) const
{
}

// Starts rumble - commits current values
void PgInputGamePad::StartRumble()
{
}

// Halts rumble
void PgInputGamePad::StopRumble(bool bQuickStop)
{
}