#pragma once

#define PG_USE_PGENTITYSTREAM


class PgNiEntityStreamingAscii2 : public NiEntityStreamingAscii
{
public:
	PgNiEntityStreamingAscii2(void);
	virtual ~PgNiEntityStreamingAscii2(void);

    virtual NiBool Load(char const* pcFileName);
	NiBool PgStoreWorkingPath(char const* pcFileName);

public:
    static void _SDMInit();
    static void _SDMShutdown();
};

NiFactoryDeclareStreaming(PgNiEntityStreamingAscii2);
