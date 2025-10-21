Q = {}     -- Quest
M = {}    -- Monster
N = {}	  -- NPC

--/////////////////////////////////////////////////////////////
--Default Quest Function
--OnTalk
--[[
function Quest_OnTalk(questID, npcID, kGround, who, kNPC)
	if nil == Q[questID] then
		InfoLog(2, "[Quest_OnTalk] if nil == Q["..questID.."] then")
		return
	end
	if nil == Q[questID].N[npcID] then
		InfoLog(2, "[Quest_OnTalk] if nil == Q["..questID.."].N["..npcID.."] then")
		return
	end
	
	InfoLog(8, "Quest[" ..questID .."], npc[" ..npcID .. "]")
	Q[questID].N[npcID].OnTalk(kGround, who, kNPC)
end
]]

--Call Dialog Event Process
--[[
function Quest_OnDialog(questID, kGround, who, sDialogID, iAns1, iAns2, iAns3, iAns4, iAns5)
	if nil == Q[questID] then
		--InfoLog(1, "[Quest_OnDialog] if nil == Q["..questID.."] then")
		return
	end
	Q[questID].OnDialog(kGround, who, sDialogID, iAns1, iAns2, iAns3, iAns4, iAns5)
end
]]

--Result NextDialog Event Process
--[[
function Quest_OnDialog_After(questID, kGround, who, iNextDialog)
	if nil == Q[questID] then
		--InfoLog(1, "[Quest_OnDialog_After] if nil == Q["..questID.."] then")
		return
	end
	Q[questID].OnDialog_After(kGround, who, iNextDialog)
end
]]

--OnAssign
--[[
function Quest_OnAssign(questID, who)
	if nil == Q[questID] then
		--InfoLog(1, "[Quest_OnAssign] if nil == Q["..questID.."] then")
		return
	end
	Q[questID].OnAssign(who)
end
]]

--OnResign
--[[
function Quest_OnResign(questID, who, kGround)
	if nil == Q[questID] then
		--InfoLog(1, "[Quest_OnResign] if nil == Q["..questID.."] then")
		return
	end
	Q[questID].OnResign(who, kGround)
end
]]

--OnComplete
--[[
function Quest_OnComplete(questID, who, kGround)
	if nil == Q[questID] then
		--InfoLog(1, "[Quest_OnComplete] if nil == Q["..questID.."] then")
		return
	end
	Q[questID].OnComplete(who, kGround)
end
]]

--/////////////////////////////////////////////////////////////
--Event
--Monster Die Check Event
--[[
function Quest_MOnDie(questID, monID, who, kMonster, kGround)
	if nil == Q[questID] then
		--InfoLog(1, "[Quest_MOnDie] if nil == Q["..questID.."] then")
		return
	end
	if nil == Q[questID].M then
		--InfoLog(1, "[Quest_MOnDie] if nil == Q["..questID.."].M then")
		return
	end
	if nil == Q[questID].M[monID] then
		--InfoLog(1, "[Quest_MOnDie] if nil == Q["..questID.."].M["..monID.."] then")
		return
	end
	
	Q[questID].M[monID].OnDie(who, kMonster, kGround)
end
]]
--[[
--Item Change Event
function Quest_ItemOnChange(questID, ItemID, who)
	if nil == Q[questID] then
		--InfoLog(1, "[Quest_ItemOnChange] if nil == Q["..questID.."] then")
		return
	end
	if nil == Q[questID].ItemToParam then
		--InfoLog(1, "[Quest_ItemOnChange] if nil == Q["..questID.."].ItemToParam then")
		return
	end
	
	if nil == Q[questID].ItemToParam[ItemID] then
		--InfoLog(1, "[Quest_ItemOnChange] if nil == Q["..questID.."].ItemToParam["..ItemID.."] then")
		return
	end
	local iParamNo = Q[questID].ItemToParam[ItemID]
	
	if nil == Q[questID].Item[iParamNo] then
		--InfoLog(1, "[Quest_ItemOnChange] if nil == Q["..questID.."].Item["..iParamNo.."] then")
		return
	end
	if nil == Q[questID].Item[iParamNo].OnChange then
		--InfoLog(1, "[Quest_ItemOnChange] if nil == Q["..questID.."].Item["..iParamNo.."].OnChange then")
		return
	end
	
	Q[questID].Item[iParamNo].OnChange(who)
end
]]

--Location Trigger Event
--[[
function Quest_LocationTrigger_OnEnter(sQuestID, kGround, who, iGroundNo, sTriggerNo)
	if nil == Q[sQuestID] then
		--InfoLog(1, "[Quest_LocationTrigger_OnEnter] if nil == Q["..sQuestID.."] then")
		return
	end
	if nil == Q[sQuestID].Location then
		--InfoLog(1, "[Quest_LocationTrigger_OnEnter] if nil == Q["..sQuestID.."].Location then")
		return
	end
	if nil == Q[sQuestID].Location[iGroundNo] then
		--InfoLog(1, "[Quest_LocationTrigger_OnEnter] if nil == Q["..sQuestID.."].Location["..iGroundNo.."] then")
		return
	end
	if nil == Q[sQuestID].Location[iGroundNo].OnEnter then
		--InfoLog(1, "[Quest_LocationTrigger_OnEnter] if nil == Q["..sQuestID.."].Location["..iGroundNo.."].OnEnter then")
		return
	end
	
	Q[sQuestID].Location[iGroundNo].OnEnter(kGround, who, sTriggerNo)
end
]]

--Mission Percent
--[[
function Quest_Mission_Percent(sQuestID, who, iMissionKey, iCurPercent)
	if nil ==Q[sQuestID] then
		--InfoLog(1, "[Quest_Mission_Percent] if nil ==Q["..sQuestID.."] then")
		return false
	end
	if nil == Q[sQuestID].Mission then
		--InfoLog(1, "[Quest_Mission_Percent] if nil == Q["..sQuestID.."].Mission then")
		return false;
	end
	if nil == Q[sQuestID].Mission[iMissionKey] then
		--InfoLog(1, "[Quest_Mission_Percent] if nil == Q["..sQuestID.."].Mission["..iMissionKey.."] then")
		return false
	end
	if nil == Q[sQuestID].Mission[iMissionKey].OnPercent then
		--InfoLog(1, "if nil == Q["..sQuestID.."].Mission["..iMissionKey.."].OnPercent then")
		return false
	end
	Q[sQuestID].Mission[iMissionKey].OnPercent(who, iCurPercent)
	return true
end
]]

--Mission Rank
--[[
function Quest_Mission_Rank(sQuestID, who, iMissionKey, iMissionLevel, iCurRank)
	if nil ==Q[sQuestID] then
		--InfoLog(1, "[Quest_Mission_Rank] if nil ==Q["..sQuestID.."] then")
		return false
	end
	if nil == Q[sQuestID].Mission then
		--InfoLog(1, "[Quest_Mission_Rank] if nil == Q["..sQuestID.."].Mission then")
		return false;
	end
	if nil == Q[sQuestID].Mission[iMissionKey] then
		--InfoLog(1, "[Quest_Mission_Rank] if nil == Q["..sQuestID.."].Mission["..iMissionKey.."] then")
		return false
	end
	if nil == Q[sQuestID].Mission[iMissionKey].OnRank then
		--InfoLog(1, "[Quest_Mission_Rank] if nil == Q["..sQuestID.."].Mission["..iMissionKey.."].OnRank then")
		return false
	end
	if nil == Q[sQuestID].Mission[iMissionKey].OnRank[iMissionLevel] then
		--InfoLog(1, "[Quest_Mission_Rank] if nil == Q["..sQuestID.."].Mission["..iMissionKey.."].OnRank["..iMissionLevel.."] then")
		return false
	end
	Q[sQuestID].Mission[iMissionKey].OnRank[iMissionLevel](who, iCurRank)
	return true
end
]]

--PVP
--[[
function Quest_Pvp_Win(sQuestID, who, bWin)
	if nil ==Q[sQuestID] then
		--InfoLog(1, "[Quest_Pvp_Win] if nil ==Q["..sQuestID.."] then")
		return false
	end
	
	if nil == Q[sQuestID].PVP then
		--InfoLog(1, "[Quest_Pvp_Win] if nil == Q["..sQuestID.."].PVP then")
		return false
	end
	
	if nil == Q[sQuestID].PVP.OnWin then
		--InfoLog(1, "[Quest_Pvp_Win] if nil == Q["..sQuestID.."].PVP.OnWin then")
		return false
	end
	
	Q[sQuestID].PVP.OnWin(who, bWin)
	return true
end
]]
