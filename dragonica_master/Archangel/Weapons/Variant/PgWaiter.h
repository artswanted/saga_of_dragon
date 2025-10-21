#ifndef WEAPON_VARIANT_BASIC_PGWAITER_H
#define WEAPON_VARIANT_BASIC_PGWAITER_H

#include <map>
#include <set>

template< class _Key, class _Val >
class PgWaiterSet
{
public:
	typedef _Key Key_Type;
	typedef _Val Value_Type;
	typedef std::set< _Val > ContWaiter;
	typedef std::vector< _Val > ContWaiterVec;
	typedef std::map< _Key, ContWaiter > ContTotal;

public:
	PgWaiterSet() {};
	~PgWaiterSet() {};

	void Clear()
	{
		m_kStack.clear();
	}
	bool AddWaiter(const _Key& rkKey, const _Val& rkWaiter)
	{
		ContTotal::mapped_type *pkElement = NULL;
		if(!Add(rkKey, pkElement))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if(pkElement)
		{
			auto ibRet = pkElement->insert(rkWaiter);
			return ibRet.second;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	bool GetWaiter(const _Key& rkKey, ContWaiter& rkOut)
	{
		ContTotal::mapped_type *pkElement = NULL;
		if(Get(rkKey, pkElement))
		{
			rkOut = *pkElement;
			return true;
		}
		rkOut.clear();
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool GetWaiter(const _Key& rkKey, ContWaiterVec& rkOut)
	{
		ContTotal::mapped_type *pkElement = NULL;
		if( Get(rkKey, pkElement) )
		{
			ContWaiter::iterator element_iter = pkElement->begin();
			while(pkElement->end() != element_iter)
			{
				const ContWaiter::value_type& rkElement = (*element_iter);
				rkOut.push_back(rkElement);
				++element_iter;
			}
			return true;
		}
		rkOut.clear();
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool DelWaiter(const _Key& rkKey)
	{
		return Del(rkKey);
	}
	bool DelWaiter(const _Key& rkKey, const _Val& rkWaiter)
	{
		ContTotal::mapped_type *pkElement = NULL;
		if(Get(rkKey, pkElement))
		{
			pkElement->erase(rkWaiter);
			ContTotal::mapped_type::iterator kIter = pkElement->find(rkWaiter);
			return (pkElement->end() == kIter);
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
protected:
	bool Add(const _Key& rkKey, ContWaiter* &pkWaiter)
	{
		ContTotal::iterator kTotal_iter = m_kStack.find(rkKey);
		if(m_kStack.end() == kTotal_iter)
		{
			ContTotal::mapped_type kElement;
			auto ibRet = m_kStack.insert(std::make_pair(rkKey, kElement));
			if(!ibRet.second)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			kTotal_iter = ibRet.first;
		}
		pkWaiter = &kTotal_iter->second;
		return true;
	}
	bool Get(const _Key& rkKey, ContWaiter* &pkWaiter)
	{
		ContTotal::iterator kTotal_iter = m_kStack.find(rkKey);
		if(m_kStack.end() != kTotal_iter)
		{
			pkWaiter = &kTotal_iter->second;
			return true;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	bool Del(const _Key& rkKey)
	{
		ContTotal::iterator kTotal_iter = m_kStack.find(rkKey);
		if(m_kStack.end() != kTotal_iter)
		{
			m_kStack.erase(rkKey);

			kTotal_iter = m_kStack.find(rkKey);//Re Find
			return (m_kStack.end() == kTotal_iter);
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

private:
	ContTotal m_kStack;
};

#endif // WEAPON_VARIANT_BASIC_PGWAITER_H