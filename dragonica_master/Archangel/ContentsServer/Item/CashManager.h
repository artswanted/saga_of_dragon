#ifndef CONTENTS_CONTENTSSERVER_ITEM_CASHMANAGER_H
#define CONTENTS_CONTENTSSERVER_ITEM_CASHMANAGER_H

#include "BM/TWrapper.h"

class PgTranPointer;

class PgCashManager
{
public:
	PgCashManager();
	~PgCashManager();

	static bool RequestCurerntLimitSell(BM::Stream &rkPacket);
	static bool RecvCurerntLimitSell(BM::Stream &rkPacket);
	static bool RequestCurerntCash(BM::Stream &rkPacket);
	static bool RecvCurrentCash(BM::Stream &rkPacket);
	static bool RequestCashModify(BM::Stream &rkPacket);
	static bool RecvCashModify(BM::Stream &rkPacket);
	static bool FinishCashProcess(BM::Stream &rkPacket);
	static void OnCashRollback_Failed(ECashShopResult const eResult, PgTranPointer& rkTran);
private:
	// 구현하면 안되는 함수 (사용금지)
	PgCashManager(PgCashManager const& rhs);
	PgCashManager const& operator = (PgCashManager const& rhs);
};

/*
class PgCashManagerWrap : public TWrapper<PgCashManager>
{
public:
	PgCashManagerWrap();
	~PgCashManagerWrap();

private:
	// 구현하면 안되는 함수 (사용금지)
	PgCashManagerWrap const& PgCashManagerWrap(PgCashManagerWrap const& rhs);
	PgCashManagerWrap const& operator = (PgCashManagerWrap const& rhs);
};
*/

#endif // CONTENTS_CONTENTSSERVER_ITEM_CASHMANAGER_H