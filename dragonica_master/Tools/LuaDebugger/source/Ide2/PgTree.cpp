#include "stdafx.h"
#include "PgTree.h"

CPgTree::CPgTree(void)
{
	m_bIsSorted = false;
}

CPgTree::~CPgTree(void)
{
}

CProjectFile* CPgTree::GetProjectFile()
{
	return pkProjectFile;
}

//bool CPgTree::operator < (const CPgTree &e)
//{
//	return pkProjectFile->GetName() < e.pkProjectFile->GetName();
//	//return b < e.b;
//}
