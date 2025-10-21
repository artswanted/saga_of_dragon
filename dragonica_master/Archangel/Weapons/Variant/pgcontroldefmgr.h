#ifndef WEAPON_VARIANT_ABIL_PGCONTROLDEFMGR_H
#define WEAPON_VARIANT_ABIL_PGCONTROLDEFMGR_H

#include "Lohengrin/MemoryTrack.h"
#include "Lohengrin/DBTables.h"
#include "Lohengrin/dbtables2.h"
#include "MonsterDefMgr.h"
#include "Skill.h"
#include "ItemDefMgr.h"
#include "PgClassDefMgr.h"
#include "PgClassPetDefMgr.h"
#include "ItemBagMgr.h"
#include "PgStoreMgr.h"
#include "PgItemOptionMgr.h"
#include "PgDefSpendMoney.h"
#include "ItemSetDefMgr.h"
#include "PgDynamicDefMgr.h"
#include "ItemMakingDefMgr.h"
#include "PgMonsterBag.h"
#include "PgObjcetUnitDefMgr.h"
#include "PgDefPropertyMgr.h"
#include "GroundEffect.h"
#include "PgDefRecommendationItemMgr.h"
#include "PgPetHatchMgr.h"
#include "DefMapMgr.h"
#include "PgDefPremiumMgr.h"

#define DEFINE_DEF_MGR(deftype)	\
public:	\
	const deftype& Get##deftype() { return m_k##deftype; }	\
protected:	\
	##deftype m_k##deftype;

class PgDefMgrBase
{
public:
	PgDefMgrBase(int iType) { m_iDefType = iType;}
	~PgDefMgrBase(void) {};
protected:
	int m_iDefType;
};

template<typename T_MGR>
class PgDefMgrHolder : PgDefMgrBase
{
public:
	PgDefMgrHolder(int iType)
		: PgDefMgrBase(iType)
	{
	}
	~PgDefMgrHolder(void) {};

	const T_MGR& GetDef()const { return m_kMgr; }
protected:
	T_MGR	m_kMgr;
};

class PgControlDefMgr
{
public:
	PgControlDefMgr(void);
	~PgControlDefMgr(void);

	BM::ACE_RW_Thread_Mutex_Ext& GetLock() { return m_kRwLock; }

	typedef enum {
		EDef_MonsterDefMgr		= 0x00000001,
		EDef_SkillDefMgr		= 0x00000002,
		EDef_ItemDefMgr			= 0x00000004,
		EDef_RareDefMgr			= 0x00000008,
		EDef_ItemEnchantDefMgr	= 0x00000010,
		EDef_ClassDefMgr		= 0x00000020,
		EDef_ItemBagMgr			= 0x00000040,
		EDef_StoreMgr			= 0x00000080,
//		EDef_GateWayMgr			= 0x00000100,
		EDef_ItemOptionMgr		= 0x00000200,
		EDef_SpendMoneyMgr		= 0x00000400,
		EDef_ItemSetDefMgr		= 0x00000800,
		EDef_DynamicDefMgr		= 0x00001000,
		EDef_EffectDefMgr		= 0x00002000,
		EDef_ItemMakingDefMgr	= 0x00004000,
		EDef_MonsterBag			= 0x00008000,
		EDef_ObjectDefMgr		= 0x00010000,
		EDef_PropertyMgr		= 0x00020000,
		EDef_GroundEffect		= 0x00040000,
		EDef_RecommendationItem = 0x00080000,
		EDef_Pet				= 0x00100000,
		EDef_Default			= 0x00200000,
		EDef_GenPointMgr		= 0x00400000,
		EDef_MapDefMgr			= 0x00800000,
		EDef_PremiumMgr			= 0x01000000,

		// Ext
		EDef_ExtMgr				= 0x10000000,
		EDef_OnlyFirst			= 0x20000000,

		EDef_All				= 0xFFFFFFFF,
	} EDefMgr_Flag;


	virtual bool Update(SReloadDef& rkDefData, int iUpdateType = EDef_All);
	static inline bool IsIncludeDefMgr(int iType, EDefMgr_Flag eFlags) { return ((iType & eFlags) != 0); }
	template<typename T_MGR>
	const T_MGR& GetCustomDef(EDefMgr_Flag eMgrFlag, T_MGR* pkNothing) const
	{
		CONT_CUSTOM_MGR::const_iterator itor = m_kCustomMgr.find(eMgrFlag);
		if (itor != m_kCustomMgr.end())
		{
			const PgDefMgrHolder<T_MGR>* pkMgr = static_cast<PgDefMgrHolder<T_MGR>*>(itor->second);
			if (pkMgr != NULL)
			{
				return pkMgr->GetDef();
			}
		}
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Cannot Get Custeom Def Type["<<eMgrFlag<<L"]");
		static T_MGR kTemp;
		return kTemp;
	}
	//const PgDefMgrBase* GetCustomDef(EDefMgr_Flag eMgrFlag) const;
	template<typename T_MGR>
	void AddCustomDef(EDefMgr_Flag eType)
	{
		BM::CAutoMutex kWLock(m_kRwLock, true);
		CONT_CUSTOM_MGR::iterator itor = m_kCustomMgr.find(eType);
		if (itor != m_kCustomMgr.end())
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"DefManager already exist Type["<<eType<<L"]");
			return;
		}
		PgDefMgrHolder<T_MGR>* pkNew = new_tr PgDefMgrHolder<T_MGR>(eType);
		m_kCustomMgr.insert(std::make_pair(eType, (PgDefMgrBase*)pkNew));
	}

protected:
	mutable BM::ACE_RW_Thread_Mutex_Ext m_kRwLock;
	void Release();

protected:
	CLASS_DECLARATION_S(BM::GUID, StoreValueKey);//상점 가격표. GUID
	
	typedef std::map<EDefMgr_Flag, PgDefMgrBase*> CONT_CUSTOM_MGR;	// <DefMgrTypeValue, DefMgr>
	CONT_CUSTOM_MGR m_kCustomMgr;

	DEFINE_DEF_MGR(CMonsterDefMgr);
	DEFINE_DEF_MGR(CSkillDefMgr);
	DEFINE_DEF_MGR(CItemDefMgr);
	DEFINE_DEF_MGR(CRareDefMgr);
	DEFINE_DEF_MGR(CItemEnchantDefMgr);
	DEFINE_DEF_MGR(PgClassDefMgr);
	DEFINE_DEF_MGR(PgClassPetDefMgr);
	DEFINE_DEF_MGR(PgPetHatchMgr);
	DEFINE_DEF_MGR(CItemBagMgr);
	DEFINE_DEF_MGR(PgStoreMgr);
	DEFINE_DEF_MGR(PgItemOptionMgr);
	DEFINE_DEF_MGR(PgDefSpendMoneyMgr);
	DEFINE_DEF_MGR(CItemSetDefMgr);
	DEFINE_DEF_MGR(PgDynamicDefMgr);
	DEFINE_DEF_MGR(CEffectDefMgr);
	DEFINE_DEF_MGR(CItemMakingDefMgr);
	DEFINE_DEF_MGR(PgMonsterBag);
	DEFINE_DEF_MGR(PgObjectUnitDefMgr);
	DEFINE_DEF_MGR(PgDefPropertyMgr);
	DEFINE_DEF_MGR(PgGroundEffectMgr);
	DEFINE_DEF_MGR(PgDefRecommendationItemMgr);
	DEFINE_DEF_MGR(PgDefMapMgr);
	DEFINE_DEF_MGR(PgDefPremiumMgr);
};


#define g_kControlDefMgr SINGLETON_STATIC(PgControlDefMgr)

#define GET_DEF(deftype, def)	\
	const deftype& def = g_kControlDefMgr.Get##deftype();

#define GET_DEF_CUSTOM(deftype, defvalue, def)	\
	const deftype& def = g_kControlDefMgr.GetCustomDef<deftype>(defvalue, NULL);

namespace ControlDefUtil
{
	extern int const iClientLoadDef;
};

#endif // WEAPON_VARIANT_ABIL_PGCONTROLDEFMGR_H