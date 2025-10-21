#pragma once 
#include <limits>

#pragma pack(1)

typedef struct tagCorePacketHeader
{
	typedef enum
	{ 
		MAX_BODY_SIZE = (1024*128) - 1,//17 비트. 100k. 까지만 허용
	}eValue;
//계산에 사용 되므로 비트 플래그를 바꾸지 마시오.(VImprovePacket)
	unsigned __int64 m_kRand : 3;//랜덤: 용도 혼란 가중
	unsigned __int64 m_kBodySize : 17;//패딩 포함한 사이즈 ^ (Pad << Rand)
	unsigned __int64 m_kEncCRC : 32;//암호화 된것의 CRC ^ Rand ^ (BodySize << Rand)
	unsigned __int64 m_kPad : 4;//패딩 사이즈 15 까지 ^ Rand
	unsigned __int64 m_kOrgCRC: 8;//패딩포함 CRC (CRC ^ BodySize) + Rand + Pad
	
	//	Rand -> Pad -> BodySize -> EncCRC, OrgCRC 순서로 알아낼 수 있음.
	void Rand(BYTE const kInRand){m_kRand = kInRand;};
	void Pad(BYTE const byInPad){m_kPad = (byInPad ^ m_kRand);};
	void BodySize(size_t const in_body_size){m_kBodySize = (in_body_size ^ (m_kPad << m_kRand));};
	void EncCRC(DWORD const dwInCRC){m_kEncCRC = (dwInCRC ^ m_kRand ^ (m_kBodySize << m_kRand));}
	void OrgCRC(BYTE const byInOrgCrc){m_kOrgCRC = ((byInOrgCrc ^ m_kBodySize) + m_kRand + m_kPad);};

	BYTE Rand()const{return static_cast<BYTE>(m_kRand);};
	BYTE Pad()const{return static_cast<BYTE>(m_kPad ^ m_kRand);};
	size_t BodySize()const{return  static_cast<size_t>(m_kBodySize ^ (m_kPad << m_kRand));};
	DWORD EncCRC()const{return  (m_kEncCRC ^ m_kRand ^ (m_kBodySize << m_kRand));}
	BYTE OrgCRC()const {return static_cast<BYTE>((m_kOrgCRC - m_kRand - m_kPad ) ^ m_kBodySize );};

	tagCorePacketHeader()
	{
		m_kRand = 0;
		m_kBodySize = 0;
		m_kEncCRC = 0;
		m_kPad = 0;
		m_kOrgCRC = 0;
	}
}SCorePacketHeader;

typedef struct tagCorePacketHeader_ServerToServer
{
	typedef size_t SIZE_TYPE;

	typedef enum
	{
		MAX_BODY_SIZE = UINT_MAX,
	}eValue;

	SIZE_TYPE kBodySize;
	tagCorePacketHeader_ServerToServer()
	{
		kBodySize = 0;
	}

	size_t BodySize()const{return kBodySize;}
	DWORD CRC()const{return 0;}

	bool IsEncode()const{return false;}

	void BodySize(size_t const dwInSize){kBodySize = dwInSize;}
	void SetEncode(bool const bIsEncode){}
	void SetCRC(DWORD dwCRC){}
}SCorePacketHeader_ServerToServer;

typedef struct tagCorePacketHeader_NC
{
	typedef short SIZE_TYPE;

	typedef enum
	{
		MAX_BODY_SIZE = SHRT_MAX,//24 비트
	}eValue;

	SIZE_TYPE kBodySize;
	tagCorePacketHeader_NC()
	{
		kBodySize = 0;
	}

	size_t BodySize()const{return kBodySize;}
	DWORD CRC()const{return 0;}

	bool IsEncode()const{return false;}

	void BodySize(size_t const dwInSize){kBodySize = static_cast< SIZE_TYPE >(dwInSize);}//사이즈 타입을 넘어 서면 곤란합니다.
	void SetEncode(bool const bIsEncode){}
	void SetCRC(DWORD dwCRC){}
}SCorePacketHeader_NC;

// 대만 SoftWorld GF 연동
typedef struct tagCorePacketHeader_GF
{
	typedef unsigned short SIZE_TYPE;

	typedef enum
	{
		MAX_BODY_SIZE = USHRT_MAX,//24 비트
	}eValue;

	SIZE_TYPE kBodySize;
	tagCorePacketHeader_GF()
		:	kBodySize(0)
	{}

	size_t BodySize()const{return kBodySize;}
	DWORD CRC()const{return 0;}

	bool IsEncode()const{return false;}

	void BodySize(size_t const dwInSize){kBodySize = static_cast< SIZE_TYPE >(dwInSize);}//사이즈 타입을 넘어 서면 곤란합니다.
	void SetEncode(bool const bIsEncode){}
	void SetCRC(DWORD dwCRC){}
}SCorePacketHeader_GF;

typedef struct tagCorePacketHeader_FileServer
{
	typedef size_t SIZE_TYPE;//20억 까지 밖에 안나오겠네
	
	typedef enum
	{
		MAX_BODY_SIZE = UINT_MAX,//24 비트in냐
	}eValue;

	SIZE_TYPE kBodySize;
	DWORD kCRC;
	BYTE  kFlag;

	tagCorePacketHeader_FileServer()
	{
		kBodySize = 0;
		kCRC = 0;
		kFlag = 0;
	}

	size_t BodySize()const{return kBodySize;}
	DWORD CRC()const{return kCRC;}

	bool IsEncode()const{return false;}

	void BodySize(size_t const dwInSize){kBodySize = dwInSize;}
	void SetEncode(bool const bIsEncode){return;}	
	void SetCRC(DWORD dwCRC){kCRC = dwCRC;}
}SCorePacketHeader_FileServer;

typedef struct tagCorePacketHeader_NoHeader
{
	typedef unsigned short SIZE_TYPE;

	typedef enum
	{
		MAX_BODY_SIZE = USHRT_MAX,//24 비트
	}eValue;

	SIZE_TYPE kBodySize;
	tagCorePacketHeader_NoHeader()
		:	kBodySize(0)
	{}

	size_t BodySize()const{return kBodySize;}
	DWORD CRC()const{return 0;}

	bool IsEncode()const{return false;}

	void BodySize(size_t const dwInSize){kBodySize = static_cast< SIZE_TYPE >(dwInSize);}//사이즈 타입을 넘어 서면 곤란합니다.
	void SetEncode(bool const bIsEncode){}
	void SetCRC(DWORD dwCRC){}
	size_t GetHeaderSize() { return 0; }
}SCorePacketHeader_NoHeader;

typedef struct tagCorePacketHeader_JAPAN
{
	typedef unsigned short SIZE_TYPE;

	typedef enum
	{
		MAX_BODY_SIZE = USHRT_MAX,//24 비트
	}eValue;

	BYTE byMainHeader;
	BYTE abySize[2];
	tagCorePacketHeader_JAPAN()
		:byMainHeader(0x55)
	{}

	size_t BodySize()const{return MAKEWORD(abySize[1], abySize[0]);}
	DWORD CRC()const{return 0;}

	bool IsEncode()const{return false;}

	void BodySize(size_t const dwInSize)
	{
		SIZE_TYPE kTempType = dwInSize;
		BYTE *pkTemp = (BYTE *)(&kTempType);

		abySize[0] = *(pkTemp+1);
		abySize[1] = *(pkTemp);
//		kBodySize = static_cast< SIZE_TYPE >(dwInSize);
	}//사이즈 타입을 넘어 서면 곤란합니다.
	void SetEncode(bool const bIsEncode){}
	void SetCRC(DWORD dwCRC){}
}SCorePacketHeader_JAPAN;


typedef struct tagCorePacketHeader_GRAVITY
{
	typedef DWORD SIZE_TYPE;

	typedef enum
	{
		MAX_BODY_SIZE = UINT_MAX,//24 비트
		COMMAND_LEN = 4,
		BODY_LEN = 4,
	}eValue;

	char acCommand[COMMAND_LEN];
	char acBodySize[BODY_LEN];
	tagCorePacketHeader_GRAVITY()
	{
		memset(acCommand, 0, sizeof(acCommand));
		memset(acBodySize, 0, sizeof(acBodySize));
	}

	size_t BodySize()const
	{
		char acTempBodySize[BODY_LEN+1] = {0,};
		memcpy(acTempBodySize, acBodySize, BODY_LEN);

		BM::vstring kStr;
		kStr = acTempBodySize;
		return (int)kStr;
	}
	DWORD CRC()const{return 0;}

	bool IsEncode()const{return false;}

	void BodySize( size_t const dwInSize ) 
	{ 
		char acTempBodySize[BODY_LEN+1] = {0,};
		sprintf_s( acTempBodySize, "%04d", dwInSize );
		memcpy( acBodySize, acTempBodySize, BODY_LEN );
	}
	void SetEncode(bool const bIsEncode){}
	void SetCRC(DWORD dwCRC){}
}SCorePacketHeader_GRAVITY;

#pragma pack()