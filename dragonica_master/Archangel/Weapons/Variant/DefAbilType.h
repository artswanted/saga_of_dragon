#ifndef WEAPON_VARIANT_BASIC_CONSTANT_DEFABILTYPE_H
#define WEAPON_VARIANT_BASIC_CONSTANT_DEFABILTYPE_H

//_C_ -> ����
//_E_ -> �߰���(�� �⺻����+ ���� ����)
//_R_ -> õ������(�⺻���� õ������ ������� �� ��)
//...(����) -> �⺻��.

typedef enum eAbilType
{
	AT_NONE=0, 
// Unit Ability
	AT_LEVEL	= 1,
	AT_CLASS	= 2,
	AT_GENDER	= 3,
	AT_RACE		= 4,
	AT_FIVE_ELEMENT_TYPE_AT_BODY= 5,
	AT_SUCCESSRATE = 6,					// ����Ȯ�� ���� ���
	AT_CARD_BGNDNO	= 7,			// ĳ���� ī�� ��� �̹��� ���ҽ� ��ȣ
	AT_BATTLE_LEVEL	= 8,			// ��������
	AT_STR_BASIC = 9,		// Class DEF ��
	AT_STR_ADD = 10,		// �߰��� STR
	AT_STR_ADD_LIMIT = 11,
	AT_INT_BASIC = 12,		// Class DEF ��
	AT_INT_ADD = 13,		// �߰��� STR
	AT_INT_ADD_LIMIT = 14,
	AT_CON_BASIC = 15,		// Class DEF ��
	AT_CON_ADD = 16,		// �߰��� STR
	AT_CON_ADD_LIMIT = 17,
	AT_DEX_BASIC = 18,		// Class DEF ��
	AT_DEX_ADD = 19,		// �߰��� STR
	AT_DEX_ADD_LIMIT = 20,
	AT_STATUS_BONUS			= 21,		// Status Bonus
	AT_ALL_STATUS			= 22,
	AT_START_SET_NO			= 23,

	AT_CLASS_01				= 31,
	AT_CLASS_10				= 40,

	AT_LEVEL_RANK			= 49,
	AT_MILEAGE_TOTAL_CASH	= 50,
	AT_MILEAGE_100			= 51,
	AT_MILEAGE_1000			= 52,
	AT_BONUS_EXP_RATE_INVILLAGE = 53,		// (Ground abil) ������ ������ ��ԵǴ� �޽� ����ġ(���� ���� ����ġ�� ���� %) (�鸸���� ��)
	AT_BONUS_EXP_INTERVAL_INVILLAGE	= 54,	// (Ground abil) ������ ������ ��ԵǴ� �޽� ����ġ�� ��l���ð�(��) (AT_BONUS_EXP_INTERVAL_INVILLAGE second ���� AT_BONUS_EXP_RATE_INVILLAGE ��ŭ �޽İ���ġ�� ����)
	AT_BONUS_EXP_RATE_OFFLINE		= 55,	// (Ground abil) �������� ���� ������ ��ԵǴ� �޽� ����ġ (�鸸���� ��)
	AT_BONUS_EXP_INTERVAL_OFFLINE	= 56,	// (Ground abil) �������� ���� ������ ��ԵǴ� �޽� ����ġ�� ���� �ð�(��)

	AT_EVENT_ITEM_CONTAINER = 57,// �̺�Ʈ �Ⱓ�� ����� ������ �����̳� ��ȣ

	AT_MAP_GROUP_NAME_NO	= 58,// �� �׷� ���̹� ��Ʈ�� ��ȣ ( ��) �̼�1, �̼�2, �̼�3 -> ��ǥ�̼Ǹ� )
	AT_EFFECT_GROUP_NO		= 59,	// Effect �ߺ��Ǵ°� ���� �ϱ� ���� Group number
	AT_SUCCESSIONAL_DEATH_COUNT		= 60,	// �����ؼ� ���� ī��Ʈ (��Ʋ������ ������ ī��Ʈ �ǰ� ����)
	AT_CHAT_BROADCAST_TYPE	= 61,	// ä�ø޽����� broadcasting �Ǵ� ��� (0=zone�� ����, 1=ground ��ü)

	AT_GMCMD_LIMIT_HP		= 70,	// ������ HP���� ������ �������� �ʵ���

	AT_HPMP_BROADCAST		= 100,	// �� ����� ������ HP�� MP�� �׶��忡 ��ε�ĳ��Ʈ ���־�� �Ѵ�.
	AT_HP					= 101,
	//AT_DP					= 102,	// DefencePoint Shiled
	AT_CP					= 103,	// ClassPoint(CP)
	AT_MP					= 105,
	AT_REFRESH_ABIL_INV		= 106,	// Abil�� �ƴϰ� Abil�� �ٽ� ���Ǿ�� �Ѵٰ� �˸��� ������ ���ȴ�.(Inven ����� ��������)
	AT_EVENT_LEVELUP		= 107,	// Abil�� �ƴϰ� Levelup�� �Ǿ����� Abil�� �ٽ� ����϶�� �˸��� �뵵
	AT_HIT_COUNT			= 108,	// ������ ������ ȸ�� (����:DoubleAttack�� ���ؼ� Hit���� ���������� ����� ���� �ִ�.)
	AT_HP_RESERVED			= 109,	// ��Ȱ�Ҷ��� ä������ �� HP��
	AT_MP_RESERVED			= 110,	// ��Ȱ�Ҷ��� ä������ �� MP��
	AT_REVIVED_BY_OTHER		= 111,	// �ٸ� ����� ���� ��Ȱ�Ǿ��°�?
	AT_COOLTIME_TYPE		= 112,	// Item CoolTime (EItemCoolTimeType value)
	AT_MUTE					= 113,	// ä�ñ���
	AT_RECENT				= 114,	// ����Ʈ ��
	AT_LAST_VILLAGE			= 115,	// ������ ����
	//AT_HP_RECOVERY_ADD		= 116,	// Auto HPȸ���� �߰��Ǵ� ���밪
	//AT_MP_RECOVERY_ADD		= 117,	// Auto HPȸ���� �߰��Ǵ� ���밪
	AT_USE_ITEM_GROUP		= 118,	// ������ ��� �׸�(��Ÿ�� ���� �׸�)
	//AT_HP_RECOVERY_ADD_RATE = 119,	// AT_HP_RECOVERY_ADD ���� Multiflier ������ ��
	//AT_MP_RECOVERY_ADD_RATE = 120,	// AT_MP_RECOVERY_ADD ���� Multiflier ������ ��

	AT_DEC_TARGETDODGE_ABS	= 122,	// Target�� ȸ���� ����(���밪 ���� '500'�� ���̶�� ȸ���� ���������� 500�� ����)
	AT_DEC_TARGETDODGE_RATE = 123,	// Target�� ȸ���� ����(Rate ���� '1000'�� ���̶�� ȸ���� ���������� 10%����)
	AT_OFFENCE_ENCHANT_LEVEL = 124,	// �÷��̾� ���� ��æƮ ����(���� ��þƮ ���� �״�� ���)
	AT_DEFENCE_ENCHANT_LEVEL= 125,	// �÷��̾� �� ��æƮ ����(�������� ���)

	AT_BONUS_EXP_RATE_EFFECT= 126,	// ����Ʈ���� ��Ե� �޽� ����ġ 

	AT_BONUS_EXP_LEVELLIMIT	= 127,	// �ش� ���� �̻��� ����ġ�� ȹ�� �Ұ���

	AT_MAX_EVADERATE_VALUE		= 140,  //�ִ� ȸ�Ǽ�����ġ
	AT_MAX_BLOCKRATE_VALUE		= 141,	//�ִ� ���� ������ġ
	AT_CRITICALRATE_VALUE		= 142,	//�ִ�ũ��Ƽ�ü�����ġ
	AT_CRITICAL_DAMAGE_VALUE    = 143,	//�ִ�ũ��Ƽ�õ�����
	AT_HIT_VALUE_CONSTANT		= 144,	//���߼�ġ���
	AT_EVADE_VALUE_CONSTANT		= 145,	//ȸ�Ǽ�ġ���
	AT_CRITICALRATE_VALUE_CONSTANT = 146,	//ũ��Ƽ�ü�����ġ���

	/* 150 - 160 reserved for Stat track*/
	AT_STAT_TRACK = 150,					// Stat track main abil mean this item has stat track
	AT_STAT_TRACK_KILL_COUNT_MON = 151,		// Stat track monster kill count storage
	AT_STAT_TRACK_KILL_COUNT_PLAYER = 152,	// Stat track player kill count storage

	AT_ATTR_ATTACK			= 201,
	AT_ATTR_DEFENCE			= 202,
	AT_LOCK_HIDDEN_ATTACK	= 203,	// Hidden�����ε� Attack �ϴ��� Hidden ���°� Ǯ���� �ʴ´�.
	AT_UNLOCK_HIDDEN_MOVE	= 204,	// Hidden�����ε� Move �ϸ� Hidden���°� Ǯ����.
	AT_GOALPOS_SET_TIME		= 205,	// AI : GoalPos() ���� �ֱ������� Update ������� �� ��찡 �ִµ� �̰��� ���� GoalPos ����� �ð�(DWORD�� Ÿ�Ժ��� �ʿ�)
	AT_BOLDNESS_RATE		= 206,	// AI : �󸶳� ����Ѱ�..�� Target�� �߰����� �� �����Ϸ� �� Ȯ��(�⺻ 60%)
	AT_PARENT_ITEM_NO		= 207,	// Item : Parent Item No (��ɼ� �����ۿ��� ȿ���� �ߺ����� �ʵ��� �ϱ� ���ؼ� �˻��ϴ� ItemNo)
	AT_DURATION_TIME		= 208,	// Item : ��� �������� ȿ���� �����ϴ� �ð�
	AT_DURATION_TIME_TYPE	= 209,	// Item : AT_DURATION_TIME �� ���� (E_TIME_ELAPSED_TYPE)
	AT_SKILL_FILTER_TYPE	= 210,	// Effect : ��ų ����� ���� �ϱ� ���� Filter type (ESkillFilterType)
	AT_FILTER_EXCEPT_01		= 211,	// Effect : Skill Filter ���� �����׸� ����Ʈ
// ���� : AT_FILTER_EXCEPT_01 - AT_FILTER_EXCEPT_10
	AT_NEED_EFFECT_01		= 221,	// Skill : ��ų�� ����ϱ� ���ؼ� �ʿ��� Effect
// ���� : AT_NEED_EFFECT_01 - AT_NEED_EFFECT_10
	AT_NEED_SKILL_01		= 231,	// Skill : ��ų�� ���� ���� �̸� ����� �ϴ� ��ųID
// ���� : AT_NEED_SKILL_01 - AT_NEED_SKILL_10
	AT_NEED_USE_SKILL_01	= 241,	// Skill : ��ų�� ����ϱ� ���� ���Ǿ�� �ϴ� ��ų
	AT_CHILD_CAST_SKILL_NO = 250,	// Fire�� Cast�� �������� ��ų�� ��� Cast�� ����Ǵ� Fire ��ų�� No�� �Է��Ѵ�.(���� ���Ǵ� ��ų ��ȣ) - UI �������� ���
	AT_PARENT_CAST_SKILL_NO = 251,	// Fire�� Cast�� �������� ��ų�� ��� Fire�� ����Ǵ� Cast ��ų�� No�� �Է��Ѵ�.
	AT_I_PHY_DEFENCE_ADD_RATE	= 252,	// Item : �ش�Item�� AT_PHY_DEFENCE ���� ���ؼ��� ����ġ Rate �� (���������۸� ����)
	AT_I_MAGIC_DEFENCE_ADD_RATE	= 253,	// Item : �ش�Item�� AT_MAGIC_DEFENCE ���� ���ؼ���  ����ġ Rate �� (���� �����۸� ����)
	AT_PROJECTILE_SPEED_RATE	= 254,	// Client : ��ų�߻�ü�� �ӵ� ������(������, 10000 �̻��̸� �ӵ� ����) �⺻�ӵ��� Client �߻�ü XML�� ����.
	AT_I_PHY_ATTACK_ADD_RATE	= 255,	// Item : Inventory�� AT_PHY_ATTACK_MIN/AT_PHY_ATTACK_MAX ���� Rate������ ����
	AT_I_MAGIC_ATTACK_ADD_RATE	= 256,	// Item : Inventory�� AT_MAGIC_ATTACK_MIN/AT_MAGIC_ATTACK_MAX ���� Rate������ ����
	AT_INVEN_PHY_DEFENCE_ADD_RATE	= 257,	// Item : Inventory�� AT_PHY_DEFENCE ���� Rate������ ���� (������ ���� ����)
	AT_INVEN_MAGIC_DEFENCE_ADD_RATE	= 258,	// Item : Inventory�� AT_MAGIC_DEFENCE ���� Rate������ ���� (������ ���� ����)
	AT_UNIT_SIZE_XY	= 259,		// Unit Size (X,Y ũ��)Attack Range ��� �Ҷ�, Unit�� ũ�⸦ ���� �� �־�� �� ���� �ִ�. (Target�� Boss�� ���� Ŭ ���)
	AT_UNIT_SIZE_Z = 260,		// Unit Size (Z ũ��)Attack Range ��� �Ҷ�, Unit�� ũ�⸦ ���� �� �־�� �� ���� �ִ�. (Target�� Boss�� ���� Ŭ ���)
	AT_CREATE_SKILLACTOR_CNT		= 261,	// ��ų ���͸� ���� ����. �� 0 �̸� 1�� ������.
	AT_REST_EXP_ADD_MAX				= 262,	// �޽� ����ġ ���ʽ� ���� �� �ִ� �ִ� ����ġ �� (64bit)
	AT_DIE_TYPE						= 263,	// ������ �ɼ�
	AT_AMP_PHY_DEFENCE_ADD_RATE		= 264,
	AT_AMP_MAGIC_DEFENCE_ADD_RATE	= 265,

	AT_JOINT_COOLTIME_SKILL_NO_1	= 270,	// ��Ÿ���� �Բ� ������ ��ų ��ȣ�� AT_JOINT_COOLTIME_SKILL_NO - 270 ~ 280
	AT_JOINT_COOLTIME_SKILL_NO_2	= 271,
	AT_JOINT_COOLTIME_SKILL_NO_3	= 272,
	AT_JOINT_COOLTIME_SKILL_NO_4	= 273,
	AT_JOINT_COOLTIME_SKILL_NO_5	= 274,
	AT_JOINT_COOLTIME_SKILL_NO_6	= 275,
	AT_JOINT_COOLTIME_SKILL_NO_7	= 276,
	AT_JOINT_COOLTIME_SKILL_NO_8	= 277,
	AT_JOINT_COOLTIME_SKILL_NO_9	= 278,
	AT_JOINT_COOLTIME_SKILL_NO_10	= 279,
	AT_JOINT_COOLTIME_SKILL_NO_MAX	= 280,
	
	AT_JOINT_COOLTIME		= 281,			// ��Ÿ���� �Բ� �����Ҷ� ����� ��ų�� ��Ÿ���� �ƴ�, �� ���� ��Ÿ���� �����Ҷ�

	AT_SAVE_DB_SKILL_COOLTIME = 282,		//�� ����� ������ ��ų�� ĳ���� �α׾ƿ��� ��, ���̺��� ����ȴ�.

	AT_CHARACTOR_SLOT = 283,				//ĳ������ ĳ��â ���� ��ġ

	AT_FILTER_EXCEPT_EX_01	= 300,
// ���� : Ȯ��! AT_FILTER_EXCEPT_EX_01 - AT_FILTER_EXCEPT_100, 300 ~ 399

	AT_NOT_MOVE = 400,			//�̵� �Ұ�
	AT_CALLER_LEVEL			= 501,	// Caller's Level
	AT_EXCEPT_GROUND_TYPE	= 502,	// EGndAttr value
	AT_CALLER_TYPE			= 503,	// Caller's Unit Type(IsTarget���� ����ؾ� �Ѵ�.)
	AT_HANDYCAP				= 504,
	AT_TEAM					= 505,
	AT_DELAY				= 506,
	AT_AI_TYPE				= 507,
	AT_DISTANCE				= 508,
	AT_NOTAPPLY_MAPATTR		= 509,	// ��� �Ұ����� �׶��� �Ӽ�(ex ��Ż�������� �̼ǿ��� ��� ���ؾ� �ϴϱ�...)
	AT_BATTLEAREA_INDEX		= 510,	// ���� ���� BattleArea
	AT_EFFECT_SIZE			= 511,
	AT_EXPERIENCE			= 512,
	AT_MONEY				= 513,
	AT_GENERATE_STYLE		= 514,
	AT_DISTANCE2			= 515, // Entity ��ȯ�Ҷ� ĳ������ ������ �������� �󸶸�ŭ �̰ݵ� ��ġ���� ��ȯ �Ұ��ΰ�
	AT_VOLUNTARILY_DIE		= 516,	//���Ƿ� ���� ����� ��Ÿ��
	AT_DIE_MOTION			= 517,	// ���� Die ��� ������ üũ
	AT_DAMAGE_PUSH_TYPE		= 518,	// Pushback by damage type
	AT_DEATH_EXP_PENALTY	= 519,	// Player �׾��� �� ���̴� ����ġ (Percentage)
	AT_LIFETIME				= 520,	// (DummyPlayer) Life time
	AT_SKILL_EXTENDED		= 521,	// ��ų�� ������ �޾Ƽ� ���� �������� ����(�߻�ü ���� ��ȭ)
	AT_DMG_DELAY_TYPE		= 522,	// Damage �޾��� �� Delay���� ��� ���� ���ΰ�? (����Skill_Abil�� ���)(0=unit, 1=skill, 2=unit+skill)
	AT_MAX_HP_EXPAND_RATE	= 523,	// Class���� MaxHP ����ġ(������)
	AT_MAX_MP_EXPAND_RATE	= 524,	// Class���� MaxMP ����ġ(������)
	AT_DICE_VALUE			= 525,	// ����:Roulette ��ų �����(�ֻ�����)
	AT_REFLECTED_DAMAGE		= 526,	// Reflected Damage (Damage�ݻ�) ũ��
	AT_1ST_ATTACK_ADDED_RATE	= 527,	// ù��° ���ݿ� ���ؼ��� �߰����� ����ġ(�������ݷ¿� ���� ������)
	AT_INDEX				= 528,	// �׳� Index������ ���� ���ڴ�.
	AT_PDMG_DEC_RATE_2ND	= 529,	// ����Damage�� ���ؼ� 2�������� Damage ���� Rate (�������� ���)
	AT_MDMG_DEC_RATE_2ND	= 530,	// ����Damage�� ���ؼ� 2�������� Damage ���� Rate (�������� ���)
	AT_APPLY_ATTACK_SPEED	= 531,	// ��ų : �� ��ų�� ���ݼӵ��� ������ ���� ���ΰ�(0:�ȹ޴´�, 1:�޴´�)
	AT_CLAW_DBL_ATTK_RATE	= 532,	// ��ų : Claw ��Ÿ �޼� ���ݽÿ� �������� �ߵ� Ȯ��(������)
	AT_ADD_MONEY_PER_LV		= 533,	// Monster Level�� ���� �߰����� �߰����� Money Rate��(������)
									// Unit : ����(�Ҹ�ġ��) ��ų�� ���� ������ �߰����� Money�� �����ؾ� �Ѵ�.
	AT_ADD_MONEY_RATE		= 534,	// �߰����� Money Drop�� �߻��� Ȯ��
	AT_MAP_NUM				= 535,	// ������ : ������ȯ ��ũ���� ��� ������ ���� Map��ȣ
	AT_PORTAL_NUM			= 536,	// ������ : ������ȯ ��ũ���� ��� ������ Portal ��ȣ
	AT_POSITION_X			= 537,	// ������ : ������ȯ ��ũ���� ��� ������ ��ǥ��(X)
	AT_POSITION_Y			= 538,	// ������ : ������ȯ ��ũ���� ��� ������ ��ǥ��(Y)
	AT_POSITION_Z			= 539,	// ������ : ������ȯ ��ũ���� ��� ������ ��ǥ��(Z)
	AT_PARENT_SKILL_NUM		= 540,	// Skill : Parent Skill number
	AT_1ST_AREA_TYPE		= 541,	// Skill : �����Ÿ� : Target�� ������, � ������ Target Area�� ������ ���ΰ�?(ESkillArea) SkillFire �Ҷ� Target�� ������� ��
	AT_2ND_AREA_TYPE		= 542,	// Skill : ȿ���Ÿ� : Target�� ������, � ������ Target Area�� ������ ���ΰ�?(ESkillArea) ������ Damage�� �� Target�� �����ϱ� ���� �Է��ϴ� �� AT_2ND_AREA_PARAM1 �� �Էµ��� ������ Skill:AT_ATTACK_RANGE �� ��� 
	AT_1ST_AREA_PARAM1		= 543,	// Skill : �����Ÿ� : NOT USE(Skill:AT_ATTACK_RANGE ���� ����϶�. �ǹ� ������ü()
	AT_2ND_AREA_PARAM1		= 544,	// Skill : ȿ���Ÿ� : 
	AT_1ST_AREA_PARAM2		= 545,	// Skill : �����Ÿ� : ������ü(��)/��(������)/����(����)
	AT_2ND_AREA_PARAM2		= 546,	// Skill : ȿ���Ÿ� : ������ü(��)/��(������)/����(����)
	AT_1ST_AREA_PARAM3		= 547,	// Skill : �����Ÿ� : ����
	AT_2ND_AREA_PARAM3		= 548,	// Skill : ȿ���Ÿ� : ����
	AT_1ST_AREA_PARAM4		= 549,	// Skill : �����Ÿ� : ����
	AT_2ND_AREA_PARAM4		= 550,	// Skill : ȿ���Ÿ� : ����
	AT_GLOBAL_COOLTIME		= 551,	// Skill : ��ų��� �Ŀ��� � ��ų�� �ش� �ð����� ��� ����.
	AT_REFLECT_DMG_HP		= 552,	// Refeclted Damage �߻�������, �������� ���� HP ��
	//AT_ABS_ADDED_DMG		= 553,	// Unit : ������ ��, ����Dmg�� �߰������� �������� Dmg
	AT_DMG_DEC_RATE_COUNT	= 554,	// AT_PDMG_DEC_RATE_2ND or AT_MDMG_DEC_RATE_2ND �ɷ� ���� ���� ����(-1:Unlimited)
	AT_AMOUNT				= 555,	// ������ ���
	AT_COLLISION_SKILL		= 556,	// ���� �浹 �Ǿ��� �� ������ �� ��ų ��ȣ(<>0 �̸� Damage�� �־�� �ϸ�, Abil���� Skill�� ���)
	AT_TEMPLATE_SKILLNO		= 557,	// ��ų : ��ų �Լ� ȣ�� �ɶ�, �� ������� ��ų�Լ��� ȣ���ϵ��� �� �ش�.
	AT_DEFAULT_DURATION		= 558,	// Item : ������ ������ �����ÿ� �ʱ⼳���Ǵ� ��������(0�̸� AT_DEFAULT_AMOUNT ���� ����)
	AT_DIE_ANI_PLAYTIME		= 559,	// Monster : ������ �״� Ani �÷��� �ð�
	AT_CASTER_STATE			= 560,	// Skill : ��ų �������� ���� ���� (ECaster_State)
	AT_FIRE_TYPE			= 561,	// Skill : Skill Fire ����(���� Damage�� �� ���ΰ� ���� ����) ESkillFireType
	AT_EXCEPT_GRADE			= 562,	// Effect : AddEffect ���� ���� UnitType ����
	AT_MAX_LEVELLIMIT		= 563,	// Item��� ������������ (�̰����� ���� ������ ���� �Ұ�)
	AT_EQUIP_MESSAGE		= 564,	// Item : �������� ��, ������ �ȳ� �޽���(TextTable ID)
	AT_MONEY_MIN			= 565,	// Monster : Drop�Ǵ� Money �ּҰ� (GetAbil(AT_MONEY_MIN) ~ GetAbil(AT_MONEY) �� Rand ������ ������)
	AT_PROWLAREA_RANGE		= 566,	// Monster : Target �߰����� ��, ������ �ִܰŸ��� �� �Ÿ����� �ٰ�����
	AT_FRAN					= 567,  // Fran ����ġ ����
	AT_TACTICS_LEVEL		= 568,	// �뺴 ����
	AT_TACTICS_EXP			= 569,	// �뺴 ����ġ
	AT_CASH					= 570,	// ĳ��
	AT_REVERSION_ITEM		= 571,	// 
	AT_R_FRAN				= 572,	// ���� ȹ�淮 ����
	AT_WARP_SKILL_NO 		= 575,	// �����̵� ��ų��ȣ
	AT_COLLISION_SKILL_FIRE	= 576,	// �浹 �������� ������ ��ų�� ����� ���ΰ�. ��ų���.
	AT_COLLISION_USE_FORCE	= 577,	// Ŭ���̾�Ʈ CheckTouchDmg �Լ����� IsIgnoreEffect(ACTIONEFFECT_DMG) �ɼ��� �����ϰ� �Լ��� ������ �ߵ� �ǵ��� ��. Ŭ���� ���
	AT_CLAW_DBL_ATTK_DMG_RATE	= 579,	// ��ų : �������� �ߵ��� ������ %(������)
	AT_CHILD_SKILL_NUM_01	= 580,	// ��ų ���� ���� �޴� �ڽ� ��ų�� (Value = Child Skill�� ���� 2�̻� �� ��� -> Base Skill No �Է�)
	AT_CHILD_SKILL_NUM_02	= 581,	// ��ų ���� ���� �޴� �ڽ� ��ų��
	AT_CHILD_SKILL_NUM_03	= 582,	// ��ų ���� ���� �޴� �ڽ� ��ų��
	AT_CHILD_SKILL_NUM_04	= 583,	// ��ų ���� ���� �޴� �ڽ� ��ų��
	AT_CHILD_SKILL_NUM_05	= 584,	// ��ų ���� ���� �޴� �ڽ� ��ų��
	AT_CHILD_SKILL_NUM_06	= 585,	// ��ų ���� ���� �޴� �ڽ� ��ų��
	AT_CHILD_SKILL_NUM_07	= 586,	// ��ų ���� ���� �޴� �ڽ� ��ų��
	AT_CHILD_SKILL_NUM_08	= 587,	// ��ų ���� ���� �޴� �ڽ� ��ų��
	AT_CHILD_SKILL_NUM_09	= 588,	// ��ų ���� ���� �޴� �ڽ� ��ų��
	AT_CHILD_SKILL_NUM_10	= 589,	// ��ų ���� ���� �޴� �ڽ� ��ų��

	AT_PLAY_CONTENTS_TYPE	= 590,	// ������ Ÿ��(EPlayContentsType)
	AT_SKILL_BACK_DISTANCE	= 591,	//Ÿ���� ���� �� �ڷ� �ణ �� �� �Ÿ�
	AT_SAVE_DB				= 592,	// SaveDB �� ���ΰ�? (Effect/Item(Group))

	AT_ADD_ITEMDROP_RATE	= 593,	// ������ ��� Ȯ�� ���(�̼� �� �̵��ÿ� ����ȴ�. �̹� �� �ִ� �ʿ��� ������� �ʴ´�.)
	AT_ADD_ONCE_ITEMDROP_RATE = 594, //������ ��� �ѹ� �� �� Ȯ��

	AT_USE_INSURANCE_ITEM_NO		= 596,	// ������ī��� �ż���ī�忡�� ���� ������ ���� ������ ��ȣ
	AT_OLD_MONSTERCARD		= 597,	// ������ī��� �ż���ī���� ������ ���� ���(������ī�忡�� �� ����� ����Ѵ�.)

	AT_SOCKETCARD_INSERT	= 598,	// ����ī�� ���� ����Ȯ��

	AT_MONSTER_CARD_ORDER	= 599,  // ���� ī�� ��ȣ(�ε���)

	AT_BONUS_CASH			= 600,	// ĳ�� ���ϸ���

	AT_MONSTER_CARD_NO		= 601,	// ���� ī�� ��ȣ

	AT_ENABLE_USE_GROUPNO	= 602,	// ��� ������ �׶��� ��ȣ

	AT_HOME_SIDEJOB			= 603,	// ����Ȩ �Ƹ�����Ʈ ��Ȳ
	
	AT_VERTICAL_DISTANCE	= 608,	// AT_CREATE_SKILLACTOR_CNT�� 2�̻��϶�, ��ų ���͸� ������ ��ġ����(�� �Ʒ��� �̰ݽ�ų �Ÿ�)
	AT_CAN_GROUND_ATTR		= 609,	// ��� ������ �� �Ӽ�

	AT_CHILD_EFFECT_NUM_01	= 610,	// ����Ʈ�� Add �� �� �߰� ����� �����ؾ��ϴ� �ڽ� ����Ʈ (Value = Child Effect�� ���� 2�̻� �� ��� -> Base Effect No �Է�)
	AT_CHILD_EFFECT_NUM_02	= 611,	// ����Ʈ�� Add �� �� �߰� ����� �����ؾ��ϴ� �ڽ� ����Ʈ (AddEffect�� ó������ �ʰ� ����� �����ͼ� ���� �ְ� ���ش�.)
	AT_CHILD_EFFECT_NUM_03	= 612,	// ����Ʈ�� Add �� �� �߰� ����� �����ؾ��ϴ� �ڽ� ����Ʈ
	AT_CHILD_EFFECT_NUM_04	= 613,	// ����Ʈ�� Add �� �� �߰� ����� �����ؾ��ϴ� �ڽ� ����Ʈ
	AT_CHILD_EFFECT_NUM_05	= 614,	// ����Ʈ�� Add �� �� �߰� ����� �����ؾ��ϴ� �ڽ� ����Ʈ
	AT_CHILD_EFFECT_NUM_06	= 615,	// ����Ʈ�� Add �� �� �߰� ����� �����ؾ��ϴ� �ڽ� ����Ʈ
	AT_CHILD_EFFECT_NUM_07	= 616,	// ����Ʈ�� Add �� �� �߰� ����� �����ؾ��ϴ� �ڽ� ����Ʈ
	AT_CHILD_EFFECT_NUM_08	= 617,	// ����Ʈ�� Add �� �� �߰� ����� �����ؾ��ϴ� �ڽ� ����Ʈ
	AT_CHILD_EFFECT_NUM_09	= 618,	// ����Ʈ�� Add �� �� �߰� ����� �����ؾ��ϴ� �ڽ� ����Ʈ
	AT_CHILD_EFFECT_NUM_10	= 619,	// ����Ʈ�� Add �� �� �߰� ����� �����ؾ��ϴ� �ڽ� ����Ʈ
	
	AT_1ST_ATTACK_ADDED_RATE_APPLY_NUM = 627,	// �� ���� AT_1ST_ATTACK_ADDED_RATE(527) ��� ���� Ƚ���� �ȴ�. 0�� ���� AT_1ST_ATTACK_ADDED_RATE�� ���� �Ѵٸ� 1Ÿ�� ����ȴ�

	AT_SOCET_CARD_EXTRACTION_CASH_ITEM = 628, //���� ī�� ����� �ʿ��� ĳ�� ������ �ѹ�
	AT_SOCET_CARD_EXTRACTION_CASH_ITEM_COUNT = 629, //���� ī�� ����� �ʿ��� ĳ�� ������ ����
	AT_SOCET_CARD_EXTRACTION_ITEM_NAME = 630, //���� īƮ ����� �ʿ��� InGame ������ �ѹ�
	AT_SOCET_CARD_EXTRACTION_ITEM_COUNT = 631, //���� īƮ ����� �ʿ��� InGame ������ ����

	AT_IGNORE_MONSTER_REGEN_COUNT = 649,	//World XML���� ������ �ִ� ���� ���ͼ��� ������ �� �ִ� ���
	AT_IGNORE_MACRO_COUNT	= 650,	//��ũ�� üũ ������ ����ġ ȹ�� ī��Ʈ
	AT_CHAT_BALLOOON_TYPE	= 651,	// ä�� ��ǳ�� Ÿ�� ���
	
	AT_SUB_PLAYER_ACTION	= 660, // �ֵ��� ����ĳ���Ϳ� ���� Actor�� ���Ǵ� �׼��϶�

	AT_RESULT_WAIT_TIME		= 700, //������, ���� ���â ���ö� ������ �ð�
	AT_NO_KILLCOUNT_MAP		= 701,	//killcount�ý����� ������� �ʴ� ��. �ʾ����

	AT_SKILL_EFFICIENCY		= 800,	// ��ų�� ȿ��
	AT_AUTO_PET_SKILL		= 810,	// �� AI ����� ���� ���
	AT_CALLER_IS_PET		= 811,	// �� ��ƼƼ�� ��ȯ�ڴ� ��(Caller�� Player�� ���� �� �ۿ� ��� �ٸ� ������� ǥ��)
	AT_CALLER_DELETE		= 812,	// Ŭ������: ��ƼƼ�� ����� Caller ����(������ ��ƼƼ ��ų ���� Caller�� �ִ� ��� ���������� �ִϸ��̼��� �������� �ʴ� ���������� Caller�� �����ؾ���)
	AT_OWNER_TYPE			= 813,	// Caller�� �ֻ��� Owner UnitType
	AT_LIFETIME_DELAY		= 814,
	AT_MOUNTED_PET_SKILL	= 815,	//�꿡 ž���ߴ����� ��Ÿ��.
	AT_TRANS_DC_PET_SKILL	= 816,	//�����̵� ��� ���� �ۼ�Ƽ��
	AT_REG_ADDUNIT_GUID_TYPE	= 817, //Ŭ�� AddUnit�ɶ� GUID �����Ű��, lua - TBL_REQ_ADD_UNIT_GUID�� ���ǵ�
	AT_PET_SKILL_FOR_AI		= 818, //DefSkillAbil_Pet�� �� ���� 1�� ������ AI�� ���� ��ų���� �ǹ��Ѵ�.
	//(AI�� ���Ǵ� ��ų ���� ������ ��Ȯ�ϰ� �ϱ� ���� ���. ��ų�� SkillType �Ǵ� TargetType������ �����ϱ�� ��ȣ�ϴ�.)
	AT_PET_FREEUPGRADE		= 819, //�� ������ ���� ���� ����� ����� �� ���ΰ�?
	AT_PET_CANNOT_RIDING_ATTACK	= 820, //�� ����� 0�� �ƴϸ� ���� ž�°��ݽ� Ÿ���� ���� �ʴ´�.

	AT_MONSTER_FIRST_TARGET	= 900,	// ���Ͱ� Ÿ���� ���� ���� Ÿ���� ����
	AT_TARGET_SELECT_TYPE   = 901,  // ���� Ÿ�� ����(E_TARGETTING_TYPE)
	AT_TARGET_SELECT_VALUE  = 902,  // 0:��������(ū>��), 1:��������(��>ū)

	AT_INDUN_PARTICIPATION_RATE	= 998,	//�δ� ������
	AT_MONSTER_KILL_COUNT	= 999,	// ���� ų ī��Ʈ
	AT_LAST_ATTACK_DAMAGE_RATE	= 1000, //������ ������ ����Ҷ� ����� Ȯ��
	AT_SP					= 1001,
	AT_HIDE					= 1002, // 0: ĳ���͸� �׸�   1: ĳ���͸� �׸��� ����(������ ���ܹ���)
	AT_EVENTSCRIPT_HIDE		= 1003, // �̺�Ʈ ��ũ��Ʈ�� ���� ���⿡�� 0: ĳ���͸� �׸�   1: ĳ���͸� �׸��� ����(������ ���ܹ���)
	AT_ADD_EXP_PER			= 1004, // Player�� ��� Exp��(�ߺ� �Ұ��� ���)2009.09.04 ����(50% �̸� 50 �Է��ؾ� ��) <-- (������)100�� 100%�� 1.9��� 190���� ����
	AT_ADD_MONEY_PER		= 1005,
	AT_MAX_TARGETNUM		= 1007,
	AT_SKILLPOWER_RATE		= 1008,	// Skill�� ���ݷ� Rate
	AT_SKILL_MIN_RANGE		= 1009,
	AT_MON_MIN_RANGE		= 1010, // ���Ͱ� PC�� �����ϴ� �ּ� �Ÿ�
	AT_MON_DROPITEM_ENFORCE	= 1011,	// ���Ͱ� �������� �����ϰ� ������ ������ ���
	AT_ADD_EXP_CAN_DUPLICATE= 1012,	// Player�� ��� Exp��(�ߺ������� ���)
	AT_ADDEXP_FINAL			= 1013,	// Player �� ���������� ��� Exp ������(�⺻=100) (AT_ADD_EXP_PER+AT_ADD_EXP_CAN_DUPLICATE)
	AT_MON_DROP_ALLITEM		= 1014, // ���Ͱ� �������� ��� ���

	AT_USED_SP				= 1015, // ����� SP.
	AT_MAX_LOOTNUM			= 1016, // �ִ� �ݱ� ����

	AT_OPTION_GROUP			= 1100,	// ������ �ɼ� Ÿ��
	AT_CLASSLIMIT_DISPLAY_FILTER	= 1101, // �������� ǥ�ÿ� ����
	AT_SKILL_GOALPOS_COUNT	= 1102, // ��ų���� ��ǥ��ġ ��� ����
	AT_DUEL					= 1103, // ���� �� (Ư�� �÷��̾� Ÿ�� ����)
	AT_DUEL_DIED_BY			= 1104, // ���� �߿� �׾��ٸ�, ���� ���� �׿��� (UT_PLAYER, UT_MONSTER, ....)
	AT_DUEL_WINS			= 1105, // ���� �¸� Ƚ��

	AT_DROPITEMGROUP		= 2008,		// Drop �ϴ� ������ �׷� ID
	AT_EXCEPT_EFFECT_TYPE	= 2010,	//�� ������ �ɸ��� ���� ����Ʈ Ÿ��.
	AT_SKILL_ATT			= 2011,	// Skill �Ӽ� (ESkillAttribute value, bitmask value)
	ATS_COOL_TIME			= 2012,	// Skill : Cooling Time (���ο� ��ų�� ����� �� �ִ� �ּҽð�)
	AT_ADD_COOL_TIME		= 2013, // Skill�� ��ü���� �߰� ��Ÿ��(��ų �⺻ ��Ÿ�� + �߰� ��Ÿ��)
	AT_ADD_COOL_TIME_RATE	= 2014, // Skill�� ��ü���� �߰� ��Ÿ�� ��밪(��ų �⺻ ��Ÿ�� + �߰� ��Ÿ��)
	AT_MAINSKILL			= 2016,	// passive skill�� Main�� �Ǵ� active skill
	AT_CASTTYPE				= 2018,	// ESkillCastType (Instant/Maintenance/CastShot)
	AT_CAST_TIME			= 2019,	// �ֹ��� �ܿ�µ� �ʿ��� �ð�
	AT_ATT_DELAY			= 2020,	// ��ų���� ���� Delay �ð�
	AT_ADD_CAST_TIME		= 2021,	// �ֹ��� �ܿ�µ� �ʿ��� �߰� �ð�(ĳ���� Ÿ�� + �߰� ĳ���� Ÿ��)
	AT_TYPE					= 2022,	// Skill type
	AT_WEAPON_LIMIT			= 2023,	// Weapon limit
	AT_CLASSLIMIT			= 2024,
	AT_LEVELLIMIT			= 2025,
	AT_STATELIMIT			= 2026,
	AT_MAINTENANCE_TIME		= 2027,	// Skill : �����ð� ���� �ϴ� ��ų�� ��� ���� �ð�
	AT_NEED_SP				= 2028,
	AT_MAINTENANCE_TIME2	= 2029,	// Skill : EFireType_TimeDelay �� �����ð����� ���. ���� 0�̸� AT_MAINTENANCE_TIME���� ���
	AT_NAMENO				= 2030,			// TB_DefRes ������
	//AT_ADDHEAL			= 2037,	// ����ġ�� ��ų ���� �߰����� HP ȸ����
	//AT_S101200701_PARAM1	= 2038, // ��ų(101200701:�񷯺���)���� Critical ��뿩�ΰ�
	AT_BLOW_ACTION_ID		= 2037, // BlowAction ���۽� ���
	AT_ANITIME_STANDUP		= 2038, // ������ �ִ� Ÿ��
	AT_ANIMATION_TIME		= 2039, // ��ų�� �ִϸ��̼��� Ÿ���� �������� �ð�
	AT_BLOW_VALUE			= 2040,	// �ٿ�� �� : Effect Abil
	AT_DMG_MAX_UNIT			= 2041,	// Skill : Damage�� �Դ� Unit�� �ִ��
	AT_DAMAGED_DELAY		= 2042, // Monster/Skill : Damage�� ���� �� ������
	AT_BLOW_DISTANCE		= 2043,	// ����� �� �з����� �Ÿ�(�ִ밪)
	AT_EFFECT_NO_DUPLICATE	= 2050, // ���� ������ ����Ʈ�� ������ �ٽ� ����Ʈ�� ���� ����
	AT_EFFECTNUM1			= 2051,	// ��ų�� �߰����� Effect Number #1 (Ȥ�� ������ �߰��� ����Ʈ)
	AT_EFFECTNUM2			= 2052,	// ��ų�� �߰����� Effect Number #2 (Ȥ�� ������ �߰��� ����Ʈ)
	AT_EFFECTNUM3			= 2053,	// ��ų�� �߰����� Effect Number #3 (Ȥ�� ������ �߰��� ����Ʈ)
	AT_EFFECTNUM4			= 2054,	// ��ų�� �߰����� Effect Number #4 (Ȥ�� ������ �߰��� ����Ʈ)
	AT_EFFECTNUM5			= 2055,	// ��ų�� �߰����� Effect Number #5 (Ȥ�� ������ �߰��� ����Ʈ)
	AT_EFFECTNUM6			= 2056,	// ��ų�� �߰����� Effect Number #6 (Ȥ�� ������ �߰��� ����Ʈ)
	AT_EFFECTNUM7			= 2057,	// ��ų�� �߰����� Effect Number #7 (Ȥ�� ������ �߰��� ����Ʈ)
	AT_EFFECTNUM8			= 2058,	// ��ų�� �߰����� Effect Number #8 (Ȥ�� ������ �߰��� ����Ʈ)
	AT_EFFECTNUM9			= 2059,	// ��ų�� �߰����� Effect Number #9 (Ȥ�� ������ �߰��� ����Ʈ)
	AT_EFFECTNUM10			= 2060,	// ��ų�� �߰����� Effect Number #10 (Ȥ�� ������ �߰��� ����Ʈ)
	AT_CUSTOMDATA1			= 2061,	// Customized data #1
	AT_CUSTOMDATA2			= 2062,	// Customized data #2
	AT_CUSTOMDATA3			= 2063,	// Customized data #3
	AT_CUSTOMDATA4			= 2064,	// Customized data #4
	AT_CUSTOMDATA5			= 2065,	// Customized data #5
 	AT_CUSTOMDATA6			= 2066,	// Customized data #6
	AT_CUSTOMDATA7			= 2067,	// Customized data #7
	AT_CUSTOMDATA8			= 2068,	// Customized data #8
	AT_CUSTOMDATA9			= 2069,	// Customized data #9
	AT_DROPITEMRES			= 2071,	// Drop Item Icon Resource Number (TB_DefRes)
	AT_DAM_EFFECT_S			= 2072,	// Damage Motion Effect Num (Small Size)
	AT_DAM_EFFECT_M			= 2073,	// Damage Motion Effect Num (Middle Size)
	AT_DAM_EFFECT_L			= 2074,	// Damage Motion Effect Num (Large Size)
	AT_PERCENTAGE			= 2075,	// Percentage value
	AT_ADDVALUE				= 2076,	// �߰����� ��
	AT_PERCENTAGE2			= 2077,	// Percentage Value #2
	AT_MONSTER_SKILLTYPE	= 2078, // ���� ��ų Type
	AT_SKILL_KIND			= 2079, // Skill �з��� (ESkillKind)
	AT_PHY_DMG_PER			= 2080,	// Skill : Phyical Damage�� ���� Percentage ��
	AT_MAGIC_DMG_PER		= 2081, // Skill : Magic Damage�� ���� Percentage ��
	AT_RANGE_TYPE			= 2082,	// Skill : Range�� ����� ����� ���ΰ�? (EAttackRangeSource)
	AT_SPECIALABIL			= 2083,	// Skill or Effect : �μ������� ����� Ư���� �ɷ� Ability Number (��:ħ���ϱ�)
	AT_MP_BURN				= 2084,	//
	AT_GOD_TIME				= 2085,	// Effect : ��ų�°� ���� Effect�Ǹ� �����Ǵ� �ð�
	AT_CASTER_MOVERANGE		= 2086,	// Skill : Caster�� �����ϴ� ����
	AT_TARGET_MOVERANGE		= 2087,	// Skill : Target�� �и��� ����
	AT_DMG_CONSUME_MP		= 2088,	// Damage ������ HP��� MP�� �����ϴ� ��(AT_C_MAGIC_DMG_DEC ���� 0�̸� ȿ������)
	AT_COUNT				= 2089,	// ������ ��Ÿ���� ����ϸ� ���ڴ�.(Skill:�ִ� ������� ���� �� �ִ°�)
	AT_ATTACK_UNIT_POS		= 2090,	// ��ų : ������ �� �ִ� Target�� ���°�(EAttackedUnitPos)(����Ʈ 0�� ������ 1�� ó��)
	AT_COMBO_HIT_COUNT		= 2091,	// ��ų : ������ �� �ִ� ��ų�� ������ ���߽� �� �޺��� ���� �Ǵ� ��
	AT_SKILL_DIVIDE_TYPE	= 2092,	// ��ų : � ������ ��ų�� ������ �Ǿ����� (�⺻, ���, ���)

// �Ӽ� ���� �߰�(Old)
	AT_OFFENSE_PROPERTY			= 2093,	// ���� �Ӽ�
	AT_OFFENSE_PROPERTY_EXPAND	= 2094,	// ���� Ȯ�� �Ӽ�
	AT_DEFENCE_PROPERTY			= 2095,	// ��� �Ӽ�
	AT_DEFENCE_PROPERTY_EXPAND	= 2096, // ��� Ȯ�� �Ӽ�
	AT_OFFENSE_PROPERTY_LEVEL	= 2097,	// ���� �Ӽ� ����
	AT_DEFENCE_PROPERTY_LEVEL	= 2098,	// ��� �Ӽ� ����

	AT_COMBO_DELAY			= 2099, // ��ų : 2092 ��ų�� �����ϸ� ������ �޺� ���½ð��� �÷��ش� (2092 ����� ������ ��ų���� ���� ��)

	AT_SKILL_HP_CHECK_HIGH	= 2100,	// ��ų : HP�� ��% �̻��� ���� �� ��ų�� ����� ����(������)
	AT_SKILL_HP_CHECK_LOW	= 2101,	// ��ų : HP�� ��% ������ ���� �� ��ų�� ����� ����(������)
	AT_SKILL_MAP_DUMMY_NUM	= 2102,	// ��ų : ���� ���̸� ã�� �� ����� ��ȣ. ���� ��ų�� ���
	AT_NEED_MAX_R_HP		= 2103,	// ��ų : ��ų ���� �Ҹ�Ǵ� MaxHP�� %�� �Ҹ�(���з�)
	AT_NEED_MAX_R_MP		= 2104,	// ��ų : ��ų ���� �Ҹ�Ǵ� MaxMP�� %�� �Ҹ�(���з�)
	AT_SKILL_RANGE_TYPE2	= 2105,	// ��ų : ��ų ���� �������� �����˻縦 �Ҷ� ����� �˻� Ÿ��. 0�� �⺻.

	AT_NEED_HP_MIN_VALUE	= 2106,	// ��ų : ��ų ���� �ʿ��� �ּ����� HP
	AT_NEED_MP_MIN_VALUE	= 2107,	// ��ų : ��ų ���� �ʿ��� �ּ����� MP

	AT_SKILL_SEND_ZONE_TYPE			= 2109, // 
	AT_SKILL_MAP_BLOCKCOUNT_X		= 2110, // ��ų : 
	AT_SKILL_MAP_BLOCKCOUNT_Y		= 2111, // ��ų : 
	AT_SKILL_MAP_SAFETY_ZONE_COUNT	= 2112, // ��ų : ��������
	AT_SKILL_MAP_DUMMY_NUM1			= 2113, // ��ų : ���� ���̸� ã�� �� ����� ��ȣ. ���� ��ų�� ���
	AT_SKILL_MAP_DUMMY_NUM2			= 2114, // ��ų : ���� ���̸� ã�� �� ����� ��ȣ. ���� ��ų�� ���
	AT_SKILL_MAP_DUMMY_NUM3			= 2115, // ��ų : ���� ���̸� ã�� �� ����� ��ȣ. ���� ��ų�� ���
	AT_SKILL_MAP_DUMMY_NUM4			= 2116, // ��ų : ���� ���̸� ã�� �� ����� ��ȣ. ���� ��ų�� ���
	AT_SKILL_MAP_DUMMY_NUM5			= 2117, // ��ų : ���� ���̸� ã�� �� ����� ��ȣ. ���� ��ų�� ���
	AT_SKILL_MAP_DUMMY_NUM6			= 2118, // ��ų : ���� ���̸� ã�� �� ����� ��ȣ. ���� ��ų�� ���
	AT_SKILL_MAP_DUMMY_NUM7			= 2119, // ��ų : ���� ���̸� ã�� �� ����� ��ȣ. ���� ��ų�� ���
	AT_SKILL_MAP_DUMMY_NUM8			= 2120, // ��ų : ���� ���̸� ã�� �� ����� ��ȣ. ���� ��ų�� ���
	
	AT_EFFECT_TYPE_WHEN_HIT			= 2130, // 0 == ������ ��(�߰��� ��� Ÿ�� �߰�)
	AT_EFFECTNUM1_WHEN_HIT			= 2131, // ��ų�� �߰����� Effect Number #1 (Ȥ�� ������ �߰��� ����Ʈ)
	AT_EFFECTNUM2_WHEN_HIT			= 2132,
	AT_EFFECTNUM3_WHEN_HIT			= 2133,
	AT_EFFECTNUM4_WHEN_HIT			= 2134,
	AT_EFFECTNUM5_WHEN_HIT			= 2135,
	AT_EFFECTNUM6_WHEN_HIT			= 2136,
	AT_EFFECTNUM7_WHEN_HIT			= 2137,
	AT_EFFECTNUM8_WHEN_HIT			= 2138,
	AT_EFFECTNUM9_WHEN_HIT			= 2139,
	AT_EFFECTNUM10_WHEN_HIT			= 2140,

	AT_SKILL_SUMMONED_PARENT_SKILL		= 2150, // ��ų : ��ȯ�ְ� ��ȯü���� ������ ��ų��ȣ (������/����Ʈ �𼭸տ����� ���)
	AT_SUMMONED_NOT_SHOW_UI				= 2151, // UI : �� ����� ���Ե� ��ȯü�� ��ȯü ���� UI�� ����� �ʴ´�.

	AT_TARGET_TYPE			= 2200, // Ÿ���� Ÿ��(Item�� TargetType�� ����) / Skill�� m_byTarget, GetTargetType()
	AT_SKILL_CHECK_NONE_EFFECT		= 2250, //�� ����� ���̸� ��ų ��󿡰� ��ų�� ����� ����Ʈ�� �ɷ��ִ��� ã�ƺ��� �ɷ����� ���� ���� ĳ���� ������Ų�� (Ŭ���̾�Ʈ�� ���)

	AT_VISITOR_USE_ITEM		= 2300,	// ����Ȩ ������ �湮�ڰ� ��� ������ ������ �̴�.

	AT_TIME_TYPE			= 2301,	// ������ ��� �ð� Ÿ��
	AT_USE_TIME				= 2302,	// ������ ��� �ð�
	
	AT_ITEM_OPTION_TIME		= 2303,	// ������ �ɼ��� ����� �Ⱓ(���� ī�� �ɼ� ��� �ð���)

	AT_CAST_TIME_RATE		= 2304,	// ��ų ĳ��Ʈ Ÿ�� ������ ������
	
	AT_PHY_DMG_PER2			= 2480,	// AT_PHY_DMG_PER���� �нú� ��ų�̳� ������ ���� ���
	AT_MAGIC_DMG_PER2		= 2481, // AT_MAGIC_DMG_PER���� �нú� ��ų�̳� ������ ���� ���
	
	AT_PHY_DMG_PER2_SUB_PLAYER = 2482,	// AT_PHY_DMG_PER���� �нú� ��ų�̳� ������ ���� ���(SUB_PLAYER���Ը� ����)
	AT_MAGIC_DMG_PER2_SUB_PLAYER = 2483,// AT_MAGIC_DMG_PER���� �нú� ��ų�̳� ������ ���� ���(SUB_PLAYER���Ը� ����)

	AT_USE_PART_CHECK_COLLISION		= 2500, // �κ� �浹�� ���(��ġ���������� Ȱ��)
	AT_PART_CHECK_COLLISION_01		= 2501, // �����Ǹ� �浹ó��, ABVShapeIdx(XML����)�� 0�� ���ü��� �־� Abil�� +1�Ͽ� ���
	AT_PART_CHECK_COLLISION_02		= 2502,
	AT_PART_CHECK_COLLISION_03		= 2503,
	AT_PART_CHECK_COLLISION_04		= 2504,
	AT_PART_CHECK_COLLISION_05		= 2505,
	AT_PART_CHECK_COLLISION_06		= 2506,
	AT_PART_CHECK_COLLISION_07		= 2507,
	AT_PART_CHECK_COLLISION_08		= 2508,
	AT_PART_CHECK_COLLISION_09		= 2509,
	AT_PART_CHECK_COLLISION_10		= 2510,

	AT_EFFECT_USE_MAPNO_01				= 2511,	//����Ʈ�� �ɸ��� ���� ������ �ʹ�ȣ��
	AT_EFFECT_USE_MAPNO_10				= 2520,

	AT_IS_CHAIN_COMBO		= 2600,
	AT_IS_OLD_COMBO			= 2601,

// Monster Ability
	AT_WALK_TYPE			= 3001,
	AT_JOB					= 3002,
	AT_DETECT_RANGE			= 3003,
	AT_CHASE_RANGE			= 3004,
	AT_MAX_SKILL_NUM		= 3005,
	AT_NORMAL_SKILL_ID		= 3006, // ����Ʈ ����
	AT_IDLE_TIME			= 3007,	// IDLE ���� �ð�
	AT_SKILL_CHOOSE_TYPE	= 3008, // AI���� Skill�� �����ϴ� ���
	AT_MOVE_RANGE			= 3009,
	AT_MON_SKILL_01			= 3010,	// AT_MON_SKILL_01 - AT_MON_SKILL_10 (����)
	AT_MON_SKILL_02			= 3011,	
	AT_MON_SKILL_03			= 3012,	
	AT_MON_SKILL_04			= 3013,	
	AT_MON_SKILL_05			= 3014,	
	AT_MON_SKILL_06			= 3015,	
	AT_MON_SKILL_07			= 3016,	
	AT_MON_SKILL_08			= 3017,	
	AT_MON_SKILL_09			= 3018,	
	AT_MON_SKILL_10			= 3019,	
	AT_MON_SKILL_RATE_01	= 3020,
	AT_MON_SKILL_RATE_02	= 3021,
	AT_MON_SKILL_RATE_03	= 3022,
	AT_MON_SKILL_RATE_04	= 3023,
	AT_MON_SKILL_RATE_05	= 3024,
	AT_MON_SKILL_RATE_06	= 3025,
	AT_MON_SKILL_RATE_07	= 3026,
	AT_MON_SKILL_RATE_08	= 3027,
	AT_MON_SKILL_RATE_09	= 3028,
	AT_MON_SKILL_RATE_10	= 3029,
	//AT_MON_SKILL_FREQUENCY_01 = 3090,	// AT_MON_SKILL_FREQUENCY_01 - AT_MON_SKILL_FREQUENCY_10 (����)
	AT_ENTITY_TYPE			= 3030, // EEntityType����
	//AT_MONSTER_TYPE			= 3031,
	AT_PATROL_MIN_TIME		= 3031,	// Monster Patrol Min time
	AT_PATROL_MAX_TIME		= 3032,	// Monster Patrol Max time
	//AT_NORMAL_SKILL_RATE	= 3032,	// ���� ����
	AT_DIE_EXP				= 3033,
	AT_UNIT_SIZE			= 3034,	// 1:Small, 2:Middle, 3:Large// ���� ũ�⿡ ���� Damage�޴� ����� �ٸ��� �ϱ� ���� ��
	AT_WAYPOINT_INDEX		= 3035,	// ���Ͱ� ���� ���� WayPoint�� index
	AT_WAYPOINT_GROUP		= 3036, // ���Ͱ� ���� ���� WayPoint�� Group
	//AT_SUMMONCOMRADE_STATE	= 3037, // ���Ͱ� ���Ḧ ��ȯ�� ���� �ֳ�?
	//AT_SUMMON_COMRADE		= 3038, // �� ���ʹ� ��ȯ�� ���̳�? �ƴϳ�?
	AT_MAX_SUMMON_COUNT		= 3037,	//�ִ� ��ȯ������ ��(���Ͱ� �ڽ����� ���� �� �ִ� �ִ� ���ͼ�)
	AT_SKILL_SUMMON_MONBAGCONTROL	= 3038,	// ��ų�� ��ȯ�� ���� ��
	AT_DAMAGEACTION_TYPE	= 3039,	// Monster:Damaged Action type (0 = normal, 1 = DamageAction without DamagedDelay, 101 = no damaged delay(boss), 111 = OnlyDamage And Not AddEffect,1001 = Parent Unit To Damage)// Effect : 0(normal), 1(Knock down)
	AT_IDLEACTION_TYPE		= 3040,	// Monster�� Idle ���¿��� ���� ���°��� �����ϰ� ������ ( 0=���� ����, 101 = ���� ���� �ʴ´�)
	AT_ITEM_BAG_PRIMARY		= 3041,	// Monster Regen - Item Bag Primary
	AT_ITEM_BAG_SECONDARY	= 3042,	// Monster Regen - Item Bag Secondary
	AT_NOT_SEE_PLAYER		= 3043, // 0 : �¾��� �� �÷��̾ ����! 101 : �¾Ƶ� �÷��̾ �Ĵٺ��� �ʴ´�.
	AT_MANUAL_DIEMOTION		= 3044, // 0 : �Ϲ����� ���� 101 : Ư���ϰ� ����(WorldAction��)
	AT_UNIT_SCALE			= 3045,	// Ŭ���̾�Ʈ���� Unit�� ���鶧 ����ϴ� Scale ������(% ���)
	AT_WAYPOINT_INDEX_FROM	= 3046,	// WayPoint �̵���� ���� Index
	AT_RETREAT_MIN_TIME		= 3047,	// AI���� �������� �ּҽð�
	AT_RETREAT_MAX_TIME		= 3048,	// AI���� �������� �ִ�ð�
	AT_RETREAT_MOVESPEED	= 3049,	// AI���� �������� �ӵ�(����ӵ��� ���� ������)
	AT_R_RETREAT_HP			= 3050,	// ���������ϴ� HP��
	AT_CHASE_MOVESPEED		= 3051,	// AI���� ���� �i�� �ӵ�(����ӵ��� ���� ������)
	AT_HIDE_ANI_TIME		= 3052,	// AI : Hide�ϰų�/��Ÿ���� �ɸ��� Ani �ð�
	AT_MONSTER_TYPE			= 3053,	// EMonsterType(0:Normal, 1:������)
	AT_HEIGHT				= 3054,	// Monster : ���������Ͱ� ���� ����
	AT_MANUAL_OPENING		= 3055,	// ������ Ư���� �������� �� ����

	//AT_MAX_DROP_ITEM_COUNT	= 3055,
	AT_ITEM_BAG_SUCCESS_RATE_CONTROL= 3056,

	AT_CHASE_DONE			= 3057,	// ���Ͱ� PC������ ���´°�?
	AT_CHASE_PATTERN		= 3058,	// ���� ���� ( 1 : ���� ��´�, �� �� : �ڸ� ����)
	AT_CHASE_ELAPSED_TIME	= 3059,

//Skill /Effect Abil 3060 ~ 3999���� ��� ����
	AT_NO_DAMAGE_USE_DAMAGEACTION	= 3060, //�������� ���� ������ ��Ǹ��� ���Ѵ�.
	AT_DELIVER_DAMAGE_BLOCK_RATE = 3061,	//��� �Դ� �������� ������ ������
	
//�켱���� ���ÿ� �ɷ��� ��� AT_BARRIER_100PERECNT_COUNT > AT_BARRIER_AMOUNT - 100% ���� ī���Ͱ� ������� ������ ������ �Ҹ�
	AT_BARRIER_AMOUNT			= 3062,		//�������� ������ �� �ִ� ��
	AT_BARRIER_100PERECNT_COUNT = 3063,		//100% �������� �����ϴ� ������ ī����
	AT_PREV_BARRIER_100PERECNT_COUNT = 3064, //���� ������ ī����(���� ���ϴ°��� �˱� ���ؼ�)
	AT_ATROPINE_USE_COUNT		= 3065,	//��Ʈ������ ����� Ƚ��
	AT_ENABLE_AUTOHEAL_BACKUP	= 3066, //�������� ���� ���¸� ����� �д�. //Autoheal����� ������� ���� ������ٰ� �ٽ� ���� �Ǿ��Ҷ� ���
	AT_PENETRATION_COUNT		= 3067, //��� ������ ���ΰ�?
	AT_DOUBLE_JUMP_USE			= 3068, //���� ���� ���	
	AT_NOT_SEE_TARGET		= 3069,	//0 : Ÿ���� �Ĵٺ��� 101 : Ÿ���� ó�ٺ��� �ʴ´�

	AT_DEFAULT_ATTACK_DELAY	= 3070,
	
	AT_NOT_FLY_TO_CAMERA	= 3071,	//�׾��� �� ī�޶�� ���ƿ��� �ʴ´�. Ŭ���̾�Ʈ��
	AT_POSTURE_STATE		= 3072,	//�����ְų� ���߿� �� �ְų�

	AT_ADD_R_NEED_HP			= 3073, // ��ų ������ ���Ǵ� HP�� %�� ���� �ñ��.(���з�) AT_NEED_HP * AT_ADD_NEED_HP / ABILITY_RATE_VALUE_FLOAT
	AT_ADD_R_NEED_MP			= 3074, // ��ų ������ ���Ǵ� MP�� %�� ���� �ñ��.(���з�) AT_NEED_MP * AT_ADD_NEED_MP / ABILITY_RATE_VALUE_FLOAT

	AT_DELIVER_DAMAGE_CALLER = 3075, // Caller�� ��� �������� �Ծ�� �ϴ� ��� ����(Entity�� ���)

	AT_DAMAGEACTION_FORCE	= 3076, // ��ų�� ���õǾ� ������ ������ ������ �׼��� ���Ѵ�.
	AT_DAMAGECHECK_INTER	= 3077,	//CheckTagetVaild���� ���. ���������� �������� ������ �ִ� �ð�����. 0�̸� 100

	AT_ADD_R_NEED_HP_2			= 3078, // AT_ADD_R_NEED_HP ����� �ߺ��� ���� �ʴ� ���� �����Ͽ� ������� �ߺ��ǵ���
	AT_ADD_R_NEED_MP_2			= 3079, // AT_ADD_R_NEED_MP ����� �ߺ��� ���� �ʴ� ���� �����Ͽ� ������� �ߺ��ǵ���
	
	AT_MIN_DROP_ITEM_COUNT	= 3081,
	AT_MAX_DROP_ITEM_COUNT	= 3082,

	AT_MIN_DROP_MONEY_COUNT = 3083,
	AT_MAX_DROP_MONEY_COUNT = 3084,
	
	AT_SPREAD_RANGE			= 3085,
	AT_IS_DEATH_PENALTY		= 3086,

	AT_HP_BACKUP			= 3087,
	AT_MP_BACKUP			= 3088,

	AT_ADD_BARRIER_AMOUNT	= 3089, // �������� �߰����� ������
	
	AT_SKILL_ON_DIE			= 3090,		//HP�� 0�϶� ��� �� ��ų ��ȣ
	AT_DAMAGE_EFFECT_DURATION	= 3091,		//������ �������׼� �ð� �����
	AT_DAMAGEACTION_RATE	= 3092,		//���� ������ �׼� �� Ȯ��. 10000����. AT_DAMAGEACTION_TYPE(3039)����� �־�� ������
	AT_AI_TARGETTING_TYPE	= 3093,		//AI�� � Ÿ���� Ÿ���� ������ �����ϴ� ���
	AT_AI_TARGETTING_SECOND_TYPE	= 3094,	//Ÿ���� Ÿ���� Hi/Low�� ����
	AT_AI_TARGETTING_THIRD_TYPE		= 3095,	//Ÿ���� ������ �ɼ�
	AT_AI_TARGETTING_CHASE			= 3097,	//Chase�� �� ����� Ÿ���� �ɼ�
	AT_AI_TARGETTING_RATE			= 3100,	//Ÿ������ �� Ȯ��. ���з�

	AT_CLASS_PARTY_BUFF_01	= 3101,		//Ŭ���� ���� ������ ��Ƽ ���� Def_Class���� ���
	AT_CLASS_PARTY_BUFF_02	= 3102,
	AT_CLASS_PARTY_BUFF_03	= 3103,
	AT_CLASS_PARTY_BUFF_04	= 3104,
	AT_CLASS_PARTY_BUFF_05	= 3105,
	AT_CLASS_PARTY_BUFF_06	= 3106,
	AT_CLASS_PARTY_BUFF_07	= 3107,
	AT_CLASS_PARTY_BUFF_08	= 3108,
	AT_CLASS_PARTY_BUFF_09	= 3109,
	AT_CLASS_PARTY_BUFF_10	= 3110, //AT_CLASS_PARTY_BUFF_01 - 10 ���� ���

	AT_EXCEPT_EFFECT_TICK	= 3111, //�ش� ��ȣ�� ����Ʈ�� ƽ ȿ���� �����ش�.
	AT_EXCEPT_CHAOS_EFFECT_TICK	= 3112, //�ش� ��ȣ�� ����Ʈ�� ī���� ������ �����ش�.
	AT_NOT_DELETE_EFFECT = 3121, // Ư�� ���ǿ��� �������� �ʴ� Effect Type�ΰ�?(0 : ������ ���� / 1 : Clear�ÿ� ������� ����) / Duration�� �������� DeleteEffect ȣ��ÿ��� ����
	AT_DAMAGE_RESETTICK_EFFECT = 3122, // �ش� ����Ʈ�� �ɷ��ִ� ���¿��� Damage�� ������ TickTime�� �ʱ�ȭ ��
	AT_IGNORE_REQ_USER_DEL_EFFECT = 3123,	// ���� ��û���� ���� �� �� ���� ����Ʈ
	AT_IGNORE_SKILLLV_TOOLTIP = 3124, // ��ų ���� ǥ���Ҷ� LVǥ�� ���� ����

	AT_PENETRATION_USE_TYPE = 3125, // ������ ����ϴ� Ÿ�� 0 : ������� ���� / 1 : ������ ���
	AT_PROJECTILE_NUM = 3126,		// �߻�ü�� ����� ����� �����ϴ� ���
	AT_IGNORE_GLOBAL_COOLTIME = 3127, // �۷ι� ��Ÿ���� �����ϰ� ��ų ��� ����( 1 �̸� ���� )
	AT_IGNORE_REFLECTED_DAMAGE_SKILL = 3128, // ������ �ݻ� ��ų�� ����(1�̸� ����)

	AT_CURE_NOT_DELETE = 3129, // ť�� ������ �������� �ʵ��� �ϴ� ���( 0 : ������ / 1 : ť� �������� <ť��� ������� ���� : ������� �߰� �ϸ� ��>)
	AT_ONDAMAGE_DEL_EFFECT = 3130, //�������� �ް� �Ǹ� �ش� ����Ʈ�� ������

	AT_HP_RECOVERY_TICK_ITEM_AMOUNT = 3131, // ������ ���� �� ƽ���� ȸ���Ǵ� �ִ뷮(UIǥ�ÿ�)
	AT_MP_RECOVERY_TICK_ITEM_AMOUNT = 3132, // ������ ���� �� ƽ���� ȸ���Ǵ� �ִ뷮(UIǥ�ÿ�)

	AT_NAME_COLOR						= 3133, // �̸�ǥ�� ����(Ŭ���), ��� int������ ���� �־�� �ϱ� ������ �����̸��� ǥ���ϱ� ���ؼ� -65536(0xFFFF0000)�� �����ؾ� ��
	AT_CUSTOM_TARGET_DAMAGE_UNITTYPE	= 3134, // �Դ� �������� ������ Ÿ�� Ÿ��
	AT_CUSTOM_TARGET_DAMAGE_RATE		= 3135, // �Դ� �������� ������ ��밪

	AT_FIRESKILL_NO_DAMAGEACTION		= 3136, // ��ų �����߿� ���̹̾׼��� �ϴ��� ����
	AT_FIRESKILL_NO_TARGET				= 3137, // ��ų ������ Ÿ�� ������ �������� ����(PgForceSequenseFireSkillFunction �ߵ��� �ι�° ��ų�� �ش� ����� �����ϸ� Ÿ���� ���� ��ų�� �ߵ����� �ʰ� �ٶ󺸴� �������� ��ų�� ������)

	AT_EXCEPT_DAMAGE_FONT = 3139, // �������� ������ ��Ʈ ó��(�� ����� �ִ� Ÿ���� ��ȣ�� ���� �ٸ� ������ ���־�� �Ѵ�.)

	AT_DEFAULT_EFFECT	= 3140,// Unit�� Add���ڸ��� �ɷ� �ִ� ����Ʈ

	AT_SHOW_EFFECT_TIME	= 3141,			// �� ����� 0�� �ƴϸ� ����Ʈ �ð��� ȭ�鿡 ��������
	AT_SHOW_EFFECT_TIME_ATTR = 3142,	// AT_SHOW_EFFECT_TIME����� �ִ� ����Ʈ�� ����� �� ȭ�� ������ ��Ұ��ΰ�.EBT_BAR_TWINKLE=1, EBT_SCREEN_BLINK=2 �Ѵٴ� 3. ������ 2

	AT_EFFECT_BEGIN_SEND_TTW_MSG = 3143,	// ����Ʈ���� �ش� ��ȣ�� TTW ��ȣ�� ������ Ŭ���̾�Ʈ�� �޽����� �����ش�.(����Ʈ ���۽ÿ�)
	AT_EFFECT_END_SEND_TTW_MSG = 3144,		// ����Ʈ���� �ش� ��ȣ�� TTW ��ȣ�� ������ Ŭ���̾�Ʈ�� �޽����� �����ش�.(����Ʈ �����ÿ�)
	
	AT_HP_DEC_MAX_HP_RATE = 3145,	// HP�� MaxHP�� %��ŭ ���� : MaxHP�� %��ŭ ����(���ҷ��� �Է��ؾ��Ѵ�. MaxHP�� 10% ��ŭ ���Ҷ�� -1000 �Է�)
	AT_MP_DEC_MAX_MP_RATE = 3146,	// MP�� MaxMP�� %��ŭ ���� : MaxMP�� %��ŭ ����(���ҷ��� �Է��ؾ��Ѵ�. MaxMP�� 10% ��ŭ ���Ҷ�� -1000 �Է�)
	
	AT_UNABLE_EFFECT_PC				= 3147, // AddEffect ���� ����(��: ��Ʈ����)(����: CUnit::AddEffect)
	AT_UNABLE_EFFECT_PC_INTERVAL	= 3148, // AddEffect ���� ����(��: ��Ʈ����)(����: CUnit::AddEffect)

	AT_ATTACK_EFFECTNUM01 = 3150, // ���ݽÿ� ���Ǵ� ����Ʈ( Ư�� ��ų �ߵ��� ���ݸ��� ����Ʈ�� �Ŵ� ��� )
	AT_ATTACK_EFFECTNUM02 = 3151, 
	AT_ATTACK_EFFECTNUM03 = 3152, 
	AT_ATTACK_EFFECTNUM04 = 3153, 
	AT_ATTACK_EFFECTNUM05 = 3154, 
	AT_ATTACK_EFFECTNUM06 = 3155, 
	AT_ATTACK_EFFECTNUM07 = 3156, 
	AT_ATTACK_EFFECTNUM08 = 3157, 
	AT_ATTACK_EFFECTNUM09 = 3158, 
	AT_ATTACK_EFFECTNUM10 = 3159, 

	AT_ATTACK_EFFECTNUM01_RATE = 3160, // AT_ATTACK_EFFECTNUM01�� �ɸ� Ȯ�� ������ 100% ( ��� ��޿� �����ϰ� ���� �Ǵ� ��� / 3940~3979 ���� ��޿� ���� �ٸ��� ����Ǵ� Ȯ�� ����� )
	AT_ATTACK_EFFECTNUM02_RATE = 3161,
	AT_ATTACK_EFFECTNUM03_RATE = 3162,
	AT_ATTACK_EFFECTNUM04_RATE = 3163,
	AT_ATTACK_EFFECTNUM05_RATE = 3164,
	AT_ATTACK_EFFECTNUM06_RATE = 3165,
	AT_ATTACK_EFFECTNUM07_RATE = 3166,
	AT_ATTACK_EFFECTNUM08_RATE = 3167,
	AT_ATTACK_EFFECTNUM09_RATE = 3168,
	AT_ATTACK_EFFECTNUM10_RATE = 3169,

	AT_ATTACK_EFFECTNUM01_TARGET_TYPE_IS_ME = 3170, // �ش� ��ȣ�� ����Ʈ�� �ڽſ��� �ɾ��ְ� �ʹٸ� 1�� ����
	AT_ATTACK_EFFECTNUM02_TARGET_TYPE_IS_ME = 3171,
	AT_ATTACK_EFFECTNUM03_TARGET_TYPE_IS_ME = 3172,
	AT_ATTACK_EFFECTNUM04_TARGET_TYPE_IS_ME = 3173,
	AT_ATTACK_EFFECTNUM05_TARGET_TYPE_IS_ME = 3174,
	AT_ATTACK_EFFECTNUM06_TARGET_TYPE_IS_ME = 3175,
	AT_ATTACK_EFFECTNUM07_TARGET_TYPE_IS_ME = 3176,
	AT_ATTACK_EFFECTNUM08_TARGET_TYPE_IS_ME = 3177,
	AT_ATTACK_EFFECTNUM09_TARGET_TYPE_IS_ME = 3178,
	AT_ATTACK_EFFECTNUM10_TARGET_TYPE_IS_ME = 3179,

	AT_MON_DEFAULT_ANGLE		= 3180,		//���Ͱ� �����Ǿ��� �� �ٶ� ����
	AT_MON_ANGLE_FIX			= 3181,		//���Ͱ� �����Ǿ��� �� �ٶ� ������ ������ų���̳�
	AT_MON_OPENING_TIME			= 3182,		//���� ������ �ð�. ������ 3000ms

	AT_CREATE_ENTITY_RANGE		= 3183,		//�������� ���� ��ȯü ��ȯ ��ġ
	AT_GROWTH_SKILL_RANGE		= 3184,		//Ÿ���� ã�� �� ���������� �� ������ ����
	AT_DETECT_RANGE_Z			= 3185,		//Ÿ���� ã�� �� ����� Z��ǥ ����. 0�̸� AI_Z_LIMIT�� ���
	AT_ABV_SIZE_RATE			= 3186,		//Ŭ�󿡼� �浹�� ������ ��ġ
	AT_DETECT_RANGE_RATE		= 3187,		//��ų�ν� ���� Ȯ��
	AT_ATTACK_RANGE_RATE		= 3188,		//
	AT_CREATE_SUMMON_DIR		= 3189,		//�����Ǿ��� �� �ٶ� Ÿ��
	
	AT_PROJECTILE_IGNORE_DOT_TARGET	= 3190,		//Ÿ���� �߻�ü �ڿ��־, �ڵ������� �߻�ü�� ���� ���� �ʴ´�(�������� �������� �ʴ� �߻�ü ���� �ʿ�)
	AT_PROJECTILE_DAMAGE_ACTION_NO	= 3191,		//�߻�ü�� �׼� ��ȣ�� ���� ������ �׼� ��ȣ�� �ٸ� ���

	AT_REVIVE_COUNT				= 3192,	//!�һ� ī��Ʈ
	AT_PREV_REVIVE_COUNT		= 3193, //@���� �һ� ī��Ʈ(���� ���ϴ°��� �˱� ���ؼ�)
	AT_REVIVE_EFFECT			= 3194, //@�һ� ���� ����Ʈ
	AT_ADD_EFFECT_DURATIONTIME	= 3195, //!����Ʈ �����ð� ����
	AT_ADD_BLESSED_DURATIONTIME	= 3196, //@���� ����Ʈ �����ð� ����
	AT_ADD_CURSED_DURATIONTIME	= 3197, //@����� ����Ʈ �����ð� ����

	AT_EQUIP_WEAPON_PHY_ATTACK_MIN = 3201, // ���� ����� ������ ���� ���ݷ� �ּ�
	AT_EQUIP_WEAPON_PHY_ATTACK_MAX = 3202, // ���� ����� ������ ���� ���ݷ� �ִ�
	AT_EQUIP_WEAPON_MAGIC_ATTACK_MIN = 3203, // ���� ����� ������ ���� ���ݷ� �ּ�
	AT_EQUIP_WEAPON_MAGIC_ATTACK_MAX = 3204, // ���� ����� ������ ���� ���ݷ� �ִ�

	AT_ITEM_ATTACK_ADD_RATE			= 3208, //!������ ���ݷ� ����
	AT_ITEM_DEFENCE_ADD_RATE		= 3209, //!������ ���� ����

	AT_ITEM_DROP_CONTAINER			 = 3210, // �� ����� ������ ���ʹ� �ʹ�, ���͹� ��� ������� �ʴ´�.
	AT_ITEM_DROP_CONTAINER_EXPENSION = 3211, // AT_ITEM_DROP_CONTAINER �ܿ� �߰��� ITEM_CONTAINER�� �ʿ��� ����

	AT_MIN_DROP_ITEM_COUNT_EXPENSION = 3212, // ������ ������ ��� Ư�� ���� �������� ��� �ϴ���(�ּ�).
	AT_MAX_DROP_ITEM_COUNT_EXPENSION = 3213, // ������ ������ ��� Ư�� ���� �������� ��� �ϴ���(�ִ�).

	AT_EVENT_MONSTER_DROP			 = 3214, // �̺�Ʈ ����. �� ����� ����Ǹ� �̺�Ʈ ���� ������ ���̺� ���.
	AT_EVENT_PARENT_MONSTER			 = 3215, // �̺�Ʈ ����. �ڽ��� ��ȯ�� �ֻ��� ���� ��ȣ.
	AT_EVENT_MONSTER_DROP_GROUP_NO	 = 3216, // ���� ������ ���� ������ �̺�Ʈ ���Ͱ� ����� ������ �׷� ��ȣ.
	AT_EVENT_MONSTER_DROP_ITEM_COUNT = 3217, // ���� ������ ���� ������ �̺�Ʈ ���Ͱ� ����� ������ ī��Ʈ.
	AT_EVENT_MONSTER_NO_HAVE_GENINFO = 3218, // ���� ������ ���� �̺�Ʈ ����. �� �ΰ��� �̿��ؼ� ������ ��� ������ ���ؾ��Ѵ�.

//OnEffectProc�� �̿��Ͽ� ó��
	AT_DMG_ADD_EFFECT_01		= 3220, // �ǰݽ� ����Ʈ�� �ɾ��ش�. (10���� ���� ~ 3229)
	AT_DMG_ADD_EFFECT_01_RATE	= 3230, // �ǰݽ� ����Ʈ�� �ɾ��� Ȯ��. (10���� ���� ~ 3239)
	AT_DMG_ADD_EFFECT_01_TARGET_TYPE_IS_ME = 3240, // �ǰݽ� ����Ʈ�� �Ŵ� ���� �ڽ��ΰ�?. (10���� ���� ~ 3249)

	AT_CRITICAL_HIT_ADD_EFFECT_01		= 3250, // ũ��Ƽ�� ���ݽ� ����Ʈ�� �ɾ��ش�. (10���� ���� ~ 3259)
	AT_CRITICAL_HIT_ADD_EFFECT_01_RATE	= 3260, // ũ��Ƽ�� ���ݽ� ����Ʈ�� �ɾ��� Ȯ��. (10���� ���� ~3269)
	AT_CRITICAL_HIT_ADD_EFFECT_01_TARGET_TYPE_IS_ME = 3270, // ũ��Ƽ�� ���ݽ� ����Ʈ�� �Ŵ� ���� �ڽ��ΰ�?. (10���� ���� ~ 3279)
	//
	AT_ROAD_MIN_TIME                = 3290, // Monster Road Min time
	AT_ROAD_MAX_TIME                = 3291, // Monster Road Min time
	AT_ROADPOINT_GROUP              = 3292, // ���Ͱ� ���� ���� RoadPoint�� Group
	AT_ROADPOINT_INDEX              = 3293, // ���Ͱ� ���� ���� RoadPoint�� index
	AT_ROADPOINT_INDEX_FROM         = 3294, // RoadPoint �̵���� ���� Index
	AT_ROADPOINT_DELAY              = 3295, // RoadPoint ������ ������ �ð�
	AT_ROADPOINT_DELAY_RESERVED     = 3296, // RoadPoint ������ ������ �ð� ���ǰ�
	AT_AI_ROAD_BREAKAWAY     		= 3297, // AI_Road�� Ÿ�� ���� ���� �߰��ϸ� ���̻� AI_Road�� Ÿ�� �ʵ��� �ϴ� ���

	AT_MARKET_HISTORY_POINT	= 3301,			// ���� ���� ����Ʈ
	AT_MARKET_ONLINE_TIME = 3302,			// ���� �¶��� ���� �ð�
	AT_MARKET_OFFLINE_TIME = 3303,			// ���� �������� ���� �ð�
	AT_MARKET_OPEN_TYPE = 3304,				// ���� ���� Ÿ�� (0: ���� ���� Ƽ���� �ƴ�, 1: ���� ����, 2: ���� ���� ����)

	AT_MON_SKILL_SECOND_01			= 3310,	// AI�� ���� �θ��� �ʴ� ��ų ���
	AT_MON_SKILL_SECOND_02			= 3311,
	AT_MON_SKILL_SECOND_03			= 3312,
	AT_MON_SKILL_SECOND_04			= 3313,
	AT_MON_SKILL_SECOND_05			= 3314,
	AT_MON_SKILL_SECOND_06			= 3315,
	AT_MON_SKILL_SECOND_07			= 3316,
	AT_MON_SKILL_SECOND_08			= 3317,
	AT_MON_SKILL_SECOND_09			= 3318,
	AT_MON_SKILL_SECOND_10			= 3319,

	AT_MON_SKILL_CALLBY_SKILL		= 3320,	//�� ��ȣ�� �ش��ϴ� ��ų�� ������ �θ���.
	AT_MON_SKILL_UNIT_TYPE			= 3321,	//��ų���� ������ Ÿ���� �ʿ��� ���
	AT_MON_SKILL_MONSTER_NO			= 3322, //��ų���� ���� ��ȣ�� �ʿ��� ���
	AT_MON_SKILL_AFTER_DIE			= 3323, //��ų�� ����ϰ� ���� �״´�.
	AT_MON_SKILL_ON_FIRST			= 3324,	//���Ͱ� ���� ���� ��� �ϴ� ��ų. ������ ��ųť�� �־��ش�
	AT_MON_SKILL_TARGET_NO			= 3325,	//��ų ���� Ÿ�� �ѹ�(���� or Ŭ������ȣ)
	AT_MON_SKILL_TARGET_NO_02		= 3326,	//��ų ���� Ÿ�� �ѹ�(���� or Ŭ������ȣ)
	AT_MON_SKILL_TARGET_NO_03		= 3327,	//��ų ���� Ÿ�� �ѹ�(���� or Ŭ������ȣ)
	AT_MON_SKILL_TARGET_TYPE		= 3328, // 3325~3327 ������ Ÿ�ٿ���, 3328���� 0�̸� �װ͵鸸 ����, 1���� ũ�� �װ� �鸸 ����(���� ��ȯ�簡 ��ȯü ��ȯ�Ҷ� �����ɶ��� �����)

	AT_TARGET_ME_EFFECTNUM			= 3329, //�ڽſ��� �� ����Ʈ ��ȣ
	AT_MON_SKILL_FORCE_MOVE_DIR		= 3330, // �����̵�, 0:������, 1:����(�н�), 2:������

	AT_MON_ENTITY_NOT_TRANS_SKILL1	= 3331, // ����Ƽ�� add �ɶ� ���� �׼��� skill1 ���� �����ϴ°��� ����(Ŭ���̾�Ʈ)

	AT_MON_RESULT_KILL_ALL			= 3332,	// ���� ���Ͱ� ������ ������ ����鵵 �� ������. �δ�������
	AT_MON_RESULT_MOVE_MAP_NUM		= 3333, // ���� ���Ͱ� ������ �̵��� �� ��ȣ
	AT_MON_RESULT_MOVE_MAP_TIME		= 3334, // ���� ���Ͱ� ������ �̵��� �� ���� ��� �ð�

	AT_MON_ADD_REMOVE				= 3335, // add : 0, remove : 1 : �����ϸ� �������� ���
	AT_MON_EFFECT_REITERATE			= 3336, // ����Ʈ ��ø Ƚ��

	AT_MON_ADD_HP_RATE				= 3338, // HP ȸ���� �ִ�ġ �������� ȸ���Ǵ� ����, ���з�

	AT_MON_CROSS_ATTACK_ANGLE		= 3339, // x ������ ȸ�� ����, ( ~ 90��)

	AT_MON_CAN_TARGET_NO_DELAY		= 3340, // Ÿ�� �Ǵܽ� �����̸� �����ϰ� ���õɼ� �ִ�.

	AT_MON_PROJECTILE_NUM			= 3341, // �߻��ϴ� �߻�ü ����
	AT_MON_PROJECTILE_RANDOM		= 3342, // �ִ� �߻�ü �������Ϸ� ���Ƿ� ������ ���ϴ��� ����(0:���Ծ���, 1:���Ѵ�)
	
	AT_MON_PROJECTILE_ALREADY_SET	= 3343, // �߻�ü����Ÿ�� �̹� �����ߴ�.
	AT_MON_PROJECTILE_ANGLE			= 3344, // �߻�ü������ ����

	AT_MON_TRANSFORM_COUNT			= 3350,	// ���Ͱ� ������ Ƚ��
	AT_MON_TRANSFORM_CLASS_01		= 3351,	// ���Ͱ� ó�� ������ Ŭ���� ��ȣ
	AT_MON_TRANSFORM_CLASS_02		= 3352,	// ���Ͱ� �ι�° ������ Ŭ���� ��ȣ
	AT_MON_TRANSFORM_CLASS_03		= 3353,	// 
	AT_MON_TRANSFORM_CLASS_04		= 3354,	// 
	AT_MON_TRANSFORM_CLASS_05		= 3355,	// 
	AT_MON_TRANSFORM_CLASS_06		= 3356,	// 
	AT_MON_TRANSFORM_CLASS_07		= 3357,	// 
	AT_MON_TRANSFORM_CLASS_08		= 3358,	// 
	AT_MON_TRANSFORM_CLASS_09		= 3359,	// 
	AT_MON_TRANSFORM_CLASS_10		= 3360,	// 
	AT_MON_TRANSFORM_CLASS			= 3361,	//����Ʈ���. ������ Ŭ���� ��ȣ.
	AT_ADJUST_PARTICLE_SCALE_BY_UNIT_SCALE	= 3370,	// Ŭ���̾�Ʈ���� ��ƼŬ�� ���϶� DB(3045���)�� ���� Ŀ�� �����ŭ ��ƼŬ ũ�⸦ ������
	AT_SKILL_SUMMON_MONBAGCONTROL_01 = 3371, // ��ų�� ��ȯ�� ���� �� (3038 Ȯ��)
	AT_SKILL_SUMMON_MONBAGCONTROL_10 = 3380, // 10 ������

	AT_GROGGY_MAX						= 3381,	//!�г�/�׷α� �ִ밪
	AT_FRENZY_R_FIRE					= 3382,	//!�г�ߵ� ����(���з�)(1000�̸� �ִ�ü���� 10%�� �г���·� ����)
	AT_FRENZY_RECOVERY					= 3383,	//!�г� �ڵ����� ��
	AT_GROGGY_RECOVERY					= 3384,	//!�׷α� �ڵ����� ��
	//AT_INIT_EFFECT_NO					= 3385,	//!�ʱ���¿��� �ߵ��Ǵ� ����Ʈ
	AT_NORMAL_SKILL_NO					= 3386,	//!�Ϲݻ��·� ���� �� �ߵ��Ǵ� ����Ʈ
	AT_FRENZY_SKILL_NO					= 3387,	//!�г���·� ���� �� �ߵ��Ǵ� ����Ʈ
	AT_GROGGY_SKILL_NO					= 3388,	//!�׷α���·� ���� �� �ߵ��Ǵ� ����Ʈ

	AT_SKILL_SUMMON_MONBAGCONTROL_TYPE		= 3389,
	AT_SKILL_SUMMON_MONBAGCONTROL_RATE		= 3390,
	AT_SKILL_SUMMON_MONBAGCONTROL_RATE_01	= 3391, // ��ų�� ��ȯ�� ���� �� (3038 Ȯ��)
	AT_SKILL_SUMMON_MONBAGCONTROL_RATE_10	= 3400, // 10 ������

	AT_ADD_ATTACK_COOL_TIME = 3401,			// ���� ��Ÿ�� �߰�(õ�з�)
	AT_ADD_TICK_ATTACK_TIME = 3402,			// Ŭ���̾�Ʈ���� Tick�� �����ϴ� ��ų�� Tick Ÿ���� �߰��� ����(õ�з�)
	AT_ADD_MOVESPEED_BY_DELAY = 3403,		// ��Ʈ�� �����̿� ���� �̵��ӵ� ������. Ŭ���̾�Ʈ��

	AT_MON_REMOVE_EFFECT_FORCESKILL	= 3404, // ���Ϳ�, ����Ʈ(����)�� ���� �ɶ� ���� �ߵ��� ��ų(����Ʈ�� ���� �Ǿ��־�� ��)
	AT_MON_BEGIN_EFFECT_FORCESKILL	= 3405, // ���Ϳ�, ����Ʈ(����)�� ���� �ɶ� ���� �ߵ��� ��ų(����Ʈ�� ���� �Ǿ��־�� ��)
	AT_REMOVE_SKILL_FORCEEFFECT		= 3406, // ��ų������ ���۵� ��������Ʈ
	AT_MON_ADD_FORCESKILL			= 3407, // ������ ��ų ���(Ÿ�̹��� �� �Լ��� ���� �ٸ�)
	AT_PAUSE_TELEPORT_TO_DUMMY_EFFECT	= 3408,	//PgTeleportToDummyEffect Tick üũ�� �Ͻ������� 
	AT_AI_FIRE_ONENTER_ADDEFFECT	= 3409,	// ��ų���� �Ҷ� ����� ����Ʈ

	AT_SKILL_AGGRO_RATE		= 3410,			// ��ų�� ��׷���. �������� ���� ���з�
	AT_IS_COUPLE_SKILL		= 3420,			// Ŀ�ý�ų �����ΰ�?
	
	AT_R_COOLTIME_RATE_SKILL= 3421,			// ��ų ��Ÿ�� ���� õ������
	AT_R_COOLTIME_RATE_ITEM	= 3422,			// ������ ��� ��Ÿ�� ���� õ������

	AT_USE_INV_SORT_TIME = 3423,	// �κ��丮 ������ ����� �ð�( DOS ���� ���� 2�ʿ� �ѹ����� ��� �� �� �ְ� )
	
	AT_FORCESKILL_FLAG				= 3430,	//������ų�� Flag��
	AT_IGNORE_NOW_FIRESKILL_CANCEL	= 3431,	//������ų�� ���ؼ� ���� ��ų�� ��ҵǴ� ���� ����
	AT_MON_CHILD_DIE				= 3432,	//������ �ڽ����� �¾ ���� ���� ���̱�
	AT_AI_SKILL_FIRE_TIME			= 3433,	//��ų�� �ߵ��� �ð�
	AT_AI_SKILL_FIRE_TERM			= 3434,	//��ų�ߵ��� ������ �Ͼ�� ������ ��

	AT_EFFECT_ESCAPE		= 3440,	// Ű�� ���ؼ� ����Ʈ�� ������ �� �ִ� ����Ʈ ����
	AT_EFFECT_ESCAPE_MIN	= 3441,	// ���� �ּҰ�
	AT_EFFECT_ESCAPE_MAX	= 3442,	// ���� �ִ밪
		
	AT_SKILL_CHOOSE_IS_IN_CLASS_NO		= 3445, //��ų���ý� �ش� Ŭ������ ������ �ߵ����� �ʵ��� üũ
	AT_SKILL_CHOOSE_IS_IN_CLASS_TYPE	= 3446,
	AT_USE_IS_ALLY_SKILL			= 3447,	//�Ʊ��� �־�� ��ų�� �ߵ�
	AT_USE_NOT_PROJECTILE_SKILL		= 3448, //�߻�ü ��ų�� ��� ��ų�� �ߵ����� �ʵ��� ����
	AT_USE_LOCK_SKILL 		= 3449, // �� ��ų �������
	AT_LOCK_SKILLNO_01 		= 3450, // ���� ��ų ��ȣ
//AT_LOCK_SKILLNO 10�� ���(3450~3459)
	AT_LOCK_SKILLNO_10 		= 3459,

	AT_AI_SKILL_FIRE_TERM_00 		= 3460, //!��ų �ߵ� ������ ī��忡 ���� Ȯ�� ����, 00�� ������� ����. ������ ù��° ���� ���õǱ� ����
//AT_AI_SKILL_FIRE_TERM_00 10�� ���(3460~3469)
	AT_AI_SKILL_FIRE_TERM_09 		= 3469,

// ������
	AT_ENABLE_AWAKE_SKILL	= 3500,	// ������ �ý����� Ȱ��ȭ
	AT_AWAKE_STATE			= 3501,	// ������ ������ ����(0 : �Ϲݻ���(��ų ���� ä�����ų�, �ð��� �ڵ� ����), 1: �ƽ� ����, 2: �Ҹ� ����(������ ȸ��X)
	AT_AWAKE_VALUE			= 3502,	// ������ ������ ��
	AT_AWAKE_TICK_DEC_VALUE	= 3503,	// Tick���� �����ϴ� ������ ��
	AT_NEED_AWAKE			= 3504,	// ��ų ���� ����ϴ� ������ ������
	AT_NEED_MAX_R_AWAKE		= 3505,	// ��ų ���� ����ϴ� ������ ������ (Max�� %�� ���)(���з�).
	AT_ADD_NEED_R_AWAKE		= 3506, // ��ų ������ ����ϴ� ������ �������� %�� ���� �ñ��.(���з�) AT_NEED_AWAKE * AT_ADD_R_NEED_HP / ABILITY_RATE_VALUE_FLOAT
	AT_ALL_NEED_AWAKE		= 3507,	// ��ų ���� �����ִ� ��� ������ ������ �Ҹ�
	AT_R_INC_AWAKE_VALUE	= 3508, // ������ ������ �ִ밪�� %�� �����ϴ°�(Max�� %�� ����)(���з�)

	AT_AWAKE_SKILL_FIRE_ADD_EFFECT			= 3510, // ������ ��ų ���� �ɾ��ִ� ����Ʈ(���۾Ƹ� ����Ʈ ��ȣ�� �־ �ɾ��ش�) ����� ������ ����Ʈ ����
	AT_MASTER_SKILL_FIRE_ADD_EFFECT			= 3511, // �ñر� ��ų ���� �ɾ��ִ� ����Ʈ(���� ����Ʈ ��ȣ�� �־ �ɾ��ش�) ����� ������ ����Ʈ ����	
	
	AT_EMPTY_AWAKE_VALUE = 3520,	// 1�̻��̸� �ش� ��ų�� ����ص� ����ġ�� �������� ����

	AT_AWAKE_CHARGE_STATE				= 3550,
	AT_IS_DAMAGED_DURING_AWAKE_CHARGE	= 3551,

// ��ų ����
	AT_CANT_USE_THIS_GATTR_FLAG = 3600,	// �� �׶��忡���� ��ų�� ��� �� �� ����(FLAG ��) 
	
	AT_CANT_SKILL_ALIVE_UNIT_NO	= 3601,	//�ش������� ��������� ��ų�� �ߵ��� �� ����
	AT_AI_SKILL_MAX_COUNT		= 3602, //AI���� �ִ� ��� ��ų�� �ߵ��� ������ ����(������ AT_COUNT�� �� ���� ������ ã�Ƽ� �ٲ��־�� ��)
	AT_AI_FIRE_LAST_SKILLNO		= 3603,	//AI Fire���� �ߵ��� ��ų��ȣ�� ������
	AT_AI_SKILL_USE_COUNT		= 3604, //PgAIActFireSkill::OnEnter���� �ʱ�ȭ, PgGround::AI_SkillFire���� ī��Ʈ
	AT_DELETE_EFFECT_NO	= 3605,		// ����Ʈ ����

// ���� ȿ��
	AT_BEGINL_SKILL_LINKAGE		= 3606,				// ����ȿ���� ���� �ް� �ϴ� ���� ��ų
	AT_SKILL_LINKAGE_POINT		= 3607,				// ����ȿ�� ����
	AT_USE_SKILL_LINKAGE_POINT	= 3608,				// ����ȿ�� ������ ����ϴ� ��ų	
	AT_SKILL_LINKAGE_POINT_MAX	= 3608,				// ���� ����Ʈ �ִ� ������
	AT_SKILL_LINKAGE_POINT_EFFECT_TARGET01 = 3610,	// ����ȿ�� ���� ���. ESkillTargetType ���� ����(��ų�� Ÿ�� ����� ��Ʈ���� �ϴ°Ͱ� ���� ������ ���� ��)
	AT_SKILL_LINKAGE_POINT_EFFECT_TARGET02 = 3611,
	AT_SKILL_LINKAGE_POINT_EFFECT_TARGET03 = 3612,
	AT_SKILL_LINKAGE_POINT_EFFECT_TARGET04 = 3613,
	AT_SKILL_LINKAGE_POINT_EFFECT_TARGET05 = 3614,
	AT_SKILL_LINKAGE_POINT_EFFECT_TARGET06 = 3615,
	AT_SKILL_LINKAGE_POINT_EFFECT_TARGET07 = 3616,
	AT_SKILL_LINKAGE_POINT_EFFECT_TARGET08 = 3617,
	AT_SKILL_LINKAGE_POINT_EFFECT_TARGET09 = 3618,
	AT_SKILL_LINKAGE_POINT_EFFECT_TARGET10 = 3619,

	AT_SKILL_LINKAGE_POINT_EFFECT01 = 3620,	//	����ȿ�� ����Ʈ
	AT_SKILL_LINKAGE_POINT_EFFECT02 = 3621,
	AT_SKILL_LINKAGE_POINT_EFFECT03 = 3622,
	AT_SKILL_LINKAGE_POINT_EFFECT04 = 3623,
	AT_SKILL_LINKAGE_POINT_EFFECT05 = 3624,
	AT_SKILL_LINKAGE_POINT_EFFECT06 = 3625,
	AT_SKILL_LINKAGE_POINT_EFFECT07 = 3626,
	AT_SKILL_LINKAGE_POINT_EFFECT08 = 3627,
	AT_SKILL_LINKAGE_POINT_EFFECT09 = 3628,
	AT_SKILL_LINKAGE_POINT_EFFECT10 = 3629,
	
	AT_SKILL_LINKAGE_FLAG		= 3630,				// ���� Flag�� �����( Unit���� SetAbil64�� ���� )
	AT_SKILL_LINKAGE_CHECK_FLAG	= 3631,				// ���� Flag���� �����ϱ����� üũ�� ��
	AT_SKILL_LINAKGE_POINT_CHECK_FLAG = 3632,		// ���� ����Ʈ�� �����ϱ� ���� üũ�� Flag��
	AT_SKILL_LINAKGE_FLAG_REMOVE = 3633,			// ��ų ����� Unit�� ���� flag���� �����Ұ�

	AT_ADD_HP_FROM_MAX_HP_RATE			= 3641,			// MAX HP�� ������ HP�� ȸ��
	AT_ADD_MP_FROM_MAX_MP_RATE			= 3642,			// MAX MP�� ������ MP�� ȸ��
	
	AT_DRAIN_HP_APPLY_TARGET			= 3643,			// ����� HP�� *-1�� �Ѱ��� ������ ��� (1== ����Ʈ �� ���,)
	AT_DRAIN_MP_APPLY_TARGET			= 3644,			// ����� MP�� *-1�� �Ѱ��� ������ ��� (1== ����Ʈ �� ���,)
	
	AT_DRAIN_HP_APPLY_RATE				= 3645,			// ��󿡰� ������Ų HP ���� ������ ����
	AT_DRAIN_MP_APPLY_RATE				= 3646,			// ��󿡰� ������Ų MP ���� ������ ����

// ��ų ���� �پ�ȭ
	AT_SKILL_CUSTOM_CHECK_LEARN_THIS_SKILL_NO	= 3650,	// �� Value�� �ش��ϴ� ��ų�� ����ٸ�(Parent SkillNo), 
	AT_SKILL_CUSTOM_VALUE						= 3651,	// (Skill��!) AT_CUSTOM_SKILL_CHECK_LEARN_THIS_SKILL_NO�� �ش��ϴ� ��ų�� ����ٸ�, �� ���� ���� ����� ��������(unit�� ����)
	AT_SKILL_CUSTOM_VALUE_STORAGE				= 3652,	// (Unit��!) ���� Ŭ�󿡼��� �̰��� ����ϸ� '�񵿱�'���̴�. Ŭ�󿡼� �޾Ƽ� ó���ϰ� ���� ��Ų��. ���������� �ʿ��� ��Ȳ�� �����ؼ� �����ֱ⸸ �Ѵ�.
//***����� AT_SKILL_CUSTOM_VALUE_STORAGE���� �� �������� ������� �ʴ´�***
//Unit�� AT_CUSTOM_SKILL_CUSTOM_VALUE�� ����� ���� �������� �����*/

// ���� �������� �ڽ��� ���� ( ����� ������ �� ������. Ȯ�� �� �� �ּ��� ������ �ּ��� ���ΰ�)

	//AT_ACTIVE_RATE_HP_RESTORE_FROM_DAMAGE	= 3670,			// �󸶸�ŭ�� Ȯ���� �������� ���� HPȸ���� ����Ұ��ΰ�
	AT_R_HP_RESTORE_FROM_DAMAGE				= 3671,			// ���� �������� %�� HP ȸ��
	
	AT_CAN_NOT_BLOCK_BY_EFFECT			  = 3680,		// Effect�� ���ؼ� ������ �Ҽ� ���� ���� 
	AT_CAN_NOT_BLOCK_BY_ACTION			  = 3681,		// �׼ǿ� ���ؼ� ������ �Ҽ� ���� ���� 
	AT_HIT_AFTER_TARGET_CHANGE_BLOCK_ABLE = 3682,	// �� ����� �ִ� ��ų�� �ǰ��� ���� ������ �Ҽ� ���� ���°� ��( 1 = ���� �ȵ�
	AT_ACTION_AFTER_CHANGE_BLOCK_ABLE	  = 3683,	// �� ����� �ִ� �׼� �Ŀ� �ڽ��� ���� �Ұ��̰ų�, ���� ���� ���°� �� (1 = ���� �ȵ�, 2 = ���� ����) 
	AT_PUSH_DIST_WHEN_BLOCK				  = 3684,	// ���Ͻ� �и��Ÿ�
	AT_PUSH_SPEED_WHEN_BLOCK			  = 3685,	// ���Ͻ� �и��ӷ�
	AT_PUSH_ACCEL_WHEN_BLOCK			  = 3686,	// ���Ͻ� �и����ӷ�

// ĳ���� �Ϻ� �ɷ�ġ �ӽ� ������ �ʿ��Ҷ�
	AT_MEMORIZED_HP				= 3701,
	AT_MEMORIZED_MAX_HP			= 3702,
	AT_MEMORIZED_MP				= 3703,
	AT_MEMORIZED_MAX_MP			= 3704,
	AT_PREV_ACTION_ID			= 3705, // ���� �׼� ���� ���� ����ߴ� �׼� ��ȣ(���������� ��)
	AT_SKILL_LINKAGE_POINT_SETED= 3706, // ��ų�� ����Ҷ�, ��ų�� ������ ��������Ʈ���� �̹� �����ߴ�.
	
	AT_TARTGET_RE_POS = 3720,			// Ÿ���� ��ġ�� ����ġ�� �����Ѵ�(��� �̸��� �������� ������, ���� ����� �����ɰ�� �̰��� ��ġ �缳���� Ÿ������ ����� ����)
	AT_ADDITIONAL_SUMMON_SKILL_NO = 3721,				// ��ȯ�� �߰������� ��ȯ�� ��ų ��ȣ(��ȯ��ų�̾����)
	AT_ADDITIONAL_SUMMON_MUST_LEARN_SKILL_NO = 3722,	// ��ȯ�� �߰������� ��ȯ�Ϸ��� �Ҷ� �� ��ų�� ��������

	AT_AI_FIRE_ONENTER_DELEFFECT	= 3727,	//��ų�ߵ��� ������ ����Ʈ
	AT_AI_RUN_ACTION_MOVESPEED	= 3728,	// PgAIActFollowCaller: �� �̵��ӵ� ��������, PgAIActChaseEnemy: �߰��� ���ǵ����� ����
	AT_FOLLOW_PATTERN			= 3729,	// ����ٴ� Ÿ��
	AT_RETURNZONE_RANGE			= 3730,	// ��ȯ�� ũ��
	AT_WALKINGZONE_RANGE		= 3731,	// ��ŷ�� ũ��
	AT_RUNNINGZONE_RANGE		= 3732,	// �뽬�� ũ��(����� ��Ʋ�� ũ�⵵ �����ϰ� ���)
	AT_WALKINGZONE_SPEED_RATE	= 3733, //��ŷ���� ����� �ӵ�(������)
	AT_RUNNINGZONE_SPEED_RATE	= 3734, //�������� ����� �ӵ�(������)

	AT_RESTORE_WHEN_HIT_TYPE	= 3750, // Ÿ�ݽ� HP,MP ȸ�� ��� ������(ESkillTargetType Ȱ��)
	AT_RESTORE_WHEN_DAMAGE_TYPE	= 3751, // Ÿ�ݽ� HP,MP ȸ�� ��� ������(ESkillTargetType Ȱ��)

	AT_RESTORE_WHEN_DAMAGE_DAMAGE_EFFECT	= 3754, // �ǰݽ�(�ǰݴ��) ����Ʈ ȿ��
	AT_RESTORE_WHEN_DAMAGE_RESTORE_EFFECT	= 3755, // �ǰݽ�(ȸ�����) ����Ʈ ȿ��

// ����Ʈ�� �ִ� �������� �ݻ� ��ų������, ������ �ݻ� �Ǿ���� ���, �� ����Ʈ�� ������ Ÿ����?( 1=����, 2=����)
	AT_EFFECT_DMG_TYPE = 3800,
	AT_END_ACTION_WHEN_EFFECT_DELETED = 3801, //����Ʈ�� �������� ���� �ؾ��� �׼��� �ִٸ�
	AT_MUST_REFRASH_EFFECT = 3802,				// ����Ʈ�� ���������ε�, �ٽ� ����Ʈ�� �Ǵٸ� Begin()�Լ��� �� Ÿ����
	AT_KICK_WHEN_EFFECT_DELETED = 3803, // Dissconet user when effect is end

// ��ų�� ����ɶ� �ٿ��ִ� ����Ʈ
	AT_ADD_EFFECT_WHEN_SKILL_END = 3810,
	AT_DEL_SUB_PLAYER			= 3811,	// SubPlayer�� ���� ��Ű�� ���
	AT_WHEN_ACTION_END_THEN_TOGGLE_OFF = 3812, // �׼��� ������ ����� �����ϴ� ��ų
	AT_R_PROVOKE_MOVE_SPEED		= 3819,	//AT_PROVOKE_EFFECT_NO ����Ʈ�� �ɸ����¿��� �Ѿư� ���ǵ�
	AT_PROVOKE_EFFECT_NO		= 3820,	//Effect->Unit���. �� ������� ��ȣ�� �ش��ϴ� ����Ʈ�� ã�Ƽ� ����Ʈ�� ĳ���͸� �켱Ÿ���� ��.

/// Effect���� � �鿪���� üũ �Ұ��ΰ�?(Effect���� ����ϸ�, DB�� Value���� üũ�� AbilType�� ����)
/// 8000 ~ 9000 ������ Ÿ�԰����� �����ָ�, Effect�� �ɸ��� �ش� ���� üũ��
	AT_CHECK_IMMUNITY_TYPE				= 3890,
	AT_ADD_EFFECT_WHEN_SUCCESS_IMMUNITY	= 3891,

// 3900 ~ 3999 Monster Card �߰� ȿ�� �����
	AT_MONSTER_CARD_ABIL_MIN = 3900,

	AT_HIT_HEAL_HP_RATE	= 3900,	// Target�� Hit �ÿ� HP�� ȸ���� Ȯ�� ( ���з� )
	AT_HIT_HEAL_HP		= 3901,	// Target�� Hit �ÿ� ȸ���ϴ� HP ( ���밪 )
	AT_HIT_HEAL_MP_RATE	= 3902,	// Target�� Hit �ÿ� MP�� ȸ���� Ȯ�� ( ���з� )
	AT_HIT_HEAL_MP		= 3903,	// Target�� Hit �ÿ� ȸ���ϴ� MP ( ���밪 )

	AT_KILL_HEAL_HP_RATE	= 3904,	// Target�� Kill �ÿ� HP�� ȸ���� Ȯ�� ( ���з� )
	AT_KILL_HEAL_HP			= 3905,	// Target�� Kill �ÿ� ȸ���ϴ� HP ( ���밪 )
	AT_KILL_HEAL_MP_RATE	= 3906,	// Target�� Kill �ÿ� MP�� ȸ���� Ȯ�� ( ���з� )
	AT_KILL_HEAL_MP			= 3907,	// Target�� Kill �ÿ� ȸ���ϴ� MP ( ���밪 )
	AT_ABS_ADDED_DMG_PHY_2		= 3908,	// Dmg ������� ���밪���� �־��ִ� �߰� Dmg( ���밪 ) (Physical����) Effect�� ���ؼ� �߰��Ǵ� ��
	AT_ABS_ADDED_DMG_MAGIC_2	= 3909,	// Dmg ������� ���밪���� �־��ִ� �߰� Dmg( ���밪 ) (Magic����) Effect�� ���ؼ� �߰��Ǵ� ��

	AT_PHY_DEFENCE_IGNORE_RATE		= 3910, // ������ ���� ������ �����ϰ� ���� �� Ȯ�� ( ���з� )
	AT_MAGIC_DEFENCE_IGNORE_RATE	= 3911, // ������ ���� ������ �����ϰ� ���� �� Ȯ�� ( ���з� )

	AT_100PERECNT_BLOCK_RATE	= 3912, // ������ ������ ���� ����� Ȯ�� ( ���з� ) ����/���� ���� �������� ��� ����Ͽ� ��ĭ �����

	AT_PHY_DMG_100PERECNT_REFLECT_RATE	= 3914,	// ���� Damage 100% �ݻ� Ȯ��( ���з� )
	AT_MAGIC_DMG_100PERECNT_REFLECT_RATE	= 3915,	// ���� Damage 100% �ݻ� Ȯ��( ���з� )

	AT_PHY_DMG_SUBTRACT		= 3916,	// �������� ���� ������ ���� ( ���밪 )
	AT_MAGIC_DMG_SUBTRACT	= 3917,	// �������� ���� ������ ���� ( ���밪 )
	AT_PROJECTILE_DMG_SUBTRACT	= 3918,	// �߻�ü�� ��� ������ ���� ( ���밪 )

	AT_GRADE_NORMAL_DMG_ADD_RATE	= 3920, // �Ϲ� ���� Ÿ�ݽ� ������ ������ = ���� �������� %����( ���з� )
	AT_GRADE_UPGRADED_DMG_ADD_RATE	= 3921, // ��ȭ ���� ������ ������ ������ = ���� �������� %����( ���з� )
	AT_GRADE_ELITE_DMG_ADD_RATE		= 3922, // ���� ���� ������ ������ ������ = ���� �������� %����( ���з� )
	AT_GRADE_BOSS_DMG_ADD_RATE		= 3923, // ���� ���� ������ ������ ������ = ���� �������� %����( ���з� )
	
	AT_ADD_ENCHANT_RATE		= 3930,			// ��þƮ ���� Ȯ�� ����
	AT_ADD_SOULCRAFT_RATE	= 3931,			// �ҿ�ũ����Ʈ ���� Ȯ�� ����
	AT_DEC_SOUL_RATE		= 3932,			// �ҿ� �Һ� ���� ����
	AT_ADD_SOUL_RATE_REFUND = 3933,			// ������ �ı��� �ҿ� ȯ�� Ȯ�� ���� 
	AT_ADD_REPAIR_COST_RATE		= 3934,			// ���� ��� ����

// �Ʒ� Ȯ�� �� AT_ATTACK_EFFECTNUM01_RATE ~ AT_ATTACK_EFFECTNUM10_RATE ���� ����� �Ѵ�.
// AT_ATTACK_EFFECTNUM01_RATE~ AT_ATTACK_EFFECTNUM10_RATE or �Ʒ� ������� �߰� �Ǿ�� �Ѵ�.

	AT_ATTACK_EFFECTNUM01_GRADE_NORMAL_RATE = 3940, // AT_ATTACK_EFFECTNUM01�� Grade Normal ���� �ɸ� Ȯ�� ������ 100%
	AT_ATTACK_EFFECTNUM02_GRADE_NORMAL_RATE = 3941,
	AT_ATTACK_EFFECTNUM03_GRADE_NORMAL_RATE = 3942,
	AT_ATTACK_EFFECTNUM04_GRADE_NORMAL_RATE = 3943,
	AT_ATTACK_EFFECTNUM05_GRADE_NORMAL_RATE = 3944,
	AT_ATTACK_EFFECTNUM06_GRADE_NORMAL_RATE = 3945,
	AT_ATTACK_EFFECTNUM07_GRADE_NORMAL_RATE = 3946,
	AT_ATTACK_EFFECTNUM08_GRADE_NORMAL_RATE = 3947,
	AT_ATTACK_EFFECTNUM09_GRADE_NORMAL_RATE = 3948,
	AT_ATTACK_EFFECTNUM10_GRADE_NORMAL_RATE = 3949,

	AT_ATTACK_EFFECTNUM01_GRADE_UPGRADED_RATE = 3950, // AT_ATTACK_EFFECTNUM01�� Grade Upgraded ���� �ɸ� Ȯ�� ������ 100%
	AT_ATTACK_EFFECTNUM02_GRADE_UPGRADED_RATE = 3951,
	AT_ATTACK_EFFECTNUM03_GRADE_UPGRADED_RATE = 3952,
	AT_ATTACK_EFFECTNUM04_GRADE_UPGRADED_RATE = 3953,
	AT_ATTACK_EFFECTNUM05_GRADE_UPGRADED_RATE = 3954,
	AT_ATTACK_EFFECTNUM06_GRADE_UPGRADED_RATE = 3955,
	AT_ATTACK_EFFECTNUM07_GRADE_UPGRADED_RATE = 3956,
	AT_ATTACK_EFFECTNUM08_GRADE_UPGRADED_RATE = 3957,
	AT_ATTACK_EFFECTNUM09_GRADE_UPGRADED_RATE = 3958,
	AT_ATTACK_EFFECTNUM10_GRADE_UPGRADED_RATE = 3959,

	AT_ATTACK_EFFECTNUM01_GRADE_ELITE_RATE = 3960, // AT_ATTACK_EFFECTNUM01�� Grade Elite ���� �ɸ� Ȯ�� ������ 100%
	AT_ATTACK_EFFECTNUM02_GRADE_ELITE_RATE = 3961,
	AT_ATTACK_EFFECTNUM03_GRADE_ELITE_RATE = 3962,
	AT_ATTACK_EFFECTNUM04_GRADE_ELITE_RATE = 3963,
	AT_ATTACK_EFFECTNUM05_GRADE_ELITE_RATE = 3964,
	AT_ATTACK_EFFECTNUM06_GRADE_ELITE_RATE = 3965,
	AT_ATTACK_EFFECTNUM07_GRADE_ELITE_RATE = 3966,
	AT_ATTACK_EFFECTNUM08_GRADE_ELITE_RATE = 3967,
	AT_ATTACK_EFFECTNUM09_GRADE_ELITE_RATE = 3968,
	AT_ATTACK_EFFECTNUM10_GRADE_ELITE_RATE = 3969,

	AT_ATTACK_EFFECTNUM01_GRADE_BOSS_RATE = 3970, // AT_ATTACK_EFFECTNUM01�� Grade Boss ���� �ɸ� Ȯ�� ������ 100%
	AT_ATTACK_EFFECTNUM02_GRADE_BOSS_RATE = 3971,
	AT_ATTACK_EFFECTNUM03_GRADE_BOSS_RATE = 3972,
	AT_ATTACK_EFFECTNUM04_GRADE_BOSS_RATE = 3973,
	AT_ATTACK_EFFECTNUM05_GRADE_BOSS_RATE = 3974,
	AT_ATTACK_EFFECTNUM06_GRADE_BOSS_RATE = 3975,
	AT_ATTACK_EFFECTNUM07_GRADE_BOSS_RATE = 3976,
	AT_ATTACK_EFFECTNUM08_GRADE_BOSS_RATE = 3977,
	AT_ATTACK_EFFECTNUM09_GRADE_BOSS_RATE = 3978,
	AT_ATTACK_EFFECTNUM10_GRADE_BOSS_RATE = 3979,

	AT_GRADE_NORMAL_PHY_DMG_DEC_RATE	= 3980, // �Ϲ� ���� �ǰݽ� ������ ������ % = ���� ���� �������� %����( ���з� )
	AT_GRADE_UPGRADED_PHY_DMG_DEC_RATE	= 3981, // ��ȭ ���� �ǰݽ� ������ ������ % = ���� ���� �������� %����( ���з� )
	AT_GRADE_ELITE_PHY_DMG_DEC_RATE		= 3982, // ���� ���� �ǰݽ� ������ ������ % = ���� ���� �������� %����( ���з� )
	AT_GRADE_BOSS_PHY_DMG_DEC_RATE		= 3983, // ���� ���� �ǰݽ� ������ ������ % = ���� ���� �������� %����( ���з� )

	AT_GRADE_NORMAL_MAGIC_DMG_DEC_RATE		= 3984, // �Ϲ� ���� �ǰݽ� ������ ������ % = ���� ���� �������� %����( ���з� )
	AT_GRADE_UPGRADED_MAGIC_DMG_DEC_RATE	= 3985, // ��ȭ ���� �ǰݽ� ������ ������ % = ���� ���� �������� %����( ���з� )
	AT_GRADE_ELITE_MAGIC_DMG_DEC_RATE		= 3986, // ���� ���� �ǰݽ� ������ ������ % = ���� ���� �������� %����( ���з� )
	AT_GRADE_BOSS_MAGIC_DMG_DEC_RATE		= 3987, // ���� ���� �ǰݽ� ������ ������ % = ���� ���� �������� %����( ���з� )

	AT_SCREEN_EFFECT_COUNT	= 3998,	//ȭ�鿡 �̹��� ���̴� ������ ����. ��� �Ѹ���. Ŭ���̾�Ʈ ���� ���
	AT_MONSTER_CARD_ABIL_MAX = 3999,

//Skill /Effect Abil 3060 ~ 3999���� ��� ���� ~!!!!!!
		
	AT_APPLY_EFFECT_MSG		= 4000,	// �� �÷��̾ ����Ʈ �ɸ��� ����Ʈ�� �޼����� �����ش�
	AT_USE_EFFECT_MSG_BY_STRINGNO	= 4001,	// �� �÷��̾�� ����Ʈ �ɸ���, �� ���� ��ȣ�� DefString���� ã�� �̸����� ����Ѵ�
	AT_APPLY_EFFECT_MSG_TYPE		= 4002, //AT_APPLY_EFFECT_MSG �޽���Ÿ��(EEffectMsgType)
	AT_CHECK_OVERLAP_MSG	= 4003, //ECheckOverlapMsgType

	AT_GRADE				= 4007,	// Pet/Monster/Item �� ���
	AT_USE_ENERGY_GAUGE_BIG	= 4008,//������ ������ ū�� ������

	AT_BASE_CLASS			= 4009,
	AT_NO_BOSS_HP_HIDE		= 4010, // ������ ������ AliveTime ��� ����
	AT_MON_ENCHANT_GRADE_NO	= 4011,	// ���� ��æƮ ��ȣ
	AT_MON_ENCHANT_LEVEL	= 4012, // ���� ��æƮ ����
	AT_MON_ENCHANT_PROB_NO	= 4013,	// ���� ��æƮ Ȯ�� ��ȣ
	AT_NO_ENCHNAT_MONSTER	= 4014,	// ��æƮ ����ȭ ���� ����

	AT_USE_ACCUMULATE_DAMAGED	= 4030, //���������� ���
	AT_ACCUMULATE_DAMAGED_NOW	= 4031, //���� ����������
	AT_ACCUMULATE_DAMAGED_MAX	= 4032, //�ִ� ����������

// Pet�� Ư���� Ÿ��
	AT_INVEN_PET_ADD_TIME_EXP_RATE		= 4060,	// �ð��� Pet ����ġ �����ÿ� �߰� ����
	AT_INVEN_PET_ADD_HUNT_EXP_RATE		= 4061,	// ������� Pet ����ġ �����ÿ� �߰� ����

	AT_PET_CHOOSE_RAND_COLOR_MAX = 4070, //�� ���� ���� ���(N>0�̸�, ���� �ִ밪. N=0�̸�, �̻��)

	AT_TIME					= 4090,
	AT_EXPERIENCE_TIME		= 4091,//���� �ð��� ��� ����ġ
    AT_ENTITY_SECOND_TYPE   = 4092,//��ƼƼ ������ Ÿ�� (EEntitySecondType����)
	AT_COLOR_INDEX			= 4100,

// Pet Ability
	//AT_HUNGER				= 4101,
	AT_HEALTH				= 4102,
	AT_MENTAL				= 4103,

// Unit�� Ư���� ���� ����������~~~
	AT_DAMAGE_IS_ONE_ALPHA 			= 4198, // AT_DAMAGE_IS_ONE + �߰� ������
	AT_CANNOT_DAMAGE_BY_GUARDIAN	= 4199, // ������� �ش� ������ �������� ���ϰ� �ϴ� ���
	AT_DAMAGE_IS_ONE				= 4200, // �������� 1�� ������ ��
	AT_UNIT_HIDDEN			= 4201,	// �����ִ� ���� (�ٸ� Player���� ������ �ʴ´�)
	AT_ENABLE_AUTOHEAL		= 4202,	// AutoHeal �� ���ΰ�? 0x01:HP, 0x02:MP, 0x04:DP
	AT_FIXED_MOVESPEED		= 4203,	// MoveSpeed ���� ����~~
	AT_CANNOT_DAMAGE		= 4204,	// ���� ���� �ʴ´�.
	AT_FROZEN				= 4205,	// ������ �� ����(�׷��� �Ѵ�� ������ ������ �� �ִ�)
	AT_CANNOT_ATTACK		= 4206, // ���� �� �� ����.
	AT_FROZEN_DMG_WAKE		= 4207,	// Frozen ������ �� Damage�� ������ Frozen ���°� Ǯ�����ΰ�? (>0:Ǯ����.)
	AT_CANNOT_USEITEM		= 4208,	// �������� ��� �� �� ���� �����̴�.
	AT_CANNOT_EQUIP			= 4209,	// ��������(����/����) ������ ����
	AT_CANNOT_CASTSKILL		= 4210,	// Casting Type ��ų ��� ����
	AT_CRITICAL_ONEHIT		= 4211,	// ���� ���ݿ� �־� ������ Critical ������ �ȴ�.
	AT_CANNOT_SEE			= 4212,	// �ֺ��� ���� ���� ��
	AT_QUEST_TALK			= 4213,	// ����Ʈ ��ȭ ���� �÷��̾�� (0: IDLE, 1: Talking)
	//AT_LOOTED_ITEM			= 4214,	// �̹� ������ ������ �׶��� �������̴� (0: None, 1: Looted)
	AT_FAKE_REMOVE_UNIT		= 4215, // �������� �ʿ��� ������ �÷��̾��̴�.(�����δ� HIDE �� CANNOT ATTACK ������ �������� ������ �ʰ� �����Ǿ� ����)
	AT_EVENT_SCRIPT			= 4216,	// Ŭ���̾�Ʈ �� �̺�Ʈ ��ũ��Ʈ�� �������̴�
	AT_EVENT_SCRIPT_TIME	= 4217,	// Ŭ���̾�Ʈ �� �̺�Ʈ ��ũ��Ʈ�� ������ �ð�.
	AT_INVINCIBLE			= 4218, // AT_CANNOT_DAMAGE�� Ÿ���� ���� ������, AT_INVINCIBLE�� ������ ������ �������� 0�� �ȴ�.
	AT_INVINCIBLE2			= 4219,	// AT_INVINCIBLE�� Ʋ������ ��ħ ó���� ���� �ʰ�, �±���� �ϳ� HP�� �ȴܴ�(���� �ı�������Ʈ��  ���)
	AT_SKILL_SPECIFIC_IDLE	= 4220, // Ư�� ���̵��� �Ѵ�.
	AT_ENABLE_CHECK_ATTACK	= 4221, // Ÿ���� ������ �� ���ݿ��θ� üũ���� ����
	AT_CHECK_ATTACK_COUNT	= 4222, // ������ �ߴ��� üũ( �����Ҷ����� ���� )
	AT_DARKNESS				= 4223, // ����(ī���� �� ���� ����Ʈ)
	AT_TIME_LIMIT			= 4224,	// ����(�� �� �ÿ�, �ش� �ʿ� �ð������� �ɸ���)
	AT_NOT_COUNT			= 4225,	// ���۴������� ���� ���� ������ ���Խ�Ű�� �ʴ´�
	AT_AI_SYNC				= 4226,	// AI ��ũ ��û
	AT_AI_FIRE_ONLEAVE_NO_CLEAR_TARGETLIST = 4227, //�ʼ������� AI Fire::OnLeave�ɶ� Ÿ�ϸ���Ʈ�� �ʱ�ȭ �� �� ����
	AT_AI_FIRESKILL_ENTER	= 4228,
	AT_AI_BLOWUP_EFFECT		= 4229,	// PgAIActBlowup::OnLeave �� Unit���� �� ����Ʈ

	AT_ENABLE_SUPER_AMMOR	= 4230, // ���� �ƸӰ� ���� ���ΰ�?(���� �ƸӴ� ���׼� �� �Ϻ� ���׼��� ���� �ϴ� ���)
	AT_ENABLE_RATE_SUPER_AMMOR = 4231, // ���� �ƸӰ� �ߵ��� Ȯ��(���з�)
	
	AT_SKILL_SPECIFIC_RUN = 4232,	//Ư�� ���� �Ѵ�
	AT_SKILL_SPECIFIC_DASH = 4233,	// Ư�� ��ø� �Ѵ�
	AT_SKILL_SPECIFIC_JUMP = 4234,	// Ư�� ������ �Ѵ�

	AT_NEWCHARACTER			= 4301, // �̺�Ʈ ��� ���� ������ ĳ�� ó�� ���� üũ��

	AT_CALL_MARKET			= 4401,	// ���� ���� ����(�ʵ忡����)
	AT_CALL_SAFE			= 4402,	// â�� ���� ����(�ʵ忡����)
	AT_PROTECTION_GALL		= 4403, // ������ �Һ� ����
	AT_CALL_STORE			= 4404,	// ���� ���� ����(�ʵ忡����)

	AT_EQUIP_LEVELLIMIT_MIN	= 4405,	// ������ ���� ���� ����
	
	// �� ������ ����Ǿ��� ���� �ȿ� �ִ� ��� ��ȣ�� ������� ����!! (4406~4435)
	
	AT_EQUIP_LEVELLIMIT_MAX	= 4436,

	AT_CALL_SHARE_SAFE		= 4437,// ���� â�� ȣ��
	AT_CANNOT_EQUIP_ACHIEVEMENT				= 4438, //���� ���� ���� ���
	
// ä�� 3�� ��ɼ� ������ ���(1���� ������� ���� ���) --> ĳ���� ���� �����۷��� ��밡��
	AT_JS_1ST_ADD_SKILL_TURNTIME			= 4500,	// ����
	AT_JS_1ST_ADD_USE_EXHAUSTION			= 4501, // ����
	AT_JS_1ST_ADD_TOOL_USE_DURATION_RATE	= 4502, // Ȯ�� ����
	AT_JS_1ST_ADD_RESULT_ITEM_COUNT			= 4503, // ����
	AT_JS_1ST_ADD_EXPERTNESS_RATE			= 4504, // ���
	
// ���� ��ų ����
	AT_JS_1ST_SUB_ADD_SKILL_TURNTIME			= 4510,	// ����
	AT_JS_1ST_SUB_ADD_USE_EXHAUSTION			= 4511, // ����
	AT_JS_1ST_SUB_ADD_TOOL_USE_DURATION_RATE	= 4512, // Ȯ�� ����
	AT_JS_1ST_SUB_ADD_RESULT_ITEM_COUNT			= 4513, // ����
	AT_JS_1ST_SUB_ADD_EXPERTNESS_RATE			= 4514, // ���

// ä�� 3�� ��ɼ� ������ ���(2���� ������� ���� ���) --> ��ġ ���������� ��� ����
	AT_JS_2ND_ADD_UPGRADE_TURN_TIME			= 4550, // ���
	AT_JS_2ND_NO_TROUBLE					= 4551,	// True or False (0 or != 0)
	AT_JS_2ND_ADD_EXPERTNESS				= 4552,	// ���
	AT_JS_2ND_AUTO_COMPLETE_SEND_MAIL		= 4553, // True or False (0 or != 0)
	AT_JS_2ND_ADD_TOOL_USE_DURATION			= 4554, // ���(������)

//��ȯ ��ų ����
	AT_SKILL_SUMMON_NEAR_BY_ENEMY			= 4701, //��ȯ ��ų ���� ���� ������ ��ġ�� ����
// ���� ��Ȱ 
	AT_COMBAT_REVIVE_DEL_EFFECT_NO			= 4710, // ���� ��Ȱ�� �����ؾ��� ����Ʈ
	AT_COMBAT_REVIVE_ADD_EFFECT_NO			= 4720, // ���� ��Ȱ�� �߰����� ����Ʈ
// ITEM
	ATI_EXPLAINID			= 5001,	// Item Tooltip text id
	AT_EQUIP_LIMIT			= 5002,
	AT_CHARACTERFILTER		= 5003,
	AT_ITEMFILTER			= 5004,
	AT_GENDERLIMIT			= 5005,
	//AT_PRICE				= 5006,	//�����ʵ�� �̵�
	//AT_SELLPRICE			= 5007,	//�����ʵ�� �̵�
	AT_PVPSELL				= 5008,
	AT_ATTRIBUTE			= 5009,	// Item(EItemCantModifyEventType,�ŷ�����), ObjUnit(EObjUnit_Attr,�ɷ�ġ)
	AT_MAX_LIMIT			= 5010,	// �κ����Կ� ������ �ִ� �ִ밳��
	AT_WEAPON_TYPE			= 5011,
	AT_HAIR_COLOR			= 5012,
	AT_HAIRBRIGHTNESS		= 5013,
	AT_DEFAULT_AMOUNT		= 5014,	// ���� ���. ������� 0->������(�ִ볻������), 1 ->����(������ ����� �� �⺻������ �����Ǵ� ����)
	AT_PRIMARY_INV			= 5015,	// �����κ��丮
	AT_VIRTUAL_ITEM			= 5016,	// ����, �κ��� ���� �ʴ� ���� ������.
	AT_ITEMMAKING_NO		= 5017, // ������ ���� �������� ���. �� �������� �� �� �ִ� ������ ��ȣ.
	AT_MAKING_TYPE			= 5018, // ������ Ÿ��, (�丮, ����, ���� ��...)
	AT_USE_ITEM_CUSTOM_TYPE		= 5019, // ����ϴ� �������� Ư���ϰ� �۾��� �ϴ°� �ִ���.
	AT_USE_ITEM_CUSTOM_VALUE_1	= 5020, // 
	//AT_ITEM_CANT_EVENT			= 5021,	// �������� �������� ���� �̺�Ʈ ����(�ŷ�, �Ǹ� ��)
	AT_ITEM_QUEST_CARD_GRADE	= 5022,
	AT_USE_ITEM_CUSTOM_VALUE_2	= 5023,
	AT_USE_ITEM_CUSTOM_VALUE_3	= 5024,
	AT_PET_INV				= 5025,	// �꿡 ������ �κ��丮
	AT_USE_ITEM_CUSTOM_VALUE_4	= 5026,

	AT_ITEM_SORT_ORDER1		= 5027,
	AT_ITEM_SORT_ORDER2		= 5028,
	AT_ITEM_SORT_ORDER3		= 5029,

	AT_DROP_TRANSFORM_TYPE	= 5030,	//��ӵ� ���� SRT Ÿ��
	AT_ITEM_USE_STATUS		= 5031, //�������� ����Ҽ� �ִ� ������ ���°� 
	AT_ITEM_USE_CONFIRM_TXT	= 5032, //�������� ����Ҷ� ��� �ؽ�Ʈ ID
	AT_ITEM_DISPLAY_GRADE	= 5033,
	AT_MISSIONITEM_LEVELTYPE    = 5034, //�̼����� ������ ���� üũ
	AT_MISSIONITEM_MISSION_NO	= 5035, //�̼����� ������ �̼ǹ�ȣ üũ
	AT_FURNITURE_TYPE		= 5040, //����Ȩ�� ���Ǵ� �����۵��� Ÿ��.
	AT_HOME_WALL_SUB_NO		= 5041, //����Ȩ ���� ���� �������� �κ��� ������ ��ȣ

	AT_COSTUME_GRADE		= 5042,	// Ŀ��Ƭ ���

	AT_ALWAYS_DROP_ITEM_1	= 5050,	// ���ͷκ��� �ݵ�� ����Ǿ� �ϴ� ������(�׻� ī��Ʈ 1)
	AT_ALWAYS_DROP_ITEM_2	= 5051,	
	AT_ALWAYS_DROP_ITEM_3	= 5052,	
	AT_ALWAYS_DROP_ITEM_4	= 5053,	
	AT_ALWAYS_DROP_ITEM_5	= 5054,	
	AT_ALWAYS_DROP_CUSTOM_ITEM_1 = 5055,	//��񿡼� �Է����� �ʰ� �ڵ忡�� ���� �Է��ϴ� ��
	AT_ALWAYS_DROP_CUSTOM_ITEM_2 = 5056,	//��񿡼� �Է����� �ʰ� �ڵ忡�� ���� �Է��ϴ� ��
	AT_ALWAYS_DROP_CUSTOM_ITEM_3 = 5057,	//��񿡼� �Է����� �ʰ� �ڵ忡�� ���� �Է��ϴ� ��
	AT_ALWAYS_DROP_CUSTOM_ITEM_4 = 5058,	//��񿡼� �Է����� �ʰ� �ڵ忡�� ���� �Է��ϴ� ��
	AT_ALWAYS_DROP_CUSTOM_ITEM_5 = 5059,	//��񿡼� �Է����� �ʰ� �ڵ忡�� ���� �Է��ϴ� ��

	AT_MAPMOVE_DELETE		= 5060, // ���̵� �ϸ� ������� ������, ����Ʈ
	AT_PICKUP_USEITEM		= 5061,	// ������ PickUp�ÿ� �κ����� �����ʰ� �ڵ����� ���Ǿ� ���� ������
	AT_BREAKCOUPLE_DELETE_ITEM	= 5062, // Ŀ��(�κ�) ������ ������� �����۵� ����

	AT_ALWAYS_DROP_CUSTOM_ITEM_COUNT_1 = 5100,	//��񿡼� �Է����� �ʰ� �ڵ忡�� ���� �Է��ϴ� ��
	AT_ALWAYS_DROP_CUSTOM_ITEM_COUNT_2 = 5101,	//��񿡼� �Է����� �ʰ� �ڵ忡�� ���� �Է��ϴ� ��
	AT_ALWAYS_DROP_CUSTOM_ITEM_COUNT_3 = 5102,	//��񿡼� �Է����� �ʰ� �ڵ忡�� ���� �Է��ϴ� ��
	AT_ALWAYS_DROP_CUSTOM_ITEM_COUNT_4 = 5103,	//��񿡼� �Է����� �ʰ� �ڵ忡�� ���� �Է��ϴ� ��
	AT_ALWAYS_DROP_CUSTOM_ITEM_COUNT_5 = 5104,	//��񿡼� �Է����� �ʰ� �ڵ忡�� ���� �Է��ϴ� ��

	AT_SPEED_HACK_COUNT		= 5500,	//�ӵ� ��ŷ�� �� ī��Ʈ.
	AT_SKILL_MAXIMUM_SPEED	= 5501,	//�� ��ų�� �ִ� �ӵ�. �� �ӵ� �̻��̸� ��ŷ��

	AT_CHECK_EXIST_ENDQUEST_MIN = 5600,	// �ش� ����Ʈ�� �Ϸ� �ؾ� ��� ���� �ϴ�.
	AT_CHECK_EXIST_ENDQUEST_MAX = 5602,	// �ش� ����Ʈ�� �Ϸ� �ؾ� ��� ���� �ϴ�.

	AT_CHECK_WANT_ENDQUEST_MIN = 5605,	// �ش� ����Ʈ�� �Ϸ� ���� �ʾƾ� ��� ���� �ϴ�.
	AT_CHECK_WANT_ENDQUEST_MAX = 5607,	// �ش� ����Ʈ�� �Ϸ� ���� �ʾƾ� ��� ���� �ϴ�.

	AT_CHECK_EXIST_INGQUEST_MIN = 5610, // �ش� ����Ʈ�� �������̾�߸� ��� ���� �ϴ�.
	AT_CHECK_EXIST_INGQUEST_MAX = 5612, // �ش� ����Ʈ�� �������̾�߸� ��� ���� �ϴ�.

	AT_CHECK_WANT_INGQUEST_MIN = 5615,	// �ش� ����Ʈ�� ���� ���̸� ��� �Ұ��� �ϴ�.
	AT_CHECK_WANT_INGQUEST_MAX = 5617,	// �ش� ����Ʈ�� ���� ���̸� ��� �Ұ��� �ϴ�.

	AT_GAMBLE_GRADE		= 5620,			// �׺� ������ ���

	//AT_RARE_OPTION_GROUP = 5621,		// ���� ������ �ɼ� ���(�Ϲ� : 0)
	AT_RARE_OPTION_MAGIC = 5622,		// ���� ������ �ɼ� ���� �ɼ�
	AT_RARE_OPTION_SKILL = 5623,		// ���� ������ �ɼ� ��ų �ɼ�

	AT_ENABLE_USE_START_TIME = 5624,	// �Ϸ��� AT_ENABLE_USE_START_TIME <-> AT_ENABLE_USE_END_TIME ���̿� ����� �����ϴ�.
	AT_ENABLE_USE_END_TIME = 5625,		// �Ϸ��� AT_ENABLE_USE_START_TIME <-> AT_ENABLE_USE_END_TIME ���̿� ����� �����ϴ�.

	AT_ENABLE_USE_TIMELIMIT = 5626,		// �Ⱓ�� ������ ������ �Һ� �������̴�.(����� ���� �⺻������ ���Ⱓ ������ �����ϴ�.)

	AT_HELP_END_QUEST1	= 5630,			// �Ϸ� ������ ���·� ���� ����Ʈ #1 (�ش� ����Ʈ�� �ݵ�� ���� ���̾�� �Ѵ�) (Value = QuestID)
	AT_HELP_END_QUEST2	= 5631,			// �Ϸ� ������ ���·� ���� ����Ʈ #2 (�ش� ����Ʈ�� �ݵ�� ���� ���̾�� �Ѵ�) (Value = QuestID)
	AT_HELP_END_QUEST3	= 5632,			// �Ϸ� ������ ���·� ���� ����Ʈ #3 (�ش� ����Ʈ�� �ݵ�� ���� ���̾�� �Ѵ�) (Value = QuestID)
	AT_HELP_END_QUEST4	= 5633,			// �Ϸ� ������ ���·� ���� ����Ʈ #4 (�ش� ����Ʈ�� �ݵ�� ���� ���̾�� �Ѵ�) (Value = QuestID)
	
// ���� �����ۿ��� Ÿ�ݽ� �߻��� ����Ʈ�� ����
	AT_ATTACK_EFFECTNUM01_FROM_POTION					 = 5700, //5700~5749 ���ݽÿ� ���Ǵ� ����Ʈ Ȯ���ȣ( Ư�� ��ų �ߵ��� ���ݸ��� ����Ʈ�� �Ŵ� ��� )
	AT_ATTACK_EFFECTNUM50_FROM_POTION					 = 5749,
	AT_ATTACK_EFFECTNUM01_RATE_FROM_POTION				 = 5750, //5750~5799 AT_ATTACK_EFFECTNUM01_FROM_POTION�� �ɸ� Ȯ�� ������ 100% ( ��� ��޿� �����ϰ� ���� �Ǵ� ���)
	AT_ATTACK_EFFECTNUM50_RATE_FROM_POTION				 = 5799,
	AT_ATTACK_EFFECTNUM01_TARGET_TYPE_IS_ME_FROM_POTION	 = 5800, //5800~5849 �ش� ��ȣ�� ����Ʈ�� �ڽſ��� �ɾ��ְ� �ʹٸ� 1�� ����
	AT_ATTACK_EFFECTNUM50_TARGET_TYPE_IS_ME_FROM_POTION	 = 5849,
	
// ��� �����ۿ��� Ÿ�ݽ� �߻��� ����Ʈ�� ����
	AT_ATTACK_EFFECTNUM01_FROM_EQUIP					 = 5850, //5850~5899 ���ݽÿ� ���Ǵ� ����Ʈ Ȯ���ȣ( Ư�� ��ų �ߵ��� ���ݸ��� ����Ʈ�� �Ŵ� ��� )
	AT_ATTACK_EFFECTNUM50_FROM_EQUIP					 = 5899,
	AT_ATTACK_EFFECTNUM01_RATE_FROM_EQUIP				 = 5900, //5900~5949 AT_ATTACK_EFFECTNUM01_FROM_EQUIP�� �ɸ� Ȯ�� ������ 100% ( ��� ��޿� �����ϰ� ���� �Ǵ� ���)
	AT_ATTACK_EFFECTNUM50_RATE_FROM_EQUIP				 = 5949,
	AT_ATTACK_EFFECTNUM01_TARGET_TYPE_IS_ME_FROM_EQUIP	 = 5950, //5950~5999 �ش� ��ȣ�� ����Ʈ�� �ڽſ��� �ɾ��ְ� �ʹٸ� 1�� ����
	AT_ATTACK_EFFECTNUM50_TARGET_TYPE_IS_ME_FROM_EQUIP	 = 5999,

// Boss Ability
	AT_AI_DELAY			= 6001,
	AT_HP_GAGE			= 6002,
	AT_C_HP_GAGE		= 6003,
	AT_CURRENT_ACTION	= 6004,
	AT_MONSTER_APPEAR	= 6005,
	AT_MONSTER_DEAD		= 6006,

// Ư���� Ÿ��
	AT_HP_GAUGE_TYPE		= 6010,
	AT_USENOT_SMALLAREA		= 6011,		// SmallArea�� ������� ����
	AT_ADDED_GAUGE_VALUE	= 6012,		// Ư�� ������ ���
	AT_ADDED_GAUGE_GROUP	= 6013,		// Ư�� ������ �׷�(�� �׷��ȣ�� ���� ������ �ش�)
	AT_HIDE_HP_GAGE			= 6014,		// HP �������� �����
	AT_OPTION_HP_GAGE		= 6015,		// HP �������ɼ�(�׻��� ��)
	AT_POT_PARTICLE_ACTION	= 6016,		// ��Ʈ�������̳�, �����۵����� Ư�� �׼ǽÿ��� ��ƼŬ�� �ٴ� �׼�
	AT_NOSHOWDAMAGENUM		= 6017,		// ApplyActionEffects()���� ShowDamageNum ��� ����(1:����������)

// ��� ���� ���
	AT_TALK_IDLE			= 6020,		// �⺻ ����
	AT_TALK_BIDLE			= 6021,		// ���� �⺻ ����
	AT_TALK_ATTACK			= 6022,		// ���� ����
	AT_TALK_HIT				= 6023,		// ������ ����
	AT_TALK_MOVE			= 6024,		// �̵� ����
	AT_MONBAGNO				= 6025,		// ���� �� ��ȣ ���� ���

//����Ȩ ���
	AT_MYHOME_TODAYHIT		= 6101,		// ����Ȩ ���� �湮�ڼ�
	AT_MYHOME_TOTALHIT		= 6102,		// ����Ȩ �� �游�� ��
	AT_MYHOME_ROOM_X		= 6103,		// ����Ȩ �ٴ� ������
	AT_MYHOME_ROOM_Y		= 6104,
	AT_MYHOME_VISITFLAG		= 6105,		// ����Ȩ �湮�� �÷���
	AT_MYHOME_STATE			= 6106,		// ���� ������ ��� �����ΰ�?
	AT_MYHOME_NAMEPLATE_NO	= 6107,		// ���� ������ ��ȣ
	AT_MYHOME_NPC_CLASS_NO	= 6108,		// ����Ȩ NPC Ŭ���� ��ȣ
	AT_MYHOME_VISITLOGCOUNT	= 6109,		// ����Ȩ �湮�� ��� ī��Ʈ

	AT_N_ATTACK_EFFECT_NO_MIN		= 6110,	// �Ϲ� ���ݽ� �߻� ����Ʈ ��� ����
	AT_N_ATTACK_EFFECT_NO_MAX		= 6119,	// �Ϲ� ���ݽ� �߻� ����Ʈ ��� ������
	AT_N_ATTACK_EFFECT_RATE_MIN		= 6120,	// �Ϲ� ���ݽ� �߻� ����Ʈ �߻�Ȯ�� ��� ����
	AT_N_ATTACK_EFFECT_RATE_MAX		= 6129,	// �Ϲ� ���ݽ� �߻� ����Ʈ �߻�Ȯ�� ��� ������
	AT_N_ATTACK_EFFECT_TARGET_MIN	= 6130,	// �Ϲ� ���ݽ� �߻� ����Ʈ Ÿ�� ��� ����
	AT_N_ATTACK_EFFECT_TARGET_MAX	= 6139,	// �Ϲ� ���ݽ� �߻� ����Ʈ Ÿ�� ��� ������

	AT_C_ATTACK_EFFECT_NO_MIN		= 6140,	// ũ�� ���ݽ� �߻� ����Ʈ ��� ����
	AT_C_ATTACK_EFFECT_NO_MAX		= 6149,	// ũ�� ���ݽ� �߻� ����Ʈ ��� ������
	AT_C_ATTACK_EFFECT_RATE_MIN		= 6150,	// ũ�� ���ݽ� �߻� ����Ʈ �߻�Ȯ�� ��� ����
	AT_C_ATTACK_EFFECT_RATE_MAX		= 6159,	// ũ�� ���ݽ� �߻� ����Ʈ �߻�Ȯ�� ��� ������
	AT_C_ATTACK_EFFECT_TARGET_MIN	= 6160,	// ũ�� ���ݽ� �߻� ����Ʈ Ÿ�� ��� ����
	AT_C_ATTACK_EFFECT_TARGET_MAX	= 6169,	// ũ�� ���ݽ� �߻� ����Ʈ Ÿ�� ��� ������

	AT_BEATTACKED_EFFECT_NO_MIN		= 6170,	// �ǰݽ� �߻� ����Ʈ ��� ����
	AT_BEATTACKED_EFFECT_NO_MAX		= 6179,	// �ǰݽ� �߻� ����Ʈ ��� ������
	AT_BEATTACKED_EFFECT_RATE_MIN	= 6180,	// �ǰݽ� �߻� ����Ʈ �߻�Ȯ�� ��� ����
	AT_BEATTACKED_EFFECT_RATE_MAX	= 6189,	// �ǰݽ� �߻� ����Ʈ �߻�Ȯ�� ��� ������
	AT_BEATTACKED_EFFECT_TARGET_MIN = 6190,	// �ǰݽ� �߻� ����Ʈ Ÿ�� ��� ����
	AT_BEATTACKED_EFFECT_TARGET_MAX = 6199,	// �ǰݽ� �߻� ����Ʈ Ÿ�� ��� ������

	AT_BLOCKED_EFFECT_NO_MIN		= 6200,	// ���� ���� �߻� ����Ʈ ��� ����
	AT_BLOCKED_EFFECT_NO_MAX		= 6209,	// ���� ���� �߻� ����Ʈ ��� ������
	AT_BLOCKED_EFFECT_RATE_MIN		= 6210,	// ���� ���� �߻� ����Ʈ �߻�Ȯ�� ��� ����
	AT_BLOCKED_EFFECT_RATE_MAX		= 6219,	// ���� ���� �߻� ����Ʈ �߻�Ȯ�� ��� ������
	AT_BLOCKED_EFFECT_TARGET_MIN	= 6220,	// ���� ���� �߻� ����Ʈ Ÿ�� ��� ����
	AT_BLOCKED_EFFECT_TARGET_MAX	= 6229,	// ���� ���� �߻� ����Ʈ Ÿ�� ��� ������

	AT_DEFAULT_ITEM_RARITY			= 6300,	// ������ ������ �⺻���� �־����� ���
	AT_DEFAULT_ITEM_ENCHANT_TYPE	= 6301,	// ������ ������ �⺻���� �־����� ��þƮ Ÿ��
	AT_DEFAULT_ITEM_ENCHANT_LEVEL	= 6302, // ������ ������ �⺻���� �־����� ��þƮ ���
	AT_DEFAULT_ITEM_OPTION_TYPE1	= 6303,	// ������ ������ �⺻���� �־����� �ɼ� Ÿ��
	AT_DEFAULT_ITEM_OPTION_LEVEL1	= 6304, // ������ ������ �⺻���� �־����� �ɼ� ���
	AT_DEFAULT_ITEM_OPTION_TYPE2	= 6305,	// ������ ������ �⺻���� �־����� �ɼ� Ÿ��
	AT_DEFAULT_ITEM_OPTION_LEVEL2	= 6306, // ������ ������ �⺻���� �־����� �ɼ� ���
	AT_DEFAULT_ITEM_OPTION_TYPE3	= 6307,	// ������ ������ �⺻���� �־����� �ɼ� Ÿ��
	AT_DEFAULT_ITEM_OPTION_LEVEL3	= 6308, // ������ ������ �⺻���� �־����� �ɼ� ���
	AT_DEFAULT_ITEM_OPTION_TYPE4	= 6309,	// ������ ������ �⺻���� �־����� �ɼ� Ÿ��
	AT_DEFAULT_ITEM_OPTION_LEVEL4	= 6310, // ������ ������ �⺻���� �־����� �ɼ� ���
	AT_DEFAULT_ITEM_CURSE			= 6311, // ������ ������ ���� ����
	AT_DEFAULT_ITEM_SEAL			= 6312,	// ������ ������ ���� ����

	AT_HOME_SIDEJOB_GOLD			= 6320,
	AT_HOME_SIDEJOB_SOUL			= 6321,
	AT_HOME_SIDEJOB_RATE			= 6322,

	AT_ELITEPATTEN_STATE		= 6330,	// ���� ���� ����
	AT_GROGGY_NOW				= 6331,	// �г�/�׷α� ���簪(0�̸� �г�->�г���� ����, �׷α�->�Ϲݻ��·� ��ȯ)
	AT_FRENZY_NOW_FIRE			= 6332,	// ������ �г�ߵ� ��
	AT_FRENZY_MAX_FIRE			= 6333,	// �г�ߵ� �Ӱ谪(AT_C_MAX_HP * AT_FRENZY_R_FIRE)
	AT_GROGGY_RELAX_TIME		= 6334,	// �׷α� ��ȭ �����ð�
	AT_ELITEPATTEN_NEXTSTATE	= 6335, // ������ ������ ���� ����
	AT_FRENZY_TIME				= 6336,	// �г������ð�(0�̸� �Ϲݻ��·� ����)(AT_GROGGY_MAX / AT_FRENZY_RECOVERY����)
	AT_EP_USE_EFFECT_NO			= 6337, // ���� ���Ͽ��� ������¿� �ɸ� ����Ʈ��ȣ
	AT_ELITEPATTEN_INITSTATE	= 6338, // ������ ���� �ʱ�ȭ����
	AT_SEND_UNIT_STATE			= 6339, // ���������� ������ ����

	AT_CREATE_SUMMONED_HP_RATE		= 6400, // ��ȯ�� �������� HP �����
	AT_CREATE_SUMMONED_ATTACK_RATE	= 6401, // ��ȯ�� �������� ���ݷ� �����
	AT_CREATE_SUMMONED_SUPPLY		= 6410, // ��ȯü �ʿ� ���ö��̼�
	AT_CREATE_UNIQUE_SUMMONED		= 6411, // ������ ��ȯü ����
	AT_CREATE_SUMMONED_IGNORE_SUMMON_EFFECT = 6412, // ��ȯü ��ȯ�� ���� ������ ���� ����Ʈ�� �ɶ�(�����ڿ���), �� ����� �ִ� ��ȯü�� ���� ������ ī��Ʈ ���� �ʰ� �ϴ� ���
	AT_SHARE_DMG_WITH_SUMMONED		= 6413, // ��ȯü�� �������� �л��Ͽ� �޴� �� ( ���з�, ex) 1000 �̶��, ��ȯ�ڴ�  90%�� �������� �ް�, ��ȯ���� (10%�� ������/��ȯü �Ѽ�) ��ŭ ������ ����
	AT_CREATE_SUMMONED_MAX_COUNT	= 6414, // ��ȯü ���� ��ȯ ����(0: ������, 1�̻���ʹ� �ִ��)

	AT_SUMMONED_REGAIN_HP_RATE		= 6420, // ��ȯü �� ���ν� 1ü�´� ȸ���� ü�·�
	AT_SUMMONED_REGAIN_MP_RATE		= 6421, // ��ȯü �� ���ν� 1ü�´� ȸ���� ������
	AT_SUMMONED_REGAIN_ENABLE		= 6422, // ��ȯü �� ���ν� ����� Ÿ��(HP=0x01, MP=0x02)

	AT_SUMMON_EFFECT_LEARN_CHECK_SKILL_NO = 6430, // ��ȯü�� �� ���� �ΰ��� ���� ����Ʈ�� �Ŵ�(�����ڿ���) ������ �ϱ����� ������� üũ�ؾ� �ϴ� ��ų ��ȣ
	AT_SUMMON_EFFECT_BASE_EFFECT_NO		  = 6431, // AT_SUMMON_EFFECT_LEARN_CHECK_SKILL_NO�� ����, �� ����� ����Ʈ ��ȣ�� �������� ����Ʈ�� �Ǵ�
	AT_SUMMON_EFFECT_MAX_EFFECT_CNT		  = 6432, // AT_SUMMON_EFFECT_BASE_EFFECT_NO�� �� �ϴ� �ִ밪
	
	AT_CREATE_HP_TYPE			= 6440, // ���� ������ HP ����Ǵ� Ÿ��(ECREATE_HP_TYPE)
	AT_ELGA_BONE_MON_HP			= 6441, // ���� ��ȣ ���� ������ HP
	AT_ELGA_EYE_MON_HP			= 6442, // ���� ������ ���� ������ HP ����
	AT_ELGA_EYE_MON_HP_TIME		= 6443, // ���� ������ ���� ������ HP ��Ͻð�
	AT_ELGA_EYE_MON_AUTO_HP		= 6444, // �ʴ������ HP ��
	
	AT_ELGA_STONE_RATE			= 6500, // �������� �����ı��� �ϳ��� ��ȣ���� ����� ���з�
	AT_ELGA_SUMMOND_HEART_RATE	= 6501, // ���� HP�� ���� ������ �濡 ��ȯ�Ǵ� ���͸� �ٸ��� �ϱ� ���� �뵵

	AT_CALCUATEABIL_MIN			= 7000,
	AT_MAX_HP					= 7001, // �⺻�� or ���밪 (+)���ִ� ��
	AT_R_MAX_HP					= 7002, // _R_ == õ������
	AT_C_MAX_HP					= 7003, // _C_ = ������
	AT_HP_RECOVERY_INTERVAL		= 7011,	// Auto Heal �Ǵ� �ֱ�
	AT_R_HP_RECOVERY_INTERVAL	= 7012,
	AT_C_HP_RECOVERY_INTERVAL	= 7013,
	AT_MAX_MP					= 7021,
	AT_R_MAX_MP					= 7022,
	AT_C_MAX_MP					= 7023,
	AT_MP_RECOVERY_INTERVAL		= 7031,
	AT_R_MP_RECOVERY_INTERVAL	= 7032,
	AT_C_MP_RECOVERY_INTERVAL	= 7033,
	AT_STR						= 7041,
	AT_R_STR					= 7042,
	AT_C_STR					= 7043,
	AT_INT						= 7051,
	AT_R_INT					= 7052,
	AT_C_INT					= 7053,
	AT_CON						= 7061,
	AT_R_CON					= 7062,
	AT_C_CON					= 7063,
	AT_DEX						= 7071,
	AT_R_DEX					= 7072,
	AT_C_DEX					= 7073,
	AT_MOVESPEED				= 7081,
	AT_R_MOVESPEED				= 7082,
	AT_C_MOVESPEED				= 7083,
	AT_MOVESPEED_SAVED			= 7084,
	AT_PHY_DEFENCE				= 7091,
	AT_R_PHY_DEFENCE			= 7092,
	AT_C_PHY_DEFENCE			= 7093,
	AT_AMP_PHY_DEFENCE			= 7094,	// _AMP_ : �ɼ� ���� ���� �Ǳ� �� ��.
	AT_MAGIC_DEFENCE			= 7101,
	AT_R_MAGIC_DEFENCE			= 7102,
	AT_C_MAGIC_DEFENCE			= 7103,
	AT_AMP_MAGIC_DEFENCE		= 7104,
	AT_ATTACK_SPEED				= 7111,
	AT_R_ATTACK_SPEED			= 7112,
	AT_C_ATTACK_SPEED			= 7113,
	AT_BLOCK_SUCCESS_VALUE				= 7121,
	AT_R_BLOCK_SUCCESS_VALUE				= 7122,
	AT_C_BLOCK_SUCCESS_VALUE				= 7123,
	AT_DODGE_SUCCESS_VALUE				= 7131,
	AT_R_DODGE_SUCCESS_VALUE				= 7132,
	AT_C_DODGE_SUCCESS_VALUE				= 7133,
	AT_CRITICAL_SUCCESS_VALUE			= 7141,
	AT_R_CRITICAL_SUCCESS_VALUE			= 7142,
	AT_C_CRITICAL_SUCCESS_VALUE			= 7143,
	AT_CRITICAL_POWER			= 7151,
	AT_R_CRITICAL_POWER			= 7152,
	AT_C_CRITICAL_POWER			= 7153,
	AT_INVEN_SIZE				= 7161,
	AT_R_INVEN_SIZE				= 7162,
	AT_C_INVEN_SIZE				= 7163,
	AT_EQUIPS_SIZE				= 7171,
	AT_R_EQUIPS_SIZE			= 7172,
	AT_C_EQUIPS_SIZE			= 7173,
	AT_ATTACK_RANGE				= 7181,	// Item, Skill
	AT_R_ATTACK_RANGE			= 7182,
	AT_C_ATTACK_RANGE			= 7183,
	AT_HP_RECOVERY				= 7191,	// Amount of Auto Heal
	AT_R_HP_RECOVERY			= 7192,
	AT_C_HP_RECOVERY			= 7193,
	AT_MP_RECOVERY				= 7201,
	AT_R_MP_RECOVERY			= 7202,
	AT_C_MP_RECOVERY			= 7203,
	AT_JUMP_HEIGHT				= 7211,
	AT_R_JUMP_HEIGHT			= 7212,
	AT_C_JUMP_HEIGHT			= 7213,
	AT_PHY_ATTACK_MAX			= 7221,	// Equip,Monster,NPC
	AT_R_PHY_ATTACK_MAX			= 7222,
	AT_C_PHY_ATTACK_MAX			= 7223,
	AT_AMP_PHY_ATTACK_MAX		= 7224,
	AT_PHY_ATTACK_MIN			= 7231, // Equip,Monster,NPC
	AT_R_PHY_ATTACK_MIN			= 7232,
	AT_C_PHY_ATTACK_MIN			= 7233,
	AT_AMP_PHY_ATTACK_MIN		= 7234,
	AT_TARGET_BACK_DISTANCE 	= 7241,//�и��� ����
	AT_TARGET_FLY_DISTANCE		= 7251,//�ߴ� ����.
	AT_NEED_MP					= 7261,	// Skill ��� �� �� �ʿ��� MP
	AT_R_NEED_MP				= 7262,
	AT_C_NEED_MP				= 7263,
	AT_NEED_HP					= 7271,
	AT_R_NEED_HP				= 7272,
	AT_C_NEED_HP				= 7273,
	AT_MAGIC_ATTACK				= 7281,
	AT_R_MAGIC_ATTACK			= 7282,
	AT_C_MAGIC_ATTACK			= 7283,
	AT_PHY_ATTACK				= 7291,
	AT_R_PHY_ATTACK				= 7292,
	AT_C_PHY_ATTACK				= 7293,
	AT_CRITICAL_MPOWER			= 7301,
	AT_R_CRITICAL_MPOWER		= 7302,
	AT_C_CRITICAL_MPOWER		= 7303,
	AT_HIT_SUCCESS_VALUE		= 7311, // ��ų : Casting�� ������ Ȯ��
	AT_R_HIT_SUCCESS_VALUE		= 7312,
	AT_C_HIT_SUCCESS_VALUE		= 7313,
	AT_WALK_SPEED				= 7321,	// �ȴ� �ӵ�
	AT_R_WALK_SPEED				= 7322,
	AT_C_WALK_SPEED				= 7323,
	AT_MAGIC_ATTACK_MAX			= 7331,
	AT_R_MAGIC_ATTACK_MAX		= 7332,
	AT_C_MAGIC_ATTACK_MAX		= 7333,
	AT_AMP_MAGIC_ATTACK_MAX		= 7334,
	AT_MAGIC_ATTACK_MIN			= 7341,
	AT_R_MAGIC_ATTACK_MIN		= 7342,
	AT_C_MAGIC_ATTACK_MIN		= 7343,
	AT_AMP_MAGIC_ATTACK_MIN		= 7344,
	AT_PHY_DMG_DEC				= 7351,	// ����Damage ������
	AT_R_PHY_DMG_DEC			= 7352,
	AT_C_PHY_DMG_DEC			= 7353,
	AT_MAGIC_DMG_DEC			= 7361,	// ����Damage ������
	AT_R_MAGIC_DMG_DEC			= 7362,
	AT_C_MAGIC_DMG_DEC			= 7363,
	AT_CASTING_SPEED			= 7371,	// Casting Speed
	AT_R_CASTING_SPEED			= 7372,
	AT_C_CASTING_SPEED			= 7373,
	AT_RESIST_FIRE				= 7381,	// �Ӽ�����(ȭ)
	AT_R_RESIST_FIRE			= 7382,
	AT_C_RESIST_FIRE			= 7383,
	AT_RESIST_WATER				= 7391,	// �Ӽ�����(��)
	AT_R_RESIST_WATER			= 7392,
	AT_C_RESIST_WATER			= 7393,
	AT_RESIST_TREE				= 7401,	// �Ӽ�����(��)
	AT_R_RESIST_TREE			= 7402,
	AT_C_RESIST_TREE			= 7403,
	AT_RESIST_IRON				= 7411,	// �Ӽ�����(��)
	AT_R_RESIST_IRON			= 7412,
	AT_C_RESIST_IRON			= 7413,
	AT_RESIST_EARTH				= 7421,	// �Ӽ�����(��)
	AT_R_RESIST_EARTH			= 7422,
	AT_C_RESIST_EARTH			= 7423,
	AT_HP_POTION_ADD_RATE		= 7431,	//HP���� ���� �߰����� ȸ����
	AT_R_HP_POTION_ADD_RATE		= 7432,
	AT_C_HP_POTION_ADD_RATE		= 7433,
	AT_MP_POTION_ADD_RATE		= 7441,	//HP���� ���� �߰����� ȸ����
	AT_R_MP_POTION_ADD_RATE		= 7442,
	AT_C_MP_POTION_ADD_RATE		= 7443,
	AT_DMG_REFLECT_RATE			= 7451,	// Damage �ݻ��� ��, �ݻ�Ǵ� ��(Damage�� ���� ��������)
	AT_R_DMG_REFLECT_RATE		= 7452,
	AT_C_DMG_REFLECT_RATE		= 7453,
	AT_DMG_REFLECT_HITRATE		= 7461,	// Damage �ݻ��� ��, �ݻ簡 �Ͼ Ȯ��
	AT_R_DMG_REFLECT_HITRATE	= 7462,
	AT_C_DMG_REFLECT_HITRATE	= 7463,
	AT_PENETRATION_RATE			= 7471,	// ������ Ȯ��
	AT_R_PENETRATION_RATE		= 7472,
	AT_C_PENETRATION_RATE		= 7473,
	AT_R_MAX_HP_DEAD			= 7481, // _R_ == �׾����� ȸ���� õ������ 
	AT_R_MAX_MP_DEAD			= 7482, // _R_ == �׾����� ȸ���� õ������
	AT_ABS_ADDED_DMG_PHY		= 7491,	// Dmg ������� ���밪���� �־��ִ� �߰� Dmg(Physical����)
	AT_R_ABS_ADDED_DMG_PHY		= 7492,
	AT_C_ABS_ADDED_DMG_PHY		= 7493,
	AT_ABS_ADDED_DMG_MAGIC		= 7501,	// Dmg ������� ���밪���� �־��ִ� �߰� Dmg(Magic����)
	AT_R_ABS_ADDED_DMG_MAGIC	= 7502,
 	AT_C_ABS_ADDED_DMG_MAGIC	= 7503,
	// AT_MAX_DP					= 7511, // �⺻�� or ���밪 (+)���ִ� ��
	// AT_R_MAX_DP					= 7512, // _R_ == õ������
	// AT_C_MAX_DP					= 7513, // _C_ = ������
	// AT_DP_RECOVERY_INTERVAL		= 7521,	// Auto Heal �Ǵ� �ֱ�
	// AT_R_DP_RECOVERY_INTERVAL	= 7522,
	// AT_C_DP_RECOVERY_INTERVAL	= 7523,
	// AT_DP_RECOVERY				= 7531,	// Amount of Auto Heal
	// AT_R_DP_RECOVERY			= 7532,
	// AT_C_DP_RECOVERY			= 7533,
	AT_VILLAGE_MOVESPEED		= 7541,
	AT_R_VILLAGE_MOVESPEED		= 7542,
	AT_C_VILLAGE_MOVESPEED		= 7543,
	AT_VILLAGE_MOVESPEED_SAVED	= 7544,

	AT_PROJECTILE_REFLECT_DMG_RATE		= 7551,	// �߻�ü �ݻ� ������%
	AT_R_PROJECTILE_REFLECT_DMG_RATE	= 7552,	
	AT_C_PROJECTILE_REFLECT_DMG_RATE	= 7553,	

	AT_PROJECTILE_REFLECT_RATE			= 7561,	// �߻�ü �������� �ݻ��� Ȯ��
	AT_R_PROJECTILE_REFLECT_RATE		= 7562,
	AT_C_PROJECTILE_REFLECT_RATE		= 7563,

	AT_PHY_REFLECT_DMG_RATE		= 7571,	// ���� �������� ��%�� �ݻ��� ������(�߻�ü�� �ݻ�)
	AT_R_PHY_REFLECT_DMG_RATE	= 7572,
	AT_C_PHY_REFLECT_DMG_RATE	= 7573,

	AT_PHY_REFLECT_RATE			= 7581,	// ���� �������� �ݻ��� Ȯ��(�߻�ü�� �ݻ�)
	AT_R_PHY_REFLECT_RATE		= 7582,
	AT_C_PHY_REFLECT_RATE		= 7583,

	AT_MAGIC_REFLECT_DMG_RATE	= 7591,	// ���� �������� ��%�� �ݻ��� ������(�߻�ü�� �ݻ�)
	AT_R_MAGIC_REFLECT_DMG_RATE	= 7592,
	AT_C_MAGIC_REFLECT_DMG_RATE	= 7593,

	AT_MAGIC_REFLECT_RATE		= 7601,	// ���� �������� �ݻ��� Ȯ��(�߻�ü�� �ݻ�)
	AT_R_MAGIC_REFLECT_RATE		= 7602,
	AT_C_MAGIC_REFLECT_RATE		= 7603,
	
	AT_HP_RESTORE_WHEN_HIT_RATE		= 7631,	//Ÿ�ݽ� HP ȸ�������� Ȯ��
	AT_R_HP_RESTORE_WHEN_HIT_RATE	= 7632,
	AT_C_HP_RESTORE_WHEN_HIT_RATE	= 7633,

	AT_HP_RESTORE_WHEN_HIT		= 7641,	//Ÿ�ݽ� HP ȸ�� �����ִ� ��(���밪)
	AT_R_HP_RESTORE_WHEN_HIT	= 7642,
	AT_C_HP_RESTORE_WHEN_HIT	= 7643,
	
	AT_MP_RESTORE_WHEN_HIT_RATE		= 7661,	//Ÿ�ݽ� MP ȸ�������� Ȯ��
	AT_R_MP_RESTORE_WHEN_HIT_RATE	= 7662,
	AT_C_MP_RESTORE_WHEN_HIT_RATE	= 7663,

	AT_MP_RESTORE_WHEN_HIT		= 7671,	//Ÿ�ݽ� MP ȸ�� �����ִ� ��(���밪)
	AT_R_MP_RESTORE_WHEN_HIT	= 7672,
	AT_C_MP_RESTORE_WHEN_HIT	= 7673,

	AT_ADD_GROGGYPOINT			= 7691, //!�׷α� ����Ʈ ����
	AT_R_ADD_GROGGYPOINT		= 7692,
	AT_C_ADD_GROGGYPOINT		= 7693,

	AT_DEC_GROGGYPOINT_RATE		= 7701, //!�׷α� ����Ʈ ���� �ӵ�(��ȭ)
	AT_R_DEC_GROGGYPOINT_RATE	= 7702,
	AT_C_DEC_GROGGYPOINT_RATE	= 7703,

	AT_GROGGY_RECOVERY_INTERVAL		= 7711, //!Auto Groggy �Ǵ� �ֱ�
	AT_R_GROGGY_RECOVERY_INTERVAL	= 7712,
	AT_C_GROGGY_RECOVERY_INTERVAL	= 7713,

	AT_SUMMONED_MAX_SUPPLY		= 7721, //!��ȯü ��ȯ �뷮
	AT_R_SUMMONED_MAX_SUPPLY	= 7722,
	AT_C_SUMMONED_MAX_SUPPLY	= 7723,

	AT_HP_RESTORE_WHEN_DAMAGE_RATE		= 7731,	//�ǰݽ�(������) HP ȸ�������� Ȯ��
	AT_R_HP_RESTORE_WHEN_DAMAGE_RATE	= 7732,
	AT_C_HP_RESTORE_WHEN_DAMAGE_RATE	= 7733,

	AT_HP_RESTORE_WHEN_DAMAGE			= 7741,	//�ǰݽ�(������) HP ȸ�� �����ִ� ��(���밪)
	AT_R_HP_RESTORE_WHEN_DAMAGE			= 7742,
	AT_C_HP_RESTORE_WHEN_DAMAGE			= 7743,
	
	AT_MP_RESTORE_WHEN_DAMAGE_RATE		= 7751,	//�ǰݽ�(������) MP ȸ�������� Ȯ��
	AT_R_MP_RESTORE_WHEN_DAMAGE_RATE	= 7752,
	AT_C_MP_RESTORE_WHEN_DAMAGE_RATE	= 7753,

	AT_MP_RESTORE_WHEN_DAMAGE			= 7761,	//�ǰݽ�(������) MP ȸ�� �����ִ� ��(���밪)
	AT_R_MP_RESTORE_WHEN_DAMAGE			= 7762,
	AT_C_MP_RESTORE_WHEN_DAMAGE			= 7763,
// ������ ��� ���� ����� ������ ����. ���� ��� ��Ʈ���� TB_DefDefence7_Wave.DamageDecRate���� �� ////
	AT_ABS_DEC_DMG_PHY			= 7771, // Dmg ��� ���� ���������� ������(10000 ����)
	AT_R_ABS_DEC_DMG_PHY		= 7772, // ���� ������ n/10000���� % ����(������ ���� 0���� ����)
	AT_C_ABS_DEC_DMG_PHY		= 7773, // �� ���� �׻� 0���� �ʱ�ȭ(�ڵ����� ����ؼ� ���� �־���)
	AT_ABS_DEC_DMG_MAGIC		= 7781, // Dmg ��� ���� ���������� ������(10000 ����)
	AT_R_ABS_DEC_DMG_MAGIC		= 7782, // ���� ������ n/10000���� % ����(������ ���� 0���� ����)
	AT_C_ABS_DEC_DMG_MAGIC		= 7783, // �� ���� �׻� 0���� �ʱ�ȭ(�ڵ����� ����ؼ� ���� �־���)
////////////////////////////////////////////////////////////////////////////////////////////////////////

	AT_OCCUPY_POINT				= 7791, //��������Ʈ
	AT_R_OCCUPY_POINT			= 7792,
	AT_C_OCCUPY_POINT			= 7793,	//�÷��̾�� _C_�� ���� ��������Ʈ ���� ����Ʈ ȿ���� ����, ������� AT_OCCUPY_POINT�� ����Ѵ�.
	
	AT_ADD_HP_RESTORE_RATE	  = 7801,	// ��ų�� ���� HP ȸ���� ������(��ų �ܿ��� ����ص� �������. �� ����� ó���� ����)
	AT_R_ADD_HP_RESTORE_RATE = 7802,
	AT_C_ADD_HP_RESTORE_RATE = 7803,

	AT_ADD_MP_RESTORE_RATE	  = 7811,	// ��ų�� ���� MP ȸ���� ������(��ų �ܿ��� ����ص� �������. �� ����� ó���� ����)
	AT_R_ADD_MP_RESTORE_RATE = 7812,
	AT_C_ADD_MP_RESTORE_RATE = 7813,

	AT_BLOCK_DAMAGE_RATE	= 7821,		// ������ ���Ŀ� ���� �������� ���ҽ�ų ����
	AT_R_BLOCK_DAMAGE_RATE	= 7822,
	AT_C_BLOCK_DAMAGE_RATE	= 7823,

/// 8000 ~ 9000 Ư�� ����Ʈ �鿪�� ���� �� 
///(Effect���� AT_CHECK_IMMUNITY_TYPE�� ���־���� ���̸�, Unit�� ����������Ұ�)
	AT_IMMUNITY_FREEZE_RATE		= 8001,	// ���� ����Ʈ ���� �鿪�� ���� ��ġ(���з�)
	AT_R_IMMUNITY_FREEZE_RATE	= 8002,
	AT_C_IMMUNITY_FREEZE_RATE	= 8003,

	AT_IMMUNITY_COLD_RATE		= 8011,	// ��� ����Ʈ ���� �鿪�� ���� ��ġ(���з�)
	AT_R_IMMUNITY_COLD_RATE		= 8012,
	AT_C_IMMUNITY_COLD_RATE		= 8013,

	AT_IMMUNITY_STUN_RATE		= 8021,	// ���� ����Ʈ�� �鿪�� ���� ��ġ(���з�)
	AT_R_IMMUNITY_STUN_RATE		= 8022,
	AT_C_IMMUNITY_STUN_RATE		= 8023,

	AT_IMMUNITY_POISON_RATE		= 8031,	// �ߵ� ����Ʈ�� �鿪�� ���� ��ġ(���з�)
	AT_R_IMMUNITY_POISON_RATE	= 8032,
	AT_C_IMMUNITY_POISON_RATE	= 8033,
	
	
	AT_FINAL_HIT_SUCCESS_RATE		= 9001,	// ���� ���߷�
	AT_R_FINAL_HIT_SUCCESS_RATE		= 9002,	
	AT_C_FINAL_HIT_SUCCESS_RATE		= 9003,	

	AT_FINAL_DODGE_SUCCESS_RATE		= 9011,	// ���� ȸ����
	AT_R_FINAL_DODGE_SUCCESS_RATE		= 9012,	
	AT_C_FINAL_DODGE_SUCCESS_RATE	= 9013,	

	AT_FINAL_BLOCK_SUCCESS_RATE		= 9021,	// ���� ������
	AT_R_FINAL_BLOCK_SUCCESS_RATE		= 9022,	
	AT_C_FINAL_BLOCK_SUCCESS_RATE	= 9023,	

	AT_FINAL_CRITICAL_SUCCESS_RATE	= 9031,	// ���� ũ��Ƽ�� ������
	AT_R_FINAL_CRITICAL_SUCCESS_RATE	= 9032,	
	AT_C_FINAL_CRITICAL_SUCCESS_RATE= 9033,	
	
	AT_FINAL_MAX_HP		= 9041, // AT_C_MAX_HP�� ������ ���� ���� ��Ŵ (���з�)
	AT_R_FINAL_MAX_HP	= 9042,
	AT_C_FINAL_MAX_HP	= 9043,

	AT_FINAL_MAX_MP		= 9051,	// AT_C_MAX_MP�� ������ ���� ���� ��Ŵ (���з�)
	AT_R_FINAL_MAX_MP	= 9052,
	AT_C_FINAL_MAX_MP	= 9053,
	
	AT_CRITICAL_POWER_DEC = 9061,					// ġ��Ÿ����
	AT_R_CRITICAL_POWER_DEC = 9062,
	AT_C_CRITICAL_POWER_DEC = 9063,

	AT_FINAL_CRITICAL_SUCCESS_RATE_DEC = 9071,		// ġ��Ÿ������
	AT_R_FINAL_CRITICAL_SUCCESS_RATE_DEC = 9072,
	AT_C_FINAL_CRITICAL_SUCCESS_RATE_DEC = 9073,

	AT_BLOCK_SUCCESS_RATE_DEC	= 9081,				// ���ϰ����
	AT_R_BLOCK_SUCCESS_RATE_DEC = 9082,
	AT_C_BLOCK_SUCCESS_RATE_DEC = 9083,

	AT_RESIST_DEBUFF = 9091,						// ����������	
	AT_R_RESIST_DEBUFF = 9092,
	AT_C_RESIST_DEBUFF= 9093,

	AT_SUCCESS_DEBUFF = 9101,						// ���ּ�����
	AT_R_SUCCESS_DEBUFF = 9102,
	AT_C_SUCCESS_DEBUFF= 9103,	
	
	AT_DEBUFF_DURATION_DEC_RATE	= 9111,	// ����� ����Ʈ �����ð� ���� ���(����Ʈ �ɸ�����-Target)
	AT_R_DEBUFF_DURATION_DEC_RATE	= 9112,
	AT_C_DEBUFF_DURATION_DEC_RATE	= 9113,

	AT_DEBUFF_DURATION_ADD_RATE	= 9121,	// ����� ����Ʈ �����ð� ���� ���(����Ʈ �Ŵ���-Caster)
	AT_R_DEBUFF_DURATION_ADD_RATE	= 9122,
	AT_C_DEBUFF_DURATION_ADD_RATE	= 9123,

	AT_DEFENCE_IGNORE_RATE	= 9131,				// ��� ���� ���
	AT_R_DEFENCE_IGNORE_RATE	= 9132,
	AT_C_DEFENCE_IGNORE_RATE	= 9133,
	
	AT_CALCUATEABIL_MAX			= 10000,
// Reserve : 10001 - 11000
// AI Action type ��
//���� Action type �� = GetAbil(AT_AI_ACTIONTYPE_MIN+Action)
	AT_AI_ACTIONTYPE_MIN		= 10001,	// AI Action type ��
	AT_AI_TYPE_NO_DAMAGEACTION	= 10011,	// AI ��Ÿ�Ժ� Enter�ɶ� �������׼��� ����, Leave�ɶ� ������
	AT_AI_CHASE_MOVESPEED_SAVED	= 10012,	// AI Chase�� �߰��� ���ǵ� ���з�
	AT_AI_CHASE_GOAL_POS		= 10013,	// AI Chase�� OnEnter���� GoalPos�� ����ϵ��� ��

	AT_HIT_DAMAGE_HP_RATE_ACTIVATE							= 10101,	// ���ݽ� ������ �߰� ������(HP%)�� �� Ȯ��
	AT_HIT_DAMAGE_HP_RATE									= 10102,	// ���ݽ� ������ �߰� ������(HP%)
	AT_HIT_DECREASE_MP_RATE_ACTIVATE						= 10103,	// ���ݽ�  ������ �߰��� MP��(%)�� ���ҽ�ų Ȯ��
	AT_HIT_DECREASE_MP_RATE									= 10104,	// ���ݽ� ������ �߰��� MP�� ����(%)
	AT_CRITICAL_DAMAGE_RATE_ACTIVATE						= 10105,	// ũ��Ƽ�ý� ������ �߰� ������(HP%)�� �� Ȯ��
	AT_CRITICAL_DAMAGE_RATE									= 10106,	// ũ��Ƽ�ý� ������ �߰� ������(HP%)
	AT_CRITICAL_DAMAGE_ACTIVATE								= 10107,	// ũ��Ƽ�ý� ������ �߰� �������� Ȯ��
	AT_CRITICAL_DAMAGE										= 10108,	// ũ��Ƽ�ý� ������ �߰� ������
	AT_ATTACKED_DECREASE_DAMAGE_ACTIVATE_HP_RATE			= 10109,	// �ǰݽ� HP�� ���� %���ϸ� ������ ����
	AT_ATTACKED_DECREASE_DAMAGE_RATE						= 10110,	// �ǰݽ� HP�� ���� %���ϸ� �����ϴ� �������� %
	AT_HIT_SUCKING_HPMP_RATE_ACTIVATE						= 10111,	// ���ݽ� HP�� MP�� ������ Ȯ��
	AT_HIT_SUCKING_HPMP										= 10112,	// ���ݽ� HP MP�� ���¾�

// �߰����� Ȯ���� ���� �ϴ°�(������) (Pet ���� �����)
	AT_GIVE_TO_CALLER_ADD_EXP_RATE			= 11001,	// ���� ��ȯ�ڿ��� ��������ġ�� �ش�
	AT_HUNTED_MON_ADD_DROPMONEY_RATE		= 11002,	// ���͸� ������ �߰����� DropMoney�� ������ Ȯ��
	AT_HUNTED_MON_ADD_DROPMONEY_VALUE_RATE	= 11003,	// AT_HUNTED_MON_ADD_DROPMONEY_RATE�� �߻������� �����ϴ� ���� ��(�̸����� �ʹ� �����-_-...)
	AT_GOLD_RUSH_DROPMONEY_VALUE_MIN		= 11004,	// ��� ���� ��ų ���� ����ϴ� ���� �ּҰ�
	AT_GOLD_RUSH_DROPMONEY_VALUE_MAX		= 11005,	// ��� ���� ��ų ���� ����ϴ� ���� �ִ밪

//----------------------------------------------------
// ���� ���� 12000~13000 ����
//----------------------------------------------------
	AT_JOBSKILL_TOOL_TYPE = 12000, // ���� ����/ä�� ���� Ÿ��
	AT_JOBSKILL_FURNITURE_TYPE = 12100, // 2�� �������� ������(����)�� � EGatherType�� ������ Ȯ���ϴ� ��

	AT_JS_RECOVERY_EXHAUSTION_R = 12101,
	AT_JS_RECOVERY_EXHAUSTION_C = 12102,
	AT_JS_RECOVERY_BLESSPOINT_R = 12103,
	AT_JS_RECOVERY_BLESSPOINT_C = 12104,
	AT_JS_USE_ITEM_LIMIT_EXHAUSTION_R = 12105,
	AT_JS_USE_ITEM_LIMIT_BLESSPOINT_R = 12106,
	AT_JS_RECOVERY_PRODUCTPOINT_R = 12107,
	AT_JS_RECOVERY_PRODUCTPOINT_C = 12108,
	AT_JS_USE_ITEM_LIMIT_PRODUCTPOINT_R = 12109,
	AT_ADDITIONAL_INVEN_SIZE	= 12110,	//�꿡 ����Ǵ� �����

	AT_JS_IGNORE_EXP_PENALTY			= 12230,			//������ų�� �г�Ƽ�� ����

	AT_JOBSKILL_SAVEIDX					= 12240,

	AT_LEARN_SKILL_EVENT				= 12400,			//���������� ��ų ��ﶧ �߻��Ǵ� �̺�Ʈ
//----------------------------------------------------
	
	AT_DMG_REFLECT_RATE_FROM_ITEM01			=  13001,	 // ���������� ���� Damage �ݻ� Ȯ�� (AT_DMG_REFLECT_DMG_FROM_ITEM01�� �� ����)
	AT_DMG_REFLECT_RATE_FROM_ITEM02			=  13002,
	AT_DMG_REFLECT_RATE_FROM_ITEM03			=  13003,
	AT_DMG_REFLECT_RATE_FROM_ITEM04			=  13004,
	AT_DMG_REFLECT_RATE_FROM_ITEM05			=  13005,
	AT_DMG_REFLECT_RATE_FROM_ITEM06			=  13006,
	AT_DMG_REFLECT_RATE_FROM_ITEM07			=  13007,
	AT_DMG_REFLECT_RATE_FROM_ITEM08			=  13008,
	AT_DMG_REFLECT_RATE_FROM_ITEM09			=  13009,
	AT_DMG_REFLECT_RATE_FROM_ITEM10			=  13010,

	AT_DMG_REFLECT_DMG_FROM_ITEM01			=  13011,	// ���������� ���� Dmage �ݻ�Ȯ���� ���� �ݻ�ɶ� ���밪
	AT_DMG_REFLECT_DMG_FROM_ITEM02			=  13012,
	AT_DMG_REFLECT_DMG_FROM_ITEM03			=  13013,
	AT_DMG_REFLECT_DMG_FROM_ITEM04			=  13014,
	AT_DMG_REFLECT_DMG_FROM_ITEM05			=  13015,
	AT_DMG_REFLECT_DMG_FROM_ITEM06			=  13016,
	AT_DMG_REFLECT_DMG_FROM_ITEM07			=  13017,
	AT_DMG_REFLECT_DMG_FROM_ITEM08			=  13018,
	AT_DMG_REFLECT_DMG_FROM_ITEM09			=  13019,
	AT_DMG_REFLECT_DMG_FROM_ITEM10			=  13020,


// 20000~30000���̴� �г�Ƽ ����Դϴ�.
	AT_PENALTY_HP_RATE			= 20000+AT_HP,			//20101
	AT_PENALTY_MP_RATE			= 20000+AT_MP,			//20105
	AT_ADD_EXPERIENCE_RATE		= 20000+AT_EXPERIENCE,	//20512

//30000~AT_ELEMENT_ABIL_MAX �Ӽ� �ý��� ���� ���
	AT_ELEMENT_ABIL_NONE			= 30000,		//�Ӽ� ��� ����
	AT_ATTACK_ADD_FIRE				= 30011,		//ȭ�� �߰� ���ݷ�(���밪)
	AT_ATTACK_ADD_ICE				= 30012,		//�ñ� �߰� ���ݷ�(���밪)
	AT_ATTACK_ADD_NATURE			= 30013,		//�ڿ� �߰� ���ݷ�(���밪)
	AT_ATTACK_ADD_CURSE				= 30014,		//���� �߰� ���ݷ�(���밪)
	AT_ATTACK_ADD_DESTROY			= 30015,		//�ı� �߰� ���ݷ�(���밪)
	AT_C_ATTACK_ADD_FIRE			= 30016,		//ȭ�� �߰� ���ݷ�(���밪)(������)
	AT_C_ATTACK_ADD_ICE				= 30017,		//�ñ� �߰� ���ݷ�(���밪)(������)
	AT_C_ATTACK_ADD_NATURE			= 30018,		//�ڿ� �߰� ���ݷ�(���밪)(������)
	AT_C_ATTACK_ADD_CURSE			= 30019,		//���� �߰� ���ݷ�(���밪)(������)
	AT_C_ATTACK_ADD_DESTROY			= 30020,		//�ı� �߰� ���ݷ�(���밪)(������)
	AT_R_ATTACK_ADD_FIRE			= 30021,		//ȭ�� �߰� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_ICE				= 30022,		//�ñ� �߰� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_NATURE			= 30023,		//�ڿ� �߰� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_CURSE			= 30024,		//���� �߰� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_DESTROY			= 30025,		//�ı� �߰� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_FIRE_PHY		= 30031,		//���� ��ȯ ȭ�� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_ICE_PHY			= 30032,		//���� ��ȯ �ñ� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_NATURE_PHY		= 30033,		//���� ��ȯ �ڿ� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_CURSE_PHY		= 30034,		//���� ��ȯ ���� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_DESTROY_PHY		= 30035,		//���� ��ȯ �ı� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_FIRE_MAGIC		= 30041,		//���� ��ȯ ȭ�� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_ICE_MAGIC		= 30042,		//���� ��ȯ �ñ� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_NATURE_MAGIC	= 30043,		//���� ��ȯ �ڿ� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_CURSE_MAGIC		= 30044,		//���� ��ȯ ���� ���ݷ�(��밪)
	AT_R_ATTACK_ADD_DESTROY_MAGIC	= 30045,		//���� ��ȯ �ı� ���ݷ�(��밪)
	AT_RESIST_ADD_FIRE				= 30051,		//ȭ�� �߰� ���׷�(���밪)
	AT_RESIST_ADD_ICE				= 30052,		//�ñ� �߰� ���׷�(���밪)
	AT_RESIST_ADD_NATURE			= 30053,		//�ڿ� �߰� ���׷�(���밪)
	AT_RESIST_ADD_CURSE				= 30054,		//���� �߰� ���׷�(���밪)
	AT_RESIST_ADD_DESTROY			= 30055,		//�ı� �߰� ���׷�(���밪)
	AT_C_RESIST_ADD_FIRE			= 30056,		//ȭ�� �߰� ���׷�(���밪)(������)
	AT_C_RESIST_ADD_ICE				= 30057,		//�ñ� �߰� ���׷�(���밪)(������)
	AT_C_RESIST_ADD_NATURE			= 30058,		//�ڿ� �߰� ���׷�(���밪)(������)
	AT_C_RESIST_ADD_CURSE			= 30059,		//���� �߰� ���׷�(���밪)(������)
	AT_C_RESIST_ADD_DESTROY			= 30060,		//�ı� �߰� ���׷�(���밪)(������)
	AT_R_RESIST_ADD_FIRE			= 30061,		//ȭ�� �߰� ���׷�(��밪)
	AT_R_RESIST_ADD_ICE				= 30062,		//�ñ� �߰� ���׷�(��밪)
	AT_R_RESIST_ADD_NATURE			= 30063,		//�ڿ� �߰� ���׷�(��밪)
	AT_R_RESIST_ADD_CURSE			= 30064,		//���� �߰� ���׷�(��밪)
	AT_R_RESIST_ADD_DESTROY			= 30065,		//�ı� �߰� ���׷�(��밪)
	AT_R_RESIST_ADD_FIRE_PHY		= 30071,		//���� ��ȯ ȭ�� ���׷�(��밪)
	AT_R_RESIST_ADD_ICE_PHY			= 30072,		//���� ��ȯ �ñ� ���׷�(��밪)
	AT_R_RESIST_ADD_NATURE_PHY		= 30073,		//���� ��ȯ �ڿ� ���׷�(��밪)
	AT_R_RESIST_ADD_CURSE_PHY		= 30074,		//���� ��ȯ ���� ���׷�(��밪)
	AT_R_RESIST_ADD_DESTROY_PHY		= 30075,		//���� ��ȯ �ı� ���׷�(��밪)
	AT_R_RESIST_ADD_FIRE_MAGIC		= 30081,		//���� ��ȯ ȭ�� ���׷�(��밪)
	AT_R_RESIST_ADD_ICE_MAGIC		= 30082,		//���� ��ȯ �ñ� ���׷�(��밪)
	AT_R_RESIST_ADD_NATURE_MAGIC	= 30083,		//���� ��ȯ �ڿ� ���׷�(��밪)
	AT_R_RESIST_ADD_CURSE_MAGIC		= 30084,		//���� ��ȯ ���� ���׷�(��밪)
	AT_R_RESIST_ADD_DESTROY_MAGIC	= 30085,		//���� ��ȯ �ı� ���׷�(��밪)
	AT_EXTRACT_ELEMENT_FIRE			= 30110,		//ȭ������ ����
	AT_EXTRACT_ELEMENT_ICE			= 30120,		//�ñ���� ����
	AT_EXTRACT_ELEMENT_NATURE		= 30130,		//�ڿ����� ����
	AT_EXTRACT_ELEMENT_CURSE		= 30140,		//���ֿ��� ����
	AT_EXTRACT_ELEMENT_DESTROY		= 30150,		//�ı����� ����
	AT_EXTRACT_ELEMENT_RANDOM		= 30160,		//���������� ����
	AT_ELEMENT_MONSTER				= 30170,		//���� �Ӽ�(1:ȭ��,2:�ñ�,3:�ڿ�,4:����,5:�ı�)
	AT_ATTACK_ADD_4ELEMENT          = 30180,        //ȭ��,�ñ�,�ڿ�,�����߰����ݷ�(���밪)
	AT_ATTACK_ADD_5ELEMENT          = 30181,        //ȭ��,�ñ�,�ڿ�,����,�ı��߰����ݷ�(���밪)
	AT_R_ATTACK_ADD_4ELEMENT        = 30190,        //ȭ��,�ñ�,�ڿ�,�����߰����ݷ�(��밪)
	AT_R_ATTACK_ADD_5ELEMENT        = 30191,        //ȭ��,�ñ�,�ڿ�,����,�ı��߰����ݷ�(��밪)
	AT_R_ATTACK_ADD_4ELEMENT_PHY    = 30200,        //������ȯȭ��,�ñ�,�ڿ�,���ְ��ݷ�(��밪)
	AT_R_ATTACK_ADD_5ELEMENT_PHY    = 30201,        //������ȯȭ��,�ñ�,�ڿ�,����,�ı����ݷ�(��밪)
	AT_R_ATTACK_ADD_4ELEMENT_MAGIC  = 30210,        //������ȯȭ��,�ñ�,�ڿ�,���ְ��ݷ�(��밪)
	AT_R_ATTACK_ADD_5ELEMENT_MAGIC  = 30211,        //������ȯȭ��,�ñ�,�ڿ�,����,�ı����ݷ�(��밪)
	AT_RESIST_ADD_4ELEMENT          = 30220,        //ȭ��,�ñ�,�ڿ�,�����߰����׷�(���밪)
	AT_RESIST_ADD_5ELEMENT          = 30221,        //ȭ��,�ñ�,�ڿ�,����,�ı��߰����׷�(���밪)
	AT_R_RESIST_ADD_4ELEMENT        = 30230,        //ȭ��,�ñ�,�ڿ�,�����߰����׷�(��밪)
	AT_R_RESIST_ADD_5ELEMENT        = 30231,        //ȭ��,�ñ�,�ڿ�,����,�ı��߰����׷�(��밪)
	AT_R_RESIST_ADD_4ELEMENT_PHY    = 30240,        //������ȯȭ��,�ñ�,�ڿ�,�������׷�(��밪)
	AT_R_RESIST_ADD_5ELEMENT_PHY    = 30241,        //������ȯȭ��,�ñ�,�ڿ�,����,�ı����׷�(��밪)
	AT_R_RESIST_ADD_4ELEMENT_MAGIC  = 30250,        //������ȯȭ��,�ñ�,�ڿ�,�������׷�(��밪)
	AT_R_RESIST_ADD_5ELEMENT_MAGIC  = 30251,        //������ȯȭ��,�ñ�,�ڿ�,����,�ı����׷�(��밪)
	AT_RESIST_RATE_ADD_FIRE			= 30301,		//ȭ�� �߰� ���׷�(���밪.���з�)
	AT_RESIST_RATE_ADD_ICE			= 30302,		//�ñ� �߰� ���׷�(���밪.���з�)
	AT_RESIST_RATE_ADD_NATURE		= 30303,		//�ڿ� �߰� ���׷�(���밪.���з�)
	AT_RESIST_RATE_ADD_CURSE		= 30304,		//���� �߰� ���׷�(���밪.���з�)
	AT_RESIST_RATE_ADD_DESTROY		= 30305,		//�ı� �߰� ���׷�(���밪.���з�)
	AT_RESIST_RATE_ADD_4ELEMENT		= 30311,		//4��Ӽ� ���׷� ����(���밪.���з�)
	AT_RESIST_RATE_ADD_5ELEMENT		= 30312,		//5��Ӽ� ���׷� ����(���밪.���з�)

	//Ŭ���̾�Ʈ ������ ���
	AT_BASE_ATTACK_ADD_FIRE			= 30331,		//ȭ�� �߰� ���ݷ�(Unit�� ����)
	AT_BASE_ATTACK_ADD_ICE			= 30332,		//�ñ� �߰� ���ݷ�
	AT_BASE_ATTACK_ADD_NATURE		= 30333,		//�ڿ� �߰� ���ݷ�
	AT_BASE_ATTACK_ADD_CURSE		= 30334,		//���� �߰� ���ݷ�
	AT_BASE_ATTACK_ADD_DESTROY		= 30335,		//�ı� �߰� ���ݷ�
	AT_BASE_RESIST_ADD_FIRE			= 30371,		//ȭ�� �߰� ���׷�
	AT_BASE_RESIST_ADD_ICE			= 30372,		//�ñ� �߰� ���׷�
	AT_BASE_RESIST_ADD_NATURE		= 30373,		//�ڿ� �߰� ���׷�
	AT_BASE_RESIST_ADD_CURSE		= 30374,		//���� �߰� ���׷�
	AT_BASE_RESIST_ADD_DESTROY		= 30375,		//�ı� �߰� ���׷�

	AT_INV_ATTACK_ADD_FIRE			= 30341,		//ȭ�� �߰� ���ݷ�(Inven�� ����)
	AT_INV_ATTACK_ADD_ICE			= 30342,		//�ñ� �߰� ���ݷ�
	AT_INV_ATTACK_ADD_NATURE		= 30343,		//�ڿ� �߰� ���ݷ�
	AT_INV_ATTACK_ADD_CURSE			= 30344,		//���� �߰� ���ݷ�
	AT_INV_ATTACK_ADD_DESTROY		= 30345,		//�ı� �߰� ���ݷ�
	AT_INV_RESIST_ADD_FIRE			= 30381,		//ȭ�� �߰� ���׷�
	AT_INV_RESIST_ADD_ICE			= 30382,		//�ñ� �߰� ���׷�
	AT_INV_RESIST_ADD_NATURE		= 30383,		//�ڿ� �߰� ���׷�
	AT_INV_RESIST_ADD_CURSE			= 30384,		//���� �߰� ���׷�
	AT_INV_RESIST_ADD_DESTROY		= 30385,		//�ı� �߰� ���׷�

	AT_ELEMENT_ABIL_MAX				= 30500,		//�Ӽ� ��� ���� ��
	
// ����Ʈ�� ������ ��Ʈ�� �Ѵ�. 
	AT_EFFCONTROL_DEL_MONEY		= 40001,	// ������ ���� ����(Value�� ���� �־���� �մϴ�)
	AT_EFFCONTROL_DEL_MONEY_PERLEVEL = 40002,// ������ ������ �ϴµ� ������ ����ġ ����(Value�� ����ġ�� �־��־�� �մϴ�.)
	AT_EFFCONTROL_DEL_ITEM		= 40003,	// ���������� ���� ����(Value���� ������ ��ȣ)
	AT_EFFCONTROL_DEL_BY_EFFECT = 40004,	// ����Ʈ�� ����Ʈ�� ����(����� : �������� ���������)
	AT_WHEN_EFFECT_ACTIVATE_THEN_DELETE_THIS_EFFECTNO = 40100,	// Ư�� ����Ʈ�� �ߵ��ɶ�, ���⿡ ��� ����Ʈ�� ���� �Ҽ� �ְ� �Ѵ�

// 41000~43000 ���̴� ��ų ȿ�� ���� ����Դϴ�.
	AT_ADJUST_SKILL_CHECK_TYPE	= 41000,		// ��ų üũ Ÿ��, EAdjustSkillCheckType 1:��� ��ų, 2:�Ϻ� ��ų��, 3:�Ϻ� ��ų �����ϰ� ���
// ��ų ����1(���� ���� ��)
	AT_E_ADJUST_SKILL_RANGE		= 41101,		// DefaultOnFindTarget �Լ��� iSkillRange�� +
	AT_R_ADJUST_SKILL_RANGE		= 41102,		// DefaultOnFindTarget �Լ��� iSkillRange�� ���з��� ����
	//AT_ADJUST_SKILL_C_RANGE		= 41103,
//��ų ����2(����ü����, ������ ���� ��)
	AT_E_ADJUST_SKILL_RANGE2	= 41111,		// DefaultOnFindTarget �Լ��� iSkillRange2�� +
	AT_R_ADJUST_SKILL_RANGE2	= 41112,		// DefaultOnFindTarget �Լ��� iSkillRange2�� ���з��� ����
	//	AT_ADJUST_SKILL_C_RANGE2	= 41113,
//��ų ������(���� ������ �ƴ�)
	AT_E_ADJUST_SKILL_DMG		= 41121,		// DoFinalDmg �Լ��� ��ų ������ �� +
	AT_R_ADJUST_SKILL_DMG		= 41122,		// DoFinalDmg �Լ��� ��ų ������ �� ���з��� ����
	//AT_ADJUST_SKILL_C_DMG		= 41123,
// ��ų ��Ÿ��(���� ��� ����)
	//AT_ADJUST_SKILL_E_COOLTIME  = 41131,		// ��Ÿ�ӿ� �� +	1000�̸� 1��
	//AT_ADJUST_SKILL_R_COOLTIME  = 41132,		// ��Ÿ�� �� ���з��� ����
	//AT_ADJUST_SKILL_C_COOLTIME  = 41133,
	
//üũ �� ��ų��(1000�� ����)
	AT_ADJUST_SKILL_BEGIN		= 42001,		// AT_ADJUST_SKILL_CHECK_TYPE�� üũ �Ǵ� ��ų�� ��ȣ(1�� Ÿ�� ����)
	AT_ADJUST_SKILL_END			= 42100,
	//AT_ADJUST_SKILL_END		= 43000,
	
	AT_ADJUST_SKILL_DMG_BY_PHASE_TIME	= 43100,	// T��
	
	AT_E_ADJUST_SKILL_RESULT_MIN_DMG_BY_PHASE		= 43101,	// min
	AT_R_ADJUST_SKILL_RESULT_MIN_DMG_BY_PHASE		= 43102,	// min
	//AT_ADJUST_SKILL_DMG_BY_PHASE_C_MIN		= 43103,	// min

	AT_E_ADJUST_SKILL_RESULT_MAX_DMG_BY_PHASE		= 43111,	// max
	AT_R_ADJUST_SKILL_RESULT_MAX_DMG_BY_PHASE		= 43112,	// max
	//AT_ADJUST_SKILL_DMG_BY_PHASE_C_MAX		= 43113,	// max

	AT_E_ADJUST_SKILL_INC_PHASE_DMG		= 43121,	// ������ ������(DB)
	AT_R_ADJUST_SKILL_INC_PHASE_DMG		= 43122,	// ������ ������(DB)
	//AT_ADJUST_SKILL_DMG_BY_PHASE_C_INC		= 43123,	// ������ ������(DB)
	
	AT_ADJUST_SKILL_ACC_INC_PHASE_DMG	= 43200,	// ������ ������ ������(��������)
	AT_ADJUST_SKILL_PHASE_DMG_IS_HIT	= 43201,	// ���� �ߴٴ� ǥ�ø� effect�� �����ϱ� ���Ѱ�

//��Ÿ Ŭ���̾�Ʈ���� ����� �����ܵ��� ���
	AT_DEF_RES_NO				= 50001,	
	AT_DONT_DISPLAY_LEVEL		= 50002, // ���͵��� ������ ǥ�� ���� �ʴ´�.
	AT_DONT_DISPLAY_ALL			= 50003, // ���͵��� �̸�, �������� ���� ǥ�� ���� �ʴ´�.
	AT_DISPLAY_LEVEL			= 50004, // �̸� ���ʿ� ���� �������(������� ��� ���)
	AT_DISPLAY_HEADBUFFICON		= 50005, // ���� ������ �Ӹ��� ǥ��
	AT_DISPLAY_GIF_TITLE		= 50006, // �Ӹ� ���� GIF ǥ��
	AT_DISPLAY_EFFECT_COUNTDOWN	= 50007, // �Ӹ� ���� ����Ʈ ī��Ʈ ���
	AT_DONT_DISPLAY_GRADE		= 50008, // ����� ǥ�� ���� �ʴ´�.
	
	AT_CUSTOM_TOOLTIP_01 		= 50050, // Ŀ�����ϰ� ǥ���� ��������

//��ŷ�� ���õ� ���
	AT_MAX_COMBO				= 50101,

//�̼� �̺�Ʈ
	AT_MISSION_EVENT			= 50150,

//�̼� �ݺ� Ƚ��
	AT_MISSION_NO				= 50151,
	AT_MISSION_COUNT			= 50152,
//����������
	AT_STRATEGIC_POINT			= 50153,
	AT_STRATEGIC_MUL_POINT		= 50154,
	AT_AGGRO_POINT				= 50155,
	AT_MIN_OCCUPY_POINT			= 50156, //@AutoHeal���� �ּ� ��������Ʈ
	AT_MAX_OCCUPY_POINT			= 50157, //@

	AT_USE_SKILL_ON_DIE			= 50159, // ���� ��ų�� �ߵ��Ǹ� ����
	AT_ELGA_STONE_COUNT			= 50160, // ���� ���� ��ȯ����

	AT_RELEASE_DEL_EFFECT_TYPE	= 50161, //!EDelEffectType
	AT_RELEASE_DEL_EFFECT		= 50162, //@PgGround::ReleaseUnit�ɶ� ����Ʈ �߿��� Ÿ�ٿ��� ����� ����Ʈ�� ������ �ϴ� ���, �����ϵ���, CUnit: AddAbil, CEffect: SetAbil

//Client_Option
	AT_CLIENT_OPTION_SAVE		= 50201,	// �ɼ� �� ��� ������ �����ؾ��ϴ� �ɼǵ�(Bit�� ������ ������)

//Penalty
	AT_MISSION_THROWUP_PENALTY	= 50501,	// �̼� ����

//Ư�� ���� �����ϸ� ������ �����ϴ� ���( 50600 ~ 50700 ���� )
	AT_SPECIFIC_REWARD_REVIVE_FEATHER_LOGIN		= 50600, // ���� �� ��Ȱ ����(100%) ���Ե� ���� ����( 1�� 1ȸ )
	AT_SPECIFICR_EWARD_REVIVE_FEATHER_LEVELUP	= 50601, // ������ �ϸ� ��ȫ�� ��Ȱ ���� ����
	AT_SPECIFIC_REWARD_EVENT					= 50602, // ���� �ð� ���� ������ ������ ������ �����ϴ� �̺�Ʈ.
/* 50700 ���� ���� */

//�̴ϸ� ������ ǥ�ÿ� ���
	AT_DISPLAY_MINIMAP_EFFECT	= 50701,	//����Ʈ�� �ɷ��� �� �̴ϸʿ� �������� �׷��ش�. (���� ����Ʈ �̴ϸ� ������ ��ȣ)
	AT_DISPLAY_MINIMAP_INSITEM	= 50702, // �� ����� �ִ� �ν���Ʈ������(��:���귯���� ��)�� �̴ϸʿ� ǥ�õȴ�. (���� �̴ϸ� ������ ��ȣ)
//�̴ϸ� ������ ǥ�ÿ� ��� ��

//���귯�� ��� �� ���
	AT_BEAR_ITEM				= 50721, // �� ����� �ִ� �������� ���귯�� ��� �� ������.
	AT_BEAR_EFFECT_RED			= 50722, // �� ����� �ִ� �÷��̾�� ���� ���� ����.( ���� �� )
	AT_BEAR_EFFECT_BLUE			= 50723, // �� ����� �ִ� �÷��̾�� ���� ���� ����.( �Ķ� �� )
	AT_BEAR_EFFECT_SPEED_LOCK	= 50724, // �� ����� ������ �ν��Ͻ� ������, ����Ʈ ��ų �ܿ��� �̼��� ������ �� ����.
	AT_BEAR_EFFECT_ITEM_LOCK	= 50725, // �� ����� ������ �������� ��� �� �� ����.
//���귯�� ��� �� ��� ��

	AT_EFFECT_DELETE_ALL_DEBUFF = 50790, // �� ����� �ִ� ����Ʈ�� ��� ������� �����Ѵ�.

//TRANSLATE ��� Ÿ�� ��ų�ΰ�? (Ŀ�ǵ� �Է� UI�� ���¸� ������)
	AT_TRANSLATE_SKILL_TYPE = 50800,
//������ ��ų��( ���̽�ų�� == �ּ� Ŀ�ǵ� ��, ������ üũ == 100/���̽�ų��, �̰͵��� ������� ���� ������ ��ų�� �������� ������)
	AT_TRANSLATE_SKILL01	= 50801,
	AT_TRANSLATE_SKILL10	= 50810,
//��� Ŀ�ǵ� �Է��� ���� �ϴ°�?(���̽�ų ������ ������, ���� ��ų���� ������)
	AT_TRANSLATE_SKILL_CMD_CNT	= 50811,

//GM ���� ���(51000~52000 ����)
	AT_GM_GODHAND				= 51000,	// �� ����� ������ ��þƮ Ȯ�� 100%
	AT_GM_JOBSKILL_USE_SUBTOOL	= 51100,	// �� ����� ������ ������� �ߵ� Ȯ�� 100% 

	AT_ITEM_SKILL				= 53000,	// ��ų�ߵ������ۿ� ���� ��ų��ȣ�� ���. �� ��� ������ ���������� ��ų ��� ����
	AT_ITEM_SKILL_EXPLAIN_ID    = 53001,	// ��ų�ߵ������ۿ� ��ų ���� ���� ex>��ũ ������ �� ��Ʈ �ɼ�

	AT_JS3_NOT_EQUIP_ITEM		= 53100,	// �� ��� ������ ������ ������ ��뷹�� -> ���ɱ��� ���� ����

	AT_MULTI_RES				= 59000,	// ��Ƽ ���ҽ�(���� ���� ���� ���� ������ �˻�)
	AT_NEW_MULTI_TRS			= 59001,    // ��Ƽ ���ҽ�(���� ���� �� ������ �˻�)
	AT_PARTY_MASTER_PROGRESS_POS	= 59100, // ��Ƽ�� ���� ����(Ʈ����) ��ġ
	AT_UNIT_PASSED_METER		= 59101,	// Used for cart mission

	AT_SKILL_CUSTOM_DATA_01		= 60001,	// ����� ������ ����� ABIL ����
											// ����� ��ų ���۽� �߻��ϴ� �ӽ� ������ ��������� ����Ұ���
											// DEF ���ǿ� ��ġ�� �ʵ��� ����Ұ�
	AT_SKILL_CUSTOM_DATA_MAX	= 60201,	// ����� ������ ����� ABIL ������


	AT_ACHIEVEMENT_PVP_KILL			= 60901, // ����, �Ŵ� ���� �ӽ� ���� ������ ����� ������� �ʴ´�.
	AT_ACHIEVEMENT_PVP_DEAD			= 60902,

	AT_ACHIEVEMENT_MISSION_KILL		= 60903,
	AT_ACHIEVEMENT_MISSION_DEAD		= 60904,
	AT_ACHIEVEMENT_MISSION_USEITEM	= 60905,
	AT_ACHIEVEMENT_MISSION_HIT		= 60906,
	AT_ACHIEVEMENT_MISSION_REVIVE	= 60907,

	AT_ACHIEVEMENT_MONSTER_TYPE = 61000,	// ���� �˻�� ���� Ÿ�� (��� ��ġ�� 100 ~ 200 �� ���� 0 �̸� ���� �˻�� ���Ͱ� �ƴ�)
	AT_ACHIEVEMENT_DATA_MIN		= 61001,	// ���� �˻�� ������ ���� ���� 

	AT_ACHIEVEMENT_LEVEL		= 61002,// ĳ���� ���� ����			

	AT_ACHIEVEMENT_ENCHANT9		= 61003,// ��þƮ ������ ���� ���� (9)
	AT_ACHIEVEMENT_ENCHANT11	= 61004,// ��þƮ ������ ���� ���� (11)
	AT_ACHIEVEMENT_ENCHANT13	= 61005,// ��þƮ ������ ���� ���� (13)
	AT_ACHIEVEMENT_ENCHANT15	= 61006,// ��þƮ ������ ���� ���� (15)

	AT_ACHIEVEMENT_SOULCRAFT3	= 61007,// �ҿ� ũ����Ʈ ���� Ƚ�� ��Ƽ��Ʈ
	AT_ACHIEVEMENT_SOULCRAFT4	= 61008,// �ҿ� ũ����Ʈ ���� Ƚ�� ������

	AT_ACHIEVEMENT_CHANGE_CLASS	= 61009,// ����	���
	AT_ACHIEVEMENT_ITEM_BREAKER	= 61010,// ������ ��Ʈ����(ȯȥ ���� ����)
	AT_ACHIEVEMENT_COOKER		= 61011,// �丮 ���� Ƚ��
	AT_ACHIEVEMENT_EATER		= 61012,// �丮 ���� Ƚ��
	AT_ACHIEVEMENT_CONQUEROR	= 61013,// ũ����Ż ���� ��� Ƚ��
	AT_ACHIEVEMENT_IMMORTALITY	= 61014,// �һ��(��Ȱ Ƚ��)
	AT_ACHIEVEMENT_ZOMBI		= 61015,// ����? (��� Ƚ��)
	AT_ACHIEVEMENT_THERICH		= 61016,// ����? (���� �Ӵ� ȹ��)

	AT_ACHIEVEMENT_PVP_ENTRY_DM				= 61017,// ������ ���� Ƚ��
	AT_ACHIEVEMENT_PVP_ENTRY_KTH			= 61018,// ������ ���� Ƚ��
	AT_ACHIEVEMENT_PVP_ENTRY_ANNIHILATION	= 61019,// ������ ���� Ƚ��
	AT_ACHIEVEMENT_PVP_ENTRY_DESTROY		= 61020,// ������ ���� Ƚ��

	AT_ACHIEVEMENT_PVP_RESULT_CP			= 61021,// PVP ��� CP
	AT_ACHIEVEMENT_PVP_RESULT_WIN			= 61022,// PVP ��� �¸� Ƚ��
	AT_ACHIEVEMENT_PVP_RESULT_LOSE			= 61023,// PVP ��� �й� Ƚ��

	AT_ACHIEVEMENT_ENCHANT_SUCCESS			= 61024,// ��þƮ ���� Ƚ��
	AT_ACHIEVEMENT_ENCHANT_FAIL				= 61025,// ��þƮ ���� Ƚ��

	AT_ACHIEVEMENT_OPENMARKET_BUY_CASH		= 61026,// ���� ���� ���� �ݾ�(ĳ��)
	AT_ACHIEVEMENT_OPENMARKET_BUY_GOLD		= 61027,// ���� ���� ���� �ݾ�(���)

	AT_ACHIEVEMENT_USE_PORTAL				= 61028,// ��Ż �̵� Ƚ��
	AT_ACHIEVEMENT_DESTROY_OBJECT			= 61029,// �ı� ������Ʈ �ı� Ƚ��
	
	AT_ACHIEVEMENT_PVP_SLAYER				= 61030,// PVP ��忡�� �ױ������� ���� Ƚ��
	AT_ACHIEVEMENT_PVP_IMMORTALITY			= 61031,// PVP ����ñ��� ���� ����
	AT_ACHIEVEMENT_PVP_KILL_VS_DEAD			= 61032,// ������ VS ���μ�

	AT_ACHIEVEMENT_NORMAL_QUEST_COMPLETE	= 61033,// ���� ����Ʈ �Ϸ� Ƚ��
	AT_ACHIEVEMENT_SCENARIO_QUEST_COMPLETE	= 61034,// ���� ����Ʈ �Ϸ� Ƚ��
	AT_ACHIEVEMENT_LOOP_QUEST_COMPLETE		= 61035,// �ݺ� ����Ʈ �Ϸ� Ƚ��
	AT_ACHIEVEMENT_GUILD_QUEST_COMPLETE		= 61036,// ��� ����Ʈ �Ϸ� Ƚ��

	AT_ACHIEVEMENT_MYHOME_HITCOUNT			= 61037,// ����Ȩ �湮 Ƚ��

	AT_ACHIEVEMENT_USE_EMOTICON				= 61038,// �̸�Ƽ�� ���

	AT_ACHIEVEMENT_LONGTIMENOSEE			= 61039,// �������̴�.

	AT_ACHIEVEMENT_SELCHAR_COUNT			= 61040,// ĳ���� ���� Ƚ�� (ĳ���ͺ� �α��� Ƚ����� �ؾ� �ϳ�...)

	AT_ACHIEVEMENT_EXPCARD_COUNT			= 61041,// ����ġ ī�� ��� Ƚ��
	AT_ACHIEVEMENT_EXPCARD_DATE				= 61042,// ����ġ ī�� ��� ����

	AT_ACHIEVEMENT_ITEM						= 61045,// ���������� �����ϴ� ���� �̰� ������ �޼��� �� ����.

	AT_ACHIEVEMENT_LOGIN_0					= 61046,// ���� �ð��뿡 ���� ī��Ʈ 0��
//������� ���� (����)
	AT_ACHIEVEMENT_LOGIN_23					= 61069,// ���� �ð��뿡 ���� ī��Ʈ 23��

	AT_ACHIEVEMENT_CRAZYLEVELUP				= 61070,// ������ �⺻

	AT_ACHIEVEMENT_FEARLESS					= 61071,// �� ���� ��ɲ�

	AT_ACHIEVEMENT_CTRLOFGOD				= 61072,// �ŵ鸰 ��Ʈ��

	AT_ACHIEVEMENT_FIGHTEROFWIND			= 61073,// �ٶ��� ������

	AT_ACHIEVEMENT_60MILLIONMEN				= 61074,// 6��п�1 �糪��?

	AT_ACHIEVEMENT_FAMOUS					= 61075,// �����ѳ� �Ѥ�;;

	AT_ACHIEVEMENT_DEALKING					= 61076,// ��ȸ Ÿ������ ���� ������ �翡 ���� �ִ� ����

	AT_ACHIEVEMENT_COMBO					= 61077,// �޺� ����

	AT_ACHIEVEMENT_USE_STYLEITEM_HAIR		= 61078,// ��Ÿ�� ������ ���
	AT_ACHIEVEMENT_USE_STYLEITEM_FACE		= 61079,// ��Ÿ�� ������ ���

	AT_ACHIEVEMENT_MYHOME_EQUIP				= 61080,// ����Ȩ ������ ��ġ Ƚ��

	AT_ACHIEVEMENT_SEND_MAIL				= 61081,// ���� ���� Ƚ��

	AT_ACHIEVEMENT_SOULEATER				= 61082,// �ҿ� ��밳��

	AT_ACHIEVEMENT_USE_SMS					= 61083,// Ȯ���� ���

	AT_ACHIEVEMENT_USE_POTION				= 61084,// ���� ���

	AT_ACHIEVEMENT_CASH_BONUS				= 61085,// DP ������

	AT_ACHIEVEMENT_BESTSELLER				= 61086,// �Ǹſ�??

	AT_ACHIEVEMENT_SPRINT					= 61087,// ���

	AT_ACHIEVEMENT_EQUIP_ITEMGRADE0			= 61088,// ���� ������ ��޺� ����
//������� ���� (����)
	AT_ACHIEVEMENT_EQUIP_ITEMGRADE4			= 61092,// ���� ������ ��޺� ����

	AT_ACHIEVEMENT_USE_PARTYMOVE			= 61093,// ��Ƽ �̵� ������ ���

	AT_ACHIEVEMENT_USE_COUPLEWARP			= 61094,// Ŀ�� �̵� ������ ���

	AT_ACHIEVEMENT_MARRY					= 61095,// ��ȥ Ƚ��

	AT_ACHIEVEMENT_COUPLE					= 61096,// Ŀ�� Ƚ��

	AT_ACHIEVEMENT_1DAYCOUPLE				= 61097,// ���� Ŀ�� Ƚ��

	AT_ACHIEVEMENT_SINGLECLUB				= 61098,// ����Ŭ��

	AT_ACHIEVEMENT_FRIEND					= 61099,// ģ�� ī��Ʈ

	AT_ACHIEVEMENT_KILL_MONSTER_MIN			= 61100,// ���� ��� Ƚ��	(����� AT_ACHIEVEMENT_MONSTER_TYPE ����� ��� �ؾ� �� ����� ���� �Ф�;;)
/**/
/**/
/**/
/**/
	AT_ACHIEVEMENT_KILL_MONSTER_MAX			= 62000,// ���� ��� Ÿ�� �ִ� 

	AT_ACHIEVEMENT_MISSION_MIN				= 62001,// �̼� �Ϸ� Ƚ�� ���� ( ����� �ణ�� ����� �ʿ� = (61201 + MissionKey * 10 + MissionLevel) ���� ���� ���°���...)
/**/
/**/
/**/
/**/
	AT_ACHIEVEMENT_MISSION_MAX				= 64000,// �̼� �Ϸ� Ƚ�� ������

	AT_ACHIEVEMENT_ENCHANT_NOFAIL9			= 64001,// ���� ���� ���� ��þƮ �޼�9
//������� ���� (����)
	AT_ACHIEVEMENT_ENCHANT_NOFAIL31			= 64023,// ���� ���� ���� ��þƮ �޼�31

	AT_ACHIEVEMENT_MAP_MONSTER_KILL_MIN		= 64031,// MAP KEY �� ������ �ʿ��� ���� ��� Ƚ��
// ������� ����  (����)
	AT_ACHIEVEMENT_MAP_MONSTER_KILL_MAX		= 64332,// MAP KEY �� ������ �ʿ��� ���� ��� Ƚ��

	AT_ACHIEVEMENT_PARTY_MASTER				= 64333,// ��Ƽ�� Ƚ��

	AT_ACHIEVEMENT_COUPLE_START_DATE		= 64334,// Ŀ�� ���� �ð�

	AT_ACHIEVEMENT_TEXTIME					= 64335,// ��� ä����

	AT_ACHIEVEMENT_BIDDING					= 64336,// ���� �߰��

	AT_ACHIEVEMENT_ILLEGALVISITOR			= 64337,// �ҹ� ������

	AT_ACHIEVEMENT_OPENMARKET_SELL_CASH		= 64338,// ���� ���� ĳ�� �Ǹ�

	AT_ACHIEVEMENT_OPENMARKET_SELL_GOLD		= 64339,// ���� ���� ��� �Ǹ�

	AT_ACHIEVEMENT_DIVORCE					= 64340,// ��ȥ Ƚ��

	AT_ACHIEVEMENT_INVITATION				= 64341,// �ʴ��� �߱� Ƚ��

	AT_ACHIEVEMENT_STALKER					= 64342,// ����Ŀ

	AT_ACHIEVEMENT_BETRAYER					= 64343,// �����

	AT_ACHIEVEMENT_MARRYMONEY				= 64344,// ���Ǳ�

	AT_ACHIEVEMENT_CASHSHOPGIFT				= 64345,// ĳ�ü� ����(��Ÿ)

	AT_ACHIEVEMENT_COMPLETE_FIRST			= 64346,// ���� ���� ���� �޼�

	AT_ACHIEVEMENT_NPC_TALK					= 64347,// NPC ��ȭ Ƚ��

	AT_ACHIEVEMENT_MISSION_NO_HIT			= 64348,// �̼� �ǰ� ���� Ŭ����(�ڱ� ���� �´� �̼�)

	AT_ACHIEVEMENT_CASHSHOPBUY_EQUIP		= 64349,// �ٹ̱� ������ ���� Ƚ��

	AT_ACHIEVEMENT_CERTIFICATEOFHERO		= 64350,// ������ ��ǥ ��� ����

	AT_ACHIEVEMENT_PET_DEAD					= 64351,// 2�� �� ��� Ƚ��

	AT_ACHIEVEMENT_PET_ALIVETIME			= 64352,// 2�� �� ���� �ð�

	AT_ACHIEVEMENT_RANDOM_QUEST_COMPLETE	= 64353,// ���� ����Ʈ(����G)

	AT_ACHIEVEMENT_POPULARPOINT				= 64354,// �α⵵

	AT_ACHIEVEMENT_TODAYPOPULARPOINT		= 64355,// ������ �α⵵
	
	AT_ACHIEVEMENT_CHEKCPLAYTIME			= 64356,// �÷��� Ÿ��

	AT_ACHIEVEMENT_RECOMMEND_POINT			= 64357,// ���� �ٸ� ��� ��õ�� ȸ���� ���� ����

	AT_ACHIEVEMENT_EMPORIA_VICTORY			= 64358,// �������� ��� ����(���� BattleIndex, 1: ���, 100: 1��, 101: 2��)

	AT_ACHIEVEMENT_PVP_LEAGUE_REWARD		= 64359,// PVP ���� ���� ����( 16��, 8��, 4��, �ؿ��, ��� )

	AT_ACHIEVEMENT_MISSION_HELPER			= 64360,// �ڽź��� 5~10���� ���� ������ �̼� �÷��� Ƚ��( ������ 40���� ���ϸ� 3ȸ�� ����, �׿ܴ� 1ȸ )

// ���⼭ ���� ���� Ƚ�� üũ �̺�Ʈ ��� ���� �󸶳� Ȱ�� �ұ�....
	AT_EVENTFUNC_CONNECT_EVENT01			= 64501, // �̺�Ʈ ��� ���� Ƚ�� ���� �̺�Ʈ ��ȣ
/**/
	AT_EVENTFUNC_CONNECT_EVENT10			= 64510, // �̺�Ʈ ��� ���� Ƚ�� ���� �̺�Ʈ ��ȣ

	AT_EVENTFUNC_CONNECT_EVENT11			= 64511, // �Ϸ� �ѹ� ���� ���� �̺�Ʈ ��ȣ
	AT_EVENTFUNC_CONNECT_EVENT12			= 64512, // ���� �Ⱓ �� �ѹ��� �ִ� ����(�ΰ� Classlimit üũ)
	AT_EVENTFUNC_CONNECT_EVENT13			= 64513, // ���� �Ⱓ �� �ѹ��� �ִ� ����(���� DraClasslimit üũ)

	AT_GET_ITEM_MESSAGE						= 64514, //Ư�������� ȹ��� ���ϸ޽��� ����

	AT_EVENTFUNC_CONNECT_COUNT01			= 64521, // ���� Ƚ�� ���
/**/
	AT_EVENTFUNC_CONNECT_COUNT10			= 64530,	// ���� Ƚ�� ���
	AT_REST_EXP_ADD_MAX_LOW32				= 64531,	//  �޽� �߰� ����ġ ���� (LOW32)
	AT_REST_EXP_ADD_MAX_HIGH32				= 64532,	//  �޽� �߰� ����ġ ���� (HIGH32)
	AT_CHARACTER_LAST_LOGOUT_TIME			= 64533,	//  ĳ���Ͱ� �ֱ� �α׾ƿ��� �ð�	

	AT_ACHIEVEMENT_DURABLENESS_ZERO			= 64540,	// ������ ���(����,���,����,����,�尩,�Ź�,����)�� ��� 0�� �Ǹ� ����

	AT_ACHIEVEMENT_NORMAL_REPAIR			= 64550,	// "������ 0"�� �Ϲ� ��� ���� ȸ��
	AT_ACHIEVEMENT_RARE_REPAIR				= 64551,	// ����
	AT_ACHIEVEMENT_UNIQUE_REPAIR			= 64552,	// �����
	AT_ACHIEVEMENT_ARTIFACT_REPAIR			= 64553,	// ����
	AT_ACHIEVEMENT_LEGEND_REPAIR			= 64554,	// ����

	AT_ACHIEVEMENT_COLLECT_COUNT			= 64560,	// ���� ���� ����

	AT_ACHIEVEMENT_INVINCIBLE 				= 64570,	// �ѹ��� �����ʰ� 20,40,60,80 ���� �޼�

	AT_ACHIEVEMENT_USE_GADACOIN				= 64580,	// �������� ��� ����
	AT_ACHIEVEMENT_USE_SOULSTONE			= 64590,	// �Ǹ��� ��ȥ�� ��� ����

	AT_ACHIEVEMENT_HIDDEN_PLAY_TIME			= 64600,	// ����� �÷��� �ð�

	AT_ACHIEVEMENT_EQUIP_SET30				= 64610,	// 30���� ��Ʈ ��� ��� ����� ȹ�� ����
	AT_ACHIEVEMENT_EQUIP_SET40				= 64611,	// 30���� ��Ʈ ��� ��� ����� ȹ�� ����
	AT_ACHIEVEMENT_EQUIP_SET50				= 64612,	// 50���� ��Ʈ ��� ��� ����� ȹ�� ����
	AT_ACHIEVEMENT_EQUIP_SET60				= 64613,	// 60���� ��Ʈ ��� ��� ����� ȹ�� ����
	
	AT_ACHIEVEMENT_RELATION_CLAER			= 64614,	// ������ ����(���� GroupNo)�� ��� Ŭ���� �ϸ� �޼��Ǵ� ����
	
	AT_ACHIEVEMENT_DUNGEON_ANYONE_DIE		= 64615,	// ��Ƽ���� ����: �ƹ��� ���� �ʰ� ���� Ŭ����( Value: 1, CustomValue: �������� ��ǥ��ȣ )
	AT_ACHIEVEMENT_DUNGEON_ATTACKED_COUNT	= 64616,	// ���� �ǰ�ȸ�� ���Ϸ� ���� Ŭ����( Value: �ǰ�ȸ��, CustomValue: �������� ��ȣ )
	AT_ACHIEVEMENT_DUNGEON_TIMEATTACK		= 64617,	// ���� �ð��ȿ� ���� Ŭ����( Value: �ð�(�ʴ��� �Է�), CustomValue: �������͹�ȣ )

	AT_ACHIEVEMENT_ATTR_LOW_FIRE			= 64700, 	// ���� ȭ���Ӽ� ���� ������ ������
	AT_ACHIEVEMENT_ATTR_HIGH_FIRE			= 64701,	// ���� ȭ���Ӽ� ���� ������ ������
	AT_ACHIEVEMENT_ATTR_LOW_ICE				= 64702,	// ���� �ñ�Ӽ� ���� ������ ������
	AT_ACHIEVEMENT_ATTR_HIGH_ICE			= 64703,	// ���� �ñ�Ӽ� ���� ������ ������
	AT_ACHIEVEMENT_ATTR_LOW_NATURE			= 64704,	// ���� �ñ�Ӽ� ���� ������ ������
	AT_ACHIEVEMENT_ATTR_HIGH_NATURE			= 64705,	// ���� �ڿ��Ӽ� ���� ������ ������
	AT_ACHIEVEMENT_ATTR_LOW_CURSE			= 64706,	// ���� ���ּӼ� ���� ������ ������
	AT_ACHIEVEMENT_ATTR_HIGH_CURSE			= 64707,	// ���� ���ּӼ� ���� ������ ������
	AT_ACHIEVEMENT_ATTR_LOW_DESTROY			= 64708,	// ���� �ı��Ӽ� ���� ������ ������

	AT_ACHIEVEMENT_ATTR_ATTACK_FIRE			= 64710,	// ȭ�� �Ӽ� ���ݷ� ������ġ �̻��� ��
	AT_ACHIEVEMENT_ATTR_ATTACK_ICE			= 64711, 	// �ñ� �Ӽ� ���ݷ� ������ġ �̻��� ��
	AT_ACHIEVEMENT_ATTR_ATTACK_NATURE		= 64712, 	// �ڿ� �Ӽ� ���ݷ� ������ġ �̻��� ��
	AT_ACHIEVEMENT_ATTR_ATTACK_CURSE		= 64713,	// ���� �Ӽ� ���ݷ� ������ġ �̻��� ��
	AT_ACHIEVEMENT_ATTR_ATTACK_DESTORY		= 64714, 	// �ı� �Ӽ� ���ݷ� ������ġ �̻��� ��
	AT_ACHIEVEMENT_ATTR_ATTACK_5ELEMENT		= 64715, 	// 5��  �Ӽ� ���ݷ� ������ġ �̻��� ��
	AT_ACHIEVEMENT_ATTR_RESIST_FIRE			= 64716, 	// ȭ�� �Ӽ� ���׷� ������ġ �̻��� ��
	AT_ACHIEVEMENT_ATTR_RESIST_ICE			= 64717, 	// �ñ� �Ӽ� ���׷� ������ġ �̻��� ��
	AT_ACHIEVEMENT_ATTR_RESIST_NATURE		= 64718, 	// �ڿ� �Ӽ� ���׷� ������ġ �̻��� ��
	AT_ACHIEVEMENT_ATTR_RESIST_CURSE		= 64719, 	// ���� �Ӽ� ���׷� ������ġ �̻��� ��
	AT_ACHIEVEMENT_ATTR_RESIST_DESTORY		= 64720, 	// �ı� �Ӽ� ���׷� ������ġ �̻��� ��
	AT_ACHIEVEMENT_ATTR_RESIST_5ELEMENT		= 64721,	// 5��  �Ӽ� ���׷� ������ġ �̻��� ��

	AT_ACHIEVEMENT_BSQ_RESULT_WIN			= 64722,
//EMPORIA ��ɰ� ���õ� ���
//EMFUNC ���� 64901~65000 ��� ����.
	AT_EMFUNC_LAST_CONTRIBUTE_MINTIME		= 64901,
	//AT_EMFUNC_END							= 65000,
	
	AT_ACHIEVEMENT_DATA_MAX					= 65000,// ���� �˻�� ������ ���� �ִ� ( ���� ���� ��Ͽ� ������ ������ 1000���� �ѱ��� �ʰ��� �Ѥ�;;)

	AT_MONSTER_TUNNING_NO					= 65001,
	AT_MONSTER_TUNNING_GRADE				= 65002,
	AT_MONSTER_TUNNING_USER_LV				= 65003,
	AT_MONSTER_TUNNING_WEIGHT				= 65004,

	AT_SHARE_HP_RELATIONSHIP				= 65006,

	AT_STRATEGYSKILL_OPEN					= 65008,//������ų ��������
	AT_STRATEGYSKILL_TABNO					= 65009,//������� ������ų ��ȣ
	AT_SKILLSET_USE_COUNT					= 65010,//SkillSet CashItem ��밹��

	AT_ACHIEVEMENT_TOTAL_POINT				= 65011, // ���� �޼� ��Ż Point
	AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_1	= AT_ACHIEVEMENT_TOTAL_POINT + 1, // 65012, ī�װ��� 1, ���� ����Ʈ
	AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_2	= AT_ACHIEVEMENT_TOTAL_POINT + 2, // 65013, ī�װ��� 2, ������ ����Ʈ
	AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_3	= AT_ACHIEVEMENT_TOTAL_POINT + 3, // 65014, ī�װ��� 3, ����Ʈ ����Ʈ
	AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_4	= AT_ACHIEVEMENT_TOTAL_POINT + 4, // 65015, ī�װ��� 4, ���� ����Ʈ
	AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_5	= AT_ACHIEVEMENT_TOTAL_POINT + 5, // 65016, ī�װ��� 5, �̼� ����Ʈ
	AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_6	= AT_ACHIEVEMENT_TOTAL_POINT + 6, // 65017, ī�װ��� 6, ��Ÿ ����Ʈ(��κ� 0���� ���õ�)

	AT_STRATEGY_FATIGABILITY				= 65101, // �����Ƿε�. �������潺(F7).
	AT_RECOVERY_STRATEGY_FATIGABILITY		= 65102, // �����Ƿε� ȸ��.

	AT_GUADIANDISCOUNT_TYPE					= 65201, // ����� ��ġ�� ���� ���.
	AT_GUADIANDISCOUNT_RATE					= 65202, // ����� ��ġ�� ������.

	AT_INSTANCE_ITEM						= 65301, // �ν��Ͻ� ������. ����� 1�� �������� �ν��Ͻ� ������.

	AT_IGNORE_PVP_MODE_ABIL					= 65401, // PvP�ÿ� �ɼ� �������� �����ؾ��ϴµ� PvP������ �Ǵ��� AT_TEAM����� AT_DUEL�������.
													 // ������ ���潺 ��忡�� AT_TEAM Abil�� ���õǴµ� PvP�� �Ǵ��Ͽ� �ɼ� �������� �����ϰ� ��.
													 // �� ����� ���� �Ǿ�������, AT_TEAM�̳� AT_DUEL�� ���� �Ǿ��־ �����ϰ� PvP���� �ƴ϶�� �Ǵ���.

// PvPLobby������ ����ϴ� ���
	AT_PVP_ROOM_INDEX						= 65500,
	AT_PVP_CELL_INDEX						= 65501,

	AT_DEBUG_JOBSKILL						= 65530,
	AT_MAX_ABIL								= USHRT_MAX,//65535( �̰��� ������ �ȵȴ� )

}EAbilType;

#define AT_ACHIEVEMENT_MISSION_KEY(key,level) (AT_ACHIEVEMENT_MISSION_MIN + (key * 10) + level)

typedef enum eDamageActionType
{
	E_DMGACT_TYPE_DEFAULT			= 0,
	E_DMGACT_TYPE_NODELAY			= 1,// No Delay
	E_DMGACT_TYPE_ONLY_DAMAGE		= 10, // Only Damage
	E_DMGACT_TYPE_NO				= 101,
	E_DMGACT_AI_CHASE_ENEMY			= 0x40000000,
	E_DMGACT_AI_FIRESKILL			= 0x80000000,
	E_DMGACT_CHECK					= 0xF00000FF,

	E_DMGACT_TYPE_TRANSIT_PARENT	= 0x00000100, //(256)
	E_DMGACT_TYPE_PART_SUPERARMOUR	= 0x00000200, //(512)
}EDamageActionType;

enum eEffectControlType
{
	E_EFFCONTROL_NONE				= 0x00,
	E_EFFCONTROL_DEL_MONEY			= 0x01,
	E_EFFCONTROL_DEL_ITEM			= 0x02,
	E_EFFCONTROL_REQ_SERVER			= E_EFFCONTROL_DEL_MONEY|E_EFFCONTROL_DEL_ITEM,	// �������� �Ǵ��� ��û(BitFlag)
};
typedef BYTE EffectControlType;

typedef enum eDynamicAbilType
{
	// ���Ͱ� �ִ� ����ġ
	DAT_NO_RC_TYPE_MIN				= 3000						,
	DAT_DIE_EXP						= 3030/*AT_DIE_EXP-3*/		,
	DAT_DIE_EXP_GRADE1											,
	DAT_DIE_EXP_GRADE2											,
	DAT_DIE_EXP_GRADE3											,
	DAT_DIE_EXP_GRADE4											,
	DAT_NO_RC_TYPE_MAX				= 4000						,

	DAT_RC_TYPE_MIN					= AT_CALCUATEABIL_MIN-1		,
	DAT_MAX_HP						= AT_MAX_HP-1				,
	DAT_MAX_HP_GRADE1											,
	DAT_MAX_HP_GRADE2											,
	DAT_MAX_HP_GRADE3											,
	DAT_MAX_HP_GRADE4											,
	DAT_HP_RECOVERY_INTERVAL		= AT_HP_RECOVERY_INTERVAL-1	,
	DAT_HP_RECOVERY_INTERVAL_GRADE1								,
	DAT_HP_RECOVERY_INTERVAL_GRADE2								,
	DAT_HP_RECOVERY_INTERVAL_GRADE3								,
	DAT_HP_RECOVERY_INTERVAL_GRADE4								,
	DAT_MAX_MP						= AT_MAX_MP-1				,
	DAT_MAX_MP_GRADE1											,
	DAT_MAX_MP_GRADE2											,
	DAT_MAX_MP_GRADE3											,
	DAT_MAX_MP_GRADE4											,
	DAT_MP_RECOVERY_INTERVAL		= AT_MP_RECOVERY_INTERVAL-1	,
	DAT_MP_RECOVERY_INTERVAL_GRADE1								,
	DAT_MP_RECOVERY_INTERVAL_GRADE2								,
	DAT_MP_RECOVERY_INTERVAL_GRADE3								,
	DAT_MP_RECOVERY_INTERVAL_GRADE4								,
	DAT_STR							= AT_STR-1					,
	DAT_STR_GRADE1												,
	DAT_STR_GRADE2												,
	DAT_STR_GRADE3												,
	DAT_STR_GRADE4												,
	DAT_INT							= AT_INT-1					,
	DAT_INT_GRADE1												,
	DAT_INT_GRADE2												,
	DAT_INT_GRADE3												,
	DAT_INT_GRADE4												,
	DAT_CON							= AT_CON-1					,
	DAT_CON_GRADE1												,
	DAT_CON_GRADE2												,
	DAT_CON_GRADE3												,
	DAT_CON_GRADE4												,
	DAT_DEX							= AT_DEX-1					,
	DAT_DEX_GRADE1												,
	DAT_DEX_GRADE2												,
	DAT_DEX_GRADE3												,
	DAT_DEX_GRADE4												,
	DAT_MOVESPEED					= AT_MOVESPEED-1			,
	DAT_MOVESPEED_GRADE1										,
	DAT_MOVESPEED_GRADE2										,
	DAT_MOVESPEED_GRADE3										,
	DAT_MOVESPEED_GRADE4										,
	DAT_PHY_DEFENCE					= AT_PHY_DEFENCE-1			,
	DAT_PHY_DEFENCE_GRADE1										,
	DAT_PHY_DEFENCE_GRADE2										,
	DAT_PHY_DEFENCE_GRADE3										,
	DAT_PHY_DEFENCE_GRADE4										,
	DAT_MAGIC_DEFENCE				= AT_MAGIC_DEFENCE-1		,
	DAT_MAGIC_DEFENCE_GRADE1									,
	DAT_MAGIC_DEFENCE_GRADE2									,
	DAT_MAGIC_DEFENCE_GRADE3									,
	DAT_MAGIC_DEFENCE_GRADE4									,
	DAT_ATTACK_SPEED				= AT_ATTACK_SPEED-1			,
	DAT_ATTACK_SPEED_GRADE1										,
	DAT_ATTACK_SPEED_GRADE2										,
	DAT_ATTACK_SPEED_GRADE3										,
	DAT_ATTACK_SPEED_GRADE4										,
	DAT_BLOCK_SUCCESS_VALUE					= AT_BLOCK_SUCCESS_VALUE-1			,
	DAT_BLOCK_SUCCESS_VALUE_GRADE1										,
	DAT_BLOCK_SUCCESS_VALUE_GRADE2										,
	DAT_BLOCK_SUCCESS_VALUE_GRADE3										,
	DAT_BLOCK_SUCCESS_VALUE_GRADE4										,
	DAT_DODGE_SUCCESS_VALUE					= AT_DODGE_SUCCESS_VALUE-1			,
	DAT_DODGE_SUCCESS_VALUE_GRADE1										,
	DAT_DODGE_SUCCESS_VALUE_GRADE2										,
	DAT_DODGE_SUCCESS_VALUE_GRADE3										,
	DAT_DODGE_SUCCESS_VALUE_GRADE4										,
	DAT_CRITICAL_SUCCESS_VALUE				= AT_CRITICAL_SUCCESS_VALUE-1		,
	DAT_CRITICAL_SUCCESS_VALUE_GRADE1									,
	DAT_CRITICAL_SUCCESS_VALUE_GRADE2									,
	DAT_CRITICAL_SUCCESS_VALUE_GRADE3									,
	DAT_CRITICAL_SUCCESS_VALUE_GRADE4									,
	DAT_CRITICAL_POWER				= AT_CRITICAL_POWER-1		,
	DAT_CRITICAL_POWER_GRADE1									,
	DAT_CRITICAL_POWER_GRADE2									,
	DAT_CRITICAL_POWER_GRADE3									,
	DAT_CRITICAL_POWER_GRADE4									,
	DAT_INVEN_SIZE					= AT_INVEN_SIZE-1			,
	DAT_INVEN_SIZE_GRADE1										,
	DAT_INVEN_SIZE_GRADE2										,
	DAT_INVEN_SIZE_GRADE3										,
	DAT_INVEN_SIZE_GRADE4										,
	DAT_EQUIPS_SIZE					= AT_EQUIPS_SIZE-1			,
	DAT_EQUIPS_SIZE_GRADE1										,
	DAT_EQUIPS_SIZE_GRADE2										,
	DAT_EQUIPS_SIZE_GRADE3										,
	DAT_EQUIPS_SIZE_GRADE4										,
	DAT_ATTACK_RANGE				= AT_ATTACK_RANGE-1			,
	DAT_ATTACK_RANGE_GRADE1										,
	DAT_ATTACK_RANGE_GRADE2										,
	DAT_ATTACK_RANGE_GRADE3										,
	DAT_ATTACK_RANGE_GRADE4										,
	DAT_HP_RECOVERY					= AT_HP_RECOVERY-1			,
	DAT_HP_RECOVERY_GRADE1										,
	DAT_HP_RECOVERY_GRADE2										,
	DAT_HP_RECOVERY_GRADE3										,
	DAT_HP_RECOVERY_GRADE4										,
	DAT_MP_RECOVERY					= AT_MP_RECOVERY-1			,
	DAT_MP_RECOVERY_GRADE1										,
	DAT_MP_RECOVERY_GRADE2										,
	DAT_MP_RECOVERY_GRADE3										,
	DAT_MP_RECOVERY_GRADE4										,
	DAT_JUMP_HEIGHT					= AT_JUMP_HEIGHT-1			,
	DAT_JUMP_HEIGHT_GRADE1										,
	DAT_JUMP_HEIGHT_GRADE2										,
	DAT_JUMP_HEIGHT_GRADE3										,
	DAT_JUMP_HEIGHT_GRADE4										,
	DAT_PHY_ATTACK_MAX				= AT_PHY_ATTACK_MAX-1		,
	DAT_PHY_ATTACK_MAX_GRADE1									,
	DAT_PHY_ATTACK_MAX_GRADE2									,
	DAT_PHY_ATTACK_MAX_GRADE3									,
	DAT_PHY_ATTACK_MAX_GRADE4									,
	DAT_PHY_ATTACK_MIN				= AT_PHY_ATTACK_MIN-1		,
	DAT_PHY_ATTACK_MIN_GRADE1									,
	DAT_PHY_ATTACK_MIN_GRADE2									,
	DAT_PHY_ATTACK_MIN_GRADE3									,
	DAT_PHY_ATTACK_MIN_GRADE4									,
	DAT_TARGET_BACK_DISTANCE		= AT_TARGET_BACK_DISTANCE-1	,
	DAT_TARGET_BACK_DISTANCE_GRADE1								,
	DAT_TARGET_BACK_DISTANCE_GRADE2								,
	DAT_TARGET_BACK_DISTANCE_GRADE3								,
	DAT_TARGET_BACK_DISTANCE_GRADE4								,
	DAT_TARGET_FLY_DISTANCE			= AT_TARGET_FLY_DISTANCE-1	,
	DAT_TARGET_FLY_DISTANCE_GRADE1								,
	DAT_TARGET_FLY_DISTANCE_GRADE2								,
	DAT_TARGET_FLY_DISTANCE_GRADE3								,
	DAT_TARGET_FLY_DISTANCE_GRADE4								,
	DAT_NEED_MP						= AT_NEED_MP-1				,
	DAT_NEED_MP_GRADE1											,
	DAT_NEED_MP_GRADE2											,
	DAT_NEED_MP_GRADE3											,
	DAT_NEED_MP_GRADE4											,
	DAT_NEED_HP						= AT_NEED_HP-1				,
	DAT_NEED_HP_GRADE1											,
	DAT_NEED_HP_GRADE2											,
	DAT_NEED_HP_GRADE3											,
	DAT_NEED_HP_GRADE4											,
	DAT_MAGIC_ATTACK				= AT_MAGIC_ATTACK-1			,
	DAT_MAGIC_ATTACK_GRADE1										,
	DAT_MAGIC_ATTACK_GRADE2										,
	DAT_MAGIC_ATTACK_GRADE3										,
	DAT_MAGIC_ATTACK_GRADE4										,
	DAT_PHY_ATTACK					= AT_PHY_ATTACK-1			,
	DAT_PHY_ATTACK_GRADE1										,
	DAT_PHY_ATTACK_GRADE2										,
	DAT_PHY_ATTACK_GRADE3										,
	DAT_PHY_ATTACK_GRADE4										,
	DAT_CRITICAL_MPOWER				= AT_CRITICAL_MPOWER-1		,
	DAT_CRITICAL_MPOWER_GRADE1									,
	DAT_CRITICAL_MPOWER_GRADE2									,
	DAT_CRITICAL_MPOWER_GRADE3									,
	DAT_CRITICAL_MPOWER_GRADE4									,
	DAT_HIT_SUCCESS_VALUE						= AT_HIT_SUCCESS_VALUE-1				,
	DAT_HIT_SUCCESS_VALUE_GRADE1											,
	DAT_HIT_SUCCESS_VALUE_GRADE2											,
	DAT_HIT_SUCCESS_VALUE_GRADE3											,
	DAT_HIT_SUCCESS_VALUE_GRADE4											,
	DAT_WALK_SPEED					= AT_WALK_SPEED-1			,
	DAT_WALK_SPEED_GRADE1										,
	DAT_WALK_SPEED_GRADE2										,
	DAT_WALK_SPEED_GRADE3										,
	DAT_WALK_SPEED_GRADE4										,
	DAT_MAGIC_ATTACK_MAX			= AT_MAGIC_ATTACK_MAX-1		,
	DAT_MAGIC_ATTACK_MAX_GRADE1									,
	DAT_MAGIC_ATTACK_MAX_GRADE2									,
	DAT_MAGIC_ATTACK_MAX_GRADE3									,
	DAT_MAGIC_ATTACK_MAX_GRADE4									,
	DAT_MAGIC_ATTACK_MIN			= AT_MAGIC_ATTACK_MIN-1		,
	DAT_MAGIC_ATTACK_MIN_GRADE1									,
	DAT_MAGIC_ATTACK_MIN_GRADE2									,
	DAT_MAGIC_ATTACK_MIN_GRADE3									,
	DAT_MAGIC_ATTACK_MIN_GRADE4									,
	DAT_PHY_DMG_DEC					= AT_PHY_DMG_DEC-1			,
	DAT_PHY_DMG_DEC_GRADE1										,
	DAT_PHY_DMG_DEC_GRADE2										,
	DAT_PHY_DMG_DEC_GRADE3										,
	DAT_PHY_DMG_DEC_GRADE4										,
	DAT_MAGIC_DMG_DEC				= AT_MAGIC_DMG_DEC-1		,
	DAT_MAGIC_DMG_DEC_GRADE1									,
	DAT_MAGIC_DMG_DEC_GRADE2									,
	DAT_MAGIC_DMG_DEC_GRADE3									,
	DAT_MAGIC_DMG_DEC_GRADE4									,
	DAT_CASTING_SPEED				= AT_CASTING_SPEED-1		,
	DAT_CASTING_SPEED_GRADE1									,
	DAT_CASTING_SPEED_GRADE2									,
	DAT_CASTING_SPEED_GRADE3									,
	DAT_CASTING_SPEED_GRADE4									,
	DAT_RESIST_FIRE					= AT_RESIST_FIRE-1			,
	DAT_RESIST_FIRE_GRADE1										,
	DAT_RESIST_FIRE_GRADE2										,
	DAT_RESIST_FIRE_GRADE3										,
	DAT_RESIST_FIRE_GRADE4										,
	DAT_RESIST_WATER				= AT_RESIST_WATER-1			,
	DAT_RESIST_WATER_GRADE1										,
	DAT_RESIST_WATER_GRADE2										,
	DAT_RESIST_WATER_GRADE3										,
	DAT_RESIST_WATER_GRADE4										,
	DAT_RESIST_TREE					= AT_RESIST_TREE-1			,
	DAT_RESIST_TREE_GRADE1										,
	DAT_RESIST_TREE_GRADE2										,
	DAT_RESIST_TREE_GRADE3										,
	DAT_RESIST_TREE_GRADE4										,
	DAT_RESIST_IRON					= AT_RESIST_IRON-1			,
	DAT_RESIST_IRON_GRADE1										,
	DAT_RESIST_IRON_GRADE2										,
	DAT_RESIST_IRON_GRADE3										,
	DAT_RESIST_IRON_GRADE4										,
	DAT_RESIST_EARTH				= AT_RESIST_EARTH-1			,
	DAT_RESIST_EARTH_GRADE1										,
	DAT_RESIST_EARTH_GRADE2										,
	DAT_RESIST_EARTH_GRADE3										,
	DAT_RESIST_EARTH_GRADE4										,
	DAT_HP_POTION_ADD_RATE			= AT_HP_POTION_ADD_RATE-1	,
	DAT_HP_POTION_ADD_RATE_GRADE1								,
	DAT_HP_POTION_ADD_RATE_GRADE2								,
	DAT_HP_POTION_ADD_RATE_GRADE3								,
	DAT_HP_POTION_ADD_RATE_GRADE4								,
	DAT_MP_POTION_ADD_RATE			= AT_MP_POTION_ADD_RATE-1	,
	DAT_MP_POTION_ADD_RATE_GRADE1								,
	DAT_MP_POTION_ADD_RATE_GRADE2								,
	DAT_MP_POTION_ADD_RATE_GRADE3								,
	DAT_MP_POTION_ADD_RATE_GRADE4								,
	DAT_DMG_REFLECT_RATE			= AT_DMG_REFLECT_RATE-1		,
	DAT_DMG_REFLECT_RATE_GRADE1									,
	DAT_DMG_REFLECT_RATE_GRADE2									,
	DAT_DMG_REFLECT_RATE_GRADE3									,
	DAT_DMG_REFLECT_RATE_GRADE4									,
	DAT_DMG_REFLECT_HITRATE			= AT_DMG_REFLECT_HITRATE-1	,
	DAT_DMG_REFLECT_HITRATE_GRADE1								,
	DAT_DMG_REFLECT_HITRATE_GRADE2								,
	DAT_DMG_REFLECT_HITRATE_GRADE3								,
	DAT_DMG_REFLECT_HITRATE_GRADE4								,
	DAT_PENETRATION_RATE			= AT_PENETRATION_RATE-1		,
	DAT_PENETRATION_RATE_GRADE1									,
	DAT_PENETRATION_RATE_GRADE2									,
	DAT_PENETRATION_RATE_GRADE3									,
	DAT_PENETRATION_RATE_GRADE4									,
// 	DAT_MAX_DP						= AT_MAX_DP-1				,
// 	DAT_MAX_DP_GRADE1											,
// 	DAT_MAX_DP_GRADE2											,
// 	DAT_MAX_DP_GRADE3											,
// 	DAT_MAX_DP_GRADE4											,
// 	DAT_DP_RECOVERY_INTERVAL		= AT_DP_RECOVERY_INTERVAL-1	,
// 	DAT_DP_RECOVERY_INTERVAL_GRADE1								,
// 	DAT_DP_RECOVERY_INTERVAL_GRADE2								,
// 	DAT_DP_RECOVERY_INTERVAL_GRADE3								,
// 	DAT_DP_RECOVERY_INTERVAL_GRADE4								,
	DAT_RC_TYPE_MAX					= AT_CALCUATEABIL_MAX		,
}EDynamicAbilType;

int const USE_AMOUNT_BASE_VALUE = 100000000;

int const ABILITY_RATE_VALUE = 10000;
__int64 const ABILITY_RATE_VALUE64 = 10000i64;
float const ABILITY_RATE_VALUE_FLOAT = 10000.0f;
float const ABILITY_FLOAT_MULTIFLY = 1000.0f;

#define MAKE_ABIL_RATE(value) ((value) * 100)

typedef enum eSkillAttribute
{
	SAT_MAINTENANCE_CAST = 0x00000001,	// �ѹ� �߻��, ��������� �߻���¸� �����ϴ� ��ų(ü�ζ���Ʈ�� ���� ��ų)
	SAT_CLIENT_CTRL_PROJECTILE = 0x00000002,	// Ŭ���̾�Ʈ�� �浹üũ�ϴ� �߻�ü ��ų
	SAT_SERVER_FIRE	= 0x00000004,	// Server���� SAT_CLIENT_CTRL_PROJECTILE �Ӽ��� ������� Fire �Լ��� ȣ��
	SAT_PHYSICS		= 0x01000000,	// �������� Skill (16777216)
	SAT_MAGIC		= 0x02000000,	// �������� skill (33554432)
	//SAT_CHARGING	= 0x04000000,	// Charging �����ΰ�? (67108864)
	SAT_DEFAULT		= 0x08000000,	// Default skill (����� �ʾƵ� �⺻���� �� �� �ִ� ��ų) (134217728)
} ESkillAttribute;	// AT_SKILL_ATT(2011)

typedef enum : int
{
	AUTOHEAL_NONE	= 0x00,
	AUTOHEAL_HP		= 0x01,
	AUTOHEAL_MP		= 0x02,
//	AUTOHEAL_DP		= 0x04,

	AUTOHEAL_PLAYER = AUTOHEAL_HP|AUTOHEAL_MP,
	AUTOHEAL_PET	= AUTOHEAL_MP,
	AUTOHEAL_MONSTER = AUTOHEAL_NONE,
} EAutoHealMode;

typedef enum eUnitGender
{
	UG_MALE = 1,
	UG_FEMALE = 2,
	UG_UNISEX = 3,
} EUnitGender;

typedef enum eAttackedUnitPos
{
	EAttacked_Pos_None = 0,
	EAttacked_Pos_Normal = (1<<0),
	EAttacked_Pos_ground = (1<<1),	// �ٴڿ� ������ ����
	EAttacked_Pos_Blowup = (1<<2),	// ���߿� ����� ����
	EAttacked_Pos_Obstacle = (1<<3),	//ĳ���Ϳ� Ÿ�� ���̿� ��ֹ��� �ִ��� �˻�
} EAttackedUnitPos;

typedef enum eAttackRangeSource
{
	EAttackRange_SkillRange	= 0,	// Skill:AT_ATTACK_RANGE ���� �д´�.
	EAttackRange_SkillRange2 = 1,	// EAttackRange_SkillRange�� ������ ������ �ǹ�
	EAttackRange_ItemRange = 2,		// Weapon:AT_ATTACK_RANGE ���� �д´�.
	EAttackRange_Skill_Item = 3,	// Skill+Item Range
	EAttackRange_UnitRange = 4,		// Unit:AT_ATTACK_RANGE ���� ����Ѵ�.
} EAttackRangeSource;

typedef enum eSkillArea
{
	ESArea_Sphere = 0,	// Default : �������� Skill Area
	ESArea_Cube = 1,	// ����ü ������ SkillArea
	ESArea_Cone = 2,	// �������� SkillArea
	ESArea_Front_Sphere = 3, //�� �� �������� �����Ÿ� �տ��� �ٽ� ������ üũ
	ESArea_Unit_Cube	= 4, //Unit�� �������� ����ü ����
	ESArea_Front_DoubleSphere = 5, //Unit���ʿ� �������� 2�� Area ����
}ESkillArea;

typedef enum eUseItemCustomType
{
	UICT_NONE = 0,
	UICT_RESET_SKILL_POINT = 1,	
	UICT_SMS = 2,	
	UICT_LETTER = 3,
	UICT_PARTY = 4,
	UICT_COUPLE = 5,
	UICT_SEAL_REMOVE = 6,
	UICT_MARKET_MODIFY = 7,
	UICT_OPEN_CHEST = 8, //���ָӴ�, ������ ���� ��..
	UICT_BONUS_RATE_BUF = 9,	//�߰� ����ġ ���.(%. �ð�����?) ����ġ, �����.
	UICT_ITEM_NAME_CHANGE = 10, //
//	UICT_AIG = 11,//������ ���� -> ��ȣ ������. �̷��� �� �ʿ�� ����.
	UICT_INSURANCE = 12,//�������
	UICT_ELIXIR = 13,//HP, MP ������.
	UICT_SAFEMODE = 14,//������ǰ
	UICT_RENTALSAFE = 15,// ĳ�ü� â��
	UICT_USERMAPMOVE = 16,// �����̵� ī��
	UICT_MOVETOPARTYMEMBER = 17,// ��Ƽ�����Է� �̵�
	UICT_SUMMONPARTYMEMBER = 18,// ��Ƽ�� ��ȯ
	UICT_ADD_SP = 19,	// Add SkillPoint
	UICT_REST = 20,		// �޽� ������
	UICT_OPEN_PACK = 21,// �� ������ 
	UICT_REVIVE = 22,	// ��Ȱ ������
	UICT_CHAOS_PORTAL = 23, //ī���� ����
	UICT_HIDDEN_PORTAL = 24, //���� ����
	UICT_GUILDMarkChange1 = 25, // ��� ��ũ ����(ĳ����)
	UICT_BIND = 26,	// ���ε� ������(ĳ����) �ڹ���?
	UICT_UNBIND = 27,// ����ε� ������(ĳ����) ����?
	UICT_STYLEITEM = 28,// ��/�Ӹ�ī��/�Ӹ���
	UICT_INVEXTEND = 29,// �ι��丮 ĭ Ȯ��
	UICT_SAFEEXTEND = 30,// â�� ĭ Ȯ��
	UICT_CREATE_CARD = 31,// ĳ���� ī�� ����
	UICT_MODIFY_CARD = 32,// ĳ���� ī�� ����
	UICT_USER_PORTAL = 33,// ��ġ ���� ��Ż
	UICT_MONEY_PACK	= 34,// �Ӵ���
	UICT_CASH_PACK	= 35,// ĳ����
	UICT_SEAL		= 36,// ����
	UICT_MONSTERCARD = 37,// ���� ī��
	UICT_DESTROYCARD = 38,// �ı� ī��
	UICT_EMOTION	 = 39,// �̸�� ī�� �׷�
	UICT_ROLLBACK	= 40,// ��þƮ �ѹ� ������
	UICT_SOCKET_SUCCESS = 41,// ���� ���� ���� Ȯ�� ��� ������
	UICT_LOCKED_CHEST = 42,// ��� ����
	UICT_CHEST_KEY	  = 43,// ���� ���¿� Ű
	UICT_BLANK		  = 44,// �� ������
	UICT_ONECEMORE	  =	45,// �ѹ� ��
	UICT_CHANGE_CLASS = 46,// ����
	UICT_GAMBLE		  = 47,// �׺� ������
	UICT_SUPER_GIRL	  = 48,// ���۰� ������
	UICT_ENCHANT_INSURANCE	= 49,// ��þƮ ����
	UICT_PLUSE_SUCCESS		= 50,// ��þƮ ���� Ȯ�� ������
	UICT_RARITY_SUCCESS		= 51,// �ҿ� ���� Ȯ�� ������
	UICT_PET_EGG			= 52,// Pet�� Egg
	UICT_PET				= 53,// Petȣ�� ������
	UICT_CALL_MARKET		= 54,// ���� ȣ��
	UICT_CALL_SAFE			= 55,// â�� ȣ��
	UICT_PROTECTION_GALL	= 56,// ������ �Ҹ� ����
	UICT_HELP_END_QUEST		= 57,// ����Ʈ ���Ḧ �����ش�
	UICT_PET_FEED			= 58,// �� ���
	UICT_EXPCARD			= 59,// ����ġ ī��
	UICT_EXPCARD_BREAKER	= 60,
	UICT_LEARNSKILL			= 61,// ��ų ����
	UICT_MAKING_SUCCESS		= 62,// �丮/���� ���� Ȯ�� ������
	UICT_EXTEND_MAX_IDX		= 63,// �κ��丮 �ִ� ���� Ȯ�� ������
	UICT_SAFE_EXTEND_MAX_IDX= 64,// â�� �ִ� ���� Ȯ�� ������
	UICT_HOME_STYLEITEM		= 65,// Ȩ ���� ���� ���� ������
	UICT_COUPLE_ITEM		= 66,// Ŀ�� �� �� ���

	UICT_HOME_POSTBOX		= 67,// ����Ȩ ������
	UICT_HOME_SAFEBOX		= 68,// ����Ȩ â��

	UICT_GAMBLE_COIN		= 69,// �׺��� ����
	UICT_ACHIEVEMENT		= 70,// ���� �޼��� ������

	UICT_COOLTIME_RATE_SKILL= 71,// ��ų ��Ÿ�� õ���� ���� ���� ������
	UICT_ADD_COOLTIME_SKILL	= 72,// ��ų ��Ÿ�� ���� ������

	UICT_PET_GROWTH			= 73,// Pet����
	UICT_CALL_STORE			= 74,// ���� ȣ��
	UICT_FORCED_ENCHANT		= 75,// ������ ��þƮ ���� ����
	UICT_EQUIP_LEVELLIMIT	= 76,// ��� ���� �� ����
	UICT_REPAIR_ITEM		= 77,// ��� ���� ������
	UICT_EXTEND_CHRACTERNUM	= 78,// ĳ���� ���� ���� Ȯ��	
	UICT_TELEPORT			= 79,// �ڷ���Ʈ ��ũ��
	UICT_SHARE_RENTALSAFE_CASH = 80,// ���� ���� ���� â��(ĳ��) : ���� �ݰ�1~4 Ȯ�� ����
	UICT_CALL_SHARE_SAFE	= 81,// ���� â�� ȣ��
	UICT_PET_EQUIP			= 82,// ��� ���� ������
	UICT_STATUS_RESET		= 83,	// Status Reset (AT_STR_ADD, AT_INT_ADD, AT_CON_ADD, AT_DEX_ADD, AT_ALL_STATUS)

	UICT_COOLTIME_RATE_ITEM	= 84,// ������ ��Ÿ�� õ���� ���� ���� ������
	UICT_ADD_COOLTIME_ITEM	= 85,// ������ ��Ÿ�� ���� ������

	UICT_REPAIR_MAX_DURATION= 86,// ������ �ִ� ������ ���� ������(�￩�� �ִ� �������� ȸ�� ��Ų��.)
	UICT_AWAKE_INCREASE_ITEM = 87,// ������ ���� ������

	UICT_EVENT_ITEM_REWARD	= 88,// �ۺ����� ���� ������ ������ ���� �ڽ�

	UICT_TRANSTOWER_DISCOUNT	= 89,// ����Ÿ�� ���α�
	UICT_TRANSTOWER_OPENMAP		= 90,// ����Ÿ�� ���¸�

	UICT_CERTIFICATEOFHERO	= 91,	// ������ ��ǥ 

	UICT_PET_RENAME			= 92,
	UICT_PET_COLORCHANGE	= 93,

	UICT_HARDCORE_KEY		= 94,	// �ϵ��ھ� �����

	UICT_HOME_SIDEJOB_NPC	= 95,	// ����Ȩ �ҿ�ũ����Ʈ NPC

	UICT_HARDCORE_BOSSKEY	= 96,// �ϵ��ھ� ���� �����

	UICT_CALL_HELP			= 97,	// ���� UI�� ȣ���ϴ� ������

	UICT_EVENT_ITEM_REWARD_GAMBLE = 98,// �ۺ����� ���� ������ ������ ���� �ڽ� �׺� ������ �ִ°�

	UICT_ENCHANT_OFFENCE		= 99,	// ���� ��ȭ ����
	UICT_ENCHANT_DEFENCE		= 100,	// �� ��ȭ ����

	UICT2_CHECK_PLAYER_CONDITION = 101,// �� ���� ������ ĳ������ ���¸� �˻��ؾ� �Ѵ�. ���� �˻簡 �ʿ��� ������ ���� �ʿ� ���� �������� �� ���Ƽ� ������

	UICT_ACTIVATE_SKILL			= 102,

	UICT_HIDDEN_REWORD		= 103,//���� �ʿ��� ��� ���� ���ھ�����

	UICT_REDICE_ITEM_OPTION	= 104,//������ �ɼ� ���� ������

	UICT_REDICE_PET_OPTION	= 105,//�� �ɼ� ���� ������

	UICT_UPGRADE_OPTION		= 106,// �ɼ� ��ȭ ������
	UICT_SKILLSET_OPEN		= 107,//SkillSet
	UICT_MONSTER_CARD_TYPE2	= 108,
	UICT_REBUILD_RAND_QUEST			= 109,// ���� ����Ʈ �ٽ� �ϱ�(�Ⱓ��)
	UICT_REBUILD_RAND_QUEST_CONSUME	= 110,// ���� ����Ʈ �ٽ� �ϱ�(�Ҹ�)

	UICT_OPTION_AMP_ITEM	= 111,// �ɼ� ���� ������
	
	UICT_MAKING_INSURANCE	= 113,// �丮 ���� (������ ��ȣ)

	UICT_MISSION_DEFENCE_CONSUME		= 114,// �̼� ���� ������
	UICT_MISSION_DEFENCE_CONSUME_ALL	= 115,// �̼� ���� ������
	UICT_SUPER_GROUND_FEATHER			= 116,// ���۴��� ��Ȱ ����
	UICT_MISSION_DEFENCE_TIMEPLUS		= 117,// �̼� ���ð� ����
	UICT_SKILL_EXTEND					= 118,	// ��ų ���� Ȯ�� ī��
	UICT_SKILLEXTEND_INSURANCE			= 119,// ��ų Ȯ�� ī�� ����
	UICT_REST_EXP						= 120,// �޽� ����ġ ȸ��
	UICT_SUPER_GROUND_MOVE_FLOOR		= 121,// ���۴��� �� �̵� ������
	UICT_MISSION_DEFENCE_POTION			= 122,// �̼� ��ȣ�� ȸ�� ������
	UICT_STRATEGYSKILL_OPEN				= 123,// ������ų Ȱ��ȭ
	UICT_SUPER_GROUND_ENTER_FLOOR		= 124,// ���۴��� �ش� ������ �ٷ� ���� ������

	UICT_ENCHANTSHIFT_INSURANCE			= 125,//��æƮ ���� ���� ��ũ��	

	UICT_GUILD_INV_EXTEND_LINE			= 126,//���ݰ� 1����(8ĭ) Ȯ��
	UICT_GUILD_INV_EXTEND_TAB			= 127,//���ݰ� 1�� Ȯ��

	UICT_MISSION_DEFENCE7_ITEM			= 128,//�ű� ���ҽ� ��� ������

	UICT_SHARE_RENTALSAFE_GOLD			= 130,// ���� ���� ���� â��(���) : ���� �ݰ�1�� Ȯ�� ����

	UICT_SCROLL_QUEST					= 140,// ����Ʈ ��ũ�� ������(�Һ�)

	UICT_RESET_ATTACHED					= 200,// ���� �ͼ� ����

	UICT_ENCHANT_SHIFT					= 201,// ��þƮ ���� ����

	UICT_REPAIR_ITEM_ALL				= 202,// ����� ��� ������ ����

	UICT_NOT_DISPLAY_DURATION			= 203,// ������ ǥ������ �ʴ� ������(AT_USE_ITEM_CUSTOM_VALUE_4 ����� ����� �� ���)
	
	UICT_VENDOR			= 204,// ���� ����
	UICT_JOBSKILL_SAVEIDX				= 205,
	UICT_OPEN_PACK2						= 206,

	UICT_REMOVE_SOCKET					= 207,// ���� ������ ����
	UICT_TREASURE_CHEST					= 208, // ��������
	UICT_TREASURE_CHEST_KEY				= 209, // �������� ����
	UICT_DEFENCE7_RELAY					= 210, // ������� �̾��ϱ� ������

	UICT_REVIVE_PARTY					= 211,	//��Ƽ ��Ȱ ������
	UICT_REVIVE_EXPEDITION				= 212,	//������ ��Ȱ ������

	UICT_RECOVERY_STRATEGYFATIGABILITY		= 213,	// ���� �Ƿε� ȸ�� ������(���밪).
	UICT_R_RECOVERY_STRATEGYFATIGABILITY	= 214,	// ���� �Ƿε� ȸ�� ������(��밪).

	UICT_PREMIUM_SERVICE				= 215,	//�����̾� ����
	
	UICT_DEFENCE7_POINT_COPY			= 216,	// ���� ���潺��� ����Ʈ ������ ������.

	UICT_ELEMENT_ENTER_CONSUME			= 217,	// ���� ���� ���� ������.

	UICT_MANUFACTURE					= 218,	// �Ｎ ���� ������

	UICT_SUMMON_SKILL_ACTOR				= 219,	// ��ų���� ��ȯ
	UICT_BUNDLE_MANUFACTURE				= 220,	// �Ｎ���� �뷮����(ĳ��) ������
	UICT_RAGNAROK_ENTER					= 222,	// ��׳���ũ ���� ���� ������

	UICT_JOBSKILL_LEARN					= 223,	// ������ų ���� ������
	UICT_BATTLEPASS_ITEM				= 224,
}EUseItemCustomType;

typedef enum
{
	PET_FEED_TYPE_1			= 0,	// ���� ��� ������(1����)
	PET_FEED_TYPE_2			= 1,	// ���� ��� ������(������)
	PET_FEED_TRAINING		= 2,	// 2�� �� �Ʒ�
	PET_FEED_PLAYING		= 3,	// 2�� �� ��� ������
//	PET_FEED_REVIVE			= 4,	// 2�� ���� ��Ȱ ������(����)
//	PET_FEED_OTHER			= 5,	// (����)
	PET_FEED_MP_DRINK		= 6,	// 2�� ���� MP Drink
//	AT_USE_ITEM_CUSTOM_VALUE_2 ����� ����� �� ���
}EUseItemPetFeedType;

typedef enum eUseItemServerCheckType
{
	UISCT_NONE		= 0,
	UISCT_SCROLL	= 1,//MapMove
}EUseItemServerCheckType;

typedef enum eItemUseStateType
{
	ITEM_USE_STATE_ALIVE = 0,
	ITEM_USE_STATE_DEAD = (1<<0),
}EItemUseStateType;

typedef enum eFurnitureType
{
	FURNITURE_TYPE_NONE				= 0,
	FURNITURE_TYPE_OBJECT			= 1,
	FURNITURE_TYPE_OBJECT_ATTACH	= 2,
	FURNITURE_TYPE_WALL				= 3,
	FURNITURE_TYPE_WALL_ATTACH		= 4,
	FURNITURE_TYPE_FLOOR			= 5,
	FURNITURE_TYPE_FLOOR_ATTACH		= 6,
	FURNITURE_TYPE_NPC				= 7,
	FURNITURE_TYPE_ETC				= 10,
} EFurnitureType;

typedef enum eCountAbilFlag
{
	AT_CF_NONE				= 0x00000000,
	AT_CF_EFFECT			= 0x00000001,// Effect���� ���
	AT_CF_EFFECT_CHK		= 0x000000FF,
	AT_CF_QUEST				= 0x00000100,// Quest���� ���
	AT_CF_QUEST_CHK			= 0x00000F00,
	AT_CF_EVENTSCRIPT		= 0x00001000,// EventScript���� ���
	AT_CF_EVENTSCRIPT_CHK	= 0x0000F000,
	AT_CF_HYPERMOVE			= 0x00010000,// ���������� ���
	AT_CF_HYPERMOVE_CHK		= 0x000F0000,
	AT_CF_BS				= 0x00100000,// ��Ʋ������� ���
	AT_CF_BS_CHK			= 0x00F00000,
	
	// �Ѱ����� ���
	AT_CF_FAKEREMOVE		= 0x01000000,
	AT_CF_FAKEREMOVE_CHK	= AT_CF_FAKEREMOVE,
	AT_CF_ELITEPATTEN		= 0x02000000,
	AT_CF_ELITEPATTEN_CHK	= AT_CF_ELITEPATTEN,
	AT_CF_GM				= 0x80000000,
	AT_CF_GM_CHK			= AT_CF_GM,

	AT_CF_ALL				= 0xFFFFFFFF,
}eCountAbilFlag;

typedef enum 
{
	EGG_NORMAL	= 0,// �Ϲ�
	EGG_BRONZE	= 1,// û��
	EGG_SILVER	= 2,// ��
	EGG_GOLD	= 3,// ��
}E_GAMBLE_GRADE;

typedef enum eShareHpRelationship : int
{
	ESHR_NONE		= 0,
	ESHR_PARENT		= 1,
	ESHR_CHILDREN	= 2,
}EShareHpRelationship;

int const MAX_ENCHANT_SUCCESS_RATE = 100000;// ��þƮ ����Ȯ�� �ִ밪
int const MAX_ITEM_EFFECT_ABIL_NUM = 10;// ������ ����Ʈ ��� �ִ� ����(������ ���)

typedef enum eAdjustSkillCheckType
{// ��ų �����ϴ� ����Ʈ�� ��ų ��ȣ �˻� Ÿ��
	EASC_NONE						= 0,
	EASC_ALL						= 1,
	EASC_CHECK_SOME_SECTION			= 2,
	EASC_CHECK_EXCLUSIVE_SECTION	= 3,
}EAdjustSkillCheckType;

typedef enum eEffectDmgType
{
	EEDT_NONE		= 0,
	EEDT_PHY		= 1,
	EEDT_MAGIC		= 2,
}EEffectDmgType;

typedef enum eAwakeState
{
	EAS_NORMAL_GUAGE	= 0,
	EAS_MAX_GUAGE		= 1,
	EAS_PENALTY_GUAGE	= 2,
}EAwakeState;

typedef enum eAdjustSkillCalcType
{
	EASCT_NONE					= 0,	
	EASCT_RANGE					= 1,	// ��ų ����1(���� ���� ���� ����)
	EASCT_RANGE2				= 2,	// ��ų ����2(����ü����, ������ ���� ���� ����)
	EASCT_DMG					= 3,	// ��ų ������(���°� ���Ǳ� �� ���� �������� ����)

	// �ܰ躰 ������ ����
	EASCT_RESULT_MIN_DMG_BY_PHASE	= 4,	// �ܰ躰 ������������ ���� min ��
	EASCT_RESULT_MAX_DMG_BY_PHASE	= 5,	// �ܰ躰 ������������ ���� max ��
	EASCT_INC_PHASE_DMG				= 6,	// �ܰ躰 ������ ���� ��
	//EASCT_ADJUST_SKILLCOOLTIME  = ?,	// ��ų ��Ÿ��(���� ��� ����)
}EAdjustSkillCalcType;


int const DRAGONIAN_LSHIFT_VAL = 50;

typedef enum eSpecificIdleType
{// AT_SKILL_SPECIFIC_IDLE
	ESIT_NONE = 0,
	ESIT_BOSS_MONSTER_IDLE = 1,		// �������� ����� idle
}ESpecificIdleType;

typedef enum ePlayContentsType
{
	EPCT_NONE			= 0,
	EPCT_ELGA			= 1,
	EPCT_ELGA_BONE_MON	= 2,
	EPCT_ELGA_EYE_MON	= 3,
} EPlayContentsType;

typedef enum eUnitDieType
{
	EUDT_NONE				= 0x00,
	EUDT_NORMAL				= 0x01,
	EUDT_NOT_CHILD_DIE		= 0x02,	//�ڽ������� ������ ����
}EUnitDieType;

typedef enum eManufactureType
{
	EMANT_NONE			= 0,
	EMANT_SPELL			= 1,
	EMANT_COOK			= 2,
	EMANT_WORKMANSHIP	= 3,
	EMANT_AUTO			= 4,
}EManufactureType;

typedef enum eLearnSkillEvent
{
	ELSE_NONE				= 0,
	ELSE_MANUFACTURE_HELP	= 1,
}ELearnSkillEvent;

typedef enum eChangeBlockAble
{
	ECBA_NONE = 0,
	ECBA_MAKE_CAN_NOT_BLOCK = 1,
	ECBA_MAKE_CAN_BLOCK = 2,
}EChangeBlockAble;

typedef enum eEffectTypeWhenHit
{
	ETWH_DEFAULT = 0,
	ETWH_ME = 1,
}EEffectTypeWhenHit;

typedef enum eHydraUnit
{
	EHU_NO	= 0x00,
	EHU_YES	= 0x01,
}EHydraUnit;

typedef enum eLockEXPError
{
	ELEE_CANT_LOCK_EXP		= 0x00,
	ELEE_SUCCESS_LOCK_EXP	= 0x01,
	ELEE_EXP_ALREADY_LOCKED = 0x02,
	ELEE_EXP_ALREADY_UNLOCK = 0x03,
	ELEE_EXP_LOCK_UNKNOW	= 0x04,
}ELockEXPError;

const int LOCK_EXP_EFFECT = 90000300;

#endif // WEAPON_VARIANT_BASIC_CONSTANT_DEFABILTYPE_H