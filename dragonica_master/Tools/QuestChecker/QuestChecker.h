#pragma once

class CQuestChecker : public PgQuestInfo
{
private:
	bool	bIsSuccess;

public:
	void	CheckItem(DWORD const dwItemNo, TiXmlElement* pkNode);
	void	CheckMonster(DWORD const dwMonsterID, TiXmlElement* pkNode);
	void	CheckGround(DWORD const dwGroundID, TiXmlElement* pkNode);
	void	ParseError(TiXmlNode* pkNode);
	bool	GetIsSuccess(){ return bIsSuccess; }

	CQuestChecker(void) : bIsSuccess(true) {};
	virtual ~CQuestChecker(void) {};
};
