#include <CEL/DBWorker_Base.h>
#include <BM/Stream.h>

static bool Q_DQT_DEF_ITEM_SET( const char* pkTbPath )
{
    BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    csvdb::load(buff, csv, BM::vstring(pkTbPath) << "TB_DefItemSet.csv");
    const size_t f_NationCodeStr = csv.col_idx("f_NationCodeStr");
    const size_t f_SetNo = csv.col_idx("SetNo");
    const size_t f_Name = csv.col_idx("Name");
    const size_t f_SetAbil01 = csv.col_idx("SetAbil01");
    const size_t f_NeedItem01 = csv.col_idx("NeedItem01");

	DBCacheUtil::PgNationCodeHelper< CONT_DEF_ITEM_SET::key_type, CONT_DEF_ITEM_SET::mapped_type, CONT_DEF_ITEM_SET > kNationCodeUtil( L"Duplicate ItemSet[" DBCACHE_KEY_PRIFIX L"] Reward Info" );
	while( csv.next_row() )
	{
		CONT_DEF_ITEM_SET::mapped_type element;
		std::wstring kNationCodeStr = UNI(csv.col(f_NationCodeStr));
        element.iSetNo = csv.col_int(f_SetNo);
        element.iNameNo = csv.col_int(f_Name);

		for(int i = 0; MAX_ITEM_SET_ABIL > i ; i++)
		{
            element.aiAbilNo[i] = csv.col_int(f_SetAbil01 + i);
		}

		for(int i = 0; MAX_ITEM_SET_NEED_ITEM_ARRAY > i ; i++)
		{
            element.aiNeedItemNo[i] = csv.col_int(f_NeedItem01 + i);
		}

		kNationCodeUtil.Add(kNationCodeStr, element.iSetNo, element, __FUNCTIONW__, __LINE__);
	}

	if( !kNationCodeUtil.IsEmpty() )
	{
		g_kTblDataMgr.SetContDef(kNationCodeUtil.GetResult());
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"TABLE [TB_DefItemSet] is row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
