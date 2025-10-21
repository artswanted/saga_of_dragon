#ifndef WEAPON_VARIANT_UNIT_PGNPC_H
#define WEAPON_VARIANT_UNIT_PGNPC_H

#include "Unit.h"

typedef enum : BYTE
{
	E_NPC_TYPE_NONE = 0,
	E_NPC_TYPE_FIXED = 1,
	E_NPC_TYPE_DYNAMIC = 2,
	E_NPC_TYPE_CREATURE = 3,
} ENpcType;

#pragma pack(1)
typedef struct tagNpcCreateInfo
{
	tagNpcCreateInfo()
	{
		iID = 0;
		iDescriptionID = 0;
		bHideMiniMap = false;
		eType = E_NPC_TYPE_NONE;
		GiveEffectNo = 0;
	}
	
	BM::GUID kGuid;
	POINT3 kPosition;
	int iID;
	int iDescriptionID;	//DefString NO;
	ENpcType eType;
	std::wstring wstrName;
	std::wstring wstrActor;
	std::wstring wstrScriptName;
	bool bHideMiniMap;
	int GiveEffectNo;
}SNpcCreateInfo;
#pragma pack()

enum ENpcMenuType
{
	NMT_ClientOnly	= 0,
	NMT_Quest		= 1,
	NMT_Shop,
	NMT_InvenSafe,
	NMT_Repair,
	NMT_Enchant,
	NMT_SoulCraft,
	NMT_Coupon,
	NMT_Post,
	NMT_Fran,
	NMT_Soul,
	NMT_Making,
	NMT_Guild,
	NMT_GuildInventory,
	NMT_GemStore,
	NMT_Socket,
	NMT_Mission_Event,
	NMT_ItemConvert,
	NMT_EventTWEffectQuest,
	NMT_EmporiaAdmin,
	NMT_RealtyDealer,
	NMT_MixupItem,
	NMT_SoulStoneTrade,
	NMT_EventQuestSys,
	NMT_BattleSquare,
	NMT_TransTower,
	NMT_PvPLeague,
	NMT_JobSkill,
	NMT_Expedition,
	NMT_ElementDungeon,
	NMT_CommunityEvent,
	NMT_RagnarokTheme,
	NMT_PetUpgrade,
	NMT_ITEM_TREE,
};

typedef std::set< unsigned short > ContMenuItem;
namespace PgNpcUtil
{
	bool ConvertScriptToMenu(std::wstring& rkScript, ContMenuItem& rkOut);
}

class PgNpc 
	:	public CUnit
{
public:
	typedef enum 
	{
		ENpc_Quest_Type_None = 0,
		ENpc_Quest_Type_Begin = 1,
		ENpc_Quest_Type_Ing = 2,
		ENpc_Quest_Type_End = 3,
	} ENpc_Quest_Type;

	PgNpc(void);
	virtual ~PgNpc(void);

	virtual HRESULT Create(void const* pkInfo);
	virtual EUnitType UnitType()const{ return UT_NPC; }
	virtual void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType=WT_DEFAULT)const;
	virtual EWRITETYPE ReadFromPacket(BM::Stream &rkPacket);
	virtual void Invalidate();
	virtual bool IsCheckZoneTime(DWORD dwElapsed){return true;}
	virtual int Tick(unsigned long ulElapsedTime, SActArg *pActArg){return 0;}
	virtual int CallAction(WORD wActCode, SActArg *pActArg);

public:
	bool Create(BM::GUID const &rkGuid, std::wstring const &rstrkName, std::wstring const &rstrkActor,
		std::wstring const &rstrkScript, POINT3 const& rkPosition, int iID, int GiveEffectNo, ENpcType const eType, bool bHideMiniMap=false );
	virtual void Init();
	virtual void VOnDie(){CUnit::VOnDie();}//얘가 죽을 일이 있냐??

	bool IsCanMenu(ENpcMenuType const eType);

	CLASS_DECLARATION_S_NO_SET(std::wstring, ActorName); // 엑터 이름
	CLASS_DECLARATION_S_NO_SET(std::wstring, MenuStr); // 물린 메뉴 영문명
	CLASS_DECLARATION_S_NO_SET(ContMenuItem, Menu); // 물린 메뉴 ID
	CLASS_DECLARATION_S_NO_SET(ENpcType, NpcType);
	CLASS_DECLARATION_S_NO_SET(bool, ExpeditionNpc);// 원정대 NPC인지 아닌지
	CLASS_DECLARATION_S_NO_SET(bool, HideMiniMap);	// 미니맵 표시, 캐릭터 이름, NPC 리스트에서 숨길 것인지 아닌지
	CLASS_DECLARATION_S_NO_SET(int, GiveEffectNo);	// 유저에게 이펙트를 준다.
protected:

};

#endif // WEAPON_VARIANT_UNIT_PGNPC_H