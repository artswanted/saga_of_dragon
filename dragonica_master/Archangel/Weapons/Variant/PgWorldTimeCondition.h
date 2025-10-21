#ifndef FREEDOM_DRAGONICA_SCENE_WORLD_PGWORLDTIMECONDITION_H
#define FREEDOM_DRAGONICA_SCENE_WORLD_PGWORLDTIMECONDITION_H

class	PgWorldTimeCondition
{
public:

	void	SetConditionAttributeName(
		std::string const &kFromConditionAttributeName,
		std::string const &kToConditionAttributeName)
	{
		m_kFromConditionAttributeName = kFromConditionAttributeName;
		m_kToConditionAttributeName = kToConditionAttributeName;
	}

	bool	CheckTheElementHasTimeConditionAttribute(TiXmlElement const *pkElement)	const;
	bool	ReadFromXmlElement(TiXmlElement const *pkElement);

public:

	bool	CheckTimeIsInsideDuration(SYSTEMTIME const &kTime)	const;
	std::string const& GetID()const { return m_kID; }
private:

	std::string m_kID;
	SYSTEMTIME	m_kDurationFrom;
	SYSTEMTIME	m_kDurationTo;

	std::string	m_kFromConditionAttributeName;
	std::string	m_kToConditionAttributeName;
};

#endif // FREEDOM_DRAGONICA_SCENE_WORLD_PGWORLDTIMECONDITION_H