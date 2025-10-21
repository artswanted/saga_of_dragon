#include "def/items.h"
#include <libcsv/csv.h>
#include <tinyxml/tinyxml.h>
#include <Lohengrin/dbtables.h>
#include <BM/localmgr.h>
#include <Variant/item.h>
#include <Variant/tabledatamanager.h>
#include <Variant/PgDBCache.h>
#include <Variant/PgStringUtil.h>
#include "csvdb/load.h"
#include <Variant/constant.h>

namespace DBCacheUtil
{
	//
	extern std::wstring const kDefaultNationCode;
	extern int iForceNationCode;

	//
	typedef std::list< BM::vstring > CONT_ERROR_MSG;
	extern Loki::Mutex kErrorMutex;
	extern CONT_ERROR_MSG kContErrorMsg;
	extern void AddErrorMsg(BM::vstring const& rkErrorMsg);
	extern bool DisplayErrorMsg();

	//
	extern std::wstring const kKeyPrifix;

	extern bool IsDefaultNation(std::wstring const& rkNationCodeStr);

	struct ConvertPair
	{
		template< typename _T_LEFT, typename _T_RIGHT >
		ConvertPair(std::pair< _T_LEFT, _T_RIGHT > const& rhs)
		{
			m_kStr << L"(" << rhs.first << L", " << rhs.second << L")";
		}
		operator std::wstring const& () const
		{
			return m_kStr;
		}
		BM::vstring m_kStr;
	};
	struct ConvertTriple
	{
		template< typename _T_K1, typename _T_K2, typename _T_K3 >
		ConvertTriple(tagTripleKey< _T_K1, _T_K2, _T_K3 > const& rhs)
		{
			m_kStr << L"(K1:" << rhs.kPriKey << L", K2:" << rhs.kSecKey << L", K3:" << rhs.kTrdKey << L")";
		}
		operator std::wstring const& () const
		{
			return m_kStr;
		}
		BM::vstring m_kStr;
	};
	struct ConvertDefenceAddMonsterKey
	{
		ConvertDefenceAddMonsterKey(CONT_DEF_DEFENCE_ADD_MONSTER::key_type const& rhs)
		{
			m_kStr << L"Key(GroupNo:" << rhs.iAddMonster_GroupNo << L", SuccessCount:" << rhs.iSuccess_Count << L")";
		}
		operator std::wstring const& () const
		{
			return m_kStr;
		}
		BM::vstring m_kStr;
	};
	struct AddError
	{
		AddError()
		{
		}
		void operator()(BM::vstring const& rhs)
		{
			AddErrorMsg(rhs);
		}
	};
}

static const char* ITEM_TABLES[] =
{
	"Table/DR2_Def/TB_DefItem.csv",
	"Table/DR2_Def/TB_DefItem_home.csv",
	"Table/DR2_Def/TB_DefItem_Pet.csv",
};

bool csvdb::def::load_items(const char* dataPath)
{
	BYTE cIsCantShareRental = 0;
	BM::vstring vstrNationCodeStr;
	DBCacheUtil::PgNationCodeHelper< CONT_DEFITEM::key_type, CONT_DEFITEM::mapped_type, CONT_DEFITEM > kNationCodeUtil( L"Duplicate ItemNo[" DBCACHE_KEY_PRIFIX L"]" );

	size_t size = _countof(ITEM_TABLES);
	for(size_t i = 0; i < size; ++i)
	{
		BM::Stream::STREAM_DATA buff;
		CsvParser csv;
		csvdb::load(buff, csv, (BM::vstring(dataPath) << ITEM_TABLES[i]));

		while(csv.next_row()/*skip header*/)
		{
			CONT_DEFITEM::mapped_type element;
			vstrNationCodeStr = csv.col(0); // f_NationCodeStr
			element.ItemNo = csv.col_int(1); /*ItemNo*/
			element.NameNo = csv.col_int(2); /*Name*/
			element.ResNo = csv.col_int(3); /*ResNo*/
			element.sType = csv.col_int(5); /*Type*/
			element.iPrice = csv.col_int(6); /*Price*/
			element.iSellPrice = csv.col_int(7); /*SellPrice*/
			element.iAttribute = csv.col_int(8); /*Attribute*/
			element.byGender = csv.col_int(9); /*Gender*/
			element.sLevel = csv.col_int(10); /*LevelLimit*/
			element.i64ClassLimit = HumanAddFiveJobClassLimit(csv.col_int64(11)); //ClassLimit
			element.i64ClassLimitDisplayFilter = csv.col_int64(12); //f_ClassLimitDisplayFilter
			element.i64DraClassLimit = csv.col_int64(13); //DraClassLimit
			element.i64DraClassLimitDisplayFilter = csv.col_int64(14); //f_DraClassLimitDisplayFilter
			for (int i = 0; i < MAX_ITEM_ABIL_LIST; i++)
				element.aAbil[i] = csv.col_int(15 + i);
			element.iOrder1 = csv.col_int(25/*+MAX_ITEM_ABIL_LIST*/); // Order1
			element.iOrder2 = csv.col_int(26); // Order2
			element.iOrder3 = csv.col_int(27); // Order3
			element.iCostumeGrade = csv.col_int(28); // CostumeGrade
			cIsCantShareRental = csv.col_int(29); // f_IsCantShareRental
			if (cIsCantShareRental != 0)
				element.iAttribute |= ICMET_Cant_UseShareRental;

			kNationCodeUtil.Add(vstrNationCodeStr, element.ItemNo, element, __FUNCTIONW__, __LINE__);
		}

	}

	if( !kNationCodeUtil.IsEmpty() )
	{
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}