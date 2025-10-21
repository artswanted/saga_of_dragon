---------------------------------------------------------------\
-- Tick은 존재하지 않는 WorldAction을 보내준다.
--function SendWorldAction_NoTick(monster,unitmgr,ActionType)
function SendWorldAction_NoTick(monster,ground,ActionType)
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	kPacket:PushInt(ActionType)
	kPacket:PushInt(0)
	if ActionType == WA_SB_ROTATION then
		kPacket:PushGuid(monster:GetTarget())
	elseif ActionType == WA_SB_DAMAGE then
	elseif ActionType == WA_PAREL_DOWN then
	else
		DeletePacket(kPacket)
		return false
	end

	local EmptyGuid = GUID()
	EmptyGuid:Empty()
	ground:Broadcast(kPacket,monster,EmptyGuid)
	DeletePacket(kPacket)

  InfoLog(4, "SendWorldAction_NoTick ...."..ActionType.."")

	return true
end

--------------------------------------------------------------------------------------
function WA_ATTACKDAMAGE_UNIT( MinPow, MaxPow, kMonster,kUnit,ground, iEffectNo, iParam, bPushback)
--  Min = kMonster:GetAbil(AT_MIN_DAMAGE)
--  Max = kMonster:GetAbil(AT_MAX_DAMAGE)
--  PhyPow = kMonster:GetAbil(AT_PHY_POWER)

	if kUnit:GetState() == US_DEAD then
		return false
	end
	if kUnit:GetAbil(AT_HP) <= 0 then
		return false
	end

	local Critical = RAND(100)
	if Critical > 90 then
		Critical = MinPow
	else
		Critical = 0 
	end
	
	local RandPow = MaxPow - MinPow + 1
	local Demage = MinPow + RAND(RandPow) + Critical
	local RemainHP = kUnit:OnDamaged(Demage, ground,kMonster)
	local emptyGuid = GUID()
	emptyGuid:Empty()
	
	local packet = NewPacket(PT_M_C_NFY_ATTACKDAMAGE)
	if kMonster == nil or kMonster:IsNil() then
		packet:PushGuid(emptyGuid)
	else
		packet:PushGuid(kMonster:GetGuid())
	end
	packet:PushGuid(kUnit:GetGuid())
	packet:PushInt(Demage)
	packet:PushInt(RemainHP)
	if bDown == nil then
		bDown = 0
	end
	packet:PushInt(iEffectNo)
	if iBackDist == nil then
		iBackDist = 0
	end
	packet:PushInt(iParam)
	
	if bPushback == nil then
	    bPushback = false
    end
    packet:PushBool(bPushback)    	    
	
	ground:Broadcast(packet,kUnit,emptyGuid)

	DeletePacket(packet)
	return true
end

function WA_GET_UNIT_INDEX(kUnit,IndexNum)
	local ptPos = kUnit:GetPos()
	local fX = ptPos:GetX()
	local fY = ptPos:GetY()
	local fHodo = math.atan2(fY,fX) / math.pi * 180
	if fHodo <= 0 then
		fHodo = fHodo + 360
	end
	
	local Index = ( fHodo + 5 ) / ( 360 / IndexNum )
	Index = math.floor(Index)

	if Index >= IndexNum then
		Index = Index - IndexNum
	end

	return Index
end


