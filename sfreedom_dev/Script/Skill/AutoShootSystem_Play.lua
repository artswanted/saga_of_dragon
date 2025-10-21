function Skill_InstallDefault_StartTimer(actor, action)
    
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if actor:IsUnderMyControl() then
        
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
        local	fCreateTime = tonumber(actor:GetParam("Entity_Create_Time"));
        local	fElapsedTime = g_world:GetAccumTime() - fCreateTime;
        
        local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
    	
        if fLifeTime == 0 then
            fLifeTime = 10.0;
        end
            
        --  타이머 시작
        action:StartTimer(fLifeTime-fElapsedTime,0.5,0)
    
    end
    
end

function Skill_AutoShootSystem_Play_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
	
	ODS("Skill_AutoShootSystem_Play_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
		actor:AttachSound(120,"AutoShootSystem");
	actor:FreeMove(true)
	return true
end

function Skill_AutoShootSystem_Play_OnTimer(actor, accumTime, action,iTimerID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	
	if action:GetTargetList()>0 then	--	타겟이 있다면 액션을 바꾼다.
	
		actor:ReserveTransitAction("a_AutoShootSystem_Fire");
	
	end
    
    return true;

end

function Skill_AutoShootSystem_Play_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local   kState = action:GetParam(0);
	
	if action:GetParamFloat(3) >0 and (accumTime - action:GetParamFloat(3)> 1) then
		Skill_InstallDefault_StartTimer(actor,action)
		action:SetParamFloat(3,0);
	end
	

    if actor:IsUnderMyControl() then
    
        --  10초가 흐르면 스스로 제거된다.
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
        local	fCreateTime = tonumber(actor:GetParam("Entity_Create_Time"));
        local	fElapsedTime = g_world:GetAccumTime() - fCreateTime;
        local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
    	
        if fLifeTime == 0 then
            fLifeTime = 10.0;
        end
        
        ODS("Skill_AutoShootSystem_Play_OnUpdate fTotalElapsedTime : "..fElapsedTime.."\n");
        
        if fElapsedTime > fLifeTime then
			action:SetParam(0,"end");
            return  false;
        end
    
    end

	return true
end

function Skill_AutoShootSystem_Play_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

--	if actor:IsUnderMyControl() then
--		Net_C_M_REQ_DELETE_ENTITY(actor:GetPilotGuid());
--	end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
    
	return true;
end

function Skill_AutoShootSystem_Play_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kParam = actor:GetAction():GetParam(0)
	
	if action:GetID() == "a_AutoShootSystem_Fire" then
		return	true
	end
	
	return (kParam == "end");
end

function Skill_AutoShootSystem_Play_OnCastingCompleted(actor, action)
end