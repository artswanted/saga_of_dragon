#ifndef WEAPON_VARIANT_USERINFO_PGHIDDENOPEN_H
#define WEAPON_VARIANT_USERINFO_PGHIDDENOPEN_H

#include "idobject.h"
#include "Lohengrin/LockUtil.h"

int const HIDDENOPEN_BIT_NUM = 1;
int const MAX_HIDDENOPEN_NUM = 1000;
int const MAX_HIDDENOPEN_BYTES = ((MAX_HIDDENOPEN_NUM / 8) * HIDDENOPEN_BIT_NUM);

typedef std::vector<BYTE> CONT_HIDDENOPEN;

class PgHiddenOpen
{
public:

	PgHiddenOpen()
	{
		Init();
	}
	~PgHiddenOpen(){}

	PgHiddenOpen& operator=( PgHiddenOpen const &rhs );

	void Init();
	bool const IsComplete(int const iIdx) const;
	bool Complete(int const iIdx);
	bool Reset(int const iIdx);

	void WriteToPacket(BM::Stream & kPacket) const;
	void ReadFromPacket(BM::Stream & kPacket);

	void GetBuffer(CONT_HIDDENOPEN & kContHiddenOpen)
	{
		kContHiddenOpen.clear();
		kContHiddenOpen.resize(MAX_HIDDENOPEN_BYTES);

		for(int i=0; i<MAX_HIDDENOPEN_BYTES; ++i)
		{
			kContHiddenOpen.push_back(m_byteHiddenOpen[i]);
		}
	}

	void GetDateBuffer(BM::DBTIMESTAMP_EX &kDate)
	{
		kDate = kLastHiddenUpdate;
	}

	void SetLastDateUpdate(BM::DBTIMESTAMP_EX &kDate)
	{
		kLastHiddenUpdate = kDate;
	}

	void LoadDB(CEL::DB_DATA_ARRAY::const_iterator & itor)
	{
		(*itor).PopMemory(m_byteHiddenOpen,MAX_HIDDENOPEN_BYTES); ++itor;
	}

protected:

	bool CalcIDToOffset(int const iIdx, int & iByteOffset,BYTE & bValue) const;
	BYTE m_byteHiddenOpen[MAX_HIDDENOPEN_BYTES];
	BM::DBTIMESTAMP_EX kLastHiddenUpdate;
};

#endif // WEAPON_VARIANT_USERINFO_PGHIDDENOPEN_H