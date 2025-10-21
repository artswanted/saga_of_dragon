#ifndef WEAPON_HELLDART_PGPROTOCOLFILTER_H
#define WEAPON_HELLDART_PGPROTOCOLFILTER_H

#include "BM/Stream.h"

#include "BM/Functor.h"

class PgProtocolFilter
	: public BM::CFunctor< WORD, HRESULT, BM::Stream >
{
public:
	PgProtocolFilter();
	virtual ~PgProtocolFilter();

public:
	HRESULT FilterProtocol(BM::Stream &kInPacket)const;//원본을 집어 넣어야함.
};

#define g_kProtocolFilter SINGLETON_STATIC(PgProtocolFilter)

#endif // WEAPON_HELLDART_PGPROTOCOLFILTER_H