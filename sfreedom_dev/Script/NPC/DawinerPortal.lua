function NPC_Portal1(npc)
	npc = ToActor(npc)
	npc:ReserveTransitAction("a_talk")
	npc:Talk(100005)
	CallUI("Select_Portal_At_Dawiner")
end
