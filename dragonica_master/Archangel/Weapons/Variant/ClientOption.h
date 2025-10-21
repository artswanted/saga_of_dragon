#ifndef WEAPON_VARIANT_OPTION_CLIENTOPTION_H
#define WEAPON_VARIANT_OPTION_CLIENTOPTION_H

#pragma pack(1)

size_t const OPTION_MAX_KEYSET_SIZE = 166;
size_t const OPTION_MAX_JOYSTICK_SIZE = 32;
size_t const OPTION_EMPTY = 302;
size_t const OPTION_RESERVED = 14;
typedef struct {
	short sVersion;								// 2	2
	BYTE byKeySet[OPTION_MAX_KEYSET_SIZE];		// 168	166
	BYTE byJoystick[OPTION_MAX_JOYSTICK_SIZE];	// 200	32
	BYTE byEmpty[OPTION_EMPTY];					// 502	302
	DWORD dwShowOption;							// 506	4
	BYTE byReserved[OPTION_RESERVED];			// 520	14
} ClientOption;

#pragma pack()

#endif // WEAPON_VARIANT_OPTION_CLIENTOPTION_H