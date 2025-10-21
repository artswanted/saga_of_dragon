#ifndef FREEDOM_DRAGONICA_SCRIPTING_MISSION_LWMISSIONMUTATOR_H
#define FREEDOM_DRAGONICA_SCRIPTING_MISSION_LWMISSIONMUTATOR_H
#include "lwPacket.h"
#include "lwUI.h"
#include <set>

//typedef std::pair<int, int> MUTATOR_NODE; // MutatorNo, Custom data
typedef std::set<int> MUTATOR_LIST;

extern MUTATOR_LIST pkMutatorsList;
class lwMissionMutator
{
public:
	static bool RegisterWrapper(lua_State *pkState);

};

//! OneClickCheckBox
void lwOneClickMutatorCheckBox(lwUIWnd kSelf);

//! return mutator packet
lwPacket lwGetMutatorPacket();

//! Return mutator count
unsigned int lwGetMutatotSelectedCount();

//! Maked Mutator UI
void lwMakeMutatorUI();

//! Clear Mutator List
void lwClearMutatorList();

//! Select all mutator
void lwSelectAllMutation();
#endif // FREEDOM_DRAGONICA_SCRIPTING_MISSION_LWMISSIONMUTATOR_H