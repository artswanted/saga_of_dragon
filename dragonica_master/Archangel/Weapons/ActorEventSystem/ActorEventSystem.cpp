#include "ActorEventSystem.h"
#include <unordered_map>

namespace AES
{

struct EventHandlerHolder
{
    EVENT_TYPE Type;
};

// todo: optimizate, need make a O(1) search
static AutoRegisterSystem* m_kEvents = NULL;
static AutoRegisterSystem* g_kLast = NULL;

bool Exec(EVENT_TYPE eventType, EVENT_BODY eventBody, EVENT_RECIVER eventReciver, EVENT_EXTRA eventExtra)
{
    AutoRegisterSystem* next = m_kEvents;
    bool found = false;
    while (next != NULL)
    {
        if (next->m_kEventType == eventType)
        {
            next->m_kCallback(eventReciver, eventBody, eventExtra);
            found = true;
        }
        next = next->m_kNext;
    }
    return found;
}

AutoRegisterSystem::AutoRegisterSystem(EVENT_TYPE eventType, EVENT_SYSTEM system)
{
    if (g_kLast)
        g_kLast->m_kNext = this;
    else
        m_kEvents = this;
    g_kLast = this;
    m_kNext = NULL;
    m_kEventType = eventType;
    m_kCallback = system;
}

}
