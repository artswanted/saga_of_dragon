#ifndef WEAPON_VARIANT_JOBSKILL_PGPROBABILITY_H
#define WEAPON_VARIANT_JOBSKILL_PGPROBABILITY_H

//
namespace ProbabilityUtil
{
	bool Verify(CONT_DEF_JOBSKILL_PROBABILITY_BAG const& rkContDef, wchar_t const* pkFunction, size_t const iLine);
	//

	template< typename _T_FIRST_TYPE, typename _T_FUNC >
	SProbability GetOne(_T_FIRST_TYPE& rkFirst, CONT_DEF_JOBSKILL_PROBABILITY_BAG const& rkContDef, int const iNo, int const iAddValue)
	{
		CONT_DEF_JOBSKILL_PROBABILITY_BAG::const_iterator find_iter = rkContDef.find(iNo);
		if( rkContDef.end() == find_iter )
		{
			return SProbability();
		}

		CONT_DEF_JOBSKILL_PROBABILITY_BAG::mapped_type const& rkDefProbBag = (*find_iter).second;
		if( _T_FUNC()(rkFirst, rkDefProbBag) )
		{
			switch( rkDefProbBag.eType )
			{
			case PT_PROBABILITY:
				{
						SProbability const kNextProbability( rkDefProbBag.Get(iAddValue) ); // Get() 함수 내부에서 iAddValue를 필터링 한다
						return SProbability( GetOne< _T_FIRST_TYPE, _T_FUNC >(rkFirst, rkContDef, kNextProbability.iResultNo, iAddValue) );
				}break;
			case PT_ITEM:
			default:
				{
					// pass through
				}break;
			}
			return SProbability( rkDefProbBag.Get(iAddValue) );
		}
		return SProbability();
	}

	//
	struct SGetOneArguments
	{
		SGetOneArguments(BM::GUID const& rkCharGuid, CONT_DEF_JOBSKILL_SAVEIDX const& rkDefJobSkillSaveIdx)
			: kCharGuid(rkCharGuid), kDefJobSkillSaveIdx(rkDefJobSkillSaveIdx), iErrorCode(0)
		{
		}

		BM::GUID const& kCharGuid;
		CONT_DEF_JOBSKILL_SAVEIDX const& kDefJobSkillSaveIdx;
		int iErrorCode;
	};

	//
	SProbability GetOne(SGetOneArguments& rkArg, CONT_DEF_JOBSKILL_PROBABILITY_BAG const& rkContDef, int const iNo, int const iAddValue = 0); // Need SaveIdx 체크하는 함수
	SProbability GetOne(CONT_DEF_JOBSKILL_PROBABILITY_BAG const& rkContDef, int const iNo, int const iAddValue = 0); // Need SaveIdx 체크하지 않는 함수

};

#endif // WEAPON_VARIANT_JOBSKILL_PGPROBABILITY_H