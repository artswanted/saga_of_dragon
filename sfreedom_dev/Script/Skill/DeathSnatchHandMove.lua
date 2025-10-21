
------------------------------------------------
-- Definitions
------------------------------------------------
-- Params Definition
-- 0[int] : Current Action State
-- 1[int] : Next Action State
-- 2[float] Fly Start Time
-- 3[int] : Target Count
-- 4[string] : First Target Guid
-- 5[int] : Skill Range
-- 6[int] : Shake Camera When Hand Reach Ground [ 0 : No 1 : Yes]
-- 7[string] : Parent Actor Guid
-- 8[int] : Hide Creation Animation [ 0 : Not Hide 1 : Hide ]

-- Point Params Definition
-- 0 : Fly Direction
-- 1 : Fly Start Pos
-- 2 : Fly Target Pos
-- 3 : Prev Actor Pos

-- Action State Definition
-- 0 : Loading
DEATHSNATCHHANDMOVE_ACTIONSTATE_LOADING = 0;
-- 1 : Flying to the Target
DEATHSNATCHHANDMOVE_ACTIONSTATE_FLYING_TO_TARGET = 1;
-- 2 : Attached To The Target
DEATHSNATCHHANDMOVE_ACTIONSTATE_ATTACHED_TO_TARGET = 2;
-- 3 : Flying back to Start Pos
DEATHSNATCHHANDMOVE_ACTIONSTATE_FLYING_TO_START = 3;
-- 4 : Wait
DEATHSNATCHHANDMOVE_ACTIONSTATE_WAIT = 4;
-- 5 : Finished
DEATHSNATCHHANDMOVE_ACTIONSTATE_FINISHED = 5;


------------------------------------------------
-- Event Handling Functions
------------------------------------------------
function Skill_DeathSnatchHandMove_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	actor:FreeMove(true);

	Skill_DeathSnatchHandMove_StartLoadingState(actor,action);	

	return true
end

function Skill_DeathSnatchHandMove_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if false == Skill_DeathSnatchHandMove_UpdateCurrentState(actor,action,accumTime) then
		return	false
	end
	
	Skill_DeathSnatchHandMove_UpdateNextState(actor,action,accumTime);
		
	return true
end

function Skill_DeathSnatchHandMove_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
	actor:EndBodyTrail();
	
	return true;
end

function Skill_DeathSnatchHandMove_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local curAction = actor:GetAction();
	local actionID = action:GetID()
	local	iCurrentActionState = Skill_DeathSnatchHandMove_GetCurrentActionState(curAction);
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end
	
	if iCurrentActionState ==  DEATHSNATCHHANDMOVE_ACTIONSTATE_FINISHED then 
		return true;
	end
	

	return false 
end

function Skill_DeathSnatchHandMove_OnCastingCompleted(actor,action)
end