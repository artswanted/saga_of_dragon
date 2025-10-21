#include "stdafx.h"
#include "lohengrin/dbtables.h"
#include "Item.h"
#include "PgPlayer.h"
#include "TableDataManager.h"
#include "PgJobSkillSaveIdx.h"
#include "PgProbability.h"

//
namespace ProbabilityUtil
{
	bool Verify(CONT_DEF_JOBSKILL_PROBABILITY_BAG const& rkContDef, wchar_t const* pkFunction, size_t const iLine)
	{
		typedef std::list< BM::vstring > CONT_ERROR_MSG;
		CONT_ERROR_MSG kErrorMsg;

		CONT_DEFITEM const* pkDefItem = NULL;
		g_kTblDataMgr.GetContDef(pkDefItem);

		CONT_DEF_JOBSKILL_PROBABILITY_BAG::const_iterator iter = rkContDef.begin();
		while( rkContDef.end() != iter )
		{
			CONT_DEF_JOBSKILL_PROBABILITY_BAG::mapped_type const& rkProbabilityBag = (*iter).second;
			switch( rkProbabilityBag.eType )
			{
			case PT_ITEM:
				{
					size_t iAtIndex = 1;
					CONT_PROBABILITY::const_iterator item_iter = rkProbabilityBag.kContProbability.begin();
					while( rkProbabilityBag.kContProbability.end() != item_iter )
					{
						CONT_PROBABILITY::value_type const& rkProbability = (*item_iter);

						if( !rkProbability.IsFail()
						&&	pkDefItem->end() == pkDefItem->find(rkProbability.iResultNo) )
						{
							kErrorMsg.push_back( BM::vstring() << L"[TB_DefJobSkill_Probability] Wrong [Idx:"<<iAtIndex<<L"] item No["<<rkProbability.iResultNo<<L"], in Probability["<<rkProbabilityBag.iNo<<L"] Type["<<rkProbabilityBag.eType<<L"]" );
						}
						if( rkProbability.IsWrong() )
						{
							kErrorMsg.push_back( BM::vstring() << L"[TB_DefJobSkill_Probability] Wrong [Idx:"<<iAtIndex<<L"] item No["<<rkProbability.iResultNo<<L"]["<<rkProbability.iCount<<L"]["<<rkProbability.iProbability<<L"], in Probability["<<rkProbabilityBag.iNo<<L"] Type["<<rkProbabilityBag.eType<<L"]" );
						}
						++item_iter;
						++iAtIndex;
					}
					if( rkProbabilityBag.iTotalProbability > rkProbabilityBag.iTotalSumProbability )
					{
						kErrorMsg.push_back( BM::vstring() << L"[TB_DefJobSkill_Probability] Wrong TotalProbability["<<rkProbabilityBag.iTotalProbability<<L"] > Sum["<<rkProbabilityBag.iTotalSumProbability<<L"], in Probability["<<rkProbabilityBag.iNo<<L"] Type["<<rkProbabilityBag.eType<<L"]" );
					}
				}break;
			case PT_PROBABILITY:
				{
					size_t iAtIndex = 1;
					CONT_PROBABILITY::const_iterator item_iter = rkProbabilityBag.kContProbability.begin();
					while( rkProbabilityBag.kContProbability.end() != item_iter )
					{
						CONT_PROBABILITY::value_type const& rkProbability = (*item_iter);

						CONT_DEF_JOBSKILL_PROBABILITY_BAG::const_iterator find_iter = rkContDef.find(rkProbability.iResultNo);
						if( !rkProbability.IsFail()
						&&	rkContDef.end() == find_iter )
						{
							kErrorMsg.push_back( BM::vstring() << L"[TB_DefJobSkill_Probability] Wrong [Idx:"<<iAtIndex<<L"] probability result No["<<rkProbability.iResultNo<<L"], in Probability["<<rkProbabilityBag.iNo<<L"] Type["<<rkProbabilityBag.eType<<L"]" );
						}
						if( !rkProbability.IsFail()
						&&	rkContDef.end() != find_iter
						&&	(*find_iter).second.eType != PT_ITEM )
						{
							kErrorMsg.push_back( BM::vstring() << L"[TB_DefJobSkill_Probability] Wrong [Idx:"<<iAtIndex<<L"] probability result No["<<rkProbability.iResultNo<<L"] Type["<<(*find_iter).second.eType<<L"], in Probability["<<rkProbabilityBag.iNo<<L"] Type["<<rkProbabilityBag.eType<<L"]" );
						}
						if( !rkProbability.IsFail()
						&&	rkProbabilityBag.iNo == rkProbability.iResultNo )
						{
							kErrorMsg.push_back( BM::vstring() << L"[TB_DefJobSkill_Probability] same [Idx:"<<iAtIndex<<L"] probability result No["<<rkProbability.iResultNo<<L"] to parent, in Probability["<<rkProbabilityBag.iNo<<L"] Type["<<rkProbabilityBag.eType<<L"]" );
						}
						if( rkProbability.IsFail()
						||	rkProbability.IsWrong() )
						{
							kErrorMsg.push_back( BM::vstring() << L"[TB_DefJobSkill_Probability] same [Idx:"<<iAtIndex<<L"] Wrong result No["<<rkProbability.iResultNo<<L"]["<<rkProbability.iCount<<L"]["<<rkProbability.iProbability<<L"] to parent, in Probability["<<rkProbabilityBag.iNo<<L"] Type["<<rkProbabilityBag.eType<<L"]" );
						}
						++item_iter;
						++iAtIndex;
					}

					if( rkProbabilityBag.iTotalProbability != rkProbabilityBag.iTotalSumProbability )
					{
						kErrorMsg.push_back( BM::vstring() << L"[TB_DefJobSkill_Probability] Wrong TotalProbability["<<rkProbabilityBag.iTotalProbability<<L"] != Sum["<<rkProbabilityBag.iTotalSumProbability<<L"], in Probability["<<rkProbabilityBag.iNo<<L"] Type["<<rkProbabilityBag.eType<<L"]" );
					}
				}break;
			default:
				{
					kErrorMsg.push_back( BM::vstring() << L"[TB_DefJobSkill_Probability] Wrong probability Type["<<rkProbabilityBag.eType<<L"], in Probability["<<rkProbabilityBag.iNo<<L"]" );
				}break;
			}
			++iter;
		}
		
		CONT_ERROR_MSG::const_iterator msg_iter = kErrorMsg.begin();
		while( kErrorMsg.end() != msg_iter )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL2__(pkFunction, iLine) << (*msg_iter));
			++msg_iter;
		}
		return kErrorMsg.empty();
	}

	//
	SProbability GetOne(SGetOneArguments& rkArg, CONT_DEF_JOBSKILL_PROBABILITY_BAG const& rkContDef, int const iNo, int const iAddValue)
	{
		struct CheckSaveIdx
		{
			bool operator ()(SGetOneArguments& rkArg, CONT_DEF_JOBSKILL_PROBABILITY_BAG::mapped_type const& rkDefProbBag)
			{
				if( !JobSkillSaveIdxUtil::Check(rkArg.kDefJobSkillSaveIdx, rkDefProbBag.i01NeedSaveIdx) )
				{
					rkArg.iErrorCode = rkDefProbBag.i01NeedSaveIdx;
					return false;
				}
				return true;
			}
		};

		return GetOne< SGetOneArguments, CheckSaveIdx >(rkArg, rkContDef, iNo, iAddValue);
	}
	SProbability GetOne(CONT_DEF_JOBSKILL_PROBABILITY_BAG const& rkContDef, int const iNo, int const iAddValue)
	{
		struct NotCheckSaveIdx
		{
			bool operator ()(int const& rkVal, CONT_DEF_JOBSKILL_PROBABILITY_BAG::mapped_type const& rkDefProbBag)
			{
				return true;
			}
		};

		return GetOne< int const, NotCheckSaveIdx >(0, rkContDef, iNo, iAddValue);
	}
};