
function Skill_Iceberg_Play_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
	
	action:SetParam(2,"CREATING");
	return true
end

function Skill_Iceberg_Play_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    local   action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   bAnimDone = actor:IsAnimationDone()
    
    ODS("Skill_Iceberg_Play_OnUpdate "..action:GetParam(2).."\n");
    
    
    if action:GetParam(2) == "CREATING" then
    
        if bAnimDone then
            action:SetParam(2,"CREATED");
            actor:PlayNext();
        end
    
    end
    
    if action:GetParam(2) == "CRASH" then
    
        actor:PlayNext();
        action:SetParam(2,"CRASHING");
    
    end
    
    if action:GetParam(2) == "CRASHING" then
    
        if bAnimDone then
            return false;    
        end
    
    end
    
	return true
end

function Skill_Iceberg_Play_GetParentActor(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   kAction = actor:GetAction();
    if kAction:GetTotalGUIDInContainer() > 0 then
    
        local   kGUID = kAction:GetGUIDInContainer(0);
        local   kPilot = g_pilotMan:FindPilot(kGUID);
        if kPilot:IsNil() == false then
        
            local   kActor = kPilot:GetActor();
            if kActor:IsNil() == false then
            
                return  kActor;
            
            end
        
        end
    
    end
    
    return  nil;

end

function Skill_Iceberg_Play_OnCleanUp(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
end

function Skill_Iceberg_Play_OnLeave(actor, action)
	return true;
end

function Skill_Iceberg_Play_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
    ODS("Skill_Iceberg_Play_OnEvent textKey:"..textKey.."\n");
    
    if textKey == "hit" or textKey == "fire" then
    
        local   kParentActor = Skill_Iceberg_Play_GetParentActor(actor,actor:GetAction());
        
        if kParentActor:IsNil() == false then
            --  대미지 이펙트
           SkillHelpFunc_DefaultHitOneTime(actor,action, true);
        end        
        
    
    end

end

function Skill_Iceberg_Play_OnCastingCompleted(actor, action)
end