/* 참고사항
SO_LINGER를 설정하지 않으면 1번 
링거옵션을 켜고, 시간이 0 이면 2번
링거옵션을 켜고, 일정시간을 주면 3번
1) closesocket()함수는 곧바로 리턴하고 송신버퍼의 데이터는 백그라운드로 보낸후 TCP연결을 정상종료한다.
2) closesocket()함수는 곧바로 리턴하고 송신버퍼의 데이터는 삭제한후 TCP연결을 강제종료한다.
3) 송신버퍼의 데이터를 모두 보내고, TCP연결을 정상종료후 closesocket()함수가 리턴한다
*/

template< typename T_SESSION >
Service_Handler_NC< T_SESSION >::Service_Handler_NC(INIT_CORE_DESC const& kInitInfo)
//	:	m_kRecvHeadBlk(4096)//초기사이즈
	:m_kInitInfo(kInitInfo)
{
	m_kIsDoWrite = false;
	m_kIsDoRead = false;

	MyHandle(ACE_INVALID_HANDLE);//closesocket용 저장.
	handle(ACE_INVALID_HANDLE);//ace 용

	m_kCloseCause = E_R_NONE;

	m_pRecvHeadBlk = NULL;

	m_kSendQueue.high_water_mark(kInitInfo.dwPackerBufferSize);//! 2000K 까지 버텨준다.
	m_kSendQueue.low_water_mark(0);//! 500K 까지 버텨준다.

	m_kDosCheckTime = ACE_OS::gettimeofday();
	m_kLastNetworkActTime = ACE_OS::gettimeofday();
	m_iRecvBytesForDosCheck = 0;
}

template< typename T_SESSION >
Service_Handler_NC< T_SESSION >::~Service_Handler_NC(void)
{
	IG_LOG LockCheck( m_kDebugLog, kCheck );

	Clear();//보내던 블럭들도 다 제거.

	if(MyHandle() != ACE_INVALID_HANDLE)
	{
		ACE_OS::closesocket( MyHandle() );//소켓은 소멸될때 제거
	}
}

template< typename T_SESSION >
void Service_Handler_NC< T_SESSION >::Clear()
{
	IG_LOG LockCheck( m_kDebugLog, kCheck );
	//std::cout<< __FUNCTION__<<":"<<__LINE__ <<":"<< this <<std::endl;
	ClearBlk(m_pRecvHeadBlk);

	while(!m_kSendQueue.is_empty())
	{
		MSG_BLK* pMsgBlk = NULL;
		ACE_Time_Value tv(0, 100);	tv += ACE_OS::gettimeofday();
		int const ret = m_kSendQueue.dequeue( pMsgBlk, &tv );
		if(-1 != ret)
		{
			ClearBlk(pMsgBlk);
		}
	}
}

template< typename T_SESSION >
HRESULT Service_Handler_NC< T_SESSION >::NewBlk(MSG_BLK* &pkBlk, size_t const blk_size)const
{
	IG_LOG LockCheck( m_kDebugLog, kCheck );

	//std::cout<< __FUNCTION__<<":"<<__LINE__ <<":"<< this <<std::endl;
	pkBlk =	new MSG_BLK(blk_size);
	if(pkBlk)
	{
		return S_OK;
	}
	return E_FAIL;
}

template< typename T_SESSION >
void Service_Handler_NC< T_SESSION >::ClearBlk(MSG_BLK* &pkBlk)const
{
	IG_LOG LockCheck( m_kDebugLog, kCheck );
	//std::cout<< __FUNCTION__<<":"<<__LINE__ <<":"<< this <<std::endl;
	while(pkBlk)
	{
		MSG_BLK* pkContBlk = pkBlk->cont();
		
		pkBlk->cont(NULL);//잘라내고
		SAFE_DELETE(pkBlk);//삭제.

		pkBlk = pkContBlk;
	}
}

template< typename T_SESSION >
void Service_Handler_NC< T_SESSION >::AdjustBlk(MSG_BLK* &pkBlk)const
{
	IG_LOG LockCheck( m_kDebugLog, kCheck );
	//std::cout<< __FUNCTION__<<":"<<__LINE__ <<":"<< this <<std::endl;
	while(pkBlk)
	{
		MSG_BLK* pkContBlk = pkBlk->cont();
		
		if(pkBlk->length())
		{//덜뽑았으면 스탑.
			break;	
		}
		else
		{
			pkBlk->cont(NULL);//잘라내고
			SAFE_DELETE(pkBlk);//삭제.
			pkBlk = pkContBlk;
		}
	}
}

template< typename T_SESSION >
void Service_Handler_NC< T_SESSION >::open(ACE_HANDLE new_handle, MSG_BLK &message_block)
{
	IG_LOG LockCheck( m_kDebugLog, kCheck );
	
	MyHandle(new_handle);
	//OnSessionOpen 전에 셋팅될 값
	ACE_INET_Addr const& addr = RemoteAddr();
	sockaddr_in* pkSockAddr = static_cast<sockaddr_in*>(addr.get_addr());
	if(NULL == pkSockAddr)
	{
		TryClose(E_ONCLOSE_BY_OPEN);
		return;
	}

	ADDR_INFO kTempAddr;
	kTempAddr.ip = pkSockAddr->sin_addr;
	kTempAddr.wPort = addr.get_port_number();

	Addr(kTempAddr);

	if(m_kSessionKey.SessionGuid() == BM::GUID::NullData())
	{//세션 GUID 할당 못받았으면.
		m_kSessionKey.SessionGuid(BM::GUID::Create());
	}

	m_kLastNetworkActTime = ACE_OS::gettimeofday();

	VNotify(NC_OPEN);//세션 등록.

//	E_RESULT_CODE eRet = E_R_NONE;
	ACE_SOCK_Stream peer(new_handle);
	int bufsiz = ACE_DEFAULT_MAX_SOCKET_BUFSIZ;
	peer.set_option(SOL_SOCKET, SO_SNDBUF, &bufsiz, sizeof(bufsiz) );

	static int i = 1;
	peer.set_option(IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i) );//! ! 네갈 Negal 지연없이 전송. 하지만 네트웍 비용 증가.

	linger lg_optval; 
	lg_optval.l_onoff = 1;
	lg_optval.l_linger = 10;
	peer.set_option(SOL_SOCKET, SO_LINGER, (char*)&lg_optval, sizeof(lg_optval));

	ACE_Sig_Action no_sigpipe( (ACE_SignalHandler)SIG_IGN );
	no_sigpipe.register_action( SIGPIPE, 0 );//! ! write 명령이 프로그램 취소를 않도록.

	if(T_SESSION::VOpen())
	{
		if( -1 == m_kWriter.open( *this, new_handle, 0, this->proactor() ) )
		{
//			eRet = E_OPEN_INIT_WRITER_FAILED;
			goto __OPEN_FAILED; 
		}

		if( -1 == m_kReader.open( *this, new_handle, 0, this->proactor() ) )
		{
//			eRet = E_OPEN_INIT_READER_FAILED;
			goto __OPEN_FAILED; 
		}

		CEL_LOG( BM::LOG_LV6, L"SessionOpen " << m_kAddr.ToString());
		m_kInitInfo.OnSessionOpen( this );//당연히 있어야 하는거라 NULL체크 안함

		BM::CAutoMutex kFlagLock(m_kFlagMutex);
		IsDoRead(true);//리드 시작
		if( !PostRead() )//! ! open을 했는데 바로 Close 가 호출 되는 어이 없는 상황!.
		{
			IsDoRead(false);
//			eRet = E_OPEN_TRY_POST_READ_FAILED;
			goto __OPEN_FAILED; 
		}
		
		return;
	}

	goto __OPEN_FAILED;
__OPEN_FAILED:
	{
		TryClose(E_ONCLOSE_BY_OPEN);
	}
}

template< typename T_SESSION >
bool Service_Handler_NC< T_SESSION >::PostWrite( MSG_BLK *pMsgBlk )//이걸 쓰는곳에서 WriteLock을 걸고 불러라.
{
	IG_LOG LockCheck( m_kDebugLog, kCheck );
	E_RESULT_CODE eRet = E_R_NONE;

	if(!pMsgBlk)
	{
		if(!m_kSendQueue.is_empty())//! ! 보낼것이 있다.
		{
			MSG_BLK* pT1 = NULL;
			ACE_Time_Value tv(0, 100);	tv += ACE_OS::gettimeofday();
			int const ret = m_kSendQueue.dequeue( pMsgBlk, &tv );
			if( -1 == ret )
			{
				ACE_ASSERT( NULL && "PostWrite Dequeue Error " );
				eRet = E_R_W_DEQUE_FAILED;
				goto __SHUT_DOWN;
			}
		}
	}

	if( NULL != pMsgBlk )
	{
		size_t const write_len = pMsgBlk->total_length();

		m_kStatistics.TrySendBytes(write_len);
		m_kStatistics.CompleteSendBytes(0);

		if(	-1 != m_kWriter.writev( *pMsgBlk, write_len ) )//! ! 이건 v를 빼줄 수도 있다.
		{//! ! 보내기 성공.
			return true;
		}
		else
		{//보내기 실패. 끝나고 goto로 TryClose 로 진입. 플래그 편경(IsDoWrite (false)) 필요 없음
			ClearBlk(pMsgBlk);//이미 deque 되었기 때문에 지우도록.
			HRESULT const hError = ::WSAGetLastError();
			eRet = E_R_W_WRITE_FAILED;
			//여기서 IsWrite 를 false 로 만들면 delete this 가 두번 난다.(가능성;;)
			goto __SHUT_DOWN;
		}
	}
	else//if( NULL != pMsgBlk ) 보낼것이 없다.
	{
		BM::CAutoMutex kFlagLock(m_kFlagMutex);//Shutdown 체크 때문에 뒀음.
		
		if(	handle() == ACE_INVALID_HANDLE
		||	m_kCloseCause != E_R_NONE	)
		{//핸들이 깨졌다.
			eRet = E_R_W_WRITE_FAILED_ALREADY_CLOSE;
			goto __SHUT_DOWN;
			return false;
		}
		else
		{
			IsDoWrite(false);//정상 모드 때문에 해당 플래그 처리 해야함.
		}
		return true;
	}
	return false;
__SHUT_DOWN:
	{
		ShutDown(eRet);//! ! 패킷 꺼내기 실패.
		return false;
	}
}

template< typename T_SESSION >
void Service_Handler_NC< T_SESSION >::handle_write_stream(const ACE_Asynch_Write_Stream::Result &result)
{//! ! Send 가 또 안걸리면 콜 될 일이 없어서 락을 안건다
	IG_LOG LockCheck( m_kDebugLog, kCheck );
	//std::cout << __FUNCTION__<<"-"<<__LINE__ << std::endl;

	MSG_BLK *pMsgBlk = &result.message_block();

	if( handle() == result.handle() 
	&&	result.success() )
	{//! ! 보내기 성공
//		m_kLastNetworkActTime = ACE_OS::gettimeofday();//보내기는 아님.(응답이 없는 죽은 세션일 수 있으므로)
//! ! 	std::cout<< "handle_write_stream 호출 ["  << rMsgBlk.total_length() << "]";		
		MSG_BLK *pNextMsgBlk = NULL;
		
		if(0 == pMsgBlk->total_length() )//보낼것 다 보냈다?
		{//! 블럭 해제.
			if(MSG_BLK::MB_STOP == pMsgBlk->msg_type())
			{//Stop 메세지 였다면 셧다운
				goto __FAILED;
			}
			ClearBlk(pMsgBlk);//ClearBlk 을 위로 올리면 msg_type 확인할때 null 로 뻑남.
		}
		else
		{//! ! 보내다 남음.
			pNextMsgBlk = pMsgBlk;
		}
		
		m_kStatistics.CompleteSendBytes(result.bytes_to_write());
		m_kStatistics.AddSendBytes(result.bytes_to_write());
		m_kStatistics.TrySendBytes(pMsgBlk->total_length());
		
		m_kStatistics.RemainMsgBytes(m_kSendQueue.message_bytes());
		m_kStatistics.RemainMsgLength(m_kSendQueue.message_length());
		m_kStatistics.RemainMsgCount(m_kSendQueue.message_count());

		//또 보낼꺼라.. IsDoWrite 를 변경 시키지 않는다.
		if(!PostWrite(pNextMsgBlk))//보내기 실패면 내부에서 delete 해준다.
		{
			goto __FAILED;
		}
		return;
	}

__FAILED:
	{//! ! 보내기 실패 메모리 반환
		ClearBlk(pMsgBlk);
		ShutDown(0);
		
		TryClose(E_ONCLOSE_BY_HANDLE_WRITE);
	}
	return;
}

template< typename T_SESSION >
bool Service_Handler_NC< T_SESSION >::PostRead()//! ! Recv
{//! ! NULL 이면 처음이고.  아닐때는 Continue가 됨.
	IG_LOG LockCheck( m_kDebugLog, kCheck );
	if( !m_pRecvHeadBlk )
	{//없으면 할당.
		NewBlk(m_pRecvHeadBlk, 512);//처음엔 작은거로 할당
	}

	MSG_BLK *pLastBlk = m_pRecvHeadBlk;
	while( pLastBlk->cont() )//! ! Last 로 이동.
	{
		pLastBlk = pLastBlk->cont();
	}
	
	size_t remain_space = pLastBlk->space();

	if(!remain_space)//! ! 마지막에 공간이 남아있지 않다면.
	{//! ! 할당
		MSG_BLK *pkAddon = NULL;
		NewBlk(pkAddon, 8192);

		if(!pkAddon)
		{
			return false;
		}

		remain_space = pkAddon->end() - pkAddon->wr_ptr();

		pLastBlk->cont( pkAddon );
		pLastBlk = pkAddon;
	}

	if( -1 == m_kReader.readv(*pLastBlk, remain_space))//! ! Inititiate read
	{
		HRESULT const hError = ::WSAGetLastError();
		ShutDown(E_POST_READ_FAILED);
		return false;
	}
	return true;
}

template< typename T_SESSION >
void Service_Handler_NC< T_SESSION >::handle_read_stream(const ACE_Asynch_Read_Stream::Result &result)
{//! ! Recv 가 또 안걸리면 콜 될 일이 없어서 락을 안건다
	IG_LOG LockCheck( m_kDebugLog, kCheck );
	//std::cout << __FUNCTION__<<"-"<<__LINE__ << std::endl;
	MSG_BLK* pkMsgBlk = &result.message_block();
{//! ! Guard 문제로.
	int iRet = 0;

	if(	!result.success() 
	||	0 == result.bytes_transferred() ) 
	{//! ! 접속 종료.
		iRet = 1;
		goto __ON_CLOSE; 
	}

	if( handle() != result.handle() )
	{
		iRet = 2;
		goto __RECV_FAILED;
	}

	m_kStatistics.AddRecvBytes(result.bytes_transferred());

	if(!IsCorrectBandwidth(result.bytes_transferred()))
	{
		iRet = 3;
		goto __RECV_FAILED;
	}

	m_kLastNetworkActTime = ACE_OS::gettimeofday();

	PACKET_TYPE kPacket;
	HEADER_TYPE kHeader;
	while( PopRecved(kPacket, kHeader) )
	{
		//DWORD const dwStart = BM::GetTime32();
		
		if( m_kInitInfo.OnRecv )//! 
		{
			if( kPacket.Size() )//내용이 없는 경우가 있다 (EX> 타임아웃 체크)
			{
				if(S_OK == VDecodeRecved(kPacket, reinterpret_cast<void*>(&kHeader)))
				{
					m_kInitInfo.OnRecv(this, &kPacket);
				}
				else
				{//패킷 풀기 실패.
					iRet = 4;
					goto __ON_CLOSE;
				}
			}
		}
	}

	if( !PostRead() )//handle_read 중인건 read 진행중인 것으로 간주, IsDoRead (false) 를 하지 않는다.
	{
		iRet = 5;
		goto __ON_CLOSE; 
	}//! ! 못받은 패킷 받기
}
	return;
__RECV_FAILED:
__ON_CLOSE://! ! PostRead를 호출 했으나 실패한 경우.
	{//read 는 셧다운이 필요 없다.
		//ClearBlk(pkMsgBlk);//받던 메모리제거는 m_pRecvHeadBlk 으로 처리되므로 신경 쓸것이 없다.
		TryClose(E_ONCLOSE_BY_HANDLE_READ);//! ! onClose에서 Recv에 대한  자원 반환이 이루어진다. 
		return;
	}
}

template< typename T_SESSION >
bool Service_Handler_NC< T_SESSION >::PopRecved( PACKET_TYPE &rOutPacket, HEADER_TYPE &kHeader)
{//! ! 패킷 하나를 빼준다. 메모리를 복사해주고 블럭은 돌려줘버린다
	IG_LOG LockCheck( m_kDebugLog, kCheck );
	HEADER_TYPE &header = kHeader;//CEL 내부 헤더.
	if( PopFromHeader( &header, sizeof(HEADER_TYPE), false ) )//! ! 헤더 길이는 있네.
	{
		size_t const total_blk_length = m_pRecvHeadBlk->total_length();//! ! 내가 꽉찬상태에서 이래되면 -_-;
		size_t const iBodySize = header.BodySize()-HEADER_ADDED_VALUE;
		if( header.BodySize() <= total_blk_length )
		{	
			rOutPacket.Clear();
			rOutPacket.Resize( iBodySize );//! ! 메모리 예약

			PopFromHeader( &header, sizeof(HEADER_TYPE), true );
			if( iBodySize > 0 )//길이가 있다면
			{
				PopFromHeader( &rOutPacket.Data().at(0), iBodySize, true );
				rOutPacket.PosAdjust();
			}

//			rOutPacket.IsEncode(header.IsEncode());
//			rOutPacket.CRC((BYTE)header.CRC());
			return true;
		}
	}
	return false;
}

template< typename T_SESSION >
bool Service_Handler_NC< T_SESSION >::PopFromHeader( void* pDst, size_t const size, bool const bPtrMove )
{
	IG_LOG LockCheck( m_kDebugLog, kCheck );
	if( size > m_pRecvHeadBlk->total_length() )
	{//! ! 빼고싶은거보다 적다
		return false;
	}

	size_t need_len = size;
	size_t offset = 0;

	char* pTarget = reinterpret_cast<char*>(pDst);

	MSG_BLK *pBlk = m_pRecvHeadBlk;
	while( need_len )
	{	
		size_t const out_len = __min( need_len, pBlk->length() );//! ! 얼마나 뺼 수 있나.
		
		if( out_len )
		{
			::memcpy( pTarget + offset, pBlk->rd_ptr(), out_len);
			
			need_len -= out_len;
			offset += out_len;
			if( bPtrMove )
			{
				pBlk->rd_ptr(out_len);
			}
		}

		pBlk = pBlk->cont();
	}

	AdjustBlk(m_pRecvHeadBlk);
	return true;
}

template< typename T_SESSION >
void Service_Handler_NC< T_SESSION >::TryClose(const E_RESULT_CODE eCause)//! ! 접속이 끊어지면 호출 됨. 사용중인 메세지 블럭도 돌려주는 처리.
{//! ! Handle_read_stream 에서만 호출 가능.
{//이 영역은 open, read. write 에서 들어오므로 무조건 safe 되어야 한다.
	BM::CAutoMutex kFlagLock(m_kFlagMutex);

	m_kCloseCause = eCause;
	
	switch(eCause)
	{
	case E_R_W_WRITE_FAILED_ALREADY_CLOSE:
	case E_ONCLOSE_BY_HANDLE_WRITE:
		{
			IsDoWrite(false);
		}break;
	case E_ONCLOSE_BY_HANDLE_READ:
		{
			IsDoRead(false);
		}break;
	case E_ONCLOSE_BY_OPEN:
	default:
		{//변경사항 없음
		}break;
	}

	ShutDown(eCause);//셧다운.
	if(	!IsDoWrite() 
	&&	!IsDoRead() )//쓰기, 보내기 모두 중지여야한다.
	{// write 와 read 동시에 섹션을 벗어나서 delete 가 두번 이루어지는것을 막는다.
		goto __DELETE_THIS;
	}
	return;
}

__DELETE_THIS:
	{
		this->VNotify(NC_CLOSE);
		CEL_LOG( BM::LOG_LV6, L"SessionClose " << m_kAddr.ToString());
		m_kInitInfo.OnDisconnect( this );
		delete this;
	}
}
	
template< typename T_SESSION >
void Service_Handler_NC< T_SESSION >::ShutDown(int const iCallType)
{
	IG_LOG LockCheck( m_kDebugLog, kCheck );
	if( ACE_INVALID_HANDLE != handle() )//소켓 살아있다.
	{
		m_bIsAlive = false;
		m_kWriter.cancel();//ASynch 개체들 
		m_kReader.cancel();
		ACE_OS::shutdown(handle(), SD_SEND);// Terminate 할때 보내던 메세지는 다 가야함.
		handle(ACE_INVALID_HANDLE);
	}
}

template< typename T_SESSION >
void Service_Handler_NC< T_SESSION >::addresses(ACE_INET_Addr const& rRemoteAddr, ACE_INET_Addr const& rLocalAddr)
{
	IG_LOG LockCheck( m_kDebugLog, kCheck );
	m_kRemoteAddr = rRemoteAddr;
}

template< typename T_SESSION >
ACE_INET_Addr const& Service_Handler_NC< T_SESSION >::RemoteAddr() const {return m_kRemoteAddr;}


template< typename T_SESSION >
void Service_Handler_NC< T_SESSION >::act (void const *pAct)
{//! ! Onconnect 시에 호출
	IG_LOG LockCheck( m_kDebugLog, kCheck );
	if( pAct )
	{
		SConnectInfo *pConnectInfo = (SConnectInfo*)pAct;
		m_kSessionKey.SessionGuid(pConnectInfo->guidSession);
		
		SAFE_DELETE(pConnectInfo);
	}
}
	
template< typename T_SESSION >
bool Service_Handler_NC< T_SESSION >::VSend( PACKET_TYPE const& rOrgPacket, bool const bIsEncryption)
{
	//send 를 시도 하는건 어쩔 수 없고 postwrite 같은것에서 막아야함.
	E_RESULT_CODE eRet = E_R_NONE;
	PACKET_TYPE kPacket = rOrgPacket;

	HEADER_TYPE kHeader;
	MSG_BLK* pkMsgBlk = NULL;

	if( handle() ==  ACE_INVALID_HANDLE)
	{//핸들이 나갔다.
		return false;
	}

	//오리지날 사이즈는 체크는 압축 때문에 의미 없음.
	BM::CAutoMutex kLock(m_kSendMutex);//암호화 순서가 지켜져야 해서.

	if(!T_SESSION::VImprovePacket(kPacket, reinterpret_cast<void*>(&kHeader)))//내부에 압축이 있어서, 사이즈가 변하니까 그냥 쓸 수가 없다.
	{//변환을 거침
		eRet = E_R_W_IMPROVE_FAILED;
		goto __SEND_FAILED;
	}
	kHeader.BodySize(kHeader.BodySize() + HEADER_ADDED_VALUE);
	size_t const imp_data_size = kPacket.Data().size();

	if(!(HEADER_TYPE::MAX_BODY_SIZE > imp_data_size))
	{//비트팩이라 사이즈가 안될 수도 있음
		ACE_ASSERT( NULL && "Service Handler -> Packet Size Too Big!" );
		eRet = E_R_W_SIZE_FAILED;
		goto __SEND_FAILED;
	}

	if(S_OK == NewBlk(pkMsgBlk, sizeof(HEADER_TYPE) + imp_data_size))
	{
		if(kPacket.IsStopSignal())
		{
			pkMsgBlk->msg_type(MSG_BLK::MB_STOP);
		}

		//헤더 셋팅 
		HEADER_TYPE *pHeader = reinterpret_cast< HEADER_TYPE* >(pkMsgBlk->wr_ptr());
		
		*pHeader = kHeader;

		pkMsgBlk->wr_ptr(sizeof(HEADER_TYPE));//Msg 블럭 포인터 이동

		if(imp_data_size)
		{
			void *pBody = reinterpret_cast< void* >(pkMsgBlk->wr_ptr());// 바디 카피
			::memcpy(pBody, &kPacket.Data().at(0), imp_data_size);
			
			pkMsgBlk->wr_ptr(imp_data_size);
		}

		//-> 보내기
		ACE_Time_Value tv(0, 100);	tv += ACE_OS::gettimeofday();
		if(-1 != m_kSendQueue.enqueue(pkMsgBlk, &tv))
		{
			BM::CAutoMutex kFlagLock(m_kFlagMutex);
			m_kStatistics.IncSendCount();
			if( !IsDoWrite() )
			{
				IsDoWrite(true);//! ! 절대 아래로 내리지 마시오. Write 실행중 handle_write_Stream 이 호출 되면 IsDoWrite가 계속 true
				if(!PostWrite( NULL ))
				{
					IsDoWrite(false);//보내기 실패
					return false;
				}
			}
			return true;
		}
		else
		{
			eRet = E_R_W_ENQUEUE_FAILED;
			goto __SEND_FAILED;
		}
	}
__SEND_FAILED:
	{
		ClearBlk(pkMsgBlk);
		ShutDown(eRet);//꺼버려.
		return false;
	}
	return false;
}

template< typename T_SESSION >
bool Service_Handler_NC< T_SESSION >::VTerminate()
{
	if(m_bIsAlive)
	{
		ShutDown(0);
		ACE_HANDLE kTempHandle = MyHandle();
		if(kTempHandle != ACE_INVALID_HANDLE)
		{
			MyHandle(ACE_INVALID_HANDLE);
			ACE_OS::closesocket( kTempHandle );//Terminate 는 원샷에 소켓 클로즈.
		}
		return true;
	}
	return false;
}

template< typename T_SESSION >
HRESULT Service_Handler_NC< T_SESSION >::Statistics(SSessionStatistics &rkOutResult) const
{
	rkOutResult = m_kStatistics;
	return S_OK;
}

template< typename T_SESSION >
bool Service_Handler_NC< T_SESSION >::IsCorrectBandwidth(__int64 const iRecvBytes)
{
	m_iRecvBytesForDosCheck += iRecvBytes;//얘는 일단 계속 더함.
	ACE_Time_Value rettime = ACE_OS::gettimeofday() - m_kDosCheckTime;
	if(rettime > ACE_Time_Value(10) )//1초 넘을때 체크.
	{
		__int64 const bandwidth = m_iRecvBytesForDosCheck/rettime.sec();
		if(bandwidth > m_kInitInfo.dwBandwidthLimit)
		{
			return false;
		}
		m_iRecvBytesForDosCheck = 0;//체크 끝이니. 클리어.
		m_kDosCheckTime = ACE_OS::gettimeofday();
	}
	
	return true;
}
