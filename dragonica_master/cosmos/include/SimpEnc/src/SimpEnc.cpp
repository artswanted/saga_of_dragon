#include "StdAfx.h"
#include <windows.h>
#include <string>
#include <vector>
#include "SimpEnc.h"

namespace BM
{
	bool SimpleEncrypt(const std::vector<char>& In, std::vector<char>& Out)
	{	
		if(!In.size()) { return	false; }

		char*	pInBuf = new char[ In.size() ];

		memcpy(pInBuf, &In.at(0), In.size());

		char*	pUint = pInBuf;
		for(int i = 0; i < (int)In.size(); i++)
		{
			*pUint++ ^= i1Enc[ i % MAX_VAL ];
		}

		pUint = pInBuf;
		for(int i = 0; i < (int)In.size(); i++)
		{
			*pUint++ ^= i2Enc[ i % HALF_VAL ];				
		}

		Out.resize(In.size());
		memcpy(&Out.at(0), pInBuf, In.size());

		delete [] pInBuf;

		return	true;
	}

	bool SimpleDecrypt(const std::vector<char>& In, std::vector<char>& Out)
	{
		if(!In.size()) { return	false; }

		char*	pInBuf = new char[ In.size() ];

		memcpy(pInBuf, &In.at(0), In.size());

		char*	pUint = pInBuf;
		for(int i = 0; i < (int)In.size(); i++)
		{
			*pUint++ ^= i2Enc[ i % HALF_VAL ];				
		}

		pUint = pInBuf;
		for(int i = 0; i < (int)In.size(); i++)
		{
			*pUint++ ^= i1Enc[ i % MAX_VAL ];
		}

		Out.resize(In.size());
		memcpy(&Out.at(0), pInBuf, In.size());

		delete [] pInBuf;

		return	true;
	}
}