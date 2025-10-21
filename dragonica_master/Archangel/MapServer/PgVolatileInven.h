#ifndef MAP_MAPSERVER_OBJECT_BATTLESQUARE_PGVOLATILEINVEN_H
#define MAP_MAPSERVER_OBJECT_BATTLESQUARE_PGVOLATILEINVEN_H

namespace VolatileInvenUtil
{
	class PgRecycleAbleNum
	{
		typedef std::list< size_t > CONT_FREE_NUM;
	public:
		PgRecycleAbleNum(size_t const iMax);
		~PgRecycleAbleNum();

		void Clear();
		size_t Pop();
		void Push(size_t FreeNum);

	private:
		size_t m_Next;
		size_t m_Max;
		CONT_FREE_NUM m_kContFreeNum;
	};
};

//////
class PgVolatileInven
{
public:
	PgVolatileInven();
	~PgVolatileInven();

	static const int MAX_VOLATILE_ITEM_COUNT = 30;
	void Clear();
	size_t Push(CONT_BS_ITEM const& rkBSItem, PgBase_Item const& rkNewItem, CONT_BS_BUFF& kOutAddBuff);
	void Swap(CONT_VOLATILE_INVEN_ITEM& rkOut, CONT_BS_BUFF& rkDelBuff);
	int IsHaveMinimapIcon() const;
	int MinimapIconCount() const;

	template<typename T_ITERATOR_VALUE>
	class SumSecondValue
	{
	public:
		typename typedef T_ITERATOR_VALUE::second_type SECOND_TYPE;
		SumSecondValue() : m_kSum(SECOND_TYPE()) {};

		void operator() (T_ITERATOR_VALUE const& _value)
		{
			m_kSum = m_kSum + _value.second;
		}

		operator SECOND_TYPE()
		{
			return m_kSum;
		}

	private:
		SECOND_TYPE m_kSum;
	};

	CLASS_DECLARATION_S(BM::GUID, OwnerGuid);
protected:
	CONT_VOLATILE_INVEN_ITEM m_kContVolatileItem;
	VolatileInvenUtil::PgRecycleAbleNum m_kNum;
	CONT_BS_BUFF_COUNT m_kContBuffCount;
	CONT_BS_MINIMAP_ICON m_kContMinmapIcon;
};


//
class PgVolatileInvenMng
{
	typedef std::map< BM::GUID, PgVolatileInven* > CONT_VOLATILE_INEVEN;
public:
	PgVolatileInvenMng();
	~PgVolatileInvenMng();

	void ClearInven();

	bool AddInven(BM::GUID const& rkOwnerGuid);
	bool DelInven(BM::GUID const& rkOwnerGuid);

	size_t PushItem(CONT_BS_ITEM const& rkBSItem, BM::GUID const& rkOwnerGuid, PgBase_Item const& rkNewItem, CONT_BS_BUFF& kOutAddBuff);
	bool SwapInven(BM::GUID const& rkOwnerGuid, CONT_VOLATILE_INVEN_ITEM& rkOut, CONT_BS_BUFF& rkDelBuff);
	bool GetMinimapIconUser(CONT_BS_HAVE_MINIMAP_ICON& rkOut) const;
	int GetMinimapIconCount(VEC_GUID const& rkVecGuid) const;
	int GetIconCount(BM::GUID const& kGuid) const;

protected:
	PgVolatileInven* Get(BM::GUID const& rkOwnerGuid);
	PgVolatileInven const* Get(BM::GUID const& rkOwnerGuid) const;

private:
	CONT_VOLATILE_INEVEN m_kContVolatileIneven;
	//DWORD m_dwTickTime;
};

#endif // MAP_MAPSERVER_OBJECT_BATTLESQUARE_PGVOLATILEINVEN_H