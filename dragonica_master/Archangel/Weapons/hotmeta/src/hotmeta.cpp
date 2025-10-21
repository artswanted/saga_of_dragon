#include "hotmeta/hotmeta.h"
#include <Lohengrin/DB.h>
#include <Variant/ItemDefMgr.h>
#include <Variant/pgcontroldefmgr.h>
#include <Variant/tabledatamanager.h>

static hotmeta::CONT_DEFHOTMETA_ABIL g_kHotCacheOriginal;
static hotmeta::CONT_HOTMETA_PACKET  g_kHotMetaPacket;
static Loki::Mutex g_kMetaMutex;

extern bool SendToServerType(const CEL::E_SESSION_TYPE eServerType, const BM::Stream &rkPacket);

static void sync()
{
    BM::Stream kPacket(hotmeta::PT_I_M_HOTMETA_SYNC);
	hotmeta::write_to_packet(kPacket);
	SendToServerType(CEL::ST_CENTER,kPacket);
}

void hotmeta::write_to_packet(BM::Stream& stream)
{
    stream.Push(g_kHotMetaPacket);
}

static HRESULT CALLBACK Q_DQT_LOAD_DEF_HOT_META(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return E_FAIL;
	}

    CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
    /**
     * Layout
     * - ItemNo
     * - AbilsCount
     * - Type0N
     * - Value0N
     */
    hotmeta::CONT_HOTMETA_PACKET kHotMetaData;
    while( rkResult.vecArray.end() != itor )
    {
        hotmeta::CONT_DEFHOTMETA_ABIL::mapped_type element;
        (*itor).Pop( element.ItemNo );	++itor;
        kHotMetaData.push_back(element.ItemNo);
        kHotMetaData.push_back(0);
        int* iCount = &kHotMetaData.back();
        for(int i = 0; MAX_ITEM_ABIL_ARRAY > i ; i++)
        {
            int iType;
            int iValue;
			(*itor).Pop( iType );		++itor;
			(*itor).Pop( iValue );		++itor;
            element.Type[i] = iType;
            element.Value[i] = iValue;
            if (iType != 0)
            {
                ++(*iCount);
                kHotMetaData.push_back(iType);
                kHotMetaData.push_back(iValue);
            }
        }
    }

    hotmeta::apply(kHotMetaData);

    {
        BM::CAutoMutex kLock(g_kMetaMutex);
        g_kHotMetaPacket.swap(kHotMetaData);
        sync();
    }
    return S_OK;
}

void hotmeta::apply(hotmeta::CONT_HOTMETA_PACKET const& abils)
{
    GET_DEF(CItemDefMgr, kItemDefMgr);

    hotmeta::CONT_DEFHOTMETA_ABIL::const_iterator jt = g_kHotCacheOriginal.begin();
    for (jt; jt != g_kHotCacheOriginal.end(); ++jt)
    {
        const int iItemNo = jt->second.ItemNo;
        CItemDef* pkItemDef = (CItemDef *)kItemDefMgr.GetDef(iItemNo);
		if(!pkItemDef)
        {
            INFO_LOG(BM::LOG_LV5, __FL__ << "Item " << iItemNo << " not exist!");
            continue;
        }

        INFO_LOG(BM::LOG_LV8, __FL__ << "Find old cache for item " << iItemNo);
        for (int i = 0; i < MAX_ITEM_ABIL_ARRAY; ++i)
        {
            if (jt->second.Type[i] == 0)
                continue;
            pkItemDef->SetAbil(jt->second.Type[i], jt->second.Value[i]);
        }
    }

    hotmeta::CONT_HOTMETA_PACKET::const_iterator iter = abils.begin();
    hotmeta::CONT_DEFHOTMETA_ABIL kTempOldCache;
    for(iter; iter != abils.end(); ++iter)
    {
        int iItemNo = *iter;
        CItemDef* pkItemDef = (CItemDef *)kItemDefMgr.GetDef(iItemNo);
		if(!pkItemDef)
            INFO_LOG(BM::LOG_LV5, __FL__ << "Item " << iItemNo << " not exist!");

        int iAbilCount = *(++iter);
        if (iAbilCount > MAX_ITEM_ABIL_ARRAY)
        {
            INFO_LOG(BM::LOG_LV5, __FL__ << "Abil overflow " << iItemNo << " abils count: " << iAbilCount);
            return;
        }

        INFO_LOG(BM::LOG_LV8, __FL__ << "Update abils for item " << iItemNo);
        hotmeta::CONT_DEFHOTMETA_ABIL::mapped_type kOldAbils;
        kOldAbils.ItemNo = iItemNo;
        for(int i = 0; i < iAbilCount; i++)
        {
            int iType = *(++iter);
            int iValue = *(++iter);
            if (iType == 0)
                continue;
            kOldAbils.Type[i] = iType;
            kOldAbils.Value[i] = pkItemDef->GetAbil(iType);
            pkItemDef->SetAbil(iType, iValue);
        }
        kTempOldCache.insert(std::make_pair(iItemNo, kOldAbils));
    }

    {
        BM::CAutoMutex kLock(g_kMetaMutex);
        g_kHotCacheOriginal.swap(kTempOldCache);
    }
}

void hotmeta::apply(BM::Stream& stream)
{
	hotmeta::CONT_HOTMETA_PACKET kMeta;
	stream.Pop(kMeta);
	hotmeta::apply(kMeta);
}

bool hotmeta::load()
{
    CEL::DB_QUERY kQuery( DT_LOCAL, -1, _T("EXEC [dbo].[up_LoadHotMeta]"));
    kQuery.QueryResultCallback(Q_DQT_LOAD_DEF_HOT_META);

    if (S_OK != g_kCoreCenter.PushQuery(kQuery, true))
    {
        INFO_LOG( BM::LOG_LV1, _T("Query Fail Data Recovery... Cause = ") << kQuery.Command() );
        return false;
    }

    return true;
}
