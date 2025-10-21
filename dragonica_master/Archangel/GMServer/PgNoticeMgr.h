#ifndef GM_GMSERVER_GM_PGNOTICEMGR_H
#define GM_GMSERVER_GM_PGNOTICEMGR_H

class PgNoticeMgr
{
	typedef struct TagNotice
	{
		BM::GUID			kNoticeGuid;
		BM::DBTIMESTAMP_EX	m_kStartTime;
		__int64				m_i64NextNotiTime;
		int					m_iRepeatCountSet;
		int					m_iRepeatTime;
		unsigned short		m_usNowTextCount;
		unsigned short		m_usRealmNo;
		unsigned short		m_usChannelNo;
		unsigned short		m_usNowLineCount;
		std::vector<std::wstring> m_kText;
		EOrderState			eOrderState;
		TagNotice():m_i64NextNotiTime(0),m_iRepeatCountSet(0),m_usNowTextCount(0),m_iRepeatTime(0),m_usNowLineCount(0),eOrderState(OS_DOING){}
	} SNotice;

	typedef std::map<BM::GUID, SNotice> CONT_NOTICE_DATA;
	typedef std::vector<std::wstring> NOTICE_TEXT;

protected:

	CONT_NOTICE_DATA m_kContNotice;
	mutable Loki::Mutex m_kMutex;
	CLASS_DECLARATION_S(__int64, NoticeLineIntervalSec);	// 한개의 공지에서 라인마다 출력될 간격 (second)
	CLASS_DECLARATION_S(__int64, NoticeItemIntervalSec);	// 한개의 공지 후, 다음 공지가 출력될 간격 (second)

private:

	unsigned short MakeSendString(std::wstring const &kText, NOTICE_TEXT &rkOut);

public:

	void SetNoticeData(SGmOrder const &kNoticeData);
	void DeleteNoticeData(SGmOrder const &kNoticeData);

	void CheckTime();
	PgNoticeMgr(void);
	~PgNoticeMgr(void);
};

#define g_kNoticeMgr SINGLETON_STATIC(PgNoticeMgr)

#endif // GM_GMSERVER_GM_PGNOTICEMGR_H