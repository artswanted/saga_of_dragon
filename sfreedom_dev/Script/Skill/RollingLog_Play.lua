
function Skill_RollingLog_Play_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local actorID = actor:GetID()
	local actionID = action:GetID()
	
	action:SetDoNotBroadCast(true)
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(0,g_world:GetAccumTime());
	action:SetParamFloat(1,g_world:GetAccumTime());
	
	-- Looking Dir
	local kLookDir = actor:GetLookingDir();
	action:SetParamFloat( 4, kLookDir:GetX() );
	action:SetParamFloat( 5, kLookDir:GetY() );
	action:SetParamFloat( 6, kLookDir:GetZ() );

	local iSkillLevel = actor:GetAbil(AT_LEVEL);
	actor:SetTargetScale(0.3 + (iSkillLevel * 0.07));
	
	return true
end

function Skill_RollingLog_Play_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   kParam = FindTargetParam();

	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(0,0,action:GetSkillRange(0,actor),0);
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN+FTO_BLOWUP);
	
	return action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
end

function Skill_RollingLog_Play_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end

end

function Skill_RollingLog_Play_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
			
	local   kMoveDir = Point3(action:GetParamFloat(4),action:GetParamFloat(5),action:GetParamFloat(6));
	local   fMoveSpeed = actor:GetAbil(AT_C_MOVESPEED);
	if fMoveSpeed == 0 then
	    fMoveSpeed = 50
    end
	
	kMoveDir:Multiply(fMoveSpeed);
	
	actor:SetMovingDelta(kMoveDir);
	
	local   fElapsedTIme = accumTime - action:GetParamFloat(1);
	if fElapsedTIme>0.3 then
	
	    action:SetParamFloat(1,accumTime);
	    action:CreateActionTargetList(actor);
	    
	    if IsSingleMode() or action:GetTargetCount() == 0 then
	    
	        Skill_RollingLog_Play_OnTargetListModified(actor,action,false);
	    
	    elseif actor:IsUnderMyControl() then
	    
	        action:BroadCastTargetListModify(actor:GetPilot());
	        action:ClearTargetList();
	    
	    end
	    
	end

	local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
    
    local   fTotalElapsedTime = accumTime - action:GetParamFloat(0);
    if fTotalElapsedTime > fLifeTime then
        return  false
    end

	return true
end

function Skill_RollingLog_Play_OnCleanUp(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--if actor:IsUnderMyControl() then
--		Net_C_M_REQ_DELETE_ENTITY(actor:GetPilotGuid());
--	end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
end

function Skill_RollingLog_Play_OnLeave(actor, action)
	return true;
end

function Skill_RollingLog_Play_OnCastingCompleted(actor, action)
end