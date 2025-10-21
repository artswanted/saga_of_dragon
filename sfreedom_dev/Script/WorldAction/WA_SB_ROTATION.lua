-- 보스	몸 돌리기

TargetActor = nil

function WA_SB_ROTATION_OnReceivePacket(wa_obj,Packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	wa_obj:SetParamGUID(0, Packet:PopGuid());	-- 타겟	GUID
	BossActor = g_world:FindActor(g_world:GetBossGUID())
	return true
end

function WA_SB_ROTATION_OnEnter(wa_obj,ElapsedTimeAtStart)
	if BossActor:IsNil() then
		return false
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local TargetActor = g_world:FindActor(wa_obj:GetParamGUID(0))
	local TargetPos = TargetActor:GetTranslate()
	
	wa_obj:SetParamInt(9, 0)			-- 본지 안본지 셋팅
	wa_obj:SetParamFloat(10, TargetPos:GetX())
	wa_obj:SetParamFloat(11, TargetPos:GetY())

	return true
end

function WA_SB_ROTATION_OnUpdate(wa_obj,ElapsedTime)
	if wa_obj:GetParamInt(9) == 0 then
		wa_obj:SetParamInt(9, 1)			-- 본지 안본지 셋팅
		local LookPoint = Point3(wa_obj:GetParamFloat(10), wa_obj:GetParamFloat(11), 0)
		BossActor:LookAt(LookPoint)			-- 쳐다보게 만들기
		BossActor:SetAnimSpeed(1)
	end
	return false
end

function WA_SB_ROTATION_OnLeave(wa_obj)
	return true
end
