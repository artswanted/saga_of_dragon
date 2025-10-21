------------------------------------------------
-- Event Handling Functions
------------------------------------------------
function Skill_DeathSnatchFinish_OnCastingCompleted(actor,action)
end

function Skill_DeathSnatchFinish_OnCheckCanEnter(actor,action)

	return	true;
end

function Skill_DeathSnatchFinish_OnFindTarget(actor,action,kTargets)
	
	if( CheckNil(nil==actor) ) then return 0 end
	if( CheckNil(actor:IsNil()) ) then return 0 end
	
	if( CheckNil(nil==action) ) then return 0 end
	if( CheckNil(action:IsNil()) ) then return 0 end
	
	ODS("Skill_DeathSnatchFinish_OnFindTarget\n");
	
	kTargets:CopyFromActionGUIDCont(action);
	
	return	kTargets:size();
end

function Skill_DeathSnatchFinish_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Skill_DeathSnatchFinish_OnEnter\n");

	
	if actor:IsMyActor() then
	
		Skill_DeathSnatchFinish_SetTargetsFromDeathSnatch(actor,action,actor:GetAction());
		action:CreateActionTargetList(actor);
		
	end
	
	actor:ResetAnimation();

	return true
end

function Skill_DeathSnatchFinish_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local action = actor:GetAction()
	local iState = action:GetParamInt(2);
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	return	false;
end

function Skill_DeathSnatchFinish_OnCleanUp(actor)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	return true;
end

function Skill_DeathSnatchFinish_OnLeave(actor, action)

	ODS("Skill_DeathSnatchFinish_OnLeave\n");

	-- if( CheckNil(nil==actor) ) then return true end
	-- if( CheckNil(actor:IsNil()) ) then return true end
	
	-- if( CheckNil(nil==action) ) then return true end
	-- if( CheckNil(action:IsNil()) ) then return true end
	
	return true;
end

function Skill_DeathSnatchFinish_OnEvent(actor,textKey)

	return	true;
end

