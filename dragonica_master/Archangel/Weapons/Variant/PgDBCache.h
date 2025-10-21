#ifndef WEAPON_VARIANT_DATABASE_PGDBCACHE_H
#define WEAPON_VARIANT_DATABASE_PGDBCACHE_H

#include <vector>
#include <map>
#include <list>
#include <string>

#include "CEL/CEL.h"
#include "CEL/CoreCenter.h"
#include "Loki/Singleton.h"
#include "Loki/Threads.h"

namespace DBCacheUtil
{
	typedef enum eAddResult
	{
		E_ADD_SUCCESS			= 0,
		E_ADD_FAIL_DUPLICATE	= 1,
		E_ADD_FAIL_NATION		= 2,
	}EAddResult;

	extern int iForceNationCode;

	bool IsDefaultNation(std::wstring const& rkNationCodeStr);

	template< typename _T_KEY, typename _T_TO_FUNC, typename _T_MSG_FUNC >
	bool IsCanNation(std::wstring const& rkNationCodeStr, _T_KEY const& _KeyVal, wchar_t const* szFunc, size_t const iLine)
	{
		if( rkNationCodeStr.empty() )
		{
			_T_MSG_FUNC()( BM::vstring() << __FL2__(szFunc, iLine) << L"[Key Val: " << _T_TO_FUNC(_KeyVal).operator std::wstring const&() << L"], Nation code is empty" );
			return true;
		}

		int const iServiceRegion = (0 < iForceNationCode)? iForceNationCode: g_kLocal.ServiceRegion();

		VEC_WSTRING kVec;
		PgStringUtil::BreakSep(rkNationCodeStr, kVec, L"/");
		std::sort(kVec.begin(), kVec.end());
		VEC_WSTRING::iterator nation_pos = std::unique(kVec.begin(), kVec.end());
		if(nation_pos != kVec.end())
		{
			do {
				_T_MSG_FUNC()( BM::vstring() << __FL2__(szFunc, iLine) << L"[Key Val: " << _T_TO_FUNC(_KeyVal).operator std::wstring const&() << L"], Nation code not unique : " << *nation_pos);
				++nation_pos;
			} while(nation_pos != kVec.end());
			return false;
		}

		VEC_WSTRING::const_iterator iter = kVec.begin();
		while( kVec.end() != iter )
		{
			int const iCurCode = PgStringUtil::SafeAtoi( (*iter) );
			if( 0 == iCurCode ) // 0 �� ���ԵǸ� -_-;;
			{
				_T_MSG_FUNC()( BM::vstring() << __FL2__(szFunc, iLine) << L"[Key Val: " << _T_TO_FUNC(_KeyVal).operator std::wstring const&() << L"], Can't use '0' with in other nation code" );
				return false;
			}
			if( iServiceRegion == iCurCode )
			{
				return true;
			}
			++iter;
		}
		return false;
	}

	#define DBCACHE_KEY_PRIFIX	L"$KEY_VALUE$"
	extern std::wstring const kKeyPrifix;

	template< typename _T_KEY, typename _T_ELEMENT, typename _T_CONT >
	struct MapContainerInsert
	{
		bool operator()(_T_CONT& rkOutCont, _T_KEY const& rkKey, _T_ELEMENT const& rkElement)
		{
			return rkOutCont.insert( std::make_pair(rkKey, rkElement) ).second;
		}
		void Merge(_T_CONT& lhs, _T_CONT const& rhs)
		{
			lhs.insert( rhs.begin(), rhs.end() );
		}
	};
	template< typename _T_KEY, typename _T_ELEMENT, typename _T_CONT >
	struct VectorContainerInsert
	{
		bool operator()(_T_CONT& rkOutCont, _T_KEY const& rkKey, _T_ELEMENT const& rkElement)
		{
			rkOutCont.push_back( rkElement );
			return true;
		}
		void Merge(_T_CONT& lhs, _T_CONT const& rhs)
		{
			_T_CONT::const_iterator iter = rhs.begin();
			while( rhs.end() != iter )
			{
				if( lhs.end() == std::find(lhs.begin(), lhs.end(), (*iter)) )
				{
					lhs.push_back( *iter );
				}
				++iter;
			}
		}
	};
	
	//
	template< typename _T_KEY, typename _T_ELEMENT, typename _T_CONT, typename _T_TO_FUNC = BM::vstring, typename _T_MSG_FUNC = DBCacheUtil::AddError, template < class, class, class > class _T_INSERT_FUNC = DBCacheUtil::MapContainerInsert>
	class PgNationCodeHelper
	{
		typedef std::set< _T_KEY > ContKey;
	public:
		PgNationCodeHelper(std::wstring const& rkDuplicateMsg, bool const bUseMsgFunc = true)
			: m_kContResult(), m_kContDefault(), m_kDuplicateMsg(rkDuplicateMsg), m_bUseMsgFunc(bUseMsgFunc)
		{
		}
		~PgNationCodeHelper()
		{
		}

		EAddResult Add(std::wstring const& rkNationCodeStr, _T_KEY const& rkKey, _T_ELEMENT const& rkNewElement, wchar_t const* szFunc, size_t const iLine)
		{
			if( IsDefaultNation(rkNationCodeStr) )
			{
				if( _T_INSERT_FUNC< _T_KEY, _T_ELEMENT, _T_CONT >()(m_kContDefault, rkKey, rkNewElement) )
				{
					return E_ADD_SUCCESS;
				}
				else
				{
					if( IsUseMsgFunc() )
					{
						m_kDuplicateMsg.Replace( kKeyPrifix, _T_TO_FUNC(rkKey).operator std::wstring const&() );
						_T_MSG_FUNC()( BM::vstring() << __FL2__(szFunc, iLine) << m_kDuplicateMsg );
						return E_ADD_FAIL_DUPLICATE;
					}
				}
			}
			else
			{
				if( IsCanNation< _T_KEY, _T_TO_FUNC, _T_MSG_FUNC >(rkNationCodeStr, rkKey, szFunc, iLine) )
				{
					if( m_kContKey.end() == m_kContKey.find(rkKey) ) // ��ȣ �ߺ� üũ
					{
						m_kContKey.insert( rkKey );
						_T_INSERT_FUNC< _T_KEY, _T_ELEMENT, _T_CONT >()(m_kContResult, rkKey, rkNewElement);
						return E_ADD_SUCCESS;
					}
					else
					{
						if( IsUseMsgFunc() )
						{
							m_kDuplicateMsg.Replace( kKeyPrifix, _T_TO_FUNC(rkKey).operator std::wstring const&() );
							_T_MSG_FUNC()( BM::vstring() << __FL2__(szFunc, iLine) << m_kDuplicateMsg );
							return E_ADD_FAIL_DUPLICATE;
						}
					}
				}
			}
			return E_ADD_FAIL_NATION;
		}
		bool IsEmpty() const
		{
			return m_kContResult.empty() && m_kContDefault.empty();
		}
		_T_CONT& GetResult()
		{
			Build();
			return m_kContResult;
		}
		bool IsUseMsgFunc()
		{
			return m_bUseMsgFunc;
		}

	protected:
		void Build()
		{
			_T_INSERT_FUNC< _T_KEY, _T_ELEMENT, _T_CONT >().Merge(m_kContResult, m_kContDefault);
			m_kContDefault.clear();
		}

	private:
		_T_CONT m_kContResult; // �����
		_T_CONT m_kContDefault; // �⺻ ��
		BM::vstring m_kDuplicateMsg;
		ContKey m_kContKey;
		bool m_bUseMsgFunc;
	};
};

typedef enum eDBQueryType
{
	DQT_INIT_START = 1,
	DQT_SERVER_LIST = 1,
	DQT_NO_OP, // �ƹ� ���۵� ���� �ʴ´�
	DQT_MAP_SERVER_CONFIG,
	DQT_TRY_LOGIN,
	DQT_TRY_GMLOGIN,
	DQT_TRY_AUTH,
	DQT_DISCONNECT_OLDLOGIN,

	DQT_GET_PLAYER_LIST,
	DQT_SELECT_PLAYER_DATA,//ĳ�� ���� ����

	DQT_UPDATE_LOGOUT = 10,//���� �ð� ������Ʈ
	DQT_UPDATE_CONNECTION_CHANNEL,	//���� ���� ä�� ����
	DQT_CLEAR_CONNECTION_CHANNEL,	//��� ���� ���� ä�� �ʱ�ȭ

	//		DQT_MEMBER_EXTERIOR,
	//		DQT_CHARACTER_INV,

	DQT_DEFCLASS_NATIONCODE,
	DQT_DEFCLASS_ABIL_NATIONCODE,
	DQT_DEFCLASS_PET,
	DQT_DEFCLASS_PET_LEVEL,
	DQT_DEFCLASS_PET_SKILL,
	DQT_DEFCLASS_PET_ITEMOPTION,
	DQT_DEFCLASS_PET_ABIL,

	DQT_DEF_ABIL_TYPE,

	DQT_DEF_BASE_CHARACTER,

	DQT_DEF_MAP_ITEM_BAG,

	DQT_DEF_FIVE_ELEMENT_INFO,
	DQT_DEF_MAP_STONE_CONTROL,

	DQT_DEFITEMRARE,
	DQT_DEFITEMRAREGROUP,
	DQT_DEFITEMCONTAINER,
	DQT_DEFITEM_RES_CONVERT,

	DQT_DEFMAP,
	DQT_DEFMAPABIL,
	DQT_DEFMAPEFFECT,
	DQT_DEFMAPENTETY,
	DQT_DEFMAPITEM,
	DQT_DEFMAPMONSTERREGEN,

	DQT_DEFMONSTERTUNNING,

	DQT_DEFNPC,
	DQT_DEFNPCABIL,

	DQT_DEFDYNAMICABILRATE,
	DQT_DEFDYNAMICABILRATE_BAG,

	DQT_DEFSKILL,
	DQT_DEFSKILL_NATIONCODE,
	DQT_DEFSKILLABIL,
	DQT_DEFSKILLABIL_NATIONCODE,
	DQT_DEFSKILLSET,

	DQT_LOAD_DEF_CHANNEL_EFFECT,
	DQT_DEFSTRINGS,
	DQT_DEFSTRINGS_NATIONCODE,

	DQT_DEF_FILTER_UNICODE,

	DQT_DEFITEMMAKING,
	DQT_DEFCOOKING,
	DQT_DEFRESULTCONTROL,

	//DQT_DEFLVEXP,

	DQT_SAVECHARACTER,
	DQT_SAVECHARACTER_EXTERN,
	DQT_SAVECHARACTER_POINT,
	DQT_SAVECHARACTER_MAPINFO,
	DQT_SAVE_CLIENTOPTION,
	DQT_DEL_SKILLSET,
	DQT_SAVE_SKILLSET,
	//DQT_GETUSERQUEST,

	DQT_GET_MYPETDATA,		// ������ ��� ���� ����
	DQT_SAVEMYPETDATA,
	DQT_DELETEMYPETDATA,

	DQT_DEFEFFECT,
	DQT_DEFEFFECTABIL,
	DQT_DEFUPGRADECLASS,
	DQT_UPDATE_MEMBER_PW,
	DQT_DEFITEMENCHANT,

	DQT_DEFCHARACTER_BASEWEAR,// ĳ���ͻ����� �����ϴ� �⺻����
	DQT_KICKUSER,
	DQT_CREATECHARACTER,// ĳ���ͻ���
	DQT_DELETECHARACTER,// ĳ���ͻ���
	DQT_REALMMERGE,// ĳ�����̸�����(����â����)
	DQT_CHECK_CHARACTERNAME_OVERLAP,

	DQT_DEF_ITEM_PLUS_UPGRADE,// ������ + ����
	DQT_DEF_ITEM_PLUS_UPGRADE_NATIONCODE, // ������ + ����(�����ڵ� �߰�)
	DQT_DEF_ITEM_RARITY_UPGRADE,// ������ + ����
	DQT_DEF_ITEM_RARITY_CONTROL,//���Ƽ �̴°�.
	DQT_DEF_ITEM_DISASSEMBLE,
	DQT_DEF_ITEM_ENCHANT_SHIFT,//��æƮ ����
	DQT_DEFMONSTERBAG, // ���� Bag ���̺�
	DQT_DEFGMCMD,	// GM Command ���

	DQT_DEF_ITEM_PLUSUP_CONTROL,//PlusUp �̴°�
	DQT_DEF_MONSTER_BAG_ELEMENTS,
	DQT_DEF_MONSTER_BAG_CONTROL,

	DQT_DEF_ITEM_BY_LEVEL,

	// Break Object
	DQT_DEF_OBJECT,
	DQT_DEF_OBJECTABIL,
	DQT_DEF_OBJECT_BAG,
	DQT_DEF_OBJECT_BAG_ELEMENTS,

	//DQT_DEF_GUILD_EXP,//��� ����ġ ���̺�

	DQT_DEF_QUEST_RANDOM,
	DQT_DEF_QUEST_RANDOM_EXP,
	DQT_DEF_QUEST_RANDOM_TACTICS_EXP,
	DQT_DEF_QUEST_RESET_SCHEDULE,
	DQT_DEF_QUEST_MIGRATION,
	DQT_UPDATE_QUEST_CLEAR_COUNT, // ����Ʈ Ŭ���� ī��Ʈ
	DQT_INIT_QUEST_CLEAR_COUNT,	 // ������� ����Ʈ Ŭ���� ī��Ʈ �ʱ�ȭ

	DQT_FRIENDLIST_ADD_BYGUID,
	DQT_FRIENDLIST_DEL,
	DQT_FRIENDLIST_UPDATE,
	DQT_FRIENDLIST_SELECT,
	DQT_FRIENDLIST_UPDATE_CHATSTATUS,
	DQT_FRIENDLIST_UPDATE_GROUP,
	DQT_FRIENDLIST_MODIFY,

	DQT_GUILD_PROC,//Guild Create,Delete
	DQT_GUILD_SELECT_BASIC_INFO,
	//DQT_GUILD_SELECT_EXTERN_INFO,
	//DQT_GUILD_SELECT_MEMBER_GRADE,
	//DQT_GUILD_SELECT_MEMBER,
	DQT_GUILD_SELECT_OWNER_LAST_LOGIN_DAY,
	DQT_GUILD_INIT_OWNER_LAST_LOGIN_DAY,
	DQT_GUILD_SELECT_NEXT_OWNER,
	DQT_GUILD_MEMBER_PROC,
	DQT_GUILD_UPDATE_MEMBER_GRADE,
	//DQT_GUILD_UPDATE_TAX_RATE,
	DQT_GUILD_UPDATE_NOTICE,
	DQT_GUILD_UPDATE_EXP_LEVEL,
	DQT_GUILD_INVENTORY_CREATE,
	DQT_GUILD_INVENTORY_LOAD,
	DQT_GUILD_INVENTORY_LOG_INSERT,
	DQT_GUILD_INVENTORY_LOG_SELECT,
	DQT_GUILD_INVENTORY_LOG_DELETE,
	DQT_GUILD_INVENTORY_EXTEND,

	DQT_GUILD_CHECK_NAME,//�ߺ� üũ
	DQT_GUILD_RENAME,//�̸� ����(ĳ���� or GM Command)
	DQT_GUILD_CHANGE_OWNER,	//������ ����
	DQT_GUILD_SENDMAIL,
	DQT_GUILD_COMMON,	// ���� ��� ó�� ����
	DQT_GUILD_MERCENARY_SAVE,
	DQT_GUILD_ENTRANCEOPEN_SAVE, // ��尡�� ���� ����
	DQT_GUILD_ENTRANCEOPEN_LIST, // ��尡�� ��û�� ����� ��� ���
	DQT_GUILD_APPLICANT_LIST, // ��尡�� ��û��
	DQT_GUILD_REQ_ENTRANCE,		 // ��尡�� ��û
	DQT_GUILD_REQ_ENTRANCE_CANCEL,// ��尡�� ��û ���
	
	DQT_GUILD_ENTRANCE_PROCESS,
	DQT_GUILD_INV_AUTHORITY, //���ݰ� ���Ѽ���

	DQT_DEF_GUILD_LEVEL,
	DQT_DEF_GUILD_SKILL,
	DQT_DEF_TACTICS_LEVEL,
	DQT_DEF_TACTICS_QUEST_PSEUDO,

	DQT_DEF_MONSTER_KILL_COUNT_REWARD,

	// Emporia
	DQT_DEF_EMPORIA,
	DQT_LOAD_EMPORIA,
	DQT_CREATE_EMPORIA,
	DQT_SAVE_EMPORIA,
	DQT_SAVE_EMPORIA_PACK,
	DQT_SWAP_EMPORIA,

	DQT_SAVE_EMPORIA_RESERVE,
	DQT_SAVE_EMPORIA_THROW,
	DQT_SAVE_EMPORIA_DELETE,
	DQT_SAVE_EMPORIA_BATTLE_STATE,

	DQT_CREATE_EMPORIA_TOURNAMENT,
	DQT_SAVE_EMPORIA_TOURNAMENT,
	DQT_SAVE_EMPORIA_MERCENARY,

	DQT_SAVE_EMPORIA_FUNCTION,
	DQT_UPDATE_EMPORIA_FUNCTION,

	// Ŀ��
	DQT_COUPLE_INFO_SELECT,
	DQT_COUPLE_UPDATE,

	DQT_DEF_MISSION_RESULT,
	DQT_DEF_MISSION_CANDIDATE,
	DQT_DEF_MISSION_ROOT,
	DQT_DEF_MISSION_QUEST,//�̼� ����Ʈ

	DQT_LOAD_MISSION_REPORT,
	DQT_SAVE_MISSION_REPORT,
	DQT_LOAD_MISSION_RANK,
	DQT_SAVE_MISSION_RANK,

	DQT_DEF_DEFENCE_ADD_MONSTER,

	//GMĿ�ǵ� ���
	DQT_GETCHARACTERTOTALCOUNT,
	DQT_CHANGE_PASSWORD,
	DQT_USER_CREATE_ACCOUNT,
	DQT_GM_FREEZE_ACCOUNT,
	DQT_GM_ADD_CASH,
	DQT_GM_GET_CASH,
	DQT_GM_CHANGE_BIRTHDAY,
	DQT_GM_CHARACTER_INFO,
	DQT_GM_CHARACTER_LIST1,
	DQT_GM_CHARACTER_LIST2,
	DQT_GM_CHECK_NAME,
	DQT_GM_SELECT_ALL_GM_MEMBER,
    DQT_DEF_SHOP_IN_EMPORIA,
	DQT_DEF_SHOP_IN_GAME,
	DQT_DEF_SHOP_IN_STOCK,

	// AP
	DQT_AP_CHECK_ACCOUNT,
	DQT_AP_CREATE_ACCOUNT,
	DQT_AP_GET_CASH,
	DQT_AP_ADD_CASH,
	DQT_AP_MODIFY_PASSWORD,
	DQT_AP_CREATE_COUPON,
	DQT_AP_MODIFY_MOBILELOCK,
	DQT_AP_TABLE_CONTROL,

	DQT_DEF_ITEM_OPTION,

	DQT_PLAYER_MONEY_AND_INV_LOAD,
	DQT_PLAYER_EXTERN_DATA_LOAD,

	DQT_UPDATE_USER_ITEM,
	DQT_UPDATE_USER_FIELD,
	DQT_SELECT_EXTERIOR_INFO,
	DQT_UPDATE_ING_QUEST,
	DQT_UPDATE_END_QUEST,

	// ���� ������
	DQT_UPDATE_DATE_CONTENTS,

	//��������
	DQT_RECEIPT_PETITION,
	DQT_SELECT_PETITION_STATE,
	DQT_REMAINDER_PETITION,
	DQT_SELECT_PETITION_DATA,
	DQT_MODIFY_PETITION_DATA,

	//WebTool
	DQT_CHANGE_CHARACTER_NAME,
	DQT_CHANGE_CHARACTER_STATE,
	DQT_CHANGE_CHARACTER_POS,
	DQT_CHANGE_SKILL_INFO,
	DQT_CHANGE_QUEST_INFO,
	DQT_CHANGE_GM_LEVEL,
	DQT_ITEM_PROCESS_CREATE,
	DQT_ITEM_PROCESS_MODIFY,
	DQT_ITEM_PROCESS_DELETE,	
	DQT_CHANGE_CHARACTER_FACE,
	DQT_CHANGE_CP,
	DQT_CHANGE_QUEST_ENDED,

	//PvP
	DQT_DEF_PVP_GROUNDMODE,
	DQT_DEF_PVP_REWARD,
	DQT_DEF_PVP_TIME,
	DQT_DEF_PVP_RANK,
	DQT_PVP_RANK_UPDATE,
	DQT_PVP_RANK,

	DQT_DEF_SPEND_MONEY,

	DQT_CREATE_NC_ACCOUNT,
	DQT_CREATE_GRAVITY_ACCOUNT,
	DQT_TRY_LOGIN_NC,
	DQT_CREATE_ACCOUNT,

	//IB����
	DQT_IB_GET_MEMBER_UID,
	DQT_USER_EVENT,

	DQT_INIT_END,

	DQT_POST_ADD_MAIL,
	DQT_POST_GET_MAIL,
	DQT_POST_MODIFY_MAIL,
	DQT_POST_GET_MAIL_ITEM_RESERVE,
	DQT_POST_GET_MAIL_ITEM,
	DQT_POST_GET_MAIL_MIN,
	DQT_POST_CHECK_EANBLE_SEND,

	DQT_UM_ARTICLE_REG,
	DQT_UM_ARTICLE_DEREG,
	DQT_UM_ARTICLE_QUERY,
	DQT_UM_ARTICLE_ITEM_RESERVE,
	DQT_UM_ARTICLE_BUY,
	DQT_UM_DEALING_QUERY,
	DQT_UM_DEALING_DELETE,
	DQT_UM_DEALING_ITEM_RESERVE,
	DQT_UM_DEALING_READ,
	DQT_UM_GET_MINIMUM_COST,
	DQT_UM_GO_TIME_PROC,
	DQT_UM_MY_ARTICLE_QUERY,
	DQT_POST_NEW_MAIL_NOTY,
	DQT_UM_MARKET_OPEN,
	DQT_UM_LOAD_MARKET,
	DQT_UM_MODIFY_MARKET,
	DQT_UM_MARKET_REMOVE,
	DQT_UM_CASH_QUERY,		//ĳ�� �޴� ����
	DQT_UM_DEV_SET_ARTICLE_STATE,	//�׽�Ʈ �ڵ�

	DQT_DEF_PROPERTY,
	DQT_DEF_MAPEFFECT,
	DQT_DEF_MAPENTITY,

	// Rank ����
	DQT_RECORD_LEVELUP,

	DQT_DEF_ITEM_RARITY_UPGRADE_COST_RATE,

	DQT_DEF_CASH_ITEM_SHOP,

	DQT_CS_CASH_QUERY,	// �ɹ� ��񿡼� ĳ�� �ݾ� ���� 
	DQT_CS_CASH_MODIFY,	// �ɹ� ��� ĳ�� �ݾ� ���� (ĳ�� ������ �Բ� �߰����� ������ ����)
	DQT_CS_ADD_GIFT,	// ���� ������ ó��
	DQT_CS_CASHSHOP,	// ĳ���� ���� ���� + ĳ�� �Һ� ��ŷ ���� ����
	DQT_CS_GIFT_QUEYR,	// ���� ���� �Ѱ� ����
	DQT_CS_RECV_GIFT,	// ���� �ޱ�
	DQT_CS_MODIFY_VISABLE_RANK,// ��ŷ ���� ����
	DQT_CS_CASH_LIMIT_ITEM_QUERY,	// Local�� �����Ǹ� ���� ����
	DQT_CS_CASH_LIMIT_ITEM_UPDATE_QUERY,	// Local�� �����Ǹ� ���� ������Ʈ

	DQT_TAKE_COUPON_REWARD,
	DQT_DEF_ACHIEVEMENTS,// ���� ���̺� 

	DQT_DEF_LOAD_COUPONEVENT,
	DQT_UM_MODIFY_MARKET_STATE,
	DQT_DEF_LOAD_RECOMMENDATIONITEM,
	DQT_SAVE_MEMBER_1ST_LOGINED,	// ���� Login �� ���� ����ϱ�

	DQT_DEF_GROUND_RARE_MONSTER,	// ��� ���� ���� ���̺�
	DQT_DEF_RARE_MONSTER_SPEECH,	// ��� ���� ��� ���̺�
	DQT_CLEAN_DELETE_CHARACTER,
	DQT_CHANGE_ACHIEVEMENT,

	DQT_SELECT_CHARA_SKILL,			// ĳ���� ��ų ������ ���� ����
	DQT_CHAR_RESET_SKILL,			// ĳ���� ��ų �ʱ�ȭ ó��

	DQT_OXQUIZ_EVENT,				// OX ���� �̺�Ʈ ����
	DQT_OXQUIZ_UPDATE_STATE,		// �̺�Ʈ ���� ����

	DQT_GET_EVENT_COUPON,			// ��񿡼� ���� �̺�Ʈ ����Ű�� ������ �´�.
	DQT_LOAD_MACROCHECKTABLE,		// ��ũ�� üũ ���̺� 30�ʸ��� ���ŵȴ�.
	DQT_GET_LAST_RECVED_GIFT,		// Ŭ���̾�Ʈ�� �뺸���ֱ� ���� ĳ�� ���� ����

	DQT_DEF_LOAD_CARDABIL,			// ĳ���� ī�� ��� ���̺� �ε�
	DQT_SAVE_PLAYER_EFFECT,			// Player Effect ���� ����
	DQT_LOAD_CARD_LOCAL,			// ĳ���� ī�� ���� ���� 
	DQT_LOAD_DEF_CARD_KEY_STRING,	// ĳ���� ī�� Ű ��Ʈ�� ����

	DQT_GEMSTORE2,					// ���� ����, �뽺���
	DQT_USER_CREATE_ACCOUNT_GALA,	// GALA�� CreateMember
	DQT_DEF_MONSTERCARD,			// ���� ī�� <-> ������ ��ũ ����
	DQT_DEF_EMOTION,				// �̸�Ƽ��, �̸��, ǳ�� �̸�Ƽ�� ����
	DQT_DEF_CASHITEMABILFILTER,		// ĳ�� �����ۿ� �������� ���ƾ� �� ��� ����
	DQT_DEF_GAMBLE,					// �׺� ������ ���̺�
	DQT_GMORDER_SELECT_MEMBER_GMLEVEL,		// Offline ������ GMLevel ���
	DQT_PCROOM,						// �Ǿ��� ������ ����Ʈ ���
	DQT_PCCAFE,						// �Ǿ��� ������ ����Ʈ ��� �ֽ�
	DQT_PCCAFE_ABIL,				// �Ǿ��� ��޺� ���
	DQT_DEF_CONVERTITEM,			// ������ ��ȯ
	DQT_LOAD_LOCAL_LIMITED_ITEM,	
	DQT_SYNC_LOCAL_LIMITED_ITEM,	
	DQT_LOAD_LOCAL_LIMITED_ITEM_CONTROL,

	//DQT_CHECK_SAFE_LIMITED_ITEM_RECORD,
	DQT_CHECK_LIMIT_LIMITED_ITEM_RECORD,
	DQT_UPDATE_SAFE_LIMITED_ITEM_RECORD,//â�� ä���(1���� ������ ��ŭ
	DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD,//��ä���(1���� �Ѱ���)
	DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD_POP,//���� ����

	DQT_SAVE_PENALTY,
	DQT_CHECK_PENALTY,
	DQT_CHANGE_DELETED_CHARACTER_NAME,

	DQT_HATCH_PET,
	DQT_REMOVE_PET,
	DQT_RENAME_PET,
	DQT_SETABIL_PET,
	DQT_LOAD_PET_ITEM,
	DQT_DEF_PET_HATCH,
	DQT_DEF_PET_BONUSSTATUS,
	//		DQT_DEF_PET_BONUSSTATUSVALUE,
	DQT_OXQUIZ_EVENT_STATE,		// OX ���� ����
	DQT_TRY_AUTH_CHECKPW,
	DQT_TRY_AUTH_CHECKPW_AP,

	DQT_LOAD_CREATE_CHARACTER_EVENT_REWARD,	// ĳ�� ���� �̺�Ʈ ����
	DQT_TRY_AUTH_CHECKPW_OLDLOGIN,
	DQT_CASH_TRAN_COMMIT,
	DQT_CASH_TRANSACTION,		// Cash Transaction begin/end
	DQT_LOAD_DEFREALTYDEALER,	// ����Ȩ ����? (�̰͵� �� �������� ������ �ټ� �ֵ��� ����� ����)
	DQT_SELECT_MYHOME_BASE,		// ����Ȩ �⺻ ���� �ε�
	DQT_LOAD_DEF_RARE_OPT_MAGIC,
	DQT_LOAD_DEF_RARE_OPT_SKILL,
	DQT_LOAD_DEF_SKILLIDX_TO_SKILLNO,

	DQT_LOAD_DEF_MIXUPITEM,

	DQT_LOAD_MYHOME_AUCTION,	// ����Ȩ ��� ���� �ε�
	DQT_LOAD_MYHOME_INFO,

	DQT_VISITLOG_ADD,
	DQT_VISITLOG_DELETE,
	DQT_VISITLOG_LIST,

	DQT_CHECK_ENABLE_VISIT_OPTION,

	DQT_VISITFLAG_MODIFY,
	DQT_MYHOME_MODIFY,

	DQT_INVITATION_CREATE,
	DQT_INVITATION_SELECT,

	DQT_VISITORS_SELECT,

	DQT_DEF_MARRYTEXT,

	DQT_DEF_HIDDENREWORDITEM,
	DQT_DEF_HIDDENREWORDBAG,

	DQT_DEF_MYHOME_DEFAULT_ITEM,

	// Event Quest
	DQT_LOAD_EVENT_QUEST,
	DQT_LOAD_EVENT_QUEST_COMPLETESTATUS,
	DQT_INSERT_EVENT_QUEST_COMPLETESTATUS,
	DQT_DELETE_EVENT_QUEST_COMPLETESTATUS,
	DQT_UPDATE_EVENT_QUEST_STATUS,
	DQT_INIT_EVENT_QUEST_STATUS,
	// Realm Quest
	DQT_LOAD_REALM_QUEST,
	DQT_UPDATE_REALM_QUEST,

	DQT_LOAD_DEF_MYHOME_TEX,
	DQT_LOAD_DEF_HOMETOWNTOMAPCOST,
	DQT_LOAD_DEF_GAMBLEMACHINE,

	DQT_LOAD_BS_GAME,
	DQT_UPDATE_BS_STATUS,
	DQT_INIT_BS_STATUS,

	DQT_LUCKYSTAR_LOAD_EVENT,
	DQT_LUCKYSTAR_LOAD_EVENT_SUB,
	DQT_LUCKYSTAR_LOAD_JOINEDUSER,
    DQT_LUCKYSTAR_UPDATE_LASTEVENT,
	DQT_LUCKYSTAR_UPDATE_EVENT_SUB,			// �̺�Ʈ ���� ����
	DQT_LUCKYSTAR_UPDATE_JOINEDUSER,
	DQT_LUCKYSTAR_UPDATE_JOINEDUSER_READED,

	DQT_GMCMD_CASHITEMGIFT_INSERT,
	DQT_GMCMD_CASHITEMGIFT_DELETE,

	DQT_EVENT_CASHITEM_GIFT1,	// �Ϸ翡 �ѹ� �α����� ������ ĳ���Ϳ��� ĳ�������� ����

	DQT_DEF_TRANSTOWER,
	DQT_DEF_PARTY_INFO,

	DQT_UPDATE_ACHIEVEMENT_FIST,// ���� ���� �޼� ����

	DQT_SELECT_ACHIEVEMENT_RANK,// ���� ��ũ ����

	DQT_DEF_MISSION_CLASS_REWARD,
	DQT_DEF_MISSION_RANK_REWARD,

	DQT_LOAD_EVENT_ITEM_REWARD,	// �ۺ����Ű� ���� ������ ������ ���� ������ ����
	DQT_LOAD_TREASURE_CHEST, //��� ���� ����

	DQT_DEF_MISSION_DEFENCE_STAGE,
	DQT_DEF_MISSION_DEFENCE_WAVE,

	DQT_DEF_MISSION_DEFENCE7_MISSION,
	DQT_DEF_MISSION_DEFENCE7_STAGE,
	DQT_DEF_MISSION_DEFENCE7_WAVE,
	DQT_DEF_MISSION_DEFENCE7_GUARDIAN,

	DQT_DEF_MISSION_BONUSMAP,

	DQT_LOAD_DEF_PLAYERPLAYTIME,//�Ƿε� �ý��� ����
	DQT_UPDATE_RESETPLAYERPLAYTIME,//��ü���� �Ƿε� �ʱ�ȭ
	DQT_PROCESS_SETPLAYERPLAYTIME,//Ư������ �˻�
	DQT_UPDATE_SETPLAYERPLAYTIME,//Ư������ �˻��� ���� ���� ����
	DQT_UPDATE_SETPLAYERPLAYTIMEBYID,////Ư������ �˻�(ID��)�� ���� ���� ����
	DQT_UPDATE_MEMBERDATA,

	DQT_POST_GROUP_MAIL,	// �׷� ���� �߼�
	DQT_LOAD_DEF_CHARCARDEFFECT,
	DQT_LOAD_DEF_SIDEJOBRATE,	// �Ƹ�����Ʈ ��ġ ���̺�
	DQT_MYHOME_SIDEJOB_ENTER,	// �Ƹ�����Ʈ ��ҷ� �̵�
	DQT_LOAD_DEF_EVENTITEMSET,	// ��Ƽ �̺�Ʈ ������ Set
	DQT_LOAD_DEF_REDICEOPTIONCOST,// ������ �ɼ� �缳�� ��� ���̺�
	DQT_LOAD_DEF_MYHOMESIDEJOBTIME,// ����Ȩ ��޺� �Ƹ�����Ʈ �ð�
	DQT_LOAD_DEF_MONSTER_ENCHANT_GRADE,
	DQT_LOAD_DEF_MONSTER_GRADE_PROBABILITY,

	DQT_LOAD_DEF_MYHOMEBUILDINGS,// ������ ��ġ�� ����Ȩ �ǹ� ����
	DQT_ADD_MYHOME,				// ����Ȩ �߰�
	DQT_LOAD_DEF_BASICOPTIONAMP,// �ҿ�ũ����Ʈ �ɼ� ���� ���̺�
	DQT_LOAD_DEF_ITEM_AMP_SPECIFIC,//���� �ɼ�����
	// Alram Mission
	DQT_LOAD_DEF_ALRAM_MISSION,
	DQT_LOAD_DEF_DEATHPENALTY,

	DQT_LOAD_DEF_SKILLEXTENDITEM,// ��ųȮ�� ��Ʈ

	DQT_MYHOME_REMOVE,			// ����Ȩ ����
	DQT_LOAD_DEF_NPC_TALK_MAP_MOVE,

	DQT_ADMIN_LOAD_DEFLOGITEMCOUNT, // �ֱ������� ������ ���� ���

	DQT_ADMIN_GM_COPYTHAT,

	DQT_NONE_RESULT_PROCESS,

	// PvP League
	DQT_LOAD_DEF_PVPLEAGUE_TIME,
	DQT_LOAD_DEF_PVPLEAGUE_SESSION,
	DQT_LOAD_DEF_PVPLEAGUE_REWARD,

	DQT_LOAD_PVPLEAGUE,
	DQT_LOAD_PVPLEAGUE_SUB,
	DQT_INSERT_PVPLEAGUE,

	DQT_INSERT_PVPLEAGUE_TEAM,
	DQT_DELETE_PVPLEAGUE_TEAM,

	DQT_INSERT_PVPLEAGUE_TOURNAMENT,
	DQT_INSERT_PVPLEAGUE_BATTLE,

	DQT_SET_PVPLEAGUE_BATTLE,
	DQT_SET_PVPLEAGUE_STATE,
	DQT_SET_PVPLEAGUE_TEAM,
	DQT_SET_PVPLEAGUE_TEAM_INDEX,

	DQT_UPDATE_SPECIFIC_REWARD,
	DQT_LOAD_DEF_SPECIFIC_REWARD,
	DQT_LOAD_DEF_SPECIFIC_REWARD_EVENT,

	//JobSkill
	DQT_LOAD_DEF_JOBSKILL_ITEMUPGRADE,
	DQT_LOAD_DEF_JOBSKILL_LOCATIONITEM,
	DQT_LOAD_DEF_JOBSKILL_PROBABILITY,
	DQT_LOAD_DEF_JOBSKILL_RECIPE,
	DQT_LOAD_DEF_JOBSKILL_SAVEIDX,
	DQT_LOAD_DEF_JOBSKILL_SKILL,
	DQT_LOAD_DEF_JOBSKILL_SKILLEXPERTNESS,
	DQT_LOAD_DEF_JOBSKILL_TOOL,
	DQT_LOAD_DEF_JOBSKILL_SHOP,

	DQT_SELECT_MEMBERID,

	DQT_UPDATE_USER_JOBSKILL_INFO_NO_OP, //no operator
	DQT_UPDATE_USER_JOBSKILL_HISTORYITEM,

	//JobSkill �������� - ����/����
	DQT_LOAD_DEF_JOBSKILL_ITEMSOULEXTRACT,
	DQT_LOAD_DEF_JOBSKILL_ITEMSOULTRANSITION,

	DQT_LOAD_DEF_SOCKET_ITEM,

	DQT_SAVE_CHARACTOR_SLOT,
	DQT_FIND_CHARACTOR_EXTEND_SLOT,

	DQT_LOAD_DEF_EXPEDITION_NPC,
	
	DQT_LOAD_DEF_EVENTGROUP,
	DQT_LOAD_DEF_EVENTMONSTERGROUP,
	DQT_LOAD_DEF_EVENTREWARDITEMGROUP,
	DQT_LOAD_DEF_EVENT_BOSSBATTLE,
	DQT_LOAD_DEF_EVENT_RACE,
	DQT_LOAD_DEF_EVENTSCHEDULE,

	DQT_SELECT_TOP1_EMPORIA_FOR_CHECK,

	DQT_UPDATE_PREMIUM_SERVICE,
	DQT_GET_MEMBER_PREMIUM_SERVICE,
	DQT_UPDATE_PREMIUM_CUSTOM_DATA,
	
	DQT_SELECT_CASHITEM_FOR_SOUL_LEVEL_DEC,
	DQT_EVENT_STORE,					// �̺�Ʈ ������ ��ȯ��
	DQT_LOAD_JUMPINGCHAREVENT,

	//������ ����Ʈ ���� �ý���
	DQT_LOAD_QUEST_LEVEL_REWARD,		// ������ ����Ʈ ���� �ý���

	DQT_LOAD_JOBSKILL_EVENT_LOCATION,	// �������� ä�� �̺�Ʈ

	DQT_SELECT_USER_QUEST_COMPLETE,			// ����Ʈ �Ϸ�����
	DQT_UPDATE_USER_QUEST_COMPLETE,			// ����Ʈ �Ϸ�����

	DQT_LOAD_DEF_CHARACTER_CREATE_SET,

	//Mission mutator
	DQT_LOAD_DEF_MISSION_MUTATOR,
	DQT_LOAD_DEF_MISSION_MUTATOR_ABIL,

	DQT_GET_MEMBER_DAILY_REWARD,

	DQT_UPDATE_USER_UNLOCK_ALL_CHARACTERS,
}EDBQueryType;

class PgDBCache
{
	friend struct ::Loki::CreateStatic< PgDBCache >;
private:
	PgDBCache(void);
	virtual ~PgDBCache(void);

public://! ! ���� ���� �Լ� Q_ �� ���δ�.
	bool Init();

	static bool Q_DQT_DEFCLASS_NATIONCODE( CEL::DB_RESULT & Result );
	static bool Q_DQT_DEFCLASS_ABIL_NATIONCODE(CEL::DB_RESULT & Result);
	static bool Q_DQT_DEFCLASS_PET( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFCLASS_PET_LEVEL( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFCLASS_PET_SKILL( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFCLASS_PET_ITEMOPTION(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEFCLASS_PET_ABIL(CEL::DB_RESULT& rkResult);	
	static bool Q_DQT_DEFITEMBAG( const char* pkTBPath );
	static bool Q_DQT_DEF_ABIL_TYPE( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFITEMCONTAINER( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFITEMABIL( const char* pkTbPath );
	static bool Q_DQT_DEFITEMRARE( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFITEMRAREGROUP( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFMAP( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFMAPABIL( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFMAPITEM( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFMAPMONSTERREGEN( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFMONSTERTUNNING( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFNPC( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFNPCABIL( CEL::DB_RESULT &rkResult );	
	static bool Q_DQT_DEFSKILL( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFSKILL_NATIONCODE( CEL::DB_RESULT & Result );
	static bool Q_DQT_DEFSKILLABIL( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFSKILLABIL_NATIONCODE( CEL::DB_RESULT & Result );
	static bool Q_DQT_DEFSKILLSET( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFRES( const char* pkTablePath );
	static bool Q_DQT_LOAD_DEF_CHANNEL_EFFECT( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFSTRINGS( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFSTRINGS_NATIONCODE( CEL::DB_RESULT & Result );
	static bool Q_DQT_DEFEFFECT(CEL::DB_RESULT& rkResult );
	static bool Q_DQT_DEFEFFECTABIL(CEL::DB_RESULT& rkResult );
	static bool Q_DQT_DEFUPGRADECLASS(CEL::DB_RESULT& rkResult );
	static bool Q_DQT_DEFITEMENCHANT( CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEFCHARACTER_BASEWEAR(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_ITEM_PLUS_UPGRADE(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_ITEM_PLUS_UPGRADE_NATIONCODE(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_ITEM_ENCHANT_SHIFT(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEFMONSTERBAG(CEL::DB_RESULT& rkResult);	
	static bool Q_DQT_DEF_ITEM_BAG_ELEMENTS(const char* pkTBPath);
	static bool Q_DQT_DEF_MONSTER_BAG_ELEMENTS(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_MONSTER_BAG_CONTROL(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_OBJECT(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_OBJECTABIL(CEL::DB_RESULT& rkResult);	
	static bool Q_DQT_DEF_ITEM_BY_LEVEL(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_MISSION_CANDIDATE(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_MISSION_ROOT(CEL::DB_RESULT& rkResult);	
	static bool Q_DQT_DEF_DEFENCE_ADD_MONSTER(CEL::DB_RESULT& rkResult);
	static bool Q_DEF_MISSION_DEFENCE_STAGE(CEL::DB_RESULT& rkResult);	
	static bool Q_DEF_MISSION_DEFENCE_WAVE(CEL::DB_RESULT& rkResult);	
	static bool Q_DEF_MISSION_DEFENCE7_MISSION(CEL::DB_RESULT& rkResult);	
	static bool Q_DEF_MISSION_DEFENCE7_STAGE(CEL::DB_RESULT& rkResult);
	static bool Q_DEF_MISSION_DEFENCE7_WAVE(CEL::DB_RESULT& rkResult);
	static bool Q_DEF_MISSION_DEFENCE7_GUARDIAN(CEL::DB_RESULT& rkResult);
	static bool Q_DEF_MISSION_BONUSMAP(CEL::DB_RESULT& rkResult);
	static bool Q_LOAD_DEF_JOBSKILL_LOCATIONITEM(CEL::DB_RESULT& rkResult);
	static bool Q_LOAD_DEF_JOBSKILL_SKILL(CEL::DB_RESULT& rkResult);
	static bool Q_LOAD_DEF_JOBSKILL_SKILLEXPERTNESS(CEL::DB_RESULT& rkResult);
	static bool Q_LOAD_DEF_JOBSKILL_TOOL(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_LOAD_DEF_JOBSKILL_SHOP(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_ITEM_OPTION(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_ITEM_RARITY_UPGRADE(CEL::DB_RESULT& rkResult);	
	static bool Q_DQT_DEF_ITEM_BAG_GROUP(const char* pkTBPath);	
	static bool Q_DQT_DEF_QUEST_REWARD(const char* pkTBPath);	
	static bool Q_DQT_DEF_QUEST_RESET_SCHEDULE(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_QUEST_RANDOM_EXP(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_QUEST_RANDOM_TACTICS_EXP(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_QUEST_MIGRATION(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_PVP_GROUNDMODE( CEL::DB_RESULT& rkResult );	
	static bool Q_DQT_DEFITEMMAKING( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFCOOKING( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEFRESULTCONTROL( CEL::DB_RESULT &rkResult );
	static bool Q_DQT_DEF_SPEND_MONEY(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_GUILD_LEVEL(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_GUILD_SKILL(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_PROPERTY(CEL::DB_RESULT &rkResult);	
	static bool Q_DQT_DEF_FIVE_ELEMENT_INFO(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_ITEM_RARITY_UPGRADE_COST_RATE(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_TACTICS_LEVEL(CEL::DB_RESULT &rkResult);	
	static bool Q_DQT_DEF_EMPORIA(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_MONSTER_KILL_COUNT_REWARD(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_ACHIEVEMENTS(CEL::DB_RESULT &rkResult);	
	static bool Q_DQT_DEF_FILTER_UNICODE(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_MONSTERCARD(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_MARRYTEXT(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_HIDDENREWORDITEM(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_HIDDENREWORDBAG(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_EMOTION(CEL::DB_RESULT &rkResult);	
	static bool Q_DQT_DEF_CONVERTITEM(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_PET_HATCH(CEL::DB_RESULT &rkResult);	
	static bool Q_DQT_DEF_LOAD_RECOMMENDATIONITEM(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_RARE_MONSTER_SPEECH(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_CARD_LOCAL(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_HOMETOWNTOMAPCOST(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_MYHOME_TEX(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_SKILLIDX_TO_SKILLNO(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_LOAD_CARDABIL(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_CARD_KEY_STRING(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_GROUND_RARE_MONSTER(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_TRANSTOWER(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_PARTY_INFO(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_DEF_MAP_ITEM_BAG(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_MISSION_RESULT(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_OBJECT_BAG_ELEMENTS(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_OBJECT_BAG(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_MISSION_CLASS_REWARD(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEF_MISSION_RANK_REWARD(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_LOAD_DEF_CHARCARDEFFECT(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_SIDEJOBRATE(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_EVENTITEMSET(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_REDICEOPTIONCOST(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_MYHOMESIDEJOBTIME(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_MONSTER_ENCHANT_GRADE(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_MYHOMEBUILDINGS(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_BASICOPTIONAMP(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_ITEM_AMP_SPECIFIC(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_ALRAM_MISSION(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_DEATHPENALTY(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_LOAD_DEF_SKILLEXTENDITEM(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_LOAD_DEF_NPC_TALK_MAP_MOVE(CEL::DB_RESULT& rkResult);
	static bool Q_DQT_DEFITEM_RES_CONVERT(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_SPECIFIC_REWARD(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_SPECIFIC_REWARD_EVENT(CEL::DB_RESULT & Result);
	static bool Q_DQT_LOAD_DEF_JOBSKILL_PROBABILITY(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_JOBSKILL_ITEMUPGRADE(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_JOBSKILL_SAVEIDX(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_JOBSKILL_RECIPE(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_JOBSKILL_ITEMSOULEXTRACT(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_JOBSKILL_ITEMSOULTRANSITION(CEL::DB_RESULT &rkResult);
	static bool Q_LOAD_DEF_SOCKET_ITEM(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_SAVE_CHARACTOR_SLOT(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_EXPEDITION_NPC(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_SELECT_TOP1_EMPORIA_FOR_CHECK(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_EVENTGROUP(CEL::DB_RESULT & Result);
	static bool Q_DQT_LOAD_DEF_EVENTMONSTERGROUP(CEL::DB_RESULT & Result);
	static bool Q_DQT_LOAD_DEF_EVENTREWARDITEMGROUP(CEL::DB_RESULT & Result);
	static bool Q_DQT_LOAD_DEF_EVENT_BOSSBATTLE(CEL::DB_RESULT & Result);
	static bool Q_DQT_LOAD_DEF_EVENT_RACE(CEL::DB_RESULT & Result);
	static bool Q_DQT_LOAD_DEF_EVENTSCHEDULE(CEL::DB_RESULT & Result);
	static bool Q_DQT_UPDATE_PREMIUM_SERVICE(CEL::DB_RESULT &rkResult);

	static bool Q_DQT_SELECT_CASHITEM_FOR_SOUL_LEVEL_DEC(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_QUEST_LEVEL_REWARD(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_JOBSKILL_EVENT_LOCATION(CEL::DB_RESULT &rkResult);

	static bool Q_DQT_LOAD_DEF_CHARACTER_CREATE_SET(CEL::DB_RESULT &rkResult);

	static bool Q_DQT_LOAD_DEF_MISSION_MUTATOR(CEL::DB_RESULT &rkResult);
	static bool Q_DQT_LOAD_DEF_MISSION_MUTATOR_ABIL(CEL::DB_RESULT &rkResult);

	static bool TableDataQuery( bool bReload = false );	
	static bool OnDBExcute(CEL::DB_RESULT &rkResult);

public:
	static bool m_bIsForTool;
	static bool DisplayErrorMsg(); // ���� �޽����� ����ϸ鼭 ������ false / ������ true
protected:
	Loki::Mutex m_kMutex;
private:
	static void AddErrorMsg(BM::vstring const& rkErrorMsg); // Def ���̺� �ε� �ϸ鼭 ���̴� ���� �޽���
};

#define g_kDBCache SINGLETON_STATIC(PgDBCache)

#endif // WEAPON_VARIANT_DATABASE_PGDBCACHE_H