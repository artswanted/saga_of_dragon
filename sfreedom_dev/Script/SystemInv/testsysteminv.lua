function GetSystemItem(guid,invtype,invpos)

	local kGuid = GUID(guid)

	local packet = NewPacket(203)	

	packet:PushInt(1)
	packet:PushGuid(kGuid)
	packet:PushByte(invtype)
	packet:PushByte(invpos)
		
	Net_Send(packet)
	DeletePacket(packet)
	
end

function RemoveSystemItem(guid)

	local kGuid = GUID(guid)

	local packet = NewPacket(205)	

	packet:PushInt(1)
	packet:PushGuid(kGuid)
		
	Net_Send(packet)
	DeletePacket(packet)
	
end
