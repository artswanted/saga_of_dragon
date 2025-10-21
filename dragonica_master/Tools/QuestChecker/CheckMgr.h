#pragma once

typedef enum eLoadType
{
	LT_INIT,
	LT_LAOD_DB,
	LT_LOAD_QUEST_TEXT_TABLE,
	LT_LOAD_TEXT_TABLE,
	LT_QUEST_LIST_LOAD,
	LT_QUEST_XML_PARSE,
	LT_MAP_XML_PARSE,
	LT_QTTV,
	LT_QUEST_XML_VERIFY,
}ELOADTYPE;

namespace DefStringHelper
{
	std::wstring GetText(int const iDefStringNo);
	std::wstring GetMapName(int const iMapNo);
	std::wstring GetEffectName(int const iEffectNo);
	std::wstring GetSkillName(int const iSkillNo);
	std::wstring GetItemName(int const iItemNo);
	std::wstring GetMonsterName(int const iMonsterNo);
}

namespace TextHelper
{
	void Clear();
	void Load();
	std::wstring GetText(int const iTextID);
};

namespace HtmlUtil
{
	bool MakeFile(std::wstring const& rkFileName, std::wstring const& rkText);
	void MakeCSS(std::wstring const& rkPath);
};

namespace CheckerUtil
{
	typedef ThreadUtil::CResultObserverMgr< ELOADTYPE > CObserverMgr;
	typedef CObserverMgr::CObserver CObserver;
	extern CObserverMgr kObserverMgr;
}

void CALLBACK OnRegist(const CEL::SRegistResult &rkArg);

typedef	struct	tagCheckMessage
{
	tagCheckMessage(const ELOADTYPE Type = LT_INIT, std::wstring const& Path = L"", int const& riExternVal = 0)
	{
		eType = Type;
		wstrPath = Path;
		iExternVal = riExternVal;
	}

	void Set(ELOADTYPE const Type, std::wstring const& rkPath = L"", int const& riExternVal = 0)
	{
		eType = Type;
		wstrPath = rkPath;
		iExternVal = riExternVal;
	}

	ELOADTYPE		eType;
	std::wstring	wstrPath;
	int	iExternVal;
}SCHECKMESSAGE;

typedef std::set< int >	CONT_NOT_EXIST_ID;
typedef std::map< int, PgQuestInfo* >			QuestContainer;

class CQuestChecker;
class CCheckMgr : public PgTask< SCHECKMESSAGE >
{
public:
	CCheckMgr();
	virtual ~CCheckMgr();

	//	메세지 처리
	virtual	void HandleMessage(MSG *rkMsg);

	void ChangeEnterMark(std::wstring& kText);
	void Clear();
	//void OutPut();
	void NotExistID();

	bool bExistMonster(const int nID) const;
	bool bExistGround(const int nID) const;
	bool bExistItem(const int nID) const;
	bool LoadDump(bool const bFromDB, std::wstring const& wstrPath);
	void Stop()
	{
		m_kIsStop = false;
	}

	void Start(int const iLoadQuestNationCode, int const iMaxKillCountValue); // 퀘스트 체커 작업의 시발점
	std::wstring GetTT(int const iTextID) const;
	std::wstring GetQuestTitleText(int const iQuestID) const;
	std::wstring GetQuestTypeText(EQuestType const eType) const;
	void MakeHelp();

private:
	QuestContainer	m_kContQuestInfo;

	void LoadQuestList(std::wstring const& wstrPath);
	void LoadMapList(std::wstring const& wstrPath);
	void QuestParse(std::wstring const& FileName);
	void MapParse(std::wstring const& FileName, int const iMapNo);
	void OutList();
	
	mutable Loki::Mutex m_kMutex;
	size_t m_iQueryQuestParseXml;
	size_t m_iCompleteQuestParseXml;
	size_t m_iQueryMapParseXml;
	size_t m_iCompleteMapParseXml;
	bool m_bReqVerify;
	int m_iMaxKillCountValue;

	void LoadTextTable(std::wstring const& wstrPath);
	bool TextTableParse(const TiXmlNode* pkNode);
	void LoadAllNpc();
	void Verify();
};

#define g_kCheckMgr SINGLETON_STATIC(CCheckMgr)