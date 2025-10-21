// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#pragma once

namespace NiManagedToolInterface
{
#pragma unmanaged
    class NiVirtualBoolBugWrapper
    {
    public:
        static int NiStream_Load(NiStream& kStream, const char* pcFilename)
        {
            return (int) kStream.Load(pcFilename);
        }

        static int NiStream_Save(NiStream& kStream, const char* pcFilename)
        {
            return (int) kStream.Save(pcFilename);
        }

        static int NiKeyBasedInterpolator_GetChannelPosed(
            NiKeyBasedInterpolator* pkInterp, unsigned short usChannel)
        {
            return (int) pkInterp->GetChannelPosed(usChannel);
        }

        static int NiBSplineInterpolator_GetChannelPosed(
            NiBSplineInterpolator* pkInterp, unsigned short usChannel)
        {
            return (int) pkInterp->GetChannelPosed(usChannel);
        }
    };
#pragma managed
}
