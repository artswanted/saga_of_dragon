-- fake casting 
function Act_FakeCasting_OnCheckCanEnter(actor, action)
	CheckNil(actor==nil)	
	CheckNil(actor:IsNil())
	if actor:IsMeetFloor() == false then
		return	false
	end
	
	return	true;
end
function Act_FakeCasting_OnEnter(actor, action)
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
	actor:Stop()
	CheckNil(action==nil)
	CheckNil(action:IsNil())
	local actionID = action:GetID()
	local iSlotCount = action:GetSlotCount()
	action:SetSlot(math.random(0, iSlotCount))

	if actionID ~= "a_rush" then	-- 돌격의 경우에는 무기를 숨기지 않는다.
		actor:HideParts(6, true)
	end

	return true
end

function Act_FakeCasting_OnUpdate(actor, accumTime, frameTime)
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end

function Act_FakeCasting_OnLeave(actor, action)
	return true
end

function Act_FakeCasting_OnCleanUp(actor, action)
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
	actor:HideParts(6, false)
	
	local kPreAction = actor:GetAction()
	if nil ~= kPreAction and not kPreAction:IsNil() then
		local kEffectID = tonumber(kPreAction:GetParam(1000))
		if kEffectID ~= 0 and actor:IsMyActor() then
			local	kPacket = NewPacket(12130) -- remove effect
			kPacket:PushGuid(actor:GetPilotGuid())
			kPacket:PushInt(kEffectID)
			Net_Send(kPacket)
			DeletePacket(kPacket)
			--RemoveStatusEffect(kEffectID)
		end
	end
	return true
end