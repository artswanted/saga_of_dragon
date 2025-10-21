--[[
who:CheckBeginQuest(__TEMP__)

who:ShowQuestDialog(__TEMP__, 다이얼로그ID)

who:GetEmptyInvenSlotNum() < 갯수

who:GiveItem(아이템 번호, 수량)

who:CheckCompleteQuest(__TEMP__)
]]--


//CLIENT_DEFAULT
if who:CheckBeginQuest(__TEMP__) then	--i'm can begin quest
	who:ShowQuestDialog(__TEMP__, 101)
end

//CLIENT (Dialog ID)
if who:CheckBeginQuest(__TEMP__) then	--i'm can begin quest
	who:ShowQuestDialog(__TEMP__, %s)
end

//PAYER_DEFAULT
if who:CheckCompleteQuest(__TEMP__) then --i'm can end quest
	who:ShowQuestDialog(__TEMP__, 601)
end

//PAYER (Dialog ID)
if who:CheckCompleteQuest(__TEMP__) then --i'm can end quest
	who:ShowQuestDialog(__TEMP__, %s)
end

//ING_DEFAULT
if who:IsIngQuest(__TEMP__) then
	who:ShowQuestDialog(__TEMP__, 501)
end

//ING (Dialog ID)
if who:IsIngQuest(__TEMP__) then
	who:ShowQuestDialog(__TEMP__, %s)
end

//TALK
local kItem = {}
kItem[%s] = %s
local iCur = RAND_S(%s, %s)
who:ShowQuestDialog(__TEMP__, kItem[iCur])

who:ShowQuestDialog(__TEMP__, %s)

----------------------------------------------------------------------------

//ONDIALOG (Prev Dialog ID, Next Dialog ID)
if dailog == %s then
	who:ShowQuestDialog(__TEMP__, %s)
	return
end

//ONDIALOG_GIVEITEM
if id1 == %s then
	who:GiveItem(%s, %s)
end