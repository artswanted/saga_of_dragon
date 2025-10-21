#pragma once
#include <Lohengrin/dbtables.h>

namespace hotmeta
{

struct TB_DefHotMetaAbil
{
	TB_DefHotMetaAbil()
	{
		ItemNo = 0;
		memset(Type, 0, sizeof(Type));
		memset(Value, 0, sizeof(Value));
	}

	int ItemNo;
	int Type[MAX_ITEM_ABIL_ARRAY];
	int Value[MAX_ITEM_ABIL_ARRAY];
    DEFAULT_TBL_PACKET_FUNC();
};

struct TblKeyIntHash
{
	static_assert(sizeof(TBL_KEY_INT) <= sizeof(size_t), "Type is too big for hash");
	size_t operator()(const TBL_KEY_INT& key) const
	{
		return std::hash<size_t>()(static_cast<int>(key));
	}
};

typedef std::unordered_map< TBL_KEY_INT, TB_DefHotMetaAbil, TblKeyIntHash > CONT_DEFHOTMETA_ABIL;
typedef std::vector<int> CONT_HOTMETA_PACKET;

}
