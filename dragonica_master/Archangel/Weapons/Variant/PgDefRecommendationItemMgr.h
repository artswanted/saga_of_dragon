#ifndef WEAPON_VARIANT_PGDEFRECOMMENDATIONITEMMGR_H
#define WEAPON_VARIANT_PGDEFRECOMMENDATIONITEMMGR_H

typedef std::map<int,std::vector<CONT_DEF_RECOMMENDATION_ITEM::const_iterator> > CONT_CLASS_RECOMMENDATION_ITEM;

class PgDefRecommendationItemMgr
{
public:

	PgDefRecommendationItemMgr(){}
	~PgDefRecommendationItemMgr(){}

public:

	bool GetRecommendationItemByItemNo(int const charclass,int const charlevel,int const iItemNo,CONT_ITEMNO & rkRecommendationItem) const;
	bool GetRecommendationItem(int const charclass,int const charlevel,CONT_ITEMNO & rkRecommendationItem) const;
	bool Build(CONT_DEF_RECOMMENDATION_ITEM const & rkContRecommendationItem);

private:

	CONT_CLASS_RECOMMENDATION_ITEM m_kContRecommendatinItem;
};

//#define g_kDefRecommendationItemMgr SINGLETON_STATIC(PgDefRecommendationItemMgr)

#endif // WEAPON_VARIANT_PGDEFRECOMMENDATIONITEMMGR_H