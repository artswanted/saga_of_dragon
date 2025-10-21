// MIMEContentAgent.cpp: implementation of the CMIMEContentAgent class.
// Author: Wes Clyburn (clyburnw@enmu.edu)
//////////////////////////////////////////////////////////////////////

//#include <afx.h>
#include "stdafx.h"
#include "../Header/MIMEContentAgent.h"

#ifdef _DEBUG
#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIMEContentAgent::CMIMEContentAgent( int nMIMEType )
{
	m_nMIMETypeIHandle = nMIMEType;
}

CMIMEContentAgent::~CMIMEContentAgent()
{

}

bool CMIMEContentAgent::QueryType(int nContentType)
{
	return nContentType == m_nMIMETypeIHandle ? true : false;
}

