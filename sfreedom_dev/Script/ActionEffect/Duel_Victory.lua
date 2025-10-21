
function Act_Duel_Victory_OnCheckCanEnter(actor, action)
	return true
end
function Act_Duel_Victory_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	-- if( CheckNil(action == nil) ) then return false end
	-- if( CheckNil(action:IsNil()) ) then return false end
	
	actor:ResetAnimation();
	if actor:IsMyActor() then
		LockPlayerInput(23)
	end
	return true
end
function Act_Duel_Victory_OnUpdate(actor, accumTime, frameTime,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	-- if( CheckNil(action == nil) ) then return false end
	-- if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:IsAnimationDone() then
		return false
	end
	return true
end

function Act_Duel_Victory_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	-- if( CheckNil(action == nil) ) then return false end
	-- if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:IsMyActor() then
		UnLockPlayerInput(23)
	end
end

function Act_Duel_Victory_OnLeave(actor, action)
	return	true;
end
