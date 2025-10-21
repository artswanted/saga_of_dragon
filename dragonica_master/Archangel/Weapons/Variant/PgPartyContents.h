#ifndef WEAPON_VARIANT_PGPARTYCONTENTS_H
#define WEAPON_VARIANT_PGPARTYCONTENTS_H

// pure virtual class
class PgPartyContents
{
public:
	PgPartyContents(){}
	virtual ~PgPartyContents(){}

	virtual void Init( PgParty &rkParty ) = 0;
	virtual HRESULT IsInvite(void)const = 0;
	virtual HRESULT IsJoin(void)const = 0;
	virtual HRESULT IsChangeMaster(void)const = 0;
	virtual HRESULT IsChangeOption(void)const = 0;
	virtual bool Leave( BM::GUID const &kCharGuid, bool const bIsMaster ) = 0;//리턴값은 종료여부
};

#endif // WEAPON_VARIANT_PGPARTYCONTENTS_H