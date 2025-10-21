#ifndef CONSENT_CONSENTSERVER_LINKAGE_NC_ADMINGATEWAY_H
#define CONSENT_CONSENTSERVER_LINKAGE_NC_ADMINGATEWAY_H

#ifdef ADMINGATEWAY_EXPORTS
#define ADMINGATEWAYDLL_API __declspec(dllexport)
#else
#define ADMINGATEWAYDLL_API __declspec(dllimport)
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#pragma comment (lib, "NC/AdminGateway.lib")

typedef unsigned short wchar_old;

class ADMINGATEWAYDLL_API CAdminGateway 
{
public:
	CAdminGateway(void);
};

ADMINGATEWAYDLL_API unsigned int Init(void(*execute)(const wchar_old* req, wchar_old* res));
ADMINGATEWAYDLL_API void Unload();

#endif // CONSENT_CONSENTSERVER_LINKAGE_NC_ADMINGATEWAY_H