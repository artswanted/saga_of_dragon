#include "stdafx.h"
#include "Common.h"
#include <limits.h>
#include <time.h>
#include "../header/CEL_EL.h"
#include "BM/Threadobject.h"
#include "BM/FileSupport.h"
#include "Themida/ThemidaSDK.h"

namespace CEL
{
	static CEL_EL::KEY_TYPE GetNextKey(CEL_EL::KEY_TYPE const& A, CEL_EL::GAB_TYPE const& B, CEL_EL::GAB_TYPE const& C)	
	{
		return ( (A)*(B) ) + (C);
	}

	CEL_EL::CEL_EL()
	{
		Clear();
	}

	CEL_EL::~CEL_EL()
	{
	}

	bool CEL_EL::EncryptInitInfo(INIT_INFO &rInitInfo)const
	{
		DWORD const dwBlockCount	= (sizeof(INIT_INFO)-sizeof(INIT_INFO_KEY_TYPE)-sizeof(INIT_INFO_CRC_TYPE)) / sizeof(INIT_INFO_KEY_TYPE) ;
		DWORD dwRemain				= (sizeof(INIT_INFO)-sizeof(INIT_INFO_KEY_TYPE)-sizeof(INIT_INFO_CRC_TYPE)) % sizeof(INIT_INFO_KEY_TYPE) ;

		INIT_INFO_KEY_TYPE const Key 	= 	MAKEWORD( BM::Rand_Range(UCHAR_MAX, 1), BM::Rand_Range(UCHAR_MAX, 1));

		char *pSrc = reinterpret_cast<char*>(&rInitInfo) + sizeof(INIT_INFO_KEY_TYPE)+sizeof(INIT_INFO_CRC_TYPE);

		INIT_INFO_CRC_TYPE CRC = 0;

		if(dwBlockCount)
		{
			INIT_INFO_KEY_TYPE* pBlock = reinterpret_cast<INIT_INFO_KEY_TYPE*>(pSrc);

			register DWORD dwCount = dwBlockCount;

			while(true)
			{
				CRC ^= *pBlock;//ORG 누적
				
				*pBlock ^= Key;			

				if(! (--dwCount) ){break;}
				++pBlock;
			} // while(dwBlockCount)
		}

		if(dwRemain)
		{
			char* pRef = pSrc + dwBlockCount * sizeof(INIT_INFO_KEY_TYPE);
			char* pCRC = reinterpret_cast<char*>(&CRC);

			while(true)
			{
				*pCRC ^= *pRef;//org acc
				
				if(! (--dwRemain) ){break;}
				++pRef;
			} // while(true)
		} // if(dwRemain)

		CRC ^= Key;//마지막에 Key로 한번 더 덮는다.

		rInitInfo.Key = Key;
		rInitInfo.CRC = CRC;
		
		return true;
	}

	bool CEL_EL::DecryptInitInfo(INIT_INFO &rInitInfo)const
	{
		DWORD const dwBlockCount	= (sizeof(INIT_INFO)-sizeof(INIT_INFO_KEY_TYPE)-sizeof(INIT_INFO_CRC_TYPE)) / sizeof(INIT_INFO_KEY_TYPE) ;
		DWORD dwRemain				= (sizeof(INIT_INFO)-sizeof(INIT_INFO_KEY_TYPE)-sizeof(INIT_INFO_CRC_TYPE)) % sizeof(INIT_INFO_KEY_TYPE) ;

		INIT_INFO_KEY_TYPE const Key = rInitInfo.Key;

		char *pSrc = reinterpret_cast<char*>(&rInitInfo) + sizeof(INIT_INFO_KEY_TYPE)+sizeof(INIT_INFO_CRC_TYPE);

		INIT_INFO_CRC_TYPE CRC = 0;

		CRC ^= Key;//마지막에 Key로 한번 더 덮은것을 제거.

		if(dwBlockCount)
		{
			INIT_INFO_KEY_TYPE* pBlock = reinterpret_cast<INIT_INFO_KEY_TYPE*>(pSrc);

			register DWORD dwCount = dwBlockCount;

			while(true)
			{
				*pBlock ^= Key;			
				
				CRC ^= *pBlock;//ORG 누적

				if(! (--dwCount) ){break;}
				++pBlock;
			} // while(dwBlockCount)
		}

		if(dwRemain)
		{
			char* pRef = pSrc + dwBlockCount * sizeof(INIT_INFO_KEY_TYPE);
			char* pCRC = reinterpret_cast<char*>(&CRC);
			while(true)
			{
				*pCRC ^= *pRef;//org acc

				if(! (--dwRemain) ){break;}
				++pRef;
			} // while(true)
		} // if(dwRemain)

		if(rInitInfo.CRC == CRC)
		{
			return true;
		}

		return false;
	}

	void CEL_EL::Clear()
	{
		m_kIsInit = false;

		m_ks.Clear();
		m_gs.Clear();
		
		m_dwInitFlag = 0;

		m_pFuncEn1 = NULL;
		m_pFuncEn2 = NULL;
		m_pFuncEn3 = NULL;

		m_pFuncDe1 = NULL;
		m_pFuncDe2 = NULL;
		m_pFuncDe3 = NULL;

		m_pFuncEnSwap = NULL;
		m_pFuncDeSwap = NULL;
		
		m_pFuncKeyRoll = NULL;

		m_dec_count = 0;
		m_enc_count = 0;
	}

	CEL_EL::eRESULT_CODE CEL_EL::Init(DWORD const& dwInitFlag)
	{
		BM::CAutoMutex kLock(m_kMutex);
	 
		m_dwInitFlag = dwInitFlag;
		switch( m_dwInitFlag & ENCRYPT_CMP_BASE )
		{
		case ENCRYPT_FAST:
			{
				m_pFuncEn1 = &CEL_EL::REN;
				m_pFuncEn2 = NULL;
				m_pFuncEn3 = NULL;

				m_pFuncDe1 = &CEL_EL::RDE;
				m_pFuncDe2 = NULL;
				m_pFuncDe3 = NULL;
			}break;
		case ENCRYPT_SECURE:
			{
				m_pFuncEn1 = &CEL_EL::REN;
				m_pFuncEn2 = &CEL_EL::DLEN;
				m_pFuncEn3 = &CEL_EL::DREN;

				m_pFuncDe1 = &CEL_EL::RDE;
				m_pFuncDe2 = &CEL_EL::DLDE;
				m_pFuncDe3 = &CEL_EL::DRDE;
			}break;
		default:
			{
				return INIT_ENCRYPT_OPT_FAULT;
			}break;
		} // switch( m_dwInitFlag & ENCRYPT_CMP_BASE )
	    	
		switch(m_dwInitFlag & SWAP_CMP_BASE)
		{
		case SWAP_ON:
			{
				m_pFuncEnSwap = &CEL_EL::SwapEn;
				m_pFuncDeSwap = &CEL_EL::SwapDe;
			}break;
		case SWAP_OFF:
			{
				m_pFuncEnSwap = NULL;
				m_pFuncDeSwap = NULL;
			}break;
		default:
			{
				return INIT_SWAP_OPT_FAULT;
			}break;
		} // switch(m_dwInitFlag & SWAP_CMP_BASE)

		switch(m_dwInitFlag & KEY_ROLL_CMP_BASE)
		{
		case KEY_ROLL_DYNAMIC:
			{
				m_pFuncKeyRoll = &CEL_EL::NextKey;
			}break;
		case KEY_ROLL_STATIC:
			{
				m_pFuncKeyRoll = NULL;
			}break;
		default:
			{
				return INIT_KEY_ROLL_OPT_FAULT;
			}break;
		}
	    
		switch(m_dwInitFlag & KEY_INIT_CMP_BASE)
		{
		case KEY_INIT_AUTO:
			{
				InitKey();
				InitGab();
			}break;
		case KEY_INIT_MANUAL:
			{
				return INIT_KEY_INIT_FAULT_PLZ_USE_AUTO;
			}break;
		default:
			{
				return INIT_KEY_INIT_FAULT;
			}break;
		} // switch(m_dwInitFlag & KEY_INIT_CMP_BASE)

		m_kIsInit = true;
		return INIT_SUCCESS;
	}

	CEL_EL::eRESULT_CODE CEL_EL::InitManual(INIT_INFO const& rInitInfo)
	{
		BM::CAutoMutex kLock(m_kMutex);
		INIT_INFO LocalInfo = rInitInfo;

		bool const bCRCFlag = DecryptInitInfo(LocalInfo);//De가 틀려도 상관은 없다.
		
		if(!bCRCFlag)
		{
			return INIT_MANUAL_CRC_ERROR;
		}

		eRESULT_CODE eRet = Init(LocalInfo.dwInitFlag);

		if(INIT_KEY_INIT_FAULT_PLZ_USE_AUTO != eRet)//키를 Manual로 셋팅 하기때문에 Init이 어떻게 되는지 지켜보는것이 중요하다..
		{
			return INIT_MANUAL_FAILED;
		}
	   
		switch(m_dwInitFlag & KEY_INIT_CMP_BASE)
		{
		case KEY_INIT_AUTO:
			{
				return INIT_KEY_INIT_FAULT_PLZ_USE_MANUAL;
			}break;
		case KEY_INIT_MANUAL:
			{
				m_gs = LocalInfo.gs ;
				m_ks = LocalInfo.ks ;			
			}break;
		default:
			{
				return INIT_KEY_INIT_FAULT;
			}break;
		} // switch(m_dwInitFlag & KEY_INIT_CMP_BASE)

		m_kIsInit = true;
		return INIT_SUCCESS;
	}

	bool CEL_EL::GetInitInfo(INIT_INFO &rInitInfo)const//이걸 인코드 할때 써야함.
	{
		BM::CAutoMutex kLock(m_kMutex);
		rInitInfo.dwInitFlag =	KEY_INIT_MANUAL
							|	(m_dwInitFlag & ENCRYPT_CMP_BASE)
							|	(m_dwInitFlag & SWAP_CMP_BASE)
							|	(m_dwInitFlag & KEY_ROLL_CMP_BASE);
		rInitInfo.gs = m_gs;
		rInitInfo.ks = m_ks;

		EncryptInitInfo(rInitInfo);
		return true;
	}

	bool CEL_EL::Encrypt(char *pSrc, DWORD const dwSrcSize, DWORD &dwOutCRC)
	{
		BM::CAutoMutex kLock(m_kMutex);
		KEY_TYPE const OrgCRC = GetDataCRC(pSrc, dwSrcSize);//원본의 CRC 기록

		if(m_pFuncEn1){	(this->*m_pFuncEn1)(pSrc, dwSrcSize);}
		if(m_pFuncEn2){	(this->*m_pFuncEn2)(pSrc, dwSrcSize);}
		if(m_pFuncEn3){	(this->*m_pFuncEn3)(pSrc, dwSrcSize);}

		if(m_pFuncEnSwap){(this->*m_pFuncEnSwap)(pSrc, dwSrcSize);}
			
		KEY_TYPE const EnedCRC = GetDataCRC(pSrc, dwSrcSize);//결과물의 CRC
		
		dwOutCRC = OrgCRC^EnedCRC;

		if(m_pFuncKeyRoll){(this->*m_pFuncKeyRoll)();}//최하단.
		
		++m_enc_count;
		return true;
	}

	bool CEL_EL::Encrypt(char const *pSrc, char *pDest, DWORD const dwSrcSize, DWORD &dwOutCRC)
	{
		BM::CAutoMutex kLock(m_kMutex);
		::memcpy(pDest, pSrc, dwSrcSize);
		bool const bRet = Encrypt(pDest, dwSrcSize, dwOutCRC);

		return bRet;
	}

	bool CEL_EL::Decrypt(char *pSrc, DWORD const dwSrcSize, DWORD &dwOutCRC)
	{
		BM::CAutoMutex kLock(m_kMutex);
		KEY_TYPE const EnedCRC = GetDataCRC(pSrc, dwSrcSize);//원본의 CRC 기록

		if(m_pFuncDeSwap){(this->*m_pFuncDeSwap)(pSrc, dwSrcSize);}

		if(m_pFuncDe3){	(this->*m_pFuncDe3)(pSrc, dwSrcSize); }
		if(m_pFuncDe2){	(this->*m_pFuncDe2)(pSrc, dwSrcSize); }
		if(m_pFuncDe1){	(this->*m_pFuncDe1)(pSrc, dwSrcSize); }

		KEY_TYPE const OrgCRC = GetDataCRC(pSrc, dwSrcSize);//결과물의 CRC
		
		dwOutCRC = OrgCRC^EnedCRC;

		if(m_pFuncKeyRoll){(this->*m_pFuncKeyRoll)();}//최하단.

		++m_dec_count;
		return true;
	}

	bool CEL_EL::Decrypt(char const *pSrc, char *pDest, DWORD const dwSrcSize, DWORD &dwOutCRC)
	{
		BM::CAutoMutex kLock(m_kMutex);
		::memcpy(pDest, pSrc, dwSrcSize);
		bool const bRet = Decrypt(pDest, dwSrcSize, dwOutCRC);

		return bRet;
	}

	CEL_EL::KEY_TYPE CEL_EL::MakeKey()
	{	
		return	MAKELONG( 
					MAKEWORD( 
						BM::Rand_Range(UCHAR_MAX, 1), 
						BM::Rand_Range(UCHAR_MAX, 1) ),
					MAKEWORD( 
						BM::Rand_Range(UCHAR_MAX, 1), 
						BM::Rand_Range(UCHAR_MAX, 1) ) );
	}

	CEL_EL::GAB_TYPE CEL_EL::MakeGab()
	{
		return 	static_cast<CEL_EL::GAB_TYPE>(BM::Rand_Range(UCHAR_MAX, 1));
	}

	void CEL_EL::InitKey()
	{
		register int iCount = MAX_RD_KEY_ARRAY;

		while(iCount)
		{
			m_ks.aRDKey[--iCount] = MakeKey();
		}
		
		m_ks.LDESKey = MakeKey();
		m_ks.RDESKey = MakeKey();
	}

	void CEL_EL::InitGab()
	{
		register int iCount = MAX_RD_KEY_ARRAY;

		while(iCount)
		{
			m_gs.aRDGab[--iCount] = MakeGab();
		}
		
		m_gs.MultiGab = __max(MakeGab(), 5);
		m_gs.LDESGab = MakeGab();
		m_gs.RDESGab = MakeGab();
	}

	void CALLBACK CEL_EL::NextKey()
	{//항상 홀수 이므로 키 값이 0이 되는 일은 없다.
		//곱하기값, 더하기 값, 필요.
		register int iCount = MAX_RD_KEY_ARRAY;

		while(iCount)
		{
			--iCount;
			m_ks.aRDKey[iCount] = GetNextKey(m_ks.aRDKey[iCount], m_gs.MultiGab, m_gs.aRDGab[iCount]);
		}
		
		m_ks.LDESKey = GetNextKey(m_ks.LDESKey, m_gs.MultiGab, m_gs.LDESGab);
		m_ks.RDESKey = GetNextKey(m_ks.RDESKey, m_gs.MultiGab, m_gs.RDESGab);
	}

	void CALLBACK CEL_EL::REN(char *pSrc, DWORD const dwSrcSize)
	{//암호화 한 결과물을 사용하지 않는다. 64*4 바이트의 주기가 있음. 레인다엘(Rijndael) 알고리즘
		if(1 > dwSrcSize){return;}

		DWORD const dwBlockCount	= ( dwSrcSize / sizeof(KEY_TYPE) ) ;//KeyType으로 나눔
		register DWORD dwRemain		= ( dwSrcSize % sizeof(KEY_TYPE) );//남은 바이트 계산

		register BYTE byIdxPRI = 0;
		register BYTE byIdxSPN = 0;

		if(dwBlockCount)
		{
			KEY_TYPE* lpRef = reinterpret_cast<KEY_TYPE*>(pSrc);

			register DWORD dwCount = dwBlockCount;
			while(true)
			{
				*lpRef ^=  ( (byIdxPRI == byIdxSPN)?(m_ks.aRDKey[byIdxPRI]):( m_ks.aRDKey[byIdxPRI] ^ m_ks.aRDKey[byIdxSPN] ) );//Index가 같으면 XOR시 0이 되므로 키를 하나만 사용
				
				( (MAX_RD_KEY_ARRAY-1) == byIdxSPN )//배열은 -1만큼 해야 하므로
					?	( byIdxSPN = 0, ++byIdxPRI )//스핀키를 원점으로 하면서 프라이머리 키를 증가.
					:	( ++byIdxSPN );//Max에 도달할때 까지 증가

				( MAX_RD_KEY_ARRAY == byIdxPRI )?( byIdxPRI = 0 ): 0;//프라이머리 키 보정 MAX가 되었을떄만 0 으로 보정.

				if(! (--dwCount) ){break;}

				++lpRef;
			} // while(true)
		} // if(dwBlockCount)

		if(dwRemain)
		{
			KEY_TYPE Key = ( (byIdxPRI == byIdxSPN)?(m_ks.aRDKey[byIdxPRI]):( m_ks.aRDKey[byIdxPRI] ^ m_ks.aRDKey[byIdxSPN] ) );

			char* pRef = pSrc + dwBlockCount * sizeof(KEY_TYPE);
			char* pKey = reinterpret_cast<char*>(&Key);
			while(true)
			{
				*pRef ^= *pKey;

				if(! (--dwRemain) ){break;}

				++pRef;
				++pKey;
			} // while(true)
		} // if(dwRemain)
	}

	void CALLBACK CEL_EL::DLEN(char *pSrc, DWORD const dwSrcSize)
	{//DES 알고리즘
		if(1 > dwSrcSize){return;}

		DWORD const dwBlockCount	= ( dwSrcSize / sizeof(KEY_TYPE) ) ;//키의 크기로 나눔.
		register DWORD dwRemain		= ( dwSrcSize % sizeof(KEY_TYPE) );//남은 바이트 계산

		KEY_TYPE Key = m_ks.LDESKey;

		if(dwBlockCount)
		{
			KEY_TYPE* lpRef = reinterpret_cast<KEY_TYPE*>(pSrc);
			register DWORD dwCount = dwBlockCount;
			while(true)
			{
				*lpRef ^= Key;
				Key = *lpRef;

				if(! (--dwCount) ){break;}
				++lpRef;
			} // while(true)
		} // if(dwBlockCount)

		if(dwRemain)
		{
			char* pRef = pSrc + (dwBlockCount * sizeof(KEY_TYPE));
			char* pKey = reinterpret_cast<char*>(&Key);
			while(true)
			{
				*pRef ^= *pKey;

				if(! (--dwRemain) ){break;}

				++pRef;
				++pKey;
			} // while(true)
		} // if(dwRemain)
	}

	void CALLBACK CEL_EL::DREN(char *pSrc, DWORD const dwSrcSize)
	{//DES 알고리즘
		if(1 > dwSrcSize){return;}

		DWORD const dwBlockCount	= ( dwSrcSize / sizeof(KEY_TYPE) ) ;//키의 크기로 나눔.
		register DWORD dwRemain		= ( dwSrcSize % sizeof(KEY_TYPE) );//남은 바이트 계산

		KEY_TYPE Key = m_ks.RDESKey;

		if(dwBlockCount)
		{
			KEY_TYPE* lpRef = reinterpret_cast<KEY_TYPE*>(pSrc + dwSrcSize - sizeof(KEY_TYPE) );//끝점 빼기 1블록.
			register DWORD dwCount = dwBlockCount;
			while(true)
			{
				*lpRef ^= Key;
				Key = *lpRef;

				if(! (--dwCount) ){break;}
				--lpRef;
			} // while(true)
		} // if(dwBlockCount)

		if(dwRemain)
		{
			char* pRef = pSrc + dwRemain - 1;//헤드 짜투리. 배열이므로 -1을 해야 한다.
			char* pKey = reinterpret_cast<char*>(&Key) + sizeof(KEY_TYPE) - 1;
			while(true)
			{
				*pRef ^= *pKey;
				
				if(! (--dwRemain) ){break;}
				--pRef;
				--pKey;
			} // while(true)
		} // if(dwRemain)
	}

	void CALLBACK CEL_EL::RDE(char *pSrc, DWORD const dwSrcSize)
	{//암호화 한 결과물을 사용하지 않는다. 64*4 바이트의 주기가 있음.
		if(1 > dwSrcSize){return;}

		DWORD const dwBlockCount	= ( dwSrcSize / sizeof(KEY_TYPE) ) ;//KeyType으로 나눔
		register DWORD dwRemain		= ( dwSrcSize % sizeof(KEY_TYPE) );//남은 바이트 계산

		register BYTE byIdxPRI = 0;
		register BYTE byIdxSPN = 0;

		if(dwBlockCount)
		{
			KEY_TYPE* lpRef = reinterpret_cast<KEY_TYPE*>(pSrc);

			register DWORD dwCount = dwBlockCount;
			while(true)
			{
				*lpRef ^=  ( (byIdxPRI == byIdxSPN)?(m_ks.aRDKey[byIdxPRI]):( m_ks.aRDKey[byIdxPRI] ^ m_ks.aRDKey[byIdxSPN] ) );//Index가 같으면 XOR시 0이 되므로 키를 하나만 사용
				
				( (MAX_RD_KEY_ARRAY-1) == byIdxSPN )//배열은 -1만큼 해야 하므로
					?	( byIdxSPN = 0, ++byIdxPRI )//스핀키를 원점으로 하면서 프라이머리 키를 증가.
					:	( ++byIdxSPN );//Max에 도달할때 까지 증가

				( MAX_RD_KEY_ARRAY == byIdxPRI )?( byIdxPRI = 0 ): 0;//프라이머리 키 보정 MAX가 되었을떄만 0 으로 보정.

				if(! (--dwCount) ){break;}

				++lpRef;
			} // while(true)
		} // if(dwBlockCount)

		if(dwRemain)
		{
			KEY_TYPE Key = ( (byIdxPRI == byIdxSPN)?(m_ks.aRDKey[byIdxPRI]):( m_ks.aRDKey[byIdxPRI] ^ m_ks.aRDKey[byIdxSPN] ) );

			char* pRef = pSrc + dwBlockCount * sizeof(KEY_TYPE);
			char* pKey = reinterpret_cast<char*>(&Key);
			while(true)
			{
				*pRef ^= *pKey;

				if(! (--dwRemain) ){break;}

				++pRef;
				++pKey;
			} // while(true)
		} // if(dwRemain)
	}

	void CALLBACK CEL_EL::DLDE(char *pSrc, DWORD const dwSrcSize)
	{
		//마지막 블럭에서 키를 찾아낸다.
		//짜투리를 먼저 풀고 나머지 블럭을 역으로 푼다(짜투리와 블럭의 푸는 순서는 상관 없음)
		
		//짜투리 밖에 없다면? DES키만 이용한다.

		if(1 > dwSrcSize){return;}

		DWORD const dwBlockCount	= ( dwSrcSize / sizeof(KEY_TYPE) ) ;//키의 크기로 나눔.
		register DWORD dwRemain		= ( dwSrcSize % sizeof(KEY_TYPE) );//남은 바이트 계산

		KEY_TYPE Key = m_ks.LDESKey;//블럭이 없다면 일반 키를 사용 하면 된다

		if(dwBlockCount)
		{
			KEY_TYPE* lpRef = reinterpret_cast<KEY_TYPE*>(pSrc);
			register DWORD dwCount = dwBlockCount;
			
			KEY_TYPE TempKey = 0;
			while(true)
			{
				TempKey = *lpRef;

				*lpRef ^= Key;

				Key = TempKey;

				if(! (--dwCount) ){break;}
				++lpRef;
			} // while(true)
		} // if(dwBlockCount)

		if(dwRemain)
		{
			char* pRef = pSrc + (dwBlockCount * sizeof(KEY_TYPE));
			char* pKey = reinterpret_cast<char*>(&Key);
			while(true)
			{
				*pRef ^= *pKey;

				if(! (--dwRemain) ){break;}

				++pRef;
				++pKey;
			} // while(true)
		} // if(dwRemain)
	}

	void CALLBACK CEL_EL::DRDE(char *pSrc, DWORD const dwSrcSize)
	{
		if(1 > dwSrcSize){return;}

		DWORD const dwBlockCount	= ( dwSrcSize / sizeof(KEY_TYPE) ) ;//키의 크기로 나눔.
		register DWORD dwRemain		= ( dwSrcSize % sizeof(KEY_TYPE) );//남은 바이트 계산

		KEY_TYPE Key = m_ks.RDESKey;

		if(dwBlockCount)
		{
			KEY_TYPE* lpRef = reinterpret_cast<KEY_TYPE*>(pSrc + dwSrcSize - sizeof(KEY_TYPE) );//끝점 빼기 1블록.
			register DWORD dwCount = dwBlockCount;
			KEY_TYPE TempKey = 0;
			while(true)
			{
				TempKey = *lpRef;
				
				*lpRef ^= Key;
				
				Key = TempKey;

				if(! (--dwCount) ){break;}
				--lpRef;
			} // while(true)
		} // if(dwBlockCount)

		if(dwRemain)
		{
			char* pRef = pSrc + dwRemain - 1;//헤드 짜투리. 배열이므로 -1을 해야 한다.
			char* pKey = reinterpret_cast<char*>(&Key) + sizeof(KEY_TYPE) - 1;
			while(true)
			{
				*pRef ^= *pKey;
				
				if(! (--dwRemain) ){break;}
				--pRef;
				--pKey;
			} // while(true)
		} // if(dwRemain)
	}

	template<typename T1>
	void Swap(T1 &t1, T1 &t2)
	{
		T1 temp = t1;
		t1 = t2;
		t2 = temp;
	}

	void CALLBACK CEL_EL::SwapEn(char *pSrc, DWORD const dwSrcSize)
	{	//블럭끼리 혹은 바이트만.
		//키 값은 현재 MAX_RD_KEY_ARRAY + LDES + RDES * 2(Gab),  확보중
		//륀다엘 키와 Gab을 이용해서 Swap을 진행
		DWORD const dwBlockCount	= ( dwSrcSize / sizeof(KEY_TYPE) ) ;//키의 크기로 나눔.

		if(MIN_SWAP_BLOCK_COUNT > dwBlockCount)//제거 가능성 있음.
		{//미니멈 블록 카운트보다 작은 블록수면 Swap을 하지 않는다.
			return;
		}

		KEY_TYPE* lpRef = reinterpret_cast<KEY_TYPE*>(pSrc);

		int iSrcIdx = 0;
		int iTgtIdx = 0;

		register int iCount = MAX_RD_KEY_ARRAY;
		while(iCount)
		{//블럭끼리 메모리가 겹치는 일이 없도록 할 것.
			--iCount;//배열 인덱스 이므로 최상단.
			iSrcIdx = m_ks.aRDKey[iCount] % dwBlockCount;
			iTgtIdx = m_gs.aRDGab[iCount] % dwBlockCount;
			
			KEY_TYPE &rSrc = *(lpRef + iSrcIdx);
			KEY_TYPE &rTgt = *(lpRef + iTgtIdx);

	//		(iSrcIdx == iTgtIdx) 
	//			? 0 
	//			: 
			Swap( rSrc, rTgt );
		}
	}

	void CALLBACK CEL_EL::SwapDe(char *pSrc, DWORD const dwSrcSize)
	{	//블럭끼리 혹은 바이트만.
		//키 값은 현재 MAX_RD_KEY_ARRAY + LDES + RDES * 2(Gab),  확보중
		//륀다엘 키와 Gab을 이용해서 Swap을 진행
		DWORD const dwBlockCount	= ( dwSrcSize / sizeof(KEY_TYPE) ) ;//키의 크기로 나눔.

		if(MIN_SWAP_BLOCK_COUNT > dwBlockCount)//제거 가능성 있음.
		{//미니멈 블록 카운트보다 작은 블록수면 Swap을 하지 않는다.
			return;
		}

		KEY_TYPE* lpRef = reinterpret_cast<KEY_TYPE*>(pSrc);
		
		int iSrcIdx = 0;
		int iTgtIdx = 0;

		register int iCount = 0;
		while(MAX_RD_KEY_ARRAY != iCount)
		{//블럭끼리 메모리가 겹치는 일이 없도록 할 것.
			iSrcIdx = m_ks.aRDKey[iCount] % dwBlockCount;
			iTgtIdx = m_gs.aRDGab[iCount] % dwBlockCount;
			
			KEY_TYPE &rSrc = *(lpRef + iSrcIdx);
			KEY_TYPE &rTgt = *(lpRef + iTgtIdx);

		//	(iSrcIdx == iTgtIdx) 
		//		? 0 
		//		: 
			Swap( rSrc, rTgt );

			++iCount;//배열 인덱스 이므로 최상단.
		}
	}

	CEL_EL::KEY_TYPE CALLBACK CEL_EL::GetDataCRC(char const *lpStream, DWORD const dwSrcSize)
	{
		size_t const block_size = sizeof(__int64);

		__int64 iCRCValue	=	0;

		__int64 iGabCRC1	=	0;
		__int64 iGabCRC2	=	0;
		__int64 iGabCRC3	=	0;
		__int64 iGabCRC4	=	0;

		__int64 iBlockCount		= (dwSrcSize/block_size);	//블럭수 가지고 오기 == __int32 iBlockCount = iStreamSize/블럭사이즈;
		__int64 iRemainCount	= (dwSrcSize%block_size);	//짜투리 길이 받기==__int32 iRemainCount = iStreamSize%블럭사이즈; 
		__int64 iRemainPos		= (iBlockCount*block_size);	//짜투리 바이트 단위 위치 iBlockCount*블럭사이즈

		__int64 const *lpBlock	= reinterpret_cast<__int64 const*>(lpStream);

		while(iBlockCount)
		{
			switch(iBlockCount%4)
			{//break 없음. 부분의 누적 CRC 값을 사용하기 위함.
			case 0:{iGabCRC1 	^= (*(lpBlock));}
			case 1:{iGabCRC2 	^= (*(lpBlock));}
			case 2:{iGabCRC3 	^= (*(lpBlock));}
			case 3:{iGabCRC4 	^= (*(lpBlock));}
			}

			iCRCValue	^= (*(lpBlock));
			lpBlock++;
			iBlockCount--;
		}
		while(iRemainCount)
		{
			switch(iRemainCount)
			{//break 없음. 부분의 누적 CRC 값을 사용하기 위함.
			case 0:{iGabCRC1 	^= (lpStream[iRemainPos]<<0);}
			case 1:{iGabCRC2 	^= (lpStream[iRemainPos]<<8);}
			case 2:{iGabCRC3 	^= (lpStream[iRemainPos]<<16);}
			case 3:{iGabCRC4 	^= (lpStream[iRemainPos]<<24);}
			case 4:{iGabCRC1 	^= (lpStream[iRemainPos]<<24);}
			case 5:{iGabCRC2 	^= (lpStream[iRemainPos]<<16);}
			case 6:{iGabCRC3 	^= (lpStream[iRemainPos]<<8);}
			case 7:{iGabCRC4 	^= (lpStream[iRemainPos]<<0);}
			}
			iCRCValue	^=	lpStream[iRemainPos];
			
			iRemainPos++;
			iRemainCount--;
		}

		__int64 const iRet = iCRCValue ^ iGabCRC1 ^ iGabCRC2 ^ iGabCRC3 ^ iGabCRC4;
		DWORD const *pkRet = reinterpret_cast<DWORD const *>(&iRet);

		return (*(pkRet)^(*(pkRet+1)));
	}

	WORD CEL_EL::CRCToWORD(DWORD const& kInCrc)
	{
		return (HIWORD(kInCrc) ^ LOWORD(kInCrc));
	}

	BYTE CEL_EL::CRCToByte(DWORD const& kInCrc)
	{
		return		LOBYTE(HIWORD(kInCrc)) ^ LOBYTE(LOWORD(kInCrc))
				^	HIBYTE(HIWORD(kInCrc)) ^ HIBYTE(LOWORD(kInCrc));
	}
}