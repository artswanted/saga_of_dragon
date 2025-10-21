#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGNOTICE_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGNOTICE_H

#include "BM/PgTask.h"
#include "Lohengrin/GameTime.h"

typedef struct tagNotice
{
	tagNotice( E_NOTICE_TYPE const _kType=NOTICE_ALL, BM::GUID const &_kGuid=BM::GUID::NullData() )
		:	kType(_kType)
		,	kGuid(_kGuid)
		,	pkPacket(NULL)
		,	bNew(false)
	{}

	~tagNotice()
	{
		Release();
	}

	void Swap( BM::Stream &kPacket )
	{
		if ( Create() )
		{
			pkPacket->Swap( kPacket	);
		}
	}

	E_NOTICE_TYPE	kType;
	BM::GUID		kGuid;

	BM::Stream		*pkPacket;
	bool			bNew;

protected:

	bool Create()
	{
		if ( !pkPacket )
		{
			pkPacket = new BM::Stream;
			bNew = true;
		}
		return NULL != pkPacket;
	}

	void Release()
	{
		if ( bNew )
		{
			delete pkPacket;
			pkPacket = NULL;
			bNew = false;
		}
	}
}SNotice;

class PgNotice_All
{
public:
	PgNotice_All();
	virtual ~PgNotice_All();

	virtual E_NOTICE_TYPE GetType()const{return NOTICE_ALL;}

	virtual void WriteToPacket( BM::Stream &kPacket )const;
	virtual bool Send( BM::Stream const &kPacket )const;

	__int64 GetRequestTime(void)const{return m_i64ReqTime;}
	bool IsSent(void)const{return m_bSent;}
	__int64 GetValidTime(void)const{return m_i64ValidTime;}

	void SetSent(){m_bSent = true;}
	void SetRequestTime( __int64 const i64ReqTime ){m_i64ReqTime = i64ReqTime;}
	void SetValidTime( __int64 const i64ValidTime ){m_i64ValidTime = i64ValidTime;}
	void SetPacket( BM::Stream const &kPacket );
	
private:
	__int64			m_i64ReqTime;// 이 시간의 의미는 공지를 전송하는 시간의 의미이다.
	bool			m_bSent;
	__int64			m_i64ValidTime;// 이 시간의 의미는 새로로그인한 놈한테도 알려주기 위해(언제까지 유효한지)
	BM::Stream		m_kPacket;
};

class PgNotice_Guild
	:	public PgNotice_All
{
public:
	PgNotice_Guild();
	virtual ~PgNotice_Guild();

	virtual E_NOTICE_TYPE GetType()const{return NOTICE_GUILD;}

	virtual void WriteToPacket( BM::Stream &kPacket )const;
	virtual bool Send( BM::Stream const &kPacket )const;

	void SetGuildGuid( BM::GUID const &kGuildGuid ){m_kGuildGuid = kGuildGuid;}

private:
	BM::GUID		m_kGuildGuid;
};

class PgNoticeSystem;
class PgNotice_GuildChatMsg
{
public:
	friend class PgNoticeSystem;
	typedef std::deque<BM::Stream> CONT_MSG;
public:
	void Send(BM::GUID const& kMemberGuid)const;
	void Add(BM::Stream & rkPacket);

private:
	CONT_MSG m_kContMsg;
};

class PgNotice_TradeChatMsg :
	public PgNotice_GuildChatMsg
{
};

class PgNoticeSystem
{
public:
	typedef std::list< PgNotice_All* >		CONT_NOTICE;
	typedef std::map<BM::GUID, PgNotice_GuildChatMsg>	CONT_GUILD_CHAT_MSG;	//first: GuildGuid, second: BM::Stream

public:
	PgNoticeSystem();
	~PgNoticeSystem();

	void Add( PgNotice_All * pkNotice );
	void OnTick();
	void OnLogin( BM::GUID const &kGuid );

	void SendGuildChatMsg( BM::GUID const& kMemberGuid, BM::GUID const& kGuildGuid )const;
	void AddGuildChatMsg( BM::GUID const& kGuildGuid, BM::Stream & rkPacket );

	void SendTradeChatMsg( BM::GUID const& kMemberGuid );
	void AddTradeChatMsg( BM::Stream & rkPacket );

	bool Load(const char* filename);
	bool Save(const char* filename);
private:
	CONT_NOTICE		m_kContNotice;
	BM::Stream		m_kLoginUserPacket;
	SET_GUID		m_kLoginPacketWaiter;
	CONT_GUILD_CHAT_MSG m_kContGuildChatMsg;

	PgNotice_TradeChatMsg m_kContTradeChatMsg;
	Loki::Mutex m_kContTradeChatMutex;
};

#define CHAT_FILENAME ".\\chat.bin"

class PgNoticeSystem_Wrapper
	:	TWrapper< PgNoticeSystem, Loki::Mutex >
{
public:
	PgNoticeSystem_Wrapper();
	virtual ~PgNoticeSystem_Wrapper();

	void Add( PgNotice_All * pkNotice );
	void OnTick();
	void OnLogin( BM::GUID const &kGuid );

	void ProcessPacket( BM::Stream * pkPacket );

	// Not impl now, we have a problem with corrupted file
	inline bool Load() { return false; } // { return Instance()->Load(CHAT_FILENAME); }
	inline bool Save() { return false; } // { return Instance()->Save(CHAT_FILENAME); }
private:
	void SendGuildChatMsg( BM::GUID const& kMemberGuid, BM::GUID const& kGuildGuid )const;
	void AddGuildChatMsg( BM::GUID const& kGuildGuid, BM::Stream & rkPacket );
};

#undef CHAT_FILENAME

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGNOTICE_H