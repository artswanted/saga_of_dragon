#include "stdafx.h"
#include "CEL/Common.h"
#include "CEL/CoreCenter.h"
#include "CEL/Header/Timer.h"
#include "CEL/Header/DBWorker.h"
#include "Cel_Log.h"

using namespace CEL;

tagAddrInfo::tagAddrInfo()
{
	Clear();
}

tagAddrInfo::tagAddrInfo(std::wstring const& wstrInIP, WORD const wInPort)
{
	Set(wstrInIP, wInPort);
}

bool tagAddrInfo::IsCorrect()const
{
	if(!ip.S_un.S_addr)
	{
		return false;
	}

	if( 0 == IP().length() )
	{
		return false;
	}

	if( 0 == wPort )
	{
		return false;
	}
	return true;
}

bool tagAddrInfo::IsCorrectSubNetMask( std::wstring const &wstrBaseIP, std::wstring const &wstrSubNetMask )const
{
	if ( wstrBaseIP.size() )
	{
		in_addr addrBaseIP;
		addrBaseIP.s_addr = ::inet_addr( MB(wstrBaseIP) );
		
		in_addr addrSubNetMask;
		if ( wstrSubNetMask.size() )
		{
			addrSubNetMask.s_addr = ::inet_addr( MB(wstrSubNetMask) );
		}
		else
		{
			addrSubNetMask.S_un.S_addr = 0;
		}

		return IsCorrectSubNetMask( addrBaseIP, addrSubNetMask );
	}
	return false;
}

bool tagAddrInfo::IsCorrectSubNetMask( in_addr const &addrBaseIP, in_addr const &addrSubNetMask )const
{
	in_addr chk_ip = ip;
	chk_ip.S_un.S_addr &=  addrSubNetMask.S_un.S_addr;
	return addrBaseIP.S_un.S_addr == chk_ip.S_un.S_addr;
}

void tagAddrInfo::Set(std::wstring const& wstrInIP, WORD const wInPort)
{
	if( wstrInIP.size() )
	{
		ip.s_addr = ::inet_addr( MB(wstrInIP ) );
	}

	wPort = wInPort;
}

void tagAddrInfo::Clear()
{
	ip.s_addr = 0;
	wPort = 0;
}

bool tagAddrInfo::operator < (const tagAddrInfo &rhs) const
{
	if( ip.S_un.S_addr < rhs.ip.S_un.S_addr ){return true;}
	if( ip.S_un.S_addr > rhs.ip.S_un.S_addr ){return false;}

	if( wPort < rhs.wPort ){return true;}
	if( wPort > rhs.wPort ){return false;}

	return false;
}

bool tagAddrInfo::operator == (const tagAddrInfo &rhs) const
{
	if(	ip.S_un.S_addr	==	rhs.ip.S_un.S_addr
	&&	wPort	==	rhs.wPort )
	{
		return true;
	}
	return false;
}

tagInitCoreDesc::tagInitCoreDesc()
{
	kOrderGuid.Generate();
	bIsAllow = false;

	OnSessionOpen	= NULL;
	OnDisconnect	= NULL;
	OnRecv			= NULL;

	dwPackerBufferSize	= E_DEFAULT_PACKET_BUFFER_SIZE;
	dwBandwidthLimit	= E_DEFAULT_PACKET_BUFFER_SIZE;

	bIsImmidiateActivate	= false;
	ServiceHandlerType(SHT_DEFAULT);
	IdentityValue(0);

	SYSTEM_INFO kSystemInfo;
	::GetSystemInfo(&kSystemInfo);

	dwThreadCount			= (kSystemInfo.dwNumberOfProcessors *2 +1);
}

void tagInitCoreDesc::WriteToPacket(BM::Stream &kPacket)const
{
	kPacket.Push(bIsAllow);
	kPacket.Push(kOrderGuid);
	kBindAddr.WriteToPacket(kPacket);
	kNATAddr.WriteToPacket(kPacket);

	kPacket.Push(dwPackerBufferSize);//! 세션당 패킷 버퍼 사이즈
	kPacket.Push(nMaxPacketLen);//! 한개의 패킷이 가질 수 있는 최대 길이. //->이거는 헤더가 int 가 되면 쓴다.
	kPacket.Push(bIsImmidiateActivate);
	kPacket.Push(dwThreadCount);//! 해당 요소의 스레드 갯수
	kPacket.Push(m_kServiceHandlerType);//기본값 0
	kPacket.Push(dwBandwidthLimit);

	kPacket.Push(m_kIdentityValue);//기본값 0

	kPacket.Push(OnSessionOpen);
	kPacket.Push(OnDisconnect);
	kPacket.Push(OnRecv);
}

void tagInitCoreDesc::ReadFromPacket(BM::Stream &kPacket)
{
	kPacket.Pop(bIsAllow);
	kPacket.Pop(kOrderGuid);
	kBindAddr.ReadFromPacket(kPacket);
	kNATAddr.ReadFromPacket(kPacket);

	kPacket.Pop(dwPackerBufferSize);//! 세션당 패킷 버퍼 사이즈
	kPacket.Pop(nMaxPacketLen);//! 한개의 패킷이 가질 수 있는 최대 길이. //->이거는 헤더가 int 가 되면 쓴다.
	kPacket.Pop(bIsImmidiateActivate);
	kPacket.Pop(dwThreadCount);//! 해당 요소의 스레드 갯수
	kPacket.Pop(m_kServiceHandlerType);//기본값 0
	kPacket.Pop(dwBandwidthLimit);

	kPacket.Pop(m_kIdentityValue);//기본값 0

	kPacket.Pop(OnSessionOpen);
	kPacket.Pop(OnDisconnect);
	kPacket.Pop(OnRecv);
}

bool tagInitCoreDesc::IsCorrect()const
{
	if(!OnSessionOpen){return false;}
	if(!OnDisconnect){return false;}
	if(!OnRecv){return false;}

//	if(!BindAddr.wstrIP.size()) {return false;}//! 커넥터는 없을 수도 있음.
//	if(!BindAddr.wPort) {return false;}
	return true;
}

tagInitDBDesc::tagInitDBDesc()
{
	kOrderGuid.Generate();

	bUseConsoleLog = true;
	OnDBExecute = NULL;
	OnDBExecuteTran = NULL;
	bIsImmidiate = false;

	SYSTEM_INFO kSystemInfo;
	::GetSystemInfo(&kSystemInfo);

	iRunType = 2;
	dwWorkerCount			= 3;//(kSystemInfo.dwNumberOfProcessors *2 +1);//디비 스레드는 그렇게 많을 필요는 없음.

	dwConnectionPoolCount = E_DEFAULT_CONN_POOL_COUNT;

	m_kLogFolder = _T("./DB_Worker_Log");
}

void tagInitDBDesc::WriteToPacket(BM::Stream &kPacket)const
{
	kPacket.Push(bIsImmidiate);
	kPacket.Push(bUseConsoleLog);
	
	kPacket.Push(kOrderGuid);

	kPacket.Push(OnDBExecute);
	kPacket.Push(OnDBExecuteTran);
	kPacket.Push(dwWorkerCount);
	kPacket.Push(iRunType);
	
	kPacket.Push(kDBInfo);

	kPacket.Push(m_kLogFolder);
}

void tagInitDBDesc::ReadFromPacket(BM::Stream &kPacket)
{
	kPacket.Pop(bIsImmidiate);
	kPacket.Pop(bUseConsoleLog);
	
	kPacket.Pop(kOrderGuid);

	kPacket.Pop(OnDBExecute);
	kPacket.Pop(OnDBExecuteTran);
	kPacket.Pop(dwWorkerCount);
	kPacket.Pop(iRunType);

	kPacket.Pop(kDBInfo);

	kPacket.Pop(m_kLogFolder);
}

tagDBResult::tagDBResult(DB_QUERY const& rkOrder):DB_QUERY(rkOrder)
{
	eRet			= DR_NONE;
	vecArray.clear();
	vecResultCount.clear();
}

tagDBResult::~tagDBResult()
{
}

void DB_RESULT::ReadFromPacket(BM::Stream & kPacket)
{
	DB_QUERY::ReadFromPacket(kPacket);
	kPacket.Pop(eRet);
	size_t iSize = 0;
	kPacket.Pop(iSize);
	DB_DATA_ARRAY kList(iSize);
	DB_DATA_ARRAY::iterator itor_Array = kList.begin();
	while (kList.end() != itor_Array)
	{
		(*itor_Array).ReadFromPacket(kPacket);
		++itor_Array;
	}
	vecArray.swap(kList);
	kPacket.Pop(vecResultCount);
}

void DB_RESULT::WriteToPacket(BM::Stream & kPacket) const
{
	DB_QUERY::WriteToPacket(kPacket);
	kPacket.Push(eRet);
	kPacket.Push(vecArray.size());
	DB_DATA_ARRAY::const_iterator itor_Array = vecArray.begin();
	while (vecArray.end() != itor_Array)
	{
		(*itor_Array).WriteToPacket(kPacket);
		++itor_Array;
	}
	kPacket.Push(vecResultCount);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tagDBData::tagDBData()
{
	DataType = DBTYPE_EMPTY;
	m_vecData.reserve( sizeof( BM::GUID ) );
}

bool tagDBData::PopMemory(void *pBuffer, size_t const out_size)const
{
	if( out_size
	&&	m_vecData.size() == out_size)
	{
		::memcpy( pBuffer, &m_vecData.at(0), out_size );
		return true;
	}

	memset(pBuffer,0,out_size);	// 만약 사이즈가 달라서 값을 가져 오지 못하면 출력될 버퍼를 0 으로 초기화 시켜 준다.
								// 이 문제는 테이블 필드에 NULL 허용시 발생 하거나 출력 버퍼와 데이터의 타입 사이즈가 틀릴때 발생한다.

	if(m_vecData.size())
	{
		CEL_LOG( BM::LOG_LV0, _T("mismatch DataType DataSize =") << m_vecData.size());
		assert(NULL && "tagDBData::PopMemory Failed");
	}
	return false;
}

bool tagDBData::Pop(std::string &rElement) const
{
	rElement.clear();	// 스트링 필드가 NULL 허용일 경우 데이터 갱신이 되지 않거나 이전 데이터를 리턴 하게 된다.
						// 그것을 방지 하기 위해서 들어온 스트링은 무조건 초기화 시킨다.

	if( DBTYPE_STR == DataType 
	&& m_vecData.size())
	{
		rElement.append( &m_vecData.at(0), m_vecData.size() );
		return true;
	}
	return false;
}

bool tagDBData::Pop(std::wstring &rElement) const
{
	rElement.clear();	// 스트링 필드가 NULL 허용일 경우 데이터 갱신이 되지 않거나 이전 데이터를 리턴 하게 된다.
						// 그것을 방지 하기 위해서 들어온 스트링은 무조건 초기화 시킨다.

	if( DBTYPE_WSTR == DataType 
	&& m_vecData.size() )
	{
		wchar_t* pwStr  = (wchar_t*)&m_vecData.at(0);
		rElement.append( pwStr, m_vecData.size()/sizeof(std::wstring::value_type) );
		return true;
	}
	return false;
}

void tagDBData::Push( void const *pData, size_t const in_size )
{
	if( 0 < in_size )
	{
		m_vecData.reserve( in_size );
		m_vecData.resize( in_size );
		::memcpy( &m_vecData.at(0), pData, in_size );
	}
}

void DB_DATA::ReadFromPacket(BM::Stream & kPacket)
{
	kPacket.Pop(DataType);
	kPacket.Pop(m_vecData);
}

void DB_DATA::WriteToPacket(BM::Stream & kPacket) const
{
	kPacket.Push(DataType);
	kPacket.Push(m_vecData);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tagDBQuery::tagDBQuery(tagDBQuery const&  rhs)
{
	Set(rhs.DBIndex(), rhs.QueryType(), rhs.wstrQuery);

	contUserData = rhs.contUserData;

	QueryGuid(rhs.QueryGuid());//! 쿼리 자체의 GUID 
	QueryOwner(rhs.QueryOwner());//! 쿼리 날린자 혹은 기타에 사용되는 OwnerGuid
		
	bHaveReturn = rhs.bHaveReturn;
	m_kStrParamCount = rhs.m_kStrParamCount;
	m_kContQueryTarget = rhs.m_kContQueryTarget;
	m_kQueryResultCallback = rhs.m_kQueryResultCallback;
}

tagDBQuery::tagDBQuery(int const InDBIndex, int const InType, std::wstring const& wstrInQuery)
{
	Set(InDBIndex, InType, wstrInQuery);
}

void tagDBQuery::Set(int const InDBIndex, int const InType, std::wstring const& wstrInQuery)
{
	m_kStrParamCount = 0;

	m_kQueryGuid.Generate();
	StartTime(BM::GetTime32());
	m_kDBIndex = InDBIndex;
	m_kQueryType = InType;
	wstrQuery = wstrInQuery;
	bHaveReturn = false;
	m_kQueryResultCallback = NULL;

	if(wstrInQuery.empty())
	{
		return;
	}

	size_t kstrLen = wstrInQuery.length();

	wchar_t * pstrBuffer = new wchar_t[kstrLen+1];
	if(!pstrBuffer)
	{
		return;
	}
	::wcsncpy_s( pstrBuffer, kstrLen+1, wstrInQuery.c_str(), kstrLen );
	pstrBuffer[kstrLen] = 0;

	::_wcsupr_s( pstrBuffer, kstrLen+1 );

	wchar_t wstrSeps[] = L" .[],?()={}";
	wchar_t * next_context = NULL;
	wchar_t * token = wcstok_s( pstrBuffer, wstrSeps, &next_context );

	while(token)
	{
		if(!wcscmp(L"CALL",token))
		{
			bHaveReturn = true;
			token = wcstok_s( NULL, wstrSeps, &next_context );
			continue;
		}

		if(!wcscmp(L"EXEC",token) || !wcscmp(L"DBO",token))
		{
			token = wcstok_s( NULL, wstrSeps, &next_context );
			continue;
		}

		wstrProc = token;
		break;
	}

	SAFE_DELETE_ARRAY(pstrBuffer);
}

void tagDBQuery::StrConvForQuery(std::wstring const& strOrg, std::wstring &strOut)
{
	std::wstring::size_type offset = 0;
	std::wstring::size_type start = 0;
	std::wstring::size_type end = 0;
	while(true)
	{	
		offset = strOrg.find(_T('\''),offset);
		
		if(std::wstring::npos == offset)
		{
			std::wstring const kStrDiv = strOrg.substr(start);
			strOut.append(kStrDiv);
			break;
		}
		else
		{//처리.
			end = offset;

			std::wstring const kStrDiv = strOrg.substr(start, end-start);
			strOut.append(kStrDiv);
			strOut.append(_T("''"));
			
			++offset;//한건 했으니 다음칸.

			start = end+1;
		}
	}
}

void tagDBQuery::PushStrParam( BYTE const *pData, size_t const in_size )
{
	size_t const iSize = in_size * 2 + 2;

	std::wstring wstrTemp(L"0x");
	wstrTemp.resize( iSize + 1 );

	for ( size_t i=0; i<in_size; ++i )
	{
		::swprintf_s( &(wstrTemp.at((i+1)*2)), 3, L"%02x", pData[i] );
	}

	wstrTemp.erase( iSize );
	PushStrParam_sub( wstrTemp );
}

void tagDBQuery::PushStrParam( std::wstring const&  input, bool const bIsNotQuat)
{
	PushStrParam_sub(input);
}

void tagDBQuery::InsertQueryTarget(BM::GUID const& kTarget)
{
	m_kContQueryTarget.insert(kTarget);
}

void tagDBQuery::PushStrParam_sub(std::wstring const&  input)
{
	if(GetStrParamCount())
	{
		wstrQuery += _T(", ");//한칸 떠주는 센스.
	}
	else
	{//처음꺼 같은경우.
		wstrQuery += _T(" ");//그냥 공배.
	}

	wstrQuery += input;
	++m_kStrParamCount;
}

void DB_QUERY::ReadFromPacket(BM::Stream & kPacket)
{
	kPacket.Pop(contUserData.Data());
	contUserData.PosAdjust();
	size_t iRdPos = 0;
	kPacket.Pop(iRdPos);
	contUserData.RdPos(iRdPos);
	kPacket.Pop(m_kStartTime);
	kPacket.Pop(m_kDBIndex);
	kPacket.Pop(m_kQueryType);
	kPacket.Pop(m_kQueryGuid);
	kPacket.Pop(m_kQueryOwner);
	kPacket.Pop(m_kContQueryTarget);
	kPacket.Pop(wstrQuery);
	kPacket.Pop(wstrProc);
	kPacket.Pop(bHaveReturn);
	kPacket.Pop(m_kStrParamCount);
}

void DB_QUERY::WriteToPacket(BM::Stream & kPacket) const
{
	kPacket.Push(contUserData.Data());
	kPacket.Push(contUserData.RdPos());	// DB_QUERY 의 복사개념이므로, 복사에 필요한 모든 정보를 넘긴다.
	kPacket.Push(m_kStartTime);
	kPacket.Push(m_kDBIndex);
	kPacket.Push(m_kQueryType);
	kPacket.Push(m_kQueryGuid);
	kPacket.Push(m_kQueryOwner);
	kPacket.Push(m_kContQueryTarget);
	kPacket.Push(wstrQuery);
	kPacket.Push(wstrProc);
	kPacket.Push(bHaveReturn);
	kPacket.Push(m_kStrParamCount);
}

void DB_QUERY::Add(tagDBQuery const& rkQuery)
{
	wstrQuery += L";\r\n";
	wstrQuery += rkQuery.Command();
}


void tagDB_QUERY_TRAN::BuildQueryTarget()//쿼리타겟 빌드.
{
	CONT_DB_QUERY::const_iterator query_itor = begin();
	while(query_itor != end())
	{
		CONT_DB_QUERY::value_type const& element = (*query_itor);

		CONT_QUERY_TARGET::const_iterator target_itor = element.m_kContQueryTarget.begin();
		while(target_itor != element.m_kContQueryTarget.end())
		{
			m_kContQueryTarget.insert(*target_itor);
			++target_itor;
		}
		++query_itor;
	}
}

void tagRegistTimerDesc::WriteToPacket(BM::Stream &kPacket)const
{
	kPacket.Push(kOrderGuid);
	kPacket.Push(kTimerGuid);//등록된 타이머 핸들

	kPacket.Push(pFunc);
	kPacket.Push(dwInterval);
	kPacket.Push(dwUserData);

	kPacket.Push(dwThreadCount);//타이머당 스레드 하나. 다중 타이머는 숫자 늘리도록.
	kPacket.Push(bIsImmidiate);// 즉시시작?
}

void tagRegistTimerDesc::ReadFromPacket(BM::Stream &kPacket)
{
	kPacket.Pop(kOrderGuid);
	kPacket.Pop(kTimerGuid);//등록된 타이머 핸들

	kPacket.Pop(pFunc);
	kPacket.Pop(dwInterval);
	kPacket.Pop(dwUserData);

	kPacket.Pop(dwThreadCount);//타이머당 스레드 하나. 다중 타이머는 숫자 늘리도록.
	kPacket.Pop(bIsImmidiate);// 즉시시작?
}

tagInitFinalSignal::tagInitFinalSignal()
{
	kIdentity = 0;
	kOrderGuid.Generate();
}

void tagInitFinalSignal::WriteToPacket(BM::Stream &kPacket)const
{
	kPacket.Push(kOrderGuid);
	kPacket.Push(kIdentity);
}

void tagInitFinalSignal::ReadFromPacket(BM::Stream &kPacket)
{
	kPacket.Pop(kOrderGuid);
	kPacket.Pop(kIdentity);
}

tagDB_RESULT_TRAN::tagDB_RESULT_TRAN(DB_RESULT_TRAN const&  rkResult)
{
	*this = rkResult;
}
tagDB_RESULT_TRAN::tagDB_RESULT_TRAN(DB_QUERY_TRAN const&  rkQuery)
{
	QueryOwner(rkQuery.QueryOwner());//! 쿼리 날린자 혹은 기타에 사용되는 OwnerGuid
	QueryType(rkQuery.QueryType());// 쿼리 타입
	DBIndex(rkQuery.DBIndex());//! 쿼리 날릴 대상 DB
	contUserData = rkQuery.contUserData;
}

tagInitCenterDesc::tagInitCenterDesc()
{
	SYSTEM_INFO kSystemInfo;
	::GetSystemInfo(&kSystemInfo);

	eOT = BM::OUTPUT_JUST_TRACE;
	pOnRegist = NULL;
	dwProactorThreadCount = (kSystemInfo.dwNumberOfProcessors *2 +1);
	bIsUseDBWorker = false;
	m_kCelLogFolder = _T("./CEL_Log");
}