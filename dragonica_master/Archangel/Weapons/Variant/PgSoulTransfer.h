#ifndef WEAPON_VARIANT_JOBSKILL_PGSOULTRANSFER_H
#define WEAPON_VARIANT_JOBSKILL_PGSOULTRANSFER_H

#include "tabledatamanager.h"

typedef enum eSoulTransMsg : int
{
	ESTM_SERVER_ERR							= 0,		//서버 에러 (클라와 서버 정보가 맞지 않는 상황)

	ESTM_NOT_SELECT_EXTRACT_ITEM			= 1,		//영력추출대상 아이템을 선택하지 않았다.
	ESTM_NOT_SELECT_TRANSITION_ITEM			= 2,		//영력보관구슬을 선택하지 않았다.
	ESTM_NOT_HAVE_EXTRACT_ITEM				= 3,		//영력추출기를 가지고 있지 않다
	ESTM_NOT_SELECT_DEST_ITEM				= 4,		//영력삽입대상 아이템을 선택하지 않았다.
	ESTM_CANT_EXTRACT_ITEM					= 5,		//영력추출을 할 수 없는 아이템이다..
	ESTM_NOT_ENOUGH_MONEY					= 6,		//소지금 부족 
	ESTM_NOT_ENOUGH_SOUL					= 7,		//소울 부족
	ESTM_SUCCESS_EXTRACT					= 8,		//추출 성공
	ESTM_SUCCESS_TRANSITION					= 9,		//삽입 성공
	ESTM_FAILED_FIND_EXTRACT_INFO			= 10,		//알맞은 추출 정보를 받아오지 못했다.
	ESTM_FAILED_FIND_TRANSITION_INFO		= 11,		//알맞은 삽입 정보를 받아오지 못했다.
	ESTM_NOT_HAVE_INSURANCE					= 12,		//보험 아이템 없음 
	ESTM_NOT_ENOUGH_RATEUP					= 13,		//확률증가 아이템 갯수 부족
	ESTM_FAILED_TRANSITION_INSURANCE		= 14,		//삽입 실패 - 보험 사용
	ESTM_FAILED_TRANSITION_NOT_INSURANCE	= 15,		//삽입 실패 - 보험 미사용
	ESTM_SELECT_RATEUP_MAX					= 16,		//확률증가아이템 최대치 사용
	ESTM_FAIL_EXTRACT						= 17,		//추출 실패
	ESTM_FAIL_TRANSITION					= 18,		//삽입 실패
	ESTM_NOT_CORRECT_TRANSITION				= 19,		//선택한 영력삽입구슬과 맞지 않는 아이템
	ESTM_INVENTORY_FULL						= 20,		//인벤토리 가득찼다.
	ESTM_NOT_HAVE_RATEUP					= 21,		//확률아이템 없다.

	ESTM_MAX
} ESoulTransMsg;

//영력전이 확률 = 만분률
int const SOUL_TRANSITION_RATE = 10000;

namespace SoulTransitionUtil
{
	//영력 추출/삽입에 적합한 아이템이냐
	bool IsSoulExtractItem(PgBase_Item const &rkItem);
	//영력 삽입에 적합한 아이템이냐
	bool IsSoulInsertItem(PgBase_Item const &rkItem);
	//영력 보관 구슬이 맞냐
	bool IsSoulTransitionItem(int const iItemNo);
	//영력 추출기 정보 받아오기
	bool GetSoulExtractInfo(int const iEquipPos, int const iLevel, CONT_DEF_JOBSKILL_SOUL_EXTRACT::value_type &rkOut);
	//아이템에 맞는 추출정보 받아오기
	bool GetSoulExtractInfo(int const iItemNo, CONT_DEF_JOBSKILL_SOUL_EXTRACT::value_type &rkOut);
	//영력 보관 구슬 생성
	bool CreateSoulTransition(PgBase_Item const &rkItem, PgBase_Item &rkItemOut);
	//영력 삽입 정보 받아오기
	bool GetSoulTransitionInfo(int const iItemNo, int const iRairity, CONT_DEF_JOBSKILL_SOUL_TRANSITION::value_type &rkOut);
	//영력 전이 (원본/목표) - 목표가 없으면 추출, 있으면 삽입
	bool TransitionItem(PgBase_Item &rkOrgItem, PgBase_Item &rkDestItem);
	bool IsTransition(PgBase_Item const &rkTransitionItem, PgBase_Item const &rkDestItem);
}

#endif // WEAPON_VARIANT_JOBSKILL_PGSOULTRANSFER_H