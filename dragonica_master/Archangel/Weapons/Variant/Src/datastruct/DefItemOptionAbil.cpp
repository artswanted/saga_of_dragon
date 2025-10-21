#include <CEL/DBWorker_Base.h>
#include <BM/Stream.h>

static bool Q_DQT_DEF_ITEM_OPTION_ABIL( const char* pkTbPath )
{
	std::wstring NationCodeStr;

	CONT_TBL_DEF_ITEM_OPTION_ABIL DefaultMap;
	CONT_TBL_DEF_ITEM_OPTION_ABIL map;

    BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    csvdb::load(buff, csv, BM::vstring(pkTbPath) << "TB_DefItemOptionAbil.csv");
    const size_t f_NationCodeStr = csv.col_idx("f_NationCodeStr");
    const size_t f_OptionAbilNo = csv.col_idx("OptionAbilNo");
    const size_t f_PieceCount = csv.col_idx("f_PieceCount");
    const size_t f_Order = csv.col_idx("f_Order");
    const size_t f_Type01 = csv.col_idx("Type01");
    const size_t f_Value01 = csv.col_idx("Value01");

	int iIDX = 0;
	while( csv.next_row() )
	{
		int iOptionAbilNo = 0;
		CONT_TBL_DEF_ITEM_OPTION_ABIL_PIECE::value_type element;

		NationCodeStr = UNI(csv.col(f_NationCodeStr));
        iOptionAbilNo = csv.col_int(f_OptionAbilNo);
        element.iPieceCount = csv.col_int(f_PieceCount);
        element.iOrder = csv.col_int(f_Order);

		for (int i=0; i<MAX_OPTION_ABIL_ARRAY; i++)
		{
            element.aiType[i] = csv.col_int(f_Type01 + (i << 1));
            element.aiValue[i] = csv.col_int(f_Value01 + (i << 1));
		}

		if( DBCacheUtil::IsDefaultNation(NationCodeStr) )
		{
			CONT_TBL_DEF_ITEM_OPTION_ABIL::iterator find_iter = DefaultMap.find(iOptionAbilNo);
			if( DefaultMap.end() == find_iter )
			{
				auto kRet = DefaultMap.insert( std::make_pair(iOptionAbilNo, CONT_TBL_DEF_ITEM_OPTION_ABIL::mapped_type(iOptionAbilNo)) );
				find_iter = kRet.first;
			}
			if( DefaultMap.end() != find_iter )
			{
				(*find_iter).second.kContPieceAbil.push_back(element);
				(*find_iter).second.kContPieceAbil.sort();
			}
			//map.insert( std::make_pair(iOptionAbilNo, element) );
		}
		else
		{
			if( DBCacheUtil::IsCanNation< CONT_TBL_DEF_ITEM_OPTION_ABIL::key_type, BM::vstring, DBCacheUtil::AddError >(NationCodeStr, iOptionAbilNo, __FUNCTIONW__, __LINE__) )
			{
				CONT_TBL_DEF_ITEM_OPTION_ABIL::iterator find_iter = map.find(iOptionAbilNo);
				if( map.end() == find_iter )
				{
					auto kRet = map.insert( std::make_pair(iOptionAbilNo, CONT_TBL_DEF_ITEM_OPTION_ABIL::mapped_type(iOptionAbilNo)) );
					find_iter = kRet.first;
				}
				if( map.end() != find_iter )
				{
					(*find_iter).second.kContPieceAbil.push_back(element);
					(*find_iter).second.kContPieceAbil.sort();
				}
			}
		}
	}

	if( map.size() || DefaultMap.size() )
	{
		// �ߺ� Ȯ���ؼ� �о� �־�� ��.
		CONT_TBL_DEF_ITEM_OPTION_ABIL::iterator default_iter = DefaultMap.begin();
		for( ; default_iter != DefaultMap.end() ; ++default_iter )
		{
			// Ű ��( iOptionAbilNo )�� ã�ƺ� ������,
			CONT_TBL_DEF_ITEM_OPTION_ABIL::iterator find_iter = map.find( default_iter->first );
			if( find_iter == map.end() )
			{ // ������ �׳� insert�ϰ� ��.
				map.insert( *default_iter );
			}
			else
			{ // ������,
				CONT_TBL_DEF_ITEM_OPTION_ABIL::mapped_type & DefaultData = default_iter->second;
				CONT_TBL_DEF_ITEM_OPTION_ABIL::mapped_type & FindData = find_iter->second;

				CONT_TBL_DEF_ITEM_OPTION_ABIL_PIECE::iterator defaultdata_iter = DefaultData.kContPieceAbil.begin();
				for( ; defaultdata_iter != DefaultData.kContPieceAbil.end() ; ++defaultdata_iter )
				{ // ���� ���� ��ȣ�� �ִ��� ã�Ƽ�.
					CONT_TBL_DEF_ITEM_OPTION_ABIL_PIECE::iterator order_iter = 
						std::find_if(
						FindData.kContPieceAbil.begin(),
						FindData.kContPieceAbil.end(),
						TBL_DEF_ITEM_OPTION_ABIL_PIECE::TBL_DEF_ITEM_OPTION_ABIL_PIECE_COMPARE_FUNCTOR(defaultdata_iter->iOrder, defaultdata_iter->iPieceCount)
						);
					if( order_iter == FindData.kContPieceAbil.end() )
					{ // ������ ����.
						FindData.kContPieceAbil.push_back( *defaultdata_iter );
						FindData.kContPieceAbil.sort();
					}
				}
			}
		}
		DefaultMap.clear();

		g_kTblDataMgr.SetContDef(map);
		return true;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"row count 0");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
