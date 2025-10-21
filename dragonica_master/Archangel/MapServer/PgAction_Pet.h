#ifndef MAP_MAPSERVER_ACTION_ACTION_PGACTION_PET_H
#define MAP_MAPSERVER_ACTION_ACTION_PGACTION_PET_H

#include "Variant/PgTotalObjectMgr.h"

class PgPet;
class PgGround;

class PgActPet_TickState
{
public:
	explicit PgActPet_TickState( UNIT_PTR_ARRAY &, PgGround * const );
	~PgActPet_TickState(void){}

	bool DoAction( PgPet * pkPet );

private:
	UNIT_PTR_ARRAY&		m_kDelUnitArray;//레퍼런스다!!!
	__int64	const		m_i64CurSecTime;//second
	PgGround * const	m_pkGround;

private:// 사용 금지
	PgActPet_TickState(void);
	PgActPet_TickState( PgActPet_TickState const & );
	PgActPet_TickState& operator = ( PgActPet_TickState const & );
};

class PgActPet_AddExp
{
public:
	explicit PgActPet_AddExp( PgClassPetDefMgr const & );
	~PgActPet_AddExp(void){}

	bool PopAction( PgPet * pkPet, SPMOD_AddAbilPet::CONT_ABILLIST &rkOutAbilList, __int64 &ri64OutPlayerExp )const;
	bool PopAbilList( PgPet * pkPet, __int64 i64AddExp, SPMOD_AddAbilPet::CONT_ABILLIST &rkOutAbilList )const;
	bool GetCheckAddExp( PgPet * pkPet, __int64& i64AddExp )const;

protected:
	__int64 GetExp( PgPet * pkPet )const;

protected:
	PgClassPetDefMgr const	&m_kPetDefMgr;

private:// 사용 금지
	PgActPet_AddExp(void);
	PgActPet_AddExp( PgActPet_AddExp const & );
	PgActPet_AddExp& operator = ( PgActPet_AddExp const & );
};

class PgActPet_Rename
{
public:
	explicit PgActPet_Rename( PgGround * const pkGround, SItemPos const &kPetItemPos );
	~PgActPet_Rename(void){}

	bool DoAction( CUnit* pkUser, std::wstring &wstrPetName );

protected:
	virtual int MakeUseOrder( CONT_PLAYER_MODIFY_ORDER &rkOrder, CUnit *pkUser )const;

private:
	SItemPos const		m_kPetItemPos;
	PgGround * const	m_pkGround;

private:// 사용 금지
	PgActPet_Rename(void);
	PgActPet_Rename( PgActPet_Rename const & );
	PgActPet_Rename& operator = ( PgActPet_Rename const & );
};

class PgActPet_Rename_ForItem
	:	public PgActPet_Rename
{
public:
	explicit PgActPet_Rename_ForItem( PgGround * const pkGround, SItemPos const &kPetItemPos, SItemPos const &kRenameItemPos );
	~PgActPet_Rename_ForItem(void){}

protected:
	virtual int MakeUseOrder( CONT_PLAYER_MODIFY_ORDER &rkOrder, CUnit *pkUser )const;

private:
	SItemPos const		m_kRenameItemPos;
};

class PgActPet_ColorChange
{
public:
	explicit PgActPet_ColorChange( PgGround * const pkGround, SItemPos const &kPetItemPos );
	~PgActPet_ColorChange(void){}

	bool DoAction( CUnit* pkUser, char cColorIndex );

protected:
	virtual int MakeUseOrder( CONT_PLAYER_MODIFY_ORDER &rkOrder, CUnit *pkUser )const;

private:
	SItemPos const		m_kPetItemPos;
	PgGround * const	m_pkGround;

private:// 사용 금지
	PgActPet_ColorChange(void);
	PgActPet_ColorChange( PgActPet_ColorChange const & );
	PgActPet_ColorChange& operator = ( PgActPet_ColorChange const & );
};

class PgActPet_ColorChange_ForItem
	:	public PgActPet_ColorChange
{
public:
	explicit PgActPet_ColorChange_ForItem( PgGround * const pkGround, SItemPos const &kPetItemPos, SItemPos const &kColorChangeItemPos );
	~PgActPet_ColorChange_ForItem(void){}

protected:
	virtual int MakeUseOrder( CONT_PLAYER_MODIFY_ORDER &rkOrder, CUnit *pkUser )const;

private:
	SItemPos const		m_kColorChangeItemPos;
};

class PgActPet_MakeClassChangeOrder
{
public:
	explicit PgActPet_MakeClassChangeOrder( CONT_PLAYER_MODIFY_ORDER &rkOrder, bool const bGMCommand=false, PgBase_Item kPetItem=PgBase_Item(), SItemPos kItemPos=SItemPos::NullData() );
	~PgActPet_MakeClassChangeOrder(void){}

	bool DoAction( CUnit *pkPlayer, int const iGrade, short nLv = 0 );//nLv이 0이면 현재 레벨

protected:
	CONT_PLAYER_MODIFY_ORDER	&m_rkOrder;
	bool const					m_bGMCommand;
	PgBase_Item					m_kPetItem;
	SItemPos					m_kItemPos;

private:// 사용 금지
	PgActPet_MakeClassChangeOrder(void);
	PgActPet_MakeClassChangeOrder( PgActPet_MakeClassChangeOrder const & );
	PgActPet_MakeClassChangeOrder& operator = ( PgActPet_MakeClassChangeOrder const & );
};

class PgActPet_RefreshPassiveSkill
{
public:
	explicit PgActPet_RefreshPassiveSkill( PgGround const *pkGround );
	~PgActPet_RefreshPassiveSkill(void){}

	static bool DoAction_ReleaseTarget( PgPet * pkPet, PgPlayer *pkCallerPlayer, PgGround *pkGround );

	bool DoAction( PgPet * pkPet );

private:
	PgGround const *m_pkGround;

private:// 사용 금지
	PgActPet_RefreshPassiveSkill(void);
	PgActPet_RefreshPassiveSkill( PgActPet_RefreshPassiveSkill const & );
	PgActPet_RefreshPassiveSkill& operator = ( PgActPet_RefreshPassiveSkill const & );
};

#endif // MAP_MAPSERVER_ACTION_ACTION_PGACTION_PET_H