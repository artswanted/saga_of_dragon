#ifndef WEAPON_VARIANT_PGNOTICEACTION_H
#define WEAPON_VARIANT_PGNOTICEACTION_H

class PgNoticeAction
{
	typedef enum eRcvType
	{
		ERT_NONE,
		ERT_USER,
		ERT_GM
	}ERcvType;
private:
	Loki::Mutex m_kMutex;

	int				m_iReceiver;
	unsigned short	m_usRealm;
	unsigned short	m_usChannel; 
	unsigned short	m_usGround;
	std::wstring	m_wstrText;
public:
	void SetNoticeData(int iReceiver = ERT_USER , unsigned short usRealm = 0, unsigned short usChannel = 0, 
		unsigned short usGround = 0, std::wstring const &rkContents = L"");
	bool Send(int iServerType);
	bool Recv(BM::Stream *pkPacket);

	PgNoticeAction(void);
	~PgNoticeAction(void);
};

#endif // WEAPON_VARIANT_PGNOTICEACTION_H