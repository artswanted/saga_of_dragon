#pragma once 

#define	DEF_XML_GROUP(id)	const char* const XML_GROUP_##id	= #id;
#define	DEF_XML_ELEMENT(id)	const char* const XML_ELEMENT_##id	= #id;
#define	DEF_XML_ATTR(id)	const char* const XML_ATTR_##id		= #id;

#define	DEF_S_G_STRING(name, value)	const char* const g_sz##name = value;

//	그룹
DEF_XML_GROUP(CONFIG)

//	요소
DEF_XML_ELEMENT(CAMERA)
DEF_XML_ELEMENT(ETC)
DEF_XML_ELEMENT(GAME)
DEF_XML_ELEMENT(GRAPHIC1)
DEF_XML_ELEMENT(GRAPHIC2)
DEF_XML_ELEMENT(GRAPHIC3)
DEF_XML_ELEMENT(KEYSET)
DEF_XML_ELEMENT(OPTION)
DEF_XML_ELEMENT(SOUND)
DEF_XML_ELEMENT(USER_UI)
DEF_XML_ELEMENT(VERSION)
DEF_XML_ELEMENT(ITEM)

//	속성
DEF_XML_ATTR(NAME)
DEF_XML_ATTR(VALUE)

//	변수다
DEF_S_G_STRING(Keyset, "%d")
DEF_S_G_STRING(ConfigFileName, "./release/config.xml")
DEF_S_G_STRING(DefaultConfigFileName, "defaultconfig.xml")

//	상수다
typedef	enum eConfigOption
{
	/*- 화면 모드 -*/
	SET_SCREEN_MODE = 0x00000001,

	/*- 그래픽 모드 -*/
	SET_GRAPHIC_MODE,

	/*- 사운드 모드 -*/
	SET_SOUND_BGM_MUTE,
	SET_SOUND_SE_MUTE,		
	SET_SOUND_BGM_VOLUME,	
	SET_SOUND_SE_VOLUME,	
}EConfigOption;
