#ifndef GM_GMSERVER_GM_PGPETITIONDATAMGR_H
#define GM_GMSERVER_GM_PGPETITIONDATAMGR_H

class PgPetitionDataMgr
{
private:
	CONT_ALL_PETITION_DATA m_kPetitionData;	
	int	m_iCount;
public:
	void LoadToPetitionData(unsigned short usRealmNo = 0) const; //usRealmNo= 0이면 모든 데이터 로드 
	bool AddPetitionData(unsigned short usRealmNo, CONT_PETITION_DATA const &rkNewData);
	void ReflashAllPetitionData();
	void SendPetitionDataToUser(BM::GUID const &kReqGuid, unsigned short usRealmNo = 0);		//진정 데이터 목록을 GM유저한테 전달
	void UpdatePetitionData(BM::GUID const &kReqGuid, const SPetitionData& UpdateData);
	void BroadcastPetitionData(BM::GUID const &kPetitionGuid, unsigned short usRealmNo = 0);

	//진정 접수 관련 함수들
	HRESULT ReceiptPetition(BM::Stream * const pkPacket);	//진정내용을 DB에 저장
	HRESULT RemainderPetition(unsigned short const usRealmNo, BM::GUID const &MemberGuid); //남은 진정 수 검색
	HRESULT Select_PetitionState(BM::Stream * const pkPacket);	//금일 접수한 진정 중 아직 대기중인 진정이 있는지 확인

	PgPetitionDataMgr(void);
	~PgPetitionDataMgr(void);
protected:
	CLASS_DECLARATION_S(int, iReflashTime);
	CLASS_DECLARATION_S(int, iMaxLoadData);
	CLASS_DECLARATION_S(bool, bCheckReceip);

	mutable Loki::Mutex m_kMutex;
};

#define g_kPetitionDataMgr SINGLETON_STATIC(PgPetitionDataMgr)

#endif // GM_GMSERVER_GM_PGPETITIONDATAMGR_H