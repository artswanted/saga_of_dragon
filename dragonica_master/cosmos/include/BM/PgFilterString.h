#pragma once 

#include "BM/STLSupport.h"
#include "Tinyxml/Tinyxml.h"
#include "Loki/singleton.h"

/*
1. 채팅 필터 OK.
공백을 포함한 단어를 구분한다.예) s h I t 와 shit 은 같은 단어로 구분되지 않고 각각 독립적으로 필터에 등록되어 걸러진다.
단어 단위로 필터링 한다
예) 필터에 등록된 단어 ass 는 assume 이나 class 와 같은 단어에 영향을 주지 않는다.
dumbass 나 assfuck 등의 단어는 따로 금칙어 등록하여 필터링.
 
2. 이름 필터 (캐릭명/길드명/파티명/펫이름 등)

알파벳만 사용 가능하다.> 숫자와 특수문자 사용 불가. ( !@#$%^&*  사용불가. Alt Code 사용 불가)
공백 사용 불가
첫 글자만 대문자, 나머지는 소문자.> bARUNSON 혹은 barunson 으로 입력하면 Barunson 으로 강제변경
채팅 필터보다 강력한 필터링. 특정 단어를 등록하면 해당 단어를 포함하는 모든 단어를 필터링예) shit 을 필터에 등록 -> shithole, shitty, bullshit, dumbshit 등 모두 막힘

굿워드 는 한글자씩.

등록된 특수문자만 쓸 수 있게 하는기능
// */

typedef enum eFilterSourceType
{
	FST_NONE			= 0x00,
	FST_BADWORD			= 0x01,
	FST_NICKNAME		= 0x02,

	FST_ALL				= FST_BADWORD | FST_NICKNAME,
}EFilterSourceType;

namespace BM
{
	class PgFilterString
	{
		typedef enum eFilterTextSetType
		{
			ETST_NONE = 0,
			ETST_BAD_WORD = 1,
			ETST_GOOD_WORD = 2,
			ETST_BLOCK_NICK_NAME = 3,
			ETST_IGNORE_WORD = 4,
		} EFilterTextSetType;

	public:
		typedef enum eFilterProcessType
		{
			E_UNKNOWN_FILTER = 0,
			E_SIMPLE_FILTER  = 1,
			E_DETAIL_FILTER  = 2,
			E_MAX_FILTER_NUM
		}EFilterProcessType;

		typedef std::vector< wchar_t > CONT_GOOD_WORD;

		typedef struct tagDelPoss
		{
			void SetStart(std::wstring::size_type const in_start_offset)
			{
				start_offset = in_start_offset;
				
				SetEnd(in_start_offset);
				AdjustCount();
			}

			void SetEnd(std::wstring::size_type const in_end_offset)
			{
				end_offset = in_end_offset;
				AdjustCount();
			}
			
			HRESULT SubStrForIgnoreCheck(std::wstring const &kSrcStr, std::wstring &kOutStr)const
			{
				if(	std::wstring::npos != end_offset 
				&&	std::wstring::npos != start_offset
				&&	2 < count )//카운트가 2보다는 커야함. 시작점. 끝점을 빼야 하므로
				{
					kOutStr = kSrcStr.substr(start_offset+1, count-2);
					return S_OK;
				}
				return E_FAIL;
			}
			std::wstring::size_type StartOffset()const{return start_offset;}
			std::wstring::size_type EndOffset()const{return end_offset;}
		protected:
			void AdjustCount()
			{
				if(	std::wstring::npos != end_offset 
				&&	std::wstring::npos != start_offset)
				{
					count = end_offset - start_offset +1;
				}
				else
				{
					count = 0;
				}
			}

			std::wstring::size_type start_offset;
			std::wstring::size_type end_offset;
			std::wstring::size_type count;
		}SDelPos;

		typedef std::list< SDelPos > CONT_FILTER_POS;

		typedef std::map< wchar_t, size_t > CONT_STRING_ANALYSIS;
		typedef struct tagStringInfo
		{
			friend class PgFilterString;

			tagStringInfo()
			{
			}

			tagStringInfo(std::wstring const& rkOrgStr);

			bool Empty()const
			{
				return kOrgStr.empty();
			}

			bool IsExist(CONT_STRING_ANALYSIS::key_type const& cChar)const
			{
				return m_mapAnalysis.end() != m_mapAnalysis.find(cChar);
			}

			std::wstring const& Str()const {return kOrgStr;};
			std::wstring const& UprStr()const {return kUprStr;};

		protected:
			void Build()
			{
				std::wstring::const_iterator char_iter = kUprStr.begin();
				while(kUprStr.end() != char_iter)
				{
					auto ret = m_mapAnalysis.insert( std::make_pair(*char_iter, 0) );
					++ret.first->second;
					++char_iter;
				}
			}

			std::wstring kUprStr;//영문의 경우 대문자화 되면 안되므로 원본 유지를 해야함.
			std::wstring kOrgStr;
		public:
			CONT_STRING_ANALYSIS m_mapAnalysis;
		}SStringInfo;

		typedef std::list< SStringInfo > CONT_STRING;
		typedef std::map< wchar_t, CONT_STRING > TOTAL_CONT; // 개 -> 개 시리즈 다 담고
		typedef std::set< wchar_t > CONT_IGNORE; // 개 -> 개 시리즈 다 담고

	public:
		PgFilterString()
			: m_eBadWordFilterProcessType(E_DETAIL_FILTER)
			, m_eBlockNameFilterProcessType(E_DETAIL_FILTER)
		{
		}

		virtual ~PgFilterString()
		{
		}
	public:
		bool ParseXml(std::wstring const& rkFileName);
		bool ParseXml(TiXmlDocument &doc);
		bool ParseXml(TiXmlNode const *pkNode, bool bUTF8);

		std::wstring const GetGoodString(size_t const size)const;

		bool RegGoodString(std::wstring const& str);
		bool RegBadString(std::wstring const& str);
		bool RegBlockNickString(std::wstring const& str);
		bool RegIgnoreString(std::wstring const& str);
		bool Filter(std::wstring &str, bool const bIsConvert, EFilterSourceType const eFilterType = FST_BADWORD)const;
		
		void SetBadWordFilterProcessType(EFilterProcessType const eProcessType);
		EFilterProcessType GetBadWordFilterProcessType() const { return m_eBadWordFilterProcessType; }

		void SetBlockNickFilterProcessType(EFilterProcessType const eProcessType);
		EFilterProcessType GetBlockNickFilterProcessType() const { return m_eBlockNameFilterProcessType; }
	protected:
		bool RegBadText(std::wstring const& str, TOTAL_CONT& rkTargetCont);
		bool FilterDetail(SStringInfo& rkInput, bool const bIsConvert, TOTAL_CONT const& rkFromCont, EFilterProcessType const eProcessType=E_DETAIL_FILTER)const;
		size_t DoFilter(SStringInfo& rkOrgInfo, std::wstring const& rkBadWord)const;
		size_t DoFilter_Simple(SStringInfo& rkOrgInfo, std::wstring const& rkBadWord)const;
		EFilterProcessType CheckFilterProcessType(int const iXMLFilterType);

	protected:
		TOTAL_CONT m_kBadWord;
		TOTAL_CONT m_kBlockNickName;
		CONT_IGNORE m_kIgnoreWord;

		CONT_GOOD_WORD m_kContGoodWord;

		EFilterProcessType	m_eBadWordFilterProcessType;
		EFilterProcessType	m_eBlockNameFilterProcessType;
	};

	class PgUnicodeFilter
	{
		typedef struct tagWorldHash
		{
			explicit tagWorldHash(int const FuncCode, wchar_t const StartChar, wchar_t const EndChar)
				: iFuncCode(FuncCode), cStartChar(StartChar), cEndChar(EndChar)
			{
			}
			explicit tagWorldHash(tagWorldHash const& rhs)
				: iFuncCode(rhs.iFuncCode), cStartChar(rhs.cStartChar), cEndChar(rhs.cEndChar)
			{
			}
			~tagWorldHash()
			{
			}

			bool operator ==(tagWorldHash const& rhs)
			{
				return	iFuncCode == rhs.iFuncCode
					&&	cStartChar == rhs.cStartChar
					&&	cEndChar == rhs.cEndChar;
			}

			int const iFuncCode;
			wchar_t const cStartChar;
			wchar_t const cEndChar;
		} SWorldHash;

	public:
		typedef std::set< wchar_t > CONT_ERROR_RESULT;
	public:
		PgUnicodeFilter();
		virtual ~PgUnicodeFilter();
	
	public:
		bool IsCorrect(int const iFuncCode, std::wstring const& kOrgStr)const;
		bool IsCorrect(int const iFuncCode, std::wstring const& kOrgStr, CONT_ERROR_RESULT &kOutResult)const;
		bool AddRange(int const iFuncCode, bool const bIsInvalid, wchar_t const kStart, wchar_t const kEnd);
	protected:
		bool IsCorrect_Sub(int const iFuncCode, wchar_t const kWord)const;
		bool IsInvalid_Sub(int const iFuncCode, wchar_t const kWord)const;
	protected:
		typedef std::list< SWorldHash > CONT_WORD_HASH;//시작점과 끝점.

		CONT_WORD_HASH m_kContCorrectRange;
		CONT_WORD_HASH m_kContInvalidRange;
	};
}

#define g_kFilterString SINGLETON_STATIC(BM::PgFilterString)
#define g_kUnicodeFilter SINGLETON_STATIC(BM::PgUnicodeFilter)
