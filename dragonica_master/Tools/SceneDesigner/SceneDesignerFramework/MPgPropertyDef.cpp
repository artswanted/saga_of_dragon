//
// HandOver, 강정욱 2008.01.29
//
// 전체적으로 사용하는 Define된 String을 입력 하는 곳입니다.
//
#include "SceneDesignerFrameworkPCH.h"
#include "MPgPropertyDef.h"

// PGProperty 에 대한 추가는
// 여기 아래 스트링, 카운트 추가와
// MEntity.h 헤더파일에 Enum에 추가를 해주기만 하면 적용이 됩니다.
const char* gs_pcPGProperty[] =
	{
		"Object",			// 평범 물체
		"MainCamera",		// 메인 카메라
        "CharacterSpawn",	// 캐릭터 스폰 C
		"Path",				// 카메라가 따라가는 패스 C
		"PhysX",			// 피직스 C
        "Trigger",			// 트리거 C
		"MinimapCamera",	// 미니맵 카메라
		"CameraWalls",		// 카메라가 넘지 못하는 벽. C
		"Ladder",			// 사다리 C
		"Rope",				// 로프 C
		"SkyBox",			// 스카이 박스
		"BaseObject",		// 통짜 맵 하나.
		"Water",			// 물
        "CollisionObject",	// 현재 쓰지 않음
        "Dummy",			// 현재 쓰지 않음
        "Puppet",			// 현재 쓰지 않음
		"PermissionArea",   // 제한 구역
		"Trap",
		"Telejump",			// 대점프 도착지역
		"PS_ROOM",			// 포탈 시스템 - 방
		"PS_PORTAL",			// 포탈 시스템 - 포탈
	};
int gs_iPGPropertyCount = 21;

const char* gs_pcPGAlphaGroup[] =
	{
		"-5",
		"-4",
		"-3",
		"-2",
		"-1",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
	};
int gs_iPGAlphaGroupCount = 11;

const char* gs_pcPGOptimization[] =
	{
		"0",
		"1",
		"2",
	};
int gs_iPGOptimizationCount = 3;


// Component관련
const char* gs_pcPropAnimationObject	= "Animation Object";
const char* gs_pcPropPhase				= "Phase";

const char* gs_pcPropSceneGraph			= "Scene Graph";
const char* gs_pcNIFFilePath			= "NIF File Path";

const char* gs_pcHideObject				= "Hide Object";