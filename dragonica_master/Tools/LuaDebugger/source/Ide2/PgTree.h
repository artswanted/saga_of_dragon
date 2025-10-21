#pragma once

#include "ProjectFile.h"

typedef CTypedPtrArray<CPtrArray, CProjectFile*> CProjectFileArray;

class CPgTree
{
public:
	typedef enum eObjectType
	{
		ObjectType_None,
		ObjectType_File,
		ObjectType_Folder,
		ObjectType_Project,
	}EObjectType;
	EObjectType m_eObjectType;

	std::list<CPgTree*> m_listChild;

public:
	bool m_bIsSorted;
	CProjectFile* pkProjectFile;
	HTREEITEM kTreeItem;

public:
	CPgTree(void);
	virtual ~CPgTree(void);

public:
	CProjectFile* GetProjectFile();

// override
public:
	//bool operator < (const CPgTree &e);
	class SortFunction_less : public std::less<CPgTree>
	{
	public:
		bool operator()(const CPgTree* lhs, const CPgTree* rhs) const {
			return lhs->pkProjectFile->GetName() > rhs->pkProjectFile->GetName();
		}
	};
	class SortFunction_greater : public std::greater<CPgTree>
	{
	public:
		bool operator()(const CPgTree* lhs, const CPgTree* rhs) const {
			return lhs->pkProjectFile->GetName() < rhs->pkProjectFile->GetName();
		}
	};
};
