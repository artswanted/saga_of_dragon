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

#ifndef NITRIBASEDGEOM_H
#define NITRIBASEDGEOM_H

#include "NiGeometry.h"
#include "NiTriBasedGeomData.h"

class NIMAIN_ENTRY NiTriBasedGeom : public NiGeometry
{
public:
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiTriBasedGeom);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

public:
    virtual ~NiTriBasedGeom();

    // shared data acess
    virtual void SetModelData(NiGeometryData* pkModelData);

    // triangles
    unsigned short GetTriangleCount() const;
    virtual void GetModelTriangle(unsigned short usTriangle, NiPoint3*& pkP0,
        NiPoint3*& pkP1, NiPoint3*& pkP2) = 0;
    void SetActiveTriangleCount(unsigned short usActive);
    unsigned short GetActiveTriangleCount() const;

    // *** begin Emergent internal use only ***

    void GetTriangleIndices(unsigned short i, unsigned short& i0,
        unsigned short& i1, unsigned short& i2) const;
    
    // *** end Emergent internal use only *** 

protected:
    // The constructed object shares the input data.
    NiTriBasedGeom(NiTriBasedGeomData* pkModelData);

    // streaming support
    NiTriBasedGeom();
};

typedef NiPointer<NiTriBasedGeom> NiTriBasedGeomPtr;

//---------------------------------------------------------------------------
//  Inline include
#include "NiTriBasedGeom.inl"

//---------------------------------------------------------------------------

#endif

