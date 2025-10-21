#ifndef CENTER_CENTERSERVER_CONTENTS_PGMISSION_H
#define CENTER_CENTERSERVER_CONTENTS_PGMISSION_H

#include "Variant/PgMission_Result.h"
#include "Variant/PgMissionMutator.h"
class PgMissionContents
	:	public PgMission
{
public:
	PgMissionContents();
	~PgMissionContents();

	typedef std::multiset<SPlayerRankInfo>						ConPlayRankInfo;

	void Clear();
	SMissionStageKey GetBonusMapSelect(int const iSelectMapCount);
	void SetStageMap(BM::Stream* const pkPacket);
	bool SetMutator(BM::Stream* const pkPacket); // setup mutator for mission
	virtual bool Start( const PgMission_Base& rhs, BM::GUID const &kMissionID, bool const bCopyStage, BM::Stream* const pkPacket);
	virtual bool Restart(const PgMission_Base& rhs, BM::Stream* const pkPacket);

	//HRESULT DoAction_Result();
	HRESULT DoAction_Result_Req(SGroundKey &kGndKey);
	HRESULT DoAction_Result_Res(BM::Stream* const pkPacket);

protected:
	bool CreateRandomStage();
};

#endif // CENTER_CENTERSERVER_CONTENTS_PGMISSION_H