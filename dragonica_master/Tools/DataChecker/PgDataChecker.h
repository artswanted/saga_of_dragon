#ifndef DATACHECKER_PGDATACHECKER_H
#define DATACHECKER_PGDATACHECKER_H

struct tagClassInfo;
struct tagPathInfo;

typedef std::map< int, tagClassInfo > CONT_CHECK_CLASSINFO;
typedef std::map< std::string, tagPathInfo > CONT_CHECK_PATHINFO;

class PgDataChecker
{
public:
	PgDataChecker();
	~PgDataChecker();

public:
	bool DataCheckerBegin();
	void DataCheckerEnd();
	bool RegistKeyEvent();

	void ShowMemu();
	
	void SetPath();
	const std::string& GetXMLPath() const { return m_kXMLPath; }
	
	bool CreateTableBin();
	bool LoadTableBin();

	bool ParseXmlForClassNo();
	bool CheckForClassNo();

	bool ParseXmlForPath();
	bool CheckForPath();
	
	bool ParseXmlForStatusEffect( const std::string& rkXml, TiXmlNode* pkRootNode );
	bool ParseXmlForProjectile( const std::string& rkXml, TiXmlNode* pkRootNode );
	bool ParseXmlForAction( const std::string& rkXml, TiXmlNode* pkRootNode );
	bool ParseXmlForActor( const std::string& rkXml, TiXmlNode* pkRootNode );
	bool ParseXmlForDropBox( const std::string& rkXml, TiXmlNode* pkRootNode );

	bool IsExistFile( char const* pcPath );

	void OpenLogFile();

private:
	Loki::Mutex				m_kMutex;

	CONT_CHECK_CLASSINFO	m_kCheckClassInfo;	// ClassNo.xml 정보
	CONT_CHECK_PATHINFO		m_kCheckPathInfo;	// Path.xml 정보

	std::string				m_kXMLPath;			// SFreedom_Dev\XML	

private:
	
	static void CALLBACK OnRegist( const CEL::SRegistResult& rkArg );
	static HRESULT CALLBACK OnDB_EXECUTE(CEL::DB_RESULT &rkResult);
	static HRESULT CALLBACK OnDB_EXECUTE_TRAN(CEL::DB_RESULT_TRAN &rkResult);	
	
	static bool CALLBACK OnF1( WORD const& rkInputKey );
	static bool CALLBACK OnF2( WORD const& rkInputKey );
	static bool CALLBACK OnF3( WORD const& rkInputKey );
	static bool CALLBACK OnTerminate(WORD const& rkInputKey); // F11
};

#define g_kDataChecker SINGLETON_STATIC(PgDataChecker)

#endif