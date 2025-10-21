-- Stun

function Act_Stun_OnEnter(actor, action)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	actor:Stop()
	actor:ResetAnimation();

	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
		action:SetSlot(1)	
	end
	
	return true
end

function Act_Stun_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	local	action = actor:GetAction()

	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:IsStun() == false then
		action:SetParam(0,"end");	
		return	false
	end
	
	if actor:IsAnimationDone() then
		actor:PlayCurrentSlot()
	end

	return true
end

function Act_Stun_OnCleanUp(actor, action)
end

function Act_Stun_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	if actor:GetAction():GetParam(0) == "end" or action:GetActionType() == "EFFECT" then
		action:GetID()
		return	true
	end
	return false
end

