#include "StdAfx.h"
#include "PgIPChecker.h"

namespace PgIPCheckerUtil
{
	SIpRange QueryResultToSIPRange(std::wstring const& kStartIP, std::wstring const& kEndIP, std::wstring const& kNationKey, std::wstring const& kNationName)
	{
		in_addr _addr_start;
		in_addr _addr_end;
		_addr_start.s_addr = ::inet_addr( MB(kStartIP) );
		_addr_end.s_addr = ::inet_addr( MB(kEndIP) );

		return SIpRange(PgIPBandFilter::AddrToDWORD(_addr_start), PgIPBandFilter::AddrToDWORD(_addr_end), kNationName, kNationKey);
	}
}

PgIPChecker::PgIPChecker()
{
}

PgIPChecker::~PgIPChecker()
{
}

bool PgIPChecker::Q_DQT_LOAD_IP_FOR_BLOCK( CEL::DB_RESULT &rkResult )
{
	return Q_DQT_LOAD_IP_COMMON(rkResult, true);
}

bool PgIPChecker::Q_DQT_LOAD_IP_FOR_ACCPET( CEL::DB_RESULT &rkResult )
{
	return Q_DQT_LOAD_IP_COMMON(rkResult, false);
}

bool PgIPChecker::Q_DQT_LOAD_IP_COMMON( CEL::DB_RESULT &rkResult , bool const bIsBlockIP)
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed[Result: "<<static_cast<int>(rkResult.eRet)<<L"] DB Load" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgIPBandFilter_Wrapper kTempWrapper;

	bool bRet = true;
	CEL::DB_DATA_ARRAY const &rkVec = rkResult.vecArray;
	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkVec.begin();
	while(rkVec.end() != result_iter)
	{
		std::wstring kStartIP, kEndIP, kNationKey, kNationName;
		
		result_iter->Pop(kStartIP);		++result_iter;
		result_iter->Pop(kEndIP);		++result_iter;
		result_iter->Pop(kNationKey);	++result_iter;
		result_iter->Pop(kNationName);	++result_iter;
		
		SIpRange const kRange(PgIPCheckerUtil::QueryResultToSIPRange(kStartIP, kEndIP, kNationKey, kNationName));

		SIpRange kRangeForCheck;
		if(!kTempWrapper.RegIPRange(kRange, bIsBlockIP, kRangeForCheck))
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<< L"IncorrectRange New: "<< kStartIP << _COMMA_ << kEndIP);
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<< L"IncorrectRange Org: "<< kRangeForCheck.start_ << _COMMA_ << kRangeForCheck.end_);
		}
	}

	if(bRet)
	{
		kTempWrapper.Build(bIsBlockIP);//빌드를 반드시 걸고. 비교 및 셋팅을 해야함.

		PgIPBandFilter::CONT_IP kPrevCont;
		PgIPBandFilter::CONT_IP kNextCont;
		GetCont(bIsBlockIP, kPrevCont);
		kTempWrapper.GetCont(bIsBlockIP, kNextCont);

		if(kPrevCont != kNextCont)
		{
			SetCont(bIsBlockIP, kNextCont);
		}
	}

	return bRet;
}

bool PgIPChecker::Q_DQT_ADD_IP_FOR_ACCEPT( CEL::DB_RESULT &rkResult )
{
	if( CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Failed[Result: "<<static_cast<int>(rkResult.eRet)<<L"] DB Load" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}