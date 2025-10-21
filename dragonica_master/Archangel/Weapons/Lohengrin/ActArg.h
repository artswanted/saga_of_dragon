#ifndef WEAPON_LOHENGRIN_UTILITY_ACTARG_H
#define WEAPON_LOHENGRIN_UTILITY_ACTARG_H

class SActArg
{
public:
	typedef std::map<int, BM::Stream> CONT_VALUE;
	mutable CONT_VALUE m_contValue;

	template< typename T >
	HRESULT Get(int const id, T &rkOutValue)const
	{
		BM::CAutoMutex kLock(m_kMutex);
		size_t const data_size = sizeof(T);
		CONT_VALUE::iterator itor = m_contValue.find(id);
		if (m_contValue.end() != itor)
		{
			// map개체나 wstring 개체 같은 경우 size가 다르다.
			//if (itor->second.Size() != data_size)
			//{
			//	return E_FAIL;
			//}
			itor->second.Pop(rkOutValue);
			itor->second.PosAdjust();
			return S_OK;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	template< typename T >
	HRESULT Set(int const id, const T &rkValue)
	{
		BM::CAutoMutex kLock(m_kMutex);
		size_t const data_size = sizeof(T);
		auto ret = m_contValue.insert( std::make_pair(id, BM::Stream()) );
		if (!ret.second)
		{
			ret.first->second.Reset();
			ret.first->second.Push(rkValue);
			return S_OK;
		}
		ret.first->second.Push(rkValue);
		return S_OK;
	}

	template< >
	HRESULT Get(int const id, BM::Stream &rkOutValue)const
	{
		HRESULT const hRt = Get(id, rkOutValue.Data());
		rkOutValue.PosAdjust();
		return hRt;
	}

	template< >
	HRESULT Set(int const id, BM::Stream const& rkValue)
	{
		return Set(id, rkValue.Data());
	}

	HRESULT Remove(int const id)
	{
		BM::CAutoMutex kLock(m_kMutex);
		CONT_VALUE::iterator itor = m_contValue.find(id);
		if (m_contValue.end() != itor)
		{
			m_contValue.erase(itor);
			return S_OK;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		BM::CAutoMutex kLock(m_kMutex);
		
		m_contValue.clear();

		size_t cont_size = 0;
		if(kPacket.Pop(cont_size))
		{
			while(cont_size--)
			{
				CONT_VALUE::key_type kKey;
				CONT_VALUE::mapped_type kValue;
				kPacket.Pop(kKey);
				kPacket.Pop(kValue.Data());
				kValue.PosAdjust();

				m_contValue.insert(std::make_pair(kKey, kValue));
			}
		}
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		BM::CAutoMutex kLock(m_kMutex);
		kPacket.Push(m_contValue.size());

		CONT_VALUE::const_iterator itor = m_contValue.begin();
		while(itor != m_contValue.end())
		{
			kPacket.Push((*itor).first);
			kPacket.Push((*itor).second.Data());
			++itor;
		}
	}

	SActArg(){};
	SActArg(const SActArg& rhs)
		: m_contValue(rhs.m_contValue)
	{
	}

	SActArg const& operator=(SActArg const& rhs)
	{
		m_contValue = rhs.m_contValue;
		return (*this);
	}

	void Clear()
	{
		m_contValue.clear();
	}
protected:
	mutable Loki::Mutex m_kMutex;
};



#endif // WEAPON_LOHENGRIN_UTILITY_ACTARG_H