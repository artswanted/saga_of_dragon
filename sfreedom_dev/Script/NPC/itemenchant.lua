--[[
function NPC_ENCHAT(npc)
	npc = ToActor(npc);
	npc:Talk(100001)
	CallUI("SFRM_ITEM_PLUS_UPGRADE")
end
]]

--[[
function NPC_ITEMREPAIR(npc)
	npc = ToActor(npc);
	npc:Talk(100001)
	CallUI("SFRM_REPAIR")
end
]]