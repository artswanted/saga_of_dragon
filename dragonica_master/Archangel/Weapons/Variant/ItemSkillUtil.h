#ifndef WEAPON_VARIANT_ITEM_ITEMSKILLUTIL_H
#define WEAPON_VARIANT_ITEM_ITEMSKILLUTIL_H

#include "Variant/Inventory.h"
#include "Variant/PgPlayer.h"

namespace ItemSkillUtil
{
	//지금 가진 아이템에 스킬시전 어빌이 있냐
	int GetItemSkillNo(int const iItemNo);
	//이 스킬을 사용할 수 있냐
	bool CheckUseItemSkill(PgPlayer *pkPlayer, int const iSkillNo, EInvType const eInvType);
	bool CheckUseItemSkillAll(PgPlayer *pkPlayer, int const iSkillNo);
	//현재 착용한 아이템에서 사용가능한 모든 스킬시전 번호 받아오기
	bool GetAllItemSkillFromEquipItem(PgPlayer *pkPlayer, std::set<int> &rkContSkillNo, EInvType const eInvType);
}

#endif // WEAPON_VARIANT_ITEM_ITEMSKILLUTIL_H