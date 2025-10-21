#ifndef MAPSERVER_MAPSERVER_PgSkillLinkageInfo_H
#define MAPSERVER_MAPSERVER_PgSkillLinkageInfo_H
//
//namespace stdext
//{
//	template<class _Kty,
//		class _Ty,
//		class _Tr = hash_compare<_Kty, _STD less<_Kty> >,
//		class _Alloc = _STD allocator< _STD pair<const _Kty, _Ty> > >
//		class hash_map;
//}

class PgSkillLinkageInfo
{
	typedef std::unordered_map<int, int> CONT_LINKAGE_EFFECTNO;

	struct SInfo
	{
		SInfo()
			:m_iMaxLv(0)
		{}
		SInfo(int const iMaxLv_in)
			:m_iMaxLv(iMaxLv_in)
		{}
		void SetMaxLv(int const iMaxLv) { m_iMaxLv = iMaxLv; }
		int GetMaxLv() const { return m_iMaxLv; }

		void AddInfo(int const iLinkageLv, int const iLinkageEffectNo, int const iSkillNo);
		bool GetInfo(int const iLinkageLv, int& iLinkageEffectNo_out, int const iSkillNo) const;
	private:
		int m_iMaxLv;
		CONT_LINKAGE_EFFECTNO m_kContEachLinkageLvEffectNo;
	};
	
	typedef std::unordered_map<int, SInfo> CONT_LINKAGE_SKILL_INFO;

public:
	PgSkillLinkageInfo();
	~PgSkillLinkageInfo();

public :
	void Release();
	bool Build();
	
	bool ParseXml(std::wstring const &strXmlPath);
	bool CheckAbil(int const iSkillNo, int const iLinkageLevel) const;
	bool GetInfo(int const iSkillNo, int const iLinkageLevel, int& iLinkageEffectNo_out) const;	// iSkillNo를 사용할때, 연계레벨(iLinkageLevel)에 해당하는 EfffectNo(iLinkageEffectNo_out)를 얻어옴	
private:
	void AddInfo(int const iSkillNo, int const iLinkageLv, int const iMaxLinkageLv, int const iLinkageEffectNo); // iSKillNo가 사용될때, 증가된 iLnkageLevel에 해당하는 iLinkageEffectNo
	void AddInfo(int const iSkillNo, SInfo const& rkInfo);
	
private:
	CONT_LINKAGE_SKILL_INFO m_kCont;
};

#define g_kSkillLinkageInfo SINGLETON_STATIC(PgSkillLinkageInfo)

#endif