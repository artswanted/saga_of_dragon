
template< typename _T_NC_LOG_STRUCT >
HRESULT PgNcLog::WriteLog(BM::DBTIMESTAMP_EX const &kNowTime, PgLogCont const &kLogCont, PgLog const* const pkLog, ConvertLog2NC::CONT_DEF_CONVERT_LOG2NC::mapped_type const& rkMapped)
{
 	// Filling NcLog
	_T_NC_LOG_STRUCT kNcLog(rkMapped.iNcLogID);
	ConvertLog2NC::CONT_NC_LOG_FIELD::const_iterator field_iter = rkMapped.kContLogField.begin();
	while( rkMapped.kContLogField.end() != field_iter )
	{
		ConvertLog2NC::CONT_NC_LOG_FIELD::key_type const& iTargetField = (*field_iter).first;
		ConvertLog2NC::CONT_NC_LOG_FIELD::mapped_type const& rkFieldValue = (*field_iter).second;

		if( ConvertLog2NC::iSingleValue == rkFieldValue.size() )
		{
			int const iSourceColumnIdx = rkFieldValue.front();
			ConvertLog2NC::ColumnValue< _T_NC_LOG_STRUCT >(kLogCont, *pkLog, iSourceColumnIdx, iTargetField, NCLogUtil::SSetNCCoulumn< _T_NC_LOG_STRUCT >(kNcLog));
		}
		else
		{
			BM::vstring kTempStr;
			ConvertLog2NC::CONT_DR_LOG_INDEX::const_iterator value_iter = rkFieldValue.begin();
			while( rkFieldValue.end() != value_iter )
			{
				int const iSourceColumnIdx = (*value_iter);
				ConvertLog2NC::ColumnValue< _T_NC_LOG_STRUCT >(kLogCont, *pkLog, iSourceColumnIdx, iTargetField, NCLogUtil::SMergerCoulumn(kTempStr));
				++value_iter;
			}
			std::wstring const& rkString = kTempStr;
			kNcLog.SetColumn(iTargetField, rkString);
		}

		++field_iter;
	}
	if( m_bInternalTest )
	{
		TestLog(kNowTime, kNcLog);
	}
	return WriteLog(kNowTime, kNcLog);
}