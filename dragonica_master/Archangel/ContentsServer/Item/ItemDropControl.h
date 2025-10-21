#ifndef CONTENTS_CONTENTSSERVER_ITEM_ITEMDROPCONTROL_H
#define CONTENTS_CONTENTSSERVER_ITEM_ITEMDROPCONTROL_H

#include "BM/TWrapper.h"

typedef struct tagTBL_LIMITED_ITEM_RECORD
{
	tagTBL_LIMITED_ITEM_RECORD()
	{
		iEventNo = 0;
		iLimit_ResetPeriod = 0;
		iSafe_ResetPeriod = 0;
		iRefreshCount = 0;
		iLimitCount = 0;
		iSafeCount = 0;
	}

	int iEventNo;
	BM::DBTIMESTAMP_EX dtLimitRefreshDate;//마지막 limit 의 갱신 시간.
	BM::DBTIMESTAMP_EX dtSafeRefreshDate;//창고의 갱신 시간
	BM::DBTIMESTAMP_EX dtStartDate;//이벤트의 시작 시간
	BM::DBTIMESTAMP_EX dtEndDate;//이벤트의 끝시간
	int iLimit_ResetPeriod;//limit 의 갱신 주기
	int iSafe_ResetPeriod;//limit 창고의 갱신 주기: 분단위.
	int iRefreshCount;//갱신값.
	int iLimitCount;//현재 limit의 남은 값
	int iSafeCount;//창고에 누적된 값

	bool IsCorrectTime(BM::DBTIMESTAMP_EX const &kNow)const
	{
		bool bIsCorrectTime = true;
		if( !dtStartDate.IsNull()
		&&	dtStartDate > kNow)
		{
			bIsCorrectTime = false;
		}

		if( !dtEndDate.IsNull()
		&&	dtEndDate < kNow)
		{
			bIsCorrectTime = false;
		}
		return bIsCorrectTime;
	}

}TBL_LIMITED_ITEM_RECORD;


class PgItemDropControlImpl
{
public:
	PgItemDropControlImpl();
	~PgItemDropControlImpl();

	HRESULT OnDBProcess(CEL::DB_RESULT &rkResult);
	void Timer1m();
	void RequestItemPop(SERVER_IDENTITY const& rkSI, SREQ_GIVE_LIMITED_ITEM const& kReq);

protected:
	HRESULT Q_DQT_LOAD_LOCAL_LIMITED_ITEM(CEL::DB_RESULT& rkResult);
	HRESULT Q_DQT_LOAD_LOCAL_LIMITED_ITEM_CONTROL(CEL::DB_RESULT& rkResult);
	HRESULT Q_DQT_CHECK_LIMIT_LIMITED_ITEM_RECORD(CEL::DB_RESULT& rkResult);
	HRESULT Q_DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD(CEL::DB_RESULT& rkResult);
	HRESULT Q_DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD_POP(CEL::DB_RESULT& rkResult);
	HRESULT Q_DQT_SYNC_LOCAL_LIMITED_ITEM(CEL::DB_RESULT& rkResult);
	HRESULT Q_DQT_UPDATE_SAFE_LIMITED_ITEM_RECORD(CEL::DB_RESULT& rkResult);

	typedef std::map< TBL_KEY_INT,	TBL_LIMITED_ITEM_RECORD >	CONT_LIMITED_ITEM_RECORD;

protected :
	CLASS_DECLARATION_S(int, QueryCount);	// DB가 너무 바쁘면, Update 하지 않도록 하기 위함.
	CLASS_DECLARATION_S(BM::GUID, MgrGuid);
	CLASS_DECLARATION_S(CONT_LIMITED_ITEM_RECORD, ContRecord);	// Data View 역할(DB 부하를 줄이기 위한 데이타)

private:
	// 구현하면 안되는 함수 (사용금지)
	PgItemDropControlImpl(PgItemDropControlImpl const& rhs);
	PgItemDropControlImpl const& operator = (PgItemDropControlImpl const& rhs);
};


class PgItemDropControl : public TWrapper<PgItemDropControlImpl>
{
public:
	PgItemDropControl();
	~PgItemDropControl();

public:
	HRESULT OnDBProcess(CEL::DB_RESULT &rkResult);
	void Timer1m();
	void RequestItemPop(SERVER_IDENTITY const& rkSI, SREQ_GIVE_LIMITED_ITEM const& kReq);
};

#define g_kItemDropControl SINGLETON_STATIC(PgItemDropControl)

#endif // CONTENTS_CONTENTSSERVER_ITEM_ITEMDROPCONTROL_H