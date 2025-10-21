#ifndef MAP_MAPSERVER_FRAMEWORK_GLOBAL_H
#define MAP_MAPSERVER_FRAMEWORK_GLOBAL_H

int const iMaxBronzeSilver = 99;
__int64 const iGoldToBronze = 10000i64;
__int64 const iSilverToBronze = 100i64;

int const BLOCK_RATE_EFFECT_NO = 11801;

int const WP_DESTROY_EFFECT_NO	= 300000101;
int const WP_FIRE_EFFECT_NO		= 300000201;
int const WP_ICE_EFFECT_NO		= 300000301;
int const WP_NATURE_EFFECT_NO	= 300000401;
int const WP_CURSE_EFFECT_NO	= 300000501;

int const DF_DESTROY_EFFECT_NO	= 300000601;
int const DF_FIRE_EFFECT_NO		= 300000701;
int const DF_ICE_EFFECT_NO		= 300000801;
int const DF_NATURE_EFFECT_NO	= 300000901;
int const DF_CURSE_EFFECT_NO	= 300001001;

int const DMG_REACTION_EFFECT_NO = 100002201;
int const SHARE_DMG_WITH_SUMMONED_EFFECT_NO=1000;	//소환사 고통분담이 AI소환체에게 적용될때 쓰이는 연출 이펙트
int const MISS_EFFECT_NO = 310301; // 미스 메세지

class PgGround;
class CUnit;
class CEffect;
class PgActionResult;

namespace GlobalHelper
{
	BM::GUID FindOwnerGuid(CUnit const* pkCaster);
	bool IsAdjustSkillEffect(CEffect const* pkEffect);	// 이 이펙트가 스킬 조정 이펙트 인가?
}

typedef enum {
	EFlags_None = 0x0000,
	EFlags_SetGoalPos = 0x0001,			// 실제로 SetGoalPos 도 수행한다.
	EFlags_MinDistance = 0x0002,		// MinDistance 값을 적용한다.
	EFlags_CheckFromGround = 0x0004,	// 바닥으로 Ray를 쏜 값을 기준으로 찾아본다.
} ESetGoalPosFlag;

typedef enum {
	ERange_NONE = 0x0000,
	ERange_OK = 0x0001,
	ERange_X = 0x0002,
	ERange_Y = 0x0004,
	ERange_Z = 0x0008,
	ERange_MIN = 0x0010,
} EInRangeRet;

int const ACTIONID_A_RUN = 100005426;
int const ACTIONID_A_DASH = 100005427;
int const ACTIONID_A_JUMP = 100001001;
int const ACTIONID_A_DASH_JUMP = 100005402;

extern bool FindRandomPos(POINT3& kPos,NxScene* pkScene,float fMinDist,float fMaxDist);
extern float GetDistance(POINT3 const& kPos1, POINT3 const& kPos2);
extern float GetDistanceQ(POINT3 const& kPos1, POINT3 const& kPos2);
extern bool GetPathInfo(NxScene *pkScene, NiNode* pkPathObject, POINT3 const& kPos, POINT3 const& kTarget, NxVec3& kNormal, NxVec3& kTargetVec, float* pfDistance);
extern bool GetPathNormal(NxScene *pkScene, NiNode* pkPathObject, POINT3 const& kPos, NxVec3& kNormal);
extern float GetVectorDot(POINT3 const& rkPos1, POINT3 const& rkMid, POINT3 const& rkPos3);
extern bool IsBetweenRange(POINT3 const& kPos1, POINT3 const& kPos2, int iMinRange, int iMaxRange);
extern EInRangeRet IsInRange(POINT3 const& kPos1, POINT3 const& kPos2, int const iRangeLimit, int const iZLimit, bool const bUseMinRange = true);
extern bool IsInCube( POINT3 const& kCubeMin, POINT3 const &kCubeMax, POINT3 const& kPos );
extern void RefreshGroundQuestInfo(CUnit* pkUnit);
extern void SendQuestInfo(CUnit* pkPlayer, short int sQuestID);
extern void SendNPCQuestInfo(PgPlayer* pkUnit, PgNpc* pkNpc);

extern POINT3 SimulateUnitPos2(PgPlayer* pkPlayer, POINT3 const& ptPos, int iActionID, DWORD dwElapsedTime, DWORD dwLatency, PgGround const* pkGround = NULL);

extern bool RealEqual( const NiPoint3& kLeft, const NiPoint3& kRight );
extern Direction GetCrossDirection( Direction eDirection, bool const bRight );
extern Direction GetRandomDirection( bool const bAll = true );
extern bool GetAxisVector3( NiNode* pkPathObject, POINT3 const &rkPos, Direction eDirection, NxVec3& rkOutAxis  );
extern bool GetPathNormal( NiNode* pkPath, POINT3 const& kPos, NxVec3& kNormal );
extern bool GetDistanceToPosition( NiNode* pkPathObject, POINT3 const &rkPos, Direction eDirection, float const fDistance, POINT3& rkOutPos );
extern bool SetValidGoalPos(NxScene *pkScene, CUnit *pkUnit, POINT3 &rkEndPos, float fHeight = AI_Z_LIMIT, float fMinDistance = 0,
					 DWORD dwFlag = EFlags_SetGoalPos /*ESetGoalPosFlag*/ );

extern void NfyShowQuestDialog(CUnit* pkUnit, BM::GUID const &rkNpcGUID, EQuestShowDialogType const eQuestShowDialogType, int const iQuestID, int const iDialogID);
extern void SendWarnMessage( BM::GUID const &kMemberGuid, int const iMessageNo, BYTE const byLevel );
extern void SendWarnMessage2(BM::GUID const &kMemberGuid, int const iMessageNo, int const iValue, BYTE const byLevel );

extern bool DoFinalDamage(CUnit* pkCaster, CUnit* pkTarget, int const iDamage, int const iSkillNo, SActArg const *pArg, DWORD const dwTimeStamp);
extern bool AdjustFinalDamageAndApply(CUnit* pkCaster, CUnit* pkTarget, PgActionResult* pkAResult, int const iSkillNo, SActArg const *pArg, DWORD const dwTimeStamp);
extern void DoTickDamage(CUnit* pkCaster, CUnit* pkTarget, int const iDamage, int const iFireEffectNo, int const iAddEffectNo, SActArg const *pArg,  bool const bAddEffectImmediately=false);
//extern bool WA_ATTACKDAMAGE_UNIT(int const iMinPow, int const iMaxPow, CUnit* pkMonster, CUnit* pkUnit, PgGround* pkGround, int const iEffectNo, int const iParam = 0, bool const bPushback = false);
extern int OnDamaged(CUnit* pkCaster, CUnit* pkTarget, int const iSkill, int iPower, PgGround* pkGround, DWORD const dwTimeStamp, bool const bReflectDamage = false);
extern HRESULT CheckClientNotifyTime(PgPlayer* pkPlayer, DWORD const dwClientTime, DWORD& dwServerTime);
extern int CalcComboBonusRate(unsigned int const iCombo);

extern CUnit* CheckRecvActionPacket( CUnit* pkReqUnit, SActionInfo const &kActionInfo, PgGround *pkGnd, BM::Stream::DEF_STREAM_TYPE const kPacketType );
extern bool CheckUnitAction( CUnit *pkActionUnit, int const iActionID, PgGround const *pkGnd, BM::Stream::DEF_STREAM_TYPE const kPacketType );
extern float GetMaxActionMoveSpeed(CUnit * const pkUnit, int const iActionID, int const iOldMoveSpeed = 0);
extern bool BackAttackCheck(CUnit* pkCaster, CUnit* pkTarget, PgGround* pkGround);

namespace GuildUtil
{
	bool InitConstantValue();
	__int64 GetHowMuchCreateGuild();
	EGuildCommandRet CheckCreateGuild(CUnit* pkCaster);
}

extern void CheckDayLoopQuestTime(SGroundKey const &rkGndKey, PgPlayer *pkPlayer, BM::PgPackedTime const &rkFailPrevTime, BM::PgPackedTime const &rkNextTime);
extern void CheckWeekLoopQuestTime(SGroundKey const &rkGndKey, PgPlayer *pkPlayer, BM::PgPackedTime const &rkFailPrevTime, BM::PgPackedTime const &rkNextTime);
extern DWORD GetTimeStampFromActArg(SActArg const& rkActArg, wchar_t const* szFunc);
extern bool CheckEnableTrade(PgBase_Item const &kItem,EItemCantModifyEventType const kEventType);

extern PgGround* GetGroundPtr(SActArg const* pkArg);

extern void CalcAwakeValue(CUnit* pkCaster, CSkillDef const* pkSkillDef);

namespace BroadcastUtil
{
	typedef std::map< SERVER_IDENTITY, CONT_GUID_LIST > CONT_BROAD_LIST; // Switch server ID, Member guid
	void AddSwitchInfo(CONT_BROAD_LIST& rkContOut, SERVER_IDENTITY const& rkSwithServer, BM::GUID const& rkMemberGuid);
	void AddSwitchInfo(CONT_BROAD_LIST& rkContOut, CONT_OBSERVER_SWITH_MEMBER_LIST const& rkCont);
	void AddSwitchInfo(CONT_BROAD_LIST& rkContOut, PgPlayer const* pkPlayer, bool const bIgnoreSelf = false);
	void Broadcast(CONT_BROAD_LIST& rkContTarget, BM::Stream const& rkSrcPacket);
};

int const REPLECT_DMG_EFFECTNO = 60401;

namespace DoFinalDamageFuncSubRoutine
{
	// 타격시(때릴때) Effect걸기
	void AddEffectWhenCasterHit(CUnit* pkCaster, CUnit* pkTarget,CAbilObject* pkCheckAbilObj, SActArg* pkArg, int const iAddEffectMax, int const iStartEffectNumAbil, int const iStartRateAbil, int const iStartTargetAbil);
	// 타격시(때릴때) HP,MP 회복
	void RestoreHPMPWhenCasterHit(CUnit* pkCaster, PgGround* pkGround);
	// 피격시(맞을때) HP,MP 회복
	void RestoreHPMPWhenTargetDamage(CUnit* pkCaster, CUnit* pkTarget, PgGround* pkGround);
};

namespace SkillFuncUtil
{
	int OnModifyHP(CUnit* pkTarget, CEffect* pkEffect, int const iMaxHPRate, int const iAddHP, CUnit* pkCaster, PgGround* pkGournd);
	int OnModifyMP(CUnit* pkTarget, CEffect* pkEffect, int const iMaxMPRate, int const iAddMP, CUnit* pkCaster, PgGround* pkGournd);
	void OnAddSkillLinkagePoint(CUnit* pkUnit, CSkillDef const* pkSkill, PgGround* pkGround);
	void SetSkillLinkage_StampMark(CUnit* pkUnit, CSkillDef const* pkSkill, bool const bSetStamp = true);
	void UseSkillLinkagePoint(CUnit* pkUnit, CSkillDef const* pkSkill, PgGround* pkGround);
	int NotifyChangedHPMPDelta(CUnit* pkUnit, CEffect* pkEffect, short const siType, int const iMax, int const iCur, int const iAdd);
	bool DoChildSkillEffect(PgPlayer* pkPlayer, int const iSkillNo, bool bDeleteEffect, SActArg* pkArg); // iSkillNo가 체크해야할 ChildSkill의 effect를 Add하거나 Delete 한다
	void AddEffectWhenHit(CSkillDef const* pkSkill, SActArg const* pkArg, CUnit* pkCaster);
};

void RefrashElemStatusEffect(CUnit* pkUnit, PgGround* pkGround);
void DeleteElemStatusEffect(CUnit* pkUnit);

int GetMonsterBagTunningNo(int const iMin,int const iMax);
#endif // MAP_MAPSERVER_FRAMEWORK_GLOBAL_H