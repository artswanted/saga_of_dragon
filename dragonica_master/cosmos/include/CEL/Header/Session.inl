namespace CEL
{
	template< typename T_HEADER_TYPE >
	PgSession< T_HEADER_TYPE >::PgSession(void)
	{
		m_bIsAlive = false;
		m_bIsFirstSend = true;
		m_bIsFirstRecv = true;
	}

	template< typename T_HEADER_TYPE >
	PgSession< T_HEADER_TYPE >::~PgSession(void)
	{
	}

	template< typename T_HEADER_TYPE >
	bool PgSession< T_HEADER_TYPE >::IsAlive() const
	{
		return m_bIsAlive;
	}

	template< typename T_HEADER_TYPE >
	bool PgSession< T_HEADER_TYPE >::VOpen()
	{
		const CEL::CEL_EL::eRESULT_CODE kSendEnRet = m_kSendEncoder.Init(CEL::CEL_EL::ENCRYPT_SECURE| CEL::CEL_EL::SWAP_ON | CEL::CEL_EL::KEY_ROLL_DYNAMIC |CEL::CEL_EL::KEY_INIT_AUTO);
		const CEL::CEL_EL::eRESULT_CODE kRecvEnRet = m_kRecvEncoder.Init(CEL::CEL_EL::ENCRYPT_SECURE| CEL::CEL_EL::SWAP_ON | CEL::CEL_EL::KEY_ROLL_DYNAMIC |CEL::CEL_EL::KEY_INIT_AUTO);
		
		if(	kSendEnRet == CEL::CEL_EL::INIT_SUCCESS 
		&&	kRecvEnRet == CEL::CEL_EL::INIT_SUCCESS )
		{
			m_bIsAlive = true;
			return true;
		}

		assert(NULL && "Session::Init EncryptLib Init Failed");
		return false;
	}

	template< typename T_HEADER_TYPE >
	bool PgSession< T_HEADER_TYPE >::VImprovePacket(CSession_Base::PACKET_TYPE &rkPacket, void * const pkPacketHeader)
	{//암호화 플래그로만.. 
		static BYTE const acPadData[40] = 
		{//실행마다 바뀌는 스태틱이어야 함
			BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1),
			BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1),
			BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1),
			BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1),
			BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1),
			BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1),
			BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1),
			BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1), BM::Rand_Index(UCHAR_MAX+1),
		};

		T_HEADER_TYPE &kHeader = *reinterpret_cast<T_HEADER_TYPE *>(pkPacketHeader);

	//사이즈가 있어야 암호화 한다. 패딩 때문에 무조건 암호화다.
		int const iRand = BM::Rand_Index(8);
		int const iPad = BM::Rand_Range(15,4);//최소 4바이트 패딩은 들어간다.

		rkPacket.Push(&acPadData[iRand], iPad);//패딩 추가

		size_t const org_size = rkPacket.Size();//패딩포함 사이즈.(사이즈가 없을 경우가 없음

		if(org_size> HEADER_TYPE::MAX_BODY_SIZE)
		{//원본이 MAX 제한보다 크면 무시.
			CEL_LOG(BM::LOG_LV0, __FL__ << _T("Send Error -Size- Size=") << org_size);
			return false;
		}

		std::vector< char > kOut;
		bool bIsCompressed = false;
		if(	org_size > 140 //최소 140 사이즈 이상에서만.
		&&	BM::LZOCompress(rkPacket.Data(), kOut))
		{//압축이 멀티 스레드 환경에서 잘 도는지 확인해야함
			rkPacket.Reset();

			bIsCompressed = true;
			rkPacket.Push(org_size);
			rkPacket.Push(kOut.size());
			rkPacket.Push(&kOut.at(0), kOut.size());
			rkPacket.Push(bIsCompressed);
			
			rkPacket.PosAdjust();
		}
		else
		{
			rkPacket.Push(bIsCompressed);
		}

		//처음이면 암호화 말고. 
		DWORD const dwOrgCrc = CEL::CEL_EL::GetDataCRC(&rkPacket.Data().at(0), (DWORD)rkPacket.Data().size());//암호화 하기전
		BYTE const byOrgCrc = CEL::CEL_EL::CRCToByte(dwOrgCrc);
		size_t const body_size = rkPacket.Data().size();//패딩까지 포함된 사이즈

		DWORD dwEncCRC = 0;
		if(m_bIsFirstSend)//
		{
			m_bIsFirstSend = false;
			dwEncCRC = dwOrgCrc;
		}
		else
		{
			m_kSendEncoder.Encrypt(&rkPacket.Data().at(0), (DWORD)rkPacket.Data().size(), dwEncCRC);
		}

		kHeader.Rand(iRand);
		kHeader.Pad(iPad);
		kHeader.BodySize(body_size);
		kHeader.EncCRC(dwEncCRC);
		kHeader.OrgCRC(byOrgCrc);
/*	
		if(kHeader.Rand() != iRand
		||	kHeader.Pad() != iPad
		||	kHeader.BodySize() != body_size
		||	kHeader.EncCRC() != dwEncCRC
		||	kHeader.OrgCRC() !=  byOrgCrc)
		{
			__asm int 3;
		}

		CEL_LOG(BM::LOG_LV0, _T("Send Header=") << kHeader.BodySize() << _COMMA_ << _T(" Real=") << org_size-iPad << _T(" Bin=")<< *(__int64*)&kHeader);
*/	
	return true;
	}

	template< typename T_HEADER_TYPE >
	HRESULT PgSession< T_HEADER_TYPE >::VDecodeRecved(CSession_Base::PACKET_TYPE &rkPacket, void * const pkPacketHeader)
	{
		T_HEADER_TYPE &kHeader = *reinterpret_cast<T_HEADER_TYPE *>(pkPacketHeader);
		if(rkPacket.Data().size())
		{
			DWORD dwEncCRC = 0;
			if(m_bIsFirstRecv)
			{
				m_bIsFirstRecv = false;
				dwEncCRC = CEL::CEL_EL::GetDataCRC(&rkPacket.Data().at(0), (DWORD)rkPacket.Data().size());
			}
			else
			{
				m_kRecvEncoder.Decrypt(&rkPacket.Data().at(0), (DWORD)rkPacket.Data().size(), dwEncCRC);
			}
		
			if(dwEncCRC == kHeader.EncCRC())
			{
				DWORD const dwOrgCrc = CEL::CEL_EL::GetDataCRC(&rkPacket.Data().at(0), (DWORD)rkPacket.Data().size());//암호화 풀기전
				BYTE const byOrgCrc = CEL::CEL_EL::CRCToByte(dwOrgCrc);
				
				if(byOrgCrc == kHeader.OrgCRC())//원본 CRC 도 같아야함.
				{
					size_t const compressed_index = (rkPacket.Data().size()-1);

					char const bIsCompressed = rkPacket.Data().at(compressed_index);
					rkPacket.Data().resize(compressed_index);//마지막 플래그 쟤 지운다.

					if(bIsCompressed)
					{//압축의 경우
						size_t kOrgSize = 0;
						size_t kCmpSize = 0;//압축 사이즈

						rkPacket.Pop(kOrgSize);
						rkPacket.Pop(kCmpSize);
						if(kOrgSize < HEADER_TYPE::MAX_BODY_SIZE
						&& kCmpSize < HEADER_TYPE::MAX_BODY_SIZE
						&& kCmpSize > 0
						&& kOrgSize > 0 
						&& kOrgSize >= kHeader.Pad())//사이즈가 합당할 때.// 패딩이상의 사이즈여야 하기도함.
						{
							std::vector< char > kCompressedData(kCmpSize);
							rkPacket.PopMemory(&kCompressedData.at(0), kCmpSize);

							std::vector< char > kUncompressedData(kOrgSize);
							if(BM::LZOUnCompress(kCompressedData, kUncompressedData))
							{
								kUncompressedData.resize(kOrgSize-kHeader.Pad());//이때 orgSize 는 compressflag 사이즈 포함 하지 않아서 -1 안한다.

								rkPacket.Data().swap(kUncompressedData);
								rkPacket.PosAdjust();

//								CEL_LOG(BM::LOG_LV0, _T("Recv1 Header=")<< kHeader.BodySize() << _T(" Real=") <<rkPacket.Size() << _T(" Bin=")<< *(__int64*)&kHeader);
								return S_OK;
							}
							else
							{//압축 풀기 실패
								CEL_LOG(BM::LOG_LV0, __FL__ << _T("Recv Error - Compress - Size=") << rkPacket.Size());
								return E_FAIL;
							}
						}
						else
						{
							CEL_LOG(BM::LOG_LV0, __FL__ << _T("Recv Error - Incorrect Size - Size=") << rkPacket.Size());
							return E_FAIL;
						}
					}
					else
					{//압축 안된 경우는
						if(kHeader.BodySize() >= kHeader.Pad())//바디가 패딩보다 커야함.
						{
							rkPacket.Data().resize(kHeader.BodySize() - kHeader.Pad() - 1);//compress 플래그 사이즈가 1.//바디 사이즈에는 1이 포함됨.
							rkPacket.PosAdjust();
//							CEL_LOG(BM::LOG_LV0, _T("Recv2 Header=")<< kHeader.BodySize() << _T(" Real=") <<rkPacket.Size() << _T(" Bin=")<< *(__int64*)&kHeader);
							return S_OK;
						}
					}
				}
			}
	
			CEL_LOG(BM::LOG_LV0, __FL__ << _T("Recv Error - Dec - Size=") << rkPacket.Size());
			return E_FAIL;
		}

//		CEL_LOG(BM::LOG_LV0, _T("Recv3 Header=")<< kHeader.BodySize() << _T(" Real=") <<rkPacket.Size() << _T(" Bin=")<< *(__int64*)&kHeader);
		return S_OK;
	}

	template< typename T_HEADER_TYPE >
	HRESULT PgSession< T_HEADER_TYPE >::VGetEncodeKey(std::vector< char > &rkKeyOut)const
	{
	//	BM::CAutoMutex kLock(m_kMutex);
		rkKeyOut.resize(sizeof(CEL::CEL_EL::INIT_INFO)*2);
			
		if(m_kSendEncoder.GetInitInfo( *(CEL::CEL_EL::INIT_INFO*)&rkKeyOut.at(0) )
		&& m_kRecvEncoder.GetInitInfo( *(CEL::CEL_EL::INIT_INFO*)&rkKeyOut.at(sizeof(CEL::CEL_EL::INIT_INFO)) ))
		{
			return S_OK;
		}
		return E_FAIL;
	}

	template< typename T_HEADER_TYPE >
	HRESULT PgSession< T_HEADER_TYPE >::VSetEncodeKey(std::vector< char > const& rkKeyIn)
	{
	//	BM::CAutoMutex kLock(m_kMutex);
		if( rkKeyIn.size() == sizeof(CEL::CEL_EL::INIT_INFO)*2)
		{
			if(CEL::CEL_EL::INIT_SUCCESS == m_kRecvEncoder.InitManual( *(CEL::CEL_EL::INIT_INFO*)&rkKeyIn.at(0) )//Sender로 Recver 를 초기화
			&& CEL::CEL_EL::INIT_SUCCESS == m_kSendEncoder.InitManual( *(CEL::CEL_EL::INIT_INFO*)&rkKeyIn.at(sizeof(CEL::CEL_EL::INIT_INFO)) ))//Recver 로 Sender 를 초기화
			{
				return S_OK;
			}
		}
		return E_FAIL;
	}

	template< typename T_HEADER_TYPE >
	bool PgSession_NoneEncWithZIP< T_HEADER_TYPE >::VImprovePacket(CSession_Base::PACKET_TYPE &rkPacket, void * const pkPacketHeader)
	{//암호화 플래그로만.. 
		T_HEADER_TYPE &kHeader = *reinterpret_cast<T_HEADER_TYPE *>(pkPacketHeader);
		
		if(	!m_bIsFirstSend
		//	(bIsEncryption//암호화 해라 하고
	//	||	m_kSendEncoder.IsInit() //라이브러리 살아있으면 무조건 암호화하는걸로 간주.
		&&	rkPacket.Data().size())//사이즈 있어야지
		{//사이즈가 있어야 암호화 한다
			size_t const org_size = rkPacket.Size();
			std::vector< char > kOut;
			bool bIsCompressed = false;
			if(	org_size > 150 //최소 10 사이즈 이상에서만.
			&&	BM::LZOCompress(rkPacket.Data(), kOut))
			{//압축이 멀티 스레드 환경에서 잘 도는지 확인해야함
				rkPacket.Reset();

				bIsCompressed = true;
	//			rkPacket.IsCompressed(true);
				rkPacket.Push(org_size);
				rkPacket.Push(kOut);
				rkPacket.Push(bIsCompressed);
				
				rkPacket.PosAdjust();
			}
			else
			{
				rkPacket.Push(bIsCompressed);
			}

			kHeader.BodySize(rkPacket.Data().size());
	//		rkPacket.CRC(0);
	 		return true;
		}

		kHeader.BodySize(rkPacket.Data().size());
		m_bIsFirstSend = false;

	//	rkPacket.CRC(0);
	//	rkPacket.IsEncode(false);
	//	rkPacket.IsCompressed(false);
		return true;
	}

	template< typename T_HEADER_TYPE >
	HRESULT PgSession_NoneEncWithZIP< T_HEADER_TYPE >::VDecodeRecved(CSession_Base::PACKET_TYPE &rPacket, void * const pkPacketHeader)
	{
	//	BM::CAutoMutex kLock(m_kMutex);
		if(	!m_bIsFirstRecv
	//		(rPacket.IsEncode()
	//	||	m_kSendEncoder.IsInit() ) //암호화기 켜졌으면 무조건 푸는거임.
		&& rPacket.Data().size() )
		{
			DWORD dwCRC = 0;
	//		if(m_kRecvEncoder.Decrypt(&rPacket.Data().at(0), (DWORD)rPacket.Data().size(), dwCRC))
			{
	//			BYTE const byCRC = CEL::CEL_EL::CRCToByte(dwCRC);//CRC 셋팅
	//			if(rPacket.CRC() == 0)
				{
					size_t const compressed_index = (rPacket.Data().size()-1);

					char const bIsCompressed = rPacket.Data().at(compressed_index);
					rPacket.Data().resize(compressed_index);//마지막 플래그 쟤 지운다.

					if(bIsCompressed)
					{
						size_t kOrgSize = 0;
						std::vector< char > kCompressedData;
						rPacket.Pop(kOrgSize);
						rPacket.Pop(kCompressedData);

						std::vector< char > kUncompressedData(kOrgSize);
						if(BM::LZOUnCompress(kCompressedData, kUncompressedData))
						{
							rPacket.Data().swap(kUncompressedData);
							rPacket.PosAdjust();
							return S_OK;
						}

						assert(NULL);
	//					throw("CompressError");
						return E_FAIL;
					}
					rPacket.PosAdjust();
					return S_OK;
				}
	//			else
				{
					assert(NULL);
			//		throw("DecryptError");
					return E_FAIL;
				}
			}
			assert(NULL);
	//		throw("DecryptError");
			return E_FAIL;
		}
		else
		{
			m_bIsFirstRecv = false;
		}
		return S_OK;
	}


	template< typename T_HEADER_TYPE >
	bool PgSession_NoneEnc< T_HEADER_TYPE >::VImprovePacket(CSession_Base::PACKET_TYPE &rkPacket, void * const pkPacketHeader)
	{
		T_HEADER_TYPE &kHeader = *reinterpret_cast<T_HEADER_TYPE *>(pkPacketHeader);
		kHeader.BodySize(rkPacket.Data().size());
		return true;
	}
	template< typename T_HEADER_TYPE >
	HRESULT PgSession_NoneEnc< T_HEADER_TYPE >::VDecodeRecved(CSession_Base::PACKET_TYPE &rPacket, void * const pkPacketHeader)
	{
		return S_OK;
	}
}

