#ifndef MACHINE_MMC_PGMMCTASK_H
#define MACHINE_MMC_PGMMCTASK_H

#include "BM/PgTask.h"

typedef enum ePrimaryEventMessageType
{
	PMET_MMC_MANAGER,
	PMET_FILE_COPY_MANAGER,
}EPrimaryEventMessageType;

PACKET_DEF( PT_MMC_LOADPATCHFILE,	1 )
PACKET_DEF( PT_MCT_REQ_GET_FILE_INFO,	 2 )
PACKET_DEF( PT_MCT_REQ_GET_FILE,	3 )
PACKET_DEF( PT_MCT_REFRESH_LOG,	4 )
PACKET_DEF( PT_MCT_SMC_ANS_GET_FILE,	5 )

class PgTask_MMC
	:public PgTask<>
{
public:
	PgTask_MMC(){};
	virtual ~PgTask_MMC(){};

public:
	virtual void HandleMessage(SEventMessage *pkMsg);
	void Close();

	bool WriteToPacket(EPrimaryEventMessageType eType, BM::Stream & rkPacket);

private:
};

#define g_kTask SINGLETON_STATIC(PgTask_MMC)

#endif // MACHINE_MMC_PGMMCTASK_H