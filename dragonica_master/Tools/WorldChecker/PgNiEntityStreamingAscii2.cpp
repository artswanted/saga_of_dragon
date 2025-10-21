#include "defines.h"
#include "CheckMgr.h"


//#define PG_USE_PACK_IN_NIENTITYSTREAMING

PgNiEntityStreamingAscii2::PgNiEntityStreamingAscii2(void)
{
}

PgNiEntityStreamingAscii2::~PgNiEntityStreamingAscii2(void)
{
}

void PgNiEntityStreamingAscii2::_SDMInit()
{
    STREAMING_EXTENSION = "PACK_GSA";
    STREAMING_DESCRIPTION = "Gamebryo ASCII Scene files (*.gsa)|*.gsa";
}

void PgNiEntityStreamingAscii2::_SDMShutdown()
{
    STREAMING_EXTENSION = NULL;
    STREAMING_DESCRIPTION = NULL;
}

NiBool PgNiEntityStreamingAscii2::Load(char const* pcFileName)
{
#ifdef PG_USE_PACK_IN_NIENTITYSTREAMING
	if (g_bUsePackData && g_bUseExtraPackData)
	{
		Flush();
		if (!PgStoreWorkingPath(pcFileName))
			return false;
		RemoveAllScenes();

		//-- Initialize
		{
			TiXmlDocument kXmlDoc(pcFileName);
			
			{
				std::wstring wstrPackFileName;
				std::wstring wstrChildName;
				PgNiFile::IsPackedFile(pcFileName, wstrPackFileName, wstrChildName);

				std::vector< char > data(0);
				__int64	iVer = 0;
				if(!BM::PgDataPackManager::LoadFromPack(wstrPackFileName, wstrChildName, data, iVer))
				{
					NILOG(PGLOG_ERROR, "[PgXmlLoader] LoadFromPack failed, %s xml parse failed\n",pcFileName);
					return false;
				}
				
				if(!data.size())
				{
					PG_ASSERT_LOG(!"failed loading from Packed Data.");
					return false;
				}

				data.push_back('\0');

				kXmlDoc.Parse(&data.at(0));

				if(kXmlDoc.Error())
				{
					PG_ASSERT_LOG(!"failed to loading path.xml");
					return false;
				}
			}

		
			m_kDOM.Init(kXmlDoc);
		}
		//-- Read from DOM (and create a Scene)
		if (!ReadFromDOM())
		{   
			Flush();
			RemoveAllScenes();

			ReportError(ERR_FILE_PARSE_FAILED, pcFileName, "", "");
			return false;
		}

		Flush();
		return true;
	}
	else
#endif
	{
		NiEntityStreamingAscii::Load(pcFileName);
		return true;
	}

	return false;
}

NiBool PgNiEntityStreamingAscii2::PgStoreWorkingPath(char const* pcFileName)
{
	NiStrcpy(m_acFullPath, NI_MAX_PATH, pcFileName);

	// Strip off the filename
	char* pcLastSlash = strrchr(m_acFullPath, NI_PATH_DELIMITER_CHAR);
	if (pcLastSlash == NULL)
	{
		pcLastSlash = strrchr(m_acFullPath, NI_PATH_DELIMITER_INCORRECT_CHAR);
	}

	if (pcLastSlash)
		*pcLastSlash = '\0';

	return true;
}
