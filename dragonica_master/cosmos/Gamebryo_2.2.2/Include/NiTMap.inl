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

//---------------------------------------------------------------------------
template <class TKEY, class TVAL> inline
NiTMap<TKEY,TVAL>::~NiTMap()
{
    // RemoveAll is called from here because it depends on virtual functions
    // implemented in NiTAllocatorMap.  It will also be called in the 
    // parent destructor, but the map will already be empty.
    NiTMap<TKEY,TVAL>::RemoveAll();
}
//---------------------------------------------------------------------------
template <class TKEY, class TVAL> inline
NiTMapItem<TKEY, TVAL>* NiTMap<TKEY, TVAL>::NewItem()
{
    return (NiTMapItem<TKEY, TVAL>*)NiTMapBase<NiTDefaultAllocator<TVAL>, 
        TKEY, TVAL >::m_kAllocator.Allocate();
}
//---------------------------------------------------------------------------
template <class TKEY, class TVAL> inline
void NiTMap<TKEY, TVAL>::DeleteItem(NiTMapItem<TKEY, TVAL>* pkItem)
{
    // set key and val to zero so that if they are smart pointers
    // their references will be decremented.
    pkItem->m_val = 0;
    NiTMapBase<NiTDefaultAllocator<TVAL>, TKEY, 
        TVAL >::m_kAllocator.Deallocate(pkItem);
}
//---------------------------------------------------------------------------
