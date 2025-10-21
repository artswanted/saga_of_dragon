#pragma once
#include <guiddef.h>
#include <Limits>

#include "ace/asynch_io.h"
#include "ace/Task.h"
#include "ace/Signal.h"
#include "ace/sock_stream.h"
#include "ace/Mutex.h"
#include "ace/Recursive_thread_Mutex.h"

#include "BM/Observer.h"
#include "BM/Guid.h"
#include "BM/ClassSupport.h"
#include "BM/LogWorker_Base.h"
#include "BM/LockChecker.h"

#include "CEL/common.h"

#define IG_LOG  /##/

namespace CEL
{
	//세션과 핸들러가 동일시 되지 않아 문제가 생김.
	// ACE 는 Handler 만 알고.
	// CEL 은 Session 만 알고.
	// 상호 참조가 안되기 때문에 이렇게.

	//! ACE_Asynch_Read_Stream -> 재사용 하면 메모리가 새므로 풀 방식은 해서는 안된다.
	template< typename T_SESSION >
	class Service_Handler_Gravity//! 가능한한 최소한의 네트웍 기능을 가진다.
		:	public ACE_Service_Handler
		,	public BM::CSubject< eNOTIFY_CAUSE >
		,	public T_SESSION
	{	//! NC_COMPLETE_R ==> Recv NC_COMPLETE_S ==> Send
		//		typedef typename ACE_Recursive_Thread_Mutex MUTEX;
		//! 선언 순서대로 우선순위.
	public:
		typedef typename T_SESSION::HEADER_TYPE								HEADER_TYPE;
		typedef typename Service_Handler_Gravity< T_SESSION >				THIS_TYPE;

		typedef typename T_SESSION::PACKET_TYPE								PACKET_TYPE;
		typedef ACE_Message_Block MSG_BLK;

		friend class ACE_Asynch_Acceptor< THIS_TYPE >;
		friend class ACE_Asynch_Connector< THIS_TYPE >;

	private:	
		typedef enum eResultCode
		{
			E_R_NONE = 0,
			E_R_W_STOP_SIGNAL,
			E_R_W_DEQUE_FAILED,
			E_R_W_WRITE_FAILED,
			E_R_W_WRITE_FAILED_ALREADY_CLOSE,//이미 닫혀있었다.

			E_R_W_IMPROVE_FAILED,
			E_R_W_ENQUEUE_FAILED,
			E_R_W_SIZE_FAILED,

			E_R_W_SUCCESS,

			E_POST_READ_FAILED,

			EC_CS_BY_RESET, //! CLOSE_SOCKET

			E_OPEN_FUNCTION_NULL,
			E_OPEN_INIT_WRITER_FAILED	,
			E_OPEN_INIT_READER_FAILED	,
			E_OPEN_TRY_POST_READ_FAILED	,
			E_OPEN_TRY_POST_WRITE_FAILED,

			E_ONCLOSE_BY_OPEN			,
			E_ONCLOSE_BY_HANDLE_READ	,
			E_ONCLOSE_BY_HANDLE_WRITE	,
			E_ONCLOSE_BY_TERMINATE_ALL	,
		}E_RESULT_CODE;

	public:
		Service_Handler_Gravity(INIT_CORE_DESC const& kInitInfo = INIT_CORE_DESC());
		virtual ~Service_Handler_Gravity(void);

	public://T_SESSION
		virtual bool VSend( PACKET_TYPE const& rPacket, bool const bIsEncryption = true);
		virtual bool VTerminate();

		ACE_INET_Addr const& RemoteAddr() const;
		ACE_Time_Value const& LastNetworkActTime(){return m_kLastNetworkActTime;}

		virtual HRESULT Statistics(SSessionStatistics &rkOutResult) const;

	protected://! 외부호출 금지. ACE 내부에서 쓰는것.
		virtual void open(ACE_HANDLE new_handle, MSG_BLK &message_block);

		virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result &result);		
		virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result &result);

		virtual void addresses(ACE_INET_Addr const& rRemoteAddr, ACE_INET_Addr const& rLocalAddr);
		virtual void act (void const *pAct);//! Onconnect 시에 호출 반드시 SConnectInfo 를 넣었어야 한다.

		bool PostWrite( MSG_BLK *pMsgBlk );//! --> 이걸 레퍼런스로 받아라.
		bool PostRead();

	protected:
		bool PopRecved(PACKET_TYPE &rOutPacket, HEADER_TYPE &kHeader);

		bool PopFromHeader( void* pDst, size_t const size, bool const bPtrMove );
		void TryClose(const E_RESULT_CODE eCause);//! 접속이 끊어지면 호출 됨. 사용중인 메세지 블럭도 돌려주는 처리.

		void ShutDown(int const iCallType);

		HRESULT NewBlk(MSG_BLK* &pkBlk, size_t const blk_size)const;
		void ClearBlk(MSG_BLK* &pkBlk)const;//완전 깨끗이 제거.
		void AdjustBlk(MSG_BLK* &pkBlk)const;
		void Clear();

		bool IsCorrectBandwidth(__int64 const iRecvBytes);
	protected:
		ACE_Asynch_Read_Stream  m_kReader;   //! 재사용 하면 메모리가 새므로 풀 방식은 해서는 안된다.
		ACE_Asynch_Write_Stream m_kWriter;   //! 재사용 하면 메모리가 새므로 풀 방식은 해서는 안된다.

		ACE_INET_Addr m_kRemoteAddr;
		ACE_Time_Value m_kLastNetworkActTime;

		ACE_Time_Value m_kDosCheckTime;
		__int64 m_iRecvBytesForDosCheck;

		ACE_Message_Queue< ACE_MT_SYNCH > m_kSendQueue;

		MSG_BLK	*m_pRecvHeadBlk;//AdjustBlk 으로 앞에 쌓인 블록들을 처리한다.

		INIT_CORE_DESC const& m_kInitInfo;

		SSessionStatistics m_kStatistics; //같은거 쓰자꾸나

		E_RESULT_CODE m_kCloseCause;
		void CheckVerify()const;

		/////////////////////////////////////
		Loki::Mutex	m_kSendMutex;//Vsend 순서 맞추기(암호화 때문에, SendQueue 에 들어가는 순서가 맞아야함.
		Loki::Mutex	m_kFlagMutex;//IsDoWrite, IsDoRead 보호.

		CLASS_DECLARATION_S(bool, IsDoWrite);
		CLASS_DECLARATION_S(bool, IsDoRead);

		CLASS_DECLARATION_S(ACE_HANDLE, MyHandle);//CloseSocket을 위함
	};

#include "Service_Handler_Gravity.inl"
}; 