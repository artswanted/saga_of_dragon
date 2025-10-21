-- Melee
use_cam_effect = false;

function Skill_Fighter_Jump_Attack_OnCastingCompleted(actor, action)
end

function Skill_Fighter_Jump_Attack_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	if actor:GetJumpAccumHeight() < 50 then
		return	false
	end

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return false
	end

	return true
end
function Skill_Fighter_Jump_Attack_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	actorID = actor:GetID()
--	actionID = action:GetID()
--	weapontype = actor:GetEquippedWeaponType();
	
	if actor:IsMyActor() == true then
		if actor:GetJumpAccumHeight()<50 then	
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;
		end
	end
    
    action:CreatActionTargetList(actor);

	actor:AttachParticle(10,"p_ef_center","e_fighter_jump_attack");
		
	actor:StopJump()
	actor:FreeMove(true)
	actor:StartWeaponTrail()
	return true
end
function Skill_Fighter_Jump_Attack_OnFindTarget(actor,action,kTargets)

	local kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetTranslate(),actor:GetLookingDir());
	kParam:SetParam_2(0,30,action:GetSkillRange(0,actor),0);
	kParam:SetParam_3(true,true);
	
	return	action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
	
end

function Skill_Fighter_Jump_Attack_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local slotNum = action:GetCurrentSlot();
	
	
	if action:GetParam(1) == "end" then
		return	false;
	end

	if animDone == true then
		action:SetParam(1,"end");
		return	false;
	end

	return true
end
function Skill_Fighter_Jump_Attack_OnCleanUp(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	actor:FreeMove(false);
	actor:EndWeaponTrail();
	return true;
end
function Skill_Fighter_Jump_Attack_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
		
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end

	if action:GetActionType()=="EFFECT" then
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	if action:GetEnable() == true then
		return false;
	
	elseif curParam == "end" and (actionID == "a_right_run" or actionID == "a_left_run") then
		return true;
	elseif actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Skill_Fighter_Jump_Attack_OnEvent(actor,textKey,seqID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	if textKey == "hit" then
	
		actor:FreeMove(false);
		SkillHelpFunc_DefaultHitOneTime(actor,action, true);
	
	end

	return	true;
end
