#pragma once

#include "BM/PgTask.h"

typedef enum ePrimaryEventMessageType
{
	PMET_MMC_ANS_LOG_FILE_INFO = 12,
	PMET_MMC_ANS_GET_FILE = 15,
	PMET_SMC_ANS_LOG_FILE_INFO = 13,
	PMET_SMC_ANS_GET_FILE = 16,
}EPrimaryEventMessageType;

//! MCT에서 다운로드 처리를 위해 사용하는 태스크
class PgTask_MCT
	:public PgTask<>
{
public:
	PgTask_MCT(){};
	virtual ~PgTask_MCT();

public:
	virtual void HandleMessage(SEventMessage *pkMsg);
	void Close();

	bool WriteToPacket(EPrimaryEventMessageType eType, BM::CPacket const * pkPacket);

private:
};

#define g_kTask SINGLETON_STATIC(PgTask_MCT)