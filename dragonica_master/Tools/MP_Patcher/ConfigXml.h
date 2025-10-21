#pragma once

#include	"defines.h"

typedef struct	tagSetConfig
{
	tagSetConfig()
	{
		FullMode		= false;
		GraphicMode		= false;
		BgmMute			= false;
		SeMute			= false;
		BgmVolume		= 100;
		SeVolume		= 100;
	}

	bool	FullMode;
	bool	GraphicMode;
	bool	BgmMute;
	bool	SeMute;
	int		BgmVolume;
	int		SeVolume;
}SetConfig;

class CConfigXml
{
private:
	SetConfig		m_ConfigVal;
	TiXmlDocument*	m_DocConfig;

public:
	CConfigXml(void);
	virtual ~CConfigXml(void);

	bool	Seek();
	bool	Clear();
	bool	Save();
	
	bool	GetUseScreenMode() const		{ return m_ConfigVal.FullMode; }
	void	SetUseScreenMode(bool State)	{ m_ConfigVal.FullMode = State; }

	bool	GetUseGraphicMode() const		{ return m_ConfigVal.GraphicMode; }
	void	SetUseGraphicMode(bool State)	{ m_ConfigVal.GraphicMode = State; }

	bool	GetUseBgmMode() const			{ return m_ConfigVal.BgmMute; }
	void	SetUseBgmMode(bool State)		{ m_ConfigVal.BgmMute = State; }

	bool	GetUseSeMode() const			{ return m_ConfigVal.SeMute; }
	void	SetUseSeMode(bool State)		{ m_ConfigVal.SeMute = State; }

	int		GetUseBgmVolume() const			{ return m_ConfigVal.BgmVolume; }
	void	SetUseBgmVolume(int Value)		{ m_ConfigVal.BgmVolume = Value; }

	int		GetUseSeVolume() const			{ return m_ConfigVal.SeVolume; }
	void	SetUseSeVolume(int Value)		{ m_ConfigVal.SeVolume = Value; }

private:
	bool	Parse(std::vector<char>& Data);
	bool	UnCompressData(const size_t& OrgFileSize, std::vector<char>& rkVecData, std::vector<char>& rkVecOut) const;
	bool	DecryptData(size_t const FileOrgSize, size_t const FileZipSize, bool const bIsCompressed, bool const bIsEncrypted, std::vector<char> const& rkVecData, std::vector<char>& rkVecOut) const;
	bool	DelElement(TiXmlDocument*& Docu) const;
	bool	GetDocOptionVal();
	bool	SetDocOptionVal() const;
};
