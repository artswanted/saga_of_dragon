#pragma once
#include <BM/Stream.h>
#include "Lohengrin/Lohengrin.h"
#include <Variant/Variant.h>

namespace AES
{

typedef BM::Stream::DEF_STREAM_TYPE     EVENT_TYPE;
typedef BM::Stream*                     EVENT_BODY;
typedef CUnit*                          EVENT_RECIVER;
typedef void*                           EVENT_EXTRA; //Used for pass extra info, for example ground in map server

typedef void(*EVENT_SYSTEM)(/* CUnit* */EVENT_RECIVER, /* BM::Stream* */EVENT_BODY, EVENT_EXTRA);

bool Exec(EVENT_TYPE eventType, EVENT_BODY eventBody, EVENT_RECIVER eventReciver, EVENT_EXTRA eventExtra);

struct AutoRegisterSystem
{
    AutoRegisterSystem *m_kNext;
    EVENT_TYPE m_kEventType;
    EVENT_SYSTEM m_kCallback;
    explicit AutoRegisterSystem(EVENT_TYPE eventType, EVENT_SYSTEM system);
};

}

#define AES_REGISTER_SYSTEM(type, func) \
    static AES::AutoRegisterSystem AES_REGISTER__##func(type, (AES::EVENT_SYSTEM)##func);

#define AES_REGISTER_TYPED_SYSTEM(type) AES_REGISTER_SYSTEM(type, type##__AES)
