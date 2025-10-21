#include "stdafx.h"
#include <BM/Stream.h>

bool SendToServerType(const CEL::E_SESSION_TYPE, const BM::Stream &)
{
    ASSERT_LOG( false, BM::LOG_LV1, __FL__ << "Don't call stub!" );
    return false;
}
