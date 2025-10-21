local PROTO_FILTERS = {}

function CheckPacket(packetType, packet) -- , packet
	local kCheker = PROTO_FILTERS[packetType]
	if kCheker == nil then
		InfoLog(6, "Not found packet " .. packetType .. " to filter!")
		return false
	end

	return kCheker(packet)
end

PROTO_FILTERS[PT_C_M_REQ_GET_DAILY] = function(packet)
	return true
end

