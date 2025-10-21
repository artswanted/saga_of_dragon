-- Cringe

function Act_Cringe_OnEnter(actor, action)	
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	actor:Stop()
	actor:ResetAnimation()
	action:SetParam(0,"")	
	return true
end
function Act_Cringe_OnUpdate(actor, accumTime, frameTime,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	fElapsedTime = accumTime - action:GetActionEnterTime();
	
	if actor:IsAnimationDone() == true then
		action:SetParam(0,"end");
		return false
	end
	
	return true
end
function Act_Cringe_OnCleanUp(actor, action)
--	ODS("Act_Cringe_OnCleanUp : "..action:GetID().." Cur : "..actor:GetAction():GetID().."\n", false, 1509)
	return true
end

function Act_Cringe_OnLeave(actor, action)

	return true
end

function Act_Cringe_OnTimer(actor,fAccumTime,action,iTimerID)

	return true
end
