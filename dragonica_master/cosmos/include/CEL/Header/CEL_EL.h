#pragma once

#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>	
#include "Loki/Threads.h"
#include "BM/ClassSupport.h"

#pragma pack (1)
	
namespace CEL
{
	//순서가 보장되는 환경에서만 동작합니다.
	//Encrypt 내부에 Lock 을 잡고 있지만.
	//Lock 은 순서 보장을 의미 하지는 않습니다.
	//사용자가 스스로 암호화 호출 메세지를 선형화 시켜 사용 해야 합니다.

	class CEL_EL
	{
	public:
		typedef DWORD KEY_TYPE;//__int64 이상 하지 말것.
		typedef BYTE GAB_TYPE;//short 이상 하지 말것
		typedef void (CALLBACK CEL_EL::*LPHSELFUNC)(char *, DWORD const);
		typedef void (CALLBACK CEL_EL::*LPHSELKEYFUNC)();
		typedef WORD INIT_INFO_KEY_TYPE;//short 이상 하지 말것
		typedef WORD INIT_INFO_CRC_TYPE;//short 이상 하지 말것

	public:
		typedef enum 
		{
			CVERSION				= 2,
			MAX_RD_KEY_ARRAY		= 8,
			MIN_SWAP_BLOCK_COUNT	= 4,
		}eCINFO;

		typedef enum 
		{
			ENCRYPT_CMP_BASE					= 0x0000000F,
			ENCRYPT_FAST						= 0x00000001,//속도는 빠르지만 쇄도효과 없음.
			ENCRYPT_SECURE						= 0x00000002,//속도는 느리지만 양방향 쇄도 효과 있음.
		
			SWAP_CMP_BASE						= 0x000000F0,
			SWAP_ON								= 0x00000010,//데이터 섞기
			SWAP_OFF							= 0x00000020,//데이터 섞지 않음
			
			KEY_ROLL_CMP_BASE					= 0x00000F00,
			KEY_ROLL_DYNAMIC					= 0x00000100,//1회의 Encrypt 후 키가 바뀜
			KEY_ROLL_STATIC						= 0x00000200,//처음 키를 계속해서 사용

			KEY_INIT_CMP_BASE					= 0x0000F000,//
			KEY_INIT_AUTO						= 0x00001000,//Lib가 Key를 자동으로 만들어줌
			KEY_INIT_MANUAL						= 0x00002000,//Key를 수동으로 셋팅
		}eINIT_FLAG;

		typedef enum 
		{
			INIT_SUCCESS						= 0x00000001,
			INIT_MANUAL_FAILED					= 0x00100000,
			INIT_MANUAL_CRC_ERROR				= 0x00200000,

			INIT_ENCRYPT_OPT_FAULT				= 0x00000010,
			INIT_SWAP_OPT_FAULT					= 0x00000100,
			INIT_KEY_ROLL_OPT_FAULT				= 0x00001000,
			INIT_KEY_INIT_FAULT					= 0x00010000,
			INIT_KEY_INIT_FAULT_PLZ_USE_AUTO	= 0x00020000,
			INIT_KEY_INIT_FAULT_PLZ_USE_MANUAL	= 0x00040000,
		}eRESULT_CODE;

	private:
		typedef struct tagKeyStore
		{//반드시 홀수
			tagKeyStore()
			{
				Clear();
			}
			void Clear()
			{
				::memset(aRDKey, 0, sizeof(KEY_TYPE)*MAX_RD_KEY_ARRAY);
				LDESKey = 0;
				RDESKey = 0;
			}

			KEY_TYPE aRDKey[MAX_RD_KEY_ARRAY];
			KEY_TYPE LDESKey;
			KEY_TYPE RDESKey;
		}KEY_STORE, *LPKEY_STORE;

		typedef struct tagGabStore
		{//반드시 홀수
			tagGabStore()
			{
				Clear();
			}

			void Clear()
			{
				MultiGab = 0;
				::memset(aRDGab, 0, sizeof(GAB_TYPE)*MAX_RD_KEY_ARRAY);
				LDESGab = 0;
				RDESGab = 0;
			}

			GAB_TYPE MultiGab;//공통적인 곱셈값
			GAB_TYPE aRDGab[MAX_RD_KEY_ARRAY];
			GAB_TYPE LDESGab;
			GAB_TYPE RDESGab;
		}GAB_STORE, *LPGAB_STORE;
		
	public:
		typedef struct tagInitInfo
		{//저장 순서를 클래스와 다르게 설정.
			tagInitInfo()
			{
				Key = 0;
				CRC = 0;
				dwInitFlag = 0;
			}

			INIT_INFO_KEY_TYPE Key;
			INIT_INFO_CRC_TYPE CRC;

			DWORD dwInitFlag;
			GAB_STORE gs;
			KEY_STORE ks;
		}INIT_INFO, *LPINIT_INFO;

	public:
		CEL_EL();
		~CEL_EL();

	public:
		eRESULT_CODE InitManual(INIT_INFO const& rInitInfo);//Key만 따로 셋팅 하는것 필요
		eRESULT_CODE Init(DWORD const& dwInitFlag);//Flag로 초기화

		void Clear();//메모리 할당해서 쓰는것이 없으므로 소멸시에는 호출 필요 없음

		bool GetInitInfo(INIT_INFO &rInitInfo)const;//여기서 받은 Info를 디코더에 전달..
		
		bool Encrypt(char *pSrc, DWORD const dwSrcSize, DWORD &dwOutCRC);
		bool Decrypt(char *pSrc, DWORD const dwSrcSize, DWORD &dwOutCRC);

		bool Encrypt(char const *pSrc, char *pDest, DWORD const dwSrcSize, DWORD &dwOutCRC);
		bool Decrypt(char const *pSrc, char *pDest, DWORD const dwSrcSize, DWORD &dwOutCRC);

		static WORD CRCToWORD(DWORD const& kInCrc);
		static BYTE CRCToByte(DWORD const& kInCrc);

		static KEY_TYPE CALLBACK GetDataCRC(char const *pSrc, DWORD const dwSrcSize);
	protected:
		bool EncryptInitInfo(INIT_INFO &rInitInfo)const;
		bool DecryptInitInfo(INIT_INFO &rInitInfo)const;

		void CALLBACK REN(char *pSrc, DWORD const dwSrcSize);
		void CALLBACK DLEN(char *pSrc, DWORD const dwSrcSize);
		void CALLBACK DREN(char *pSrc, DWORD const dwSrcSize);

		void CALLBACK RDE(char *pSrc, DWORD const dwSrcSize);
		void CALLBACK DLDE(char *pSrc, DWORD const dwSrcSize);
		void CALLBACK DRDE(char *pSrc, DWORD const dwSrcSize);

		void CALLBACK SwapEn(char *pSrc, DWORD const dwSrcSize);
		void CALLBACK SwapDe(char *pSrc, DWORD const dwSrcSize);

		void InitKey();
		void InitGab();

		void CALLBACK NextKey();//다음키 셋팅

		KEY_TYPE MakeKey();
		GAB_TYPE MakeGab();

	protected:
		CLASS_DECLARATION_S_NO_SET(bool, IsInit);//외부에서는 Set을 할 수 없음

		KEY_STORE m_ks;
		GAB_STORE m_gs;

		DWORD m_dwInitFlag;
		
		LPHSELFUNC m_pFuncEn1;
		LPHSELFUNC m_pFuncEn2;
		LPHSELFUNC m_pFuncEn3;

		LPHSELFUNC m_pFuncDe1;
		LPHSELFUNC m_pFuncDe2;
		LPHSELFUNC m_pFuncDe3;

		LPHSELFUNC m_pFuncEnSwap;
		LPHSELFUNC m_pFuncDeSwap;

		LPHSELKEYFUNC m_pFuncKeyRoll;

		size_t m_dec_count;
		size_t m_enc_count;
		
		mutable Loki::Mutex m_kMutex;

	protected:
		CEL_EL& operator=(const CEL_EL &rhs);//대입연산자 사용 불가.
		CEL_EL(const CEL_EL &rhs);//복사생성자 사용 불가.
	};
}

#pragma pack ()