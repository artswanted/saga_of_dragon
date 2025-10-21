#pragma once

namespace	BM
{
	const int	MAX_VAL		= 8;
	const int	HALF_VAL	= 4;
	const unsigned char i1Enc[ MAX_VAL ] = { 255, 127, 124, 53, 12, 180, 191, 128 };
	const unsigned char i2Enc[ HALF_VAL ] = { 1, 56, 128, 244 };

	bool SimpleEncrypt(const std::vector<char>& In, std::vector<char>& Out);
	bool SimpleDecrypt(const std::vector<char>& In, std::vector<char>& Out);
}