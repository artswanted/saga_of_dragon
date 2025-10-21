#ifndef WEAPON_VARIANT_JOBSKILL_PGJOBSKILLSAVEIDX_H
#define WEAPON_VARIANT_JOBSKILL_PGJOBSKILLSAVEIDX_H

//
template< size_t _T_MAX_ARY_SIZE>
class PgSaveIdxAry
{
	enum
	{
		E_ONE_BIT_PER_BYTE_COUNT = 8,
		E_ALL_BIT_ON = 0xFF,
	};
public:
	PgSaveIdxAry()
	{
		Clear();
	}
	~PgSaveIdxAry()
	{
	}

	void Init(BYTE const (&abySaveIdx)[_T_MAX_ARY_SIZE])
	{
		ReadFromBuff(abySaveIdx);
	}
	void Clear()
	{
		ZeroMemory(m_abySaveIdx, sizeof(m_abySaveIdx));
	}

	void Set(int const iSaveIdx)
	{
		int const iIdx = ConvertToByteIdx(iSaveIdx);
		int const iBitIdx = ConvertToBitIdx(iSaveIdx);
		if( IsSafeByteIdx(iIdx) )
		{
			m_abySaveIdx[iIdx] |= (1 << iBitIdx);
		}
	}
	void Deset(int const iSaveIdx)
	{
		int const iIdx = ConvertToByteIdx(iSaveIdx);
		int const iBitIdx = ConvertToBitIdx(iSaveIdx);
		if( IsSafeByteIdx(iIdx) )
		{
			m_abySaveIdx[iIdx] &= (E_ALL_BIT_ON ^ (1 << iBitIdx));
		}
	}
	bool Get(int const iSaveIdx) const
	{
		int const iIdx = ConvertToByteIdx(iSaveIdx);
		int const iBitIdx = ConvertToBitIdx(iSaveIdx);
		if( IsSafeByteIdx(iIdx) )
		{
			return 0 != (m_abySaveIdx[iIdx] & (1 << iBitIdx));
		}
		return false;
	}
	bool IsSafe(int const iSaveIdx) const
	{
		return	IsSafeByteIdx( ConvertToByteIdx(iSaveIdx) );
	}
	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop( m_abySaveIdx );
	}
	void WriteToPacket(BM::Stream& rkPacket) const
	{
		rkPacket.Push( m_abySaveIdx );
	}
	void WriteToBuff(BYTE (&abySaveIdx)[_T_MAX_ARY_SIZE]) const
	{
		memcpy_s(abySaveIdx, sizeof(abySaveIdx), m_abySaveIdx, sizeof(m_abySaveIdx));
	}
	void ReadFromBuff(BYTE const (&abySaveIdx)[_T_MAX_ARY_SIZE])
	{
		memcpy_s(m_abySaveIdx, sizeof(m_abySaveIdx), abySaveIdx, sizeof(abySaveIdx));
	}
	void CopyTo(PgSaveIdxAry< _T_MAX_ARY_SIZE >& rhs) const
	{
		BM::Stream kPacket;
		WriteToPacket(kPacket);
		rhs.ReadFromPacket(kPacket);
	}
protected:
	bool IsSafeByteIdx(int const iByteIdx) const
	{
		return	(0 <= iByteIdx)
			&&	(_T_MAX_ARY_SIZE > iByteIdx);
	}
	int ConvertToByteIdx(int const iSaveIdx) const
	{
		return iSaveIdx / E_ONE_BIT_PER_BYTE_COUNT;
	}
	int ConvertToBitIdx(int const iSaveIdx) const
	{
		return iSaveIdx % E_ONE_BIT_PER_BYTE_COUNT;
	}

private:
	BYTE m_abySaveIdx[_T_MAX_ARY_SIZE];
};

//
namespace JobSkillSaveIdxUtil
{
	typedef PgSaveIdxAry< MAX_DB_JOBKSILL_SAVEIDX_SIZE > PgJobSkillSaveIdx;

	bool Check(CONT_DEF_JOBSKILL_SAVEIDX const& kDefJobSkillSaveIdx, int const iSaveIdx);
	bool IsUseableSaveIdx( PgPlayer const * pkPlayer, int const iSaveIdx );
}

//
typedef JobSkillSaveIdxUtil::PgJobSkillSaveIdx PgJobSkillSaveIdx;

#endif // WEAPON_VARIANT_JOBSKILL_PGJOBSKILLSAVEIDX_H