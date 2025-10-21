#ifndef WEAPON_AILEPACK_BASIC_CONSTANT_H
#define WEAPON_AILEPACK_BASIC_CONSTANT_H

namespace CASH
{
	typedef struct _SCashCost
	{
		_SCashCost()
		{
			i64UsedCash = i64UsedMileage = i64AddedMileage = 0i64;
		}

		explicit _SCashCost(__int64 const _Cash, __int64 const _Mileage, __int64 const _AddedMileage)
		{
			i64UsedCash = _Cash;
			i64UsedMileage = _Mileage;
			i64AddedMileage = _AddedMileage;
		}

		_SCashCost(_SCashCost const& rhs)
		{
			(*this) = rhs;
		}

		_SCashCost const& operator=(_SCashCost const& rhs)
		{
			i64UsedCash = rhs.i64UsedCash;
			i64UsedMileage = rhs.i64UsedMileage;
			i64AddedMileage = rhs.i64AddedMileage;
			return (*this);
		}

		void WriteToPacket(BM::Stream &kPacket)const
		{
			kPacket.Push(i64UsedCash);
			kPacket.Push(i64UsedMileage);
			kPacket.Push(i64AddedMileage);
		}

		void ReadFromPacket(BM::Stream &kPacket)
		{
			kPacket.Pop(i64UsedCash);
			kPacket.Pop(i64UsedMileage);
			kPacket.Pop(i64AddedMileage);
		}

		std::wstring ToString()
		{
			BM::vstring vString;
			vString << _T("UsedCash=") << i64UsedCash << _T(", UsedMilage=") << i64UsedMileage << _T(", AddedMilaged=") << i64AddedMileage;
			return vString;
		}

		////////////////////////
		// 값이 Cash 차감이면 음수, Cash 적립이면 양수
		///////////////////////
		__int64 i64UsedCash;		// 사용한 Cash
		__int64 i64UsedMileage;		// 사용한 Mileage
		__int64 i64AddedMileage;	// 충전되는 Mileage
	} SCashCost;


};

#endif // WEAPON_AILEPACK_BASIC_CONSTANT_H