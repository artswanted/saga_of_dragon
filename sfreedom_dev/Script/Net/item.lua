function Net_RecvItemChange(packet)
-- PT_M_C_NFY_ITEM_CHANGE 12348
	RecvItemChange(packet)
	return true
end

function Net_RecvPickupItem(packet)
	--	SGroundBoxInfo kGBoxInfo
	--	if (kGBoxInfo.eRet == GITEM_ANS_OK)
	--	{
	--		byte byItemNum
	--		{
	--			SItemPos sFrom
	--			SItemPos sTo
	--		}
	--	}
	
	--MessageBox("Net_RecvPickupItem", "a")

--	iReturn = packet:PopInt()
--	if iReturn == 1 then 
--		ODS("Success : Net_RecvPickupItem \n")
		RecvPickupItem(packet)
--	else
--		MessageBox("Fail : Net_RecvPickupItem", "a")
--	end
end

function Net_AnsGiveItemSet(packet)
	-- PT_M_C_ANS_GIVEITEMSET
	RecvAddItem(packet)
end

function Net_ReqGiveItemSet(iItemNo)

	packet = NewPacket(12420)
	
	packet:PushInt(iItemNo)

	Net_Send(packet)
	DeletePacket(packet)
end

function Net_NfyItemState(packet)
	RecvItemState(packet)
end

function Net_M_C_ITEMCHANGED(packet)
	RecvItemChanged(packet)
end

function Net_RecvAnsEquipItem(packet)
	textNo = packet:PopInt()
	AddWarnDataStr(GetTextW(textNo), 0)
end

function Net_PT_M_C_NFY_NEEDITEM_MESSAGE(packet)
	local iItemNo = 0
	local kItemName = "???"
	packet:PopInt(iItemNo)
	if 0 > iItemNo then
	    kItemName = GetDefString(iItemNo)
	end
	
	local kMsg = WideString(string.format(GetTextW(12):GetStr(),kItemName))
	g_ChatMgrClient:AddLogMessage( kMsg, true, 1)
end
