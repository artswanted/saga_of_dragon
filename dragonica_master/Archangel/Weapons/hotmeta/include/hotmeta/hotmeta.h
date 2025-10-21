#pragma once
#include "hotmeta/hotmeta_db.h"
#include <Lohengrin/packettype.h>

namespace hotmeta
{

PACKET_DEF(PT_I_M_HOTMETA_SYNC, 0x795b);

bool load();
void apply(hotmeta::CONT_HOTMETA_PACKET const& abils);
void apply(BM::Stream& stream);
void write_to_packet(BM::Stream& stream);

} // namespace hotmeta
