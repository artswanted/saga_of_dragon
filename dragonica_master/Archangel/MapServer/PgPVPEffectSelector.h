#ifndef MAPSERVER_MAPSERVER_PgPVPEffectSelector_H
#define MAPSERVER_MAPSERVER_PgPVPEffectSelector_H

class CUnit;

class PgPVPEffectSelector
{
public:
	typedef std::vector<int> CONT_INT;
		
	struct SData
	{
		__int64 i64ClassLimit;
		CONT_INT kContEffectNo;

		SData():
		 i64ClassLimit(0)
		{}

		bool Empty() const;
	};

	//typedef std::unordered_map<__int64, SData> CONT_PVP_EFFECT;
	typedef std::vector<SData>
		//std::unordered_map<__int64, SData> 
		CONT_PVP_EFFECT;
public:
	PgPVPEffectSelector();
	~PgPVPEffectSelector();

	bool Build();									// 초기화 (xml 파싱함수 호출)
	void Release();									// 해제

	bool ParseXml(std::wstring const &kXmlPath);	// xml 파싱
	
	bool AddEffect(CUnit* pkUnit, SActArg* pkArg) const ;	// Unit에 알맞은 PvPEffect를 건다
	bool DelEffect(CUnit* pkUnit) const;					// Unit에 걸었던 PvPEffect를 삭제한다

private:
	CONT_PVP_EFFECT m_kPVPEffectCont;
};


#define g_kPVPEffectSlector SINGLETON_STATIC(PgPVPEffectSelector)

#endif