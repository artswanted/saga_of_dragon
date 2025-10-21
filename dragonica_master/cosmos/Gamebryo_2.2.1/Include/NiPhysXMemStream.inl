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
inline unsigned int NiPhysXMemStream::GetSize() const
{
    return m_pkStream->GetSize();
}
//---------------------------------------------------------------------------
inline void* NiPhysXMemStream::GetBuffer()
{
    return m_pkStream->Str();
}
//---------------------------------------------------------------------------
inline void NiPhysXMemStream::Reset()
{
    m_pkStream->Seek(-(int)m_pkStream->GetSize());
}
//---------------------------------------------------------------------------
inline NxU8 NiPhysXMemStream::readByte() const
{
    NxU8 kV;
    assert(m_pkStream);
    NIVERIFY(m_pkStream->Read(&kV, sizeof(NxU8)) == sizeof(NxU8));

    return kV;
}
//---------------------------------------------------------------------------
inline NxU16 NiPhysXMemStream::readWord() const
{
    NxU16 kV;
    assert(m_pkStream);
    NIVERIFY(m_pkStream->Read(&kV, sizeof(NxU16)) == sizeof(NxU16));

    return kV;
}
//---------------------------------------------------------------------------
inline NxU32 NiPhysXMemStream::readDword() const
{
    NxU32 kV;
    assert(m_pkStream);
    NIVERIFY(m_pkStream->Read(&kV, sizeof(NxU32)) == sizeof(NxU32));

    return kV;
}
//---------------------------------------------------------------------------
inline NxF32 NiPhysXMemStream::readFloat() const
{
    NxF32 kV;
    assert(m_pkStream);
    NIVERIFY(m_pkStream->Read(&kV, sizeof(NxF32)) == sizeof(NxF32));

    return kV;
}
//---------------------------------------------------------------------------
inline NxF64 NiPhysXMemStream::readDouble() const
{
    NxF64 kV;
    assert(m_pkStream);
    NIVERIFY(m_pkStream->Read(&kV, sizeof(NxF64)) == sizeof(NxF64));

    return kV;
}
//---------------------------------------------------------------------------
inline void NiPhysXMemStream::readBuffer(void* buffer, NxU32 size) const
{
    assert(m_pkStream);
    NIVERIFY(m_pkStream->Read(buffer, size) == size);
}
//---------------------------------------------------------------------------
inline NxStream& NiPhysXMemStream::storeByte(NxU8 b)
{
    assert(m_pkStream);
    NIVERIFY(m_pkStream->Write(&b, sizeof(NxU8)) == sizeof(NxU8));
    
    return *this;
}
//---------------------------------------------------------------------------
inline NxStream& NiPhysXMemStream::storeWord(NxU16 w)
{
    assert(m_pkStream);
    NIVERIFY(m_pkStream->Write(&w, sizeof(NxU16)) == sizeof(NxU16));
    
    return *this;
}
//---------------------------------------------------------------------------
inline NxStream& NiPhysXMemStream::storeDword(NxU32 d)
{
    assert(m_pkStream);
    NIVERIFY(m_pkStream->Write(&d, sizeof(NxU32)) == sizeof(NxU32));
    
    return *this;
}
//---------------------------------------------------------------------------
inline NxStream& NiPhysXMemStream::storeFloat(NxF32 f)
{
    assert(m_pkStream);
    NIVERIFY(m_pkStream->Write(&f, sizeof(NxF32)) == sizeof(NxF32));
    
    return *this;
}
//---------------------------------------------------------------------------
inline NxStream& NiPhysXMemStream::storeDouble(NxF64 f)
{
    assert(m_pkStream);
    NIVERIFY(m_pkStream->Write(&f, sizeof(NxF64)) == sizeof(NxF64));
    
    return *this;
}
//---------------------------------------------------------------------------
inline NxStream& NiPhysXMemStream::storeBuffer(const void* buffer, NxU32 size)
{
    assert(m_pkStream);
    NIVERIFY(m_pkStream->Write(buffer, size) == size);
    
    return *this;
}
//---------------------------------------------------------------------------
