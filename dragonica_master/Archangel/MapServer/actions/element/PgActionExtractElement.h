#pragma once
#include "PgAction.h"

class PgAction_Extract_Element
	:	public PgUtilAction
{
public:
    SGroundKey const &m_kGndKey;
    BM::Stream & m_kPacket;

	explicit PgAction_Extract_Element(SGroundKey const &kGroundKey, BM::Stream & kPacket):m_kGndKey(kGroundKey), m_kPacket(kPacket){}
	virtual ~PgAction_Extract_Element(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);
};
