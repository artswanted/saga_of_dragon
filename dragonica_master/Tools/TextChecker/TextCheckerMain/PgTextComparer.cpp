#include "stdafx.h"

#define _NEW_CHECKTYPE_

PgTextComparer::PgTextComparer()
{	
	m_kHTMLColor_Vector.push_back( HTML_COLOR_RED );
	m_kHTMLColor_Vector.push_back( HTML_COLOR_SEG );
	m_kHTMLColor_Vector.push_back( HTML_COLOR_YEL );
	m_kHTMLColor_Vector.push_back( HTML_COLOR_GRE );
	m_kHTMLColor_Vector.push_back( HTML_COLOR_BLU );
	m_kHTMLColor_Vector.push_back( HTML_COLOR_IND );
	m_kHTMLColor_Vector.push_back( HTML_COLOR_PIN );
}

PgTextComparer::~PgTextComparer()
{
	m_kLhsFileName.clear();
	m_kRhsFileName.clear();	
	m_kCheckResultVector.clear();
}

void PgTextComparer::Check(SXmlInfo const& kInLhSXmlInfo, SXmlInfo const& kInRhSXmlInfo)
{
	// 두파일의 이름을 저장하고, 결과값을 저장할 컨테이너를 초기화 한다.	
	m_kLhsFileName = UNI(kInLhSXmlInfo.kFileName);
	m_kRhsFileName = UNI(kInRhSXmlInfo.kFileName);
	m_kCheckResultVector.clear();

	std::wstring kLhsCheckResult;
	std::wstring kRhsCheckResult;
	
	for( CONT_DRCA_TEXTTBL_INFO::const_iterator Lhs_itor = kInLhSXmlInfo.kTextMap.begin(); Lhs_itor != kInLhSXmlInfo.kTextMap.end(); ++Lhs_itor)
	{// Lhs의 TextTable과 Rhs의 TextTable 중 

		CONT_DRCA_TEXTTBL_INFO::const_iterator Rhs_itor = kInRhSXmlInfo.kTextMap.find((*Lhs_itor).first);

		if( Rhs_itor != kInRhSXmlInfo.kTextMap.end() )
		{// 서로 ID가 같고
						
			SDetailTextInfo const& kLhsData = (*Lhs_itor).second;
			SDetailTextInfo const& kRhsData = (*Rhs_itor).second;

			if( kLhsData.kText != kRhsData.kText ) 
			{// Text는 다르면

				if(	0 < kLhsData.kCommandInfoVector.size()  
					||	0 < kRhsData.kCommandInfoVector.size()
					)
				{// lhs, rhs쪽 Text중 하나이상이 커맨드를 가지고 있다면 차이점을 찾는다.
					FindCheckSpot(kLhsData, kLhsCheckResult, kRhsData, kRhsCheckResult);

					if( !kLhsCheckResult.empty() || !kRhsCheckResult.empty() )
					{// 커맨드의 위치가 양쪽 모두 같다면, check 결과에 저장할 필요 없다.

						// FindCheckSpot의 결과 값과 보여주기 위해 필요한 정보를, 결과 컨테이너에 저장한다.
						SShowCheckInfo kShowCheckInfo(kLhsData.kIdStr, kLhsData.kText, kLhsCheckResult, kRhsData.kText,kRhsCheckResult);					
						m_kCheckResultVector.push_back(kShowCheckInfo);
					}
				}
			}
		}
	}
}

bool PgTextComparer::ShowResultToFile(FILE* kOutFS) const
{
	// if( kOutFS.is_open() )
	{
		WriteToFile( kOutFS, _T("/////////////////") + NEWLINE );
		WriteToFile( kOutFS, _T("// Check %Unit //") + NEWLINE );
		WriteToFile( kOutFS, _T("/////////////////") + NEWLINE );
		WriteToFile( kOutFS, _T(" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -") + NEWLINE );		
		WriteToFile( kOutFS, _T("////////////////////////////////") + NEWLINE);
		WriteToFile( kOutFS, _T("// please look at \"(num)☞\" //") + NEWLINE);
		WriteToFile( kOutFS, _T("////////////////////////////////") + NEWLINE + NEWLINE );

		std::wstring const kFileNames = m_kLhsFileName + NEWLINE  + m_kRhsFileName + NEWLINE + NEWLINE ;
		WriteToFile( kOutFS, kFileNames);

		for(CONT_TEXTCHECK_RESULT_INFO::const_iterator itor = m_kCheckResultVector.begin(); itor != m_kCheckResultVector.end(); ++itor)
		{
			std::wstring const& kIdStr = (*itor).kIdStr;

			std::wstring const& kLhsText = (*itor).kLhsText;
			std::wstring const& kRhsText = (*itor).kRhsText;

			std::wstring const& kOutLhsResultText = (*itor).kLhsCheckedText;
			std::wstring const& kOutRhsResultText = (*itor).kRhsCheckedText;
#ifdef _NEW_CHECKTYPE_
			// Check 표시된 문장 출력
			if(kOutLhsResultText.empty())
			{
				WriteToFile( kOutFS, kIdStr + COLON_STR + kLhsText + NEWLINE );
			}
			else
			{
				WriteToFile( kOutFS, kIdStr + COLON_STR + kOutLhsResultText + NEWLINE );
			}
			
			if(kOutRhsResultText.empty())
			{
				WriteToFile( kOutFS, kIdStr + COLON_STR + kRhsText + NEWLINE + NEWLINE );
			}
			else
			{
				WriteToFile( kOutFS, kIdStr + COLON_STR + kOutRhsResultText + NEWLINE + NEWLINE );
			}
#else
			// ID와 원본 문장 출력
			WriteToFile( kOutFS, kIdStr + COLON_STR + kLhsText + NEWLINE  );
			WriteToFile( kOutFS, kOutLhsResultText + NEWLINE  );

			WriteToFile( kOutFS, kIdStr + COLON_STR + kRhsText + NEWLINE  );
			WriteToFile( kOutFS, kOutRhsResultText + NEWLINE  );		
#endif

			WriteToFile( kOutFS, _T(" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -") + NEWLINE + NEWLINE );
		}
		return true;
	}

	return false;
}

void PgTextComparer::FindCheckSpot(SDetailTextInfo const& kInLhsDetailTextInfo, std::wstring& kOutLhsResult,
							    SDetailTextInfo const& kInRhsDetailTextInfo, std::wstring& kOutRhsResult)
{
	std::vector<SCheckInfo>	kCheckInfoVecor;
	// 커맨드가 서로 다른곳을 보여줌
	// 커맨드의 갯수가 서로 다른 경우, 한쪽의 문장이 끝나면, 남은 커맨드의 위치를 모두 표기해줌	
	
	SDetailTextInfo const* pkLarge=NULL; 
	SDetailTextInfo const* pkSmall=NULL;
	
	if( kInLhsDetailTextInfo.kCommandInfoVector.size() > kInRhsDetailTextInfo.kCommandInfoVector.size() )
	{// 커맨드의 갯수가 더 큰쪽을 찾음
		pkLarge = &kInLhsDetailTextInfo;
		pkSmall = &kInRhsDetailTextInfo;
	}
	else
	{
		pkLarge = &kInRhsDetailTextInfo;
		pkSmall = &kInLhsDetailTextInfo;		
	}
	
	unsigned int uiSmallindex=0;
	CONT_UINT kLargeDiffSpotVector;
	CONT_UINT kSmallDiffSpotVector;
	
	// Large와 Small의 같은 번째 커맨드 끼리 비교함
	// ex) A : %d %f %s
	//     B : %s %d %f  (A의 %f와 B의 %d는 같은 번째 커맨드 임)
	
	unsigned int uiCnt=0;
	for(CONT_COMMAND_INFO::const_iterator Large_itor = pkLarge->kCommandInfoVector.begin(); Large_itor != pkLarge->kCommandInfoVector.end(); ++Large_itor)
	{
		if(uiCnt >= pkSmall->kCommandInfoVector.size())
		{// 더이상 Small의 커맨드가 없다면 남은, 커맨드 index는 모두 기억해야할 대상이다.
			kLargeDiffSpotVector.push_back(uiCnt);
		}
		else
		{
			std::wstring const& kLargeCommandUnit = (*Large_itor).kUnitStr;
			std::wstring const& kSmallCommandUnit = pkSmall->kCommandInfoVector[uiSmallindex].kUnitStr;
				
			if(kLargeCommandUnit != kSmallCommandUnit)
			{//  커맨드가 다르면
				// Large, Small쪽 모두 해당 index를 기억한다.
				kLargeDiffSpotVector.push_back(uiCnt);
				kSmallDiffSpotVector.push_back(uiSmallindex);
			}
			++uiSmallindex;
		}
		++uiCnt;
	}
	
	if(kInLhsDetailTextInfo.kCommandInfoVector.size() > kInRhsDetailTextInfo.kCommandInfoVector.size())
	{// Lhs정보를 LhsResult쪽에 저장, Rhs정보를 RhsResult쪽에 저장. (위에서 교차가 됐을수 있으므로)		
		SCheckInfo kCheckInfo(*pkLarge,kLargeDiffSpotVector,*pkSmall, kSmallDiffSpotVector);
		kCheckInfoVecor.push_back( kCheckInfo);
//		StoreCheckResult(pkLarge, kLargeDiffSpotVector,kOutLhsResult);
//		StoreCheckResult(pkSmall, kSmallDiffSpotVector,kOutRhsResult);		
		StoreCheckResult_HTML_Type(pkLarge, kLargeDiffSpotVector,kOutLhsResult);
		StoreCheckResult_HTML_Type(pkSmall, kSmallDiffSpotVector,kOutRhsResult);
	}
	else
	{
		SCheckInfo kCheckInfo(*pkSmall, kSmallDiffSpotVector,*pkLarge,kLargeDiffSpotVector);
		kCheckInfoVecor.push_back(kCheckInfo);

//		StoreCheckResult(pkSmall, kSmallDiffSpotVector,kOutLhsResult);
//		StoreCheckResult(pkLarge, kLargeDiffSpotVector,kOutRhsResult);
		StoreCheckResult_HTML_Type(pkSmall, kSmallDiffSpotVector,kOutLhsResult);
		StoreCheckResult_HTML_Type(pkLarge, kLargeDiffSpotVector,kOutRhsResult);
	}	
}

void PgTextComparer::StoreCheckResult(SDetailTextInfo const* const pkInTextInfo, CONT_UINT const& kInDiffSpotVector, std::wstring& kOutResult)
{
	kOutResult.clear();
	
	std::wstring const &kText	= pkInTextInfo->kText;
	std::wstring const &kIdStr	= pkInTextInfo->kIdStr;
	CONT_COMMAND_INFO const& kCommandInfoVector = pkInTextInfo->kCommandInfoVector;	
	
	// old type - 2칸인 문자가 판별이 안되어 포기
	// 서로 다른 커맨드갯수를 표현 
	// ex)	%d %f %s {C=0xFFFFFFFF/}
	//		%s %f %d %%
	//		1     2  3	<-uiDiffSpotCnt

	// new type - 현재 표현 방식
	// ex)	(1)☞%d %f  (2)☞%s  (3)☞{C=0xFFFFFFFF/}
	//		(1)☞%s %f  (2)☞%d  (3)☞%%

	if(!kInDiffSpotVector.empty())
	{// 같은 위치에 다른 커맨드가 있었다면
		
		CONT_UINT::const_iterator DiffSpot_itor = kInDiffSpotVector.begin();

#ifndef _NEW_CHECKTYPE_
		for(unsigned int i=0; i < kIdStr.size()+COLON_STR.size(); ++i ) 
		{// 틀린곳 표시는 원본 문장 다음에 출력되므로, id와 COLON을 출력한만큼 ' '를 출력하여 자리를 맞추고
			kOutResult.push_back( _T(' ') );
		}
#endif	
		unsigned int uiCheckIndex=0;
		unsigned int uiDiffSpotCnt=0;
		unsigned int uiDiffSpotCntLength=0;		// uiDiffSpotCnt의 자릿수(칸을 맞추기 위함)

#ifdef _NEW_CHECKTYPE_
		size_t accIndex =0;
		std::wstring kCopyText = kText;
#endif
		for(unsigned int i=0; i < kText.size(); ++i)
		{
			// 몇번째 커맨드가 서로 다른지 이미 찾아두었기 때문에, 서로 다른 커맨드의 index를 얻어와
			uiCheckIndex = (*DiffSpot_itor);
			
			if(i ==  kCommandInfoVector[uiCheckIndex].uiIndex)
			{// 원본 문장의 index와, 서로 다른 커맨드의 원본문장에서의 index가 같다면 표시한다.
				
				// 카운트를 증가 하고
				++uiDiffSpotCnt;

				// 카운트의 숫자 길이를 구하여
				uiDiffSpotCntLength = NumLenthCnt(uiDiffSpotCnt);
				
				// 10진수 전환 문자로 전환한다.
				char acBuf[DIFFSPOT_MAX_LENGTH]={0,};				
				::_itoa_s(uiDiffSpotCnt,acBuf, DEC);		
#ifndef _NEW_CHECKTYPE_
				kOutResult = kOutResult + std::wstring(UNI(acBuf));
#endif	
				// 다음 서로 다른 커맨드의 index를 찾기위해, 다른 커맨드 컨테이너의 다음 인덱스를 설정한다.
				++DiffSpot_itor;				
				
#ifdef _NEW_CHECKTYPE_
				std::wstring sss = _T(" (") + std::wstring(UNI(acBuf)) + _T(")☞");				
				kCopyText.insert(i+accIndex, sss.c_str());
				accIndex += sss.size();
#endif
				if(DiffSpot_itor == kInDiffSpotVector.end())
				{// 커맨드가 다른 위치를 모두 출력했으면 끝낸다.
					break;
				}
			}
			else
			{
				if( 1 < uiDiffSpotCntLength ) 
				{// 카운트가 숫자 2자리 이상이면 ' '을 출력하지 않고, 카운트 자리값을 맞춘다.
					--uiDiffSpotCntLength;
				}
				else
				{	
#ifndef _NEW_CHECKTYPE_					
					static int const MAX_CHAR_VALUE = 0xff;	// 이런식으로 해선 안된다.
					if( MAX_CHAR_VALUE < kText[i] )
					{// 파일에 출력되었을때 2칸 짜리 라면 공백을 한번 더 출력 해준다
						kOutResult.push_back(_T('_'));	kOutResult.push_back(_T('_'));
					}
					else
					{
						kOutResult.push_back(_T(' '));
					}
#endif
				}
			}	
		}
#ifdef _NEW_CHECKTYPE_
		kOutResult += kCopyText;
#endif
	}
}

unsigned int const PgTextComparer::NumLenthCnt(unsigned int const& uiInNum) const
{
	unsigned int uiTemp = uiInNum;
	unsigned int uiCnt=0;
	do
	{
		++uiCnt;
	}while(uiTemp/=10);

	return uiCnt;
}


bool PgTextComparer::ShowResultToHTML(FILE* kOutFS) const
{
	// if( kOutFS.is_open() )
	{
		WriteToFile( kOutFS, HTML_DecoText( _T("/////////////////"), HTML_COLOR_RED, 5 ) + NEWLINE );
		WriteToFile( kOutFS, HTML_DecoText( _T("//   Compare   //"), HTML_COLOR_RED, 5 ) + NEWLINE );
		WriteToFile( kOutFS, HTML_DecoText( _T("/////////////////"), HTML_COLOR_RED, 5 ) + NEWLINE );

		std::wstring const kFileNames = m_kLhsFileName + NEWLINE + m_kRhsFileName + NEWLINE + NEWLINE ;
		
		WriteToFile( kOutFS, HTML_DecoText(kFileNames, HTML_COLOR_BLU, 4) );

		for(CONT_TEXTCHECK_RESULT_INFO::const_iterator itor = m_kCheckResultVector.begin(); itor != m_kCheckResultVector.end(); ++itor)
		{// 기본적으로 CheckResult에는 어느 한쪽이 다른 커맨드 정보를 가지고 있을 경우다.
			std::wstring const& kIdStr = (*itor).kIdStr;

			std::wstring const& kLhsText = (*itor).kLhsText;
			std::wstring const& kRhsText = (*itor).kRhsText;

			std::wstring const& kOutLhsResultText = (*itor).kLhsCheckedText;
			std::wstring const& kOutRhsResultText = (*itor).kRhsCheckedText;
			
			if(kOutLhsResultText.empty())
			{
				WriteToFile( kOutFS, kIdStr + COLON_STR + kLhsText + NEWLINE );
			}
			else
			{
				WriteToFile( kOutFS, kIdStr + COLON_STR + kOutLhsResultText + NEWLINE );
			}
			
			if(kOutRhsResultText.empty())
			{
				WriteToFile( kOutFS, kIdStr + COLON_STR + kRhsText + NEWLINE + NEWLINE );
			}
			else
			{
				WriteToFile( kOutFS, kIdStr + COLON_STR + kOutRhsResultText + NEWLINE + NEWLINE );
			}
		}
		return true;
	}
}

void PgTextComparer::StoreCheckResult_HTML_Type(SDetailTextInfo const* const pkInTextInfo, CONT_UINT const& kInDiffSpotVector, std::wstring& kOutResult)
{
	kOutResult.clear();
	
	std::wstring const &kText	= pkInTextInfo->kText;
	std::wstring const &kIdStr	= pkInTextInfo->kIdStr;
	CONT_COMMAND_INFO const& kCommandInfoVector = pkInTextInfo->kCommandInfoVector;	
	
	CONT_WSTRING::const_iterator HTML_color_itor = m_kHTMLColor_Vector.begin();
	
	if(!kInDiffSpotVector.empty())
	{// 같은 위치에 다른 커맨드가 있었다면
		
		CONT_UINT::const_iterator DiffSpot_itor = kInDiffSpotVector.begin();

		unsigned int uiCheckIndex = 0;
		unsigned int uiDiffSpotCnt = 0;

		size_t accIndex = 0;
		std::wstring kCopyText = kText;

		for(unsigned int i=0; i < kText.size(); ++i)
		{
			// 몇번째 커맨드가 서로 다른지 이미 찾아두었기 때문에, 서로 다른 커맨드의 index를 얻어와
			uiCheckIndex = (*DiffSpot_itor);
			
			size_t Index = kCommandInfoVector[uiCheckIndex].uiIndex;

			if(i ==  kCommandInfoVector[uiCheckIndex].uiIndex)
			{// 원본 문장의 index와, 서로 다른 커맨드의 원본문장에서의 index가 같다면 표시한다.
				
				// 카운트를 증가 하고
				++uiDiffSpotCnt;
				// 다음 서로 다른 커맨드의 index를 찾기위해, 다른 커맨드 컨테이너의 다음 인덱스를 설정한다.
				++DiffSpot_itor;				
				
				//html 명령어 올리기
				std::wstring kTempHTMLCmd = HTML_OPEN_MARK + HTML_FONT + HTML_Color( (*HTML_color_itor) ) + HTML_CLOSE_MARK;
				//std::wstring kTempHTMLCmd = L"[";

				kCopyText.insert(i+accIndex, kTempHTMLCmd.c_str());
				accIndex += kTempHTMLCmd.size();
								
				//html 명령어 막기
				std::wstring::size_type UnitSize = kCommandInfoVector[uiCheckIndex].kUnitStr.size();
				kTempHTMLCmd = HTML_Sup(uiDiffSpotCnt) + HTML_END_OPEN_MARK + HTML_FONT + HTML_CLOSE_MARK;
				//kTempHTMLCmd =  L"]";

				kCopyText.insert(i+accIndex+UnitSize, kTempHTMLCmd.c_str());				
				accIndex += kTempHTMLCmd.size();
				
				//색조정
				++HTML_color_itor;
				if ( HTML_color_itor == m_kHTMLColor_Vector.end() )
				{
					 HTML_color_itor = m_kHTMLColor_Vector.begin();
				}

				if(DiffSpot_itor == kInDiffSpotVector.end())
				{// 커맨드가 다른 위치를 모두 출력했으면 끝낸다.
					break;
				}
			}
		}
		kOutResult = kCopyText;
	}
}