#ifndef CENTER_CENTERSERVER_MAINFRAME_CONSTANT_H
#define CENTER_CENTERSERVER_MAINFRAME_CONSTANT_H

typedef enum
{
	EVAR_KIND_NONE = 0,
	EVAR_KIND_PVP = 1,
	EVar_Kind_GM_Emoticon = 12,
	EVar_Kind_Premium_Service = 19,
} EVAR_KIND;


typedef enum 
{
	EVAR_PVP_NONE = 0,
	EVAR_PVP_CREATEROOM_GMLEVEL = 1,
} EVAR_PVP;

typedef enum
{
	EVar_GM_Emoticon_Default = 1,
} EVAR_GM_EMOTICON;

typedef enum
{
	EVar_VIP_Emoticon = 1,
}EVAR_PREMIUM_SERVICE;

#endif // CENTER_CENTERSERVER_MAINFRAME_CONSTANT_H