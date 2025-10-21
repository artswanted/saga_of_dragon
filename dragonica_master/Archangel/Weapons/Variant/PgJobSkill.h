#ifndef WEAPON_VARIANT_JOBSKILL_PGJOBSKILL_H
#define WEAPON_VARIANT_JOBSKILL_PGJOBSKILL_H

#include "PgPlayer.h"

typedef enum eJobSkillEnum : int
{
	//JSE_LEARN_2ND_MAX = 2,
	JSE_SKILL_TIME_DEVIDE = 1000,
	JSE_SKILL_PROBABILITY_DEVIDE = 10000,
	JSE_EXPERTNESS_DEVIDE = 10000,
	JSE_EXHAUSTION_RATE_DEVIDE = 10000,
	JSE_PROBABILITY_UPRATE_DEVIDE = 1000000,
} EJobSkillEnum;

typedef enum eJobSkillToolType : int
{
	JSTT_AXE      = 1,	//도끼
	JSTT_PICKAX   = 2,  //곡괭이
	JSTT_HOE      = 3,  //호미
	JSTT_ROD      = 4,  //낙시대
	JSTT_ALL	  = 10, //만능도구
	JSTT_CHAINSOW = 11, //전기톱
	JSTT_DRILL    = 12, //드릴
	JSTT_CUTTER   = 13, //예초기
	JSTT_NET      = 14, //그물
	JSTT_ALL_SUB  = 20, //만능 보조 도구
}EJobSkillToolType;

typedef enum eJobSkillMaterialType : int
{
	JSMRT_ERR          = 0,  //Error
	JSMRT_WOOD         = 1,  //목재류
	JSMRT_METAL        = 2,  //금속
	JSMRT_JEWEL        = 3,  //보석
	JSMRT_HERB         = 4,  //약초
	JSMRT_VEGETABLE    = 5,  //채소
	JSMRT_FISH         = 6,  //어류
}EJobSkillMaterialType;

typedef enum eJobSkillLearnResult
{
	JSLR_OK               = 0,   //구매 성공
	JSLR_ALREADY_BUY      = 1,   //이미 구매한 스킬
	JSLR_NOT_CLASS        = 2,   //1차 전직 필요
	JSLR_NEED_SKILL       = 3,   //선행 스킬 필요
	JSLR_NEED_EXPERTNESS  = 4,   //선행스킬 숙련도 필요
	JSLR_FULL_COUNT       = 5,   //배울 수 있는 최대 갯수 초과(2차 스킬 만 적용)
	JSLR_NEED_MONEY       = 6,   //배울 수 있는 최대 갯수 초과(2차 스킬 만 적용)
	JSLR_ERR              = 7,   //기타 에러(널체크, 이상한 스킬 배우려고 한다던지; 서버에서 실패)
}EJOBSKILL_LEARN_RET;

typedef enum eJobSkillMessage : short
{
	JSEC_NONE = 0,
	JSEC_NEED_GUIDE = 1, // 도감이 필요 합니다.
	JSEC_NEED_SKILL = 2, // 스킬이 필요 합니다.
	JSEC_NEED_SKILL_EXPERTNESS = 3, // 스킬 + 숙련도가 필요 합니다.
	JSEC_WRONG_SKILL_LOCATION = 4,	//잘못된[스킬]을 사용하셨습니다. [써야할 스킬]을 사용해야 합니다.
	JSEC_SUCCEED_GET_ITEM = 5,	//[아이템]채집에 성공하였습니다.
	JSEC_NEED_GUIDE_FAIL_LOCATION = 6, // 도감을 배우지 않아 실패 하였습니다.
	JSEC_NEED_CHAR_LEVEL = 7, // 캐릭터 레벨이 낮아 숙련도가 쌓이지 않습니다. 숙련도 [숙련도]이상이 되려면[레벨]이 되어야 합니다.
	JSEC_FULL_EXPERTNESS = 8, //[스킬]기술의 숙련도가 최대치에 도달했습니다.
	JSEC_NEED_USE_RATE_EXHAUSTION = 9, // XX% 이상일 때만 사용 할 수 있습니다.
	JSEC_NEED_USE_RATE_BLESSPOINT = 10, // XX% 이하일 때만 사용 할 수 있습니다.
	JSEC_NEED_USE_RATE_PRODUCTPOINT = 11, // XX% 이하일 때만 사용 할 수 있습니다.
} EJobSkillMessage;

namespace JobSkill_LearnUtil
{
	EJOBSKILL_LEARN_RET IsEnableLearnJobSkill(PgPlayer const* pPlayer, int const iJobSkillNo);	//직업스킬 배울 수 있냐
	int GetHaveJobSkillTypeCount(PgPlayer *pPlayer, EJobSkillType const eType);//같은타입 스킬 갯수 체크 - 주기술/보조기술 로 구분
	int GetHaveJobSkillTypeCount(PgPlayer *pPlayer, int const iJobSkillNo);//같은타입 스킬 갯수 체크 - 주기술/보조기술 로 구분
	int GetHaveJobSkillTypeCount_Detail(PgPlayer *pPlayer, int const iJobSkillNo);//같은타입 스킬 갯수 체크 - 세부항목(기존)
	bool IsEnableUseJobSkill(PgPlayer const *pPlayer, int const iJobSkillNo);	//직업스킬 사용할 수 있냐
	bool IsMainJobSkill(EJobSkillType const eType); //주스킬이냐
};

namespace JobSkill_Util
{
	//직업도구냐
	bool IsJobSkill_Tool(int const iItemNo);
	bool IsUseableJobSkill_Tool(PgPlayer const *pPlayer, int const iItemNo);
	//도감이냐
	bool IsJobSkill_SaveIndex(int const iItemNo);
	bool GetJobSkill_SaveIndex(int const iItemNo,  CONT_DEF_JOBSKILL_SAVEIDX::mapped_type& rkSaveIdx);
	//직업스킬 재료 아이템이냐
	bool IsJobSkill_Item(int const iItemNo);
	bool GetJobSkill_Item(int const iItemNo, CONT_DEF_JOBSKILL_ITEM_UPGRADE::mapped_type &rkItemUpgrade);
	//직업스킬 타입 리턴
	EJobSkillType GetJobSkillType(int const iSkillNo);
	//아이템 재료타입 리턴
	eJobSkillMaterialType GetJobSkillMaterialType(int const iItemNo);
	//현재 스킬의 보조 스킬 리턴
	int GetJobSkill_SubSkill(int const iMainSkillNo);
	//현재 스킬의 메인 스킬 리턴
	int GetJobSkill_MainSkill(int const iSubSkillNo);
};


namespace JobSkill_Third
{
	typedef std::vector<std::pair<int,CONT_PROBABILITY::value_type> > CONT_JS3_RESULT_ITEM;	//first: 확률, second: 아이템번호

	SJobSkillSaveIdx const * const GetJobSkillSaveIdx(int const iSaveIdx);
	HRESULT GetTotalProbability(int const iRecipeItemNo);
	bool UseResItem(int const iDefResourceGroupNo, int const iDefGrade, int const iItemNo, int const iResourceGroupNo, int const iGrade);
	HRESULT CheckNeedSkill(PgPlayer const *pkPlayer, int const iSaveIdx);
	HRESULT CheckNeedItems(PgInventory * const pkInv, int const iRecipeItemNo, CONT_JS3_RESITEM_INFO const& kContResItemInfo);
	HRESULT CheckInvenSize(PgInventory * const pkInv, int const iRecipeItemNo, CONT_JS3_RESITEM_INFO const& kContResItemInfo);
	HRESULT CheckNeedProductPoint(PgPlayer * pkPlayer, int const iRecipeItemNo);
	HRESULT GetResProbabilityItem(PgInventory * const pkInv, int const iItemNo, CONT_JS3_RESITEM_INFO const& kContResItemInfo, CONT_JS3_RESULT_ITEM & rkContItem, bool & rbRemainResultItem);
	HRESULT GetResProbabilityItem(int const iItemNo, int const iResourceProbabilityUp, JobSkill_Third::CONT_JS3_RESULT_ITEM & rkContItem, bool & rbRemainResultItem);
	bool GetJobSkill3_Recipe(int const iItemNo, CONT_DEF_JOBSKILL_RECIPE::mapped_type & rkOutRecipe);
	bool GetContGroupItemNo(int const iGroupNo, int const iGrade, VEC_INT & kContItemNo);

	//재료 성공률 
	int GetResourceProbabilityUp(PgInventory * const pkInv, CONT_JS3_RESITEM_INFO const& kContResItemInfo);
	int GetJobSkill3ProbabilityNo(int const iItemNo);
	int GetJobSkill3NeedProductPoint(int const iItemNo);
	int GetJobSkill3ExpertnessGain(int const iRecipeItemNo);
};

#endif // WEAPON_VARIANT_JOBSKILL_PGJOBSKILL_H