-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)

function Skill_SideDashAttack_OnCastingCompleted(actor, action)
end

function Skill_SideDashAttack_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if actor:IsMyActor() then
		action:StartTimer(10.0,0.1,0);
    end
    
	action:SetParamFloat(1, g_fDashSpeed);	-- Start Velocity
	return	true;
end

function Skill_SideDashAttack_OnTimer(actor,fAccumTime,action,iTimerID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsUnderMyControl() then
		ODS("Skill_SideDashAttack_OnTimer fAccumTime:"..fAccumTime.."\n");
		
		action:CreateActionTargetList(actor);
		action:BroadCastTargetListModify(actor:GetPilot());
	end
	
		
	return	true;
end


function Skill_SideDashAttack_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iAttackRange = action:GetSkillRange(0,actor);
	ODS("Skill_Sliding_Tackle_OnFindTarget iAttackRange:"..iAttackRange.."\n");
	
	if iAttackRange == 0 then
		iAttackRange = 30
	end
	
	local	kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,30,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	
	local iFound=action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);

	kTargets:DeleteInActionGUIDCont(action);
	kTargets:AddToActionGUIDCont(action);
	
	return	iFound;
	
end

function Skill_SideDashAttack_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

    --ODS("Skill_SideDashAttack_OnUpdate\n");
	local action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	
	if action:GetParam(2) == "" and 
		action:GetNextActionName() == "a_dash_attack" then
		
	    action:SetNextActionName("a_idle");
	    
	end
	
	if action:GetParam(2) == "ToDashAttack!" then
	    action:SetParam(2,"");
		return false
	end		
	
	if action:GetParam(3) == "EndNow" then
		action:DeleteTimer(0);
		return false
	end
			
	actor:FindPathNormal()		
	
	local fAccel = -2000 * frameTime;
	local fVelocity = action:GetParamFloat(1);
	
	ODS("__________Dash Attack Velocity " .. fVelocity .. " fAccel:"..fAccel.."\n")

	if fVelocity>0 then
	
		local dir = action:GetDirection()
		actor:Walk(dir, fVelocity)

		
		fVelocity = fVelocity+fAccel;
		action:SetParamFloat(1,fVelocity);
		
	else 
		
		action:DeleteTimer(0);
		action:SetParam(2,"end");
		return	false;
		
	end
	

	
	return	true;
end
function Skill_SideDashAttack_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:EndBodyTrail();
	actor:EndWeaponTrail();
	return	true;
end

function Skill_SideDashAttack_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	return	Act_Dash_OnLeave(actor,action);
	
end

function Skill_SideDashAttack_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true,true);
			return;
		end

		SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    
    end

end
