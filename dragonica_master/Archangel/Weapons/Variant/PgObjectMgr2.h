#ifndef WEAPON_VARIANT_UNIT_PGOBJECTMGR2_H
#define WEAPON_VARIANT_UNIT_PGOBJECTMGR2_H

#include "PgObjectMgr.h"

class PgObserverPlayer
{
public:
	explicit PgObserverPlayer( PgPlayer * pkPlayer, BM::GUID const &kOwnerID );
	explicit PgObserverPlayer( PgNetModule<> const &kNetModule, BM::GUID const &kCharID, BM::GUID const &kOwnerID );
	~PgObserverPlayer(void){}

	PgNetModule<> const &GetNetModule(void)const{return m_kNetModule;}
	BM::GUID const &GetOwnerID(void)const{return m_kOwnerID;}
	void SetOwnerID( BM::GUID const &kOwnerID ){m_kOwnerID=kOwnerID;}

	void SetPlayer( PgPlayer * pkPlayer );

	BM::GUID GetCharID(void)const{return m_kCharID;}

	bool Send( BM::Stream const &kPacket ){return m_kNetModule.Send( kPacket, true, true );}
private:
	PgNetModule<>	m_kNetModule;
	BM::GUID		m_kOwnerID;
	BM::GUID		m_kCharID;

private:// 사용금지
	PgObserverPlayer(void);
};

class PgObjectMgr2
	:	protected PgObjectMgr//protected 상속이다!!!!!
{
public:
	typedef std::map< BM::GUID, PgObserverPlayer >	CONT_OBSERVER;// first CharGuid

public:
	PgObjectMgr2(void);
	virtual ~PgObjectMgr2(void);

	void InitObjectMgr();// 상속받은 함수임

protected:
	void ReleaseObserver();

	HRESULT RegistObserver( PgNetModule<> const &kNetModule, BM::GUID const &kCharGuid, BM::GUID const &kTargetID, EUnitType const kTargetUnitType );
	HRESULT UnRegistObserver( BM::GUID const &kID, BM::GUID *pkOutOwnerID );
	HRESULT UnRegistObserver( PgObserverPlayer const &kObserver );

	HRESULT ChangeObserver( PgNetModule<> const &kNetModule, BM::GUID const &kCharGuid, BM::GUID const &kTargetID, EUnitType const kTargetUnitType, BM::GUID &rkOutOldTargetID );

	void UnRegistHasObserver( CUnit *pkUnit, CUnit* pkTarget, ContGuidSet* pkContHasObserver = NULL );

	bool UnRegistUnit( CUnit *pkUnit );// 상속받은 함수임
	
	CUnit *GetRandomUnit( EUnitType const eType, BM::GUID const &kIgnoreID );

	BM::GUID GetFirstObserver(void)const;
	HRESULT GetObserver( BM::GUID const &kCharGuid, PgNetModule<>& rkOutModule )const;
	

protected:
	CONT_OBSERVER		m_kContObserver;
};

#endif // WEAPON_VARIANT_UNIT_PGOBJECTMGR2_H