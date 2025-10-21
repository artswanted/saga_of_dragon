// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#ifndef _WIN32_WINNT		// Windows XP 이상에서만 기능을 사용할 수 있습니다.                   
#define _WIN32_WINNT 0x0501	// 다른 버전의 Windows에 맞도록 적합한 값으로 변경해 주십시오.
#endif						

#include <stdio.h>
#include <tchar.h>

#define NOMINMAX

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include "BM/BM.h"
#include "BM/FileSupport.h"

//// Imports gamebryo libs

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "NiSystem.lib")
#pragma comment(lib, "NiMain.lib")
#pragma comment(lib, "NiInput.lib")
#pragma comment(lib, "NiAnimation.lib")
#pragma comment(lib, "NiCollision.lib")
#pragma comment(lib, "NiBinaryShaderLibDX9.lib")
#pragma comment(lib, "NSBShaderLibDX9.lib")
#pragma comment(lib, "NSFParserLibDX9.lib")
#pragma comment(lib, "NiD3DXEffectShaderLibDX9.lib")
#pragma comment(lib, "NiFont.lib")
#pragma comment(lib, "NiParticle.lib")
#pragma comment(lib, "NiMilesAudio.lib")
#pragma comment(lib, "NiUserInterface.lib")
#pragma comment(lib, "mss32.lib")
#pragma comment(lib, "NiEntity.lib")


//#include <NiLog.h>
//#include <NiDI8InputSystem.h>
//#include <NiAnimation.h>
//#include <NiCollision.h>
//#include <NiDirectXVersion.h>
//#include <NiDX9Renderer.h>
//#include <NiDX9SystemDesc.h>
//#include <NiFont.h>
//#include <Ni2DString.h>
//#include <NiParticle.h>
//#include <NiAudio.h>
//#include <NiMilesAudio.h>
//#include <NiD3DShaderFactory.h>
//#include <NiLight.h>
//#include <NiBillboardNode.h>
//#include <NiStringsExtraData.h>
//#include <NiPick.h>
//#include <NiLoopedThread.h>
//#include <NiEntity.h>
//#include <NiFactories.h>


//// PhysX
//
#pragma comment(lib, "NiPhysX.lib")
#pragma comment(lib, "NxCooking.lib")
#pragma comment(lib, "NxCharacter.lib")
#pragma comment(lib, "PhysXLoader.lib")
#pragma comment(lib, "NiPhysXParticle.lib")

#include <ControllerManager.h> 
#include <NxCapsuleController.h> 
#include <NiPhysX.h>
#include <NxCooking.h>
#include <NiPhysXParticle.h>
