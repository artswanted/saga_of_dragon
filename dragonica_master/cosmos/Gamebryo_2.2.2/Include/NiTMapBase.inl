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

#include <NiUniversalTypes.h>
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
NiTMapBase<TheAllocator, TKEY, TVAL>::NiTMapBase(unsigned int uiHashSize)
{
    m_uiHashSize = uiHashSize;
    m_kAllocator.m_uiCount = 0;

    unsigned int uiSize = sizeof(NiTMapItem<TKEY,TVAL>*)*m_uiHashSize;
    m_ppkHashTable = (NiTMapItem<TKEY,TVAL>**)NiMalloc(uiSize);
    assert(m_ppkHashTable);
    memset(m_ppkHashTable, 0, 
        m_uiHashSize * sizeof(NiTMapItem<TKEY, TVAL>*));
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
NiTMapBase<TheAllocator, TKEY, TVAL>::~NiTMapBase()
{
    RemoveAll();
    NiFree(m_ppkHashTable);
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
unsigned int NiTMapBase<TheAllocator, TKEY, TVAL>::GetCount() const
{ 
    return m_kAllocator.m_uiCount;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
bool NiTMapBase<TheAllocator, TKEY, TVAL>::IsEmpty() const
{ 
    return m_kAllocator.m_uiCount == 0;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
void NiTMapBase<TheAllocator, TKEY, TVAL>::SetAt(TKEY key, TVAL val)
{
    // look up hash table location for key
    unsigned int uiIndex = KeyToHashIndex(key);
    NiTMapItem<TKEY, TVAL>* pkItem = m_ppkHashTable[uiIndex];
    
    unsigned int uiDepth = 0;


    // search list at hash table location for key
    while (pkItem)
    {
        uiDepth++;
        if (IsKeysEqual(key, pkItem->m_key))
        {
            // item already in hash table, set its new value
            pkItem->m_val = val;
            return;
        }
        pkItem = pkItem->m_pkNext;
    }

    // add object to beginning of list for this hash table index
    pkItem = (NiTMapItem<TKEY, TVAL>*)NewItem();

    assert(pkItem);
    SetValue(pkItem, key, val);
    pkItem->m_pkNext = m_ppkHashTable[uiIndex];
    m_ppkHashTable[uiIndex] = pkItem;
    m_kAllocator.m_uiCount++;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
bool NiTMapBase<TheAllocator, TKEY, TVAL>::RemoveAt(TKEY key)
{
    // look up hash table location for key
    unsigned int uiIndex = KeyToHashIndex(key);
    NiTMapItem<TKEY, TVAL>* pkItem = m_ppkHashTable[uiIndex];

    // search list at hash table location for key
    if (pkItem)
    {
        if (IsKeysEqual(key, pkItem->m_key))
        {
            // item at front of list, remove it
            m_ppkHashTable[uiIndex] = pkItem->m_pkNext;
            ClearValue(pkItem);
            
            DeleteItem(pkItem);

            m_kAllocator.m_uiCount--;
            return true;
        }
        else
        {
            // search rest of list for item
            NiTMapItem<TKEY, TVAL>* pkPrev = pkItem;
            NiTMapItem<TKEY, TVAL>* pkCurr = pkPrev->m_pkNext;
            while (pkCurr && !IsKeysEqual(key, pkCurr->m_key))
            {
                pkPrev = pkCurr;
                pkCurr = pkCurr->m_pkNext;
            }
            if (pkCurr)
            {
                // found the item, remove it
                pkPrev->m_pkNext = pkCurr->m_pkNext;
                ClearValue(pkCurr);
                
                DeleteItem(pkCurr);

                m_kAllocator.m_uiCount--;
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
void NiTMapBase<TheAllocator, TKEY, TVAL>::RemoveAll()
{
    for (unsigned int i = 0; i < m_uiHashSize; i++) 
    {
        while (m_ppkHashTable[i])
        {
            NiTMapItem<TKEY, TVAL>* pkSave = m_ppkHashTable[i];
            m_ppkHashTable[i] = m_ppkHashTable[i]->m_pkNext;
            ClearValue(pkSave);
            
            DeleteItem(pkSave);
        }
    }

    m_kAllocator.m_uiCount = 0;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
bool NiTMapBase<TheAllocator, TKEY, TVAL>::GetAt(TKEY key, TVAL& val) const
{
    // look up hash table location for key
    unsigned int uiIndex = KeyToHashIndex(key);
    NiTMapItem<TKEY, TVAL>* pkItem = m_ppkHashTable[uiIndex];

    // search list at hash table location for key
    while (pkItem)
    {
        if (IsKeysEqual(key, pkItem->m_key))
        {
            // item found
            val = pkItem->m_val;
            return true;
        }
        pkItem = pkItem->m_pkNext;
    }

    return false;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
unsigned int NiTMapBase<TheAllocator, TKEY, TVAL>::
    KeyToHashIndex(TKEY key) const
{
    // Modular arithmetic is used for building key.  If a different scheme
    // is preferred, this member function is virtual, so you can derive a
    // class and implement your own.
    return (unsigned int) (((size_t) key) % m_uiHashSize);
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
bool NiTMapBase<TheAllocator, TKEY, TVAL>::IsKeysEqual(TKEY key1, TKEY key2)
    const
{
    return key1 == key2;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
void NiTMapBase<TheAllocator, TKEY, TVAL>::
    SetValue(NiTMapItem<TKEY, TVAL>* pkItem, TKEY key,
    TVAL val)
{
    pkItem->m_key = key;
    pkItem->m_val = val;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
void NiTMapBase<TheAllocator, TKEY, TVAL>::
    ClearValue(NiTMapItem<TKEY, TVAL>* /* pkItem */)
{
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
NiTMapIterator NiTMapBase<TheAllocator, TKEY, TVAL>::GetFirstPos() const
{
    for (unsigned int i = 0; i < m_uiHashSize; i++) 
    {
        if (m_ppkHashTable[i])
            return m_ppkHashTable[i];
    }
    return 0;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL> inline
void NiTMapBase<TheAllocator, TKEY, TVAL>::
    GetNext(NiTMapIterator& pos, TKEY& key, TVAL& val) const
{
    NiTMapItem<TKEY, TVAL>* pkItem = (NiTMapItem<TKEY, TVAL>*) pos;
    
    key = pkItem->m_key;
    val = pkItem->m_val;

    if (pkItem->m_pkNext)
    {
        pos = pkItem->m_pkNext;
        return;
    }

    unsigned int i = KeyToHashIndex(pkItem->m_key);
    for (++i; i < m_uiHashSize; i++) 
    {
        pkItem = m_ppkHashTable[i];
        if (pkItem) 
        {
            pos = pkItem;
            return;
        }
    }

    pos = 0;
}
//---------------------------------------------------------------------------
