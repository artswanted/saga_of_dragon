function SendMail(name,payment,invtype,invpos,money)

	local packet = NewPacket(16002)	
	local kWName = WideString(name)
	local kWTitle = WideString("test mail")
	local kWText = WideString("none")
	
	local kValue = INT64()
	kValue:Set(money, 0)
    
	packet:PushWString(kWName)
	packet:PushWString(kWTitle)
	packet:PushWString(kWText)
	packet:PushByte(invtype)
	packet:PushByte(invpos)
	packet:PushInt64(kValue)
	packet:PushByte(payment)
	
	Net_Send(packet)
	DeletePacket(packet)
	
end

function RecvMail(low,high,count)

	local packet = NewPacket(16006)
	local kValue = INT64()
	kValue:Set(low,high)
    	packet:PushInt64(kValue)
	packet:PushByte(count)
	Net_Send(packet)
	DeletePacket(packet)

end

function RecvMail64(index)

	local packet = NewPacket(16006)
	packet:PushInt64(index)
	packet:PushByte(8)
	Net_Send(packet)
	DeletePacket(packet)
	
end

function ModifyMail(mguid,mtype)

	local packet = NewPacket(16010)

	local guid = GUID(mguid)
	
	packet:PushGuid(guid)
	packet:PushByte(mtype)
	
	Net_Send(packet)
	DeletePacket(packet)

end

function SendMailAll(name,payment,money)

	for invtype = 1,4 do
	
		for invpos = 0,35 do 
		
			SendMail(name,payment,invtype,invpos,money)
			
		end
	
	end

end

function Net_PT_M_C_POST_ANS_MAIL_SEND(packet)
	
	local result = packet:PopByte()
	ODS("[PT_M_C_POST_ANS_MAIL_SEND] Result:" ..result  .."\n")
end

function Net_PT_M_C_POST_ANS_MAIL_RECV(packet)

	g_MailTable = {}
	g_MailModifyCount = 1
	
	local result = packet:PopByte()
	local totalmailcount = packet:PopByte()
	local mailcount = packet:PopSize_t()
	
	ODS("[PT_M_C_POST_ANS_MAIL_RECV] Result:" ..result .." Count:" ..mailcount .."/" ..totalmailcount .."\n")
	
	if mailcount == 0 then
		return
	end
	
	for counter = 1,mailcount do
		
		local ItemTable = {}
		ItemTable["guid"] = packet:PopGuid()
		ItemTable["index"] = packet:PopInt64()
		ItemTable["from"] = packet:PopWString()
		ItemTable["title"] = packet:PopWString()
		ItemTable["text"] = packet:PopWString()
		ItemTable["state"] = packet:PopByte()
		ItemTable["payment"] = packet:PopByte()
		ItemTable["money"] = packet:PopInt64()
		ItemTable["limittime"] = packet:PopDateTime()
		ItemTable["itemno"] = packet:PopInt()
		ItemTable["itemcount"] = packet:PopWord()
		ItemTable["enchant_01"] = packet:PopInt64()
		ItemTable["enchant_02"] = packet:PopInt64()
		ItemTable["itemstate"] = packet:PopByte()
		
		g_MailTable[counter] = ItemTable
		
		ODS("MAIL GUID:" ..ItemTable.guid:GetString() .." FROM:" ..ItemTable.from:GetStr() .."STATE" ..ItemTable.state .."\n")
	end
end

function Net_PT_M_C_POST_ANS_MAIL_MODIFY(packet)

	local result = packet:PopByte()
	local low = packet:PopInt()
	local high = packet:PopInt()
	local state = packet:PopByte()
	ODS("[PT_M_C_POST_ANS_MAIL_MODIFY] Result:" ..result .." index:(" ..low .."," ..high ..")" .." State:" ..state .."\n")
	
end

function Net_PT_M_C_POST_NOTI_NEW_MAIL(packet)

	local count = packet:PopInt()
	ODS("[Net_PT_M_C_POST_NOTI_NEW_MAIL] COUNT:" .. count .. "\n")

end	

function GetMailMinIndex()

	local packet = NewPacket(16034)
	Net_Send(packet)
	DeletePacket(packet)

end

function Net_PT_M_C_POST_ANS_MAIL_MIN(packet)

	local countl = packet:PopInt()
	local counth = packet:PopInt()
	ODS("[Net_PT_M_C_POST_ANS_MAIL_MIN] MINL:" .. countl .." : MINH:" ..counth .. "\n")

end	
