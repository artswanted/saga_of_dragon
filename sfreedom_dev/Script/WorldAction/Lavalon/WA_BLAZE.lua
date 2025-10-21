-- 화염 작렬!

function WA_BLAZE_OnReceivePacket(wa_obj,Packet)
	

	return true
end

function WA_BLAZE_OnEnter(wa_obj,ElapsedTimeAtStart)
	if BossActor:IsNil() then

	return true
end

function WA_BLAZE_OnUpdate(wa_obj,ElapsedTime)
	return false
end

function WA_BLAZE_OnLeave(wa_obj)
	return true
end
