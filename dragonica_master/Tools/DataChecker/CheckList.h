#ifndef DATACHECKER_CHECKLIST_H
#define DATACHECKER_CHECKLIST_H

typedef struct tagClassInfo
{
	tagClassInfo()
	{
	}

	tagClassInfo( const std::string& rkActorID, const std::string& rkPilotPath )
		:m_kActorID( rkActorID ), m_kPilotPath( rkPilotPath )
	{
	}
	
	std::string m_kActorID;
	std::string m_kPilotPath;

} SClassInfo;

typedef struct tagPathInfo
{
	tagPathInfo()
	{
	}

	tagPathInfo( const std::string& rkID, const std::string& rkPath )
		:m_kID( rkID ), m_kPath( rkPath )
	{
	}

	std::string m_kID;
	std::string m_kPath;
} SPathInfo;

#endif