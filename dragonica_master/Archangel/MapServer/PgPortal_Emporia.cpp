#include "stdafx.h"
#include "PgPortal_Emporia.h"

PgPortal_Emporia::PgPortal_Emporia( PgGTrigger_PortalEmporia const * pGTrigger_PortalEmporia )
:	PgEmporia( pGTrigger_PortalEmporia->GetEmporiaKey(), SEmporiaGuildInfo(), EMPORIA_GATE_CLOSE, BM::DBTIMESTAMP_EX() )
,	m_pGTrigger_PortalEmporia(pGTrigger_PortalEmporia)
{
}

PgPortal_Emporia::PgPortal_Emporia(void)
{
}

PgPortal_Emporia::~PgPortal_Emporia(void)
{
}

void PgPortal_Emporia::WriteToPacket_ToClient( BM::Stream &kPacket )const
{
	kPacket.Push( m_pGTrigger_PortalEmporia->GetID() );
	m_kOwner.WriteToPacket( kPacket );
}
