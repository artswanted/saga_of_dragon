#ifndef WEAPON_VARIANT_PGMACROCHECKTABLE_H
#define WEAPON_VARIANT_PGMACROCHECKTABLE_H

#include "Lohengrin/dbtables.h"

class PgMacroCheckTable
{
public:

	PgMacroCheckTable(){}
	~PgMacroCheckTable(){}

	SMACRO_CHECK_TABLE GetMacroCheckTable()
	{
		BM::CAutoMutex kLock(m_kMutex);
		return m_kTable;
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		BM::CAutoMutex kLock(m_kMutex);
		m_kTable.WriteToPacket(kPacket);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		BM::CAutoMutex kLock(m_kMutex);
		m_kTable.ReadFromPacket(kPacket);
	}

private:

	SMACRO_CHECK_TABLE	m_kTable;

	mutable Loki::Mutex m_kMutex;
};

#define g_kMacroCheckTable SINGLETON_STATIC(PgMacroCheckTable)

#endif // WEAPON_VARIANT_PGMACROCHECKTABLE_H