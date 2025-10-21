function RarityUpgrade(property,usesuccessrateitem)

	local packet = NewPacket(12999)	

	packet:PushInt(property)
	packet:PushByte(1)
	packet:PushByte(0)
	packet:PushByte(usesuccessrateitem)
	packet:PushByte(0)
	packet:PushByte(0)
		
	Net_Send(packet)
	DeletePacket(packet)
	
end