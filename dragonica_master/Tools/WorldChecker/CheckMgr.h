#pragma once


class PgWater;
class PgHome;



void CALLBACK OnRegist(const CEL::SRegistResult &rkArg);

typedef enum eLoadType
{
	LT_INIT,
	LT_XMLPATH,
	LT_GSAPATH,
	LT_NIFPATH,
	LT_COMPARE,
	LT_XMLPATHERR,
	LT_GSAPATHERR,
	LT_NIFPATHERR,	
	LT_FOLDER_LIST,
	LT_FOLDER_XML_LIST,
	LT_FOLDER_GSA_LIST,
	LT_FOLDER_NIF_LIST,
	LT_FOLDER_XMLERR_LIST,
	LT_FOLDER_GSAERR_LIST,
	LT_FOLDER_NIFERR_LIST,
	LT_COMPARELIST,
	LT_QUIT,

	LT_NIFEFFECT,
	LT_NIFEFFECTERR,
	LT_COMPARE_EFFECT,
}ELOADTYPE;

typedef enum eLoadTypeData
{
	DT_XML,
	DT_GSA,
	DT_NIF,
	DT_MAXTYPE,
}ELOADTYPEDATA;


typedef	struct	tagCheckMessage
{
	tagCheckMessage(const ELOADTYPE Type = LT_INIT, const std::wstring& Path = L"")
	{
		eType		= Type;
		wstrPath	= Path;
	}

	void	Set(const ELOADTYPE Type, const std::wstring& Path = L"")
	{
		eType		= Type;
		wstrPath	= Path;
	}

	ELOADTYPE		eType;
	std::wstring	wstrPath;	
}SCHECKMESSAGE;




class CCheckMgr : public PgTask< SCHECKMESSAGE >
{
public:
	CCheckMgr()
	{
	}
	virtual ~CCheckMgr() {}

	//	皋技瘤 贸府
	virtual	void	HandleMessage(MSG *rkMsg);
	
	void	Clear()
	{
	}

	void Stop()
	{
		m_kIsStop = true;
	}

private:
	// Path
	typedef std::map<std::wstring, std::wstring> XmlPathContainer;
	// Gsa
	typedef std::map<std::wstring, std::wstring> GsaPathContainer;
	// Nif
	typedef std::map<std::wstring, std::wstring> ContNifPath;
	typedef std::map<std::wstring, ContNifPath> NifPathContainer;

	// List
	typedef std::map<std::wstring, std::wstring> ListPathContainer;

	void	LoadXmlPath();
	bool	LoadXmlPathFile(TiXmlDocument &rkXmlDoc, const std::wstring &rkFileName);

	void	LoadXmlEffect(const std::wstring& wstrPath);
	

	const std::string& GetGsaPathList(const int nID);

	void			ContInit();
	void			LoadGsaPath();
	void			LoadNifPath();
	bool			LoadGsa(std::wstring& wstrPath, char const *pcGsaPath);
	void			CompareElementsFile();
	bool			IsFileCheck(const ELOADTYPE Type, const char* szFileName);
	void			LogSaveFile(const ELOADTYPE Type, std::wstring szMsg);

	void			LoadFolder_List(const std::wstring& wstrPath);
	bool			GetFolderList(BM::FolderHash& Hash, int eType);
	void			GetListType(int eType, std::wstring& szTypeName);
	void			SaveFolderList(int eType, std::wstring& FileDirName);
	void			GetFileListFolderSTR(int eType, std::wstring& szFileListtFolder);
	void			CompareListElementFiles();

	void			CompareListElementXML();
	void			CompareListElementGSA();
	void			CompareListElementNIF();

	bool			DefaultNodeSetting(NiNode *pkRootNode);

	void			CompareElementsFile_Effect();

protected:
	mutable Loki::Mutex m_kMutex;	

	//XmlTypeContainer m_kXmlTypeContainer;
	XmlPathContainer m_kXmlPathContainer;	
	GsaPathContainer m_kGsaPathList;
	NifPathContainer m_kNifPathList;


	ListPathContainer m_kFolderXmlPathListContainer;
	ListPathContainer m_kFolderGsaPathListContainer;
	ListPathContainer m_kFolderNifPathListContainer;

	ListPathContainer m_kFolderXmlPathContainer;
	ListPathContainer m_kFolderGsaPathContainer;
	ListPathContainer m_kFolderNifPathContainer;

	BM::FolderHash		FileHashList;
};

#define g_kCheckMgr SINGLETON_STATIC(CCheckMgr)



class CLogMgr : public PgTask < std::wstring >
{
public:
	CLogMgr() {}
	virtual ~CLogMgr() {}

	//	皋技瘤 贸府
	virtual	void	HandleMessage(MSG *rkMsg)
	{
		if(rkMsg) { AddLog((*rkMsg)); }
	}

	void Stop()
	{
		m_kIsStop = true;
	}

private:
	Loki::Mutex m_kAddError;
	void	AddLog(const std::wstring& Log)
	{
		BM::CAutoMutex kLock(m_kAddError);
		g_Core.AddListItem(Log);
	}
};

#define g_kLogMgr SINGLETON_STATIC(CLogMgr)



