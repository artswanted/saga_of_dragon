#ifndef WEAPON_VARIANT_JOBSKILL_PGJOBSKILLTOOL_H
#define WEAPON_VARIANT_JOBSKILL_PGJOBSKILLTOOL_H

namespace JobSkillToolUtil
{
	bool IsCanEquip(PgPlayer* pkPlayer, PgBase_Item const &rkToolItem, int &iErrorRet);//착용 가능한 도구인가?
	bool CheckNeedSkill(int const iJobSkillNo, int const iJobSkillExpertness, PgJobSkillExpertness const &rkJobSkillExpertness);//IsCanEquip에서 사용. 착용 가능 스킬을 배웠는지 체크
	int GetToolType(int const iItemNo);//도구의 타입을 가져옴. 0이면 도구가 아님.
	bool GetToolInfo(int const iItemNo, CONT_DEF_JOBSKILL_TOOL::mapped_type &kOutValue);//도구 정보 가져온다
	int GetToolGatherType(int const iItemNo);
	int GetResultCount(int const iItemNo);//도구로 얻을수 있는 채집물 횟수(갯수가 아님! 이거 만큼 확률 계산을 통해 최종 갯수를 구하게 됨)
	DWORD CalcOptionTurnTime(int const iItemNo, DWORD const dwDurTime);//도구 능력치에 따라 변경되는 truntime을 계산하여 리턴
	int GetUseSkill(PgPlayer * pkPlayer, int const iGatherType, int const iMainJobSkillNo, int const iSubJobSkillNo);//GatherType에 사용가능한 도구가 있는지 확인하여 스킬 번호를 리턴
	int GetUseItem(PgPlayer * pkPlayer, int const iGatherType, int const iUseSkillNo);//사용가능한 툴 아이템 번호를 리턴함
};

#endif // WEAPON_VARIANT_JOBSKILL_PGJOBSKILLTOOL_H