-- Stun

function Act_SpecificIdle_OnEnter(actor, action)
	if( actor==nil ) then return false end
	if( actor:IsNil() ) then return false end
	if( action==nil ) then return false end
	if( action:IsNil() ) then return false end	
	
	actor:Stop()
	--actor:ResetAnimation();
	
	local iNum = actor:GetSpecificIdle()
	if iNum == 0 then
		return false
	end
	if actor:GetAnimationLength(action:GetSlotAnimName(iNum-1)) == 0 then
		action:SetSlot(4)	
	else
		action:SetSlot(iNum-1)
	end
	actor:ResetAnimation();
	actor:PlayCurrentSlot()
	
	return true
end

function Act_SpecificIdle_OnUpdate(actor, accumTime, frameTime)
	if( actor==nil ) then return false end
	if( actor:IsNil() ) then return  false end
	
	local	action = actor:GetAction()
	if( action==nil ) then return false end
	if( action:IsNil() ) then return false end
	
	local iNum = actor:GetSpecificIdle()
	if iNum == 0 then
		action:SetParam(0,"end");	
		return	false
	end
	
	if actor:IsAnimationDone() then
		if actor:GetAnimationLength(action:GetSlotAnimName(iNum-1)) == 0 then
			action:SetSlot(4)	
		else
			action:SetSlot(iNum-1)
		end
		actor:ResetAnimation();
		actor:PlayCurrentSlot()
	end

	return true
end

function Act_SpecificIdle_OnCleanUp(actor, action)
end

function Act_SpecificIdle_OnLeave(actor, action)
	if( actor==nil ) then return true end
	if( actor:IsNil() ) then return true end
	if( action==nil ) then return false end
	if( action:IsNil() ) then return false end
	local kCurAction = actor:GetAction();
	if( kCurAction:IsNil() ) then return true end
	
	if kCurAction:GetParam(0) == "end" then
		return	true
	end
	
	return false
end

