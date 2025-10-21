#include "stdafx.H"
#include "PgWorldTimeCondition.h"

bool	PgWorldTimeCondition::CheckTheElementHasTimeConditionAttribute(TiXmlElement const *pkElement) const
{
	assert(pkElement);
	if(!pkElement)
	{
		return	false;
	}

	bool	bHasFromCondition = false;
	bool	bHasToCondition = false;

	const TiXmlAttribute *pkAttr = pkElement->FirstAttribute();
	while(pkAttr)
	{
		std::string kAttrName(pkAttr->Name());
		std::string kAttrValue(pkAttr->Value());

		if(kAttrName == m_kFromConditionAttributeName)
		{
			bHasFromCondition = true;
		}
		else if(kAttrName == m_kToConditionAttributeName)
		{
			bHasToCondition = true;
		}

		pkAttr = pkAttr->Next();
	}

	return	(bHasFromCondition && bHasToCondition);
}
bool	PgWorldTimeCondition::ReadFromXmlElement(TiXmlElement const *pkElement)
{
	assert(pkElement);
	if(!pkElement)
	{
		return	false;
	}
	int	iFrom_Year=0,iFrom_Month=0,iFrom_Day=0,iFrom_Hour=0,iFrom_Minute=0;
	int	iTo_Year=0,iTo_Month=0,iTo_Day=0,iTo_Hour=0,iTo_Minute=0;

	const TiXmlAttribute *pkAttr = pkElement->FirstAttribute();
	while(pkAttr)
	{
		std::string kAttrName(pkAttr->Name());
		std::string kAttrValue(pkAttr->Value());

		if(kAttrName == m_kFromConditionAttributeName)
		{
			::sscanf(kAttrValue.c_str(),"%d,%d,%d,%d,%d",&iFrom_Year,&iFrom_Month,&iFrom_Day,&iFrom_Hour,&iFrom_Minute);
		}
		else if(kAttrName == m_kToConditionAttributeName)
		{
			::sscanf(kAttrValue.c_str(),"%d,%d,%d,%d,%d",&iTo_Year,&iTo_Month,&iTo_Day,&iTo_Hour,&iTo_Minute);
		}
		else if(kAttrName == "ID")
		{
			m_kID = kAttrValue;
		}

		pkAttr = pkAttr->Next();
	}

	m_kDurationFrom.wYear = static_cast<WORD>(iFrom_Year);
	m_kDurationFrom.wMonth = static_cast<WORD>(iFrom_Month);
	m_kDurationFrom.wDay = static_cast<WORD>(iFrom_Day);
	m_kDurationFrom.wHour = static_cast<WORD>(iFrom_Hour);
	m_kDurationFrom.wMinute = static_cast<WORD>(iFrom_Minute);

	m_kDurationTo.wYear = static_cast<WORD>(iTo_Year);
	m_kDurationTo.wMonth = static_cast<WORD>(iTo_Month);
	m_kDurationTo.wDay = static_cast<WORD>(iTo_Day);
	m_kDurationTo.wHour = static_cast<WORD>(iTo_Hour);
	m_kDurationTo.wMinute = static_cast<WORD>(iTo_Minute);

	return	true;
}
bool	PgWorldTimeCondition::CheckTimeIsInsideDuration(SYSTEMTIME const &kTime)	const
{
	bool	bFromTimeCorrect = false;
	bool	bToTimeCorrect = false;
	if(kTime.wYear>m_kDurationFrom.wYear)
	{
		bFromTimeCorrect = true;
	}
	else if(kTime.wYear == m_kDurationFrom.wYear)
	{
		if(kTime.wMonth > m_kDurationFrom.wMonth)
		{
			bFromTimeCorrect = true;
		}
		else if(kTime.wMonth == m_kDurationFrom.wMonth)
		{
			if(kTime.wDay > m_kDurationFrom.wDay)
			{
				bFromTimeCorrect = true;
			}
			else if(kTime.wDay == m_kDurationFrom.wDay)
			{
				if(kTime.wHour > m_kDurationFrom.wHour)
				{
					bFromTimeCorrect = true;
				}
				else if(kTime.wHour == m_kDurationFrom.wHour)
				{
					if(kTime.wMinute > m_kDurationFrom.wMinute)
					{
						bFromTimeCorrect = true;
					}
				}
			}
		}
	}

	if(bFromTimeCorrect)
	{
		if(kTime.wYear<m_kDurationTo.wYear)
		{
			bToTimeCorrect = true;
		}
		else if(kTime.wYear == m_kDurationTo.wYear)
		{
			if(kTime.wMonth < m_kDurationTo.wMonth)
			{
				bToTimeCorrect = true;
			}
			else if(kTime.wMonth == m_kDurationTo.wMonth)
			{
				if(kTime.wDay < m_kDurationTo.wDay)
				{
					bToTimeCorrect = true;
				}
				else if(kTime.wDay == m_kDurationTo.wDay)
				{
					if(kTime.wHour < m_kDurationTo.wHour)
					{
						bToTimeCorrect = true;
					}
					else if(kTime.wHour == m_kDurationTo.wHour)
					{
						if(kTime.wMinute < m_kDurationTo.wMinute)
						{
							bToTimeCorrect = true;
						}
					}
				}
			}
		}
	}
	return	( bFromTimeCorrect && bToTimeCorrect );
}