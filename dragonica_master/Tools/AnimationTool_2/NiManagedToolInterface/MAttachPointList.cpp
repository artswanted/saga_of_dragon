#include "stdafx.h"
#include "MAttachPointList.H"
#include <stdio.h>
#include "ManagedMacros.H"

using namespace	NiManagedToolInterface;

bool	MAttachPointList::AddAttachPoint(String* kAttachPointName)
{
	std::string kName(MStringToCharPointer(kAttachPointName));
	return	AddAttachPoint(kName);
}
bool	MAttachPointList::AddAttachPoint(std::string const &kAttachPointName)
{
	if( std::find(m_pkAttachPointCont->begin(),m_pkAttachPointCont->end(),kAttachPointName) != m_pkAttachPointCont->end())
	{
		return false;
	}
	m_pkAttachPointCont->push_back(kAttachPointName);

	return	true;
}
bool	MAttachPointList::RemoveAttachPoint(String* kAttachPointName)
{
	std::string kName(MStringToCharPointer(kAttachPointName));
	unsigned int uiNum = m_pkAttachPointCont->size();

	AttachPointVector	*pkNewVector = NiExternalNew AttachPointVector();

	for(unsigned int ui=0;ui<m_pkAttachPointCont->size();++ui)
	{
		if(m_pkAttachPointCont->at(ui) == kName)
		{
			for(unsigned int uj=ui;uj<m_pkAttachPointCont->size()-1;++uj)
			{
				pkNewVector->push_back(m_pkAttachPointCont->at(uj+1));
			}

			m_pkAttachPointCont = pkNewVector;
			return	true;
		}
		pkNewVector->push_back(m_pkAttachPointCont->at(ui));
	}

	return	false;
}

bool	MAttachPointList::Load(String* kPath)
{
	FILE	*fp = fopen(GetFilePath(kPath).c_str(),"rb");
	if(!fp)
	{
		return false;
	}

	Load(fp);

	fclose(fp);

	return	true;
}
std::string MAttachPointList::GetFilePath(String *kPath)
{
	std::string kDataFilePath(MStringToCharPointer(kPath));

	kDataFilePath = kDataFilePath.substr(0,kDataFilePath.size()-1);
	kDataFilePath += "AttachPoint.txt";

	return	kDataFilePath;
}
bool	MAttachPointList::Load(FILE *fp)
{
	std::string kPointName;

	kPointName = ReadNextToken(fp);
	while(kPointName.empty() == false)
	{
		AddAttachPoint(kPointName);
		kPointName = ReadNextToken(fp);
	}

	return	true;
}
std::string MAttachPointList::ReadNextToken(FILE *fp)
{
	std::string kToken;

	while(true)
	{
		int	iChar = fgetc(fp);
		if(iChar == EOF)
		{
			break;
		}
		if(iChar == '\r')
		{
			continue;
		}
		if(iChar == '\n')
		{
			break;
		}

		kToken += static_cast<char>(iChar);
	}
	return	kToken;
}
bool	MAttachPointList::Save(String* kPath)
{
	FILE	*fp = fopen(GetFilePath(kPath).c_str(),"wb");
	if(!fp)
	{
		return false;
	}

	Save(fp);

	fclose(fp);

	return	true;
}
bool	MAttachPointList::Save(FILE *fp)
{
	
	unsigned int uiNum = m_pkAttachPointCont->size();
	for(unsigned int ui=0;ui<uiNum;++ui)
	{
		fprintf(fp,"%s\r\n",m_pkAttachPointCont->at(ui).c_str());
	}

	return	true;
}