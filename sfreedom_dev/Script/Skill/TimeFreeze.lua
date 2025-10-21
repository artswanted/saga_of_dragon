------------------------------------------------
-- Definitions
------------------------------------------------
-- Param Definition
-- 0[int] : Action State (See Action State Definition)

-- Action State
TIMEFREEZE_ACTIONSTATE_PLAYING=0
TIMEFREEZE_ACTIONSTATE_DONE=1


------------------------------------------------
-- Event Handling Functions
------------------------------------------------
function Skill_TimeFreeze_OnCheckCanEnter(actor,action)

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_TimeFreeze_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_TimeFreeze_Fire(actor,action);

end

function Skill_TimeFreeze_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iSkillRange = Skill_TimeFreeze_GetSkillRange(actor,action);
	
	local kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetPos(),Point3(0,0,0));
	kParam:SetParam_2(0,0,iSkillRange,0);
	kParam:SetParam_3(true,0);		

	action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
	
	return	kTargets:size();

end

function Skill_TimeFreeze_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local actionID = action:GetID()
	
	Skill_TimeFreeze_SetActionState(action,TIMEFREEZE_ACTIONSTATE_PLAYING);
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_TimeFreeze_OnCastingCompleted(actor,action);

	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if nil~=kSoundID and kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end
	
	return true
end

function Skill_TimeFreeze_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	local	bAnimDone = actor:IsAnimationDone();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local iActionState = Skill_TimeFreeze_GetActionState(action);
	
	if iActionState == TIMEFREEZE_ACTIONSTATE_PLAYING then
	
		if bAnimDone then
			Skill_TimeFreeze_SetActionState(action,TIMEFREEZE_ACTIONSTATE_DONE);
		end
			
	
	elseif iActionState == TIMEFREEZE_ACTIONSTATE_DONE then
	
		return	false;
	end
	

	return true
end

function Skill_TimeFreeze_OnCleanUp(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	Skill_TimeFreeze_DetachCircleEffects(actor,actor:GetAction());
	
	return true;
end

function Skill_TimeFreeze_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	local	iActionState = Skill_TimeFreeze_GetActionState(curAction);
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end
	
	if iActionState == TIMEFREEZE_ACTIONSTATE_DONE then 
		return true;
	end
	

	return false 
end

function Skill_TimeFreeze_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		
		action:GetTargetList():ApplyActionEffects();
    
    end

end

function Skill_TimeFreeze_OnEvent(actor, textKey)

	if textKey == "fire" then
		
		Skill_TimeFreeze_ProcessTextKeyEvent_Fire(actor,actor:GetAction());
	
	elseif textKey == "hit" then
	
		Skill_TimeFreeze_ProcessTextKeyEvent_Hit(actor,actor:GetAction());
		
	end

end

------------------------------------------------
-- Implementation Functions
------------------------------------------------
function Skill_TimeFreeze_ProcessTextKeyEvent_Hit(actor,action)

	Skill_TimeFreeze_ProcessTextKeyEvent_Fire(actor,action);

end
function Skill_TimeFreeze_ProcessTextKeyEvent_Fire(actor,action)

	Skill_TimeFreeze_CreateActionTargetList(actor,action);
	Skill_TimeFreeze_RequestTargetListModify(actor,action);
	Skill_TimeFreeze_AttachFireEffects(actor,action)

end

function Skill_TimeFreeze_CreateActionTargetList(actor,action)

	if actor:IsMyActor() == false then
		return
	end

	action:CreateActionTargetList(actor);

end

function Skill_TimeFreeze_RequestTargetListModify(actor,action)

	if actor:IsMyActor() == false then
		return
	end

	action:BroadCastTargetListModify(actor:GetPilot());
	action:ClearTargetList();

end

function Skill_TimeFreeze_AttachFireEffects(actor,action)

	actor:AttachParticle(71022,"char_root","ef_skill_timefreeze_01_char_root");
	
	if actor:IsMyActor() then
		actor:AttachParticle(71024,"char_root","ef_flash_01");
		actor:SetParticleAlphaGroup(71024,-8);
	end

end
function Skill_TimeFreeze_AttachCircleEffects(actor,action)

	actor:AttachParticle(71023,"char_root","ef_skill_timefreeze_02_char_root");

end
function Skill_TimeFreeze_DetachCircleEffects(actor,action)

	actor:DetachFrom(71023);

end

function Skill_TimeFreeze_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_TimeFreeze_AttachCircleEffects(actor,action);

	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end


function Skill_TimeFreeze_GetSkillRange(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end

	return	iAttackRange;
end

------------------------------------------------
-- Parameter Set/Get Functions
------------------------------------------------

function Skill_TimeFreeze_SetActionState(action,iState)

	action:SetParamInt(0,iState);

end
function Skill_TimeFreeze_GetActionState(action)

	return	action:GetParamInt(0);

end
