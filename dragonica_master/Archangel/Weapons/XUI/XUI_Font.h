#ifndef WEAPON_XUI_XUI_FONT_H
#define WEAPON_XUI_XUI_FONT_H

//#include <NiDynamicTexture.h>
//#include <NiScreenTexture.h>
//#include <list>
#include <tchar.h>
#include "BM/STLSupport.h"
#include "BM/Point.h"
#include "BM/Stream.h"
#include "ft2build.h"
#include FT_FREETYPE_H

typedef struct _ARGB16
{
	unsigned short sARGB;
}ARGB16;
int const MAX_FONT_TEX_COUNT = 5;

namespace XUI 
{

	extern	const	WCHAR*	XUI_SAVE_FONT;
	extern	const	WCHAR*	XUI_RESTORE_FONT;
	extern	const	WCHAR*	XUI_SAVE_COLOR;
	extern	const	WCHAR*	XUI_RESTORE_COLOR;

	typedef struct tagLineInfo
	{
		std::wstring m_kWstr;
		int				m_iLen;
		bool			m_bReturn;	//리턴으로 개행된 라인인가
		tagLineInfo()
		{
			Init();
		}
		void Init()
		{
			m_kWstr.clear();
			m_iLen = 0;
			m_bReturn = false;
		}

		void Set(std::wstring &rkStr, int iValue)
		{
			m_kWstr = rkStr;
			m_iLen = iValue;
		}

	}SLineInfo;
	typedef std::vector<SLineInfo> VEC_LINE;

	class	CXUI_Font;
	typedef struct	tagFontDef
	{
		tagFontDef();
		tagFontDef(CXUI_Font *pFont, DWORD const dwFontColor=0xffffffff, DWORD const dwOption=0);
		void Set(CXUI_Font *pFont, DWORD const dwFontColor=0xffffffff, DWORD const dwOption=0);

		DWORD dwOptionFlag;
		DWORD dwColor;
		CXUI_Font *m_pFont;
	}PgFontDef;

	typedef struct tagParsedText
	{
		tagParsedText()
			:m_iExtraDataPackIndex(-1)
		{
		}

		tagParsedText(PgFontDef const& rkFontDef, int const iInAlignHeight, std::wstring const& strInText)
			:m_iExtraDataPackIndex(-1)
		{
			Set(rkFontDef, iInAlignHeight, strInText);
		}

		void Set(PgFontDef const& rkFontDef, int const iInAlignHeight, std::wstring const& strInText)
		{
			m_FontDef = rkFontDef;
			iAlignHeight = iInAlignHeight;
			strText = strInText;
		}
//	protected:
		PgFontDef m_FontDef;
		int	iAlignHeight;
		std::wstring strText;
		int	m_iExtraDataPackIndex;
	}PgParsedText;

	typedef struct tagParsedChar
	{
		tagParsedChar(const TCHAR wChar, CXUI_Font *pFont, int const iAlignHeight,int iExtraDataPackIndex);
		tagParsedChar(const WCHAR wChar, CXUI_Font *pFont, DWORD const dwFontColor, DWORD const dwFontOption, int const iAlignHeight,int iExtraDataPackIndex);
		bool IsEqual(const tagParsedChar &kChar)const;
		bool operator == (const WCHAR wChar)const;		
		size_t Width();

		PgFontDef m_FontDef;
		int	m_iAlignHeight;
		TCHAR	m_wChar;
		int	m_iWidth;
		int	m_iExtraDataPackIndex;

	}PgParsedChar;

	typedef std::vector<PgParsedChar> CONT_PARSED_CHAR;

	typedef	struct	tagExtraData
	{
		typedef std::vector<char>	CONT_BINARY_ARRAY;

		std::wstring	m_kType;
		std::wstring	m_kValue;
		CONT_BINARY_ARRAY m_kBinaryValue;

		tagExtraData()
		{
		};

		tagExtraData(std::wstring const &kType,std::wstring const &kValue)
			:m_kType(kType),m_kValue(kValue)
		{
		};
		tagExtraData(std::wstring const &kType, CONT_BINARY_ARRAY &kBinaryValue)
			:m_kType(kType),m_kBinaryValue(kBinaryValue)
		{
		};
	}PgExtraData;
	typedef	std::vector<PgExtraData> CONT_EXTRA_DATA;

	typedef	struct	tagExtraDataPack
	{
		CONT_EXTRA_DATA	m_kExtraDataCont;

		void	AddExtraData(std::wstring const &kType, std::wstring const &kValue)
		{
			m_kExtraDataCont.push_back(PgExtraData(kType,kValue));
		}
		void	AddExtraData(std::wstring const &kType, tagExtraData::CONT_BINARY_ARRAY &kBinaryValue)
		{
			m_kExtraDataCont.push_back(PgExtraData(kType,kBinaryValue));
		}

	}PgExtraDataPack;
	typedef	std::vector<PgExtraDataPack> CONT_EXTRA_DATA_PACK;

	typedef	struct	tagExtraDataPackInfo
	{
		CONT_EXTRA_DATA_PACK	m_kExtraDataPackCont;

		size_t	GetExtraDataPackCount()	const	{	return	m_kExtraDataPackCont.size();	}

		PgExtraDataPack	const	*GetExtraDataPack(unsigned int uiIndex)	const
		{
			if(m_kExtraDataPackCont.size() <= uiIndex)
			{
				return	NULL;
			}

			return	&m_kExtraDataPackCont[uiIndex];
		}
		void	PushToPacket(BM::Stream &kPacket)	const
		{
			unsigned	int	iCount = (unsigned int)m_kExtraDataPackCont.size();
			kPacket.Push(iCount);
			for(unsigned int i=0;i<iCount;i++)
			{
				CONT_EXTRA_DATA	const	&kDataCont = m_kExtraDataPackCont[i].m_kExtraDataCont;

				unsigned int	iCount2 = (unsigned int)kDataCont.size();
				kPacket.Push(iCount2);

				for(unsigned int j=0;j<iCount2;j++)
				{
					PgExtraData	const	&kData = kDataCont[j];
					kPacket.Push(kData.m_kType);
					kPacket.Push(kData.m_kValue);
					kPacket.Push(kData.m_kBinaryValue);
				}
			}

		}
		void	PopFromPacket(BM::Stream &kPacket)
		{
			unsigned int	iCount = 0;
			kPacket.Pop(iCount);
			m_kExtraDataPackCont.resize(iCount);

			for(unsigned int i=0;i<iCount;i++)
			{
				CONT_EXTRA_DATA	&kDataCont = m_kExtraDataPackCont[i].m_kExtraDataCont;

				unsigned int	iCount2 = 0; 
				kPacket.Pop(iCount2);

				kDataCont.resize(iCount2);

				for(unsigned int j=0;j<iCount2;j++)
				{
					PgExtraData	&kData = kDataCont[j];
					kPacket.Pop(kData.m_kType);
					kPacket.Pop(kData.m_kValue);
					kPacket.Pop(kData.m_kBinaryValue);
				}
			}
		}

		int	AddExtraDataPack(PgExtraDataPack const &kExtraDataPack)
		{
			if(kExtraDataPack.m_kExtraDataCont.size() == 0)
			{
				return	-1;
			}

			m_kExtraDataPackCont.push_back(kExtraDataPack);

			return	static_cast<unsigned int>(m_kExtraDataPackCont.size()-1);
		}

		void	ClearAllExtraDataPack()
		{
			m_kExtraDataPackCont.clear();
		}

	}PgExtraDataPackInfo;

	class	CXUI_Style_String
	{
		std::wstring m_OriginalString;
		CONT_PARSED_CHAR	m_vChar;
		std::wstring 	m_AdjustedString;
		POINT	m_ptSize;

		PgExtraDataPackInfo	m_kExtraDataPackInfo;

	public:
		CXUI_Style_String();
		CXUI_Style_String(PgFontDef const& kFontDef,std::wstring const& wString);

		void operator = (CXUI_Style_String const& kStyleString);
		bool operator == (CXUI_Style_String const& kStyleString);
		bool operator != (CXUI_Style_String const& kStyleString);
		void operator += (PgParsedChar &kChar);
		void operator += (CXUI_Style_String const& kStyleString);
		bool operator == (std::wstring const& wString);
		bool operator != (std::wstring const& wString);
		PgParsedChar&	operator[](int _Pos);

		void AddParsedChar(PgParsedChar& kChar,int iCharWidth = -1);
		void RecalculateAlignHeight();
	public:

		void Clear();
		size_t Length()	const	{	return	m_vChar.size();	}

		const CONT_PARSED_CHAR	&GetCharVector()	const {	return	m_vChar;	}	
		bool	IsEqual(CXUI_Style_String const& kStyleString);
		bool	IsEqual(std::wstring const& wString);

		const	std::wstring&	GetOriginalString() const	{	return	m_OriginalString;	}
		std::wstring	GetNormalString()	const
		{
			std::wstring	kString;
			int	iLen = static_cast<int>(m_vChar.size());
			for(int i=0;i<iLen;++i)
			{
				kString += m_vChar[i].m_wChar;
			}

			return	kString;
		}

		void CreateStyleString(PgFontDef const& kFontDef,std::wstring const& wString);
		POINT	GetSize()	const	{	return	m_ptSize;	}

		void SetOriginalString(std::wstring const& kString)	{	m_OriginalString = kString;	}
		const	std::wstring& GetAdjustedString() const 	{	return m_AdjustedString;	}
		bool	AdjustText();

		void ProcessWordWrap(int iLimitWidth,bool bUseUnitWordWrap,CXUI_Style_String &kout_Result,int iIndent=0);
		static	int GetCharWidth(const PgParsedChar &wChar);

		PgExtraDataPackInfo const&	GetExtraDataPackInfo()	const{	return	m_kExtraDataPackInfo;	}
		PgExtraDataPackInfo &	GetExtraDataPackInfo()	{	return	m_kExtraDataPackInfo;	}
		void	SetExtraDataPackInfo(PgExtraDataPackInfo const &kInfo)	{	m_kExtraDataPackInfo = kInfo;	}

	private:

		std::wstring::size_type	ParseNextOption(std::wstring const &kOptionStr,std::wstring::size_type iStartPos,std::wstring &koutOptionType,std::wstring &koutOptionValue);
		bool	ParseValue(std::wstring const& rkOrg, wchar_t option, std::wstring &Out);
		bool	ParseText(std::wstring const& strText, std::list<PgParsedText> &rkOutList,PgFontDef const& kFontDef);


	};
	
	typedef unsigned int (*F_CreateFontFunc) (std::wstring const& wstrFontFileName, FT_Library &ftlib, FT_Face *pkFace, FT_Byte*& ptOutPtr); 
	class CXUI_Font
	{
	public:
		enum	FONT_STYLE
		{
			FS_NONE=0,
			FS_BOLD=1<<0,//1
			FS_ITALIC=1<<1,//2
			FS_UNDERLINE=1<<2,//4
		};

		FONT_STYLE	m_Style;
		FONT_STYLE	m_Style2;

		int	m_iBoldSize;

		FT_Encoding	m_Encoding;

		typedef std::map<unsigned short, POINT> PointMap;
	public:

		CXUI_Font(std::wstring const& wFontKey);
		virtual ~CXUI_Font();

		int InitFontFile(std::wstring const& wstrFontFileName, int const iFontSize, FT_Library &ftlib);
		virtual	int CalcWidth(std::wstring const& text, int iTextLen=0)const;
		virtual	int const CalcWidthAddReturn(std::wstring const& text, std::wstring &rkOut, VEC_LINE &rkLine, int const iWidth);

		virtual	int GetWidth(const TCHAR code)const;
		virtual	int GetHeight(const TCHAR code)const;
		virtual	int GetHeight()const;
		virtual	int MaxHeight(std::wstring const& text)const;
		virtual	bool IsNeedAnimation(const TCHAR code) const		{ return false; }
		virtual int IsEmoticon()const								{ return false; }

		FONT_STYLE	GetStyle() const;
		void SetStyle(FONT_STYLE style);
		void SetStyle2(FONT_STYLE style2);	// 항상 적용되도록 하는 FONT_STYLE - 임시;

		virtual	int Draw(_ARGB16 * ptr, int const iMaxBuffSize, int const dx, int const dy, int const pitch, unsigned short const code,_ARGB16 const& color);
		virtual	int Draw(_ARGB16 * ptr, int const iMaxBuffSize, int const dx, int const dy, int const pitch, wchar_t const* strText, _ARGB16 const& color);

		std::wstring const&	GetFontKey()const{	return	m_wFontKey;	}

		POINT2	GetFontSize()const;
		void SetFontSize(int const iWidth, int const iHeight);

		bool	GetSizeFromSizeMap(unsigned short const sCode, POINT &kSize)const;
		FT_Face* GetFontFace()	{return &m_FontFace;}
		
	protected:
		mutable Loki::Mutex m_kMutex;
		mutable PointMap	m_kSizeMap;//mutable 조심. lock 걸고 쓸것.
		
		int	m_iFontWidth, m_iFontHeight;
		FT_Face	m_FontFace;
		std::wstring m_wFontKey;
		unsigned	short	GetCodeByEncoding(unsigned	short sUnicode) const;
	};

	class	CXUI_2DString
	{
	public:

		CXUI_2DString(CXUI_Style_String const& kText);		
		CXUI_2DString(PgFontDef const& kFontDef,std::wstring const& wString);
		virtual	~CXUI_2DString(){};

		virtual	void Draw(int iScreenX,int iScreenY,float fR,float fG,float fB,float fA,bool bOutline = false)=0;
		virtual	void SetText(PgFontDef const& kFontDef, std::wstring const& wText,int iLimitWidth= -1,bool bUseWordWrap = false)=0;
		virtual	void SetText(CXUI_Style_String const& kText,int iLimitWidth= -1,bool bUseWordWrap = false)=0;

		virtual void SetDrawByTime(bool bValue) = 0;
		virtual void ResetInitTime() = 0;

		virtual	POINT	GetSize() = 0;

		virtual	bool	GetParsedCharAt(int iTargetX,int iTargetY,PgParsedChar &kfoundChar)	
		{
			return	false;
		};

		CXUI_Style_String&	GetText()	{	return	m_kText;	}

	protected:
		CXUI_Style_String m_kText;
	private:

		void CreateStyleString(PgFontDef const& kFontDef,std::wstring const& wString);

	};

	typedef CXUI_2DString* (*F_Create2DStringFunc) (CXUI_Style_String const& kText); 
	typedef POINT2 (*F_CalculateOnlySizeFunc) (CXUI_Style_String const& kText); 
	class	CXUI_FontManager 
	{
		F_Create2DStringFunc	m_Create2DStringFunc;
		F_CreateFontFunc		m_CreateFontFunc;	//팩된 폰트를 읽기 위해서
		F_CalculateOnlySizeFunc	m_CalculateOnlySizeFunc;

	public:
		CXUI_FontManager();
		~CXUI_FontManager();

	public:
		void Init(F_Create2DStringFunc	 Create2DStringFunc, F_CreateFontFunc fCreateFontFunc = NULL);
		void Destroy();

		CXUI_Font* AddNewFont(std::wstring const& wstrFontKey, std::wstring const& wstrFontFileName, int const iFontSize);
		CXUI_Font* AddNewFont(std::wstring const& wstrFontKey, CXUI_Font *pkFont);
		CXUI_Font* SetFont(std::wstring const& wstrFontKey);
		CXUI_Font*	GetFont(std::wstring const& wstrFontKey);
		CXUI_Font*	GetCurrentFont()	{	return	m_CurrentFont;	}

		CXUI_2DString *CreateNew2DString(PgFontDef const& kFontDef, std::wstring const& Text);
		CXUI_2DString *CreateNew2DString(CXUI_Style_String const& kString);

		POINT2 CalculateOnlySize(CXUI_Style_String const& kString);
		void SetCalculateOnlySizeFunc(F_CalculateOnlySizeFunc pkFunc){ m_CalculateOnlySizeFunc = pkFunc; }

		FT_Library	GetFTLib()	{	return	m_ftlib;	}

	protected:

		FT_Library  m_ftlib;

		typedef std::map< std::wstring, CXUI_Font* > FontHash;
		FontHash m_kFontHash;
		CXUI_Font* m_CurrentFont;//현재 찍을 폰트

		typedef struct stFontFileInfo
		{
			FT_Byte*		m_pkByte;
			unsigned int	m_uiSize;
			stFontFileInfo() : m_pkByte(NULL), m_uiSize(0)  {}
			
		}SFontFileInfo;
		typedef std::map< std::wstring, SFontFileInfo > FontFileHash;
		FontFileHash m_kFontFileHash;
	};
}

#define g_kFontMgr SINGLETON_STATIC(XUI::CXUI_FontManager)

#endif // WEAPON_XUI_XUI_FONT_H