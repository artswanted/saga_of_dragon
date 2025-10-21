#pragma once
#include "defines.h"

class PgEnvInfoParser
{
public:
	bool ParseFromPack(std::wstring const& kFile, SEnvData& kData_out);

private:
	bool Parse(TiXmlNode const* pkNode, SEnvData& kData_out);	
	DWORD RGBtoBGR(DWORD const& rdwRGB);
//	void SetLocaleAccordingToXMLEncode(std::string const kEncode);
};