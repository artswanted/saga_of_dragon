#include "stdafx.h"
#include <tchar.h>
#include "../Header/stringUtil.h"


void StringUtil::TrimLeft(std::wstring& s)
{
	if( 0 == s.size() )		return;

	while(1)
	{
		if ( _T('\t') == s[0] || _T('\r') == s[0] || _T('\n') == s[0] || _T(' ') == s[0] )
		{
			s.erase(0, 1);
		}
		else
		{
			break;
		}
	}//while
}

void StringUtil::TrimRight(std::wstring& s)
{
	int i = (int)s.size();

	if( !i )	return;

	i--;

	while(1)
	{
		if ( _T('\t') == s[i] || _T('\r') == s[i] || _T('\n') == s[i] || _T(' ') == s[i] )
		{
			s.erase(i);
			--i;
			
			if( 0 >= i )	return;
		}
		else
		{
			break;
		}
	}
}


std::wstring StringUtil::Left(std::wstring& s, int i)
{
	if( 0 == s.size() || 0 >= i )	return _T("");

	std::wstring sTmp;
	for( int j=0; j<i; j++ )
	{
		sTmp += s[j];
	}
	return sTmp;
}

std::wstring StringUtil::Right(std::wstring& s, int i)
{
	if( 0 == s.size()  || 0 >= i )	return _T("");

	std::wstring sTmp;
	for( int j=0; j<i; j++ )
	{
		sTmp += s[s.size() - i + j];
	}
	return sTmp;
}


std::wstring StringUtil::Mid(std::wstring& s, int first, int n)
{
	if( 0 == (int)s.size() || (int)(s.size()) < first || (int)s.size() < n || (int)s.size() < (first + n) )
	{
		return _T("");
	}

	std::wstring sTmp;

	for( int i=0; i<n; i++ )
	{
		sTmp += s[ first + i ] ;
	}
	return sTmp;
}


